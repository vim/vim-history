/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * screen.c: code for displaying on the screen
 */

#include "vim.h"

/*
 * The attributes that are actually active for writing to the screen.
 */
static int	screen_attr = 0;

/*
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
static int	screen_cur_row, screen_cur_col;	/* last known cursor position */

#ifdef EXTRA_SEARCH
/*
 * When highlighting matches for the last use search pattern:
 * - search_hl_prog points to the regexp program for it
 * - search_hl_attr contains the attributes to be used
 * - search_hl_ic is the value for "reg_ic" for this search
 */
vim_regexp	*search_hl_prog = NULL;
int		search_hl_attr;
int		search_hl_ic;
#endif

/*
 * Flags for w_valid.
 * These are suppose to be used only in screen.c.  From other files, use the
 * functions that set or reset them.
 *
 * VALID_BOTLINE    VALID_BOTLINE_AP
 *     on		on		w_botline valid
 *     off		on		w_botline approximated
 *     off		off		w_botline not valid
 *     on		off		not possible
 */
#define VALID_WROW	0x01	/* w_wrow (window row) is valid */
#define VALID_WCOL	0x02	/* w_wcol (window col) is valid */
#define VALID_VIRTCOL	0x04	/* w_virtcol (file col) is valid */
#define VALID_CHEIGHT	0x08	/* w_cline_height is valid */
#define VALID_CROW	0x10	/* w_cline_row is valid */
#define VALID_BOTLINE	0x20	/* w_botine and w_empty_rows are valid */
#define VALID_BOTLINE_AP 0x40	/* w_botine is approximated */

/*
 * Buffer for one screen line.
 */
static char_u *current_LinePointer;

static void win_update __ARGS((WIN *wp));
static int win_line __ARGS((WIN *, linenr_t, int, int));
#ifdef RIGHTLEFT
static void screen_line __ARGS((int row, int endcol, int clear_rest, int rlflag));
#define SCREEN_LINE(r, e, c, rl)    screen_line((r), (e), (c), (rl))
#else
static void screen_line __ARGS((int row, int endcol, int clear_rest));
#define SCREEN_LINE(r, e, c, rl)    screen_line((r), (e), (c))
#endif
#ifdef EXTRA_SEARCH
static void start_search_hl __ARGS((void));
static void end_search_hl __ARGS((void));
#endif
static void screen_start_highlight __ARGS((int attr));
static void comp_botline __ARGS((void));
static void screen_char __ARGS((char_u *, int, int));
static void screenclear2 __ARGS((void));
static void lineclear __ARGS((char_u *p));
static void check_cursor_moved __ARGS((WIN *wp));
static void curs_rows __ARGS((int do_botline));
static void validate_virtcol_win __ARGS((WIN *wp));
static int screen_ins_lines __ARGS((int, int, int, int));
static int highlight_status __ARGS((int *attr));
static void win_redr_ruler __ARGS((WIN *wp, int always));
static void intro_message __ARGS((void));

/*
 * update_screenline() - like update_screen() but only for cursor line
 *
 * Must only be called when something in the cursor line has changed (e.g.
 * character inserted or deleted).
 *
 * Check if the size of the cursor line has changed since the last screen
 * update.  If it did change, lines below the cursor will move up or down and
 * we need to call the routine update_screen() to examine the entire screen.
 */
    void
update_screenline()
{
    int		row;
    int		old_cline_height;

    if (!screen_valid(TRUE))
	return;

    if (must_redraw)			/* must redraw whole screen */
    {
	update_screen(must_redraw);
	return;
    }

    if (!redrawing())
    {
	redraw_later(NOT_VALID);	/* remember to update later */
	return;
    }

    /*
     * If the screen has scrolled, or some lines after the cursor line have
     * been invalidated, call update_screen().
     */
    if (curwin->w_lsize_valid <= curwin->w_cursor.lnum - curwin->w_topline ||
				curwin->w_lsize_lnum[0] != curwin->w_topline)
    {
	update_screen(VALID_TO_CURSCHAR);
	return;
    }

    /*
     * Get the current height of the cursor line, as it is on the screen.
     * w_lsize[] must be used here, since w_cline_height might already have
     * been updated to the new height of the line in the buffer.
     */
    old_cline_height = curwin->w_lsize[curwin->w_cursor.lnum
							  - curwin->w_topline];

    /*
     * Check if the cursor line is still at the same position.	Be aware of
     * the cursor having moved around, w_cline_row may be invalid, use the
     * values from w_lsize[] by calling curs_rows().
     */
    check_cursor_moved(curwin);
    if (!(curwin->w_valid & VALID_CROW))
	curs_rows(FALSE);
#ifdef EXTRA_SEARCH
    start_search_hl();
#endif

    /*
     * w_virtcol needs to be valid.
     */
    validate_virtcol();
    cursor_off();
    row = win_line(curwin, curwin->w_cursor.lnum,
		curwin->w_cline_row, curwin->w_height);

    display_hint = HINT_NONE;
#ifdef EXTRA_SEARCH
    end_search_hl();
#endif

    if (row == curwin->w_height + 1)	/* line too long for window */
    {
	if (curwin->w_topline < curwin->w_cursor.lnum)
	{
	    /*
	     * Window needs to be scrolled up to show the cursor line.
	     * We know w_botline was valid before the change, so it should now
	     * be one less.  This removes the need to recompute w_botline in
	     * update_topline().
	     */
	    --curwin->w_botline;
	    curwin->w_valid |= VALID_BOTLINE_AP;
	}
	update_topline();
	update_screen(VALID_TO_CURSCHAR);
    }
    else if (!dollar_vcol)
    {
	/*
	 * If the cursor line changed size, delete or insert screen lines and
	 * redraw the rest of the window.
	 */
	if (old_cline_height != curwin->w_cline_height)
	{
	    if (curwin->w_cline_height < old_cline_height)
		win_del_lines(curwin, row,
		      old_cline_height - curwin->w_cline_height, FALSE, TRUE);
	    else
		win_ins_lines(curwin,
				 curwin->w_cline_row + curwin->w_cline_height,
		      curwin->w_cline_height - old_cline_height, FALSE, TRUE);
	    update_screen(VALID_TO_CURSCHAR);
	}
#ifdef SYNTAX_HL
	/*
	 * If syntax lost its sync, have to redraw the following lines.
	 */
	else if (syntax_present(curbuf) && row < cmdline_row &&
			      syntax_check_changed(curwin->w_cursor.lnum + 1))
	    update_screen(VALID_TO_CURSCHAR);
#endif
	else if (clear_cmdline || redraw_cmdline)
	    showmode();		    /* clear cmdline, show mode and ruler */
    }
}

/*
 * Redraw the current window later, with UpdateScreen(type).
 * Set must_redraw only of not already set to a higher value.
 * e.g. if must_redraw is CLEAR, type == NOT_VALID will do nothing.
 */
    void
redraw_later(type)
    int	    type;
{
    if (curwin->w_redr_type < type)
	curwin->w_redr_type = type;
    if (must_redraw < type)	/* must_redraw is the maximum of all windows */
	must_redraw = type;
}

/*
 * Mark all windows to be redrawn later.
 */
    void
redraw_all_later(type)
    int		type;
{
    WIN		    *wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_redr_type < type)
	    wp->w_redr_type = type;
    redraw_later(type);
}

/*
 * Mark all windows that are editing the current buffer to be udpated later.
 */
    void
redraw_curbuf_later(type)
    int		type;
{
    WIN		    *wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_redr_type < type && wp->w_buffer == curbuf)
	    wp->w_redr_type = type;
    redraw_later(type);
}

/*
 * update all windows that are editing the current buffer
 */
    void
update_curbuf(type)
    int		type;
{
    redraw_curbuf_later(type);
    update_screen(type);
}

/*
 * update_screen()
 *
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to NextScreen, and update curwin->w_botline.
 */

    void
update_screen(type)
    int		    type;
{
    WIN		    *wp;
    static int	    did_intro = FALSE;

    if (!screen_valid(TRUE))
	return;

    dollar_vcol = 0;

    if (must_redraw)
    {
	if (type < must_redraw)	    /* use maximal type */
	    type = must_redraw;
	must_redraw = 0;
    }

    if (curwin->w_lsize_valid == 0 && type < NOT_VALID)
	type = NOT_VALID;

    if (!redrawing())
    {
	redraw_later(type);		/* remember type for next time */
	curwin->w_redr_type = type;
	curwin->w_lsize_valid = 0;	/* don't use w_lsize[] now */
	return;
    }

    /*
     * if the screen was scrolled up when displaying a message, scroll it down
     */
    if (msg_scrolled)
    {
	clear_cmdline = TRUE;
	if (msg_scrolled > Rows - 5)	    /* clearing is faster */
	    type = CLEAR;
	else if (type != CLEAR)
	{
	    check_for_delay(FALSE);
	    if (screen_ins_lines(0, 0, msg_scrolled, (int)Rows) == FAIL)
		type = CLEAR;
	    win_rest_invalid(firstwin);	    /* should do only first/last few */
	}
	msg_scrolled = 0;
	need_wait_return = FALSE;
    }

    /* reset cmdline_row now (may have been changed temporarily) */
    compute_cmdrow();

    /* Check for changed highlighting */
    if (need_highlight_changed)
	highlight_changed();

    if (type == CLEAR)		/* first clear screen */
    {
	screenclear();		/* will reset clear_cmdline */
	type = NOT_VALID;
    }

    if (clear_cmdline)		/* first clear cmdline */
    {
	check_for_delay(FALSE);
	msg_row = cmdline_row;
	msg_col = 0;
	msg_clr_eos();		/* will reset clear_cmdline */
    }

    /*
     * Only start redrawing if there is really something to do.
     */
    if (type == INVERTED)
	update_curswant();
    if (!((type == VALID && curwin->w_topline == curwin->w_lsize_lnum[0])
	    || (type == INVERTED
		&& curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		&& (curwin->w_valid & VALID_VIRTCOL)
		&& curwin->w_old_curswant == curwin->w_curswant)))
    {
	/*
	 * go from top to bottom through the windows, redrawing the ones that
	 * need it
	 */
	curwin->w_redr_type = type;
	cursor_off();
#ifdef EXTRA_SEARCH
	start_search_hl();
#endif
	for (wp = firstwin; wp; wp = wp->w_next)
	{
	    if (wp->w_redr_type)
		win_update(wp);
	    if (wp->w_redr_status)
		win_redr_status(wp);
	}
#ifdef EXTRA_SEARCH
	end_search_hl();
#endif
    }
    if (redraw_cmdline)
	showmode();

    display_hint = HINT_NONE;

    /* May put up an introductory message when not editing a file */
    if (!did_intro && bufempty()
	    && curbuf->b_fname == NULL
	    && firstwin->w_next == NULL
	    && vim_strchr(p_shm, SHM_INTRO) == NULL)
	intro_message();
    did_intro = TRUE;
}

#ifdef USE_GUI
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    void
updateWindow(wp)
    WIN	    *wp;
{
    cursor_off();
#ifdef EXTRA_SEARCH
    start_search_hl();
#endif
    win_update(wp);
    if (wp->w_redr_status)
	win_redr_status(wp);
    if (redraw_cmdline)
	showmode();
#ifdef EXTRA_SEARCH
    end_search_hl();
#endif
}
#endif

/*
 * Update all windows for the current buffer, except curwin.
 * Used after modifying text, to update the other windows on the same buffer.
 */
    void
update_other_win()
{
    WIN	    *wp;
    int	    first = TRUE;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp != curwin && wp->w_buffer == curbuf)
	{
	    if (first)
	    {
		cursor_off();
#ifdef EXTRA_SEARCH
		start_search_hl();
#endif
		first = FALSE;
	    }
	    wp->w_redr_type = NOT_VALID;
	    /*
	     * don't do the actual redraw if wait_return() has just been
	     * called and the user typed a ":"
	     */
	    if (!skip_redraw)
		win_update(wp);
	}

#ifdef EXTRA_SEARCH
    end_search_hl();
#endif
}

/*
 * Update a single window.
 *
 * This may cause the windows below it also to be redrawn.
 */
    static void
