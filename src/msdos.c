/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
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

#include <io.h>
#include "vim.h"
#include "globals.h"
#include "param.h"
#include "proto.h"
#include <conio.h>
#include <fcntl.h>
#include <bios.h>

static int WaitForChar __ARGS((int));
static int cbrk_handler __ARGS(());

typedef struct filelist
{
	char	**file;
	int		nfiles;
	int		maxfiles;
} FileList;

static void		addfile __ARGS((FileList *, char *, int));
static int		pstrcmp();	/* __ARGS((char **, char **)); BCC does not like this */
static void		strlowcpy __ARGS((char *, char *));
static int		expandpath __ARGS((FileList *, char *, int, int, int));

static int cbrk_pressed = FALSE;	/* set by ctrl-break interrupt */
static int ctrlc_pressed = FALSE;	/* set when ctrl-C or ctrl-break detected */
static int delayed_redraw = FALSE;	/* set when ctrl-C detected */

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
vim_remove(name)
	char *name;
{
	setperm(name, 0);    /* default permissions */
	return unlink(name);
}

/*
 * mch_write(): write the output buffer to the screen
 */
	void
mch_write(s, len)
	char	*s;
	int		len;
{
	char	*p;
	int		row, col;

	if (term_console)		/* translate ESC | sequences into bios calls */
		while (len--)
		{
			if (s[0] == '\n')
				putch('\r');
			else if (s[0] == ESC && len > 1 && s[1] == '|')
			{
				switch (s[2])
				{
				case 'J':	clrscr();
							goto got3;

				case 'K':	clreol();
							goto got3;

				case 'L':	insline();
							goto got3;

				case 'M':	delline();
got3:						s += 3;
							len -= 2;
							continue;

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':	p = s + 2;
							row = getdigits(&p);		/* no check for length! */
							if (p > s + len)
								break;
							if (*p == ';')
							{
								++p;
								col = getdigits(&p);	/* no check for length! */
								if (p > s + len)
									break;
								if (*p == 'H')
								{
									gotoxy(col, row);
									len -= p - s;
									s = p + 1;
									continue;
								}
							}
							else if (*p == 'm')
							{
								if (row == 0)
									normvideo();
								else
									textattr(row);
								len -= p - s;
								s = p + 1;
								continue;
							}
				}
			}
			putch(*s++);
		}
	else
		write(1, s, (unsigned)len);
}

#define POLL_SPEED 10	/* milliseconds between polls */

/*
 * Simulate WaitForChar() by slowly polling with bioskey(1) or kbhit().
 *
 * If Vim should work over the serial line after a 'ctty com1' we must use
 * kbhit() and getch(). (jw)
 * Usually kbhit() is not used, because then CTRL-C and CTRL-P
 * will be catched by DOS (mool).
 */

	static int
