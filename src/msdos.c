/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:		Bram Moolenaar			mool@oce.nl
 *								Tim Thompson			twitch!tjt
 *								Tony Andrews			onecom!wldrdg!tony
 *								G. R. (Fred) Walter 	watmath!watcgl!grwalter
 */

/*
 * msdos.c
 *
 * MSDOS system-dependent routines.
 * A cheap plastic imitation of the amiga dependent code.
 * A lot in this file was made by Juergen Weigert (jw).
 */

#include "vim.h"
#include "globals.h"
#include "param.h"
#include "proto.h"
#include <conio.h>
#include <fcntl.h>
#include <bios.h>

static int WaitForChar __ARGS((int));
static int cbrk_handler __ARGS(());

#ifdef WILD_CARDS
typedef struct filelist
{
	char	**file;
	int		nfiles;
	int		maxfiles;
} FileList;

static void		addfile __ARGS((FileList *, char *));
static int		pstrcmp __ARGS((char **, char **));
static void		strlowcpy __ARGS((char *, char *));
static int		expandpath __ARGS((FileList *, char *, int, int, int));
#endif

static int cbrk_pressed = FALSE;	/* set by ctrl-break interrupt */
static int ctrlc_pressed = FALSE;	/* set when ctrl-C or ctrl-break detected */
static int delayed_redraw = FALSE;	/* set when ctrl-C detected */

/*
 * the number of calls to Write is reduced by using the buffer "outbuf"
 */
#define BSIZE	2048
static u_char	outbuf[BSIZE];
static int		bpos = 0;

#ifdef DEBUG
/*
 * Put two characters in the video buffer without calling BIOS or DOS.
 */
blink(n)
	int n;
{
	char far *p;
	static int counter;

	p = MK_FP(0xb800, 0x10 + n);		/* p points in screen buffer */
	*p = counter;
	*(p + 1) = counter;
	*(p + 2) = counter;
	*(p + 3) = counter;
	++counter;
}
#endif

	void
vim_delay()
{
	delay(500);
}

/*
 * this version of remove is not scared by a readonly (backup) file
 */
	int
remove(name)
	char *name;
{
	setperm(name, 0);    /* default permissions */
	return unlink(name);
}

/*
 * flushbuf(): flush the output buffer
 */
	void
flushbuf()
{
	if (bpos != 0)
	{
		write(1, (char *)outbuf, (long)bpos);
		bpos = 0;
	}
}

/*
 * outchar(c): put a character into the output buffer.
 * Flush it if it becomes full.
 */
	void
outchar(c)
	unsigned c;
{
	outbuf[bpos] = c;
	++bpos;
	if (bpos >= BSIZE)
		flushbuf();
}

/*
 * outstr(s): put a string character at a time into the output buffer.
 */
	void
outstr(s)
	register char *s;
{
	if (!s)			/* s is NULL in case of not defined termcap entry */
		return;
	/*
	 * The prefix ESC| is used to emulate capabilities
	 * missing in ansi.sys by direct calls to conio routines.
	 * If we want to avoid this we need the nansi.sys driver. (jw)
	 * Only works if the string starts with ESC!
	 */
	if (s[0] == ESC && s[1] == '|')
	{
		flushbuf();
		switch (s[2])
		{
		case 'L':	insline();
			 			return;

		case 'M':	delline();
					return;

		default:	outstr("OOPS");
					return;
		}
	}
#ifdef TERMCAP
	tputs(s, 1, outchar);
#else
	while (*s)
		outchar(*s++);
#endif
}

#define POLL_SPEED 10	/* milliseconds between polls */

/*
 * Simulate WaitForChar() by slowly polling with bioskey(1).
 *
 * If Vim should work over the serial line after a 'ctty com1' we must use
 * kbhit() and getch(). (jw)
 * Usually kbhit() is not used, because then CTRL-C will be catched by DOS (mool).
 */

	static int
WaitForChar(msec)
	int msec;
{
	do
	{
#ifdef USE_KBHIT
		if (kbhit() || cbrk_pressed)
#else
		if (bioskey(1) || cbrk_pressed)
#endif
			return 1;
		delay(POLL_SPEED);
		msec -= POLL_SPEED;
	}
	while (msec >= 0);
	return 0;
}

