/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 * VMS port			by Henk Elbers
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 */

#include	"vim.h"
#include	"globals.h"
#include	"option.h"
#include	"proto.h"
#ifndef VAX
#undef O_RDWR
#undef AF_OSI
#include	"xti.h"
#define		select	t_select
#endif /*!VAX*/

#define EFN	0			/* Event flag */

static void	vms_flushbuf(void);
static void	vms_outchar(int c);
static int	vms_x(unsigned int fun);
static int	vms_inchar(int wtime);
static char	*vms_vwild(char *arg, char *wilds);
static int	vms_sys(char *cmd, char *log);

char	ibuf[16 /*1*/];			/* Input buffer */

#define NOTIM	0
#define TIM_0	1
#define TIM_1	2

typedef struct
{
    char	class;
    char	type;
    short	width;
    union
    {
	struct
	{
	    char	_basic[3];
	    char	length;
	}	y;
	int	basic;
    }	x;
    int		extended;
}	TT_MODE;

typedef struct
{
    short	buflen;
    short	itemcode;
    char	*bufadrs;
    int		*retlen;
}	ITEM;

typedef struct
{
    ITEM	equ;
    int		nul;
}	ITMLST1;

typedef struct
{
    ITEM	index;
    ITEM	string;
    int	nul;
}	ITMLST2;

#if defined(HAVE_X11) && defined(WANT_X11)
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>

Window		x11_window = 0;
Display		*x11_display = NULL;
int		got_x_error = FALSE;

static int	get_x11_windis __ARGS((void));
static void set_x11_title __ARGS((char_u *));
static void set_x11_icon __ARGS((char_u *));
#endif

static int	get_x11_title __ARGS((int));
static int	get_x11_icon __ARGS((int));

static void	may_core_dump __ARGS((void));

static int	Read __ARGS((char_u *, long));
static int	WaitForChar __ARGS((long));
static int	RealWaitForChar __ARGS((int, long));
static void	fill_inbuf __ARGS((int));

static RETSIGTYPE	sig_winch __ARGS(SIGPROTOARG);
static RETSIGTYPE	deathtrap __ARGS(SIGPROTOARG);

static void	catch_signals __ARGS((RETSIGTYPE (*func)()));
static int	have_wildcard __ARGS((int, char_u **));
static int	have_dollars __ARGS((int, char_u **));

static int	do_resize = FALSE;
static char_u	*oldtitle = NULL;
static char_u	*fixedtitle = (char_u *)"Thanks for flying Vim";
static char_u	*oldicon = NULL;
static char_u	*extra_shell_arg = NULL;
static int	show_shell_mess = TRUE;
static int	core_dump = FALSE;	/* core dump in mch_windexit() */
static TT_MODE	orgmode;

static void	ass_tty(void);
static TT_MODE	get_tty(void);
static void	set_tty(int r, int c);
static void	reset_tty(TT_MODE *mode);

#ifdef SYS_SIGLIST_DECLARED
/*
 * I have seen
 *	extern char *_sys_siglist[NSIG];
 * on Irix, Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.in
 */
#endif

static struct
{
    int		sig;		/* Signal number, eg. SIGSEGV etc */
    char	*name;		/* Signal name (not char_u!). */
    int		dump;		/* Should this signal cause a core dump? */
} signal_info[] =
{
#ifdef SIGHUP
	{SIGHUP,	"HUP",		FALSE},
#endif
#ifdef SIGINT
	{SIGINT,	"INT",		FALSE},
#endif
#ifdef SIGQUIT
	{SIGQUIT,	"QUIT",		TRUE},
#endif
#ifdef SIGILL
	{SIGILL,	"ILL",		TRUE},
#endif
#ifdef SIGTRAP
	{SIGTRAP,	"TRAP",		TRUE},
#endif
#ifdef SIGABRT
	{SIGABRT,	"ABRT",		TRUE},
#endif
#ifdef SIGEMT
	{SIGEMT,	"EMT",		TRUE},
#endif
#ifdef SIGFPE
	{SIGFPE,	"FPE",		TRUE},
#endif
#ifdef SIGBUS
	{SIGBUS,	"BUS",		TRUE},
#endif
#ifdef SIGSEGV
	{SIGSEGV,	"SEGV",		TRUE},
#endif
#ifdef SIGSYS
	{SIGSYS,	"SYS",		TRUE},
#endif
#ifdef SIGPIPE
	{SIGPIPE,	"PIPE",		FALSE},
#endif
#ifdef SIGALRM
	{SIGALRM,	"ALRM",		FALSE},
#endif
#ifdef SIGTERM
	{SIGTERM,	"TERM",		FALSE},
#endif
#ifdef SIGVTALRM
	{SIGVTALRM,	"VTALRM",	FALSE},
#endif
#ifdef SIGPROF
	{SIGPROF,	"PROF",		FALSE},
#endif
#ifdef SIGXCPU
	{SIGXCPU,	"XCPU",		TRUE},
#endif
#ifdef SIGXFSZ
	{SIGXFSZ,	"XFSZ",		TRUE},
#endif
#ifdef SIGUSR1
	{SIGUSR1,	"USR1",		FALSE},
#endif
#ifdef SIGUSR2
	{SIGUSR2,	"USR2",		FALSE},
#endif
	{-1,		"Unknown!",	-1}
};

static short	iochan;			/* TTY I/O channel */
static short	iosb[4];		/* IO status block */
static int	owidth = 80;
static int	opage = 24;

/*
 * mch_write()
 */
    void
mch_write(char_u *s, int len)
{
#ifdef USE_GUI
    if (gui.in_use && !gui.dying)
    {
	gui_write(s, len);
	if (p_wd)
	    gui_wait_for_chars(p_wd);
    }
    else
#endif
    {
	while (len-- > 0)
	    vms_outchar(*s++);
	vms_flushbuf();
    }
}

/*
 *	mch_inchar()	low level input funcion.
 *
 *	Get a characters from the keyboard.
 *	Return the number of characters that are available.
 *	If wtime == 0 do not wait for characters.
 *	If wtime == n wait a short time for characters.
 *	If wtime == -1 wait forever for characters.
 */

    int
