/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 * X command server by Flemming Madsen
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 *
 * if_xcmdsrv.c: Functions for passing commands through an X11 display.
 *
 */

#include "vim.h"

#if defined(FEAT_XCMDSRV) || defined(PROTO)

# ifdef FEAT_X11
#  include <X11/Intrinsic.h>
#  include <X11/Xatom.h>
# endif

/*
 * This file provides procedures that implement the command server functionality
 * of Vim when in contact with an X11 server.
 *
 * Adapted from TCL/TK's send command  in tkSend.c of the tk 3.6 distribution.
 * Adapted for use in Vim by Flemming Madsen. Protocol changed to that of tk 4
 */

/*
 * Copyright (c) 1989-1993 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

/*
 * The information below is used for communication between processes
 * during "send" commands.  Each process keeps a private window, never
 * even mapped, with one property, "Comm".  When a command is sent to
 * an interpreter, the command is appended to the comm property of the
 * communication window associated with the interp's process.  Similarly,
 * when a result is returned from a sent command, it is also appended
 * to the comm property.
 *
 * Each command and each result takes the form of ASCII text.  For a
 * command, the text consists of a nul character followed by several
 * nul-terminated ASCII strings.  The first string consists of the
 * single letter "c" for an expression, or "k" for keystrokes.  Subsequent
 * strings have the form "option value" where the following options are
 * supported:
 *
 * -r commWindow serial
 *
 *	This option means that a response should be sent to the window
 *	whose X identifier is "commWindow" (in hex), and the response should
 *	be identified with the serial number given by "serial" (in decimal).
 *	If this option isn't specified then the send is asynchronous and
 *	no response is sent.
 *
 * -n name
 *	"Name" gives the name of the application for which the command is
 *	intended.  This option must be present.
 *
 * -s script
 *	"Script" is the script to be executed.  This option must be
 *	present.  Taken as a series of keystrokes in a "k" command where
 *	<Key>'s are expanded
 *
 * The options may appear in any order.  The -n and -s options must be
 * present, but -r may be omitted for asynchronous RPCs.  For compatibility
 * with future releases that may add new features, there may be additional
 * options present;  as long as they start with a "-" character, they will
 * be ignored.
 *
 * A result also consists of a zero character followed by several null-
 * terminated ASCII strings.  The first string consists of the single
 * letter "r".  Subsequent strings have the form "option value" where
 * the following options are supported:
 *
 * -s serial
 *	Identifies the command for which this is the result.  It is the
 *	same as the "serial" field from the -s option in the command.  This
 *	option must be present.
 *
 * -r result
 *	"Result" is the result string for the script, which may be either
 *	a result or an error message.  If this field is omitted then it
 *	defaults to an empty string.
 *
 * -c code
 * -i errorInfo
 * -e errorCode
 *	Not applicable for Vim
 *
 * Options may appear in any order, and only the -s option must be
 * present.  As with commands, there may be additional options besides
 * these;  unknown options are ignored.
 */

/*
 * Maximum size property that can be read at one time by
 * this module:
 */

#define MAX_PROP_WORDS 100000

/*
 * Forward declarations for procedures defined later in this file:
 */

static int	x_error_check __ARGS((Display *dpy, XErrorEvent *error_event));
static int	AppendPropCarefully __ARGS((Display *display,
		    Window window, Atom property, char_u *value, int length));
static Window	LookupName __ARGS((Display *dpy, char_u *name,
		    int delete, char_u **loose));
static int	SendInit __ARGS((Display *dpy));
static int      DoRegisterName __ARGS((Display *dpy, char_u *name));
static int      IsSerialName __ARGS((char_u *name));
static void	InjectStr __ARGS((char_u *str));
static void	DeleteAnyLingerer __ARGS((Display *dpy, Window w));

/* Private variables for the "server" functionality */
static Atom	registryProperty;
static int	got_x_error = FALSE;

