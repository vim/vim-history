/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * misc1.c: functions that didn't seem to fit elsewhere
 */

#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>		/* for chdir() */
#endif

static int get_indent_str __ARGS((char_u *ptr));

/*
 * count the size of the indent in the current line
 */
    int
get_indent()
{
    return get_indent_str(ml_get_curline());
}

/*
 * count the size of the indent in line "lnum"
 */
    int
get_indent_lnum(lnum)
    linenr_t	lnum;
{
    return get_indent_str(ml_get(lnum));
}

/*
 * count the size of the indent in line "ptr"
 */
    static int
get_indent_str(ptr)
    char_u  *ptr;
{
    int	    count = 0;

    for ( ; *ptr; ++ptr)
    {
	if (*ptr == TAB)    /* count a tab for what it is worth */
	    count += (int)curbuf->b_p_ts - (count % (int)curbuf->b_p_ts);
	else if (*ptr == ' ')
	    ++count;		/* count a space for one */
	else
	    break;
    }
    return (count);
}

/*
 * set the indent of the current line
 * leaves the cursor on the first non-blank in the line
 */
    void
set_indent(size, del_first)
    int		size;
    int		del_first;
{
    int		oldstate = State;
    int		c;
#ifdef RIGHTLEFT
    int		old_p_ri = p_ri;

    p_ri = 0;			    /* don't want revins in ident */
#endif

    State = INSERT;		    /* don't want REPLACE for State */
    curwin->w_cursor.col = 0;
    if (del_first)		    /* delete old indent */
    {
				    /* vim_iswhite() is a define! */
	while ((c = gchar_cursor()), vim_iswhite(c))
	    (void)del_char(FALSE);
    }
    if (!curbuf->b_p_et)	    /* if 'expandtab' is set, don't use TABs */
	while (size >= (int)curbuf->b_p_ts)
	{
	    ins_char(TAB);
	    size -= (int)curbuf->b_p_ts;
	}
    while (size)
    {
	ins_char(' ');
	--size;
    }
    State = oldstate;
#ifdef RIGHTLEFT
    p_ri = old_p_ri;
#endif
}

#if defined(CINDENT) || defined(SMARTINDENT)

static int cin_is_cinword __ARGS((char_u *line));

/*
 * Return TRUE if the string "line" starts with a word from 'cinwords'.
 */
    static int
cin_is_cinword(line)
    char_u	*line;
{
    char_u  *cinw;
    char_u  *cinw_buf;
    int	    cinw_len;
    int	    retval = FALSE;
    int	    len;

    cinw_len = STRLEN(curbuf->b_p_cinw) + 1;
    cinw_buf = alloc((unsigned)cinw_len);
    if (cinw_buf != NULL)
    {
	line = skipwhite(line);
	for (cinw = curbuf->b_p_cinw; *cinw; )
	{
	    len = copy_option_part(&cinw, cinw_buf, cinw_len, ",");
	    if (STRNCMP(line, cinw_buf, len) == 0 &&
		     (!vim_iswordc(line[len]) || !vim_iswordc(line[len - 1])))
	    {
		retval = TRUE;
		break;
	    }
	}
	vim_free(cinw_buf);
    }
    return retval;
}
#endif

/*
 * open_line: Add a new line below or above the current line.
 *
 * Caller must take care of undo.
 *
 * Return TRUE for success, FALSE for failure
 */
    int
open_line(dir, redraw, del_spaces, old_indent)
    int		dir;		/* FORWARD or BACKWARD */
    int		redraw;		/* > 0: redraw afterwards, < 0: insert lines*/
    int		del_spaces;	/* delete spaces after cursor */
    int		old_indent;	/* indent for after ^^D in Insert mode */
{
    char_u  *saved_line;	/* copy of the original line */
    char_u  *p_extra = NULL;	/* what goes to next line */
    int	    extra_len = 0;	/* length of p_extra string */
    FPOS    old_cursor;		/* old cursor position */
    int	    newcol = 0;		/* new cursor column */
    int	    newindent = 0;	/* auto-indent of the new line */
    int	    n;
    int	    trunc_line = FALSE;	/* truncate current line afterwards */
    int	    retval = FALSE;	/* return value, default is FAIL */
    int	    lead_len;		/* length of comment leader */
    char_u  *lead_flags;	/* position in 'comments' for comment leader */
    char_u  *leader = NULL;	/* copy of comment leader */
    char_u  *allocated = NULL;	/* allocated memory */
    char_u  *p;
    int	    saved_char = NUL;	/* init for GCC */
    FPOS    *pos;
    int	    old_plines = 0;	/* init for GCC */
    int	    new_plines = 0;	/* init for GCC */
#ifdef SMARTINDENT
    int	    no_si = FALSE;	/* reset did_si afterwards */
    int	    first_char = NUL;	/* init for GCC */
#endif

    /*
     * make a copy of the current line so we can mess with it
     */
    saved_line = vim_strsave(ml_get_curline());
    if (saved_line == NULL)	    /* out of memory! */
	return FALSE;

    if (State == INSERT || State == REPLACE)
    {
	p_extra = saved_line + curwin->w_cursor.col;
#ifdef SMARTINDENT
	if (curbuf->b_p_si)	    /* need first char after new line break */
	{
	    p = skipwhite(p_extra);
	    first_char = *p;
	}
#endif
	extra_len = STRLEN(p_extra);
	saved_char = *p_extra;
	*p_extra = NUL;
    }

    u_clearline();		/* cannot do "U" command when adding lines */
#ifdef SMARTINDENT
    did_si = FALSE;
#endif

    /*
     * If 'autoindent' and/or 'smartindent' is set, try to figure out what
     * indent to use for the new line.
     */
    if (curbuf->b_p_ai
#ifdef SMARTINDENT
			|| curbuf->b_p_si
#endif
					    )
    {
	/*
	 * count white space on current line
	 */
	newindent = get_indent();
	if (newindent == 0)
	    newindent = old_indent;	/* for ^^D command in insert mode */

	/*
	 * If we just did an auto-indent, then we didn't type anything on
	 * the prior line, and it should be truncated.
	 */
	if (dir == FORWARD && did_ai)
	    trunc_line = TRUE;

#ifdef SMARTINDENT
	/*
	 * Do smart indenting.
	 * In insert/replace mode (only when dir == FORWARD)
	 * we may move some text to the next line. If it starts with '{'
	 * don't add an indent. Fixes inserting a NL before '{' in line
	 *	"if (condition) {"
	 */
	else if (curbuf->b_p_si && *saved_line != NUL &&
				       (p_extra == NULL || first_char != '{'))
	{
	    char_u  *ptr;
	    char_u  last_char;

	    old_cursor = curwin->w_cursor;
	    ptr = saved_line;
	    lead_len = get_leader_len(ptr, NULL);
	    if (dir == FORWARD)
	    {
		/*
		 * Skip preprocessor directives, unless they are
		 * recognised as comments.
		 */
		if (lead_len == 0 && ptr[0] == '#')
		{
		    while (ptr[0] == '#' && curwin->w_cursor.lnum > 1)
			ptr = ml_get(--curwin->w_cursor.lnum);
		    newindent = get_indent();
		}
		lead_len = get_leader_len(ptr, NULL);
		if (lead_len > 0)
		{
		    /*
		     * This case gets the following right:
		     *	    \*
		     *	     * A comment (read '\' as '/').
		     *	     *\
		     * #define IN_THE_WAY
		     *	    This should line up here;
		     */
		    p = skipwhite(ptr);
		    if (p[0] == '/' && p[1] == '*')
			p++;
		    if (p[0] == '*')
		    {
			for (p++; *p; p++)
			{
			    if (p[0] == '/' && p[-1] == '*')
			    {
				/*
				 * End of C comment, indent should line up
				 * with the line containing the start of
				 * the comment
				 */
				curwin->w_cursor.col = p - ptr;
				if ((pos = findmatch(NULL, NUL)) != NULL)
				{
				    curwin->w_cursor.lnum = pos->lnum;
				    newindent = get_indent();
				}
			    }
			}
		    }
		}
		else	/* Not a comment line */
		{
		    /* Find last non-blank in line */
		    p = ptr + STRLEN(ptr) - 1;
		    while (p > ptr && vim_iswhite(*p))
			--p;
		    last_char = *p;

		    /*
		     * find the character just before the '{' or ';'
		     */
		    if (last_char == '{' || last_char == ';')
		    {
			if (p > ptr)
			    --p;
			while (p > ptr && vim_iswhite(*p))
			    --p;
		    }
		    /*
		     * Try to catch lines that are split over multiple
		     * lines.  eg:
		     *	    if (condition &&
		     *			condition) {
		     *		Should line up here!
		     *	    }
		     */
		    if (*p == ')')
		    {
			curwin->w_cursor.col = p - ptr;
			if ((pos = findmatch(NULL, '(')) != NULL)
			{
			    curwin->w_cursor.lnum = pos->lnum;
			    newindent = get_indent();
			    ptr = ml_get_curline();
			}
		    }
		    /*
		     * If last character is '{' do indent, without
		     * checking for "if" and the like.
		     */
		    if (last_char == '{')
		    {
			did_si = TRUE;	/* do indent */
			no_si = TRUE;	/* don't delete it when '{' typed */
		    }
		    /*
		     * Look for "if" and the like, use 'cinwords'.
		     * Don't do this if the previous line ended in ';' or
		     * '}'.
		     */
		    else if (last_char != ';' && last_char != '}'
						       && cin_is_cinword(ptr))
			did_si = TRUE;
		}
	    }
	    else /* dir == BACKWARD */
	    {
		/*
		 * Skip preprocessor directives, unless they are
		 * recognised as comments.
		 */
		if (lead_len == 0 && ptr[0] == '#')
		{
		    int was_backslashed = FALSE;

		    while ((ptr[0] == '#' || was_backslashed) &&
			 curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
		    {
			if (*ptr && ptr[STRLEN(ptr) - 1] == '\\')
			    was_backslashed = TRUE;
			else
			    was_backslashed = FALSE;
			ptr = ml_get(++curwin->w_cursor.lnum);
		    }
		    if (was_backslashed)
			newindent = 0;	    /* Got to end of file */
		    else
			newindent = get_indent();
		}
		p = skipwhite(ptr);
		if (*p == '}')	    /* if line starts with '}': do indent */
		    did_si = TRUE;
		else		    /* can delete indent when '{' typed */
		    can_si_back = TRUE;
	    }
	    curwin->w_cursor = old_cursor;
	}
	if (curbuf->b_p_si)
	    can_si = TRUE;
#endif /* SMARTINDENT */

	did_ai = TRUE;
    }

    /*
     * Find out if the current line starts with a comment leader.
     * This may then be inserted in front of the new line.
     */
    lead_len = get_leader_len(saved_line, &lead_flags);
    if (lead_len > 0)
    {
	char_u	*lead_repl = NULL;	    /* replaces comment leader */
	int	lead_repl_len = 0;	    /* length of *lead_repl */
	char_u	lead_middle[COM_MAX_LEN];   /* middle-comment string */
	char_u	lead_end[COM_MAX_LEN];	    /* end-comment string */
	char_u	*comment_end = NULL;	    /* where lead_end has been found */
	int	extra_space = FALSE;	    /* append extra space */
	int	current_flag;

	/*
	 * If the comment leader has the start, middle or end flag, it may not
	 * be used or may be replaced with the middle leader.
	 */
	for (p = lead_flags; *p && *p != ':'; ++p)
	{
	    if (*p == COM_START || *p == COM_MIDDLE)
	    {
		current_flag = *p;
		if (*p == COM_START)
		{
		    /*
		     * Doing "O" on a start of comment does not insert leader.
		     */
		    if (dir == BACKWARD)
		    {
			lead_len = 0;
			break;
		    }

		   /* find start of middle part */
		    (void)copy_option_part(&p, lead_middle, COM_MAX_LEN, ",");
		}

		/*
		 * Isolate the strings of the middle and end leader.
		 */
		while (*p && p[-1] != ':')	/* find end of middle flags */
		    ++p;
		(void)copy_option_part(&p, lead_middle, COM_MAX_LEN, ",");
		while (*p && p[-1] != ':')	/* find end of end flags */
		    ++p;
		(void)copy_option_part(&p, lead_end, COM_MAX_LEN, ",");

		/*
		 * If the end of the comment is in the same line, don't use
		 * the comment leader.
		 */
		if (dir == FORWARD)
		{
		    n = STRLEN(lead_end);
		    for (p = saved_line + lead_len; *p; ++p)
			if (STRNCMP(p, lead_end, n) == 0)
			{
			    comment_end = p;
			    lead_len = 0;
			    break;
			}
		}

		/*
		 * Doing "o" on a start of comment inserts the middle leader.
		 */
		if (lead_len)
		{
		    if (current_flag == COM_START)
		    {
			lead_repl = lead_middle;
			lead_repl_len = STRLEN(lead_middle);
		    }

		    /*
		     * If we have hit RETURN immediately after the start
		     * comment leader, then put a space after the middle
		     * comment leader on the next line.
		     */
		    if (!vim_iswhite(saved_line[lead_len - 1]) &&
			    ((p_extra != NULL &&
				     (int)curwin->w_cursor.col == lead_len) ||
			     (p_extra == NULL && saved_line[lead_len] == NUL)))
			extra_space = TRUE;
		}
		break;
	    }
	    if (*p == COM_END)
	    {
		/*
		 * Doing "o" on the end of a comment does not insert leader.
		 * Remember where the end is, might want to use it to find the
		 * start (for C-comments).
		 */
		if (dir == FORWARD)
		{
		    comment_end = skipwhite(saved_line);
		    lead_len = 0;
		    break;
		}

		/*
		 * Doing "O" on the end of a comment inserts the middle leader.
		 * Find the string for the middle leader, searching backwards.
		 */
		while (p > curbuf->b_p_com && *p != ',')
		    --p;
		for (lead_repl = p; lead_repl > curbuf->b_p_com &&
					    lead_repl[-1] != ':'; --lead_repl)
		    ;
		lead_repl_len = p - lead_repl;
		break;
	    }
	    if (*p == COM_FIRST)
	    {
		/*
		 * Comment leader for first line only:	Don't repeat leader
		 * when using "O", blank out leader when using "o".
		 */
		if (dir == BACKWARD)
		    lead_len = 0;
		else
		{
		    lead_repl = (char_u *)"";
		    lead_repl_len = 0;
		}
		break;
	    }
	}
	if (lead_len)
	{
	    /* allocate buffer (may concatenate p_exta later) */
	    leader = alloc(lead_len + lead_repl_len + extra_space +
							      extra_len + 1);
	    allocated = leader;		    /* remember to free it later */

	    if (leader == NULL)
		lead_len = 0;
	    else
	    {
		STRNCPY(leader, saved_line, lead_len);
		leader[lead_len] = NUL;

		/*
		 * Replace leader with lead_repl, right or left adjusted
		 */
		if (lead_repl != NULL)
		{
		    for (p = lead_flags; *p && *p != ':'; ++p)
			if (*p == COM_RIGHT || *p == COM_LEFT)
			    break;
		    if (*p == COM_RIGHT)    /* right adjusted leader */
		    {
			/* find last non-white in the leader to line up with */
			for (p = leader + lead_len - 1; p > leader &&
							 vim_iswhite(*p); --p)
			    ;

			++p;
			if (p < leader + lead_repl_len)
			    p = leader;
			else
			    p -= lead_repl_len;
			vim_memmove(p, lead_repl, (size_t)lead_repl_len);
			if (p + lead_repl_len > leader + lead_len)
			    p[lead_repl_len] = NUL;

			/* blank-out any other chars from the old leader. */
			while (--p >= leader)
			    if (!vim_iswhite(*p))
				*p = ' ';
		    }
		    else		    /* left adjusted leader */
		    {
			p = skipwhite(leader);
			vim_memmove(p, lead_repl, (size_t)lead_repl_len);

			/* blank-out any other chars from the old leader. */
			for (p += lead_repl_len; p < leader + lead_len; ++p)
			    if (!vim_iswhite(*p))
				*p = ' ';
			*p = NUL;
		    }

		    /* Recompute the indent, it may have changed. */
		    if (curbuf->b_p_ai
#ifdef SMARTINDENT
					|| curbuf->b_p_si
#endif
							   )
			newindent = get_indent_str(leader);
		}

		lead_len = STRLEN(leader);
		if (extra_space)
		{
		    leader[lead_len++] = ' ';
		    leader[lead_len] = NUL;
		}

		newcol = lead_len;

		/*
		 * if a new indent will be set below, remove the indent that
		 * is in the comment leader
		 */
		if (newindent
#ifdef SMARTINDENT
				|| did_si
#endif
					   )
		{
		    while (lead_len && vim_iswhite(*leader))
		    {
			--lead_len;
			--newcol;
			++leader;
		    }
		}

	    }
#ifdef SMARTINDENT
	    did_si = can_si = FALSE;
#endif
	}
	else if (comment_end != NULL)
	{
	    /*
	     * We have finished a comment, so we don't use the leader.
	     * If this was a C-comment and 'ai' or 'si' is set do a normal
	     * indent to align with the line containing the start of the
	     * comment.
	     */
	    if (comment_end[0] == '*' && comment_end[1] == '/' &&
			(curbuf->b_p_ai
#ifdef SMARTINDENT
					|| curbuf->b_p_si
#endif
							   ))
	    {
		old_cursor = curwin->w_cursor;
		curwin->w_cursor.col = comment_end - saved_line;
		if ((pos = findmatch(NULL, NUL)) != NULL)
		{
		    curwin->w_cursor.lnum = pos->lnum;
		    newindent = get_indent();
		}
		curwin->w_cursor = old_cursor;
	    }
	}
    }

    /* (State == INSERT || State == REPLACE), only when dir == FORWARD */
    if (p_extra != NULL)
    {
	*p_extra = saved_char;		/* restore char that NUL replaced */

	/*
	 * When 'ai' set or "del_spaces" TRUE, skip to the first non-blank.
	 *
	 * When in REPLACE mode, put the deleted blanks on the replace stack,
	 * preceded by a NUL, so they can be put back when a BS is entered.
	 */
	if (State == REPLACE)
	    replace_push(NUL);	    /* end of extra blanks */
	if (curbuf->b_p_ai || del_spaces)
	{
	    while (*p_extra == ' ' || *p_extra == '\t')
	    {
		if (State == REPLACE)
		    replace_push(*p_extra);
		++p_extra;
	    }
	}
	if (*p_extra != NUL)
	    did_ai = FALSE;	    /* append some text, don't trucate now */
    }

    if (p_extra == NULL)
	p_extra = (char_u *)"";		    /* append empty line */

    /* concatenate leader and p_extra, if there is a leader */
    if (lead_len)
    {
	STRCAT(leader, p_extra);
	p_extra = leader;
    }

    old_cursor = curwin->w_cursor;
    if (dir == BACKWARD)
	--curwin->w_cursor.lnum;
    if (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_t)0, FALSE) == FAIL)
	goto theend;
    changed_line_abv_curs();	    /* update cursor screen position later */
    mark_adjust(curwin->w_cursor.lnum + 1, (linenr_t)MAXLNUM, 1L, 0L);
    if (newindent
#ifdef SMARTINDENT
		    || did_si
#endif
				)
    {
	++curwin->w_cursor.lnum;
#ifdef SMARTINDENT
	if (did_si)
	{
	    if (p_sr)
		newindent -= newindent % (int)curbuf->b_p_sw;
	    newindent += (int)curbuf->b_p_sw;
	}
#endif
	set_indent(newindent, FALSE);
	/*
	 * In REPLACE mode, for each character in the new indent, there must
	 * be a NUL on the replace stack, for when it is deleted with BS
	 */
	if (State == REPLACE)
	    for (n = 0; n < (int)curwin->w_cursor.col; ++n)
		replace_push(NUL);
	newcol += curwin->w_cursor.col;
#ifdef SMARTINDENT
	if (no_si)
	    did_si = FALSE;
#endif
    }
    /*
     * In REPLACE mode, for each character in the extra leader, there must be
     * a NUL on the replace stack, for when it is deleted with BS.
     */
    if (State == REPLACE)
	while (lead_len-- > 0)
	    replace_push(NUL);

    curwin->w_cursor = old_cursor;

    if (dir == FORWARD)
    {
	if (redraw)	/* want to know the old number of screen lines */
	{
	    old_plines = plines(curwin->w_cursor.lnum);
	    new_plines = old_plines;
	}
	if (trunc_line || State == INSERT || State == REPLACE)
	{
	    if (trunc_line)
	    {
		    /* find start of trailing white space */
		for (n = STRLEN(saved_line); n > 0 &&
					  vim_iswhite(saved_line[n - 1]); --n)
		    ;
		saved_line[n] = NUL;
	    }
	    else		    /* truncate current line at cursor */
		*(saved_line + curwin->w_cursor.col) = NUL;
	    ml_replace(curwin->w_cursor.lnum, saved_line, FALSE);
	    saved_line = NULL;
	    if (redraw)
		new_plines = plines(curwin->w_cursor.lnum);
	}

	/*
	 * Get the cursor to the start of the line, so that 'curwin->w_wrow'
	 * gets set to the right physical line number for the stuff that
	 * follows...
	 */
	curwin->w_cursor.col = 0;

	if (redraw)
	{
	    /*
	     * If we're doing an open on the last logical line, then go ahead
	     * and scroll the screen up. Otherwise, just insert blank lines
	     * at the right place if the number of screen lines changed.
	     * We use calls to plines() in case the cursor is resting on a
	     * long line, we want to know the row below the line.
	     *
	     * Note: using w_cline_row from before the change!
	     */
	    n = curwin->w_cline_row + new_plines;
	    if (n + plines(curwin->w_cursor.lnum + 1) - 1 >=
						      curwin->w_height - p_so)
		scrollup(1L);
	    else
		win_ins_lines(curwin, n,
		  plines(curwin->w_cursor.lnum + 1) + new_plines - old_plines,
								  TRUE, TRUE);
	}

	/*
	 * Put the cursor on the new line.  Careful: the scrollup() above may
	 * have moved w_cursor, we must use old_cursor.
	 */
	curwin->w_cursor.lnum = old_cursor.lnum + 1;
    }
    else if (redraw)
    {
	/*
	 * Insert physical line above current line.
	 * Note: use w_cline_row from before the change.
	 */
	win_ins_lines(curwin, curwin->w_cline_row, 1, TRUE, TRUE);
    }

    curwin->w_cursor.col = newcol;

