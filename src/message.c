/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * message.c: functions for displaying messages on the command line
 */

#define MESSAGE_FILE		/* don't include prototype for smsg() */

#include "vim.h"

#ifdef __QNX__
# include <stdarg.h>
#endif

static void msg_home_replace_attr __ARGS((char_u *fname, int attr));
static int  msg_use_printf __ARGS((void));
static void msg_screen_putchar __ARGS((int c, int attr));
static int  msg_check_screen __ARGS((void));
static void redir_write __ARGS((char_u *s));
#ifdef CON_DIALOG
static char_u *msg_show_console_dialog __ARGS((char_u *message, char_u *buttons, int dfltbutton));
#endif

/*
 * msg(s) - displays the string 's' on the status line
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 * return TRUE if wait_return not called
 */
    int
msg(s)
    char_u	    *s;
{
    return msg_attr(s, 0);
}

    int
msg_attr(s, attr)
    char_u	   *s;
    int		    attr;
{
    static int	    entered = 0;
    int		    retval;

    /*
     * It is possible that displaying a messages causes a problem (e.g.,
     * when redrawing the window), which causes another message, etc..	To
     * break this loop, limit the recursiveness to 3 levels.
     */
    if (entered >= 3)
	return TRUE;
    ++entered;

    msg_start();
    msg_outtrans_attr(s, attr);
    msg_clr_eos();
    retval = msg_end();

    --entered;
    return retval;
}

/*
 * automatic prototype generation does not understand this function
 */
#ifndef PROTO
# ifndef __QNX__

int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
smsg __ARGS((char_u *, long, long, long,
			long, long, long, long, long, long, long));
int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
smsg_attr __ARGS((int, char_u *, long, long, long,
			long, long, long, long, long, long, long));

/* VARARGS */
    int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
