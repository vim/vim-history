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
 * Faster text screens by John Lange (jlange@zilker.net)
 *
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
# include <sys/movedata.h>
#else
# include <alloc.h>
#endif

#if defined(DJGPP) || defined(PROTO)
# define _cdecl	    /* DJGPP doesn't have this */
#endif

static int cbrk_pressed = FALSE;    /* set by ctrl-break interrupt */
static int ctrlc_pressed = FALSE;   /* set when ctrl-C or ctrl-break detected */
static int delayed_redraw = FALSE;  /* set when ctrl-C detected */

static int bioskey_read = _NKEYBRD_READ;   /* bioskey() argument: read key */
static int bioskey_ready = _NKEYBRD_READY; /* bioskey() argument: key ready? */

#ifdef FEAT_MOUSE
static int mouse_avail = FALSE;		/* mouse present */
static int mouse_active;		/* mouse enabled */
static int mouse_hidden;		/* mouse not shown */
static int mouse_click = -1;		/* mouse status */
static int mouse_last_click = -1;	/* previous status at click */
static int mouse_x = -1;		/* mouse x coodinate */
static int mouse_y = -1;		/* mouse y coodinate */
static long mouse_click_time = 0;	/* biostime() of last click */
static int mouse_click_count = 0;	/* count for multi-clicks */
static int mouse_click_x = 0;		/* x of previous mouse click */
static int mouse_click_y = 0;		/* y of previous mouse click */
static linenr_t mouse_topline = 0;	/* topline at previous mouse click */
static int mouse_x_div = 8;		/* column = x coord / mouse_x_div */
static int mouse_y_div = 8;		/* line   = y coord / mouse_y_div */
#endif

#define BIOSTICK    55		    /* biostime() increases one tick about
					every 55 msec */

static int orig_attr = 0x0700;		/* video attributes when starting */

#ifdef DJGPP
/*
 * For DJGPP, use our own functions for fast text screens.  JML 1/18/98
 */

unsigned long	S_ulScreenBase = 0xb8000;
unsigned short	S_uiAttribute = 0;
int		S_iCurrentRow = 0;	/* These are 0 offset */
int		S_iCurrentColumn = 0;
int		S_iLeft = 0;	/* Scroll window; these are 1 offset */
int		S_iTop = 0;
int		S_iRight = 0;
int		S_iBottom = 0;
short		S_selVideo;	/* Selector for DJGPP direct video transfers */

/*
 * Use burst writes to improve mch_write speed - VJN 01/10/99
 */
unsigned short	S_linebuffer[8000]; /* <VN> enough for 160x50 */
unsigned short	S_blankbuffer[256]; /* <VN> max length of console line */
unsigned short	*S_linebufferpos = S_linebuffer;
int		S_iBufferRow;
int		S_iBufferColumn;

    static void
mygotoxy(int x, int y)
{
    S_iCurrentRow = y - 1;
    S_iCurrentColumn = x - 1;
}

    static void
setblankbuffer(unsigned short uiValue)
{
    int				i;
    static unsigned short	olduiValue = 0;

    if (olduiValue != uiValue)
    {
	/* Load blank line buffer with spaces */
	for (i = 0; i < Columns; ++i)
	    S_blankbuffer[i] = uiValue;
	olduiValue = uiValue;
    }
}

    static void
myclreol(void)
{
    /* Clear to end of line */
    setblankbuffer(S_uiAttribute | ' ');
    _dosmemputw(S_blankbuffer, S_iRight - S_iCurrentColumn, S_ulScreenBase
			 + (S_iCurrentRow) * (Columns << 1)
			 + (S_iCurrentColumn << 1));
}

    static void
myclrscr(void)
{
    /* Clear whole screen */
    short	iColumn;
    int		endpoint = (Rows * Columns) << 1;

    setblankbuffer(S_uiAttribute | ' ');

    for (iColumn = 0; iColumn < endpoint; iColumn += (Columns << 1))
	_dosmemputw(S_blankbuffer, Columns, S_ulScreenBase + iColumn);
}

    static void
mydelline(void)
{
    short iRow, iColumn;

    /* Copy the lines underneath */
    for (iRow = S_iCurrentRow; iRow < S_iBottom - 1; iRow++)
	movedata(S_selVideo, ((iRow + 1) * Columns) << 1,
		S_selVideo, (iRow * Columns) << 1,
		(S_iRight - S_iLeft + 1) << 1);

    /* Clear the new row */
    setblankbuffer(S_uiAttribute | ' ');

    iColumn = (S_iLeft - 1) << 1;
    _dosmemputw(S_blankbuffer, (S_iRight - S_iLeft) + 1, S_ulScreenBase
			 + (S_iBottom - 1) * (Columns << 1) + iColumn);
}

    static void
myinsline(void)
{
    short iRow, iColumn;

    /* Copy the lines underneath */
    for (iRow = S_iBottom - 1; iRow >= S_iTop; iRow--)
	movedata(S_selVideo, ((iRow - 1) * Columns) << 1,
		S_selVideo, (iRow * Columns) << 1,
		(S_iRight - S_iLeft + 1) << 1);

    /* Clear the new row */
    setblankbuffer(S_uiAttribute | ' ');

    iColumn = (S_iLeft - 1) << 1;
    _dosmemputw(S_blankbuffer, (S_iRight - S_iLeft) + 1, S_ulScreenBase
			 + (S_iTop - 1) * (Columns << 1) + iColumn);
}

