/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_msdos.c
 *
 * MSDOS system-dependent routines.
 * A cheap plastic imitation of the amiga dependent code.
 * A lot in this file was made by Juergen Weigert (jw).
 *
 * DJGPP changes by Gert van Antwerpen
 */

#include <io.h>
#include "vim.h"

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

#if defined(DJGPP) || defined(PROTO)
# define _cdecl	    /* DJGPP doesn't have this */
#endif

static int WaitForChar __ARGS((long));
#ifdef USE_MOUSE
static void show_mouse __ARGS((int));
static void mouse_area __ARGS((void));
#endif
static int change_drive __ARGS((int));
static _cdecl int cbrk_handler __ARGS(());
static void set_interrupts __ARGS((int on));

static _cdecl int	pstrcmp();  /* __ARGS((char **, char **)); BCC does not like this */
static void	namelowcpy __ARGS((char_u *, char_u *));

static int cbrk_pressed = FALSE;    /* set by ctrl-break interrupt */
static int ctrlc_pressed = FALSE;   /* set when ctrl-C or ctrl-break detected */
static int delayed_redraw = FALSE;  /* set when ctrl-C detected */

#ifdef USE_MOUSE
static int mouse_avail = FALSE;	    /* mouse present */
static int mouse_active;	    /* mouse enabled */
static int mouse_hidden;	    /* mouse not shown */
static int mouse_click = -1;	    /* mouse status */
static int mouse_last_click = -1;   /* previous status at click */
static int mouse_x = -1;	    /* mouse x coodinate */
static int mouse_y = -1;	    /* mouse y coodinate */
static long mouse_click_time = 0;   /* biostime() of last click */
static int mouse_click_count = 0;   /* count for multi-clicks */
static int mouse_click_x = 0;	    /* x of previous mouse click */
static int mouse_click_y = 0;	    /* y of previous mouse click */
static linenr_t mouse_topline = 0;  /* topline at previous mouse click */
static int mouse_x_div = 8;	    /* column = x coord / mouse_x_div */
static int mouse_y_div = 8;	    /* line   = y coord / mouse_y_div */
#endif

#define BIOSTICK    55		    /* biostime() increases one tick about
					every 55 msec */

    long_u
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
 * don't do anything for about "msec" msec
 */
    void
