/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 * move.c: Functions for moving the cursor and scrolling text.
 *
 * There are two ways to move the cursor:
 * 1. Move the cursor directly, the text is scrolled to keep the cursor in the
 *    window.
 * 2. Scroll the text, the cursor is moved into the text visible in the
 *    window.
 * The 'scrolloff' option makes this a bit complicated.
 */

#include "vim.h"

static void comp_botline __ARGS((win_t *wp));
static void curs_rows __ARGS((win_t *wp, int do_botline));
static void check_cursor_moved __ARGS((win_t *wp));
static void validate_botline_win __ARGS((win_t *wp));
static void validate_cheight __ARGS((void));

/*
 * Compute wp->w_botline for the current wp->w_topline.  Can be called after
 * wp->w_topline changed.
 */
    static void
comp_botline(wp)
    win_t	*wp;
{
    int		n;
    linenr_t	lnum;
    int		done;
#ifdef FEAT_FOLDING
    linenr_t    last;
    int		folded;
#endif

    /*
     * If w_cline_row is valid, start there.
     * Otherwise have to start at w_topline.
     */
    check_cursor_moved(wp);
    if (wp->w_valid & VALID_CROW)
    {
	lnum = wp->w_cursor.lnum;
	done = wp->w_cline_row;
    }
    else
    {
	lnum = wp->w_topline;
	done = 0;
    }

    for ( ; lnum <= wp->w_buffer->b_ml.ml_line_count; ++lnum)
    {
#ifdef FEAT_FOLDING
	last = lnum;
	folded = FALSE;
	if (hasFoldingWin(wp, lnum, NULL, &last, TRUE, NULL))
	{
	    n = 1;
	    folded = TRUE;
	}
	else
#endif
	    n = plines_win(wp, lnum, TRUE);
	if (
#ifdef FEAT_FOLDING
		lnum <= wp->w_cursor.lnum
		&& last >= wp->w_cursor.lnum
#else
		lnum == wp->w_cursor.lnum
#endif
	   )
	{
	    wp->w_cline_row = done;
	    wp->w_cline_height = n;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = folded;
#endif
	    wp->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	if (done + n > wp->w_height)
	    break;
	done += n;
#ifdef FEAT_FOLDING
	lnum = last;
#endif
    }

    /* wp->w_botline is the line that is just below the window */
    wp->w_botline = lnum;
    wp->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    /*
     * Also set wp->w_empty_rows, otherwise scroll_cursor_bot() won't work
     */
    if (done == 0)
	wp->w_empty_rows = 0;	/* single line that doesn't fit */
    else
	wp->w_empty_rows = wp->w_height - done;
}

/*
 * Update curwin->w_topline and redraw if necessary.
 * Used to update the screen before printing a message.
 */
    void
update_topline_redraw()
{
    update_topline();
    if (must_redraw)
	update_screen(0);
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    void
update_topline()
{
    long	line_count;
    int		halfheight;
    int		n;
    linenr_t	old_topline;
    linenr_t	lnum;
    int		check_topline = FALSE;
    int		check_botline = FALSE;
#ifdef FEAT_MOUSE
    int		save_so = p_so;
#endif

    if (!screen_valid(TRUE))
	return;

    check_cursor_moved(curwin);
    if (curwin->w_valid & VALID_TOPLINE)
	return;

#ifdef FEAT_MOUSE
    /* When dragging with the mouse, don't scroll that quickly */
    if (mouse_dragging)
	p_so = mouse_dragging - 1;
#endif

    old_topline = curwin->w_topline;

    /*
     * If the buffer is empty, always set topline to 1.
     */
    if (bufempty())		/* special case - file is empty */
    {
	if (curwin->w_topline != 1)
	    redraw_later(NOT_VALID);
	curwin->w_topline = 1;
	curwin->w_botline = 2;
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
#ifdef FEAT_SCROLLBIND
	curwin->w_scbind_pos = 1;
#endif
    }

    /*
     * If the cursor is above or near the top of the window, scroll the window
     * to show the line the cursor is in, with 'scrolloff' context.
     */
    else
    {
	if (curwin->w_topline > 1)
	{
	    /* If the cursor is above topline, scrolling is always needed.
	     * If the cursor is far below topline and there is no folding,
	     * scrolling down is never needed. */
	    if (curwin->w_cursor.lnum < curwin->w_topline)
		check_topline = TRUE;
	    else if (curwin->w_cursor.lnum < curwin->w_topline + p_so
#ifdef FEAT_FOLDING
		    || hasAnyFolding(curwin)
#endif
		    )
	    {
		/* Cursor is at or below topline, check if there are
		 * 'scrolloff' window lines above the cursor.  If not, need to
		 * scroll. */
		n = 0;
		for (lnum = curwin->w_topline; lnum < curwin->w_cursor.lnum;
			++lnum)
		{
		    if (n >= p_so)
			break;
#ifdef FEAT_FOLDING
		    /* A sequence of folded lines counts for one.
		     * Don't count it when the cursor is in it. */
		    if (hasFolding(lnum, NULL, &lnum))
		    {
			if (lnum >= curwin->w_cursor.lnum)
			    break;
			++n;
		    }
		    else
#endif
			n += plines(lnum);
		}
		if (n < p_so)
		    check_topline = TRUE;
	    }
	}

	if (check_topline)
	{
	    halfheight = curwin->w_height / 2 - 1;
	    if (halfheight < 2)
		halfheight = 2;

#ifdef FEAT_FOLDING
	    if (hasAnyFolding(curwin))
	    {
		/* Count the number of logical lines between the cursor and
		 * topline + p_so (approximation of how much will be
		 * scrolled). */
		n = 0;
		for (lnum = curwin->w_cursor.lnum;
				      lnum < curwin->w_topline + p_so; ++lnum)
		{
		    ++n;
		    /* stop at end of file or when we know we are far off */
		    if (lnum >= curbuf->b_ml.ml_line_count || n >= halfheight)
			break;
		    (void)hasFolding(lnum, NULL, &lnum);
		}
	    }
	    else
#endif
		n = curwin->w_topline + p_so - curwin->w_cursor.lnum;

	    /* If we weren't very close to begin with, we scroll to put the
	     * cursor in the middle of the window.  Otherwise put the cursor
	     * near the top of the window. */
	    if (n >= halfheight)
		scroll_cursor_halfway(FALSE);
	    else
	    {
		scroll_cursor_top((int)p_sj, FALSE);
		check_botline = TRUE;
	    }
	}

	else
	{
#ifdef FEAT_FOLDING
	    /* Make sure topline is the first line of a fold. */
	    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
	    check_botline = TRUE;
	}
    }

    /*
     * If the cursor is below the bottom of the window, scroll the window
     * to put the cursor on the window.
     * When w_botline is invalid, recompute it first, to avoid a redraw later.
     * If w_botline was approximated, we might need a redraw later in a few
     * cases, but we don't want to spend (a lot of) time recomputing w_botline
     * for every small change.
     */
    if (check_botline)
    {
	if (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();

	if (curwin->w_botline <= curbuf->b_ml.ml_line_count)
	{
	    if (curwin->w_cursor.lnum < curwin->w_botline
		    && ((long)curwin->w_cursor.lnum >= (long)curwin->w_botline - p_so
#ifdef FEAT_FOLDING
			|| hasAnyFolding(curwin)
#endif
			))
	    {
		/* Cursor is above botline, check if there are 'scrolloff'
		 * window lines below the cursor.  If not, need to scroll. */
		n = 0;
#ifdef FEAT_FOLDING
		if (hasFolding(curwin->w_cursor.lnum, NULL, &lnum))
		    ++lnum;
		else
#endif
		    lnum = curwin->w_cursor.lnum + 1;
		while (lnum < curwin->w_botline)
		{
		    if (n >= p_so)
			break;
#ifdef FEAT_FOLDING
		    /* A sequence of folded lines counts for one.
		     * Don't count it when the botline is in it. */
		    if (hasFolding(lnum, NULL, &lnum))
		    {
			if (lnum >= curwin->w_botline)
			    break;
			++n;
		    }
		    else
#endif
			n += plines(lnum);
		    ++lnum;
		}
		if (n >= p_so)
		    /* sufficient context, no need to scroll */
		    check_botline = FALSE;
	    }
	    if (check_botline)
	    {
#ifdef FEAT_FOLDING
		if (hasAnyFolding(curwin))
		{
		    /* Count the number of logical lines between the cursor and
		     * botline - p_so (approximation of how much will be
		     * scrolled). */
		    line_count = 0;
		    for (lnum = curwin->w_cursor.lnum;
				     lnum >= curwin->w_botline - p_so; --lnum)
		    {
			++line_count;
			/* stop at end of file or when we know we are far off */
			if (lnum <= 0 || line_count > curwin->w_height + 1)
			    break;
			(void)hasFolding(lnum, &lnum, NULL);
		    }
		}
		else
#endif
		    line_count = curwin->w_cursor.lnum - curwin->w_botline
								   + 1 + p_so;
		if (line_count <= curwin->w_height + 1)
		    scroll_cursor_bot((int)p_sj, FALSE);
		else
		    scroll_cursor_halfway(FALSE);
	    }
	}
    }
    curwin->w_valid |= VALID_TOPLINE;

    /*
     * Need to redraw when topline changed.
     */
    if (curwin->w_topline != old_topline)
    {
	if (curwin->w_skipcol)
	{
	    curwin->w_skipcol = 0;
	    redraw_later(NOT_VALID);
	}
	else
	    redraw_later(VALID);
	/* May need to set w_skipcol when cursor in w_topline. */
	if (curwin->w_cursor.lnum == curwin->w_topline)
	    validate_cursor();
    }

#ifdef FEAT_MOUSE
    p_so = save_so;
#endif
}

    void
update_curswant()
{
    if (curwin->w_set_curswant)
    {
	validate_virtcol();
	curwin->w_curswant = curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
	    + curwin->w_coladd
#endif
	    ;
	curwin->w_set_curswant = FALSE;
    }
}

/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    static void
check_cursor_moved(wp)
    win_t	*wp;
{
    if (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
				     |VALID_CHEIGHT|VALID_CROW|VALID_TOPLINE);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
#ifdef FEAT_VIRTUALEDIT
	wp->w_valid_coladd = wp->w_coladd;
#endif
    }
    else if (wp->w_cursor.col != wp->w_valid_cursor.col
	     || wp->w_leftcol != wp->w_valid_leftcol
#ifdef FEAT_VIRTUALEDIT
	     || wp->w_coladd != wp->w_valid_coladd
#endif
	     )
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.col = wp->w_cursor.col;
	wp->w_valid_leftcol = wp->w_leftcol;
#ifdef FEAT_VIRTUALEDIT
	wp->w_valid_coladd = wp->w_coladd;
#endif
    }
}

/*
 * Call this function when some window settings have changed, which require
 * the cursor position, botline and topline to be recomputed and the window to
 * be redrawn.  E.g, when changing the 'wrap' option or folding.
 */
    void
changed_window_setting()
{
    curwin->w_lines_valid = 0;
    changed_line_abv_curs();
    invalidate_botline();
    update_topline();
    redraw_later(NOT_VALID);
}

/*
 * Set wp->w_topline to a certain number.
 */
    void
set_topline(wp, lnum)
    win_t	*wp;
    linenr_t	lnum;
{
#ifdef FEAT_FOLDING
    /* go to first of folded lines */
    (void)hasFoldingWin(wp, lnum, &lnum, NULL, TRUE, NULL);
#endif
    /* Approximate the value of w_botline */
    wp->w_botline += lnum - wp->w_topline;
    wp->w_topline = lnum;
    wp->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    /* Don't set VALID_TOPLINE here! */
    redraw_later(VALID);
}

/*
 * Call this function when the length of the cursor line (in screen
 * characters) has changed, and the change is before the cursor.
 * Need to take care of w_botline separately!
 */
    void
changed_cline_bef_curs()
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    void
changed_cline_bef_curs_win(wp)
    win_t	*wp;
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
						|VALID_CHEIGHT|VALID_TOPLINE);
}

