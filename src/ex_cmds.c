/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * ex_cmds.c: functions for command line commands
 */

#include "vim.h"

#ifdef EX_EXTRA
static int linelen __ARGS((int *has_tab));
#endif
static void do_filter __ARGS((linenr_t line1, linenr_t line2,
				    char_u *buff, int do_in, int do_out));
#ifdef VIMINFO
static char_u *viminfo_filename __ARGS((char_u	*));
static void do_viminfo __ARGS((FILE *fp_in, FILE *fp_out, int want_info,
					     int want_marks, int force_read));
static int read_viminfo_up_to_marks __ARGS((char_u *line, FILE *fp,
						   int forceit, int writing));
#endif

static int do_sub_msg __ARGS((void));
static int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
help_compare __ARGS((const void *s1, const void *s2));

    void
do_ascii()
{
    int		c;
    char	buf1[20];
    char	buf2[20];
    char_u	buf3[3];

    c = gchar_cursor();
    if (c == NUL)
    {
	MSG("empty line");
	return;
    }
    if (c == NL)	    /* NUL is stored as NL */
	c = NUL;
    if (vim_isprintc(c) && (c < ' ' || c > '~'))
    {
	transchar_nonprint(buf3, c);
	sprintf(buf1, "  <%s>", (char *)buf3);
    }
    else
	buf1[0] = NUL;
    if (c >= 0x80)
	sprintf(buf2, "  <M-%s>", transchar(c & 0x7f));
    else
	buf2[0] = NUL;
    sprintf((char *)IObuff, "<%s>%s%s  %d,  Hex %02x,  Octal %03o",
					   transchar(c), buf1, buf2, c, c, c);
    msg(IObuff);
}

#ifdef EX_EXTRA
/*
 * Handle ":left", ":center" and ":right" commands: align text.
 */
    void
do_align(eap)
    EXARG	*eap;
{
    FPOS    save_curpos;
    int	    len;
    int	    indent = 0;
    int	    new_indent;
    int	    has_tab;
    int	    width;

#ifdef RIGHTLEFT
    if (curwin->w_p_rl)
    {
	/* switch left and right aligning */
	if (eap->cmdidx == CMD_right)
	    eap->cmdidx = CMD_left;
	else if (eap->cmdidx == CMD_left)
	    eap->cmdidx = CMD_right;
    }
#endif

    width = atoi((char *)eap->arg);
    save_curpos = curwin->w_cursor;
    if (eap->cmdidx == CMD_left)    /* width is used for new indent */
    {
	if (width >= 0)
	    indent = width;
    }
    else
    {
	/*
	 * if 'textwidth' set, use it
	 * else if 'wrapmargin' set, use it
	 * if invalid value, use 80
	 */
	if (width <= 0)
	    width = curbuf->b_p_tw;
	if (width == 0 && curbuf->b_p_wm > 0)
	    width = Columns - curbuf->b_p_wm;
	if (width <= 0)
	    width = 80;
    }

    if (u_save((linenr_t)(eap->line1 - 1), (linenr_t)(eap->line2 + 1)) == FAIL)
	return;
    for (curwin->w_cursor.lnum = eap->line1;
		 curwin->w_cursor.lnum <= eap->line2; ++curwin->w_cursor.lnum)
    {
	if (eap->cmdidx == CMD_left)		/* left align */
	    new_indent = indent;
	else
	{
	    len = linelen(eap->cmdidx == CMD_right ? &has_tab
						   : NULL) - get_indent();

	    if (len <= 0)			/* skip blank lines */
		continue;

	    if (eap->cmdidx == CMD_center)
		new_indent = (width - len) / 2;
	    else
	    {
		new_indent = width - len;	/* right align */

		/*
		 * Make sure that embedded TABs don't make the text go too far
		 * to the right.
		 */
		if (has_tab)
		    while (new_indent > 0)
		    {
			set_indent(new_indent, TRUE);	/* set indent */
			if (linelen(NULL) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    do
				set_indent(++new_indent, TRUE);	/* set indent */
			    while (linelen(NULL) <= width);
			    --new_indent;
			    break;
			}
			--new_indent;
		    }
	    }
	}
	if (new_indent < 0)
	    new_indent = 0;
	set_indent(new_indent, TRUE);		/* set indent */
    }
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);

    /*
     * If the cursor is after the first changed line, its position needs to be
     * updated.
     */
    if (curwin->w_cursor.lnum > eap->line1)
    {
	changed_line_abv_curs();
	invalidate_botline();
    }
    else if (curwin->w_cursor.lnum == eap->line1)
	changed_cline_bef_curs();

    /*
     * If the start of the aligned lines is before botline, it may have become
     * approximated (lines got longer or shorter).
     */
    if (botline_approximated() && eap->line1 < curwin->w_botline)
	approximate_botline();
    update_screen(NOT_VALID);
}

/*
 * Get the length of the current line, excluding trailing white space.
 */
    static int
linelen(has_tab)
    int	    *has_tab;
{
    char_u  *line;
    char_u  *first;
    char_u  *last;
    int	    save;
    int	    len;

    /* find the first non-blank character */
    line = ml_get_curline();
    first = skipwhite(line);

    /* find the character after the last non-blank character */
    for (last = first + STRLEN(first);
				last > first && vim_iswhite(last[-1]); --last)
	;
    save = *last;
    *last = NUL;
    len = linetabsize(line);		/* get line length */
    if (has_tab != NULL)		/* check for embedded TAB */
	*has_tab = (vim_strrchr(first, TAB) != NULL);
    *last = save;

    return len;
}

/*
 * Handle ":retab" command.
 */
    void