mch_inchar(char_u *buf, int maxlen, long wtime)
{
    int		len, c, i = 0, m, ret, res, term[2] = { 0, 0 };
    char	mbuf[128];

    vms_flushbuf();
    if (wtime == -1)
	res = vms_x(NOTIM);			/* without timeout */
    else
	res = vms_x(wtime ? TIM_1 : TIM_0);	/* with timeout */
    if (res != SS$_NORMAL)
	return(0);
    mbuf[i] = *ibuf;
    vms_flushbuf();
    c = (ibuf[0] & 0xFF);			/* Allow multinational */
    *buf = c;
    return(1);

#ifdef USE_GUI
    if (gui.in_use)
    {
	if (!gui_wait_for_chars(wtime))
	    return 0;
	return Read(buf, (long)maxlen);
    }
#endif
#ifdef nee
    if (wtime >= 0)
    {
	ret = sys$qiow(EFN, iochan, IO$_READLBLK | IO$M_TIMED, iosb, 0, 0,
		       ibuf, 1, wtime, term, 0, 0);
	if ((ret != SS$_NORMAL) && (ret != SS$_TIMEOUT))
	    return(0);
	buf[i] = *ibuf;
	vms_flushbuf();
	return(1);
	while (WaitForChar(wtime) == 0)		/* no character available */
	{
	    if (!do_resize)			/* return if not interrupted by resize */
		return 0;
	    set_winsize(0, 0, FALSE);
	    do_resize = FALSE;
	}
    }
    else		/* wtime == -1 */
    {
	/*
	 * If there is no character available within 'updatetime' seconds
	 * flush all the swap files to disk
	 * Also done when interrupted by SIGWINCH.
	 */
	if (WaitForChar(p_ut) == 0)
	    updatescript(0);
    }

    for (;;)	/* repeat until we got a character */
    {
	if (do_resize)		/* window changed size */
	{
	    set_winsize(0, 0, FALSE);
	    do_resize = FALSE;
	}
	/*
	 * we want to be interrupted by the winch signal
	 */
	WaitForChar(-1L);
	if (do_resize)		/* interrupted by SIGWINCHsignal */
	    continue;

	/*
	 * For some terminals we only get one character at a time.
	 * We want the get all available characters, so we could keep on
	 * trying until none is available
	 * For some other terminals this is quite slow, that's why we don't do
	 * it.
	 */
	len = Read(buf, (long)maxlen);
	if (len > 0)
	    return len;
    }
    if (wtime == -1)
	ret = sys$qiow(EFN, iochan, IO$_READLBLK, iosb, 0, 0,
		       ibuf, 1, 0, term, 0, 0);
    else
	ret = sys$qiow(EFN, iochan, IO$_READLBLK | IO$M_TIMED, iosb, 0, 0,
		       ibuf, 1, wtime, term, 0, 0);
    if ((ret != SS$_NORMAL) && (ret != SS$_TIMEOUT))
	return(0);
    buf[i] = *ibuf;
    vms_flushbuf();
    return(1);
#endif /*nee*/
}

/*
 *	mch_char_avail	return non-zero if a character is available
 */

    int
mch_char_avail(void)
{
#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_update();
	return(!is_input_buf_empty());
    }
#endif
    return(WaitForChar(0L));
}

/*
 *	mch_avail_mem		always on VMS
 */

    long
mch_avail_mem(int special)
{
	return 0x7fffffff;		/* virtual memory eh */
}

/*
 *	mch_delay
 */

    void
mch_delay(long msec, int ignoreinput)
{
	if (!ignoreinput)
		WaitForChar(msec);
}

/*
 *	sig_winch
 *
 * We need correct potatotypes, otherwise mean compilers will barf when
 * the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */

    static RETSIGTYPE
sig_winch SIGDEFARG(sigarg)
{
#if defined(SIGWINCH)
	/* this is not required on all systems, but it doesn't hurt anybody */
	signal(SIGWINCH, (RETSIGTYPE (*)())sig_winch);
#endif
	do_resize = TRUE;
	SIGRETURN;
}

/*
 *	mch_resize
 */

	void
mch_resize(void)
{
	do_resize = TRUE;
}

/*
 * This function handles deadly signals.
 * It tries to preserve any swap file and exit properly.
 * (partly from Elvis).
 */
    static RETSIGTYPE
deathtrap SIGDEFARG(sigarg)
{
    static int		entered = 0;
#ifdef SIGHASARG
    int			i;

    for (i = 0; signal_info[i].dump != -1; i++)
    {
	if (sigarg == signal_info[i].sig)
	{
	    if (signal_info[i].dump)
		core_dump = TRUE;
	    break;
	}
    }
#endif

    /*
     * If something goes wrong after entering here, we may get here again.
     * When this happens, give a message and try to exit nicely (resetting the
     * terminal mode, etc.)
     * When this happens twice, just exit, don't even try to give a message,
     * stack may be corrupt or something weird.
     */
    if (entered == 2)
    {
	may_core_dump();
	exit(7);
    }
    if (entered)
    {
	OUT_STR("Vim: Double signal, exiting\n");
	vms_flushbuf();
	getout(1);
    }
    ++entered;

    sprintf((char *)IObuff, "Vim: Caught %s %s\n",
#ifdef SIGHASARG
			    "deadly signal", signal_info[i].name
#else
			    "some", "deadly signal"
#endif
			    );

    /*preserve_exit();			** preserve files and exit */

    SIGRETURN;
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 * When running the GUI iconify the window.
 */
    void
mch_suspend(void)
{
    int		sw_detached = 1, res, ppid = getppid(), pid = getpid();
    char	symstr[80];
    stat_t	osb, nsb;

#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_iconify();
	return;
    }
#endif
#ifdef SIGTSTP
    vms_flushbuf();	/* needed to make cursor visible on some systems */
    settmode(0);
    vms_flushbuf();	/* needed to disable mouse on some systems */
    kill(0, SIGTSTP);	/* send ourselves a STOP signal */

    /*
     * Set oldtitle to NULL, so the current title is obtained again.
     */
    if (oldtitle != fixedtitle)
    {
	vim_free(oldtitle);
	oldtitle = NULL;
    }
    settmode(1);
#else
    if (!sw_detached)
    {
	OUT_STR("new shell started\n");
	(void)mch_call_shell(NULL, SHELL_COOKED);
	return;
    }
    vms_flushbuf();
    settmode(0);				/* set to cooked mode */

    sprintf(symstr, "%08X", pid);
    vim_setenv("VIMPID", symstr);

    /* windgoto(wins[cw].rows-1,0);	*/
    /* putchar('\r');	*/
    /* putchar('\n');	*/
    /* outstr(T_ED);	*/
    /* printf("\033[%d;1H\033[J", ttmode.x.y.length);	*/

    res = lib$attach(&ppid);
    if (!(res & 1))
    {
	OUT_STR("\nDetach failed, new shell started\n");
	(void)mch_call_shell(NULL, SHELL_COOKED);
    }

    /* set_tty(P(P_LI), P(P_CO));*/
    /* screenclear(cw);*/
    /* updatescreen();*/

    settmode(1);					/* set to raw mode */
    resettitle();
#endif
    need_check_timestamps = TRUE;
}

    void