win_update(wp)
    WIN	    *wp;
{
    int		    type = wp->w_redr_type;
    int		    row;
    int		    endrow;
    linenr_t	    lnum;
    linenr_t	    lastline;	    /* only valid if endrow != Rows -1 */
    int		    done;	    /* if TRUE, we hit the end of the file */
    int		    didline;	    /* if TRUE, we finished the last line */
    int		    srow = 0;	    /* starting row of the current line */
    int		    idx;
    int		    i;
    long	    j;
    static int	    recursive = FALSE;	/* being called recursively */
    int		    old_botline = wp->w_botline;
    int		    must_start_top = FALSE; /* update must start at top row */
    int		    must_end_bot = FALSE;   /* update must end at bottom row */

    if (type == NOT_VALID)
    {
	wp->w_redr_status = TRUE;
	wp->w_lsize_valid = 0;
    }

    idx = 0;
    row = 0;
    lnum = wp->w_topline;
    validate_virtcol_win(wp);

    /* The number of rows shown is w_height. */
    /* The default last row is the status/command line. */
    endrow = wp->w_height;

    /*
     * If there are no changes on the screen, handle two special cases:
     * 1: we are off the top of the screen by a few lines: scroll down
     * 2: wp->w_topline is below wp->w_lsize_lnum[0]: may scroll up
     */
    if (type == VALID || type == VALID_TO_CURSCHAR ||
	    type == VALID_BEF_CURSCHAR || type == INVERTED)
    {
	if (wp->w_topline < wp->w_lsize_lnum[0])    /* may scroll down */
	{
	    j = wp->w_lsize_lnum[0] - wp->w_topline;
	    if (j < wp->w_height - 2		    /* not too far off */
		    && (type != VALID_TO_CURSCHAR
			       || wp->w_lsize_lnum[0] < curwin->w_cursor.lnum)
		    && (type != VALID_BEF_CURSCHAR
			  || wp->w_lsize_lnum[0] < curwin->w_cursor.lnum - 1))
	    {
		lastline = wp->w_lsize_lnum[0] - 1;
		i = plines_m_win(wp, wp->w_topline, lastline);
		if (i < wp->w_height - 2)	/* less than a screen off */
		{
		    /*
		     * Try to insert the correct number of lines.
		     * If not the last window, delete the lines at the bottom.
		     * win_ins_lines may fail.
		     */
		    if (i > 0)
			check_for_delay(FALSE);
		    if (win_ins_lines(wp, 0, i, FALSE, wp == firstwin) == OK
							 && wp->w_lsize_valid)
		    {
			must_start_top = TRUE;
			endrow = i;

			if ((wp->w_lsize_valid += j) > wp->w_height)
			    wp->w_lsize_valid = wp->w_height;
			for (idx = wp->w_lsize_valid; idx - j >= 0; idx--)
			{
			    wp->w_lsize_lnum[idx] = wp->w_lsize_lnum[idx - j];
			    wp->w_lsize[idx] = wp->w_lsize[idx - j];
			}
			idx = 0;
		    }
		}
		else if (lastwin == firstwin)
		{
		    screenclear();  /* far off: clearing the screen is faster */
		    must_start_top = TRUE;
		    must_end_bot = TRUE;
		}
	    }
	    else if (lastwin == firstwin)
	    {
		screenclear();	    /* far off: clearing the screen is faster */
		must_start_top = TRUE;
		must_end_bot = TRUE;
	    }
	}
	else				/* may scroll up */
	{
	    j = -1;
			/* try to find wp->w_topline in wp->w_lsize_lnum[] */
	    for (i = 0; i < wp->w_lsize_valid; i++)
	    {
		if (wp->w_lsize_lnum[i] == wp->w_topline)
		{
		    j = i;
		    break;
		}
		row += wp->w_lsize[i];
	    }
	    if (j == -1)    /* wp->w_topline is not in wp->w_lsize_lnum */
	    {
		row = 0;    /* start at the first row */
		if (lastwin == firstwin)
		{
		    screenclear();  /* far off: clearing the screen is faster */
		    must_start_top = TRUE;
		    must_end_bot = TRUE;
		}
	    }
	    else
	    {
		/*
		 * Try to delete the correct number of lines.
		 * wp->w_topline is at wp->w_lsize_lnum[i].
		 */
		if (row)
		{
		    check_for_delay(FALSE);
		    if (win_del_lines(wp, 0, row, FALSE, wp == firstwin) == OK)
			must_end_bot = TRUE;
		}
		if ((row == 0 || must_end_bot) && wp->w_lsize_valid)
		{
		    /*
		     * Skip the lines (below the deleted lines) that are still
		     * valid and don't need redrawing.	Copy their info
		     * upwards, to compensate for the deleted lines.  Leave
		     * row and lnum on the first line that needs redrawing.
		     */
		    srow = row;
		    row = 0;
		    for (;;)
		    {
			if ((type == VALID_TO_CURSCHAR &&
						 lnum == wp->w_cursor.lnum) ||
				(type == VALID_BEF_CURSCHAR &&
					       lnum == wp->w_cursor.lnum - 1))
			{
			    wp->w_lsize_valid = idx;
			    break;
			}
			wp->w_lsize[idx] = wp->w_lsize[j];
			wp->w_lsize_lnum[idx] = lnum;
			if (row + srow + (int)wp->w_lsize[j] > wp->w_height)
			{
			    wp->w_lsize_valid = idx + 1;
			    break;
			}
			++lnum;

			row += wp->w_lsize[idx++];
			if ((int)++j >= wp->w_lsize_valid)
			{
			    wp->w_lsize_valid = idx;
			    break;
			}
		    }
		}
		else
		    row = 0;	    /* update all lines */
	    }
	}
	if (endrow == wp->w_height && idx == 0)	    /* no scrolling */
	    wp->w_lsize_valid = 0;
    }

    done = didline = FALSE;

    if (VIsual_active)	    /* check if we are updating the inverted part */
    {
	linenr_t    from, to;

	/*
	 * Find the line numbers that need to be updated: The lines between
	 * the old cursor position and the current cursor position.  Also
	 * check if the Visual position changed.
	 */
	if (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
	{
	    from = curwin->w_cursor.lnum;
	    to = wp->w_old_cursor_lnum;
	}
	else
	{
	    from = wp->w_old_cursor_lnum;
	    to = curwin->w_cursor.lnum;
	}

	if (VIsual.lnum != wp->w_old_visual_lnum)
	{
	    if (wp->w_old_visual_lnum < from)
		from = wp->w_old_visual_lnum;
	    if (wp->w_old_visual_lnum > to)
		to = wp->w_old_visual_lnum;
	    if (VIsual.lnum < from)
		from = VIsual.lnum;
	    if (VIsual.lnum > to)
		to = VIsual.lnum;
	}

	/*
	 * If in block mode and changed column or curwin->w_curswant: update
	 * all lines.
	 * First compute the actual start and end column.
	 */
	if (VIsual_mode == Ctrl('V'))
	{
	    colnr_t	from1, from2, to1, to2;

	    getvcol(wp, &VIsual, &from1, NULL, &to1);
	    getvcol(wp, &curwin->w_cursor, &from2, NULL, &to2);
	    if (from2 < from1)
		from1 = from2;
	    if (to2 > to1)
		to1 = to2;
	    ++to1;
	    if (curwin->w_curswant == MAXCOL)
		to1 = MAXCOL;

	    if (from1 != wp->w_old_cursor_fcol || to1 != wp->w_old_cursor_lcol)
	    {
		if (from > VIsual.lnum)
		    from = VIsual.lnum;
		if (to < VIsual.lnum)
		    to = VIsual.lnum;
	    }
	    wp->w_old_cursor_fcol = from1;
	    wp->w_old_cursor_lcol = to1;
	}

	/*
	 * There is no need to update lines above the top of the window.
	 * If "must_start_top" is set, always start at w_topline.
	 */
	if (from < wp->w_topline || must_start_top)
	    from = wp->w_topline;

	/*
	 * If we know the value of w_botline, use it to restrict the update to
	 * the lines that are visible in the window.
	 */
	if (wp->w_valid & VALID_BOTLINE)
	{
	    if (from >= wp->w_botline)
		from = wp->w_botline - 1;
	    if (to >= wp->w_botline)
		to = wp->w_botline - 1;
	}

	/*
	 * Find the minimal part to be updated.
	 *
	 * If "lnum" is past "from" already, start at w_topline (can
	 * happen when scrolling).
	 */
	if (lnum > from)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    row = 0;
	}
	while (lnum < from && idx < wp->w_lsize_valid)	    /* find start */
	{
	    row += wp->w_lsize[idx++];
	    ++lnum;
	}
	if (!must_end_bot && !must_start_top)
	{
	    srow = row;
	    for (j = idx; j < wp->w_lsize_valid; ++j)	    /* find end */
	    {
		if (wp->w_lsize_lnum[j] == to + 1)
		{
		    endrow = srow;
		    break;
		}
		srow += wp->w_lsize[j];
	    }
	}
	else
	{
	    /* Redraw until the end, otherwise we could miss something when
	     * doing CTRL-U. */
	    endrow = wp->w_height;
	}

	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_curswant = curwin->w_curswant;
    }
    else
    {
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
    }

    /*
     * Update the screen rows from "row" to "endrow".
     * Start at line "lnum" which is at wp->w_lsize_lnum[idx].
     */
    for (;;)
    {
	if (row == endrow)
	{
	    didline = TRUE;
	    break;
	}

	if (lnum > wp->w_buffer->b_ml.ml_line_count)
	{
	    done = TRUE;	/* hit the end of the file */
	    break;
	}
	srow = row;
	row = win_line(wp, lnum, srow, endrow);
	if (row > endrow)	/* past end of screen */
	{
	    /* we may need the size of that too long line later on */
	    wp->w_lsize[idx] = plines_win(wp, lnum);
	    wp->w_lsize_lnum[idx++] = lnum;
	    break;
	}

	wp->w_lsize[idx] = row - srow;
	wp->w_lsize_lnum[idx++] = lnum;
	if (++lnum > wp->w_buffer->b_ml.ml_line_count)
	{
	    done = TRUE;
	    break;
	}
    }
    if (idx > wp->w_lsize_valid)
	wp->w_lsize_valid = idx;

    /* Do we have to do off the top of the screen processing ? */
    if (endrow != wp->w_height)
    {
	row = 0;
	for (idx = 0; idx < wp->w_lsize_valid && row < wp->w_height; idx++)
	    row += wp->w_lsize[idx];

	if (row < wp->w_height)
	{
	    done = TRUE;
	}
	else if (row > wp->w_height)	/* Need to blank out the last line */
	{
	    lnum = wp->w_lsize_lnum[idx - 1];
	    srow = row - wp->w_lsize[idx - 1];
	    didline = FALSE;
	}
	else
	{
	    lnum = wp->w_lsize_lnum[idx - 1] + 1;
	    didline = TRUE;
	}
    }

    wp->w_empty_rows = 0;
    /*
     * If we didn't hit the end of the file, and we didn't finish the last
     * line we were working on, then the line didn't fit.
     */
    if (!done && !didline)
    {
	if (lnum == wp->w_topline)
	{
	    /*
	     * Single line that does not fit!
	     * Fill last line with '@' characters.
	     */
	    screen_fill(wp->w_winpos + wp->w_height - 1,
		    wp->w_winpos + wp->w_height, 0, (int)Columns, '@', '@',
		    highlight_attr[HLF_AT]);
	    wp->w_botline = lnum + 1;
	}
	else
	{
	    /*
	     * Clear the rest of the screen and mark the unused lines.
	     */
#ifdef RIGHTLEFT
	    if (wp->w_p_rl)
	    {
		screen_fill(wp->w_winpos + srow,
			wp->w_winpos + wp->w_height, 0, (int)Columns - 1,
			' ', ' ', highlight_attr[HLF_AT]);
		screen_fill(wp->w_winpos + srow,
			wp->w_winpos + wp->w_height, (int)Columns - 1,
			(int)Columns, '@', ' ', highlight_attr[HLF_AT]);
	    }
	    else
#endif
		screen_fill(wp->w_winpos + srow,
			wp->w_winpos + wp->w_height, 0, (int)Columns, '@', ' ',
			highlight_attr[HLF_AT]);
	    wp->w_botline = lnum;
	    wp->w_empty_rows = wp->w_height - srow;
	}
    }
    else
    {
	/* make sure the rest of the screen is blank */
	/* put '~'s on rows that aren't part of the file. */
#ifdef RIGHTLEFT
	if (wp->w_p_rl)
	{
	    screen_fill(wp->w_winpos + row,
		    wp->w_winpos + wp->w_height, 0, (int)Columns - 1,
		    ' ', ' ', highlight_attr[HLF_AT]);
	    screen_fill(wp->w_winpos + row,
		    wp->w_winpos + wp->w_height, (int)Columns - 1,
		    (int)Columns, '~', ' ', highlight_attr[HLF_AT]);
	}
	else
#endif
	    screen_fill(wp->w_winpos + row,
			wp->w_winpos + wp->w_height, 0, (int)Columns, '~', ' ',
			highlight_attr[HLF_AT]);
	wp->w_empty_rows = wp->w_height - row;

	if (done)		/* we hit the end of the file */
	    wp->w_botline = wp->w_buffer->b_ml.ml_line_count + 1;
	else
	    wp->w_botline = lnum;
    }

    /*
     * There is a trick with w_botline.  If we invalidate it on each change
     * that might modify it, this will cause a lot of expensive calls to
     * plines() in update_topline() each time.	Therefore the value of
     * w_botline is often approximated, and this value is used to compute the
     * value of w_topline.  If the value of w_botline was wrong, check that
     * the value of w_topline is correct (cursor is on the visible part of the
     * text).  If it's not, we need to redraw again.  Mostly this just means
     * scrolling up a few lines, so it doesn't look too bad.  Only do this for
     * the current window (where changes are relevant).
     */
    wp->w_valid |= VALID_BOTLINE;
    wp->w_redr_type = 0;
    if (wp == curwin && wp->w_botline != old_botline && !recursive)
    {
	recursive = TRUE;
	update_topline();	/* may invalidate w_botline again */
	if (must_redraw)
	{
	    win_update(wp);
	    must_redraw = 0;
	}
	recursive = FALSE;
    }
}

/*
 * Display line "lnum" of window 'wp' on the screen.
 * Start at row "startrow", stop when "endrow" is reached.
 * wp->w_virtcol needs to be valid.
 *
 * Return the number of last row the line occupies.
 */

    static int
