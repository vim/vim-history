/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * msdos.c
 *
 * MSDOS system-dependent routines.
 * A cheap plastic imitation of the amiga dependent code.
 * A lot in this file was made by Juergen Weigert (jw).
 *
 * DJGPP changes by Gert van Antwerpen
 */

#include <io.h>
#include "vim.h"
#include "globals.h"
#include "option.h"
#include "proto.h"
#include <conio.h>
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include <bios.h>
#ifdef DJGPP
# include <dpmi.h>
# include <signal.h>
#else
# include <alloc.h>
#endif

static int WaitForChar __ARGS((long));
#ifdef USE_MOUSE
static void show_mouse __ARGS((int));
static void mouse_area __ARGS((void));
#endif
static int change_drive __ARGS((int));
static void slash_adjust __ARGS((char_u *));
static int cbrk_handler __ARGS(());

typedef struct filelist
{
	char_u	**file;
	int		nfiles;
	int		maxfiles;
} FileList;

static void		addfile __ARGS((FileList *, char_u *, int));
static int		pstrcmp();	/* __ARGS((char **, char **)); BCC does not like this */
static void		strlowcpy __ARGS((char_u *, char_u *));
static int		expandpath __ARGS((FileList *, char_u *, int, int, int));

static int cbrk_pressed = FALSE;	/* set by ctrl-break interrupt */
static int ctrlc_pressed = FALSE;	/* set when ctrl-C or ctrl-break detected */
static int delayed_redraw = FALSE;	/* set when ctrl-C detected */

#ifdef USE_MOUSE
static int mouse_avail = FALSE;		/* mouse present */
static int mouse_active;			/* mouse enabled */
static int mouse_hidden;			/* mouse not shown */
static int mouse_click = 0;			/* mouse status */
static int mouse_x = -1;			/* mouse x coodinate */
static int mouse_y = -1;			/* mouse y coodinate */
static long mouse_click_time = 0;	/* biostime() of last click */
static int mouse_click_count = 0;	/* count for multi-clicks */
static int mouse_last_click = 0;	/* previous status at click */
static int mouse_click_x = 0;		/* x of previous mouse click */
static int mouse_click_y = 0;		/* y of previous mouse click */
static linenr_t mouse_topline = 0;	/* topline at previous mouse click */
#endif

#define BIOSTICK	55				/* biostime() increases one tick about
										every 55 msec */

	long
mch_avail_mem(special)
	int special;
{
#ifdef DJGPP
	return _go32_dpmi_remaining_virtual_memory();
#else
	return coreleft();
#endif
}

/*
 * don't do anything for about 500 msec
 */
	void
mch_delay(msec, ignoreinput)
	long	msec;
	int		ignoreinput;
{
	long	starttime;

	if (ignoreinput)
	{
		starttime = biostime(0, 0L);
		while (biostime(0, 0L) < starttime + msec / BIOSTICK)
			;
	}
	else
		WaitForChar(msec);
}

/*
 * this version of remove is not scared by a readonly (backup) file
 *
 * returns -1 on error, 0 otherwise (just like remove())
 */
	int
vim_remove(name)
	char_u	*name;
{
	(void)setperm(name, 0);    /* default permissions */
	return unlink((char *)name);
}

/*
 * mch_write(): write the output buffer to the screen
 */
	void