mch_windinit(void)
{
    Columns = 80;
    Rows = 24;

    vms_flushbuf();
    (void)mch_get_winsize();

    /*
     * arrange for signals to be handled
     */
#if defined(SIGWINCH)
    signal(SIGWINCH, (RETSIGTYPE (*)())sig_winch);
#endif
#ifdef SIGTSTP
    signal(SIGTSTP, SIG_DFL);
#endif
    catch_signals(deathtrap);
}

    static void
catch_signals(func)
    RETSIGTYPE (*func)();
{
	int		i;

	for (i = 0; signal_info[i].dump != -1; i++)
		signal(signal_info[i].sig, func);
}

    void
reset_signals(void)
{
	catch_signals(SIG_DFL);
}

/*
 * Check_win checks whether we have an interactive window.
 * If not, just give a warning message and continue.
 */

    int
mch_check_win(int argc, char **argv)
{
	if (isatty(1))
		return OK;
	return FAIL;
}

    int
mch_check_input(void)
{
	if (isatty(0))
		return OK;
	return FAIL;
}

#if defined(HAVE_X11) && defined(WANT_X11)

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    static int
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
	XGetErrorText(dpy, error_event->error_code, (char *)IObuff, IOSIZE);
	STRCAT(IObuff, "\nVim: Got X error\n");

	/* preserve_exit();			** preserve files and exit */

	return 0;			/* NOTREACHED */
}

/*
 * Another X Error handler, just used to check for errors.
 */
    static int
x_error_check(Display *dpy, XErrorEvent *error_event)
{
	got_x_error = TRUE;
	return 0;
}

/*
 *	get_x11_windis		try to get x11 window and display
 *
 *	return FAIL for failure, OK otherwise
 */

    static int
get_x11_windis(void)
{
    char		*winid;
    XTextProperty	text_prop;
    int			(*old_handler)();
    static int		result = -1;
    static int		x11_display_opened_here = FALSE;

    /* X just exits if it finds an error otherwise! */
    XSetErrorHandler(x_error_handler);

#ifdef USE_GUI_MOTIF
    if (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	if (x11_display_opened_here && x11_display != NULL)
	{
	    XCloseDisplay(x11_display);
	    x11_display = NULL;
	    x11_display_opened_here = FALSE;
	}
	return gui_get_x11_windis(&x11_window, &x11_display);
    }
#endif

    if (result != -1)		/* Have already been here and set this */
	return result;		/* Don't do all these X calls again */

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    if (x11_window == 0 &&
	(winid = (char *)vim_getenv((unsigned char *)"WINDOWID")) != NULL)
	    x11_window = (Window)atol(winid);
    if (x11_window != 0 && x11_display == NULL)
    {
	x11_display = XOpenDisplay(NULL);
	if (x11_display != NULL)
	{
	    /*
	     * Try to get the window title.  I don't actually want it yet, so
	     * there may be a simpler call to use, but this will cause the
	     * error handler x_error_check() to be called if anything is wrong,
	     * such as the window pointer being invalid (as can happen when the
	     * user changes his DISPLAY, but not his WINDOWID) -- webb
	     */
	    old_handler = XSetErrorHandler(x_error_check);
	    got_x_error = FALSE;
	    if (XGetWMName(x11_display, x11_window, &text_prop))
		XFree((void *)text_prop.value);
	    XSync(x11_display, False);
	    if (got_x_error)
	    {
		/* Maybe window id is bad */
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = NULL;
	    }
	    else
		x11_display_opened_here = TRUE;
	    XSetErrorHandler(old_handler);
	}
    }
    if (x11_window == 0 || x11_display == NULL)
	return (result = FAIL);
    return (result = OK);
}

/*
 *	get_x11_title	Determine original x11 Window Title
 */

    static int
get_x11_title(int test_only)
{
    XTextProperty	text_prop;
    int			retval = FALSE;

    if (get_x11_windis() == OK)
    {
	/* Get window name if any */
	if (XGetWMName(x11_display, x11_window, &text_prop))
	{
	    if (text_prop.value != NULL)
	    {
		retval = TRUE;
		if (!test_only)
		    oldtitle = strsave((char_u *)text_prop.value);
	    }
	    XFree((void *)text_prop.value);
	}
    }
    if (oldtitle == NULL && !test_only)		/* could not get old title */
	oldtitle = fixedtitle;
    return retval;
}

/*
 *	get_x11_icon	Determine original x11 Window icon
 */

    static int
get_x11_icon(int test_only)
{
    XTextProperty	text_prop;
    int			retval = FALSE;

    if (get_x11_windis() == OK)
    {
		/* Get icon name if any */
	if (XGetWMIconName(x11_display, x11_window, &text_prop))
	{
	    if (text_prop.value != NULL)
	    {
		retval = TRUE;
		if (!test_only)
		    oldicon = strsave((char_u *)text_prop.value);
	    }
	    XFree((void *)text_prop.value);
	}
    }

	    /* could not get old icon, use terminal name */
    if (oldicon == NULL && !test_only)
    {
	if (STRNCMP(term_strings[KS_NAME], "builtin_", 8) == 0)
	    oldicon = term_strings[KS_NAME] + 8;
	else
	    oldicon = term_strings[KS_NAME];
    }

    return retval;
}

/*
 *	set_x11_title	Set x11 Window Title
 *
 *	get_x11_windis() must be called before this and have returned OK
 */

    static void