win_line(wp, lnum, startrow, endrow)
    WIN		    *wp;
    linenr_t	    lnum;
    int		    startrow;
    int		    endrow;
{
    char_u	    *screenp;
    int		    c = 0;		/* init for GCC */
    int		    col;		/* visual column on screen */
    long	    vcol;		/* visual column for tabs */
#ifdef SYNTAX_HL
    int		    rcol;		/* real column in the line */
#endif
    int		    row;		/* row in the window, excl w_winpos */
    int		    screen_row;		/* row on the screen, incl w_winpos */
    char_u	    *ptr;
#ifdef SYNTAX_HL
    char_u	    *line;
#endif
    char_u	    extra[16];		/* "%ld" must fit in here */
    char_u	    *p_extra;
    char_u	    *showbreak = NULL;
    int		    n_extra;
    int		    n_spaces = 0;

    int		    fromcol, tocol;	/* start/end of inverting */
    int		    noinvcur = FALSE;	/* don't invert the cursor */
    FPOS	    *top, *bot;
    int		    area_highlighting;	/* Visual or incsearch highlighting in
					   this line */
    int		    attr;		/* attributes for area highlighting */
    int		    area_attr = 0;	/* attributes desired by highlighting */
    int		    search_attr = 0;	/* attributes sesired by 'searchhl' */
#ifdef SYNTAX_HL
    int		    syntax_attr = 0;	/* attributes desired by syntax */
    int		    has_syntax = FALSE;	/* this buffer has syntax highl. */
#endif
    int		    has_syn_or_lbr;	/* has syntax or linebreak */
    int		    char_attr;		/* attributes for next character */
    int		    saved_attr = 0;	/* char_attr saved for showbreak */
#ifdef EXTRA_SEARCH
    char_u	    *matchp;
    char_u	    *search_hl_start = NULL;
    char_u	    *search_hl_end = NULL;
#endif

    if (startrow > endrow)		/* past the end already! */
	return startrow;

    row = startrow;
    screen_row = row + wp->w_winpos;
    attr = highlight_attr[HLF_V];

    /*
     * To speed up the loop below, set has_syn_or_lbr when there is linebreak
     * and/or syntax processing to be done.
     */
    has_syn_or_lbr = wp->w_p_lbr;
#ifdef SYNTAX_HL
    if (syntax_present(wp->w_buffer))
    {
	syntax_start(wp, lnum);
	has_syntax = TRUE;
	has_syn_or_lbr = TRUE;
    }
#endif
    col = 0;
    vcol = 0;
    fromcol = -10;
    tocol = MAXCOL;
    area_highlighting = FALSE;
    char_attr = 0;

    /*
     * handle visual active in this window
     */
    if (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
					/* Visual is after curwin->w_cursor */
	if (ltoreq(curwin->w_cursor, VIsual))
	{
	    top = &curwin->w_cursor;
	    bot = &VIsual;
	}
	else				/* Visual is before curwin->w_cursor */
	{
	    top = &VIsual;
	    bot = &curwin->w_cursor;
	}
	if (VIsual_mode == Ctrl('V'))	/* block mode */
	{
	    if (lnum >= top->lnum && lnum <= bot->lnum)
	    {
		fromcol = wp->w_old_cursor_fcol;
		tocol = wp->w_old_cursor_lcol;
	    }
	}
	else				/* non-block mode */
	{
	    if (lnum > top->lnum && lnum <= bot->lnum)
		fromcol = 0;
	    else if (lnum == top->lnum)
		getvcol(wp, top, (colnr_t *)&fromcol, NULL, NULL);
	    if (lnum == bot->lnum)
	    {
		getvcol(wp, bot, NULL, NULL, (colnr_t *)&tocol);
		++tocol;
	    }

	    if (VIsual_mode == 'V')	/* linewise */
	    {
		if (fromcol > 0)
		    fromcol = 0;
		tocol = MAXCOL;
	    }
	}
	    /* if the cursor can't be switched off, don't invert the
	     * character where the cursor is */
#ifndef MSDOS
	if (!highlight_match && *T_VI == NUL &&
			    lnum == curwin->w_cursor.lnum && wp == curwin)
	    noinvcur = TRUE;
#endif

	if (tocol <= (int)wp->w_leftcol)    /* inverting is left of screen */
	    fromcol = 0;
					/* start of invert is left of screen */
	else if (fromcol >= 0 && fromcol < (int)wp->w_leftcol)
	    fromcol = wp->w_leftcol;

	/* if inverting in this line, can't optimize cursor positioning */
	if (fromcol >= 0)
	    area_highlighting = TRUE;
    }

    /*
     * handle incremental search position highlighting
     */
    else if (highlight_match && wp == curwin && search_match_len)
    {
	if (lnum == curwin->w_cursor.lnum)
	{
	    getvcol(curwin, &(curwin->w_cursor),
					    (colnr_t *)&fromcol, NULL, NULL);
	    curwin->w_cursor.col += search_match_len;
	    getvcol(curwin, &(curwin->w_cursor),
					    (colnr_t *)&tocol, NULL, NULL);
	    curwin->w_cursor.col -= search_match_len;
	    area_highlighting = TRUE;
	    attr = highlight_attr[HLF_I];
	    if (fromcol == tocol)	/* do at least one character */
		tocol = fromcol + 1;	/* happens when past end of line */
	}
    }

    ptr = ml_get_buf(wp->w_buffer, lnum, FALSE);
#ifdef EXTRA_SEARCH
    matchp = ptr;
#endif
#ifdef SYNTAX_HL
    line = ptr;
    rcol = 0;
#endif

    if (!wp->w_p_wrap)	    /* advance to first character to be displayed */
    {
	while ((colnr_t)vcol < wp->w_leftcol && *ptr)
	{
	    vcol += win_chartabsize(wp, *ptr++, (colnr_t)vcol);
#ifdef SYNTAX_HL
	    ++rcol;
#endif
	}
	if ((colnr_t)vcol > wp->w_leftcol)
	{
	    n_spaces = vcol - wp->w_leftcol;	/* begin with some spaces */
	    vcol = wp->w_leftcol;
	}
    }

#ifdef EXTRA_SEARCH
    /*
     * Handle highlighting the last used search pattern.
     */
    if (search_hl_prog != NULL)
    {
	reg_ic = search_hl_ic;
	for (;;)
	{
	    if (vim_regexec(search_hl_prog, matchp, TRUE))
	    {
		search_hl_start = search_hl_prog->startp[0];
		search_hl_end = search_hl_prog->endp[0];
		if (search_hl_end <= ptr)    /* match before leftcol */
		{
		    if (matchp == search_hl_end) /* empty match */
			++matchp;
		    else
			matchp = search_hl_end;
		    continue;
		}
		if (search_hl_start < ptr)  /* match at leftcol */
		    search_attr = search_hl_attr;
	    }
	    else
	    {
		search_hl_start = NULL;
		search_hl_end = NULL;
	    }
	    break;
	}
	if (search_hl_start != NULL)
	    area_highlighting = TRUE;
    }
#endif

    screenp = current_LinePointer;
#ifdef RIGHTLEFT
    if (wp->w_p_rl)
    {
	col = Columns - 1;		    /* col follows screenp here */
	screenp += Columns - 1;
    }
#endif
    if (wp->w_p_nu)
    {
#ifdef RIGHTLEFT
	if (wp->w_p_rl)			    /* reverse line numbers */
	{
	    char_u *c1, *c2, t;

	    sprintf((char *)extra, " %-7ld", (long)lnum);
	    for (c1 = extra, c2 = extra + STRLEN(extra) - 1; c1 < c2;
								   c1++, c2--)
	    {
		t = *c1;
		*c1 = *c2;
		*c2 = t;
	    }
	}
	else
#endif
	    sprintf((char *)extra, "%7ld ", (long)lnum);
	p_extra = extra;
	n_extra = 8;
	vcol -= 8;	/* so vcol is 0 when line number has been printed */
    }
    else
    {
	p_extra = NULL;
	n_extra = 0;
    }

    for (;;)
    {
	if (area_highlighting)	/* Visual or match highlighting in this line */
	{
	    if (((vcol == fromcol && !(noinvcur &&
					   (colnr_t)vcol == wp->w_virtcol)) ||
		    (noinvcur && (colnr_t)vcol == wp->w_virtcol + 1 &&
			    vcol >= fromcol)) && vcol < tocol)
		area_attr = attr;		    /* start highlighting */
	    else if (area_attr && (vcol == tocol ||
				(noinvcur && (colnr_t)vcol == wp->w_virtcol)))
		area_attr = 0;			    /* stop highlighting */

#ifdef EXTRA_SEARCH
	    /*
	     * Check for start/end of search pattern match.
	     * After end, check for start/end of next match.
	     * When another match, have to check for start again.
	     * Watch out for matching an empty string!
	     */
	    if (!n_extra && !n_spaces)
	    {
		for (;;)
		{
		    if (ptr == search_hl_start)
			search_attr = search_hl_attr;
		    if (ptr == search_hl_end)
		    {
			search_attr = 0;
			reg_ic = search_hl_ic;
			if (vim_regexec(search_hl_prog, ptr, FALSE))
			{
			    search_hl_start = search_hl_prog->startp[0];
			    search_hl_end = search_hl_prog->endp[0];
			    if (search_hl_start != search_hl_end)
				continue;
			    ++search_hl_end; /* try again after empty match */
			}
		    }
		    break;
		}
	    }
#endif

	    if (area_attr)
		char_attr = area_attr;
#ifdef SYNTAX_HL
	    else if (!search_attr && has_syntax)
		char_attr = syntax_attr;
#endif
	    else
		char_attr = search_attr;
	}

    /* Get the next character to put on the screen. */

	/*
	 * If 'showbreak' is set it contains the characters to put at the
	 * start of each broken line.
	 */
	if (
#ifdef RIGHTLEFT
		(wp->w_p_rl ? col == -1 : col == Columns)
#else
		col == Columns
#endif
		&& (*ptr != NUL
		    || (wp->w_p_list && n_extra == 0)
		    || (n_extra && *p_extra)
		    || n_spaces)
		&& vcol != 0 && *p_sbr != 0)
	{
	    showbreak = p_sbr;
	    saved_attr = char_attr;	    /* save current attributes */
	}
	if (showbreak != NULL)
	{
	    if (*showbreak == NUL)
	    {
		showbreak = NULL;
		char_attr = saved_attr;	    /* restore attributes */
	    }
	    else
	    {
		c = *showbreak++;
		char_attr = highlight_attr[HLF_AT];
	    }
	}

	if (showbreak == NULL)
	{
	    /*
	     * The 'extra' array contains the extra stuff that is inserted to
	     * represent special characters (non-printable stuff).
	     */
	    if (n_extra)
	    {
		c = *p_extra++;
		n_extra--;
	    }
	    else if (n_spaces)
	    {
		c = ' ';
		n_spaces--;
	    }
	    else
	    {
		c = *ptr++;

		if (has_syn_or_lbr)
		{
#ifdef SYNTAX_HL
		    if (has_syntax)
		    {
			syntax_attr = get_syntax_attr(rcol++, line);
			if (!area_attr && !search_attr)
			    char_attr = syntax_attr;
		    }
#endif
		    /*
		     * Found last space before word: check for line break
		     */
		    if (wp->w_p_lbr && vim_isbreak(c) && !vim_isbreak(*ptr)
							  && !wp->w_p_list)
		    {
			n_spaces = win_lbr_chartabsize(wp, ptr - 1,
				(colnr_t)vcol, NULL) - 1;
			if (vim_iswhite(c))
			    c = ' ';
		    }
		}

		/*
		 * Handling of non-printable characters.
		 */
		if (!safe_vim_isprintc(c))
		{
		    /*
		     * when getting a character from the file, we may have to
		     * turn it into something else on the way to putting it
		     * into 'NextScreen'.
		     */
		    if (c == TAB && !wp->w_p_list)
		    {
			/* tab amount depends on current column */
			n_spaces = (int)wp->w_buffer->b_p_ts -
					 vcol % (int)wp->w_buffer->b_p_ts - 1;
			c = ' ';
		    }
		    else if (c == NUL && wp->w_p_list)
		    {
			p_extra = (char_u *)"";
			n_extra = 1;
			c = '$';
			--ptr;	    /* put it back at the NUL */
			char_attr = highlight_attr[HLF_AT];
		    }
		    else if (c != NUL)
		    {
			p_extra = transchar(c);
			n_extra = charsize(c) - 1;
			c = *p_extra++;
		    }
		}
	    }
	}

	/*
	 * At end of the text line.
	 */
	if (c == NUL)
	{
	    if (area_attr)
	    {
		/* invert at least one char, used for Visual and empty line or
		 * highlight match at end of line. If it's beyond the last
		 * char on the screen, just overwrite that one (tricky!) */
		if (vcol == fromcol)
		{
#ifdef RIGHTLEFT
		    if (wp->w_p_rl)
		    {
			if (col < 0)
			{
			    ++screenp;
			    ++col;
			}
		    }
		    else
#endif
		    {
			if (col >= Columns)
			{
			    --screenp;
			    --col;
			}
		    }
		    *screenp = ' ';
		    *(screenp + Columns) = char_attr;
#ifdef RIGHTLEFT
		    if (wp->w_p_rl)
			--col;
		    else
#endif
			++col;
		}
	    }

	    SCREEN_LINE(screen_row, col, TRUE, wp->w_p_rl);
	    row++;

	    /*
	     * Update w_cline_height if we can (saves a call to plines()
	     * later).
	     */
	    if (wp == curwin && lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cline_row = startrow;
		curwin->w_cline_height = row - startrow;
		curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
	    }

	    break;
	}

	/*
	 * At end of screen line.
	 */
	if (
#ifdef RIGHTLEFT
	    wp->w_p_rl ? (col < 0) :
#endif
				    (col >= Columns)
						    )
	{
	    SCREEN_LINE(screen_row, col, TRUE, wp->w_p_rl);
	    col = 0;
	    ++row;
	    ++screen_row;
	    if (!wp->w_p_wrap)
		break;
	    if (row == endrow)	    /* line got too long for screen */
	    {
		++row;
		break;
	    }

	    /*
	     * Special trick to make copy/paste of wrapped lines work with
	     * xterm/screen: write an extra character beyond the end of the
	     * line. This will work with all terminal types (regardless of the
	     * xn,am settings).
	     * Only do this on a fast tty.
	     * Only do this if the cursor is on the current line (something
	     * has been written in it).
	     * Don't do this for the GUI.
	     */
	    if (p_tf && screen_cur_row == screen_row - 1
#ifdef USE_GUI
		     && !gui.in_use
#endif
					)
	    {
		if (screen_cur_col != Columns)
		    screen_char(LinePointers[screen_row - 1] + Columns - 1,
					  screen_row - 1, (int)(Columns - 1));
		screen_char(LinePointers[screen_row],
						screen_row - 1, (int)Columns);
		screen_start();		/* don't know where cursor is now */
	    }

	    screenp = current_LinePointer;
#ifdef RIGHTLEFT
	    if (wp->w_p_rl)
	    {
		col = Columns - 1;	/* col is not used if breaking! */
		screenp += Columns - 1;
	    }
#endif
	}

	/*
	 * Store the character.
	 */
	*screenp = c;
	*(screenp + Columns) = char_attr;

#ifdef RIGHTLEFT
	if (wp->w_p_rl)
	{
	    --screenp;
	    --col;
	}
	else
#endif
	{
	    ++screenp;
	    ++col;
	}
	++vcol;

	/* stop before '$' of change command */
	if (dollar_vcol && wp == curwin && vcol >= (long)wp->w_virtcol)
	{
	    SCREEN_LINE(screen_row, col, FALSE, wp->w_p_rl);
	    break;
	}
    }

    return (row);
}

/*
 * Move one "cooked" screen line to the screen, but only the characters that
 * have actually changed.  Handle insert/delete character.
 * 'endcol' gives the columns where valid characters are.
 * 'clear_rest' is TRUE if the rest of the line needs to be cleared.
 * 'rlflag' is TRUE in a rightleft window:
 *	When TRUE and clear_rest, line is cleared in 0 -- endcol.
 *	When FALSE and clear_rest, line is cleared in endcol -- Columns-1.
 */
    static void
screen_line(row, endcol, clear_rest
#ifdef RIGHTLEFT
				    , rlflag
#endif
						)
    int	    row;
    int	    endcol;
    int	    clear_rest;
#ifdef RIGHTLEFT
    int	    rlflag;
#endif
{
    char_u	    *screenp_from;
    char_u	    *screenp_to;
    int		    col = 0;
    int		    force = FALSE;	/* force update rest of the line */

    screenp_from = current_LinePointer;
    screenp_to = LinePointers[row];

#ifdef RIGHTLEFT
    if (rlflag)
    {
	if (clear_rest)
	{
	    while (col <= endcol && *screenp_to == ' '
					      && *(screenp_to + Columns) == 0)
	    {
		++screenp_to;
		++col;
	    }
	    if (col <= endcol)
		screen_fill(row, row + 1, col, endcol + 1, ' ', ' ', 0);
	}
	col = endcol + 1;
	screenp_to = LinePointers[row] + col;
	screenp_from += col;
    }

    while (rlflag ? (col < Columns) : (col < endcol))
#else
    while (col < endcol)
#endif
    {
	if (force || *screenp_from != *screenp_to ||
		*(screenp_from + Columns) != *(screenp_to + Columns))
	{
	    /*
	     * Special handling when 'xs' termcap flag set (hpterm):
	     * Attributes for characters are stored at the position where the
	     * cursor is when writing the highlighting code.  The
	     * start-highlighting code must be written with the cursor on the
	     * first highlighted character.  The stop-highlighting code must
	     * be written with the cursor just after the last highlighted
	     * character.
	     * Overwriting a character doesn't remove it's highlighting.  Need
	     * to clear the rest of the line, and force redrawing it
	     * completely.
	     */
	    if (       p_wiv
		    && !force
#ifdef USE_GUI
		    && !gui.in_use
#endif
		    && *(screenp_to + Columns)
		    && *(screenp_from + Columns) != *(screenp_to + Columns))
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(row, col);
		out_str(T_CE);		/* clear rest of this screen line */
		screen_start();		/* don't know where cursor is now */
		force = TRUE;		/* force redraw of rest of the line */

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		if (col > 0 && *(screenp_to + Columns - 1))
		{
		    screen_attr = *(screenp_to + Columns - 1);
		    term_windgoto(row, col);
		    screen_stop_highlight();
		}
		else
		    screen_attr = 0;	    /* highlighting has stopped */
	    }
	    *screenp_to = *screenp_from;

#if defined(USE_GUI) || defined(UNIX)
	    /* The bold trick makes a single row of pixels appear in the next
	     * character.  When a bold character is removed, the next
	     * character should be redrawn too.  This happens for our own GUI
	     * and for some xterms. */
	    if (
# ifdef USE_GUI
		    gui.in_use
# endif
# if defined(USE_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    vim_is_xterm(T_NAME)
# endif
		    )
	    {
		int		n;

		n = *(screenp_to + Columns);
		if (col + 1 < Columns && (n > HL_ALL || (n & HL_BOLD)))
		    *(screenp_to + 1) = 0;
	    }
#endif
	    *(screenp_to + Columns) = *(screenp_from + Columns);
	    screen_char(screenp_to, row, col);
	}
	else if (  p_wiv
#ifdef USE_GUI
		&& !gui.in_use
#endif
		&& col > 0)
	{
	    if (*(screenp_to + Columns) == *(screenp_to + Columns - 1))
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    else if (screen_attr)
	    {
		screen_stop_highlight();
	    }
	}

	++screenp_to;
	++screenp_from;
	++col;
    }

    if (clear_rest
#ifdef RIGHTLEFT
		    && !rlflag
#endif
				   )
    {
	/* blank out the rest of the line */
	while (col < Columns && *screenp_to == ' ' &&
		*(screenp_to + Columns) == 0)
	{
	    ++screenp_to;
	    ++col;
	}
	if (col < Columns)
	    screen_fill(row, row + 1, col, (int)Columns, ' ', ' ', 0);
    }
}