do_retab(eap)
    EXARG	*eap;
{
    linenr_t	lnum;
    int		got_tab = FALSE;
    long	num_spaces = 0;
    long	num_tabs;
    long	len;
    long	col;
    long	vcol;
    long	start_col = 0;		/* For start of white-space string */
    long	start_vcol = 0;		/* For start of white-space string */
    int		temp;
    long	old_len;
    char_u	*ptr;
    char_u	*new_line = (char_u *)1;    /* init to non-NULL */
    int		did_something = FALSE;
    int		did_undo;		/* called u_save for current line */
    int		new_ts;
    int		save_list;

    save_list = curwin->w_p_list;
    curwin->w_p_list = 0;	    /* don't want list mode here */

    new_ts = getdigits(&(eap->arg));
    if (new_ts == 0)
	new_ts = curbuf->b_p_ts;
    for (lnum = eap->line1; !got_int && lnum <= eap->line2; ++lnum)
    {
	ptr = ml_get(lnum);
	col = 0;
	vcol = 0;
	did_undo = FALSE;
	for (;;)
	{
	    if (vim_iswhite(ptr[col]))
	    {
		if (!got_tab && num_spaces == 0)
		{
		    /* First consecutive white-space */
		    start_vcol = vcol;
		    start_col = col;
		}
		if (ptr[col] == ' ')
		    num_spaces++;
		else
		    got_tab = TRUE;
	    }
	    else
	    {
		if (got_tab || (eap->forceit && num_spaces > 1))
		{
		    /* Retabulate this string of white-space */

		    /* len is virtual length of white string */
		    len = num_spaces = vcol - start_vcol;
		    num_tabs = 0;
		    if (!curbuf->b_p_et)
		    {
			temp = new_ts - (start_vcol % new_ts);
			if (num_spaces >= temp)
			{
			    num_spaces -= temp;
			    num_tabs++;
			}
			num_tabs += num_spaces / new_ts;
			num_spaces -= (num_spaces / new_ts) * new_ts;
		    }
		    if (curbuf->b_p_et || got_tab ||
					(num_spaces + num_tabs < len))
		    {
			if (did_undo == FALSE)
			{
			    did_undo = TRUE;
			    if (u_save((linenr_t)(lnum - 1),
						(linenr_t)(lnum + 1)) == FAIL)
			    {
				new_line = NULL;	/* flag out-of-memory */
				break;
			    }
			}

			/* len is actual number of white characters used */
			len = num_spaces + num_tabs;
			old_len = STRLEN(ptr);
			new_line = lalloc(old_len - col + start_col + len + 1,
									TRUE);
			if (new_line == NULL)
			    break;
			if (start_col > 0)
			    vim_memmove(new_line, ptr, (size_t)start_col);
			vim_memmove(new_line + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_line + start_col;
			for (col = 0; col < len; col++)
			    ptr[col] = (col < num_tabs) ? '\t' : ' ';
			ml_replace(lnum, new_line, FALSE);
			did_something = TRUE;
			ptr = new_line;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		num_spaces = 0;
	    }
	    if (ptr[col] == NUL)
		break;
	    vcol += chartabsize(ptr[col++], (colnr_t)vcol);
	}
	if (new_line == NULL)		    /* out of memory */
	    break;
	line_breakcheck();
    }
    if (got_int)
	emsg(e_interr);
    if (did_something)
	CHANGED;

    curwin->w_p_list = save_list;	/* restore 'list' */

    if (curbuf->b_p_ts != new_ts || did_something)
    {
	/*
	 * Cursor may need updating when change is before or at the cursor
	 * line.  w_botline may be wrong a bit now.
	 */
	if (curbuf->b_p_ts != new_ts || eap->line1 < curwin->w_cursor.lnum)
	    changed_line_abv_curs();	    /* recompute cursor pos compl. */
	else if (eap->line1 == curwin->w_cursor.lnum)
	    changed_cline_bef_curs();	    /* recompute curosr pos partly */
	approximate_botline();
    }
    curbuf->b_p_ts = new_ts;
    coladvance(curwin->w_curswant);

    u_clearline();
    update_screen(NOT_VALID);
}
#endif

/*
 * :move command - move lines line1-line2 to line dest
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_move(line1, line2, dest)
    linenr_t	line1;
    linenr_t	line2;
    linenr_t	dest;
{
    char_u	*str;
    linenr_t	l;
    linenr_t	extra;	    /* Num lines added before line1 */
    linenr_t	num_lines;  /* Num lines moved */
    linenr_t	last_line;  /* Last line in file after adding new text */

    if (dest >= line1 && dest < line2)
    {
	EMSG("Move lines into themselves");
	return FAIL;
    }

    num_lines = line2 - line1 + 1;

    /*
     * First we copy the old text to its new location -- webb
     * Also copy the flag that ":global" command uses.
     */
    if (u_save(dest, dest + 1) == FAIL)
	return FAIL;
    for (extra = 0, l = line1; l <= line2; l++)
    {
	str = vim_strsave(ml_get(l + extra));
	if (str != NULL)
	{
	    ml_append(dest + l - line1, str, (colnr_t)0, FALSE);
	    vim_free(str);
	    if (dest < line1)
		extra++;
	}
    }

    /*
     * Now we must be careful adjusting our marks so that we don't overlap our
     * mark_adjust() calls.
     *
     * We adjust the marks within the old text so that they refer to the
     * last lines of the file (temporarily), because we know no other marks
     * will be set there since these line numbers did not exist until we added
     * our new lines.
     *
     * Then we adjust the marks on lines between the old and new text positions
     * (either forwards or backwards).
     *
     * And Finally we adjust the marks we put at the end of the file back to
     * their final destination at the new text position -- webb
     */
    last_line = curbuf->b_ml.ml_line_count;
    mark_adjust(line1, line2, last_line - line2, 0L);
    if (dest >= line2)
    {
	mark_adjust(line2 + 1, dest, -num_lines, 0L);
	curbuf->b_op_start.lnum = dest - num_lines + 1;
	curbuf->b_op_end.lnum = dest;
    }
    else
    {
	mark_adjust(dest + 1, line1 - 1, num_lines, 0L);
	curbuf->b_op_start.lnum = dest + 1;
	curbuf->b_op_end.lnum = dest + num_lines;
    }
    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    mark_adjust(last_line - num_lines + 1, last_line,
					     -(last_line - dest - extra), 0L);

    /*
     * Now we delete the original text -- webb
     */
    if (u_save(line1 + extra - 1, line2 + extra + 1) == FAIL)
	return FAIL;

    for (l = line1; l <= line2; l++)
	ml_delete(line1 + extra, TRUE);

    CHANGED;
    if (!global_busy && num_lines > p_report)
	smsg((char_u *)"%ld line%s moved", num_lines, plural(num_lines));

    /*
     * Leave the cursor on the last of the moved lines.
     */
    if (dest >= line1)
	curwin->w_cursor.lnum = dest;
    else
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;
    changed_line_abv_curs();
    /*
     * TODO: should recompute w_botline for simple situations.
     */
    invalidate_botline();
    return OK;
}

/*
 * :copy command - copy lines line1-line2 to line n
 */
    void
do_copy(line1, line2, n)
    linenr_t	line1;
    linenr_t	line2;
    linenr_t	n;
{
    linenr_t	    lnum;
    char_u	    *p;

    lnum = line2 - line1 + 1;
    mark_adjust(n + 1, (linenr_t)MAXLNUM, lnum, 0L);
    curbuf->b_op_start.lnum = n + 1;
    curbuf->b_op_end.lnum = n + lnum;
    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;

    /*
     * there are three situations:
     * 1. destination is above line1
     * 2. destination is between line1 and line2
     * 3. destination is below line2
     *
     * n = destination (when starting)
     * curwin->w_cursor.lnum = destination (while copying)
     * line1 = start of source (while copying)
     * line2 = end of source (while copying)
     */
    if (u_save(n, n + 1) == FAIL)
	return;
    curwin->w_cursor.lnum = n;
    while (line1 <= line2)
    {
	/* need to use vim_strsave() because the line will be unlocked
	    within ml_append */
	p = vim_strsave(ml_get(line1));
	if (p != NULL)
	{
	    ml_append(curwin->w_cursor.lnum, p, (colnr_t)0, FALSE);
	    vim_free(p);
	}
		/* situation 2: skip already copied lines */
	if (line1 == n)
	    line1 = curwin->w_cursor.lnum;
	++line1;
	if (curwin->w_cursor.lnum < line1)
	    ++line1;
	if (curwin->w_cursor.lnum < line2)
	    ++line2;
	++curwin->w_cursor.lnum;
    }
    CHANGED;
    changed_line_abv_curs();
    /*
     * TODO: should recompute w_botline for simple situations.
     */
    invalidate_botline();
    msgmore((long)lnum);
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 */
    void
do_bang(addr_count, line1, line2, forceit, arg, do_in, do_out)
    int		addr_count;
    linenr_t	line1, line2;
    int		forceit;
    char_u	*arg;
    int		do_in, do_out;
{
    static  char_u  *prevcmd = NULL;	    /* the previous command */
    char_u	    *newcmd = NULL;	    /* the new command */
    int		    free_newcmd = FALSE;    /* need to free() newcmd */
    int		    ins_prevcmd;
    char_u	    *t;
    char_u	    *p;
    char_u	    *trailarg;
    int		    len;
    int		    scroll_save = msg_scroll;

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    if (check_restricted() || check_secure())
	return;

    if (addr_count == 0)		/* :! */
    {
	msg_scroll = FALSE;	    /* don't scroll here */
	autowrite_all();
	msg_scroll = scroll_save;
    }

    /*
     * Try to find an embedded bang, like in :!<cmd> ! [args]
     * (:!! is indicated by the 'forceit' variable)
     */
    ins_prevcmd = forceit;
    trailarg = arg;
    do
    {
	len = STRLEN(trailarg) + 1;
	if (newcmd != NULL)
	    len += STRLEN(newcmd);
	if (ins_prevcmd)
	{
	    if (prevcmd == NULL)
	    {
		emsg(e_noprev);
		vim_free(newcmd);
		return;
	    }
	    len += STRLEN(prevcmd);
	}
	if ((t = alloc(len)) == NULL)
	{
	    vim_free(newcmd);
	    return;
	}
	*t = NUL;
	if (newcmd != NULL)
	    STRCAT(t, newcmd);
	if (ins_prevcmd)
	    STRCAT(t, prevcmd);
	p = t + STRLEN(t);
	STRCAT(t, trailarg);
	vim_free(newcmd);
	newcmd = t;

	/*
	 * Scan the rest of the argument for '!', which is replaced by the
	 * previous command.  "\!" is replaced by "!" (this is vi compatible).
	 */
	trailarg = NULL;
	while (*p)
	{
	    if (*p == '!')
	    {
		if (p > newcmd && p[-1] == '\\')
		    vim_memmove(p - 1, p, (size_t)(STRLEN(p) + 1));
		else
		{
		    trailarg = p;
		    *trailarg++ = NUL;
		    ins_prevcmd = TRUE;
		    break;
		}
	    }
	    ++p;
	}
    } while (trailarg != NULL);

    vim_free(prevcmd);
    prevcmd = newcmd;

    if (bangredo)	    /* put cmd in redo buffer for ! command */
    {
	AppendToRedobuff(prevcmd);
	AppendToRedobuff((char_u *)"\n");
	bangredo = FALSE;
    }
    /*
     * Add quotes around the command, for shells that need them.
     */
    if (*p_shq != NUL)
    {
	newcmd = alloc((unsigned)(STRLEN(prevcmd) + 2 * STRLEN(p_shq) + 1));
	if (newcmd == NULL)
	    return;
	STRCPY(newcmd, p_shq);
	STRCAT(newcmd, prevcmd);
	STRCAT(newcmd, p_shq);
	free_newcmd = TRUE;
    }
    if (addr_count == 0)		/* :! */
    {
	    /* echo the command */
	msg_start();
	msg_putchar(':');
	msg_putchar('!');
	msg_outtrans(newcmd);
	msg_clr_eos();
	windgoto(msg_row, msg_col);

	do_shell(newcmd, 0);
    }
    else				/* :range! */
	/* Careful: This may recursively call do_bang() again! (because of
	 * autocommands) */
	do_filter(line1, line2, newcmd, do_in, do_out);
    if (free_newcmd)
	vim_free(newcmd);
}

/*
 * call a shell to execute a command
 */
    void
do_shell(cmd, flags)
    char_u  *cmd;
    int	    flags;	/* may be SHELL_DOOUT when output is redirected */
{
    BUF	    *buf;
#ifndef USE_GUI_WIN32
    int	    save_nwr;
#endif

    /*
     * Disallow shell commands for "rvim".
     * Disallow shell commands from .exrc and .vimrc in current directory for
     * security reasons.
     */
    if (check_restricted() || check_secure())
    {
	msg_end();
	return;
    }

#ifdef WIN32
    /*
     * Check if external commands are allowed now.
     */
    if (can_end_termcap_mode(TRUE) == FALSE)
	return;
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    msg_putchar('\r');			/* put cursor at start of line */
#ifdef AUTOCMD
    if (!autocmd_busy)
#endif
	stoptermcap();
    msg_putchar('\n');			/* may shift screen one line up */

    /* warning message before calling the shell */
    if (p_warn
#ifdef AUTOCMD
		&& !autocmd_busy
#endif
				   )
	for (buf = firstbuf; buf; buf = buf->b_next)
	    if (buf_changed(buf))
	    {
		MSG_PUTS("[No write since last change]\n");
		break;
	    }

/* This windgoto is required for when the '\n' resulted in a "delete line 1"
 * command to the terminal. */

    if (!swapping_screen())
	windgoto(msg_row, msg_col);
    cursor_on();
    (void)call_shell(cmd, SHELL_COOKED | flags);
    need_check_timestamps = TRUE;

/*
 * put the message cursor at the end of the screen, avoids wait_return() to
 * overwrite the text that the external command showed
 */
    if (!swapping_screen())
    {
	msg_row = Rows - 1;
	msg_col = 0;
    }

#ifdef AUTOCMD
    if (autocmd_busy)
	must_redraw = CLEAR;
    else
#endif
    {
	/*
	 * For ":sh" there is no need to call wait_return(), just redraw.
	 * Also for the Win32 GUI (the output is in a console window).
	 * Otherwise there is probably text on the screen that the user wants
	 * to read before redrawing, so call wait_return().
	 */
#ifndef USE_GUI_WIN32
	if (cmd == NULL)
	{
#endif
	    must_redraw = CLEAR;
	    need_wait_return = FALSE;
	    dont_wait_return = TRUE;
#ifndef USE_GUI_WIN32
	}
	else
	{
	    /*
	     * If we switch screens when starttermcap() is called, we really
	     * want to wait for "hit return to continue".
	     */
	    save_nwr = no_wait_return;
	    if (swapping_screen())
		no_wait_return = FALSE;
#ifdef AMIGA
	    wait_return(term_console ? -1 : TRUE);	/* see below */
#else
	    wait_return(TRUE);
#endif
	    no_wait_return = save_nwr;
	}
#endif /* USE_GUI_WIN32 */
	starttermcap();	    /* start termcap if not done by wait_return() */

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the
	 * window size is wrong is very small, but we need to redraw the
	 * screen.  Don't do this if ':' hit in wait_return().	THIS IS UGLY
	 * but it saves an extra redraw.
	 */
#ifdef AMIGA
	if (skip_redraw)		/* ':' hit in wait_return() */
	    must_redraw = CLEAR;
	else if (term_console)
	{
	    OUT_STR("\033[0 q");	/* get window size */
	    if (got_int)
		must_redraw = CLEAR;	/* if got_int is TRUE, redraw needed */
	    else
		must_redraw = 0;	/* no extra redraw needed */
	}
#endif /* AMIGA */
    }

    /* display any error messages now */
    mch_display_error();
}

/*
 * do_filter: filter lines through a command given by the user
 *
 * We use temp files and the call_shell() routine here. This would normally
 * be done using pipes on a UNIX machine, but this is more portable to
 * non-unix machines. The call_shell() routine needs to be able
 * to deal with redirection somehow, and should handle things like looking
 * at the PATH env. variable, and adding reasonable extensions to the
 * command name given by the user. All reasonable versions of call_shell()
 * do this.
 * We use input redirection if do_in is TRUE.
 * We use output redirection if do_out is TRUE.
 */
    static void
do_filter(line1, line2, buff, do_in, do_out)
    linenr_t	line1, line2;
    char_u	*buff;
    int		do_in, do_out;
{
    char_u	*itmp = NULL;
    char_u	*otmp = NULL;
    linenr_t	linecount;
    FPOS	cursor_save;
#ifdef AUTOCMD
    BUF		*old_curbuf = curbuf;
#endif

    if (*buff == NUL)	    /* no filter command */
	return;

#ifdef WIN32
    /*
     * Check if external commands are allowed now.
     */
    if (can_end_termcap_mode(TRUE) == FALSE)
	return;
#endif

    cursor_save = curwin->w_cursor;
    linecount = line2 - line1 + 1;
    curwin->w_cursor.lnum = line1;
    curwin->w_cursor.col = 0;
    changed_line_abv_curs();
    invalidate_botline();

    /*
     * 1. Form temp file names
     * 2. Write the lines to a temp file
     * 3. Run the filter command on the temp file
     * 4. Read the output of the command into the buffer
     * 5. Delete the original lines to be filtered
     * 6. Remove the temp files
     */

    if ((do_in && (itmp = vim_tempname('i')) == NULL) ||
			       (do_out && (otmp = vim_tempname('o')) == NULL))
    {
	emsg(e_notmp);
	goto filterend;
    }

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		/* don't call wait_return() while busy */
    if (do_in && buf_write(curbuf, itmp, NULL, line1, line2,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	msg_putchar('\n');		    /* keep message from buf_write() */
	--no_wait_return;
	(void)emsg2(e_notcreate, itmp);	    /* will call wait_return */
	goto filterend;
    }
#ifdef AUTOCMD
    if (curbuf != old_curbuf)
	goto filterend;
#endif

    if (!do_out)
	msg_putchar('\n');

#if (defined(UNIX) && !defined(ARCHIE)) || defined(OS2)
/*
 * put braces around the command (for concatenated commands)
 */
    sprintf((char *)IObuff, "(%s)", (char *)buff);
    if (do_in)
    {
	STRCAT(IObuff, " < ");
	STRCAT(IObuff, itmp);
    }
#else
/*
 * for shells that don't understand braces around commands, at least allow
 * the use of commands in a pipe.
 */
    STRCPY(IObuff, buff);
    if (do_in)
    {
	char_u	    *p;

	/*
	 * If there is a pipe, we have to put the '<' in front of it.
	 * Don't do this when 'shellquote' is not empty, otherwise the
	 * redirection would be inside the quotes.
	 */
	p = vim_strchr(IObuff, '|');
	if (p && *p_shq == NUL)
	    *p = NUL;
	STRCAT(IObuff, " <");	    /* " < " causes problems on Amiga */
	STRCAT(IObuff, itmp);
	p = vim_strchr(buff, '|');
	if (p && *p_shq == NUL)
	    STRCAT(IObuff, p);
    }
#endif
    if (do_out)
    {
	char_u *p;

	if ((p = vim_strchr(p_srr, '%')) != NULL && p[1] == 's')
	{
	    p = IObuff + STRLEN(IObuff);
	    *p++ = ' '; /* not really needed? Not with sh, ksh or bash */
	    sprintf((char *)p, (char *)p_srr, (char *)otmp);
	}
	else
				/* " %s %s" causes problems on Amiga */
	    sprintf((char *)IObuff + STRLEN(IObuff), " %s%s",
						 (char *)p_srr, (char *)otmp);
    }

    windgoto((int)Rows - 1, 0);
    cursor_on();

    /*
     * When not redirecting the output the command can write anything to the
     * screen. If 'shellredir' is equal to ">", screen may be messed up by
     * stderr output of external command. Clear the screen later.
     * If do_in is FALSE, this could be something like ":r !cat", which may
     * also mess up the screen, clear it later.
     */
    if (!do_out || STRCMP(p_srr, ">") == 0 || !do_in)
	must_redraw = CLEAR;
    else
	redraw_later(NOT_VALID);

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    if (call_shell(IObuff, SHELL_FILTER | SHELL_COOKED |
					  (do_out ? SHELL_DOOUT : 0)) == FAIL)
    {
	must_redraw = CLEAR;
	wait_return(FALSE);
    }
    need_check_timestamps = TRUE;

    if (do_out)
    {
	if (u_save((linenr_t)(line2), (linenr_t)(line2 + 1)) == FAIL)
	{
	    goto error;
	}
	if (readfile(otmp, NULL, line2, (linenr_t)0, (linenr_t)MAXLNUM,
							 READ_FILTER) == FAIL)
	{
	    msg_putchar('\n');
	    emsg2(e_notread, otmp);
	    goto error;
	}
#ifdef AUTOCMD
	if (curbuf != old_curbuf)
	    goto filterend;
#endif

	if (do_in)
	{
	    /*
	     * Put cursor on first filtered line for ":range!cmd".
	     * Adjust '[ and '] (set by buf_write()).
	     */
	    curwin->w_cursor.lnum = line1;
	    del_lines(linecount, TRUE, TRUE);
	    curbuf->b_op_start.lnum -= linecount;	/* adjust '[ */
	    curbuf->b_op_end.lnum -= linecount;		/* adjust '] */
	    write_lnum_adjust(-linecount);		/* adjust last line
							   for next write */
	}
	else
	{
	    /*
	     * Put cursor on last new line for ":r !cmd".
	     */
	    curwin->w_cursor.lnum = curbuf->b_op_end.lnum;
	    linecount = curbuf->b_op_end.lnum - curbuf->b_op_start.lnum + 1;
	}
	beginline(BL_WHITE | BL_FIX);	    /* cursor on first non-blank */
	--no_wait_return;

	if (linecount > p_report)
	{
	    if (do_in)
	    {
		sprintf((char *)msg_buf, "%ld lines filtered", (long)linecount);
		if (msg(msg_buf) && !msg_scroll)
		{
		    keep_msg = msg_buf;	    /* display message after redraw */
		    keep_msg_attr = 0;
		}
	    }
	    else
		msgmore((long)linecount);
	}
    }
    else
    {
error:
	/* put cursor back in same position for ":w !cmd" */
	curwin->w_cursor = cursor_save;
	--no_wait_return;
	wait_return(FALSE);
    }

filterend:

#ifdef AUTOCMD
    if (curbuf != old_curbuf)
    {
	--no_wait_return;
	EMSG("*Filter* Autocommands must not change current buffer");
    }
#endif
    if (itmp != NULL)
	vim_remove(itmp);
    if (otmp != NULL)
	vim_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

#ifdef VIMINFO

static int no_viminfo __ARGS((void));
static int  viminfo_errcnt;

    static int
no_viminfo()
{
    /* "vim -i NONE" does not read or write a viminfo file */
    return (use_viminfo != NULL && STRCMP(use_viminfo, "NONE") == 0);
}

/*
 * Report an error for reading a viminfo file.
 * Count the number of errors.	When there are more than 10, return TRUE.
 */
    int
viminfo_error(message, line)
    char    *message;
    char_u  *line;
{
    sprintf((char *)IObuff, "viminfo: %s in line: ", message);
    STRNCAT(IObuff, line, IOSIZE - STRLEN(IObuff));
    emsg(IObuff);
    if (++viminfo_errcnt >= 10)
    {
	EMSG("viminfo: Too many errors, skipping rest of file");
	return TRUE;
    }
    return FALSE;
}

/*
 * read_viminfo() -- Read the viminfo file.  Registers etc. which are already
 * set are not over-written unless force is TRUE. -- webb
 */
    int
read_viminfo(file, want_info, want_marks, forceit)
    char_u  *file;
    int	    want_info;
    int	    want_marks;
    int	    forceit;
{
    FILE    *fp;

    if (no_viminfo())
	return FAIL;

    file = viminfo_filename(file);	    /* may set to default if NULL */
    if ((fp = fopen((char *)file, READBIN)) == NULL)
	return FAIL;

    if (p_verbose > 0)
	smsg((char_u *)"Reading viminfo file \"%s\"%s%s", file,
		    want_info ? " info" : "",
		    want_marks ? " marks" : "");

    viminfo_errcnt = 0;
    do_viminfo(fp, NULL, want_info, want_marks, forceit);

    fclose(fp);

    return OK;
}

/*
 * write_viminfo() -- Write the viminfo file.  The old one is read in first so
 * that effectively a merge of current info and old info is done.  This allows
 * multiple vims to run simultaneously, without losing any marks etc.  If
 * forceit is TRUE, then the old file is not read in, and only internal info is
 * written to the file. -- webb
 */
    void
write_viminfo(file, forceit)
    char_u  *file;
    int	    forceit;
{
    FILE	    *fp_in = NULL;	/* input viminfo file, if any */
    FILE	    *fp_out = NULL;	/* output viminfo file */
    char_u	    *tempname = NULL;	/* name of temp viminfo file */
    struct stat	    st_new;		/* stat() of potential new file */
    char_u	    *wp;
#ifdef UNIX
    int		    shortname = FALSE;	/* use 8.3 file name */
    mode_t	    umask_save;
    struct stat	    st_old;		/* stat() of existing viminfo file */
#endif

    if (no_viminfo())
	return;

    file = viminfo_filename(file);	/* may set to default if NULL */
    file = vim_strsave(file);		/* make a copy, don't want NameBuff */

    if (file != NULL)
    {
	fp_in = fopen((char *)file, READBIN);
	if (fp_in == NULL)
	{
	    /* if it does exist, but we can't read it, don't try writing */
	    if (stat((char *)file, &st_new) == 0)
		goto end;
#ifdef UNIX
	    /*
	     * For Unix we create the .viminfo non-accessible for others,
	     * because it may contain text from non-accessible documents.
	     */
	    umask_save = umask(077);
#endif
	    fp_out = fopen((char *)file, WRITEBIN);
#ifdef UNIX
	    (void)umask(umask_save);
#endif
	}
	else
	{
	    /*
	     * There is an existing viminfo file.  Create a temporary file to
	     * write the new viminfo into, in the same directory as the
	     * existing viminfo file, which will be renamed later.
	     */
#ifdef UNIX
	    /*
	     * For Unix we check the owner of the file.  It's not very nice to
	     * overwrite a user's viminfo file after a "su root", with a
	     * viminfo file that the user can't read.
	     */
	    st_old.st_dev = st_old.st_ino = 0;
	    st_old.st_mode = 0600;
	    if (stat((char *)file, &st_old) == 0 && getuid() &&
		    !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
	    {
		int	tt;

		/* avoid a wait_return for this message, it's annoying */
		tt = msg_didany;
		EMSG2("Viminfo file is not writable: %s", file);
		msg_didany = tt;
		goto end;
	    }
#endif

	    /*
	     * Make tempname.
	     * May try twice: Once normal and once with shortname set, just in
	     * case somebody puts his viminfo file in an 8.3 filesystem.
	     */
	    for (;;)
	    {
		tempname = buf_modname(
#ifdef UNIX
					shortname,
#else
# ifdef SHORT_FNAME
					TRUE,
# else
					FALSE,
# endif
#endif
					       file, (char_u *)".tmp", FALSE);
		if (tempname == NULL)		/* out of memory */
		    break;

		/*
		 * Check if tempfile already exists.  Never overwrite an
		 * existing file!
		 */
		if (stat((char *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    /*
		     * Check if tempfile is same as original file.  May happen
		     * when modname gave the same file back.  E.g.  silly
		     * link, or file name-length reached.  Try again with
		     * shortname set.
		     */
		    if (!shortname && st_new.st_dev == st_old.st_dev &&
			    st_new.st_ino == st_old.st_ino)
		    {
			vim_free(tempname);
			tempname = NULL;
			shortname = TRUE;
			continue;
		    }
#endif
		    /*
		     * Try another name.  Change one character, just before
		     * the extension.  This should also work for an 8.3
		     * file name, when after adding the extension it still is
		     * the same file as the original.
		     */
		    wp = tempname + STRLEN(tempname) - 5;
		    if (wp < gettail(tempname))	    /* empty file name? */
			wp = gettail(tempname);
		    for (*wp = 'z'; stat((char *)tempname, &st_new) == 0; --*wp)
		    {
			/*
			 * They all exist?  Must be something wrong! Don't
			 * write the viminfo file then.
			 */
			if (*wp == 'a')
			{
			    vim_free(tempname);
			    tempname = NULL;
			    break;
			}
		    }
		}
		break;
	    }

	    if (tempname != NULL)
	    {
		fp_out = fopen((char *)tempname, WRITEBIN);

		/*
		 * If we can't create in the same directory, try creating a
		 * "normal" temp file.
		 */
		if (fp_out == NULL)
		{
		    vim_free(tempname);
		    if ((tempname = vim_tempname('o')) != NULL)
			fp_out = fopen((char *)tempname, WRITEBIN);
		}
#ifdef UNIX
		/*
		 * Set file protection same as original file, but strip s-bit
		 * and make sure the owner can read/write it.
		 */
		if (fp_out != NULL)
		{
		    (void)mch_setperm(tempname,
				      (long)((st_old.st_mode & 0777) | 0600));
		    /* this only works for root: */
		    (void)chown((char *)tempname, st_old.st_uid, st_old.st_gid);
		}
#endif
	    }
	}
    }

    /*
     * Check if the new viminfo file can be written to.
     */
    if (file == NULL || fp_out == NULL)
    {
	EMSG2("Can't write viminfo file %s!", file == NULL ? (char_u *)"" :
					      fp_in == NULL ? file : tempname);
	if (fp_in != NULL)
	    fclose(fp_in);
	goto end;
    }

    if (p_verbose > 0)
	smsg((char_u *)"Writing viminfo file \"%s\"", file);

    viminfo_errcnt = 0;
    do_viminfo(fp_in, fp_out, !forceit, !forceit, FALSE);

    fclose(fp_out);	    /* errors are ignored !? */
    if (fp_in != NULL)
    {
	fclose(fp_in);
	/*
	 * In case of an error, don't overwrite the original viminfo file.
	 */
	if (viminfo_errcnt || vim_rename(tempname, file) == -1)
	    vim_remove(tempname);
    }
end:
    vim_free(file);
    vim_free(tempname);
}

/*
 * Get the viminfo file name to use.
 * If "file" is given and not empty, use it (has already been expanded by
 * cmdline functions).
 * Otherwise use "-i file_name", value from 'viminfo' or the default, and
 * expand environment variables.
 */
    static char_u *
viminfo_filename(file)
    char_u	*file;
{
    if (file == NULL || *file == NUL)
    {
	if (use_viminfo != NULL)
	    file = use_viminfo;
	else if ((file = find_viminfo_parameter('n')) == NULL || *file == NUL)
	{
#ifdef VIMINFO_FILE2
	    /* don't use $HOME when not defined (turned into "c:/"!). */
	    if (vim_getenv((char_u *)"HOME") == NULL)
	    {
		/* don't use $VIM when not available. */
		expand_env((char_u *)"$VIM", NameBuff, MAXPATHL);
		if (STRCMP("$VIM", NameBuff) != 0)  /* $VIM was expanded */
		    file = (char_u *)VIMINFO_FILE2;
		else
		    file = (char_u *)VIMINFO_FILE;
	    }
	    else
#endif
		file = (char_u *)VIMINFO_FILE;
	}
	expand_env(file, NameBuff, MAXPATHL);
	return NameBuff;
    }
    return file;
}

/*
 * do_viminfo() -- Should only be called from read_viminfo() & write_viminfo().
 */
    static void
do_viminfo(fp_in, fp_out, want_info, want_marks, force_read)
    FILE    *fp_in;
    FILE    *fp_out;
    int	    want_info;
    int	    want_marks;
    int	    force_read;
{
    int	    count = 0;
    int	    eof = FALSE;
    char_u  *line;

    if ((line = alloc(LSIZE)) == NULL)
	return;

    if (fp_in != NULL)
    {
	if (want_info)
	    eof = read_viminfo_up_to_marks(line, fp_in, force_read,
							      fp_out != NULL);
	else
	    /* Skip info, find start of marks */
	    while (!(eof = vim_fgets(line, LSIZE, fp_in)) && line[0] != '>')
		;
    }
    if (fp_out != NULL)
    {
	/* Write the info: */
	fprintf(fp_out, "# This viminfo file was generated by vim\n");
	fprintf(fp_out, "# You may edit it if you're careful!\n\n");
	write_viminfo_search_pattern(fp_out);
	write_viminfo_sub_string(fp_out);
	write_viminfo_history(fp_out);
	write_viminfo_registers(fp_out);
	write_viminfo_filemarks(fp_out);
	write_viminfo_bufferlist(fp_out);
	count = write_viminfo_marks(fp_out);
    }
    if (fp_in != NULL && want_marks)
	copy_viminfo_marks(line, fp_in, fp_out, count, eof);
    vim_free(line);
}

/*
 * read_viminfo_up_to_marks() -- Only called from do_viminfo().  Reads in the
 * first part of the viminfo file which contains everything but the marks that
 * are local to a file.  Returns TRUE when end-of-file is reached. -- webb
 */
    static int
read_viminfo_up_to_marks(line, fp, forceit, writing)
    char_u  *line;
    FILE    *fp;
    int	    forceit;
    int	    writing;
{
    int	    eof;

    prepare_viminfo_history(forceit ? 9999 : 0);
    eof = vim_fgets(line, LSIZE, fp);
    while (!eof && line[0] != '>')
    {
	switch (line[0])
	{
		/* Characters reserved for future expansion, ignored now */
	    case '+': /* "+40 /path/dir file", for running vim without args */
	    case '|': /* to be defined */
	    case '-': /* to be defined */
	    case '!': /* to be defined */
	    case '@': /* to be defined */
	    case '^': /* to be defined */
	    case '*': /* to be defined */
		/* A comment */
	    case NUL:
	    case '\r':
	    case '\n':
	    case '#':
		eof = vim_fgets(line, LSIZE, fp);
		break;
	    case '%': /* entry for buffer list */
		eof = read_viminfo_bufferlist(line, fp, writing);
		break;
	    case '"':
		eof = read_viminfo_register(line, fp, forceit);
		break;
	    case '/':	    /* Search string */
	    case '&':	    /* Substitute search string */
	    case '~':	    /* Last search string, followed by '/' or '&' */
		eof = read_viminfo_search_pattern(line, fp, forceit);
		break;
	    case '$':
		eof = read_viminfo_sub_string(line, fp, forceit);
		break;
	    case ':':
	    case '?':
	    case '=':
		eof = read_viminfo_history(line, fp);
		break;
	    case '\'':
		/* How do we have a file mark when the file is not in the
		 * buffer list?
		 */
		eof = read_viminfo_filemark(line, fp, forceit);
		break;
	    default:
		if (viminfo_error("Illegal starting char", line))
		    eof = TRUE;
		else
		    eof = vim_fgets(line, LSIZE, fp);
		break;
	}
    }
    finish_viminfo_history();
    return eof;
}

/*
 * check string read from viminfo file
 * remove '\n' at the end of the line
 * - replace CTRL-V CTRL-V with CTRL-V
 * - replace CTRL-V 'n'    with '\n'
 */
    void
viminfo_readstring(p)
    char_u	*p;
{
    while (*p != NUL && *p != '\n')
    {
	if (*p == Ctrl('V'))
	{
	    if (p[1] == 'n')
		p[0] = '\n';
	    vim_memmove(p + 1, p + 2, STRLEN(p));
	}
	++p;
    }
    *p = NUL;
}

/*
 * write string to viminfo file
 * - replace CTRL-V with CTRL-V CTRL-V
 * - replace '\n'   with CTRL-V 'n'
 * - add a '\n' at the end
 */
    void
viminfo_writestring(fd, p)
    FILE    *fd;
    char_u  *p;
{
    int	    c;

    while ((c = *p++) != NUL)
    {
	if (c == Ctrl('V') || c == '\n')
	{
	    putc(Ctrl('V'), fd);
	    if (c == '\n')
		c = 'n';
	}
	putc(c, fd);
    }
    putc('\n', fd);
}
#endif /* VIMINFO */

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
    void
do_fixdel()
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != NULL && *p == 0x7f ?
					 (char_u *)"\010" : (char_u *)"\177");
}

    void
print_line_no_prefix(lnum, use_number)
    linenr_t	lnum;
    int		use_number;
{
    char_u	numbuf[20];

    if (curwin->w_p_nu || use_number)
    {
	sprintf((char *)numbuf, "%7ld ", (long)lnum);
	msg_puts_attr(numbuf, highlight_attr[HLF_N]);	/* Highlight line nrs */
    }
    msg_prt_line(ml_get(lnum));
}
    void
print_line(lnum, use_number)
    linenr_t	lnum;
    int		use_number;
{
    msg_start();
    print_line_no_prefix(lnum, use_number);
}

/*
 * Implementation of ":file[!] [fname]".
 */
    void
do_file(arg, forceit)
    char_u  *arg;
    int	    forceit;
{
    char_u	*fname, *sfname, *xfname;
    BUF		*buf;

    if (*arg != NUL)
    {
	/*
	 * The name of the current buffer will be changed.
	 * A new buffer entry needs to be made to hold the old
	 * file name, which will become the alternate file name.
	 */
	fname = curbuf->b_ffname;
	sfname = curbuf->b_sfname;
	xfname = curbuf->b_fname;
	curbuf->b_ffname = NULL;
	curbuf->b_sfname = NULL;
	if (setfname(arg, NULL, TRUE) == FAIL)
	{
	    curbuf->b_ffname = fname;
	    curbuf->b_sfname = sfname;
	    return;
	}
	curbuf->b_notedited = TRUE;
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, FALSE);
	if (buf != NULL)
	    curwin->w_alt_fnum = buf->b_fnum;
	vim_free(fname);
	vim_free(sfname);
    }
    /* print full file name if :cd used */
    fileinfo(FALSE, FALSE, forceit);
}


/*
 * Do the Ex mode :insert and :append commands.
 * "getline" can be NULL, in which case a line is obtained from the user.
 */
    void
do_append(lnum, getline, cookie)
    linenr_t	lnum;
    char_u	*(*getline) __ARGS((int, void *, int));
    void	*cookie;		/* argument for getline() */
{
    char_u	*theline;
    int		did_undo = FALSE;
    int		lfirst = lnum;

    State = INSERT;		    /* behave like in Insert mode */
    while (1)
    {
	msg_scroll = TRUE;
	need_wait_return = FALSE;
	if (getline == NULL)
	    theline = getcmdline(NUL, 0L, 0);
	else
	    theline = getline(NUL, cookie, 0);
	lines_left = Rows - 1;
	if (theline == NULL || (theline[0] == '.' && theline[1] == NUL))
	    break;

	if (!did_undo && u_save(lnum, lnum + 1) == FAIL)
	    break;
	did_undo = TRUE;
	mark_adjust(lnum + 1, (linenr_t)MAXLNUM, 1L, 0L);
	ml_append(lnum, theline, (colnr_t)0, FALSE);
	CHANGED;

	vim_free(theline);
	++lnum;
    }
    State = NORMAL;

    /* "start" is set to lfirst+1 unless that position is invalid (when
     * lfirst pointed to the end of the buffer and nothig was appended)
     * "end" is set to lnum when something has been appended, otherwise
     * it is the same than "start"  -- Acevedo */
    curbuf->b_op_start.lnum = (lfirst < curbuf->b_ml.ml_line_count) ?
	lfirst + 1 : curbuf->b_ml.ml_line_count;
    curbuf->b_op_end.lnum = (lfirst < lnum) ? lnum : curbuf->b_op_start.lnum;
    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    curwin->w_cursor.lnum = lnum;
    check_cursor_lnum();
    beginline(BL_SOL | BL_FIX);
    changed_line_abv_curs();
    invalidate_botline();

    dont_wait_return = TRUE;	    /* don't use wait_return() now */
    need_wait_return = FALSE;
    update_screen(NOT_VALID);
}

/*
 * Do the Ex mode :change command.
 * "getline" can be NULL, in which case a line is obtained from the user.
 */
    void
do_change(start, end, getline, cookie)
    linenr_t	start;
    linenr_t	end;
    char_u	*(*getline) __ARGS((int, void *, int));
    void	*cookie;		/* argument for getline() */
{
    if (end >= start && u_save(start - 1, end + 1) == FAIL)
	return;

    mark_adjust(start, end, (long)MAXLNUM, (long)(start - end - 1));
    while (end >= start)
    {
	if (curbuf->b_ml.ml_flags & ML_EMPTY)	    /* nothing to delete */
	    break;
	ml_delete(start, FALSE);
	CHANGED;
	end--;
    }
    do_append(start - 1, getline, cookie);
}

    void
do_z(line, arg)
    linenr_t	line;
    char_u	*arg;
{
    char_u	*x;
    int		bigness = curwin->w_height - 3;
    char_u	kind;
    int		minus = 0;
    linenr_t	start, end, curs, i;

    if (bigness < 1)
	bigness = 1;

    x = arg;
    if (*x == '-' || *x == '+' || *x == '=' || *x == '^' || *x == '.')
	x++;

    if (*x != 0)
    {
	if (!isdigit(*x))
	{
	    EMSG("non-numeric argument to :z");
	    return;
	}
	else
	    bigness = atoi((char *)x);
    }

    kind = *arg;

    switch (kind)
    {
	case '-':
	    start = line - bigness;
	    end = line;
	    curs = line;
	    break;

	case '=':
	    start = line - bigness / 2 + 1;
	    end = line + bigness / 2 - 1;
	    curs = line;
	    minus = 1;
	    break;

	case '^':
	    start = line - bigness * 2;
	    end = line - bigness;
	    curs = line - bigness;
	    break;

	case '.':
	    start = line - bigness / 2;
	    end = line + bigness / 2;
	    curs = end;
	    break;

	default:  /* '+' */
	    start = line;
	    end = line + bigness;
	    curs = end;
	    break;
    }

    if (start < 1)
	start = 1;

    if (end > curbuf->b_ml.ml_line_count)
	end = curbuf->b_ml.ml_line_count;

    if (curs > curbuf->b_ml.ml_line_count)
	curs = curbuf->b_ml.ml_line_count;

    for (i = start; i <= end; i++)
    {
	int j;

	if (minus && (i == line))
	{
	    msg_putchar('\n');

	    for (j = 1; j < Columns; j++)
		msg_putchar('-');
	}

	print_line(i, FALSE);

	if (minus && (i == line))
	{
	    msg_putchar('\n');

	    for (j = 1; j < Columns; j++)
		msg_putchar('-');
	}
    }

    curwin->w_cursor.lnum = curs;
}

/*
 * Check if the restricted flag is set.
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    int
check_restricted()
{
    if (restricted)
    {
	EMSG("Shell commands not allowed in rvim");
	return TRUE;
    }
    return FALSE;
}

/*
 * Check if the secure flag is set (.exrc or .vimrc in current directory).
 * If so, give an error message and return TRUE.
 * Otherwise, return FALSE.
 */
    int
check_secure()
{
    if (secure)
    {
	secure = 2;
	emsg(e_curdir);
	return TRUE;
    }
    return FALSE;
}

static char_u	    *old_sub = NULL;	/* previous substitute pattern */

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 */
static long	    sub_nsubs;	    /* total number of substitutions */
static linenr_t	    sub_nlines;	    /* total number of lines changed */

/* do_sub()
 *
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/gc
 *
 * The trailing 'g' is optional and, if present, indicates that multiple
 * substitutions should be performed on each line, if applicable.
 * The trailing 'c' is optional and, if present, indicates that a confirmation
 * will be asked for each replacement.
 * The usual escapes are supported as described in the regexp docs.
 */
    void
do_sub(eap)
    EXARG	*eap;
{
    linenr_t	    lnum;
    long	    i;
    char_u	   *ptr;
    char_u	   *old_line;
    vim_regexp	   *prog;
    static int	    do_all = FALSE;	/* do multiple substitutions per line */
    static int	    do_ask = FALSE;	/* ask for confirmation */
    int		    do_error = TRUE;	/* if false, ignore errors */
    int		    do_print = FALSE;	/* print last line with subst. */
    char_u	   *pat = NULL, *sub = NULL;	/* init for GCC */
    int		    delimiter;
    int		    sublen;
    int		    got_quit = FALSE;
    int		    got_match = FALSE;
    int		    temp;
    int		    which_pat;
    char_u	    *cmd;
    int		    save_reg_ic;

    cmd = eap->arg;
    if (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }

#ifdef FKMAP		    /* reverse the flow of the Farsi characters */
    if (p_altkeymap && curwin->w_p_rl)
	lrF_sub(cmd);
#endif

    if (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	/* use last used regexp */
    else
	which_pat = RE_SUBST;	/* use last substitute regexp */

				/* new pattern and substitution */
    if (eap->cmdidx == CMD_substitute && *cmd != NUL && !vim_iswhite(*cmd)
		    && vim_strchr((char_u *)"0123456789gcr|\"", *cmd) == NULL)
    {
				/* don't accept alphanumeric for separator */
	if (isalpha(*cmd))
	{
	    EMSG("Regular expressions can't be delimited by letters");
	    return;
	}
	/*
	 * undocumented vi feature:
	 *  "\/sub/" and "\?sub?" use last used search pattern (almost like
	 *  //sub/r).  "\&sub&" use last substitute pattern (like //sub/).
	 */
	if (*cmd == '\\')
	{
	    ++cmd;
	    if (vim_strchr((char_u *)"/?&", *cmd) == NULL)
	    {
		emsg(e_backslash);
		return;
	    }
	    if (*cmd != '&')
		which_pat = RE_SEARCH;	    /* use last '/' pattern */
	    pat = (char_u *)"";		    /* empty search pattern */
	    delimiter = *cmd++;		    /* remember delimiter character */
	}
	else		/* find the end of the regexp */
	{
	    which_pat = RE_LAST;	    /* use last used regexp */
	    delimiter = *cmd++;		    /* remember delimiter character */
	    pat = cmd;			    /* remember start of search pat */
	    cmd = skip_regexp(cmd, delimiter, (int)p_magic);
	    if (cmd[0] == delimiter)	    /* end delimiter found */
		*cmd++ = NUL;		    /* replace it with a NUL */
	}

	/*
	 * Small incompatibility: vi sees '\n' as end of the command, but in
	 * Vim we want to use '\n' to find/substitute a NUL.
	 */
	sub = cmd;	    /* remember the start of the substitution */

	while (cmd[0])
	{
	    if (cmd[0] == delimiter)		/* end delimiter found */
	    {
		*cmd++ = NUL;			/* replace it with a NUL */
		break;
	    }
	    if (cmd[0] == '\\' && cmd[1] != 0)	/* skip escaped characters */
		++cmd;
	    ++cmd;
	}

	if (!eap->skip)
	{
	    vim_free(old_sub);
	    old_sub = vim_strsave(sub);
	}
    }
    else if (!eap->skip)	/* use previous pattern and substitution */
    {
	if (old_sub == NULL)	/* there is no previous command */
	{
	    emsg(e_nopresub);
	    return;
	}
	pat = NULL;		/* search_regcomp() will use previous pattern */
	sub = old_sub;
    }

    /*
     * find trailing options
     */
    if (!p_ed)
    {
	if (p_gd)		/* default is global on */
	    do_all = TRUE;
	else
	    do_all = FALSE;
	do_ask = FALSE;
    }
    while (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off
	 * 'r' is never inverted.
	 */
	if (*cmd == 'g')
	    do_all = !do_all;
	else if (*cmd == 'c')
	    do_ask = !do_ask;
	else if (*cmd == 'e')
	    do_error = !do_error;
	else if (*cmd == 'r')	    /* use last used regexp */
	    which_pat = RE_LAST;
	else if (*cmd == 'p')
	    do_print = TRUE;
	else
	    break;
	++cmd;
    }

    /*
     * check for a trailing count
     */
    cmd = skipwhite(cmd);
    if (isdigit(*cmd))
    {
	i = getdigits(&cmd);
	if (i <= 0 && !eap->skip && do_error)
	{
	    emsg(e_zerocount);
	    return;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    if (*cmd && *cmd != '\"')	    /* if not end-of-line or comment */
    {
	eap->nextcmd = check_nextcmd(cmd);
	if (eap->nextcmd == NULL)
	{
	    emsg(e_trailing);
	    return;
	}
    }

    if (eap->skip)	    /* not executing commands, only parsing */
	return;

    if ((prog = search_regcomp(pat, RE_SUBST, which_pat, SEARCH_HIS)) == NULL)
    {
	if (do_error)
	    emsg(e_invcmd);
	return;
    }

    /*
     * ~ in the substitute pattern is replaced with the old pattern.
     * We do it here once to avoid it to be replaced over and over again.
     */
    sub = regtilde(sub, (int)p_magic);

    old_line = NULL;
    for (lnum = eap->line1; lnum <= eap->line2 && !(got_int || got_quit);
								       ++lnum)
    {
	ptr = ml_get(lnum);
	if (vim_regexec(prog, ptr, TRUE))  /* a match on this line */
	{
	    char_u	*new_end, *new_start = NULL;
	    char_u	*old_match, *old_copy;
	    char_u	*prev_old_match = NULL;
	    char_u	*p1;
	    int		did_sub = FALSE;
	    int		match, lastone;
	    unsigned	len, needed_len;
	    unsigned	new_start_len = 0;

	    /* make a copy of the line, so it won't be taken away when updating
		the screen */
	    if ((old_line = vim_strsave(ptr)) == NULL)
		continue;
	    vim_regexec(prog, old_line, TRUE);	/* match again on this line to
						 * update the pointers. TODO:
						 * remove extra vim_regexec() */
	    if (!got_match)
	    {
		setpcmark();
		got_match = TRUE;
	    }

	    old_copy = old_match = old_line;
	    for (;;)		/* loop until nothing more to replace */
	    {
		/*
		 * Save the position of the last change for the final cursor
		 * position (just like the real vi).
		 */
		curwin->w_cursor.lnum = lnum;
		curwin->w_cursor.col = (int)(prog->startp[0] - old_line);
		changed_cline_bef_curs();

		/*
		 * Match empty string does not count, except for first match.
		 * This reproduces the strange vi behaviour.
		 * This also catches endless loops.
		 */
		if (old_match == prev_old_match && old_match == prog->endp[0])
		{
		    ++old_match;
		    goto skip;
		}
		old_match = prog->endp[0];
		prev_old_match = old_match;

		/* update_screen() may change reg_ic: save it */
		save_reg_ic = reg_ic;

		/*
		 * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		 */
		while (do_ask)
		{
		    temp = RedrawingDisabled;
		    RedrawingDisabled = FALSE;
		    search_match_len = prog->endp[0] - prog->startp[0];
		    /* invert the matched string
		     * remove the inversion afterwards */
		    if (search_match_len == 0)
			search_match_len = 1;	    /* show something! */
		    highlight_match = TRUE;
		    update_topline();
		    validate_cursor();
		    update_screen(NOT_VALID);
		    highlight_match = FALSE;
		    redraw_later(NOT_VALID);
		    if (msg_row == Rows - 1)
			msg_didout = FALSE;	    /* avoid a scroll-up */
		    /* write message same highlighting as for wait_return */
		    smsg_attr(highlight_attr[HLF_R],
			    (char_u *)"replace with %s (y/n/a/q/^E/^Y)?",
			    sub);
		    showruler(TRUE);
		    RedrawingDisabled = temp;

#ifdef USE_GUI_WIN32
		    dont_scroll = FALSE;	/* allow scrolling here */
#endif
		    ++no_mapping;		/* don't map this key */
		    i = vgetc();
		    --no_mapping;

			/* clear the question */
		    msg_didout = FALSE;		/* don't scroll up */
		    msg_col = 0;
		    gotocmdline(TRUE);
		    if (i == 'q' || i == ESC || i == Ctrl('C')
#ifdef UNIX
			    || i == intr_char
#endif
			    )
		    {
			got_quit = TRUE;
			break;
		    }
		    else if (i == 'n')
			goto skip;
		    else if (i == 'y')
			break;
		    else if (i == 'a')
		    {
			do_ask = FALSE;
			break;
		    }
		    else if (i == Ctrl('E'))
			scrollup_clamp();
		    else if (i == Ctrl('Y'))
			scrolldown_clamp();
		}
		if (got_quit)
		    break;

		reg_ic = save_reg_ic;

		/* get length of substitution part */
		sublen = vim_regsub(prog, sub, old_line, FALSE, (int)p_magic);
		if (new_start == NULL)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_len = STRLEN(old_copy) + sublen + 25;
		    if ((new_start = alloc_check(new_start_len)) == NULL)
			goto outofmem;
		    *new_start = NUL;
		    new_end = new_start;
		}
		else
		{
		    /*
		     * Extend the temporary buffer to do the substitution into.
		     * Avoid an alloc()/free(), it takes a lot of time.
		     */
		    len = STRLEN(new_start);
		    needed_len = len + STRLEN(old_copy) + sublen + 1;
		    if (needed_len > new_start_len)
		    {
			needed_len += 20;	/* get some extra */
			if ((p1 = alloc_check(needed_len)) == NULL)
			    goto outofmem;
			STRCPY(p1, new_start);
			vim_free(new_start);
			new_start = p1;
			new_start_len = needed_len;
		    }
		    new_end = new_start + len;
		}

		/*
		 * copy the text up to the part that matched
		 */
		i = prog->startp[0] - old_copy;
		vim_memmove(new_end, old_copy, (size_t)i);
		new_end += i;

		vim_regsub(prog, sub, new_end, TRUE, (int)p_magic);
		sub_nsubs++;
		did_sub = TRUE;

		/*
		 * Now the trick is to replace CTRL-Ms with a real line break.
		 * This would make it impossible to insert CTRL-Ms in the text.
		 * That is the way vi works. In Vim the line break can be
		 * avoided by preceding the CTRL-M with a CTRL-V. Now you can't
		 * precede a line break with a CTRL-V, big deal.
		 */
		while ((p1 = vim_strchr(new_end, CR)) != NULL)
		{
		    if (p1 == new_end || p1[-1] != Ctrl('V'))
		    {
			if (u_inssub(lnum) == OK)   /* prepare for undo */
			{
			    *p1 = NUL;		    /* truncate up to the CR */
			    mark_adjust(lnum, (linenr_t)MAXLNUM, 1L, 0L);
			    ml_append(lnum - 1, new_start,
					(colnr_t)(p1 - new_start + 1), FALSE);
			    ++lnum;
			    ++eap->line2;	/* number of lines increases */
			    STRCPY(new_start, p1 + 1);	/* copy the rest */
			    new_end = new_start;
			}
		    }
		    else			    /* remove CTRL-V */
		    {
			STRCPY(p1 - 1, p1);
			new_end = p1;
		    }
		}

		/* remember next character to be copied */
		old_copy = prog->endp[0];
		/*
		 * continue searching after the match
		 * prevent endless loop with patterns that match empty strings,
		 * e.g. :s/$/pat/g or :s/[a-z]* /(&)/g
		 */
skip:
		match = -1;
		lastone = (*old_match == NUL || got_int || got_quit || !do_all);
		if (lastone || do_ask ||
		      (match = vim_regexec(prog, old_match, (int)FALSE)) == 0)
		{
		    if (new_start)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * Old_match has to be adjusted, we use the end of the
			 * line as reference, because the substitute may have
			 * changed the number of characters.
			 */
			STRCAT(new_start, old_copy);
			i = old_line + STRLEN(old_line) - old_match;
			if (u_savesub(lnum) == OK)
			    ml_replace(lnum, new_start, TRUE);
			/* When asking, undo is saved each time, must also set
			 * changed flag each time. */
			if (do_ask)
			    CHANGED;

			vim_free(old_line);	    /* free the temp buffer */
			old_line = new_start;
			new_start = NULL;
			old_match = old_line + STRLEN(old_line) - i;
			if (old_match < old_line)	/* safety check */
			{
			    EMSG("do_sub internal error: old_match < old_line");
			    old_match = old_line;
			}
			old_copy = old_line;
		    }
		    if (match == -1 && !lastone)
			match = vim_regexec(prog, old_match, (int)FALSE);
		    if (match <= 0)   /* quit loop if there is no more match */
			break;
		}
		line_breakcheck();

	    }
	    if (did_sub)
		++sub_nlines;
	    vim_free(old_line);	    /* free the copy of the original line */
	    old_line = NULL;
	}
	line_breakcheck();
    }
    curbuf->b_op_start.lnum = eap->line1;
    curbuf->b_op_end.lnum = eap->line2;
    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;

outofmem:
    vim_free(old_line);	    /* may have to free an allocated copy of the line */
    if (sub_nsubs)
    {
	CHANGED;
	approximate_botline();
	if (!global_busy)
	{
	    update_topline();
	    beginline(BL_WHITE | BL_FIX);
	    update_screen(NOT_VALID); /* need this to update LineSizes */
	    if (!do_sub_msg() && do_ask)
		MSG("");
	}
	if (do_print)
	    print_line(curwin->w_cursor.lnum, FALSE);
    }
    else if (!global_busy)
    {
	if (got_int)		/* interrupted */
	    emsg(e_interr);
	else if (got_match)	/* did find something but nothing substituted */
	    MSG("");
	else			/* nothing found */
	{
	    if (do_error)
		emsg(e_nomatch);
	}
    }

    vim_free(prog);
}

/*
 * Give message for number of substitutions.
 * Can also be used after a ":global" command.
 * Return TRUE if a message was given.
 */
    static int
do_sub_msg()
{
    /*
     * Only report substitutions when:
     * - more than 'report' substitutions
     * - command was typed by user, or number of changed lines > 'report'
     * - giving messages is not disabled by 'lazyredraw'
     */
    if (sub_nsubs > p_report &&
	    (KeyTyped || sub_nlines > 1 || p_report < 1) &&
	    messaging())
    {
	sprintf((char *)msg_buf, "%s%ld substitution%s on %ld line%s",
		got_int ? "(Interrupted) " : "",
		sub_nsubs, plural(sub_nsubs),
		(long)sub_nlines, plural((long)sub_nlines));
	if (msg(msg_buf))
	{
	    keep_msg = msg_buf;
	    keep_msg_attr = 0;
	}
	return TRUE;
    }
    if (got_int)
    {
	emsg(e_interr);
	return TRUE;
    }
    return FALSE;
}

/*
 * do_glob(cmd)
 *
 * Execute a global command of the form:
 *
 * g/pattern/X : execute X on all lines where pattern matches
 * v/pattern/X : execute X on all lines where pattern does not match
 *
 * where 'X' is an EX command
 *
 * The command character (as well as the trailing slash) is optional, and
 * is assumed to be 'p' if missing.
 *
 * This is implemented in two passes: first we scan the file for the pattern and
 * set a mark for each line that (not) matches. secondly we execute the command
 * for each line that has a mark. This is required because after deleting
 * lines we do not know where to search for the next match.
 */
    void
do_glob(eap)
    EXARG	*eap;
{
    linenr_t	    lnum;	/* line number according to old situation */
    linenr_t	    old_lcount; /* b_ml.ml_line_count before the command */
    int		    ndone;
    int		    type;	/* first char of cmd: 'v' or 'g' */
    char_u	    *cmd;	/* command argument */

    char_u	    delim;	/* delimiter, normally '/' */
    char_u	   *pat;
    vim_regexp	   *prog;
    int		    match;
    int		    which_pat;

    if (global_busy)
    {
	EMSG("Cannot do :global recursive");	/* will increment global_busy */
	return;
    }

    type = *eap->cmd;
    cmd = eap->arg;
    which_pat = RE_LAST;	    /* default: use last used regexp */
    sub_nsubs = 0;
    sub_nlines = 0;

    /*
     * undocumented vi feature:
     *	"\/" and "\?": use previous search pattern.
     *		 "\&": use previous substitute pattern.
     */
    if (*cmd == '\\')
    {
	++cmd;
	if (vim_strchr((char_u *)"/?&", *cmd) == NULL)
	{
	    emsg(e_backslash);
	    return;
	}
	if (*cmd == '&')
	    which_pat = RE_SUBST;	/* use previous substitute pattern */
	else
	    which_pat = RE_SEARCH;	/* use previous search pattern */
	++cmd;
	pat = (char_u *)"";
    }
    else if (*cmd == NUL)
    {
	EMSG("Regular expression missing from global");
	return;
    }
    else
    {
	delim = *cmd;		/* get the delimiter */
	if (delim)
	    ++cmd;		/* skip delimiter if there is one */
	pat = cmd;		/* remember start of pattern */
	cmd = skip_regexp(cmd, delim, (int)p_magic);
	if (cmd[0] == delim)		    /* end delimiter found */
	    *cmd++ = NUL;		    /* replace it with a NUL */
    }

#ifdef FKMAP		/* when in Farsi mode, reverse the character flow */
    if (p_altkeymap && curwin->w_p_rl)
	lrFswap(pat,0);
#endif

    if ((prog = search_regcomp(pat, RE_BOTH, which_pat, SEARCH_HIS)) == NULL)
    {
	emsg(e_invcmd);
	return;
    }

/*
 * pass 1: set marks for each (not) matching line
 */
    ndone = 0;
    for (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
    {
	/* a match on this line? */
	match = vim_regexec(prog, ml_get(lnum), (int)TRUE);
	if ((type == 'g' && match) || (type == 'v' && !match))
	{
	    ml_setmarked(lnum);
	    ndone++;
	}
	line_breakcheck();
    }

/*
 * pass 2: execute the command for each line that has been marked
 */
    if (got_int)
	MSG("Interrupted");
    else if (ndone == 0)
	msg(e_nomatch);
    else
    {
	/*
	 * Set current position only once for a global command.
	 * If global_busy is set, setpcmark() will not do anything.
	 * If there is an error, global_busy will be incremented.
	 */
	setpcmark();

	global_busy = 1;
	old_lcount = curbuf->b_ml.ml_line_count;
	while (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
	{
	    curwin->w_cursor.lnum = lnum;
	    curwin->w_cursor.col = 0;
	    if (*cmd == NUL || *cmd == '\n')
		do_cmdline((char_u *)"p", NULL, NULL, DOCMD_NOWAIT);
	    else
		do_cmdline(cmd, NULL, NULL, DOCMD_NOWAIT);
	    ui_breakcheck();
	}

	global_busy = 0;
	adjust_cursor();	/* cursor may be beyond the end of the line */

	/*
	 * Redraw everything.  Could use CLEAR, which is faster in some
	 * situations, but when there are few changes this makes the display
	 * flicker.
	 */
	redraw_later(NOT_VALID);

	/* If subsitutes done, report number of substitues, otherwise report
	 * number of extra or deleted lines. */
	if (!do_sub_msg())
	    msgmore(curbuf->b_ml.ml_line_count - old_lcount);
    }

    ml_clearmarked();	   /* clear rest of the marks */
    vim_free(prog);
}

#ifdef VIMINFO
    int
read_viminfo_sub_string(line, fp, force)
    char_u  *line;
    FILE    *fp;
    int	    force;
{
    if (old_sub != NULL && force)
	vim_free(old_sub);
    if (force || old_sub == NULL)
    {
	viminfo_readstring(line);
	old_sub = vim_strsave(line + 1);
    }
    return vim_fgets(line, LSIZE, fp);
}

    void
write_viminfo_sub_string(fp)
    FILE    *fp;
{
    if (get_viminfo_parameter('/') != 0 && old_sub != NULL)
    {
	fprintf(fp, "\n# Last Substitute String:\n$");
	viminfo_writestring(fp, old_sub);
    }
}
#endif /* VIMINFO */


/*
 * ":help": open a read-only window on the help.txt file
 */
    void
do_help(eap)
    EXARG   *eap;
{
    char_u  *arg;
    FILE    *helpfd;		/* file descriptor of help file */
    int	    n;
    WIN	    *wp;
    int	    num_matches;
    char_u  **matches;
    int	    need_free = FALSE;

    if (eap != NULL)
    {
	/*
	 * A ":help" command ends at the first LF, or at a '|' that is
	 * followed by some text.  Set nextcmd to the following command.
	 */
	for (arg = eap->arg; *arg; ++arg)
	{
	    if (*arg == '\n' || *arg == '\r' || (*arg == '|' && arg[1] != NUL))
	    {
		*arg++ = NUL;
		eap->nextcmd = arg;
		break;
	    }
	}
	arg = eap->arg;

	if (eap->skip)	    /* not executing commands */
	    return;
    }
    else
	arg = (char_u *)"";

    /*
     * If an argument is given, check if there is a match for it.
     */
    if (*arg != NUL)
    {
	n = find_help_tags(arg, &num_matches, &matches);
	if (num_matches == 0 || n == FAIL)
	{
	    EMSG2("Sorry, no help for %s", arg);
	    return;
	}

	/* The first match is the best match. */
	arg = vim_strsave(matches[0]);
	need_free = TRUE;
	FreeWild(num_matches, matches);
    }

    /*
     * If there is already a help window open, use that one.
     */
    if (!curwin->w_buffer->b_help)
    {
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
	    if (wp->w_buffer != NULL && wp->w_buffer->b_help)
		break;
	if (wp != NULL && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	else
	{
	    /*
	     * There is no help buffer yet.
	     * Try to open the file specified by the "helpfile" option.
	     */
	    if ((helpfd = fopen((char *)p_hf, READBIN)) == NULL)
	    {
		smsg((char_u *)"Sorry, help file \"%s\" not found", p_hf);
		goto erret;
	    }
	    fclose(helpfd);

	    if (win_split(0, FALSE, FALSE) == FAIL)
		goto erret;

	    if (curwin->w_height < p_hh)
		win_setheight((int)p_hh);

#ifdef RIGHTLEFT
	    curwin->w_p_rl = 0;		    /* help window is left-to-right */
#endif
	    curwin->w_p_nu = 0;		    /* no line numbers */

	    /* Save the values of the options we will change.  Do this before
	     * do_ecmd(), because there could be modelines in the help file */
	    vim_free(help_save_isk);
	    help_save_isk = vim_strsave(curbuf->b_p_isk);
	    help_save_ts = curbuf->b_p_ts;

	    /*
	     * open help file (do_ecmd() will set b_help flag, readfile() will
	     * set b_p_ro flag)
	     */
	    (void)do_ecmd(0, p_hf, NULL, NULL, (linenr_t)0,
						   ECMD_HIDE + ECMD_SET_HELP);

	    /* accept all chars for keywords, except ' ', '*', '"', '|' */
	    set_string_option_direct((char_u *)"isk", -1,
					     (char_u *)"!-~,^*,^|,^\"", TRUE);
	    curbuf->b_p_ts = 8;
	    curwin->w_p_list = FALSE;
	    check_buf_options(curbuf);
	    (void)init_chartab();	/* needed because 'isk' changed */
	}
    }

    if (!p_im)
	restart_edit = 0;	    /* don't want insert mode in help file */

    if (arg == NULL || *arg == NUL)
    {
	arg = (char_u *)"help.txt";	    /* go to the index */
	need_free = FALSE;
    }
    do_tag(arg, DT_HELP, 1, FALSE);

erret:
    if (need_free)
	vim_free(arg);
}

/*
 * Return a heuristic indicating how well the given string matches.  The
 * smaller the number, the better the match.  This is the order of priorities,
 * from best match to worst match:
 *	- Match with least alpha-numeric characters is better.
 *	- Match with least total characters is better.
 *	- Match towards the start is better.
 * Assumption is made that the matched_string passed has already been found to
 * match some string for which help is requested.  webb.
 */
    int
help_heuristic(matched_string, offset, wrong_case)
    char_u  *matched_string;
    int	    offset;		/* offset for match */
    int	    wrong_case;		/* no matching case */
{
    int	    num_letters;
    char_u  *p;

    num_letters = 0;
    for (p = matched_string; *p; p++)
	if (isalnum(*p))
	    num_letters++;

    /*
     * Multiply the number of letters by 100 to give it a much bigger
     * weighting than the number of characters.
     * If there match is only a match while ignoring case, add 5000.
     * If the match starts in the middle of a word, add 10000 to put it
     * somewhere in the last half.
     * If the match is more than 2 chars from the start, multiply by 200 to
     * put it after matches at the start.
     */
    if (isalnum(matched_string[offset]) && offset > 0 &&
					  isalnum(matched_string[offset - 1]))
	offset += 10000;
    else if (offset > 2)
	offset *= 200;
    if (wrong_case)
	offset += 5000;
    return (int)(100 * num_letters + STRLEN(matched_string) + offset);
}

/*
 * Compare functions for qsort() below, that checks the help heuristics number
 * that has been put after the tagname by find_tags().
 */
    static int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
help_compare(s1, s2)
    const void	*s1;
    const void	*s2;
{
    char    *p1;
    char    *p2;

    p1 = *(char **)s1 + strlen(*(char **)s1) + 1;
    p2 = *(char **)s2 + strlen(*(char **)s2) + 1;
    return strcmp(p1, p2);
}

/*
 * Find all help tags matching "arg", sort them and return in matches[], with
 * the number of matches in num_matches.
 * We try first with case, and then ignoring case.  Then we try to choose the
 * "best" match from the ones found.
 */
    int
find_help_tags(arg, num_matches, matches)
    char_u	*arg;
    int		*num_matches;
    char_u	***matches;
{
    char_u	*s, *d;
    int		i;
    static char *(mtable[]) = {"*", "g*", "[*", "]*",
			       "/*", "/\\*", "/\\(\\)",
			       "?", ":?", "?<CR>", "\"*",
			       "[count]", "[quotex]", "[range]",
			       "[pattern]"};
    static char *(rtable[]) = {"star", "gstar", "[star", "]star",
			       "/star", "/\\\\star", "/\\\\(\\\\)",
			       "?", ":?", "?<CR>", "quotestar",
			       "\\[count]", "\\[quotex]", "\\[range]",
			       "\\[pattern]"};

    d = IObuff;		    /* assume IObuff is long enough! */

    /*
     * Recognize a few exceptions to the rule.	Some strings that contain '*'
     * with "star".  Otherwise '*' is recognized as a wildcard.
     */
    for (i = sizeof(mtable) / sizeof(char *); --i >= 0; )
    {
	if (STRCMP(arg, mtable[i]) == 0)
	{
	    STRCPY(d, rtable[i]);
	    break;
	}
    }

    if (i < 0)	    /* no match in table, replace single characters */
    {
	for (s = arg; *s; ++s)
	{
	    /*
	     * Replace "|" with "bar" and '"' with "quote" to match the name of
	     * the tags for these commands.
	     * Replace "*" with ".*" and "?" with "." to match command line
	     * completion.
	     * Insert a backslash before '~', '$' and '.' to avoid their
	     * special meaning.
	     */
	    if (d - IObuff > IOSIZE - 10)	/* getting too long!? */
		break;
	    switch (*s)
	    {
		case '|':   STRCPY(d, "bar");
			    d += 3;
			    continue;
		case '\"':  STRCPY(d, "quote");
			    d += 5;
			    continue;
		case '*':   *d++ = '.';
			    break;
		case '?':   *d++ = '.';
			    continue;
		case '$':
		case '.':
		case '~':   *d++ = '\\';
			    break;
	    }

	    /*
	     * Replace "^x" by "CTRL-X". Don't do this for "^_" to make
	     * ":help i_^_CTRL-D" work.
	     */
	    if (*s < ' ' || (*s == '^' && s[1] && s[1] != '_'))	/* ^x */
	    {
		if (d > IObuff && d[-1] != '_')
		    *d++ = '_';		/* prepend a '_' */
		STRCPY(d, "CTRL-");
		d += 5;
		if (*s < ' ')
		    *d++ = *s + '@';
		else
		    *d++ = *++s;
		if (s[1] != NUL && s[1] != '_')
		    *d++ = '_';		/* append a '_' */
		continue;
	    }
	    else if (*s == '^')		/* "^" or "CTRL-^" or "^_" */
		*d++ = '\\';

	    /*
	     * Insert a backslash before a backslash after a slash, for search
	     * pattern tags: "/\|" --> "/\\|".
	     */
	    else if (s[0] == '\\' && s[1] != '\\' &&
						  *arg == '/' && s == arg + 1)
		*d++ = '\\';

	    *d++ = *s;

	    /*
	     * If tag starts with ', toss everything after a second '. Fixes
	     * CTRL-] on 'option'. (would include the trailing '.').
	     */
	    if (*s == '\'' && s > arg && *arg == '\'')
		break;
	}
	*d = NUL;
    }

    *matches = (char_u **)"";
    *num_matches = 0;
    if (find_tags(IObuff, num_matches, matches,
			       TAG_HELP | TAG_WILD | TAG_NAMES, MAXCOL) == OK)
#ifdef HAVE_QSORT
	/*
	 * Sort the matches found on the heuristic number that is after the
	 * tag name.  If there is no qsort, the output will be messy!
	 */
	qsort((void *)*matches, (size_t)*num_matches,
					      sizeof(char_u *), help_compare)
#endif
	;
    return OK;
}