mch_write(s, len)
	char_u	*s;
	int		len;
{
	char_u	*p;
	int		row, col;

	if (term_console)		/* translate ESC | sequences into bios calls */
		while (len--)
		{
			if (p_wd)		/* testing: wait a bit for each char */
				WaitForChar(p_wd);

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
								if (*p == 'H' || *p == 'r')
								{
									if (*p == 'H')		/* set cursor position */
										gotoxy(col, row);
									else				/* set scroll region  */
										window(1, row, Columns, col);
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

/*
 * Simulate WaitForChar() by slowly polling with bioskey(1) or kbhit().
 *
 * If Vim should work over the serial line after a 'ctty com1' we must use
 * kbhit() and getch(). (jw)
 * Usually kbhit() is not used, because then CTRL-C and CTRL-P
 * will be catched by DOS (mool).
 *
 * return TRUE if a character is available, FALSE otherwise
 */

#define FOREVER 1999999999L

	static	int
WaitForChar(msec)
	long	msec;
{
	static int	last_status = 0;
	union REGS	regs;
	long		starttime;
	int			x, y;

	starttime = biostime(0, 0L);

	for (;;)
	{
#ifdef USE_MOUSE
		long	clicktime;

		if (mouse_avail && mouse_active && !mouse_click)
		{
			regs.x.ax = 3;
			int86(0x33, &regs, &regs);		/* check mouse status */
				/* only recognize button-down and button-up event */
			x = regs.x.cx / 8;
			y = regs.x.dx / 8;
			if ((last_status == 0) != (regs.x.bx == 0))
			{
				if (last_status)		/* button up */
					mouse_click = MOUSE_RELEASE;
				else					/* button down */
				{
					mouse_click = regs.x.bx;

					/*
					 * Find out if this is a multi-click
					 */
					clicktime = biostime(0, 0L);
					if (mouse_click_x == x && mouse_click_y == y &&
							mouse_topline == curwin->w_topline &&
							mouse_click_count != 4 &&
							mouse_click == mouse_last_click &&
							clicktime < mouse_click_time + p_mouset / BIOSTICK)
						++mouse_click_count;
					else
						mouse_click_count = 1;
					mouse_click_time = clicktime;
					mouse_last_click = mouse_click;
					mouse_click_x = x;
					mouse_click_y = y;
					mouse_topline = curwin->w_topline;
					SET_NUM_MOUSE_CLICKS(mouse_click, mouse_click_count);
				}
			}
			else if (last_status && (x != mouse_x || y != mouse_y))
				mouse_click = MOUSE_DRAG;
			last_status = regs.x.bx;
			if (mouse_hidden && mouse_x >= 0 && (mouse_x != x || mouse_y != y))
			{
				mouse_hidden = FALSE;
				show_mouse(TRUE);
			}
			mouse_x = x;
			mouse_y = y;
		}
#endif

		if ((p_biosk ? bioskey(1) : kbhit()) || cbrk_pressed
#ifdef USE_MOUSE
													|| mouse_click
#endif
				)
			return TRUE;
		/*
		 * Use biostime() to wait until our time is done.
		 * Don't use delay(), it doesn't work properly under Windows 95
		 * (because it disables interrupts?).
		 */
		if (msec != FOREVER && biostime(0, 0L) > starttime + msec / BIOSTICK)
			break;
	}
	return FALSE;
}

/*
 * mch_inchar(): low level input funcion.
 * Get a characters from the keyboard.
 * If time == 0 do not wait for characters.
 * If time == n wait a short time for characters.
 * If time == -1 wait forever for characters.
 *
 * return the number of characters obtained
 */
	int
mch_inchar(buf, maxlen, time)
	char_u		*buf;
	int 		maxlen;
	long 		time;
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

#ifdef USE_MOUSE
	if (time != 0)
		show_mouse(TRUE);
#endif
	if (time >= 0)
	{
		if (WaitForChar(time) == 0) 	/* no character available */
		{
#ifdef USE_MOUSE
			show_mouse(FALSE);
#endif
			return 0;
		}
	}
	else	/* time == -1 */
	{
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk
	 */
		if (WaitForChar(p_ut) == 0)
			updatescript(0);
	}
	WaitForChar(FOREVER);		/* wait for key or mouse click */

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
#ifdef USE_MOUSE
	if (mouse_click && maxlen >= 6)
	{
		len = 5;
		*buf++ = ESC + 128;
		*buf++ = 'M';
		*buf++ = mouse_click;
		*buf++ = mouse_x + '!';
		*buf++ = mouse_y + '!';
		mouse_click = 0;
	}
	else
#endif
	{
#ifdef USE_MOUSE
		mouse_hidden = TRUE;
#endif
		if (p_biosk)
		{
			while ((len == 0 || bioskey(1)) && len < maxlen)
			{
				c = bioskey(0);			/* get the key */
				/*
				 * translate a few things for inchar():
				 * 0x0000 == CTRL-break			-> 3	(CTRL-C)
				 * 0x0300 == CTRL-@     		-> NUL
				 * 0xnn00 == extended key code	-> K_NUL, nn
				 * K_NUL					  	-> K_NUL, 3
				 */
				if (c == 0)
					c = 3;
				else if (c == 0x0300)
					c = NUL;
				else if ((c & 0xff) == 0 || c == K_NUL)
				{
					if (c == K_NUL)
						c = 3;
					else
						c >>= 8;
					*buf++ = K_NUL;
					++len;
				}

				*buf++ = c;
				len++;
			}
		}
		else
		{
			while ((len == 0 || kbhit()) && len < maxlen)
			{
				switch (c = getch())
				{
					case 0:
						/* NUL means that there is another character.
						 * Get it immediately, because kbhit() doesn't always
						 * return TRUE for the second character.
						 */
						*buf++ = K_NUL;
						*buf++ = getch();
						len++;
						break;
					case K_NUL:
						*buf++ = K_NUL;
						*buf++ = 3;
						++len;
						break;
					case 3:
						cbrk_pressed = TRUE;
						/*FALLTHROUGH*/
					default:
						*buf++ = c;
				}
				++len;
			}
		}
	}
#ifdef USE_MOUSE
	show_mouse(FALSE);
#endif
	beep_count = 0;			/* may beep again now that we got some chars */
	return len;
}

/*
 * return non-zero if a character is available
 */
	int
mch_char_avail()
{
	return WaitForChar(0L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
	void
mch_suspend()
{
	MSG_OUTSTR("new shell started\n");
	(void)call_shell(NULL, SHELL_COOKED);
	need_check_timestamps = TRUE;
}

extern int _fmode;
/*
 * we do not use windows, there is not much to do here
 */
	void
mch_windinit()
{
	union REGS regs;

	_fmode = O_BINARY;		/* we do our own CR-LF translation */
	flushbuf();
	(void)mch_get_winsize();

#ifdef USE_MOUSE
/* find out if a MS compatible mouse is available */
	regs.x.ax = 0;
	(void)int86(0x33, &regs, &regs);
	mouse_avail = regs.x.ax;
#endif
}

#ifdef USE_MOUSE
	static void
show_mouse(on)
	int		on;
{
	static int		was_on = FALSE;
	union REGS		regs;

	if (mouse_avail)
	{
		if (!mouse_active || mouse_hidden)
			on = FALSE;
		/*
		 * Careful: Each switch on must be compensated by exactly one switch
		 * off
		 */
		if (on && !was_on || !on && was_on)
		{
			was_on = on;
			regs.x.ax = on ? 1 : 2;
			int86(0x33, &regs, &regs);	/* show mouse */
			if (on)
				mouse_area();
		}
	}
}

/*
 * Set area where mouse can be moved to.
 */
	static void
mouse_area()
{
	union REGS		regs;

	if (mouse_avail)
	{	
		regs.x.cx = 0;	/* mouse visible between cx and dx */
		regs.x.dx = ((unsigned int)Columns - 1) * 8;
		regs.x.ax = 7;
		(void)int86(0x33, &regs, &regs);
		regs.x.cx = 0;	/* mouse visible between cx and dx */
		regs.x.dx = ((unsigned int)Rows - 1) * 8;
		regs.x.ax = 8;
		(void)int86(0x33, &regs, &regs);
	}
}

#endif

	int
mch_check_win(argc, argv)
	int		argc;
	char	**argv;
{
	if (isatty(1))
		return OK;
	return FAIL;
}

	int
mch_check_input()
{
	if (isatty(0))
		return OK;
	return FAIL;
}

#ifdef USE_FNAME_CASE
/*
 * fname_case(): Set the case of the filename, if it already exists.
 */
	void
fname_case(name)
	char_u *name;
{
	char_u			*tail;
	struct ffblk	fb;

	slash_adjust(name);
	if (findfirst(name, &fb, 0) == 0)
	{
		tail = gettail(name);
		if (STRLEN(tail) == STRLEN(fb.ff_name))
			STRCPY(tail, fb.ff_name);
	}
}
#endif

/*
 * mch_settitle(): set titlebar of our window.
 * Dos console has no title.
 */
	void
mch_settitle(title, icon)
	char_u *title;
	char_u *icon;
{
}

/*
 * Restore the window/icon title. (which we don't have)
 */
	void
mch_restore_title(which)
	int which;
{
}

	int
mch_can_restore_title()
{
	return FALSE;
}

	int
mch_can_restore_icon()
{
	return FALSE;
}

/*
 * Insert user name in s[len].
 */
	int
mch_get_user_name(s, len)
	char_u	*s;
	int		len;
{
	*s = NUL;
	return FAIL;
}

/*
 * Insert host name is s[len].
 */
	void
mch_get_host_name(s, len)
	char_u	*s;
	int		len;
{
#ifdef DJGPP
	STRNCPY(s, "PC (32 bits Vim)", len);
#else
	STRNCPY(s, "PC (16 bits Vim)", len);
#endif
}

/*
 * return process ID
 */
	long
mch_get_pid()
{
	return (long)0;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
	int
mch_dirname(buf, len)
	char_u	*buf;
	int		len;
{
	return (getcwd(buf, len) != NULL ? OK : FAIL);
}

/*
 * Change default drive (just like _chdrive of Borland C 3.1)
 */
	static int
change_drive(drive)
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

/*
 * Get absolute filename into buffer 'buf' of length 'len' bytes.
 * All slashes are replaced with backslashes, to avoid trouble when comparing
 * file names.
 *
 * return FAIL for failure, OK otherwise
 */
	int
FullName(fname, buf, len, force)
	char_u	*fname, *buf;
	int		len;
	int		force;
{
	if (fname == NULL)	/* always fail */
	{
		*buf = NUL;
		return FAIL;
	}

	if (!force && isFullName(fname))		/* allready expanded */
	{
		STRNCPY(buf, fname, len);
		slash_adjust(buf);
		return OK;
	}

#ifdef __BORLANDC__		/* the old Turbo C does not have this */
	if (_fullpath(buf, fname, len) == NULL)
	{
		STRNCPY(buf, fname, len);	/* failed, use the relative path name */
		slash_adjust(buf);
		return FAIL;
	}
	slash_adjust(buf);
	return OK;
#else					/* almost the same as FullName in unix.c */
	{
		int		l;
		char_u	olddir[MAXPATHL];
		char_u	*p, *q;
		int		c;
		int		retval = OK;

		*buf = 0;
		/*
		 * change to the directory for a moment,
		 * and then do the getwd() (and get back to where we were).
		 * This will get the correct path name with "../" things.
		 */
		p = vim_strrchr(fname, '/');
		q = vim_strrchr(fname, '\\');
		if (q != NULL && (p == NULL || q > p))
			p = q;
		q = vim_strrchr(fname, ':');
		if (q != NULL && (p == NULL || q > p))
			p = q;
		if (p != NULL)
		{
			if (getcwd(olddir, MAXPATHL) == NULL)
			{
				p = NULL;		/* can't get current dir: don't chdir */
				retval = FAIL;
			}
			else
			{
				q = p + 1;
				c = *q;					/* truncate at start of fname */
				*q = NUL;
#ifdef DJGPP
				STRCPY(buf, fname);
				slash_adjust(buf);		/* needed when fname starts with \ */
				if (vim_chdir(buf))		/* change to the directory */
#else
				if (vim_chdir(fname))	/* change to the directory */
#endif
					retval = FAIL;
				else
					fname = q;
				*q = c;
			}
		}
		if (getcwd(buf, len) == NULL)
		{
			retval = FAIL;
			*buf = NUL;
		}
		/*
		 * Concatenate the file name to the path.
		 */
		l = STRLEN(buf);
		if (l && buf[l - 1] != '/' && buf[l - 1] != '\\')
			strcat(buf, "/");
		if (p)
			vim_chdir(olddir);
		strcat(buf, fname);
		slash_adjust(buf);
		return retval;
	}
#endif
}

/*
 * Replace all slashes by backslashes.
 * This used to be the other way around, but MS-DOS sometimes has problems
 * with slashes (e.g. in a command name).  We can't have mixed slashes and
 * backslashes, because comparing file names will not work correctly.  The
 * commands that use a file name should try to avoid the need to type a
 * backslash twice.
 */
	static void
slash_adjust(p)
	char_u	*p;
{
#ifdef DJGPP
	/* DJGPP can't handle a file name that starts with a backslash, and when it
	 * starts with a slash there should be no backslashes */
	if (*p == '\\' || *p == '/')
		while (*p)
		{
			if (*p == '\\')
				*p = '/';
			++p;
		}
	else
#endif
	while (*p)
	{
		if (*p == '/')
			*p = '\\';
		++p;
	}
}

/*
 * return TRUE is fname is an absolute path name
 */
	int
isFullName(fname)
	char_u		*fname;
{
	return (vim_strchr(fname, ':') != NULL);
}

/*
 * get file permissions for 'name'
 * -1 : error
 * else FA_attributes defined in dos.h
 */
	long
getperm(name)
	char_u *name;
{
	int r;

	r = _chmod(name, 0, 0);		 /* get file mode */
	return r;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
	int
setperm(name, perm)
	char_u	*name;
	long	perm;
{
	perm |= FA_ARCH;		/* file has changed, set archive bit */
	return (_chmod((char *)name, 1, (int)perm) == -1 ? FAIL : OK);
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 *
 * beware of a trailing backslash
 */
	int
mch_isdir(name)
	char_u *name;
{
	int		f;
	char_u	*p;

	p = name + strlen(name);
	if (p > name)
		--p;
	if (*p == '\\')					/* remove trailing backslash for a moment */
		*p = NUL;
	else
		p = NULL;
	f = _chmod(name, 0, 0);
	if (p != NULL)
		*p = '\\';					/* put back backslash */
	if (f == -1)
		return FALSE;				/* file does not exist at all */
	if ((f & FA_DIREC) == 0)
		return FALSE;				/* not a directory */
	return TRUE;
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
	ml_close_all(TRUE);				/* remove all memfiles */
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
 * because we may be called by DOS. The redraw is in mch_inchar().
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
#ifndef DJGPP
	static void interrupt (*old_cint)();
#endif
	static void interrupt (*old_cbrk)();

	if (raw)
	{
		saved_cbrk = getcbrk();			/* save old ctrl-break setting */
		setcbrk(0);						/* do not check for ctrl-break */
#ifdef DJGPP
		old_cbrk = signal(SIGINT,catch_cbrk);	/* critical error interrupt */
#else
		old_cint = getvect(0x24); 		/* save old critical error interrupt */
		setvect(0x24, catch_cint);		/* install our critical error interrupt */
		old_cbrk = getvect(0x1B); 		/* save old ctrl-break interrupt */
		setvect(0x1B, catch_cbrk);		/* install our ctrl-break interrupt */
		ctrlbrk(cbrk_handler);			/* vim's ctrl-break handler */
#endif
		if (term_console)
			outstr(T_ME);				/* set colors */
	}
	else
	{
		setcbrk(saved_cbrk);			/* restore ctrl-break setting */
#ifdef DJGPP
		signal(SIGINT,old_cbrk);		/* critical error interrupt */
#else
		setvect(0x24, old_cint);		/* restore critical error interrupt */
		setvect(0x1B, old_cbrk);		/* restore ctrl-break interrupt */
#endif
		/* restore ctrl-break handler, how ??? */
		if (term_console)
			normvideo();				/* restore screen colors */
	}
}

#ifdef USE_MOUSE
	void
mch_setmouse(on)
	int		on;
{
	mouse_active = on;
	mouse_hidden = TRUE;		/* dont show it until moved */
}
#endif

/*
 * set screen mode
 * return FAIL for failure, OK otherwise
 */
	int
mch_screenmode(arg)
	char_u		*arg;
{
	int				mode;
	int				i;
	static char_u *(names[]) = {"BW40", "C40", "BW80", "C80", "MONO", "C4350"};
	static int		modes[]  = { BW40,   C40,   BW80,   C80,   MONO,   C4350};

	mode = -1;
	if (isdigit(*arg))				/* mode number given */
		mode = atoi((char *)arg);
	else
	{
		for (i = 0; i < sizeof(names) / sizeof(char_u *); ++i)
			if (stricmp((char *)names[i], (char *)arg) == 0)
			{
				mode = modes[i];
				break;
			}
	}
	if (mode == -1)
	{
		EMSG("Unsupported screen mode");
		return FAIL;
	}
	textmode(mode);					/* use Borland function */
	return OK;
}

/*
 * Structure used by Turbo-C/Borland-C to store video parameters.
 */
#ifndef DJGPP
extern struct text_info _video;
#endif

/*
 * try to get the real window size
 * return FAIL for failure, OK otherwise
 */
	int
mch_get_winsize()
{
	int i;
	struct text_info ti;
/*
 * The screenwidth is returned by the BIOS OK.
 * The screenheight is in a location in the bios RAM, if the display is EGA or
 * VGA.
 */
	if (!term_console)
		return FAIL;
	gettextinfo(&ti);
	Columns = ti.screenwidth;
	Rows = ti.screenheight;
#ifndef DJGPP
	if (ti.currmode > 10)
		Rows = *(char far *)MK_FP(0x40, 0x84) + 1;
#endif
	/*
	 * don't call set_window() when not doing full screen, since it will move
	 * the cursor.  Also skip this when exiting.
	 */
	if (full_screen && !exiting)
		set_window();

	if (Columns < MIN_COLUMNS || Rows < MIN_ROWS + 1)
	{
		/* these values are overwritten by termcap size or default */
		Columns = 80;
		Rows = 25;
		return FAIL;
	}
	check_winsize();

	return OK;
}

/*
 * Set the active window for delline/insline.
 */
	void
set_window()
{
#ifndef DJGPP
	_video.screenheight = Rows;
#endif
	window(1, 1, Columns, Rows);
	screen_start();
}

	void
mch_set_winsize()
{
	/* should try to set the window size to Rows and Columns */
	/* may involve switching display mode.... */

#ifdef USE_MOUSE
	mouse_area();			/* set area where mouse can go */
#endif
}

/*
 * call shell, return FAIL for failure, OK otherwise
 */
	int
call_shell(cmd, options)
	char_u	*cmd;
	int		options;		/* SHELL_FILTER if called by do_filter() */
							/* SHELL_COOKED if term needs cooked mode */
							/* SHELL_EXPAND if called by ExpandWildCards() */
{
	int		x;
	char_u	*newcmd;

	flushbuf();

	if (options & SHELL_COOKED)
		settmode(0);		/* set to cooked mode */

	if (cmd == NULL)
		x = system(p_sh);
	else
	{ 				/* we use "command" to start the shell, slow but easy */
		newcmd = alloc(STRLEN(p_sh) + STRLEN(cmd) + 5);
		if (newcmd == NULL)
			x = 1;
		else
		{
			sprintf(newcmd, "%s /c %s", p_sh, cmd);
			x = system(newcmd);
			vim_free(newcmd);
		}
	}
	settmode(1);			/* set to raw mode */

	if (x && !expand_interactively)
	{
		msg_outchar('\n');
		msg_outnum((long)x);
		MSG_OUTSTR(" returned\n");
	}

	/* resettitle();				we don't have titles */
	(void)mch_get_winsize();		/* display mode may have been changed */
	return (x ? FAIL : OK);
}

/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
	void
mch_breakcheck()
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
	char_u		*f;
	int			isdir;
{
	char_u		*p;

	if (!fl->file)
	{
		fl->file = (char_u **)alloc(sizeof(char_u *) * FL_CHUNK);
		if (!fl->file)
			return;
		fl->nfiles = 0;
		fl->maxfiles = FL_CHUNK;
	}
	if (fl->nfiles >= fl->maxfiles)
	{
		char_u	**t;
		int		i;

		t = (char_u **)lalloc((long_u)(sizeof(char_u *) * (fl->maxfiles + FL_CHUNK)), TRUE);
		if (!t)
			return;
		for (i = fl->nfiles - 1; i >= 0; i--)
			t[i] = fl->file[i];
		vim_free(fl->file);
		fl->file = t;
		fl->maxfiles += FL_CHUNK;
	}
	p = alloc((unsigned)(STRLEN(f) + 1 + isdir));
	if (p)
	{
		STRCPY(p, f);
		slash_adjust(p);
		/*
		 * Append a backslash after directory names.
		 */
		if (isdir)
			strcat(p, "\\");
	}
	fl->file[fl->nfiles++] = p;
}

	static int
pstrcmp(a, b)
	char_u **a, **b;
{
	return (strcmp(*a, *b));
}

	int
mch_has_wildcard(s)
	char_u *s;
{
	for ( ; *s; ++s)
		if (*s == '?' || *s == '*')
			return TRUE;
	return FALSE;
}

	static void
strlowcpy(d, s)
	char_u *d, *s;
{
#ifdef DJGPP
	if (USE_LONG_FNAME)		/* don't lower case on Windows 95/NT systems */
		while (*s)
			*d++ = *s++;
	else
#endif
		while (*s)
			*d++ = tolower(*s++);
	*d = NUL;
}

	static int
expandpath(fl, path, fonly, donly, notf)
	FileList	*fl;
	char_u		*path;
	int			fonly, donly, notf;
{
	char_u	*buf;
	char_u	*p, *s, *e;
	int		lastn, c, retval;
	struct ffblk fb;

	lastn = fl->nfiles;
	buf = alloc(STRLEN(path) + BASENAMELEN + 5);	/* make room for file name */
	if (buf == NULL)
		return 1;

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

	/* if the file name ends in "*" and does not contain a ".", addd ".*" */
	if (e[-1] == '*' && vim_strchr(s, '.') == NULL)
	{
		*e++ = '.';
		*e++ = '*';
	}
	/* now we have one wildcard component between s and e */
	*e = NUL;
	retval = 0;
	/* If we are expanding wildcards we try both files and directories */
	if ((c = findfirst(buf, &fb, (*path || !notf) ? FA_DIREC : 0)) != 0)
	{
		/* not found */
		STRCPY(e, path);
		if (notf)
			addfile(fl, buf, FALSE);
		vim_free(buf);
		return 1; /* unexpanded or empty */
	}
	while (!c)
	{
		strlowcpy(s, fb.ff_name);	/* may expand "*" to "12345678.123" */
			/* ignore "." and ".." */
		if (*s != '.' || (s[1] != NUL && (s[1] != '.' || s[2] != NUL)))
		{
			strcat(buf, path);
			if (!mch_has_wildcard(path))
				addfile(fl, buf, mch_isdir(buf));
			else
				retval |= expandpath(fl, buf, fonly, donly, notf);
		}
		c = findnext(&fb);
	}
	vim_free(buf);
	qsort(fl->file + lastn, fl->nfiles - lastn, sizeof(char_u *), pstrcmp);
	return retval;
}

/*
 * MSDOS rebuilt of Scott Ballantynes ExpandWildCards for amiga/arp.
 * jw
 */

	int
ExpandWildCards(num_pat, pat, num_file, file, files_only, list_notfound)
	int 	num_pat;
	char_u	**pat;
	int 	*num_file;
	char_u	***file;
	int 	files_only, list_notfound;
{
	int			i, retval = 0;
	FileList	f;

	f.file = NULL;
	f.nfiles = 0;

	for (i = 0; i < num_pat; i++)
	{
		if (!mch_has_wildcard(pat[i]))
			addfile(&f, pat[i], files_only ? FALSE : mch_isdir(pat[i]));
		else
			retval |= expandpath(&f, pat[i], files_only, 0, list_notfound);
	}

	*num_file = f.nfiles;
	*file = (*num_file > 0) ? f.file : (char_u **)"";

	return (*num_file > 0) ? OK : FAIL;
}

#ifdef USE_VIM_CHDIR
/*
 * The normal chdir() does not change the default drive.
 * This one does.
 */
	int
vim_chdir(path)
	char *path;
{
	if (path[0] == NUL)				/* just checking... */
		return 0;
	if (path[1] == ':')				/* has a drive name */
	{
		if (change_drive(toupper(path[0]) - 'A' + 1))
			return -1;				/* invalid drive name */
		path += 2;
	}
	if (*path == NUL)				/* drive name only */
		return 0;
	return chdir(path);				/* let the normal chdir() do the rest */
}
#endif

#ifdef DJGPP
/*
 * djgpp_rename() works around a bug in rename (aka MoveFile) in
 * Windows 95: rename("foo.bar", "foo.bar~") will generate a
 * file whose shortfilename is "FOO.BAR" (its longfilename will
 * be correct: "foo.bar~").  Because a file can be accessed by
 * either its SFN or its LFN, "foo.bar" has effectively been
 * renamed to "foo.bar", which is not at all what was wanted.  This
 * seems to happen only when renaming files with three-character
 * extensions by appending a suffix that does not include ".".
 * Windows NT gets it right, however, with an SFN of "FOO~1.BAR".
 * This works like win95rename in win32.c, but is a bit simpler.
 *
 * Like rename(), returns 0 upon success, non-zero upon failure.
 * Should probably set errno appropriately when errors occur.
 */

#undef rename

	int
djgpp_rename(const char *OldFile, const char *NewFile)
{
	char_u	*TempFile;
	int		retval;
	int		fd;

	/* rename() works correctly without long file names, so use that */
	if (!_use_lfn())
		return rename(OldFile, NewFile);

	if ((TempFile = alloc((unsigned)(STRLEN(OldFile) + TMPNAMELEN))) == NULL)
		return -1;
	
	STRCPY(TempFile, OldFile);
	STRCPY(gettail(TempFile), TMPNAME1);
	if (rename(OldFile, TempFile))
		retval = -1;
	else
	{
		/* now create an empty file called OldFile; this prevents
		 * the operating system using OldFile as an alias (SFN)
		 * if we're renaming within the same directory.  For example,
		 * we're editing a file called filename.asc.txt by its SFN,
		 * filena~1.txt.  If we rename filena~1.txt to filena~1.txt~
		 * (i.e., we're making a backup while writing it), the SFN
		 * for filena~1.txt~ will be filena~1.txt, by default, which
		 * will cause all sorts of problems later in buf_write.  So, we
		 * create an empty file called filena~1.txt and the system will have
		 * to find some other SFN for filena~1.txt~, such as filena~2.txt
		 */
		if ((fd = open(OldFile, O_RDWR|O_CREAT|O_EXCL, 0444)) < 0)
			return -1;
		retval = rename(TempFile, NewFile);
		close(fd);
		vim_remove((char_u *)OldFile);
	}
	vim_free(TempFile);

	return retval;	/* success */
}
#endif

/*
 * Special version of getenv(): use $HOME when $VIM not defined.
 */
	char_u *
vim_getenv(var)
	char_u *var;
{
	char_u	*retval;

	retval = (char_u *)getenv((char *)var);

	if (retval == NULL && STRCMP(var, "VIM") == 0)
		retval = (char_u *)getenv("HOME");

	return retval;
}