/*
 * Scroll the screen one line up, clear the last line.
 */
    static void
myscroll(void)
{
    short		iRow, iColumn;

    /* Copy the screen */
    for (iRow = S_iTop; iRow < S_iBottom; iRow++)
	movedata(S_selVideo, (iRow * Columns) << 1,
		S_selVideo, ((iRow - 1) * Columns) << 1,
		(S_iRight - S_iLeft + 1) << 1);

    /* Clear the bottom row */
    setblankbuffer(S_uiAttribute | ' ');

    iColumn = (S_iLeft - 1) << 1;
    _dosmemputw(S_blankbuffer, (S_iRight - S_iLeft) + 1, S_ulScreenBase
			 + (S_iBottom - 1) * (Columns << 1) + iColumn);
}

    static void
myflush(void)
{
    if (S_linebufferpos != S_linebuffer)
    {
	_dosmemputw(S_linebuffer, (S_linebufferpos - S_linebuffer),
		S_ulScreenBase
		      + S_iBufferRow * (Columns << 1) + (S_iBufferColumn << 1));
	S_linebufferpos = S_linebuffer;
    }
}

    static int
myputch(int iChar)
{
    unsigned short uiValue;

    if (iChar == '\n')
    {
	myflush();
	if (S_iCurrentRow >= S_iBottom - S_iTop)
	    myscroll();
	else
	{
	    S_iCurrentColumn = S_iLeft - 1;
	    S_iCurrentRow++;
	}
    }
    else if (iChar == '\r')
    {
	myflush();
	S_iCurrentColumn = S_iLeft - 1;
    }
    else if (iChar == '\b')
    {
	myflush();
	if (S_iCurrentColumn >= S_iLeft)
	    S_iCurrentColumn--;
    }
    else if (iChar == 7)
    {
	sound(440);	/* short beep */
	delay(200);
	nosound();
    }
    else
    {
	uiValue = S_uiAttribute | (unsigned char)iChar;

	/*
	 * Normal char - are we starting to buffer?
	 */
	if (S_linebufferpos == S_linebuffer)
	{
	    S_iBufferColumn = S_iCurrentColumn;
	    S_iBufferRow = S_iCurrentRow;
	}

	*S_linebufferpos++ = uiValue;

	S_iCurrentColumn++;
	if (S_iCurrentColumn >= S_iRight && S_iCurrentRow >= S_iBottom - S_iTop)
	{
	    myflush();
	    myscroll();
	    S_iCurrentColumn = S_iLeft - 1;
	    S_iCurrentRow++;
	}
    }

    return 0;
}

    static void
mywindow(int iLeft, int iTop, int iRight, int iBottom)
{
    S_iLeft = iLeft;
    S_iTop = iTop;
    S_iRight = iRight;
    S_iBottom = iBottom;
    window(iLeft, iTop, iRight, iBottom);
}

    static void
mytextinit(struct text_info *pTextinfo)
{
    S_selVideo = __dpmi_segment_to_descriptor(S_ulScreenBase >> 4);
    S_uiAttribute = pTextinfo->normattr << 8;
}

    static void
get_screenbase(void)
{
    static union REGS	    regs;

    /* old Hercules grafic card has different base address (Macewicz) */
    regs.h.ah = 0x0f;
    (void)int86(0x10, &regs, &regs);	/* int 10 0f */
    if (regs.h.al == 0x07)		/* video mode 7 -- hercules mono */
	S_ulScreenBase = 0xb0000;
    else
	S_ulScreenBase = 0xb8000;
}

    static void
mytextattr(int iAttribute)
{
    S_uiAttribute = (unsigned short)iAttribute << 8;
}

    static void
mynormvideo(void)
{
    mytextattr(orig_attr);
}

    static void
mytextcolor(int iTextColor)
{
    S_uiAttribute = (unsigned short)((S_uiAttribute & 0xf000)
					   | (unsigned short)iTextColor << 8);
}

    static void
mytextbackground(int iBkgColor)
{
    S_uiAttribute = (unsigned short)((S_uiAttribute & 0x0f00)
					 | (unsigned short)(iBkgColor << 12));
}
/*
 * Getdigits: Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */

    static long
mygetdigits(pp)
    char_u **pp;
{
    char_u	*p;
    long	retval = 0;

    p = *pp;
    if (*p == '-')		/* skip negative sign */
	++p;
    while(isdigit(*p))
    {
	retval = (retval * 10) + (*p - '0');
	++p;
    }
    if (**pp == '-')		/* process negative sign */
	retval = -retval;

    *pp = p;
    return retval;
}
#else
# define mygotoxy gotoxy
# define myputch putch
# define myscroll scroll
# define mywindow window
# define mynormvideo normvideo
# define mytextattr textattr
# define mytextcolor textcolor
# define mytextbackground textbackground
# define mygetdigits getdigits
# define myclreol clreol
# define myclrscr clrscr
# define myinsline insline
# define mydelline delline
#endif