smsg(s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
    char_u	*s;
    long	a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
{
    return smsg_attr(0, s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
}

/* VARARGS */
    int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
smsg_attr(attr, s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
    int		attr;
    char_u	*s;
    long	a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
{
    sprintf((char *)IObuff, (char *)s, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
    return msg_attr(IObuff, attr);
}

# else /* __QNX__ */

int smsg(char_u *s, ...)
{
    va_list arglist;

    va_start(arglist, s);
    vsprintf((char *)IObuff, (char *)s, arglist);
    va_end(arglist);
    return msg(IObuff);
}

int smsg_attr(int attr, char_u *s, ...)
{
    va_list arglist;

    va_start(arglist, s);
    vsprintf((char *)IObuff, (char *)s, arglist);
    va_end(arglist);
    return msg_attr(IObuff, attr);
}

# endif /* __QNX__ */
#endif

/*
 * emsg() - display an error message
 *
 * Rings the bell, if appropriate, and calls message() to do the real work
 * When terminal not initialized (yet) mch_errmsg(..) is used.
 *
 * return TRUE if wait_return not called
 */
    int
emsg(s)
    char_u	   *s;
{
    char_u	    *Buf;
    static int	    last_lnum = 0;
    static char_u   *last_sourcing_name = NULL;
    int		    attr;
    int		    other_sourcing_name;

    if (emsg_off)		/* no error messages at the moment */
	return TRUE;

    if (global_busy)		/* break :global command */
	++global_busy;

    if (p_eb)
	beep_flush();		/* also includes flush_buffers() */
    else
	flush_buffers(FALSE);	/* flush internal buffers */
    did_emsg = TRUE;		/* flag for DoOneCmd() */
    emsg_on_display = TRUE;	/* remember there is an error message */
    ++msg_scroll;		/* don't overwrite a previous message */
    attr = hl_attr(HLF_E);	/* set highlight mode for error messages */
    if (msg_scrolled)
	need_wait_return = TRUE;    /* needed in case emsg() is called after
				     * wait_return has reset need_wait_return
				     * and a redraw is expected because
				     * msg_scrolled is non-zero */

/*
 * First output name and line number of source of error message
 */
    if (sourcing_name != NULL)
    {
	if (last_sourcing_name != NULL)
	    other_sourcing_name = STRCMP(sourcing_name, last_sourcing_name);
	else
	    other_sourcing_name = TRUE;
    }
    else
	other_sourcing_name = FALSE;

    if (sourcing_name != NULL
	    && (other_sourcing_name || sourcing_lnum != last_lnum)
	    && (Buf = alloc(MAXPATHL + 30)) != NULL)
    {
	++no_wait_return;
	if (other_sourcing_name)
	{
	    sprintf((char *)Buf, "Error detected while processing %s:",
					    sourcing_name);
	    msg_attr(Buf, attr);
	}
	    /* lnum is 0 when executing a command from the command line
	     * argument, we don't want a line number then */
	if (sourcing_lnum != 0)
	{
	    sprintf((char *)Buf, "line %4ld:", sourcing_lnum);
	    msg_attr(Buf, hl_attr(HLF_N));
	}
	--no_wait_return;
	last_lnum = sourcing_lnum;  /* only once for each line */
	vim_free(Buf);
    }

    /* remember the last sourcing name printed, also when it's empty */
    if (sourcing_name == NULL || other_sourcing_name)
    {
	vim_free(last_sourcing_name);
	if (sourcing_name == NULL)
	    last_sourcing_name = NULL;
	else
	    last_sourcing_name = vim_strsave(sourcing_name);
    }
    msg_nowait = FALSE;			/* wait for this msg */

#ifdef WANT_EVAL
    set_internal_string_var((char_u *)"errmsg", s);
#endif
    return msg_attr(s, attr);
}

    int
emsg2(s, a1)
    char_u *s, *a1;
{
    if (emsg_off)		/* no error messages at the moment */
	return TRUE;

    /* Check for NULL strings (just in case) */
    if (a1 == NULL)
	a1 = (char_u *)"[NULL]";
    /* Check for very long strings (can happen with ":help ^A<CR>") */
    if (STRLEN(s) + STRLEN(a1) >= (size_t)IOSIZE)
	a1 = (char_u *)"[string too long]";
    sprintf((char *)IObuff, (char *)s, (char *)a1);
    return emsg(IObuff);
}

    int
emsgn(s, n)
    char_u *s;
    long    n;
{
    if (emsg_off)		/* no error messages at the moment */
	return TRUE;
    sprintf((char *)IObuff, (char *)s, n);
    return emsg(IObuff);
}

/*
 * Like msg(), but truncate to a single line if p_shm contains 't', or when
 * "force" is TRUE.
 * Careful: The string may be changed!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    char_u *
msg_trunc_attr(s, force, attr)
    char_u	*s;
    int		force;
    int		attr;
{
    int		n;

    if ((force || shortmess(SHM_TRUNC)) && (n = (int)STRLEN(s) -
		    (int)(Rows - cmdline_row - 1) * Columns - sc_col + 1) > 0)
    {
	s += n;
	*s = '<';
    }
    if (msg_attr(s, attr))
	return s;
    return NULL;
}

/*
 * wait for the user to hit a key (normally a return)
 * if 'redraw' is TRUE, clear and redraw the screen
 * if 'redraw' is FALSE, just redraw the screen
 * if 'redraw' is -1, don't redraw at all
 */
    void
wait_return(redraw)
    int	    redraw;
{
    int		    c;
    int		    oldState;
    int		    tmpState;

    if (redraw == TRUE)
	must_redraw = CLEAR;

/*
 * With the global command (and some others) we only need one return at the
 * end. Adjust cmdline_row to avoid the next message overwriting the last one.
 * When inside vgetc(), we can't wait for a typed character at all.
 */
    if (vgetc_busy)
	return;
    if (no_wait_return)
    {
	need_wait_return = TRUE;
	if (!exmode_active)
	    cmdline_row = msg_row;
	return;
    }

    redir_off = TRUE;		    /* don't redirect this message */
    oldState = State;
    if (quit_more)
    {
	c = CR;			    /* just pretend CR was hit */
	quit_more = FALSE;
	got_int = FALSE;
    }
    else if (exmode_active)
    {
	MSG_PUTS(" ");	  /* make sure the cursor is on the right line */
	c = CR;			    /* no need for a return in ex mode */
	got_int = FALSE;
    }
    else
    {
	State = HITRETURN;
#ifdef USE_MOUSE
	setmouse();
#endif
	if (msg_didout)		    /* start on a new line */
	    msg_putchar('\n');
	if (got_int)
	    MSG_PUTS("Interrupt: ");

#ifdef ORG_HITRETURN
	MSG_PUTS_ATTR("Press RETURN to continue", hl_attr(HLF_R));
	do {
	    c = vgetc();
	} while (vim_strchr((char_u *)"\r\n: ", c) == NULL);
	if (c == ':')			/* this can vi too (but not always!) */
	    stuffcharReadbuff(c);
#else
	MSG_PUTS_ATTR("Press RETURN or enter command to continue",
							      hl_attr(HLF_R));
	if (!msg_use_printf())
	    msg_clr_eos();
	do
	{
	    c = vgetc();
	    if (!global_busy)
		got_int = FALSE;
	} while (c == Ctrl('C')
#ifdef USE_GUI
				|| c == K_SCROLLBAR || c == K_HORIZ_SCROLLBAR
#endif
#ifdef USE_MOUSE
				|| c == K_LEFTDRAG   || c == K_LEFTRELEASE
				|| c == K_MIDDLEDRAG || c == K_MIDDLERELEASE
				|| c == K_RIGHTDRAG  || c == K_RIGHTRELEASE
				|| c == K_IGNORE     ||
				(!mouse_has(MOUSE_RETURN) &&
				     (c == K_LEFTMOUSE ||
				      c == K_MIDDLEMOUSE ||
				      c == K_RIGHTMOUSE))
#endif
				);
	ui_breakcheck();
#ifdef USE_MOUSE
	/*
	 * Avoid that the mouse-up event causes visual mode to start.
	 */
	if (c == K_LEFTMOUSE || c == K_MIDDLEMOUSE || c == K_RIGHTMOUSE)
	    jump_to_mouse(MOUSE_SETPOS, NULL);
	else
#endif
	    if (vim_strchr((char_u *)"\r\n ", c) == NULL)
	{
	    stuffcharReadbuff(c);
	    do_redraw = TRUE;	    /* need a redraw even though there is
				       something in the stuff buffer */
	}
#endif
    }
    redir_off = FALSE;

    /*
     * If the user hits ':', '?' or '/' we get a command line from the next
     * line.
     */
    if (c == ':' || c == '?' || c == '/')
    {
	if (!exmode_active)
	    cmdline_row = msg_row;
	skip_redraw = TRUE;	    /* skip redraw once */
	do_redraw = FALSE;
    }

/*
 * If the window size changed set_winsize() will redraw the screen.
 * Otherwise the screen is only redrawn if 'redraw' is set and no ':' typed.
 */
    tmpState = State;
    State = oldState;		    /* restore State before set_winsize */
#ifdef USE_MOUSE
    setmouse();
#endif
    msg_check();

    /*
     * When switching screens, we need to output an extra newline on exit.
     */
#ifdef UNIX
    if (swapping_screen() && !termcap_active)
	newline_on_exit = TRUE;
#endif

    need_wait_return = FALSE;
    emsg_on_display = FALSE;	/* can delete error message now */
    msg_didany = FALSE;		/* reset lines_left at next msg_start() */
    lines_left = -1;
    if (keep_msg != NULL && linetabsize(keep_msg) >=
				  (Rows - cmdline_row - 1) * Columns + sc_col)
	keep_msg = NULL;	    /* don't redisplay message, it's too long */

    if (tmpState == SETWSIZE)	    /* got resize event while in vgetc() */
    {
	starttermcap();		    /* start termcap before redrawing */
	set_winsize(0, 0, FALSE);
    }
    else if (!skip_redraw && (redraw == TRUE || (msg_scrolled && redraw != -1)))
    {
	starttermcap();		    /* start termcap before redrawing */
	update_screen(VALID);
    }

    dont_wait_return = TRUE;	    /* don't wait again in main() */
}

/*
 * Prepare for outputting characters in the command line.
 */
    void
msg_start()
{
    int		did_return = FALSE;

    keep_msg = NULL;			    /* don't display old message now */
    if (!msg_scroll && full_screen)	    /* overwrite last message */
    {
	msg_row = cmdline_row;
	msg_col = 0;
    }
    else if (msg_didout)		    /* start message on next line */
    {
	msg_putchar('\n');
	did_return = TRUE;
	if (!exmode_active)
	    cmdline_row = msg_row;
    }
    if (!msg_didany)
	lines_left = cmdline_row;
    msg_didout = FALSE;			    /* no output on current line yet */
    cursor_off();

    /* when redirecting, may need to start a new line. */
    if (!did_return)
	redir_write((char_u *)"\n");
}

    void
msg_putchar(c)
    int		c;
{
    msg_putchar_attr(c, 0);
}

    void
msg_putchar_attr(c, attr)
    int		c;
    int		attr;
{
    char_u	buf[4];

    if (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    else
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    msg_puts_attr(buf, attr);
}

    void
msg_outnum(n)
    long	n;
{
    char_u	buf[20];

    sprintf((char *)buf, "%ld", n);
    msg_puts(buf);
}

    void
msg_home_replace(fname)
    char_u	*fname;
{
    msg_home_replace_attr(fname, 0);
}

    void
msg_home_replace_hl(fname)
    char_u	*fname;
{
    msg_home_replace_attr(fname, hl_attr(HLF_D));
}

    static void
msg_home_replace_attr(fname, attr)
    char_u  *fname;
    int	    attr;
{
    char_u	*name;

    name = home_replace_save(NULL, fname);
    if (name != NULL)
	msg_outtrans_attr(name, attr);
    vim_free(name);
}

/*
 * Output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output upto a NUL character.
 * Use attributes 'attr'.
 * Return the number of characters it takes on the screen.
 */
    int
msg_outtrans(str)
    char_u	    *str;
{
    return msg_outtrans_attr(str, 0);
}

    int
msg_outtrans_attr(str, attr)
    char_u	*str;
    int		attr;
{
    return msg_outtrans_len_attr(str, (int)STRLEN(str), attr);
}

    int
msg_outtrans_len(str, len)
    char_u	*str;
    int		len;
{
    return msg_outtrans_len_attr(str, len, 0);
}
    int
msg_outtrans_len_attr(str, len, attr)
    char_u	*str;
    int		len;
    int		attr;
{
    int retval = 0;

    while (--len >= 0)
    {
	msg_puts_attr(transchar(*str), attr);
	retval += charsize(*str);
	++str;
    }
    return retval;
}

    void
msg_make(arg)
    char_u  *arg;
{
    int	    i;
    static char_u *str = (char_u *)"eeffoc", *rs = (char_u *)"Plon#dqg#vxjduB";

    arg = skipwhite(arg);
    for (i = 5; *arg && i >= 0; --i)
	if (*arg++ != str[i])
	    break;
    if (i < 0)
    {
	msg_putchar('\n');
	for (i = 0; rs[i]; ++i)
	    msg_putchar(rs[i] - 3);
    }
}

/*
 * Output the string 'str' upto a NUL character.
 * Return the number of characters it takes on the screen.
 *
 * If K_SPECIAL is encountered, then it is taken in conjunction with the
 * following character and shown as <F1>, <S-Up> etc.  In addition, if 'all'
 * is TRUE, then any other character which has its 8th bit set is shown as
 * <M-x>, where x is the equivalent character without its 8th bit set.	If a
 * character is displayed in one of these special ways, is also highlighted
 * (its highlight name is '8' in the p_hl variable).
 * Otherwise characters are not highlighted.
 * This function is used to show mappings, where we want to see how to type
 * the character/string -- webb
 */
    int
msg_outtrans_special(str, all)
    char_u	*str;
    int		all;	/* <M-a> etc as well as <F1> etc */
{
    int	    retval = 0;
    char_u  *string;
    int	    c;
    int	    modifiers;
    int	    attr;

    attr = hl_attr(HLF_8);
    for (; *str; ++str)
    {
	c = *str;
	if (c == K_SPECIAL && str[1] != NUL && str[2] != NUL)
	{
	    modifiers = 0x0;
	    if (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;
		c = *str;
	    }
	    if (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
		if (c == K_ZERO)	/* display <Nul> as ^@ */
		    c = NUL;
	    }
	    if (IS_SPECIAL(c) || modifiers)	/* special key */
	    {
		string = get_special_key_name(c, modifiers);
		msg_puts_attr(string, attr);
		retval += STRLEN(string);
		continue;
	    }
	}
	/* output unprintable meta characters, and <M-Space> */
	if (all && (((c & 0x80) && (!vim_isprintc(c) || c == 0xa0))
		    || c == ' '))
	{
	    string = get_special_key_name(c, 0);
	    msg_puts_attr(string, attr);
	    retval += STRLEN(string);
	}
	else
	{
	    msg_puts(transchar(c));
	    retval += charsize(c);
	}
    }
    return retval;
}

/*
 * print line for :print or :list command
 */
    void
msg_prt_line(s)
    char_u	*s;
{
    int		c;
    int		col = 0;

    int		n_extra = 0;
    int		c_extra = 0;
    char_u	*p_extra = NULL;	    /* init to make SASC shut up */
    int		n;
    int		attr= 0;
    char_u	*trail = NULL;

    /* find start of trailing whitespace */
    if (curwin->w_p_list && lcs_trail)
    {
	trail = s + STRLEN(s);
	while (trail > s && vim_iswhite(trail[-1]))
	    --trail;
    }

    /* output a space for an empty line, otherwise the line will be
     * overwritten */
    if (*s == NUL && !curwin->w_p_list)
	msg_putchar(' ');

    for (;;)
    {
	if (n_extra)
	{
	    --n_extra;
	    if (c_extra)
		c = c_extra;
	    else
		c = *p_extra++;
	}
	else
	{
	    attr = 0;
	    c = *s++;
	    if (c == TAB && (!curwin->w_p_list || lcs_tab1))
	    {
		/* tab amount depends on current column */
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
		if (!curwin->w_p_list)
		{
		    c = ' ';
		    c_extra = ' ';
		}
		else
		{
		    c = lcs_tab1;
		    c_extra = lcs_tab2;
		    attr = hl_attr(HLF_AT);
		}
	    }
	    else if (c == NUL && curwin->w_p_list && lcs_eol)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		n_extra = 1;
		c = lcs_eol;
		attr = hl_attr(HLF_AT);
		--s;
	    }
	    else if (c != NUL && (n = charsize(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar(c);
		c_extra = NUL;
		c = *p_extra++;
	    }
	    else if (c == ' ' && trail != NULL && s > trail)
	    {
		c = lcs_trail;
		attr = hl_attr(HLF_AT);
	    }
	}

	if (c == NUL)
	    break;

	msg_putchar_attr(c, attr);
	col++;
    }
    msg_clr_eos();
}

/*
 * Output a string to the screen at position msg_row, msg_col.
 * Update msg_row and msg_col for the next message.
 */
    void
msg_puts(s)
    char_u	*s;
{
    msg_puts_attr(s, 0);
}

    void
msg_puts_title(s)
    char_u	*s;
{
    msg_puts_attr(s, hl_attr(HLF_T));
}

/*
 * if printing a string will exceed the screen width, print "..." in the
 * middle.
 */
    void
msg_puts_long(longstr)
    char_u	*longstr;
{
    msg_puts_long_len(longstr, (int)strlen((char *)longstr));
}

    void
msg_puts_long_attr(longstr, attr)
    char_u	*longstr;
    int		attr;
{
    msg_puts_long_len_attr(longstr, (int)strlen((char *)longstr), attr);
}

    void
msg_puts_long_len(longstr, len)
    char_u	*longstr;
    int		len;
{
    msg_puts_long_len_attr(longstr, len, 0);
}

    void
msg_puts_long_len_attr(longstr, len, attr)
    char_u	*longstr;
    int		len;
    int		attr;
{
    int		slen = len;
    int		room;

    room = Columns - msg_col;
    if (len > room && room >= 20)
    {
	slen = (room - 3) / 2;
	msg_outtrans_len_attr(longstr, slen, attr);
	msg_puts_attr((char_u *)"...", hl_attr(HLF_AT));
    }
    msg_outtrans_len_attr(longstr + len - slen, slen, attr);
}

    void
msg_puts_attr(s, attr)
    char_u	*s;
    int		attr;
{
    int		oldState;
    char_u	buf[20];
    char_u	*p;

    dont_wait_return = FALSE;	/* may call wait_return() in main() */

    /*
     * If redirection is on, also write to the redirection file.
     */
    redir_write(s);

    /*
     * If there is no valid screen, use fprintf so we can see error messages.
     * If termcap is not active, we may be writing in an alternate console
     * window, cursor positioning may not work correctly (window size may be
     * different, e.g. for WIN32 console) or we just don't know where the
     * cursor is.
     */
    if (msg_use_printf())
    {
#ifdef WIN32
	if (!silent_mode)
	    mch_settmode(TMODE_COOK);	/* handle '\r' and '\n' correctly */
#endif
	while (*s)
	{
	    if (!silent_mode)
	    {
		p = &buf[0];
		if (*s == '\n')	/* NL --> CR NL translation (for Unix) */
		    *p++ = '\r';
		*p++ = *s;
		*p = '\0';
		mch_errmsg((char *)buf);
	    }

	    /* primitive way to compute the current column */
	    if (*s == '\r' || *s == '\n')
		msg_col = 0;
	    else
		++msg_col;
	    ++s;
	}
	msg_didout = TRUE;	    /* assume that line is not empty */

#ifdef WIN32
	if (!silent_mode)
	    mch_settmode(TMODE_RAW);
#endif
	return;
    }

    msg_didany = TRUE;		/* remember that something was outputted */
    while (*s)
    {
	/*
	 * The screen is scrolled up when:
	 * - When outputting a newline in the last row
	 * - when outputting a character in the last column of the last row
	 *   (some terminals scroll automatically, some don't. To avoid
	 *   problems we scroll ourselves)
	 */
	if (msg_row >= Rows - 1 && (*s == '\n' || msg_col >= Columns - 1 ||
			      (*s == TAB && msg_col >= ((Columns - 1) & ~7))))
	{
	    screen_del_lines(0, 0, 1, (int)Rows, TRUE);	/* always works */
	    msg_row = Rows - 2;
	    if (msg_col >= Columns)	/* can happen after screen resize */
		msg_col = Columns - 1;
	    ++msg_scrolled;
	    need_wait_return = TRUE;	/* may need wait_return in main() */
	    if (cmdline_row > 0 && !exmode_active)
		--cmdline_row;
	    /*
	     * if screen is completely filled wait for a character
	     */
	    if (p_more && --lines_left == 0 && State != HITRETURN &&
							       !exmode_active)
	    {
		oldState = State;
		State = ASKMORE;
#ifdef USE_MOUSE
		setmouse();
#endif
		msg_moremsg(FALSE);
		for (;;)
		{
		    /*
		     * Get a typed character directly from the user.
		     * Don't use vgetc(), it syncs undo and eats mapped
		     * characters.  Disadvantage: Special keys and mouse
		     * cannot be used here, typeahead is ignored.
		     */
		    out_flush();
		    (void)ui_inchar(buf, 20, -1L);
		    switch (buf[0])
		    {
		    case CR:		/* one extra line */
		    case NL:
			lines_left = 1;
			break;
		    case ':':		/* start new command line */
			stuffcharReadbuff(':');
			cmdline_row = Rows - 1;	    /* put ':' on this line */
			skip_redraw = TRUE;	    /* skip redraw once */
			dont_wait_return = TRUE;    /* don't wait in main() */
			/*FALLTHROUGH*/
		    case 'q':		/* quit */
		    case Ctrl('C'):
		    case ESC:
			got_int = TRUE;
			quit_more = TRUE;
			break;
		    case 'd':		/* Down half a page */
			lines_left = Rows / 2;
			break;
		    case ' ':		/* one extra page */
			lines_left = Rows - 1;
			break;
		    default:		/* no valid response */
#ifdef UNIX
			if (buf[0] == intr_char)
			{
			    got_int = TRUE;
			    quit_more = TRUE;
			    break;
			}
#endif
			msg_moremsg(TRUE);
			continue;
		    }
		    break;
		}
		/* clear the --more-- message */
		screen_fill((int)Rows - 1, (int)Rows,
						0, (int)Columns, ' ', ' ', 0);
		State = oldState;
#ifdef USE_MOUSE
		setmouse();
#endif
		if (quit_more)
		{
		    msg_row = Rows - 1;
		    msg_col = 0;
		    return;	    /* the string is not displayed! */
		}
	    }
	}
	if (*s == '\n')		    /* go to next line */
	{
	    msg_didout = FALSE;	    /* remember that line is empty */
	    msg_col = 0;
	    if (++msg_row >= Rows)  /* safety check */
		msg_row = Rows - 1;
	}
	else if (*s == '\r')	    /* go to column 0 */
	{
	    msg_col = 0;
	}
	else if (*s == '\b')	    /* go to previous char */
	{
	    if (msg_col)
		--msg_col;
	}
	else if (*s == TAB)	    /* translate into spaces */
	{
	    do
		msg_screen_putchar(' ', attr);
	    while (msg_col & 7);
	}
	else
	    msg_screen_putchar(*s, attr);
	++s;
    }
}

/*
 * Returns TRUE when messages should be printed to stderr.
 * This is used when there is no valid screen, so we can see error messages.
 * If termcap is not active, we may be writing in an alternate console
 * window, cursor positioning may not work correctly (window size may be
 * different, e.g. for WIN32 console) or we just don't know where the
 * cursor is.
 */
    static int
msg_use_printf()
{
    return (!msg_check_screen()
#ifdef WIN32
	    || !termcap_active
#endif
	    || (swapping_screen() && !termcap_active)
	       );
}

    static void
msg_screen_putchar(c, attr)
    int	    c;
    int	    attr;
{
    msg_didout = TRUE;	    /* remember that line is not empty */
    screen_putchar(c, msg_row, msg_col, attr);
    if (++msg_col >= Columns)
    {
	msg_col = 0;
	++msg_row;
    }
}

    void
msg_moremsg(full)
    int	    full;
{
    int	    attr;

    attr = hl_attr(HLF_M);
    screen_puts((char_u *)"-- More --", (int)Rows - 1, 0, attr);
    if (full)
	screen_puts((char_u *)
		" (RET: line, SPACE: page, d: half page, q: quit)",
		(int)Rows - 1, 10, attr);
}

/*
 * msg_check_screen - check if the screen is initialized.
 * Also check msg_row and msg_col, if they are too big it may cause a crash.
 * While starting the GUI the terminal codes will be set for the GUI, but the
 * output goes to the terminal.  Don't use the terminal codes then.
 */
    static int
msg_check_screen()
{
    if (!full_screen || !screen_valid(FALSE))
	return FALSE;

    if (msg_row >= Rows)
	msg_row = Rows - 1;
    if (msg_col >= Columns)
	msg_col = Columns - 1;
    return TRUE;
}

/*
 * clear from current message position to end of screen
 * Note: msg_col is not updated, so we remember the end of the message
 * for msg_check().
 */
    void
msg_clr_eos()
{
    if (!msg_check_screen()
#ifdef WIN32
	    || !termcap_active
#endif
	    || (swapping_screen() && !termcap_active)
						)
    {
	if (full_screen)	/* only when termcap codes are valid */
	{
	    if (*T_CD)
		out_str(T_CD);	/* clear to end of display */
	    else if (*T_CE)
		out_str(T_CE);	/* clear to end of line */
	}
    }
    else
    {
	screen_fill(msg_row, msg_row + 1, msg_col, (int)Columns, ' ', ' ', 0);
	screen_fill(msg_row + 1, (int)Rows, 0, (int)Columns, ' ', ' ', 0);
    }
}

/*
 * Clear the command line.
 */
    void
msg_clr_cmdline()
{
    msg_row = cmdline_row;
    msg_col = 0;
    msg_clr_eos();
}

/*
 * end putting a message on the screen
 * call wait_return if the message does not fit in the available space
 * return TRUE if wait_return not called.
 */
    int
msg_end()
{
    /*
     * if the string is larger than the window,
     * or the ruler option is set and we run into it,
     * we have to redraw the window.
     * Do not do this if we are abandoning the file or editing the command line.
     */
    if (!exiting && msg_check() && State != CMDLINE)
    {
	wait_return(FALSE);
	return FALSE;
    }
    out_flush();
    return TRUE;
}

/*
 * If the written message has caused the screen to scroll up, or if we
 * run into the shown command or ruler, we have to redraw the window later.
 */
    int
msg_check()
{
    if (msg_scrolled || (msg_row == Rows - 1 && msg_col >= sc_col))
    {
	redraw_all_later(NOT_VALID);
	redraw_cmdline = TRUE;
	return TRUE;
    }
    return FALSE;
}

/*
 * May write a string to the redirection file.
 */
    static void
redir_write(s)
    char_u	*s;
{
    static int	    cur_col = 0;

    if (redir_fd != NULL && !redir_off)
    {
	/* If the string doesn't start with CR or NL, go to msg_col */
	if (*s != '\n' && *s != '\r')
	{
	    while (cur_col < msg_col)
	    {
		fputs(" ", redir_fd);
		++cur_col;
	    }
	}

	fputs((char *)s, redir_fd);

	/* Adjust the current column */
	while (*s)
	{
	    if (*s == '\r' || *s == '\n')
		cur_col = 0;
	    else if (*s == '\t')
		cur_col += (8 - cur_col % 8);
	    else
		++cur_col;
	    ++s;
	}
    }
}

/*
 * Give a warning message (for searching).
 * Use 'w' highlighting and may repeat the message after redrawing
 */
    void
give_warning(message, hl)
    char_u  *message;
    int	    hl;
{
    keep_msg = NULL;
    if (hl)
	keep_msg_attr = hl_attr(HLF_W);
    else
	keep_msg_attr = 0;
    if (msg_attr(message, keep_msg_attr) && !msg_scrolled)
	keep_msg = message;
    msg_didout = FALSE;	    /* overwrite this message */
    msg_nowait = TRUE;	    /* don't wait for this message */
    msg_col = 0;
}

/*
 * Advance msg cursor to column "col".
 */
    void
msg_advance(col)
    int	    col;
{
    if (col >= Columns)		/* not enough room */
	col = Columns - 1;
    while (msg_col < col)
	msg_putchar(' ');
}

#if defined(CON_DIALOG) || defined(PROTO)
/*
 * Used for "confirm()" function, and the :confirm command prefix.
 * Versions which haven't got flexible dialogs yet, and console
 * versions, get this generic handler which uses the command line.
 *
 * type  = one of:
 *	   VIM_QUESTION, VIM_INFO, VIM_WARNING, VIM_ERROR or VIM_GENERIC
 * title = title string (can be NULL for default)
 * (neither used in console dialogs at the moment)
 *
 * Format of the "buttons" string:
 * "Button1Name\nButton2Name\nButton3Name"
 * The first button should normally be the default/accept
 * The second button should be the 'Cancel' button
 * Other buttons- use your imagination!
 * A '&' in a button name becomes a shortcut, so each '&' should be before a
 * different letter.
 */
/* ARGSUSED */
    int
do_dialog(type, title, message, buttons, dfltbutton)
    int		type;
    char_u	*title;
    char_u	*message;
    char_u	*buttons;
    int		dfltbutton;
{
    int		oldState;
    char_u	buf[20];	/* for getting keystrokes */
    int		retval = 0;
    char_u	*hotkeys;

#ifndef NO_CONSOLE
    /* Don't output anything in silent mode ("ex -s") */
    if (silent_mode)
	return dfltbutton;   /* return default option */
#endif

#ifdef GUI_DIALOG
    /* When GUI is running, use the GUI dialog */
    if (gui.in_use)
	return gui_mch_dialog(type, title, message, buttons, dfltbutton);
#endif

    oldState = State;
    State = CONFIRM;
#ifdef USE_MOUSE
    setmouse();
#endif

    /*
     * Since we wait for a keypress, don't make the
     * user press RETURN as well afterwards.
     */
    ++no_wait_return;
    hotkeys = msg_show_console_dialog(message, buttons, dfltbutton);

    if (hotkeys != NULL)
    {
	for (;;)
	{
	    /*
	     * Get a typed character directly from the user.
	     * Don't use vgetc(), it syncs undo and eats mapped
	     * characters.  Disadvantage: Special keys and mouse
	     * cannot be used here, typeahead is ignored.
	     */
	    cursor_on();
	    out_flush();
	    (void)ui_inchar(buf, 20, -1L);
	    switch (buf[0])
	    {
	    case CR:		/* User accepts default option */
	    case NL:
		retval = dfltbutton;
		break;
	    case Ctrl('C'):		/* User aborts/cancels */
	    case ESC:
		retval = 0;
		break;
	    default:		/* Could be a hotkey? */
#ifdef UNIX
		if (buf[0] == intr_char)
		{
		    retval = 0;	/* another way of cancelling */
		    break;
		}
#endif
		for (retval = 0; hotkeys[retval]; retval++)
		{
		    if (hotkeys[retval] == TO_LOWER(buf[0]))
			break;
		}
		if (hotkeys[retval])
		{
		    retval++;
		    break;
		}
		/* No hotkey match, so keep waiting */
		continue;
	    }
	    break;
	}

	vim_free(hotkeys);
    }

    State = oldState;
#ifdef USE_MOUSE
    setmouse();
#endif
    --no_wait_return;
    need_wait_return = FALSE;
    dont_wait_return = TRUE;	    /* don't wait again in main() */

    return retval;
}

char_u	*confirm_msg = NULL;	    /* ":confirm" message */

/*
 * Format the dialog string, and display it at the bottom of
 * the screen. Return a string of hotkey chars (if defined) for
 * each 'button'. If a button has no hotkey defined, the string
 * has the buttons first letter.
 *
 * Returns allocated array, or NULL for error.
 *
 */
    static char_u *
msg_show_console_dialog(message, buttons, dfltbutton)
    char_u	*message;
    char_u	*buttons;
    int		dfltbutton;
{
    int		len = 0;
    int		lenhotkey = 1;	/*first button*/
    char_u	*hotk;
    char_u	*p;
    char_u	*q;
    char_u	*r;

    /*
     * First compute how long a string we need to allocate for the message.
     */
    r = buttons;
    while (*r)
    {
	if (*r == DLG_BUTTON_SEP)
	{
	    len++;	    /* '\n' -> ', ' */
	    lenhotkey++;    /* each button needs a hotkey */
	}
	else if (*r == DLG_HOTKEY_CHAR)
	{
	    len++;	    /* '&a' -> '[a]' */
	}
	r++;
    }

    len += STRLEN(message)
	    + 2			/* for the NL's */
	    + STRLEN(buttons)
	    + 3;		/* for the ": " and NUL */

    lenhotkey++;		/* for the NUL */

    /*
     * Now allocate and load the strings
     */
    vim_free(confirm_msg);
    confirm_msg = alloc(len);
    if (confirm_msg == NULL)
	return NULL;
    *confirm_msg = NUL;
    hotk = alloc(lenhotkey);
    if (hotk == NULL)
	return NULL;

    *confirm_msg = '\n';
    STRCPY(confirm_msg + 1, message);

    p = confirm_msg + 1 + STRLEN(message);
    q = hotk;
    r = buttons;
    *q = (char_u)TO_LOWER(*r);	/* define lowercase hotkey */

    *p++ = '\n';

    while (*r)
    {
	if (*r == DLG_BUTTON_SEP)
	{
	    *p++ = ',';
	    *p++ = ' ';	    /* '\n' -> ', ' */
	    *(++q) = (char_u)TO_LOWER(*(r + 1)); /* next hotkey */
	    if (dfltbutton)
		--dfltbutton;
	}
	else if (*r == DLG_HOTKEY_CHAR)
	{
	    r++;
	    if (*r == DLG_HOTKEY_CHAR)		/* duplicate magic = literal */
		*p++ = *r;
	    else
	    {
		/* '&a' -> '[a]' */
		*p++ = (dfltbutton == 1) ? '[' : '(';
		*p++ = *r;
		*p++ = (dfltbutton == 1) ? ']' : ')';
		*q = (char_u)TO_LOWER(*r);	/* define lowercase hotkey */
	    }
	}
	else
	{
	    *p++ = *r;	    /* everything else copy literally */
	}
	r++;
    }
    *p++ = ':';
    *p++ = ' ';
    *p = NUL;
    *(++q) = NUL;

    display_confirm_msg();
    return hotk;
}

/*
 * Display the ":confirm" message.  Also called when screen resized.
 */
    void
display_confirm_msg()
{
    if (confirm_msg != NULL)
	msg_puts_attr(confirm_msg, hl_attr(HLF_M));
}

#endif /* CON_DIALOG */

#if defined(CON_DIALOG) || defined(GUI_DIALOG)

/*
 * Various stock dialogs used throughout Vim when :confirm is used.
 */
#if 0	/* not used yet */
    void
vim_dialog_ok(type, title, message)
    int		type;
    char_u	*title;
    char_u	*message;
{
    (void)do_dialog(type,
			  title == NULL ? (char_u *)"Information" : title,
			  message,
			  (char_u *)"&OK", 1);
}
#endif

#if 0	/* not used yet */
    int
vim_dialog_okcancel(type, title, message, dflt)
    int		type;
    char_u	*title;
    char_u	*message;
    int		dflt;
{
    if (do_dialog(type,
		title == NULL ? (char_u *)"Confirmation" : title,
		message,
		(char_u *)"&OK\n&Cancel", dflt) == 1)
	return VIM_OK;
    return VIM_CANCEL;
}
#endif

    int
vim_dialog_yesno(type, title, message, dflt)
    int		type;
    char_u	*title;
    char_u	*message;
    int		dflt;
{
    if (do_dialog(type,
		title == NULL ? (char_u *)"Question" : title,
		message,
		(char_u *)"&Yes\n&No", dflt) == 1)
	return VIM_YES;
    return VIM_NO;
}

    int
vim_dialog_yesnocancel(type, title, message, dflt)
    int		type;
    char_u	*title;
    char_u	*message;
    int		dflt;
{
    switch (do_dialog(type,
		title == NULL ? (char_u *)"Question" : title,
		message,
		(char_u *)"&Yes\n&No\n&Cancel", dflt))
    {
	case 1: return VIM_YES;
	case 2: return VIM_NO;
    }
    return VIM_CANCEL;
}

    int
vim_dialog_yesnoallcancel(type, title, message, dflt)
    int		type;
    char_u	*title;
    char_u	*message;
    int		dflt;
{
    switch (do_dialog(type,
		title == NULL ? (char_u *)"Question" : title,
		message,
		(char_u *)"&Yes\n&No\nSave &All\n&Discard All\n&Cancel", dflt))
    {
	case 1: return VIM_YES;
	case 2: return VIM_NO;
	case 3: return VIM_ALL;
	case 4: return VIM_DISCARDALL;
    }
    return VIM_CANCEL;
}

#endif /* GUI_DIALOG || CON_DIALOG */

#if defined(USE_BROWSE) || defined(PROTO)
/*
 * Generic browse function.  Calls gui_mch_browse() when possible.
 * Later this may pop-up a non-GUI file selector (external command?).
 */
    char_u *
do_browse(saving, title, dflt, ext, initdir, filter, buf)
    int		saving;		/* write action */
    char_u	*title;		/* title for the window */
    char_u	*dflt;		/* default file name */
    char_u	*ext;		/* extension added */
    char_u	*initdir;	/* initial directory, NULL for current dir */
    char_u	*filter;	/* file name filter */
    BUF		*buf;		/* buffer to read/write for */
{
    char_u		*fname;
    static char_u	*last_dir = NULL;    /* last used directory */
    char_u		*tofree = NULL;


    /* Must turn off browse straight away, or :so autocommands will get the
     * flag too!  */
    browse = FALSE;

    if (title == NULL)
    {
	if (saving)
	    title = (char_u *)"Save File dialog";
	else
	    title = (char_u *)"Open File dialog";
    }

    /* When no directory specified, use buffer dir, last dir or current dir */
    if (initdir == NULL || *initdir == NUL)
    {
	/* When saving or 'browsedir' is "buffer", use buffer fname */
	if ((saving || *p_bsdir == 'b') && buf != NULL && buf->b_ffname != NULL)
	{
	    dflt = gettail(curbuf->b_ffname);
	    tofree = vim_strsave(curbuf->b_ffname);
	    if (tofree != NULL)
	    {
		initdir = tofree;
		*gettail(initdir) = NUL;
	    }
	}
	/* When 'browsedir' is "last", use dir from last browse */
	else if (*p_bsdir == 'l')
	    initdir = last_dir;
	/* When 'browsedir is "current", use current directory.  This is the
	 * default already, leave initdir empty. */
    }

# ifdef USE_GUI
    if (gui.in_use)		/* when this changes, also adjust f_has()! */
    {
	fname = gui_mch_browse(saving, title, dflt, ext, initdir, filter);
    }
    else
# endif
    {
	/* TODO: non-GUI file selector here */
	fname = NULL;
    }

    /* keep the directory for next time */
    if (fname != NULL)
    {
	vim_free(last_dir);
	last_dir = vim_strsave(fname);
	if (last_dir != NULL)
	{
	    *gettail(last_dir) = NUL;
	    if (*last_dir == NUL)
	    {
		/* filename only returned, must be in current dir*/
		vim_free(last_dir);
		last_dir = alloc(MAXPATHL);
		if (last_dir != NULL)
		    mch_dirname(last_dir, MAXPATHL);
	    }
	}
    }

    vim_free(tofree);

    return fname;
}
#endif