#if 0 /* not used */
/*
 * Call this function when the length of the cursor line (in screen
 * characters) has changed, and the position of the cursor doesn't change.
 * Need to take care of w_botline separately!
 */
    void
changed_cline_aft_curs()
{
    curwin->w_valid &= ~VALID_CHEIGHT;
}
#endif

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    void
changed_line_abv_curs()
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

    void
changed_line_abv_curs_win(wp)
    win_t	*wp;
{
    wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW
						|VALID_CHEIGHT|VALID_TOPLINE);
}

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    void
validate_botline()
{
    if (!(curwin->w_valid & VALID_BOTLINE))
	comp_botline(curwin);
}

/*
 * Make sure the value of wp->w_botline is valid.
 */
    static void
validate_botline_win(wp)
    win_t	*wp;
{
    if (!(wp->w_valid & VALID_BOTLINE))
	comp_botline(wp);
}

/*
 * Mark curwin->w_botline as invalid (because of some change in the buffer).
 */
    void
invalidate_botline()
{
    curwin->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

    void
invalidate_botline_win(wp)
    win_t	*wp;
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

#if 0 /* never used */
/*
 * Mark curwin->w_botline as approximated (because of some small change in the
 * buffer).
 */
    void
approximate_botline()
{
    curwin->w_valid &= ~VALID_BOTLINE;
}
#endif

    void
approximate_botline_win(wp)
    win_t	*wp;
{
    wp->w_valid &= ~VALID_BOTLINE;
}

#if 0 /* not used */
/*
 * Return TRUE if curwin->w_botline is valid.
 */
    int
botline_valid()
{
    return (curwin->w_valid & VALID_BOTLINE);
}
#endif

#if 0 /* not used */
/*
 * Return TRUE if curwin->w_botline is valid or approximated.
 */
    int
botline_approximated()
{
    return (curwin->w_valid & VALID_BOTLINE_AP);
}
#endif

/*
 * Return TRUE if curwin->w_wrow and curwin->w_wcol are valid.
 */
    int
cursor_valid()
{
    check_cursor_moved(curwin);
    return ((curwin->w_valid & (VALID_WROW|VALID_WCOL)) ==
						      (VALID_WROW|VALID_WCOL));
}

/*
 * Validate cursor position.  Makes sure w_wrow and w_wcol are valid.
 * w_topline must be valid, you may need to call update_topline() first!
 */
    void
validate_cursor()
{
    check_cursor_moved(curwin);
    if ((curwin->w_valid & (VALID_WCOL|VALID_WROW)) != (VALID_WCOL|VALID_WROW))
	curs_columns(TRUE);
}

#if defined(FEAT_GUI) || defined(PROTO)
/*
 * validate w_cline_row.
 */
    void
validate_cline_row()
{
    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();
    check_cursor_moved(curwin);
    if (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin, FALSE);
}
#endif

/*
 * Compute wp->w_cline_row and wp->w_cline_height, based on the current value
 * of wp->w_topine.
 *
 * Returns OK when cursor is in the window, FAIL when it isn't.
 */
    static void
curs_rows(wp, do_botline)
    win_t	*wp;
    int		do_botline;		/* also compute w_botline */
{
    linenr_t	lnum;
    int		i;
    int		all_invalid;
    int		valid;
#ifdef FEAT_FOLDING
    long	fold_count;
#endif

    /* Check if wp->w_lines[].wl_size is invalid */
    all_invalid = (!redrawing()
			|| wp->w_lines_valid == 0
			|| wp->w_lines[0].wl_lnum > wp->w_topline);
    i = 0;
    wp->w_cline_row = 0;
    for (lnum = wp->w_topline; lnum < wp->w_cursor.lnum; ++i)
    {
	valid = FALSE;
	if (!all_invalid && i < wp->w_lines_valid)
	{
	    if (wp->w_lines[i].wl_lnum < lnum || !wp->w_lines[i].wl_valid)
		continue;		/* skip changed or deleted lines */
	    if (wp->w_lines[i].wl_lnum == lnum)
	    {
#ifdef FEAT_FOLDING
		/* Check for newly inserted lines below this row, in which
		 * case we need to check for folded lines. */
		if (!wp->w_buffer->b_mod_set
			|| wp->w_lines[i].wl_lastlnum < wp->w_cursor.lnum
			|| wp->w_buffer->b_mod_top
					     > wp->w_lines[i].wl_lastlnum + 1)
#endif
		valid = TRUE;
	    }
	    else if (wp->w_lines[i].wl_lnum > lnum)
		--i;			/* hold at inserted lines */
	}
	if (valid)
	{
#ifdef FEAT_FOLDING
	    lnum = wp->w_lines[i].wl_lastlnum + 1;
	    /* Cursor inside folded lines, don't count this row */
	    if (lnum > wp->w_cursor.lnum)
		break;
#else
	    ++lnum;
#endif
	    wp->w_cline_row += wp->w_lines[i].wl_size;
	}
	else
	{
#ifdef FEAT_FOLDING
	    fold_count = foldedCount(wp, lnum, NULL);
	    if (fold_count)
	    {
		lnum += fold_count;
		if (lnum > wp->w_cursor.lnum)
		    break;
		++wp->w_cline_row;
	    }
	    else
#endif
		wp->w_cline_row += plines_win(wp, lnum++, TRUE);
	}
    }

    check_cursor_moved(wp);
    if (!(wp->w_valid & VALID_CHEIGHT))
    {
	if (all_invalid || (i < wp->w_lines_valid && !wp->w_lines[i].wl_valid))
	{
	    wp->w_cline_height = plines_win(wp, wp->w_cursor.lnum, TRUE);
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = hasFoldingWin(wp, wp->w_cursor.lnum,
						      NULL, NULL, TRUE, NULL);
#endif
	}
	else if (i > wp->w_lines_valid)
	{
	    /* a line that is too long to fit on the last screen line */
	    wp->w_cline_height = 0;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = FALSE;
#endif
	}
	else
	{
	    wp->w_cline_height = wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
	    wp->w_cline_folded = wp->w_lines[i].wl_folded;
#endif
	}
    }

    wp->w_valid |= VALID_CROW|VALID_CHEIGHT;

    /* validate botline too, if update_screen doesn't do it */
    if (do_botline && all_invalid)
	validate_botline_win(wp);
}

/*
 * Validate curwin->w_virtcol only.
 */
    void
validate_virtcol()
{
    validate_virtcol_win(curwin);
}

/*
 * Validate wp->w_virtcol only.
 */
    void
validate_virtcol_win(wp)
    win_t	*wp;
{
    check_cursor_moved(wp);
    if (!(wp->w_valid & VALID_VIRTCOL))
    {
	getvcol(wp, &wp->w_cursor, NULL, &(wp->w_virtcol), NULL);
	wp->w_valid |= VALID_VIRTCOL;
    }
}

/*
 * Validate curwin->w_cline_height only.
 */
    static void
validate_cheight()
{
    check_cursor_moved(curwin);
    if (!(curwin->w_valid & VALID_CHEIGHT))
    {
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
#ifdef FEAT_FOLDING
	curwin->w_cline_folded = hasFolding(curwin->w_cursor.lnum, NULL, NULL);
#endif
	curwin->w_valid |= VALID_CHEIGHT;
    }
}

/*
 * validate w_wcol and w_virtcol only.	Only correct when 'wrap' on!
 */
    void
validate_cursor_col()
{
    colnr_t off;
    colnr_t col;

    validate_virtcol();
    if (!(curwin->w_valid & VALID_WCOL))
    {
	col = curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
	    + curwin->w_coladd
#endif
	    ;
	off = curwin_col_off();
	col += off;

	/* long line wrapping, adjust curwin->w_wrow */
	if (curwin->w_p_wrap && col >= (colnr_t)W_WIDTH(curwin)
#ifdef FEAT_VERTSPLIT
		&& curwin->w_width != 0
#endif
		)
	{
	    col -= W_WIDTH(curwin);
	    col = col % (W_WIDTH(curwin) - off + curwin_col_off2());
	}
	curwin->w_wcol = col;
	curwin->w_valid |= VALID_WCOL;
    }
}

/*
 * Compute offset of a window, occupied by line number, fold column and sign
 * column (these don't move when scrolling horizontally).
 */
    int
win_col_off(wp)
    win_t	*wp;
{
    return ((wp->w_p_nu ? 8 : 0)
#ifdef FEAT_CMDWIN
	    + (cmdwin_type == 0 && wp == curwin ? 0 : 1)
#endif
#ifdef FEAT_FOLDING
	    + wp->w_p_fdc
#endif
#ifdef FEAT_SIGNS
	    + (wp->w_buffer->b_signlist != NULL ? 2 : 0)
#endif
	   );
}

    int
curwin_col_off()
{
    return win_col_off(curwin);
}

/*
 * Return the difference in column offset for the second screen line of a
 * wrapped line.  It's 8 if 'number' is on and 'n' is in 'cpoptions'.
 */
    int
win_col_off2(wp)
    win_t	*wp;
{
    if (wp->w_p_nu && vim_strchr(p_cpo, CPO_NUMCOL) != NULL)
	return 8;
    return 0;
}

    int
curwin_col_off2()
{
    return win_col_off2(curwin);
}

/*
 * compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 * Also updates curwin->w_leftcol.
 */
    void
curs_columns(scroll)
    int		scroll;		/* when TRUE, may scroll horizontally */
{
    int		diff;
    int		extra;		/* offset for first screen line */
    int		n;
    int		width = 0;
    int		new_leftcol;
    colnr_t	startcol;
    colnr_t	endcol;
    colnr_t	prev_skipcol;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    if (!(curwin->w_valid & VALID_CROW))
	curs_rows(curwin, FALSE);

    /*
     * Compute the number of virtual columns.
     */
#ifdef FEAT_FOLDING
    if (curwin->w_cline_folded)
	/* In a folded line the cursor is always in column 0 */
	startcol = curwin->w_virtcol = endcol = 0;
    else
#endif
	getvcol(curwin, &curwin->w_cursor,
				&startcol, &(curwin->w_virtcol), &endcol);

    /* remove '$' from change command when cursor moves onto it */
    if (startcol > dollar_vcol)
	dollar_vcol = 0;

    extra = curwin_col_off();
    curwin->w_wcol = curwin->w_virtcol + extra
#ifdef FEAT_VIRTUALEDIT
	+ curwin->w_coladd
#endif
	;
    endcol += extra;

#ifdef FEAT_VIRTUALEDIT
    if (ve_flags == VE_ALL
# ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
# endif
	    )
	startcol = endcol = curwin->w_wcol;
#endif

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    if (curwin->w_p_wrap
#ifdef FEAT_VERTSPLIT
	    && curwin->w_width != 0
#endif
	    )
    {
	width = W_WIDTH(curwin) - extra + curwin_col_off2();

	/* long line wrapping, adjust curwin->w_wrow */
	if (curwin->w_wcol >= W_WIDTH(curwin))
	{
	    if (W_WIDTH(curwin) <= extra)
	    {
		/* No room for text, put cursor in last char of window. */
		curwin->w_wcol = W_WIDTH(curwin) - 1;
		curwin->w_wrow = curwin->w_height - 1;
	    }
	    else
	    {
		n = (curwin->w_wcol - W_WIDTH(curwin)) / width + 1;
		curwin->w_wcol -= n * width;
		curwin->w_wrow += n;

#ifdef FEAT_LINEBREAK
		/* When cursor wraps to first char of next line in Insert
		 * mode, the 'showbreak' string isn't shown, backup to first
		 * column */
		if (*p_sbr && *ml_get_cursor() == NUL
				      && curwin->w_wcol == (int)STRLEN(p_sbr))
		    curwin->w_wcol = 0;
#endif
	    }
	}
    }

    /* No line wrapping: compute curwin->w_leftcol if scrolling is on and line
     * is not folded.
     * If scrolling is off, curwin->w_leftcol is assumed to be 0 */
    else if (scroll
#ifdef FEAT_FOLDING
	    && !curwin->w_cline_folded
#endif
	    )
    {
	/*
	 * If Cursor is left of the screen, scroll rightwards.
	 * If Cursor is right of the screen, scroll leftwards
	 * If we get closer to the edge than 'sidescrolloff', scroll a little
	 * extra
	 */
	if ((extra = (int)startcol - (int)curwin->w_leftcol - p_siso) < 0
		|| (extra = (int)endcol
			 - (int)(curwin->w_leftcol + W_WIDTH(curwin) - p_siso)
			 + 1) > 0)
	{
	    if (extra < 0)
		diff = -extra;
	    else
		diff = extra;

	    /* far off, put cursor in middle of window */
	    if (p_ss == 0 || diff >= W_WIDTH(curwin) / 2)
		new_leftcol = curwin->w_wcol - W_WIDTH(curwin) / 2;
	    else
	    {
		if (diff < p_ss)
		    diff = p_ss;
		if (extra < 0)
		    new_leftcol = curwin->w_leftcol - diff;
		else
		    new_leftcol = curwin->w_leftcol + diff;
	    }
	    if (new_leftcol < 0)
		new_leftcol = 0;
	    if (new_leftcol != (int)curwin->w_leftcol)
	    {
		curwin->w_leftcol = new_leftcol;
		/* screen has to be redrawn with new curwin->w_leftcol */
		redraw_later(NOT_VALID);
	    }
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    else if (curwin->w_wcol > (int)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    else
	curwin->w_wcol = 0;

    prev_skipcol = curwin->w_skipcol;

    n = 0;
    if ((curwin->w_wrow >= curwin->w_height
		|| ((prev_skipcol > 0
			|| curwin->w_wrow + p_so >= curwin->w_height)
		    && (n = plines_win(curwin, curwin->w_cursor.lnum, FALSE))
						    - 1 >= curwin->w_height))
	    && curwin->w_height != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
#ifdef FEAT_VERTSPLIT
	    && curwin->w_width != 0
#endif
	    )
    {
	/* Cursor past end of screen.  Happens with a single line that does
	 * not fit on screen.  Find a skipcol to show the text around the
	 * cursor.  Avoid scrolling all the time. compute value of "extra":
	 * 1: Less than "p_so" lines above
	 * 2: Less than "p_so" lines below
	 * 3: both of them */
	extra = 0;
	if (curwin->w_skipcol + p_so * width > curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
							    + curwin->w_coladd
#endif
		)
	    extra = 1;
	/* Compute last display line of the buffer line that we want at the
	 * bottom of the window. */
	if (n == 0)
	    n = plines_win(curwin, curwin->w_cursor.lnum, FALSE);
	--n;
	if (curwin->w_wrow + p_so < n)
	    n = curwin->w_wrow + p_so;
	if ((colnr_t)n >= curwin->w_height + curwin->w_skipcol / width)
	    extra += 2;

	if (extra == 3)
	{
	    /* not enough room for 'scrolloff', put cursor in the middle */
	    n = (curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
		    + curwin->w_coladd
#endif
				     ) / width;
	    if (n > curwin->w_height / 2)
		n -= curwin->w_height / 2;
	    else
		n = 0;
	    curwin->w_skipcol = n * width;
	}
	else if (extra == 1)
	{
	    /* less then 'scrolloff' lines above, decrease skipcol */
	    extra = (curwin->w_skipcol + p_so * width - curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
							    + curwin->w_coladd
#endif
				     + width - 1) / width;
	    if (extra > 0)
	    {
		if ((colnr_t)(extra * width) > curwin->w_skipcol)
		    extra = curwin->w_skipcol / width;
		curwin->w_skipcol -= extra * width;
	    }
	}
	else if (extra == 2)
	{
	    /* less then 'scrolloff' lines below, increase skipcol */
	    endcol = (n - curwin->w_height + 1) * width;
	    if (endcol > curwin->w_skipcol)
		curwin->w_skipcol = endcol;
	}

	curwin->w_wrow -= curwin->w_skipcol / width;
	if (curwin->w_wrow >= curwin->w_height)
	{
	    /* small window, make sure cursor is in it */
	    extra = curwin->w_wrow - curwin->w_height + 1;
	    curwin->w_skipcol += extra * width;
	    curwin->w_wrow -= extra;
	}

	extra = (curwin->w_skipcol - prev_skipcol) / width;
	if (extra > 0)
	    win_ins_lines(curwin, 0, extra, FALSE, FALSE);
	else if (extra < 0)
	    win_del_lines(curwin, 0, -extra, FALSE, FALSE);

    }
    else
	curwin->w_skipcol = 0;
    if (prev_skipcol != curwin->w_skipcol)
	redraw_later(NOT_VALID);

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

/*
 * Scroll the current window down by "line_count" logical lines.
 */
/*ARGSUSED*/
    void
scrolldown(line_count, byfold)
    long	line_count;
    int		byfold;		/* TRUE: count a closed fold as one line */
{
    long	done = 0;	/* total # of physical lines done */
    int		wrow;
    int		moved = FALSE;

#ifdef FEAT_FOLDING
    linenr_t	first;

    /* Make sure w_topline is at the first of a sequence of folded lines. */
    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
    validate_cursor();		/* w_wrow needs to be valid */
    while (line_count-- > 0)
    {
	if (curwin->w_topline == 1)
	    break;
	--curwin->w_topline;
#ifdef FEAT_FOLDING
	/* A sequence of folded lines only counts for one logical line */
	if (hasFolding(curwin->w_topline, &first, NULL))
	{
	    ++done;
	    if (!byfold)
		line_count -= curwin->w_topline - first - 1;
	    curwin->w_botline -= curwin->w_topline - first;
	    curwin->w_topline = first;
	}
	else
#endif
	    done += plines(curwin->w_topline);
	--curwin->w_botline;		/* approximate w_botline */
	invalidate_botline();
    }
    curwin->w_wrow += done;		/* keep w_wrow updated */
    curwin->w_cline_row += done;	/* keep w_cline_row updated */

    /*
     * Compute the row number of the last row of the cursor line
     * and move the cursor onto the displayed part of the window.
     */
    wrow = curwin->w_wrow;
    if (curwin->w_p_wrap
#ifdef FEAT_VERTSPLIT
		&& curwin->w_width != 0
#endif
	    )
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 - (curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
							    + curwin->w_coladd
#endif
							  ) / W_WIDTH(curwin);
    }
    while (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
#ifdef FEAT_FOLDING
	if (hasFolding(curwin->w_cursor.lnum, &first, NULL))
	{
	    --wrow;
	    if (first == 1)
		curwin->w_cursor.lnum = 1;
	    else
		curwin->w_cursor.lnum = first - 1;
	}
	else
#endif
	    wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    if (moved)
    {
#ifdef FEAT_FOLDING
	/* Move cursor to first line of closed fold. */
	foldAdjustCursor();
#endif
	coladvance(curwin->w_curswant);
    }
}

/*
 * Scroll the current window up by "line_count" logical lines.
 */
/*ARGSUSED*/
    void
scrollup(line_count, byfold)
    long	line_count;
    int		byfold;		/* TRUE: count a closed fold as one line */
{
#ifdef FEAT_FOLDING
    linenr_t	lnum;

    if (byfold && hasAnyFolding(curwin))
    {
	/* count each sequence of folded lines as one logical line */
	lnum = curwin->w_topline;
	while (line_count--)
	{
	    (void)hasFolding(lnum, NULL, &lnum);
	    ++lnum;
	    if (lnum >= curbuf->b_ml.ml_line_count)
	    {
		lnum = curbuf->b_ml.ml_line_count;
		break;
	    }
	}
	/* approximate w_botline */
	curwin->w_botline += lnum - curwin->w_topline;
	curwin->w_topline = lnum;
    }
    else
#endif
    {
	curwin->w_topline += line_count;
	curwin->w_botline += line_count;	/* approximate w_botline */
    }

#ifdef FEAT_FOLDING
    if (!byfold && hasAnyFolding(curwin))
    {
	/* Make sure w_topline is at the first of a sequence of folded lines. */
	(void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
    }
#endif

    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    if (curwin->w_topline > curbuf->b_ml.ml_line_count)
	curwin->w_topline = curbuf->b_ml.ml_line_count;
    if (curwin->w_cursor.lnum < curwin->w_topline)
    {
	curwin->w_cursor.lnum = curwin->w_topline;
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	coladvance(curwin->w_curswant);
    }
}

#if defined(FEAT_INS_EXPAND) || defined(PROTO)
/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    void
scrolldown_clamp()
{
    int	    end_row;

    if (curwin->w_topline <= 1)
	return;

    validate_cursor();	    /* w_wrow needs to be valid */

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow + plines(curwin->w_topline - 1);
    if (curwin->w_p_wrap
#ifdef FEAT_VERTSPLIT
		&& curwin->w_width != 0
#endif
	    )
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 - (curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
							    + curwin->w_coladd
#endif
							  ) / W_WIDTH(curwin);
    }
    if (end_row < curwin->w_height - p_so)
    {
	--curwin->w_topline;
	--curwin->w_botline;	    /* approximate w_botline */
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * Scroll the screen one line up, but don't do it if it would move the cursor
 * off the screen.
 */
    void
scrollup_clamp()
{
    int	    start_row;

    if (curwin->w_topline == curbuf->b_ml.ml_line_count)
	return;

    validate_cursor();	    /* w_wrow needs to be valid */

    /*
     * Compute the row number of the first row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go before 'scrolloff' lines from the screen start.
     */
    start_row = curwin->w_wrow - plines(curwin->w_topline);
    if (curwin->w_p_wrap
#ifdef FEAT_VERTSPLIT
		&& curwin->w_width != 0
#endif
	    )
    {
	validate_virtcol();
	start_row -= (curwin->w_virtcol
#ifdef FEAT_VIRTUALEDIT
			+ curwin->w_coladd
#endif
			) / W_WIDTH(curwin);
    }
    if (start_row >= p_so)
    {
	++curwin->w_topline;
	++curwin->w_botline;		/* approximate w_botline */
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}
#endif /* FEAT_INS_EXPAND */

/*
 * Recompute topline to put the cursor at the top of the window.
 * Scroll at least "min_scroll" lines.
 * If "always" is TRUE, always set topline (for "zt").
 */
    void
scroll_cursor_top(min_scroll, always)
    int	    min_scroll;
    int	    always;
{
    int		scrolled = 0;
    int		extra = 0;
    int		used;
    int		i;
    linenr_t	top;		/* just above displayed lines */
    linenr_t	bot;		/* just below displayed lines */
    linenr_t	old_topline = curwin->w_topline;
    linenr_t	new_topline;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    used = curwin->w_cline_height;

#ifdef FEAT_FOLDING
    if (hasFolding(curwin->w_cursor.lnum, &top, &bot))
    {
	--top;
	++bot;
    }
    else
#endif
    {
	top = curwin->w_cursor.lnum - 1;
	bot = curwin->w_cursor.lnum + 1;
    }
    new_topline = top + 1;

    /*
     * Check if the lines from "top" to "bot" fit in the window.  If they do,
     * set new_topline and advance "top" and "bot" to include more lines.
     */
    while (top > 0)
    {
#ifdef FEAT_FOLDING
	if (hasFolding(top, &top, NULL))
	    /* count one logical line for a sequence of folded lines */
	    i = 1;
	else
#endif
	    i = plines(top);
	used += i;
	extra += i;
	if (extra <= (
#ifdef FEAT_MOUSE
		    mouse_dragging ? mouse_dragging - 1 :
#endif
		    p_so)
		&& bot < curbuf->b_ml.ml_line_count)
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(bot, NULL, &bot))
		/* count one logical line for a sequence of folded lines */
		++used;
	    else
#endif
		used += plines(bot);
	}
	if (used > curwin->w_height)
	    break;
	if (top < curwin->w_topline)
	    scrolled += i;

	/*
	 * If scrolling is needed, scroll at least 'sj' lines.
	 */
	if ((new_topline >= curwin->w_topline || scrolled >= min_scroll)
		&& extra > (
#ifdef FEAT_MOUSE
		    mouse_dragging ? mouse_dragging - 1 :
#endif
		    p_so))
	    break;

	new_topline = top;
	--top;
	++bot;
    }

    /*
     * If we don't have enough space, put cursor in the middle.
     * This makes sure we get the same position when using "k" and "j"
     * in a small window.
     */
    if (used > curwin->w_height)
	scroll_cursor_halfway(FALSE);
    else
    {
	/*
	 * If "always" is FALSE, only adjust topline to a lower value, higher
	 * value may happen with wrapping lines
	 */
	if (new_topline < curwin->w_topline || always)
	    curwin->w_topline = new_topline;
	if (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
	if (curwin->w_topline != old_topline)
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	curwin->w_valid |= VALID_TOPLINE;
    }
}

/*
 * Recompute topline to put the cursor at the bottom of the window.
 * Scroll at least "min_scroll" lines.
 * If "set_topbot" is TRUE, set topline and botline first (for "zb").
 * This is messy stuff!!!
 */
    void
scroll_cursor_bot(min_scroll, set_topbot)
    int	    min_scroll;
    int	    set_topbot;
{
    int		used;
    int		scrolled = 0;
    int		extra = 0;
    int		i;
    linenr_t	lnum;
    linenr_t	line_count;
    linenr_t	top;
    linenr_t	bot;
    linenr_t	old_topline = curwin->w_topline;
    linenr_t	old_botline = curwin->w_botline;
    linenr_t	old_valid = curwin->w_valid;
    int		old_empty_rows = curwin->w_empty_rows;
    linenr_t	cln;		    /* Cursor Line Number */

    cln = curwin->w_cursor.lnum;
    if (set_topbot)
    {
	used = 0;
	curwin->w_botline = cln + 1;
	for (curwin->w_topline = curwin->w_botline;
		curwin->w_topline != 1;
		curwin->w_topline = lnum)
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(curwin->w_topline - 1, &lnum, NULL))
		i = 1;
	    else
#endif
	    {
		lnum = curwin->w_topline - 1;
		i = plines(lnum);
	    }
	    if (used + i > curwin->w_height)
		break;
	    used += i;
	}
	curwin->w_empty_rows = curwin->w_height - used;
	curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;
	if (curwin->w_topline != old_topline)
	    curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
    }
    else
	validate_botline();

    validate_cheight();
    used = curwin->w_cline_height;

    /* If the cursor is below botline, we will at least scroll by the height
     * of the cursor line.  Correct for empty lines, which are really part of
     * botline. */
    if (cln >= curwin->w_botline)
    {
	scrolled = used;
	if (cln == curwin->w_botline)
	    scrolled -= curwin->w_empty_rows;
    }

    /*
     * Stop counting lines to scroll when
     * - hitting start of the file
     * - scrolled nothing or at least 'sj' lines
     * - at least 'so' lines below the cursor
     * - lines between botline and cursor have been counted
     */
#ifdef FEAT_FOLDING
    if (hasFolding(curwin->w_cursor.lnum, &top, &bot))
    {
	--top;
	++bot;
    }
    else
#endif
    {
	top = cln - 1;
	bot = cln + 1;
    }

    while (top > 0)
    {
	if ((((scrolled <= 0 || scrolled >= min_scroll)
			&& extra >= (
#ifdef FEAT_MOUSE
			    mouse_dragging ? mouse_dragging - 1 :
#endif
			    p_so))
		    || bot > curbuf->b_ml.ml_line_count)
		&& top < curwin->w_botline)
	    break;
#ifdef FEAT_FOLDING
	if (hasFolding(top, &top, NULL))
	    /* count one logical line for a sequence of folded lines */
	    i = 1;
	else
#endif
	    i = plines(top);
	used += i;
	if (used > curwin->w_height)
	    break;
	if (top >= curwin->w_botline)
	{
	    scrolled += i;
	    if (top == curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}
	if (bot <= curbuf->b_ml.ml_line_count)
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(bot, NULL, &bot))
		i = 1;
	    else
#endif
		i = plines(bot);
	    used += i;
	    if (used > curwin->w_height)
		break;
	    if (extra < (
#ifdef FEAT_MOUSE
			mouse_dragging ? mouse_dragging - 1 :
#endif
			p_so) || scrolled < min_scroll)
	    {
		extra += i;
		if (bot >= curwin->w_botline)
		{
		    scrolled += i;
		    if (bot == curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
	--top;
	++bot;
    }

    /* curwin->w_empty_rows is larger, no need to scroll */
    if (scrolled <= 0)
	line_count = 0;
    /* more than a screenfull, don't scroll but redraw */
    else if (used > curwin->w_height)
	line_count = used;
    /* scroll minimal number of lines */
    else
    {
	line_count = 0;
	for (i = 0, lnum = curwin->w_topline;
		i < scrolled && lnum < curwin->w_botline; ++lnum)
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(lnum, NULL, &lnum))
		++i;
	    else
#endif
		i += plines(lnum);
	    ++line_count;
	}
	if (i < scrolled)	/* below curwin->w_botline, don't scroll */
	    line_count = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    if (line_count >= curwin->w_height && line_count > min_scroll)
	scroll_cursor_halfway(FALSE);
    else
	scrollup(line_count, TRUE);

    /*
     * If topline didn't change we need to restore w_botline and w_empty_rows
     * (we changed them).
     * If topline did change, update_screen() will set botline.
     */
    if (curwin->w_topline == old_topline && set_topbot)
    {
	curwin->w_botline = old_botline;
	curwin->w_empty_rows = old_empty_rows;
	curwin->w_valid = old_valid;
    }
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Recompute topline to put the cursor halfway the window
 * If "atend" is TRUE, also put it halfway at the end of the file.
 */
    void
scroll_cursor_halfway(atend)
    int	    atend;
{
    int		above = 0;
    linenr_t	topline;
    int		below = 0;
    linenr_t	botline;
    int		used;
    int		i;
    linenr_t	lnum;

    topline = botline = curwin->w_cursor.lnum;
    used = plines(topline);
    while (topline > 1)
    {
	if (below <= above)	    /* add a line below the cursor first */
	{
	    if (botline + 1 <= curbuf->b_ml.ml_line_count)
	    {
#ifdef FEAT_FOLDING
		if (hasFolding(botline + 1, NULL, &lnum))
		    i = 1;
		else
#endif
		{
		    lnum = botline + 1;
		    i = plines(lnum);
		}
		used += i;
		if (used > curwin->w_height)
		    break;
		below += i;
		botline = lnum;
	    }
	    else
	    {
		++below;	    /* count a "~" line */
		if (atend)
		    ++used;
	    }
	}

	if (below > above)	    /* add a line above the cursor */
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(topline - 1, &lnum, NULL))
		i = 1;
	    else
#endif
	    {
		lnum = topline - 1;
		i = plines(lnum);
	    }
	    used += i;
	    if (used > curwin->w_height)
		break;
	    above += i;
	    topline = lnum;
	}
    }
#ifdef FEAT_FOLDING
    if (!hasFolding(topline, &curwin->w_topline, NULL))
#endif
	curwin->w_topline = topline;
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
    curwin->w_valid |= VALID_TOPLINE;
}

/*
 * Correct the cursor position so that it is in a part of the screen at least
 * 'so' lines from the top and bottom, if possible.
 * If not possible, put it at the same position as scroll_cursor_halfway().
 * When called topline must be valid!
 */
    void
cursor_correct()
{
    int		above = 0;	    /* screen lines above topline */
    linenr_t	topline;
    int		below = 0;	    /* screen lines below botline */
    linenr_t	botline;
    int		above_wanted, below_wanted;
    linenr_t	cln;		    /* Cursor Line Number */
    int		max_off;

    /*
     * How many lines we would like to have above/below the cursor depends on
     * whether the first/last line of the file is on screen.
     */
    above_wanted = p_so;
    below_wanted = p_so;
#ifdef FEAT_MOUSE
    if (mouse_dragging)
    {
	above_wanted = mouse_dragging - 1;
	below_wanted = mouse_dragging - 1;
    }
#endif
    if (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	if (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    if (curwin->w_botline == curbuf->b_ml.ml_line_count + 1
#ifdef FEAT_MOUSE
	    && !mouse_dragging
#endif
	    )
    {
	below_wanted = 0;
	max_off = (curwin->w_height - 1) / 2;
	if (above_wanted > max_off)
	    above_wanted = max_off;
    }

    /*
     * If there are sufficient file-lines above and below the cursor, we can
     * return now.
     */
    cln = curwin->w_cursor.lnum;
    if (cln >= curwin->w_topline + above_wanted
	    && cln < curwin->w_botline - below_wanted
#ifdef FEAT_FOLDING
	    && !hasAnyFolding(curwin)
#endif
	    )
	return;

    /*
     * Narrow down the area where the cursor can be put by taking lines from
     * the top and the bottom until:
     * - the desired context lines are found
     * - the lines from the top is past the lines from the bottom
     */
    topline = curwin->w_topline;
    botline = curwin->w_botline - 1;
    while ((above < above_wanted || below < below_wanted) && topline < botline)
    {
	if (below < below_wanted && (below <= above || above >= above_wanted))
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(botline, &botline, NULL))
		++below;
	    else
#endif
		below += plines(botline);
	    --botline;
	}
	if (above < above_wanted && (above < below || below >= below_wanted))
	{
#ifdef FEAT_FOLDING
	    if (hasFolding(topline, NULL, &topline))
		++above;
	    else
#endif
		above += plines(topline);
	    ++topline;
	}
    }
    if (topline == botline || botline == 0)
	curwin->w_cursor.lnum = topline;
    else if (topline > botline)
	curwin->w_cursor.lnum = botline;
    else
    {
	if (cln < topline && curwin->w_topline > 1)
	{
	    curwin->w_cursor.lnum = topline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
	if (cln > botline && curwin->w_botline <= curbuf->b_ml.ml_line_count)
	{
	    curwin->w_cursor.lnum = botline;
	    curwin->w_valid &=
			    ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW);
	}
    }
    curwin->w_valid |= VALID_TOPLINE;
}

static int get_scroll_overlap __ARGS((linenr_t lnum, int dir));

/*
 * move screen 'count' pages up or down and update screen
 *
 * return FAIL for failure, OK otherwise
 */
    int
onepage(dir, count)
    int	    dir;
    long    count;
{
    linenr_t	    lp;
    long	    n;
    int		    off;
    int		    retval = OK;

    if (curbuf->b_ml.ml_line_count == 1)    /* nothing to do */
    {
	beep_flush();
	return FAIL;
    }

    for ( ; count > 0; --count)
    {
	validate_botline();
	/*
	 * It's an error to move a page up when the first line is already on
	 * the screen.	It's an error to move a page down when the last line
	 * is on the screen and the topline is 'scrolloff' lines from the
	 * last line.
	 */
	if (dir == FORWARD
		? ((curwin->w_topline >= curbuf->b_ml.ml_line_count - p_so)
		    && curwin->w_botline > curbuf->b_ml.ml_line_count)
		: (curwin->w_topline == 1))
	{
	    beep_flush();
	    retval = FAIL;
	    break;
	}

	if (dir == FORWARD)
	{
					/* at end of file */
	    if (curwin->w_botline > curbuf->b_ml.ml_line_count)
	    {
		curwin->w_topline = curbuf->b_ml.ml_line_count;
		curwin->w_valid &= ~(VALID_WROW|VALID_CROW);
	    }
	    else
	    {
		lp = curwin->w_botline;
		off = get_scroll_overlap(lp, -1);
		curwin->w_topline = curwin->w_botline - off;
		curwin->w_cursor.lnum = curwin->w_topline;
		curwin->w_valid &= ~(VALID_WCOL|VALID_CHEIGHT|VALID_WROW|
				   VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    }
	}
	else	/* dir == BACKWARDS */
	{
	    lp = curwin->w_topline - 1;
	    off = get_scroll_overlap(lp, 1);
	    lp += off;
	    if (lp > curbuf->b_ml.ml_line_count)
		lp = curbuf->b_ml.ml_line_count;
	    curwin->w_cursor.lnum = lp;
	    n = 0;
	    while (n <= curwin->w_height && lp >= 1)
	    {
		n += plines(lp);
		--lp;
#ifdef FEAT_FOLDING
		(void)hasFolding(lp, &lp, NULL);
#endif
	    }
	    if (n <= curwin->w_height)		    /* at begin of file */
	    {
		curwin->w_topline = 1;
		curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
	    }
	    else if (lp >= curwin->w_topline - 2)   /* very long lines */
	    {
		--curwin->w_topline;
		comp_botline(curwin);
		curwin->w_cursor.lnum = curwin->w_botline - 1;
		curwin->w_valid &= ~(VALID_WCOL|VALID_CHEIGHT|
						       VALID_WROW|VALID_CROW);
	    }
	    else
	    {
		curwin->w_topline = lp + 2;
		curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
	    }
	}
    }
#ifdef FEAT_FOLDING
    foldAdjustCursor();
#endif
    cursor_correct();
    beginline(BL_SOL | BL_FIX);
    curwin->w_valid &= ~(VALID_WCOL|VALID_WROW|VALID_VIRTCOL);

    /*
     * Avoid the screen jumping up and down when 'scrolloff' is non-zero.
     */
    if (dir == FORWARD && curwin->w_cursor.lnum < curwin->w_topline + p_so)
	scroll_cursor_top(1, FALSE);

    redraw_later(VALID);
    return retval;
}

/*
 * Decide how much overlap to use for page-up or page-down scrolling.
 * This is symmetric, so that doing both keeps the same lines displayed.
 */
    static int
get_scroll_overlap(lnum, dir)
    linenr_t	lnum;
    int		dir;
{
    int		h1, h2, h3, h4;
    int		min_height = curwin->w_height - 2;
    linenr_t	l;

    h1 = plines_check(lnum);
    if (h1 > min_height)
	return 0;
    else
    {
	l = lnum;
#ifdef FEAT_FOLDING
	if (dir > 0)
	    (void)hasFolding(l, NULL, &l);
	else
	    (void)hasFolding(l, &l, NULL);
#endif
	l += dir;
	h2 = plines_check(l);
	if (h2 + h1 > min_height)
	    return 0;
	else
	{
#ifdef FEAT_FOLDING
	    if (dir > 0)
		(void)hasFolding(l, NULL, &l);
	    else
		(void)hasFolding(l, &l, NULL);
#endif
	    l += dir;
	    h3 = plines_check(l);
	    if (h3 + h2 > min_height)
		return 0;
	    else
	    {
#ifdef FEAT_FOLDING
		if (dir > 0)
		    (void)hasFolding(l, NULL, &l);
		else
		    (void)hasFolding(l, &l, NULL);
#endif
		l += dir;
		h4 = plines_check(l);
		if (h4 + h3 + h2 > min_height || h3 + h2 + h1 > min_height)
		    return 1;
		else
		    return 2;
	    }
	}
    }
}

/* #define KEEP_SCREEN_LINE */
/*
 * Scroll 'scroll' lines up or down.
 */
    void
halfpage(flag, Prenum)
    int		flag;
    linenr_t	Prenum;
{
    long	scrolled = 0;
    int		i;
    int		n;
    int		room;

    if (Prenum)
	curwin->w_p_scr = (Prenum > curwin->w_height) ?
						curwin->w_height : Prenum;
    n = (curwin->w_p_scr <= curwin->w_height) ?
				    curwin->w_p_scr : curwin->w_height;

    validate_botline();
    room = curwin->w_empty_rows;
    if (flag)	    /* scroll down */
    {
	while (n > 0 && curwin->w_botline <= curbuf->b_ml.ml_line_count)
	{
	    i = plines(curwin->w_topline);
	    n -= i;
	    if (n < 0 && scrolled)
		break;
#ifdef FEAT_FOLDING
	    (void)hasFolding(curwin->w_topline, NULL, &curwin->w_topline);
#endif
	    ++curwin->w_topline;
	    curwin->w_valid &= ~(VALID_CROW|VALID_WROW);
	    scrolled += i;

	    /*
	     * Correct w_botline for changed w_topline.
	     */
	    room += i;
	    do
	    {
		i = plines(curwin->w_botline);
		if (i > room)
		    break;
#ifdef FEAT_FOLDING
		(void)hasFolding(curwin->w_botline, NULL, &curwin->w_botline);
#endif
		++curwin->w_botline;
		room -= i;
	    } while (curwin->w_botline <= curbuf->b_ml.ml_line_count);

#ifndef KEEP_SCREEN_LINE
	    if (curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	    {
		++curwin->w_cursor.lnum;
		curwin->w_valid &= ~(VALID_VIRTCOL|VALID_CHEIGHT|VALID_WCOL);
	    }
#endif
	}

#ifndef KEEP_SCREEN_LINE
	/*
	 * When hit bottom of the file: move cursor down.
	 */
	if (n > 0)
	{
	    curwin->w_cursor.lnum += n;
	    check_cursor_lnum();
	}
#else
	/* try to put the cursor in the same screen line */
	while ((curwin->w_cursor.lnum < curwin->w_topline || scrolled > 0)
			     && curwin->w_cursor.lnum < curwin->w_botline - 1)
	{
	    scrolled -= plines(curwin->w_cursor.lnum);
	    if (scrolled < 0 && curwin->w_cursor.lnum >= curwin->w_topline)
		break;
#ifdef FEAT_FOLDING
	    (void)hasFolding(curwin->w_cursor.lnum, NULL,
						      &curwin->w_cursor.lnum);
#endif
	    ++curwin->w_cursor.lnum;
	}
#endif
    }
    else	    /* scroll up */
    {
	while (n > 0 && curwin->w_topline > 1)
	{
	    i = plines(curwin->w_topline - 1);
	    n -= i;
	    if (n < 0 && scrolled)
		break;
	    scrolled += i;
	    --curwin->w_topline;
#ifdef FEAT_FOLDING
	    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
	    curwin->w_valid &= ~(VALID_CROW|VALID_WROW|
					      VALID_BOTLINE|VALID_BOTLINE_AP);
#ifndef KEEP_SCREEN_LINE
	    if (curwin->w_cursor.lnum > 1)
	    {
		--curwin->w_cursor.lnum;
		curwin->w_valid &= ~(VALID_VIRTCOL|VALID_CHEIGHT|VALID_WCOL);
	    }
#endif
	}
#ifndef KEEP_SCREEN_LINE
	/*
	 * When hit top of the file: move cursor up.
	 */
	if (n > 0)
	{
	    if (curwin->w_cursor.lnum > (linenr_t)n)
		curwin->w_cursor.lnum -= n;
	    else
		curwin->w_cursor.lnum = 1;
	}
#else
	/* try to put the cursor in the same screen line */
	scrolled += n;	    /* move cursor when topline is 1 */
	while (curwin->w_cursor.lnum > curwin->w_topline &&
		 (scrolled > 0 || curwin->w_cursor.lnum >= curwin->w_botline))
	{
	    scrolled -= plines(curwin->w_cursor.lnum - 1);
	    if (scrolled < 0 && curwin->w_cursor.lnum < curwin->w_botline)
		break;
	    --curwin->w_cursor.lnum;
# ifdef FEAT_FOLDING
	    foldAdjustCursor();
# endif
	}
#endif
    }
# ifdef FEAT_FOLDING
    /* Move cursor to first line of closed fold. */
    foldAdjustCursor();
# endif
    cursor_correct();
    beginline(BL_SOL | BL_FIX);
    redraw_later(VALID);
}