#ifdef LISPINDENT
    /*
     * May do lisp indenting.
     */
    if (leader == NULL && curbuf->b_p_lisp && curbuf->b_p_ai)
	fixthisline(get_lisp_indent);
#endif
#ifdef CINDENT
    /*
     * May do indenting after opening a new line.
     */
    if (leader == NULL && curbuf->b_p_cin &&
	    in_cinkeys(dir == FORWARD ? KEY_OPEN_FORW :
			KEY_OPEN_BACK, ' ', linewhite(curwin->w_cursor.lnum)))
	fixthisline(get_c_indent);
#endif

    /*
     * w_botline won't change much when inserting a new line.
     */
    approximate_botline();

    if (redraw > 0)
    {
	update_topline();
	update_screen(VALID_BEF_CURSCHAR);
    }
    CHANGED;

    retval = TRUE;		/* success! */
theend:
    vim_free(saved_line);
    vim_free(allocated);
    return retval;
}

/*
 * get_leader_len() returns the length of the prefix of the given string
 * which introduces a comment.	If this string is not a comment then 0 is
 * returned.
 * When "flags" is non-zero, it is set to point to the flags of the recognized
 * comment leader.
 */
    int
get_leader_len(line, flags)
    char_u  *line;
    char_u  **flags;
{
    int	    i, j;
    int	    got_com = FALSE;
    int	    found_one;
    char_u  part_buf[COM_MAX_LEN];  /* buffer for one option part */
    char_u  *string;		    /* pointer to comment string */
    char_u  *list;

    if (!fo_do_comments)	    /* don't format comments at all */
	return 0;

    i = 0;
    while (vim_iswhite(line[i]))    /* leading white space is ignored */
	++i;

    /*
     * Repeat to match several nested comment strings.
     */
    while (line[i])
    {
	/*
	 * scan through the 'comments' option for a match
	 */
	found_one = FALSE;
	for (list = curbuf->b_p_com; *list; )
	{
	    /*
	     * Get one option part into part_buf[].  Advance list to next one.
	     * put string at start of string.
	     */
	    if (!got_com && flags != NULL)  /* remember where flags started */
		*flags = list;
	    (void)copy_option_part(&list, part_buf, COM_MAX_LEN, ",");
	    string = vim_strchr(part_buf, ':');
	    if (string == NULL)	    /* missing ':', ignore this part */
		continue;
	    *string++ = NUL;	    /* isolate flags from string */

	    /*
	     * When already found a nested comment, only accept further
	     * nested comments.
	     */
	    if (got_com && vim_strchr(part_buf, COM_NEST) == NULL)
		continue;

	    /*
	     * Line contents and string must match.
	     */
	    for (j = 0; string[j] != NUL && string[j] == line[i + j]; ++j)
		;
	    if (string[j] != NUL)
		continue;

	    /*
	     * When 'b' flag used, there must be white space or an
	     * end-of-line after the string in the line.
	     */
	    if (vim_strchr(part_buf, COM_BLANK) != NULL &&
			      !vim_iswhite(line[i + j]) && line[i + j] != NUL)
		continue;

	    /*
	     * We have found a match, stop searching.
	     */
	    i += j;
	    got_com = TRUE;
	    found_one = TRUE;
	    break;
	}

	/*
	 * No match found, stop scanning.
	 */
	if (!found_one)
	    break;

	/*
	 * Include any trailing white space.
	 */
	while (vim_iswhite(line[i]))
	    ++i;

	/*
	 * If this comment doesn't nest, stop here.
	 */
	if (vim_strchr(part_buf, COM_NEST) == NULL)
	    break;
    }
    return (got_com ? i : 0);
}

/*
 * plines(p) - return the number of physical screen lines taken by line 'p'
 */
    int
plines(p)
    linenr_t	p;
{
    return plines_win(curwin, p);
}

    int
plines_win(wp, p)
    WIN		*wp;
    linenr_t	p;
{
    long	col;
    char_u	*s;
    int		lines;

    if (!wp->w_p_wrap)
	return 1;

    s = ml_get_buf(wp->w_buffer, p, FALSE);
    if (*s == NUL)		/* empty line */
	return 1;

    col = win_linetabsize(wp, s);

    /*
     * If list mode is on, then the '$' at the end of the line takes up one
     * extra column.
     */
    if (wp->w_p_list)
	col += 1;

    /*
     * If 'number' mode is on, add another 8.
     */
    if (wp->w_p_nu)
	col += 8;

    lines = (col + (Columns - 1)) / Columns;
    if (lines <= wp->w_height)
	return lines;
    return (int)(wp->w_height);	    /* maximum length */
}

/*
 * Like plines_win(), but only reports the number of physical screen lines used
 * from the start of the line to the given column number.
 */
    int
plines_win_col(wp, p, column)
    WIN		*wp;
    linenr_t	p;
    long	column;
{
    register long	col;
    register char_u	*s;
    register int	lines;

    if (!wp->w_p_wrap)
	return 1;

    s = ml_get_buf(wp->w_buffer, p, FALSE);

    col = 0;
    while (*s != NUL && --column >= 0)
	col += win_lbr_chartabsize(wp, s++, (colnr_t)col, NULL);

    /*
     * If *s is a TAB, and the TAB is not displayed as ^I, and we're not in
     * INSERT mode, then col must be adjusted so that it represents the last
     * screen position of the TAB.  This only fixes an error when the TAB wraps
     * from one screen line to the next (when 'columns' is not a multiple of
     * 'ts') -- webb.
     */
    if (*s == TAB && (State & NORMAL) && !wp->w_p_list)
	col += win_lbr_chartabsize(wp, s, (colnr_t)col, NULL) - 1;

    /*
     * If 'number' mode is on, add another 8.
     */
    if (wp->w_p_nu)
	col += 8;

    lines = 1 + col / Columns;
    if (lines <= wp->w_height)
	return lines;
    return (int)(wp->w_height);	    /* maximum length */
}


/*
 * Count the physical lines (rows) for the lines "first" to "last" inclusive.
 */
    int
plines_m(first, last)
    linenr_t	    first, last;
{
    return plines_m_win(curwin, first, last);
}

    int
plines_m_win(wp, first, last)
    WIN		    *wp;
    linenr_t	    first, last;
{
    int count = 0;

    while (first <= last)
	count += plines_win(wp, first++);
    return (count);
}

/*
 * Insert or replace a single character at the cursor position.
 * When in REPLACE mode, replace any existing character.
 */
    void
