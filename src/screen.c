/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * screen.c: code for displaying on the screen
 *
 * Output to the screen (console, terminal emulator or GUI window) is minized
 * by remembering what is already on the screen, and only updating the parts
 * that changed.
 *
 * ScreenLines[] contains a copy of the whole screen, as it is currently
 * displayed (excluding text written by external commands).
 * ScreenAttrs[] contains the associated attributes.
 * LineOffset[] contains the offset into ScreenLines[] for each line.
 * For double-byte characters, two consecutive bytes in ScreenLines[] can form
 * one character which occupies two display cells.
 * For UTF-8 a multi-byte character is converted to Unicode and stored in
 * ScreenLinesUC[].  ScreenLines[] contains the first byte only.  For an ASCII
 * character without composing chars ScreenLinesUC[] will be 0.  When the
 * character occupies two display cells the next byte in ScreenLines[] is 0.
 * ScreenLinesC1[] and ScreenLinesC2[] contain up to two composing characters
 * (drawn on top of the first character).  They are 0 when not used.
 *
 * The screen_*() functions write to the screen and handle updating
 * ScreenLines[].
 *
 * update_screen() is the function that updates all windows and status lines.
 * It is called form the main loop when must_redraw is non-zero.  It may be
 * called from other places when an immediated screen update is needed.
 *
 * The part of the buffer that is displayed in a window is set with:
 * - w_topline (first buffer line in window)
 * - w_leftcol (leftmost window cell in window),
 * - w_skipcol (skipped window cells of first line)
 *
 * Commands that only move the cursor around in a window, do not need to take
 * action to update the display.  The main loop will check if w_topline is
 * valid and update it (scroll the window) when needed.
 *
 * Commands that scroll a window change w_topline and must call
 * adjust_cursor() to move the cursor into the visible part of the window, and
 * call redraw_later(VALID) to have the window displayed by update_screen()
 * later.
 *
 * Commands that change text in the buffer must call changed_bytes() or
 * changed_lines() to mark the area that changed and will require updating
 * later.  The main loop will call update_screen(), which will update each
 * window that shows the changed buffer.  This assumes text above the change
 * can remain displayed as it is.  Text after the change may need updating for
 * scrolling, folding and syntax highlighting.
 *
 * Commands that change how a window is displayed (e.g., setting 'list') or
 * invalidate the contents of a window in another way (e.g., change fold
 * settings), must call redraw_later(NOT_VALID) to have the whole window
 * redisplayed by update_screen() later.
 *
 * Commands that change how a buffer is displayed (e.g., setting 'tabstop')
 * must call redraw_curbuf_later(NOT_VALID) to have all the windows for the
 * buffer redisplayed by update_screen() later.
 *
 * Commands that move the window position must call redraw_later(NOT_VALID).
 * TODO: should minimize redrawing by scrolling when possible.
 *
 * Commands that change everything (e.g., resizing the screen) must call
 * redraw_all_later(NOT_VALID) or redraw_all_later(CLEAR).
 *
 * Things that are handled indirectly:
 * - When messages scroll the screen up, msg_scrolled will be set and
 *   update_screen() called to redraw.
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

#ifdef FEAT_SEARCH_EXTRA
/*
 * When highlighting matches for the last use search pattern:
 * search_hl_rm.regprog points to the regexp program for it (for all buffers)
 * search_hl_rm		contains last found match (may continue in next line)
 * search_hl_buf	the buffer to search for a match
 * search_hl_lnum	the line to search for a match
 * search_hl_attr	contains the attributes to be used
 * search_hl_ic		the value for "reg_ic" for this search
 */
regmmatch_t	search_hl_rm;
buf_t		*search_hl_buf;
linenr_t	search_hl_lnum;
int		search_hl_attr;
int		search_hl_ic;
#endif

/*
 * Flags for w_valid.
 * These are set when something in a window structure becomes invalid, except
 * when the cursor is moved.  Call check_cursor_moved() before testing one of
 * the flags.
 * These are reset when that thing has been updated and is valid again.
 *
 * Every function that invalidates one of these must call one of the
 * invalidate_* functions.
 *
 * w_valid is supposed to be used only in screen.c.  From other files, use the
 * functions that set or reset the flags.
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
#define VALID_CHEIGHT	0x08	/* w_cline_height and w_cline_folded valid */
#define VALID_CROW	0x10	/* w_cline_row is valid */
#define VALID_BOTLINE	0x20	/* w_botine and w_empty_rows are valid */
#define VALID_BOTLINE_AP 0x40	/* w_botine is approximated */
#define VALID_TOPLINE	0x80	/* w_topline is valid (for cursor position) */

struct stl_hlrec
{
    char_u  *start;
    int	    userhl;
};

#ifdef FEAT_FOLDING
static int win_fold_level = 0;	/* fold level to show for 'foldcolumn' */
#endif

/*
 * Buffer for one screen line (characters and attributes).
 */
static schar_t	*current_ScreenLine;
#ifdef FEAT_FOLDING
static sattr_t	*current_ScreenAttrs;
#endif

static void win_update __ARGS((win_t *wp));
static void win_draw_end __ARGS((win_t *wp, int c, int row));
#ifdef FEAT_FOLDING
static void fold_line __ARGS((win_t *wp, long fold_count, int level, linenr_t lnum, int row));
static int foldcolumn_char __ARGS((void));
#endif
static int win_line __ARGS((win_t *, linenr_t, int, int));
static int char_needs_redraw __ARGS((int off_from, int off_to, int len));
#ifdef FEAT_RIGHTLEFT
static void screen_line __ARGS((int row, int coloff, int endcol, int clear_width, int rlflag));
# define SCREEN_LINE(r, o, e, c, rl)    screen_line((r), (o), (e), (c), (rl))
#else
static void screen_line __ARGS((int row, int coloff, int endcol, int clear_width));
# define SCREEN_LINE(r, o, e, c, rl)    screen_line((r), (o), (e), (c))
#endif
#if defined(FEAT_WILDMENU) && defined(FEAT_VERTSPLIT)
static void win_redraw_last_status __ARGS((frame_t *frp));
#endif
#ifdef FEAT_VERTSPLIT
static void draw_vsep_win __ARGS((win_t *wp, int row));
#endif
#ifdef FEAT_SEARCH_EXTRA
static void start_search_hl __ARGS((void));
static void end_search_hl __ARGS((void));
static void next_search_hl __ARGS((linenr_t lnum, colnr_t mincol));
#endif
static void screen_start_highlight __ARGS((int attr));
static void comp_botline __ARGS((win_t *wp));
static void screen_char __ARGS((unsigned off, int row, int col));
#ifdef FEAT_MBYTE
static void screen_char_n __ARGS((unsigned off, int n, int row, int col));
#endif
static void screenclear2 __ARGS((void));
static void lineclear __ARGS((unsigned off));
static void check_cursor_moved __ARGS((win_t *wp));
static void curs_rows __ARGS((win_t *wp, int do_botline));
static void validate_virtcol_win __ARGS((win_t *wp));
static int win_do_lines __ARGS((win_t *wp, int row, int line_count, int mayclear, int del));
static int screen_ins_lines __ARGS((int, int, int, int));
static void msg_pos_mode __ARGS((void));
#ifdef FEAT_WINDOWS
static int fillchar_status __ARGS((int *attr, int is_curwin));
#endif
#ifdef FEAT_VERTSPLIT
static int fillchar_vsep __ARGS((int *attr));
#endif
#ifdef FEAT_STL_OPT
static void win_redr_custom __ARGS((win_t *wp, int Ruler));
static int build_stl_str_hl __ARGS((win_t *wp, char_u *out, char_u *fmt, int fillchar, int maxlen, struct stl_hlrec *hl));
#endif
#ifdef FEAT_CMDL_INFO
static void win_redr_ruler __ARGS((win_t *wp, int always));
#endif
#if defined(FEAT_STL_OPT) || defined(FEAT_CMDL_INFO)
static void get_rel_pos __ARGS((win_t *wp, char_u	*str));
#endif
static int mouse_comp_pos __ARGS((int *rowp, int *colp, linenr_t *lnump));
static int get_scroll_overlap __ARGS((linenr_t lnum, int dir));
static void intro_message __ARGS((void));

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * e.g. if must_redraw is CLEAR, type NOT_VALID will do nothing.
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
    win_t	*wp;

    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	if (wp->w_redr_type < type)
	    wp->w_redr_type = type;
    if (must_redraw < type)	/* must_redraw is the maximum of all windows */
	must_redraw = type;
}

/*
 * Mark all windows that are editing the current buffer to be udpated later.
 */
    void
redraw_curbuf_later(type)
    int		type;
{
    redraw_buf_later(type, curbuf);
}

    void
redraw_buf_later(type, buf)
    int		type;
    buf_t	*buf;
{
    win_t	*wp;

    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	if (wp->w_redr_type < type && wp->w_buffer == buf)
	    wp->w_redr_type = type;
    if (must_redraw < type)	/* must_redraw is the maximum of all windows */
	must_redraw = type;
}

/*
 * Changed something in the current window, at buffer line "lnum", that
 * requires that line and possibly other lines to be redrawn.
 * Used when entering/leaving Insert mode with the cursor on a folded line.
 * Used to remove the "$" from a change command.
 * Note that when also inserting/deleting lines w_redraw_top and w_redraw_bot
 * may become invalid and the whole window will have to be redrawn.
 */
/*ARGSUSED*/
    void