/*
 * serverRegisterName --
 *	This procedure is called to associate an ASCII name
 *	with Vim.  We will try real hard to get a unique one
 *
 *  Result:
 *	 0: Ok
 *	-1: Name occupied
 *
 */
    int
serverRegisterName(dpy, name)
    Display	*dpy;		/* Display to register with */
    char_u	*name;		/* The name that will be used to
				 * refer to the vim instance in later
				 * "server" commands.  Must be globally
				 * unique. */
{
    int		i;
    int		res;
    char_u	*p = NULL;

    res = DoRegisterName(dpy, name);
    if (res < 0)
    {
	i = 1;
	do
	{
	    if (res < -1 || i > 1000)
	    {
		MSG_ATTR( _("Unable to register a command server name"),
			 hl_attr(HLF_W));
		return -1;
	    }
	    if (p == NULL)
		p = alloc(STRLEN(name) + 10);
	    if (p == NULL)
	    {
		res = -10;
		continue;
	    }
	    sprintf((char *)p, "%s-%03d", name, i++);
	    res = DoRegisterName(dpy, p);
	}
	while (res < 0);
	vim_free(p);
    }
    return 0;
}

    static int
DoRegisterName(dpy, name)
    Display	*dpy;
    char_u	*name;
{
    Window	w;
    XErrorHandler old_handler;
#define MAX_NAME_LENGTH 100
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    if (commProperty == None)
    {
	if (SendInit(dpy) < 0)
	    return -2;
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  It's important to lock the server
     * here to prevent conflicting changes to the registry property.
     */
    XGrabServer(dpy);
    w = LookupName(dpy, name, 0, NULL);
    if (w != (Window) 0)
    {
	Status		status;
	int		dummyInt;
	unsigned int	dummyUns;
	Window		dummyWin;

	/*
	 * The name is currently registered.  See if the commWindow
	 * associated with the name exists.  If not, or if the commWindow
	 * is *our* commWindow, then just unregister the old name (this
	 * could happen if an application dies without cleaning up the
	 * registry).
	 */
	old_handler = XSetErrorHandler(x_error_check);
	status = XGetGeometry(dpy, w, &dummyWin, &dummyInt, &dummyInt,
		              &dummyUns, &dummyUns, &dummyUns, &dummyUns);
	(void) XSetErrorHandler(old_handler);
	if ((status != Success) && (w != commWindow))
	{
	    XUngrabServer(dpy);
	    XFlush(dpy);
	    return -1;
	}
	(void) LookupName(dpy, name, /*delete=*/TRUE, NULL);
    }
    sprintf((char *)propInfo, "%x %.*s", (uint) commWindow,
						       MAX_NAME_LENGTH, name);
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING, 8,
		    PropModeAppend, propInfo, STRLEN(propInfo) + 1);
    XUngrabServer(dpy);
    XSync(dpy, False);
    (void) XSetErrorHandler(old_handler);

    if (!got_x_error)
    {
#ifdef FEAT_EVAL
	set_vim_var_string(VV_SEND_SERVER, name, -1);
#endif
	serverName = vim_strsave(name);
	return 0;
    }
    return -2;
}

#if defined(FEAT_GUI) || defined(PROTO)
/*
 * serverChangeRegisteredWindow --
 *	Clean out new ID from registry and set it as comm win.
 *	Change any registered window ID.
 */
    void
serverChangeRegisteredWindow(dpy, newwin)
    Display	*dpy;		/* Display to register with */
    Window	newwin;		/* Re-register to this ID */
{
    char_u	propInfo[MAX_NAME_LENGTH + 20];

    commWindow = newwin;
    if (registryProperty == None)
    {
	if (SendInit(dpy) < 0)
	    return;
    }
    XGrabServer(dpy);
    DeleteAnyLingerer(dpy, newwin);
    if (serverName != NULL)
    {
	/* Reinsert name if we was already registered */
	(void) LookupName(dpy, serverName, /*delete=*/TRUE, NULL);
	sprintf((char *)propInfo, "%x %.*s",
		(uint) newwin, MAX_NAME_LENGTH, serverName);
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING, 8,
			PropModeAppend, (char_u *) propInfo,
			STRLEN(propInfo) + 1);
    }
    XUngrabServer(dpy);
}
#endif

