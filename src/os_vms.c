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

#ifndef VAX
# undef O_RDWR
# undef AF_OSI
# include	"xti.h"
# define	select	t_select
#endif

#define EFN	0			/* Event flag */

#ifdef PROTO
 typedef int DESC;
#endif

static void	vms_flushbuf(void);
static void	vms_outchar(int c);
static int	vms_x(unsigned int fun);
static int	vms_inchar(int wtime);
static int	vms_sys(char *cmd, char *log, char *inp);

char	ibuf[16];			/* Input buffer */

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

#ifdef FEAT_X11
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>

Window		x11_window = 0;
Display		*x11_display = NULL;
int		got_x_error = FALSE;

# ifdef FEAT_TITLE
static int	get_x11_windis __ARGS((void));
static void	set_x11_title __ARGS((char_u *));
static void	set_x11_icon __ARGS((char_u *));
# endif
#endif

#ifdef FEAT_TITLE
static int	get_x11_title __ARGS((int));
static int	get_x11_icon __ARGS((int));
static char_u	*oldtitle = NULL;
static int	did_set_title = FALSE;
static char_u	*fixedtitle = (char_u *)"Thanks for flying Vim";
static char_u	*oldicon = NULL;
static int	did_set_icon = FALSE;
#endif

static void	may_core_dump __ARGS((void));

static int	Read __ARGS((char_u *, long));
static int	WaitForChar __ARGS((long));
static void	fill_inbuf __ARGS((int));

static RETSIGTYPE	sig_winch __ARGS(SIGPROTOARG);
static RETSIGTYPE	deathtrap __ARGS(SIGPROTOARG);
static void set_signals __ARGS((void));
static void catch_signals __ARGS((RETSIGTYPE (*func_deadly)(), RETSIGTYPE (*func_other)()));

static int	have_wildcard __ARGS((int, char_u **));
static int	have_dollars __ARGS((int, char_u **));

static int	do_resize = FALSE;
static char_u	*extra_shell_arg = NULL;
static int	show_shell_mess = TRUE;
static int	deadly_signal = 0;	/* The signal we caught */
static TT_MODE	orgmode;

static void	ass_tty(void);
static TT_MODE	get_tty(void);
static void	set_tty(int r, int c);
static void	reset_tty(TT_MODE *mode);
static int curr_tmode = TMODE_COOK;	/* contains current terminal mode */

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

static struct signalinfo
{
    int     sig;	/* Signal number, eg. SIGSEGV etc */
    char    *name;	/* Signal name (not char_u!). */
    char    deadly;	/* Catch as a deadly signal? */
} signal_info[] =
{
#ifdef SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#endif
#ifdef SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#endif
#ifdef SIGILL
    {SIGILL,	    "ILL",	TRUE},
#endif
#ifdef SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#endif
#ifdef SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#endif
#ifdef SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#endif
#ifdef SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#endif
#ifdef SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#endif
#ifdef SIGSEGV
    {SIGSEGV,	    "SEGV",	TRUE},
#endif
#ifdef SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#endif
#ifdef SIGALRM
    {SIGALRM,	    "ALRM",	FALSE}, /* Perl's alarm() can trigger it */
#endif
#ifdef SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#endif
#ifdef SIGVTALRM
    {SIGVTALRM,     "VTALRM",	TRUE},
#endif
#ifdef SIGPROF
    {SIGPROF,	    "PROF",	TRUE},
#endif
#ifdef SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#endif
#ifdef SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#endif
#ifdef SIGUSR1
    {SIGUSR1,	    "USR1",	TRUE},
#endif
#ifdef SIGUSR2
    {SIGUSR2,	    "USR2",	TRUE},
#endif
#ifdef SIGINT
    {SIGINT,	    "INT",	FALSE},
#endif
#ifdef SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#endif
#ifdef SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#endif
#ifdef SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#endif
    {-1,	    "Unknown!", FALSE}
};

static short	iochan;			/* TTY I/O channel */
static short	iosb[4];		/* IO status block */
static int	owidth = 80;
static int	opage = 24;

    void