ins_char(c)
    int		c;
{
    char_u	    *p;
    char_u	    *newp;
    char_u	    *oldp;
    int		    oldlen;
    int		    extra;
    colnr_t	    col = curwin->w_cursor.col;
    linenr_t	    lnum = curwin->w_cursor.lnum;

    oldp = ml_get(lnum);
    oldlen = STRLEN(oldp) + 1;

    if (State != REPLACE || *(oldp + col) == NUL)
	extra = 1;
    else
	extra = 0;

    /*
     * A character has to be put on the replace stack if there is a
     * character that is replaced, so it can be put back when BS is used.
     */
    if (State == REPLACE)
    {
	replace_push(NUL);
	if (!extra)
	    replace_push(*(oldp + col));
    }
    newp = alloc_check((unsigned)(oldlen + extra));
    if (newp == NULL)
	return;
    vim_memmove(newp, oldp, (size_t)col);
    p = newp + col;
    vim_memmove(p + extra, oldp + col, (size_t)(oldlen - col));
    *p = c;
    ml_replace(lnum, newp, FALSE);

    /*
     * If we're in insert or replace mode and 'showmatch' is set, then check for
     * right parens and braces. If there isn't a match, then beep. If there
     * is a match AND it's on the screen, then flash to it briefly. If it
     * isn't on the screen, don't do anything.
     */
#ifdef RIGHTLEFT
    if (p_sm && (State & INSERT) &&
	((!(curwin->w_p_rl ^ p_ri) && (c == ')' || c == '}' || c == ']')) ||
	 ((curwin->w_p_rl ^ p_ri) && (c == '(' || c == '{' || c == '['))))
#else
    if (p_sm && (State & INSERT) && (c == ')' || c == '}' || c == ']'))
#endif
	showmatch();

#ifdef RIGHTLEFT
    if (!p_ri || State == REPLACE)	/* normal insert: cursor right */
#endif
	++curwin->w_cursor.col;
    CHANGED;
    /*
     * TODO: should try to update w_row here, to avoid recomputing it later.
     */
    changed_cline_bef_curs();
    approximate_botline();	/* w_botline might have changed */
}

/*
 * Insert a string at the cursor position.
 * Note: Nothing special for replace mode.
 */
    void
ins_str(s)
    char_u  *s;
{
    char_u	*oldp, *newp;
    int		newlen = STRLEN(s);
    int		oldlen;
    colnr_t	col = curwin->w_cursor.col;
    linenr_t	lnum = curwin->w_cursor.lnum;

    oldp = ml_get(lnum);
    oldlen = STRLEN(oldp);

    newp = alloc_check((unsigned)(oldlen + newlen + 1));
    if (newp == NULL)
	return;
    vim_memmove(newp, oldp, (size_t)col);
    vim_memmove(newp + col, s, (size_t)newlen);
    vim_memmove(newp + col + newlen, oldp + col, (size_t)(oldlen - col + 1));
    ml_replace(lnum, newp, FALSE);
    curwin->w_cursor.col += newlen;
    CHANGED;
    changed_cline_bef_curs();
    approximate_botline();	/* w_botline might have changed */
}

/*
 * Delete one character under the cursor.
 * If 'fixpos' is TRUE, don't let the cursor on the NUL after the line.
 *
 * return FAIL for failure, OK otherwise
 */
    int
del_char(fixpos)
    int		fixpos;
{
    return del_chars(1L, fixpos);
}

/*
 * Delete 'count' characters under the cursor.
 * If 'fixpos' is TRUE, don't leave the cursor on the NUL after the line.
 *
 * return FAIL for failure, OK otherwise
 */
    int
del_chars(count, fixpos)
    long	count;
    int		fixpos;
{
    char_u	*oldp, *newp;
    colnr_t	oldlen;
    linenr_t	lnum = curwin->w_cursor.lnum;
    colnr_t	col = curwin->w_cursor.col;
    int		was_alloced;
    long	movelen;

    oldp = ml_get(lnum);
    oldlen = STRLEN(oldp);

    /*
     * Can't do anything when the cursor is on the NUL after the line.
     */
    if (col >= oldlen)
	return FAIL;

    /*
     * When count is too big, reduce it.
     */
    movelen = (long)oldlen - (long)col - count + 1; /* includes trailing NUL */
    if (movelen <= 1)
    {
	/*
	 * If we just took off the last character of a non-blank line, and
	 * fixpos is TRUE, we don't want to end up positioned at the NUL.
	 */
	if (col > 0 && fixpos)
	    --curwin->w_cursor.col;
	count = oldlen - col;
	movelen = 1;
    }

    /*
     * If the old line has been allocated the deletion can be done in the
     * existing line. Otherwise a new line has to be allocated
     */
    was_alloced = ml_line_alloced();	    /* check if oldp was allocated */
    if (was_alloced)
	newp = oldp;			    /* use same allocated memory */
    else
    {					    /* need to allocated a new line */
	newp = alloc((unsigned)(oldlen + 1 - count));
	if (newp == NULL)
	    return FAIL;
	vim_memmove(newp, oldp, (size_t)col);
    }
    vim_memmove(newp + col, oldp + col + count, (size_t)movelen);
    if (!was_alloced)
	ml_replace(lnum, newp, FALSE);

    CHANGED;

    /*
     * When the new character under the cursor is a TAB, the cursor will move
     * on the screen, so we can't use changed_cline_aft_curs() here.
     */
    changed_cline_bef_curs();
    approximate_botline();	/* w_botline might have changed */
    return OK;
}

/*
 * Delete from cursor to end of line.
 *
 * return FAIL for failure, OK otherwise
 */
    int
truncate_line(fixpos)
    int		fixpos;	    /* if TRUE fix the cursor position when done */
{
    char_u	*newp;
    linenr_t	lnum = curwin->w_cursor.lnum;
    colnr_t	col = curwin->w_cursor.col;

    if (col == 0)
	newp = vim_strsave((char_u *)"");
    else
	newp = vim_strnsave(ml_get(lnum), col);

    if (newp == NULL)
	return FAIL;

    ml_replace(lnum, newp, FALSE);

    /*
     * If "fixpos" is TRUE we don't want to end up positioned at the NUL.
     */
    if (fixpos && curwin->w_cursor.col > 0)
	--curwin->w_cursor.col;

    CHANGED;
    changed_cline_bef_curs();
    approximate_botline();	/* w_botline might have changed */
    return OK;
}

    void
del_lines(nlines, dowindow, undo)
    long	    nlines;	    /* number of lines to delete */
    int		    dowindow;	    /* if true, update the window */
    int		    undo;	    /* if true, prepare for undo */
{
    int		    num_plines = 0;

    if (nlines <= 0)
	return;
    /*
     * There's no point in keeping the window updated if redrawing is disabled
     * or we're deleting more than a window's worth of lines.
     */
    if (!redrawing() || !botline_approximated())
	dowindow = FALSE;
    else
    {
	validate_cursor();
	if (nlines > (curwin->w_height - curwin->w_wrow) && dowindow)
	{
	    dowindow = FALSE;
	    /* flaky way to clear rest of window */
	    win_del_lines(curwin, curwin->w_wrow, curwin->w_height, TRUE, TRUE);
	}
    }

    /*
     * Assume that w_botline won't change much.  If it does, there is a small
     * risc of an extra redraw or scroll-up.
     */
    approximate_botline();

    /* save the deleted lines for undo */
    if (undo && u_savedel(curwin->w_cursor.lnum, nlines) == FAIL)
	return;

    /* adjust marks for deleted lines and lines that follow */
    mark_adjust(curwin->w_cursor.lnum, curwin->w_cursor.lnum + nlines - 1,
						  (linenr_t)MAXLNUM, -nlines);

    while (nlines-- > 0)
    {
	if (curbuf->b_ml.ml_flags & ML_EMPTY)	    /* nothing to delete */
	    break;

	/*
	 * Set up to delete the correct number of physical lines on the
	 * window
	 */
	if (dowindow)
	    num_plines += plines(curwin->w_cursor.lnum);

	ml_delete(curwin->w_cursor.lnum, TRUE);

	CHANGED;

	/* If we delete the last line in the file, stop */
	if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	{
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    break;
	}
    }
    curwin->w_cursor.col = 0;
    /*
     * The cursor will stay in the same line number, but the contents will be
     * different, so need to update it's screen posision later.
     */
    changed_cline_bef_curs();

    /*
     * Delete the correct number of physical lines on the window
     */
    if (dowindow && num_plines > 0)
    {
	validate_cline_row();
	win_del_lines(curwin, curwin->w_cline_row, num_plines, TRUE, TRUE);
    }
}

    int
gchar(pos)
    FPOS *pos;
{
    return (int)(*(ml_get_pos(pos)));
}

    int
gchar_cursor()
{
    return (int)(*(ml_get_cursor()));
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
    void
pchar_cursor(c)
    int c;
{
    *(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE) +
						    curwin->w_cursor.col) = c;
}

/*
 * Put *pos at end of current buffer
 */
    void
goto_endofbuf(pos)
    FPOS    *pos;
{
    char_u  *p;

    pos->lnum = curbuf->b_ml.ml_line_count;
    pos->col = 0;
    p = ml_get(pos->lnum);
    while (*p++)
	++pos->col;
}

/*
 * When extra == 0: Return TRUE if the cursor is before or on the first
 *		    non-blank in the line.
 * When extra == 1: Return TRUE if the cursor is before the first non-blank in
 *		    the line.
 */
    int
inindent(extra)
    int	    extra;
{
    char_u	*ptr;
    colnr_t	col;

    for (col = 0, ptr = ml_get_curline(); vim_iswhite(*ptr); ++col)
	++ptr;
    if (col >= curwin->w_cursor.col + extra)
	return TRUE;
    else
	return FALSE;
}

/*
 * Skip to next part of an option argument: Skip space and comma.
 */
    char_u *
skip_to_option_part(p)
    char_u  *p;
{
    if (*p == ',')
	++p;
    while (*p == ' ')
	++p;
    return p;
}

    char *
plural(n)
    long n;
{
    static char buf[2] = "s";

    if (n == 1)
	return &(buf[1]);
    return &(buf[0]);
}

/*
 * set_Changed is called when something in the current buffer is changed
 */
    void
set_Changed()
{
    int	    save_msg_scroll = msg_scroll;

    if (!curbuf->b_changed)
    {
	change_warning(0);
	if (curbuf->b_may_swap)	    /* still need to create a swap file */
	{
	    ml_open_file(curbuf);

	    /* The ml_open_file() can cause an ATTENTION message.
	     * Wait two seconds, to make sure the user reads this unexpected
	     * message.  Since we could be anywhere, call wait_return() now,
	     * and don't let the emsg() set msg_scroll. */
	    if (need_wait_return)
	    {
		out_flush();
		ui_delay(2000L, TRUE);
		wait_return(TRUE);
		msg_scroll = save_msg_scroll;
	    }
	}
	curbuf->b_changed = TRUE;
	check_status(curbuf);
    }
    modified = TRUE;		    /* used for redrawing */
    tag_modified = TRUE;	    /* used for tag searching check */
}

/*
 * unset_Changed is called when the changed flag must be reset for buffer 'buf'
 */
    void