/*
 * serverSendToVim --
 *	Send to an instance of Vim via the X display.
 *
 * Results:
 *	An interger return code. 0 is OK. Negative is error.
 */
    int
serverSendToVim(dpy, name, cmd)
    Display	*dpy;			/* Where to send. */
    char_u	*name;			/* Where to send. */
    char_u	*cmd;			/* What to send. */
{
    Window	    w;
    Atom	    *plist;
    XErrorHandler   old_handler;
#define STATIC_SPACE 500
    char_u	    *property, staticSpace[STATIC_SPACE];
    int		    length;
    int		    res;
    char_u	    *loosename = NULL;

    if (commProperty == None && dpy != NULL)
    {
	if (SendInit(dpy) < 0)
	    return -1;
    }

    /* Execute locally if no display or target is ourselves */
    if (dpy == NULL || (serverName != NULL && STRCMP(name, serverName) == 0))
    {
	InjectStr(cmd);
	return 0;
    }

    /*
     * Bind the server name to a communication window.
     *
     * Find any survivor with a serialno attached to the name if the
     * original registrant of the wanted name is no longer present.
     *
     * Delete any lingering names from dead editors.
     */
    old_handler = XSetErrorHandler(x_error_check);
    while (TRUE)
    {
	got_x_error = FALSE;
	w = LookupName(dpy, name, 0, &loosename);
	/* Check that the window is hot */
	if (w != None)
	{
	    plist = XListProperties(dpy, w, &res);
	    XSync(dpy, False);
	    if (plist != NULL)
		XFree(plist);
	    if (got_x_error)
	    {
		LookupName(dpy, loosename ? loosename : name,
			   /*DELETE=*/TRUE, NULL);
		continue;
	    }
	}
	break;
    }
    XSetErrorHandler(old_handler);
    if (w == None)
    {
	EMSG2(_("E247: no registered server named %s"), name);
	return -1;
    }
    else if (loosename != NULL)
	name = loosename;

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     */
    length = STRLEN(name) + STRLEN(cmd) + 10;
    if (length <= STATIC_SPACE)
	property = staticSpace;
    else
	property = (char_u *)alloc((unsigned) length);

    sprintf((char *)property, "%c%c%c-n %s%c-s %s",
	              0, 'k', 0, name, 0, cmd);
    if (name == loosename)
	vim_free(loosename);

    res = AppendPropCarefully(dpy, w, commProperty, property, length + 1);
    if (length > STATIC_SPACE)
	vim_free(property);
    if (res < 0)
    {
	EMSG(_("E248: Failed to send command to the destination program"));
	return -1;
    }

    return 0;
}


/*
 * GetInterpNames --
 *	This procedure is invoked to fetch a list of all the
 *	Vim instance names currently registered for the display.
 *
 *	The result is a newline separated list. Caller must free list.
 */
    char_u *