WaitForChar(msec)
	int msec;
{
	do
	{
		if ((p_biosk ? bioskey(1) : kbhit()) || cbrk_pressed)
			return 1;
		delay(POLL_SPEED);
		msec -= POLL_SPEED;
	}
	while (msec >= 0);
	return 0;
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
	char		*buf;
	int 		maxlen;
	int 		time;
{
	int 		len = 0;
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

	if (time >= 0)
	{
		if (time == 0)			/* don't know if time == 0 is allowed */
			time = 1;
		if (WaitForChar(time) == 0) 	/* no character available */
			return 0;
	}
	else	/* time == -1 */
	{
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
	if (p_biosk)
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
	else
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
	return len;
}

/*
 * We have no job control, fake it by starting a new shell.
 */
	void
mch_suspend()
{
	outstr("new shell started\n");
	call_shell(NULL, 0, TRUE);
}

extern int _fmode;
/*
 * we do not use windows, there is not much to do here
 */
	void
mch_windinit()
{
	_fmode = O_BINARY;		/* we do our own CR-LF translation */
	flushbuf();
	mch_get_winsize();
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
	/*
	 * In some cases with DOS 6.0 on a NEC notebook there is a 12 seconds
	 * delay when starting up that can be avoided by the next two lines.
	 * Don't ask me why!
	 * This could be fixed by removing setver.sys from config.sys. Forget it.
	gotoxy(1,1);
	cputs(" ");
	 */
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
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return non-zero for success.
 */
	int
dirname(buf, len)
	char	*buf;
	int		len;
{
	return (getcwd(buf, len) != NULL);
}

/*
 * Change default drive (for Turbo C, Borland C already has it)
 */
#ifndef __BORLANDC__
	int
_chdrive(drive)
	int drive;
{
	unsigned dummy;
	union REGS regs;

	regs.h.ah = 0x0e;
	regs.h.dl = drive - 1;
	intdos(&regs, &regs);	/* set default drive */
	regs.h.ah = 0x19;
	intdos(&regs, &regs);	/* get default drive */
	if (regs.h.al == drive - 1)
		return 0;
	else
		return -1;
}
#endif

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 */
	int
FullName(fname, buf, len)
	char	*fname, *buf;
	int		len;
{
	if (fname == NULL)	/* always fail */
		return 0;

#ifdef __BORLANDC__		/* the old Turbo C does not have this */
	if (_fullpath(buf, fname, len) == NULL)
	{
		strncpy(buf, fname, len);	/* failed, use the relative path name */
		return 0;
	}
	return 1;
#else					/* almost the same as FullName in unix.c */
	{
		int		l;
		char	olddir[MAXPATHL];
		char	*p, *q;
		int		c;
		int		retval = 1;

		*buf = 0;
		/*
		 * change to the directory for a moment,
		 * and then do the getwd() (and get back to where we were).
		 * This will get the correct path name with "../" things.
		 */
		p = strrchr(fname, '/');
		q = strrchr(fname, '\\');
		if (q && (p == NULL || q > p))
			p = q;
		q = strrchr(fname, ':');
		if (q && (p == NULL || q > p))
			p = q;
		if (p != NULL)
		{
			if (getcwd(olddir, MAXPATHL) == NULL)
			{
				p = NULL;		/* can't get current dir: don't chdir */
				retval = 0;
			}
			else
			{
				if (*p == ':' || (p > fname && p[-1] == ':'))
					q = p + 1;
				else
					q = p;
				c = *q;
				*q = NUL;
				if (chdir(fname))
					retval = 0;
				else
					fname = p + 1;
				*q = c;
			}
		}
		if (getcwd(buf, len) == NULL)
		{
			retval = 0;
			*buf = NUL;
		}
		l = strlen(buf);
		if (l && buf[l - 1] != '/' && buf[l - 1] != '\\')
			strcat(buf, "\\");
		if (p)
			chdir(olddir);
		strcat(buf, fname);
		return retval;
	}
#endif
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
	long	perm;
{
	perm &= ~FA_ARCH;
	return _chmod(name, 1, (int)perm);
}

/*
 * check if "name" is a directory
 */
	int
isdir(name)
	char *name;
{
	int f;

	f = _chmod(name, 0, 0);
	if (f == -1)
		return -1;					/* file does not exist at all */
	if ((f & FA_DIREC) == 0)
		return 0;					/* not a directory */
	return 1;
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
 * Does not change the tty, as bioskey() and kbhit() work raw all the time.
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
		if (term_console)
			outstr(T_TP);				/* set colors */
	}
	else
	{
		setcbrk(saved_cbrk);			/* restore ctrl-break setting */
		setvect(0x24, old_cint);		/* restore critical error interrupt */
		setvect(0x1B, old_cbrk);		/* restore ctrl-break interrupt */
		/* restore ctrl-break handler, how ??? */
		if (term_console)
			normvideo();				/* restore screen colors */
	}
}

/*
 * Structure used by Turbo-C/Borland-C to store video parameters.
 */
extern struct text_info _video;

	int
mch_get_winsize()
{
	int i;
	struct text_info ti;
/*
 * The screenwidth is returned by the BIOS OK.
 * The screenheight is in a location in the bios RAM, if the display is EGA or VGA.
 */
	if (!term_console)
		return 1;
	gettextinfo(&ti);
	Columns = ti.screenwidth;
	Rows = ti.screenheight;
	if (ti.currmode > 10)
		Rows = *(char far *)MK_FP(0x40, 0x84) + 1;
	set_window();

	if (Columns < 5 || Columns > MAX_COLUMNS ||
					Rows < 2 || Rows > MAX_COLUMNS)
	{
		/* these values are overwritten by termcap size or default */
		Columns = 80;
		Rows = 25;
		return 1;
	}
	Rows_max = Rows;				/* remember physical max height */

	check_winsize();
	script_winsize();

	return 0;
}

/*
 * Set the active window for delline/insline.
 */
	void
set_window()
{
	_video.screenheight = Rows;
	window(1, 1, Columns, Rows);
}

	void
mch_set_winsize()
{
	/* should try to set the window size to Rows and Columns */
	/* may involve switching display mode.... */
}

	int
call_shell(cmd, filter, cooked)
	char	*cmd;
	int 	filter; 		/* if != 0: called by dofilter() */
	int		cooked;
{
	int		x;
	char	newcmd[200];

	flushbuf();

	if (cooked)
		settmode(0);		/* set to cooked mode */

	if (cmd == NULL)
		x = system(p_sh);
	else
	{ 					/* we use "command" to start the shell, slow but easy */
		sprintf(newcmd, "%s /c %s", p_sh, cmd);
		x = system(newcmd);
	}
	outchar('\n');
	if (cooked)
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
	}
}

#define FL_CHUNK 32

	static void
addfile(fl, f, isdir)
	FileList	*fl;
	char		*f;
	int			isdir;
{
	char		*p;

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

		t = (char **)lalloc(sizeof(char *) * (fl->maxfiles + FL_CHUNK), TRUE);
		if (!t)
			return;
		for (i = fl->nfiles - 1; i >= 0; i--)
			t[i] = fl->file[i];
		free(fl->file);
		fl->file = t;
		fl->maxfiles += FL_CHUNK;
	}
	p = alloc((unsigned)(strlen(f) + 1 + isdir));
	if (p)
	{
		strcpy(p, f);
		if (isdir)
			strcat(p, "\\");
	}
	fl->file[fl->nfiles++] = p;
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
	/* If we are expanding wildcards we try both files and directories */
	if ((c = findfirst(buf, &fb, (*path || !notf) ? FA_DIREC : 0)) != 0)
	{
		/* not found */
		strcpy(e, path);
		if (notf)
			addfile(fl, buf, FALSE);
		return 1; /* unexpanded or empty */
	}
	while (!c)
	{
		strlowcpy(s, fb.ff_name);
		if (*s != '.' || (s[1] != '\0' && (s[1] != '.' || s[2] != '\0')))
		{
			strcat(buf, path);
			if (!has_wildcard(path))
				addfile(fl, buf, (isdir(buf) > 0));
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
			addfile(&f, pat[i], files_only ? FALSE : (isdir(pat[i]) > 0));
		else
			r |= expandpath(&f, pat[i], files_only, 0, list_notfound);
	}
	if (r == 0)
	{
		*num_file = f.nfiles;
		*file = f.file;
	}
	else
	{
		*num_file = 0;
		*file = NULL;
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

/*
 * The normal chdir() does not change the default drive.
 * This one does.
 */
#undef chdir
	int
vim_chdir(path)
	char *path;
{
	if (path[0] == NUL)				/* just checking... */
		return 0;
	if (path[1] == ':')				/* has a drive name */
	{
		if (_chdrive(toupper(path[0]) - 'A' + 1))
			return -1;				/* invalid drive name */
		path += 2;
	}
	if (*path == NUL)				/* drive name only */
		return 0;
	return chdir(path);				/* let the normal chdir() do the rest */
}
