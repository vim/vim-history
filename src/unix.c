/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * unix.c -- BSD and SYSV code
 *
 * A lot of this file was written by Juergen Weigert.
 */

#include "vim.h"
#include "globals.h"
#include "param.h"
#include "proto.h"

#include <fcntl.h>
#if !defined(pyr) && !defined(NOT_BOTH_TIME)
# include <time.h>			/* on some systems time.h should not be
							   included together with sys/time.h */
#endif
#include <sys/ioctl.h>
#ifndef M_XENIX
# include <sys/types.h>
#endif
#include <signal.h>

#ifndef USE_SYSTEM		/* use fork/exec to start the shell */
# include <sys/wait.h>
# if !defined(SCO) && !defined(SOLARIS) && !defined(hpux) && !defined(__NetBSD__) && !defined(__FreeBSD__) && !defined(_SEQUENT_) && !defined(UNISYS)	/* SCO returns pid_t */
extern int fork();
# endif
# if !defined(linux) && !defined(SOLARIS) && !defined(USL) && !defined(sun) && !(defined(hpux) && defined(__STDC__)) && !defined(__NetBSD__) && !defined(__FreeBSD__) && !defined(USL) && !defined(UNISYS)
extern int execvp __ARGS((const char *, const char **));
# endif
#endif

#if defined(SYSV_UNIX) || defined(USL)
# if defined(__sgi) || defined(UTS2) || defined(UTS4) || defined(MIPS) || defined (MIPSEB) || defined(__osf__)
#  include <sys/time.h>
# endif
# if defined(M_XENIX) || defined(SCO)
#  include <stropts.h>
# endif
# if defined(M_XENIX) || defined(SCO) || defined(UNICOS)
#  include <sys/select.h>
#  define bzero(a, b)	memset((a), 0, (b))
# endif
# if !defined(M_XENIX) && !defined(UNICOS)
#  include <poll.h>
# endif
# if defined(SCO) || defined(ISC)
#  include <sys/stream.h>
#  include <sys/ptem.h>
# endif
# if defined(M_UNIX) && !defined(SCO)
#  include <sys/time.h>
# endif       /* M_UNIX */
# ifdef ISC
#  include <termios.h>
# endif
# include <termio.h>
#else	/* SYSV_UNIX */
# include <sys/time.h>
# if defined(hpux) || defined(linux)
#  include <termio.h>
#  if defined(hpux) && !defined(SIGWINCH)	/* hpux 9.01 has it */
#   define SIGWINCH SIGWINDOW
#  endif
# else
#  include <sgtty.h>
# endif	/* hpux */
#endif	/* !SYSV_UNIX */

#if (defined(pyr) || defined(NO_FD_ZERO)) && defined(SYSV_UNIX) && defined(FD_ZERO)
# undef FD_ZERO
#endif

#if defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#ifdef USE_X11

# include <X11/Xlib.h>
# include <X11/Xutil.h>

Window		x11_window = 0;
Display		*x11_display = NULL;

static int	get_x11_windis __ARGS((void));
#ifdef BUGGY
static void set_x11_title __ARGS((char_u *));
static void set_x11_icon __ARGS((char_u *));
#endif
#endif

static void get_x11_title __ARGS((void));
static void get_x11_icon __ARGS((void));

static int	Read __ARGS((char_u *, long));
static int	WaitForChar __ARGS((int));
static int	RealWaitForChar __ARGS((int));
static void fill_inbuf __ARGS((void));
#ifdef USL
static void sig_winch __ARGS((int));
#else
# if defined(SIGWINCH) && !defined(linux) && !defined(__alpha) && !defined(mips) && !defined(_SEQUENT_) && !defined(SCO) && !defined(SOLARIS) && !defined(ISC)
static void sig_winch __ARGS((int, int, struct sigcontext *));
# endif
#endif

static int do_resize = FALSE;
static char_u *oldtitle = NULL;
static char_u *oldicon = NULL;
static char_u *extra_shell_arg = NULL;
static int show_shell_mess = TRUE;

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

	void
mch_write(s, len)
	char_u	*s;
	int		len;
{
	write(1, (char *)s, len);
}

/*
 * GetChars(): low level input funcion.
 * Get a characters from the keyboard.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
	int
GetChars(buf, maxlen, wtime)
	char_u	*buf;
	int		maxlen;
	int		wtime;			/* don't use "time", MIPS cannot handle it */
{
	int		len;

	if (wtime >= 0)
	{
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
		if (WaitForChar((int)p_ut) == 0)
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
		WaitForChar(-1);
		if (do_resize)		/* interrupted by SIGWINCHsignal */
			continue;
		len = Read(buf, (long)maxlen);
		if (len > 0)
			return len;
	}
}