serverGetVimNames(dpy)
    Display	*dpy;
{
    char_u	    *regProp;
    char_u	    *entry;
    char_u	    *p;
    int		    result, actualFormat;
    unsigned long   numItems, bytesAfter;
    Atom	    actualType;
    Atom	    *plist;
    Window          w;
    garray_T	    ga;
    XErrorHandler   old_handler;

    if (registryProperty == None)
    {
	if (SendInit(dpy) < 0)
	    return NULL;
    }
    ga_init2(&ga, 1, 100);

    /*
     * Read the registry property.
     */
    regProp = NULL;
    result = XGetWindowProperty(dpy, RootWindow(dpy, 0), registryProperty, 0L,
	                        (long)MAX_PROP_WORDS, False,
				XA_STRING, &actualType,
				&actualFormat, &numItems, &bytesAfter,
				&regProp);

    if (actualType == None)
    {
	EMSG(_("E249: couldn't read VIM instance registry property"));
	return NULL;
    }

    /*
     * If the property is improperly formed, then delete it.
     */
    if ((result != Success) || (actualFormat != 8)
	    || (actualType != XA_STRING))
    {
	if (regProp != NULL)
	{
	    XFree(regProp);
	}
	EMSG(_("E251: VIM instance registry property is badly formed.  Deleted!"));
	return NULL;
    }

    /*
     * Scan all of the names out of the property.
     */
    old_handler = XSetErrorHandler(x_error_check);
    ga_init2(&ga, 1, 100);
    for (p = regProp; (p-regProp) < numItems; p++)
    {
	entry = p;
	while ((*p != 0) && (!isspace(*p)))
	    p++;
	if (*p != 0)
	{
	    got_x_error = FALSE;
	    w = None;
	    sscanf((char *)entry, "%x", (uint*) &w);
	    plist = XListProperties(dpy, w, &result);
	    XSync(dpy, False);
	    if (!got_x_error)
	    {
		ga_concat(&ga, p + 1);
		ga_concat(&ga, (char_u *)"\n");
	    }
	    if (plist != NULL)
		XFree(plist);
	    while (*p != 0)
		p++;
	}
    }
    (void) XSetErrorHandler(old_handler);
    XFree(regProp);
    return ga.ga_data;
}

/*
 * SendInit --
 *	This procedure is called to initialize the
 *	communication channels for sending commands and
 *	receiving results.
 */
    static int
SendInit(dpy)
    Display *dpy;
{
    XErrorHandler old_handler;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */
    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;

    commProperty = XInternAtom(dpy, "Comm", False);
    /* Change this back to "InterpRegistry" to talk to tk processes */
    registryProperty = XInternAtom(dpy, "VimRegistry", False);

    if (commWindow == None)
    {
	commWindow =
	    XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy),
		                getpid(), 0, 10, 10, 0,
				WhitePixel(dpy, DefaultScreen(dpy)),
				WhitePixel(dpy, DefaultScreen(dpy)));
	XSelectInput(dpy, commWindow, PropertyChangeMask);
	XGrabServer(dpy);
	DeleteAnyLingerer(dpy, commWindow);
	XUngrabServer(dpy);
    }

    XSync(dpy, False);
    (void) XSetErrorHandler(old_handler);

    return got_x_error ? -1 : 0;
}

/*
 * LookupName --
 *	Given an interpreter name, see if the name exists in
 *	the interpreter registry for a particular display.
 *
 * Results:
 *	If the given name is registered, return the ID of
 *	the window associated with the name.  If the name
 *	isn't registered, then return 0.
 *
 * Side effects:
 *	If the registry property is improperly formed, then
 *	it is deleted.  If "delete" is non-zero, then if the
 *	named interpreter is found it is removed from the
 *	registry property.
 */
    static Window