redrawWinline(lnum, invalid)
    linenr_t	lnum;
    int		invalid;	/* window line height is invalid now */
{
#ifdef FEAT_FOLDING
    int		i;
#endif

    if (curwin->w_redraw_top == 0 || curwin->w_redraw_top > lnum)
	curwin->w_redraw_top = lnum;
    if (curwin->w_redraw_bot == 0 || curwin->w_redraw_bot < lnum)
	curwin->w_redraw_bot = lnum;
    redraw_later(VALID);

#ifdef FEAT_FOLDING
    if (invalid)
    {
	/* A w_lines[] entry for this lnum has become invalid. */
	i = find_wl_entry(curwin, lnum);
	if (i >= 0)
	    curwin->w_lines[i].wl_valid = FALSE;
    }
#endif
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
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 */
    void
update_screen(type)
    int		    type;
{
    win_t	    *wp;
    static int	    did_intro = FALSE;
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    int		    did_one;
#endif

    if (!screen_valid(TRUE))
	return;

    if (must_redraw)
    {
	if (type < must_redraw)	    /* use maximal type */
	    type = must_redraw;
	must_redraw = 0;
    }

    if (curwin->w_lines_valid == 0 && type < NOT_VALID)
	type = NOT_VALID;

    if (!redrawing())
    {
	redraw_later(type);		/* remember type for next time */
	if (type > INVERTED)
	    curwin->w_lines_valid = 0;	/* don't use w_lines[].wl_size now */
	return;
    }

#ifdef FEAT_GUI
    updating_screen = TRUE;
#endif
#ifdef FEAT_SYN_HL
    ++display_tick;	    /* let syntax code know we're in a next round of
			     * display updating */
#endif

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
	    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	    {
		if (W_WINROW(wp) >= msg_scrolled)
		    break;
		if (W_WINROW(wp) + wp->w_height > msg_scrolled
			&& wp->w_redr_type < REDRAW_TOP)
		{
		    wp->w_upd_rows = msg_scrolled - W_WINROW(wp);
		    wp->w_redr_type = REDRAW_TOP;
		    break;
		}
		wp->w_redr_type = NOT_VALID;
		if (W_WINROW(wp) + wp->w_height + W_STATUS_HEIGHT(wp)
							       > msg_scrolled)
		    break;
#ifdef FEAT_WINDOWS
		wp->w_redr_status = TRUE;
#endif
	    }
	    redraw_cmdline = TRUE;
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
	msg_clr_cmdline();	/* will reset clear_cmdline */
    }

    /*
     * Only start redrawing if there is really something to do.
     */
    if (type == INVERTED)
	update_curswant();
    if (curwin->w_redr_type < type
	    && !((type == VALID
		    && curwin->w_lines[0].wl_valid
		    && curwin->w_topline == curwin->w_lines[0].wl_lnum)
#ifdef FEAT_VISUAL
		|| (type == INVERTED
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
#endif
		))
	curwin->w_redr_type = type;

#ifdef FEAT_SYN_HL
    /*
     * Correct stored syntax highlighting info for changes in each displayed
     * buffer.  Each buffer must only be done once.
     */
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	if (wp->w_buffer->b_mod_set)
	{
	    win_t	*wwp;

	    /* Check if we already did this buffer. */
	    for (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		if (wwp->w_buffer == wp->w_buffer)
		    break;
	    if (wwp == wp && syntax_present(wp->w_buffer))
		syn_stack_apply_changes(wp->w_buffer);
	}
#endif

    /*
     * Go from top to bottom through the windows, redrawing the ones that need
     * it.
     */
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    did_one = FALSE;
#endif
#ifdef FEAT_SEARCH_EXTRA
    search_hl_rm.regprog = NULL;
#endif
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
    {
	if (wp->w_redr_type)
	{
	    cursor_off();
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
	    if (!did_one)
	    {
		did_one = TRUE;
# ifdef FEAT_SEARCH_EXTRA
		start_search_hl();
# endif
# ifdef FEAT_CLIPBOARD
		/* When Visual area changed, may have to update selection. */
		if (clipboard.available && clip_isautosel())
		    clip_update_selection();
# endif
	    }
#endif
	    win_update(wp);
	}

#ifdef FEAT_WINDOWS
	/* redraw status line after each window to minimize cursor movement */
	if (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp);
	}
#endif
    }
#if defined(FEAT_SEARCH_EXTRA)
    end_search_hl();
#endif

    /* Reset b_mod_set flags.  Going through all windows is probably faster
     * than going through all buffers (there could be many buffers). */
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	wp->w_buffer->b_mod_set = FALSE;

#ifdef FEAT_GUI
    updating_screen = FALSE;
    gui_may_resize_shell();
#endif

    if (redraw_cmdline)
	showmode();

    /* May put up an introductory message when not editing a file */
    if (!did_intro && bufempty()
	    && curbuf->b_fname == NULL
	    && firstwin->w_next == NULL
	    && vim_strchr(p_shm, SHM_INTRO) == NULL)
	intro_message();
    did_intro = TRUE;
}

#if defined(FEAT_SIGNS) || defined(PROTO)
    static int
lnum2row(wp, lnum)
    win_t	*wp;
    int		lnum;
{
    int		row = 0;
    int		j;
    linenr_t	i;

    for (i = wp->w_topline, j = 0;
			      i < wp->w_botline && j < wp->w_lines_valid; i++)
    {
	row += wp->w_lines[j++].wl_size;
#ifdef FEAT_FOLDING
	(void)hasFolding(i, NULL, &i);
#endif
	if (i >= lnum)
	    break;
    }
    return row - 1;
}

    void
update_debug_sign(buf, lnum)
    buf_t	*buf;
    int		lnum;
{
    win_t	*wp;
    int		row;

    if (buf != NULL && lnum > 0)
    {					/* update/delete a specific mark */
	for (wp = firstwin; wp; wp = wp->w_next)
	{
	    if (wp->w_buffer == buf && lnum < wp->w_botline)
	    {
		/* update mark */
		row = lnum2row(wp, lnum);
		if (row > 0)
		{
		    screen_start();	/* not sure of screen cursor */
		    win_line(wp, lnum, row, row + 1);
		}
	    }
	}
    }
    else
    {					/* delete all marks */
	for (wp = firstwin; wp; wp = wp->w_next)
	    win_update(wp);
    }
}
#endif


#ifdef FEAT_GUI
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    void
updateWindow(wp)
    win_t	*wp;
{
    cursor_off();
    updating_screen = TRUE;
#ifdef FEAT_SEARCH_EXTRA
    start_search_hl();
#endif
#ifdef FEAT_CLIPBOARD
    /* When Visual area changed, may have to update selection. */
    if (clipboard.available && clip_isautosel())
	clip_update_selection();
#endif
    win_update(wp);
#ifdef FEAT_WINDOWS
    if (wp->w_redr_status
# ifdef FEAT_CMDL_INFO
	    || p_ru
# endif
# ifdef FEAT_STL_OPT
	    || *p_stl
# endif
	    )
	win_redr_status(wp);
#endif
    if (redraw_cmdline)
	showmode();
#ifdef FEAT_SEARCH_EXTRA
    end_search_hl();
#endif
    updating_screen = FALSE;
    gui_may_resize_shell();

}
#endif

/*
 * Update a single window.
 *
 * This may cause the windows below it also to be redrawn (when clearing the
 * screen or scrolling lines).
 *
 * How the window is redrawn depends on wp->w_redr_type.  Each type also
 * implies the one below it.
 * NOT_VALID	redraw the whole window
 * REDRAW_TOP	redraw the top w_upd_rows window lines, otherwise like VALID
 * INVERTED	redraw the changed part of the Visual area
 * VALID	1. scroll up/down to adjust for a changed w_topline
 *		2. update lines at the top when scrolled down
 *		3. redraw changed text:
 *		   - if wp->w_buffer->b_mod_set set, udpate lines between
 *		     b_mod_top and b_mod_bot.
 *		   - if wp->w_redraw_top non-zero, redraw lines between
 *		     wp->w_redraw_top and wp->w_redr_bot.
 *		   - continue redrawing when syntax status is invalid.
 *		4. if scrolled up, update lines at the bottom.
 * This results in three areas that may need updating:
 * top:	from first row to top_end (when scrolled down)
 * mid: from mid_start to mid_end (update inversion or changed text)
 * bot: from bot_start to last row (when scrolled up)
 */
    static void
win_update(wp)
    win_t	*wp;
{
    buf_t	*buf = wp->w_buffer;
    int		type;
    int		top_end = 0;	/* Below last row of the top area that needs
				   updating.  0 when no top area updating. */
    int		mid_start = 999;/* first row of the mid area that needs
				   updating.  999 when no mid area updating. */
    int		mid_end = 0;	/* Below last row of the mid area that needs
				   updating.  0 when no mid area updating. */
    int		bot_start = 999;/* first row of the bot area that needs
				   updating.  999 when no bot area updating */
#ifdef FEAT_SEARCH_EXTRA
    int		top_to_mod = FALSE;    /* redraw above mod_top */
#endif

    int		row;		/* current window row to display */
    linenr_t	lnum;		/* current buffer lnum to display */
    int		idx;		/* current index in w_lines[] */
    int		srow;		/* starting row of the current line */

    int		eof = FALSE;	/* if TRUE, we hit the end of the file */
    int		didline = FALSE; /* if TRUE, we finished the last line */
    int		i;
    long	j;
    static int	recursive = FALSE;	/* being called recursively */
    int		old_botline = wp->w_botline;
#ifdef FEAT_FOLDING
    long	fold_count;
#endif
#ifdef FEAT_SYN_HL
    /* remember what happened to the previous line, to know if
     * check_visual_highlight() can be used */
#define DID_NONE 1	/* didn't update a line */
#define DID_LINE 2	/* updated a normal line */
#define DID_FOLD 3	/* updated a folded line */
    int		did_update = DID_NONE;
#endif
    linenr_t	mod_top = 0;
    linenr_t	mod_bot = 0;
#ifdef FEAT_SEARCH_EXTRA
    linenr_t	first_search_lnum = 0;	/* first lnum to search for multi-line
					   search pattern */
#endif

    type = wp->w_redr_type;

    if (type == NOT_VALID)
    {
#ifdef FEAT_WINDOWS
	wp->w_redr_status = TRUE;
#endif
	wp->w_lines_valid = 0;
    }

    /* Window is zero-height: nothing to draw. */
    if (wp->w_height == 0)
	return;

#ifdef FEAT_VERTSPLIT
    /* Window is zero-width: Only need to draw the separator. */
    if (wp->w_width == 0)
    {
	/* draw the vertical separator right of this window */
	draw_vsep_win(wp, 0);
	return;
    }
#endif

    if (buf->b_mod_set && buf->b_mod_xlines != 0 && wp->w_redraw_top != 0)
    {
	/*
	 * When there are both inserted/deleted lines and specific lines to be
	 * redrawn,, w_redraw_top and w_redraw_bot may be invalid, just redraw
	 * everything (only happens when redrawing is off for while).
	 */
	type = NOT_VALID;
    }
    else
    {
	/*
	 * Set mod_top to the first line that needs displaying because of
	 * changes.  Set mod_bot to the first line after the changes.
	 */
	mod_top = wp->w_redraw_top;
	if (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	else
	    mod_bot = 0;
	wp->w_redraw_top = 0;	/* reset for next time */
	wp->w_redraw_bot = 0;
	if (buf->b_mod_set)
	{
	    if (mod_top == 0 || mod_top > buf->b_mod_top)
		mod_top = buf->b_mod_top;
	    if (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_SEARCH_EXTRA
	    /* When 'hlsearch' is on and using a multi-line search pattern, a
	     * change in one line may make the Search highlighting in a
	     * previous line invalid.  Simple solution: redraw all visible
	     * lines above the change.
	     */
	    if (search_hl_rm.regprog != NULL
		    && re_multiline(search_hl_rm.regprog))
		top_to_mod = TRUE;
#endif
	}
#ifdef FEAT_FOLDING
	if (mod_top != 0 && hasAnyFolding(wp))
	{
	    linenr_t	lnumt, lnumb;

	    /*
	     * A change in a line can cause lines above it to become folded or
	     * unfolded.  Find the top most buffer line that may be affected.
	     * If the line was previously folded and displayed, get the first
	     * line of that fold.  If the line is folded now, get the first
	     * folded line.  Use the minimum of these two.
	     */

	    /* Find last valid w_lines[] entry above mod_top.  Set lnumt to
	     * the line below it.  If there is no valid entry, use w_topline.
	     * Find the first valid w_lines[] entry below mod_bot.  Set lnumb
	     * to this line.  If there is no valid entry, use MAXLNUM. */
	    lnumt = wp->w_topline;
	    lnumb = MAXLNUM;
	    for (i = 0; i < wp->w_lines_valid; ++i)
		if (wp->w_lines[i].wl_valid)
		{
		    if (wp->w_lines[i].wl_lastlnum < mod_top)
			lnumt = wp->w_lines[i].wl_lastlnum + 1;
		    if (lnumb == MAXLNUM && wp->w_lines[i].wl_lnum >= mod_bot)
			lnumb = wp->w_lines[i].wl_lnum;
		}

	    (void)hasFoldingWin(wp, mod_top, &mod_top, NULL, TRUE, NULL);
	    if (mod_top > lnumt)
		mod_top = lnumt;

	    /* Now do the same for the bottom line (one above mod_bot). */
	    --mod_bot;
	    (void)hasFoldingWin(wp, mod_bot, NULL, &mod_bot, TRUE, NULL);
	    ++mod_bot;
	    if (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	/* Never start above w_topline */
	if (mod_top != 0 && mod_top < wp->w_topline)
	    mod_top = wp->w_topline;
    }

    /*
     * When only displaying the lines at the top, set top_end.  Used when
     * window has scrolled down for msg_scrolled.
     */
    if (type == REDRAW_TOP)
    {
	j = 0;
	for (i = 0; i < wp->w_lines_valid; ++i)
	{
	    j += wp->w_lines[i].wl_size;
	    if (j >= wp->w_upd_rows)
	    {
		top_end = j;
		break;
	    }
	}
	if (top_end == 0)
	    /* not found (cannot happen?): redraw everything */
	    type = NOT_VALID;
	else
	    /* top area defined, the rest is VALID */
	    type = VALID;
    }

    /*
     * If there are no changes on the screen that require a complete redraw,
     * handle three cases:
     * 1: we are off the top of the screen by a few lines: scroll down
     * 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
     * 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
     *    w_lines[] that needs updating.
     * Don't do this when w_topline is the first changed line, the scrolling
     * will be done further down then.
     * Also don't do this when the window is part of a vertical split.
     */
    if ((type == VALID || type == INVERTED)
	    && !(buf->b_mod_set && wp->w_topline == mod_top)
#ifdef FEAT_VERTSPLIT
	    && W_WIDTH(wp) == Columns
#endif
	    )
    {
	if (wp->w_topline < wp->w_lines[0].wl_lnum && wp->w_lines[0].wl_valid)
	{
	    /*
	     * New topline is above old topline: May scroll down.
	     */
#ifdef FEAT_FOLDING
	    if (hasAnyFolding(wp))
	    {
		linenr_t ln;

		/* count the number of lines we are off, counting a sequence
		 * of folded lines as one */
		j = 0;
		for (ln = wp->w_topline; ln < wp->w_lines[0].wl_lnum; ++ln)
		{
		    ++j;
		    if (j >= wp->w_height - 2)
			break;
		    (void)hasFoldingWin(wp, ln, NULL, &ln, TRUE, NULL);
		}
	    }
	    else
#endif
		j = wp->w_lines[0].wl_lnum - wp->w_topline;
	    if (j < wp->w_height - 2)		/* not too far off */
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_lines[0].wl_lnum - 1);
		if (i < wp->w_height - 2)	/* less than a screen off */
		{
		    /*
		     * Try to insert the correct number of lines.
		     * If not the last window, delete the lines at the bottom.
		     * win_ins_lines may fail when the terminal can't do it.
		     */
		    if (i > 0)
			check_for_delay(FALSE);
		    if (win_ins_lines(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			if (wp->w_lines_valid != 0)
			{
			    /* Need to update rows that are new, stop at the
			     * first one that scrolled down. */
			    top_end = i;

			    if ((wp->w_lines_valid += j) > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    for (idx = wp->w_lines_valid; idx - j >= 0; idx--)
				wp->w_lines[idx] = wp->w_lines[idx - j];
			}
		    }
		    else
			mid_start = 0;		/* redraw all lines */
		}
		else
		    mid_start = 0;		/* redraw all lines */
	    }
	    else
		mid_start = 0;		/* redraw all lines */
	}
	else
	{
	    /*
	     * New topline is at or below old topline: May scroll up.
	     * When topline didn't change, find first entry in w_lines[] that
	     * needs updating.
	     */

	    /* try to find wp->w_topline in wp->w_lines[].wl_lnum */
	    j = -1;
	    row = 0;
	    for (i = 0; i < wp->w_lines_valid; i++)
	    {
		if (wp->w_lines[i].wl_valid
			&& wp->w_lines[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    break;
		}
		row += wp->w_lines[i].wl_size;
	    }
	    if (j == -1)
	    {
		/* if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		 * lines */
		mid_start = 0;
	    }
	    else
	    {
		/*
		 * Try to delete the correct number of lines.
		 * wp->w_topline is at wp->w_lines[i].wl_lnum.
		 */
		if (row)
		{
		    check_for_delay(FALSE);
		    if (win_del_lines(wp, 0, row, FALSE, wp == firstwin) == OK)
			bot_start = wp->w_height - row;
		    else
			mid_start = 0;		/* redraw all lines */
		}
		if ((row == 0 || bot_start < 999) && wp->w_lines_valid != 0)
		{
		    /*
		     * Skip the lines (below the deleted lines) that are still
		     * valid and don't need redrawing.	Copy their info
		     * upwards, to compensate for the deleted lines.  Set
		     * bot_start to the first row that needs redrawing.
		     */
		    bot_start = 0;
		    idx = 0;
		    for (;;)
		    {
			wp->w_lines[idx] = wp->w_lines[j];
			/* stop at line that didn't fit */
			if (bot_start + row + (int)wp->w_lines[j].wl_size
							       > wp->w_height)
			{
			    wp->w_lines_valid = idx + 1;
			    break;
			}
			bot_start += wp->w_lines[idx++].wl_size;

			/* stop at the last valid entry in w_lines[].wl_size */
			if (++j >= wp->w_lines_valid)
			{
			    wp->w_lines_valid = idx;
			    break;
			}
		    }
		}
	    }
	}

	/* When starting redraw in the first line, redraw all lines.  When
	 * there is only one window it's probably faster to clear the screen
	 * first. */
	if (mid_start == 0)
	{
	    mid_end = wp->w_height;
	    if (lastwin == firstwin)
		screenclear();
	}
    }
    else
    {
	/* Not VALID or INVERTED: redraw all lines. */
	mid_start = 0;
	mid_end = wp->w_height;
    }

#ifdef FEAT_VISUAL
    /* check if we are updating or removing the inverted part */
    if ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum && type != NOT_VALID))
    {
	linenr_t    from, to;

	if (VIsual_active)
	{
	    if (VIsual_active && VIsual_mode != wp->w_old_visual_mode)
	    {
		/*
		 * If the type of Visual selection changed, redraw the whole
		 * selection.
		 */
		if (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		else
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
	    }
	    else
	    {
		/*
		 * Find the line numbers that need to be updated: The lines
		 * between the old cursor position and the current cursor
		 * position.  Also check if the Visual position changed.
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
		    if (from == 0)	/* Visual mode just started */
			from = to;
		}

		if (VIsual.lnum != wp->w_old_visual_lnum)
		{
		    if (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    if (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    if (VIsual.lnum < from)
			from = VIsual.lnum;
		    if (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    /*
	     * If in block mode and changed column or curwin->w_curswant:
	     * update all lines.
	     * First compute the actual start and end column.
	     */
	    if (VIsual_mode == Ctrl_V)
	    {
		colnr_t	from1, from2, to1, to2;

		if (lt(VIsual, curwin->w_cursor))
		{
		    getvvcol(wp, &VIsual, &from1, NULL, &to1);
		    getvvcol(wp, &curwin->w_cursor, &from2, NULL, &to2);
		}
		else
		{
		    getvvcol(wp, &curwin->w_cursor, &from1, NULL, &to1);
		    getvvcol(wp, &VIsual, &from2, NULL, &to2);
		}
		if (from2 < from1)
		    from1 = from2;
		if (to2 > to1)
		{
		    if (*p_sel == 'e' && from2 - 1 >= to1)
			to1 = from2 - 1;
		    else
			to1 = to2;
		}
		++to1;
		if (curwin->w_curswant == MAXCOL)
		    to1 = MAXCOL;

		if (from1 != wp->w_old_cursor_fcol
			|| to1 != wp->w_old_cursor_lcol)
		{
		    if (from > VIsual.lnum)
			from = VIsual.lnum;
		    if (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = from1;
		wp->w_old_cursor_lcol = to1;
	    }
	}
	else
	{
	    /* Use the line numbers of the old Visual area. */
	    if (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    else
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	/*
	 * There is no need to update lines above the top of the window.
	 */
	if (from < wp->w_topline)
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
	 * (TODO: assumes all w_lines[] entries are valid, is that right?)
	 */
	if (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    mid_start = 0;
	    while (lnum < from && idx < wp->w_lines_valid)	/* find start */
	    {
		mid_start += wp->w_lines[idx++].wl_size;
#ifdef FEAT_FOLDING
		if (idx < wp->w_lines_valid)
		    lnum = wp->w_lines[idx].wl_lnum;
		else
#endif
		    ++lnum;
	    }
	    srow = mid_start;
	    mid_end = wp->w_height;
	    for ( ; idx < wp->w_lines_valid; ++idx)		/* find end */
	    {
		if (wp->w_lines[idx].wl_lnum >= to + 1)
		{
		    /* Only update up to first row of this line */
		    mid_end = srow;
		    break;
		}
		srow += wp->w_lines[idx].wl_size;
	    }
	}
    }

    if (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_curswant = curwin->w_curswant;
    }
    else
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
    }
#endif /* FEAT_VISUAL */

#ifdef FEAT_SEARCH_EXTRA
    search_hl_buf = buf;
    search_hl_lnum = 0;		/* disable any previous match */
    first_search_lnum = 0;
#endif

    /*
     * Update all the window rows.
     */
    idx = 0;		/* first entry in w_lines[].wl_size */
    row = 0;
    srow = 0;
    lnum = wp->w_topline;	/* first line shown in window */
    for (;;)
    {
	/* stop updating when reached the end of the window (check for _past_
	 * the end of the window is at the end of the loop) */
	if (row == wp->w_height)
	{
	    didline = TRUE;
	    break;
	}

	/* stop updating when hit the end of the file */
	if (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    break;
	}

	/* Remember the starting row of the line that is going to be dealt
	 * with.  It is used further down when the line doesn't fit. */
	srow = row;

	/*
	 * Update a line when it is in an area that needs updating, when it
	 * has changes or w_lines[idx] is invalid.
	 */
	if (row < top_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_SEARCH_EXTRA
		|| top_to_mod
#endif
		|| (row >= bot_start)
		|| idx >= wp->w_lines_valid
		|| (mod_top != 0
		    && (lnum == mod_top
			|| (lnum >= mod_top
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_LINE
				    && syntax_present(buf)
				    && syntax_check_changed(lnum))
#endif
				)))))
	{
#ifdef FEAT_SEARCH_EXTRA
	    if (lnum == mod_top)
		top_to_mod = FALSE;
#endif

	    /*
	     * When at start of changed lines: May scroll following lines
	     * up or down to minimize redrawing.
	     * Don't scroll when dollar_vcol is non-zero, keep the "$".
	     */
	    if (mod_top != 0 && lnum == mod_top && mod_bot != MAXLNUM
			     && !(dollar_vcol != 0 && mod_bot == mod_top + 1))
	    {
		int		old_rows = 0;
		int		new_rows = 0;
		int		xtra_rows;
		linenr_t	l;


		/* Count the old number of window rows, using w_lines[], which
		 * should still contain the sizes for the lines as they are
		 * currently displayed. */
		for (i = idx; i < wp->w_lines_valid; ++i)
		{
		    /* Only valid lines have a meaningful wl_lnum.  Invalid
		     * lines are part of the changed area. */
		    if (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lnum == mod_bot)
			break;
		    old_rows += wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
		    if (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lastlnum + 1 == mod_bot)
		    {
			/* Must have found the last valid entry above mod_bot.
			 * Add following invalid entries. */
			++i;
			while (i < wp->w_lines_valid
						  && !wp->w_lines[i].wl_valid)
			    old_rows += wp->w_lines[i++].wl_size;
			break;
		    }
#endif
		}

		/* When able to count old number of rows, count new window
		 * rows, and may insert/delete lines */
		if (i < wp->w_lines_valid)
		{
		    j = idx;
		    for (l = lnum; l < mod_bot; ++l)
		    {
#ifdef FEAT_FOLDING
			if (hasFoldingWin(wp, l, NULL, &l, TRUE, NULL))
			    ++new_rows;
			else
#endif
			    new_rows += plines_win(wp, l, TRUE);
			++j;
			if (new_rows > wp->w_height - row - 2)
			{
			    /* it's getting too much, must redraw the rest */
			    new_rows = 9999;
			    break;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    if (xtra_rows < 0)
		    {
			/* May scroll text up.  If there is not enough
			 * remaining text or scrolling fails, must redraw the
			 * rest.  If scrolling works, must redraw the text
			 * below the scrolled text. */
			if (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			else
			{
			    check_for_delay(FALSE);
			    if (win_del_lines(wp, row,
					    -xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    else
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    else if (xtra_rows > 0)
		    {
			/* May scroll text down.  If there is not enough
			 * remaining text of scrolling fails, must redraw the
			 * rest. */
			if (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			else
			{
			    check_for_delay(FALSE);
			    if (win_ins_lines(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			}
		    }

		    /* When not updating the rest, may need to move w_lines[]
		     * entries. */
		    if (mod_bot != MAXLNUM && i != j)
		    {
			if (j < i)
			{
			    int x = row + new_rows;

			    /* move entries in w_lines[] upwards */
			    for (;;)
			    {
				/* stop at last valid entry in w_lines[] */
				if (i >= wp->w_lines_valid)
				{
				    wp->w_lines_valid = j;
				    break;
				}
				wp->w_lines[j] = wp->w_lines[i];
				/* stop at a line that won't fit */
				if (x + (int)wp->w_lines[j].wl_size
							   > wp->w_height)
				{
				    wp->w_lines_valid = j + 1;
				    break;
				}
				x += wp->w_lines[j++].wl_size;
				++i;
			    }
			    if (bot_start > x)
				bot_start = x;
			}
			else /* j > i */
			{
			    /* move entries in w_lines[] downwards */
			    j -= i;
			    wp->w_lines_valid += j;
			    if (wp->w_lines_valid > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    for (i = wp->w_lines_valid; i - j >= idx; --i)
				wp->w_lines[i] = wp->w_lines[i - j];
			}
		    }
		}
		/* When buffer lines have been inserted/deleted, and
		 * insering/deleting window lines is not possible, need to
		 * check for redraw until the end of the window. */
		else if (buf->b_mod_xlines != 0)
		    bot_start = 0;

		/* When inserting or deleting lines and 'number' is set:
		 * Redraw all lines below the change to update the line
		 * numbers. */
		if (buf->b_mod_xlines != 0 && wp->w_p_nu)
		    bot_start = 0;
	    }

#ifdef FEAT_FOLDING
	    /*
	     * When lines are folded, display one line for all of them.
	     * Otherwise, display normally (can be several display lines when
	     * 'wrap' is on).
	     */
	    fold_count = foldedCount(wp, lnum, &win_fold_level);
	    if (fold_count)
	    {
		fold_line(wp, fold_count, win_fold_level, lnum, row);
		++row;
		--fold_count;
		wp->w_lines[idx].wl_folded = TRUE;
		wp->w_lines[idx].wl_lastlnum = lnum + fold_count;
#ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
#endif
	    }
	    else
#endif
	    {

#ifdef FEAT_SEARCH_EXTRA
		/*
		 * When using a multi-line pattern, start searching at the top
		 * of the window or just after a closed fold.
		 */
		if (search_hl_rm.regprog != NULL
			&& search_hl_lnum == 0
			&& re_multiline(search_hl_rm.regprog))
		{
		    int		n;

		    if (first_search_lnum == 0)
		    {
#ifdef FEAT_FOLDING
			for (first_search_lnum = lnum;
				first_search_lnum > wp->w_topline;
							  --first_search_lnum)
			    if (hasFoldingWin(wp, first_search_lnum - 1,
						      NULL, NULL, TRUE, NULL))
				break;
#else
			first_search_lnum = wp->w_topline;
#endif
		    }
		    n = 0;
		    while (first_search_lnum < lnum)
		    {
			next_search_hl(first_search_lnum, (colnr_t)n);
			if (search_hl_lnum != 0)
			{
			    first_search_lnum = search_hl_lnum
						+ search_hl_rm.endpos[0].lnum;
			    n = search_hl_rm.endpos[0].col;
			}
			else
			{
			    ++first_search_lnum;
			    n = 0;
			}

		    }
		}
#endif

		row = win_line(wp, lnum, srow, wp->w_height);
#ifdef FEAT_FOLDING
		wp->w_lines[idx].wl_folded = FALSE;
		wp->w_lines[idx].wl_lastlnum = lnum;
		/* Past start of open fold: make fold level positive */
		if (win_fold_level < 0)
		    win_fold_level = -win_fold_level;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_LINE;
#endif
	    }

	    wp->w_lines[idx].wl_lnum = lnum;
	    wp->w_lines[idx].wl_valid = TRUE;
	    if (row > wp->w_height)	/* past end of screen */
	    {
		/* we may need the size of that too long line later on */
		if (dollar_vcol == 0)
		    wp->w_lines[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		break;
	    }
	    if (dollar_vcol == 0)
		wp->w_lines[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_count + 1;
#else
	    ++lnum;
#endif
	}
	else
	{
	    /* This line does not need updating, advance to the next one */
	    row += wp->w_lines[idx++].wl_size;
	    if (row > wp->w_height)	/* past end of screen */
		break;
#ifdef FEAT_FOLDING
	    lnum = wp->w_lines[idx - 1].wl_lastlnum + 1;
#else
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
	}

	if (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    break;
	}
    }
    if (idx > wp->w_lines_valid)
	wp->w_lines_valid = idx;

    /*
     * If we didn't hit the end of the file, and we didn't finish the last
     * line we were working on, then the line didn't fit.
     */
    wp->w_empty_rows = 0;
    if (!eof && !didline)
    {
	if (lnum == wp->w_topline)
	{
	    /*
	     * Single line that does not fit!
	     * Don't overwrite it, it can be edited.
	     */
	    wp->w_botline = lnum + 1;
	}
	else if (*p_dy != NUL)
	{
	    /*
	     * Last line isn't finished: Display "@@@" at the end.
	     */
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (int)W_ENDCOL(wp) - 3, (int)W_ENDCOL(wp),
		    '@', '@', hl_attr(HLF_AT));
	    wp->w_botline = lnum;
	}
	else
	{
	    win_draw_end(wp, '@', srow);
	    wp->w_botline = lnum;
	}
    }
    else
    {
#ifdef FEAT_VERTSPLIT
	draw_vsep_win(wp, row);
#endif
	/* make sure the rest of the screen is blank */
	/* put '~'s on rows that aren't part of the file. */
	win_draw_end(wp, '~', row);

	if (eof)		/* we hit the end of the file */
	    wp->w_botline = buf->b_ml.ml_line_count + 1;
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
    if (wp == curwin && wp->w_botline != old_botline && !recursive)
    {
	recursive = TRUE;
	curwin->w_valid &= ~VALID_TOPLINE;
	update_topline();	/* may invalidate w_botline again */
	if (must_redraw)
	{
	    /* Don't update for changes in buffer again. */
	    i = curbuf->b_mod_set;
	    curbuf->b_mod_set = FALSE;
	    win_update(curwin);
	    must_redraw = 0;
	    curbuf->b_mod_set = i;
	}
	recursive = FALSE;
    }

    wp->w_redr_type = 0;
}

/*
 * Clear the rest of the window and mark the unused lines with "c".
 */
    static void
win_draw_end(wp, c, row)
    win_t	*wp;
    int		c;
    int		row;
{
#ifdef FEAT_RIGHTLEFT
    if (wp->w_p_rl)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_WINCOL(wp), W_ENDCOL(wp) - 1,
		' ', ' ', hl_attr(HLF_AT));
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_ENDCOL(wp) - 1, W_ENDCOL(wp),
		c, ' ', hl_attr(HLF_AT));
    }
    else
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_WINCOL(wp), (int)W_ENDCOL(wp),
		c, ' ', hl_attr(HLF_AT));
    wp->w_empty_rows = wp->w_height - row;
}

#ifdef FEAT_FOLDING
/*
 * Display one folded line.
 */
    static void
fold_line(wp, fold_count, level, lnum, row)
    win_t	*wp;
    long	fold_count;
    int		level;
    linenr_t	lnum;
    int		row;
{
    char_u	buf[31];
    char_u	dashes[51];
    enum hlf_value attr;
    pos_t	*top, *bot;
    linenr_t	lnume = lnum + fold_count - 1;
    int		from;
    int		len;
    char_u	*p;
    char_u	*text = NULL;

    /* Compose the folded-line string with 'foldtext', if set. */
    if (*wp->w_p_fdt != NUL)
    {
	for (p = wp->w_p_fdt + 1; *p; ++p)
	{
	    if (*p == '\\' && p[1] != NUL)
		++p;
	    else if (*p == *wp->w_p_fdt)
	    {
		*p = NUL;

		/* Set "reg_folded_count" to number of folded lines, for "\f"
		 * in substitute pattern. */
		sprintf((char *)buf, "%3ld", fold_count);
		reg_folded_count = buf;

		/* Set "reg_level_string" to a string of "level" dashes, for
		 * "\d" in substitute pattern. */
		if (level > 50)
		    level = 50;
		reg_level_string = dashes;
		vim_memset(dashes, '-', (size_t)level);
		dashes[level] = NUL;

		/* Match the pattern and replace with substitution. */
		text = do_string_sub(ml_get_buf(wp->w_buffer, lnum, FALSE),
					wp->w_p_fdt + 1, p + 1, (char_u *)"");
		*p = *wp->w_p_fdt;
		reg_folded_count = NULL;
		reg_level_string = NULL;
		if (text != NULL)
		{
		    /* Replace unprintable characters, if there are any.  But
		     * replace a TAB with a space. */
		    for (p = text; *p; ++p)
		    {
#ifdef FEAT_MBYTE
			if (has_mbyte && (len = mb_ptr2len_check(p)) > 1)
			    p += len - 1;
			else
#endif
			    if (*p == TAB)
			    *p = ' ';
			else if (ptr2cells(p) > 1)
			    break;
		    }
		    if (*p)
		    {
			p = transstr(text);
			vim_free(text);
			text = p;
		    }
		}
		break;
	    }
	}
    }
    if (text == NULL)
    {
	sprintf((char *)buf, "+--- %ld lines folded ", fold_count);
	text = buf;
    }
    len = STRLEN(text);
    if (wp->w_p_fdc)
	len += 2;
    if (len > W_WIDTH(wp))
	len = W_WIDTH(wp);
    if (wp->w_p_fdc)
    {
	STRNCPY(current_ScreenLine + 2, text, len - 2);
	current_ScreenLine[0] = foldcolumn_char();
	current_ScreenLine[1] = ' ';
    }
    else
	STRNCPY(current_ScreenLine, text, len);
    while (len < W_WIDTH(wp))
	current_ScreenLine[len++] = fill_fold;
    if (text != buf)
	vim_free(text);

    /*
     * If all folded lines are in the Visual area, highlight the line.
     */
    attr = HLF_F;
#ifdef FEAT_VISUAL
    if (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	/* Visual is after curwin->w_cursor */
	if (ltoreq(curwin->w_cursor, VIsual))
	{
	    top = &curwin->w_cursor;
	    bot = &VIsual;
	}
	else	/* Visual is before curwin->w_cursor */
	{
	    top = &VIsual;
	    bot = &curwin->w_cursor;
	}
	if (lnum >= top->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > top->lnum
			    || (lnum == top->lnum
				&& top->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
		>= STRLEN(ml_get_buf(wp->w_buffer, lnume, FALSE)))))))
	    attr = HLF_V;
    }

    if (attr == HLF_V && VIsual_mode == Ctrl_V)
    {
	from = 0;
	if (wp->w_old_cursor_fcol < (colnr_t)W_WIDTH(wp))
	{
	    vim_memset(current_ScreenAttrs,
			       hl_attr(HLF_F), (size_t)wp->w_old_cursor_fcol);
	    if (wp->w_old_cursor_lcol < (colnr_t)W_WIDTH(wp))
		from = wp->w_old_cursor_lcol;
	    else
		from = W_WIDTH(wp);
	    vim_memset(current_ScreenAttrs + wp->w_old_cursor_fcol,
		      hl_attr(HLF_V), (size_t)(from - wp->w_old_cursor_fcol));
	}
	if (from < W_WIDTH(wp))
	    vim_memset(current_ScreenAttrs + from,
				hl_attr(HLF_F), (size_t)(W_WIDTH(wp) - from));
    }
    else
#endif
	vim_memset(current_ScreenAttrs, hl_attr(attr), (size_t)(W_WIDTH(wp)));
    SCREEN_LINE(row + W_WINROW(wp), W_WINCOL(wp), W_WIDTH(wp),
							  W_WIDTH(wp), FALSE);

    /*
     * Update w_cline_height and w_cline_folded if the cursor line was
     * updated (saves a call to plines() later).
     */
    if (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_row = row;
	curwin->w_cline_height = 1;
	curwin->w_cline_folded = TRUE;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }
}

/*
 * Get the character to put in the 'foldcolumn' for the current line.
 */
    static int
foldcolumn_char()
{
    if (win_fold_level == 0)		/* no fold here */
	return ' ';
    if (win_fold_level < 0)		/* start of open fold */
	return '-';
    if (win_fold_level <= 9)
	return '0' + win_fold_level;	/* continued fold */
    return '>';				/* continued deep fold */
}
#endif /* FEAT_FOLDING */

/*
 * Display line "lnum" of window 'wp' on the screen.
 * Start at row "startrow", stop when "endrow" is reached.
 * wp->w_virtcol needs to be valid.
 *
 * Return the number of last row the line occupies.
 */

    static int
win_line(wp, lnum, startrow, endrow)
    win_t	*wp;
    linenr_t	lnum;
    int		startrow;
    int		endrow;
{
    int		col;			/* visual column on screen */
    unsigned	off;			/* offset in ScreenLines/ScreenAttrs */
    int		c = 0;			/* init for GCC */
    long	vcol = 0;		/* virtual column (for tabs) */
    char_u	*line;			/* current line */
    char_u	*ptr;			/* current position in "line" */
    int		row;			/* row in the window, excl w_winrow */
    int		screen_row;		/* row on the screen, incl w_winrow */

    char_u	extra[18];		/* "%ld" must fit in here */
    int		n_extra = 0;		/* number of extra chars */
    char_u	*p_extra = NULL;	/* string of extra chars */
    int		c_extra = NUL;		/* extra chars, all the same */
    int		extra_attr = 0;		/* attributes when n_extra != 0 */

    /* saved "extra" items for when draw_state becomes WL_LINE (again) */
    int		saved_n_extra = 0;
    char_u	*saved_p_extra = NULL;
    int		saved_c_extra = 0;
    int		saved_char_attr = 0;

    int		n_attr = 0;		/* chars with special attr */
    int		saved_attr2 = 0;	/* char_attr saved for n_attr */

    int		n_skip = 0;		/* nr of chars to skip for 'nowrap' */

    int		fromcol, tocol;		/* start/end of inverting */
    int		noinvcur = FALSE;	/* don't invert the cursor */
#ifdef FEAT_VISUAL
    pos_t	*top, *bot;
#endif
    pos_t	pos;
    long	v;

    int		char_attr = 0;		/* attributes for next character */
    int		area_highlighting = FALSE; /* Visual or incsearch highlighting
					      in this line */
    int		attr = 0;		/* attributes for area highlighting */
    int		area_attr = 0;		/* attributes desired by highlighting */
    int		search_attr = 0;	/* attributes desired by 'searchhl' */
#ifdef FEAT_SYN_HL
    int		syntax_attr = 0;	/* attributes desired by syntax */
    int		has_syntax = FALSE;	/* this buffer has syntax highl. */
#endif
    int		extra_check;		/* has syntax or linebreak */
#ifdef FEAT_SEARCH_EXTRA
    char_u	*search_hl_start = NULL;
    char_u	*search_hl_end = NULL;
#endif
#ifdef FEAT_MBYTE
    int		multi_attr = 0;		/* attributes desired by multibyte */
    int		mb_l = 1;		/* multi-byte byte length */
    int		mb_c = 0;		/* decoded multi-byte character */
    int		mb_utf8 = FALSE;	/* screen char is UTF-8 char */
    int		u8c_c1 = 0;		/* first composing UTF-8 char */
    int		u8c_c2 = 0;		/* second composing UTF-8 char */
#endif
    colnr_t	trailcol = MAXCOL;	/* start of trailing spaces */
#ifdef FEAT_LINEBREAK
    int		need_showbreak = FALSE;
#endif
#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    int		save_got_int;
#endif
#ifdef FEAT_SIGNS
    int_u	type;			/* sign type (if signs are used) */
    int		debug_attr = 0;		/* debugger atrribute override */
#endif

    /* draw_state: items that are drawn in sequence: */
#define WL_START	0		/* nothing done yet */
#ifdef FEAT_FOLDING
# define WL_FOLD	WL_START + 1	/* 'foldcolumn' */
#else
# define WL_FOLD	WL_START
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	WL_FOLD + 1	/* column for signs */
#else
# define WL_SIGN	WL_FOLD		/* column for signs */
#endif
#define WL_NR		WL_SIGN + 1	/* line number */
#ifdef FEAT_LINEBREAK
# define WL_SBR		WL_NR + 1	/* 'showbreak' */
#else
# define WL_SBR		WL_NR
#endif
#define WL_LINE		WL_SBR + 1	/* text in the line */
    int		draw_state = WL_START;	/* what to draw next */


    if (startrow > endrow)		/* past the end already! */
	return startrow;

    row = startrow;
    screen_row = row + W_WINROW(wp);

    /*
     * To speed up the loop below, set extra_check when there is linebreak,
     * trailing white space and/or syntax processing to be done.
     */
#ifdef FEAT_LINEBREAK
    extra_check = wp->w_p_lbr;
#else
    extra_check = 0;
#endif
#ifdef FEAT_SYN_HL
    if (syntax_present(wp->w_buffer))
    {
	syntax_start(wp, lnum);
	has_syntax = TRUE;
	extra_check = TRUE;
    }
#endif

#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    /* reset got_int, otherwise regexp won't work */
    save_got_int = got_int;
    got_int = 0;
#endif

    /*
     * handle visual active in this window
     */
    fromcol = -10;
    tocol = MAXCOL;
#ifdef FEAT_VISUAL
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
	if (VIsual_mode == Ctrl_V)	/* block mode */
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
	    {
		if (VIsual_mode == 'V')	/* linewise */
		    fromcol = 0;
		else
		    getvcol(wp, top, (colnr_t *)&fromcol, NULL, NULL);
	    }
	    if (VIsual_mode != 'V' && lnum == bot->lnum)
	    {
		if (*p_sel == 'e' && bot->col == 0)
		{
		    fromcol = -10;
		    tocol = MAXCOL;
		}
		else
		{
		    pos = *bot;
		    if (*p_sel == 'e')
			--pos.col;
		    getvcol(wp, &pos, NULL, NULL, (colnr_t *)&tocol);
		    ++tocol;
		}
	    }
	}

#ifndef MSDOS
	/* Check if the character under the cursor should not be inverted */
	if (!highlight_match && lnum == curwin->w_cursor.lnum && wp == curwin
# ifdef FEAT_GUI
		&& !gui.in_use
# endif
		)
	    noinvcur = TRUE;
#endif

	/* if inverting in this line set area_highlighting */
	if (fromcol >= 0)
	{
	    area_highlighting = TRUE;
	    attr = hl_attr(HLF_V);
#if defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
	    if (clipboard.available && !clipboard.owned && clip_isautosel())
		attr = hl_attr(HLF_VNC);
#endif
	}
    }

    /*
     * handle 'insearch' and ":s///c" highlighting
     */
    else
#endif /* FEAT_VISUAL */
	if (highlight_match
	    && wp == curwin
	    && lnum >= curwin->w_cursor.lnum
	    && lnum <= curwin->w_cursor.lnum + search_match_lines)
    {
	if (lnum == curwin->w_cursor.lnum)
	    getvcol(curwin, &(curwin->w_cursor),
					     (colnr_t *)&fromcol, NULL, NULL);
	else
	    fromcol = 0;
	if (lnum == curwin->w_cursor.lnum + search_match_lines)
	{
	    pos.lnum = lnum;
	    pos.col = search_match_endcol;
	    getvcol(curwin, &pos, (colnr_t *)&tocol, NULL, NULL);
	}
	else
	    tocol = MAXCOL;
	if (fromcol == tocol)		/* do at least one character */
	    tocol = fromcol + 1;	/* happens when past end of line */
	area_highlighting = TRUE;
	attr = hl_attr(HLF_I);
    }

#ifdef FEAT_SIGNS
    type = buf_getsigntype(wp->w_buffer, lnum);
    if (type != 0)
    {
	area_highlighting = TRUE;
	debug_attr = get_debug_highlight(type);
    }
#endif

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = line;

    /* find start of trailing whitespace */
    if (wp->w_p_list && lcs_trail)
    {
	trailcol = STRLEN(ptr);
	while (trailcol > (colnr_t)0 && vim_iswhite(ptr[trailcol - 1]))
	    --trailcol;
	trailcol += ptr - line;
	extra_check = TRUE;
    }

    /*
     * 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
     * first character to be displayed.
     */
    if (wp->w_p_wrap)
	v = wp->w_skipcol;
    else
	v = wp->w_leftcol;
    if (v > 0)
    {
#ifdef FEAT_MBYTE
	char_u	*prev_ptr = ptr;
#endif
	while (vcol < v && *ptr != NUL)
	{
	    c = win_lbr_chartabsize(wp, ptr, (colnr_t)vcol, NULL);
	    vcol += c;
#ifdef FEAT_MBYTE
	    prev_ptr = ptr;
	    if (has_mbyte)
		ptr += mb_ptr2len_check(ptr);
	    else
#endif
		++ptr;
	}
	/* Handle a character that's not completely on the screen: Put ptr at
	 * that character but skip the first few screen characters. */
	if (vcol > v)
	{
	    vcol -= c;
#ifdef FEAT_MBYTE
	    ptr = prev_ptr;
#else
	    --ptr;
#endif
	    n_skip = v - vcol;
	}

	/*
	 * Adjust for when the inverted text is before the screen,
	 * and when the start of the inverted text is before the screen.
	 */
	if (tocol <= vcol)
	    fromcol = 0;
	else if (fromcol >= 0 && fromcol < vcol)
	    fromcol = vcol;

#ifdef FEAT_LINEBREAK
	/* When w_skipcol is non-zero, first line needs 'showbreak' */
	if (wp->w_p_wrap)
	    need_showbreak = TRUE;
#endif
    }

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Handle highlighting the last used search pattern.
     */
    if (search_hl_rm.regprog != NULL)
    {
	v = ptr - line;
	next_search_hl(lnum, (colnr_t)v);

	/* Need to get the line again, a multi-line regexp may have made it
	 * invalid. */
	line = ml_get_buf(wp->w_buffer, lnum, FALSE);
	ptr = line + v;

	if (search_hl_lnum != 0)
	{
	    if (search_hl_lnum == lnum)
		search_hl_start = line + search_hl_rm.startpos[0].col;
	    else
		search_hl_start = line;
	    if (search_hl_rm.endpos[0].lnum == lnum - search_hl_lnum)
		search_hl_end = line + search_hl_rm.endpos[0].col;
	    else
		search_hl_end = line + MAXCOL;
	    /* Highlight one character for an empty match. */
	    if (*search_hl_start == NUL && search_hl_start > line)
		--search_hl_start;
	    else if (search_hl_start == search_hl_end)
		++search_hl_end;
	    if (search_hl_start < ptr)  /* match at leftcol */
		search_attr = search_hl_attr;
	    area_highlighting = TRUE;
	}
	else
	{
	    search_hl_start = NULL;
	    search_hl_end = NULL;
	}
    }
#endif

    off = current_ScreenLine - ScreenLines;
    col = 0;
#ifdef FEAT_RIGHTLEFT
    if (wp->w_p_rl)
    {
	col = W_WIDTH(wp) - 1;
	off += col;
    }
#endif

    /*
     * Repeat for the whole displayed line.
     */
    for (;;)
    {
	/* Skip this quickly when working on the text. */
	if (draw_state != WL_LINE)
	{
#ifdef FEAT_FOLDING
	    if (draw_state == WL_FOLD - 1 && n_extra == 0)
	    {
		draw_state = WL_FOLD;
		if (wp->w_p_fdc)
		{
		    /* Draw the 'foldcolumn'. */
		    if (row == startrow)
			extra[0] = foldcolumn_char();
		    else
			extra[0] = ' ';
		    extra[1] = ' ';
		    n_extra = 2;
		    p_extra = extra;
		    c_extra = NUL;
		    char_attr = hl_attr(HLF_F);
		}
	    }
#endif

#ifdef FEAT_SIGNS
	    if (draw_state == WL_SIGN - 1 && n_extra == 0)
	    {
		draw_state = WL_SIGN;
		if (wp->w_buffer->b_signlist != NULL)
		{
		    /* Draw a space to put the sign in. */
		    c_extra = ' ';
		    n_extra = 2;
		    char_attr = 0;
		}
	    }
#endif

	    if (draw_state == WL_NR - 1 && n_extra == 0)
	    {
		draw_state = WL_NR;
		/* Display the line number.  After the first fill with blanks
		 * when the 'n' flag isn't in 'cpo' */
		if (wp->w_p_nu
			&& (row == startrow
			    || vim_strchr(p_cpo, CPO_NUMCOL) == NULL))
		{
		    /* Draw the line number (empty space after wrapping). */
		    if (row == startrow)
		    {
			sprintf((char *)extra, "%7ld ", (long)lnum);
			if (wp->w_skipcol > 0)
			    for (p_extra = extra; *p_extra == ' '; ++p_extra)
				*p_extra = '-';
#ifdef FEAT_RIGHTLEFT
			if (wp->w_p_rl)		    /* reverse line numbers */
			{
			    char_u *c1, *c2, t;

			    for (c1 = extra, c2 = extra + STRLEN(extra) - 1;
				    c1 < c2; c1++, c2--)
			    {
				t = *c1;
				*c1 = *c2;
				*c2 = t;
			    }
			}
#endif
			p_extra = extra;
			c_extra = NUL;
		    }
		    else
			c_extra = ' ';
		    n_extra = 8;
		    char_attr = hl_attr(HLF_N);
		}
	    }

#ifdef FEAT_LINEBREAK
	    if (draw_state == WL_SBR - 1 && n_extra == 0)
	    {
		draw_state = WL_SBR;
		if (*p_sbr != NUL && need_showbreak)
		{
		    /* Draw 'showbreak' at the start of each broken line. */
		    p_extra = p_sbr;
		    c_extra = NUL;
		    n_extra = STRLEN(p_sbr);
		    char_attr = hl_attr(HLF_AT);
		    need_showbreak = FALSE;
		}
	    }
#endif

	    if (draw_state == WL_LINE - 1 && n_extra == 0)
	    {
		draw_state = WL_LINE;
		if (saved_n_extra)
		{
		    /* Continue item from end of wrapped line. */
		    n_extra = saved_n_extra;
		    c_extra = saved_c_extra;
		    p_extra = saved_p_extra;
		    char_attr = saved_char_attr;
		}
		else
		    char_attr = 0;
	    }
	}

	if (draw_state == WL_LINE && area_highlighting)
	{
	    /* handle Visual or match highlighting in this line */
	    if (((vcol == fromcol
			    && !(noinvcur
				&& (colnr_t)vcol == wp->w_virtcol
#ifdef FEAT_VIRTUALEDIT
								+ wp->w_coladd
#endif
				))
			|| (noinvcur
			    && (colnr_t)vcol == wp->w_virtcol + 1
#ifdef FEAT_VIRTUALEDIT
								+ wp->w_coladd
#endif
			    && vcol >= fromcol))
		    && vcol < tocol)
		area_attr = attr;		    /* start highlighting */
	    else if (area_attr
		    && (vcol == tocol
			|| (noinvcur
			    && (colnr_t)vcol == wp->w_virtcol
#ifdef FEAT_VIRTUALEDIT
								+ wp->w_coladd
#endif
			    )))
#ifdef FEAT_SIGNS
		area_attr = debug_attr;		    /* stop highlighting */
	    else if (debug_attr && ((fromcol == -10 && tocol == MAXCOL)
					 || (vcol < fromcol || vcol > tocol)))
		area_attr = debug_attr;
#else
		area_attr = 0;			/* stop highlighting */
#endif

#ifdef FEAT_SEARCH_EXTRA
	    /*
	     * Check for start/end of search pattern match.
	     * After end, check for start/end of next match.
	     * When another match, have to check for start again.
	     * Watch out for matching an empty string!
	     */
	    if (!n_extra)
	    {
		for (;;)
		{
		    if (search_hl_start != NULL
			    && ptr >= search_hl_start
			    && ptr < search_hl_end)
		    {
			search_attr = search_hl_attr;
		    }
		    else if (ptr == search_hl_end)
		    {
			search_attr = 0;

			v = ptr - line;
			next_search_hl(lnum, (colnr_t)v);

			/* Need to get the line again, a multi-line regexp may
			 * have made it invalid. */
			line = ml_get_buf(wp->w_buffer, lnum, FALSE);
			ptr = line + v;

			if (search_hl_lnum != 0)
			{
			    search_hl_start = line
					       + search_hl_rm.startpos[0].col;
			    if (search_hl_rm.endpos[0].lnum == 0)
				search_hl_end = line
						 + search_hl_rm.endpos[0].col;
			    else
				search_hl_end = line + MAXCOL;

			    /* Matching end-of-line: highlight last character
			     * in the line. */
			    if (*search_hl_start == NUL
						    && search_hl_start > line)
				--search_hl_start;

			    /* for a non-null match, loop to check if the
			     * match starts at the current position */
			    if (search_hl_start != search_hl_end)
				continue;

			    /* highlight empty match, try again after it */
			    ++search_hl_end;
			}
		    }
		    break;
		}
	    }
#endif

	    if (area_attr)
		char_attr = area_attr;
#ifdef FEAT_SYN_HL
	    else if (!search_attr && has_syntax)
		char_attr = syntax_attr;
#endif
	    else
		char_attr = search_attr;
	}

	/*
	 * Get the next character to put on the screen.
	 */
	/*
	 * The 'extra' array contains the extra stuff that is inserted to
	 * represent special characters (non-printable stuff).  When all
	 * characters are the same, c_extra is used.
	 * For the '$' of the 'list' option, n_extra == 1, p_extra == "".
	 */
#ifdef FEAT_MBYTE
	mb_l = 1;
#endif
	if (n_extra)
	{
	    if (c_extra)
	    {
		c = c_extra;
#ifdef FEAT_MBYTE
		mb_c = c;	/* doesn't handle multi-byte! */
#endif
	    }
	    else
	    {
		c = *p_extra;
#ifdef FEAT_MBYTE
		if (has_mbyte)
		{
		    mb_c = c;
		    if (cc_utf8)
		    {
			/* If the UTF-8 character is more than one byte:
			 * Decode it into "mb_c". */
			mb_l = mb_ptr2len_check(p_extra);
			mb_utf8 = FALSE;
			if (mb_l > n_extra)
			    mb_l = 1;
			else if (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(p_extra, &u8c_c1, &u8c_c2);
			    mb_utf8 = TRUE;
			}
		    }
		    else
		    {
			/* if this is a DBCS character, put it in "mb_c" */
			mb_l = MB_BYTE2LEN(c);
			if (mb_l >= n_extra)
			    mb_l = 1;
			else if (mb_l > 1)
			    mb_c = (c << 8) + p_extra[1];
		    }
		    /* if a double-width char doesn't fit use a '>' */
		    if (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (col <= 0) :
# endif
				    (col >= W_WIDTH(wp) - 1)
			    && mb_char2cells(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			++n_extra;
			--p_extra;
			/* FIXME: Bad hack! however it works */
			extra_attr = char_attr;
			n_attr = 2;
			multi_attr = hl_attr(HLF_AT);
		    }
		    else
		    {
			n_extra -= mb_l - 1;
			p_extra += mb_l - 1;
		    }
		}
#endif
		++p_extra;
	    }
	    --n_extra;
	}
	else
	{
	    /*
	     * Get a character from the line itself.
	     */
	    c = *ptr;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		mb_c = c;
		if (cc_utf8)
		{
		    /* If the UTF-8 character is more than one byte: Decode it
		     * into "mb_c". */
		    mb_l = mb_ptr2len_check(ptr);
		    mb_utf8 = FALSE;
		    if (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, &u8c_c1, &u8c_c2);
			/* Overlong encoded ASCII or ASCII with composing char
			 * is displayed normally, except a NUL. */
			if (mb_c < 0x80)
			    c = mb_c;
			mb_utf8 = TRUE;
		    }
		    if ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && !vim_isprintc(mb_c)))
		    {
			/*
			 * Illegal UTF-8 byte: display as <xx>.
			 */
			transchar_hex(extra, mb_c);
			p_extra = extra;
			c = *p_extra++;
			mb_c = c;
			mb_utf8 = FALSE;
			n_extra = STRLEN(p_extra);
			c_extra = NUL;
			if (!area_attr && !search_attr)
			{
			    n_attr = n_extra + 1;
			    extra_attr = hl_attr(HLF_AT);
			    saved_attr2 = char_attr; /* save current attr */
			}
		    }
		    else if (mb_l == 0)  /* at the NUL at end-of-line */
			mb_l = 1;
		}
		else	/* cc_dbcs */
		{
		    mb_l = MB_BYTE2LEN(c);
		    if (mb_l == 0)  /* at the NUL at end-of-line */
			mb_l = 1;
		    else if (mb_l > 1)
			mb_c = (c << 8) + ptr[1];
		}
		/* if a double-width char doesn't fit use a '>' */
		if (
# ifdef FEAT_RIGHTLEFT
			wp->w_p_rl ? (col <= 0) :
# endif
			(col >= W_WIDTH(wp) - 1)
			&& mb_char2cells(mb_c) == 2)
		{
		    c = '>';
		    mb_c = c;
		    mb_utf8 = FALSE;
		    mb_l = 1;
		    --ptr;
		    /* FIXME: Bad hack! however it works */
		    extra_attr = char_attr;
		    n_attr = 2;
		    multi_attr = hl_attr(HLF_AT);
		}
		else
		    ptr += mb_l - 1;

	    }
#endif
	    ++ptr;

	    if (extra_check)
	    {
#ifdef FEAT_SYN_HL
		if (has_syntax)
		{
		    v = ptr - line;
		    syntax_attr = get_syntax_attr((colnr_t)v - 1);

		    /* Need to get the line again, a multi-line regexp may
		     * have made it invalid. */
		    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = line + v;

		    if (!area_attr && !search_attr)
			char_attr = syntax_attr;
		}
#endif
#ifdef FEAT_LINEBREAK
		/*
		 * Found last space before word: check for line break
		 */
		if (wp->w_p_lbr && vim_isbreak(c) && !vim_isbreak(*ptr)
						      && !wp->w_p_list)
		{
		    n_extra = win_lbr_chartabsize(wp,
# ifdef FEAT_MBYTE
			    ptr - mb_l,
# else
			    ptr - 1,
# endif
			    (colnr_t)vcol, NULL) - 1;
		    c_extra = ' ';
		    if (vim_iswhite(c))
			c = ' ';
		}
#endif

		if (trailcol != MAXCOL && ptr > line + trailcol && c == ' ')
		{
		    c = lcs_trail;
		    if (!area_attr && !search_attr)
		    {
			n_attr = 1;
			extra_attr = hl_attr(HLF_AT);
			saved_attr2 = char_attr; /* save current attr */
		    }
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
		 * into "ScreenLines".
		 */
		if (c == TAB && (!wp->w_p_list || lcs_tab1))
		{
		    /* tab amount depends on current column */
		    n_extra = (int)wp->w_buffer->b_p_ts
				   - vcol % (int)wp->w_buffer->b_p_ts - 1;
		    if (wp->w_p_list)
		    {
			c = lcs_tab1;
			c_extra = lcs_tab2;
			if (!area_attr && !search_attr)
			{
			    n_attr = n_extra + 1;
			    extra_attr = hl_attr(HLF_AT);
			    saved_attr2 = char_attr; /* save current attr */
			}
		    }
		    else
		    {
			c_extra = ' ';
			c = ' ';
		    }
#ifdef FEAT_MBYTE
		    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
		}
		else if (c == NUL && wp->w_p_list && lcs_eol != NUL)
		{
		    p_extra = (char_u *)"";
		    n_extra = 1;
		    c_extra = NUL;
		    c = lcs_eol;
		    --ptr;	    /* put it back at the NUL */
		    char_attr = hl_attr(HLF_AT);
#ifdef FEAT_MBYTE
		    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
		}
		else if (c != NUL)
		{
		    p_extra = transchar(c);
		    n_extra = byte2cells(c) - 1;
		    c_extra = NUL;
		    c = *p_extra++;
#ifdef FEAT_MBYTE
		    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
		}
#ifdef FEAT_VIRTUALEDIT
		else if (VIsual_active && VIsual_mode == Ctrl_V
			 && ve_block && vcol < tocol && col < W_WIDTH(wp))
		{
		    c = ' ';
		    --ptr;	    /* put it back at the NUL */
		}
#endif
	    }
	}


	/*
	 * Handle the case where we are in column 0 but not on the first
	 * character of the line and the user wants us to show us a
	 * special character (via 'listchars' option "precedes:<char>".
	 */
	if (lcs_prec != NUL
		&& (wp->w_p_wrap ? wp->w_skipcol > 0 : wp->w_leftcol > 0)
		&& (
#ifdef FEAT_RIGHTLEFT
		    wp->w_p_rl ? col == W_WIDTH(wp) - 1 :
#endif
		    col == 0)
		&& c != NUL)
	{
	    c = lcs_prec;
	    extra_attr = hl_attr(HLF_AT); /* later copied to char_attr */
	    n_attr = 1;
	    saved_attr2 = char_attr; /* save current attr */
#ifdef FEAT_MBYTE
	    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
	}

	if (n_attr)
	    char_attr = extra_attr;

	/*
	 * At end of the text line.
	 */
	if (c == NUL)
	{
	    /* invert at least one char, used for Visual and empty line or
	     * highlight match at end of line. If it's beyond the last
	     * char on the screen, just overwrite that one (tricky!) */
	    if ((area_attr && vcol == fromcol)
#ifdef FEAT_SEARCH_EXTRA
		    /* highlight 'hlsearch' match in empty line */
		    || (search_attr && *line == NUL && wp->w_leftcol == 0)
#endif
	       )
	    {
#ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		{
		    if (col < 0)
		    {
			++off;
			++col;
		    }
		}
		else
#endif
		{
		    if (col >= W_WIDTH(wp))
		    {
			--off;
			--col;
		    }
		}
		ScreenLines[off] = ' ';
		ScreenAttrs[off] = char_attr;
#ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		    --col;
		else
#endif
		    ++col;
	    }

	    SCREEN_LINE(screen_row, W_WINCOL(wp), col, (int)W_WIDTH(wp),
								  wp->w_p_rl);
	    row++;

	    /*
	     * Update w_cline_height and w_cline_folded if the cursor line was
	     * updated (saves a call to plines() later).
	     */
	    if (wp == curwin && lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cline_row = startrow;
		curwin->w_cline_height = row - startrow;
#ifdef FEAT_FOLDING
		curwin->w_cline_folded = FALSE;
#endif
		curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
	    }

	    break;
	}

	/* line continues beyond line end */
	if (lcs_ext
		&& !wp->w_p_wrap
		&& (
#ifdef FEAT_RIGHTLEFT
		    wp->w_p_rl ? col == 0 :
#endif
		    col == W_WIDTH(wp) - 1)
		&& (*ptr != NUL
		    || (wp->w_p_list && lcs_eol != NUL)
		    || (n_extra && (c_extra != NUL || *p_extra != NUL))))
	{
	    c = lcs_ext;
	    char_attr = hl_attr(HLF_AT);
	}

	/*
	 * Store character to be displayed.
	 * Skip characters that are left of the screen for 'nowrap'.
	 */
	if (draw_state < WL_LINE || n_skip <= 0)
	{
	    /*
	     * Store the character.
	     */
	    ScreenLines[off] = c;
#ifdef FEAT_MBYTE
	    if (cc_utf8)
	    {
		if (mb_utf8)
		{
		    ScreenLinesUC[off] = mb_c;
		    ScreenLinesC1[off] = u8c_c1;
		    ScreenLinesC2[off] = u8c_c2;
		}
		else
		    ScreenLinesUC[off] = 0;
	    }
	    if (multi_attr)
	    {
		ScreenAttrs[off] = multi_attr;
		multi_attr = 0;
	    }
	    else
#endif
		ScreenAttrs[off] = char_attr;

#ifdef FEAT_MBYTE
	    if (has_mbyte && mb_char2cells(mb_c) > 1)
	    {
		/* Need to advance two screen columns. */
#ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		{
		    --off;
		    --col;
		}
		else
#endif
		{
		    ++off;
		    ++col;
		}
		if (cc_utf8)
		    /* UTF-8: Put a 0 in the second screen char. */
		    ScreenLines[off] = 0;
		else
		    /* DBCS: Put second byte in the second screen char. */
		    ScreenLines[off] = mb_c & 0xff;
		++vcol;
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
	    {
		--off;
		--col;
	    }
	    else
#endif
	    {
		++off;
		++col;
	    }
	}
	else
	    --n_skip;

	/* Only advance the "vcol" when after the 'number' column. */
	if (draw_state >= WL_SBR)
	    ++vcol;

	/* restore attributes after last 'listchars' or 'number' char */
	if (n_attr && --n_attr == 0)
	    char_attr = saved_attr2;

	/* When still displaying '$' of change command, stop at cursor */
	if (dollar_vcol != 0 && wp == curwin && vcol >= (long)wp->w_virtcol
#ifdef FEAT_VIRTUALEDIT
								+ wp->w_coladd
#endif
		)
	{
	    SCREEN_LINE(screen_row, W_WINCOL(wp), col, 0, wp->w_p_rl);
	    /* returned row is not used.  Pretend we have one line only. */
	    ++row;
	    break;
	}

	/*
	 * At end of screen line and there is more to come: Display the line
	 * so far.  If there is no more to display it is catched above.
	 */
	if (
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (col < 0) :
#endif
				    (col >= W_WIDTH(wp))
		&& (*ptr != NUL
			|| (wp->w_p_list && lcs_eol != NUL)
			|| (n_extra && (c_extra != NUL || *p_extra != NUL)))
		)
	{
	    SCREEN_LINE(screen_row, W_WINCOL(wp), col, (int)W_WIDTH(wp),
								  wp->w_p_rl);
	    ++row;
	    ++screen_row;

	    /* When not wrapping break here. */
	    if (!wp->w_p_wrap)
		break;

	    /* When the window is too narrow draw all "@" lines. */
	    if (draw_state != WL_LINE)
	    {
		win_draw_end(wp, '@', row);
#ifdef FEAT_VERTSPLIT
		draw_vsep_win(wp, row);
#endif
		row = endrow;
	    }

	    /* When line got too long for screen break here. */
	    if (row == endrow)
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
	     * Don't do this for double-width characters.
	     * Don't do this for a window not at the right screen border.
	     */
	    if (p_tf && screen_cur_row == screen_row - 1
#ifdef FEAT_GUI
		     && !gui.in_use
#endif
#ifdef FEAT_MBYTE
		     && !(has_mbyte
			 && (mb_off2cells(LineOffset[screen_row]) == 2
			     || mb_off2cells(LineOffset[screen_row - 1]
						    + (int)Columns - 2) == 2))
#endif
		     && W_WIDTH(wp) == Columns)
	    {
		if (screen_cur_col != W_WIDTH(wp))
		    screen_char(LineOffset[screen_row - 1]
						      + (unsigned)Columns - 1,
					  screen_row - 1, (int)(Columns - 1));
		screen_char(LineOffset[screen_row],
						screen_row - 1, (int)Columns);
		screen_start();		/* don't know where cursor is now */
	    }

	    col = 0;
	    off = current_ScreenLine - ScreenLines;
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
	    {
		col = W_WIDTH(wp) - 1;	/* col is not used if breaking! */
		off += W_WIDTH(wp) - 1;
	    }
#endif

	    /* reset the drawing state for the start of a wrapped line */
	    draw_state = WL_START;
	    saved_n_extra = n_extra;
	    saved_p_extra = p_extra;
	    saved_c_extra = c_extra;
	    saved_char_attr = char_attr;
	    n_extra = 0;
#ifdef FEAT_LINEBREAK
	    need_showbreak = TRUE;
#endif
	}

    }	/* for every character in the line */

#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    /* restore got_int, unless CTRL-C was hit while redrawing */
    if (!got_int)
	got_int = save_got_int;
#endif

    return (row);
}

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 */
    static int
char_needs_redraw(off_from, off_to, len)
    int		off_from;
    int		off_to;
    int		len;
{
    if (len > 0
	    && ((ScreenLines[off_from] != ScreenLines[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])

#ifdef FEAT_MBYTE
		|| (cc_dbcs
		    && MB_BYTE2LEN(ScreenLines[off_from]) > 1
		    && len > 1
		    && ScreenLines[off_from + 1] != ScreenLines[off_to + 1])
		|| (cc_utf8
		    && (ScreenLinesUC[off_from] != ScreenLinesUC[off_to]
			|| (ScreenLinesUC[off_from] != 0
			    && (ScreenLinesC1[off_from]
						      != ScreenLinesC1[off_to]
				|| ScreenLinesC2[off_from]
						  != ScreenLinesC2[off_to]))))
#endif
	       ))
	return TRUE;
    return FALSE;
}

/*
 * Move one "cooked" screen line to the screen, but only the characters that
 * have actually changed.  Handle insert/delete character.
 * "coloff" gives the first column on the screen for this line.
 * "endcol" gives the columns where valid characters are.
 * "clear_width" is non-zero if the rest of the line up to "clear_width"
 * columns needs to be cleared.
 * "rlflag" is TRUE in a rightleft window:
 *	When TRUE and clear_width, line is cleared in 0 -- endcol.
 *	When FALSE and clear_width, line is cleared in endcol -- Columns-1.
 */
    static void
screen_line(row, coloff, endcol, clear_width
#ifdef FEAT_RIGHTLEFT
				    , rlflag
#endif
						)
    int	    row;
    int	    coloff;
    int	    endcol;
    int	    clear_width;
#ifdef FEAT_RIGHTLEFT
    int	    rlflag;
#endif
{
    unsigned	    off_from;
    unsigned	    off_to;
    int		    col = 0;
#if defined(FEAT_GUI) || defined(UNIX) || defined(FEAT_VERTSPLIT)
    int		    hl;
#endif
    int		    force = FALSE;	/* force update rest of the line */
    int		    redraw_this		/* bool: does character need redraw? */
#ifdef FEAT_GUI
				= TRUE	/* For GUI when while-loop empty */
#endif
				;
    int		    redraw_next;	/* redraw_this for next character */
#ifdef FEAT_MBYTE
    int		    char_cells;		/* 1: normal char */
					/* 2: occupies two display cells */
# define CHAR_CELLS char_cells
#else
# define CHAR_CELLS 1
#endif
#ifdef UNIX
    int		    using_xterm = FALSE;

# ifdef FEAT_GUI
    if (!gui.in_use)
# endif
	using_xterm = vim_is_xterm(T_NAME);
#endif

    off_from = current_ScreenLine - ScreenLines;
    off_to = LineOffset[row] + coloff;

#ifdef FEAT_RIGHTLEFT
    if (rlflag)
    {
	/* Clear rest first, because it's left of the text. */
	if (clear_width)
	{
	    while (col <= endcol && ScreenLines[off_to] == ' '
						  && ScreenAttrs[off_to] == 0)
	    {
		++off_to;
		++col;
	    }
	    if (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);
	}
	col = endcol + 1;
	off_to = LineOffset[row] + col + coloff;
	off_from += col;
	endcol = Columns;
    }
#endif /* FEAT_RIGHTLEFT */

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);

    while (col < endcol)
    {
#ifdef FEAT_MBYTE
	if (has_mbyte && (col + 1 < endcol))
	{
	    if (cc_utf8)
		/* UTF-8: next cell has a 0 character if it's double width */
		char_cells = (ScreenLines[off_from + 1] == 0 ? 2 : 1);
	    else
		/* DBCS: bytelen == nr of cells */
		char_cells = MB_BYTE2LEN(ScreenLines[off_from]);
	}
	else
	    char_cells = 1;
#endif

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + CHAR_CELLS,
			      off_to + CHAR_CELLS, endcol - col - CHAR_CELLS);

#ifdef FEAT_GUI
	/* If the next character was bold, then redraw the current character to
	 * remove any pixels that might have spilt over into us.  This only
	 * happens in the GUI.
	 */
	if (redraw_next && gui.in_use)
	{
	    hl = ScreenAttrs[off_to + CHAR_CELLS];
	    if (hl > HL_ALL || (hl & HL_BOLD))
		redraw_this = TRUE;
	}
#endif

	if (redraw_this)
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
#ifdef FEAT_GUI
		    && !gui.in_use
#endif
		    && ScreenAttrs[off_to] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_to])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(row, col + coloff);
		out_str(T_CE);		/* clear rest of this screen line */
		screen_start();		/* don't know where cursor is now */
		force = TRUE;		/* force redraw of rest of the line */
		redraw_next = TRUE;	/* or else next char would miss out */

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		if (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		else
		    screen_attr = 0;	    /* highlighting has stopped */
	    }
#ifdef FEAT_MBYTE
	    if (cc_dbcs)
	    {
		/* Check if overwriting a double-byte with a single-byt or
		 * ther other way around requires another character to be
		 * redrawn.  For UTF-8 this isn't needed, because comparing
		 * ScreenLinesUC[] is sufficient. */
		if (char_cells == 1
			&& col + 1 < endcol
			&& MB_BYTE2LEN(ScreenLines[off_to]) > 1)
		{
		    /* Writing a single-cell character over a double-cell
		     * character: need to redraw the next cell. */
		    ScreenLines[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		else if (char_cells == 2
			&& col + 2 < endcol
			&& MB_BYTE2LEN(ScreenLines[off_to]) == 1
			&& MB_BYTE2LEN(ScreenLines[off_to + 1]) > 1)
		{
		    /* Writing the second half of a double-cell character over
		     * a double-cell character: need to redraw the second
		     * cell. */
		    ScreenLines[off_to + 2] = 0;
		    redraw_next = TRUE;
		}
	    }
#endif

	    ScreenLines[off_to] = ScreenLines[off_from];
#ifdef FEAT_MBYTE
	    if (cc_utf8)
	    {
		ScreenLinesUC[off_to] = ScreenLinesUC[off_from];
		ScreenLinesC1[off_to] = ScreenLinesC1[off_from];
		ScreenLinesC2[off_to] = ScreenLinesC2[off_from];
	    }
	    if (char_cells == 2)
		ScreenLines[off_to + 1] = ScreenLines[off_from + 1];
#endif

#if defined(FEAT_GUI) || defined(UNIX)
	    /* The bold trick makes a single row of pixels appear in the next
	     * character.  When a bold character is removed, the next
	     * character should be redrawn too.  This happens for our own GUI
	     * and for some xterms. */
	    if (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# if defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    using_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		if (hl > HL_ALL || (hl & HL_BOLD))
		    redraw_next = TRUE;
	    }
#endif
	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_MBYTE
	    if (cc_dbcs && char_cells == 2)
	    {
		/* just a hack: It makes two bytes of DBCS have same attr */
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_from];
		screen_char_n(off_to, 2, row, col + coloff);
	    }
	    else
#endif
		screen_char(off_to, row, col + coloff);
	}
	else if (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0)
	{
	    if (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    else if (screen_attr)
		screen_stop_highlight();
	}

	off_to += CHAR_CELLS;
	off_from += CHAR_CELLS;
	col += CHAR_CELLS;
    }

    if (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !rlflag
#endif
				   )
    {
#ifdef FEAT_GUI
	int startCol = col;
#endif

	/* blank out the rest of the line */
	while (col < clear_width && ScreenLines[off_to] == ' '
						  && ScreenAttrs[off_to] == 0)
	{
	    ++off_to;
	    ++col;
	}
	if (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    if (gui.in_use && (col > startCol || !redraw_this)
# ifdef FEAT_MBYTE
		    && !cc_dbcs
# endif
	       )
	    {
		hl = ScreenAttrs[off_to];
		if (hl > HL_ALL || (hl & HL_BOLD))
		    screen_char(off_to - 1, row, col + coloff - 1);
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
#ifdef FEAT_VERTSPLIT
	    off_to += clear_width - col;
	    col = clear_width;
#endif
	}

#ifdef FEAT_VERTSPLIT
	/* For a window that's left of another, draw the separator char. */
	if (col + coloff < Columns)
	{
	    int c;

	    c = fillchar_vsep(&hl);
	    if (ScreenLines[off_to] != c || ScreenAttrs[off_to] != hl)
	    {
		ScreenLines[off_to] = c;
		ScreenAttrs[off_to] = hl;
		screen_char(off_to, row, col + coloff);
	    }
	}
#endif
    }
}

#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * mark all status lines for redraw; used after first :cd
 */
    void
status_redraw_all()
{
    win_t	*wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_status_height)
	{
	    wp->w_redr_status = TRUE;
	    redraw_later(VALID);
	}
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    void
redraw_statuslines()
{
    win_t	*wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_redr_status)
	    win_redr_status(wp);
}
#endif

#if defined(FEAT_WILDMENU) && defined(FEAT_VERTSPLIT)
/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    static void
win_redraw_last_status(frp)
    frame_t	*frp;
{
    if (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = TRUE;
    else if (frp->fr_layout == FR_ROW)
    {
	for (frp = frp->fr_child; frp != NULL; frp = frp->fr_next)
	    win_redraw_last_status(frp);
    }
    else /* frp->fr_layout == FR_COL */
    {
	frp = frp->fr_child;
	while (frp->fr_next != NULL)
	    frp = frp->fr_next;
	win_redraw_last_status(frp);
    }
}
#endif

#ifdef FEAT_VERTSPLIT
/*
 * Draw the verticap separator right of window "wp" starting with line "row".
 */
    static void
draw_vsep_win(wp, row)
    win_t	*wp;
    int		row;
{
    int		hl;
    int		c;

    if (wp->w_vsep_width)
    {
	/* draw the vertical separator right of this window */
	c = fillchar_vsep(&hl);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_ENDCOL(wp), W_ENDCOL(wp) + 1,
		c, ' ', hl);
    }
}
#endif

#ifdef FEAT_WILDMENU
/*
 * Private gettail for win_redr_status_matches(): Find tail of file name path
 * but ignore trailing "/".
 */
static char_u *m_gettail __ARGS((char_u *s));
static int status_match_len __ARGS((char_u *s));

    static char_u *
m_gettail(s)
    char_u	*s;
{
    char_u	*p;
    char_u	*t = s;
    int		had_sep = FALSE;

    for (p = s; *p != NUL; )
    {
	if (vim_ispathsep(*p))
	    had_sep = TRUE;
	else if (had_sep)
	{
	    t = p;
	    had_sep = FALSE;
	}
#ifdef FEAT_MBYTE
	if (has_mbyte)
	    p += mb_ptr2len_check(p);
	else
#endif
	    ++p;
    }
    return t;
}

/*
 * Get the lenght of an item as it will be shown in that status line.
 */
    static int
status_match_len(s)
    char_u	*s;
{
    int	len = 0;

#ifdef FEAT_MENU
    int emenu = (expand_context == EXPAND_MENUS
				       || expand_context == EXPAND_MENUNAMES);

    /* Check for menu separators - replace with '|'. */
    if (emenu && menu_is_separator(s))
	return 1;
#endif

    while (*s != NUL)
    {
	/* Don't display backslashes used for escaping, they look ugly. */
	if (rem_backslash(s)
#ifdef FEAT_MENU
		|| (emenu && (s[0] == '\\' && s[1] != NUL))
#endif
		)
	    ++s;
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    len += ptr2cells(s);
	    s += mb_ptr2len_check(s);
	}
	else
#endif
	    len += ptr2cells(s++);

    }

    return len;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    void
win_redr_status_matches(num_matches, matches, match)
    int		num_matches;
    char_u	**matches;	/* list of matches */
    int		match;
{
#define L_MATCH(m) (fmatch ? m_gettail(matches[m]) : matches[m])
    int		fmatch = (expand_context == EXPAND_FILES);

    int		row;
    char_u	*buf;
    int		len;
    int		fillchar;
    int		attr;
    int		i;
    int		highlight = TRUE;
    char_u	*selstart = NULL;
    char_u	*selend = NULL;
    static int	first_match = 0;
    int		add_left = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    int		emenu;
#endif

    if (matches == NULL)	/* interrupted completion? */
	return;

    buf = alloc((unsigned)Columns + 1);
    if (buf == NULL)
	return;

    if (match == -1)	/* don't show match but original text */
    {
	match = 0;
	highlight = FALSE;
    }
    len = status_match_len(L_MATCH(match)) + 3;	/* count 1 for the ending ">" */
    if (match == 0)
	first_match = 0;
    else if (match < first_match)
    {
	/* jumping left, as far as we can go */
	first_match = match;
	add_left = TRUE;
    }
    else
    {
	/* check if match fits on the screen */
	for (i = first_match; i < match; ++i)
	    len += status_match_len(L_MATCH(i)) + 2;
	if (first_match > 0)
	    len += 2;
	/* jumping right, put match at the left */
	if ((long)len > Columns)
	{
	    first_match = match;
	    /* if showing the last match, we can add some on the left */
	    len = 2;
	    for (i = match; i < num_matches; ++i)
	    {
		len += status_match_len(L_MATCH(i)) + 2;
		if ((long)len >= Columns)
		    break;
	    }
	    if (i == num_matches)
		add_left = TRUE;
	}
    }
    if (add_left)
	while (first_match > 0)
	{
	    len += status_match_len(L_MATCH(first_match - 1)) + 2;
	    if ((long)len >= Columns)
		break;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, TRUE);

    if (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    else
    {
	STRCPY(buf, "< ");
	len = 2;
    }

    i = first_match;
    while ((long)(len + status_match_len(L_MATCH(i)) + 2) < Columns)
    {
	if (i == match)
	    selstart = buf + len;

	s = L_MATCH(i);
	/* Check for menu separators - replace with '|' */
#ifdef FEAT_MENU
	emenu = (expand_context == EXPAND_MENUS
				       || expand_context == EXPAND_MENUNAMES);
	if (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    len += STRLEN(buf + len);
	}
	else
#endif
	    for ( ; *s; ++s)
	{
	    /* Don't display backslashes used for escaping, they look ugly. */
	    if (rem_backslash(s)
#ifdef FEAT_MENU
		    || (emenu
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
		    )
		++s;
	    STRCPY(buf + len, transchar(*s));
	    len += STRLEN(buf + len);
	}
	if (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	if (++i == num_matches)
		break;
    }

    if (i != num_matches)
	*(buf + len++) = '>';

    *(buf + len) = NUL;

    row = cmdline_row - 1;
    if (row >= 0)
    {
	if (!wild_menu_showing)
	{
	    if (msg_scrolled && !wild_menu_showing)
	    {
		/* Put the wildmenu just above the command line.  If there is
		 * no room, scroll the screen one line up. */
		if (cmdline_row == Rows - 1)
		{
		    screen_del_lines(0, 0, 1, (int)Rows, TRUE);
		    ++msg_scrolled;
		}
		else
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    else
	    {
		/* create status line if needed */
		if (lastwin->w_status_height == 0)
		{
		    save_p_ls = p_ls;
		    p_ls = 2;
		    last_status(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, row, 0, attr);
	if (selstart != NULL && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, row, (int)(selstart - buf), hl_attr(HLF_WM));
	}

	screen_fill(row, row + 1, len, (int)Columns, fillchar, fillchar, attr);
    }

#ifdef FEAT_VERTSPLIT
    win_redraw_last_status(topframe);
#else
    lastwin->w_redr_status = TRUE;
#endif
    vim_free(buf);
}
#endif

#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    void
win_redr_status(wp)
    win_t	*wp;
{
    int		row;
    char_u	*p;
    int		len;
    int		fillchar;
    int		attr;
#ifdef FEAT_VERTSPLIT
    int		this_ru_col;
#else
# define this_ru_col ru_col
#endif

    wp->w_redr_status = FALSE;
    if (wp->w_status_height == 0)
    {
	/* no status line, can only be last window */
	redraw_cmdline = TRUE;
    }
    else if (!redrawing())
    {
	/* Don't redraw right now, do it later. */
	wp->w_redr_status = TRUE;
    }
#ifdef FEAT_STL_OPT
    else if (*p_stl)
    {
	/* redraw custom status line */
	win_redr_custom(wp, FALSE);
    }
#endif
    else
    {
	fillchar = fillchar_status(&attr, wp == curwin);

	if (buf_spname(wp->w_buffer) != NULL)
	    STRCPY(NameBuff, buf_spname(wp->w_buffer));
	else
	{
	    home_replace(wp->w_buffer, wp->w_buffer->b_fname, NameBuff,
							      MAXPATHL, TRUE);
	    trans_characters(NameBuff, MAXPATHL);
	}
	p = NameBuff;
	len = STRLEN(p);

	if (wp->w_buffer->b_help
		|| wp->w_preview
		|| bufIsChanged(wp->w_buffer)
		|| wp->w_buffer->b_p_ro)
	    *(p + len++) = ' ';
	if (wp->w_buffer->b_help)
	{
	    STRCPY(p + len, _("[help]"));
	    len += STRLEN(p + len);
	}
	if (wp->w_preview)
	{
	    STRCPY(p + len, _("[Preview]"));
	    len += STRLEN(p + len);
	}
	if (bufIsChanged(wp->w_buffer))
	{
	    STRCPY(p + len, "[+]");
	    len += 3;
	}
	if (wp->w_buffer->b_p_ro)
	{
	    STRCPY(p + len, "[RO]");
	    len += 4;
	}

#ifdef FEAT_VERTSPLIT
	this_ru_col = ru_col - (Columns - W_WIDTH(wp));
	if (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		/* No room for file name! */
	    len = 1;
	}
	else
#endif
	    if (len > this_ru_col - 1)
	    {
		p += len - (this_ru_col - 1);
		*p = '<';
		len = this_ru_col - 1;
	    }

	row = W_WINROW(wp) + wp->w_height;
	screen_puts(p, row, W_WINCOL(wp), attr);
	screen_fill(row, row + 1, len + W_WINCOL(wp),
			this_ru_col + W_WINCOL(wp), fillchar, fillchar, attr);

#ifdef FEAT_CMDL_INFO
	win_redr_ruler(wp, TRUE);
#endif
    }

#ifdef FEAT_VERTSPLIT
    /*
     * May need to draw the character below the vertical separator.
     */
    if (wp->w_vsep_width != 0)
    {
	int		connected = FALSE;
	frame_t	*fr;

	/* Find out if the status line is connected to the status line of
	 * the window right of it. */
	fr = wp->w_frame;
	while (fr->fr_parent != NULL)
	{
	    if (fr->fr_parent->fr_layout == FR_COL)
	    {
		if (fr->fr_next != NULL)
		    break;
	    }
	    else
	    {
		if (fr->fr_next != NULL)
		{
		    connected = TRUE;
		    break;
		}
	    }
	    fr = fr->fr_parent;
	}

	if (connected)
	    fillchar = fillchar_status(&attr, wp == curwin);
	else
	    fillchar = fillchar_vsep(&attr);
	screen_putchar(fillchar, W_WINROW(wp) + wp->w_height, W_ENDCOL(wp),
									attr);
    }
#endif
}
#endif

#if defined(FEAT_STL_OPT) || defined(PROTO)
/*
 * Redraw the status line or ruler of window wp.
 */
    static void
win_redr_custom(wp, Ruler)
    win_t	*wp;
    int		Ruler;
{
    int		attr;
    int		curattr;
    int		row;
    int		col = 0;
    int		maxlen;
    int		n;
    int		len;
    int		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*p;
    char_u	c;
    struct	stl_hlrec hl[STL_MAX_ITEM];

    /* setup environment for the task at hand */
    row = W_WINROW(wp) + wp->w_height;
    fillchar = fillchar_status(&attr, wp == curwin);
    maxlen = W_WIDTH(wp);
    p = p_stl;
    if (Ruler)
    {
	p = p_ruf;
	/* advance past any leading group spec - implicit in ru_col */
	if (*p == '%')
	{
	    if (*++p == '-')
		p++;
	    if (atoi((char *) p))
		while (isdigit(*p))
		    p++;
	    if (*p++ != '(')
		p = p_ruf;
	}
#ifdef FEAT_VERTSPLIT
	col = ru_col - (Columns - W_WIDTH(wp));
	if (col < 0)
	    col = 0;
#else
	col = ru_col;
#endif
	maxlen = W_WIDTH(wp) - col;
	if (!wp->w_status_height)
	{
	    row = Rows - 1;
	    --maxlen;	/* writing in last column may cause scrolling */
	    fillchar = ' ';
	    attr = 0;
	}
    }
    if (maxlen >= sizeof(buf))
	maxlen = sizeof(buf) - 1;
    if (maxlen <= 0)
	return;
#ifdef FEAT_VERTSPLIT
    col += W_WINCOL(wp);
#endif

    len = build_stl_str_hl(wp, buf, p, fillchar, maxlen, hl);

    for (p = buf + len; p < buf + maxlen; p++)
	*p = fillchar;
    buf[maxlen] = 0;

    curattr = attr;
    p = buf;
    for (n = 0; hl[n].start != NULL; n++)
    {
	c = hl[n].start[0];
	hl[n].start[0] = 0;
	screen_puts(p, row, col, curattr);

	hl[n].start[0] = c;
	col += hl[n].start - p;
	p = hl[n].start;

	if (hl[n].userhl == 0)
	    curattr = attr;
	else if (wp == curwin || !wp->w_status_height)
	    curattr = highlight_user[hl[n].userhl - 1];
	else
	    curattr = highlight_stlnc[hl[n].userhl - 1];
    }
    screen_puts(p, row, col, curattr);
}

# if defined(FEAT_TITLE) || defined(PROTO)
    int
build_stl_str(wp, out, fmt, fillchar, maxlen)
    win_t	*wp;
    char_u	*out;
    char_u	*fmt;
    int		fillchar;
    int		maxlen;
{
    return build_stl_str_hl(wp, out, fmt, fillchar, maxlen, NULL);
}
# endif

/*
 * Build a string from the status line items in fmt, return length of string.
 *
 * Items are drawn interspersed with the text that surrounds it
 * Specials: %-<wid>(xxx%) => group, %= => middle marker, %< => truncation
 * Item: %-<minwid>.<maxwid><itemch> All but <itemch> are optional
 *
 * if maxlen is not zero, the string will be filled at any middle marker
 * or truncated if too long, fillchar is used for all whitespace
 */
    static int
build_stl_str_hl(wp, out, fmt, fillchar, maxlen, hl)
    win_t	*wp;
    char_u	*out;
    char_u	*fmt;
    int		fillchar;
    int		maxlen;
    struct stl_hlrec *hl;
{
    char_u	*p;
    char_u	*s;
    char_u	*t;
    char_u	*linecont;
#ifdef FEAT_EVAL
    win_t	*o_curwin;
    buf_t	*o_curbuf;
    void	*save_funccalp;
#endif
    int		empty_line;
    colnr_t	virtcol;
    long	l;
    long	n;
    int		prevchar_isflag;
    int		prevchar_isitem;
    int		itemisflag;
    char_u	*str;
    long	num;
    int		itemcnt;
    int		curitem;
    int		groupitem[STL_MAX_ITEM];
    int		groupdepth;
    struct stl_item
    {
	char_u		*start;
	int		minwid;
	int		maxwid;
	enum
	{
	    Normal,
	    Empty,
	    Group,
	    Middle,
	    Highlight,
	    Trunc
	}		type;
    }		item[STL_MAX_ITEM];
    int		minwid;
    int		maxwid;
    int		zeropad;
    char_u	base;
    char_u	opt;
    char_u	tmp[70];

    if (!fillchar)
	fillchar = ' ';
    /*
     * Get line & check if empty (cursorpos will show "0-1").
     * If inversion is possible we use it. Else '=' characters are used.
     */
    linecont = ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE);
    empty_line = (*linecont == NUL);

    groupdepth = 0;
    p = out;
    curitem = 0;
    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    for (s = fmt; *s;)
    {
	if (*s && *s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;
	while (*s && *s != '%')
	    *p++ = *s++;
	if (!*s)
	    break;
	s++;
	if (*s == '%')
	{
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    continue;
	}
	if (*s == STL_MIDDLEMARK)
	{
	    s++;
	    if (groupdepth > 0)
		continue;
	    item[curitem].type = Middle;
	    item[curitem++].start = p;
	    continue;
	}
	if (*s == STL_TRUNCMARK)
	{
	    s++;
	    item[curitem].type = Trunc;
	    item[curitem++].start = p;
	    continue;
	}
	if (*s == ')')
	{
	    s++;
	    if (groupdepth < 1)
		continue;
	    groupdepth--;
	    l = p - item[groupitem[groupdepth]].start;
	    if (curitem > groupitem[groupdepth] + 1
		    && item[groupitem[groupdepth]].minwid == 0)
	    {			    /* remove group if all items are empty */
		for (n = groupitem[groupdepth] + 1; n < curitem; n++)
		    if (item[n].type == Normal)
			break;
		if (n == curitem)
		    p = item[groupitem[groupdepth]].start;
	    }
	    if (item[groupitem[groupdepth]].maxwid < l)
	    {					    /* truncate */
		n = item[groupitem[groupdepth]].maxwid;
		mch_memmove(item[groupitem[groupdepth]].start,
			    item[groupitem[groupdepth]].start + l - n,
			    (size_t)n);
		t = item[groupitem[groupdepth]].start;
		*t = '<';
		l -= n;
		p -= l;
		for (n = groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    item[n].start -= l;
		    if (item[n].start < t)
			item[n].start = t;
		}
	    }
	    else if (abs(item[groupitem[groupdepth]].minwid) > l)
	    {					    /* fill */
		n = item[groupitem[groupdepth]].minwid;
		if (n < 0)
		{
		    n = 0 - n;
		    while (l++ < n)
			*p++ = fillchar;
		}
		else
		{
		    mch_memmove(item[groupitem[groupdepth]].start + n - l,
			        item[groupitem[groupdepth]].start,
			        (size_t)l);
		    l = n - l;
		    p += l;
		    for (n = groupitem[groupdepth] + 1; n < curitem; n++)
			item[n].start += l;
		    for (t = item[groupitem[groupdepth]].start; l > 0; l--)
			*t++ = fillchar;
		}
	    }
	    continue;
	}
	minwid = 0;
	maxwid = 50;
	zeropad = FALSE;
	l = 1;
	if (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	if (*s == '-')
	{
	    s++;
	    l = -1;
	}
	while (*s && isdigit(*s))
	{
	    minwid *= 10;
	    minwid += *s - '0';
	    s++;
	}
	if (*s == STL_HIGHLIGHT)
	{
	    item[curitem].type = Highlight;
	    item[curitem].start = p;
	    item[curitem].minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    continue;
	}
	if (*s == '.')
	{
	    s++;
	    if (isdigit(*s))
		maxwid = 0;
	    while (*s && isdigit(*s))
	    {
		maxwid *= 10;
		maxwid += *s - '0';
		s++;
	    }
	}
	minwid = (minwid > 50 ? 50 : minwid) * l;
	if (*s == '(')
	{
	    groupitem[groupdepth++] = curitem;
	    item[curitem].type = Group;
	    item[curitem].start = p;
	    item[curitem].minwid = minwid;
	    item[curitem].maxwid = maxwid;
	    s++;
	    curitem++;
	    continue;
	}
	if (vim_strchr(STL_ALL, *s) == NULL)
	{
	    s++;
	    continue;
	}
	opt = *s++;

	/* OK - now for the real work */
	base = 'D';
	itemisflag = FALSE;
	num = -1;
	str = NULL;
	switch (opt)
	{
	case STL_FILEPATH:
	case STL_FULLPATH:
	case STL_FILENAME:
	    if (buf_spname(wp->w_buffer) != NULL)
		STRCPY(NameBuff, buf_spname(wp->w_buffer));
	    else
	    {
		t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					: wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
		trans_characters(NameBuff, MAXPATHL);
	    }
	    if (opt != STL_FILENAME)
		str = NameBuff;
	    else
		str = gettail(NameBuff);
	    break;

	case STL_VIM_EXPR: /* '{' */
	    itemisflag = TRUE;
	    t = p;
	    while (*s != '}')
		*p++ = *s++;
	    s++;
	    *p = 0;
	    p = t;

#ifdef FEAT_EVAL
	    sprintf((char *)tmp, "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"actual_curbuf", tmp);

	    o_curbuf = curbuf;
	    o_curwin = curwin;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    /* Don't want to use local function variables here. */
	    save_funccalp = save_funccal();

	    str = eval_to_string(p, &t);
	    if (str != NULL && *str != 0)
	    {
		t = str;
		if (*t == '-')
		    t++;
		while (*t && isdigit(*t))
		    t++;
		if (*t == 0)
		{
		    num = atoi((char *) str);
		    vim_free(str);
		    str = NULL;
		    itemisflag = FALSE;
		}
	    }
	    restore_funccal(save_funccalp);
	    curwin = o_curwin;
	    curbuf = o_curbuf;
	    do_unlet((char_u *)"g:actual_curbuf");
#endif
	    break;

	case STL_LINE:
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    break;

	case STL_NUMLINES:
	    num = wp->w_buffer->b_ml.ml_line_count;
	    break;

	case STL_COLUMN:
	    num = !(State & INSERT) && empty_line
		  ? 0 : (int)wp->w_cursor.col + 1;
	    break;

	case STL_VIRTCOL:
	case STL_VIRTCOL_ALT:
	    /* In list mode virtcol needs to be recomputed */
	    virtcol = wp->w_virtcol;
	    if (wp->w_p_list && lcs_tab1 == NUL)
	    {
		wp->w_p_list = FALSE;
		getvcol(wp, &wp->w_cursor, NULL, &virtcol, NULL);
		wp->w_p_list = TRUE;
	    }
	    if (opt == STL_VIRTCOL_ALT
		    && virtcol == (colnr_t)(!(State & INSERT) && empty_line
			    ? 0 : (int)wp->w_cursor.col))
	    {
		break;
	    }
	    num = (long)virtcol + 1;
	    break;

	case STL_PERCENTAGE:
	    num = (int)(((long)wp->w_cursor.lnum * 100L) /
			(long)wp->w_buffer->b_ml.ml_line_count);
	    break;

	case STL_ALTPERCENT:
	    str = tmp;
	    get_rel_pos(wp, str);
	    break;

	case STL_ARGLISTSTAT:
	    tmp[0] = 0;
	    if (append_arg_number(wp, tmp, FALSE, (int)sizeof(tmp)))
		str = tmp;
	    break;

	case STL_BUFNO:
	    num = wp->w_buffer->b_fnum;
	    break;

	case STL_OFFSET_X:
	    base= 'X';
	case STL_OFFSET:
#ifdef FEAT_BYTEOFF
	    l = ml_find_line_or_offset(wp->w_buffer, wp->w_cursor.lnum, NULL);
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0 ?
		  0L : l + 1 + (!(State & INSERT) && empty_line ?
			        0 : (int)wp->w_cursor.col);
#endif
	    break;

	case STL_BYTEVAL_X:
	    base= 'X';
	case STL_BYTEVAL:
	    if ((State & INSERT) || empty_line)
		num = 0;
	    else
	    {
		num = linecont[wp->w_cursor.col];
#ifdef FEAT_MBYTE
		if (cc_dbcs && MB_BYTE2LEN((int)num) > 1)
		    num = (num << 8) + linecont[wp->w_cursor.col + 1];
		else if (cc_utf8)
		    num = utf_ptr2char(linecont + wp->w_cursor.col);
#endif
	    }
	    if (num == NL)
		num = 0;
	    else if (num == CR && get_fileformat(wp->w_buffer) == EOL_MAC)
		num = NL;
	    break;

	case STL_ROFLAG:
	case STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    if (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : "[RO]");
	    break;

	case STL_HELPFLAG:
	case STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    if (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[help]"));
	    break;

#ifdef FEAT_AUTOCMD
	case STL_FILETYPE:
	    if (*wp->w_buffer->b_p_ft != NUL)
	    {
		sprintf((char *)tmp, "[%s]", wp->w_buffer->b_p_ft);
		str = tmp;
	    }
	    break;

	case STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    if (*wp->w_buffer->b_p_ft != NUL)
	    {
		sprintf((char *)tmp, ",%s", wp->w_buffer->b_p_ft);
		for (t = tmp; *t != 0; t++)
                    *t = TO_UPPER(*t);
		str = tmp;
	    }
	    break;
#endif

	case STL_PREVIEWFLAG:
	case STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    if (wp->w_preview)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    break;

	case STL_MODIFIED:
	case STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    if (bufIsChanged(wp->w_buffer))
		str = (char_u *)((opt == STL_MODIFIED_ALT) ? ",+" : "[+]");
	    break;
	}

	item[curitem].start = p;
	item[curitem].type = Normal;
	if (str != NULL && *str)
	{
	    t = str;
	    if (itemisflag)
	    {
		if ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = STRLEN(t);
	    if (l > 0)
		prevchar_isitem = TRUE;
	    if (l > maxwid)
	    {
		t += (l - maxwid + 1);
		*p++ = '<';
	    }
	    if (minwid > 0)
	    {
		for (; l < minwid; l++)
		    *p++ = fillchar;
		minwid = 0;
	    }
	    else
		minwid *= -1;
	    while (*t)
	    {
		*p++ = *t++;
		if (p[-1] == ' ')
		    p[-1] = fillchar;
	    }
	    for (; l < minwid; l++)
		*p++ = fillchar;
	}
	else if (num >= 0)
	{
	    int nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u nstr[20];

	    prevchar_isitem = TRUE;
	    t = nstr;
	    if (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    if (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (nbase == 8 ? 'o' : 'd');
	    *t = 0;

	    for (n = num, l = 1; n >= nbase; n /= nbase)
		l++;
	    if (opt == STL_VIRTCOL_ALT)
		l++;
	    if (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		while (l-- > maxwid)
		    num /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = 0;
		sprintf((char *) p, (char *) nstr, 0, num, n);
	    }
	    else
		sprintf((char *) p, (char *) nstr, minwid, num);
	    p += STRLEN(p);
	}
	else
	    item[curitem].type = Empty;

	if (opt == STL_VIM_EXPR)
	    vim_free(str);

	if (num >= 0 || (!itemisflag && str && *str))
	    prevchar_isflag = FALSE;	    /* Item not NULL, but not a flag */
	curitem++;
    }
    *p = 0;
    itemcnt = curitem;
    num = STRLEN(out);

    if (maxlen && num > maxlen)
    {					    /* Apply STL_TRUNC */
	for (l = 0; l < itemcnt; l++)
	    if (item[l].type == Trunc)
		break;
	if (itemcnt == 0)
	    s = out;
	else
	{
	    l = l == itemcnt ? 0 : l;
	    s = item[l].start;
	}
	if ((int) (s - out) > maxlen)
	{   /* Truncation mark is beyond max length */
	    s = out + maxlen - 1;
	    for (l = 0; l < itemcnt; l++)
		if (item[l].start > s)
		    break;
	    *s++ = '>';
	    *s = 0;
	    itemcnt = l;
	}
	else
	{
	    int		shift = num - maxlen;

	    p = s + shift;
	    mch_memmove(s, p, STRLEN(p) + 1);
	    *s = '<';
	    for (; l < itemcnt; l++)
	    {
		if (item[l].start - shift >= out)
		    item[l].start -= shift;
		else
		    item[l].start = out;
	    }
	}
	num = maxlen;
    }
    else if (num < maxlen)
    {					    /* Apply STL_MIDDLE if any */
	for (l = 0; l < itemcnt; l++)
	    if (item[l].type == Middle)
		break;
	if (l < itemcnt)
	{
	    p = item[l].start + maxlen - num;
	    mch_memmove(p, item[l].start, STRLEN(item[l].start) + 1);
	    for (s = item[l].start; s < p; s++)
		*s = fillchar;
	    for (l++; l < itemcnt; l++)
		item[l].start += maxlen - num;
	    num = maxlen;
	}
    }

    if (hl != NULL)
    {
	for (l = 0; l < itemcnt; l++)
	{
	    if (item[l].type == Highlight)
	    {
		hl->start = item[l].start;
		hl->userhl = item[l].minwid;
		hl++;
	    }
	}
	hl->start = NULL;
	hl->userhl = 0;
    }

    return (int)num;
}
#endif /* FEAT_STL_OPT */

/*
 * Output a single character directly to the screen and update ScreenLines.
 * Not for multi-byte chars!
 */
    void
screen_putchar(c, row, col, attr)
    int	    c;
    int	    row, col;
    int	    attr;
{
    char_u	buf[2];

    {
	buf[0] = c;
	buf[1] = NUL;
	screen_puts(buf, row, col, attr);
    }
}

/*
 * Get a single character directly from ScreenLines.
 * Also return its attribute in *attrp;
 * For multi-byte chars only the first byte is obtained!
 */
    int
screen_getchar(row, col, attrp)
    int	    row, col;
    int	    *attrp;
{
    unsigned off;

    /* safety check */
    if (ScreenLines != NULL && row < screen_Rows && col < screen_Columns)
    {
	off = LineOffset[row] + col;
	*attrp = ScreenAttrs[off];
	return ScreenLines[off];
    }
    return 0;
}

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    void
screen_puts(text, row, col, attr)
    char_u	*text;
    int		row;
    int		col;
    int		attr;
{
    unsigned	off;
#ifdef FEAT_MBYTE
    int		mbyte_blen = 0;
    int		mbyte_cells = 1;
    int		u8c = 0;
    int		u8c_c1 = 0;
    int		u8c_c2 = 0;
#endif
#ifdef UNIX
    int		using_xterm = FALSE;

# ifdef FEAT_GUI
    if (!gui.in_use)
# endif
	using_xterm = vim_is_xterm(T_NAME);
#endif

    if (ScreenLines != NULL && row < screen_Rows)	/* safety check */
    {
	off = LineOffset[row] + col;
	while (*text && col < screen_Columns)
	{
#ifdef FEAT_MBYTE
	    /* check if this is the first byte of a multibyte */
	    if (has_mbyte)
	    {
		mbyte_blen = mb_ptr2len_check(text);
		if (cc_dbcs)
		    mbyte_cells = mbyte_blen;
		else	/* cc_utf8 */
		{
		    u8c = utfc_ptr2char(text, &u8c_c1, &u8c_c2);
		    mbyte_cells = utf_char2cells(u8c);
		}
	    }
#endif

	    if (ScreenLines[off] != *text
#ifdef FEAT_MBYTE
		    || (mbyte_cells == 2
			&& ScreenLines[off + 1] != (cc_dbcs ? text[1] : 0))
		    || (cc_utf8
			&& mbyte_blen > 1
			&& (ScreenLinesUC[off] != u8c
			    || ScreenLinesC1[off] != u8c_c1
			    || ScreenLinesC2[off] != u8c_c2))
#endif
		    || ScreenAttrs[off] != attr
		    || exmode_active
		    )
	    {
#if defined(FEAT_GUI) || defined(UNIX)
		/* The bold trick makes a single row of pixels appear in the
		 * next character.  When a bold character is removed, the next
		 * character should be redrawn too.  This happens for our own
		 * GUI and for some xterms.
		 * Don't do this for the last drawn character, because the
		 * next character may not be redrawn. */
		if (
# ifdef FEAT_GUI
			gui.in_use
# endif
# if defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			using_xterm
# endif
		   )
		{
		    int		n;

		    n = ScreenAttrs[off];
		    if (col + 1
# ifdef FEAT_MBYTE
				+ mbyte_cells - 1
# endif
				< screen_Columns
			    && (n > HL_ALL || (n & HL_BOLD))
			    && text[1] != NUL)
			ScreenLines[off + 1
# ifdef FEAT_MBYTE
			    + mbyte_cells - 1
# endif
			    ] = 0;
		}
#endif
		ScreenLines[off] = *text;
		ScreenAttrs[off] = attr;
#ifdef FEAT_MBYTE
		if (cc_utf8)
		{
		    if (*text < 0x80 && u8c_c1 == 0 && u8c_c2 == 0)
			ScreenLinesUC[off] = 0;
		    else
		    {
			ScreenLinesUC[off] = u8c;
			ScreenLinesC1[off] = u8c_c1;
			ScreenLinesC2[off] = u8c_c2;
		    }
		    if (mbyte_cells == 2)
		    {
			ScreenLines[off + 1] = 0;
			ScreenAttrs[off + 1] = attr;
		    }
		    screen_char(off, row, col);
		}
		else if (mbyte_cells == 2)
		{
		    ScreenLines[off + 1] = text[1];
		    ScreenAttrs[off + 1] = attr;
		    screen_char_n(off, 2, row, col);
		}
		else
#endif
		    screen_char(off, row, col);
	    }
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		off += mbyte_cells;
		col += mbyte_cells;
		text += mbyte_blen;
	    }
	    else
#endif
	    {
		++off;
		++col;
		++text;
	    }
	}
    }
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Prepare for 'searchhl' highlighting.
 */
    static void
start_search_hl()
{
    if (p_hls && !no_hlsearch)
    {
	search_hl_rm.regprog = last_pat_prog();
	search_hl_attr = hl_attr(HLF_L);
	search_hl_ic = reg_ic;
    }
}

/*
 * Clean up for 'searchhl' highlighting.
 */
    static void
end_search_hl()
{
    if (search_hl_rm.regprog != NULL)
    {
	vim_free(search_hl_rm.regprog);
	search_hl_rm.regprog = NULL;
    }
}

static char_u *search_hl_getline __ARGS((linenr_t lnum));

    static char_u *
search_hl_getline(lnum)
    linenr_t	lnum;
{
    /* when looking behind for a match/no-match we can't go before line 1 */
    if (search_hl_lnum + lnum < 1)
	return NULL;
    return ml_get_buf(search_hl_buf, search_hl_lnum + lnum, FALSE);
}

/*
 * Search for a next 'searchl' match.
 * Uses search_hl_buf.
 * Sets search_hl_lnum and search_hl_rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * search_hl_lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    static void
next_search_hl(lnum, mincol)
    linenr_t	lnum;
    colnr_t	mincol;		/* minimal column for a match */
{
    linenr_t	l;
    colnr_t	matchcol;
    long	nmatched;
    buf_t	*curbuf_save;

    if (search_hl_lnum != 0)
    {
	/* Check for three situations:
	 * 1. If the "lnum" is below a previous match, start a new search.
	 * 2. If the previous match includes "mincol", use it.
	 * 3. Continue after the previous match.
	 */
	l = search_hl_lnum + search_hl_rm.endpos[0].lnum;
	if (lnum > l)
	    search_hl_lnum = 0;
	else if (lnum < l || search_hl_rm.endpos[0].col > mincol)
	    return;
    }

    /* use the right buffer for multi-line regexp and 'iskeyword' */
    curbuf_save = curbuf;
    curbuf = search_hl_buf;
    reg_ic = search_hl_ic;

    /*
     * Repeat searching for a match until one is found that includes "mincol"
     * or none is found in this line.
     */
    for (;;)
    {
	/* Three situations:
	 * 1. No useful previous match: search from start of line.
	 * 2. Not Vi compatible or empty match: continue at next character.
	 *    Break the loop if this is beyond the end of the line.
	 * 3. Vi compatible searching: continue at end of previous match.
	 */
	if (search_hl_lnum == 0)
	    matchcol = 0;
	else if (vim_strchr(p_cpo, CPO_SEARCH) == NULL
		|| (search_hl_rm.endpos[0].lnum == 0
		    && search_hl_rm.endpos[0].col
					     == search_hl_rm.startpos[0].col))
	{
	    matchcol = search_hl_rm.startpos[0].col + 1;
	    if (ml_get_buf(search_hl_buf, lnum, FALSE)[matchcol - 1] == NUL)
	    {
		search_hl_lnum = 0;
		break;
	    }
	}
	else
	    matchcol = search_hl_rm.endpos[0].col;

	search_hl_lnum = lnum;
	nmatched = vim_regexec_multi(&search_hl_rm, search_hl_getline,
				 matchcol, curbuf->b_ml.ml_line_count - lnum);
	if (nmatched == 0)
	{
	    search_hl_lnum = 0;		/* no match found */
	    break;
	}
	if (search_hl_rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| search_hl_rm.endpos[0].col > mincol)
	    break;			/* useful match found */
    }
    curbuf = curbuf_save;
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
    attrentry_t *aep = NULL;

    screen_attr = attr;
    if (full_screen
#ifdef WIN32
		    && termcap_active
#endif
				       )
    {
#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    char	buf[20];

	    sprintf(buf, IF_EB("\033|%dh", ESC_STR "|%dh"), attr);		/* internal GUI code */
	    OUT_STR(buf);
	}
	else
#endif
	{
	    if (attr > HL_ALL)				/* special HL attr. */
	    {
		if (t_colors > 1)
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
		if (t_colors > 1)
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
#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    char	buf[20];

	    sprintf(buf, IF_EB("\033|%dH", ESC_STR "|%dH"), screen_attr);	/* internal GUI code */
	    OUT_STR(buf);
	}
	else
#endif
	{
	    if (screen_attr > HL_ALL)			/* special HL attr. */
	    {
		attrentry_t *aep;

		if (t_colors > 1)
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
	    if (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

	    if (t_colors > 1)
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
 * The machine specific code may override this again.
 */
    void
reset_cterm_colors()
{
    if (t_colors > 1)
    {
	/* set Normal cterm colors */
	if (cterm_normal_fg_color || cterm_normal_bg_color)
	    out_str(T_OP);
	if (cterm_normal_fg_bold)
	    out_str(T_ME);
    }
}

/*
 * put character ScreenLines["off"] on the screen at position 'row' and 'col'
 */
    static void
screen_char(off, row, col)
    unsigned	off;
    int		row;
    int		col;
{
    int		attr;

    /*
     * Outputting the last character on the screen may scrollup the screen.
     * Don't to it!  At the same time check for illegal values, just in case.
     */
    if (row >= screen_Rows
	    || col >= screen_Columns
	    || (row == screen_Rows - 1 && col == screen_Columns - 1))
	return;

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
    attr = ScreenAttrs[off];
    if (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

    if (screen_attr != attr)
	screen_start_highlight(attr);

#ifdef FEAT_MBYTE
    if (cc_utf8 && ScreenLinesUC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	/* Convert UTF-8 character to bytes and write it. */
	buf[utfc_char2bytes(off, buf)] = NUL;
	out_str(buf);
	if (utf_char2cells(ScreenLinesUC[off]) > 1)
	    ++screen_cur_col;
    }
    else
#endif
	out_char(ScreenLines[off]);
    screen_cur_col++;
}

#ifdef FEAT_MBYTE

/*
 * Used for cc_dbcs only: Put "n" characters of ScreenLines["off"] on the
 * screen at position 'row' and 'col'.
 * The attributes of the first character is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    static void
screen_char_n(off, n, row, col)
    unsigned	off;
    int		n;
    int		row;
    int		col;
{
    /*
     * Outputting the last character on the screen may scrollup the screen.
     * Don't to it!  At the same time check for illegal values, just in case.
     */
    if (off + n >= (unsigned)(screen_Rows * screen_Columns))
	return;

    /* Output the first character normally, then write the rest directly. */
    screen_char(off, row, col);

    screen_cur_col += n - 1;
    while (--n > 0)
	out_char(ScreenLines[++off]);
}
#endif

/*
 * Fill the screen from 'start_row' to 'end_row', from 'start_col' to 'end_col'
 * with character 'c1' in first column followed by 'c2' in the other columns.
 * Use attributes 'attr'.
 * Cannot handle multi-byte characters, c1 and c2 must be < 0x80!
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
    int		    off;
    int		    did_delete;
    int		    c;
    int		    norm_term;

    if (end_row > screen_Rows)		/* safety check */
	end_row = screen_Rows;
    if (end_col > screen_Columns)	/* safety check */
	end_col = screen_Columns;
    if (ScreenLines == NULL
	    || start_row >= end_row
	    || start_col >= end_col)	/* nothing to do */
	return;

    /* it's a "normal" terminal when not in a GUI or cterm */
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
			    t_colors <= 1);
    for (row = start_row; row < end_row; ++row)
    {
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	if (c2 == ' '
		&& end_col == Columns
		&& *T_CE != NUL
		&& (attr == 0
		    || (norm_term
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0))))
	{
	    /*
	     * check if we really need to clear something
	     */
	    col = start_col;
	    if (c1 != ' ')			/* don't clear first char */
		++col;

	    off = LineOffset[row] + col;

	    /* skip blanks (used often, keep it fast!) */
	    while (col < end_col && ScreenLines[off] == ' '
						     && ScreenAttrs[off] == 0)
	    {
		++col;
		++off;
	    }
	    if (col < end_col)		/* something to be cleared */
	    {
		screen_stop_highlight();
		term_windgoto(row, col);/* clear rest of this screen line */
		out_str(T_CE);
		screen_start();		/* don't know where cursor is now */
		col = end_col - col;
		while (col--)		/* clear chars in ScreenLines */
		{
		    ScreenLines[off] = ' ';
#ifdef FEAT_MBYTE
		    if (cc_utf8)
			ScreenLinesUC[off] = 0;
#endif
		    ScreenAttrs[off] = 0;
		    ++off;
		}
	    }
	    did_delete = TRUE;		/* the chars are cleared now */
	}

	off = LineOffset[row] + start_col;
	c = c1;
	for (col = start_col; col < end_col; ++col)
	{
	    if (ScreenLines[off] != c || ScreenAttrs[off] != attr)
	    {
		ScreenLines[off] = c;
#ifdef FEAT_MBYTE
		if (cc_utf8)
		    ScreenLinesUC[off] = 0;
#endif
		ScreenAttrs[off] = attr;
		if (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
	    ++off;
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
 * Resize the shell to Rows and Columns.
 * Allocate ScreenLines[] and associated items.
 *
 * There may be some time between setting Rows and Columns and (re)allocating
 * ScreenLines[].  This happens when starting up and when (manually) changing
 * the shell size.  Always use screen_Rows and screen_Columns to access items
 * in ScreenLines[].  Use Rows and Columns for positioning text etc. where the
 * final size of the shell is needed.
 */
    void
screenalloc(clear)
    int	    clear;
{
    int		    new_row, old_row;
#ifdef FEAT_GUI
    int		    old_Rows;
#endif
    win_t	    *wp;
    int		    outofmem = FALSE;
    int		    len;
    schar_t	    *new_ScreenLines;
#ifdef FEAT_MBYTE
    u8char_t	    *new_ScreenLinesUC = NULL;
    u8char_t	    *new_ScreenLinesC1 = NULL;
    u8char_t	    *new_ScreenLinesC2 = NULL;
#endif
    sattr_t	    *new_ScreenAttrs;
    unsigned	    *new_LineOffset;
    static int	    entered = FALSE;		/* avoid recursiveness */

    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    if ((ScreenLines != NULL
		&& Rows == screen_Rows
		&& Columns == screen_Columns
#ifdef FEAT_MBYTE
		&& cc_utf8 == (ScreenLinesUC != NULL)
#endif
		)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && ScreenLines == NULL))
	return;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    if (entered)
	return;
    entered = TRUE;

#ifdef FEAT_GUI_BEOS
    vim_lock_screen();  /* be safe, put it here */
#endif

    comp_col();		/* recompute columns for shown command and ruler */

    /*
     * We're changing the size of the screen.
     * - Allocate new arrays for ScreenLines and ScreenAttrs.
     * - Move lines from the old arrays into the new arrays, clear extra
     *	 lines (unless the screen is going to be cleared).
     * - Free the old arrays.
     *
     * If anything fails, make ScreenLines NULL, so we don't do anything!
     * Continuing with the old ScreenLines may result in a crash, because the
     * size is wrong.
     */
    for (wp = firstwin; wp; wp = wp->w_next)
	win_free_lsize(wp);

    new_ScreenLines = (schar_t *)lalloc((long_u)(
			      (Rows + 1) * Columns * sizeof(schar_t)), FALSE);
#ifdef FEAT_MBYTE
    if (cc_utf8)
    {
	new_ScreenLinesUC = (u8char_t *)lalloc((long_u)(
			     (Rows + 1) * Columns * sizeof(u8char_t)), FALSE);
	new_ScreenLinesC1 = (u8char_t *)lalloc((long_u)(
			     (Rows + 1) * Columns * sizeof(u8char_t)), FALSE);
	new_ScreenLinesC2 = (u8char_t *)lalloc((long_u)(
			     (Rows + 1) * Columns * sizeof(u8char_t)), FALSE);
    }
#endif
    new_ScreenAttrs = (sattr_t *)lalloc((long_u)(
			      (Rows + 1) * Columns * sizeof(sattr_t)), FALSE);
    new_LineOffset = (unsigned *)lalloc((long_u)(
					 Rows * sizeof(unsigned)), FALSE);

    for (wp = firstwin; wp; wp = wp->w_next)
	if (win_alloc_lines(wp) == FAIL)
	{
	    outofmem = TRUE;
	    break;
	}

    if (new_ScreenLines == NULL
#ifdef FEAT_MBYTE
	    || (cc_utf8 && (new_ScreenLinesUC == NULL
		   || new_ScreenLinesC1 == NULL || new_ScreenLinesC2 == NULL))
#endif
	    || new_ScreenAttrs == NULL
	    || new_LineOffset == NULL
	    || outofmem)
    {
	do_outofmem_msg();
	vim_free(new_ScreenLines);
	new_ScreenLines = NULL;
#ifdef FEAT_MBYTE
	vim_free(new_ScreenLinesUC);
	new_ScreenLinesUC = NULL;
	vim_free(new_ScreenLinesC1);
	new_ScreenLinesC1 = NULL;
	vim_free(new_ScreenLinesC2);
	new_ScreenLinesC2 = NULL;
#endif
	vim_free(new_ScreenAttrs);
	new_ScreenAttrs = NULL;
	vim_free(new_LineOffset);
	new_LineOffset = NULL;
    }
    else
    {
	for (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_LineOffset[new_row] = new_row * Columns;

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    if (!clear)
	    {
		(void)vim_memset(new_ScreenLines + new_row * Columns,
				      ' ', (size_t)Columns * sizeof(schar_t));
#ifdef FEAT_MBYTE
		if (cc_utf8)
		{
		    (void)vim_memset(new_ScreenLinesUC + new_row * Columns,
				       0, (size_t)Columns * sizeof(u8char_t));
		    (void)vim_memset(new_ScreenLinesC1 + new_row * Columns,
				       0, (size_t)Columns * sizeof(u8char_t));
		    (void)vim_memset(new_ScreenLinesC2 + new_row * Columns,
				       0, (size_t)Columns * sizeof(u8char_t));
		}
#endif
		(void)vim_memset(new_ScreenAttrs + new_row * Columns,
					0, (size_t)Columns * sizeof(sattr_t));
		old_row = new_row + (screen_Rows - Rows);
		if (old_row >= 0)
		{
		    if (screen_Columns < Columns)
			len = screen_Columns;
		    else
			len = Columns;
		    mch_memmove(new_ScreenLines + new_LineOffset[new_row],
			    ScreenLines + LineOffset[old_row],
			    (size_t)len * sizeof(schar_t));
#ifdef FEAT_MBYTE
		    if (cc_utf8 && ScreenLinesUC != NULL)
		    {
			mch_memmove(new_ScreenLinesUC + new_LineOffset[new_row],
				ScreenLinesUC + LineOffset[old_row],
				(size_t)len * sizeof(u8char_t));
			mch_memmove(new_ScreenLinesC1 + new_LineOffset[new_row],
				ScreenLinesC1 + LineOffset[old_row],
				(size_t)len * sizeof(u8char_t));
			mch_memmove(new_ScreenLinesC2 + new_LineOffset[new_row],
				ScreenLinesC2 + LineOffset[old_row],
				(size_t)len * sizeof(u8char_t));
		    }
#endif
		    mch_memmove(new_ScreenAttrs + new_LineOffset[new_row],
			    ScreenAttrs + LineOffset[old_row],
			    (size_t)len * sizeof(sattr_t));
		}
	    }
	}
	/* Use the last line of the screen for the current line. */
	current_ScreenLine = new_ScreenLines + Rows * Columns;
#ifdef FEAT_FOLDING
	/* Note: current_ScreenLineUC doesn't exist */
	current_ScreenAttrs = new_ScreenAttrs + Rows * Columns;
#endif
    }

    vim_free(ScreenLines);
#ifdef FEAT_MBYTE
    vim_free(ScreenLinesUC);
    vim_free(ScreenLinesC1);
    vim_free(ScreenLinesC2);
#endif
    vim_free(ScreenAttrs);
    vim_free(LineOffset);
    ScreenLines = new_ScreenLines;
#ifdef FEAT_MBYTE
    ScreenLinesUC = new_ScreenLinesUC;
    ScreenLinesC1 = new_ScreenLinesC1;
    ScreenLinesC2 = new_ScreenLinesC2;
#endif
    ScreenAttrs = new_ScreenAttrs;
    LineOffset = new_LineOffset;

    /* It's important that screen_Rows and screen_Columns reflect the actual
     * size of ScreenLines[].  Set them before calling anything. */
#ifdef FEAT_GUI
    old_Rows = screen_Rows;
#endif
    screen_Rows = Rows;
    screen_Columns = Columns;

    must_redraw = CLEAR;	/* need to clear the screen later */
    if (clear)
	screenclear2();

#ifdef FEAT_GUI
    else if (gui.in_use
	    && !gui.starting
	    && ScreenLines != NULL
	    && old_Rows != Rows)
    {
	(void)gui_redraw_block(0, 0, (int)Rows - 1, (int)Columns - 1, 0);
	/*
	 * Adjust the position of the cursor, for when executing an external
	 * command.
	 */
	if (msg_row >= Rows)		/* Rows got smaller */
	    msg_row = Rows - 1;		/* put cursor at last row */
	else if (Rows > old_Rows)	/* Rows got bigger */
	    msg_row += Rows - old_Rows; /* put cursor in same place */
	if (msg_col >= Columns)		/* Columns got smaller */
	    msg_col = Columns - 1;	/* put cursor at last column */
    }
#endif

#ifdef FEAT_GUI_BEOS
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

    if (starting == NO_SCREEN || ScreenLines == NULL)
	return;

    screen_stop_highlight();	/* don't want highlighting here */
    out_str(T_CL);		/* clear the display */

    /* blank out ScreenLines */
    for (i = 0; i < Rows; ++i)
	lineclear(LineOffset[i]);

    screen_cleared = TRUE;	    /* can use contents of ScreenLines now */

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
 * Clear one line in ScreenLines.
 */
    static void
lineclear(off)
    unsigned  off;
{
    (void)vim_memset(ScreenLines + off, ' ', (size_t)Columns * sizeof(schar_t));
#ifdef FEAT_MBYTE
    if (cc_utf8)
	(void)vim_memset(ScreenLinesUC + off, 0, (size_t)Columns * sizeof(u8char_t));
#endif
    (void)vim_memset(ScreenAttrs + off, 0, (size_t)Columns * sizeof(sattr_t));
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
    /* Can't use ScreenLines unless initialized */
    if (ScreenLines == NULL)
	return;

    if (col != screen_cur_col || row != screen_cur_row)
    {
	/* Check for valid position. */
	if (row < 0)	/* window without text lines? */
	    row = 0;
	if (row >= screen_Rows)
	    row = screen_Rows - 1;
	if (col >= screen_Columns)
	    col = screen_Columns - 1;

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
	     * correct attributes.  Also avoid UTF-8 characters.
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
		p = ScreenAttrs + LineOffset[row] + wouldbe_col;
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
#if defined(FEAT_MOUSE) && defined(FEAT_MBYTE)
		if (cc_utf8)
		{
		    /* Don't use an UTF-8 char for positioning, it's slow. */
		    i = col - wouldbe_col;
		    p = ScreenLines + LineOffset[row] + wouldbe_col;
		    while (i && MB_BYTE2LEN(*p++) == 1)
			--i;
		    if (i)
			cost = 999;
		}
#endif
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
			int	off;

			off = LineOffset[row] + screen_cur_col;
			while (i--)
			{
			    if (ScreenAttrs[off] != screen_attr)
				screen_stop_highlight();
			    out_char(ScreenLines[off]);
			    ++off;
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
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		W_WINCOL(curwin) + (
#ifdef FEAT_RIGHTLEFT
		curwin->w_p_rl ? ((int)W_WIDTH(curwin) - 1 - curwin->w_wcol) :
#endif
							    curwin->w_wcol));
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
    void
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

#if 0 /* never used */
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
    if (curwin->w_cursor.lnum < curwin->w_topline
	    || curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count
	    || !(curwin->w_valid & (VALID_BOTLINE|VALID_BOTLINE_AP))
	    || curwin->w_cursor.lnum >= curwin->w_botline)
	return FAIL;

    check_cursor_moved(curwin);
    if ((curwin->w_valid & (VALID_CROW|VALID_CHEIGHT))
						!= (VALID_CROW|VALID_CHEIGHT))
	curs_rows(curwin, TRUE);
    return OK;
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
    static void
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
    void
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
#ifdef FEAT_FOLDING
	    + (wp->w_p_fdc ? 2 : 0)
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
    if (ve_all
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
		    && (n = plines_win(curwin, curwin->w_cursor.lnum, FALSE)
						    - 1) >= curwin->w_height))
	    && curwin->w_height != 0
	    && curwin->w_cursor.lnum == curwin->w_topline
#ifdef FEAT_VERTSPLIT
	    && curwin->w_width != 0
#endif
	    )
    {
	/* Cursor past end of screen.  Happens with a single line that does
	 * not fit on screen.  Find a skipcol to show the text around the
	 * cursor.  Avoid scrolling all the time. */
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
	    n = plines_win(curwin, curwin->w_cursor.lnum, FALSE) - 1;
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
	    extra = (curwin->w_skipcol - prev_skipcol) / width;
	    if (extra > 0)
		win_ins_lines(curwin, 0, extra, FALSE, FALSE);
	    else if (extra < 0)
		win_del_lines(curwin, 0, -extra, FALSE, FALSE);
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
		win_ins_lines(curwin, 0, extra, FALSE, FALSE);
		curwin->w_skipcol -= extra * width;
	    }
	}
	else if (extra == 2)
	{
	    /* less then 'scrolloff' lines below, increase skipcol */
	    endcol = (n - curwin->w_height + 1) * width;
	    if (endcol > curwin->w_skipcol)
	    {
		win_del_lines(curwin, 0, (endcol - prev_skipcol) / width,
								FALSE, FALSE);
		curwin->w_skipcol = endcol;
	    }
	}
	curwin->w_wrow -= curwin->w_skipcol / width;
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
    void
scrolldown(line_count)
    long    line_count;
{
    long    done = 0;		/* total # of physical lines done */
    int	    wrow;
    int	    moved = FALSE;

#ifdef FEAT_FOLDING
    linenr_t	first;

    /* Make sure w_topline is at the first of a sequence of folded lines. */
    (void)hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
    validate_cursor();		/* w_wrow needs to be valid */
    while (line_count--)
    {
	if (curwin->w_topline == 1)
	    break;
	--curwin->w_topline;
#ifdef FEAT_FOLDING
	/* A sequence of folded lines only counts for one logical line */
	if (hasFolding(curwin->w_topline, &first, NULL))
	{
	    ++done;
	    curwin->w_botline -= curwin->w_topline - first;
	    curwin->w_topline = first;
	}
	else
#endif
	    done += plines(curwin->w_topline);
	--curwin->w_botline;		/* approximate w_botline */
	curwin->w_valid &= ~VALID_BOTLINE;
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
	coladvance(curwin->w_curswant);
}

/*
 * Scroll the current window up by "line_count" logical lines.
 */
    void
scrollup(line_count)
    long    line_count;
{
#ifdef FEAT_FOLDING
    if (hasAnyFolding(curwin))
    {
	linenr_t	lnum = curwin->w_topline;

	/* count each sequence of folded lines as one logical line */
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

/*
 * insert 'line_count' lines at 'row' in window 'wp'
 * if 'invalid' is TRUE the wp->w_lines[].wl_lnum is invalidated.
 * if 'mayclear' is TRUE the screen will be cleared if it is faster than
 * scrolling.
 * Returns FAIL if the lines are not inserted, OK for success.
 */
    int
win_ins_lines(wp, row, line_count, invalid, mayclear)
    win_t	*wp;
    int		row;
    int		line_count;
    int		invalid;
    int		mayclear;
{
    int		did_delete;
    int		nextrow;
    int		lastrow;
    int		retval;

    if (invalid)
	wp->w_lines_valid = 0;

    if (wp->w_height < 5)
	return FAIL;

    if (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, FALSE);
    if (retval != MAYBE)
	return retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to avoid
     * messing up those windows, better just redraw.
     */
    did_delete = FALSE;
#ifdef FEAT_WINDOWS
    if (wp->w_next || wp->w_status_height)
    {
	if (screen_del_lines(0, W_WINROW(wp) + wp->w_height - line_count,
					  line_count, (int)Rows, FALSE) == OK)
	    did_delete = TRUE;
	else if (wp->w_next)
	    return FAIL;
    }
#endif
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    if (!did_delete)
    {
#ifdef FEAT_WINDOWS
	wp->w_redr_status = TRUE;
#endif
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + W_STATUS_HEIGHT(wp);
	lastrow = nextrow + line_count;
	if (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - line_count, lastrow - line_count,
		  W_WINCOL(wp), (int)W_ENDCOL(wp),
		  ' ', ' ', 0);
    }

    if (screen_ins_lines(0, W_WINROW(wp) + row, line_count, (int)Rows) == FAIL)
    {
	    /* deletion will have messed up other windows */
	if (did_delete)
	{
#ifdef FEAT_WINDOWS
	    wp->w_redr_status = TRUE;
#endif
	    win_rest_invalid(wp->w_next);
	}
	return FAIL;
    }

    return OK;
}

/*
 * delete "line_count" window lines at "row" in window "wp"
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    int
win_del_lines(wp, row, line_count, invalid, mayclear)
    win_t	*wp;
    int		row;
    int		line_count;
    int		invalid;
    int		mayclear;
{
    int		retval;

    if (invalid)
	wp->w_lines_valid = 0;

    if (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, TRUE);
    if (retval != MAYBE)
	return retval;

    if (screen_del_lines(0, W_WINROW(wp) + row, line_count,
						    (int)Rows, FALSE) == FAIL)
	return FAIL;

#ifdef FEAT_WINDOWS
    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    if (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	if (screen_ins_lines(0, W_WINROW(wp) + wp->w_height - line_count,
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
#endif
	redraw_cmdline = TRUE;
    return OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    static int
win_do_lines(wp, row, line_count, mayclear, del)
    win_t	*wp;
    int		row;
    int		line_count;
    int		mayclear;
    int		del;
{
    int		retval;

    if (!redrawing() || line_count <= 0)
	return FAIL;

    /* only a few lines left: redraw is faster */
    if (mayclear && Rows - line_count < 5)
    {
	screenclear();	    /* will set wp->w_lines_valid to 0 */
	return FAIL;
    }

    /*
     * Delete all remaining lines
     */
    if (row + line_count >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_WINCOL(wp), (int)W_ENDCOL(wp),
		' ', ' ', 0);
	return OK;
    }

#ifdef FEAT_VERTSPLIT
    /* Can't use scrolling for vertically split window. */
    if (W_WIDTH(wp) != Columns)
	return FAIL;
#endif

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
	if (del)
	    retval = screen_del_lines(W_WINROW(wp) + row, 0, line_count,
						   wp->w_height - row, FALSE);
	else
	    retval = screen_ins_lines(W_WINROW(wp) + row, 0, line_count,
							  wp->w_height - row);
	scroll_region_reset();
	return retval;
    }

    if (wp->w_next && p_tf)	/* don't delete/insert on fast terminal */
	return FAIL;

    return MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    void
win_rest_invalid(wp)
    win_t	*wp;
{
    while (wp != NULL)
    {
	wp->w_lines_valid = 0;
	wp->w_redr_type = NOT_VALID;
#ifdef FEAT_WINDOWS
	wp->w_redr_status = TRUE;
#endif
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
 * insert lines on the screen and update ScreenLines[]
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
    unsigned	temp;
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

#ifdef FEAT_GUI_BEOS
    vim_lock_screen();
#endif
    if (*T_CCS != NUL)	   /* cursor relative to region */
	cursor_row = row;
    else
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
	j = end - 1 - i;
	temp = LineOffset[j];
	while ((j -= line_count) >= row)
	    LineOffset[j + line_count] = LineOffset[j];
	LineOffset[j + line_count] = temp;
	lineclear(temp);
    }
#ifdef FEAT_GUI_BEOS
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

#ifdef FEAT_GUI
    if (gui.in_use)
	out_flush();	/* always flush after a scroll */
#endif
    return OK;
}

/*
 * delete lines on the screen and update ScreenLines[]
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
    unsigned	temp;
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

#ifdef FEAT_GUI_BEOS
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
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
	j = row + i;
	temp = LineOffset[j];
	while ((j += line_count) <= end - 1)
	    LineOffset[j - line_count] = LineOffset[j];
	LineOffset[j - line_count] = temp;
	lineclear(temp);
    }
#ifdef FEAT_GUI_BEOS
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

#ifdef FEAT_GUI
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
    int		need_clear;
    int		length = 0;
    int		do_mode;
    int		attr;
    int		nwr_save;
#ifdef FEAT_INS_EXPAND
    int		sub_attr;
#endif

    do_mode = (p_smd && ((State & INSERT) || restart_edit
#ifdef FEAT_VISUAL
		|| VIsual_active
#endif
		));
    if (do_mode || Recording)
    {
	/*
	 * Don't show mode right now, when not redrawing or inside a mapping.
	 * Call char_avail() only when we are going to show something, because
	 * it takes a bit of time.
	 */
	if (!redrawing() || (char_avail() && !KeyTyped))
	{
	    redraw_cmdline = TRUE;		/* show mode later */
	    return 0;
	}

	nwr_save = need_wait_return;

	/* wait a bit before overwriting an important message */
	check_for_delay(FALSE);

	/* if the cmdline is more than one line high, erase top lines */
	need_clear = clear_cmdline;
	if (clear_cmdline && cmdline_row < Rows - 1)
	    msg_clr_cmdline();			/* will reset clear_cmdline */

	/* Position on the last line in the window, column 0 */
	msg_pos_mode();
	cursor_off();
	attr = hl_attr(HLF_CM);			/* Highlight mode */
	if (do_mode)
	{
	    MSG_PUTS_ATTR("--", attr);
#if defined(FEAT_HANGULIN) && defined(FEAT_GUI)
	    if (gui.in_use)
	    {
		if (hangul_input_state_get())
		    MSG_PUTS_ATTR(" ÇÑ±Û", attr);   /* HANGUL */
	    }
#endif
#ifdef FEAT_INS_EXPAND
	    if (edit_submode != NULL)		/* CTRL-X in Insert mode */
	    {
		msg_puts_attr(edit_submode, attr);
		if (edit_submode_extra != NULL)
		{
		    MSG_PUTS_ATTR(" ", attr);	/* add a space in between */
		    if ((int)edit_submode_highl < (int)HLF_COUNT)
			sub_attr = hl_attr(edit_submode_highl);
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
#ifdef FEAT_RIGHTLEFT
		    if (p_ri)
			MSG_PUTS_ATTR(_(" REVERSE"), attr);
#endif
		    MSG_PUTS_ATTR(_(" INSERT"), attr);
		}
		else if (State == REPLACE)
		    MSG_PUTS_ATTR(_(" REPLACE"), attr);
		else if (State == VREPLACE)
		    MSG_PUTS_ATTR(_(" VREPLACE"), attr);
		else if (restart_edit == 'I')
		    MSG_PUTS_ATTR(_(" (insert)"), attr);
		else if (restart_edit == 'R')
		    MSG_PUTS_ATTR(_(" (replace)"), attr);
		else if (restart_edit == 'V')
		    MSG_PUTS_ATTR(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		if (p_hkmap)
		    MSG_PUTS_ATTR(_(" Hebrew"), attr);
# ifdef FEAT_FKMAP
		if (p_fkmap)
		    MSG_PUTS_ATTR(farsi_text_5, attr);
# endif
#endif
		if ((State & INSERT) && p_paste)
		    MSG_PUTS_ATTR(_(" (paste)"), attr);

#ifdef FEAT_VISUAL
		if (VIsual_active)
		{
		    if (VIsual_select)
			MSG_PUTS_ATTR(_(" SELECT"), attr);
		    else
			MSG_PUTS_ATTR(_(" VISUAL"), attr);
		    if (VIsual_mode == Ctrl_V)
			MSG_PUTS_ATTR(_(" BLOCK"), attr);
		    else if (VIsual_mode == 'V')
			MSG_PUTS_ATTR(_(" LINE"), attr);
		}
#endif
	    }
	    MSG_PUTS_ATTR(" --", attr);
	    need_clear = TRUE;
	}
	if (Recording)
	{
	    MSG_PUTS_ATTR(_("recording"), attr);
	    need_clear = TRUE;
	}
	if (need_clear || clear_cmdline)
	    msg_clr_eos();
	msg_didout = FALSE;		/* overwrite this message */
	length = msg_col;
	msg_col = 0;
	need_wait_return = nwr_save;	/* never ask for hit-return for this */
    }
    else if (clear_cmdline)		/* just clear anything */
	msg_clr_cmdline();		/* will reset clear_cmdline */

#ifdef FEAT_CMDL_INFO
    /* If the last window has no status line, the ruler is after the mode
     * message and must be redrawn */
# ifdef FEAT_WINDOWS
    if (lastwin->w_status_height == 0)
# endif
	win_redr_ruler(lastwin, TRUE);
#endif
    redraw_cmdline = FALSE;
    clear_cmdline = FALSE;

    return length;
}

/*
 * Position for a mode message.
 */
    static void
msg_pos_mode()
{
    msg_col = 0;
    msg_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
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
    else
    {
	msg_pos_mode();
	if (Recording)
	    MSG_PUTS_ATTR(_("recording"), hl_attr(HLF_CM));
	msg_clr_eos();
    }
}

#ifdef FEAT_WINDOWS
/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    static int
fillchar_status(attr, is_curwin)
    int		*attr;
    int		is_curwin;
{
    int fill;
    if (is_curwin)
    {
	*attr = hl_attr(HLF_S);
	fill = fill_stl;
    }
    else
    {
	*attr = hl_attr(HLF_SNC);
	fill = fill_stlnc;
    }
    /* Use fill when there is highlighting, and highlighting of current
     * window differs, or the fillchars differ, or this is not the
     * current window */
    if (*attr != 0 && ((hl_attr(HLF_S) != hl_attr(HLF_SNC)
			|| !is_curwin || firstwin == lastwin)
		    || (fill_stl != fill_stlnc)))
	return fill;
    if (is_curwin)
	return '^';
    return '=';
}
#endif

#ifdef FEAT_VERTSPLIT
/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 */
    static int
fillchar_vsep(attr)
    int	    *attr;
{
    *attr = hl_attr(HLF_C);
    if (*attr == 0 && fill_vert == ' ')
	return '|';
    else
	return fill_vert;
}
#endif

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    void
showruler(always)
    int	    always;
{
    if (!always && !redrawing())
	return;
#if defined(FEAT_STL_OPT) && defined(FEAT_WINDOWS)
    if (*p_stl && curwin->w_status_height)
	win_redr_custom(curwin, FALSE);
    else
#endif
#ifdef FEAT_CMDL_INFO
	win_redr_ruler(curwin, always);
#endif

#ifdef FEAT_TITLE
    if (need_maketitle
# ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
# endif
       )
	maketitle();
#endif
}

#ifdef FEAT_CMDL_INFO
    static void
win_redr_ruler(wp, always)
    win_t	*wp;
    int		always;
{
    char_u	buffer[70];
    int		row;
    int		fillchar;
    int		attr;
    int		empty_line = FALSE;
    colnr_t	virtcol;
    int		i;
    int		o;
#ifdef FEAT_VERTSPLIT
    int		this_ru_col;
    int		off = 0;
    int		width = Columns;
# define WITH_OFF(x) x
# define WITH_WIDTH(x) x
#else
# define WITH_OFF(x) 0
# define WITH_WIDTH(x) Columns
# define this_ru_col ru_col
#endif

    /* If 'ruler' off or redrawing disabled, don't do anything */
    if (!p_ru)
	return;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    if (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_line_count)
	return;

#ifdef FEAT_STL_OPT
    if (*p_ruf)
    {
	win_redr_custom(wp, TRUE);
	return;
    }
#endif

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
#ifdef FEAT_VIRTUALEDIT
	    || wp->w_coladd != wp->w_ru_coladd
#endif
	    || wp->w_topline != wp->w_ru_topline
	    || empty_line != wp->w_ru_empty)
    {
	cursor_off();
#ifdef FEAT_WINDOWS
	if (wp->w_status_height)
	{
	    row = W_WINROW(wp) + wp->w_height;
	    fillchar = fillchar_status(&attr, wp == curwin);
# ifdef FEAT_VERTSPLIT
	    off = W_WINCOL(wp);
	    width = W_WIDTH(wp);
# endif
	}
	else
#endif
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
#ifdef FEAT_VERTSPLIT
	    width = Columns;
	    off = 0;
#endif
	}

	/* In list mode virtcol needs to be recomputed */
	virtcol = wp->w_virtcol;
	if (wp->w_p_list && lcs_tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvcol(wp, &wp->w_cursor, NULL, &virtcol, NULL);
	    wp->w_p_list = TRUE;
	}
#ifdef FEAT_VIRTUALEDIT
	virtcol += wp->w_coladd;
#endif

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

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	i = STRLEN(buffer);
	get_rel_pos(wp, buffer + i + 1);
	o = STRLEN(buffer + i + 1);
	if (wp->w_status_height == 0)	/* can't use last char of screen */
	    ++o;
#ifdef FEAT_VERTSPLIT
	this_ru_col = ru_col - (Columns - width);
	if (this_ru_col < 0)
	    this_ru_col = 0;
#endif
	if (this_ru_col + i + o < WITH_WIDTH(width))
	{
	    while (this_ru_col + i + o < WITH_WIDTH(width))
		buffer[i++] = fillchar;
	    get_rel_pos(wp, buffer + i);
	}
	/* Truncate at window boundary. */
	if (this_ru_col + (int)STRLEN(buffer) > WITH_WIDTH(width))
	    buffer[WITH_WIDTH(width) - this_ru_col] = NUL;

	screen_puts(buffer, row, this_ru_col + WITH_OFF(off), attr);
	screen_fill(row, row + 1,
		this_ru_col + WITH_OFF(off) + (int)STRLEN(buffer),
		(int)WITH_OFF(off) + WITH_WIDTH(width),
		fillchar, fillchar, attr);
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
#ifdef FEAT_VIRTUALEDIT
	wp->w_ru_coladd = wp->w_coladd;
#endif
	wp->w_ru_empty = empty_line;
	wp->w_ru_topline = wp->w_topline;
    }
}
#endif

#if defined(FEAT_STL_OPT) || defined(FEAT_CMDL_INFO)
/*
 * Get relative cursor position in window, in the form 99%, using "Top", "Bot"
 * or "All" when appropriate.
 */
    static void
get_rel_pos(wp, str)
    win_t	*wp;
    char_u	*str;
{
    long	above; /* number of lines above window */
    long	below; /* number of lines below window */

    above = wp->w_topline - 1;
    below = wp->w_buffer->b_ml.ml_line_count - wp->w_botline + 1;
    if (below <= 0)
	STRCPY(str, above == 0 ? _("All") : _("Bot"));
    else if (above <= 0)
	STRCPY(str, _("Top"));
    else
	sprintf((char *)str, "%2d%%",
		(int)(above * 100 / (above + below)));
}
#endif

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
	out_flush();
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
    return (ScreenLines != NULL);
}

#if defined(FEAT_MOUSE) || defined(PROTO)

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
 * CURSOR_MOVED bit, this function returns one of IN_UNKNOWN, IN_BUFFER,
 * IN_STATUS_LINE or IN_SEP_LINE depending on where the cursor was clicked.
 *
 * If flags has MOUSE_MAY_STOP_VIS, then Visual mode will be stopped, unless
 * the mouse is on the status line of the same window.
 *
 * If flags has MOUSE_DID_MOVE, nothing is done if the mouse didn't move since
 * the last call.
 *
 * If flags has MOUSE_SETPOS, nothing is done, only the current position is
 * remembered.
 */
    int
jump_to_mouse(flags, inclusive)
    int		flags;
    int		*inclusive;	/* used for inclusive operator, can be NULL */
{
    static int	on_status_line = 0;	/* #lines below bottom of window */
#ifdef FEAT_VERTSPLIT
    static int	on_sep_line = 0;	/* on separator right of window */
#endif
    static int	prev_row = -1;
    static int	prev_col = -1;

    win_t	*wp, *old_curwin;
    pos_t	old_cursor;
    int		count;
    int		first;
    int		row = mouse_row;
    int		col = mouse_col;

    mouse_past_bottom = FALSE;
    mouse_past_eol = FALSE;

    if ((flags & MOUSE_DID_MOVE)
	    && prev_row == mouse_row
	    && prev_col == mouse_col)
    {
retnomove:
	/* before moving the cursor for a left click wich is NOT in a status
	 * line, stop Visual mode */
	if (on_status_line)
	    return IN_STATUS_LINE;
#ifdef FEAT_VERTSPLIT
	if (on_sep_line)
	    return IN_SEP_LINE;
#endif
#ifdef FEAT_VISUAL
	if (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode();
	    redraw_curbuf_later(INVERTED);	/* delete the inversion */
	}
#endif
	return IN_BUFFER;
    }

    prev_row = mouse_row;
    prev_col = mouse_col;

    if ((flags & MOUSE_SETPOS))
	goto retnomove;				/* ugly goto... */

    old_curwin = curwin;
    old_cursor = curwin->w_cursor;

    if (!(flags & MOUSE_FOCUS))
    {
	if (row < 0 || col < 0)			/* check if it makes sense */
	    return IN_UNKNOWN;

#ifdef FEAT_WINDOWS
	/* find the window where the row is in */
	wp = mouse_find_win(&row, &col);
#else
	wp = firstwin;
#endif
	/*
	 * winpos and height may change in win_enter()!
	 */
	if (row >= wp->w_height)		/* In (or below) status line */
	    on_status_line = row - wp->w_height + 1;
	else
	    on_status_line = 0;
#ifdef FEAT_VERTSPLIT
	if (col >= wp->w_width)		/* In separator line */
	    on_sep_line = col - wp->w_width + 1;
	else
	    on_sep_line = 0;
#endif

#ifdef FEAT_VISUAL
	/* Before jumping to another buffer, or moving the cursor for a left
	 * click, stop Visual mode. */
	if (VIsual_active
		&& (wp->w_buffer != curwin->w_buffer
		    || (!on_status_line
# ifdef FEAT_VERTSPLIT
			&& !on_sep_line
# endif
			&& (flags & MOUSE_MAY_STOP_VIS))))
	{
	    end_visual_mode();
	    redraw_curbuf_later(INVERTED);	/* delete the inversion */
	}
#endif
#ifdef FEAT_WINDOWS
	win_enter(wp, TRUE);			/* can make wp invalid! */
# ifdef CHECK_DOUBLE_CLICK
	/* set topline, to be able to check for double click ourselves */
	if (curwin != old_curwin)
	    set_mouse_topline(curwin->w_topline);
# endif
#endif
	if (on_status_line)			/* In (or below) status line */
	{
	    /* Don't use start_arrow() if we're in the same window */
	    if (curwin == old_curwin)
		return IN_STATUS_LINE;
	    else
		return IN_STATUS_LINE | CURSOR_MOVED;
	}
#ifdef FEAT_VERTSPLIT
	if (on_sep_line)			/* In (or below) status line */
	{
	    /* Don't use start_arrow() if we're in the same window */
	    if (curwin == old_curwin)
		return IN_SEP_LINE;
	    else
		return IN_SEP_LINE | CURSOR_MOVED;
	}
#endif

	curwin->w_cursor.lnum = curwin->w_topline;
#ifdef FEAT_GUI
	/* remember topline, needed for double click */
	gui_prev_topline = curwin->w_topline;
#endif
    }
    else if (on_status_line)
    {
#ifdef FEAT_WINDOWS
	/* Drag the status line */
	count = row - curwin->w_winrow - curwin->w_height + 1 - on_status_line;
	win_drag_status_line(count);
#endif
	return IN_STATUS_LINE;			/* Cursor didn't move */
    }
#ifdef FEAT_VERTSPLIT
    else if (on_sep_line)
    {
	/* Drag the separator column */
	count = col - curwin->w_wincol - curwin->w_width + 1 - on_sep_line;
	win_drag_vsep_line(count);
	return IN_SEP_LINE;			/* Cursor didn't move */
    }
#endif
    else /* keep_window_focus must be TRUE */
    {
#ifdef FEAT_VISUAL
	/* before moving the cursor for a left click, stop Visual mode */
	if (flags & MOUSE_MAY_STOP_VIS)
	{
	    end_visual_mode();
	    redraw_curbuf_later(INVERTED);	/* delete the inversion */
	}
#endif

	row -= W_WINROW(curwin);
#ifdef FEAT_VERTSPLIT
	col -= W_WINCOL(curwin);
#endif

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
#ifdef FEAT_FOLDING
		hasFolding(curwin->w_topline, &curwin->w_topline, NULL);
#endif
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
#ifdef FEAT_FOLDING
		hasFolding(curwin->w_topline, NULL, &curwin->w_topline);
#endif
	    }
	    redraw_later(VALID);
	    curwin->w_valid &=
		      ~(VALID_WROW|VALID_CROW|VALID_BOTLINE|VALID_BOTLINE_AP);
	    row = curwin->w_height - 1;
	}
	else if (row == 0)
	{
	    /* When dragging the mouse, while the text has been scrolled up as
	     * far as it goes, moving the mouse in the top line should scroll
	     * the text down (done later when recomputing w_topline). */
	    if (mouse_dragging
		    && curwin->w_cursor.lnum
				       == curwin->w_buffer->b_ml.ml_line_count
		    && curwin->w_cursor.lnum == curwin->w_topline)
		curwin->w_valid &= ~(VALID_TOPLINE);
	}
    }

    /* compute the position in the buffer line from the posn on the screen */
    if (mouse_comp_pos(&row, &col, &curwin->w_cursor.lnum))
	mouse_past_bottom = TRUE;

#ifdef FEAT_VISUAL
    /* Start Visual mode before coladvance(), for when 'sel' != "old" */
    if ((flags & MOUSE_MAY_VIS) && !VIsual_active)
    {
	check_visual_highlight();
	VIsual = old_cursor;
	VIsual_active = TRUE;
	VIsual_reselect = TRUE;
	/* if 'selectmode' contains "mouse", start Select mode */
	may_start_select('o');
	setmouse();
	if (p_smd)
	    redraw_cmdline = TRUE;	/* show visual mode later */
    }
#endif

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

    if (curwin == old_curwin && curwin->w_cursor.lnum == old_cursor.lnum
	    && curwin->w_cursor.col == old_cursor.col)
	return IN_BUFFER;		/* Cursor has not moved */
    return IN_BUFFER | CURSOR_MOVED;	/* Cursor has moved */
}

/*
 * Compute the position in the buffer line from the posn on the screen.
 * Only works for current window.
 * Returns TRUE if the position is below the last line.
 */
    static int
mouse_comp_pos(rowp, colp, lnump)
    int		*rowp;
    int		*colp;
    linenr_t	*lnump;
{
    int		col = *colp;
    int		row = *rowp;
    linenr_t	lnum;
    int		retval = FALSE;
    int		off;
    int		count;

#ifdef FEAT_RIGHTLEFT
    if (curwin->w_p_rl)
	col = W_WIDTH(curwin) - 1 - col;
#endif

    lnum = curwin->w_topline;
    if (curwin->w_p_wrap)	/* lines wrap */
    {
	while (row > 0)
	{
	    count = plines(lnum);
	    if (count > row)
	    {
		/* Position is in this buffer line.  Compute the column
		 * without wrapping. */
		off = curwin_col_off() - curwin_col_off2();
		if (col < off)
		    col = off;
		col += row * (W_WIDTH(curwin) - off);
		/* add skip column (for long wrapping line) */
		col += curwin->w_skipcol;
		break;
	    }
	    if (lnum == curbuf->b_ml.ml_line_count)
	    {
		retval = TRUE;
		break;		/* past end of file */
	    }
	    row -= count;
#ifdef FEAT_FOLDING
	    (void)hasFolding(lnum, NULL, &lnum);
#endif
	    ++lnum;
	}
    }
    else			/* lines don't wrap */
    {
#ifdef FEAT_FOLDING
	if (hasAnyFolding(curwin))
	{
	    while (row > 0)
	    {
		--row;
		++lnum;
		if (lnum > curbuf->b_ml.ml_line_count)
		    break;
		(void)hasFolding(lnum, NULL, &lnum);
	    }
	}
	else
#endif
	    lnum += row;
	if (lnum > curbuf->b_ml.ml_line_count)
	{
	    lnum = curbuf->b_ml.ml_line_count;
	    retval = TRUE;
	}
	col += curwin->w_leftcol;
    }

    /* skip line number and fold column in front of the line */
    col -= curwin_col_off();
    if (col < 0)
	col = 0;

    *colp = col;
    *rowp = row;
    *lnump = lnum;
    return retval;
}

#ifdef FEAT_WINDOWS
/*
 * Find the window at screen position "*rowp" and "*colp".  The positions are
 * updated to become relative to the top-left of the window.
 */
/*ARGSUSED*/
    win_t *
mouse_find_win(rowp, colp)
    int		*rowp;
    int		*colp;
{
    frame_t	*fp;

    fp = topframe;
    for (;;)
    {
	if (fp->fr_layout == FR_LEAF)
	    break;
#ifdef FEAT_VERTSPLIT
	if (fp->fr_layout == FR_ROW)
	{
	    for (fp = fp->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    {
		if (*colp < fp->fr_width)
		    break;
		*colp -= fp->fr_width;
	    }
	}
#endif
	else    /* fr_layout == FR_COL */
	{
	    for (fp = fp->fr_child; fp->fr_next != NULL; fp = fp->fr_next)
	    {
		if (*rowp < fp->fr_height)
		    break;
		*rowp -= fp->fr_height;
	    }
	}
    }
    return fp->fr_win;
}
#endif

#if defined(FEAT_GUI_MOTIF) || defined(FEAT_GUI_GTK) || defined (FEAT_GUI_MAC) \
	|| defined(FEAT_GUI_ATHENA) || defined(FEAT_GUI_MSWIN) || defined(PROTO)
/*
 * Translate window coordinates to buffer position without any side effects
 */
    int
get_fpos_of_mouse(mpos)
    pos_t	*mpos;
{
    win_t	*wp;
    int		count;
    char_u	*ptr;
    int		row = mouse_row;
    int		col = mouse_col;

    if (row < 0 || col < 0)		/* check if it makes sense */
	return IN_UNKNOWN;

#ifdef FEAT_WINDOWS
    /* find the window where the row is in */
    wp = mouse_find_win(&row, &col);
#else
    wp = firstwin;
#endif
    /*
     * winpos and height may change in win_enter()!
     */
    if (row >= wp->w_height)	/* In (or below) status line */
	return IN_STATUS_LINE;
#ifdef FEAT_VERTSPLIT
    if (col >= wp->w_width)	/* In vertical separator line */
	return IN_SEP_LINE;
#endif

    if (wp != curwin)
	return IN_UNKNOWN;

    /* compute the position in the buffer line from the posn on the screen */
    if (mouse_comp_pos(&row, &col, &mpos->lnum))
	return IN_STATUS_LINE; /* past bottom */

    /* try to advance to the specified column */
    mpos->col = 0;
    count = 0;
    ptr = ml_get_buf(wp->w_buffer, mpos->lnum, FALSE);
    while (count <= col && *ptr != NUL)
    {
	++mpos->col;
	count += win_lbr_chartabsize(wp, ptr, count, NULL);
#ifdef FEAT_MBYTE
	if (has_mbyte)
	    ptr += mb_ptr2len_check(ptr);
	else
#endif
	    ++ptr;
    }
    if (mpos->col == 0)
	return IN_BUFFER;
    --mpos->col;
    return IN_BUFFER;
}
#endif

#endif /* FEAT_MOUSE */

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
#ifdef FEAT_FOLDING
	    (void)hasFolding(curwin->w_cursor.lnum, NULL, &curwin->w_cursor.lnum);
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
#ifdef FEAT_FOLDING
	    (void)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL);
#endif
	}
#endif
    }
    cursor_correct();
    beginline(BL_SOL | BL_FIX);
    redraw_later(VALID);
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
    char_u	vers[20];
    static char	*(lines[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
	"",
	N_("Vim is freely distributable"),
	N_("type  :help uganda<Enter>     if you like Vim "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version6<Enter>   for version info"),
	NULL,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };

    row = ((int)Rows - (int)(sizeof(lines) / sizeof(char *))) / 2;
    if (!p_cp)
	row += 2;
#if defined(WIN32) && !defined(FEAT_GUI_W32)
    if (mch_windows95())
	row -= 2;
#endif
#if defined(__BEOS__) && defined(__INTEL__)
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
	    if (*lines[i] != NUL)
	    {
		col = strlen(_(lines[i]));
		if (i == 2)
		{
		    STRCPY(vers, mediumVersion);
		    if (highest_patch())
		    {
			/* Check for 9.9x, alpha/beta version */
			if (isalpha(mediumVersion[3]))
			    sprintf((char *)vers + 4, ".%d%s", highest_patch(),
							   mediumVersion + 4);
			else
			    sprintf((char *)vers + 3, ".%d", highest_patch());
		    }
		    col += STRLEN(vers);
		}
		col = (Columns - col) / 2;
		if (col < 0)
		    col = 0;
		screen_puts((char_u *)_(lines[i]), row, col, 0);
		if (i == 2)
		    screen_puts(vers, row, col + 8, 0);
	    }
	    ++row;
	}
#if defined(WIN32) && !defined(FEAT_GUI_W32)
	if (mch_windows95())
	{
	    screen_puts((char_u *)_("WARNING: Windows 95 detected"),
					    row + 1, col + 8, hl_attr(HLF_E));
	    screen_puts((char_u *)_("type  :help windows95<Enter>  for info on this"),
							     row + 2, col, 0);
	}
#endif
#if defined(__BEOS__) && defined(__INTEL__)
	screen_puts((char_u *)_("     WARNING: Intel CPU detected.    "),
					    row + 1, col + 4, hl_attr(HLF_E));
	screen_puts((char_u *)_(" PPC has a much better architecture. "),
					    row + 2, col + 4, hl_attr(HLF_E));
#endif
    }
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
/*ARGSUSED*/
    void
ex_intro(eap)
    exarg_t	*eap;
{
    screenclear();
    intro_message();
    wait_return(TRUE);
}