/*
 * return non-zero if a character is available
 */
	int
mch_char_avail()
{
	return WaitForChar(0);
}

	long
mch_avail_mem(special)
	int special;
{
	return 0x7fffffff;		/* virual memory eh */
}

#ifndef FD_ZERO
	void
vim_delay()
{
	poll(0, 0, 500);
}
#else
# if (defined(__STDC__) && !defined(hpux)) || defined(ultrix)
extern int select __ARGS((int, fd_set *, fd_set *, fd_set *, struct timeval *));
# endif

	void
vim_delay()
{
	struct timeval tv;

	tv.tv_sec = 25 / 50;
	tv.tv_usec = (25 % 50) * (1000000/50);
	select(0, 0, 0, 0, &tv);
}
#endif

	static void
#if defined(__alpha) || (defined(mips) && !defined(USL))
sig_winch()
#else
# if defined(_SEQUENT_) || defined(SCO) || defined(ISC)
sig_winch(sig, code)
	int		sig;
	int		code;
# else
#  if defined(USL)
sig_winch(sig)
	int		sig;
#  else
sig_winch(sig, code, scp)
	int		sig;
	int		code;
	struct sigcontext *scp;
#  endif
# endif
#endif
{
#if defined(SIGWINCH)
		/* this is not required on all systems, but it doesn't hurt anybody */
	signal(SIGWINCH, (void (*)())sig_winch);
#endif
	do_resize = TRUE;
}

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
	void
mch_suspend()
{
#ifdef SIGTSTP
	settmode(0);
	kill(0, SIGTSTP);		/* send ourselves a STOP signal */
	settmode(1);
#else
	OUTSTR("new shell started\n");
	(void)call_shell(NULL, 0, TRUE);
#endif
}

	void
mch_windinit()
{
	Columns = 80;
	Rows = 24;

	flushbuf();

	(void)mch_get_winsize();
#if defined(SIGWINCH)
	signal(SIGWINCH, (void (*)())sig_winch);
#endif
}

/*
 * Check_win checks whether we have an interactive window.
 * If not, a new window is opened with the newcli command.
 * If we would open a window ourselves, the :sh and :! commands would not
 * work properly (Why? probably because we are then running in a background CLI).
 * This also is the best way to assure proper working in a next Workbench release.
 *
 * For the -e option (quickfix mode) we open our own window and disable :sh.
 * Otherwise we would never know when editing is finished.
 */
#define BUF2SIZE 320		/* lenght of buffer for argument with complete path */

	void
check_win(argc, argv)
	int		argc;
	char	**argv;
{
	if (!isatty(0) || !isatty(1))
    {
		fprintf(stderr, "VIM: no controlling terminal\n");
		exit(2);
    }
}

/*
 * fname_case(): Set the case of the filename, if it already exists.
 *				 This will cause the filename to remain exactly the same.
 */
	void
fname_case(name)
	char_u *name;
{
}

#ifdef USE_X11
/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
	static int
get_x11_windis()
{
	char		*winid;

	/*
	 * If WINDOWID not set, should try another method to find out
	 * what the current window number is. The only code I know for
	 * this is very complicated.
	 * We assume that zero is invalid for WINDOWID.
	 */
	if (x11_window == 0 && (winid = getenv("WINDOWID")) != NULL) 
		x11_window = (Window)atol(winid);
	if (x11_window != 0 && x11_display == NULL)
		x11_display = XOpenDisplay(NULL);
	if (x11_window == 0 || x11_display == NULL)
		return FAIL;
	return OK;
}

/*
 * Determine original x11 Window Title
 */
	static void
get_x11_title()
{
	XTextProperty	text_prop;

	if (get_x11_windis() == OK)
	{
			/* Get window name if any */
		if (XGetWMName(x11_display, x11_window, &text_prop))
		{
			if (text_prop.value != NULL)
				oldtitle = strsave((char_u *)text_prop.value);
			XFree((void *)text_prop.value);
		}
	}
	if (oldtitle == NULL)		/* could not get old title */
		oldtitle = (char_u *)"Thanks for flying Vim";
}

/*
 * Determine original x11 Window icon
 */

	static void