LookupName(dpy, name, delete, loose)
    Display	*dpy;	    /* Display whose registry to check. */
    char_u	*name;	    /* Name of an interpreter. */
    int		delete;	    /* If non-zero, delete info about name. */
    char_u	**loose;    /* Do another search matching -999 if not found
			       Return result here if a match is found */
{
    char_u	    *regProp, *entry;
    register char_u *p;
    int		    result, actualFormat;
    unsigned long   numItems, bytesAfter;
    Atom	    actualType;
    Window	    returnValue;

    /*
     * Read the registry property.
     */
    regProp = NULL;
    result = XGetWindowProperty(dpy, RootWindow(dpy, 0), registryProperty, 0L,
	                        (long)MAX_PROP_WORDS, False,
				XA_STRING, &actualType,
				&actualFormat, &numItems, &bytesAfter,
				&regProp);

    if (actualType == None)
	return 0;

    /*
     * If the property is improperly formed, then delete it.
     */
    if ((result != Success) || (actualFormat != 8) || (actualType != XA_STRING))
    {
	if (regProp != NULL)
	    XFree(regProp);
	XDeleteProperty(dpy, RootWindow(dpy, 0), registryProperty);
	return 0;
    }

    /*
     * Scan the property for the desired name.
     */
    returnValue = None;
    entry = NULL;	/* Not needed, but eliminates compiler warning. */
    for (p = regProp; (p - regProp) < numItems; )
    {
	entry = p;
	while ((*p != 0) && (!isspace(*p)))
	    p++;
	if ((*p != 0) && (STRCMP(name, p + 1) == 0))
	{
	    sscanf((char *)entry, "%x", (uint*) &returnValue);
	    break;
	}
	while (*p != 0)
	    p++;
	p++;
    }

    if (loose != NULL && returnValue == None && !IsSerialName(name))
    {
	for (p = regProp; (p - regProp) < numItems; )
	{
	    entry = p;
	    while ((*p != 0) && (!isspace(*p)))
		p++;
	    if ((*p != 0) && IsSerialName(p + 1)
		    && (STRNCMP(name, p + 1, STRLEN(name)) == 0))
	    {
		sscanf((char *)entry, "%x", (uint*) &returnValue);
		*loose = vim_strsave(p + 1);
		break;
	    }
	    while (*p != 0)
		p++;
	    p++;
	}
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */
    if ((delete) && (returnValue != None))
    {
	int count;

	while (*p != 0)
	    p++;
	p++;
	count = numItems - (p-regProp);
	if (count > 0)
	    memcpy(entry, p, count);
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty, XA_STRING,
		        8, PropModeReplace, regProp,
		        (int) (numItems - (p-entry)));
	XSync(dpy, False);
    }

    XFree(regProp);
    return returnValue;
}

/*
 * Delete any lingering occurences of window id. WE promise that any occurences
 * is not ours since it is not yet put into the registry (by us)
 *
 * This is necessary in the following scenario:
 * 1. There is an old windowid for an exit'ed vim in the registry
 * 2. We get that id for our commWindow but only want to send, not register.
 * 3. The window will mistakenly be regarded valid because of own commWindow
 */
    static void
DeleteAnyLingerer(dpy, win)
    Display *dpy;	/* Display whose registry to check. */
    Window  win;	/* Window to remove */
{
    char_u	    *regProp, *entry = NULL;
    register char_u *p;
    int		    result, actualFormat;
    unsigned long   numItems, bytesAfter;
    Atom	    actualType;
    Window	    wwin;

    /* Read the registry property.  */
    regProp = NULL;
    result = XGetWindowProperty(dpy, RootWindow(dpy, 0), registryProperty, 0L,
				(long)MAX_PROP_WORDS, False,
				XA_STRING, &actualType,
				&actualFormat, &numItems, &bytesAfter,
				&regProp);

    if (actualType == None)
	return;

    /* If the property is improperly formed, then delete it.  */
    if ((result != Success) || (actualFormat != 8) || (actualType != XA_STRING))
    {
	if (regProp != NULL)
	    XFree(regProp);
	XDeleteProperty(dpy, RootWindow(dpy, 0), registryProperty);
	return;
    }

    /* Scan the property for the window id.  */
    for (p = regProp; (p - regProp) < numItems; )
    {
	if ((*p != 0))
	{
	    sscanf((char *)p, "%x", (uint *)&wwin);
	    if (wwin == win)
	    {
		int lastHalf;

		/* Copy down the remainder to delete entry */
		entry = p;
		while (*p != 0)
		    p++;
		p++;
		lastHalf = numItems - (p - regProp);
		if (lastHalf > 0)
		    memcpy(entry, p, lastHalf);
		numItems = (entry - regProp) + lastHalf;
		p = entry;
		continue;
	    }
	}
	while (*p != 0)
	    p++;
	p++;
    }

    if (entry != NULL)
    {
	XChangeProperty(dpy, RootWindow(dpy, 0), registryProperty,
			XA_STRING, 8, PropModeReplace, regProp,
			(int) (p - regProp));
	XSync(dpy, False);
    }

    XFree(regProp);
}

