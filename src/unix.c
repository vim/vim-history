/* vi:ts=4:sw=4
 *
 *
 * VIM - Vi IMitation
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
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <signal.h>

#ifdef SYSV
# include <poll.h>
# include <termio.h>
#else
# include <sgtty.h>
# include <sys/time.h>
#endif


static int	Read __ARGS((int, char *, long));
static int	WaitForChar __ARGS((int));
static int	RealWaitForChar __ARGS((int));
#ifndef linux
static void sig_winch __ARGS((void));
#endif

static int do_resize = FALSE;

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

/*
 * the number of calls to Write is reduced by using the buffer "outbuf"
 */
#define BSIZE	2048
static u_char	outbuf[BSIZE];
static int		bpos = 0;

/*
 * flushbuf(): flush the output buffer
 */
	void
flushbuf()
{
	if (bpos != 0)
		write(1, (char *)outbuf, (long)bpos);
	bpos = 0;
}

/*
 * outchar(c): put a character into the output buffer. Flush it if it becomes full.
 */
	void
outchar(c)
	unsigned c;
{
	if (c == '\n')		/* turn LF into CR-LF (CRMOD does not seem to do this) */
		outchar('\r');
	outbuf[bpos] = c;
	++bpos;
	if (bpos >= BSIZE)
		flushbuf();
}

/*
 * GetChars(): low level input funcion.
 * Get a characters from the keyboard.
 * If type == T_PEEK do not wait for characters.
 * If type == T_WAIT wait a short time for characters.
 * If type == T_BLOCK wait for characters.
 */
	int
GetChars(buf, maxlen, type)
	char	*buf;
	int		maxlen;
	int		type;
{
	int		len;
	int		time = 1000;

	switch (type)
	{
	case T_PEEK:
		time = 20;
	case T_WAIT:
		if (WaitForChar(time) == 0)		/* no character available */
			return 0;
		break;

	case T_BLOCK:
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
		len = Read(0, buf, (long)maxlen);
		if (len > 0)
			return len;
	}
}

	void
vim_delay()
{
#ifdef SYSV
	poll(0,0, 500);
#else
	struct timeval tv;

	tv.tv_sec = 25 / 50;
	tv.tv_usec = (25 % 50) * (1000000/50);
	select(0, 0, 0, 0, &tv);
#endif
}

	static void
sig_winch()
{
#if defined(SYSV) || defined(linux)
	signal(SIGWINCH, sig_winch);
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
	call_shell(NULL, 0);
#endif
}

	void