static const struct
{
    char_u	scancode;
    char_u	metakey;
} altkey_table[] =
{
    {0x1e, 0xe1}, /* a */
    {0x30, 0xe2}, /* b */
    {0x2e, 0xe3}, /* c */
    {0x20, 0xe4}, /* d */
    {0x12, 0xe5}, /* e */
    {0x21, 0xe6}, /* f */
    {0x22, 0xe7}, /* g */
    {0x23, 0xe8}, /* h */
    {0x17, 0xe9}, /* i */
    {0x24, 0xea}, /* j */
    {0x25, 0xeb}, /* k */
    {0x26, 0xec}, /* l */
    {0x32, 0xed}, /* m */
    {0x31, 0xee}, /* n */
    {0x18, 0xef}, /* o */
    {0x19, 0xf0}, /* p */
    {0x10, 0xf1}, /* q */
    {0x13, 0xf2}, /* r */
    {0x1f, 0xf3}, /* s */
    {0x14, 0xf4}, /* t */
    {0x16, 0xf5}, /* u */
    {0x2f, 0xf6}, /* v */
    {0x11, 0xf7}, /* w */
    {0x2d, 0xf8}, /* x */
    {0x15, 0xf9}, /* y */
    {0x2c, 0xfa}, /* z */
    {0x78, 0xb1}, /* 1 */
    {0x79, 0xb2}, /* 2 */
    {0x7a, 0xb3}, /* 3 */
    {0x7b, 0xb4}, /* 4 */
    {0x7c, 0xb5}, /* 5 */
    {0x7d, 0xb6}, /* 6 */
    {0x7e, 0xb7}, /* 7 */
    {0x7f, 0xb8}, /* 8 */
    {0x80, 0xb9}, /* 9 */
    {0x81, 0xb0}, /* 0 */
};

/*
 * Translate extended keycodes into meta-chars where applicable
 */
    static int
translate_altkeys(int rawkey)
{
    int i, c;

    if ((rawkey & 0xff) == 0)
    {
	c = (rawkey >> 8);
	for (i = sizeof(altkey_table) / sizeof(altkey_table[0]); --i >= 0; )
	{
	    if (c == altkey_table[i].scancode)
		return (int)altkey_table[i].metakey;
	}
    }
    return rawkey;
}

/*
 * Set normal fg/bg color, based on T_ME.  Called whem t_me has been set.
 */
    void