/*
 * serverEventProc --
 *	This procedure is invoked by the varous X event loops
 *	throughout Vims when a property changes on the communication
 *	window.  This procedure reads the property and handles
 *	command requests and responses.
 *
 * Results:
 *	None.
 *
 */
    void
serverEventProc(dpy, eventPtr)
    Display	   *dpy;
    XEvent	    *eventPtr;		/* Information about event. */
{
    char_u	    *propInfo;
    char_u	    *p;
    int		    result, actualFormat;
    unsigned long   numItems, bytesAfter;
    Atom	    actualType;

    if ((eventPtr->xproperty.atom != commProperty)
	    || (eventPtr->xproperty.state != PropertyNewValue))
	return;

    /*
     * Read the comm property and delete it.
     */
    propInfo = NULL;
    result = XGetWindowProperty(dpy, commWindow, commProperty, 0L,
				(long)MAX_PROP_WORDS, True,
				XA_STRING, &actualType,
				&actualFormat, &numItems, &bytesAfter,
				&propInfo);

    /*
     * If the property doesn't exist or is improperly formed
     * then ignore it.
     */
    if ((result != Success) || (actualType != XA_STRING)
	    || (actualFormat != 8))
    {
	if (propInfo != NULL)
	    XFree(propInfo);
	return;
    }

    /*
     * Several commands and results could arrive in the property at
     * one time;  each iteration through the outer loop handles a
     * single command or result.
     */
    for (p = propInfo; (p - propInfo) < numItems; )
    {
	/*
	 * Ignore leading NULs; each command or result starts with a
	 * NUL so that no matter how badly formed a preceding command
	 * is, we'll be able to tell that a new command/result is
	 * starting.
	 */
	if (*p == 0)
	{
	    p++;
	    continue;
	}

	if ((*p == 'c' || *p == 'k') && (p[1] == 0))
	{
	    Window	resWindow;
	    char_u	*name, *script, *serial, *end, *res;
	    Bool        asKeys = *p == 'k';
	    garray_T	reply;

	    /*
	     * This is an incoming command from some other application.
	     * Iterate over all of its options.  Stop when we reach
	     * the end of the property or something that doesn't look
	     * like an option.
	     */
	    p += 2;
	    name = NULL;
	    resWindow = None;
	    serial = (char_u *)"";
	    script = NULL;
	    while (((p - propInfo) < numItems) && (*p == '-'))
	    {
		switch (p[1])
		{
		    case 'r':
			resWindow = (Window)strtoul((char *)p + 2,
							  (char **) &end, 16);
			if ((end == p + 2) || (*end != ' '))
			    resWindow = None;
			else
			    p = serial = end + 1;
			break;
		    case 'n':
			if (p[2] == ' ')
			    name = p + 3;
			break;
		    case 's':
			if (p[2] == ' ')
			    script = p + 3;
			break;
		}
		while (*p != 0)
		    p++;
		p++;
	    }

	    if ((script == NULL) || (name == NULL))
		continue;

	    /*
	     * Initialize the result property, so that we're ready at any
	     * time if we need to return an error.
	     */
	    if (resWindow != None)
	    {
		ga_init2(&reply, 1, 100);
		ga_grow(&reply, 50);
		sprintf(reply.ga_data, "%cr%c-s %s%c-r ", 0, 0, serial, 0);
		reply.ga_len = 10 + STRLEN(serial);
		reply.ga_room -= reply.ga_len;
	    }
	    res = NULL;
	    if (serverName != NULL && STRCMP(name, serverName) == 0)
	    {
		if (asKeys)
		    InjectStr(script);
		else
		    res = eval_to_string(script, NULL);
	    }
	    if (resWindow != None)
	    {
		if (res != NULL)
		    ga_concat(&reply, res);
		ga_append(&reply, 0);
		(void) AppendPropCarefully(dpy, resWindow, commProperty,
					   reply.ga_data, reply.ga_len);
	    }
	    vim_free(res);
	}
	else
	{
	    /*
	     * Didn't recognize this thing.  Just skip through the next
	     * null character and try again.
	     * Even if we get an 'r'(eply) we will throw it away as we
	     * never specify (and thus expect) one
	     */
	    while (*p != 0)
		p++;
	    p++;
	}
    }
    XFree(propInfo);
}