/*
 * mark all status lines for redraw; used after first :cd
 */
    void
status_redraw_all()
{
    WIN	    *wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_status_height)
	{
	    wp->w_redr_status = TRUE;
	    redraw_later(NOT_VALID);
	}
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    void
win_redr_status(wp)
    WIN	    *wp;
{
    int	    row;
    char_u  *p;
    int	    len;
    int	    fillchar;
    int	    attr;

    if (wp->w_status_height)			/* if there is a status line */
    {
	fillchar = highlight_status(&attr);

	p = wp->w_buffer->b_fname;
	if (p == NULL)
	    STRCPY(NameBuff, "[No File]");
	else
	{
	    home_replace(wp->w_buffer, p, NameBuff, MAXPATHL);
	    trans_characters(NameBuff, MAXPATHL);
	}
	p = NameBuff;
	len = STRLEN(p);

	if (wp->w_buffer->b_help || buf_changed(wp->w_buffer) ||
							 wp->w_buffer->b_p_ro)
	    *(p + len++) = ' ';
	if (wp->w_buffer->b_help)
	{
	    STRCPY(p + len, "[help]");
	    len += 6;
	}
	if (buf_changed(wp->w_buffer))
	{
	    STRCPY(p + len, "[+]");
	    len += 3;
	}
	if (wp->w_buffer->b_p_ro)
	{
	    STRCPY(p + len, "[RO]");
	    len += 4;
	}

	if (len > ru_col - 1)
	{
	    p += len - (ru_col - 1);
	    *p = '<';
	    len = ru_col - 1;
	}

	row = wp->w_winpos + wp->w_height;
	screen_puts(p, row, 0, attr);
	screen_fill(row, row + 1, len, ru_col, fillchar, fillchar, attr);

	win_redr_ruler(wp, TRUE);
    }
    else    /* no status line, can only be last window */
	redraw_cmdline = TRUE;
    wp->w_redr_status = FALSE;
}

/*
 * Output a single character directly to the screen and update NextScreen.
 */
    void
