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

#ifdef FEAT_EX_EXTRA
static int linelen __ARGS((int *has_tab));
#endif
static void do_filter __ARGS((linenr_t line1, linenr_t line2, exarg_t *eap,
				    char_u *cmd, int do_in, int do_out));
#ifdef FEAT_VIMINFO
static char_u *viminfo_filename __ARGS((char_u	*));
static void do_viminfo __ARGS((FILE *fp_in, FILE *fp_out, int want_info,
					     int want_marks, int force_read));
static int read_viminfo_up_to_marks __ARGS((char_u *line, FILE *fp,
						   int forceit, int writing));
#endif

static int check_overwrite __ARGS((exarg_t *eap, buf_t *buf, char_u *fname, char_u *ffname, int other));
static int check_readonly __ARGS((int *forceit, buf_t *buf));
#ifdef FEAT_AUTOCMD
static void delbuf_msg __ARGS((char_u *name));
#endif
static int do_sub_msg __ARGS((void));
static int
#ifdef __BORLANDC__
    _RTLENTRYF
#endif
	help_compare __ARGS((const void *s1, const void *s2));

/*
 * ":ascii" and "ga".
 */
/*ARGSUSED*/
    void
do_ascii(eap)
    exarg_t	*eap;
{
    int		c;
    char	buf1[20];
    char	buf2[20];
    char_u	buf3[5];
#ifdef FEAT_MBYTE
    int		c1 = 0;
    int		c2 = 0;
    int		len;

    if (cc_utf8)
	c = utfc_ptr2char(ml_get_cursor(), &c1, &c2);
    else
#endif
	c = gchar_cursor();
    if (c == NUL)
    {
	MSG("NUL");
	return;
    }

#ifdef FEAT_MBYTE
    IObuff[0] = NUL;
    if (!has_mbyte || (cc_dbcs && c < 0x100) || c < 0x80)
#endif
    {
	if (c == NL)	    /* NUL is stored as NL */
	    c = NUL;
	if (vim_isprintc(c) && (c < ' '
#ifndef EBCDIC
		    || c > '~'
#endif
			       ))
	{
	    transchar_nonprint(buf3, c);
	    sprintf(buf1, "  <%s>", (char *)buf3);
	}
	else
	    buf1[0] = NUL;
#ifndef EBCDIC
	if (c >= 0x80)
	    sprintf(buf2, "  <M-%s>", transchar(c & 0x7f));
	else
#endif
	    buf2[0] = NUL;
	sprintf((char *)IObuff, _("<%s>%s%s  %d,  Hex %02x,  Octal %03o"),
		transchar(c), buf1, buf2, c, c, c);
#ifdef FEAT_MBYTE
	c = c1;
	c1 = c2;
	c2 = 0;
#endif
    }

#ifdef FEAT_MBYTE
    /* Repeat for combining characters. */
    while (has_mbyte && (c >= 0x100 || (cc_utf8 && c >= 0x80)))
    {
	len = STRLEN(IObuff);
	/* This assumes every multi-byte char is printable... */
	if (len > 0)
	    IObuff[len++] = ' ';
	IObuff[len++] = '<';
	if (utf_iscomposing(c)
#ifdef USE_GUI
		&& !gui.in_use
#endif
		)
	    IObuff[len++] = ' '; /* draw composing char on top of a space */
	IObuff[len + mb_char2bytes(c, IObuff + len)] = NUL;
	if (c < 0x10000)
	    sprintf((char *)IObuff + STRLEN(IObuff),
					 "> %d, Hex %04x, Octal %o", c, c, c);
	else
	    sprintf((char *)IObuff + STRLEN(IObuff),
					 "> %d, Hex %08x, Octal %o", c, c, c);
	c = c1;
	c1 = c2;
	c2 = 0;
    }
#endif

    msg(IObuff);
}

#ifdef FEAT_EX_EXTRA
/*
 * ":left", ":center" and ":right": align text.
 */
    void
ex_align(eap)
    exarg_t	*eap;
{
    pos_t	save_curpos;
    int		len;
    int		indent = 0;
    int		new_indent;
    int		has_tab;
    int		width;

#ifdef FEAT_RIGHTLEFT
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
	    width = W_WIDTH(curwin) - curbuf->b_p_wm;
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
			(void)set_indent(new_indent, 0);
			if (linelen(NULL) <= width)
			{
			    /*
			     * Now try to move the line as much as possible to
			     * the right.  Stop when it moves too far.
			     */
			    do
				(void)set_indent(++new_indent, 0);
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
	(void)set_indent(new_indent, 0);		/* set indent */
    }
    changed_lines(eap->line1, 0, eap->line2 + 1, 0L);
    curwin->w_cursor = save_curpos;
    beginline(BL_WHITE | BL_FIX);
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
 * ":retab".
 */
    void
ex_retab(eap)
    exarg_t	*eap;
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
    int		did_undo;		/* called u_save for current line */
    int		new_ts;
    int		save_list;
    linenr_t	first_line = 0;		/* first changed line */
    linenr_t	last_line = 0;		/* last changed line */

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
			    mch_memmove(new_line, ptr, (size_t)start_col);
			mch_memmove(new_line + start_col + len,
				      ptr + col, (size_t)(old_len - col + 1));
			ptr = new_line + start_col;
			for (col = 0; col < len; col++)
			    ptr[col] = (col < num_tabs) ? '\t' : ' ';
			ml_replace(lnum, new_line, FALSE);
			if (first_line == 0)
			    first_line = lnum;
			last_line = lnum;
			ptr = new_line;
			col = start_col + len;
		    }
		}
		got_tab = FALSE;
		num_spaces = 0;
	    }
	    if (ptr[col] == NUL)
		break;
	    vcol += chartabsize(ptr + col, (colnr_t)vcol);
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		col += mb_ptr2len_check(ptr + col);
	    else
#endif
		++col;
	}
	if (new_line == NULL)		    /* out of memory */
	    break;
	line_breakcheck();
    }
    if (got_int)
	EMSG(_(e_interr));

    if (curbuf->b_p_ts != new_ts)
	redraw_curbuf_later(NOT_VALID);
    if (first_line != 0)
	changed_lines(first_line, 0, last_line + 1, 0L);

    curwin->w_p_list = save_list;	/* restore 'list' */

    curbuf->b_p_ts = new_ts;
    coladvance(curwin->w_curswant);

    u_clearline();
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
	EMSG(_("Move lines into themselves"));
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

    if (!global_busy && num_lines > p_report)
    {
	if (num_lines == 1)
	    MSG(_("1 line moved"));
	else
	    smsg((char_u *)_("%ld lines moved"), num_lines);
    }

    /*
     * Leave the cursor on the last of the moved lines.
     */
    if (dest >= line1)
	curwin->w_cursor.lnum = dest;
    else
	curwin->w_cursor.lnum = dest + (line2 - line1) + 1;

    if (line1 < dest)
	changed_lines(line1, 0, dest + num_lines + 1, 0L);
    else
	changed_lines(dest + 1, 0, line1 + num_lines, 0L);

    return OK;
}

/*
 * ":copy"
 */
    void
ex_copy(line1, line2, n)
    linenr_t	line1;
    linenr_t	line2;
    linenr_t	n;
{
    linenr_t	count;
    char_u	*p;

    count = line2 - line1 + 1;
    curbuf->b_op_start.lnum = n + 1;
    curbuf->b_op_end.lnum = n + count;
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
	/* need to use vim_strsave() because the line will be unlocked within
	 * ml_append() */
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

    appended_lines_mark(n, count);

    msgmore((long)count);
}

/*
 * Handle the ":!cmd" command.	Also for ":r !cmd" and ":w !cmd"
 * Bangs in the argument are replaced with the previously entered command.
 * Remember the argument.
 *
 * RISCOS: Bangs only replaced when followed by a space, since many
 * pathnames contain one.
 */
    void