/*
 * AppendPropCarefully --
 *	Append a given property to a given window, but set up
 *	an X error handler so that if the append fails this
 *	procedure can return an error code rather than having
 *	Xlib panic.
 *
 *  Return:
 *	0 on OK - -1 on error
 */
    static int
AppendPropCarefully(dpy, window, property, value, length)
    Display *dpy;		/* Display on which to operate. */
    Window window;		/* Window whose property is to
				 * be modified. */
    Atom property;		/* Name of property. */
    char_u *value;		/* Characters  to append to property. */
    int  length;                /* How much to append */
{
    XErrorHandler old_handler;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    XChangeProperty(dpy, window, property, XA_STRING, 8,
	            PropModeAppend, value, length);
    XSync(dpy, False);
    (void) XSetErrorHandler(old_handler);
    return got_x_error ? -1 : 0;
}


/*
 * Another X Error handler, just used to check for errors.
 */
/* ARGSUSED */
    static int
x_error_check(dpy, error_event)
    Display *dpy;
    XErrorEvent	*error_event;
{
    got_x_error = TRUE;
    return 0;
}

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room
 */
    static void
InjectStr(str)
    char_u	*str;
{
    char_u      *ptr = NULL;

    str = replace_termcodes((char_u *)str, &ptr, FALSE, TRUE);
    add_to_input_buf(str, STRLEN(str));
    vim_free((char_u *)(ptr));
}

/*
 * Check if name looks like it had a 3 digit serial number appended
 */
    static int
IsSerialName(str)
    char_u	*str;
{
    if (STRLEN(str) < 5)
	return FALSE;
    str = str + STRLEN(str) - 4;
    if (*str++ != '-')
	return FALSE;
    if (!isdigit(*str++))
	return FALSE;
    if (!isdigit(*str++))
	return FALSE;
    if (!isdigit(*str++))
	return FALSE;

    return TRUE;
}

static int check_connection __ARGS((void));

    static int
check_connection()
{
    if (X_DISPLAY == NULL)
    {
	EMSG(_("E240: No connection to X server"));
	return FAIL;
    }
    return OK;
}

/*
 * Implements ":serversend {server} {string}"
 */
    void
ex_serversend(eap)
    exarg_T	*eap;
{
    char_u	*p;
    char_u	*s;

    if (check_connection() == FAIL)
	return;
    p = skiptowhite(eap->arg);
    if ((s = vim_strnsave(eap->arg, p - eap->arg)) == NULL)
	return;
    p = skipwhite(p);

    if (serverSendToVim(X_DISPLAY, s, p) < 0)
	EMSG2(_("E241: Unable to send to %s"), s);
    vim_free(s);
}

/*
 * Implements ":serverlist"
 */
/*ARGSUSED*/
    void
