/* vi:ts=4:sw=4
 *
 *
 * VIM - Vi IMproved
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
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
# if !defined(SCO) && !defined(SOLARIS)			/* SCO returns pid_t */
extern int fork();
# endif
# if !defined(linux) && !defined(SOLARIS) && !defined(USL)
extern int execvp __ARGS((const char *, const char **));
# endif
#endif

#ifdef SYSV_UNIX
# if defined(__sgi) || defined(UTS2) || defined(UTS4)
#  include <sys/time.h>
# endif
# if defined(M_XENIX) || defined(SCO) || defined(UNICOS)
#  ifndef UNICOS
#   include <stropts.h>
#  endif
#  include <sys/select.h>
#  define bzero(a, b)	memset((a), 0, (b))
# else
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

#if defined(ESIX) || defined(M_UNIX)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

static int	Read __ARGS((char *, long));
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

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

	void
mch_write(s, len)
	char	*s;
	int		len;
{
	write(1, s, len);
}

/*
 * GetChars(): low level input funcion.
 * Get a characters from the keyboard.
 * If time == 0 do not wait for characters.
 * If time == n wait a short time for characters.
 * If time == -1 wait forever for characters.
 */
	int
GetChars(buf, maxlen, time)
	char	*buf;
	int		maxlen;
	int		time;
{
	int		len;

	if (time >= 0)
	{
		if (time < 20)		/* don't know if this is necessary */
			time = 20;
		if (WaitForChar(time) == 0)		/* no character available */
			return 0;
	}
	else		/* time == -1 */
	{
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk
	 */
		if (WaitForChar((int)p_ut) == 0)
			updatescript(0);
	}

	for (;;)	/* repeat until we got a character */
	{
		/* 
		 * we want to be interrupted by the winch signal
		 */
		WaitForChar(-1);
		if (do_resize)
		{
			debug("do_resize!\n");
			set_winsize(0, 0, FALSE);
			do_resize = FALSE;
			continue;
		}
		len = Read(buf, (long)maxlen);
		if (len > 0)
			return len;
	}
}

#if defined(SYSV_UNIX) && !defined(M_XENIX) && !defined(UNICOS)
	void
vim_delay()
{
	poll(0, 0, 500);
}
#else
extern int select __ARGS((int, fd_set *, fd_set *, fd_set *, struct timeval *));

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
#if defined(__alpha) || defined(mips)
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
#if defined(SIGWINCH) && (defined(SYSV_UNIX) || defined(linux) || defined(hpux) || defined(USL))
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
	outstr("new shell started\n");
	call_shell(NULL, 0, TRUE);
#endif
}

	void