unset_Changed(buf)
    BUF	    *buf;
{
    if (buf->b_changed)
    {
	buf->b_changed = 0;
	check_status(buf);
    }
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *		 need to be updated
 */
    void
check_status(buf)
    BUF	    *buf;
{
    WIN	    *wp;
    int	    i;

    i = 0;
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	if (wp->w_buffer == buf && wp->w_status_height)
	{
	    wp->w_redr_status = TRUE;
	    ++i;
	}
    if (i)
	redraw_later(NOT_VALID);
}

/*
 * If the file is readonly, give a warning message with the first change.
 * Don't do this for autocommands.
 * Don't use emsg(), because it flushes the macro buffer.
 * If we have undone all changes b_changed will be FALSE, but b_did_warn
 * will be TRUE.
 */
    void
change_warning(col)
    int	    col;		/* column for message; non-zero when in insert
				   mode and 'showmode' is on */
{
    if (curbuf->b_did_warn == FALSE
	    && curbuf_changed() == 0
	    && !p_im
#ifdef AUTOCMD
	    && !autocmd_busy
#endif
	    && curbuf->b_p_ro)
    {
	/*
	 * Do what msg() does, but with a column offset.
	 */
	msg_start();
	msg_col = col;
	MSG_PUTS_ATTR("Warning: Changing a readonly file",
						       highlight_attr[HLF_W]);
	msg_clr_eos();
	(void)msg_end();
	ui_delay(1000L, TRUE);	/* give him some time to think about it */
	curbuf->b_did_warn = TRUE;
	redraw_cmdline = FALSE;	/* don't redraw and erase the message */
    }
}

/*
 * Ask for a reply from the user, a 'y' or a 'n'.
 * No other characters are accepted, the message is repeated until a valid
 * reply is entered or CTRL-C is hit.
 * If direct is TRUE, don't use vgetc() but ui_inchar(), don't get characters
 * from any buffers but directly from the user.
 *
 * return the 'y' or 'n'
 */
    int
ask_yesno(str, direct)
    char_u  *str;
    int	    direct;
{
    int	    r = ' ';
    char_u  buf[20];
    int	    len = 0;
    int	    idx = 0;

    if (exiting)		/* put terminal in raw mode for this question */
	settmode(TMODE_RAW);
    ++no_wait_return;
#ifdef USE_GUI_WIN32
    dont_scroll = TRUE;		/* disallow scrolling here */
#endif
    while (r != 'y' && r != 'n')
    {
	/* same highlighting as for wait_return */
	smsg_attr(highlight_attr[HLF_R], (char_u *)"%s (y/n)?", str);
	if (direct)
	{
	    out_flush();
	    if (idx >= len)
	    {
		len = ui_inchar(buf, 20, -1L);
		idx = 0;
	    }
	    r = buf[idx++];
	}
	else
	    r = vgetc();
	if (r == Ctrl('C') || r == ESC)
	    r = 'n';
	msg_putchar(r);	    /* show what you typed */
	out_flush();
    }
    --no_wait_return;
    return r;
}

/*
 * get a number from the user
 */
    int
get_number(colon)
    int	colon;			/* allow colon to abort */
{
    int	n = 0;
    int	c;

#ifdef USE_GUI_WIN32
    dont_scroll = TRUE;		/* disallow scrolling here */
#endif
    for (;;)
    {
	windgoto(msg_row, msg_col);
	c = vgetc();
	if (vim_isdigit(c))
	{
	    n = n * 10 + c - '0';
	    msg_putchar(c);
	}
	else if (c == K_DEL || c == K_BS || c == Ctrl('H'))
	{
	    n /= 10;
	    MSG_PUTS("\b \b");
	}
	else if (n == 0 && c == ':' && colon)
	{
	    stuffcharReadbuff(':');
	    if (!exmode_active)
		cmdline_row = msg_row;
	    skip_redraw = TRUE;	    /* skip redraw once */
	    do_redraw = FALSE;
	    break;
	}
	else if (c == CR || c == NL || c == Ctrl('C') || c == ESC)
	    break;
    }
    return n;
}

    void
msgmore(n)
    long n;
{
    long pn;

    if (global_busy ||	    /* no messages now, wait until global is finished */
	    keep_msg ||	    /* there is a message already, skip this one */
	    !messaging())   /* 'lazyredraw' set, don't do messages now */
	return;

    if (n > 0)
	pn = n;
    else
	pn = -n;

    if (pn > p_report)
    {
	sprintf((char *)msg_buf, "%ld %s line%s %s",
		pn, n > 0 ? "more" : "fewer", plural(pn),
		got_int ? "(Interrupted)" : "");
	if (msg(msg_buf))
	{
	    keep_msg = msg_buf;
	    keep_msg_attr = 0;
	}
    }
}

/*
 * flush map and typeahead buffers and give a warning for an error
 */
    void
beep_flush()
{
    flush_buffers(FALSE);
    vim_beep();
}

/*
 * give a warning for an error
 */
    void
vim_beep()
{
    if (p_vb)
    {
	out_str(T_VB);
    }
    else
    {
#ifdef MSDOS
	/*
	 * The number of beeps outputted is reduced to avoid having to wait
	 * for all the beeps to finish. This is only a problem on systems
	 * where the beeps don't overlap.
	 */
	if (beep_count == 0 || beep_count == 10)
	{
	    out_char('\007');
	    beep_count = 1;
	}
	else
	    ++beep_count;
#else
	out_char('\007');
#endif
    }
}

/*
 * To get the "real" home directory:
 * - get value of $HOME
 * For Unix:
 *  - go to that directory
 *  - do mch_dirname() to get the real name of that directory.
 *  This also works with mounts and links.
 *  Don't do this for MS-DOS, it will change the "current dir" for a drive.
 */
static char_u	*homedir = NULL;

    void
init_homedir()
{
    char_u  *var;

    var = vim_getenv((char_u *)"HOME");
#if defined(OS2) || defined(MSDOS) || defined(WIN32)
    /*
     * Default home dir is C:/
     * Best assumption we can make in such a situation.
     */
    if (var == NULL)
	var = "C:/";
#endif
    if (var != NULL)
    {
#ifdef UNIX
	if (mch_dirname(NameBuff, MAXPATHL) == OK)
	{
	    if (!vim_chdir((char *)var) && mch_dirname(IObuff, IOSIZE) == OK)
		var = IObuff;
	    vim_chdir((char *)NameBuff);
	}
#endif
	homedir = vim_strsave(var);
    }
}

/*
 * Expand environment variable with path name.
 * "~/" is also expanded, using $HOME.	For Unix "~user/" is expanded.
 * If anything fails no expansion is done and dst equals src.
 */
    void
expand_env(src, dst, dstlen)
    char_u  *src;	    /* input string e.g. "$HOME/vim.hlp" */
    char_u  *dst;	    /* where to put the result */
    int	    dstlen;	    /* maximum length of the result */
{
    char_u  *tail;
    int	    c;
    char_u  *var;
    int	    copy_char;
    int	    mustfree;	    /* var was allocated, need to free it later */
    char_u  *p;
    int	    at_start = TRUE;

    src = skipwhite(src);
    --dstlen;		    /* leave one char space for "\," */
    while (*src && dstlen > 0)
    {
	copy_char = TRUE;
	if (*src == '$' || (*src == '~' && at_start))
	{
	    mustfree = FALSE;

	    /*
	     * The variable name is copied into dst temporarily, because it may
	     * be a string in read-only memory and a NUL needs to be inserted.
	     */
	    if (*src == '$')				/* environment var */
	    {
		tail = src + 1;
		var = dst;
		c = dstlen - 1;

#ifdef UNIX
		/* Unix has ${var-name} type environment vars */
		if (*tail == '{' && !vim_isIDc('{'))
		{
		    tail++;	/* ignore '{' */
		    while (c-- > 0 && *tail && *tail != '}')
			*var++ = *tail++;
		    tail++;	/* ignore '}' */
		}
		else
#endif
		{
		    while (c-- > 0 && *tail && vim_isIDc(*tail))
		    {
#ifdef OS2		/* env vars only in uppercase */
			*var++ = TO_UPPER(*tail);
			tail++;	    /* toupper() may be a macro! */
#else
			*var++ = *tail++;
#endif
		    }
		}

		*var = NUL;
#if defined(OS2) || defined(MSDOS) || defined(WIN32)
		/* use "C:/" when $HOME is not set */
		if (STRCMP(dst, "HOME") == 0)
		    var = homedir;
		else
#endif
		{
		    var = vim_getenv(dst);
		    /*
		     * When expanding $VIM fails, try using the directory name
		     * from 'helpfile'.
		     */
		    if (var == NULL && STRCMP(dst, "VIM") == 0)
		    {
			p = NULL;
			if (vim_strchr(p_hf, '$') == NULL)
			    p = p_hf;
#ifdef USE_EXE_NAME
			/*
			 * Use the name of the executable, obtained from
			 * argv[0].
			 */
			else
			{
			    p = exe_name;
# ifdef MSDOS
			    /*
			     * Try the DOS search path.  The executable may in
			     * fact be called differently, so try this last.
			     */
			    if (p == NULL || *p == NUL)
				p = searchpath("vim.exe");
# endif
			}
#endif
			if (p != NULL)
			{
			    char_u	*pend;

			    /* remove the file name */
			    pend = gettail(p);
			    /* remove "doc/" from 'helpfile', if present */
			    if (p == p_hf && pend - 4 >= p
					&& fnamencmp(pend - 4, "doc", 3) == 0
					&& (pend - 4 == p
						|| vim_ispathsep(pend[-5])))
				pend -= 4;
#ifdef USE_EXE_NAME
			    /* remove "src/" from exe_name, if present */
			    if (p == exe_name)
			    {
				if (pend - 4 >= p
					&& fnamencmp(pend - 4, "src", 3) == 0
					&& (pend - 4 == p
						|| vim_ispathsep(pend[-5])))
				    pend -= 4;
			    }
#endif
			    /* remove trailing path separator */
			    if (pend > p && vim_ispathsep(pend[-1]))
				--pend;
			    var = vim_strnsave(p, (int)(pend - p));
			    mustfree = TRUE;
			    if (!mch_isdir(var))
			    {
				vim_free(var);
				var = NULL;
			    }
			}
#ifdef HAVE_PATHDEF
			/* for Unix we can use default_vim_dir */
			if (var == NULL)
			{
			    var = default_vim_dir;
			    mustfree = FALSE;
			}
#endif
		    }
		}
	    }
							/* home directory */
	    else if (  src[1] == NUL
		    || vim_ispathsep(src[1])
		    || vim_strchr((char_u *)" ,\t\n", src[1]) != NULL)
	    {
		var = homedir;
		tail = src + 1;
	    }
	    else					/* user directory */
	    {
#ifndef UNIX
		/* cannot expand user's home directory, so don't try */
		var = NULL;
		tail = (char_u *)"";	/* for gcc */
#else
		/*
		 * Copy ~user to dst[], so we can put a NUL after it.
		 */
		tail = src;
		var = dst;
		c = dstlen - 1;
		while (	   c-- > 0
			&& *tail
			&& vim_isfilec(*tail)
			&& !vim_ispathsep(*tail))
		    *var++ = *tail++;
		*var = NUL;

		/*
		 * If the system supports getpwnam(), use it.
		 * Otherwise, or if getpwnam() fails, the shell is used to
		 * expand ~user.  This is slower and may fail if the shell
		 * does not support ~user (old versions of /bin/sh).
		 */
# if defined(HAVE_GETPWNAM) && defined(HAVE_PWD_H)
		{
		    struct passwd *pw;

		    pw = getpwnam((char *)dst + 1);
		    if (pw != NULL)
			var = (char_u *)pw->pw_dir;
		    else
			var = NULL;
		}
		if (var == NULL)
# endif
		{
		    var = ExpandOne(dst, NULL, 0, WILD_EXPAND_FREE);
		    mustfree = TRUE;
		}
#endif /* UNIX */
	    }

	    if (var != NULL && *var != NUL &&
			  (STRLEN(var) + STRLEN(tail) + 1 < (unsigned)dstlen))
	    {
		STRCPY(dst, var);
		dstlen -= STRLEN(var);
		dst += STRLEN(var);
		    /* if var[] ends in a path separator and tail[] starts
		     * with it, skip a character */
		if (*var && vim_ispathsep(*(dst-1)) && vim_ispathsep(*tail))
		    ++tail;
		src = tail;
		copy_char = FALSE;
	    }
	    if (mustfree)
		vim_free(var);
	}

	if (copy_char)	    /* copy at least one char */
	{
	    /*
	     * Recogize the start of a new name, for '~'.
	     */
	    at_start = FALSE;
	    if (src[0] == '\\')
	    {
		*dst++ = *src++;
		--dstlen;
	    }
	    else if (src[0] == ' ' || src[0] == ',')
		at_start = TRUE;
	    *dst++ = *src++;
	    --dstlen;
	}
    }
    *dst = NUL;
}

/*
 * Call expand_env() and store the result in an allocated string.
 * This is not very memory efficient, this expects the result to be freed
 * again soon.
 */
    char_u *
expand_env_save(src)
    char_u	*src;
{
    char_u	*p;

    p = alloc(MAXPATHL);
    if (p != NULL)
	expand_env(src, p, MAXPATHL);
    return p;
}

/*
 * Replace home directory by "~" in each space or comma separated file name in
 * 'src'.  If anything fails (except when out of space) dst equals src.
 */
    void
home_replace(buf, src, dst, dstlen)
    BUF	    *buf;	    /* when not NULL, check for help files */
    char_u  *src;	    /* input file name */
    char_u  *dst;	    /* where to put the result */
    int	    dstlen;	    /* maximum length of the result */
{
    size_t  dirlen = 0, envlen = 0;
    size_t  len;
    char_u  *homedir_env;
    char_u  *p;

    if (src == NULL)
    {
	*dst = NUL;
	return;
    }

    /*
     * If the file is a help file, remove the path completely.
     */
    if (buf != NULL && buf->b_help)
    {
	STRCPY(dst, gettail(src));
	return;
    }

    /*
     * We check both the value of the $HOME environment variable and the
     * "real" home directory.
     */
    if (homedir != NULL)
	dirlen = STRLEN(homedir);
    homedir_env = vim_getenv((char_u *)"HOME");
    if (homedir_env != NULL)
	envlen = STRLEN(homedir_env);

    src = skipwhite(src);
    while (*src && dstlen > 0)
    {
	/*
	 * Here we are at the beginning of a file name.
	 * First, check to see if the beginning of the file name matches
	 * $HOME or the "real" home directory. Check that there is a '/'
	 * after the match (so that if e.g. the file is "/home/pieter/bla",
	 * and the home directory is "/home/piet", the file does not end up
	 * as "~er/bla" (which would seem to indicate the file "bla" in user
	 * er's home directory)).
	 */
	p = homedir;
	len = dirlen;
	for (;;)
	{
	    if (   len
		&& fnamencmp(src, p, len) == 0
		&& (vim_ispathsep(src[len])
		    || src[len] == ','
		    || src[len] == ' '
		    || src[len] == NUL))
	    {
		src += len;
		if (--dstlen > 0)
		    *dst++ = '~';

		/*
		 * If it's just the home directory, add  "/".
		 */
		if (!vim_ispathsep(src[0]) && --dstlen > 0)
		    *dst++ = '/';
	    }
	    if (p == homedir_env)
		break;
	    p = homedir_env;
	    len = envlen;
	}

	/* skip to separator: space or comma */
	while (*src && *src != ',' && *src != ' ' && --dstlen > 0)
	    *dst++ = *src++;
	/* skip separator */
	while ((*src == ' ' || *src == ',') && --dstlen > 0)
	    *dst++ = *src++;
    }
    /* if (dstlen == 0) out of space, what to do??? */

    *dst = NUL;
}

/*
 * Like home_replace, store the replaced string in allocated memory.
 * When something fails, NULL is returned.
 */
    char_u  *
home_replace_save(buf, src)
    BUF	    *buf;	    /* when not NULL, check for help files */
    char_u  *src;	    /* input file name */
{
    char_u	*dst;
    unsigned	len;

    len = 3;			/* space for "~/" and trailing NUL */
    if (src != NULL)		/* just in case */
	len += STRLEN(src);
    dst = alloc(len);
    if (dst != NULL)
	home_replace(buf, src, dst, len);
    return dst;
}

/*
 * Compare two file names and return:
 * FPC_SAME   if they both exist and are the same file.
 * FPC_SAMEX  if they both don't exist and have the same file name.
 * FPC_DIFF   if they both exist and are different files.
 * FPC_NOTX   if they both don't exist.
 * FPC_DIFFX  if one of them doesn't exist.
 * For the first name environment variables are expanded
 */
    int
fullpathcmp(s1, s2, checkname)
    char_u *s1, *s2;
    int	    checkname;		/* when both don't exist, check file names */
{
#ifdef UNIX
    char_u	    exp1[MAXPATHL];
    char_u	    full1[MAXPATHL];
    char_u	    full2[MAXPATHL];
    struct stat	    st1, st2;
    int		    r1, r2;

    expand_env(s1, exp1, MAXPATHL);
    r1 = stat((char *)exp1, &st1);
    r2 = stat((char *)s2, &st2);
    if (r1 != 0 && r2 != 0)
    {
	/* if stat() doesn't work, may compare the names */
	if (checkname)
	{
	    if (fnamecmp(exp1, s2) == 0)
		return FPC_SAMEX;
	    r1 = mch_FullName(exp1, full1, MAXPATHL, FALSE);
	    r2 = mch_FullName(s2, full2, MAXPATHL, FALSE);
	    if (r1 == OK && r2 == OK && fnamecmp(full1, full2) == 0)
		return FPC_SAMEX;
	}
	return FPC_NOTX;
    }
    if (r1 != 0 || r2 != 0)
	return FPC_DIFFX;
    if (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
	return FPC_SAME;
    return FPC_DIFF;
#else
    char_u  *exp1;		/* expanded s1 */
    char_u  *full1;		/* full path of s1 */
    char_u  *full2;		/* full path of s2 */
    int	    retval = FPC_DIFF;
    int	    r1, r2;

    /* allocate one buffer to store three paths (alloc()/free() is slow!) */
    if ((exp1 = alloc(MAXPATHL * 3)) != NULL)
    {
	full1 = exp1 + MAXPATHL;
	full2 = full1 + MAXPATHL;

	expand_env(s1, exp1, MAXPATHL);
	r1 = mch_FullName(exp1, full1, MAXPATHL, FALSE);
	r2 = mch_FullName(s2, full2, MAXPATHL, FALSE);

	/* If mch_FullName() fails, the file probably doesn't exist. */
	if (r1 != OK && r2 != OK)
	{
	    if (checkname && fnamecmp(exp1, s2) == 0)
		return FPC_SAMEX;
	    retval = FPC_NOTX;
	}
	else if (r1 != OK || r2 != OK)
	    retval = FPC_DIFFX;
	else if (fnamecmp(full1, full2))
	    retval = FPC_DIFF;
	else
	    retval = FPC_SAME;
    }
    vim_free(exp1);
    return retval;
#endif
}

/*
 * get the tail of a path: the file name.
 */
    char_u *
gettail(fname)
    char_u *fname;
{
    char_u  *p1, *p2;

    if (fname == NULL)
	return (char_u *)"";
    for (p1 = p2 = fname; *p2; ++p2)	/* find last part of path */
    {
	if (vim_ispathsep(*p2))
	    p1 = p2 + 1;
    }
    return p1;
}

/*
 * get the next path component (just after the next path separator).
 */
    char_u *
getnextcomp(fname)
    char_u *fname;
{
    while (*fname && !vim_ispathsep(*fname))
	++fname;
    if (*fname)
	++fname;
    return fname;
}

/*
 * Get a pointer to one character past the head of a path name.
 * Unix: after "/"; DOS: after "c:\"; Amiga: after "disk:/"; Mac: no head.
 * If there is no head, path is returned.
 */
    char_u *
get_past_head(path)
    char_u  *path;
{
    char_u  *retval;

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    /* may skip "c:" */
    if (isalpha(path[0]) && path[1] == ':')
	retval = path + 2;
    else
	retval = path;
#else
# if defined(AMIGA)
    /* may skip "label:" */
    retval = vim_strchr(path, ':');
    if (retval == NULL)
	retval = path;
# else	/* Unix */
    retval = path;
# endif
#endif

    while (vim_ispathsep(*retval))
	++retval;

    return retval;
}

/*
 * return TRUE if 'c' is a path separator.
 */
    int
vim_ispathsep(c)
    int c;
{
#ifdef UNIX
    return (c == '/');	    /* UNIX has ':' inside file names */
#else
# ifdef BACKSLASH_IN_FILENAME
    return (c == ':' || c == '/' || c == '\\');
# else
#  ifdef COLON_AS_PATHSEP
    return (c == ':');
#  else	/* Amiga */
    return (c == ':' || c == '/');
#  endif
# endif
#endif
}

/*
 * Concatenate file names fname1 and fname2 into allocated memory.
 * Only add a '/' or '\\' when 'sep' is TRUE and it is neccesary.
 */
    char_u  *
concat_fnames(fname1, fname2, sep)
    char_u  *fname1;
    char_u  *fname2;
    int	    sep;
{
    char_u  *dest;

    dest = alloc((unsigned)(STRLEN(fname1) + STRLEN(fname2) + 3));
    if (dest != NULL)
    {
	STRCPY(dest, fname1);
	if (sep && *dest && !vim_ispathsep(*(dest + STRLEN(dest) - 1)))
	    STRCAT(dest, PATHSEPSTR);
	STRCAT(dest, fname2);
    }
    return dest;
}

/*
 * FullName_save - Make an allocated copy of a full file name.
 * Returns NULL when out of memory.
 */
    char_u  *
FullName_save(fname, force)
    char_u	*fname;
    int		force;	    /* force expansion, even when it already looks
			       like a full path name */
{
    char_u	*buf;
    char_u	*new_fname = NULL;

    buf = alloc((unsigned)MAXPATHL);
    if (buf != NULL)
    {
	if (mch_FullName(fname, buf, MAXPATHL, force) != FAIL)
	    new_fname = vim_strsave(buf);
	else
	    new_fname = vim_strsave(fname);
	vim_free(buf);
    }
    return new_fname;
}

#ifdef CINDENT

/*
 * Functions for C-indenting.
 * Most of this originally comes from Eric Fischer.
 */
/*
 * Below "XXX" means that this function may unlock the current line.
 */

static int	cin_isdefault __ARGS((char_u *));
static char_u	*after_label __ARGS((char_u *l));
static int	get_indent_nolabel __ARGS((linenr_t lnum));
static int	skip_label __ARGS((linenr_t, char_u **pp, int ind_maxcomment));
static int	cin_ispreproc __ARGS((char_u *));
static int	cin_iscomment __ARGS((char_u *));
static int	commentorempty __ARGS((char_u *));
static int	cin_isterminated __ARGS((char_u *, int));
static int	cin_isfuncdecl __ARGS((char_u *));
static char_u	*skip_string __ARGS((char_u *p));
static int	cin_isif __ARGS((char_u *));
static int	cin_iselse __ARGS((char_u *));
static int	cin_isdo __ARGS((char_u *));
static int	cin_iswhileofdo __ARGS((char_u *, linenr_t, int));
static FPOS	*find_start_brace __ARGS((int));
static FPOS	*find_match_paren __ARGS((int, int));
static int	find_last_paren __ARGS((char_u *l));
static int	find_match __ARGS((int lookfor, linenr_t ourscope,
			int ind_maxparen, int ind_maxcomment));

/*
 * Recognize a label: "label:".
 * Note: curwin->w_cursor must be where we are looking for the label.
 */
    int
cin_islabel(ind_maxcomment)		/* XXX */
    int		ind_maxcomment;
{
    char_u	*s;

    s = skipwhite(ml_get_curline());

    /*
     * Exclude "default" from labels, since it should be indented
     * like a switch label.  Same for C++ scope declarations.
     */
    if (cin_isdefault(s))
	return FALSE;
    if (cin_isscopedecl(s))
	return FALSE;

    if (!vim_isIDc(*s))	    /* need at least one ID character */
	return FALSE;

    while (vim_isIDc(*s))
	s++;

    s = skipwhite(s);

    /* "::" is not a label, it's C++ */
    if (*s == ':' && s[1] != ':')
    {
	/*
	 * Only accept a label if the previous line is terminated or is a case
	 * label.
	 */
	FPOS	cursor_save;
	FPOS	*trypos;
	char_u	*line;

	cursor_save = curwin->w_cursor;
	while (curwin->w_cursor.lnum > 1)
	{
	    --curwin->w_cursor.lnum;

	    /*
	     * If we're in a comment now, skip to the start of the comment.
	     */
	    curwin->w_cursor.col = 0;
	    if ((trypos = find_start_comment(ind_maxcomment)) != NULL) /* XXX */
		curwin->w_cursor = *trypos;

	    line = ml_get_curline();
	    if (cin_ispreproc(line))	/* ignore #defines, #if, etc. */
		continue;
	    if (commentorempty(line))
		continue;

	    curwin->w_cursor = cursor_save;
	    if (cin_isterminated(line, TRUE) || cin_isscopedecl(line)
							  || cin_iscase(line))
		return TRUE;
	    return FALSE;
	}
	curwin->w_cursor = cursor_save;
	return TRUE;		/* label at start of file??? */
    }
    return FALSE;
}

/*
 * Recognize a switch label: "case .*:" or "default:".
 */
     int
cin_iscase(s)
    char_u *s;
{
    s = skipwhite(s);
    if (STRNCMP(s, "case", 4) == 0 && !vim_isIDc(s[4]))
    {
	for (s += 4; *s; ++s)
	    if (*s == ':')
	    {
		if (s[1] == ':')	/* skip over "::" for C++ */
		    ++s;
		else
		    return TRUE;
	    }
	return FALSE;
    }

    if (cin_isdefault(s))
	return TRUE;
    return FALSE;
}

/*
 * Recognize a "default" switch label.
 */
    static int
cin_isdefault(s)
    char_u  *s;
{
    return (STRNCMP(s, "default", 7) == 0 &&
	    *(s = skipwhite(s + 7)) == ':' &&
	    s[1] != ':');
}

/*
 * Recognize a "public/private/proctected" scope declaration label.
 */
    int
cin_isscopedecl(s)
    char_u	*s;
{
    int		i;

    s = skipwhite(s);
    if (STRNCMP(s, "public", 6) == 0)
	i = 6;
    else if (STRNCMP(s, "protected", 9) == 0)
	i = 9;
    else if (STRNCMP(s, "private", 7) == 0)
	i = 7;
    else
	return FALSE;
    return (*(s = skipwhite(s + i)) == ':' && s[1] != ':');
}

/*
 * Return a pointer to the first non-empty non-comment character after a ':'.
 * Return NULL if not found.
 *	  case 234:    a = b;
 *		       ^
 */
    static char_u *
after_label(l)
    char_u  *l;
{
    for ( ; *l; ++l)
	if (*l == ':')
	{
	    if (l[1] == ':')	    /* skip over "::" for C++ */
		++l;
	    else
		break;
	}
    if (*l == NUL)
	return NULL;
    l = skipwhite(l + 1);
    if (commentorempty(l))
	return NULL;
    return l;
}

/*
 * Get indent of line "lnum", skipping a label.
 * Return 0 if there is nothing after the label.
 */
    static int
get_indent_nolabel(lnum)		/* XXX */
    linenr_t	lnum;
{
    char_u	*l;
    FPOS	fp;
    colnr_t	col;
    char_u	*p;

    l = ml_get(lnum);
    p = after_label(l);
    if (p == NULL)
	return 0;

    fp.col = p - l;
    fp.lnum = lnum;
    getvcol(curwin, &fp, &col, NULL, NULL);
    return (int)col;
}

/*
 * Find indent for line "lnum", ignoring any case or jump label.
 * Also return a pointer to the text (after the label).
 *   label:	if (asdf && asdfasdf)
 *		^
 */
    static int
skip_label(lnum, pp, ind_maxcomment)
    linenr_t	lnum;
    char_u	**pp;
    int		ind_maxcomment;
{
    char_u	*l;
    int		amount;
    FPOS	cursor_save;

    cursor_save = curwin->w_cursor;
    curwin->w_cursor.lnum = lnum;
    l = ml_get_curline();
				    /* XXX */
    if (cin_iscase(l) || cin_isscopedecl(l) || cin_islabel(ind_maxcomment))
    {
	amount = get_indent_nolabel(lnum);
	l = after_label(ml_get_curline());
	if (l == NULL)		/* just in case */
	    l = ml_get_curline();
    }
    else
    {
	amount = get_indent();
	l = ml_get_curline();
    }
    *pp = l;

    curwin->w_cursor = cursor_save;
    return amount;
}

/*
 * Recognize a preprocessor statement: Any line that starts with '#'.
 */
    static int
cin_ispreproc(s)
    char_u *s;
{
    s = skipwhite(s);
    if (*s == '#')
	return TRUE;
    return 0;
}

/*
 * Recognize the start of a C or C++ comment.
 */
    static int
cin_iscomment(p)
    char_u  *p;
{
    return (p[0] == '/' && (p[1] == '*' || p[1] == '/'));
}

/*
 * Recognize an empty or comment line.
 */
    static int
commentorempty(s)
    char_u *s;
{
    s = skipwhite(s);
    if (*s == NUL || cin_iscomment(s))
	return TRUE;
    return FALSE;
}

/*
 * Recognize a line that starts with '{' or '}', or ends with ';', '{' or '}'.
 * Don't consider "} else" a terminated line.
 * Also consider a line terminated if it ends in ','.  This is not 100%
 * correct, but this mostly means we are in initializations and then it's OK.
 */
    static int
cin_isterminated(s, incl_open)
    char_u	*s;
    int		incl_open;	/* include '{' at the end as terminator */
{
    s = skipwhite(s);

    if (*s == '{' || (*s == '}' && !cin_iselse(s)))
	return TRUE;

    while (*s)
    {
	if (cin_iscomment(s))	/* at start of comment ignore rest of line */
	    return FALSE;
	s = skip_string(s);
	if ((*s == ';' || (incl_open && *s == '{') || *s == '}' || *s == ',')
						     && commentorempty(s + 1))
	    return TRUE;
	s++;
    }
    return FALSE;
}

/*
 * Recognize the basic picture of a function declaration -- it needs to
 * have an open paren somewhere and a close paren at the end of the line and
 * no semicolons anywhere.
 */
    static int
cin_isfuncdecl(s)
    char_u *s;
{
    while (*s && *s != '(' && *s != ';')
	if (cin_iscomment(s++))
	    return FALSE;	    /* comment before () ??? */
    if (*s != '(')
	return FALSE;		    /* ';' before any () or no '(' */

    while (*s && *s != ';')
    {
	if (*s == ')' && commentorempty(s + 1))
	    return TRUE;
	if (cin_iscomment(s++))
	    return FALSE;	    /* comment between ( and ) ??? */
    }
    return FALSE;
}

/*
 * Skip over a "string" and a 'c' character.
 */
    static char_u *
skip_string(p)
    char_u  *p;
{
    int	    i;

    /*
     * We loop, because strings may be concatenated: "date""time".
     */
    for ( ; ; ++p)
    {
	if (p[0] == '\'')		    /* 'c' or '\n' or '\000' */
	{
	    if (!p[1])			    /* ' at end of line */
		break;
	    i = 2;
	    if (p[1] == '\\')		    /* '\n' or '\000' */
	    {
		++i;
		while (isdigit(p[i - 1]))   /* '\000' */
		    ++i;
	    }
	    if (p[i] == '\'')		    /* check for trailing ' */
	    {
		p += i;
		continue;
	    }
	}
	else if (p[0] == '"')		    /* start of string */
	{
	    for (++p; p[0]; ++p)
	    {
		if (p[0] == '\\' && p[1])
		    ++p;
		else if (p[0] == '"')	    /* end of string */
		    break;
	    }
	    continue;
	}
	break;				    /* no string found */
    }
    if (!*p)
	--p;				    /* backup from NUL */
    return p;
}

    static int
cin_isif(p)
    char_u  *p;
{
    return (STRNCMP(p, "if", 2) == 0 && !vim_isIDc(p[2]));
}

    static int
cin_iselse(p)
    char_u  *p;
{
    if (*p == '}')	    /* accept "} else" */
	p = skipwhite(p + 1);
    return (STRNCMP(p, "else", 4) == 0 && !vim_isIDc(p[4]));
}

    static int
cin_isdo(p)
    char_u  *p;
{
    return (STRNCMP(p, "do", 2) == 0 && !vim_isIDc(p[2]));
}

/*
 * Check if this is a "while" that should have a matching "do".
 * We only accept a "while (condition) ;", with only white space between the
 * ')' and ';'. The condition may be spread over several lines.
 */
    static int
cin_iswhileofdo(p, lnum, ind_maxparen)	    /* XXX */
    char_u	*p;
    linenr_t	lnum;
    int		ind_maxparen;
{
    FPOS	cursor_save;
    FPOS	*trypos;
    int		retval = FALSE;

    p = skipwhite(p);
    if (*p == '}')		/* accept "} while (cond);" */
	p = skipwhite(p + 1);
    if (STRNCMP(p, "while", 5) == 0 && !vim_isIDc(p[5]))
    {
	cursor_save = curwin->w_cursor;
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = 0;
	p = ml_get_curline();
	while (*p && *p != 'w')	/* skip any '}', until the 'w' of the "while" */
	{
	    ++p;
	    ++curwin->w_cursor.col;
	}
	if ((trypos = findmatchlimit(NULL, 0, 0, ind_maxparen)) != NULL)
	{
	    p = ml_get_pos(trypos) + 1;
	    p = skipwhite(p);
	    if (*p == ';')
		retval = TRUE;
	}
	curwin->w_cursor = cursor_save;
    }
    return retval;
}

/*
 * Find the start of a comment, not knowing if we are in a comment right now.
 * Search starts at w_cursor.lnum and goes backwards.
 */
    FPOS *
find_start_comment(ind_maxcomment)	    /* XXX */
    int		ind_maxcomment;
{
    FPOS	*pos;
    char_u	*line;
    char_u	*p;

    if ((pos = findmatchlimit(NULL, '*', FM_BACKWARD, ind_maxcomment)) == NULL)
	return NULL;

    /*
     * Check if the comment start we found is inside a string.
     */
    line = ml_get(pos->lnum);
    for (p = line; *p && (unsigned)(p - line) < pos->col; ++p)
	p = skip_string(p);
    if ((unsigned)(p - line) > pos->col)
	return NULL;
    return pos;
}

/*
 * Find the '{' at the start of the block we are in.
 * Return NULL of no match found.
 * Ignore a '{' that is in a comment, makes indenting the next three lines
 * work. */
/* foo()    */
/* {	    */
/* }	    */

    static FPOS *
find_start_brace(ind_maxcomment)	    /* XXX */
    int		ind_maxcomment;
{
    FPOS	cursor_save;
    FPOS	*trypos;
    FPOS	*pos;
    static FPOS	pos_copy;

    cursor_save = curwin->w_cursor;
    while ((trypos = findmatchlimit(NULL, '{', FM_BLOCKSTOP, 0)) != NULL)
    {
	pos_copy = *trypos;	/* copy FPOS, next findmatch will change it */
	trypos = &pos_copy;
	curwin->w_cursor = *trypos;
	pos = NULL;
	if (!cin_iscomment(skipwhite(ml_get(trypos->lnum))) &&
		 (pos = find_start_comment(ind_maxcomment)) == NULL) /* XXX */
	    break;
	if (pos != NULL)
	    curwin->w_cursor.lnum = pos->lnum;
    }
    curwin->w_cursor = cursor_save;
    return trypos;
}

/*
 * Find the matching '(', failing if it is in a comment.
 * Return NULL of no match found.
 */
    static FPOS *
find_match_paren(ind_maxparen, ind_maxcomment)	    /* XXX */
    int		ind_maxparen;
    int		ind_maxcomment;
{
    FPOS	cursor_save;
    FPOS	*trypos;
    static FPOS	pos_copy;

    cursor_save = curwin->w_cursor;
    if ((trypos = findmatchlimit(NULL, '(', 0, ind_maxparen)) != NULL)
    {
	if (cin_iscomment(skipwhite(ml_get(trypos->lnum))))
	    trypos = NULL;
	else
	{
	    pos_copy = *trypos;	    /* copy trypos, findmatch will change it */
	    trypos = &pos_copy;
	    curwin->w_cursor = *trypos;
	    if (find_start_comment(ind_maxcomment) != NULL) /* XXX */
		trypos = NULL;
	}
    }
    curwin->w_cursor = cursor_save;
    return trypos;
}

/*
 * Set w_cursor.col to the column number of the last ')' in line "l".
 */
    static int
find_last_paren(l)
    char_u *l;
{
    int	    i;
    int	    retval = FALSE;

    curwin->w_cursor.col = 0;		    /* default is start of line */

    for (i = 0; l[i]; i++)
    {
	i = skip_string(l + i) - l;	    /* ignore parens in quotes */
	if (l[i] == ')')
	{
	    curwin->w_cursor.col = i;
	    retval = TRUE;
	}
    }
    return retval;
}

    int
get_c_indent()
{
    /*
     * spaces from a block's opening brace the prevailing indent for that
     * block should be
     */
    int ind_level = curbuf->b_p_sw;

    /*
     * spaces from the edge of the line an open brace that's at the end of a
     * line is imagined to be.
     */
    int ind_open_imag = 0;

    /*
     * spaces from the prevailing indent for a line that is not precededof by
     * an opening brace.
     */
    int ind_no_brace = 0;

    /*
     * column where the first { of a function should be located
     */
    int ind_first_open = 0;

    /*
     * spaces from the prevailing indent a leftmost open brace should be
     * located
     */
    int ind_open_extra = 0;

    /*
     * spaces from the matching open brace (real location for one at the left
     * edge; imaginary location from one that ends a line) the matching close
     * brace should be located
     */
    int ind_close_extra = 0;

    /*
     * spaces from the edge of the line an open brace sitting in the leftmost
     * column is imagined to be
     */
    int ind_open_left_imag = 0;

    /*
     * spaces from the switch() indent a "case xx" label should be located
     */
    int ind_case = curbuf->b_p_sw;

    /*
     * spaces from the "case xx:" code after a switch() should be located
     */
    int ind_case_code = curbuf->b_p_sw;

    /*
     * spaces from the class declaration indent a scope declaration label
     * should be located
     */
    int ind_scopedecl = curbuf->b_p_sw;

    /*
     * spaces from the scope declaration label code should be located
     */
    int ind_scopedecl_code = curbuf->b_p_sw;

    /*
     * amount K&R-style parameters should be indented
     */
    int ind_param = curbuf->b_p_sw;

    /*
     * amount a function type spec should be indented
     */
    int ind_func_type = curbuf->b_p_sw;

    /*
     * additional spaces beyond the prevailing indent a continuation line
     * should be located
     */
    int ind_continuation = curbuf->b_p_sw;

    /*
     * spaces from the indent of the line with an unclosed parentheses
     */
    int ind_unclosed = curbuf->b_p_sw * 2;

    /*
     * spaces from the comment opener when there is nothing after it.
     */
    int ind_in_comment = 3;

    /*
     * max lines to search for an open paren
     */
    int ind_maxparen = 20;

    /*
     * max lines to search for an open comment
     */
    int ind_maxcomment = 30;

    FPOS	cur_curpos;
    int		amount;
    int		scope_amount;
    int		cur_amount;
    colnr_t	col;
    char_u	*theline;
    char_u	*linecopy;
    FPOS	*trypos;
    FPOS	our_paren_pos;
    char_u	*start;
    int		start_brace;
#define BRACE_IN_COL0		1	    /* '{' is in comumn 0 */
#define BRACE_AT_START		2	    /* '{' is at start of line */
#define BRACE_AT_END		3	    /* '{' is at end of line */
    linenr_t	ourscope;
    char_u	*l;
    char_u	*look;
    int		lookfor;
#define LOOKFOR_IF		1
#define LOOKFOR_DO		2
#define LOOKFOR_CASE		3
#define LOOKFOR_ANY		4
#define LOOKFOR_TERM		5
#define LOOKFOR_UNTERM		6
#define LOOKFOR_SCOPEDECL	7
    int		whilelevel;
    linenr_t	lnum;
    char_u	*options;
    int		fraction = 0;	    /* init for GCC */
    int		divider;
    int		n;
    int		iscase;

    for (options = curbuf->b_p_cino; *options; )
    {
	l = options++;
	if (*options == '-')
	    ++options;
	n = getdigits(&options);
	divider = 0;
	if (*options == '.')	    /* ".5s" means a fraction */
	{
	    fraction = atol((char *)++options);
	    while (isdigit(*options))
	    {
		++options;
		if (divider)
		    divider *= 10;
		else
		    divider = 10;
	    }
	}
	if (*options == 's')	    /* "2s" means two times 'shiftwidth' */
	{
	    if (n == 0 && fraction == 0)
		n = curbuf->b_p_sw;	/* just "s" is one 'shiftwidth' */
	    else
	    {
		n *= curbuf->b_p_sw;
		if (divider)
		    n += (curbuf->b_p_sw * fraction + divider / 2) / divider;
	    }
	    ++options;
	}
	if (l[1] == '-')
	    n = -n;
	switch (*l)
	{
	    case '>': ind_level = n; break;
	    case 'e': ind_open_imag = n; break;
	    case 'n': ind_no_brace = n; break;
	    case 'f': ind_first_open = n; break;
	    case '{': ind_open_extra = n; break;
	    case '}': ind_close_extra = n; break;
	    case '^': ind_open_left_imag = n; break;
	    case ':': ind_case = n; break;
	    case '=': ind_case_code = n; break;
	    case 'p': ind_param = n; break;
	    case 't': ind_func_type = n; break;
	    case 'c': ind_in_comment = n; break;
	    case '+': ind_continuation = n; break;
	    case '(': ind_unclosed = n; break;
	    case ')': ind_maxparen = n; break;
	    case '*': ind_maxcomment = n; break;
	    case 'g': ind_scopedecl = n; break;
	    case 'h': ind_scopedecl_code = n; break;
	}
    }

    /* remember where the cursor was when we started */

    cur_curpos = curwin->w_cursor;

    /* get the current contents of the line.
     * This is required, because onle the most recent line obtained with
     * ml_get is valid! */

    linecopy = vim_strsave(ml_get(cur_curpos.lnum));
    if (linecopy == NULL)
	return 0;

    /*
     * In insert mode and the cursor is on a ')' trunctate the line at the
     * cursor position.  We don't want to line up with the matching '(' when
     * inserting new stuff.
     */
    if ((State & INSERT) && linecopy[curwin->w_cursor.col] == ')')
	linecopy[curwin->w_cursor.col] = NUL;

    theline = skipwhite(linecopy);

    /* move the cursor to the start of the line */

    curwin->w_cursor.col = 0;

    /*
     * #defines and so on always go at the left when included in 'cinkeys'.
     */
    if (*theline == '#' && (*linecopy == '#' || in_cinkeys('#', ' ', TRUE)))
    {
	amount = 0;
    }

    /*
     * Is it a non-case label?	Then that goes at the left margin too.
     */
    else if (cin_islabel(ind_maxcomment))	    /* XXX */
    {
	amount = 0;
    }

    /*
     * If we're inside a comment and not looking at the start of the
     * comment...
     */
    else if (!cin_iscomment(theline) &&
	      (trypos = find_start_comment(ind_maxcomment)) != NULL) /* XXX */
    {

	/* find how indented the line beginning the comment is */
	getvcol(curwin, trypos, &col, NULL, NULL);
	amount = col;

	/* if our line starts with an asterisk, line up with the
	 * asterisk in the comment opener; otherwise, line up
	 * with the first character of the comment text.
	 */
	if (theline[0] == '*')
	{
	    amount += 1;
	}
	else
	{
	    /*
	     * If we are more than one line away from the comment opener, take
	     * the indent of the previous non-empty line.
	     * If we are just below the comment opener and there are any
	     * white characters after it line up with the text after it.
	     * up with them; otherwise, just use a single space.
	     */
	    amount = -1;
	    for (lnum = cur_curpos.lnum - 1; lnum > trypos->lnum; --lnum)
	    {
		if (linewhite(lnum))		    /* skip blank lines */
		    continue;
		amount = get_indent_lnum(lnum);	    /* XXX */
		break;
	    }
	    if (amount == -1)			    /* use the comment opener */
	    {
		start = ml_get(trypos->lnum);
		look = start + trypos->col + 2;	    /* skip / and * */
		if (*look)			    /* if something after it */
		    trypos->col = skipwhite(look) - start;
		getvcol(curwin, trypos, &col, NULL, NULL);
		amount = col;
		if (!*look)
		    amount += ind_in_comment;
	    }
	}
    }

    /*
     * Are we inside parentheses?
     */						    /* XXX */
    else if ((trypos = find_match_paren(ind_maxparen, ind_maxcomment)) != NULL)
    {
	/*
	 * If the matching paren is more than one line away, use the indent of
	 * a previous non-empty line that matches the same paren.
	 */
	amount = -1;
	our_paren_pos = *trypos;
	if (theline[0] != ')')
	{
	    for (lnum = cur_curpos.lnum - 1; lnum > our_paren_pos.lnum; --lnum)
	    {
		l = skipwhite(ml_get(lnum));
		if (commentorempty(l))	    /* skip comment lines */
		    continue;
		if (cin_ispreproc(l))	    /* ignore #defines, #if, etc. */
		    continue;
		curwin->w_cursor.lnum = lnum;

		/* Skip a comment. XXX */
		if ((trypos = find_start_comment(ind_maxcomment)) != NULL)
		{
		    lnum = trypos->lnum + 1;
		    continue;
		}

		/* XXX */
		if ((trypos = find_match_paren(ind_maxparen,
						   ind_maxcomment)) != NULL &&
					 trypos->lnum == our_paren_pos.lnum &&
					     trypos->col == our_paren_pos.col)
		{
		    amount = get_indent_lnum(lnum);	/* XXX */
		    break;
		}
	    }
	}

	/*
	 * Line up with line where the matching paren is.
	 * If the line starts with a '(' or the indent for unclosed
	 * parentheses is zero, line up with the unclosed parentheses.
	 */
	if (amount == -1)
	{
	    amount = skip_label(our_paren_pos.lnum, &look, ind_maxcomment);
	    if (theline[0] == ')' || ind_unclosed == 0 ||
						      *skipwhite(look) == '(')
	    {

		/*
		 * If we're looking at a close paren, line up right there;
		 * otherwise, line up with the next non-white character.
		 */
		if (theline[0] != ')')
		{
		    col = our_paren_pos.col + 1;
		    look = ml_get(our_paren_pos.lnum);
		    while (vim_iswhite(look[col]))
			col++;
		    if (look[col] != NUL)	/* In case of trailing space */
			our_paren_pos.col = col;
		    else
			our_paren_pos.col++;
		}

		/*
		 * Find how indented the paren is, or the character after it if
		 * we did the above "if".
		 */
		getvcol(curwin, &our_paren_pos, &col, NULL, NULL);
		amount = col;
	    }
	    else
		amount += ind_unclosed;
	}
    }

    /*
     * Are we at least inside braces, then?
     */
    else if ((trypos = find_start_brace(ind_maxcomment)) != NULL) /* XXX */
    {
	ourscope = trypos->lnum;
	start = ml_get(ourscope);

	/*
	 * Now figure out how indented the line is in general.
	 * If the brace was at the start of the line, we use that;
	 * otherwise, check out the indentation of the line as
	 * a whole and then add the "imaginary indent" to that.
	 */
	look = skipwhite(start);
	if (*look == '{')
	{
	    getvcol(curwin, trypos, &col, NULL, NULL);
	    amount = col;
	    if (*start == '{')
		start_brace = BRACE_IN_COL0;
	    else
		start_brace = BRACE_AT_START;
	}
	else
	{
	    /*
	     * that opening brace might have been on a continuation
	     * line.  if so, find the start of the line.
	     */
	    curwin->w_cursor.lnum = ourscope;

	    /*
	     * position the cursor over the rightmost paren, so that
	     * matching it will take us back to the start of the line.
	     */
	    lnum = ourscope;
	    if (find_last_paren(start) &&
		    (trypos = find_match_paren(ind_maxparen,
						     ind_maxcomment)) != NULL)
		lnum = trypos->lnum;

	    /*
	     * It could have been something like
	     *	   case 1: if (asdf &&
	     *			ldfd) {
	     *		    }
	     */
	    amount = skip_label(lnum, &l, ind_maxcomment);

	    start_brace = BRACE_AT_END;
	}

	/*
	 * if we're looking at a closing brace, that's where
	 * we want to be.  otherwise, add the amount of room
	 * that an indent is supposed to be.
	 */
	if (theline[0] == '}')
	{
	    /*
	     * they may want closing braces to line up with something
	     * other than the open brace.  indulge them, if so.
	     */
	    amount += ind_close_extra;
	}
	else
	{
	    /*
	     * If we're looking at an "else", try to find an "if"
	     * to match it with.
	     * If we're looking at a "while", try to find a "do"
	     * to match it with.
	     */
	    lookfor = 0;
	    if (cin_iselse(theline))
		lookfor = LOOKFOR_IF;
	    else if (cin_iswhileofdo(theline, cur_curpos.lnum, ind_maxparen))
								    /* XXX */
		lookfor = LOOKFOR_DO;
	    if (lookfor)
	    {
		curwin->w_cursor.lnum = cur_curpos.lnum;
		if (find_match(lookfor, ourscope, ind_maxparen,
							ind_maxcomment) == OK)
		{
		    amount = get_indent();	/* XXX */
		    goto theend;
		}
	    }

	    /*
	     * We get here if we are not on an "while-of-do" or "else" (or
	     * failed to find a matching "if").
	     * Search backwards for something to line up with.
	     * First set amount for when we don't find anything.
	     */

	    /*
	     * if the '{' is  _really_ at the left margin, use the imaginary
	     * location of a left-margin brace.  Otherwise, correct the
	     * location for ind_open_extra.
	     */

	    if (start_brace == BRACE_IN_COL0)	    /* '{' is in column 0 */
	    {
		amount = ind_open_left_imag;
	    }
	    else
	    {
		if (start_brace == BRACE_AT_END)    /* '{' is at end of line */
		    amount += ind_open_imag;
		else
		{
		    amount -= ind_open_extra;
		    if (amount < 0)
			amount = 0;
		}
	    }

	    if (cin_iscase(theline))	/* it's a switch() label */
	    {
		lookfor = LOOKFOR_CASE;	/* find a previous switch() label */
		amount += ind_case;
	    }
	    else if (cin_isscopedecl(theline))	/* private:, ... */
	    {
		lookfor = LOOKFOR_SCOPEDECL;	/* class decl is this block */
		amount += ind_scopedecl;
	    }
	    else
	    {
		lookfor = LOOKFOR_ANY;
		amount += ind_level;	/* ind_level from start of block */
	    }
	    scope_amount = amount;
	    whilelevel = 0;

	    /*
	     * Search backwards.  If we find something we recognize, line up
	     * with that.
	     *
	     * if we're looking at an open brace, indent
	     * the usual amount relative to the conditional
	     * that opens the block.
	     */
	    curwin->w_cursor = cur_curpos;
	    for (;;)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		/*
		 * If we went all the way back to the start of our scope, line
		 * up with it.
		 */
		if (curwin->w_cursor.lnum <= ourscope)
		{
		    if (lookfor == LOOKFOR_UNTERM)
			amount += ind_continuation;
		    else if (lookfor != LOOKFOR_TERM)
			amount = scope_amount;
		    break;
		}

		/*
		 * If we're in a comment now, skip to the start of the comment.
		 */					    /* XXX */
		if ((trypos = find_start_comment(ind_maxcomment)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    continue;
		}

		l = ml_get_curline();

		/*
		 * If this is a switch() label, may line up relative to that.
		 * if this is a C++ scope declaration, do the same.
		 */
		iscase = cin_iscase(l);
		if (iscase || cin_isscopedecl(l))
		{
		    /*
		     *	case xx:
		     *	    c = 99 +	    <- this indent plus continuation
		     *->	   here;
		     */
		    if (lookfor == LOOKFOR_UNTERM)
		    {
			amount += ind_continuation;
			break;
		    }

		    /*
		     *	case xx:	<- line up with this case
		     *	    x = 333;
		     *	case yy:
		     */
		    if (       (iscase && lookfor == LOOKFOR_CASE)
			    || (!iscase && lookfor == LOOKFOR_SCOPEDECL))
		    {
			/*
			 * Check that this case label is not for another
			 * switch()
			 */				    /* XXX */
			if ((trypos = find_start_brace(ind_maxcomment)) ==
					     NULL || trypos->lnum == ourscope)
			{
			    amount = get_indent();	/* XXX */
			    break;
			}
			continue;
		    }

		    n = get_indent_nolabel(curwin->w_cursor.lnum);  /* XXX */

		    /*
		     *	 case xx: if (cond)	    <- line up with this if
		     *		      y = y + 1;
		     * ->	  s = 99;
		     *
		     *	 case xx:
		     *	     if (cond)		<- line up with this line
		     *		 y = y + 1;
		     * ->    s = 99;
		     */
		    if (lookfor == LOOKFOR_TERM)
		    {
			if (n)
			    amount = n;
			break;
		    }

		    /*
		     *	 case xx: x = x + 1;	    <- line up with this x
		     * ->	  y = y + 1;
		     *
		     *	 case xx: if (cond)	    <- line up with this if
		     * ->	       y = y + 1;
		     */
		    if (n)
		    {
			amount = n;
			l = after_label(ml_get_curline());
			if (l != NULL && cin_is_cinword(l))
			    amount += ind_level + ind_no_brace;
			break;
		    }

		    /*
		     * Try to get the indent of a statement before the switch
		     * label.  If nothing is found, line up relative to the
		     * switch label.
		     *	    break;		<- may line up with this line
		     *	 case xx:
		     * ->   y = 1;
		     */
		    scope_amount = get_indent() + (iscase    /* XXX */
					? ind_case_code : ind_scopedecl_code);
		    lookfor = LOOKFOR_ANY;
		    continue;
		}

		/*
		 * Looking for a switch() label or C++ scope declaration,
		 * ignore other lines.
		 */
		if (lookfor == LOOKFOR_CASE || lookfor == LOOKFOR_SCOPEDECL)
		    continue;

		/*
		 * Ignore jump labels with nothing after them.
		 */
		if (cin_islabel(ind_maxcomment))
		{
		    l = after_label(ml_get_curline());
		    if (l == NULL || commentorempty(l))
			continue;
		}

		/*
		 * Ignore #defines, #if, etc.
		 * Ignore comment and empty lines.
		 * (need to get the line again, cin_islabel() may have
		 * unlocked it)
		 */
		l = ml_get_curline();
		if (cin_ispreproc(l) || commentorempty(l))
		    continue;

		/*
		 * What happens next depends on the line being terminated.
		 */
		if (!cin_isterminated(l, FALSE))
		{
		    /*
		     * if we're in the middle of a paren thing,
		     * go back to the line that starts it so
		     * we can get the right prevailing indent
		     *	   if ( foo &&
		     *		    bar )
		     */
		    /*
		     * position the cursor over the rightmost paren, so that
		     * matching it will take us back to the start of the line.
		     */
		    (void)find_last_paren(l);
		    if ((trypos = find_match_paren(ind_maxparen,
						     ind_maxcomment)) != NULL)
		    {
			/*
			 * Check if we are on a case label now.  This is
			 * handled above.
			 *     case xx:  if ( asdf &&
			 *			asdf)
			 */
			curwin->w_cursor.lnum = trypos->lnum;
			l = ml_get_curline();
			if (cin_iscase(l) || cin_isscopedecl(l))
			{
			    ++curwin->w_cursor.lnum;
			    continue;
			}
		    }

		    /*
		     * Get indent and pointer to text for current line,
		     * ignoring any jump label.	    XXX
		     */
		    cur_amount = skip_label(curwin->w_cursor.lnum,
							  &l, ind_maxcomment);

		    /*
		     * If this is just above the line we are indenting, and it
		     * starts with a '{', line it up with this line.
		     *		while (not)
		     * ->	{
		     *		}
		     */
		    if (lookfor != LOOKFOR_TERM && theline[0] == '{')
		    {
			amount = cur_amount + ind_open_extra;
			break;
		    }

		    /*
		     * Check if we are after an "if", "while", etc.
		     * Also allow "} else".
		     */
		    if (cin_is_cinword(l) || cin_iselse(l))
		    {
			/*
			 * Found an unterminated line after an if (), line up
			 * with the last one.
			 *   if (cond)
			 *	    100 +
			 * ->		here;
			 */
			if (lookfor == LOOKFOR_UNTERM)
			{
			    amount += ind_continuation;
			    break;
			}

			/*
			 * If this is just above the line we are indenting, we
			 * are finished.
			 *	    while (not)
			 * ->		here;
			 * Otherwise this indent can be used when the line
			 * before this is terminated.
			 *	yyy;
			 *	if (stat)
			 *	    while (not)
			 *		xxx;
			 * ->	here;
			 */
			amount = cur_amount;
			if (lookfor != LOOKFOR_TERM)
			{
			    amount += ind_level + ind_no_brace;
			    break;
			}

			/*
			 * Special trick: when expecting the while () after a
			 * do, line up with the while()
			 *     do
			 *	    x = 1;
			 * ->  here
			 */
			l = skipwhite(ml_get_curline());
			if (cin_isdo(l))
			{
			    if (whilelevel == 0)
				break;
			    --whilelevel;
			}

			/*
			 * When searching for a terminated line, don't use the
			 * one between the "if" and the "else".
			 * Need to use the scope of this "else".  XXX
			 */
			if (cin_iselse(l) &&
				((trypos = find_start_brace(ind_maxcomment))
								    == NULL ||
				find_match(LOOKFOR_IF, trypos->lnum,
					ind_maxparen, ind_maxcomment) == FAIL))
			    break;
		    }

		    /*
		     * If we're below an unterminated line that is not an
		     * "if" or something, we may line up with this line or
		     * add someting for a continuation line, depending on
		     * the line before this one.
		     */
		    else
		    {
			/*
			 * Found two unterminated lines on a row, line up with
			 * the last one.
			 *   c = 99 +
			 *	    100 +
			 * ->	    here;
			 */
			if (lookfor == LOOKFOR_UNTERM)
			    break;

			/*
			 * Found first unterminated line on a row, may line up
			 * with this line, remember its indent
			 *	    100 +
			 * ->	    here;
			 */
			amount = cur_amount;
			if (lookfor != LOOKFOR_TERM)
			    lookfor = LOOKFOR_UNTERM;
		    }
		}

		/*
		 * Check if we are after a while (cond);
		 * If so: Ignore until the matching "do".
		 */
							/* XXX */
		else if (cin_iswhileofdo(l,
					 curwin->w_cursor.lnum, ind_maxparen))
		{
		    /*
		     * Found an unterminated line after a while ();, line up
		     * with the last one.
		     *	    while (cond);
		     *	    100 +		<- line up with this one
		     * ->	    here;
		     */
		    if (lookfor == LOOKFOR_UNTERM)
		    {
			amount += ind_continuation;
			break;
		    }

		    if (whilelevel == 0)
		    {
			lookfor = LOOKFOR_TERM;
			amount = get_indent();	    /* XXX */
			if (theline[0] == '{')
			    amount += ind_open_extra;
		    }
		    ++whilelevel;
		}

		/*
		 * We are after a "normal" statement.
		 * If we had another statement we can stop now and use the
		 * indent of that other statement.
		 * Otherwise the indent of the current statement may be used,
		 * search backwards for the next "normal" statement.
		 */
		else
		{
		    /*
		     * Handle "do {" line.
		     */
		    if (whilelevel > 0)
		    {
			l = skipwhite(ml_get_curline());
			if (cin_isdo(l))
			{
			    amount = get_indent();	/* XXX */
			    --whilelevel;
			    continue;
			}
		    }

		    /*
		     * Found a terminated line above an unterminated line. Add
		     * the amount for a continuation line.
		     *	 x = 1;
		     *	 y = foo +
		     * ->	here;
		     */
		    if (lookfor == LOOKFOR_UNTERM)
		    {
			amount += ind_continuation;
			break;
		    }

		    /*
		     * Found a terminated line above a terminated line or "if"
		     * etc. line. Use the amount of the line below us.
		     *	 x = 1;				x = 1;
		     *	 if (asdf)		    y = 2;
		     *	     while (asdf)	  ->here;
		     *		here;
		     * ->foo;
		     */
		    if (lookfor == LOOKFOR_TERM)
		    {
			if (whilelevel == 0)
			    break;
		    }

		    /*
		     * First line above the one we're indenting is terminated.
		     * To know what needs to be done look further backward for
		     * a terminated line.
		     */
		    else
		    {
			/*
			 * position the cursor over the rightmost paren, so
			 * that matching it will take us back to the start of
			 * the line.  Helps for:
			 *     func(asdr,
			 *	      asdfasdf);
			 *     here;
			 */
term_again:
			l = ml_get_curline();
			if (find_last_paren(l) &&
				(trypos = find_match_paren(ind_maxparen,
						     ind_maxcomment)) != NULL)
			{
			    /*
			     * Check if we are on a case label now.  This is
			     * handled above.
			     *	   case xx:  if ( asdf &&
			     *			    asdf)
			     */
			    curwin->w_cursor.lnum = trypos->lnum;
			    l = ml_get_curline();
			    if (cin_iscase(l) || cin_isscopedecl(l))
			    {
				++curwin->w_cursor.lnum;
				continue;
			    }
			}

			/*
			 * Get indent and pointer to text for current line,
			 * ignoring any jump label.
			 */
			amount = skip_label(curwin->w_cursor.lnum,
							  &l, ind_maxcomment);

			if (theline[0] == '{')
			    amount += ind_open_extra;
			lookfor = LOOKFOR_TERM;

			/*
			 * If we're at the end of a block, skip to the start of
			 * that block.
			 */
			curwin->w_cursor.col = 0;
			if (*skipwhite(l) == '}' &&
				   (trypos = find_start_brace(ind_maxcomment))
							    != NULL) /* XXX */
			{
			    curwin->w_cursor.lnum = trypos->lnum;
			    /* if not "else {" check for terminated again */
			    if (!cin_iselse(skipwhite(ml_get_curline())))
				goto term_again;
			    ++curwin->w_cursor.lnum;
			}
		    }
		}
	    }
	}
    }

    /*
     * ok -- we're not inside any sort of structure at all!
     *
     * this means we're at the top level, and everything should
     * basically just match where the previous line is, except
     * for the lines immediately following a function declaration,
     * which are K&R-style parameters and need to be indented.
     */
    else
    {
	/*
	 * if our line starts with an open brace, forget about any
	 * prevailing indent and make sure it looks like the start
	 * of a function
	 */

	if (theline[0] == '{')
	{
	    amount = ind_first_open;
	}

	/*
	 * If the NEXT line is a function declaration, the current
	 * line needs to be indented as a function type spec.
	 * Don't do this if the current line looks like a comment.
	 */
	else if (cur_curpos.lnum < curbuf->b_ml.ml_line_count
		&& !commentorempty(theline)
		&& cin_isfuncdecl(ml_get(cur_curpos.lnum + 1)))
	{
	    amount = ind_func_type;
	}
	else
	{
	    amount = 0;
	    curwin->w_cursor = cur_curpos;

	    /* search backwards until we find something we recognize */

	    while (curwin->w_cursor.lnum > 1)
	    {
		curwin->w_cursor.lnum--;
		curwin->w_cursor.col = 0;

		l = ml_get_curline();

		/*
		 * If we're in a comment now, skip to the start of the comment.
		 */						/* XXX */
		if ((trypos = find_start_comment(ind_maxcomment)) != NULL)
		{
		    curwin->w_cursor.lnum = trypos->lnum + 1;
		    continue;
		}

		/*
		 * If the line looks like a function declaration, and we're
		 * not in a comment, put it the left margin.
		 */
		if (cin_isfuncdecl(theline))
		    break;

		/*
		 * Skip preprocessor directives and blank lines.
		 */
		if (cin_ispreproc(l))
		    continue;

		if (commentorempty(l))
		    continue;

		/*
		 * If the PREVIOUS line is a function declaration, the current
		 * line (and the ones that follow) needs to be indented as
		 * parameters.
		 */
		if (cin_isfuncdecl(l))
		{
		    amount = ind_param;
		    break;
		}

		/*
		 * Doesn't look like anything interesting -- so just
		 * use the indent of this line.
		 *
		 * Position the cursor over the rightmost paren, so that
		 * matching it will take us back to the start of the line.
		 */
		find_last_paren(l);

		if ((trypos = find_match_paren(ind_maxparen,
						     ind_maxcomment)) != NULL)
		    curwin->w_cursor.lnum = trypos->lnum;
		amount = get_indent();	    /* XXX */
		break;
	    }
	}
    }

theend:
    /* put the cursor back where it belongs */
    curwin->w_cursor = cur_curpos;

    vim_free(linecopy);

    if (amount < 0)
	return 0;
    return amount;
}

    static int
find_match(lookfor, ourscope, ind_maxparen, ind_maxcomment)
    int		lookfor;
    linenr_t	ourscope;
    int		ind_maxparen;
    int		ind_maxcomment;
{
    char_u	*look;
    FPOS	*theirscope;
    char_u	*mightbeif;
    int		elselevel;
    int		whilelevel;

    if (lookfor == LOOKFOR_IF)
    {
	elselevel = 1;
	whilelevel = 0;
    }
    else
    {
	elselevel = 0;
	whilelevel = 1;
    }

    curwin->w_cursor.col = 0;

    while (curwin->w_cursor.lnum > ourscope + 1)
    {
	curwin->w_cursor.lnum--;
	curwin->w_cursor.col = 0;

	look = skipwhite(ml_get_curline());
	if (cin_iselse(look)
		|| cin_isif(look)
		|| cin_isdo(look)			    /* XXX */
		|| cin_iswhileofdo(look, curwin->w_cursor.lnum, ind_maxparen))
	{
	    /*
	     * if we've gone outside the braces entirely,
	     * we must be out of scope...
	     */
	    theirscope = find_start_brace(ind_maxcomment);  /* XXX */
	    if (theirscope == NULL)
		break;

	    /*
	     * and if the brace enclosing this is further
	     * back than the one enclosing the else, we're
	     * out of luck too.
	     */
	    if (theirscope->lnum < ourscope)
		break;

	    /*
	     * and if they're enclosed in a *deeper* brace,
	     * then we can ignore it because it's in a
	     * different scope...
	     */
	    if (theirscope->lnum > ourscope)
		continue;

	    /*
	     * if it was an "else" (that's not an "else if")
	     * then we need to go back to another if, so
	     * increment elselevel
	     */
	    look = skipwhite(ml_get_curline());
	    if (cin_iselse(look))
	    {
		mightbeif = skipwhite(look + 4);
		if (!cin_isif(mightbeif))
		    ++elselevel;
		continue;
	    }

	    /*
	     * if it was a "while" then we need to go back to
	     * another "do", so increment whilelevel.  XXX
	     */
	    if (cin_iswhileofdo(look, curwin->w_cursor.lnum, ind_maxparen))
	    {
		++whilelevel;
		continue;
	    }

	    /* If it's an "if" decrement elselevel */
	    look = skipwhite(ml_get_curline());
	    if (cin_isif(look))
	    {
		elselevel--;
		/*
		 * When looking for an "if" ignore "while"s that
		 * get in the way.
		 */
		if (elselevel == 0 && lookfor == LOOKFOR_IF)
		    whilelevel = 0;
	    }

	    /* If it's a "do" decrement whilelevel */
	    if (cin_isdo(look))
		whilelevel--;

	    /*
	     * if we've used up all the elses, then
	     * this must be the if that we want!
	     * match the indent level of that if.
	     */
	    if (elselevel <= 0 && whilelevel <= 0)
	    {
		return OK;
	    }
	}
    }
    return FAIL;
}

#endif /* CINDENT */

#ifdef LISPINDENT
    int
get_lisp_indent()
{
    FPOS	*pos, realpos;
    long	amount = 0;
    char_u	*that;
    colnr_t	col;
    colnr_t	maybe;
    colnr_t	firsttry;


    realpos = curwin->w_cursor;
    curwin->w_cursor.col = 0;

    if ((pos = findmatch(NULL, '(')) != NULL)
    {
	curwin->w_cursor.lnum = pos->lnum;
	curwin->w_cursor.col = pos->col;
	col = pos->col;

	that = ml_get_curline();
	maybe = get_indent();	    /* XXX */

	if (maybe == 0)
	    amount = 2;
	else
	{
	    while (*that && col)
	    {
		amount += lbr_chartabsize(that, (colnr_t)amount);
		col--;
		that++;
	    }

	    that++;
	    amount++;
	    firsttry = amount;

	    /*
	     * Go to the start of the second word.
	     * If there is no second word, go back to firsttry.
	     * Also stop at a '('.
	     */

	    while (vim_iswhite(*that))
	    {
		amount += lbr_chartabsize(that, (colnr_t)amount);
		that++;
	    }
	    while (*that && !vim_iswhite(*that) && *that != '(')
	    {
		amount += lbr_chartabsize(that, (colnr_t)amount);
		that++;
	    }
	    while (vim_iswhite(*that))
	    {
		amount += lbr_chartabsize(that, (colnr_t)amount);
		that++;
	    }
	    if (! *that)
		amount = firsttry;
	}
    }
    else    /* no matching '(' found, use indent of previous non-empty line */
    {
	while (curwin->w_cursor.lnum > 1)
	{
	    --curwin->w_cursor.lnum;
	    if (!linewhite(curwin->w_cursor.lnum))
		break;
	}
	amount = get_indent();	    /* XXX */
    }

    curwin->w_cursor = realpos;

    if (amount < 0)
	amount = 0;
    return (int)amount;
}
#endif /* LISPINDENT */

/*
 * Preserve files and exit.
 * When called IObuff must contain a message.
 */
    void
preserve_exit()
{
    BUF	    *buf;

#ifdef USE_GUI
    if (gui.in_use)
    {
	gui.dying = TRUE;
	out_trash();	/* trash any pending output */
    }
    else
#endif
    {
	windgoto((int)Rows - 1, 0);

	/*
	 * Switch terminal mode back now, so these messages end up on the
	 * "normal" screen (if there are two screens).
	 */
	settmode(TMODE_COOK);
#ifdef WIN32
	if (can_end_termcap_mode(FALSE) == TRUE)
#endif
	    stoptermcap();
	out_flush();
    }

    out_str(IObuff);
    screen_start();		    /* don't know where cursor is now */
    out_flush();

    ml_close_notmod();		    /* close all not-modified buffers */

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
    {
	if (buf->b_ml.ml_mfp != NULL && buf->b_ml.ml_mfp->mf_fname != NULL)
	{
	    OUT_STR("Vim: preserving files...\n");
	    screen_start();	    /* don't know where cursor is now */
	    out_flush();
	    ml_sync_all(FALSE, FALSE);	/* preserve all swap files */
	    break;
	}
    }

    ml_close_all(FALSE);	    /* close all memfiles, without deleting */

    OUT_STR("Vim: Finished.\n");

    getout(1);
}

/*
 * return TRUE if "fname" exists.
 */
    int
vim_fexists(fname)
    char_u  *fname;
{
    struct stat st;

    if (stat((char *)fname, &st))
	return FALSE;
    return TRUE;
}

/*
 * Check for CTRL-C pressed, but only once in a while.
 * Should be used instead of ui_breakcheck() for functions that check for
 * each line in the file.  Calling ui_breakcheck() each time takes too much
 * time, because it can be a system call.
 */

#ifndef BREAKCHECK_SKIP
# ifdef USE_GUI		    /* assume the GUI only runs on fast computers */
#  define BREAKCHECK_SKIP 200
# else
#  define BREAKCHECK_SKIP 32
# endif
#endif

    void
line_breakcheck()
{
    static int	count = 0;

    if (++count == BREAKCHECK_SKIP)
    {
	count = 0;
	ui_breakcheck();
    }
}

#ifndef NO_EXPANDPATH
/*
 * Generic wildcard expansion code.
 *
 * Return FAIL when no single file was found.  In this case "num_file" is not
 * set, and "file" may contain an error message.
 * Return OK when some files found.  "num_file" is set to the number of
 * matches, "file" to the array of matches.  Call FreeWild() later.
 */
    int
expand_wildcards(num_pat, pat, num_file, file, flags)
    int	    num_pat;	    /* number of input patterns */
    char_u  **pat;	    /* array of input patterns */
    int	    *num_file;	    /* resulting number of files */
    char_u  ***file;	    /* array of resulting files */
    int	    flags;	    /* EW_DIR, EW_FILE, EW_NOTFOUND */
{
    int			i;
    struct growarray	ga;
    char_u		*p;
    static int		recursive = FALSE;

    /*
     * expand_env() is called to expand things like "~user".  If this fails,
     * it calls ExpandOne(), which brings us back here.  In this case, always
     * call the machine specific expansion function, if possible.  Otherwise,
     * return FAIL.
     */
    if (recursive)
#ifdef SPECIAL_WILDCHAR
	return mch_expand_wildcards(num_pat, pat, num_file, file, flags);
#else
	return FAIL;
#endif

#ifdef SPECIAL_WILDCHAR
    /*
     * If there are any special wildcard characters which we cannot handle
     * here, call machine specific function for all the expansion.  This
     * avoids starting the shell for each argument separately.
     */
    for (i = 0; i < num_pat; i++)
    {
	if (vim_strpbrk(pat[i], (char_u *)SPECIAL_WILDCHAR) != NULL)
	    return mch_expand_wildcards(num_pat, pat, num_file, file, flags);
    }
#endif

    recursive = TRUE;

    /*
     * The matching file names are stored in a growarray.  Init it empty.
     */
    ga_init(&ga);
    ga.ga_itemsize = sizeof(char_u *);
    ga.ga_growsize = 30;

    for (i = 0; i < num_pat; ++i)
    {
	/*
	 * First expand environment variables, "~/" and "~user/".
	 */
	p = pat[i];
	if (vim_strpbrk(p, (char_u *)"$~") != NULL)
	{
	    p = expand_env_save(p);
	    if (p == NULL)
		p = pat[i];
	}

	/*
	 * If there are wildcards: Expand file names and add each match to the
	 * list.  If there is no match, and EW_NOTFOUND is given, add the
	 * pattern.
	 * If there are no wildcards: Add the file name if it exists or when
	 * EW_NOTFOUND is given.
	 */
	if (mch_has_wildcard(p)
		? (mch_expandpath(&ga, p, flags) == 0 && (flags & EW_NOTFOUND))
		: ((flags & EW_NOTFOUND) || mch_getperm(p) >= 0))
	    addfile(&ga, p, flags);

	if (p != pat[i])
	    vim_free(p);
    }

    *num_file = ga.ga_len;
    *file = (ga.ga_data != NULL) ? (char_u **)ga.ga_data : (char_u **)"";

    recursive = FALSE;

    return (ga.ga_data != NULL) ? OK : FAIL;
}

/*
 * Add a file to a file list.  Accepted flags:
 * EW_DIR	add directories.
 * EW_FILE	add files.
 * EW_NOTFOUND	add even when it doesn't exist.
 */
    void
addfile(gap, f, flags)
    struct growarray	*gap;
    char_u		*f;	/* filename */
    int			flags;
{
    char_u	*p;
    int		isdir;

    /* if the file/dir doesn't exist, may not add it */
    if (!(flags & EW_NOTFOUND) && mch_getperm(f) < 0)
	return;

#ifdef FNAME_ILLEGAL
    /* if the file/dir contains illegal characters, don't add it */
    if (vim_strpbrk(f, (char_u *)FNAME_ILLEGAL) != NULL)
	return;
#endif

    isdir = mch_isdir(f);
    if ((isdir && !(flags & EW_DIR)) || (!isdir && !(flags & EW_FILE)))
	return;

    /* Make room for another item in the file list. */
    if (ga_grow(gap, 1) == FAIL)
	return;

    p = alloc((unsigned)(STRLEN(f) + 1 + isdir));
    if (p == NULL)
	return;

    STRCPY(p, f);
#ifdef BACKSLASH_IN_FILENAME
    slash_adjust(p);
#endif
    /*
     * Append a slash or backslash after directory names.
     */
    if (isdir)
	STRCAT(p, PATHSEPSTR);
    ((char_u **)gap->ga_data)[gap->ga_len++] = p;
    --gap->ga_room;
}
#endif /* !NO_EXPANDPATH */

/*
 * Free the list of files returned by expand_wildcards() or other expansion
 * functions.
 */
    void
FreeWild(num, file)
    int	    num;
    char_u  **file;
{
    if (file == NULL || num == 0)
	return;
#if defined(__EMX__) && defined(__ALWAYS_HAS_TRAILING_NULL_POINTER) /* XXX */
    /*
     * Is this still OK for when other functions than expand_wildcards() have
     * been used???
     */
    _fnexplodefree((char **)file);
#else
    while (num--)
	vim_free(file[num]);
    vim_free(file);
#endif
}