mch_delay(msec, ignoreinput)
    long    msec;
    int	    ignoreinput;
{
    long    starttime;

    if (ignoreinput)
    {
	/*
	 * We busy-wait here.  Unfortunately, delay() and usleep() have been
	 * reported to give problems with the original Windows 95.  This is
	 * fixed in service pack 1, but not everybody installed that.
	 */
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
    char_u  *name;
{
    (void)mch_setperm(name, 0);    /* default permissions */
    return unlink((char *)name);
}

/*
 * mch_write(): write the output buffer to the screen
 */
    void
mch_write(s, len)
    char_u  *s;
    int	    len;
{
    char_u	*p;
    int		row, col;

    if (term_console)	    /* translate ESC | sequences into bios calls */
	while (len--)
	{
	    if (p_wd)	    /* testing: wait a bit for each char */
		WaitForChar(p_wd);

	    if (s[0] == '\n')
		putch('\r');
	    else if (s[0] == ESC && len > 1 && s[1] == '|')
	    {
		switch (s[2])
		{
#ifdef DJGPP
		case 'B':   ScreenVisualBell();
			    goto got3;
#endif
		case 'J':   clrscr();
			    goto got3;

		case 'K':   clreol();
			    goto got3;

		case 'L':   insline();
			    goto got3;

		case 'M':   delline();
got3:			    s += 3;
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
		case '9':   p = s + 2;
			    row = getdigits(&p);    /* no check for length! */
			    if (p > s + len)
				break;
			    if (*p == ';')
			    {
				++p;
				col = getdigits(&p); /* no check for length! */
				if (p > s + len)
				    break;
				if (*p == 'H' || *p == 'r')
				{
				    if (*p == 'H')  /* set cursor position */
					gotoxy(col, row);
				    else	    /* set scroll region  */
					window(1, row, Columns, col);
				    len -= p - s;
				    s = p + 1;
				    continue;
				}
			    }
			    else if (*p == 'm' || *p == 'f' || *p == 'b')
			    {
				if (*p == 'm')	    /* set color */
				{
				    if (row == 0)
					normvideo();/* reset color */
				    else
					textattr(row);
				}
				else if (*p == 'f') /* set foreground color */
				    textcolor(row);
				else		    /* set background color */
				    textbackground(row);

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

#ifdef DJGPP
/*
 * DJGPP provides a kbhit() function that goes to the BIOS instead of DOS.
 * This doesn't work for terminals connected to a serial port.
 * Redefine kbhit() here to make it work.
 */
    static int
vim_kbhit(void)
{
    union REGS regs;

    regs.h.ah = 0x0b;
    (void)intdos(&regs, &regs);
    return regs.h.al;
}

#ifdef kbhit
# undef kbhit	    /* might have been defined in conio.h */
#endif

#define kbhit()	vim_kbhit()
#endif

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

    static  int
WaitForChar(msec)
    long    msec;
{
    union REGS	regs;
    long	starttime;
    int		x, y;

    starttime = biostime(0, 0L);

    for (;;)
    {
#ifdef USE_MOUSE
	long		clicktime;
	static int	last_status = 0;

	if (mouse_avail && mouse_active && mouse_click < 0)
	{
	    regs.x.ax = 3;
	    int86(0x33, &regs, &regs);	    /* check mouse status */
		/* only recognize button-down and button-up event */
	    x = regs.x.cx / mouse_x_div;
	    y = regs.x.dx / mouse_y_div;
	    if ((last_status == 0) != (regs.x.bx == 0))
	    {
		if (last_status)	/* button up */
		    mouse_click = MOUSE_RELEASE;
		else			/* button down */
		{
		    /*
		     * Translate MSDOS mouse events to Vim mouse events.
		     * TODO: should handle middle mouse button, by pressing
		     * left and right at the same time.
		     */
		    if (regs.x.bx & MSDOS_MOUSE_LEFT)
			mouse_click = MOUSE_LEFT;
		    else if (regs.x.bx & MSDOS_MOUSE_RIGHT)
			mouse_click = MOUSE_RIGHT;
		    else if (regs.x.bx & MSDOS_MOUSE_MIDDLE)
			mouse_click = MOUSE_MIDDLE;

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
						    || mouse_click >= 0
#endif
		)
	    return TRUE;
	/*
	 * Use biostime() to wait until our time is done.
	 * We busy-wait here.  Unfortunately, delay() and usleep() have been
	 * reported to give problems with the original Windows 95.  This is
	 * fixed in service pack 1, but not everybody installed that.
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
    char_u	*buf;
    int		maxlen;
    long	time;
{
    int		len = 0;
    int		c;
    static int	nextchar = 0;	    /* may keep character when maxlen == 1 */

/*
 * if we got a ctrl-C when we were busy, there will be a "^C" somewhere
 * on the sceen, so we need to redisplay it.
 */
    if (delayed_redraw)
    {
	delayed_redraw = FALSE;
	update_screen(CLEAR);
	setcursor();
	out_flush();
    }

    /* return remaining character from last call */
    if (nextchar)
    {
	*buf = nextchar;
	nextchar = 0;
	return 1;
    }

#ifdef USE_MOUSE
    if (time != 0)
	show_mouse(TRUE);
#endif
    if (time >= 0)
    {
	if (WaitForChar(time) == 0)	/* no character available */
	{
#ifdef USE_MOUSE
	    show_mouse(FALSE);
#endif
	    return 0;
	}
    }
    else    /* time == -1 */
    {
    /*
     * If there is no character available within 2 seconds (default)
     * write the autoscript file to disk
     */
	if (WaitForChar(p_ut) == 0)
	    updatescript(0);
    }
    WaitForChar(FOREVER);	/* wait for key or mouse click */

/*
 * Try to read as many characters as there are, until the buffer is full.
 */
    /*
     * we will get at least one key. Get more if they are available
     * After a ctrl-break we have to read a 0 (!) from the buffer.
     * bioskey(1) will return 0 if no key is available and when a
     * ctrl-break was typed. When ctrl-break is hit, this does not always
     * implies a key hit.
     */
    cbrk_pressed = FALSE;
#ifdef USE_MOUSE
    if (mouse_click >= 0 && maxlen >= 5)
    {
	len = 5;
	*buf++ = ESC + 128;
	*buf++ = 'M';
	*buf++ = mouse_click;
	*buf++ = mouse_x + '!';
	*buf++ = mouse_y + '!';
	mouse_click = -1;
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
		c = bioskey(0);		/* get the key */
		/*
		 * translate a few things for inchar():
		 * 0x0000 == CTRL-break		-> 3	(CTRL-C)
		 * 0x0300 == CTRL-@		-> NUL
		 * 0xnn00 == extended key code	-> K_NUL, nn
		 * K_NUL			-> K_NUL, 3
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

		if (len < maxlen)
		{
		    *buf++ = c;
		    len++;
		}
		else
		    nextchar = c;
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
			++len;
			c = getch();
			break;
		    case K_NUL:
			*buf++ = K_NUL;
			++len;
			c = 3;
			break;
		    case 3:
			cbrk_pressed = TRUE;
			/*FALLTHROUGH*/
		    default:
			break;
		}
		if (len < maxlen)
		{
		    *buf++ = c;
		    ++len;
		}
		else
		    nextchar = c;
	    }
	}
    }
#ifdef USE_MOUSE
    show_mouse(FALSE);
#endif

    beep_count = 0;	    /* may beep again now that we got some chars */
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
    suspend_shell();
}

extern int _fmode;

/*
 * Prepare window for use by Vim.
 * we do not use windows, there is not much to do here
 */
    void
mch_windinit()
{
    union REGS regs;

    term_console = TRUE;    /* assume using the console for the things here */
    _fmode = O_BINARY;	    /* we do our own CR-LF translation */
    out_flush();
    set_interrupts(TRUE);   /* catch interrupts */

#ifdef DJGPP
    /*
     * Use Long File Names by default, if $LFN not set.
     */
    if (getenv("LFN") == NULL)
	putenv("LFN=y");
    /*
     * Always use the shell for system() when redirecting.
     */
    __system_flags &= ~__system_redirect;
#endif

#ifdef USE_MOUSE
/* find out if a MS compatible mouse is available */
    regs.x.ax = 0;
    (void)int86(0x33, &regs, &regs);
    mouse_avail = regs.x.ax;
    /* best guess for mouse coordinate computations */
    mch_get_winsize();
    if (Columns <= 40)
	mouse_x_div = 16;
    if (Rows == 30)
	mouse_y_div = 16;
#endif

    /*
     * Try switching to 16 colors for background, instead of 8 colors and
     * blinking.  Does this always work?  Can the old value be restored?
     */
    regs.x.ax = 0x1003;
    regs.h.bl = 0x00;
    regs.h.bh = 0x00;
    int86(0x10, &regs, &regs);
}

#ifdef USE_MOUSE
    static void
show_mouse(on)
    int	    on;
{
    static int	    was_on = FALSE;
    union REGS	    regs;

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
 * Set area where mouse can be moved to: The whole screen.
 * Rows must be valid when calling!
 */
    static void
mouse_area()
{
    union REGS	    regs;
    int		    mouse_y_max;	    /* maximum mouse y coord */

    if (mouse_avail)
    {
	mouse_y_max = Rows * mouse_y_div - 1;
	if (mouse_y_max < 199)	    /* is this needed? */
	    mouse_y_max = 199;
	regs.x.cx = 0;	/* mouse visible between cx and dx */
	regs.x.dx = 639;
	regs.x.ax = 7;
	(void)int86(0x33, &regs, &regs);
	regs.x.cx = 0;	/* mouse visible between cx and dx */
	regs.x.dx = mouse_y_max;
	regs.x.ax = 8;
	(void)int86(0x33, &regs, &regs);
    }
}

#endif

    int
mch_check_win(argc, argv)
    int	    argc;
    char    **argv;
{
    /* store argv[0], may be used for $VIM */
    if (*argv[0] != NUL)
	exe_name = FullName_save((char_u *)argv[0], FALSE);

    if (isatty(1))
	return OK;
    return FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    int
mch_input_isatty()
{
    if (isatty(read_cmd_fd))
	return TRUE;
    return FALSE;
}

#ifdef USE_FNAME_CASE
/*
 * fname_case(): Set the case of the file name, if it already exists.
 */
    void
fname_case(name)
    char_u *name;
{
    char_u	    *tail;
    struct ffblk    fb;

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
    char_u  *s;
    int	    len;
{
    *s = NUL;
    return FAIL;
}

/*
 * Insert host name is s[len].
 */
    void
mch_get_host_name(s, len)
    char_u  *s;
    int	    len;
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
    char_u  *buf;
    int	    len;
{
    return (getcwd((char *)buf, len) != NULL ? OK : FAIL);
}

/*
 * Change default drive (just like _chdrive of Borland C 3.1)
 */
    static int
change_drive(drive)
    int drive;
{
    union REGS regs;

    regs.h.ah = 0x0e;
    regs.h.dl = drive - 1;
    intdos(&regs, &regs);   /* set default drive */
    regs.h.ah = 0x19;
    intdos(&regs, &regs);   /* get default drive */
    if (regs.h.al == drive - 1)
	return 0;
    else
	return -1;
}

/*
 * Get absolute file name into buffer 'buf' of length 'len' bytes.
 * All slashes are replaced with backslashes, to avoid trouble when comparing
 * file names.
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_FullName(fname, buf, len, force)
    char_u  *fname, *buf;
    int	    len;
    int	    force;
{
    if (fname == NULL)	/* always fail */
    {
	*buf = NUL;
	return FAIL;
    }

    if (!force && mch_isFullName(fname))	/* allready expanded */
    {
	STRNCPY(buf, fname, len);
	slash_adjust(buf);
	return OK;
    }

#ifdef __BORLANDC__	/* the old Turbo C does not have this */
    if (_fullpath((char *)buf, (char *)fname, len) == NULL)
    {
	STRNCPY(buf, fname, len);   /* failed, use the relative path name */
	slash_adjust(buf);
	return FAIL;
    }
    slash_adjust(buf);
    return OK;
#else			/* almost the same as mch_FullName in os_unix.c */
    {
	int	l;
	char_u	olddir[MAXPATHL];
	char_u	*p, *q;
	int	c;
	int	retval = OK;

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
		p = NULL;	/* can't get current dir: don't chdir */
		retval = FAIL;
	    }
	    else
	    {
		q = p + 1;
		c = *q;			/* truncate at start of fname */
		*q = NUL;
#ifdef DJGPP
		STRCPY(buf, fname);
		slash_adjust(buf);	/* needed when fname starts with \ */
		if (vim_chdir(buf))	/* change to the directory */
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
    void
slash_adjust(p)
    char_u  *p;
{
#ifdef OLD_DJGPP    /* this seems to have been fixed in DJGPP 2.01 */
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
mch_isFullName(fname)
    char_u	*fname;
{
    return (vim_strchr(fname, ':') != NULL);
}

/*
 * get file permissions for 'name'
 * -1 : error
 * else FA_attributes defined in dos.h
 */
    long
mch_getperm(name)
    char_u *name;
{
    return (long)_chmod((char *)name, 0, 0);	 /* get file mode */
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_setperm(name, perm)
    char_u	*name;
    long	perm;
{
    perm |= FA_ARCH;	    /* file has changed, set archive bit */
    return (_chmod((char *)name, 1, (int)perm) == -1 ? FAIL : OK);
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(name)
    char_u	*name;
{
    /* DOS 6.2 share.exe causes "seek error on file write" errors when making
     * the swap file hidden.  Thus don't do it. */
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
    int	    f;
    char_u  *p;

    p = name + strlen((char *)name);
    if (p > name)
	--p;
    if (*p == '\\')		    /* remove trailing backslash for a moment */
	*p = NUL;
    else
	p = NULL;
    f = _chmod((char *)name, 0, 0);
    if (p != NULL)
	*p = '\\';		    /* put back backslash */
    if (f == -1)
	return FALSE;		    /* file does not exist at all */
    if ((f & FA_DIREC) == 0)
	return FALSE;		    /* not a directory */
    return TRUE;
}

/*
 * Careful: mch_windexit() may be called before mch_windinit()!
 */
    void
mch_windexit(r)
    int r;
{
    settmode(TMODE_COOK);
    stoptermcap();
    set_interrupts(FALSE);	    /* restore interrupts */
    out_char('\r');
    out_char('\n');
    out_flush();
    ml_close_all(TRUE);		    /* remove all memfiles */
    exit(r);
}

#ifdef DJGPP
# define INT_ARG    int
#else
# define INT_ARG
#endif

/*
 * function for ctrl-break interrupt
 */
    static void interrupt
#ifdef DJGPP
catch_cbrk(int a)
#else
catch_cbrk()
#endif
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
	_cdecl
cbrk_handler()
{
    delayed_redraw = TRUE;
    return 1;		    /* resume operation after ctrl-break */
}

/*
 * function for critical error interrupt
 * For DOS 1 and 2 return 0 (Ignore).
 * For DOS 3 and later return 3 (Fail)
 */
    static void interrupt
catch_cint(bp, di, si, ds, es, dx, cx, bx, ax)
    unsigned bp, di, si, ds, es, dx, cx, bx, ax;
{
    ax = (ax & 0xff00);	    /* set AL to 0 */
    if (_osmajor >= 3)
	ax |= 3;	    /* set AL to 3 */
}

/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 * Does not change the tty, as bioskey() and kbhit() work raw all the time.
 */
    void
mch_settmode(tmode)
    int  tmode;
{
}

/*
 * Set the interrupt vectors for use with Vim on or off.
 */
    static void
set_interrupts(on)
    int	    on;				/* TRUE means as used within Vim */
{
    static int saved_cbrk;
#ifndef DJGPP
    static void interrupt (*old_cint)();
#endif
    static void interrupt (*old_cbrk)(INT_ARG);

    if (on)
    {
	saved_cbrk = getcbrk();		/* save old ctrl-break setting */
	setcbrk(0);			/* do not check for ctrl-break */
#ifdef DJGPP
	old_cbrk = signal(SIGINT, catch_cbrk);	/* critical error interrupt */
#else
	old_cint = getvect(0x24);	/* save old critical error interrupt */
	setvect(0x24, catch_cint);	/* install our critical error interrupt */
	old_cbrk = getvect(0x1B);	/* save old ctrl-break interrupt */
	setvect(0x1B, catch_cbrk);	/* install our ctrl-break interrupt */
	ctrlbrk(cbrk_handler);		/* vim's ctrl-break handler */
#endif
	if (term_console)
	    out_str(T_ME);		/* set colors */
    }
    else
    {
	setcbrk(saved_cbrk);		/* restore ctrl-break setting */
#ifdef DJGPP
	signal(SIGINT,old_cbrk);	/* critical error interrupt */
#else
	setvect(0x24, old_cint);	/* restore critical error interrupt */
	setvect(0x1B, old_cbrk);	/* restore ctrl-break interrupt */
#endif
	/* restore ctrl-break handler, how ??? */
	if (term_console)
	    normvideo();		/* restore screen colors */
    }
}

#ifdef USE_MOUSE
    void
mch_setmouse(on)
    int	    on;
{
    mouse_active = on;
    mouse_hidden = TRUE;	/* dont show it until moved */
}
#endif

/*
 * set screen mode
 * return FAIL for failure, OK otherwise
 */
    int
mch_screenmode(arg)
    char_u	*arg;
{
    int		    mode;
    int		    i;
    static char	   *(names[]) = {"BW40", "C40", "BW80", "C80", "MONO", "C4350"};
    static int	    modes[]  = { BW40,	 C40,	BW80,	C80,   MONO,   C4350};

    mode = -1;
    if (isdigit(*arg))		    /* mode number given */
	mode = atoi((char *)arg);
    else
    {
	for (i = 0; i < sizeof(names) / sizeof(char_u *); ++i)
	    if (stricmp(names[i], (char *)arg) == 0)
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
    textmode(mode);		    /* use Borland function */

    /* Screen colors may have changed. */
    out_str(T_ME);
#ifdef USE_MOUSE
    if (mode <= 1 || mode == 4 || mode == 5 || mode == 13 || mode == 0x13)
	mouse_x_div = 16;
    else
	mouse_x_div = 8;
    if (mode == 0x11 || mode == 0x12)
	mouse_y_div = 16;
    else if (mode == 0x10)
	mouse_y_div = 14;
    else
	mouse_y_div = 8;
    ui_get_winsize();	    /* Rows is used in mouse_area() */
    mouse_area();	    /* set area where mouse can go */
#endif
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
    mouse_area();	    /* set area where mouse can go */
#endif
}

/*
 * call shell, return FAIL for failure, OK otherwise
 */
    int
mch_call_shell(cmd, options)
    char_u  *cmd;
    int	    options;	    /* SHELL_FILTER if called by do_filter() */
			    /* SHELL_COOKED if term needs cooked mode */
{
    int	    x;
    char_u  *newcmd;

    out_flush();

    if (options & SHELL_COOKED)
	settmode(TMODE_COOK);	/* set to normal mode */
    set_interrupts(FALSE);	    /* restore interrupts */

#ifdef DJGPP
    /* ignore signals while external command is running */
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
#endif
    if (cmd == NULL)
	x = system((char *)p_sh);
    else
    {
#ifdef DJGPP
	/* DJGPP uses the value of $SHELL, use that to execute the "cmd" */
	setenv("SHELL", p_sh, 1);
	x = system(cmd);
#else
	/* we use "command" to start the shell, slow but easy */
	newcmd = alloc(STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 3);
	if (newcmd == NULL)
	    x = 1;
	else
	{
	    sprintf((char *)newcmd, "%s %s %s", p_sh, p_shcf, cmd);
	    x = system((char *)newcmd);
	    vim_free(newcmd);
	}
#endif
    }
#ifdef DJGPP
    signal(SIGINT, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
#endif
    settmode(TMODE_RAW);	/* set to raw mode */
    set_interrupts(TRUE);	/* catch interrupts */

    if (x && !expand_interactively)
    {
	msg_putchar('\n');
	msg_outnum((long)x);
	MSG_PUTS(" returned\n");
    }

    /* resettitle();		    we don't have titles */
    (void)ui_get_winsize();	    /* display mode may have been changed */
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

    static int
	_cdecl
pstrcmp(a, b)
    char_u **a, **b;
{
    return (stricmp((char *)*a, (char *)*b));
}

    int
mch_has_wildcard(s)
    char_u *s;
{
    return (vim_strpbrk(s, (char_u *)"?*$~") != NULL);
}

    static void
namelowcpy(d, s)
    char_u *d, *s;
{
#ifdef DJGPP
    if (USE_LONG_FNAME)	    /* don't lower case on Windows 95/NT systems */
	while (*s)
	    *d++ = *s++;
    else
#endif
	while (*s)
	    *d++ = TO_LOWER(*s++);
    *d = NUL;
}

/*
 * Recursive function to expand one path section with wildcards.
 * Return the number of matches found.
 */
    int
mch_expandpath(gap, path, flags)
    struct growarray	*gap;
    char_u		*path;
    int			flags;
{
    char_u	    *buf;
    char_u	    *p, *s, *e;
    int		    start_len, c;
    struct ffblk    fb;
    int		    matches;

    start_len = gap->ga_len;
    buf = alloc(STRLEN(path) + BASENAMELEN + 5);   /* make room for file name */
    if (buf == NULL)
	return 0;

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

    /* If we are expanding wildcards we try both files and directories */
    if ((c = findfirst((char *)buf, &fb,
			    (*path || (flags & EW_DIR)) ? FA_DIREC : 0)) != 0)
    {
	/* not found */
	vim_free(buf);
	return 0; /* unexpanded or empty */
    }

    while (!c)
    {
	namelowcpy((char *)s, fb.ff_name);
	/* ignore "." and ".." */
	if (*s != '.' || (s[1] != NUL && (s[1] != '.' || s[2] != NUL)))
	{
	    STRCAT(buf, path);
	    if (mch_has_wildcard(path))
		(void)mch_expandpath(gap, buf, flags);
	    else if (mch_getperm(buf) >= 0)	/* add existing file */
		addfile(gap, buf, flags);
	}
	c = findnext(&fb);
    }
    vim_free(buf);

    matches = gap->ga_len - start_len;
    if (matches)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						   sizeof(char_u *), pstrcmp);
    return matches;
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
    if (path[0] == NUL)		    /* just checking... */
	return 0;
    if (path[1] == ':')		    /* has a drive name */
    {
	if (change_drive(TO_LOWER(path[0]) - 'a' + 1))
	    return -1;		    /* invalid drive name */
	path += 2;
    }
    if (*path == NUL)		    /* drive name only */
	return 0;
    return chdir(path);		    /* let the normal chdir() do the rest */
}
#endif

#ifdef DJGPP
/*
 * djgpp_rename() works around a bug in rename (aka MoveFile) in
 * Windows 95: rename("foo.bar", "foo.bar~") will generate a
 * file whose short file name is "FOO.BAR" (its long file name will
 * be correct: "foo.bar~").  Because a file can be accessed by
 * either its SFN or its LFN, "foo.bar" has effectively been
 * renamed to "foo.bar", which is not at all what was wanted.  This
 * seems to happen only when renaming files with three-character
 * extensions by appending a suffix that does not include ".".
 * Windows NT gets it right, however, with an SFN of "FOO~1.BAR".
 * This works like win95rename in os_win32.c, but is a bit simpler.
 *
 * Like rename(), returns 0 upon success, non-zero upon failure.
 * Should probably set errno appropriately when errors occur.
 */

#undef rename

    int
djgpp_rename(const char *OldFile, const char *NewFile)
{
    char_u  *TempFile;
    int	    retval;
    int	    fd;

    /* rename() works correctly without long file names, so use that */
    if (!_USE_LFN)
	return rename(OldFile, NewFile);

    if ((TempFile = alloc((unsigned)(STRLEN(OldFile) + 13))) == NULL)
	return -1;

    STRCPY(TempFile, OldFile);
    STRCPY(gettail(TempFile), "axlqwqhy.ba~");
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

    return retval;  /* success */
}
#endif

/*
 * Special version of getenv(): use $HOME when $VIM not defined.
 */
    char_u *
vim_getenv(var)
    char_u *var;
{
    char_u  *retval;

    retval = (char_u *)getenv((char *)var);

    if (retval == NULL && STRCMP(var, "VIM") == 0)
	retval = (char_u *)getenv("HOME");

    return retval;
}

#ifdef DJGPP
/*
 * setlocale() for DJGPP with MS-DOS codepage support
 * Author: Cyril Slobin <slobin@fe.msk.ru>
 *
 * Scaled down a lot for use by Vim: Only support setlocale(LC_ALL, "").
 */

#undef setlocale

#include <go32.h>
#include <inlines/ctype.ha>
#include <locale.h>

#define UPCASE (__dj_ISALNUM | __dj_ISALPHA | __dj_ISGRAPH | __dj_ISPRINT | __dj_ISUPPER)
#define LOCASE (__dj_ISALNUM | __dj_ISALPHA | __dj_ISGRAPH | __dj_ISPRINT | __dj_ISLOWER)

    void
djgpp_setlocale()
{
    __dpmi_regs regs;
    struct { char id; unsigned short off, seg; } __attribute__ ((packed)) info;
    unsigned char buffer[0x82], lower, upper;
    int i;

    regs.x.ax = 0x6502;
    regs.x.bx = 0xffff;
    regs.x.dx = 0xffff;
    regs.x.cx = 5;
    regs.x.es = __tb >> 4;
    regs.x.di = __tb & 0xf;

    __dpmi_int(0x21, &regs);

    if (regs.x.flags & 1)
	return;

    dosmemget(__tb, 5, &info);
    dosmemget((info.seg << 4) + info.off, 0x82, buffer);

    if (*(short *)buffer != 0x80)
	return;

    /* Fix problem of underscores being replaced with y-umlaut. (Levin) */
    if (buffer[26] == 0x5f)
	buffer[26] = 0x98;

    for (i = 0; i < 0x80; i++)
    {
	lower = i + 0x80;
	upper = (buffer+2)[i];
	if (lower != upper)
	{
	    __dj_ctype_flags[lower+1] = LOCASE;
	    __dj_ctype_toupper[lower+1] = upper;
	    if (__dj_ctype_flags[upper+1] == 0)
		__dj_ctype_flags[upper+1] = UPCASE;
	    if (__dj_ctype_tolower[upper+1] == upper)
		__dj_ctype_tolower[upper+1] = lower;
	}
    }

    return;
}
#endif /* DJGPP */