screen_putchar(c, row, col, attr)
    int	    c;
    int	    row, col;
    int	    attr;
{
    char_u	buf[2];

    buf[0] = c;
    buf[1] = NUL;
    screen_puts(buf, row, col, attr);
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update NextScreen.
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if NextScreen, row and/or col is invalid, nothing is done.
 */
    void
screen_puts(text, row, col, attr)
    char_u  *text;
    int	    row;
    int	    col;
    int	    attr;
{
    char_u  *screenp;

    if (NextScreen != NULL && row < Rows)	    /* safety check */
    {
	screenp = LinePointers[row] + col;
	while (*text && col < Columns)
	{
	    if (*screenp != *text || *(screenp + Columns) != attr ||
								exmode_active)
	    {
		*screenp = *text;
		*(screenp + Columns) = attr;
		screen_char(screenp, row, col);
	    }
	    ++screenp;
	    ++col;
	    ++text;
	}
    }
}

#ifdef EXTRA_SEARCH
/*
 * Prepare for 'searchhl' highlighting.
 */
    static void
start_search_hl()
{
    if (p_hls)
    {
	search_hl_prog = last_pat_prog();
	search_hl_attr = highlight_attr[HLF_L];
	search_hl_ic = reg_ic;
    }
}

/*
 * Clean up for 'searchhl' highlighting.
 */
    static void
end_search_hl()
{
    if (search_hl_prog != NULL)
    {
	vim_free(search_hl_prog);
	search_hl_prog = NULL;
    }
}
#endif

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    void
screen_start()
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Note that the cursor has gone down to the next line, column 0.
 * Used for Ex mode.
 */
    void
screen_down()
{
    screen_cur_col = 0;
    if (screen_cur_row < Rows - 1)
	++screen_cur_row;
}

      static void
screen_start_highlight(attr)
      int	attr;
{
    struct attr_entry *aep = NULL;

    screen_attr = attr;
    if (full_screen
#ifdef WIN32
		    && termcap_active
#endif
				       )
    {
#ifdef USE_GUI
	if (gui.in_use)
	{
	    char	buf[20];

	    sprintf(buf, "\033|%dh", attr);		/* internal GUI code */
	    OUT_STR(buf);
	}
	else
#endif
	{
	    if (attr > HL_ALL)				/* special HL attr. */
	    {
		if (*T_CCO != NUL)
		    aep = syn_cterm_attr2entry(attr);
		else
		    aep = syn_term_attr2entry(attr);
		if (aep == NULL)	    /* did ":syntax clear" */
		    attr = 0;
		else
		    attr = aep->ae_attr;
	    }
	    if ((attr & HL_BOLD) && T_MD != NULL)	/* bold */
		out_str(T_MD);
	    if ((attr & HL_STANDOUT) && T_SO != NULL)	/* standout */
		out_str(T_SO);
	    if ((attr & HL_UNDERLINE) && T_US != NULL)	/* underline */
		out_str(T_US);
	    if ((attr & HL_ITALIC) && T_CZH != NULL)	/* italic */
		out_str(T_CZH);
	    if ((attr & HL_INVERSE) && T_MR != NULL)	/* inverse (reverse) */
		out_str(T_MR);

	    /*
	     * Output the color or start string after bold etc., in case the
	     * bold etc. override the color setting.
	     */
	    if (aep != NULL)
	    {
		if (*T_CCO != NUL)
		{
		    if (aep->ae_u.cterm.fg_color)
			term_fg_color(aep->ae_u.cterm.fg_color - 1);
		    if (aep->ae_u.cterm.bg_color)
			term_bg_color(aep->ae_u.cterm.bg_color - 1);
		}
		else
		{
		    if (aep->ae_u.term.start != NULL)
			out_str(aep->ae_u.term.start);
		}
	    }
	}
    }
}

      void
screen_stop_highlight()
{
    int	    do_ME = FALSE;	    /* output T_ME code */

    if (screen_attr
#ifdef WIN32
			&& termcap_active
#endif
					   )
    {
#ifdef USE_GUI
	if (gui.in_use)
	{
	    char	buf[20];

	    sprintf(buf, "\033|%dH", screen_attr);	/* internal GUI code */
	    OUT_STR(buf);
	}
	else
#endif
	{
	    if (screen_attr > HL_ALL)			/* special HL attr. */
	    {
		struct attr_entry *aep;

		if (*T_CCO != NUL)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    if (aep != NULL && (aep->ae_u.cterm.fg_color ||
						    aep->ae_u.cterm.bg_color))
			do_ME = TRUE;
		}
		else
		{
		    aep = syn_term_attr2entry(screen_attr);
		    if (aep != NULL && aep->ae_u.term.stop != NULL)
		    {
			if (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			else
			    out_str(aep->ae_u.term.stop);
		    }
		}
		if (aep == NULL)	    /* did ":syntax clear" */
		    screen_attr = 0;
		else
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    if (screen_attr & HL_STANDOUT)
	    {
		if (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		else
		    out_str(T_SE);
	    }
	    if (screen_attr & HL_UNDERLINE)
	    {
		if (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		else
		    out_str(T_UE);
	    }
	    if (screen_attr & HL_ITALIC)
	    {
		if (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		else
		    out_str(T_CZR);
	    }
	    if (do_ME || (screen_attr & HL_BOLD) || (screen_attr & HL_INVERSE))
		out_str(T_ME);

	    if (*T_CCO != NUL)
	    {
		/* set Normal cterm colors */
		if (cterm_normal_fg_color)
		    term_fg_color(cterm_normal_fg_color - 1);
		if (cterm_normal_bg_color)
		    term_bg_color(cterm_normal_bg_color - 1);
		if (cterm_normal_fg_bold)
		    out_str(T_MD);
	    }
	}
    }
    screen_attr = 0;
}

/*
 * Reset the colors for a cterm.  Used when leaving Vim.
 */
    void
reset_cterm_colors()
{
    if (*T_CCO != NUL)
    {
	/* set Normal cterm colors */
	if (cterm_normal_fg_color || cterm_normal_bg_color)
	    out_str(T_OP);
	if (cterm_normal_fg_bold)
	    out_str(T_ME);
    }
}

/*
 * put character '*p' on the screen at position 'row' and 'col'
 */
    static void
screen_char(p, row, col)
    char_u  *p;
    int	    row;
    int	    col;
{
    /*
     * Outputting the last character on the screen may scrollup the screen.
     * Don't to it!
     */
    if (col == Columns - 1 && row == Rows - 1)
	return;

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    if (screen_attr != *(p + Columns))
	screen_stop_highlight();

    windgoto(row, col);

    if (screen_attr != *(p + Columns))
	screen_start_highlight(*(p + Columns));

    out_char(*p);
    screen_cur_col++;
}

/*
 * Fill the screen from 'start_row' to 'end_row', from 'start_col' to 'end_col'
 * with character 'c1' in first column followed by 'c2' in the other columns.
 * Use attributes 'attr'.
 */
    void
screen_fill(start_row, end_row, start_col, end_col, c1, c2, attr)
    int	    start_row, end_row;
    int	    start_col, end_col;
    int	    c1, c2;
    int	    attr;
{
    int		    row;
    int		    col;
    char_u	    *screenp;
    char_u	    *attrp;
    int		    did_delete;
    int		    c;
    int		    norm_term;

    if (end_row > Rows)		    /* safety check */
	end_row = Rows;
    if (end_col > Columns)	    /* safety check */
	end_col = Columns;
    if (NextScreen == NULL ||
	    start_row >= end_row || start_col >= end_col)   /* nothing to do */
	return;

    /* it's a "normal" terminal when not in a GUI or cterm */
    norm_term = (
#ifdef USE_GUI
	    !gui.in_use &&
#endif
			    *T_CCO == NUL);
    for (row = start_row; row < end_row; ++row)
    {
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	if (c2 == ' ' && end_col == Columns && *T_CE != NUL
		&& (attr == 0 || (norm_term && attr <= HL_ALL
		    && ((attr & ~(HL_BOLD | HL_ITALIC)) == 0))))
	{
	    /*
	     * check if we really need to clear something
	     */
	    col = start_col;
	    screenp = LinePointers[row] + start_col;
	    if (c1 != ' ')			/* don't clear first char */
	    {
		++col;
		++screenp;
	    }

	    /* skip blanks (used often, keep it fast!) */
	    attrp = screenp + Columns;
	    while (col < end_col && *screenp == ' ' && *attrp == 0)
	    {
		++col;
		++screenp;
		++attrp;
	    }
	    if (col < end_col)		/* something to be cleared */
	    {
		screen_stop_highlight();
		term_windgoto(row, col);/* clear rest of this screen line */
		out_str(T_CE);
		screen_start();		/* don't know where cursor is now */
		col = end_col - col;
		while (col--)		/* clear chars in NextScreen */
		{
		    *attrp++ = 0;
		    *screenp++ = ' ';
		}
	    }
	    did_delete = TRUE;		/* the chars are cleared now */
	}

	screenp = LinePointers[row] + start_col;
	c = c1;
	for (col = start_col; col < end_col; ++col)
	{
	    if (*screenp != c || *(screenp + Columns) != attr)
	    {
		*screenp = c;
		*(screenp + Columns) = attr;
		if (!did_delete || c != ' ')
		    screen_char(screenp, row, col);
	    }
	    ++screenp;
	    if (col == start_col)
	    {
		if (did_delete)
		    break;
		c = c2;
	    }
	}
	if (row == Rows - 1)		/* overwritten the command line */
	{
	    redraw_cmdline = TRUE;
	    if (c1 == ' ' && c2 == ' ')
		clear_cmdline = FALSE;	/* command line has been cleared */
	}
    }
}

/*
 * compute wp->w_botline. Can be called after wp->w_topline changed.
 */
    static void
comp_botline()
{
    int		n;
    linenr_t	lnum;
    int		done;

    /*
     * If w_cline_row is valid, start there.
     * Otherwise have to start at w_topline.
     */
    check_cursor_moved(curwin);
    if (curwin->w_valid & VALID_CROW)
    {
	lnum = curwin->w_cursor.lnum;
	done = curwin->w_cline_row;
    }
    else
    {
	lnum = curwin->w_topline;
	done = 0;
    }

    for ( ; lnum <= curwin->w_buffer->b_ml.ml_line_count; ++lnum)
    {
	n = plines(lnum);
	if (lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cline_row = done;
	    curwin->w_cline_height = n;
	    curwin->w_valid |= (VALID_CROW|VALID_CHEIGHT);
	}
	if (done + n > curwin->w_height)
	    break;
	done += n;
    }

    /* curwin->w_botline is the line that is just below the window */
    curwin->w_botline = lnum;
    curwin->w_valid |= VALID_BOTLINE|VALID_BOTLINE_AP;

    /*
     * Also set curwin->w_empty_rows, otherwise scroll_cursor_bot() won't work
     */
    if (done == 0)
	curwin->w_empty_rows = 0;	/* single line that doesn't fit */
    else
	curwin->w_empty_rows = curwin->w_height - done;
}

    void
screenalloc(clear)
    int	    clear;
{
    int		    new_row, old_row;
    WIN		    *wp;
    int		    outofmem = FALSE;
    int		    len;
    char_u	    *new_NextScreen;
    char_u	    **new_LinePointers;
    static int	    entered = FALSE;		/* avoid recursiveness */

    /*
     * Allocation of the screen buffers is done only when the size changes
     * and when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    if ((NextScreen != NULL &&
			  Rows == screen_Rows && Columns == screen_Columns) ||
	    Rows == 0 || Columns == 0 || (!full_screen && NextScreen == NULL))
	return;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    if (entered)
	return;
    entered = TRUE;

#ifdef USE_GUI_BEOS
    vim_lock_screen();  /* be safe, put it here */
#endif

    comp_col();		/* recompute columns for shown command and ruler */

    /*
     * We're changing the size of the screen.
     * - Allocate new arrays for NextScreen.
     * - Move lines from the old arrays into the new arrays, clear extra
     *	 lines (unless the screen is going to be cleared).
     * - Free the old arrays.
     *
     * If anything fails, make NextScreen NULL, so we don't do anything!
     * Continuing with the old NextScreen may result in a crash, because the
     * size is wrong.
     */
    for (wp = firstwin; wp; wp = wp->w_next)
	win_free_lsize(wp);

    new_NextScreen = (char_u *)malloc((size_t)((Rows + 1) * Columns * 2));
    new_LinePointers = (char_u **)malloc((size_t)(sizeof(char_u *) * Rows));

    for (wp = firstwin; wp; wp = wp->w_next)
    {
	if (win_alloc_lsize(wp) == FAIL)
	{
	    outofmem = TRUE;
	    break;
	}
    }

    if (new_NextScreen == NULL || new_LinePointers == NULL || outofmem)
    {
	do_outofmem_msg();
	vim_free(new_NextScreen);
	new_NextScreen = NULL;
	vim_free(new_LinePointers);
	new_LinePointers = NULL;
    }
    else
    {
	for (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_LinePointers[new_row] = new_NextScreen + new_row * Columns * 2;

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    if (!clear)
	    {
		lineclear(new_LinePointers[new_row]);
		old_row = new_row + (screen_Rows - Rows);
		if (old_row >= 0)
		{
		    if (screen_Columns < Columns)
			len = screen_Columns;
		    else
			len = Columns;
		    vim_memmove(new_LinePointers[new_row],
			    LinePointers[old_row], (size_t)len);
		    vim_memmove(new_LinePointers[new_row] + Columns,
			    LinePointers[old_row] + screen_Columns,
								 (size_t)len);
		}
	    }
	}
	current_LinePointer = new_NextScreen + Rows * Columns * 2;
    }

    vim_free(NextScreen);
    vim_free(LinePointers);
    NextScreen = new_NextScreen;
    LinePointers = new_LinePointers;

    must_redraw = CLEAR;	/* need to clear the screen later */
    if (clear)
	screenclear2();

#ifdef USE_GUI
    else if (gui.in_use && NextScreen != NULL && Rows != screen_Rows)
    {
	(void)gui_redraw_block(0, 0, (int)Rows - 1, (int)Columns - 1);
	/*
	 * Adjust the position of the cursor, for when executing an external
	 * command.
	 */
	if (msg_row >= Rows)		    /* Rows got smaller */
	    msg_row = Rows - 1;		    /* put cursor at last row */
	else if (Rows > screen_Rows)	    /* Rows got bigger */
	    msg_row += Rows - screen_Rows;  /* put cursor in same place */
	if (msg_col >= Columns)		    /* Columns got smaller */
	    msg_col = Columns - 1;	    /* put cursor at last column */
    }
#endif

    screen_Rows = Rows;
    screen_Columns = Columns;
#ifdef USE_GUI_BEOS
    vim_unlock_screen();
#endif
    entered = FALSE;
}

    void
screenclear()
{
    check_for_delay(FALSE);
    screenalloc(FALSE);	    /* allocate screen buffers if size changed */
    screenclear2();	    /* clear the screen */
}

    static void
screenclear2()
{
    int	    i;

    if (starting || NextScreen == NULL)
	return;

    screen_stop_highlight();	/* don't want highlighting here */
    out_str(T_CL);		/* clear the display */

				/* blank out NextScreen */
    for (i = 0; i < Rows; ++i)
	lineclear(LinePointers[i]);

    screen_cleared = TRUE;	    /* can use contents of NextScreen now */

    win_rest_invalid(firstwin);
    clear_cmdline = FALSE;
    redraw_cmdline = TRUE;
    if (must_redraw == CLEAR)	    /* no need to clear again */
	must_redraw = NOT_VALID;
    compute_cmdrow();
    msg_row = cmdline_row;	    /* put cursor on last line for messages */
    msg_col = 0;
    screen_start();		    /* don't know where cursor is now */
    msg_scrolled = 0;		    /* can't scroll back */
    msg_didany = FALSE;
    msg_didout = FALSE;
}

/*
 * Clear one line in NextScreen.
 */
    static void
lineclear(p)
    char_u  *p;
{
    (void)vim_memset(p, ' ', (size_t)Columns);
    (void)vim_memset(p + Columns, 0, (size_t)Columns);
}

/*
 * Update curwin->w_topline and redraw if necessary.
 */
    void
update_topline_redraw()
{
    update_topline();
    if (must_redraw)
	update_screen(must_redraw);
}

/*
 * Update curwin->w_topline to move the cursor onto the screen.
 */
    void
update_topline()
{
    long	    line_count;
    int		    temp;
    linenr_t	    old_topline;

    if (!screen_valid(TRUE))
	return;

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
    }

    /*
     * If the cursor is above the top of the window, scroll the window to put
     * it at the top of the window.
     * If we weren't very close to begin with, we scroll to put the cursor in
     * the middle of the window.
     */
    else if (curwin->w_cursor.lnum < curwin->w_topline + p_so &&
							curwin->w_topline > 1)
    {
	temp = curwin->w_height / 2 - 1;
	if (temp < 2)
	    temp = 2;
				/* not very close, put cursor halfway screen */
	if (curwin->w_topline + p_so - curwin->w_cursor.lnum >= temp)
	    scroll_cursor_halfway(FALSE);
	else
	    scroll_cursor_top((int)p_sj, FALSE);
    }

    /*
     * If the cursor is below the bottom of the window, scroll the window
     * to put the cursor on the window. If the cursor is less than a
     * windowheight down compute the number of lines at the top which have
     * the same or more rows than the rows of the lines below the bottom.
     * When w_botline is invalid, recompute it first, to avoid a redraw later.
     * If w_botline was approximated, we might need a redraw later in a few
     * cases, but we don't want to spend (a lot of) time recomputing w_botline
     * for every small change.
     */
    else
    {
	if (!(curwin->w_valid & VALID_BOTLINE_AP))
	    validate_botline();
	if ((long)curwin->w_cursor.lnum >= (long)curwin->w_botline - p_so &&
		curwin->w_botline <= curbuf->b_ml.ml_line_count)
	{
	    line_count = curwin->w_cursor.lnum - curwin->w_botline + 1 + p_so;
	    if (line_count <= curwin->w_height + 1)
		scroll_cursor_bot((int)p_sj, FALSE);
	    else
		scroll_cursor_halfway(FALSE);
	}
    }

    /*
     * Need to redraw when topline changed.
     */
    if (curwin->w_topline != old_topline)
	redraw_later(VALID);
}

    void
update_curswant()
{
    if (curwin->w_set_curswant)
    {
	validate_virtcol();
	curwin->w_curswant = curwin->w_virtcol;
	curwin->w_set_curswant = FALSE;
    }
}

    void
windgoto(row, col)
    int	    row;
    int	    col;
{
    char_u	    *p;
    int		    i;
    int		    plan;
    int		    cost;
    int		    wouldbe_col;
    int		    noinvcurs;
    char_u	    *bs;
    int		    goto_cost;
    int		    attr;

#define GOTO_COST   7	/* asssume a term_windgoto() takes about 7 chars */
#define HIGHL_COST  5	/* assume unhighlight takes 5 chars */

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4

    if (col != screen_cur_col || row != screen_cur_row)
    {
	/* check if no cursor movement is allowed in highlight mode */
	if (screen_attr && *T_MS == NUL)
	    noinvcurs = HIGHL_COST;
	else
	    noinvcurs = 0;
	goto_cost = GOTO_COST + noinvcurs;

	/*
	 * Plan how to do the positioning:
	 * 1. Use CR to move it to column 0, same row.
	 * 2. Use T_LE to move it a few columns to the left.
	 * 3. Use NL to move a few lines down, column 0.
	 * 4. Move a few columns to the right with T_ND or by writing chars.
	 *
	 * Don't do this if the cursor went beyond the last column, the cursor
	 * position is unknown then (some terminals wrap, some don't )
	 *
	 * First check if the highlighting attibutes allow us to write
	 * characters to move the cursor to the right.
	 */
	if (row >= screen_cur_row && screen_cur_col < Columns)
	{
	    /*
	     * If the cursor is in the same row, bigger col, we can use CR
	     * or T_LE.
	     */
	    bs = NULL;			    /* init for GCC */
	    attr = screen_attr;
	    if (row == screen_cur_row && col < screen_cur_col)
	    {
		/* "le" is preferred over "bc", because "bc" is obsolete */
		if (*T_LE)
		    bs = T_LE;		    /* "cursor left" */
		else
		    bs = T_BC;		    /* "backspace character (old) */
		if (*bs)
		    cost = (screen_cur_col - col) * STRLEN(bs);
		else
		    cost = 999;
		if (col + 1 < cost)	    /* using CR is less characters */
		{
		    plan = PLAN_CR;
		    wouldbe_col = 0;
		    cost = 1;		    /* CR is just one character */
		}
		else
		{
		    plan = PLAN_LE;
		    wouldbe_col = col;
		}
		if (noinvcurs)		    /* will stop highlighting */
		{
		    cost += noinvcurs;
		    attr = 0;
		}
	    }

	    /*
	     * If the cursor is above where we want to be, we can use CR LF.
	     */
	    else if (row > screen_cur_row)
	    {
		plan = PLAN_NL;
		wouldbe_col = 0;
		cost = (row - screen_cur_row) * 2;  /* CR LF */
		if (noinvcurs)		    /* will stop highlighting */
		{
		    cost += noinvcurs;
		    attr = 0;
		}
	    }

	    /*
	     * If the cursor is in the same row, smaller col, just use write.
	     */
	    else
	    {
		plan = PLAN_WRITE;
		wouldbe_col = screen_cur_col;
		cost = 0;
	    }

	    /*
	     * Check if any characters that need to be written have the
	     * correct attributes.
	     */
	    i = col - wouldbe_col;
	    if (i > 0)
		cost += i;
	    if (cost < goto_cost && i > 0)
	    {
		/*
		 * Check if the attributes are correct without additionally
		 * stopping highlighting.
		 */
		p = LinePointers[row] + wouldbe_col + Columns;
		while (i && *p++ == attr)
		    --i;
		if (i)
		{
		    /*
		     * Try if it works when highlighting is stopped here.
		     */
		    if (*--p == 0)
		    {
			cost += noinvcurs;
			while (i && *p++ == 0)
			    --i;
		    }
		    if (i)
			cost = 999;	/* different attributes, don't do it */
		}
	    }

	    /*
	     * We can do it without term_windgoto()!
	     */
	    if (cost < goto_cost)
	    {
		if (plan == PLAN_LE)
		{
		    if (noinvcurs)
			screen_stop_highlight();
		    while (screen_cur_col > col)
		    {
			out_str(bs);
			--screen_cur_col;
		    }
		}
		else if (plan == PLAN_CR)
		{
		    if (noinvcurs)
			screen_stop_highlight();
		    out_char('\r');
		    screen_cur_col = 0;
		}
		else if (plan == PLAN_NL)
		{
		    if (noinvcurs)
			screen_stop_highlight();
		    while (screen_cur_row < row)
		    {
			out_char('\n');
			++screen_cur_row;
		    }
		    screen_cur_col = 0;
		}

		i = col - screen_cur_col;
		if (i > 0)
		{
		    /*
		     * Use cursor-right if it's one character only.  Avoids
		     * removing a line of pixels from the last bold char, when
		     * using the bold trick in the GUI.
		     */
		    if (T_ND[0] != NUL && T_ND[1] == NUL)
		    {
			while (i--)
			    out_char(*T_ND);
		    }
		    else
		    {
			p = LinePointers[row] + screen_cur_col;
			while (i--)
			{
			    if (*(p + Columns) != screen_attr)
				screen_stop_highlight();
			    out_char(*p++);
			}
		    }
		}
	    }
	}
	else
	    cost = 999;

	if (cost >= goto_cost)
	{
	    if (noinvcurs)
		screen_stop_highlight();
	    if (row == screen_cur_row && (col > screen_cur_col) &&
								*T_CRI != NUL)
		term_cursor_right(col - screen_cur_col);
	    else
		term_windgoto(row, col);
	}
	screen_cur_row = row;
	screen_cur_col = col;
    }
}

/*
 * Set cursor to current position.
 */
    void
setcursor()
{
    if (redrawing())
    {
	validate_cursor();
	windgoto(curwin->w_winpos + curwin->w_wrow,
#ifdef RIGHTLEFT
		curwin->w_p_rl ? ((int)Columns - 1 - curwin->w_wcol) :
#endif
							    curwin->w_wcol);
    }
}

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
    int	    scrolled = 0;
    int	    extra = 0;
    int	    used;
    int	    i;
    int	    sline;	/* screen line for cursor */
    int	    old_topline = curwin->w_topline;

    /*
     * Decrease topline until:
     * - it has become 1
     * - (part of) the cursor line is moved off the screen or
     * - moved at least 'scrolljump' lines and
     * - at least 'scrolloff' lines above and below the cursor
     */
    validate_cheight();
    used = curwin->w_cline_height;
    for (sline = 1; sline < curwin->w_cursor.lnum; ++sline)
    {
	i = plines(curwin->w_cursor.lnum - sline);
	used += i;
	extra += i;
	if (extra <= p_so &&
		   curwin->w_cursor.lnum + sline < curbuf->b_ml.ml_line_count)
	    used += plines(curwin->w_cursor.lnum + sline);
	if (used > curwin->w_height)
	    break;
	if (curwin->w_cursor.lnum - sline < curwin->w_topline)
	    scrolled += i;

	/*
	 * If scrolling is needed, scroll at least 'sj' lines.
	 */
	if ((curwin->w_cursor.lnum - (sline - 1) >= curwin->w_topline ||
				      scrolled >= min_scroll) && extra > p_so)
	    break;
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
	if (curwin->w_cursor.lnum - (sline - 1) < curwin->w_topline || always)
	    curwin->w_topline = curwin->w_cursor.lnum - (sline - 1);
	if (curwin->w_topline > curwin->w_cursor.lnum)
	    curwin->w_topline = curwin->w_cursor.lnum;
	if (curwin->w_topline != old_topline)
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
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
    int		sline;		/* screen line for cursor from bottom */
    int		i;
    linenr_t	lnum;
    linenr_t	line_count;
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
		--curwin->w_topline)
	{
	    i = plines(curwin->w_topline - 1);
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
    for (sline = 1; sline < cln; ++sline)
    {
	if ((((scrolled <= 0 || scrolled >= min_scroll) && extra >= p_so) ||
		cln + sline > curbuf->b_ml.ml_line_count) &&
		cln - sline < curwin->w_botline)
	    break;
	i = plines(cln - sline);
	used += i;
	if (used > curwin->w_height)
	    break;
	if (cln - sline >= curwin->w_botline)
	{
	    scrolled += i;
	    if (cln - sline == curwin->w_botline)
		scrolled -= curwin->w_empty_rows;
	}
	if (cln + sline <= curbuf->b_ml.ml_line_count)
	{
	    i = plines(cln + sline);
	    used += i;
	    if (used > curwin->w_height)
		break;
	    if (extra < p_so || scrolled < min_scroll)
	    {
		extra += i;
		if (cln + sline >= curwin->w_botline)
		{
		    scrolled += i;
		    if (cln + sline == curwin->w_botline)
			scrolled -= curwin->w_empty_rows;
		}
	    }
	}
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
	for (i = 0, lnum = curwin->w_topline;
		i < scrolled && lnum < curwin->w_botline; ++lnum)
	    i += plines(lnum);
	if (i >= scrolled)	/* it's possible to scroll */
	    line_count = lnum - curwin->w_topline;
	else		    /* below curwin->w_botline, don't scroll */
	    line_count = 9999;
    }

    /*
     * Scroll up if the cursor is off the bottom of the screen a bit.
     * Otherwise put it at 1/2 of the screen.
     */
    if (line_count >= curwin->w_height && line_count > min_scroll)
	scroll_cursor_halfway(FALSE);
    else
	scrollup(line_count);

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
    linenr_t	cln;		    /* Cursor Line Number */

    topline = botline = cln = curwin->w_cursor.lnum;
    used = plines(cln);
    while (topline > 1)
    {
	if (below <= above)	    /* add a line below the cursor */
	{
	    if (botline + 1 <= curbuf->b_ml.ml_line_count)
	    {
		i = plines(botline + 1);
		used += i;
		if (used > curwin->w_height)
		    break;
		below += i;
		++botline;
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
	    i = plines(topline - 1);
	    used += i;
	    if (used > curwin->w_height)
		break;
	    above += i;
	    --topline;
	}
    }
    curwin->w_topline = topline;
    curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
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
    if (curwin->w_topline == 1)
    {
	above_wanted = 0;
	max_off = curwin->w_height / 2;
	if (below_wanted > max_off)
	    below_wanted = max_off;
    }
    validate_botline();
    if (curwin->w_botline == curbuf->b_ml.ml_line_count + 1)
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
    if (cln >= curwin->w_topline + above_wanted &&
				      cln < curwin->w_botline - below_wanted)
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
	    below += plines(botline);
	    --botline;
	}
	if (above < above_wanted && (above < below || below >= below_wanted))
	{
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
}


/*
 * Check if the cursor has moved.  Set the w_valid flag accordingly.
 */
    static void
check_cursor_moved(wp)
    WIN	    *wp;
{
    if (wp->w_cursor.lnum != wp->w_valid_cursor.lnum)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL
						   |VALID_CHEIGHT|VALID_CROW);
	wp->w_valid_cursor = wp->w_cursor;
	wp->w_valid_leftcol = wp->w_leftcol;
    }
    else if (wp->w_cursor.col != wp->w_valid_cursor.col
	     || wp->w_leftcol != wp->w_valid_leftcol)
    {
	wp->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL);
	wp->w_valid_cursor.col = wp->w_cursor.col;
	wp->w_valid_leftcol = wp->w_leftcol;
    }
}

/*
 * Call this function when the length of the cursor line (in screen
 * characters) has changed, and the change is before the cursor.
 * Need to take care of w_botline separately!
 */
    void
changed_cline_bef_curs()
{
    curwin->w_valid &= ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CHEIGHT);
}

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

/*
 * Call this function when the length of a line (in screen characters) above
 * the cursor have changed.
 * Need to take care of w_botline separately!
 */
    void
changed_line_abv_curs()
{
    curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_VIRTCOL|VALID_CROW|VALID_CHEIGHT);
}