mch_write(char_u *s, int len)
{
#ifdef FEAT_GUI
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
 * low level input funcion.
 *
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    int
mch_inchar(char_u *buf, int maxlen, long wtime)
{
    int		c, res;

#ifdef FEAT_GUI
    if (gui.in_use)
    {
	if (!gui_wait_for_chars(wtime))
	    return 0;
	return read_from_input_buf(buf, (long)maxlen);
    }
#endif
    /* first check to see if any characters read by
     * mch_breakcheck(), mch_delay() or mch_char_avail()
     */
    if (!vim_is_input_buf_empty())
	return read_from_input_buf(buf, (long)maxlen);

    vms_flushbuf();
    if (wtime == -1)
	res = vms_x(NOTIM);			/* without timeout */
    else
	res = vms_x(wtime ? TIM_1 : TIM_0);	/* with timeout */
    if (res != SS$_NORMAL)
	return(0);
    vms_flushbuf();
    c = (ibuf[0] & 0xFF);			/* Allow multinational */
    *buf = c;
    return(1);
}

/*
 * return non-zero if a character is available
 */
    int
mch_char_avail(void)
{
#ifdef FEAT_GUI
    if (gui.in_use)
    {
	gui_mch_update();
	return !vim_is_input_buf_empty();
    }
#endif
    return WaitForChar(0L);
}

    void
mch_delay(long msec, int ignoreinput)
{
    if (ignoreinput)
	sleep(msec/1000);  /* as good as it gets for VMS? */
    else
	WaitForChar(msec);
}

/*
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

    void
mch_resize(void)
{
    do_resize = TRUE;
}

/*
 * We need correct potatotypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h  (jw).
 */
#if defined(SIGWINCH)
/* ARGSUSED */
    static RETSIGTYPE
sig_winch SIGDEFARG(sigarg)
{
    /* this is not required on all systems, but it doesn't hurt anybody */
    signal(SIGWINCH, (RETSIGTYPE (*)())sig_winch);
    do_resize = TRUE;
    SIGRETURN;
}
#endif

#if defined(SIGINT)
/* ARGSUSED */
    static RETSIGTYPE
catch_sigint SIGDEFARG(sigarg)
{
    /* this is not required on all systems, but it doesn't hurt anybody */
    signal(SIGINT, (RETSIGTYPE (*)())catch_sigint);
    got_int = TRUE;
    SIGRETURN;
}
#endif

#ifdef SET_SIG_ALARM
/*
 * signal function for alarm().
 */
/* ARGSUSED */
    static RETSIGTYPE
sig_alarm SIGDEFARG(sigarg)
{
    /* doesn't do anything, just to break a system call */
    SIGRETURN;
}
#endif


/*
 * This function handles deadly signals.
 * It tries to preserve any swap file and exit properly.
 * (partly from Elvis).
 */

   static RETSIGTYPE
deathtrap SIGDEFARG(sigarg)
{
    static int	    entered = 0;
#ifdef SIGHASARG
    int     i;

    /* try to find the name of this signal */
    for (i = 0; signal_info[i].sig != -1; i++)
	if (sigarg == signal_info[i].sig)
	    break;
    deadly_signal = sigarg;
#endif

    full_screen = FALSE;	/* don't write message to the GUI, it might be
				 * part of the problem... */
    /*
     * If something goes wrong after entering here, we may get here again.
     * When this happens, give a message and try to exit nicely (resetting the
     * terminal mode, etc.)
     * When this happens twice, just exit, don't even try to give a message,
     * stack may be corrupt or something weird.
     */
    if (entered >= 2)
    {
	reset_signals();	/* don't catch any signals anymore */
	may_core_dump();
	exit(7);
    }
    if (entered++)
    {
	OUT_STR(_("Vim: Double signal, exiting\n"));
	out_flush();
	reset_signals();	/* don't catch any signals anymore */
	getout(1);
    }

    sprintf((char *)IObuff, _("Vim: Caught %s %s\n"),
#ifdef SIGHASARG
		    _("deadly signal"), signal_info[i].name
#else
		    _("some"), _("deadly signal")
#endif
	   );

    preserve_exit();		/* preserve files and exit */

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

#ifdef FEAT_GUI
    if (gui.in_use)
    {
	gui_mch_iconify();
	return;
    }
#endif
#ifdef SIGTSTP
    vms_flushbuf();	/* needed to make cursor visible on some systems */
    settmode(TMODE_COOK);
    vms_flushbuf();	/* needed to disable mouse on some systems */
    kill(0, SIGTSTP);	/* send ourselves a STOP signal */

# ifdef FEAT_TITLE
    /*
     * Set oldtitle to NULL, so the current title is obtained again.
     */
    if (oldtitle != fixedtitle)
    {
	vim_free(oldtitle);
	oldtitle = NULL;
    }
# endif
    settmode(TMODE_RAW);
#else
    if (!sw_detached)
    {
	suspend_shell();
	return;
    }
    vms_flushbuf();
    settmode(TMODE_COOK);		/* set to cooked mode */

    sprintf(symstr, "%08X", pid);
    /* added 3rd parameter SK 980904 */
    mch_setenv("VIMPID", symstr, 1);

    /* windgoto(wins[cw].rows-1,0);	*/
    /* putchar('\r');	*/
    /* putchar('\n');	*/
    /* outstr(T_ED);	*/
    /* printf("\033[%d;1H\033[J", ttmode.x.y.length);	*/

    res = lib$attach(&ppid);
    if (!(res & 1))
    {
	OUT_STR(_("\nDetach failed"));
	suspend_shell();
    }

    /* set_tty(P(P_LI), P(P_CO));*/
    /* screenclear(cw);*/
    /* updatescreen();*/

    settmode(TMODE_RAW);		/* set to raw mode */
#ifdef FEAT_TITLE
    resettitle();
#endif
#endif
    need_check_timestamps = TRUE;
}

    void
mch_shellinit(void)
{
    Columns = 80;
    Rows = 24;

    vms_flushbuf();
    (void)mch_get_shellsize();
    set_signals();
}

    static void
set_signals()
{
#if defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    signal(SIGWINCH, (RETSIGTYPE (*)())sig_winch);
#endif

    /*
     * We want the STOP signal to work, to make mch_suspend() work.
     * For "rvim" the STOP signal is ignored.
     */
#ifdef SIGTSTP
    signal(SIGTSTP, restricted ? SIG_IGN : SIG_DFL);
#endif
#ifdef _REENTRANT
    signal(SIGCONT, sigcont_handler);
#endif

    /*
     * We want to ignore breaking of PIPEs.
     */
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif

    /*
     * We want to catch CTRL-C (only works while in Cooked mode).
     */
#ifdef SIGINT
    signal(SIGINT, (RETSIGTYPE (*)())catch_sigint);
#endif

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#ifdef SIGALRM
    signal(SIGALRM, SIG_IGN);
#endif

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_signals(deathtrap, SIG_ERR);

#if defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    if (gui.in_use)
	signal(SIGHUP, SIG_IGN);
#endif
}

    void
reset_signals()
{
    catch_signals(SIG_DFL, SIG_DFL);
#ifdef _REENTRANT
    /* SIGCONT isn't in the list, because its default action is ignore */
    signal(SIGCONT, SIG_DFL);
#endif
}

    static void
catch_signals(func_deadly, func_other)
    RETSIGTYPE (*func_deadly)();
    RETSIGTYPE (*func_other)();
{
    int     i;

    for (i = 0; signal_info[i].sig != -1; i++)
	if (signal_info[i].deadly)
	    signal(signal_info[i].sig, func_deadly);
#ifdef SIG_ERR
	else if (func_other != SIG_ERR)
#else
	else if (func_other != BADSIG)
#endif
	    signal(signal_info[i].sig, func_other);
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

#ifdef FEAT_TITLE

#ifdef FEAT_X11

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    static int
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
	XGetErrorText(dpy, error_event->error_code, (char *)IObuff, IOSIZE);
	STRCAT(IObuff, _("\nVim: Got X error\n"));

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
 *	FIXME: Include changes that were made to the Unix version of this
 *	function that apply here too.
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

#ifdef FEAT_GUI_MOTIF
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
	gui_get_x11_windis(&x11_window, &x11_display);
	return OK;
    }
#endif

    if (result != -1)		/* Have already been here and set this */
	return result;		/* Don't do all these X calls again */

    /* When started with the "-X" argument, don't try connecting. */
    if (x_no_connect)
	return FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    if (x11_window == 0 &&
	(winid = (char *)mch_getenv((char_u *)"WINDOWID")) != NULL)
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
		    oldtitle = vim_strsave((char_u *)text_prop.value);
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
		    oldicon = vim_strsave((char_u *)text_prop.value);
	    }
	    XFree((void *)text_prop.value);
	}
    }

	    /* could not get old icon, use terminal name */
    if (oldicon == NULL && !test_only)
    {
	if (STRNCMP(term_str(KS_NAME), "builtin_", 8) == 0)
	    oldicon = term_str(KS_NAME) + 8;
	else
	    oldicon = term_str(KS_NAME);
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

#else	/* FEAT_X11 */

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
		if (STRNCMP(term_str(KS_NAME), "builtin_", 8) == 0)
			oldicon = term_str(KS_NAME) + 8;
		else
			oldicon = term_str(KS_NAME);
	}
	return FALSE;
}