ex_serverlist(eap)
    exarg_T	*eap;
{
    char_u	*p, *cur, *next;

    if (check_connection() == FAIL)
	return;
    p = serverGetVimNames(X_DISPLAY);
    if (p == NULL)
	MSG(_("No servers found for this display"));
    else
    {
	next = cur = p;
	while (*next)
	{
	    cur = next;
	    if ((next = vim_strchr(cur, '\n')) == NULL)
		next = cur + STRLEN(cur);
	    if (next > cur)
	    {
		msg_putchar('\n');
		if (serverName != NULL
			&& STRNCMP(serverName, cur, next - cur) == 0
			&& (next - cur) == STRLEN(serverName))
		    msg_outtrans((char_u *)"> ");
		else
		    msg_outtrans((char_u *)"  ");
		msg_outtrans_len(cur, next - cur);
	    }
	    if (*next == '\n')
		next++;
	    out_flush();
	}
	vim_free(p);
    }
}

static char_u *build_drop_cmd __ARGS((int filec, char **filev));

    void
cmdsrv_main(argc, argv, cmdTarget)
    int		argc;
    char	**argv;
    char_u	*cmdTarget;
{
    char_u	*res, *s;
    int		i;
    char_u	*serverStr;
    int		didone = FALSE;

    setup_term_clip();
    if (xterm_dpy != NULL)
    {
	for (i = 1; i < argc; i++)
	{
	    if (STRCMP(argv[i], "--") == 0)
		break;
	    else if (STRICMP(argv[i], "--remote") == 0
		     || STRICMP(argv[i], "--serversend") == 0)
	    {
		if (i == argc - 1)
		    mainerr_arg_missing((char_u *)argv[i]);
		if (argv[i][2] == 'r')
		{
		    serverStr = build_drop_cmd(argc - i - 1, argv + i + 1);
		    argc = i;
		}
		else
		{
		    serverStr = (char_u *)argv[i + 1];
		    i++;
		}
		s = cmdTarget != NULL ? cmdTarget : gettail((char_u *)argv[0]);
		if (serverSendToVim(xterm_dpy, s, serverStr) < 0)
		{
		    MSG_ATTR(_("Send failed. Trying to execute locally"),
							      hl_attr(HLF_W));
		    break;      /* Break out to let vim start normally.  */
		}
	    }
	    else if (STRICMP(argv[i], "--serverlist") == 0)
	    {
		res = serverGetVimNames(xterm_dpy);
		if (res != NULL && *res)
		    mch_msg((char *)res);
		vim_free(res);
	    }
	    else
		continue;
	    didone = TRUE;
	}

	if (didone)
	    exit(0);     /* Mission accomplished - get out */
    }
}

    static char_u *
build_drop_cmd(filec, filev)
    int		filec;
    char	**filev;
{
    garray_T	ga;
    int		i;
    char_u	*inicmd = NULL;
    char_u	*p;
    char_u	cwd[MAXPATHL];

    if (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    if (filec <= 0 || mch_dirname(cwd, MAXPATHL) != OK)
	return NULL;
    if ((p = vim_strsave_escaped(cwd, PATH_ESC_CHARS)) == NULL)
	return NULL;
    ga_init2(&ga, 1, 100);
    ga_concat(&ga, (char_u *)"<C-\\><C-N>:cd ");
    ga_concat(&ga, p);
    ga_concat(&ga, (char_u *)"<CR>:drop ");
    for (i = 0; i < filec; i++)
    {
	vim_free(p);
	p = vim_strsave_escaped((char_u *)filev[i], PATH_ESC_CHARS);
	if (p == NULL)
	{
	    vim_free(ga.ga_data);
	    return NULL;
	}
	ga_concat(&ga, p);
	ga_concat(&ga, (char_u *)" ");
    }
    ga_concat(&ga, (char_u *)"<CR>:cd -");
    if (inicmd != NULL)
    {
	ga_concat(&ga, (char_u *)"<CR>:");
	ga_concat(&ga, inicmd);
    }
    ga_concat(&ga, (char_u *)"<CR>:<Esc>"); /* Execute & clear command line */
    return ga.ga_data;
}

#endif	/* FEAT_XCMDSRV */