/*
 * Set wp->w_topline to a certain number.
 */
    void
set_topline(wp, lnum)
    WIN		    *wp;
    linenr_t	    lnum;
{
    /* Approximate the value of w_botline */
    wp->w_botline += lnum - wp->w_topline;
    wp->w_topline = lnum;
    wp->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
}

/*
 * Make sure the value of curwin->w_botline is valid.
 */
    void
validate_botline()
{
    if (!(curwin->w_valid & VALID_BOTLINE))
	comp_botline();
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
    WIN	    *wp;
{
    wp->w_valid &= ~(VALID_BOTLINE|VALID_BOTLINE_AP);
}

/*
 * Mark curwin->w_botline as approximated (because of some small change in the
 * buffer).
 */
    void
approximate_botline()
{
    curwin->w_valid &= ~VALID_BOTLINE;
}

/*
 * Return TRUE if curwin->w_botline is valid.
 */
    int
botline_valid()
{
    return (curwin->w_valid & VALID_BOTLINE);
}

/*
 * Return TRUE if curwin->w_botline is valid or approximated.
 */
    int
botline_approximated()
{
    return (curwin->w_valid & VALID_BOTLINE_AP);
}

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
	curs_rows(FALSE);
}

/*
 * Check if w_cline_row and w_cline_height are valid, or can be made valid.
 * Can be called when topline and botline have not been updated.
 * Used to decide to redraw or keep the window update.
 *
 * Return OK if w_cline_row is valid.
 */
    int
may_validate_crow()
{
    if (curwin->w_cursor.lnum < curwin->w_topline ||
	    curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count ||
	    !(curwin->w_valid & (VALID_BOTLINE|VALID_BOTLINE_AP)) ||
	    curwin->w_cursor.lnum >= curwin->w_botline)
	return FAIL;

    check_cursor_moved(curwin);
    if ((curwin->w_valid & (VALID_CROW|VALID_CHEIGHT)) !=
			  (VALID_CROW|VALID_CHEIGHT))
	curs_rows(TRUE);
    return OK;
}

/*
 * Compute curwin->w_cline_row and curwin->w_cline_height.
 * Must only be called when w_topine is valid!
 *
 * Returns OK when cursor is in the window, FAIL when it isn't.
 */
    static void