mch_windinit()
{
	Columns = 80;
	Rows = 24;

	flushbuf();

	mch_get_winsize();
	signal(SIGWINCH, sig_winch);
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
 * get name of current directory into buffer 'buf' of length 'len' bytes
 */
	int 
dirname(buf, len)
	char *buf;
	int len;
{
#ifdef SYSV
	extern int		errno;
	extern char		*sys_errlist[];

	if (getcwd(buf,len) == NULL)
	{
	    strcpy(buf, sys_errlist[errno]);
	    return 1;
	}
    return 0;
#else
	return (int)getwd(buf);
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
	*buf = 0;
#if defined(linux) || defined(UNIX_WITHOUT_AMD)
    {
    	int l;

    	if (*fname != '/')
		{
#ifdef SYSV
			(void)getcwd(buf,len);
#else
			(void)getwd(buf);
#endif
			l = strlen(buf);
			if (l && buf[l-1] != '/')
				strcat(buf, "/");
		}
    }
#endif /* UNIX_WITHOUT_AMD */
	strcat(buf, fname);
	return 0;
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
	return chmod(name, perm);
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
	return (statb.st_mode & S_IFMT) != S_IFREG;
}

	void
mch_windexit(r)
	int r;
{
	flushbuf();

	settmode(0);
	stopscript();					/* remove autoscript file */
	exit(r);
}

	void
mch_settmode(raw)
	int				raw;
{
#if defined(ECHOE) && defined(ICANON)
	/* for "new" tty systems */
	static struct termio told;
		   struct termio tnew;

	if (raw)
	{
		ioctl(0, TCGETA, &told);
		tnew = told;
		tnew.c_iflag &= ~ICRNL;			/* enables typing ^V^M */
		tnew.c_lflag = ~(ICANON | ECHO | ISIG | ECHOE);
		ioctl(0, TCSETA, &tnew);
	}
	else
		ioctl(0, TCSETA, &told);
#else
	/* for "old" tty systems */
	static struct sgttyb ttybold;
		   struct sgttyb ttybnew;

	if (raw)
	{
		ioctl(0, TIOCGETP, &ttybold);
		ttybnew = ttybold;
		ttybnew.sg_flags &= ~(CRMOD | ECHO);
		ttybnew.sg_flags |= RAW;
		ioctl(0, TIOCSETP, &ttybnew);
	}
	else
		ioctl(0, TIOCSETP, &ttybold);
#endif
}

	int
mch_get_winsize()
{
	int				old_Rows = Rows;
	int				old_Columns = Columns;
	char			*p;
	struct winsize	ws;

	Columns = 0;
	Rows = 0;
	if (!do_resize)
	{
	    if (p = getenv("LINES"))
			Rows = atoi(p);
	    if (p = getenv("COLUMNS"))
			Columns = atoi(p);
	    if (Columns <= 0 || Rows <= 0)
			do_resize = TRUE;
	}
	if (do_resize)
	{
# ifdef TIOCGWINSZ
#  ifdef DEBUG
	    extern int errno;
#  endif /* DEBUG */

	    if (ioctl(0, TIOCGWINSZ, &ws))
	    {
			debug1("TIOCGWINSZ ioctl errno %d\n", errno);
	    }
	    else
	    {
			Columns = ws.ws_col;
			Rows = ws.ws_row;
	    }
# endif /* TIOCGWINSZ */
	    do_resize = FALSE;
	}
	if (Columns <= 0 || Rows <= 0)
	{
		Columns = old_Columns;
		Rows = old_Rows;
		return 1;
	}
	debug2("mch_get_winsize: %dx%d\n", (int)Columns, (int)Rows);

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
call_shell(cmd, dummy)
	char	*cmd;
	int		dummy;
{
	int		x;
	char	newcmd[1024];

	flushbuf();

	settmode(0); 				/* set to cooked mode */

	if (cmd == NULL)
		x = system(p_sh);
	else
	{			/* we use "sh" to start the shell, slow but easy */
		sprintf(newcmd, "%s -c \"%s\"", p_sh, cmd);
		x = system(newcmd);
	}
	if (x == 127)
	{
		smsg("Cannot execute shell sh");
		outchar('\n');
	}
	else if (x)
	{
		smsg("%d returned", x);
		outchar('\n');
	}

	settmode(1); 						/* set to raw mode */
	return x;
}

/*
 * The input characters are buffered to be able to check for a CTRL-C.
 * This should be done with signals, but I don't know how to do that in
 * a portable way for a tty in RAW mode.
 */

#define INBUFLEN 50
static char		inbuf[INBUFLEN];
static int		inbufcount = 0;

	static int
Read(fd, buf, maxlen)
	int		fd;
	char	*buf;
	long	maxlen;
{
	if (inbufcount)		/* characters in inbuf[] */
	{
		if (maxlen > inbufcount)
			maxlen = inbufcount;
		memmove(buf, inbuf, maxlen);
		inbufcount -= maxlen;
		if (inbufcount)
				memmove(inbuf, inbuf + maxlen, inbufcount);
		return (int)maxlen;
	}
	return read(fd, buf, maxlen);
}

	void
breakcheck()
{
	int len;

/*
 * check for CTRL-C typed by reading all available characters
 */
	if (RealWaitForChar(0))		/* if characters available */
	{
		len = read(0, inbuf + inbufcount, (long)(INBUFLEN - inbufcount));
		while (len-- > 0)
		{
			/*
			 * if a CTRL-C was typed, remove it from the buffer and set got_int
			 */
			if (inbuf[inbufcount++] == 3)
			{
				/* remove everything typed before the CTRL-C */
				if (len)
					memmove(inbuf, inbuf + inbufcount, len);
				inbufcount = 0;
				got_int = TRUE;
				flush_buffers();		/* remove all typeahead */
			}
		}
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
#ifdef SYSV
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
	return (select(1, &fdset, 0, 0, (ticks >= 0) ? &tv : 0));
#endif
}

	int 
remove(buf)
#ifdef linux
	const
#endif
			char *buf;
{
	return unlink(buf);
}

#ifdef WILD_CARDS
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
	size_t	len;
	FILE	*fd;
	char	*buffer;
	char	*p;

	*num_file = 0;		/* default: no files found */
	*file = (char **)"";

/*
 * get a name for the temp file
 */
	strcpy(tmpname, TMPNAME2);
	if (*mktemp(tmpname) == NUL)
	{
		*file = (char **)e_notmp;
	    return 1;
	}

/*
 * let the shell expand the patterns and write the result into the temp file
 */
	len = TMPNAMELEN + 10;
	for (i = 0; i < num_pat; ++i)		/* count the length of the patterns */
		len += strlen(pat[i]) + 3;
	command = (char *)alloc(len);
	if (command == NULL)
		return 1;
	strcpy(command, "echo > ");			/* built the shell command */
	strcat(command, tmpname);
	for (i = 0; i < num_pat; ++i)
	{
		strcat(command, " \"");
		strcat(command, pat[i]);
		strcat(command, "\"");
	}
	i = call_shell(command, 0);				/* execute it */
	free(command);
	if (i)									/* call_shell failed */
	{
		remove(tmpname);
		sleep(1);			/* give the user a chance to read error messages */
		updateScreen(CLEAR);			/* probably messed up screen */
		return 1;
	}

/*
 * read the names from the file into memory
 */
 	fd = fopen(tmpname, "r");
	if (fd == NULL)
	{
		*file = (char **)e_notopen;
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
		*file = (char **)e_notread;
		free(buffer);
		return 1;
	}

	p = buffer;
	for (i = 0; *p; ++i)		/* get number of entries */
	{
		while (*p != ' ' && *p != '\n')
			++p;
		if (*p == '\n')
		{
			++i;
			break;
		}
		skipspace(&p);
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
		while (*p != ' ' && *p != '\n')
			++p;
		if (*p == '\n')
		{
			*p = NUL;
			break;
		}
		*p++ = NUL;
		skipspace(&p);
	}
	return 0;
}

	void
FreeWild(num, file)
	int		num;
	char	**file;
{
	if (file == NULL || num <= 0)
		return;
	free(file[0]);
	free(file);
}
#endif	/* WILD_CARDS */