do_bang(addr_count, eap, forceit, do_in, do_out)
    int		addr_count;
    exarg_t	*eap;
    int		forceit;
    int		do_in, do_out;
{
    char_u		*arg = eap->arg;	/* command */
    linenr_t		line1 = eap->line1;	/* start of range */
    linenr_t		line2 = eap->line2;	/* end of range */
    static char_u	*prevcmd = NULL;	/* the previous command */
    char_u		*newcmd = NULL;		/* the new command */
    int			free_newcmd = FALSE;    /* need to free() newcmd */
    int			ins_prevcmd;
    char_u		*t;
    char_u		*p;
    char_u		*trailarg;
    int			len;
    int			scroll_save = msg_scroll;

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
		EMSG(_(e_noprev));
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
	    if (*p == '!'
#ifdef RISCOS
			&& (p[1] == ' ' || p[1] == NUL)
#endif
					)
	    {
		if (p > newcmd && p[-1] == '\\')
		    mch_memmove(p - 1, p, (size_t)(STRLEN(p) + 1));
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
	do_filter(line1, line2, eap, newcmd, do_in, do_out);
    if (free_newcmd)
	vim_free(newcmd);
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
do_filter(line1, line2, eap, cmd, do_in, do_out)
    linenr_t	line1, line2;
    exarg_t	*eap;		/* for forced 'ff' and 'fcc' */
    char_u	*cmd;
    int		do_in, do_out;
{
    char_u	*itmp = NULL;
    char_u	*otmp = NULL;
    linenr_t	linecount;
    pos_t	cursor_save;
    char_u	*cmd_buf;
#ifdef FEAT_AUTOCMD
    buf_t	*old_curbuf = curbuf;
#endif

    if (*cmd == NUL)	    /* no filter command */
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
	EMSG(_(e_notmp));
	goto filterend;
    }

/*
 * The writing and reading of temp files will not be shown.
 * Vi also doesn't do this and the messages are not very informative.
 */
    ++no_wait_return;		/* don't call wait_return() while busy */
    if (do_in && buf_write(curbuf, itmp, NULL, line1, line2, eap,
					   FALSE, FALSE, FALSE, TRUE) == FAIL)
    {
	msg_putchar('\n');		/* keep message from buf_write() */
	--no_wait_return;
	(void)EMSG2(_(e_notcreate), itmp);	/* will call wait_return */
	goto filterend;
    }
#ifdef FEAT_AUTOCMD
    if (curbuf != old_curbuf)
	goto filterend;
#endif

    if (!do_out)
	msg_putchar('\n');

    cmd_buf = make_filter_cmd(cmd, itmp, otmp);
    if (cmd_buf == NULL)
	goto filterend;

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

    /*
     * When call_shell() fails wait_return() is called to give the user a
     * chance to read the error messages. Otherwise errors are ignored, so you
     * can see the error messages from the command that appear on stdout; use
     * 'u' to fix the text
     * Switch to cooked mode when not redirecting stdin, avoids that something
     * like ":r !cat" hangs.
     * Pass on the SHELL_DOOUT flag when the output is being redirected.
     */
    if (call_shell(cmd_buf, SHELL_FILTER | SHELL_COOKED |
					  (do_out ? SHELL_DOOUT : 0)))
    {
	must_redraw = CLEAR;
	wait_return(FALSE);
    }
    vim_free(cmd_buf);

    need_check_timestamps = TRUE;

    if (do_out)
    {
	if (u_save((linenr_t)(line2), (linenr_t)(line2 + 1)) == FAIL)
	{
	    goto error;
	}
	redraw_curbuf_later(VALID);
	if (readfile(otmp, NULL, line2, (linenr_t)0, (linenr_t)MAXLNUM, eap,
							 READ_FILTER) == FAIL)
	{
	    msg_putchar('\n');
	    EMSG2(_(e_notread), otmp);
	    goto error;
	}
#ifdef FEAT_AUTOCMD
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
	    del_lines(linecount, TRUE);
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
		sprintf((char *)msg_buf, _("%ld lines filtered"), (long)linecount);
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

#ifdef FEAT_AUTOCMD
    if (curbuf != old_curbuf)
    {
	--no_wait_return;
	EMSG(_("*Filter* Autocommands must not change current buffer"));
    }
#endif
    if (itmp != NULL)
	mch_remove(itmp);
    if (otmp != NULL)
	mch_remove(otmp);
    vim_free(itmp);
    vim_free(otmp);
}

/*
 * call a shell to execute a command
 */
    void
do_shell(cmd, flags)
    char_u	*cmd;
    int		flags;	/* may be SHELL_DOOUT when output is redirected */
{
    buf_t	*buf;
#ifndef FEAT_GUI_MSWIN
    int		save_nwr;
#endif
#ifdef MSWIN
    int		winstart = FALSE;
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

#ifdef MSWIN
    /*
     * Check if external commands are allowed now.
     */
    if (can_end_termcap_mode(TRUE) == FALSE)
	return;

    /*
     * Check if ":!start" is used.
     */
    if (cmd)
	winstart = (STRNICMP(cmd, "start ", 6) == 0);
#endif

    /*
     * For autocommands we want to get the output on the current screen, to
     * avoid having to type return below.
     */
    msg_putchar('\r');			/* put cursor at start of line */
#ifdef FEAT_AUTOCMD
    if (!autocmd_busy)
#endif
    {
#ifdef MSWIN
	if (!winstart)
#endif
	    stoptermcap();
    }
#ifdef MSWIN
    if (!winstart)
#endif
	msg_putchar('\n');		/* may shift screen one line up */

    /* warning message before calling the shell */
    if (p_warn
#ifdef FEAT_AUTOCMD
		&& !autocmd_busy
#endif
		&& !msg_silent)
	for (buf = firstbuf; buf; buf = buf->b_next)
	    if (bufIsChanged(buf))
	    {
#ifdef FEAT_GUI_MSWIN
		if (!winstart)
		    starttermcap();	/* don't want a message box here */
#endif
		MSG_PUTS(_("[No write since last change]\n"));
#ifdef FEAT_GUI_MSWIN
		if (!winstart)
		    stoptermcap();
#endif
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

#ifdef FEAT_AUTOCMD
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
#ifndef FEAT_GUI_MSWIN
	if (cmd == NULL
# ifdef WIN32
		|| (winstart && !need_wait_return)
# endif
	   )
	{
	    must_redraw = CLEAR;
	    need_wait_return = FALSE;
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
# ifdef AMIGA
	    wait_return(term_console ? -1 : TRUE);	/* see below */
# else
	    wait_return(TRUE);
# endif
	    no_wait_return = save_nwr;
	}
#endif /* FEAT_GUI_W32 */

#ifdef MSWIN
	if (!winstart) /* if winstart==TRUE, never stopped termcap! */
#endif
	    starttermcap();	/* start termcap if not done by wait_return() */

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
	    OUT_STR(IF_EB("\033[0 q", ESC_STR "[0 q"));	/* get window size */
	    if (got_int)
		must_redraw = CLEAR;	/* if got_int is TRUE, redraw needed */
	    else
		must_redraw = 0;	/* no extra redraw needed */
	}
#endif
    }

    /* display any error messages now */
    mch_display_error();
}

/*
 * Create a shell command from a command string, input redirection file and
 * output redirection file.
 * Returns an allocated string with the shell command, or NULL for failure.
 */
    char_u *
make_filter_cmd(cmd, itmp, otmp)
    char_u	*cmd;		/* command */
    char_u	*itmp;		/* NULL or name of input file */
    char_u	*otmp;		/* NULL or name of output file */
{
    char_u	*buf;
    long_u	len;
    char_u	*p;

    len = STRLEN(cmd) + 3;				/* "()" + NUL */
    if (itmp != NULL)
	len += STRLEN(itmp) + 9;			/* " { < " + " } " */
    if (otmp != NULL)
	len += STRLEN(otmp) + STRLEN(p_srr) + 2;	/* "  " */
    buf = lalloc(len, TRUE);
    if (buf == NULL)
	return NULL;

#if (defined(UNIX) && !defined(ARCHIE)) || defined(OS2)
    /*
     * put braces around the command (for concatenated commands)
     */
    sprintf((char *)buf, "(%s)", (char *)cmd);
    if (itmp != NULL)
    {
	STRCAT(buf, " < ");
	STRCAT(buf, itmp);
    }
#else
    /*
     * for shells that don't understand braces around commands, at least allow
     * the use of commands in a pipe.
     */
    STRCPY(buf, cmd);
    if (itmp != NULL)
    {
	/*
	 * If there is a pipe, we have to put the '<' in front of it.
	 * Don't do this when 'shellquote' is not empty, otherwise the
	 * redirection would be inside the quotes.
	 */
	if (*p_shq == NUL)
	{
	    p = vim_strchr(buf, '|');
	    if (p != NULL)
		*p = NUL;
	}
#ifdef RISCOS
	STRCAT(buf, " { < ");	/* Use RISC OS notation for input. */
	STRCAT(buf, itmp);
	STRCAT(buf, " } ");
#else
	STRCAT(buf, " <");	/* " < " causes problems on Amiga */
	STRCAT(buf, itmp);
#endif
	if (*p_shq == NUL)
	{
	    p = vim_strchr(cmd, '|');
	    if (p != NULL)
	    {
		STRCAT(buf, " ");   /* insert a space before the '|' for DOS */
		STRCAT(buf, p);
	    }
	}
    }
#endif
    if (otmp != NULL)
    {
	if ((p = vim_strchr(p_srr, '%')) != NULL && p[1] == 's')
	{
	    p = buf + STRLEN(buf);
	    *p++ = ' '; /* not really needed? Not with sh, ksh or bash */
	    sprintf((char *)p, (char *)p_srr, (char *)otmp);
	}
	else
	    sprintf((char *)buf + STRLEN(buf),
#ifndef RISCOS
		    " %s%s",	/* " %s %s" causes problems on Amiga */
#else
		    " %s %s",	/* But is needed for RISC OS */
#endif
		    (char *)p_srr, (char *)otmp);
    }

    return buf;
}

#ifdef FEAT_VIMINFO

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
    sprintf((char *)IObuff, _("viminfo: %s in line: "), message);
    STRNCAT(IObuff, line, IOSIZE - STRLEN(IObuff));
    emsg(IObuff);
    if (++viminfo_errcnt >= 10)
    {
	EMSG(_("viminfo: Too many errors, skipping rest of file"));
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
    if ((fp = mch_fopen((char *)file, READBIN)) == NULL)
	return FAIL;

    if (p_verbose > 0)
	smsg((char_u *)_("Reading viminfo file \"%s\"%s%s"), file,
		    want_info ? _(" info") : "",
		    want_marks ? _(" marks") : "");

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
    struct stat	    st_new;		/* mch_stat() of potential new file */
    char_u	    *wp;
#if defined(UNIX) || defined(VMS)
     mode_t	    umask_save;
#endif
#ifdef UNIX
    int		    shortname = FALSE;	/* use 8.3 file name */
    struct stat	    st_old;		/* mch_stat() of existing viminfo file */
#endif

    if (no_viminfo())
	return;

    file = viminfo_filename(file);	/* may set to default if NULL */
    file = vim_strsave(file);		/* make a copy, don't want NameBuff */

    if (file != NULL)
    {
	fp_in = mch_fopen((char *)file, READBIN);
	if (fp_in == NULL)
	{
	    /* if it does exist, but we can't read it, don't try writing */
	    if (mch_stat((char *)file, &st_new) == 0)
		goto end;
#if defined(UNIX) || defined(VMS)
	    /*
	     * For Unix we create the .viminfo non-accessible for others,
	     * because it may contain text from non-accessible documents.
	     */
	    umask_save = umask(077);
#endif
	    fp_out = mch_fopen((char *)file, WRITEBIN);
#if defined(UNIX) || defined(VMS)
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
	    if (mch_stat((char *)file, &st_old) == 0 && getuid() &&
		    !(st_old.st_uid == getuid()
			    ? (st_old.st_mode & 0200)
			    : (st_old.st_gid == getgid()
				    ? (st_old.st_mode & 0020)
				    : (st_old.st_mode & 0002))))
	    {
		int	tt;

		/* avoid a wait_return for this message, it's annoying */
		tt = msg_didany;
		EMSG2(_("Viminfo file is not writable: %s"), file);
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
#  ifdef FEAT_GUI_W32
					gui_is_win32s(),
#  else
					FALSE,
#  endif
# endif
#endif
					file,
#ifdef VMS
					(char_u *)"-tmp",
#else
# ifdef RISCOS
					(char_u *)"/tmp",
# else
					(char_u *)".tmp",
# endif
#endif
					FALSE);
		if (tempname == NULL)		/* out of memory */
		    break;

		/*
		 * Check if tempfile already exists.  Never overwrite an
		 * existing file!
		 */
		if (mch_stat((char *)tempname, &st_new) == 0)
		{
#ifdef UNIX
		    /*
		     * Check if tempfile is same as original file.  May happen
		     * when modname() gave the same file back.  E.g.  silly
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
		    for (*wp = 'z'; mch_stat((char *)tempname, &st_new) == 0; --*wp)
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
		fp_out = mch_fopen((char *)tempname, WRITEBIN);

		/*
		 * If we can't create in the same directory, try creating a
		 * "normal" temp file.
		 */
		if (fp_out == NULL)
		{
		    vim_free(tempname);
		    if ((tempname = vim_tempname('o')) != NULL)
			fp_out = mch_fopen((char *)tempname, WRITEBIN);
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
	EMSG2(_("Can't write viminfo file %s!"), file == NULL ? (char_u *)"" :
					      fp_in == NULL ? file : tempname);
	if (fp_in != NULL)
	    fclose(fp_in);
	goto end;
    }

    if (p_verbose > 0)
	smsg((char_u *)_("Writing viminfo file \"%s\""), file);

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
	    mch_remove(tempname);
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
# ifdef VMS
	    if (mch_getenv((char_u *)"SYS$LOGIN") == NULL)
# else
	    if (mch_getenv((char_u *)"HOME") == NULL)
# endif
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
	fprintf(fp_out, _("# This viminfo file was generated by vim\n"));
	fprintf(fp_out, _("# You may edit it if you're careful!\n\n"));
	write_viminfo_search_pattern(fp_out);
	write_viminfo_sub_string(fp_out);
	write_viminfo_history(fp_out);
	write_viminfo_registers(fp_out);
#ifdef FEAT_EVAL
	write_viminfo_varlist(fp_out);
#endif
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
	    case '^': /* to be defined */
	    case '*': /* to be defined */
	    case '<': /* long line - ignored */
		/* A comment */
	    case NUL:
	    case '\r':
	    case '\n':
	    case '#':
		eof = vim_fgets(line, LSIZE, fp);
		break;
	    case '!': /* global variable */
#ifdef FEAT_EVAL
		eof = read_viminfo_varlist(line, fp, writing);
#else
		eof = vim_fgets(line, LSIZE, fp);
#endif
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
	    case '@':
		eof = read_viminfo_history(line, fp);
		break;
	    case '\'':
		/* How do we have a file mark when the file is not in the
		 * buffer list?
		 */
		eof = read_viminfo_filemark(line, fp, forceit);
		break;
	    default:
		if (viminfo_error(_("Illegal starting char"), line))
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
 *
 * Check for a long line as written by viminfo_writestring().
 *
 * Return the string in allocated memory (NULL when out of memory).
 */
    char_u *
viminfo_readstring(p, fp)
    char_u	*p;
    FILE	*fp;
{
    char_u	*retval;
    char_u	*s, *d;
    long	len;

    if (p[0] == Ctrl_V && isdigit(p[1]))
    {
	len = atol((char *)p + 1);
	retval = lalloc(len, TRUE);
	if (retval == NULL)
	{
	    /* Line too long?  File messed up?  Skip next line. */
	    (void)vim_fgets(p, 10, fp);
	    return NULL;
	}
	(void)vim_fgets(retval, (int)len, fp);
	s = retval + 1;	    /* Skip the leading '<' */
    }
    else
    {
	retval = vim_strsave(p);
	if (retval == NULL)
	    return NULL;
	s = retval;
    }

    /* Change CTRL-V CTRL-V to CTRL-V and CTRL-V n to \n in-place. */
    d = retval;
    while (*s != NUL && *s != '\n')
    {
	if (s[0] == Ctrl_V && s[1] != NUL)
	{
	    if (s[1] == 'n')
		*d++ = '\n';
	    else
		*d++ = Ctrl_V;
	    s += 2;
	}
	else
	    *d++ = *s++;
    }
    *d = NUL;
    return retval;
}

/*
 * write string to viminfo file
 * - replace CTRL-V with CTRL-V CTRL-V
 * - replace '\n'   with CTRL-V 'n'
 * - add a '\n' at the end
 *
 * For a long line:
 * - write " CTRL-V <length> \n " in first line
 * - write " < <string> \n "	  in second line
 */
    void
viminfo_writestring(fd, p)
    FILE	*fd;
    char_u	*p;
{
    int		c;
    char_u	*s;
    int		len = 0;

    for (s = p; *s != NUL; ++s)
    {
	if (*s == Ctrl_V || *s == '\n')
	    ++len;
	++len;
    }

    /* If the string will be too long, write its length and put it in the next
     * line.  Take into account that some room is needed for what comes before
     * the string (e.g., variable name).  Add something to the length for the
     * '<', NL and trailing NUL. */
    if (len > LSIZE / 2)
	fprintf(fd, IF_EB("\026%d\n<", CTRL_V_STR "%d\n<"), len + 3);

    while ((c = *p++) != NUL)
    {
	if (c == Ctrl_V || c == '\n')
	{
	    putc(Ctrl_V, fd);
	    if (c == '\n')
		c = 'n';
	}
	putc(c, fd);
    }
    putc('\n', fd);
}
#endif /* FEAT_VIMINFO */

/*
 * Implementation of ":fixdel", also used by get_stty().
 *  <BS>    resulting <Del>
 *   ^?		^H
 * not ^?	^?
 */
/*ARGSUSED*/
    void
do_fixdel(eap)
    exarg_t	*eap;
{
    char_u  *p;

    p = find_termcode((char_u *)"kb");
    add_termcode((char_u *)"kD", p != NULL
	    && *p == DEL ? (char_u *)CTRL_H_STR : DEL_STR, FALSE);
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
	msg_puts_attr(numbuf, hl_attr(HLF_N));	/* Highlight line nrs */
    }
    msg_prt_line(ml_get(lnum));
}

/*
 * Print a text line.  Also in silent mode ("ex -s").
 */
    void
print_line(lnum, use_number)
    linenr_t	lnum;
    int		use_number;
{
    int		save_silent = silent_mode;

    silent_mode = FALSE;
    msg_start();
    print_line_no_prefix(lnum, use_number);
    if (save_silent)
    {
	msg_putchar('\n');
	cursor_on();		/* msg_start() switches it off */
	out_flush();
	silent_mode = save_silent;
    }
}

/*
 * ":file[!] [fname]".
 */
    void
ex_file(eap)
    exarg_t	*eap;
{
    char_u	*fname, *sfname, *xfname;
    buf_t	*buf;

    if (*eap->arg != NUL)
    {
#ifdef FEAT_AUTOCMD
	buf = curbuf;
	apply_autocmds(EVENT_BUFFILEPRE, NULL, NULL, FALSE, curbuf);
	/* buffer changed, don't change name now */
	if (buf != curbuf)
	    return;
#endif
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
	if (setfname(eap->arg, NULL, TRUE) == FAIL)
	{
	    curbuf->b_ffname = fname;
	    curbuf->b_sfname = sfname;
	    return;
	}
	curbuf->b_flags |= BF_NOTEDITED;
	buf = buflist_new(fname, xfname, curwin->w_cursor.lnum, FALSE);
	if (buf != NULL)
	    curwin->w_alt_fnum = buf->b_fnum;
	vim_free(fname);
	vim_free(sfname);
#ifdef FEAT_AUTOCMD
	apply_autocmds(EVENT_BUFFILEPOST, NULL, NULL, FALSE, curbuf);
#endif
    }
    /* print full file name if :cd used */
    fileinfo(FALSE, FALSE, eap->forceit);
}

/*
 * ":update".
 */
    void
ex_update(eap)
    exarg_t	*eap;
{
    if (curbufIsChanged())
	(void)do_write(eap);
}

/*
 * ":write".
 */
    void
ex_write(eap)
    exarg_t	*eap;
{
    if (eap->usefilter)		/* input lines to shell command */
	do_bang(1, eap, FALSE, TRUE, FALSE);
    else
	(void)do_write(eap);
}

/*
 * write current buffer to file 'eap->arg'
 * if 'eap->append' is TRUE, append to the file
 *
 * if *eap->arg == NUL write to current file
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_write(eap)
    exarg_t	*eap;
{
    int		other;
    char_u	*fname = NULL;		/* init to shut up gcc */
    char_u	*ffname;
    int		retval = FAIL;
    char_u	*free_fname = NULL;
#ifdef FEAT_BROWSE
    char_u	*browse_file = NULL;
#endif

    if (not_writing())		/* check 'write' option */
	return FAIL;

    ffname = eap->arg;
#ifdef FEAT_BROWSE
    if (cmdmod.browse)
    {
	browse_file = do_browse(TRUE, (char_u *)_("Save As"), NULL,
						  NULL, ffname, NULL, curbuf);
	if (browse_file == NULL)
	    goto theend;
	ffname = browse_file;
    }
#endif
    if (*ffname == NUL)
	other = FALSE;
    else
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	if (free_fname != NULL)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, put its name in the list of alternate file names.
     */
    if (other && vim_strchr(p_cpo, CPO_ALTWRITE) != NULL)
	setaltfname(ffname, fname, (linenr_t)1);

    /*
     * Writing to the current file is not allowed in readonly mode
     * and need a file name.
     * "nofile" and "scratch" buffers cannot be written implicitly either.
     */
    if (!other && (
#ifdef FEAT_QUICKFIX
		bt_nofile(curbuf) || bt_scratch(curbuf) ||
#endif
		check_fname() == FAIL || check_readonly(&eap->forceit, curbuf)))
	goto theend;

    if (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	if (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_line_count)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    if (p_confirm || cmdmod.confirm)
	    {
		if (vim_dialog_yesno(VIM_QUESTION, NULL,
			       (char_u *)_("Write partial file?"), 2) != VIM_YES)
		    goto theend;
		eap->forceit = TRUE;
	    }
	    else
#endif
	    {
		EMSG(_("Use ! to write partial buffer"));
		goto theend;
	    }
	}
    }

    if (check_overwrite(eap, curbuf, fname, ffname, other) == OK)
	retval = (buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				eap, eap->append, eap->forceit, TRUE, FALSE));

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    return retval;
}

/*
 * Check if it is allowed to overwrite a file.  If b_flags has BF_NOTEDITED,
 * BF_NEW or BF_READERR, check for overwriting current file.
 * May set eap->forceit if a dialog says it's OK to overwrite.
 * Return OK if it's OK, FAIL if it is not.
 */
/*ARGSUSED*/
    static int
check_overwrite(eap, buf, fname, ffname, other)
    exarg_t	*eap;
    buf_t	*buf;
    char_u	*fname;	    /* file name to be used (can differ from
			       buf->ffname) */
    char_u	*ffname;    /* full path version of fname */
    int		other;	    /* writing under other name */
{
    /*
     * write to other file or b_flags set or not writing the whole file:
     * overwriting only allowed with '!'
     */
    if (       (other
		|| (buf->b_flags & BF_NOTEDITED)
		|| ((buf->b_flags & BF_NEW)
		    && vim_strchr(p_cpo, CPO_OVERNEW) == NULL)
		|| (buf->b_flags & BF_READERR))
	    && !eap->forceit
	    && !eap->append
	    && !p_wa
	    && vim_fexists(ffname))
    {
#ifdef UNIX
	    /* with UNIX it is possible to open a directory */
	if (mch_isdir(ffname))
	{
	    EMSG2(_("\"%s\" is a directory"), ffname);
	    return FAIL;
	}
#endif
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	if (p_confirm || cmdmod.confirm)
	{
	    char_u	buff[IOSIZE];

	    dialog_msg(buff, _("Overwrite existing file \"%.*s\"?"), fname);
	    if (vim_dialog_yesno(VIM_QUESTION, NULL, buff, 2) != VIM_YES)
		return FAIL;
	    eap->forceit = TRUE;
	}
	else
#endif
	{
	    EMSG(_(e_exists));
	    return FAIL;
	}
    }
    return OK;
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    void
ex_wnext(eap)
    exarg_t	*eap;
{
    int		i;

    if (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (int)eap->line2;
    else
	i = curwin->w_arg_idx - (int)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_line_count;
    if (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    void
do_wqall(eap)
    exarg_t	*eap;
{
    buf_t	*buf;
    int		error = 0;

    if (eap->cmdidx == CMD_xall || eap->cmdidx == CMD_wqall)
	exiting = TRUE;

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
    {
	if (bufIsChanged(buf))
	{
	    /*
	     * Check if there is a reason the buffer cannot be written:
	     * 1. if the 'write' option is set
	     * 2. if there is no file name (even after browsing)
	     * 3. if the 'readonly' is set (even after a dialog)
	     * 4. if overwriting is allowed (even after a dialog)
	     */
	    if (not_writing())
	    {
		++error;
		break;
	    }
#ifdef FEAT_BROWSE
	    /* ":browse wall": ask for file name if there isn't one */
	    if (buf->b_ffname == NULL && cmdmod.browse)
		buf->b_ffname = do_browse(TRUE, (char_u *)_("Save As"), NULL,
					       NULL, (char_u *)"", NULL, buf);
#endif
	    if (buf->b_ffname == NULL)
	    {
		EMSG(_(e_noname));
		++error;
	    }
	    else if (check_readonly(&eap->forceit, buf)
		    || check_overwrite(eap, buf, buf->b_fname, buf->b_ffname,
							       FALSE) == FAIL)
	    {
		++error;
	    }
	    else
	    {
		if (buf_write_all(buf) == FAIL)
		    ++error;
#ifdef FEAT_AUTOCMD
		/* an autocommand may have deleted the buffer */
		if (!buf_valid(buf))
		    buf = firstbuf;
#endif
	    }
	}
    }
    if (exiting)
    {
	if (!error)
	    getout(0);		/* exit Vim */
	not_exiting();
    }
}

/*
 * Check the 'write' option.
 * Return TRUE and give a message when it's not st.
 */
    int
not_writing()
{
    if (p_write)
	return FALSE;
    EMSG(_("File not written: Writing is disabled by 'write' option"));
    return TRUE;
}

/*
 * Check if a buffer is read-only.  Ask for overruling in a dialog.
 * Return TRUE and give an error message when the buffer is readonly.
 */
    static int
check_readonly(forceit, buf)
    int		*forceit;
    buf_t	*buf;
{
    if (!*forceit && buf->b_p_ro)
    {
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	if ((p_confirm || cmdmod.confirm) && buf->b_fname != NULL)
	{
	    char_u	buff[IOSIZE];

	    dialog_msg(buff, _("'readonly' option is set for \"%.*s\".\nDo you wish to override it?"),
		    buf->b_fname);

	    if (vim_dialog_yesno(VIM_QUESTION, NULL, buff, 2) == VIM_YES)
	    {
		/* Set forceit, to force the writing of a readonly file */
		*forceit = TRUE;
		return FALSE;
	    }
	    else
		return TRUE;
	}
	else
#endif
	    EMSG(_(e_readonly));
	return TRUE;
    }
    return FALSE;
}

/*
 * try to abandon current file and edit a new or existing file
 * 'fnum' is the number of the file, if zero use ffname/sfname
 *
 * return 1 for "normal" error, 2 for "not written" error, 0 for success
 * -1 for succesfully opening another file
 * 'lnum' is the line number for the cursor in the new file (if non-zero).
 */
    int
getfile(fnum, ffname, sfname, setpm, lnum, forceit)
    int		fnum;
    char_u	*ffname;
    char_u	*sfname;
    int		setpm;
    linenr_t	lnum;
    int		forceit;
{
    int		other;
    int		retval;
    char_u	*free_me = NULL;

    if (fnum == 0)
    {
	fname_expand(&ffname, &sfname);	/* make ffname full path, set sfname */
	other = otherfile(ffname);
	free_me = ffname;		/* has been allocated, free() later */
    }
    else
	other = (fnum != curbuf->b_fnum);

    if (other)
	++no_wait_return;	    /* don't wait for autowrite message */
    if (other && !forceit && curbuf->b_nwindows == 1 && !P_HID(curbuf)
		   && curbufIsChanged() && autowrite(curbuf, forceit) == FAIL)
    {
	if (other)
	    --no_wait_return;
	EMSG(_(e_nowrtmsg));
	retval = 2;	/* file has been changed */
	goto theend;
    }
    if (other)
	--no_wait_return;
    if (setpm)
	setpcmark();
    if (!other)
    {
	if (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = 0;	/* it's in the same file */
    }
    else if (do_ecmd(fnum, ffname, sfname, NULL, lnum,
		(P_HID(curbuf) ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0)) == OK)
	retval = -1;	/* opened another file */
    else
	retval = 1;	/* error encountered */

theend:
    vim_free(free_me);
    return retval;
}

/*
 * start editing a new file
 *
 *     fnum: file number; if zero use ffname/sfname
 *   ffname: the file name
 *		- full path if sfname used,
 *		- any file name if sfname is NULL
 *		- empty string to re-edit with the same file name (but may be
 *		    in a different directory)
 *		- NULL to start an empty buffer
 *   sfname: the short file name (or NULL)
 *	eap: contains the command to be executed after loading the file and
 *	     forced 'ff' and 'fcc'
 *  newlnum: if > 0: put cursor on this line number (if possible)
 *	     if ECMD_LASTL: use last position in loaded file
 *	     if ECMD_LAST: use last position in all files
 *	     if ECMD_ONE: use first line
 *    flags:
 *	   ECMD_HIDE: if TRUE don't free the current buffer
 *     ECMD_SET_HELP: set b_help flag of (new) buffer before opening file
 *	 ECMD_OLDBUF: use existing buffer if it exists
 *	ECMD_FORCEIT: ! used for Ex command
 *	 ECMD_ADDBUF: don't edit, just add to buffer list
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_ecmd(fnum, ffname, sfname, eap, newlnum, flags)
    int		fnum;
    char_u	*ffname;
    char_u	*sfname;
    exarg_t	*eap;
    linenr_t	newlnum;
    int		flags;
{
    int		other_file;		/* TRUE if editing another file */
    int		oldbuf;			/* TRUE if using existing buffer */
#ifdef FEAT_AUTOCMD
    int		auto_buf = FALSE;	/* TRUE if autocommands brought us
					   into the buffer unexpectedly */
    char_u	*new_name = NULL;
#endif
    buf_t	*buf;
#if defined(FEAT_AUTOCMD) || defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    buf_t	*old_curbuf = curbuf;
#endif
    char_u	*free_fname = NULL;
#ifdef FEAT_BROWSE
    char_u	*browse_file = NULL;
#endif
    int		retval = FAIL;
    long	n;
    linenr_t	lnum;
    linenr_t	topline = 0;
    int		newcol = -1;
    int		solcol = -1;
    pos_t	*pos;
#ifdef FEAT_SUN_WORKSHOP
    char_u	*cp;
#endif
    char_u	*command = NULL;

    if (eap != NULL)
	command = eap->do_ecmd_cmd;

    if (fnum != 0)
    {
	if (fnum == curbuf->b_fnum)	/* file is already being edited */
	    return OK;			/* nothing to do */
	other_file = TRUE;
    }
    else
    {
#ifdef FEAT_BROWSE
	if (cmdmod.browse)
	{
	    browse_file = do_browse(FALSE, (char_u *)_("Edit File"), NULL,
						  NULL, ffname, NULL, curbuf);
	    if (browse_file == NULL)
		goto theend;
	    ffname = browse_file;
	}
#endif
	/* if no short name given, use ffname for short name */
	if (sfname == NULL)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
# ifdef USE_LONG_FNAME
	if (USE_LONG_FNAME)
# endif
	    fname_case(sfname);	    /* set correct case for short file name */
#endif

	if ((flags & ECMD_ADDBUF) && (ffname == NULL || *ffname == NUL))
	    goto theend;

	if (ffname == NULL)
	    other_file = TRUE;
					    /* there is no file name */
	else if (*ffname == NUL && curbuf->b_ffname == NULL)
	    other_file = FALSE;
	else
	{
	    if (*ffname == NUL)		    /* re-edit with same file name */
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); /* may expand to full path name */
	    if (free_fname != NULL)
		ffname = free_fname;
	    other_file = otherfile(ffname);
#ifdef FEAT_SUN_WORKSHOP
	    if (usingSunWorkShop && (cp = vim_strrchr(sfname, '/')) != NULL)
		sfname = ++cp;
#endif
	}
    }

    /*
     * if the file was changed we may not be allowed to abandon it
     * - if we are going to re-edit the same file
     * - or if we are the only window on this file and if ECMD_HIDE is FALSE
     */
    if (  ((!other_file && !(flags & ECMD_OLDBUF))
	    || (curbuf->b_nwindows == 1
		&& !(flags & (ECMD_HIDE | ECMD_ADDBUF))))
	&& check_changed(curbuf, FALSE, !other_file,
					(flags & ECMD_FORCEIT), FALSE))
    {
	if (fnum == 0 && other_file && ffname != NULL)
	    setaltfname(ffname, sfname, newlnum < 0 ? 0 : newlnum);
	goto theend;
    }

#ifdef FEAT_VISUAL
    /*
     * End Visual mode before switching to another buffer, so the text can be
     * copied into the GUI selection buffer.
     */
    reset_VIsual();
#endif

    /*
     * If we are starting to edit another file, open a (new) buffer.
     * Otherwise we re-use the current buffer.
     */
    if (other_file)
    {
	if (!(flags & ECMD_ADDBUF))
	{
	    curwin->w_alt_fnum = curbuf->b_fnum;
	    buflist_altfpos();
	}

	if (fnum)
	    buf = buflist_findnr(fnum);
	else
	{
	    if (flags & ECMD_ADDBUF)
	    {
		linenr_t	tlnum = 1L;

		if (command != NULL)
		{
		    tlnum = atol((char *)command);
		    if (tlnum <= 0)
			tlnum = 1L;
		}
		(void)buflist_new(ffname, sfname, tlnum, FALSE);
		goto theend;
	    }
	    buf = buflist_new(ffname, sfname, 0L, TRUE);
	}
	if (buf == NULL)
	    goto theend;
	if (buf->b_ml.ml_mfp == NULL)		/* no memfile yet */
	{
	    oldbuf = FALSE;
	    buf->b_nwindows = 0;
	}
	else					/* existing memfile */
	{
	    oldbuf = TRUE;
	    (void)buf_check_timestamp(buf, FALSE);
	}

	/* May jump to last used line number for a loaded buffer or when asked
	 * for explicitly */
	if ((oldbuf && newlnum == ECMD_LASTL) || newlnum == ECMD_LAST)
	{
	    pos = buflist_findfpos(buf);
	    newlnum = pos->lnum;
	    solcol = pos->col;
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new().
	 */
	if (buf != curbuf)
	{
#ifdef FEAT_AUTOCMD
	    /*
	     * Be careful: The autocommands may delete any buffer and change
	     * the current buffer.
	     * - If the buffer we are going to edit is deleted, give up.
	     * - If the current buffer is deleted, prefer to load the new
	     *   buffer when loading a buffer is required.  This avoids
	     *   loading another buffer which then must be closed again.
	     * - If we ended up in the new buffer already, need to skip a few
	     *	 things, set auto_buf.
	     */
	    if (buf->b_fname != NULL)
		new_name = vim_strsave(buf->b_fname);
	    au_new_curbuf = buf;
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf);
	    if (!buf_valid(buf))	/* new buffer has been deleted */
	    {
		delbuf_msg(new_name);	/* frees new_name */
		goto theend;
	    }
	    if (buf == curbuf)		/* already in new buffer */
		auto_buf = TRUE;
	    else
	    {
		if (curbuf == old_curbuf)
#endif
		    buf_copy_options(buf, BCO_ENTER);

		/* close the current buffer */
		close_buffer(curwin, curbuf, !(flags & ECMD_HIDE), FALSE);

#ifdef FEAT_AUTOCMD
		/* Be careful again, like above. */
		if (!buf_valid(buf))	/* new buffer has been deleted */
		{
		    delbuf_msg(new_name);	/* frees new_name */
		    goto theend;
		}
		if (buf == curbuf)		/* already in new buffer */
		    auto_buf = TRUE;
		else
#endif

		{
		    curwin->w_buffer = buf;
		    curbuf = buf;
		    ++curbuf->b_nwindows;
		    /* set 'fileformat' */
		    if (*p_ffs && !oldbuf)
			set_fileformat(default_fileformat(), TRUE);
		}

		/* May get the window options from the last time this buffer
		 * was in this window (or another window).  If not used
		 * before, reset the local window options to the global
		 * values. */
		get_winopts(buf);

#ifdef FEAT_AUTOCMD
	    }
	    vim_free(new_name);
	    au_new_curbuf = NULL;
#endif
	}
	else
	    ++curbuf->b_nwindows;

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.col = 0;
    }
    else /* !other_file */
    {
	if ((flags & ECMD_ADDBUF) || check_fname() == FAIL)
	    goto theend;
	oldbuf = (flags & ECMD_OLDBUF);
    }

    if ((flags & ECMD_SET_HELP) || keep_help_flag)
    {
	curbuf->b_help = TRUE;
	curbuf->b_p_bin = FALSE;	/* reset 'bin' before reading file */
    }

/*
 * other_file	oldbuf
 *  FALSE	FALSE	    re-edit same file, buffer is re-used
 *  FALSE	TRUE	    re-edit same file, nothing changes
 *  TRUE	FALSE	    start editing new file, new buffer
 *  TRUE	TRUE	    start editing in existing buffer (nothing to do)
 */
    if (!other_file && !oldbuf)		/* re-use the buffer */
    {
	set_last_cursor(curwin);	/* may set b_last_cursor */
	if (newlnum == ECMD_LAST || newlnum == ECMD_LASTL)
	{
	    newlnum = curwin->w_cursor.lnum;
	    solcol = curwin->w_cursor.col;
	}
#ifdef FEAT_AUTOCMD
	buf = curbuf;
	if (buf->b_fname != NULL)
	    new_name = vim_strsave(buf->b_fname);
	else
	    new_name = NULL;
#endif
	buf_freeall(curbuf, FALSE);	/* free all things for buffer */
#ifdef FEAT_AUTOCMD
	/* If autocommands deleted the buffer we were going to re-edit, give
	 * up and jump to the end. */
	if (!buf_valid(buf))
	{
	    delbuf_msg(new_name);	/* frees new_name */
	    goto theend;
	}
	vim_free(new_name);

	/* If autocommands change buffers under our fingers, forget about
	 * re-editing the file.  Should do the buf_clear(), but perhaps the
	 * autocommands changed the buffer... */
	if (buf != curbuf)
	    goto theend;
#endif
	buf_clear(curbuf);
	curbuf->b_op_start.lnum = 0;	/* clear '[ and '] marks */
	curbuf->b_op_end.lnum = 0;
    }

/*
 * If we get here we are sure to start editing
 */
    /* don't redraw until the cursor is in the right line */
    ++RedrawingDisabled;

    /* Assume success now */
    retval = OK;

    /*
     * Reset cursor position, could be used by autocommands.
     */
    adjust_cursor();

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx(curwin);

#ifdef FEAT_AUTOCMD
    if (!auto_buf)
#endif
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	win_init(curwin);

#ifdef FEAT_SUN_WORKSHOP
	if (usingSunWorkShop && curbuf->b_ffname)
	    vim_chdirfile(curbuf->b_ffname);
#endif
	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	lnum = curwin->w_cursor.lnum;
	topline = curwin->w_topline;
	if (!oldbuf)			    /* need to read the file */
	{
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    swap_exists_action = SEA_DIALOG;
#endif
	    curbuf->b_flags |= BF_CHECK_RO; /* set/reset 'ro' flag */

	    /*
	     * Open the buffer and read the file.
	     */
	    (void)open_buffer(FALSE, eap);

#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    if (swap_exists_action == SEA_QUIT)
		retval = FAIL;
	    handle_swap_exists(old_curbuf);
#endif
	}
#ifdef FEAT_AUTOCMD
	else
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
	check_arg_idx(curwin);
#endif

	/*
	 * If autocommands change the cursor position or topline, we should
	 * keep it.
	 */
	if (curwin->w_cursor.lnum != lnum)
	{
	    newlnum = curwin->w_cursor.lnum;
	    newcol = curwin->w_cursor.col;
	}
	if (curwin->w_topline == topline)
	    topline = 0;

	/* Even when cursor didn't move we need to recompute topline. */
	changed_line_abv_curs();

#ifdef FEAT_FOLDING
	/* It's like all lines in the buffer changed.  Need to update
	 * automatic folding. */
	if (other_file)
	    clearFolding(curwin);
	foldUpdateAll(curwin);
#endif

#ifdef FEAT_TITLE
	maketitle();
#endif
    }

    if (command == NULL)
    {
	if (newcol >= 0)	/* position set by autocommands */
	{
	    curwin->w_cursor.lnum = newlnum;
	    curwin->w_cursor.col = newcol;
	    adjust_cursor();
	}
	else if (newlnum > 0)	/* line number from caller or old position */
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    if (solcol >= 0 && !p_sol)
	    {
		/* 'sol' is off: Use last known column. */
		curwin->w_cursor.col = solcol;
		check_cursor_col();
	    }
	    else
		beginline(BL_SOL | BL_FIX);
	}
	else			/* no line number, go to last line in Ex mode */
	{
	    if (exmode_active)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    /* Check if cursors in other windows on the same buffer are still valid */
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    if (oldbuf
#ifdef FEAT_AUTOCMD
		&& !auto_buf
#endif
			    )
	fileinfo(FALSE, TRUE, FALSE);

    if (command != NULL)
	do_cmdline(command, NULL, NULL, DOCMD_VERBOSE);
    --RedrawingDisabled;
    if (!skip_redraw)
    {
	n = p_so;
	if (topline == 0 && command == NULL)
	    p_so = 999;			/* force cursor halfway the window */
	update_topline();
#ifdef FEAT_SCROLLBIND
	curwin->w_scbind_pos = curwin->w_topline;
#endif
	p_so = n;
	redraw_curbuf_later(NOT_VALID);	/* redraw this buffer later */
    }

    if (p_im)
	need_start_insertmode = TRUE;

#ifdef FEAT_SUN_WORKSHOP
    if (usingSunWorkShop && curbuf->b_ffname)
	vim_chdirfile(curbuf->b_ffname);

    if (gui.in_use && curbuf != NULL && curbuf->b_fname != NULL)
	workshop_file_opened((char *)curbuf->b_ffname, curbuf->b_p_ro);
#endif

theend:
#ifdef FEAT_BROWSE
    vim_free(browse_file);
#endif
    vim_free(free_fname);
    return retval;
}

#ifdef FEAT_AUTOCMD
    static void
delbuf_msg(name)
    char_u	*name;
{
    EMSG2(_("Autocommands unexpectedly deleted new buffer %s"),
	    name == NULL ? (char_u *)"" : name);
    vim_free(name);
    au_new_curbuf = NULL;
}
#endif

/*
 * ":insert" and ":append", also used by ":change"
 */
    void
ex_append(eap)
    exarg_t	*eap;
{
    char_u	*theline;
    int		did_undo = FALSE;
    linenr_t	lnum = eap->line2;

    if (eap->cmdidx != CMD_append)
	--lnum;

    State = INSERT;		    /* behave like in Insert mode */
    while (1)
    {
	msg_scroll = TRUE;
	need_wait_return = FALSE;
	if (eap->getline == NULL)
	    theline = getcmdline(
#ifdef FEAT_EVAL
		    eap->cstack->cs_whilelevel > 0 ? -1 :
#endif
		    NUL, 0L, 0);
	else
	    theline = eap->getline(
#ifdef FEAT_EVAL
		    eap->cstack->cs_whilelevel > 0 ? -1 :
#endif
		    NUL, eap->cookie, 0);
	lines_left = Rows - 1;
	if (theline == NULL || (theline[0] == '.' && theline[1] == NUL))
	    break;

	if (!did_undo && u_save(lnum, lnum + 1) == FAIL)
	    break;
	did_undo = TRUE;
	ml_append(lnum, theline, (colnr_t)0, FALSE);
	appended_lines_mark(lnum, 1L);

	vim_free(theline);
	++lnum;
	msg_didout = TRUE;	/* also scroll for empty line */
    }
    State = NORMAL;

    /* "start" is set to eap->line2+1 unless that position is invalid (when
     * eap->line2 pointed to the end of the buffer and nothig was appended)
     * "end" is set to lnum when something has been appended, otherwise
     * it is the same than "start"  -- Acevedo */
    curbuf->b_op_start.lnum = (eap->line2 < curbuf->b_ml.ml_line_count) ?
	eap->line2 + 1 : curbuf->b_ml.ml_line_count;
    curbuf->b_op_end.lnum = (eap->line2 < lnum)
					     ? lnum : curbuf->b_op_start.lnum;
    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;
    curwin->w_cursor.lnum = lnum;
    check_cursor_lnum();
    beginline(BL_SOL | BL_FIX);

    need_wait_return = FALSE;	/* don't use wait_return() now */
    ex_no_reprint = TRUE;
}

/*
 * ":change"
 */
    void
ex_change(eap)
    exarg_t	*eap;
{
    linenr_t	lnum;

    if (eap->line2 >= eap->line1
	    && u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
	return;

    for (lnum = eap->line2; lnum >= eap->line1; --lnum)
    {
	if (curbuf->b_ml.ml_flags & ML_EMPTY)	    /* nothing to delete */
	    break;
	ml_delete(eap->line1, FALSE);
    }
    deleted_lines_mark(eap->line1, (long)(eap->line2 - lnum));

    /* ":append" on the line above the deleted lines. */
    eap->line2 = eap->line1;
    ex_append(eap);
}

    void
ex_z(eap)
    exarg_t	*eap;
{
    char_u	*x;
    int		bigness = curwin->w_height - 3;
    char_u	kind;
    int		minus = 0;
    linenr_t	start, end, curs, i;
    linenr_t	lnum = eap->line2;

    if (bigness < 1)
	bigness = 1;

    x = eap->arg;
    if (*x == '-' || *x == '+' || *x == '=' || *x == '^' || *x == '.')
	x++;

    if (*x != 0)
    {
	if (!isdigit(*x))
	{
	    EMSG(_("non-numeric argument to :z"));
	    return;
	}
	else
	    bigness = atoi((char *)x);
    }

    kind = *eap->arg;

    switch (kind)
    {
	case '-':
	    start = lnum - bigness;
	    end = lnum;
	    curs = lnum;
	    break;

	case '=':
	    start = lnum - bigness / 2 + 1;
	    end = lnum + bigness / 2 - 1;
	    curs = lnum;
	    minus = 1;
	    break;

	case '^':
	    start = lnum - bigness * 2;
	    end = lnum - bigness;
	    curs = lnum - bigness;
	    break;

	case '.':
	    start = lnum - bigness / 2;
	    end = lnum + bigness / 2;
	    curs = end;
	    break;

	default:  /* '+' */
	    start = lnum;
	    end = lnum + bigness;
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

	if (minus && i == lnum)
	{
	    msg_putchar('\n');

	    for (j = 1; j < Columns; j++)
		msg_putchar('-');
	}

	print_line(i, FALSE);

	if (minus && i == lnum)
	{
	    msg_putchar('\n');

	    for (j = 1; j < Columns; j++)
		msg_putchar('-');
	}
    }

    curwin->w_cursor.lnum = curs;
    ex_no_reprint = TRUE;
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
	EMSG(_("Shell commands not allowed in rvim"));
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
	EMSG(_(e_curdir));
	return TRUE;
    }
#ifdef HAVE_SANDBOX
    /*
     * In the sandbox more things are not allowed, including the things
     * disallowed in secure mode.
     */
    if (sandbox != 0)
    {
	EMSG(_(e_sandbox));
	return TRUE;
    }
#endif
    return FALSE;
}

static char_u	*old_sub = NULL;	/* previous substitute pattern */
static int	global_need_beginline;	/* call beginline() after ":g" */

/*
 * When ":global" is used to number of substitutions and changed lines is
 * accumulated until it's finished.
 */
static long	sub_nsubs;	/* total number of substitutions */
static linenr_t	sub_nlines;	/* total number of lines changed */

/*
 * Get a line for multi-line regexp matching in do_sub().
 * The first line may be a copy, in which case sub_firstline points to that
 * copy.
 */
static linenr_t sub_firstlnum;
static char_u *sub_firstline;

static char_u *sub_getline __ARGS((linenr_t lnum));

    static char_u *
sub_getline(lnum)
    linenr_t	lnum;
{
    if (lnum == 0 && sub_firstline != NULL)
	return sub_firstline;
    /* when looking behind for a match/no-match we can't go before line 1 */
    if (lnum + sub_firstlnum < 1)
	return NULL;
    return ml_get(lnum + sub_firstlnum);
}

/* do_sub()
 *
 * Perform a substitution from line eap->line1 to line eap->line2 using the
 * command pointed to by eap->arg which should be of the form:
 *
 * /pattern/substitution/{flags}
 *
 * The usual escapes are supported as described in the regexp docs.
 */
    void
do_sub(eap)
    exarg_t	*eap;
{
    linenr_t	lnum;
    long	i;
    regmmatch_t regmatch;
    static int	do_all = FALSE;		/* do multiple substitutions per line */
    static int	do_ask = FALSE;		/* ask for confirmation */
    static int	do_error = TRUE;	/* if false, ignore errors */
    static int	do_print = FALSE;	/* print last line with subs. */
    static int	do_ic = 0;		/* ignore case flag */
    char_u	*pat = NULL, *sub = NULL;	/* init for GCC */
    int		delimiter;
    int		sublen;
    int		got_quit = FALSE;
    int		got_match = FALSE;
    int		temp;
    int		which_pat;
    char_u	*cmd;
    int		save_reg_ic;
    int		save_State;
    linenr_t	first_line = 0;	/* first changed line */
    linenr_t	last_line= 0;	/* below last changed line AFTER the
					 * change */
    linenr_t	old_line_count = curbuf->b_ml.ml_line_count;
    linenr_t	line2;
    long	nmatch;		/* number of lines in match */

    cmd = eap->arg;
    if (!global_busy)
    {
	sub_nsubs = 0;
	sub_nlines = 0;
    }

#ifdef FEAT_FKMAP	/* reverse the flow of the Farsi characters */
    if (p_altkeymap && curwin->w_p_rl)
	lrF_sub(cmd);
#endif

    if (eap->cmdidx == CMD_tilde)
	which_pat = RE_LAST;	/* use last used regexp */
    else
	which_pat = RE_SUBST;	/* use last substitute regexp */

				/* new pattern and substitution */
    if (eap->cmd[0] == 's' && *cmd != NUL && !vim_iswhite(*cmd)
		    && vim_strchr((char_u *)"0123456789gcr|\"", *cmd) == NULL)
    {
				/* don't accept alphanumeric for separator */
	if (isalpha(*cmd))
	{
	    EMSG(_("Regular expressions can't be delimited by letters"));
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
		EMSG(_(e_backslash));
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
	    cmd = skip_regexp(cmd, delimiter, p_magic);
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
	    EMSG(_(e_nopresub));
	    return;
	}
	pat = NULL;		/* search_regcomp() will use previous pattern */
	sub = old_sub;
    }

    /*
     * Find trailing options.  When '&' is used, keep old options.
     */
    if (*cmd == '&')
	++cmd;
    else
    {
	if (!p_ed)
	{
	    if (p_gd)		/* default is global on */
		do_all = TRUE;
	    else
		do_all = FALSE;
	    do_ask = FALSE;
	}
	do_error = TRUE;
	do_print = FALSE;
	do_ic = 0;
    }
    while (*cmd)
    {
	/*
	 * Note that 'g' and 'c' are always inverted, also when p_ed is off.
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
	else if (*cmd == 'i')	    /* ignore case */
	    do_ic = 'i';
	else if (*cmd == 'I')	    /* don't ignore case */
	    do_ic = 'I';
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
	    EMSG(_(e_zerocount));
	    return;
	}
	eap->line1 = eap->line2;
	eap->line2 += i - 1;
    }

    /*
     * check for trailing command or garbage
     */
    cmd = skipwhite(cmd);
    if (*cmd && *cmd != '"')	    /* if not end-of-line or comment */
    {
	eap->nextcmd = check_nextcmd(cmd);
	if (eap->nextcmd == NULL)
	{
	    EMSG(_(e_trailing));
	    return;
	}
    }

    if (eap->skip)	    /* not executing commands, only parsing */
	return;

    if ((regmatch.regprog = search_regcomp(pat, RE_SUBST, which_pat,
							 SEARCH_HIS)) == NULL)
    {
	if (do_error)
	    EMSG(_(e_invcmd));
	return;
    }

    /* the 'i' or 'I' flag overrules 'ignorecase' and 'smartcase' */
    if (do_ic == 'i')
	reg_ic = TRUE;
    else if (do_ic == 'I')
	reg_ic = FALSE;

    sub_firstline = NULL;

    /*
     * ~ in the substitute pattern is replaced with the old pattern.
     * We do it here once to avoid it to be replaced over and over again.
     */
    sub = regtilde(sub, p_magic);

    /*
     * Check for a match on each line.
     */
    line2 = eap->line2;
    for (lnum = eap->line1; lnum <= line2 && !(got_int || got_quit); ++lnum)
    {
	sub_firstlnum = lnum;
	nmatch = vim_regexec_multi(&regmatch, sub_getline, (colnr_t)0,
					   curbuf->b_ml.ml_line_count - lnum);
	if (nmatch)
	{
	    colnr_t	copycol;
	    colnr_t	matchcol;
	    colnr_t	prev_matchcol = MAXCOL;
	    char_u	*new_end, *new_start = NULL;
	    unsigned	new_start_len = 0;
	    char_u	*p1;
	    int		did_sub = FALSE;
	    int		lastone;
	    unsigned	len, needed_len;
	    long	nmatch_tl = 0;	/* nr of lines matched below lnum */
	    int		do_again;	/* do it again after joining lines */

	    /*
	     * The new text is build up step by step, to avoid too much
	     * copying.  There are these pieces:
	     * sub_firstline	The old text, unmodifed.
	     * copycol		Column in the old text where we started
	     *			looking for a match; from here old text still
	     *			needs to be copied to the new text.
	     * matchcol		Column number of the old text where to look
	     *			for the next match.  It's just after the
	     *			previous match or one further.
	     * prev_matchcol	Column just after the previous match (if any).
	     *			Mostly equal to matchcol, except for the first
	     *			match and after skipping an empty match.
	     * regmatch.*pos	Where the pattern matched in the old text.
	     * new_start	The new text, all that has been produced so
	     *			far.
	     * new_end		The new text, where to append new text.
	     *
	     * lnum		The line number where we were looking for the
	     *			first match in the old line.
	     * sub_firstlnum	The line number in the buffer where to look
	     *			for a match.  Can be different from "lnum"
	     *			when the substitute string contains line
	     *			breaks.
	     *
	     * Special situations:
	     * - When the substitute string contains a line break, the part up
	     *   to the line break is inserted in the text, but the copy of
	     *   the original line is kept.  "sub_firstlnum" is adjusted for
	     *   the inserted lines.
	     * - When the matched pattern contains a line break, the old line
	     *   is taken from the line at the end of the pattern.  The lines
	     *   in the match are deleted later, "sub_firstlnum" is adjusted
	     *   accordingly.
	     *
	     * The new text is built up in new_start[].  It has some extra
	     * room to avoid using alloc()/free() too often.  new_start_len is
	     * the lenght of the allocated memory at new_start.
	     *
	     * Make a copy of the old line, so it won't be taken away when
	     * updating the screen or handling a multi-line match.  The "old_"
	     * pointers point into this copy.
	     */
	    sub_firstline = vim_strsave(ml_get(sub_firstlnum));
	    if (sub_firstline == NULL)
	    {
		vim_free(new_start);
		goto outofmem;
	    }
	    copycol = 0;
	    matchcol = 0;

	    /* At first match, remember current cursor position. */
	    if (!got_match)
	    {
		setpcmark();
		got_match = TRUE;
	    }

	    /*
	     * Loop until nothing more to replace in this line.
	     * 1. Handle match with empty string.
	     * 2. If do_ask is set, ask for confirmation.
	     * 3. substitute the string.
	     * 4. if do_all is set, find next match
	     * 5. break if there isn't another match in this line
	     */
	    for (;;)
	    {
		/* Save the line number of the last change for the final
		 * cursor position (just like Vi). */
		curwin->w_cursor.lnum = lnum;
		do_again = FALSE;

		/*
		 * 1. Match empty string does not count, except for first
		 * match.  This reproduces the strange vi behaviour.
		 * This also catches endless loops.
		 */
		if (matchcol == prev_matchcol
			&& regmatch.endpos[0].lnum == 0
			&& matchcol == regmatch.endpos[0].col)
		{
		    ++matchcol; /* search for a match at next column */
		    goto skip;
		}

		/* Normally we continue searching for a match just after the
		 * previous match. */
		matchcol = regmatch.endpos[0].col;
		prev_matchcol = matchcol;

		/*
		 * 2. If do_ask is set, ask for confirmation.
		 */
		if (do_ask)
		{
		    /* update_screen() may change reg_ic: save it */
		    save_reg_ic = reg_ic;

		    /* change State to CONFIRM, so that the mouse works
		     * properly */
		    save_State = State;
		    State = CONFIRM;
#ifdef FEAT_MOUSE
		    setmouse();		/* disable mouse in xterm */
#endif
		    curwin->w_cursor.col = regmatch.startpos[0].col;

		    /*
		     * Loop until 'y', 'n', 'q', CTRL-E or CTRL-Y typed.
		     */
		    while (do_ask)
		    {
			/* Invert the matched string.
			 * Remove the inversion afterwards. */
			temp = RedrawingDisabled;
			RedrawingDisabled = 0;

			search_match_lines = regmatch.endpos[0].lnum;
			search_match_endcol = regmatch.endpos[0].col;
			highlight_match = TRUE;

			update_topline();
			validate_cursor();
			update_screen(NOT_VALID);
			highlight_match = FALSE;
			redraw_later(NOT_VALID);

			if (msg_row == Rows - 1)
			    msg_didout = FALSE;		/* avoid a scroll-up */
			/* write message same highlighting as for wait_return */
			smsg_attr(hl_attr(HLF_R),
				(char_u *)_("replace with %s (y/n/a/q/^E/^Y)?"),
				sub);
			showruler(TRUE);
			RedrawingDisabled = temp;

#ifdef USE_ON_FLY_SCROLL
			dont_scroll = FALSE;	/* allow scrolling here */
#endif
			++no_mapping;		/* don't map this key */
			++allow_keys;		/* allow special keys */
			i = safe_vgetc();
			--allow_keys;
			--no_mapping;

			/* clear the question */
			msg_didout = FALSE;	/* don't scroll up */
			msg_col = 0;
			gotocmdline(TRUE);
			need_wait_return = FALSE; /* no hit-return prompt */
			if (i == 'q' || i == ESC || i == Ctrl_C
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
			else if (i == Ctrl_E)
			    scrollup_clamp();
			else if (i == Ctrl_Y)
			    scrolldown_clamp();
		    }
		    reg_ic = save_reg_ic;
		    State = save_State;
#ifdef FEAT_MOUSE
		    setmouse();
#endif

		    if (got_quit)
			break;
		}

		/* Move the cursor to the start of the line, to avoid that it
		 * is beyond the end of the line after the substitution. */
		curwin->w_cursor.col = 0;

		/*
		 * 3. substitute the string.
		 */
		/* get length of substitution part */
		sublen = vim_regsub_multi(&regmatch, sub_getline,
			curbuf->b_ml.ml_line_count - lnum,
				      sub, sub_firstline, FALSE, p_magic, TRUE);

		/* Need room for:
		 * - result so far in new_start (not for first sub in line)
		 * - original text up to match
		 * - length of substituted part
		 * - original text after match
		 */
		if (nmatch == 1)
		    p1 = sub_firstline;
		else
		{
		    p1 = ml_get(sub_firstlnum + nmatch - 1);
		    nmatch_tl += nmatch - 1;
		}
		i = regmatch.startpos[0].col - copycol;
		needed_len = i + (STRLEN(p1) - regmatch.endpos[0].col)
								 + sublen + 1;
		if (new_start == NULL)
		{
		    /*
		     * Get some space for a temporary buffer to do the
		     * substitution into (and some extra space to avoid
		     * too many calls to alloc()/free()).
		     */
		    new_start_len = needed_len + 50;
		    if ((new_start = alloc_check(new_start_len)) == NULL)
			goto outofmem;
		    *new_start = NUL;
		    new_end = new_start;
		}
		else
		{
		    /*
		     * Check if the temporary buffer is long enough to do the
		     * substitution into.  If not, make it larger (with a bit
		     * extra to avoid too many calls to alloc()/free()).
		     */
		    len = STRLEN(new_start);
		    needed_len += len;
		    if (needed_len > new_start_len)
		    {
			new_start_len = needed_len + 50;
			if ((p1 = alloc_check(new_start_len)) == NULL)
			{
			    vim_free(new_start);
			    goto outofmem;
			}
			mch_memmove(p1, new_start, (size_t)(len + 1));
			vim_free(new_start);
			new_start = p1;
		    }
		    new_end = new_start + len;
		}

		/*
		 * copy the text up to the part that matched
		 */
		mch_memmove(new_end, sub_firstline + copycol, (size_t)i);
		new_end += i;

		(void)vim_regsub_multi(&regmatch, sub_getline,
				 curbuf->b_ml.ml_line_count - lnum,
					   sub, new_end, TRUE, p_magic, TRUE);
		sub_nsubs++;
		did_sub = TRUE;

		/* For a multi-line match, make a copy of the last matched
		 * line and continue in that one. */
		if (nmatch > 1)
		{
		    sub_firstlnum += nmatch - 1;
		    vim_free(sub_firstline);
		    sub_firstline = vim_strsave(ml_get(sub_firstlnum));
		    /* When going beyond the last line, stop substituting. */
		    if (sub_firstlnum <= line2)
			do_again = TRUE;
		    else
			do_all = FALSE;
		}

		/* Remember next character to be copied. */
		copycol = regmatch.endpos[0].col;

		/*
		 * Now the trick is to replace CTRL-M chars with a real line
		 * break.  This would make it impossible to insert a CTRL-M in
		 * the text.  The line break can be avoided by preceding the
		 * CTRL-M with a backslash.  To be able to insert a backslash,
		 * they must be doubled in the string and are halved here.
		 * That is Vi compatible.
		 */
		for (p1 = new_end; *p1; ++p1)
		{
		    if (p1[0] == '\\' && p1[1] != NUL)
			STRCPY(p1, p1 + 1);	    /* remove backslash */
		    else if (*p1 == CR)
		    {
			if (u_inssub(lnum) == OK)   /* prepare for undo */
			{
			    *p1 = NUL;		    /* truncate up to the CR */
			    ml_append(lnum - 1, new_start,
					(colnr_t)(p1 - new_start + 1), FALSE);
			    mark_adjust(lnum, (linenr_t)MAXLNUM, 1L, 0L);
			    if (do_ask)
				appended_lines(lnum - 1, 1L);
			    else
			    {
				if (first_line == 0)
				    first_line = lnum;
				last_line = lnum + 1;
			    }
			    /* All line numbers increase. */
			    ++sub_firstlnum;
			    ++lnum;
			    ++line2;
			    /* move the cursor to the new line, like Vi */
			    ++curwin->w_cursor.lnum;
			    STRCPY(new_start, p1 + 1);	/* copy the rest */
			    p1 = new_start;
			}
		    }
		}

		/*
		 * 4. If do_all is set, find next match.
		 * Prevent endless loop with patterns that match empty
		 * strings, e.g. :s/$/pat/g or :s/[a-z]* /(&)/g.
		 */
skip:
		nmatch = -1;
		lastone = (sub_firstline[matchcol] == NUL
			     || got_int || got_quit || !(do_all || do_again));
		if (lastone
			|| do_ask
			|| (nmatch = vim_regexec_multi(&regmatch, sub_getline,
				matchcol,
				  curbuf->b_ml.ml_line_count - sub_firstlnum))
			    == 0)
		{
		    if (new_start != NULL)
		    {
			/*
			 * Copy the rest of the line, that didn't match.
			 * matchcol has to be adjusted, we use the end of the
			 * line as reference, because the substitute may have
			 * changed the number of characters.
			 */
			STRCAT(new_start, sub_firstline + copycol);
			matchcol = STRLEN(sub_firstline) - matchcol;

			if (u_savesub(lnum) != OK)
			    break;
			ml_replace(lnum, new_start, TRUE);

			if (nmatch_tl > 0)
			{
			    /*
			     * Matched lines have now been substituted and are
			     * useless, delete them.  The part after the match
			     * has been appended to new_start, we don't need
			     * it in the buffer.
			     */
			    ++lnum;
			    if (u_savedel(lnum, nmatch_tl) != OK)
				break;
			    for (i = 0; i < nmatch_tl; ++i)
				ml_delete(lnum, (int)FALSE);
			    mark_adjust(lnum, lnum + nmatch_tl - 1,
						   (long)MAXLNUM, -nmatch_tl);
			    if (do_ask)
				deleted_lines(lnum, nmatch_tl);
			    --lnum;
			    line2 -= nmatch_tl; /* nr of lines decreases */
			}

			/* When asking, undo is saved each time, must also set
			 * changed flag each time. */
			if (do_ask)
			    changed_bytes(lnum, 0);
			else
			{
			    if (first_line == 0)
				first_line = lnum;
			    last_line = lnum + 1;
			}

			sub_firstlnum = lnum;
			vim_free(sub_firstline);    /* free the temp buffer */
			sub_firstline = new_start;
			new_start = NULL;
			matchcol = STRLEN(sub_firstline) - matchcol;
			copycol = 0;
		    }
		    if (nmatch == -1 && !lastone)
			nmatch = vim_regexec_multi(&regmatch, sub_getline,
				matchcol,
				  curbuf->b_ml.ml_line_count - sub_firstlnum);

		    /*
		     * 5. break if there isn't another match in this line
		     */
		    if (nmatch <= 0)
			break;
		}

		line_breakcheck();
	    }

	    if (did_sub)
		++sub_nlines;
	    vim_free(sub_firstline);	/* free the copy of the original line */
	    sub_firstline = NULL;
	}

	line_breakcheck();
    }
    curbuf->b_op_start.lnum = eap->line1;
    curbuf->b_op_end.lnum = line2;
    curbuf->b_op_start.col = curbuf->b_op_end.col = 0;

    if (first_line != 0)
    {
	/* Need to subtract the number of added lines from "last_line" to get
	 * the line number before the change (same as adding the number of
	 * deleted lines). */
	i = curbuf->b_ml.ml_line_count - old_line_count;
	changed_lines(first_line, 0, last_line - i, i);
    }

outofmem:
    vim_free(sub_firstline); /* may have to free allocated copy of the line */
    if (sub_nsubs)
    {
	if (!global_busy)
	{
	    beginline(BL_WHITE | BL_FIX);
	    if (!do_sub_msg() && do_ask)
		MSG("");
	}
	else
	    global_need_beginline = TRUE;
	if (do_print)
	    print_line(curwin->w_cursor.lnum, FALSE);
    }
    else if (!global_busy)
    {
	if (got_int)		/* interrupted */
	    EMSG(_(e_interr));
	else if (got_match)	/* did find something but nothing substituted */
	    MSG("");
	else if (do_error)	/* nothing found */
	    EMSG2(_(e_patnotf2), get_search_pat());
    }

    vim_free(regmatch.regprog);
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
    if (sub_nsubs > p_report
	    && (KeyTyped || sub_nlines > 1 || p_report < 1)
	    && messaging())
    {
	if (got_int)
	    STRCPY(msg_buf, _("(Interrupted) "));
	else
	    msg_buf[0] = NUL;
	if (sub_nsubs == 1)
	    STRCAT(msg_buf, _("1 substitution"));
	else
	    sprintf((char *)msg_buf + STRLEN(msg_buf), _("%ld substitutions"),
								   sub_nsubs);
	if (sub_nlines == 1)
	    STRCAT(msg_buf, _(" on 1 line"));
	else
	    sprintf((char *)msg_buf + STRLEN(msg_buf), _(" on %ld lines"),
							    (long)sub_nlines);
	if (msg(msg_buf))
	{
	    keep_msg = msg_buf;
	    keep_msg_attr = 0;
	}
	return TRUE;
    }
    if (got_int)
    {
	EMSG(_(e_interr));
	return TRUE;
    }
    return FALSE;
}

/*
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
ex_global(eap)
    exarg_t	*eap;
{
    linenr_t	lnum;		/* line number according to old situation */
    linenr_t	old_lcount;	/* b_ml.ml_line_count before the command */
    int		ndone;
    int		type;		/* first char of cmd: 'v' or 'g' */
    char_u	*cmd;		/* command argument */

    char_u	delim;		/* delimiter, normally '/' */
    char_u	*pat;
    regmmatch_t	regmatch;
    int		match;
    int		which_pat;

    if (global_busy)
    {
	EMSG(_("Cannot do :global recursive"));	/* will increment global_busy */
	return;
    }

    if (eap->forceit)		    /* ":global!" is like ":vglobal" */
	type = 'v';
    else
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
	    EMSG(_(e_backslash));
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
	EMSG(_("Regular expression missing from global"));
	return;
    }
    else
    {
	delim = *cmd;		/* get the delimiter */
	if (delim)
	    ++cmd;		/* skip delimiter if there is one */
	pat = cmd;		/* remember start of pattern */
	cmd = skip_regexp(cmd, delim, p_magic);
	if (cmd[0] == delim)		    /* end delimiter found */
	    *cmd++ = NUL;		    /* replace it with a NUL */
    }

#ifdef FEAT_FKMAP	/* when in Farsi mode, reverse the character flow */
    if (p_altkeymap && curwin->w_p_rl)
	lrFswap(pat,0);
#endif

    if ((regmatch.regprog = search_regcomp(pat, RE_BOTH, which_pat,
							 SEARCH_HIS)) == NULL)
    {
	EMSG(_(e_invcmd));
	return;
    }

/*
 * pass 1: set marks for each (not) matching line
 */
    ndone = 0;
    for (lnum = eap->line1; lnum <= eap->line2 && !got_int; ++lnum)
    {
	/* a match on this line? */
	sub_firstlnum = lnum;
	sub_firstline = NULL;
	match = vim_regexec_multi(&regmatch, sub_getline, (colnr_t)0,
					   curbuf->b_ml.ml_line_count - lnum);
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
	MSG(_(e_interr));
    else if (ndone == 0)
	smsg((char_u *)_(e_patnotf2), pat);
    else
    {
	/*
	 * Set current position only once for a global command.
	 * If global_busy is set, setpcmark() will not do anything.
	 * If there is an error, global_busy will be incremented.
	 */
	setpcmark();

	/* When the command writes a message, don't overwrite the command. */
	msg_didout = TRUE;

	global_need_beginline = FALSE;
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
	if (global_need_beginline)
	    beginline(BL_WHITE | BL_FIX);
	else
	    adjust_cursor();	/* cursor may be beyond the end of the line */

	/* If it looks like no message was written, allow overwriting the
	 * command with the report for number of changes. */
	if (msg_col == 0 && msg_scrolled == 0)
	    msg_didout = FALSE;

	/* If subsitutes done, report number of substitues, otherwise report
	 * number of extra or deleted lines. */
	if (!do_sub_msg())
	    msgmore(curbuf->b_ml.ml_line_count - old_lcount);
    }

    ml_clearmarked();	   /* clear rest of the marks */
    vim_free(regmatch.regprog);
}

#ifdef FEAT_VIMINFO
    int
read_viminfo_sub_string(line, fp, force)
    char_u  *line;
    FILE    *fp;
    int	    force;
{
    if (old_sub != NULL && force)
	vim_free(old_sub);
    if (force || old_sub == NULL)
	old_sub = viminfo_readstring(line + 1, fp);
    return vim_fgets(line, LSIZE, fp);
}

    void
write_viminfo_sub_string(fp)
    FILE    *fp;
{
    if (get_viminfo_parameter('/') != 0 && old_sub != NULL)
    {
	fprintf(fp, _("\n# Last Substitute String:\n$"));
	viminfo_writestring(fp, old_sub);
    }
}
#endif /* FEAT_VIMINFO */

#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * Set up for a tagpreview.
 */
    void
prepare_tagpreview()
{
    win_t	*wp;

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * If there is already a preview window open, use that one.
     */
    if (!curwin->w_preview)
    {
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
	    if (wp->w_preview)
		break;
	if (wp != NULL)
	    win_enter(wp, TRUE);
	else
	{
	    /*
	     * There is no preview window open yet.  Create one.
	     */
	    if (win_split(g_do_tagpreview > 0 ? g_do_tagpreview : 0, 0)
								      == FAIL)
		return;
	    curwin->w_preview = TRUE;
	}
    }

#if 0	/* don't think we need this, cursor goes back to other window */
    if (!p_im)
	restart_edit = 0;	/* don't want insert mode in preview window */
#endif
}

#endif /* FEAT_WINDOWS */


/*
 * ":help": open a read-only window on the help.txt file
 */
    void
ex_help(eap)
    exarg_t	*eap;
{
    char_u	*arg;
    FILE	*helpfd;	/* file descriptor of help file */
    int		n;
#ifdef FEAT_WINDOWS
    win_t	*wp;
#endif
    int		num_matches;
    char_u	**matches;
    int		need_free = FALSE;
    char_u	*p;

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
	    EMSG2(_("Sorry, no help for %s"), arg);
	    return;
	}

	/* The first match is the best match. */
	arg = vim_strsave(matches[0]);
	need_free = TRUE;
	FreeWild(num_matches, matches);
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * Re-use an existing help window or open a new one.
     */
    if (!curwin->w_buffer->b_help)
    {
#ifdef FEAT_WINDOWS
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
	    if (wp->w_buffer != NULL && wp->w_buffer->b_help)
		break;
	if (wp != NULL && wp->w_buffer->b_nwindows > 0)
	    win_enter(wp, TRUE);
	else
#endif
	{
	    /*
	     * There is no help buffer yet.
	     * Try to open the file specified by the "helpfile" option.
	     */
	    if ((helpfd = mch_fopen((char *)p_hf, READBIN)) == NULL)
	    {
		smsg((char_u *)_("Sorry, help file \"%s\" not found"), p_hf);
		goto erret;
	    }
	    fclose(helpfd);

#ifdef FEAT_WINDOWS
	    /* split off help window; put it at far top if not specified */
	    if (win_split(0, cmdmod.split == 0 ? WSP_TOP : 0) == FAIL)
#else
	    /* use current window */
	    if (!can_abandon(curbuf, FALSE))
#endif
		goto erret;

#ifdef FEAT_WINDOWS
	    if (curwin->w_height < p_hh)
		win_setheight((int)p_hh);
#endif

#ifdef FEAT_RIGHTLEFT
	    curwin->w_p_rl = 0;		    /* help window is left-to-right */
#endif
	    curwin->w_p_nu = 0;		    /* no line numbers */

#ifdef FEAT_FOLDING
	    /* Disable folding in the help window */
	    curwin->w_p_fdl = 0;
	    curwin->w_p_fen = FALSE;
#endif

	    /*
	     * open help file (do_ecmd() will set b_help flag, readfile() will
	     * set b_p_ro flag)
	     */
	    (void)do_ecmd(0, p_hf, NULL, NULL, ECMD_LASTL,
						   ECMD_HIDE + ECMD_SET_HELP);
	}
    }

    if (!p_im)
	restart_edit = 0;	    /* don't want insert mode in help file */

    if (arg == NULL || *arg == NUL)
    {
	arg = (char_u *)"help.txt";	    /* go to the index */
	need_free = FALSE;
    }
    do_tag(arg, DT_HELP, 1, FALSE, TRUE);

    /*
     * Always set these options after jumping to a help tag, because the user
     * may have an autocommand that gets in the way.
     * accept all chars for keywords, except ' ', '*', '"', '|'.  Only set it
     * when needed, buf_init_chartab() is some work.
     */
    p =
#ifdef EBCDIC
	    (char_u *)"65-255,^*,^|,^\"";
#else
	    (char_u *)"!-~,^*,^|,^\"";
#endif
    if (STRCMP(curbuf->b_p_isk, p) != 0)
    {
	set_string_option_direct((char_u *)"isk", -1, p, OPT_FREE);
	check_buf_options(curbuf);
	(void)buf_init_chartab(curbuf, FALSE);
    }

    curbuf->b_p_ts = 8;
    curwin->w_p_list = FALSE;

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
 *	- Match starting with "+" is worse (feature instead of command)
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
     * If there only is a match while ignoring case, add 5000.
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
    /* Features are less interesting than the subjects themselves, but "+"
     * alone is not a feature. */
    if (matched_string[0] == '+' && matched_string[1] != NUL)
	offset += 100;
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
 * The matches will be sorted with a "best" match algorithm.
 */
    int
find_help_tags(arg, num_matches, matches)
    char_u	*arg;
    int		*num_matches;
    char_u	***matches;
{
    char_u	*s, *d;
    int		i;
    static char *(mtable[]) = {"*", "g*", "[*", "]*", ":*",
			       "/*", "/\\*", "\"*", "/\\(\\)",
			       "?", ":?", "?<CR>", "g?", "g?g?", "g??",
			       "[count]", "[quotex]", "[range]",
			       "[pattern]", "\\|"};
    static char *(rtable[]) = {"star", "gstar", "[star", "]star", ":star",
			       "/star", "/\\\\star", "quotestar", "/\\\\(\\\\)",
			       "?", ":?", "?<CR>", "g?", "g?g?", "g??",
			       "\\[count]", "\\[quotex]", "\\[range]",
			       "\\[pattern]", "\\\\bar"};

    d = IObuff;		    /* assume IObuff is long enough! */

    /*
     * Recognize a few exceptions to the rule.	Some strings that contain '*'
     * with "star".  Otherwise '*' is recognized as a wildcard.
     */
    for (i = sizeof(mtable) / sizeof(char *); --i >= 0; )
	if (STRCMP(arg, mtable[i]) == 0)
	{
	    STRCPY(d, rtable[i]);
	    break;
	}

    if (i < 0)	/* no match in table */
    {
	/* Replace "\S" with "/\\S", etc.  Otherwise every tag is matched. */
	if (arg[0] == '\\' && arg[1] != NUL && arg[2] == NUL)
	{
	    STRCPY(d, "/\\\\x");
	    d[3] = arg[1];
	}
	else
	{
	  /* replace "[:...:]" with "\[:...:]" */
	  if (arg[0] == '[' && arg[1] == ':')
	      *d++ = '\\';

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
		case '"':   STRCPY(d, "quote");
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
#ifdef EBCDIC
		    *d++ = CtrlChar(*s);
#else
		    *d++ = *s + '@';
#endif
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
	    else if (s[0] == '\\' && s[1] != '\\'
					       && *arg == '/' && s == arg + 1)
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
    }

    *matches = (char_u **)"";
    *num_matches = 0;
    if (find_tags(IObuff, num_matches, matches,
	TAG_HELP | TAG_REGEXP | TAG_NAMES | TAG_VERBOSE, (int)MAXCOL) == OK)
	/*
	 * Sort the matches found on the heuristic number that is after the
	 * tag name.
	 */
	qsort((void *)*matches, (size_t)*num_matches,
					      sizeof(char_u *), help_compare)
	;
    return OK;
}

/*
 * After reading a help file: May cleanup a help buffer when syntax
 * highlighting is not used.
 */
    void
fix_help_buffer()
{
    linenr_t	lnum;
    char_u	*line;
    int		in_example = FALSE;
    int		len;
    char_u	*p;
    char_u	*rt;
    int		mustfree;

    /* set filetype to "help". */
    set_option_value((char_u *)"ft", 0L, (char_u *)"help", TRUE);

#ifdef FEAT_SYN_HL
    if (!syntax_present(curbuf))
#endif
    {
	for (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
	{
	    line = ml_get_buf(curbuf, lnum, FALSE);
	    len = STRLEN(line);
	    if (in_example && len > 0 && !vim_iswhite(line[0]))
	    {
		/* End of example: non-white or '<' in first column. */
		if (line[0] == '<')
		{
		    /* blank-out a '<' in the first column */
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[0] = ' ';
		}
		in_example = FALSE;
	    }
	    if (!in_example && len > 0)
	    {
		if (line[len - 1] == '>' && (len == 1 || line[len - 2] == ' '))
		{
		    /* blank-out a '>' in the last column (start of example) */
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[len - 1] = ' ';
		    in_example = TRUE;
		}
		else if (line[len - 1] == '~')
		{
		    /* blank-out a '~' at the end of line (header marker) */
		    line = ml_get_buf(curbuf, lnum, TRUE);
		    line[len - 1] = ' ';
		}
	    }
	}
    }

    /*
     * In the "help.txt" file, add the locally added help files.
     * This uses the very first line in the help file.
     */
    if (fnamecmp(gettail(curbuf->b_fname), "help.txt") == 0)
    {
	for (lnum = 1; lnum < curbuf->b_ml.ml_line_count; ++lnum)
	{
	    line = ml_get_buf(curbuf, lnum, FALSE);
	    if (STRNCMP(line, "LOCAL ADDITIONS", 15) == 0)
	    {
		/* Go through all directories in 'runtimepath', skipping
		 * $VIMRUNTIME. */
		p = p_rtp;
		while (*p != NUL)
		{
		    copy_option_part(&p, NameBuff, MAXPATHL, ",");
		    mustfree = FALSE;
		    rt = vim_getenv((char_u *)"VIMRUNTIME", &mustfree);
		    if (fnamecmp(NameBuff, rt) != 0)
		    {
			int	fcount;
			char_u	**fnames;
			FILE	*fd;
			char_u	*s;
			int	fi;

			/* Find all "doc/ *.txt" files in this directory. */
			add_pathsep(NameBuff);
			STRCAT(NameBuff, "doc/*.txt");
			if (gen_expand_wildcards(1, &NameBuff, &fcount,
					     &fnames, EW_FILE|EW_SILENT) == OK
				&& fcount > 0)
			{
			    for (fi = 0; fi < fcount; ++fi)
			    {
				fd = fopen((char *)fnames[fi], "r");
				if (fd != NULL)
				{
				    vim_fgets(IObuff, IOSIZE, fd);
				    if (IObuff[0] == '*'
					    && (s = vim_strchr(IObuff + 1, '*'))
								      != NULL)
				    {
					/* Change tag definition to a
					 * reference and remove <CR>/<NL>. */
					IObuff[0] = '|';
					*s = '|';
					while (*s != NUL)
					{
					    if (*s == '\r' || *s == '\n')
						*s = NUL;
					    ++s;
					}
					ml_append(lnum, IObuff, (colnr_t)0,
								       FALSE);
					++lnum;
				    }
				    fclose(fd);
				}
			    }
			    FreeWild(fcount, fnames);
			}
		    }
		    if (mustfree)
			vim_free(rt);
		}
		break;
	    }
	}
    }
}

#if defined(FEAT_EX_EXTRA) || defined(PROTO)
/*
 * ":helptags"
 */
    void
ex_helptags(eap)
    exarg_t	*eap;
{
    FILE	*fd_tags;
    FILE	*fd;
    garray_t	ga;
    int		filecount;
    char_u	**files;
    char_u	*p1, *p2;
    int		fi;
    char_u	*s;
    int		i;
    char_u	*fname;

    if (!mch_isdir(eap->arg))
    {
	EMSG2(_("Not a directory: %s"), eap->arg);
	return;
    }

    /*
     * Find all *.txt files.
     */
    STRCPY(NameBuff, eap->arg);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "*.txt");
    if (gen_expand_wildcards(1, &NameBuff, &filecount, &files,
						    EW_FILE|EW_SILENT) == FAIL
	    || filecount == 0)
    {
	EMSG2("No match: %s", NameBuff);
	return;
    }

    /*
     * Open the tags file for writing.
     * Do this before scanning through all the files.
     */
    STRCPY(NameBuff, eap->arg);
    add_pathsep(NameBuff);
    STRCAT(NameBuff, "tags");
    fd_tags = fopen((char *)NameBuff, "w");
    if (fd_tags == NULL)
    {
	EMSG2(_("Cannot open %s for writing"), NameBuff);
	FreeWild(filecount, files);
	return;
    }

    /*
     * Go over all the files and extract the tags.
     */
    ga_init2(&ga, sizeof(char_u *), 100);
    for (fi = 0; fi < filecount && !got_int; ++fi)
    {
	fd = fopen((char *)files[fi], "r");
	if (fd == NULL)
	{
	    EMSG2(_("Unable to open %s for reading"), files[fi]);
	    continue;
	}
	fname = gettail(files[fi]);

	while (!vim_fgets(IObuff, IOSIZE, fd) && !got_int)
	{
	    p1 = vim_strchr(IObuff, '*');	/* find first '*' */
	    while (p1 != NULL)
	    {
		p2 = vim_strchr(p1 + 1, '*');	/* find second '*' */
		if (p2 != NULL && p2 > p1 + 1)	/* skip "*" and "**" */
		{
		    for (s = p1 + 1; s < p2; ++s)
			if (*s == ' ' || *s == '\t' || *s == '|')
			    break;

		    /*
		     * Only accept a *tag* when it consists of valid
		     * characters, there is no '-' before it and is followed
		     * by a white character or end-of-line.
		     */
		    if (s == p2
			    && (p1 == IObuff || p1[-1] != '-')
			    && (vim_strchr((char_u *)" \t\n\r", s[1]) != NULL
				|| s[1] == '\0'))
		    {
			*p2 = '\0';
			++p1;
			if (ga_grow(&ga, 1) == FAIL)
			{
			    got_int = TRUE;
			    break;
			}
			s = alloc((unsigned)(p2 - p1 + STRLEN(fname) + 2));
			if (s == NULL)
			{
			    got_int = TRUE;
			    break;
			}
			((char_u **)ga.ga_data)[ga.ga_len] = s;
			++ga.ga_len;
			--ga.ga_room;
			sprintf((char *)s, "%s\t%s", p1, fname);

			/* find next '*' */
			p2 = vim_strchr(p2 + 1, '*');
		    }
		}
		p1 = p2;
	    }
	    line_breakcheck();
	}

	fclose(fd);
    }

    FreeWild(filecount, files);

    if (!got_int)
    {
	/*
	 * Sort the tags.
	 */
	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	/*
	 * Check for duplicates.
	 */
	for (i = 1; i < ga.ga_len; ++i)
	{
	    p1 = ((char_u **)ga.ga_data)[i - 1];
	    p2 = ((char_u **)ga.ga_data)[i];
	    while (*p1 == *p2)
	    {
		if (*p2 == '\t')
		{
		    *p2 = NUL;
		    sprintf((char *)NameBuff,
			    _("Duplicate tag \"%s\" in file %s"),
			    ((char_u **)ga.ga_data)[i], p2 + 1);
		    EMSG(NameBuff);
		    *p2 = '\t';
		    break;
		}
		++p1;
		++p2;
	    }
	}

	/*
	 * Write the tags into the file.
	 */
	for (i = 0; i < ga.ga_len; ++i)
	{
	    s = ((char_u **)ga.ga_data)[i];
	    fprintf(fd_tags, "%s\t/*", s);
	    for (p1 = s; *p1 != '\t'; ++p1)
	    {
		/* insert backslash before '\\' and '/' */
		if (*p1 == '\\' || *p1 == '/')
		    putc('\\', fd_tags);
		putc(*p1, fd_tags);
	    }
	    fprintf(fd_tags, "*\n");
	}
    }

    for (i = 0; i < ga.ga_len; ++i)
	vim_free(((char_u **)ga.ga_data)[i]);
    ga_clear(&ga);
    fclose(fd_tags);	    /* there is no check for an error... */
}
#endif

#if defined(FEAT_SIGNS) || defined(PROTO)

static void show_signs __ARGS((void));

/*
 * The following formats of the :sign command are supported:
 *
 *	:sign id lnum idx file
 *	:sign id idx file
 *	:sign id file
 *
 * Where:
 *	id is a sign identifier
 *	lnum is the (optional) line number
 *	idx is the sign index passed in the sign= part of the highlight cmd
 *	file is the file the sign refers to
 *
 * The first case sets a sign and the second case moves the cursor to a sign.
 */
    void
ex_sign(eap)
    exarg_t	*eap;
{
    char_u	*arg;			/* parse command line */
    char_u	*arg1;			/* the 1st argument */
    char_u	*arg2;			/* the second argument */
    char_u	*arg3;			/* the third argument */
    int		markId;			/* unique mark identifier */
    int		lnum = 0;		/* line number mark displayed on */
    int		idx;			/* which mark to use */
    char_u	*filename;		/* filename which gets the mark */
    buf_t	*buf;			/* buffer to set mark in */
    char_u	cmd[MAXPATHL];		/* build :edit command here */
    win_t	*win;			/* used for warping to a sign */

    if (eap->cmdidx == CMD_signs)
    {
	show_signs();
	return;
    }

    filename = NULL;
    arg = eap->arg;
    idx = -1;
    if (vim_iswhite(*arg))
	arg = skipwhite(arg);

    /* First argument must be a digit (the mark id) */
    arg1 = arg;
    arg = skiptowhite(arg);
    if (arg == skipdigits(arg1))
    {
	markId = atoi((char *)arg1);
	arg = skipwhite(arg);
    }
    else
    {
	EMSG(_("Missing sign id"));
	return;
    }

    /* Second argument may be a line number, sign index, or filename */
    arg2 = arg;
    arg = skiptowhite(arg);
    if (arg == skipdigits(arg2))
	arg = skipwhite(arg);		/* arg2 is a number */
    else
	filename = arg2;

    if (filename == NULL)
    {
        arg3 = arg;
	arg = skiptowhite(arg);
	if (arg == skipdigits(arg3))
	{				/* arg2 and arg3 are both numbers */
	    arg = skipwhite(arg);
	    lnum = atoi((char *)arg2);
	    idx = atoi((char *)arg3);
	    filename = arg;
	}
	else
	{
	    idx = atoi((char *)arg2);
	    filename = arg3;
	}
    }

    /* Verify filename is a string */
    if (*filename == NUL)
    {
	EMSG(_("Missing filename"));
	return;
    }

    /* Does filename get us a valid buffer? */
    buf = buflist_findname((char_u *)filename);
    if (buf != NULL)
    {
	if (idx > 0 && lnum > 0)	/* create a new sign */
	{
	    buf_addsign(buf, markId, lnum, idx);
	    update_debug_sign(buf, lnum);
	}
	else if (idx > 0 && lnum == 0)	/* change the sign type */
	{
	    lnum = buf_change_sign_type(buf, markId, idx);
	    update_debug_sign(buf, lnum);
	}
	else if ((lnum = buf_findsign(buf, markId)) > 0)
	{				/* goto a sign ... */
	    if ((win = buf_jump_open_win(buf)) != NULL)
	    {				/* ... in a current window */
		sprintf((char *)cmd, "%dG", lnum);
		add_to_input_buf(cmd, strlen((char *) cmd));
	    }
	    else
	    {				/* ... not currently in a window */
		sprintf((char *)cmd, "e +%d %s", lnum, buf->b_fname);
		do_cmdline(cmd, NULL, NULL, DOCMD_NOWAIT);
	    }
	}
	else
	    EMSG2(_("Invalid line number: %d"), lnum);
    }
    else
	EMSG2(_("Invalid buffer name: %s"), filename);
}

    void
ex_unsign(eap)
    exarg_t	*eap;
{
    char_u	*arg;			/* argument pointer */
    char_u	*filename;		/* filename which gets the mark */
    int		markId;			/* unique mark identifier */
    int		lnum;			/* line number mark displayed on */
    buf_t	*buf;			/* buffer of mark we want to delete */

    arg = eap->arg;
    if (vim_iswhite(*arg))
	arg = skipwhite(arg);

    if (*arg == '*')
    {
	buf_delete_all_signs();
	update_debug_sign(NULL, 0);
    }
    else
    {
	markId = atoi((char *) arg);
	if (markId > 0)
	    arg = skipdigits(arg);
	else
	{
	    markId = buf_findsign_id(curwin->w_buffer, curwin->w_cursor.lnum);
	    if (markId > 0)
	    {
		buf_delsign(curwin->w_buffer, markId);
		update_debug_sign(curwin->w_buffer, curwin->w_cursor.lnum);
	    }
	    else
		EMSG(_("Missing sign ID"));
	    return;
	}
	arg = skipwhite(arg);

	filename = arg;
	if (*filename == NUL)
	{
	    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		if ((lnum = buf_delsign(buf, markId)) != 0)
		    update_debug_sign(buf, lnum);
	}
	else
	{
	    buf = buflist_findname((char_u *) filename);
	    if (buf != NULL)
	    {
		    lnum = buf_delsign(buf, markId);
		    update_debug_sign(buf, lnum);
	    }
	    else
		EMSG2(_("Cannot find buffer: %s"), filename);
	}
    }
}

    static void
print_sign(glist)
    signlist_t	*glist;
{
    char	lbuf[BUFSIZ];

    sprintf(lbuf, _("    line %d, id %d, type %d"),
        glist->lineno, glist->id, glist->type);
    MSG_PUTS_ATTR(lbuf, 0);
    msg_putchar('\n');
}


    static void
show_signs()
{
    buf_t	*buf;
    signlist_t	*glist;
    char	lbuf[BUFSIZ];

    MSG_PUTS_TITLE(_("\n--- Signs ---"));
    msg_putchar('\n');
    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
    {
	if (buf->b_signlist != NULL)
	{
	    sprintf(lbuf, _("Signs for %s:"), buf->b_fname);
	    MSG_PUTS_ATTR(lbuf, hl_attr(HLF_D));
	    msg_putchar('\n');
	}
	for (glist = buf->b_signlist; glist != NULL; glist = glist->next)
	    print_sign(glist);
    }
}

#endif