get_x11_icon()
{
	XTextProperty text_prop;

	if (get_x11_windis() == OK)
	{
			/* Get icon name if any */
		if (XGetWMIconName(x11_display, x11_window, &text_prop))
		{
			if (text_prop.value != NULL)
				oldicon = strsave((char_u *)text_prop.value);
			XFree((void *)text_prop.value);
		}
	}

		/* could not get old icon, use terminal name */
	if (oldicon == NULL)
	{
		if (STRNCMP(term_strings.t_name, "builtin_", 8) == 0)
			oldicon = term_strings.t_name + 8;
		else
			oldicon = term_strings.t_name;
	}
}

#if BUGGY

This is not included, because it probably does not work at all.
On my FreeBSD/Xfree86 in a shelltool I get all kinds of error messages and
Vim is stopped in an uncontrolled way.

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
	static void
set_x11_title(title)
	char_u		*title;
{
	XTextProperty text_prop;

		/* Get icon name if any */
	text_prop.value = title;
	text_prop.nitems = STRLEN(title);
	XSetWMName(x11_display, x11_window, &text_prop);
	if (XGetWMName(x11_display, x11_window, &text_prop)) 	/* required? */
		XFree((void *)text_prop.value);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
	static void
set_x11_icon(icon)
	char_u		*icon;
{
	XTextProperty text_prop;

		/* Get icon name if any */
	text_prop.value = icon;
	text_prop.nitems = STRLEN(icon);
	XSetWMIconName(x11_display, x11_window, &text_prop);
	if (XGetWMIconName(x11_display, x11_window, &text_prop)) /* required? */
		XFree((void *)text_prop.value);
}
#endif

#else	/* USE_X11 */

	static void
get_x11_title()
{
	oldtitle = (char_u *)"Thanks for flying Vim";
}

	static void
get_x11_icon()
{
	if (STRNCMP(term_strings.t_name, "builtin_", 8) == 0)
		oldicon = term_strings.t_name + 8;
	else
		oldicon = term_strings.t_name;
}

#endif	/* USE_X11 */


/*
 * set the window title and icon
 * Currently only works for x11.
 */
	void
mch_settitle(title, icon)
	char_u *title;
	char_u *icon;
{
	int			type = 0;

	if (term_strings.t_name == NULL)		/* no terminal name (yet) */
		return;

/*
 * if the window ID and the display is known, we may use X11 calls
 */
#ifdef USE_X11
	if (get_x11_windis() == OK)
		type = 1;
#endif

	/*
	 * note: if terminal is xterm, title is set with escape sequence rather
	 * 		 than x11 calls, because the x11 calls don't always work
	 */
	if (	STRCMP(term_strings.t_name, "xterm") == 0 ||
			STRCMP(term_strings.t_name, "builtin_xterm") == 0)
		type = 2;

		/*
		 * Note: getting the old window title for iris-ansi will only
		 * currently work if you set WINDOWID by hand, it is not
		 * done automatically like an xterm.
		 */
	if (STRCMP(term_strings.t_name, "iris-ansi") == 0 ||
			 STRCMP(term_strings.t_name, "iris-ansi-net") == 0)
		type = 3;

	if (type)
	{
		if (title != NULL)
		{
			if (oldtitle == NULL)				/* first call, save title */
				get_x11_title();

			switch(type)
			{
#ifdef USE_X11
#ifdef BUGGY
			case 1:	set_x11_title(title);				/* x11 */
					break;
#endif
#endif
			case 2: outstrn((char_u *)"\033]2;");		/* xterm */
					outstrn(title);
					outchar(Ctrl('G'));
					flushbuf();
					break;

			case 3: outstrn((char_u *)"\033P1.y");		/* iris-ansi */
					outstrn(title);
					outstrn((char_u *)"\234");
					flushbuf();
					break;
			}
		}

		if (icon != NULL)
		{
			if (oldicon == NULL)				/* first call, save icon */
				get_x11_icon();

			switch(type)
			{
#ifdef USE_X11
#ifdef BUGGY
			case 1:	set_x11_icon(icon);					/* x11 */
					break;
#endif
#endif
			case 2: outstrn((char_u *)"\033]1;");		/* xterm */
					outstrn(icon);
					outchar(Ctrl('G'));
					flushbuf();
					break;

			case 3: outstrn((char_u *)"\033P3.y");		/* iris-ansi */
					outstrn(icon);
					outstrn((char_u *)"\234");
					flushbuf();
					break;
			}
		}
	}
}

/*
 * Restore the window/icon title.
 * which is one of:
 *	1  Just restore title
 *  2  Just restore icon
 *	3  Restore title and icon
 */
	void
mch_restore_title(which)
	int which;
{
	mch_settitle((which & 1) ? oldtitle : NULL, (which & 2) ? oldicon : NULL);
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
	int 
vim_dirname(buf, len)
	char_u *buf;
	int len;
{
#if defined(SYSV_UNIX) || defined(USL) || defined(hpux) || defined(linux)
	extern int		errno;
	extern char		*sys_errlist[];

	if (getcwd((char *)buf, len) == NULL)
	{
	    STRCPY(buf, sys_errlist[errno]);
	    return FAIL;
	}
    return OK;
#else
	return (getwd((char *)buf) != NULL ? OK : FAIL);
#endif
}

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK for success
 */
	int 
FullName(fname, buf, len)
	char_u *fname, *buf;
	int len;
{
	int		l;
	char_u	olddir[MAXPATHL];
	char_u	*p;
	int		c;
	int		retval = OK;

	if (fname == NULL)	/* always fail */
	{
		*buf = NUL;
		return FAIL;
	}

	*buf = 0;
	if (!isFullName(fname))			/* if not an absolute path */
	{
		/*
		 * If the file name has a path, change to that directory for a moment,
		 * and then do the getwd() (and get back to where we were).
		 * This will get the correct path name with "../" things.
		 */
		if ((p = STRRCHR(fname, '/')) != NULL)
		{
#if defined(SYSV_UNIX) || defined(USL) || defined(hpux) || defined(linux)
			if (getcwd((char *)olddir, MAXPATHL) == NULL)
#else
			if (getwd((char *)olddir) == NULL)
#endif
			{
				p = NULL;		/* can't get current dir: don't chdir */
				retval = FAIL;
			}
			else
			{
				c = *p;
				*p = NUL;
				if (chdir((char *)fname))
					retval = FAIL;
				else
					fname = p + 1;
				*p = c;
			}
		}
#if defined(SYSV_UNIX) || defined(USL) || defined(hpux) || defined(linux)
		if (getcwd((char *)buf, len) == NULL)
#else
		if (getwd((char *)buf) == NULL)
#endif
		{
			retval = FAIL;
			*buf = NUL;
		}
		l = STRLEN(buf);
		if (l && buf[l - 1] != '/')
			STRCAT(buf, "/");
		if (p)
			chdir((char *)olddir);
	}
	STRCAT(buf, fname);
	return retval;
}

/*
 * return TRUE is fname is an absolute path name
 */
	int
isFullName(fname)
	char_u		*fname;
{
	return (*fname == '/');
}

/*
 * get file permissions for 'name'
 */
	long 
getperm(name)
	char_u *name;
{
	struct stat statb;

	if (stat((char *)name, &statb))
		return -1;
	return statb.st_mode;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
	int
setperm(name, perm)
	char_u *name;
	int perm;
{
#ifdef SCO
	return (chmod((char *)name, (mode_t)perm) == 0 ? OK : FAIL);
#else
	return (chmod((char *)name, perm) == 0 ? OK : FAIL);
#endif
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory
 * return -1 for error
 */
	int 
isdir(name)
	char_u *name;
{
	struct stat statb;

	if (stat((char *)name, &statb))
		return -1;
#ifdef _POSIX_SOURCE
	return (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
#else
	return ((statb.st_mode & S_IFMT) == S_IFDIR ? TRUE : FALSE);
#endif
}

	void
mch_windexit(r)
	int r;
{
	settmode(0);
	exiting = TRUE;
	mch_settitle(oldtitle, oldicon);	/* restore xterm title */
	stoptermcap();
	flushbuf();
	ml_close_all(); 				/* remove all memfiles */
	exit(r);
}

	void
mch_settmode(raw)
	int				raw;
{
#if defined(ECHOE) && defined(ICANON) && !defined(__NeXT__)
	/* for "new" tty systems */
# ifdef CONVEX
	static struct termios told;
		   struct termios tnew;
# else
	static struct termio told;
		   struct termio tnew;
# endif
#ifdef TIOCLGET
	static unsigned long tty_local;
#endif

	if (raw)
	{
#ifdef TIOCLGET
		ioctl(0, TIOCLGET, &tty_local);
#endif
		ioctl(0, TCGETA, &told);
		tnew = told;
		/*
		 * ICRNL enables typing ^V^M
		 */
		tnew.c_iflag &= ~ICRNL;
		tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#ifdef IEXTEN
					| IEXTEN		/* IEXTEN enables typing ^V on SOLARIS */
#endif
						);
		tnew.c_cc[VMIN] = 1;			/* return after 1 char */
		tnew.c_cc[VTIME] = 0;			/* don't wait */
		ioctl(0, TCSETA, &tnew);
	}
	else
	{
		ioctl(0, TCSETA, &told);
#ifdef TIOCLGET
		ioctl(0, TIOCLSET, &tty_local);
#endif
	}
#else
# ifndef TIOCSETN
#  define TIOCSETN TIOCSETP		/* for hpux 9.0 */
# endif
	/* for "old" tty systems */
	static struct sgttyb ttybold;
		   struct sgttyb ttybnew;

	if (raw)
	{
		ioctl(0, TIOCGETP, &ttybold);
		ttybnew = ttybold;
		ttybnew.sg_flags &= ~(CRMOD | ECHO);
		ttybnew.sg_flags |= RAW;
		ioctl(0, TIOCSETN, &ttybnew);
	}
	else
		ioctl(0, TIOCSETN, &ttybold);
#endif
}

/*
 * set screen mode, always fails.
 */
	int
mch_screenmode(arg)
	char_u	 *arg;
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
mch_get_winsize()
{
	int			old_Rows = Rows;
	int			old_Columns = Columns;
	char_u		*p;

	Columns = 0;
	Rows = 0;

/*
 * 1. try using an ioctl. It is the most accurate method.
 */
# ifdef TIOCGSIZE
	{
		struct ttysize	ts;

	    if (ioctl(0, TIOCGSIZE, &ts) == 0)
	    {
			Columns = ts.ts_cols;
			Rows = ts.ts_lines;
	    }
	}
# else /* TIOCGSIZE */
#  ifdef TIOCGWINSZ
	{
		struct winsize	ws;

	    if (ioctl(0, TIOCGWINSZ, &ws) == 0)
	    {
			Columns = ws.ws_col;
			Rows = ws.ws_row;
	    }
	}
#  endif /* TIOCGWINSZ */
# endif /* TIOCGSIZE */

/*
 * 2. get size from environment
 */
	if (Columns == 0 || Rows == 0)
	{
	    if ((p = (char_u *)getenv("LINES")))
			Rows = atoi((char *)p);
	    if ((p = (char_u *)getenv("COLUMNS")))
			Columns = atoi((char *)p);
	}

#ifdef TERMCAP
/*
 * 3. try reading the termcap
 */
	if (Columns == 0 || Rows == 0)
	{
		extern void getlinecol();

		getlinecol();	/* get "co" and "li" entries from termcap */
	}
#endif

/*
 * 4. If everything fails, use the old values
 */
	if (Columns <= 0 || Rows <= 0)
	{
		Columns = old_Columns;
		Rows = old_Rows;
		return FAIL;
	}

	check_winsize();

/* if size changed: screenalloc will allocate new screen buffers */
	return OK;
}

	void
mch_set_winsize()
{
	/* should try to set the window size to Rows and Columns */
}

	int 
call_shell(cmd, dummy, cooked)
	char_u	*cmd;
	int		dummy;
	int		cooked;
{
#ifdef USE_SYSTEM		/* use system() to start the shell: simple but slow */

	int		x;
	char_u	newcmd[1024];

	flushbuf();

	if (cooked)
		settmode(0); 				/* set to cooked mode */

	if (cmd == NULL)
		x = system(p_sh);
	else
	{
		sprintf(newcmd, "%s %s -c \"%s\"", p_sh,
						extra_shell_arg == NULL ? "" : extra_shell_arg, cmd);
		x = system(newcmd);
	}
	if (x == 127)
	{
		outstrn((char_u *)"\nCannot execute shell sh\n");
	}
#ifdef WEBB_COMPLETE
	else if (x && !expand_interactively)
#else
	else if (x)
#endif
	{
		outchar('\n');
		outnum((long)x);
		outstrn((char_u *)" returned\n");
	}

	if (cooked)
		settmode(1); 						/* set to raw mode */
	resettitle();
	return (x ? FAIL : OK);

#else /* USE_SYSTEM */		/* first attempt at not using system() */

	char_u	newcmd[1024];
	int		pid;
	int		status = -1;
	char	**argv = NULL;
	int		argc;
	int		i;
	char_u	*p;
	int		inquote;

	flushbuf();
	signal(SIGINT, SIG_IGN);	/* we don't want to be killed here */
	if (cooked)
		settmode(0);			/* set to cooked mode */

	/*
	 * 1: find number of arguments
	 * 2: separate them and built argv[]
	 */
	STRCPY(newcmd, p_sh);
	for (i = 0; i < 2; ++i)	
	{
		p = newcmd;
		inquote = FALSE;
		argc = 0;
		for (;;)
		{
			if (i == 1)
				argv[argc] = (char *)p;
			++argc;
			while (*p && (inquote || (*p != ' ' && *p != TAB)))
			{
				if (*p == '"')
					inquote = !inquote;
				++p;
			}
			if (*p == NUL)
				break;
			if (i == 1)
				*p++ = NUL;
			skipspace(&p);
		}
		if (i == 0)
		{
			argv = (char **)alloc((unsigned)((argc + 4) * sizeof(char *)));
			if (argv == NULL)		/* out of memory */
				goto error;
		}
	}
	if (cmd != NULL)
	{
		if (extra_shell_arg != NULL)
			argv[argc++] = (char *)extra_shell_arg;
		argv[argc++] = "-c";
		argv[argc++] = (char *)cmd;
	}
	argv[argc] = NULL;

	if ((pid = fork()) == -1)		/* maybe we should use vfork() */
	{
		outstrn((char_u *)"\nCannot fork\n");
	}
	else if (pid == 0)		/* child */
	{
		signal(SIGINT, SIG_DFL);
		if (!show_shell_mess)
		{
			fclose(stdout);
			fclose(stderr);
		}
		execvp(argv[0], (char **)argv);
		exit(127);			/* exec failed, return failure code */
	}
	else					/* parent */
	{
		wait(&status);
		status = (status >> 8) & 255;
		if (status)
		{
#ifdef WEBB_COMPLETE
			if (status == 127)
			{
				outstrn((char_u *)"\nCannot execute shell ");
				outstrn(p_sh);
				outchar('\n');
			}
			else if (!expand_interactively)
			{
				outchar('\n');
				outnum((long)status);
				outstrn((char_u *)" returned\n");
			}
#else
			outchar('\n');
			if (status == 127)
			{
				outstrn((char_u *)"Cannot execute shell ");
				outstrn(p_sh);
			}
			else
			{
				outnum((long)status);
				outstrn((char_u *)" returned");
			}
			outchar('\n');
#endif /* WEBB_COMPLETE */
		}
	}
	free(argv);

error:
	if (cooked)
		settmode(1); 						/* set to raw mode */
	resettitle();
	signal(SIGINT, SIG_DFL);
	return (status ? FAIL : OK);

#endif /* USE_SYSTEM */
}

/*
 * The input characters are buffered to be able to check for a CTRL-C.
 * This should be done with signals, but I don't know how to do that in
 * a portable way for a tty in RAW mode.
 */

#define INBUFLEN 250
static char_u		inbuf[INBUFLEN];	/* internal typeahead buffer */
static int		inbufcount = 0;		/* number of chars in inbuf[] */

	static int
Read(buf, maxlen)
	char_u	*buf;
	long	maxlen;
{
	if (inbufcount == 0)		/* if the buffer is empty, fill it */
		fill_inbuf();
	if (maxlen > inbufcount)
		maxlen = inbufcount;
	memmove((char *)buf, (char *)inbuf, maxlen);
	inbufcount -= maxlen;
	if (inbufcount)
		memmove((char *)inbuf, (char *)inbuf + maxlen, inbufcount);
	return (int)maxlen;
}

	void
breakcheck()
{
/*
 * check for CTRL-C typed by reading all available characters
 */
	if (RealWaitForChar(0))		/* if characters available */
		fill_inbuf();
}

	static void
fill_inbuf()
{
	int		len;

	if (inbufcount >= INBUFLEN)		/* buffer full */
		return;
	len = read(0, inbuf + inbufcount, (long)(INBUFLEN - inbufcount));
	if (len <= 0)	/* cannot read input??? */
	{
		fprintf(stderr, "Vim: Error reading input, exiting...\n");
		exit(1);
	}
	while (len-- > 0)
	{
		/*
		 * if a CTRL-C was typed, remove it from the buffer and set got_int
		 */
		if (inbuf[inbufcount] == 3)
		{
			/* remove everything typed before the CTRL-C */
			memmove((char *)inbuf, (char *)inbuf + inbufcount, len + 1);
			inbufcount = 0;
			got_int = TRUE;
		}
		++inbufcount;
	}
}

/* 
 * Wait "ticks" until a character is available from the keyboard or from inbuf[]
 * ticks = -1 will block forever
 */

	static int
WaitForChar(ticks)
	int ticks;
{
	if (inbufcount)		/* something in inbuf[] */
		return 1;
	return RealWaitForChar(ticks);
}

/* 
 * Wait "ticks" until a character is available from the keyboard
 * ticks = -1 will block forever
 */
	static int
RealWaitForChar(ticks)
	int ticks;
{
#ifndef FD_ZERO
	struct pollfd fds;

	fds.fd = 0;
	fds.events = POLLIN;
	return (poll(&fds, 1, ticks));
#else
	struct timeval tv;
	fd_set fdset;

	if (ticks >= 0)
    {
   		tv.tv_sec = ticks / 1000;
		tv.tv_usec = (ticks % 1000) * (1000000/1000);
    }

	FD_ZERO(&fdset);
	FD_SET(0, &fdset);
	return (select(1, &fdset, NULL, NULL, (ticks >= 0) ? &tv : NULL));
#endif
}

#if !defined(__alpha) && !defined(mips) && !defined(SCO) && !defined(remove) && !defined(CONVEX)
	int 
remove(buf)
# if defined(linux) || defined(__STDC__) || defined(__NeXT__) || defined(M_UNIX)
	const
# endif
			char *buf;
{
	return unlink(buf);
}
#endif

/*
 * ExpandWildCard() - this code does wild-card pattern matching using the shell
 *
 * Mool: return 0 for success, 1 for error (you may loose some memory) and
 *       put an error message in *file.
 *
 * num_pat is number of input patterns
 * pat is array of pointers to input patterns
 * num_file is pointer to number of matched file names
 * file is pointer to array of pointers to matched file names
 * On Unix we do not check for files only yet
 * list_notfound is ignored
 */

extern char *mktemp __ARGS((char *));
#ifndef SEEK_SET
# define SEEK_SET 0
#endif
#ifndef SEEK_END
# define SEEK_END 2
#endif

	int
ExpandWildCards(num_pat, pat, num_file, file, files_only, list_notfound)
	int 			num_pat;
	char_u		  **pat;
	int 		   *num_file;
	char_u		 ***file;
	int				files_only;
	int				list_notfound;
{
	char_u	tmpname[TMPNAMELEN];
	char_u	*command;
	int		i;
	int		dir;
	size_t	len;
	FILE	*fd;
	char_u	*buffer;
	char_u	*p;
	int		use_glob = FALSE;

	*num_file = 0;		/* default: no files found */
	*file = (char_u **)"";

	/*
	 * If there are no wildcards, just copy the names to allocated memory.
	 * Saves a lot of time, because we don't have to start a new shell.
	 */
	if (!have_wildcard(num_pat, pat))
	{
		*file = (char_u **)alloc(num_pat * sizeof(char_u *));
		if (*file == NULL)
		{
			*file = (char_u **)"";
			return FAIL;
		}
		for (i = 0; i < num_pat; i++)
			(*file)[i] = strsave(pat[i]);
		*num_file = num_pat;
		return OK;
	}

/*
 * get a name for the temp file
 */
	STRCPY(tmpname, TMPNAME2);
	if (*mktemp((char *)tmpname) == NUL)
	{
		emsg(e_notmp);
	    return FAIL;
	}

/*
 * let the shell expand the patterns and write the result into the temp file
 * If we use csh, glob will work better than echo.
 */
	if ((len = STRLEN(p_sh)) >= 3 && STRCMP(p_sh + len - 3, "csh") == 0)
		use_glob = TRUE;

	len = TMPNAMELEN + 11;
	for (i = 0; i < num_pat; ++i)		/* count the length of the patterns */
		len += STRLEN(pat[i]) + 3;
	command = alloc(len);
	if (command == NULL)
		return FAIL;
	if (use_glob)
		STRCPY(command, "glob >");		/* built the shell command */
	else
		STRCPY(command, "echo >");		/* built the shell command */
	STRCAT(command, tmpname);
	for (i = 0; i < num_pat; ++i)
	{
#ifdef USE_SYSTEM
		STRCAT(command, " \"");				/* need extra quotes because we */
		STRCAT(command, pat[i]);			/*   start the shell twice */
		STRCAT(command, "\"");
#else
		STRCAT(command, " ");
		STRCAT(command, pat[i]);
#endif
	}
#ifdef WEBB_COMPLETE
	if (expand_interactively)
		show_shell_mess = FALSE;
#endif /* WEBB_COMPLETE */
	if (use_glob)							/* Use csh fast option */
		extra_shell_arg = (char_u *)"-f";
	i = call_shell(command, 0, FALSE);		/* execute it */
	extra_shell_arg = NULL;
	show_shell_mess = TRUE;
	free(command);
	if (i == FAIL)							/* call_shell failed */
	{
		remove((char *)tmpname);
#ifdef WEBB_COMPLETE
		/* With interactive completion, the error message is not printed */
		if (!expand_interactively)
#endif /* WEBB_COMPLETE */
		{
			must_redraw = CLEAR;			/* probably messed up screen */
			msg_outchar('\n');				/* clear bottom line quickly */
			cmdline_row = Rows - 1;			/* continue on last line */
		}
		return FAIL;
	}

/*
 * read the names from the file into memory
 */
 	fd = fopen((char *)tmpname, "r");
	if (fd == NULL)
	{
		emsg2(e_notopen, tmpname);
		return FAIL;
	}
	fseek(fd, 0L, SEEK_END);
	len = ftell(fd);				/* get size of temp file */
	fseek(fd, 0L, SEEK_SET);
	buffer = alloc(len + 1);
	if (buffer == NULL)
	{
		remove((char *)tmpname);
		fclose(fd);
		return FAIL;
	}
	i = fread((char *)buffer, 1, len, fd);
	fclose(fd);
	remove((char *)tmpname);
	if (i != len)
	{
		emsg2(e_notread, tmpname);
		free(buffer);
		return FAIL;
	}

	if (use_glob)		/* file names are separated with NUL */
	{
		buffer[len] = NUL;					/* make sure the buffers ends in NUL */
		i = 0;
		for (p = buffer; p < buffer + len; ++p)
			if (*p == NUL)					/* count entry */
				++i;
		if (len)
			++i;							/* count last entry */
	}
	else				/* file names are separated with SPACE */
	{
		buffer[len] = '\n';					/* make sure the buffers ends in NL */
		p = buffer;
		for (i = 0; *p != '\n'; ++i)		/* count number of entries */
		{
			while (*p != ' ' && *p != '\n')	/* skip entry */
				++p;
			skipspace(&p);					/* skip to next entry */
		}
	}
	*num_file = i;
	*file = (char_u **)alloc(sizeof(char_u *) * i);
	if (*file == NULL)
	{
		free(buffer);
		*file = (char_u **)"";
		return FAIL;
	}
	p = buffer;
	for (i = 0; i < *num_file; ++i)
	{
		(*file)[i] = p;
		if (use_glob)
		{
			while (*p && p < buffer + len)		/* skip entry */
				++p;
			++p;								/* skip NUL */
		}
		else
		{
			while (*p != ' ' && *p != '\n')		/* skip entry */
				++p;
			if (*p == '\n')						/* last entry */
				*p = NUL;
			else
			{
				*p++ = NUL;
				skipspace(&p);					/* skip to next entry */
			}
		}
	}
	for (i = 0; i < *num_file; ++i)
	{
		dir = (isdir((*file)[i]) == TRUE);
		if (dir < 0)			/* if file doesn't exist don't add '/' */
			dir = 0;
		p = alloc((unsigned)(STRLEN((*file)[i]) + 1 + dir));
		if (p)
		{
			STRCPY(p, (*file)[i]);
			if (dir)
				STRCAT(p, "/");
		}
		(*file)[i] = p;
	}
	free(buffer);
	return OK;
}

	void
FreeWild(num, file)
	int		num;
	char_u	**file;
{
	if (file == NULL || num == 0)
		return;
	while (num--)
		free(file[num]);
	free(file);
}

	int
has_wildcard(p)
	char_u *p;
{
#ifdef __STDC__
	return strpbrk((char *)p, "*?[{`~$") != NULL;
#else
	for ( ; *p; ++p)
		if (STRCHR("*?[{`~$", *p) != NULL)
			return 1;
	return 0;
#endif
}

	int
have_wildcard(num, file)
	int		num;
	char_u	**file;
{
	register int i;

	for (i = 0; i < num; i++)
		if (has_wildcard(file[i]))
			return 1;
	return 0;
}

#if defined(M_XENIX) || defined(UTS2)
/*
 * Scaled-down version of rename, which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
	int
rename(src, dest)
	char_u *src, *dest;
{
	struct stat		st;

	if (stat(dest, &st) >= 0)	/* fail if destination exists */
		return -1;
	if (link(src, dest) != 0)	/* link file to new name */
		return -1;
	if (unlink(src) == 0)		/* delete link to old name */
		return 0;
	return -1;
}
#endif /* M_XENIX || UTS2 */
