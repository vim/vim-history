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
    attr = highlight_attr[HLF_E];   /* set highlight mode for error messages */
    if (msg_scrolled)
	need_wait_return = TRUE;    /* needed in case emsg() is called after
				     * wait_return has reset need_wait_return
				     * and a redraw is expected because
				     * msg_scrolled is non-zero */

/*
 * First output name and line number of source of error message
 */
    if (sourcing_name != NULL &&
	   (sourcing_name != last_sourcing_name || sourcing_lnum != last_lnum)
				      && (Buf = alloc(MAXPATHL + 30)) != NULL)
    {
	++no_wait_return;
	if (sourcing_name != last_sourcing_name)
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
	    msg_attr(Buf, highlight_attr[HLF_N]);
	}
	--no_wait_return;
	last_lnum = sourcing_lnum;  /* only once for each line */
	vim_free(Buf);
    }
    last_sourcing_name = sourcing_name;	/* do this also when it is NULL */
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
 * Like msg(), but truncate to a single line if p_shm contains 't'.
 * Careful: The string may be changed!
 * Returns a pointer to the printed message, if wait_return() not called.
 */
    char_u *
msg_trunc(s)
    char_u  *s;
{
    int	    n;

    if (shortmess(SHM_TRUNC) && (n = (int)STRLEN(s) -
		    (int)(Rows - cmdline_row - 1) * Columns - sc_col + 1) > 0)
    {
	s += n;
	*s = '<';
    }
    if (msg(s))
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
	MSG_PUTS_ATTR("Press RETURN to continue", highlight_attr[HLF_R]);
	do {
	    c = vgetc();
	} while (vim_strchr((char_u *)"\r\n: ", c) == NULL);
	if (c == ':')			/* this can vi too (but not always!) */
	    stuffcharReadbuff(c);
#else
	MSG_PUTS_ATTR("Press RETURN or enter command to continue",
						       highlight_attr[HLF_R]);
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

/* msg_putchar_attr() doesn't exist, use msg_puts_attr() instead */

    void
msg_putchar(c)
    int	    c;
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
    msg_puts(buf);
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
    msg_home_replace_attr(fname, highlight_attr[HLF_D]);
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

    attr = highlight_attr[HLF_8];
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
	if (all && (c & 0x80) && !vim_isprintc(c))
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
    int		si = 0;
    int		c;
    int		col = 0;

    int		n_extra = 0;
    int		n_spaces = 0;
    char_u	*p = NULL;	    /* init to make SASC shut up */
    int		n;

    /* output a space for an empty line, otherwise the line will be
     * overwritten */
    if (*s == NUL && !curwin->w_p_list)
	msg_putchar(' ');

    for (;;)
    {
	if (n_extra)
	{
	    --n_extra;
	    c = *p++;
	}
	else if (n_spaces)
	{
	    --n_spaces;
	    c = ' ';
	}
	else
	{
	    c = s[si++];
	    if (c == TAB && !curwin->w_p_list)
	    {
		/* tab amount depends on current column */
		n_spaces = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
		c = ' ';
	    }
	    else if (c == NUL && curwin->w_p_list)
	    {
		p = (char_u *)"";
		n_extra = 1;
		c = '$';
	    }
	    else if (c != NUL && (n = charsize(c)) > 1)
	    {
		n_extra = n - 1;
		p = transchar(c);
		c = *p++;
	    }
	}

	if (c == NUL)
	    break;

	msg_putchar(c);
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
    msg_puts_attr(s, highlight_attr[HLF_T]);
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

    attr = highlight_attr[HLF_M];
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
	keep_msg_attr = highlight_attr[HLF_W];
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