/*
 * GetChars(): low level input function.
 * Get characters from the keyboard.
 * If type == T_PEEK do not wait for characters.
 * If type == T_WAIT wait a short time for characters.
 * If type == T_BLOCK wait for characters.
 */
	int
GetChars(buf, maxlen, type)
	char		*buf;
	int 		maxlen;
	int 		type;
{
	int 		len = 0;
	int 		time = 1000;	/* one second */
	int			c;

/*
 * if we got a ctrl-C when we were busy, there will be a "^C" somewhere
 * on the sceen, so we need to redisplay it.
 */
	if (delayed_redraw)
	{
		delayed_redraw = FALSE;
		updateScreen(CLEAR);
		setcursor();
		flushbuf();
	}

	switch (type)
	{
	case T_PEEK:
		time = 1;
		 /* FALLTHROUGH */

	case T_WAIT:
		if (WaitForChar(time) == 0) 	/* no character available */
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

/*
 * Try to read as many characters as there are.
 * Works for the controlling tty only.
 */
	--maxlen;		/* may get two chars at once */
	/*
	 * we will get at least one key. Get more if they are available
	 * After a ctrl-break we have to read a 0 (!) from the buffer.
	 * bioskey(1) will return 0 if no key is available and when a
	 * ctrl-break was typed. When ctrl-break is hit, this does not always
	 * implies a key hit.
	 */
	cbrk_pressed = FALSE;
#ifdef USE_KBHIT
	while ((len == 0 || kbhit()) && len < maxlen)
	{
		switch (c = getch())
		{
		case 0:
				*buf++ = K_NUL;
				break;
		case 3:
				cbrk_pressed = TRUE;
				/*FALLTHROUGH*/
		default:
				*buf++ = c;
		}
		len++;
	}
#else
	while ((len == 0 || bioskey(1)) && len < maxlen)
	{
		c = bioskey(0);			/* get the key */
		if (c == 0)				/* ctrl-break */
			c = 3;				/* return a CTRL-C */
		if ((c & 0xff) == 0)
		{
			if (c == 0x0300)		/* CTRL-@ is 0x0300, translated into K_ZERO */
				c = K_ZERO;
			else		/* extended key code 0xnn00 translated into K_NUL, nn */
			{
				c >>= 8;
				*buf++ = K_NUL;
				++len;
			}
		}

		*buf++ = c;
		len++;
	}
#endif
	return len;
}

extern int _fmode;

	void
textfile(on)
	int on;
{
	/*
	 * in O_TEXT mode we read and write files with CR/LF translation.
	 */
	_fmode = on ? O_TEXT : O_BINARY;
}

/*
 * We have no job control, fake it by starting a new shell.
 */
	void
mch_suspend()
{
	outstr("new shell started\n");
	stoptermcap();
	call_shell(NULL, 0);
	starttermcap();
}

/*
 * we do not use windows, there is not much to do here
 */
	void
mch_windinit()
{
	textfile(p_tx);
	flushbuf();
}

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
 *				 msdos filesystem is far to primitive for that. do nothing.
 */
	void
fname_case(name)
	char *name;
{
}

/*
 * settitle(): set titlebar of our window.
 * Dos console has no title.
 */
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
	char	*buf;
	int		len;
{
	return (int)getcwd(buf, len);
}

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 */
	int
FullName(fname, buf, len)
	char	*fname, *buf;
	int		len;
{
	/* failed, because we are under MSDOS */
	strncpy(buf, fname, len);
	return 0;
}

/*
 * get file permissions for 'name'
 * -1 : error
 * else FA_attributes defined in dos.h
 */
	long
getperm(name)
	char *name;
{
	int r;

	r = _chmod(name, 0, 0);		 /* get file mode */
	return r;
}

/*
 * set file permission for 'name' to 'perm'
 */
	int
setperm(name, perm)
	char	*name;
	int		perm;
{
	perm &= ~FA_ARCH;
	return _chmod(name, 1, perm);
}

/*
 * check if "name" is a directory
 */
	int
isdir(name)
	char *name;
{
	return (_chmod(name, 0, 0) & FA_DIREC) ? 1 : 0;
}

/*
 * Careful: mch_windexit() may be called before mch_windinit()!
 */
	void
mch_windexit(r)
	int r;
{
	settmode(0);
	stoptermcap();
	flushbuf();
	stopscript(); 				/* remove autoscript file */
	exit(r);
}

/*
 * function for ctrl-break interrupt
 */
	void interrupt
catch_cbrk()
{
	cbrk_pressed = TRUE;
	ctrlc_pressed = TRUE;
}

/*
 * ctrl-break handler for DOS. Never called when a ctrl-break is typed, because
 * we catch interrupt 1b. If you type ctrl-C while Vim is waiting for a
 * character this function is not called. When a ctrl-C is typed while Vim is
 * busy this function may be called. By that time a ^C has been displayed on
 * the screen, so we have to redisplay the screen. We can't do that here,
 * because we may be called by DOS. The redraw is in GetChars().
 */
	static int
cbrk_handler()
{
	delayed_redraw = TRUE;
	return 1; 				/* resume operation after ctrl-break */
}

/*
 * function for critical error interrupt
 * For DOS 1 and 2 return 0 (Ignore).
 * For DOS 3 and later return 3 (Fail)
 */
	void interrupt
catch_cint(bp, di, si, ds, es, dx, cx, bx, ax)
	unsigned bp, di, si, ds, es, dx, cx, bx, ax;
{
	ax = (ax & 0xff00);		/* set AL to 0 */
	if (_osmajor >= 3)
		ax |= 3;			/* set AL to 3 */
}

/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 *
 * Does not change the tty, as bioskey() works raw all the time.
 */

extern void interrupt CINT_FUNC();

	void
mch_settmode(raw)
	int  raw;
{
	static int saved_cbrk;
	static void interrupt (*old_cint)();
	static void interrupt (*old_cbrk)();

	if (raw)
	{
		saved_cbrk = getcbrk();			/* save old ctrl-break setting */
		setcbrk(0);						/* do not check for ctrl-break */
		old_cint = getvect(0x24); 		/* save old critical error interrupt */
		setvect(0x24, catch_cint);		/* install our critical error interrupt */
		old_cbrk = getvect(0x1B); 		/* save old ctrl-break interrupt */
		setvect(0x1B, catch_cbrk);		/* install our ctrl-break interrupt */
		ctrlbrk(cbrk_handler);			/* vim's ctrl-break handler */
	}
	else
	{
		setcbrk(saved_cbrk);			/* restore ctrl-break setting */
		setvect(0x24, old_cint);		/* restore critical error interrupt */
		setvect(0x1B, old_cbrk);		/* restore ctrl-break interrupt */
		/* restore ctrl-break handler, how ??? */
	}
}

	int
mch_get_winsize()
{
	struct text_info ti;

	debug("mch_get_winsize\n");
	if (!term_console)
		return 1;
	gettextinfo(&ti);
	Columns = ti.screenwidth;
	Rows = ti.screenheight;
	if (Columns < 5 || Columns > MAX_COLUMNS ||
					Rows < 2 || Rows > MAX_COLUMNS)
	{
		/* these values are not used. overwritten by termcap size or default */
		Columns = 80;
		Rows = 24;
		return 1;
	}
	return 0;
}

	void
mch_set_winsize()
{
	/* should try to set the window size to Rows and Columns */
	/* may involve switching display mode.... */
}

	int
call_shell(cmd, filter)
	char	*cmd;
	int 	filter; 		/* if != 0: called by dofilter() */
{
	int		x;
	char	newcmd[200];

	flushbuf();

	settmode(0);		/* set to cooked mode */

	if (cmd == NULL)
		x = system(p_sh);
	else
	{ 					/* we use "command" to start the shell, slow but easy */
		sprintf(newcmd, "%s /c %s", p_sh, cmd);
		x = system(newcmd);
	}
	outchar('\n');
	settmode(1);		/* set to raw mode */

	if (x)
	{
		smsg("%d returned", x);
		outchar('\n');
	}

	resettitle();
	return x;
}

/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
	void
breakcheck()
{
	if (ctrlc_pressed)
	{
		ctrlc_pressed = FALSE;
		got_int = TRUE;
		flush_buffers();		/* remove all typeahead and macro stuff */
	}
}

#ifdef WILD_CARDS
#define FL_CHUNK 32

	static void
addfile(fl, f)
	FileList	*fl;
	char		*f;
{
	if (!fl->file)
	{
		fl->file = (char **)alloc(sizeof(char *) * FL_CHUNK);
		if (!fl->file)
			return;
		fl->nfiles = 0;
		fl->maxfiles = FL_CHUNK;
	}
	if (fl->nfiles >= fl->maxfiles)
	{
		char	**t;
		int		i;

		t = (char **)alloc(sizeof(char *) * (fl->maxfiles + FL_CHUNK));
		if (!t)
			return;
		for (i = fl->nfiles - 1; i >= 0; i--)
			t[i] = fl->file[i];
		free(fl->file);
		fl->file = t;
		fl->maxfiles += FL_CHUNK;
	}
	fl->file[fl->nfiles++] = f;
}

	static int
pstrcmp(a, b)
	char **a, **b;
{
	return (strcmp(*a, *b));
}

	int
has_wildcard(s)
	char *s;
{
	if (s)
		for ( ; *s; ++s)
			if (*s == '?' || *s == '*')
				return 1;
	return 0;
}

	static void
strlowcpy(d, s)
	char *d, *s;
{
	while (*s)
		*d++ = tolower(*s++);
	*d = '\0';
}

	static int
expandpath(fl, path, fonly, donly, notf)
	FileList	*fl;
	char		*path;
	int			fonly, donly, notf;
{
	char	buf[MAXPATH];
	char	*p, *s, *e;
	int		lastn, c, r;
	struct	ffblk fb;

	lastn = fl->nfiles;

/*
 * Find the first part in the path name that contains a wildcard.
 * Copy it into buf, including the preceding characters.
 */
	p = buf;
	s = NULL;
	e = NULL;
	while (*path)
	{
		if (*path == '\\' || *path == ':' || *path == '/')
		{
			if (e)
				break;
			else
				s = p;
		}
		if (*path == '*' || *path == '?')
			e = p;
		*p++ = *path++;
	}
	e = p;
	if (s)
		s++;
	else
		s = buf;

	/* now we have one wildcard component between s and e */
	*e = '\0';
	r = 0;
	if ((c = findfirst(buf, &fb, *path ? FA_DIREC : 0)) != 0)
	{
		/* not found */
		strcpy(e, path);
		if (notf)
			addfile(fl, strsave(buf));
		return 1; /* unexpanded or empty */
	}
	while (!c)
	{
		strlowcpy(s, fb.ff_name);
		if (*s != '.' || (s[1] != '\0' && (s[1] != '.' || s[2] != '\0')))
		{
			strcat(buf, path);
			if (!has_wildcard(path))
				addfile(fl, strsave(buf));
			else
				r |= expandpath(fl, buf, fonly, donly, notf);
		}
		c = findnext(&fb);
	}
	qsort(fl->file + lastn, fl->nfiles - lastn, sizeof(char *), pstrcmp);
	return r;
}

/*
 * MSDOS rebuilt of Scott Ballantynes ExpandWildCard for amiga/arp.
 * jw
 */

	int
ExpandWildCards(num_pat, pat, num_file, file, files_only, list_notfound)
	int 	num_pat;
	char	**pat;
	int 	*num_file;
	char	***file;
	int 	files_only, list_notfound;
{
	int			i, r = 0;
	FileList	f;

	f.file = NULL;
	f.nfiles = 0;
	for (i = 0; i < num_pat; i++)
	{
		if (!has_wildcard(pat[i]))
			addfile(&f, strsave(pat[i]));
		else
			r |= expandpath(&f, pat[i], files_only, 0, list_notfound);
	}
	if (r == 0)
	{
		*num_file = f.nfiles;
		*file = f.file;
	}
	return r;
}

	void
FreeWild(num, file)
	int		num;
	char	**file;
{
	if (file == NULL || num <= 0)
		return;
	while (num--)
		free(file[num]);
	free(file);
}
#endif /* WILD_CARDS */