curs_rows(do_botline)
    int	    do_botline;		/* also compute w_botline */
{
    linenr_t	lnum;
    int		i;
    int		lsize_invalid;

    /* Check if curwin->w_lsize[] is invalid */
    lsize_invalid = (!redrawing() || curwin->w_lsize_valid == 0 ||
				curwin->w_lsize_lnum[0] != curwin->w_topline);
    i = 0;
    curwin->w_cline_row = 0;
    for (lnum = curwin->w_topline; lnum < curwin->w_cursor.lnum; ++lnum)
	if (lsize_invalid)
	    curwin->w_cline_row += plines(lnum);
	else
	    curwin->w_cline_row += curwin->w_lsize[i++];

    check_cursor_moved(curwin);
    if (!(curwin->w_valid & VALID_CHEIGHT))
    {
	if (lsize_invalid)
	    curwin->w_cline_height = plines(lnum);
	/* Check for a line that is too long to fit on the last screen line. */
	else if (i > curwin->w_lsize_valid)
	    curwin->w_cline_height = 0;
	else
	    curwin->w_cline_height = curwin->w_lsize[i];
    }

    curwin->w_valid |= VALID_CROW|VALID_CHEIGHT;

    /* validate botline too, if update_screen doesn't do it */
    if (do_botline && lsize_invalid)
	validate_botline();
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
    static void
validate_virtcol_win(wp)
    WIN	    *wp;
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
    void
validate_cheight()
{
    check_cursor_moved(curwin);
    if (!(curwin->w_valid & VALID_CHEIGHT))
    {
	curwin->w_cline_height = plines(curwin->w_cursor.lnum);
	curwin->w_valid |= VALID_CHEIGHT;
    }
}

/*
 * validate w_wcol and w_virtcol only.	Only for when 'wrap' on!
 */
    void
validate_cursor_col()
{
    validate_virtcol();
    if (!(curwin->w_valid & VALID_WCOL))
    {
	curwin->w_wcol = curwin->w_virtcol;
	if (curwin->w_p_nu)
	    curwin->w_wcol += 8;

	/* long line wrapping, adjust curwin->w_wrow */
	if (curwin->w_p_wrap && curwin->w_wcol >= Columns)
	    curwin->w_wcol = curwin->w_wcol % Columns;

	curwin->w_valid |= VALID_WCOL;
    }
}

/*
 * compute curwin->w_wcol and curwin->w_virtcol.
 * Also updates curwin->w_wrow and curwin->w_cline_row.
 */
    void
curs_columns(scroll)
    int scroll;		/* when TRUE, may scroll horizontally */
{
    int	    diff;
    int	    extra;
    int	    new_leftcol;
    colnr_t startcol;
    colnr_t endcol;

    /*
     * First make sure that w_topline is valid (after moving the cursor).
     */
    update_topline();

    /*
     * Next make sure that w_cline_row is valid.
     */
    if (!(curwin->w_valid & VALID_CROW))
	curs_rows(FALSE);

    /*
     * Compute the number of virtual columns.
     */
    getvcol(curwin, &curwin->w_cursor,
				&startcol, &(curwin->w_virtcol), &endcol);

    /* remove '$' from change command when cursor moves onto it */
    if (startcol > dollar_vcol)
	dollar_vcol = 0;

    curwin->w_wcol = curwin->w_virtcol;
    if (curwin->w_p_nu)
    {
	curwin->w_wcol += 8;
	endcol += 8;
    }

    /*
     * Now compute w_wrow, counting screen lines from w_cline_row.
     */
    curwin->w_wrow = curwin->w_cline_row;

    if (curwin->w_p_wrap)	/* long line wrapping, adjust curwin->w_wrow */
    {
	while (curwin->w_wcol >= Columns)
	{
	    curwin->w_wcol -= Columns;
	    curwin->w_wrow++;
	}
    }
    else if (scroll)	/* no line wrapping, compute curwin->w_leftcol if
			 * scrolling is on.  If scrolling is off,
			 * curwin->w_leftcol is assumed to be 0 */
    {
			/* If Cursor is left of the screen, scroll rightwards */
			/* If Cursor is right of the screen, scroll leftwards */
	if ((extra = (int)startcol - (int)curwin->w_leftcol) < 0 ||
	     (extra = (int)endcol - (int)(curwin->w_leftcol + Columns) + 1) > 0)
	{
	    if (extra < 0)
		diff = -extra;
	    else
		diff = extra;

		/* far off, put cursor in middle of window */
	    if (p_ss == 0 || diff >= Columns / 2)
		new_leftcol = curwin->w_wcol - Columns / 2;
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
		curwin->w_leftcol = 0;
	    else
		curwin->w_leftcol = new_leftcol;
		    /* screen has to be redrawn with new curwin->w_leftcol */
	    redraw_later(NOT_VALID);
	}
	curwin->w_wcol -= curwin->w_leftcol;
    }
    else if (curwin->w_wcol > (int)curwin->w_leftcol)
	curwin->w_wcol -= curwin->w_leftcol;
    else
	curwin->w_wcol = 0;

	/* Cursor past end of screen */
	/* happens with line that does not fit on screen */
    if (curwin->w_wrow > curwin->w_height - 1)
	curwin->w_wrow = curwin->w_height - 1;

    curwin->w_valid |= VALID_WCOL|VALID_WROW|VALID_VIRTCOL;
}

/*
 * Scroll up 'line_count' lines.
 */
    void
scrolldown(line_count)
    long    line_count;
{
    long    done = 0;		/* total # of physical lines done */
    int	    wrow;
    int	    moved = FALSE;

    validate_cursor();		/* w_wrow needs to be valid */
    while (line_count--)
    {
	if (curwin->w_topline == 1)
	    break;
	done += plines(--curwin->w_topline);
	--curwin->w_botline;		/* approximate w_botline */
	curwin->w_valid &= ~VALID_BOTLINE;
    }
    curwin->w_wrow += done;		/* keep w_wrow updated */
    curwin->w_cline_row += done;	/* keep w_cline_row updated */

    /*
     * Compute the row number of the last row of the cursor line
     * and move it onto the screen.
     */
    wrow = curwin->w_wrow;
    if (curwin->w_p_wrap)
    {
	validate_virtcol();
	validate_cheight();
	wrow += curwin->w_cline_height - 1 - curwin->w_virtcol / Columns;
    }
    while (wrow >= curwin->w_height && curwin->w_cursor.lnum > 1)
    {
	wrow -= plines(curwin->w_cursor.lnum--);
	curwin->w_valid &=
	      ~(VALID_WROW|VALID_WCOL|VALID_CHEIGHT|VALID_CROW|VALID_VIRTCOL);
	moved = TRUE;
    }
    if (moved)
	coladvance(curwin->w_curswant);
}

    void
scrollup(line_count)
    long    line_count;
{
    curwin->w_topline += line_count;
    curwin->w_botline += line_count;	    /* approximate w_botline */
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

/*
 * Scroll the screen one line down, but don't do it if it would move the
 * cursor off the screen.
 */
    void
scrolldown_clamp()
{
    int	    end_row;

    if (curwin->w_topline == 1)
	return;

    validate_cursor();	    /* w_wrow needs to be valid */

    /*
     * Compute the row number of the last row of the cursor line
     * and make sure it doesn't go off the screen. Make sure the cursor
     * doesn't go past 'scrolloff' lines from the screen end.
     */
    end_row = curwin->w_wrow + plines(curwin->w_topline - 1);
    if (curwin->w_p_wrap)
    {
	validate_cheight();
	validate_virtcol();
	end_row += curwin->w_cline_height - 1 - curwin->w_virtcol / Columns;
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
    if (curwin->w_p_wrap)
    {
	validate_virtcol();
	start_row -= curwin->w_virtcol / Columns;
    }
    if (start_row >= p_so)
    {
	++curwin->w_topline;
	++curwin->w_botline;		/* approximate w_botline */
	curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
    }
}

/*
 * insert 'line_count' lines at 'row' in window 'wp'
 * if 'invalid' is TRUE the wp->w_lsize_lnum[] is invalidated.
 * if 'mayclear' is TRUE the screen will be cleared if it is faster than
 * scrolling.
 * Returns FAIL if the lines are not inserted, OK for success.
 */
    int
win_ins_lines(wp, row, line_count, invalid, mayclear)
    WIN	    *wp;
    int	    row;
    int	    line_count;
    int	    invalid;
    int	    mayclear;
{
    int	    did_delete;
    int	    nextrow;
    int	    lastrow;
    int	    retval;

    if (invalid)
	wp->w_lsize_valid = 0;

    if (!redrawing() || line_count <= 0 || wp->w_height < 5)
	return FAIL;

    if (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    if (mayclear && Rows - line_count < 5)  /* only a few lines left: redraw is faster */
    {
	screenclear();	    /* will set wp->w_lsize_valid to 0 */
	return FAIL;
    }

    /*
     * Delete all remaining lines
     */
    if (row + line_count >= wp->w_height)
    {
	screen_fill(wp->w_winpos + row, wp->w_winpos + wp->w_height,
						0, (int)Columns, ' ', ' ', 0);
	return OK;
    }

    /*
     * when scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     */
    clear_cmdline = TRUE;

    /*
     * if the terminal can set a scroll region, use that
     */
    if (scroll_region)
    {
	scroll_region_set(wp, row);
	retval = screen_ins_lines(wp->w_winpos + row, 0, line_count,
							  wp->w_height - row);
	scroll_region_reset();
	return retval;
    }

    if (wp->w_next && p_tf)	/* don't delete/insert on fast terminal */
	return FAIL;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to avoid
     * messing up those windows, better just redraw.
     */
    did_delete = FALSE;
    if (wp->w_next || wp->w_status_height)
    {
	if (screen_del_lines(0, wp->w_winpos + wp->w_height - line_count,
					  line_count, (int)Rows, FALSE) == OK)
	    did_delete = TRUE;
	else if (wp->w_next)
	    return FAIL;
    }
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    if (!did_delete)
    {
	wp->w_redr_status = TRUE;
	redraw_cmdline = TRUE;
	nextrow = wp->w_winpos + wp->w_height + wp->w_status_height;
	lastrow = nextrow + line_count;
	if (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - line_count, lastrow - line_count,
						0, (int)Columns, ' ', ' ', 0);
    }

    if (screen_ins_lines(0, wp->w_winpos + row, line_count, (int)Rows) == FAIL)
    {
	    /* deletion will have messed up other windows */
	if (did_delete)
	{
	    wp->w_redr_status = TRUE;
	    win_rest_invalid(wp->w_next);
	}
	return FAIL;
    }

    return OK;
}

/*
 * delete 'line_count' lines at 'row' in window 'wp'
 * If 'invalid' is TRUE curwin->w_lsize_lnum[] is invalidated.
 * If 'mayclear' is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    int
win_del_lines(wp, row, line_count, invalid, mayclear)
    WIN		    *wp;
    int		    row;
    int		    line_count;
    int		    invalid;
    int		    mayclear;
{
    int		retval;

    if (invalid)
	wp->w_lsize_valid = 0;

    if (!redrawing() || line_count <= 0)
	return FAIL;

    if (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    /* only a few lines left: redraw is faster */
    if (mayclear && Rows - line_count < 5)
    {
	screenclear();	    /* will set wp->w_lsize_valid to 0 */
	return FAIL;
    }

    /*
     * Delete all remaining lines
     */
    if (row + line_count >= wp->w_height)
    {
	screen_fill(wp->w_winpos + row, wp->w_winpos + wp->w_height,
						0, (int)Columns, ' ', ' ', 0);
	return OK;
    }

    /*
     * when scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     */
    clear_cmdline = TRUE;

    /*
     * if the terminal can set a scroll region, use that
     */
    if (scroll_region)
    {
	scroll_region_set(wp, row);
	retval = screen_del_lines(wp->w_winpos + row, 0, line_count,
						   wp->w_height - row, FALSE);
	scroll_region_reset();
	return retval;
    }

    if (wp->w_next && p_tf)	/* don't delete/insert on fast terminal */
	return FAIL;

    if (screen_del_lines(0, wp->w_winpos + row, line_count,
						    (int)Rows, FALSE) == FAIL)
	return FAIL;

    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    if (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	if (screen_ins_lines(0, wp->w_winpos + wp->w_height - line_count,
					       line_count, (int)Rows) == FAIL)
	{
	    wp->w_redr_status = TRUE;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    else
	redraw_cmdline = TRUE;
    return OK;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    void
win_rest_invalid(wp)
    WIN		*wp;
{
    while (wp)
    {
	wp->w_lsize_valid = 0;
	wp->w_redr_type = NOT_VALID;
	wp->w_redr_status = TRUE;
	wp = wp->w_next;
    }
    redraw_cmdline = TRUE;
}

/*
 * The rest of the routines in this file perform screen manipulations. The
 * given operation is performed physically on the screen. The corresponding
 * change is also made to the internal screen image. In this way, the editor
 * anticipates the effect of editing changes on the appearance of the screen.
 * That way, when we call screenupdate a complete redraw isn't usually
 * necessary. Another advantage is that we can keep adding code to anticipate
 * screen changes, and in the meantime, everything still works.
 */

/*
 * types for inserting or deleting lines
 */
#define USE_T_CAL   1
#define USE_T_CDL   2
#define USE_T_AL    3
#define USE_T_CE    4
#define USE_T_DL    5
#define USE_T_SR    6
#define USE_NL	    7
#define USE_T_CD    8

/*
 * insert lines on the screen and update NextScreen
 * 'end' is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used 'off' is the offset from the top for the region.
 * 'row' and 'end' are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    static int
screen_ins_lines(off, row, line_count, end)
    int		off;
    int		row;
    int		line_count;
    int		end;
{
    int		i;
    int		j;
    char_u	*temp;
    int		cursor_row;
    int		type;
    int		result_empty;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     */
    if (!screen_valid(TRUE) || line_count <= 0 || line_count > p_ttyscroll)
	return FAIL;

    /*
     * There are seven ways to insert lines:
     * 1. Use T_CD (clear to end of display) if it exists and the result of
     *	  the insert is just empty lines
     * 2. Use T_CAL (insert multiple lines) if it exists and T_AL is not
     *	  present or line_count > 1. It looks better if we do all the inserts
     *	  at once.
     * 3. Use T_CDL (delete multiple lines) if it exists and the result of the
     *	  insert is just empty lines and T_CE is not present or line_count >
     *	  1.
     * 4. Use T_AL (insert line) if it exists.
     * 5. Use T_CE (erase line) if it exists and the result of the insert is
     *	  just empty lines.
     * 6. Use T_DL (delete line) if it exists and the result of the insert is
     *	  just empty lines.
     * 7. Use T_SR (scroll reverse) if it exists and inserting at row 0 and
     *	  the 'da' flag is not set or we have clear line capability.
     *
     * Careful: In a hpterm scroll reverse doesn't work as expected, it moves
     * the scrollbar for the window. It does have insert line, use that if it
     * exists.
     */
    result_empty = (row + line_count >= end);
    if (*T_CD != NUL && result_empty)
	type = USE_T_CD;
    else if (*T_CAL != NUL && (line_count > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    else if (*T_CDL != NUL && result_empty && (line_count > 1 || *T_CE == NUL))
	type = USE_T_CDL;
    else if (*T_AL != NUL)
	type = USE_T_AL;
    else if (*T_CE != NUL && result_empty)
	type = USE_T_CE;
    else if (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    else if (*T_SR != NUL && row == 0 && (*T_DA == NUL || *T_CE))
	type = USE_T_SR;
    else
	return FAIL;

    /*
     * For clearing the lines screen_del_lines is used. This will also take
     * care of t_db if necessary.
     */
    if (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	return screen_del_lines(off, row, line_count, end, FALSE);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    if (*T_DB)
	screen_del_lines(off, end - line_count, line_count, end, FALSE);

#ifdef USE_GUI_BEOS
    vim_lock_screen();
#endif
    if (*T_CCS != NUL)	   /* cursor relative to region */
	cursor_row = row;
    else
	cursor_row = row + off;

    /*
     * Shift LinePointers line_count down to reflect the inserted lines.
     * Clear the inserted lines in NextScreen.
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
	j = end - 1 - i;
	temp = LinePointers[j];
	while ((j -= line_count) >= row)
	    LinePointers[j + line_count] = LinePointers[j];
	LinePointers[j + line_count] = temp;
	lineclear(temp);
    }
#ifdef USE_GUI_BEOS
    vim_unlock_screen();
#endif

    screen_stop_highlight();
    windgoto(cursor_row, 0);
    if (type == USE_T_CAL)
    {
	term_append_lines(line_count);
	screen_start();		/* don't know where cursor is now */
    }
    else
    {
	for (i = 0; i < line_count; i++)
	{
	    if (type == USE_T_AL)
	    {
		if (i && cursor_row != 0)
		    windgoto(cursor_row, 0);
		out_str(T_AL);
	    }
	    else  /* type == USE_T_SR */
		out_str(T_SR);
	    screen_start();	    /* don't know where cursor is now */
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    if (type == USE_T_SR && *T_DA)
    {
	for (i = 0; i < line_count; ++i)
	{
	    windgoto(off + i, 0);
	    out_str(T_CE);
	    screen_start();	    /* don't know where cursor is now */
	}
    }

#ifdef USE_GUI
    if (gui.in_use)
	out_flush();	/* always flush after a scroll */
#endif
    return OK;
}

/*
 * delete lines on the screen and update NextScreen
 * 'end' is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used 'off' is the offset from the top for the region.
 * 'row' and 'end' are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    int
screen_del_lines(off, row, line_count, end, force)
    int		off;
    int		row;
    int		line_count;
    int		end;
    int		force;		/* even when line_count > p_ttyscroll */
{
    int		j;
    int		i;
    char_u	*temp;
    int		cursor_row;
    int		cursor_end;
    int		result_empty;	/* result is empty until end of region */
    int		can_delete;	/* deleting line codes can be used */
    int		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     */
    if (!screen_valid(TRUE) || line_count <= 0 ||
					 (!force && line_count > p_ttyscroll))
	return FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + line_count >= end;

    /*
     * We can delete lines only when 'db' flag not set or when 'ce' option
     * available.
     */
    can_delete = (*T_DB == NUL || *T_CE);

    /*
     * There are four ways to delete lines:
     * 1. Use T_CD if it exists and the result is empty.
     * 2. Use newlines if row == 0 and count == 1 or T_CDL does not exist.
     * 3. Use T_CDL (delete multiple lines) if it exists and line_count > 1 or
     *	  none of the other ways work.
     * 4. Use T_CE (erase line) if the result is empty.
     * 5. Use T_DL (delete line) if it exists.
     */
    if (*T_CD != NUL && result_empty)
	type = USE_T_CD;
#if defined(__BEOS__) && defined(BEOS_DR8)
    /*
     * USE_NL does not seem to work in Terminal of DR8 so we set T_DB="" in
     * its internal termcap... this works okay for tests which test *T_DB !=
     * NUL.  It has the disadvantage that the user cannot use any :set t_*
     * command to get T_DB (back) to empty_option, only :set term=... will do
     * the trick...
     * Anyway, this hack will hopefully go away with the next OS release.
     * (Olaf Seibert)
     */
    else if (row == 0 && T_DB == empty_option
					&& (line_count == 1 || *T_CDL == NUL))
#else
    else if (row == 0 && (
#ifndef AMIGA
	/* On the Amiga, somehow '\n' on the last line doesn't always scroll
	 * up, so use delete-line command */
			    line_count == 1 ||
#endif
						*T_CDL == NUL))
#endif
	type = USE_NL;
    else if (*T_CDL != NUL && line_count > 1 && can_delete)
	type = USE_T_CDL;
    else if (*T_CE != NUL && result_empty)
	type = USE_T_CE;
    else if (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    else if (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    else
	return FAIL;

#ifdef USE_GUI_BEOS
    vim_lock_screen();
#endif
    if (*T_CCS != NUL)	    /* cursor relative to region */
    {
	cursor_row = row;
	cursor_end = end;
    }
    else
    {
	cursor_row = row + off;
	cursor_end = end + off;
    }

    /*
     * Now shift LinePointers line_count up to reflect the deleted lines.
     * Clear the inserted lines in NextScreen.
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
	j = row + i;
	temp = LinePointers[j];
	while ((j += line_count) <= end - 1)
	    LinePointers[j - line_count] = LinePointers[j];
	LinePointers[j - line_count] = temp;
	lineclear(temp);
    }
#ifdef USE_GUI_BEOS
    vim_unlock_screen();
#endif

    /* delete the lines */
    screen_stop_highlight();

    if (type == USE_T_CD)
    {
	windgoto(cursor_row, 0);
	out_str(T_CD);
	screen_start();			/* don't know where cursor is now */
    }
    else if (type == USE_T_CDL)
    {
	windgoto(cursor_row, 0);
	term_delete_lines(line_count);
	screen_start();			/* don't know where cursor is now */
    }
	/*
	 * Deleting lines at top of the screen or scroll region: Just scroll
	 * the whole screen (scroll region) up by outputting newlines on the
	 * last line.
	 */
    else if (type == USE_NL)
    {
	windgoto(cursor_end - 1, 0);
	for (i = line_count; --i >= 0; )
	    out_char('\n');		/* cursor will remain on same line */
    }
    else
    {
	for (i = line_count; --i >= 0; )
	{
	    if (type == USE_T_DL)
	    {
		windgoto(cursor_row, 0);
		out_str(T_DL);		/* delete a line */
	    }
	    else /* type == USE_T_CE */
	    {
		windgoto(cursor_row + i, 0);
		out_str(T_CE);		/* erase a line */
	    }
	    screen_start();		/* don't know where cursor is now */
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    if (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	for (i = line_count; i > 0; --i)
	{
	    windgoto(cursor_end - i, 0);
	    out_str(T_CE);		/* erase a line */
	    screen_start();		/* don't know where cursor is now */
	}
    }

#ifdef USE_GUI
    if (gui.in_use)
	out_flush();	/* always flush after a scroll */
#endif
    return OK;
}

/*
 * show the current mode and ruler
 *
 * If clear_cmdline is TRUE, clear the rest of the cmdline.
 * If clear_cmdline is FALSE there may be a message there that needs to be
 * cleared only if a mode is shown.
 * Return the length of the message (0 if no message).
 */
    int
showmode()
{
    int	    need_clear = FALSE;
    int	    length = 0;
    int	    do_mode;
    int	    attr;
#ifdef INSERT_EXPAND
    int	    sub_attr;
#endif

    do_mode = (p_smd && ((State & INSERT) || restart_edit || VIsual_active));
    if (do_mode || Recording)
    {
	/*
	 * Don't show mode right now, when not redrawing or inside a mapping.
	 * Call char_avail() only when we are going to show something, because
	 * it takes a bit of time.
	 */
	if (!redrawing() || (char_avail() && !KeyTyped))
	{
	    redraw_cmdline = TRUE;	    /* show mode later */
	    return 0;
	}

	check_for_delay(FALSE);
	msg_didout = FALSE;		/* never scroll up */
	msg_col = 0;
	gotocmdline(FALSE);
	attr = highlight_attr[HLF_CM];	/* Highlight mode */
	if (do_mode)
	{
	    MSG_PUTS_ATTR("--", attr);
#ifdef INSERT_EXPAND
	    if (edit_submode != NULL)	    /* CTRL-X in Insert mode */
	    {
		msg_puts_attr(edit_submode, attr);
		if (edit_submode_extra != NULL)
		{
		    MSG_PUTS_ATTR(" ", attr);	/* add a space in between */
		    if (edit_submode_highl < HLF_COUNT)
			sub_attr = highlight_attr[edit_submode_highl];
		    else
			sub_attr = attr;
		    msg_puts_attr(edit_submode_extra, sub_attr);
		}
	    }
	    else
#endif
	    {
		if (State == INSERT)
		{
#ifdef RIGHTLEFT
		    if (p_ri)
			MSG_PUTS_ATTR(" REVERSE", attr);
#endif
		    MSG_PUTS_ATTR(" INSERT", attr);
		}
		else if (State == REPLACE)
		    MSG_PUTS_ATTR(" REPLACE", attr);
		else if (restart_edit == 'I')
		    MSG_PUTS_ATTR(" (insert)", attr);
		else if (restart_edit == 'R')
		    MSG_PUTS_ATTR(" (replace)", attr);
#ifdef RIGHTLEFT
		if (p_hkmap)
		    MSG_PUTS_ATTR(" Hebrew", attr);
# ifdef FKMAP
		if (p_fkmap)
		    MSG_PUTS_ATTR(farsi_text_5, attr);
# endif
#endif
		if ((State & INSERT) && p_paste)
		    MSG_PUTS_ATTR(" (paste)", attr);
		if (VIsual_active)
		{
		    MSG_PUTS_ATTR(" VISUAL", attr);
		    if (VIsual_mode == Ctrl('V'))
			MSG_PUTS_ATTR(" BLOCK", attr);
		    else if (VIsual_mode == 'V')
			MSG_PUTS_ATTR(" LINE", attr);
		}
	    }
	    MSG_PUTS_ATTR(" --", attr);
	    need_clear = TRUE;
	}
	if (Recording)
	{
	    MSG_PUTS_ATTR("recording", attr);
	    need_clear = TRUE;
	}
	if (need_clear || clear_cmdline)
	    msg_clr_eos();
	msg_didout = FALSE;		/* overwrite this message */
	length = msg_col;
	msg_col = 0;
    }
    else if (clear_cmdline)		/* just clear anything */
    {
	msg_row = cmdline_row;
	msg_col = 0;
	msg_clr_eos();			/* will reset clear_cmdline */
    }
    win_redr_ruler(lastwin, TRUE);
    redraw_cmdline = FALSE;
    clear_cmdline = FALSE;

    return length;
}

/*
 * delete mode message
 */
    void
unshowmode(force)
    int	    force;
{
    /*
     * Don't delete it right now, when not redrawing or insided a mapping.
     */
    if (!redrawing() || (!force && char_avail() && !KeyTyped))
	redraw_cmdline = TRUE;		/* delete mode later */
    else if (Recording)
	MSG("recording");
    else
	MSG("");
}

    static int
highlight_status(attr)
    int	    *attr;
{
    *attr = highlight_attr[HLF_S];
    if (*attr)				/* can use highlighting */
	return ' ';
    return '=';
}

/*
 * If ruler option is set: show current cursor position.
 * If always is FALSE, only print if position has changed.
 */
    void
showruler(always)
    int	    always;
{
    win_redr_ruler(curwin, always);
}

    static void
win_redr_ruler(wp, always)
    WIN	    *wp;
    int	    always;
{
    char_u	    buffer[30];
    int		    row;
    int		    fillchar;
    int		    attr;
    int		    empty_line = FALSE;
    colnr_t	    virtcol;

    /* If 'ruler' off or redrawing disabled, don't do anything */
    if (!p_ru || (!always && !redrawing()))
	return;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    if (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_line_count)
	return;

    /*
     * Check if the line is empty (will show "0-1").
     */
    if (*ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_line = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    if (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
	    || empty_line != wp->w_ru_empty)
    {
	cursor_off();
	if (wp->w_status_height)
	{
	    row = wp->w_winpos + wp->w_height;
	    fillchar = highlight_status(&attr);
	}
	else
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
	}

	/* In list mode virtcol needs to be recomputed */
	virtcol = wp->w_virtcol;
	if (wp->w_p_list)
	{
	    wp->w_p_list = FALSE;
	    getvcol(wp, &wp->w_cursor, NULL, &virtcol, NULL);
	    wp->w_p_list = TRUE;
	}

	/*
	 * Some sprintfs return the length, some return a pointer.
	 * To avoid portability problems we use strlen() here.
	 */
	sprintf((char *)buffer, "%ld,",
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	col_print(buffer + STRLEN(buffer),
		!(State & INSERT) && empty_line
		    ? 0
		    : (int)wp->w_cursor.col + 1,
		(int)virtcol + 1);

	screen_puts(buffer, row, ru_col, attr);
	screen_fill(row, row + 1, ru_col + (int)STRLEN(buffer),
				      (int)Columns, fillchar, fillchar, attr);
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_empty = empty_line;
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    void
check_for_delay(check_msg_scroll)
    int	    check_msg_scroll;
{
    if (emsg_on_display || (check_msg_scroll && msg_scroll))
    {
	ui_delay(1000L, TRUE);
	emsg_on_display = FALSE;
	if (check_msg_scroll)
	    msg_scroll = FALSE;
    }
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "clear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    int
screen_valid(clear)
    int	    clear;
{
    screenalloc(clear);	    /* allocate screen buffers if size changed */
    return (NextScreen != NULL);
}

#ifdef USE_MOUSE
/*
 * Move the cursor to the specified row and column on the screen.
 * Change current window if neccesary.	Returns an integer with the
 * CURSOR_MOVED bit set if the cursor has moved or unset otherwise.
 *
 * If flags has MOUSE_FOCUS, then the current window will not be changed, and
 * if the mouse is outside the window then the text will scroll, or if the
 * mouse was previously on a status line, then the status line may be dragged.
 *
 * If flags has MOUSE_MAY_VIS, then VIsual mode will be started before the
 * cursor is moved unless the cursor was on a status line.  Ignoring the
 * CURSOR_MOVED bit, this function returns one of IN_UNKNOWN, IN_BUFFER, or
 * IN_STATUS_LINE depending on where the cursor was clicked.
 *
 * If flags has MOUSE_DID_MOVE, nothing is done if the mouse didn't move since
 * the last call.
 *
 * If flags has MOUSE_SETPOS, nothing is done, only the current position is
 * remembered.
 */
    int
jump_to_mouse(flags, inclusive)
    int	    flags;
    int	    *inclusive;		/* used for inclusive operator, can be NULL */
{
    static int on_status_line = 0;	/* #lines below bottom of window */
    static int prev_row = -1;
    static int prev_col = -1;

    WIN	    *wp, *old_curwin;
    FPOS    old_cursor;
    int	    count;
    int	    first;
    int	    row = mouse_row;
    int	    col = mouse_col;

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    if ((flags & MOUSE_DID_MOVE) && prev_row == mouse_row &&
							prev_col == mouse_col)
	return IN_BUFFER;		/* mouse pointer didn't move */

    prev_row = mouse_row;
    prev_col = mouse_col;

    if ((flags & MOUSE_SETPOS))
	return IN_BUFFER;		/* mouse pointer didn't move */

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    if (!(flags & MOUSE_FOCUS))
    {
	if (row < 0 || col < 0)	    /* check if it makes sense */
	    return IN_UNKNOWN;

	/* find the window where the row is in */
	for (wp = firstwin; wp->w_next; wp = wp->w_next)
	    if (row < wp->w_next->w_winpos)
		break;
	/*
	 * winpos and height may change in win_enter()!
	 */
	row -= wp->w_winpos;
	if (row >= wp->w_height)    /* In (or below) status line */
	    on_status_line = row - wp->w_height + 1;
	else
	    on_status_line = 0;
	win_enter(wp, TRUE);	    /* can make wp invalid! */
	if (on_status_line)	    /* In (or below) status line */
	{
	    /* Don't use start_arrow() if we're in the same window */
	    if (curwin == old_curwin)
		return IN_STATUS_LINE;
	    else
		return IN_STATUS_LINE | CURSOR_MOVED;
	}

	curwin->w_cursor.lnum = curwin->w_topline;
    }
    else if (on_status_line)
    {
	/* Drag the status line */
	count = row - curwin->w_winpos - curwin->w_height + 1 - on_status_line;
	win_drag_status_line(count);
	return IN_STATUS_LINE;	    /* Cursor didn't move */
    }
    else /* keep_window_focus must be TRUE */
    {
	row -= curwin->w_winpos;

	/*
	 * When clicking beyond the end of the window, scroll the screen.
	 * Scroll by however many rows outside the window we are.
	 */
	if (row < 0)
	{
	    count = 0;
	    for (first = TRUE; curwin->w_topline > 1; --curwin->w_topline)
	    {
		count += plines(curwin->w_topline - 1);
		if (!first && count > -row)
		    break;
		first = FALSE;
	    }
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    redraw_later(VALID);
	    row = 0;
	}
	else if (row >= curwin->w_height)
	{
	    count = 0;
	    for (first = TRUE; curwin->w_topline < curbuf->b_ml.ml_line_count;
							++curwin->w_topline)
	    {
		count += plines(curwin->w_topline);
		if (!first && count > row - curwin->w_height + 1)
		    break;
		first = FALSE;
	    }
	    redraw_later(VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    row = curwin->w_height - 1;
	}
	curwin->w_cursor.lnum = curwin->w_topline;
    }

#ifdef RIGHTLEFT
    if (curwin->w_p_rl)
	col = Columns - 1 - col;
#endif

    if (curwin->w_p_wrap)	/* lines wrap */
    {
	while (row)
	{
	    count = plines(curwin->w_cursor.lnum);
	    if (count > row)
	    {
		col += row * Columns;
		break;
	    }
	    if (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	    {
		mouse_past_bottom = TRUE;
		break;
	    }
	    row -= count;
	    ++curwin->w_cursor.lnum;
	}
    }
    else				/* lines don't wrap */
    {
	curwin->w_cursor.lnum += row;
	if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	{
	    curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    mouse_past_bottom = TRUE;
	}
	col += curwin->w_leftcol;
    }

    if (curwin->w_p_nu)			/* skip number in front of the line */
	if ((col -= 8) < 0)
	    col = 0;

    curwin->w_curswant = col;
    curwin->w_set_curswant = FALSE;	/* May still have been TRUE */
    if (coladvance(col) == FAIL)	/* Mouse click beyond end of line */
    {
	if (inclusive != NULL)
	    *inclusive = TRUE;
	mouse_past_eol = TRUE;
    }
    else if (inclusive != NULL)
	*inclusive = FALSE;

    if ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
#ifdef USE_MOUSE
	setmouse();
#endif
	if (p_smd)
	    redraw_cmdline = TRUE;	/* show visual mode later */
    }

    if (curwin == old_curwin && curwin->w_cursor.lnum == old_cursor.lnum &&
				       curwin->w_cursor.col == old_cursor.col)
	return IN_BUFFER;		/* Cursor has not moved */
    return IN_BUFFER | CURSOR_MOVED;	/* Cursor has moved */
}
#endif /* USE_MOUSE */

/*
 * Return TRUE if redrawing should currently be done.
 */
    int
redrawing()
{
    return (!RedrawingDisabled && !(p_lz && char_avail() && !KeyTyped));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    int
messaging()
{
    return (!(p_lz && char_avail() && !KeyTyped));
}

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
		? ((curwin->w_topline >= curbuf->b_ml.ml_line_count - p_so) &&
			curwin->w_botline > curbuf->b_ml.ml_line_count)
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
		/*
		 * When there are three or less lines on the screen, move them
		 * all to above the screen.
		 */
		if (curwin->w_botline - curwin->w_topline <= 3)
		    off = 0;
		/*
		 * Make sure at least w_botline gets onto the screen, also
		 * when 'scrolloff' is non-zero and with very long lines.
		 */
		else if (plines(curwin->w_botline) +
			plines(curwin->w_botline - 1) +
			plines(curwin->w_botline - 2) >= curwin->w_height - 2)
		    off = 0;
		else
		    off = 2;
		curwin->w_topline = curwin->w_botline - off;
		curwin->w_cursor.lnum = curwin->w_topline;
		curwin->w_valid &= ~(VALID_WCOL|VALID_CHEIGHT|VALID_WROW|
				   VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    }
	}
	else	/* dir == BACKWARDS */
	{
	    lp = curwin->w_topline;
	    /*
	     * If the first two lines on the screen are not too big, we keep
	     * them on the screen.
	     */
	    if ((n = plines(lp)) > curwin->w_height / 2)
		--lp;
	    else if (lp < curbuf->b_ml.ml_line_count &&
				    n + plines(lp + 1) < curwin->w_height / 2)
		++lp;
	    curwin->w_cursor.lnum = lp;
	    n = 0;
	    while (n <= curwin->w_height && lp >= 1)
	    {
		n += plines(lp);
		--lp;
	    }
	    if (n <= curwin->w_height)		    /* at begin of file */
	    {
		curwin->w_topline = 1;
		curwin->w_valid &= ~(VALID_WROW|VALID_CROW|VALID_BOTLINE);
	    }
	    else if (lp >= curwin->w_topline - 2)   /* very long lines */
	    {
		--curwin->w_topline;
		comp_botline();
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
    cursor_correct();
    beginline(BL_SOL | BL_FIX);
    curwin->w_valid &= ~(VALID_WCOL|VALID_WROW|VALID_VIRTCOL);

    /*
     * Avoid the screen jumping up and down when 'scrolloff' is non-zero.
     */
    if (dir == FORWARD && curwin->w_cursor.lnum < curwin->w_topline + p_so)
	scroll_cursor_top(1, FALSE);

    update_screen(VALID);
    return retval;
}

/* #define KEEP_SCREEN_LINE */

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
	curwin->w_p_scroll = (Prenum > curwin->w_height) ?
						curwin->w_height : Prenum;
    n = (curwin->w_p_scroll <= curwin->w_height) ?
				    curwin->w_p_scroll : curwin->w_height;

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
	    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	}
#else
	    /* try to put the cursor in the same screen line */
	while ((curwin->w_cursor.lnum < curwin->w_topline || scrolled > 0)
			     && curwin->w_cursor.lnum < curwin->w_botline - 1)
	{
	    scrolled -= plines(curwin->w_cursor.lnum);
	    if (scrolled < 0 && curwin->w_cursor.lnum >= curwin->w_topline)
		break;
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
	}
#endif
    }
    cursor_correct();
    beginline(BL_SOL | BL_FIX);
    update_screen(VALID);
}

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    static void
intro_message()
{
    int		i;
    int		row;
    int		col;
    static char	*(lines[]) =
    {
	"VIM - Vi IMproved",
	"",
	"version ",
	"by Bram Moolenaar et al.",
	"",
	"Vim is freely distributable",
	"type  :help uganda<Enter>     if you like Vim ",
	"",
	"type  :q<Enter>               to exit         ",
	"type  :help<Enter>  or  <F1>  for on-line help",
	"type  :help version5<Enter>   for version info",
	NULL,
	"",
	"Running in Vi compatible mode",
	"type  :set nocp<Enter>        for Vim defaults",
	"type  :help cp-default<Enter> for info on this",
    };

    row = ((int)Rows - (int)(sizeof(lines) / sizeof(char *))) / 2;
    if (!p_cp)
	row += 2;
#if defined(WIN32) && !defined(USE_GUI_WIN32)
    if (mch_windows95())
	row -= 2;
#endif
    if (row > 2 && Columns >= 50)
    {
	for (i = 0; i < (int)(sizeof(lines) / sizeof(char *)); ++i)
	{
	    if (lines[i] == NULL)
	    {
		if (!p_cp)
		    break;
		continue;
	    }
	    col = strlen(lines[i]);
	    if (i == 2)
		col += strlen(mediumVersion);
	    col = (Columns - col) / 2;
	    if (col < 0)
		col = 0;
	    screen_puts((char_u *)lines[i], row, col, 0);
	    if (i == 2)
		screen_puts((char_u *)mediumVersion, row, col + 8, 0);
	    ++row;
	}
#if defined(WIN32) && !defined(USE_GUI_WIN32)
	if (mch_windows95())
	{
	    screen_puts((char_u *)"WARNING: Windows 95 detected", row + 1, col + 8, highlight_attr[HLF_E]);
	    screen_puts((char_u *)"type  :help windows95<Enter>  for info on this", row + 2, col, 0);
	}
#endif
    }
}

/*
 * ":intro" command: clear screen, display intro screen and wait for return.
 */
    void
do_intro()
{
    screenclear();
    intro_message();
    wait_return(TRUE);
}