#endif	/* FEAT_X11 */


    int
mch_can_restore_title(void)
{
#ifdef FEAT_GUI
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
#ifdef FEAT_GUI
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
mch_settitle(title, icon)
    char_u *title;
    char_u *icon;
{
    int		type = 0;
    static int	recursive = 0;

    if (T_NAME == NULL)		/* no terminal name (yet) */
	return;
    if (title == NULL && icon == NULL)	/* nothing to do */
	return;

    /* When one of the X11 functions causes a deadly signal, we get here again
     * recursively.  Avoid hanging then (something is probably locked). */
    if (recursive)
	return;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    if (get_x11_windis() == OK)
	type = 1;
#else
# ifdef FEAT_GUI_BEOS
    /* we always have a 'window' */
    type = 1;
# endif
#endif

    /*
     * Note: if "t_TS" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */

    if ((type || *T_TS != NUL) && title != NULL)
    {
	if (oldtitle == NULL)		/* first call, save title */
	    (void)get_x11_title(FALSE);

	if (*T_TS != NUL)		/* it's OK if t_fs is empty */
	    term_settitle(title);
#ifdef FEAT_X11
	else
	    set_x11_title(title);		/* x11 */
#endif
	did_set_title = TRUE;
    }

    if ((type || *T_CIS != NUL) && icon != NULL)
    {
	if (oldicon == NULL)		/* first call, save icon */
	    get_x11_icon(FALSE);

	if (*T_CIS != NUL)
	{
	    out_str(T_CIS);			/* set icon start */
	    out_str_nf(icon);
	    out_str(T_CIE);			/* set icon end */
	    out_flush();
	}
#ifdef FEAT_X11
	else
	    set_x11_icon(icon);			/* x11 */
#endif
	did_set_icon = TRUE;
    }
    --recursive;
}


/*
 * Restore the window/icon title.
 * which is one of:
 * 1  Just restore title
 * 2  Just restore icon
 * 3  Restore title and icon
 */
    void
mch_restore_title(int which)
{
    /* only restore the title or icon when it has been set */
    mch_settitle(((which & 1) && did_set_title) ?
			(oldtitle ? oldtitle : p_titleold) : NULL,
				((which & 2) && did_set_icon) ? oldicon : NULL);
}

#endif /* FEAT_TITLE */

    int
vim_is_xterm(name)
    char_u *name;
{
    if (name == NULL)
	return FALSE;
    return (STRNICMP(name, "xterm", 5) == 0
		|| STRNICMP(name, "kterm", 5)==0
		|| STRCMP(name, "builtin_xterm") == 0);
}

    int
vim_is_iris(name)
    char_u  *name;
{
    if (name == NULL)
	return FALSE;
    return (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

    int
vim_is_vt300(name)
    char_u  *name;
{
    if (name == NULL)
	return FALSE;	/* actually all ANSI comp. terminals should be here  */
    return (STRNICMP(name, "vt3", 3) == 0  /* it will cover all from VT100-VT300 */
	    || STRNICMP(name, "vt2", 3) == 0  /* Note: from VT340 can hanle colors  */
	    || STRNICMP(name, "vt1", 3) == 0
	    || STRCMP(name, "builtin_vt320") == 0);
}


/*
 * Return TRUE if "name" is a terminal for which 'ttyfast' should be set.
 * This should include all windowed terminal emulators.
 */

vim_is_fastterm(name)
    char_u  *name;
{
    if (name == NULL)
	return FALSE;
    if (vim_is_vt300(name) || vim_is_xterm(name) || vim_is_iris(name) )
	return TRUE;
    return (   STRNICMP(name, "hpterm", 6) == 0
	    || STRNICMP(name, "sun-cmd", 7) == 0
	    || STRNICMP(name, "screen", 6) == 0
	    || STRNICMP(name, "rxvt", 4) == 0
	    || STRNICMP(name, "dtterm", 6) == 0 );
}


/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    int
mch_get_user_name(char_u *s, int len)
{
    strncpy((char *)s, cuserid(NULL), len);
    return OK;
}

/*
 * Insert host name is s[len].
 */

    void
mch_get_host_name(char_u *s, int len)
{
#if 0 /* defined(__DECC) && defined(__STDC__) */
    extern char_u *sys_hostname;

    /* presumably compiled with /decc */
    strcpy((char *)s, (char *)sys_hostname);
#else
# ifdef VAXC
    /* presumably compiled with /standard=vaxc */
    vaxc$gethostname((char *)s, len);
# else
    gethostname((char *)s, len);
# endif
#endif
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
    sprintf(er, _("Error %d"), err);
    return er;
}
#endif

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    int
mch_dirname(char_u *buf, int len)
{
    if (!getcwd((char *)buf, len))
    {
	STRCPY(buf, strerror(errno));
	return FAIL;
    }

    /*********************************************
    If we want tranlate from VMS to UNIX format, but
    it is much more useful to use native VMS path
    internally as well.

    STRNCPY(buf, decc$translate_vms((char *)buf), len);
    STRCAT(buf, "/");

    **********************************************/

    return OK;
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
		    if (mch_chdir((char *)buf))
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
	    mch_chdir((char *)olddir);
    }

    /* TODO: check for buffer overflow. */
    STRCAT(buf, vms_fixfilename(fname));
    return retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    int
mch_isFullName(char_u *fname)
{
    if (fname[0] == '/' || fname[0] == '.' || strchr((char *)fname, ':') || strchr((char *)fname,'[') || strchr((char *)fname,']') || strchr((char *)fname,'<') || strchr((char *)fname,'>') )
	return 1;
    else
	return 0;
}


/*
 * get file permissions for 'name'
 */
    long
getperm(char_u *name)
{
	struct stat statb;

	if (mch_stat((char *)name, &statb))
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

    if (*name == NUL)	    /* Some stat()s don't flag "" as an error. */
	return FALSE;
    if (stat((char *)name, &statb))
	return FALSE;
#ifdef _POSIX_SOURCE
	return (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
#else
	return ((statb.st_mode & S_IFMT) == S_IFDIR ? TRUE : FALSE);
#endif
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Return 1 if "name" can be executed, 0 if not.
 * Return -1 if unknown.
 */
    int
mch_can_exe(name)
    char_u	*name;
{
    /* TODO */
    return -1;
}
#endif

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    int
mch_nodetype(name)
    char_u	*name;
{
    /* TODO */
    return NODE_NORMAL;
}

    void
mch_init(void)
{
}

    void
mch_windexit(int r)
{
    exiting = TRUE;

#ifdef FEAT_GUI
    if (!gui.in_use)
#endif
    {
#ifdef FEAT_TITLE
	mch_restore_title(3);   /* restore xterm title and icon name */
#endif
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	if (newline_on_exit || (msg_didout && !swapping_screen()))
	    out_char('\n');
	else
	    msg_clr_eos();  /* clear the rest of the display */

	/* Cursor may have been switched off without calling starttermcap()
	 * when doing "vim -u vimrc" and vimrc contains ":q". */
	if (full_screen)
	    cursor_on();
    }
    vms_flushbuf();
    ml_close_all(TRUE);			/* remove all memfiles */
    may_core_dump();
    settmode(TMODE_COOK);
#ifdef FEAT_GUI
    if (gui.in_use)
	gui_exit(r);
#endif
    exit(r);
}

    static void
may_core_dump(void)
{
    if (deadly_signal != 0)
    {
	signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);  /* Die using the signal we caught */
    }
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
    while (cp1 = (char *)mch_getenv((char_u *)cp2))
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

#ifdef FEAT_MOUSE

/*
 * set mouse clicks on or off (only works for xterms)
 */

    void
mch_setmouse(int on)
{
    static int	ison = FALSE;
    int		xterm_mouse_vers;

    if (on == ison)		/* return quickly if nothing to do */
	return;

    xterm_mouse_vers = use_xterm_mouse();
    if (xterm_mouse_vers > 0)
    {
	if (on) /* enable mouse events, use mouse tracking if available */
	    out_str_nf((char_u *)
		    (xterm_mouse_vers > 1 ? "\033[?1002h" : "\033[?1000h"));
	else    /* disable mouse events, could probably always send the same */
	    out_str_nf((char_u *)
		    (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	ison = on;
    }
# ifdef FEAT_MOUSE_DEC
    else if (ttym_flags == TTYM_DEC)
    {
	if (on) /* enable mouse events */
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	else    /* disable mouse events */
	    out_str_nf((char_u *)"\033['z");
	ison = on;
    }
# endif
}

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    void
check_mouse_termcode()
{
# ifdef FEAT_MOUSE_XTERM
    if (use_xterm_mouse())
    {
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033[M");
	if (*p_mouse != NUL)
	{
	    /* force mouse off and maybe on to send possibly new mouse
	     * activation sequence to the xterm, with(out) drag tracing. */
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    else
	del_mouse_termcode(KS_MOUSE);
# endif
# ifdef FEAT_MOUSE_GPM
    if (!use_xterm_mouse())
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MG");
# endif
# ifdef FEAT_MOUSE_NET
    /* can be added always, there is no conflict */
    set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
# endif
# ifdef FEAT_MOUSE_DEC
    /* conflicts with xterm mouse: "\033[" and "\033[M" */
    if (!use_xterm_mouse())
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)"\033[");
    else
	del_mouse_termcode(KS_DEC_MOUSE);
# endif
}

/*
 * Return non-zero when using an xterm mouse, according to 'ttymouse'.
 * Return 1 for "xterm".
 * Return 2 for "xterm2".
 */
    int
use_xterm_mouse()
{
    if (ttym_flags == TTYM_XTERM2)
	return 2;
    if (ttym_flags == TTYM_XTERM)
	return 1;
    return 0;
}

#endif /* USE_MOUSE */

/*
 * set screen mode, always fails.
 */
    int
mch_screenmode(char_u *arg)
{
    EMSG(_("Screen mode setting not supported"));
    return FAIL;
}

/*
 * Get the current window size in Rows and Columns.
 */
    int
mch_get_shellsize(void)
{
    TT_MODE	tmode;

    tmode = get_tty();			/* get size from VMS	*/
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    return OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    void
mch_set_shellsize(void)
{
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
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    /* Nothing to do. */
}

/*
 *	options: SHELL_*, see vim.h
 */

    int
mch_call_shell(char_u *cmd, int options)
{
    int		x;
    char	*ifn = NULL;
    char	*ofn = NULL;

    vms_flushbuf();
    if (options & SHELL_COOKED)
	settmode(TMODE_COOK);			/* set to cooked mode */
    if (cmd)
    {
	if (ofn = strchr((char *)cmd, '>'))
	    *ofn++ = '\0';
	if (ifn = strchr((char *)cmd, '<'))
	{
	   char *p;
	    *ifn++ = '\0';
	    /* chop off any trailing spaces */
	    p = strchr(ifn,' ');
	    if (p)
		*p = '\0';
	}
	if (ofn)
	    x = vms_sys((char *)cmd, ofn, ifn);
	else
	    x = system((char *)cmd);
    }
    else
	x = system((char *)p_sh);
    if (x == 127)
	OUT_STR(_("\nCannot execute shell sh\n"));
    else if (x && !(options & SHELL_SILENT))
    {
	OUT_STR(_("\nshell returned "));
	msg_outnum((long)x);
	msg_putchar('\n');
    }
    settmode(TMODE_RAW);			/* set to raw mode */
#ifdef FEAT_TITLE
    resettitle();
#endif
    return x;
}

#ifdef TESTING_PTY
# undef FEAT_GUI
#endif

/*
 * The input characters are buffered to be able to check for a CTRL-C.
 * This should be done with signals, but I don't know how to do that in
 * a portable way for a tty in RAW mode.
 */

    void
mch_breakcheck(void)
{
#ifdef FEAT_GUI
    if (gui.starting)  /* Transition state, can't check for ^C yet */
	return;
    if (gui.in_use)
    {
	gui_mch_update();
	return;
    }
#endif /* FEAT_GUI */
    /*
     * check for CTRL-C typed by reading all available characters
     */
    if (RealWaitForChar(0, 0L))		/* if characters available */
    {
	add_to_input_buf((char_u *)ibuf,1);
	fill_input_buf(FALSE);
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
    if (!vim_is_input_buf_empty())	/* something in inbuf[] */
      return 1;

#ifdef FEAT_MOUSE_DEC
    /* May need to query the mouse position. */
    if (WantQueryMouse)
    {
      WantQueryMouse = 0;
      mch_write((char_u *)"\033[1'|", 5);
    }
#endif

	if (RealWaitForChar(0, msec))
	{
	    add_to_input_buf((char_u *)ibuf, 1);
	    return 1;
	}
	else
    {
	    return 0;
    }
}

/*
 *	RealWaitForChar		Wait "msec" msec until a character is
 *				available from file descriptor "fd".
 *				Time == -1 will block forever. When a
 *				GUI is being used, this will not be
 *				used for input -- webb
 */

    int
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
    return(res == SS$_NORMAL ? 1 : 0);
}


#define EXPL_ALLOC_INC 16
int vms_match_num = 0;
int vms_match_alloced = 0;
int vms_match_free = 0;
char_u **vms_fmatch = NULL;

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further mathces
 *
 */
int
vms_wproc( char *name, int type )
{
    char xname[MAXPATHL];
    int i;

    if (vms_match_num == 0) {
	/* first time through, setup some things */
	if (NULL == vms_fmatch) {
	    vms_fmatch = (char_u **)alloc(EXPL_ALLOC_INC * sizeof(char *));
	    if (!vms_fmatch)
		return 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	else {
	    /* re-use existing space */
	    vms_match_free = vms_match_alloced;
	}
    }
    /* remove version from filename (if it exists) */
    strcpy(xname,name);
    {
	char *cp = strchr(xname,';');

	if (cp)
	    *cp = '\0';
	/* also may have the form: filename.ext.ver */
	cp = strchr(xname,'.');
	if (cp) {
	   ++cp;
	   cp = strchr(cp,'.');
	   if (cp)
		*cp = '\0';
	}
	cp = strchr(xname,']');
	if (cp)
	    return 1; /* do not add directories at all*/
    }
    /* translate name */
    strcpy(xname,decc$translate_vms(xname));

    /* if name already exists, don't add it */
    for (i = 0; i<vms_match_num; i++) {
	if (0 == STRCMP((char_u *)xname,vms_fmatch[i]))
	    return 1;
    }
    if (--vms_match_free == 0) {
	/* add more spacE to store matches */
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = (char_u **)realloc(vms_fmatch,
		sizeof(char **) * vms_match_alloced);
	if (!vms_fmatch)
	    return 0;
	vms_match_free = EXPL_ALLOC_INC;
    }
#ifdef APPEND_DIR_SLASH
    if (type == DECC$K_DIRECTORY) {
	STRCAT(xname,"/");
	vms_fmatch[vms_match_num] = vim_strsave((char_u *)xname);
    }
    else {
	vms_fmatch[vms_match_num] =
	    vim_strsave((char_u *)xname);
    }
#else
    vms_fmatch[vms_match_num] =
	vim_strsave((char_u *)xname);
#endif
    ++vms_match_num;
    return 1;
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
 */

    int
mch_expand_wildcards(int num_pat, char_u **pat, int *num_file, char_u ***file, int flags)
{
    int		i, j = 0, cnt = 0;
    char	*cp;
    char_u	buf[MAXPATHL];
    int		dir;
    int files_alloced, files_free;

    *num_file = 0;			/* default: no files found	*/
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = (char_u **) alloc(sizeof(char_u **) * files_alloced);
    if (*file == NULL)
    {
	*num_file = 0;
	return FAIL;
    }
    for (i = 0; i < num_pat; i++)
    {
	/* expand environment var or home dir */
	if (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	else
	    STRCPY(buf,pat[i]);

	vms_match_num = 0; /* reset collection counter */
	cnt = decc$to_vms((char *)buf, vms_wproc, 1, 0);
	if (cnt > 0)
	    cnt = vms_match_num;

	if (cnt < 1)
	    continue;

	for (i = 0; i < cnt; i++)
	{
	    /* files should exist if expanding interactively */
	    if (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		continue;
	    /* do not include directories */
	    dir = (mch_isdir(vms_fmatch[i]));
	    if (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		continue;
	    /* allocate memory for pointers */
	    if (--files_free < 1)
	    {
		files_alloced += EXPL_ALLOC_INC;
		*file = (char_u **)realloc(*file,
		    sizeof(char_u **) * files_alloced);
		if (*file == NULL)
		{
		    *file = (char_u **)"";
		    *num_file = 0;
		    return(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*num_file++] = vms_fmatch[i];
	}
    }
    return OK;
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
	else if (vim_strchr((char_u *)
#ifdef VIM_BACKTICK
	    "*%$~`"
#else
	    "*%$~"
#endif
	    , *p) != NULL)
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


    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    static char		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = NULL;
    unsigned long	attrib;
    int			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    char		*sbuf = NULL;

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
    else if ((sbuf = getenv((char *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = (char_u *)malloc((size_t)lengte);
	if (cp)
	    strcpy((char *)cp, sbuf);
	return cp;
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


    static int
vms_sys(char *cmd, char *out, char *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status, substatus;

    if (cmd)
	vul_desc(&cdsc, cmd);
    if (out)
	vul_desc(&odsc, out);
    if (inp)
	vul_desc(&idsc, inp);

    status = lib$spawn(
	    cmd ? &cdsc : NULL,		/* command string */
	    inp ? &idsc : NULL,		/* input file */
	    out ? &odsc : NULL,		/* output file */
	    0, 0, 0, &substatus, 0, 0, 0, 0, 0, 0);
    if (status != SS$_NORMAL)
	substatus = status;
    if ((substatus&STS$M_SUCCESS) == 0)     /* Command failed.	    */
	return(FALSE);
    return(TRUE);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    int
mch_setenv(char *var, char *value, int x)
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

    if (mch_stat((char *)name, &statb))
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
mch_expandpath(garray_t *gap, char_u *path, int flags)
{
    int		i,cnt = 0;
    char	*cp;

    vms_match_num = 0;
    cnt = decc$to_vms((char *)path, vms_wproc, 1, 0);
    if (cnt > 0)
	cnt = vms_match_num;
    for (i = 0; i < cnt; i++)
    {
	if (mch_getperm(vms_fmatch[i]) >= 0) /* add existing file */
	    addfile(gap, vms_fmatch[i], flags);
    }
    return cnt;
}

static char *Fspec_Rms;	/* rms file spec, passed implicitly between routines */

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) while (Str[1] == '/') ++Str

/* attempt to translate a mixed unix-vms file specification to pure vms */

static void
vms_unix_mixed_filespec(char *in, char *out)
{
    char *lastcolon;
    char *end_of_dir;
    char ch;
    int len;

    /* copy vms filename portion up to last colon
     * (node and/or disk)
     */
    lastcolon = strrchr(in, ':');   /* find last colon */
    if (lastcolon != NULL) {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = NULL;	/* default: no directory */

    /* start of directory portion */
    ch = *in;
    if ((ch == '[') || (ch == '/') || (ch == '<') ) {	/* start of directory(s) ? */
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    } else if (EQN(in, "../", 3)) { /* Unix parent directory? */
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    } else {		    /* not a special character */
	while (EQN(in, "./", 2)) {	/* Ignore Unix "current dir" */
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    if (strchr(in, '/') == NULL) {  /* any more Unix directories ? */
	strcpy(out, in);	/* No - get rest of the spec */
	return;
    } else {
	*out++ = '[';	    /* Yes, denote a Vms subdirectory */
	ch = '.';
	--in;
	}
    }

    /* if we get here, there is a directory part of the filename */

    /* initialize output file spec */
    *out++ = ch;
    ++in;

    while (*in != '\0') {
	ch = *in;
	if ((ch == ']') || (ch == '/') || (ch == '>') ) {	/* end of (sub)directory ? */
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	else if (EQN(in, "../", 3)) {	/* Unix parent directory? */
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	else {
	    while (EQN(in, "./", 2)) {  /* Ignore Unix "current dir" */
	    end_of_dir = out;
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    ch = *in;
	    }
	}

    /* Place next character into output file spec */
	*out++ = ch;
	++in;
    }

    *out = '\0';    /* Terminate output file spec */

    if (end_of_dir != NULL) /* Terminate directory portion */
	*end_of_dir = ']';
}

/* for decc$to_vms in vms_fixfilename */
static
vms_fspec_proc(char *fil, int val)
{
    strcpy(Fspec_Rms,fil);
    return(1);
}

/*
 * change '/' to '.' (or ']' for the last one)
 */
    void *
vms_fixfilename(void *instring)
{
    static char		*buf = NULL;
    static size_t	buflen = 0;
    size_t		len;

    /* get a big-enough buffer */
    len = strlen(instring) + 1;
    if (len > buflen)
    {
	buflen = len + 128;
	if (buf)
	    buf = (char *)realloc(buf, buflen);
	else
	    buf = (char *)calloc(buflen, sizeof(char));
    }

    /* if this already is a vms file specification, copy it
     * else if VMS understands how to translate the file spec, let it do so
     * else translate mixed unix-vms file specs to pure vms
     */

    Fspec_Rms = buf;				/* for decc$to_vms */
    if (strchr(instring,'/') == NULL)
	strcpy(buf, instring);			/* already a vms file spec */
    else if (strchr(instring,'"') == NULL){	/* regular file */
	if (decc$to_vms(instring, vms_fspec_proc, 0, 0) <= 0)
	    vms_unix_mixed_filespec(instring, buf);
    }
    else
	vms_unix_mixed_filespec(instring, buf); /* we have a passwd in the path */

    return buf;
}
/*
 * Remove version number from file name
 * we need it in some special cases as:
 * creating swap file name and writing new file
 */

void *
vms_remove_version(void * fname)
{
    char_u	*cp;

    if ((cp = vim_strchr( fname, ';')) != NULL) /* remove version */
	*cp = '\0';
    vms_fixfilename(fname);
    return fname;
}