mch_windinit()
{
	Columns = 80;
	Rows = 24;

	flushbuf();

	mch_get_winsize();
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
	int argc;
	char **argv;
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
	char *name;
{
}

	void
settitle(str)
	char *str;
{
}

	void
resettitle()
{
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return non-zero for success.
 */
	int 
dirname(buf, len)
	char *buf;
	int len;
{
#if defined(SYSV_UNIX) || defined(hpux) || defined(linux)
	extern int		errno;
	extern char		*sys_errlist[];

	if (getcwd(buf,len) == NULL)
	{
	    strcpy(buf, sys_errlist[errno]);
	    return 0;
	}
    return 1;
#else
	return (getwd(buf) != NULL);
#endif
}

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 */
	int 
FullName(fname, buf, len)
	char *fname, *buf;
	int len;
{
	int		l;
	char	olddir[MAXPATHL];
	char	*p;
	int		c;
	int		retval = 1;

	if (fname == NULL)	/* always fail */
		return 0;

	*buf = 0;
	if (*fname != '/')
	{
		/*
		 * If the file name has a path, change to that directory for a moment,
		 * and then do the getwd() (and get back to where we were).
		 * This will get the correct path name with "../" things.
		 */
		if ((p = strrchr(fname, '/')) != NULL)
		{
#if defined(SYSV_UNIX) || defined(hpux) || defined(linux)
			if (getcwd(olddir, MAXPATHL) == NULL)
#else
			if (getwd(olddir) == NULL)
#endif
			{
				p = NULL;		/* can't get current dir: don't chdir */
				retval = 0;
			}
			else
			{
				c = *p;
				*p = NUL;
				if (chdir(fname))
					retval = 0;
				else
					fname = p + 1;
				*p = c;
			}
		}
#if defined(SYSV_UNIX) || defined(hpux) || defined(linux)
		if (getcwd(buf, len) == NULL)
#else
		if (getwd(buf) == NULL)
#endif
		{
			retval = 0;
			*buf = NUL;
		}
		l = strlen(buf);
		if (l && buf[l - 1] != '/')
			strcat(buf, "/");
		if (p)
			chdir(olddir);
	}
	strcat(buf, fname);
	return retval;
}

/*
 * get file permissions for 'name'
 */
	long 
getperm(name)
	char *name;
{
	struct stat statb;

	if (stat(name, &statb))
		return -1;
	return statb.st_mode;
}

/*
 * set file permission for 'name' to 'perm'
 */
	int
setperm(name, perm)
	char *name;
	int perm;
{
#ifdef SCO
	return chmod(name, (mode_t)perm);
#else
	return chmod(name, perm);
#endif
}

/*
 * check if "name" is a directory
 */
	int 
isdir(name)
	char *name;
{
	struct stat statb;

	if (stat(name, &statb))
		return -1;
#ifdef _POSIX_SOURCE
	return S_ISDIR(statb.st_mode);
#else
	return (statb.st_mode & S_IFMT) == S_IFDIR;
#endif
}

	void
mch_windexit(r)
	int r;
{
	settmode(0);
	stoptermcap();
	flushbuf();
	stopscript();					/* remove autoscript file */
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
		tnew.c_iflag &= ~(ICRNL | IXON);		/* ICRNL enables typing ^V^M */
												/* IXON enables typing ^S/^Q */
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
	char		*p;

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
	    if ((p = (char *)getenv("LINES")))
			Rows = atoi(p);
	    if ((p = (char *)getenv("COLUMNS")))
			Columns = atoi(p);
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
		return 1;
	}
	debug2("mch_get_winsize: %dx%d\n", (int)Columns, (int)Rows);

	Rows_max = Rows;				/* remember physical max height */

	check_winsize();
	script_winsize();

/* if size changed: screenalloc will allocate new screen buffers */
	return (0);
}

	void
mch_set_winsize()
{
	/* should try to set the window size to Rows and Columns */
}

	int 
call_shell(cmd, dummy, cooked)
	char	*cmd;
	int		dummy;
	int		cooked;
{
#ifdef USE_SYSTEM		/* use system() to start the shell: simple but slow */

	int		x;
	char	newcmd[1024];

	flushbuf();

	if (cooked)
		settmode(0); 				/* set to cooked mode */

	if (cmd == NULL)
		x = system(p_sh);
	else
	{
		sprintf(newcmd, "%s -c \"%s\"", p_sh, cmd);
		x = system(newcmd);
	}
	if (x == 127)
	{
		emsg("Cannot execute shell sh");
		outchar('\n');
	}
	else if (x)
	{
		smsg("%d returned", x);
		outchar('\n');
	}

	if (cooked)
		settmode(1); 						/* set to raw mode */
	return x;

#else /* USE_SYSTEM */		/* first attempt at not using system() */

	char	newcmd[1024];
	int		pid;
	int		status = -1;
	char	**argv = NULL;
	int		argc;
	int		i;
	char	*p;
	int		inquote;

	flushbuf();
	signal(SIGINT, SIG_IGN);	/* we don't want to be killed here */
	if (cooked)
		settmode(0);			/* set to cooked mode */

	/*
	 * 1: find number of arguments
	 * 2: separate them and built argv[]
	 */
	strcpy(newcmd, p_sh);
	for (i = 0; i < 2; ++i)	
	{
		p = newcmd;
		inquote = FALSE;
		argc = 0;
		for (;;)
		{
			if (i == 1)
				argv[argc] = p;
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
			argv = (char **)alloc((unsigned)((argc + 3) * sizeof(char *)));
			if (argv == NULL)		/* out of memory */
				goto error;
		}
	}
	if (cmd != NULL)
	{
		argv[argc++] = "-c";
		argv[argc++] = cmd;
	}
	argv[argc] = NULL;

	if ((pid = fork()) == -1)		/* maybe we should use vfork() */
		emsg("Cannot fork");
	else if (pid == 0)		/* child */
	{
		signal(SIGINT, SIG_DFL);
		execvp(argv[0], argv);
		exit(127);			/* exec failed, return failure code */
	}
	else					/* parent */
	{
		wait(&status);
		status = (status >> 8) & 255;
		if (status)
		{
			if (status == 127)
				emsg2("Cannot execute shell %s", p_sh);
			else
				smsg("%d returned", status);
			outchar('\n');
		}
	}
	free(argv);

error:
	if (cooked)
		settmode(1); 						/* set to raw mode */
	signal(SIGINT, SIG_DFL);
	return status;

#endif /* USE_SYSTEM */
}

/*
 * The input characters are buffered to be able to check for a CTRL-C.
 * This should be done with signals, but I don't know how to do that in
 * a portable way for a tty in RAW mode.
 */

#define INBUFLEN 50
static char		inbuf[INBUFLEN];	/* internal typeahead buffer */
static int		inbufcount = 0;		/* number of chars in inbuf[] */

	static int
Read(buf, maxlen)
	char	*buf;
	long	maxlen;
{
	if (inbufcount == 0)		/* if the buffer is empty, fill it */
		fill_inbuf();
	if (maxlen > inbufcount)
		maxlen = inbufcount;
	memmove(buf, inbuf, maxlen);
	inbufcount -= maxlen;
	if (inbufcount)
		memmove(inbuf, inbuf + maxlen, inbufcount);
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
			memmove(inbuf, inbuf + inbufcount, len + 1);
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
	char		  **pat;
	int 		   *num_file;
	char		 ***file;
	int				files_only;
	int				list_notfound;
{
	char	tmpname[TMPNAMELEN];
	char	*command;
	int		i;
	int		dir;
	size_t	len;
	FILE	*fd;
	char	*buffer;
	char	*p;
	int		use_glob = FALSE;

	*num_file = 0;		/* default: no files found */
	*file = (char **)"";

	/*
	 * If there are no wildcards, just copy the names to allocated memory.
	 * Saves a lot of time, because we don't have to start a new shell.
	 */
	if (!have_wildcard(num_pat, pat))
	{
		*file = (char **)alloc(num_pat * sizeof(char *));
		if (*file == NULL)
		{
			*file = (char **)"";
			return 1;
		}
		for (i = 0; i < num_pat; i++)
			(*file)[i] = strsave(pat[i]);
		*num_file = num_pat;
		return 0;
	}

/*
 * get a name for the temp file
 */
	strcpy(tmpname, TMPNAME2);
	if (*mktemp(tmpname) == NUL)
	{
		emsg(e_notmp);
	    return 1;
	}

/*
 * let the shell expand the patterns and write the result into the temp file
 * If we use csh, glob will work better than echo.
 */
	if ((len = strlen(p_sh)) >= 3 && strcmp(p_sh + len - 3, "csh") == 0)
		use_glob = TRUE;

	len = TMPNAMELEN + 10;
	for (i = 0; i < num_pat; ++i)		/* count the length of the patterns */
		len += strlen(pat[i]) + 3;
	command = (char *)alloc(len);
	if (command == NULL)
		return 1;
	if (use_glob)
		strcpy(command, "glob >");			/* built the shell command */
	else
		strcpy(command, "echo >");			/* built the shell command */
	strcat(command, tmpname);
	for (i = 0; i < num_pat; ++i)
	{
#ifdef USE_SYSTEM
		strcat(command, " \"");				/* need extra quotes because we */
		strcat(command, pat[i]);			/*   start the shell twice */
		strcat(command, "\"");
#else
		strcat(command, " ");
		strcat(command, pat[i]);
#endif
	}
	i = call_shell(command, 0, FALSE);		/* execute it */
	free(command);
	if (i)									/* call_shell failed */
	{
		remove(tmpname);
		sleep(1);			/* give the user a chance to read error messages */
		must_redraw = CLEAR;				/* probably messed up screen */
		return 1;
	}

/*
 * read the names from the file into memory
 */
 	fd = fopen(tmpname, "r");
	if (fd == NULL)
	{
		emsg(e_notopen);
		return 1;
	}
	fseek(fd, 0L, SEEK_END);
	len = ftell(fd);				/* get size of temp file */
	fseek(fd, 0L, SEEK_SET);
	buffer = (char *)alloc(len + 1);
	if (buffer == NULL)
	{
		remove(tmpname);
		fclose(fd);
		return 1;
	}
	i = fread(buffer, 1, len, fd);
	fclose(fd);
	remove(tmpname);
	if (i != len)
	{
		emsg(e_notread);
		free(buffer);
		return 1;
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
	*file = (char **)alloc(sizeof(char *) * i);
	if (*file == NULL)
	{
		free(buffer);
		*file = (char **)"";
		return 1;
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
		dir = (isdir((*file)[i]) > 0);
		if (dir < 0)			/* if file doesn't exist don't add '/' */
			dir = 0;
		p = alloc((unsigned)(strlen((*file)[i]) + 1 + dir));
		if (p)
		{
			strcpy(p, (*file)[i]);
			if (dir)
				strcat(p, "/");
		}
		(*file)[i] = p;
	}
	free(buffer);
	return 0;
}

	void
FreeWild(num, file)
	int		num;
	char	**file;
{
	if (file == NULL || num == 0)
		return;
	while (num--)
		free(file[num]);
	free(file);
}

	int
has_wildcard(p)
	char *p;
{
#ifdef __STDC__
	return strpbrk(p, "*?[{`~$") != NULL;
#else
	for ( ; *p; ++p)
		if (strchr("*?[{`~$", *p) != NULL)
			return 1;
	return 0;
#endif
}

	int
have_wildcard(num, file)
	int		num;
	char	**file;
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
	char *src, *dest;
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