mch_set_normal_colors()
{
    char_u	*p;
    int		n;

    cterm_normal_fg_color = (orig_attr & 0xf) + 1;
    cterm_normal_bg_color = ((orig_attr >> 4) & 0xf) + 1;
    if (T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	if (*p == 'm' && n > 0)
	{
	    cterm_normal_fg_color = (n & 0xf) + 1;
	    cterm_normal_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
}

#if defined(MCH_CURSOR_SHAPE) || defined(PROTO)
/*
 * Save/restore the shape of the cursor.
 * call with FALSE to save, TRUE to restore
 */
    static void
mch_restore_cursor_shape(int restore)
{
    static union REGS	    regs;
    static int		    saved = FALSE;

    if (restore)
    {
	if (saved)
	    regs.h.ah = 0x01;	    /*Set Cursor*/
	else
	    return;
    }
    else
    {
	regs.h.ah = 0x03;	    /*Get Cursor*/
	regs.h.bh = 0x00;	    /*Page */
	saved = TRUE;
    }

    (void)int86(0x10, &regs, &regs);
}

/*
 * Set the shape of the cursor.
 * 'thickness' can be from 0 (thin) to 7 (block)
 */
    static void
mch_set_cursor_shape(int thickness)
{
    union REGS	    regs;

    regs.h.ch = 7 - thickness;	    /*Starting Line*/
    regs.h.cl = 7;		    /*Ending Line*/
    regs.h.ah = 0x01;		    /*Set Cursor*/
    (void)int86(0x10, &regs, &regs);
}

    void
mch_update_cursor(void)
{
    int		idx;
    int		thickness;

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    if (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 7;
    else
	thickness = (7 * shape_table[idx].percentage + 90) / 100;
    mch_set_cursor_shape(thickness);
}
#endif

/*
 * Return amount of memory currently available.
 */
    long_u
mch_avail_mem(int special)
{
#ifdef DJGPP
    return _go32_dpmi_remaining_virtual_memory();
#else
    return coreleft();
#endif
}

#ifdef FEAT_MOUSE

/*
 * Set area where mouse can be moved to: The whole screen.
 * Rows and Columns must be valid when calling!
 */
    static void
mouse_area(void)
{
    union REGS	    regs;

    if (mouse_avail)
    {
	regs.x.cx = 0;	/* mouse visible between cx and dx */
	regs.x.dx = Columns * mouse_x_div - 1;
	regs.x.ax = 7;
	(void)int86(0x33, &regs, &regs);

	regs.x.cx = 0;	/* mouse visible between cx and dx */
	regs.x.dx = Rows * mouse_y_div - 1;
	regs.x.ax = 8;
	(void)int86(0x33, &regs, &regs);
    }
}

    static void
show_mouse(int on)
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
	if ((on && !was_on) || (!on && was_on))
	{
	    was_on = on;
	    regs.x.ax = on ? 1 : 2;
	    int86(0x33, &regs, &regs);	/* show mouse */
	    if (on)
		mouse_area();
	}
    }
}

#endif

/*
 * Version of kbhit() and getch() that use direct console I/O.
 * This avoids trouble with CTRL-P and the like, and should work over a telnet
 * connection (it works for Xvi).
 */

static int cons_key = -1;

/*
 * Try to get one character directly from the console.
 * If there is a key, it is stored in cons_key.
 * Only call when cons_key is -1!
 */
    static void
cons_getkey(void)
{
    union REGS regs;

    /* call DOS function 6: Direct console I/O */
    regs.h.ah = 0x06;
    regs.h.dl = 0xff;
    (void)intdos(&regs, &regs);
    if ((regs.x.flags & 0x40) == 0)	/* zero flag not set? */
	cons_key = (regs.h.al & 0xff);
}

/*
 * Return TRUE if a character is available.
 */
    static int
cons_kbhit(void)
{
    if (cons_key < 0)
	cons_getkey();
    return (cons_key >= 0);
}

/*
 * Return a character from the console.
 * Should only be called when vim_kbhit() returns TRUE.
 */
    static int
cons_getch(void)
{
    int	    c = -1;

    if (cons_key < 0)
	cons_getkey();
    c = cons_key;
    cons_key = -1;
    return c;
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
WaitForChar(long msec)
{
    union REGS	regs;
    long	starttime;
    int		x, y;

    starttime = biostime(0, 0L);

    for (;;)
    {
#ifdef FEAT_MOUSE
	long		clicktime;
	static int	old_status = 0;

	if (mouse_avail && mouse_active && mouse_click < 0)
	{
	    regs.x.ax = 3;
	    int86(0x33, &regs, &regs);	    /* check mouse status */
		/* only recognize button-down and button-up event */
	    x = regs.x.cx / mouse_x_div;
	    y = regs.x.dx / mouse_y_div;
	    if ((old_status == 0) != (regs.x.bx == 0))
	    {
		if (old_status)	/* button up */
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
	    else if (old_status && (x != mouse_x || y != mouse_y))
		mouse_click = MOUSE_DRAG;
	    old_status = regs.x.bx;
	    if (mouse_hidden && mouse_x >= 0 && (mouse_x != x || mouse_y != y))
	    {
		mouse_hidden = FALSE;
		show_mouse(TRUE);
	    }
	    mouse_x = x;
	    mouse_y = y;
	}
#endif

	if ((p_consk ? cons_kbhit()
				 : p_biosk ? bioskey(bioskey_ready) : kbhit())
		|| cbrk_pressed
#ifdef FEAT_MOUSE
						    || mouse_click >= 0
#endif
		)
	    return TRUE;
	/*
	 * Use biostime() to wait until our time is done.
	 * We busy-wait here.  Unfortunately, delay() and usleep() have been
	 * reported to give problems with the original Windows 95.  This is
	 * fixed in service pack 1, but not everybody installed that.
	 * The DJGPP implementation of usleep() uses a busy-wait loop too.
	 */
	if (msec != FOREVER && biostime(0, 0L) > starttime + msec / BIOSTICK)
	    break;

#ifdef DJGPP
	/* Yield the CPU to the next process. */
	__dpmi_yield();
#endif
    }
    return FALSE;
}

/*
 * don't do anything for about "msec" msec
 */
    void
mch_delay(
    long	msec,
    int		ignoreinput)
{
    long	starttime;

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
mch_remove(char_u *name)
{
    (void)mch_setperm(name, 0);    /* default permissions */
    return unlink((char *)name);
}

/*
 * mch_write(): write the output buffer to the screen
 */
    void
mch_write(
    char_u	*s,
    int		len)
{
    char_u	*p;
    int		row, col;

    if (term_console && full_screen)
	while (len--)
	{
	    /* translate ESC | sequences into bios calls */
	    if (p_wd)	    /* testing: wait a bit for each char */
		WaitForChar(p_wd);

	    if (s[0] == '\n')
#ifdef DJGPP
	    {
		myflush();
		S_iCurrentColumn = S_iLeft - 1;
	    }
#else
		myputch('\r');
#endif
	    else if (s[0] == ESC && len > 1 && s[1] == '|')
	    {
		switch (s[2])
		{
#ifdef DJGPP
		case 'B':   ScreenVisualBell();
			    goto got3;
#endif
		case 'J':
#ifdef DJGPP
			    myflush();
#endif
			    myclrscr();
			    goto got3;

		case 'K':
#ifdef DJGPP
			    myflush();
#endif
			    myclreol();
			    goto got3;

		case 'L':
#ifdef DJGPP
			    myflush();
#endif
			    myinsline();
			    goto got3;

		case 'M':
#ifdef DJGPP
			    myflush();
#endif
			    mydelline();
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
			    row = mygetdigits(&p);    /* no check for length! */
			    if (p > s + len)
				break;
			    if (*p == ';')
			    {
				++p;
				col = mygetdigits(&p); /* no check for length! */
				if (p > s + len)
				    break;
				if (*p == 'H' || *p == 'r')
				{
#ifdef DJGPP
				    myflush();
#endif
				    if (*p == 'H')  /* set cursor position */
					mygotoxy(col, row);
				    else	    /* set scroll region  */
					mywindow(1, row, Columns, col);
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
					mynormvideo();/* reset color */
				    else
					mytextattr(row);
				}
				else if (*p == 'f') /* set foreground color */
				    mytextcolor(row);
				else		    /* set background color */
				    mytextbackground(row);

				len -= p - s;
				s = p + 1;
				continue;
			    }
		}
	    }
	    myputch(*s++);
	}
    else
	write(1, s, (unsigned)len);
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
mch_inchar(
    char_u	*buf,
    int		maxlen,
    long	time)
{
    int		len = 0;
    int		c;
    int		tmp_c;
    static int	nextchar = 0;	    /* may keep character when maxlen == 1 */
#ifdef FEAT_AUTOCMD
    static int	once_already = 0;
#endif

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

#ifdef FEAT_MOUSE
    if (time != 0)
	show_mouse(TRUE);
#endif
    if (time >= 0)
    {
	if (WaitForChar(time) == 0)	/* no character available */
	{
#ifdef FEAT_MOUSE
	    show_mouse(FALSE);
#endif
#ifdef FEAT_AUTOCMD
	    once_already = 0;
#endif
	    return 0;
	}
    }
    else    /* time == -1 */
    {
#ifdef DJGPP
	myflush();
	gotoxy(S_iCurrentColumn + 1, S_iCurrentRow + 1);
#endif
#ifdef FEAT_AUTOCMD
	if (once_already == 2)
	    updatescript(0);
	else if (once_already == 1)
	{
	    setcursor();
	    once_already = 2;
	    return 0;
	}
	else
#endif
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk
	 */
	    if (WaitForChar(p_ut) == 0)
	{
#ifdef FEAT_AUTOCMD
            if (has_cursorhold() && get_real_state() == NORMAL_BUSY)
            {
                apply_autocmds(EVENT_CURSORHOLD, NULL, NULL, FALSE, curbuf);
                update_screen(VALID);
		once_already = 1;
                return 0;
            }
            else
#endif
		updatescript(0);
	}
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
#ifdef FEAT_MOUSE
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
#ifdef FEAT_MOUSE
	mouse_hidden = TRUE;
#endif
	if (p_biosk && !p_consk)
	{
	    while ((len == 0 || bioskey(bioskey_ready)) && len < maxlen)
	    {
		c = translate_altkeys(bioskey(bioskey_read)); /* get the key */
		/*
		 * translate a few things for inchar():
		 * 0x0000 == CTRL-break		-> 3	(CTRL-C)
		 * 0x0300 == CTRL-@		-> NUL
		 * 0xnn00 == extended key code	-> K_NUL, nn
		 * 0xnne0 == enhanced keyboard	-> K_NUL, nn
		 * K_NUL			-> K_NUL, 3
		 */
		if (c == 0)
		    c = 3;
		else if (c == 0x0300)
		    c = NUL;
		else if ((c & 0xff) == 0
			|| c == K_NUL
			|| c == 0x4e2b
			|| c == 0x4a2d
			|| c == 0x372a
			|| ((c & 0xff) == 0xe0 && c != 0xe0))
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
	    while ((len == 0 || (p_consk ? cons_kbhit() : kbhit()))
		    && len < maxlen)
	    {
		switch (c = (p_consk ? cons_getch() : getch()))
		{
		    case 0:
			/* NUL means that there is another character.
			 * Get it immediately, because kbhit() doesn't always
			 * return TRUE for the second character.
			 */
			if (p_consk)
			    c = cons_getch();
			else
			    c = getch();
			tmp_c = translate_altkeys(c << 8);
			if (tmp_c == (c << 8))
			{
			    *buf++ = K_NUL;
			    ++len;
			}
			else
			    c = tmp_c;
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
#ifdef FEAT_MOUSE
    show_mouse(FALSE);
#endif

    beep_count = 0;	    /* may beep again now that we got some chars */
#ifdef FEAT_AUTOCMD
    once_already = 0;
#endif
    return len;
}

/*
 * return non-zero if a character is available
 */
    int
mch_char_avail(void)
{
    return WaitForChar(0L);
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
catch_cbrk(void)
#endif
{
    cbrk_pressed = TRUE;
    ctrlc_pressed = TRUE;
}

#ifndef DJGPP
/*
 * ctrl-break handler for DOS. Never called when a ctrl-break is typed, because
 * we catch interrupt 1b. If you type ctrl-C while Vim is waiting for a
 * character this function is not called. When a ctrl-C is typed while Vim is
 * busy this function may be called. By that time a ^C has been displayed on
 * the screen, so we have to redisplay the screen. We can't do that here,
 * because we may be called by DOS. The redraw is in mch_inchar().
 */
    static int _cdecl
cbrk_handler(void)
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
#endif

/*
 * Set the interrupt vectors for use with Vim on or off.
 * on == TRUE means as used within Vim
 */
    static void
set_interrupts(int on)
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
	    mynormvideo();		/* restore screen colors */
    }
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    void
mch_suspend(void)
{
    suspend_shell();
}

extern int _fmode;

/*
 * Prepare window for use by Vim.
 */
    void
mch_shellinit(void)
{
    union REGS regs;

    /*
     * Get the video attributes at the cursor.  These will be used as the
     * default attributes.
     */
    regs.h.ah = 0x08;
    regs.h.bh = 0x00;		/* video page 0 */
    int86(0x10, &regs, &regs);
    orig_attr = regs.h.ah;
    mynormvideo();
    if (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (orig_attr & 0xf) + 1;
    if (cterm_normal_bg_color == 0)
	cterm_normal_bg_color = ((orig_attr >> 4) & 0xf) + 1;

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

    get_screenbase();
#endif

#ifdef FEAT_MOUSE
/* find out if a MS compatible mouse is available */
    regs.x.ax = 0;
    (void)int86(0x33, &regs, &regs);
    mouse_avail = regs.x.ax;
    /* best guess for mouse coordinate computations */
    mch_get_shellsize();
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

    /*
     * Test if we have an enhanced AT keyboard.  Write 0xFFFF to the keyboard
     * buffer and try to read it back.  If we can't in 16 tries, it's an old
     * type XT keyboard.
     */
    regs.h.ah = 0x05;
    regs.x.cx = 0xffff;
    int86(0x16, &regs, &regs);
    if (regs.h.al != 1)	/* skip this when keyboard buffer is full */
    {
	int i;

	for (i = 0; i < 16; ++i)
	{
	    regs.h.ah = 0x10;
	    int86(0x16, &regs, &regs);
	    if (regs.x.ax == 0xffff)
		break;
	}
	if (i == 16)	/* 0xffff not read, must be old keyboard */
	{
	    bioskey_read = 0;
	    bioskey_ready = 1;
	}
    }

#ifdef MCH_CURSOR_SHAPE
    /* Save the old cursor shape */
    mch_restore_cursor_shape(FALSE);
    /* Initialise the cursor shape */
    mch_update_cursor();
#endif
}

    int
mch_check_win(
    int		argc,
    char	**argv)
{
    /* store argv[0], may be used for $VIM */
    if (*argv[0] != NUL)
	exe_name = FullName_save((char_u *)argv[0], FALSE);

    /*
     * Try the DOS search path.  The executable may in
     * fact be called differently, so try this last.
     */
    if (exe_name == NULL || *exe_name == NUL)
	exe_name = searchpath("vim.exe");

    if (isatty(1))
	return OK;
    return FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    int
mch_input_isatty(void)
{
    if (isatty(read_cmd_fd))
	return TRUE;
    return FALSE;
}

#if defined(USE_FNAME_CASE) || defined(PROTO)
/*
 * fname_case(): Set the case of the file name, if it already exists.
 */
    void
fname_case(char_u *name)
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
 * Insert user name in s[len].
 */
    int
mch_get_user_name(
    char_u	*s,
    int		len)
{
    *s = NUL;
    return FAIL;
}

/*
 * Insert host name is s[len].
 */
    void
mch_get_host_name(
    char_u	*s,
    int		len)
{
#ifdef DJGPP
    STRNCPY(s, _("PC (32 bits Vim)"), len);
#else
    STRNCPY(s, _("PC (16 bits Vim)"), len);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(void)
{
    return (long)0;
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    int
mch_dirname(
    char_u	*buf,
    int		len)
{
#ifdef DJGPP
    if (getcwd((char *)buf, len) == NULL)
	return FAIL;
    /* turn the '/'s returned by DJGPP into '\'s */
    slash_adjust(buf);
    return OK;
#else
    return (getcwd((char *)buf, len) != NULL ? OK : FAIL);
#endif
}

/*
 * Change default drive (just like _chdrive of Borland C 3.1)
 */
    static int
change_drive(int drive)
{
    union REGS regs;

    regs.h.ah = 0x0e;
    regs.h.dl = drive - 1;
    intdos(&regs, &regs);   /* set default drive */
    regs.h.ah = 0x19;
    intdos(&regs, &regs);   /* get default drive */
    if (regs.h.al == drive - 1)
	return 0;
    return -1;
}

/*
 * Get absolute file name into buffer 'buf' of length 'len' bytes.
 * All slashes are replaced with backslashes, to avoid trouble when comparing
 * file names.
 * When 'shellslash' set do it the other way around.
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    int		len,
    int		force)
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

#ifdef __BORLANDC__		/* Only Borland C++ has this */
    if (_fullpath((char *)buf, (char *)fname, len - 1) == NULL)
    {
	STRNCPY(buf, fname, len);   /* failed, use the relative path name */
	slash_adjust(buf);
	return FAIL;
    }
    /* Append a backslash after a directory name, unless it's already there */
    {
	int	c;

	if (mch_isdir(buf) && (c = buf[STRLEN(buf) - 1]) != '\\' && c != '/')
	    STRCAT(buf, pseps);
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
		if (p == fname)			/* /fname	    */
		    q = p + 1;			/* -> /		    */
		else if (q + 1 == p)		/* ... c:\foo	    */
		    q = p + 1;			/* -> c:\	    */
		else				/* but c:\foo\bar   */
		    q = p;			/* -> c:\foo	    */

		c = *q;			/* truncate at start of fname */
		*q = NUL;
#ifdef DJGPP
		STRCPY(buf, fname);
		slash_adjust(buf);	/* needed when fname starts with \ */
		if (mch_chdir(buf))	/* change to the directory */
#else
		if (mch_chdir(fname))	/* change to the directory */
#endif
		    retval = FAIL;
		else
		{
		    fname = q;
		    if (c == psepc)	    /* if we cut the name at a */
			fname++;	    /* '\', don't add it again */
		}
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
	    strcat(buf, pseps);
	if (p)
	    mch_chdir(olddir);
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
 * When 'shellslash' set do it the other way around.
 */
    void
slash_adjust(char_u *p)
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
	if (*p == psepcN)
	    *p = psepc;
	++p;
    }
}

/*
 * return TRUE is fname is an absolute path name
 */
    int
mch_isFullName(char_u *fname)
{
    return (vim_strchr(fname, ':') != NULL);
}

/*
 * get file permissions for 'name'
 * -1 : error
 * else FA_attributes defined in dos.h
 */
    long
mch_getperm(char_u *name)
{
    return (long)_chmod((char *)name, 0, 0);	 /* get file mode */
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_setperm(
    char_u	*name,
    long	perm)
{
    perm |= FA_ARCH;	    /* file has changed, set archive bit */
    return (_chmod((char *)name, 1, (int)perm) == -1 ? FAIL : OK);
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name)
{
    /* DOS 6.2 share.exe causes "seek error on file write" errors when making
     * the swap file hidden.  Thus don't do it. */
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 *
 * beware of a trailing (back)slash
 */
    int
mch_isdir(char_u *name)
{
    int	    f;
    char_u  *p;

    p = name + strlen((char *)name);
    if (p > name)
	--p;
    if (*p == '\\' || *p == '/')   /* remove trailing (back)slash for now */
	*p = NUL;
    else
	p = NULL;
    f = _chmod((char *)name, 0, 0);
    if (p != NULL)
	*p = psepc;		    /* put back (back)slash */
    if (f == -1)
	return FALSE;		    /* file does not exist at all */
    if ((f & FA_DIREC) == 0)
	return FALSE;		    /* not a directory */
    return TRUE;
}

/*
 * Careful: mch_windexit() may be called before mch_shellinit()!
 */
    void
mch_windexit(int r)
{
    settmode(TMODE_COOK);
    stoptermcap();
    set_interrupts(FALSE);	    /* restore interrupts */
    /* Somehow outputting CR-NL causes the original colors to be restored */
    out_char('\r');
    out_char('\n');
    out_flush();
    ml_close_all(TRUE);		    /* remove all memfiles */
#ifdef MCH_CURSOR_SHAPE
    mch_restore_cursor_shape(TRUE);
#endif
    exit(r);
}

/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 * Does not change the tty, as bioskey() and kbhit() work raw all the time.
 */
    void
mch_settmode(int tmode)
{
}

#ifdef FEAT_MOUSE
    void
mch_setmouse(int on)
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
mch_screenmode(char_u *arg)
{
    int		    mode;
    int		    i;
    static char	   *(names[]) = {"BW40", "C40", "BW80", "C80", "MONO", "C4350"};
    static int	    modes[]   = { BW40,	  C40,	 BW80,	 C80,	MONO,	C4350};

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
	EMSG(_("Unsupported screen mode"));
	return FAIL;
    }
    textmode(mode);		    /* use Borland function */
#ifdef DJGPP
    /* base address may have changed */
    get_screenbase();
#endif

    /* Screen colors may have changed. */
    out_str(T_ME);

#ifdef FEAT_MOUSE
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
    shell_resized();
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
mch_get_shellsize(void)
{
    struct text_info textinfo;

    /*
     * The screenwidth is returned by the BIOS OK.
     * The screenheight is in a location in the bios RAM, if the display is
     * EGA or VGA.
     */
    if (!term_console)
	return FAIL;
    gettextinfo(&textinfo);
    Columns = textinfo.screenwidth;
    Rows = textinfo.screenheight;
#ifndef DJGPP
    if (textinfo.currmode > 10)
	Rows = *(char far *)MK_FP(0x40, 0x84) + 1;
#endif

    if (Columns < MIN_COLUMNS || Rows < MIN_LINES)
    {
	/* these values are overwritten by termcap size or default */
	Columns = 80;
	Rows = 25;
	return FAIL;
    }
#ifdef DJGPP
    mytextinit(&textinfo);   /* Added by JML, 1/15/98 */
#endif

    return OK;
}

/*
 * Set the active window for delline/insline.
 */
    static void
set_window(void)
{
    if (term_console)
    {
#ifndef DJGPP
	_video.screenheight = Rows;
#endif
	mywindow(1, 1, Columns, Rows);
    }
    screen_start();
}

    void
mch_set_shellsize(void)
{
    /* Should try to set the window size to Rows and Columns.
     * May involve switching display mode....
     * We assume the user knows the size and just use it. */
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize()
{
#ifdef FEAT_MOUSE
    /* best guess for mouse coordinate computations */
    if (Columns <= 40)
	mouse_x_div = 16;
    if (Rows == 30)
	mouse_y_div = 16;
#endif
    set_window();
#ifdef FEAT_MOUSE
    mouse_area();	/* set area where mouse can go */
#endif
}

#if defined(DJGPP) || defined(PROTO)
/*
 * Check the number of Columns with a BIOS call.  This avoids a crash of the
 * DOS console when 'columns' is set to a too large value.
 */
    void
mch_check_columns()
{
    static union REGS	regs;

    regs.h.ah = 0x0f;
    (void)int86(0x10, &regs, &regs);
    if ((unsigned)Columns > (unsigned)regs.h.ah)
	Columns = (unsigned)regs.h.ah;
}
#endif

/*
 * call shell, return FAIL for failure, OK otherwise
 * options: SHELL_*, see vim.h.
 */
    int
mch_call_shell(
    char_u	*cmd,
    int		options)
{
    int		x;
#ifndef DJGPP
    char_u	*newcmd;
#endif

    out_flush();

    if (options & SHELL_COOKED)
	settmode(TMODE_COOK);	/* set to normal mode */
    set_interrupts(FALSE);	/* restore interrupts */

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
	/*
	 * Use 'shell' for system().
	 */
	setenv("SHELL", (char *)p_sh, 1);
	x = system(cmd);
#else
	/* we use "command" to start the shell, slow but easy */
	newcmd = alloc(STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 3);
	if (newcmd == NULL)
	    x = -1;
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

    if (x && !(options & SHELL_SILENT))
    {
	msg_putchar('\n');
	msg_outnum((long)x);
	MSG_PUTS(_(" returned\n"));
    }

    return x;
}

/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
    void
mch_breakcheck(void)
{
    if (ctrlc_pressed)
    {
	ctrlc_pressed = FALSE;
	got_int = TRUE;
    }
}

static int _cdecl pstrcmp();  /* BCC does not like the types */

    static int _cdecl
pstrcmp(a, b)
    char_u **a, **b;
{
    return (stricmp((char *)*a, (char *)*b));
}

    int
mch_has_wildcard(char_u *s)
{
#ifdef VIM_BACKTICK
    return (vim_strpbrk(s, (char_u *)"?*$~`") != NULL);
#else
    return (vim_strpbrk(s, (char_u *)"?*$~") != NULL);
#endif
}

    static void
namelowcpy(
    char_u *d,
    char_u *s)
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
 * This only expands '?' and '*'.
 * Return the number of matches found.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "wildc".
 */
    static int
dos_expandpath(
    garray_t	*gap,
    char_u	*path,
    char_u	*wildc,
    int		flags)		/* EW_* flags */
{
    char_u		*buf;
    char_u		*p, *s, *e;
    int			start_len, c;
    struct ffblk	fb;
    int			matches;
    int			len;

    start_len = gap->ga_len;
    /* make room for file name */
    buf = alloc(STRLEN(path) + BASENAMELEN + 5);
    if (buf == NULL)
	return 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    while (*path)
    {
	if (path >= wildc && rem_backslash(path))	/* remove a backslash */
	    ++path;
	else if (*path == '\\' || *path == ':' || *path == '/')
	{
	    if (e != NULL)
		break;
	    else
		s = p + 1;
	}
	else if (*path == '*' || *path == '?')
	    e = p;
	*p++ = *path++;
    }
    e = p;
    *e = NUL;
    /* now we have one wildcard component between s and e */

    /* if the file name ends in "*" and does not contain a ".", add ".*" */
    if (e[-1] == '*' && vim_strchr(s, '.') == NULL)
    {
	*e++ = '.';
	*e++ = '*';
	*e = NUL;
    }

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
	    len = STRLEN(buf);
	    STRCPY(buf + len, path);
	    if (mch_has_wildcard(path))
	    {
		/* need to expand another component of the path */
		/* remove backslashes for the remaining components only */
		(void)dos_expandpath(gap, buf, buf + len + 1, flags);
	    }
	    else
	    {
		/* no more wildcards, check if there is a match */
		/* remove backslashes for the remaining components only */
		if (*path)
		    backslash_halve(buf + len + 1);
		if (mch_getperm(buf) >= 0)	/* add existing file */
		    addfile(gap, buf, flags);
	    }
	}
	c = findnext(&fb);
    }
    vim_free(buf);

    matches = gap->ga_len - start_len;
    if (matches)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   sizeof(char_u *), pstrcmp);
    return matches;
}

    int
mch_expandpath(
    garray_t	*gap,
    char_u	*path,
    int		flags)		/* EW_* flags */
{
    return dos_expandpath(gap, path, path, flags);
}

/*
 * Change directory to "path".
 * The normal chdir() does not change the default drive.  This one does.
 * Return 0 for success, -1 for failure.
 */
    int
mch_chdir(char *path)
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

#ifdef DJGPP
/*
 * mch_rename() works around a bug in rename (aka MoveFile) in
 * Windows 95: rename("foo.bar", "foo.bar~") will generate a
 * file whose short file name is "FOO.BAR" (its long file name will
 * be correct: "foo.bar~").  Because a file can be accessed by
 * either its SFN or its LFN, "foo.bar" has effectively been
 * renamed to "foo.bar", which is not at all what was wanted.  This
 * seems to happen only when renaming files with three-character
 * extensions by appending a suffix that does not include ".".
 * Windows NT gets it right, however, with an SFN of "FOO~1.BAR".
 * This works like mch_rename in os_win32.c, but is a bit simpler.
 *
 * Like rename(), returns 0 upon success, non-zero upon failure.
 * Should probably set errno appropriately when errors occur.
 */

    int
mch_rename(const char *OldFile, const char *NewFile)
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
	mch_remove((char_u *)OldFile);

	/* If renaming to NewFile failed, rename TempFile back to OldFile, so
	 * that it looks like nothing happened. */
	if (retval)
	    rename(TempFile, OldFile);
    }
    vim_free(TempFile);

    return retval;  /* success */
}
#endif

/*
 * Special version of getenv(): Use uppercase name.
 */
    char_u *
mch_getenv(char_u *name)
{
    int		i;
#define MAXENVLEN 50
    char_u	var_copy[MAXENVLEN + 1];

    /*
     * Take a copy of the argument, and force it to upper case before passing
     * to getenv().  On DOS systems, getenv() doesn't like lower-case argument
     * (unlike Win32 et al.)  If the name is too long, just use it plain.
     */
    if (STRLEN(name) < MAXENVLEN)
    {
	for (i = 0; name[i] != NUL; ++i)
	    var_copy[i] = toupper(name[i]);
	var_copy[i] = NUL;
	name = var_copy;
    }
    return (char_u *)getenv((char *)name);
}

#if defined(DJGPP) || defined(PROTO)
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

    char *
djgpp_setlocale(void)
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
	return NULL;

    dosmemget(__tb, 5, &info);
    dosmemget((info.seg << 4) + info.off, 0x82, buffer);

    if (*(short *)buffer != 0x80)
	return NULL;

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

    return "C";
}
#endif /* DJGPP */