set_x11_title(char_u *title)
{
#if XtSpecificationRelease >= 4
    XTextProperty	text_prop;

    text_prop.value = title;
    text_prop.nitems = STRLEN(title);
    text_prop.encoding = XA_STRING;
    text_prop.format = 8;
    XSetWMName(x11_display, x11_window, &text_prop);
#else
    XStoreName(x11_display, x11_window, (char *)title);
#endif
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    static void
set_x11_icon(char_u *icon)
{
#if XtSpecificationRelease >= 4
    XTextProperty text_prop;

    text_prop.value = icon;
    text_prop.nitems = STRLEN(icon);
    text_prop.encoding = XA_STRING;
    text_prop.format = 8;
    XSetWMIconName(x11_display, x11_window, &text_prop);
#else
    XSetIconName(x11_display, x11_window, (char *)icon);
#endif
    XFlush(x11_display);
}

#else	/* HAVE_X11 && WANT_X11 */

    static int
get_x11_title(int test_only)
{
	if (!test_only)
		oldtitle = fixedtitle;
	return FALSE;
}

    static int
get_x11_icon(int test_only)
{
	if (!test_only)
	{
		if (STRNCMP(term_strings[KS_NAME], "builtin_", 8) == 0)
			oldicon = term_strings[KS_NAME] + 8;
		else
			oldicon = term_strings[KS_NAME];
	}
	return FALSE;
}

#endif	/* HAVE_X11 && WANT_X11 */


    int
mch_can_restore_title(void)
{
#ifdef USE_GUI
    /*
     * If GUI is (going to be) used, we can always set the window title.
     * Saves a bit of time, because the X11 display server does not need to be
     * contacted.
     */
    if (gui.starting || gui.in_use)
	return TRUE;
#endif
    return get_x11_title(TRUE);
}

/*
 *	mch_can_restore_icon
 */

    int
mch_can_restore_icon(void)
{
#ifdef USE_GUI
    /*
     * If GUI is (going to be) used, we can always set the icon name.
     * Saves a bit of time, because the X11 display server does not need to be
     * contacted.
     */
    if (gui.starting || gui.in_use)
	return TRUE;
#endif
    return(get_x11_icon(TRUE));
}

/*
 *	mch_settitle	set the window title and icon (xterm only)
 */

    void
mch_settitle(char_u *title, char_u *icon)
{
    int		type = 0;
    char_u	title_buf[80];

    if (term_strings[KS_NAME] == NULL)		/* no terminal name (yet) */
	return;
    if (title == NULL && icon == NULL)		/* nothing to do */
	return;
/*
 * if the window ID and the display is known, we may use X11 calls
 */
#if defined(HAVE_X11) && defined(WANT_X11)
    if (get_x11_windis() == OK)
	type = 1;
#endif

    /*
     * Note: if terminal is xterm, title is set with escape sequence rather
     *		 than x11 calls, because the x11 calls don't always work
     * Check only if the start of the terminal name is "xterm", also catch
     * "xterms".
     */
    if (is_xterm(term_strings[KS_NAME]))
	type = 2;
    if (is_iris_ansi(term_strings[KS_NAME]))
	type = 3;
    if (type)
    {
	if (title != NULL)
	{
	    if (oldtitle == NULL)		/* first call, save title */
		(void)get_x11_title(FALSE);
	    switch(type)
	    {
#if defined(HAVE_X11) && defined(WANT_X11)
		case 1:
		    set_x11_title(title);			/* x11 */
		    break;
#endif /*HAVE_X11+WANT_X11*/
		case 2:
		    sprintf((char *)title_buf, "\033]2;%s\007", title);
		    OUT_STR_NF(title_buf);
		    vms_flushbuf();
		    break;
		case 3:
		    sprintf((char *)title_buf, "\033P1.y%s\234", title);
		    OUT_STR_NF(title_buf);
		    vms_flushbuf();
		    break;
	    }
	}
	if (icon != NULL)
	{
	    if (oldicon == NULL)		/* first call, save icon */
		get_x11_icon(FALSE);
	    switch(type)
	    {
#if defined(HAVE_X11) && defined(WANT_X11)
		case 1:
		    set_x11_icon(icon);			/* x11 */
		    break;
#endif /*HAVE_X11&&WANT_X11*/
		case 2:					/* xterm */
		    sprintf((char *)title_buf, "\033]1;%s\007", icon);
		    OUT_STR_NF(title_buf);
		    vms_flushbuf();
		    break;
		case 3:					/* iris-ansi */
		    sprintf((char *)title_buf, "\033P3.y%s\234", icon);
		    OUT_STR_NF(title_buf);
		    vms_flushbuf();
		    break;
	    }
	}
    }
}

    int
is_xterm(char_u *name)
{
    if (name == NULL)
	return FALSE;
    return(vim_strnicmp((char *)name, "xterm", (size_t)5) == 0 ||
	    STRCMP(name, "builtin_xterm") == 0);
}

    int
is_iris_ansi(char_u *name)
{
    if (name == NULL)
	return FALSE;
    return(vim_strnicmp((char *)name, "iris-ansi", (size_t)9) == 0 ||
	    STRCMP(name, "builtin_iris-ansi") == 0);
}

/*
 * Return TRUE if "name" is a terminal for which 'ttyfast' should be set.
 * This should include all windowed terminal emulators.
 */
    int
is_fastterm(char_u *name)
{
    if (name == NULL)
	return FALSE;
    if (is_xterm(name) || is_iris_ansi(name))
	return TRUE;
    return(vim_strnicmp((char *)name, "hpterm", (size_t)6) == 0 ||
	    vim_strnicmp((char *)name, "sun-cmd", (size_t)7) == 0);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *	1  Just restore title
 *  2  Just restore icon
 *	3  Restore title and icon
 */
    void
mch_restore_title(int which)
{
    mch_settitle((which & 1) ? oldtitle : NULL, (which & 2) ? oldicon : NULL);
}

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    int
mch_get_user_name(char_u *s, int len)
{
    strncpy((char *)s, cuserid(NULL), len);
    return(OK);
}

/*
 * Insert host name is s[len].
 */

    void
mch_get_host_name(char_u *s, int len)
{
    gethostname((char *)s, len);
}

/*
 * return process ID
 */
    long
mch_get_pid(void)
{
    return(long)getpid();
}

#if !defined(HAVE_STRERROR)
static char *strerror __ARGS((int));

    static char *
strerror(int err)
{
    extern int		sys_nerr;
    extern char		*sys_errlist[];
    static char		er[20];

    if (err > 0 && err < sys_nerr)
	    return (sys_errlist[err]);
    sprintf(er, "Error %d", err);
    return er;
}
#endif

/*
 *	mch_dirname	Get name of current directory into buffer 'buf'
 *			of length 'len' bytes. Return OK for success,
 *			FAIL for failure.
 */

    int
mch_dirname(char_u *buf, int len)
{
    if (!getcwd((char *)buf, len))
    {
	STRCPY(buf, strerror(errno));
	return(FAIL);
    }
    return(OK);
}

/*
 *	mch_FullName	get absolute filename into buffer 'buf' of
 *			length 'len' bytes, return FAIL for failure,
 *			OK for success
 *	force		also expand when already absolute path name
 */

    int
mch_FullName(char_u *fname, char_u *buf, int len, int force)
{
    int		l, retval = OK;
    char_u	olddir[MAXPATHL], *p, c;

    *buf = NUL;
    if (!fname)						/* always fail */
	return(FAIL);
    if (force || !mch_isFullName(fname)) /* if forced or not an absolute path */
    {	/*
	 * If the file name has a path, change to that directory for a
	 * moment, and then do the getwd() (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	if ((p = vim_strrchr(fname, '/')) != NULL)
	{
	    if (mch_dirname(olddir, MAXPATHL) == FAIL)
	    {
		p = NULL;		/* can't get current dir: don't chdir */
		retval = FAIL;
	    }
	    else
	    {	/*
		 * The directory is copied into buf[], to be able to remove
		 * the file name without changing it (could be a string in
		 * read-only memory)
		 */
		if (p - fname >= len)
		    retval = FAIL;
		else
		{
		    STRNCPY(buf, fname, p - fname);
		    buf[p - fname] = NUL;
		    if (vim_chdir((char *)buf))
			retval = FAIL;
		    else
			fname = p + 1;
		    *buf = NUL;
		}
	    }
	}
	if (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	l = STRLEN(buf);
	if (p)
	    vim_chdir((char *)olddir);
    }
    STRCAT(buf, fname);
    return(retval);
}

/*
 *	mch_isFullName	return TRUE is fname is an absolute path name
 */

    int
mch_isFullName(char_u *fname)
{
    return(strchr((char *)fname, ':')?1:0);
    /*return(strchr((char *)fname, ':') || strchr((char *)fname, '['));*/
}


/*
 * get file permissions for 'name'
 */
    long
getperm(char_u *name)
{
	struct stat statb;

	if (stat((char *)name, &statb))
		return -1;
	return statb.st_mode;
}

/*
 *	mch_setperm	set file permission for 'name' to 'perm'
 *			return FAIL for failure, OK otherwise
 */
    int
mch_setperm(char_u *name, long perm)
{
    return(chmod((char *)name, perm) == 0 ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name)
{
    /* can't hide a file */
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory
 * return -1 for error
 */
    int
mch_isdir(char_u *name)
{
	struct stat statb;

	if (stat((char *)name, &statb))
		return FALSE;
#ifdef _POSIX_SOURCE
	return (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
#else
	return ((statb.st_mode & S_IFMT) == S_IFDIR ? TRUE : FALSE);
#endif
}

    void
mch_windexit(int r)
{
    mch_settitle(oldtitle, oldicon);	/* restore xterm title */
    settmode(0);
    exiting = TRUE;
    stoptermcap();
    vms_flushbuf();
    ml_close_all(TRUE);			/* remove all memfiles */
    may_core_dump();
    exit(r);
}

    static void
may_core_dump(void)
{
#ifdef SIGQUIT
	signal(SIGQUIT, SIG_DFL);
	if (core_dump)
		kill(getpid(), SIGQUIT);		/* force a core dump */
#endif
}

    void
mch_settmode(int raw)
{
    if (raw)
	set_tty(0, 0);
    else
	reset_tty(&orgmode);
}

    static void
ass_tty(void)
{
    DESC	odsc;
    char	*cp1, *cp2 = "TT";

    if (iochan)
	return;
    while (cp1 = (char *)vim_getenv((char_u *)cp2))
	cp2 = cp1;
    vul_desc(&odsc, cp2);
    (void)sys$assign(&odsc, &iochan, 0, 0);
}

    static void
set_tty(int r, int c)
{
    char	oname[40];
    long	mystatus;
    TT_MODE	newmode;		/* New TTY mode bits		*/
    static int	first_time = 1;

    ass_tty();
    if (first_time)
    {
	orgmode = get_tty();
	first_time = 0;
    }
    newmode = get_tty();
    if (c)
	newmode.width		=  c;
    if (r)
	newmode.x.y.length	=  r;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    mystatus = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, sizeof(newmode), 0, 0, 0, 0);
    if (mystatus!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	return;
}

    static TT_MODE
get_tty(void)
{
    long	mystatus;
    TT_MODE	tt_mode;

    ass_tty();
    mystatus = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, sizeof(tt_mode), 0, 0, 0, 0);
    if (mystatus != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    return(tt_mode);
}

    static void
reset_tty(TT_MODE *mode)
{
    long	mystatus;

    switch (mode->width)
    {
	    case 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	break;
	    case 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	break;
	    default:	break;
    }
    vms_flushbuf();
    mystatus = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			mode, sizeof(TT_MODE), 0,0,0,0);
    if (mystatus!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	return;
    (void)sys$dassgn(iochan);
    iochan = 0;
}

/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */

    void
vul_desc(DESC *des, char *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */

    static void
vul_item(ITEM *itm, short len, short cod, char *adr, int *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

/*
 *	get_tt_width	retourneer terminal-width, 132 voor non-tty
 */

    int
get_tt_width(char *dev)
{
    struct
    {
	unsigned short	bl0;		/* Maximale bufferlengte	*/
	unsigned short	cd0;		/* Itemcode			*/
	void		*bf0;		/* Adres waar antwoord in komt	*/
	unsigned short	*la0;		/* Adres waar lengte in komt	*/
	unsigned short	bl1;		/* Maximale bufferlengte	*/
	unsigned short	cd1;		/* Itemcode			*/
	void		*bf1;		/* Adres waar antwoord in komt	*/
	unsigned short	*la1;		/* Adres waar lengte in komt	*/
	unsigned short	bl2;		/* Maximale bufferlengte	*/
	unsigned short	cd2;		/* Itemcode			*/
	void		*bf2;		/* Adres waar antwoord in komt	*/
	unsigned short	*la2;		/* Adres waar lengte in komt	*/
	unsigned long	nl;
    }	itm;
    unsigned int		_width;
    unsigned short		l_width;
    unsigned int		_type;
    unsigned short		l_type;
    unsigned int		_page;
    unsigned short		l_page;
    DESC			devnam;
    struct
    {
	unsigned short	status;		/* I/O status */
	unsigned short	byte_count;
	unsigned long	pid;
    }				iosb;

    devnam.dsc$b_dtype = DSC$K_DTYPE_T;
    devnam.dsc$b_class = DSC$K_CLASS_S;
    devnam.dsc$w_length = strlen(dev);
    devnam.dsc$a_pointer = dev;
    itm.bl0 = sizeof(unsigned int);
    itm.cd0 = DVI$_DEVBUFSIZ;
    itm.bf0 = &_width;
    itm.la0 = &l_width;
    itm.bl1 = sizeof(unsigned int);
    itm.cd1 = DVI$_DEVCLASS;
    itm.bf1 = &_type;
    itm.la1 = &l_type;
    itm.bl2 = sizeof(unsigned int);
    itm.cd2 = DVI$_TT_PAGE;
    itm.bf2 = &_page;
    itm.la2 = &l_page;
    itm.nl = 0L;
    if (sys$getdviw(0, 0, &devnam, &itm, &iosb, 0, 0, 0) != SS$_NORMAL)
	_width = 132;
    if (_type != DC$_TERM)
	_width = 132;
    owidth = _width;
    opage = _page;
    return(_width);
}

/*
 * Try to get the code for "t_kb" from the stty setting
 *
 * Even if termcap claims a backspace key, the user's setting *should*
 * prevail.  stty knows more about reality than termcap does, and if
 * somebody's usual erase key is DEL (which, for most BSD users, it will
 * be), they're going to get really annoyed if their erase key starts
 * doing forward deletes for no reason. (Eric Fischer)
 */

    void
get_stty(void)
{
}

#ifdef USE_MOUSE

/*
 * set mouse clicks on or off (only works for xterms)
 */

    void
mch_setmouse(int on)
{
    static int	ison = FALSE;

    if (on == ison)		/* return quickly if nothing to do */
	return;
    if (is_xterm(term_strings[KS_NAME]))
    {
	if (on)
	    OUT_STR_NF((char_u *)"\033[?1000h"); /* xterm: enable mouse events */
	else
	    OUT_STR_NF((char_u *)"\033[?1000l"); /* xterm: disable mouse events */
    }
    ison = on;
}
#endif

/*
 * set screen mode, always fails.
 */
    int
mch_screenmode(char_u *arg)
{
	EMSG("Screen mode setting not supported");
	return FAIL;
}

/*
 * Try to get the current window size:
 * 1. with an ioctl(), most accurate method
 * 2. from the environment variables LINES and COLUMNS
 * 3. from the termcap
 * 4. keep using the old values
 */
    int
mch_get_winsize(void)
{
    TT_MODE		tmode;

    tmode = get_tty();			/* get size from VMS	*/
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    check_winsize();
    return(OK);
}

    void
mch_set_winsize(void)
{
#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_set_winsize();
	return;
    }
#endif
    set_tty(Rows, Columns);
    switch (Columns)
    {
	case 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	break;
	case 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	break;
	default:	break;
    }
    vms_flushbuf();
}

/*
 *	options		SHELL_FILTER	if called by do_filter()
 *			SHELL_COOKED	if term needs cooked mode
 *			SHELL_EXPAND	if called by
 *					mch_expand_wildcards()
 */

    int
mch_call_shell(char_u *cmd, int options)
{
    int		x;
    char	*ifn, *ofn;

    vms_flushbuf();
    if (options & SHELL_COOKED)
	settmode(0);				/* set to cooked mode */
    if (cmd)
    {
	if (ofn = strchr((char *)cmd, '>'))
	    *ofn++ = '\0';
	if (ifn = strchr((char *)cmd, '<'))
	    *ifn++ = '\0';
	if (ofn)
	    x = vms_sys((char *)cmd, ofn);
	else
	    x = system((char *)cmd);
    }
    else
	x = system((char *)p_sh);
    if (x == 127)
	OUT_STR("\nCannot execute shell sh\n");
    else if (x && !expand_interactively)
    {
	msg_putchar('\n');
	msg_outnum((long)x);
	OUT_STR(" returned\n");
    }
    settmode(1);					/* set to raw mode */
    resettitle();
    return(x ? FAIL : OK);
}

#ifdef TESTING_PTY
# undef USE_GUI
#endif

/*
 * The input characters are buffered to be able to check for a CTRL-C.
 * This should be done with signals, but I don't know how to do that in
 * a portable way for a tty in RAW mode.
 */

/*
 * Internal typeahead buffer.  Includes extra space for long key code
 * descriptions which would otherwise overflow.  The buffer is considered full
 * when only this extra space (or part of it) remains.
 */
#define INBUFLEN 250

static char_u	inbuf[INBUFLEN + MAX_KEY_CODE_LEN];
static int	inbufcount = 0;		/* number of chars in inbuf[] */

/*
 * is_input_buf_full(), is_input_buf_empty(), add_to_input_buf(), and
 * trash_input_buf() are functions for manipulating the input buffer.  These
 * are used by the gui_* calls when a GUI is used to handle keyboard input.
 *
 * NOTE: These functions will be identical in msdos.c etc, and should probably
 * be taken out and put elsewhere, but at the moment inbuf is only local.
 */

    int
is_input_buf_full()
{
	return (inbufcount >= INBUFLEN);
}

    int
is_input_buf_empty()
{
	return (inbufcount == 0);
}

/* Add the given bytes to the input buffer */
    void
add_to_input_buf(char_u *s, int len)
{
	if (inbufcount + len > INBUFLEN + MAX_KEY_CODE_LEN)
		return;		/* Shouldn't ever happen! */

	while (len--)
		inbuf[inbufcount++] = *s++;
}

/* Remove everything from the input buffer.  Called when ^C is found */
    void
trash_input_buf()
{
	inbufcount = 0;
}

    static int
Read(char_u *buf, long maxlen)
{
	if (inbufcount == 0)		/* if the buffer is empty, fill it */
		fill_inbuf(TRUE);
	if (maxlen > inbufcount)
		maxlen = inbufcount;
	vim_memmove(buf, inbuf, (size_t)maxlen);
	inbufcount -= maxlen;
	if (inbufcount)
		vim_memmove(inbuf, inbuf + maxlen, (size_t)inbufcount);
	return (int)maxlen;
}


    void
mch_breakcheck(void)
{
#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_update();
	return;
    }
#endif /* USE_GUI */
    /*
     * check for CTRL-C typed by reading all available characters
     */
    if (RealWaitForChar(0, 0L))		/* if characters available */
	fill_inbuf(FALSE);
}

/*
 *	fill_inbuf()
 */

    static void
fill_inbuf(int exit_on_error)
{
    int		len, try;

#ifdef USE_GUI
    if (gui.in_use)
    {
	gui_update();
	return;
    }
#endif
    if (is_input_buf_full())
	return;
    /*
     * Fill_inbuf() is only called when we really need a character.
     * If we can't get any, but there is some in the buffer, just return.
     * If we can't get any, and there isn't any in the buffer, we give up and
     * exit Vim.
     */
    for (try = 0; try < 100; ++try)
    {
	    len = read(0, (char *)inbuf + inbufcount,
										     (size_t)(INBUFLEN - inbufcount));
	    if (len > 0)
		    break;
	    if (!exit_on_error)
		    return;
    }
    if (len <= 0)
    {
	    windgoto((int)Rows - 1, 0);
	    fprintf(stderr, "Vim: Error reading input, exiting...\n");
	    ml_sync_all(FALSE, TRUE);		/* preserve all swap files */
	    getout(1);
    }
    while (len-- > 0)
    {
	    /*
	     * if a CTRL-C was typed, remove it from the buffer and set got_int
	     */
	    if (inbuf[inbufcount] == 3)
	    {
		    /* remove everything typed before the CTRL-C */
		    vim_memmove(inbuf, inbuf + inbufcount, (size_t)(len + 1));
		    inbufcount = 0;
		    got_int = TRUE;
	    }
	    ++inbufcount;
    }
}

/*
 * Wait "msec" msec until a character is available from the keyboard or from
 * inbuf[]. msec == -1 will block forever.
 * When a GUI is being used, this will never get called -- webb
 */

    static int
WaitForChar(long msec)
{
    if (inbufcount)		/* something in inbuf[] */
	return 1;
    return RealWaitForChar(0, msec);
}

/*
 *	RealWaitForChar		Wait "msec" msec until a character is
 *				available from file descriptor "fd".
 *				Time == -1 will block forever. When a
 *				GUI is being used, this will not be
 *				used for input -- webb
 */

    static int
RealWaitForChar(int fd, long msec)
{
    int		res;

    vms_flushbuf();
    switch (msec)
    {
	case -1: res = vms_x(NOTIM); break;	/* without timeout	*/
	case 0:  res = vms_x(TIM_0); break;	/* with timeout		*/
	default: res = vms_x(TIM_1); break;	/* with timeout		*/
    }
    return((!res || (res == SS$_NORMAL)) ? 0 : 1);
}

/*
 *	mch_expand_wildcards	this code does wild-card pattern
 *				matching NOT using the shell
 *
 *	return OK for success, FAIL for error (you may loose some
 *	memory) and put an error message in *file.
 *
 *	num_pat	   number of input patterns
 *	pat	   array of pointers to input patterns
 *	num_file   pointer to number of matched file names
 *	file	   pointer to array of pointers to matched file names
 *
 *	TODO: On VMS we do not check for files only yet
 *	TODO: On VMS EW_NOTFOUND is ignored
 */

    int
mch_expand_wildcards(int num_pat, char_u **pat, int *num_file, char_u ***file, int flags)
{
    int		i, j = 0, cnt = 0;
    char	*cp;

    *num_file = 0;			/* default: no files found	*/
    *file = (char_u **)"";
    for (i=0; i<num_pat; i++)
	while (cp = vms_vwild((char *)pat[i], NULL))
	    cnt++;
    if (cnt > 0)
    {
	if (!(*file = (char_u **)alloc(cnt * sizeof(char_u *))))
	{
	    *file = (char_u **)"";
	    return(FAIL);
	}
	for (i=0; i<num_pat; i++)
	    while (cp = vms_vwild((char *)pat[i], NULL))
		(*file)[j++] = vim_strsave((char_u *)cp);
    }
    else
    {
	if (!(*file = (char_u **)alloc(num_pat * sizeof(char_u *))))
	{
	    *file = (char_u **)"";
	    return(FAIL);
	}
	for (i=0; i<num_pat; i++)
	    (*file)[cnt++] = vim_strsave(pat[i]);
    }
    *num_file = cnt;
    return(OK);
}

/*
 *	mch_has_wildcard
 */

    int
mch_has_wildcard(char_u *p)
{
    for ( ; *p; ++p)
    {
	if (*p == '\\' && p[1] != NUL)
	    ++p;
	else if (vim_strchr((char_u *)"*%", *p) != NULL)
	    return TRUE;
    }
    return FALSE;
}

    static int
have_wildcard(int num, char_u **file)
{
    register int i;

    for (i = 0; i < num; i++)
	if (mch_has_wildcard(file[i]))
	    return 1;
    return 0;
}

/*
 *	have_dollars
 */

    static int
have_dollars(int num, char_u **file)
{
    register int i;

    for (i = 0; i < num; i++)
	if (vim_strchr(file[i], '$') != NULL)
	    return TRUE;
    return FALSE;
}

    void
getlinecol(void)
{
    char_u	tbuf[TBUFSZ];

    if (term_strings[KS_NAME] /*&& TGETENT(tbuf, term_strings[KS_NAME]) > 0*/)
    {
	if (Columns == 0)
	    Columns = 80; /*tgetnum("co");*/
	if (Rows == 0)
	    Rows = 24; /*tgetnum("li");*/
    }
}

/*
 *	from the VMS port of stevie
 */

    void
sig(int x)
{
    signal(SIGINT, sig);
    signal(SIGQUIT, sig);
    got_int = TRUE;
}

    unsigned char *
vim_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    static char		buffer[LNM$C_NAMLENGTH+1];
    unsigned char	*cp = NULL;
    unsigned long	attrib;
    int			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;

    vul_desc(&d_lognam, (char *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, sizeof(int), LNM$_INDEX, (char *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    if (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, NULL,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	if (cp = (char_u *)alloc((unsigned)(lengte+1)))
	    strcpy((char *)cp, buffer);
	return(cp);
    }
    else
	return(NULL);
}

#define NOBUF	1024			/* MM says bug buffers win!	*/
char	obuf[NOBUF];			/* Output buffer		*/
int	nobuf;				/* # of bytes in above		*/

    static void
vms_outchar(int c)
{
    if (nobuf >= NOBUF)
	vms_flushbuf();
    obuf[nobuf++] = c;
}

    static void
vms_flushbuf(void)
{
    int     status;
    status = SS$_NORMAL;
    if (nobuf != 0)
    {
	status = sys$qiow(EFN, iochan, IO$_WRITELBLK|IO$M_NOFORMAT, iosb, 0, 0,
			    obuf, nobuf, 0, 0, 0, 0);
	if (status == SS$_NORMAL)
	    status = iosb[0] & 0xFFFF;
	nobuf = 0;
    }
}

/*
 * If wtime == 0 do not wait for characters.		TIM_0	1
 * If wtime == n wait a short time for characters.	TIM_1	2
 * If wtime == -1 wait forever for characters.		NOTIM	0
 */

    static int
vms_inchar(int wtime)
{
    char	mbuf[128];
    int		i = 0, m, res;

    vms_flushbuf();
    if (wtime == -1)
	res = vms_x(NOTIM);			/* without timeout	*/
    else
	res = vms_x(wtime ? TIM_1 : TIM_0);	/* with timeout		*/
    if (res != SS$_NORMAL)
	return(-1);
    mbuf[i] = *ibuf;
    vms_flushbuf();
    return(ibuf[0] & 0xFF);			/* Allow multinational	*/
}

    static int
vms_x(unsigned int fun)
{
    int			ret, term[2] = { 0, 0 };

    switch(fun)
    {
	case NOTIM:		/* wait forever for characters		*/
	    ret = sys$qiow(EFN, iochan, IO$_READLBLK, iosb, 0, 0,
			   ibuf, 1, 0, term, 0, 0);
	    break;
	case TIM_1:		/* wait a short time for characters	*/
	    ret = sys$qiow(EFN, iochan, IO$_READLBLK | IO$M_TIMED, iosb, 0, 0,
			   ibuf, 1, 1, term, 0, 0);
	    break;
	default:		/* do not wait for characters		*/
	    ret = sys$qiow(EFN, iochan, IO$_READLBLK | IO$M_TIMED, iosb, 0, 0,
			   ibuf, 1, 0, term, 0, 0);
	    break;
    }
    if (ret != SS$_NORMAL)
	return(ret);
    if ((fun && (iosb[0] != SS$_TIMEOUT) && (iosb[0] != SS$_NORMAL)) ||
	(!fun && (iosb[0] != SS$_NORMAL)))
	return(iosb[0]);
    return(iosb[1] + iosb[3]);
}

/*
 *	vms_vwild	expand wildcards
 *
 *	arg	file string
 *	wilds	default wildcards
 */

    static char *
vms_vwild(char *arg, char *wilds)
{
    int			i, status;
    char		*cp;
    static struct FAB	f;
    static struct NAM	n;
    static char		exp_str[NAM$C_MAXRSS], res_str[NAM$C_MAXRSS];
    static char		inp_str[NAM$C_MAXRSS], fil_nam[NAM$C_MAXRSS];
    static char		*inp_ptr = NULL, *defwld = "[]*.*;0";

    if (!inp_ptr || strcmp(inp_ptr, arg)) /* new argument, re-initialize */
    {
	f = cc$rms_fab;
	n = cc$rms_nam;

	n.nam$l_rsa = res_str;
	n.nam$b_rss = sizeof(res_str);
	n.nam$l_esa = exp_str;
	n.nam$b_ess = sizeof(exp_str);
	f.fab$l_nam = &n;
	f.fab$l_fna = arg;
	f.fab$b_fns = strlen(arg);
	f.fab$l_dna = (wilds ? wilds : defwld);
	f.fab$b_dns = strlen(f.fab$l_dna);
	inp_ptr = inp_str;
	strcpy(inp_ptr, arg);
	if ((status = sys$parse(&f)) != RMS$_NORMAL)
	    return(NULL);
    }

    if ((status = sys$search(&f)) == RMS$_NORMAL)
    {
	*(cp = fil_nam) = '\0';			/* re-build filename */
	if (n.nam$l_fnb & NAM$M_NODE)
	    strncat(cp, n.nam$l_node, n.nam$b_node);
	if (n.nam$l_fnb & NAM$M_EXP_DEV)
	    strncat(cp, n.nam$l_dev, n.nam$b_dev);
	if (n.nam$l_fnb & NAM$M_EXP_DIR)
	    strncat(cp, n.nam$l_dir, n.nam$b_dir);
	if (n.nam$l_fnb & NAM$M_EXP_NAME)
	    strncat(cp, n.nam$l_name, n.nam$b_name);
	if (n.nam$l_fnb & NAM$M_EXP_TYPE)
	    strncat(cp, n.nam$l_type, n.nam$b_type);
	if (n.nam$l_fnb & NAM$M_EXP_VER)
	    strncat(cp, n.nam$l_ver, n.nam$b_ver);
	for (i=0; i<strlen(fil_nam); i++)
	    if (isupper(fil_nam[i]))
		fil_nam[i] = tolower(fil_nam[i]);
	return(fil_nam);
    }
    else
    {
	inp_ptr = NULL;
	return(NULL);
    }
}

    static int
vms_sys(char *cmd, char *log)
{
    DESC	cdsc, ldsc;
    long	status, substatus;

    if (cmd)
	vul_desc(&cdsc, cmd);
    if (log)
	vul_desc(&ldsc, log);
    status = lib$spawn(cmd ? &cdsc : NULL, 0, log ? &ldsc : NULL, 0, 0, 0,
			&substatus, 0, 0, 0);
    if (status != SS$_NORMAL)
	substatus = status;
    if ((substatus&STS$M_SUCCESS) == 0)     /* Command failed.	    */
	return(FALSE);
    return(TRUE);
}

/*
 *	vim_setenv	VMS version of setenv()
 */

    int
vim_setenv(char *var, char *value)
{
    int		res, dum;
    long	attrib = 0L;
    char	acmode = PSL$C_SUPER;	/* SYSNAM privilege nodig	*/
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, value ? strlen(value) : 0, value ? LNM$_STRING : 0,
	    value, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    return((res == 1) ? 0 : -1);
}

/*
 *	mch_getperm	Get file permissions for 'name',
 *			Returns -1 when it doesn't exist.
 */

    long
mch_getperm(char_u *name)
{
    struct stat	statb;

    if (stat((char *)name, &statb))
	return(-1);
    return(statb.st_mode);
}

/*
 *	mch_input_isatty	Return TRUE if the input comes from a
 *				terminal, FALSE otherwise.
 */

    int
mch_input_isatty(void)
{
    if (isatty(read_cmd_fd))
	return(TRUE);
    return(FALSE);
}

/*
 *	mch_expandpath	Recursively expand one path component into all
 *			matching files and/or directories.
 *			Return the number of matches found.
 */

    int
mch_expandpath(struct growarray *gap, char_u *path, int flags)
{
    int		cnt = 0;
    char	*cp;

    while (cp = vms_vwild((char *)path, NULL))
    {
	if (mch_getperm((char_u *)cp) >= 0)	/* add existing file */
	{
	    addfile(gap, (char_u *)cp, flags);
	    cnt++;
	}
    }
    return cnt;
}
