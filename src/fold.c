/* vim:set ts=8 sts=4 sw=4:
 * vim600:fdm=marker fdl=1 fdc=3:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * fold.c: code for folding
 */

#include "vim.h"

#if defined(FEAT_FOLDING) || defined(PROTO)

/* local declarations. {{{1 */
/* typedef fold_t {{{2 */
/*
 * The toplevel folds for each window are stored in the w_folds growarray.
 * Each toplevel fold can contain an array of second level folds in the
 * fd_nested growarray.
 * The info stored in both growarrays is the same: An array of fold_t.
 */
typedef struct
{
    linenr_t	fd_top;		/* first line of fold; for nested fold
				 * relative to parent */
    linenr_t	fd_len;		/* number of lines in the fold */
    garray_t	fd_nested;	/* array of nested folds */
    char	fd_flags;	/* see below */
    char	fd_small;	/* TRUE, FALSE or MAYBE: fold smaller than
				   'foldminlines'; MAYBE applies to nested
				   folds too */
} fold_t;

#define FD_OPEN		0	/* fold is open (nested ones can be closed) */
#define FD_CLOSED	1	/* fold is closed */
#define FD_LEVEL	2	/* depends on 'foldlevel' (nested folds too) */

#define MAX_LEVEL	20	/* maximum fold depth */

/* static functions {{{2 */
static int checkCloseRec __ARGS((garray_t *gap, linenr_t lnum, int level));
static void cloneFoldGrowArray __ARGS((garray_t *item, garray_t *result));
static int foldFind __ARGS((garray_t *gap, linenr_t lnum, fold_t **fpp));
static void checkupdate __ARGS((win_t *wp));
static void setFoldRepeat __ARGS((linenr_t lnum, long count, int open));
static linenr_t setManualFold __ARGS((linenr_t lnum, int opening, int recurse, int *donep));
static void foldOpenNested __ARGS((fold_t *fpr));
static void deleteFoldEntry __ARGS((garray_t *gap, int idx, int recursive));
static void deleteFoldRecurse __ARGS((garray_t *gap));
static void foldMarkAdjustRecurse __ARGS((garray_t *gap, linenr_t line1, linenr_t line2, long amount, long amount_after));
static int getDeepestNestingRecurse __ARGS((garray_t *gap));
static int check_closed __ARGS((win_t *win, fold_t *fp, int *use_levelp, int level, int *maybe_smallp, linenr_t lnum_off));
static void checkSmall __ARGS((win_t *wp, fold_t *fp, linenr_t lnum_off));
static void setSmallMaybe __ARGS((garray_t *gap));
static void foldCreateMarkers __ARGS((linenr_t start, linenr_t end));
static void foldAddMarker __ARGS((linenr_t lnum, char_u *marker, int markerlen));
static void deleteFoldMarkers __ARGS((fold_t *fp, int recursive, linenr_t lnum_off));
static void foldDelMarker __ARGS((linenr_t lnum, char_u *marker, int markerlen));
static void foldUpdateIEMS __ARGS((win_t *wp, linenr_t top, linenr_t bot));
static void parseMarker __ARGS((win_t *wp));

static char *e_nofold = N_("No fold found");

/*
 * While updating the folds lines between invalid_top and invalid_bot have an
 * undefined fold level.  Only used for the window currently being updated.
 */
static linenr_t invalid_top = (linenr_t)0;
static linenr_t invalid_bot = (linenr_t)0;

/* Flags used for "done" argument of setManualFold. */
#define DONE_NOTHING	0
#define DONE_ACTION	1	/* did close or open a fold */
#define DONE_FOLD	2	/* did find a fold */

static int foldstartmarkerlen;
static char_u *foldendmarker;
static int foldendmarkerlen;

/* Exported folding functions. {{{1 */
/* copyFoldingState() {{{2 */
#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * Copy that folding state from window "wp_from" to window "wp_to".
 */
    void
copyFoldingState(wp_from, wp_to)
    win_t	*wp_from;
    win_t	*wp_to;
{
    wp_to->w_fold_manual = wp_from->w_fold_manual;
    wp_to->w_foldinvalid = wp_from->w_foldinvalid;
    cloneFoldGrowArray(&wp_from->w_folds, &wp_to->w_folds);
}
#endif

/* hasAnyFolding() {{{2 */
/*
 * Return TRUE if there may be folded lines in the current window.
 */
    int
hasAnyFolding(win)
    win_t	*win;
{
    /* very simple now, but can become more complex later */
    return (win->w_p_fen
	    && (!foldmethodIsManual(win) || win->w_folds.ga_len > 0));
}

/* hasFolding() {{{2 */
/*
 * Return TRUE if line "lnum" in the current window is part of a closed
 * fold.
 * When returning TRUE, *firstp and *lastp are set to the first and last
 * lnum of the sequence of folded lines (skipped when NULL).
 * When "levelp" is not NULL and "cache" is FALSE, the level of the fold is
 * stored at it.  The level is zero if there is no closed fold.  The level is
 * negative if "lnum" is the first line of the fold.
 */
    int
hasFolding(lnum, firstp, lastp)
    linenr_t	lnum;
    linenr_t	*firstp;
    linenr_t	*lastp;
{
    return hasFoldingWin(curwin, lnum, firstp, lastp, TRUE, NULL);
}

/* hasFoldingWin() {{{2 */
    int
hasFoldingWin(win, lnum, firstp, lastp, cache, levelp)
    win_t	*win;
    linenr_t	lnum;
    linenr_t	*firstp;
    linenr_t	*lastp;
    int		cache;		/* when TRUE: use cached values of window */
    int		*levelp;	/* where to store fold level at "lnum" */
{
    int		had_folded = FALSE;
    linenr_t	first = 0;
    linenr_t	last = 0;
    linenr_t	lnum_rel = lnum;
    int		x;
    fold_t	*fp;
    int		level =  0;
    int		use_level = FALSE;
    int		maybe_small = FALSE;
    garray_t	*gap;

    checkupdate(win);
    /*
     * Return quickly when there is no folding at all in this window.
     */
    if (!hasAnyFolding(win))
    {
	if (levelp != NULL)
	    *levelp = 0;
	return FALSE;
    }

    if (cache)
    {
	/*
	 * First look in cached info for displayed lines.  This is probably
	 * the fastest, but it can only be used if the entry is still valid.
	 */
	x = find_wl_entry(win, lnum);
	if (x >= 0)
	{
	    first = win->w_lines[x].wl_lnum;
	    last = win->w_lines[x].wl_lastlnum;
	    had_folded = win->w_lines[x].wl_folded;
	}
    }

    if (first == 0)
    {
	/*
	 * Recursively search for a fold that contains "lnum".
	 */
	gap = &win->w_folds;
	for (;;)
	{
	    if (!foldFind(gap, lnum_rel, &fp))
		break;
	    first += fp->fd_top;
	    last += fp->fd_top;

	    /* is this fold closed? */
	    had_folded = check_closed(win, fp, &use_level, level,
					       &maybe_small, lnum - lnum_rel);
	    if (had_folded)
	    {
		/* Fold closed: Set last and quit loop. */
		last += fp->fd_len - 1;
		break;
	    }

	    /* Fold found, but it's open: Check nested folds.  Line number is
	     * relative to containing fold. */
	    gap = &fp->fd_nested;
	    lnum_rel -= fp->fd_top;
	    ++level;
	}
    }

    if (!had_folded)
    {
	if (levelp != NULL)
	{
	    if (lnum_rel == 0)		/* "lnum" is first line of open fold */
		*levelp = -level;
	    else
		*levelp = level;
	}
	return FALSE;
    }

    if (lastp != NULL)
	*lastp = last;
    if (firstp != NULL)
	*firstp = first;
    if (levelp != NULL)
    {
	if (lnum_rel == 0)
	    *levelp = -(level + 1);	/* open fold starts here too */
	else
	    *levelp = level + 1;
    }
    return TRUE;
}

/* foldLevel() {{{2 */
/*
 * Return fold level at line number "lnum" in the current window.
 */
    int
foldLevel(lnum)
    linenr_t	lnum;
{
    linenr_t	lnum_rel = lnum;
    fold_t	*fp;
    int		level =  0;
    garray_t	*gap;

    /* While updating the folds lines between invalid_top and invalid_bot have
     * an undefined fold level.  Otherwise update the folds first. */
    if (invalid_top == (linenr_t)0)
	checkupdate(curwin);
    else if (lnum >= invalid_top && lnum <= invalid_bot)
	return -1;

    /* Return quickly when there is no folding at all in this window. */
    if (!hasAnyFolding(curwin))
	return 0;

    /* Recursively search for a fold that contains "lnum". */
    gap = &curwin->w_folds;
    for (;;)
    {
	if (!foldFind(gap, lnum_rel, &fp))
	    break;
	/* Check nested folds.  Line number is relative to containing fold. */
	gap = &fp->fd_nested;
	lnum_rel -= fp->fd_top;
	++level;
    }

    return level;
}

/* lineFolded()	{{{2 */
/*
 * Low level function to check if a line is folded.  Doesn't use any caching.
 * Return TRUE if line is folded.
 * Return FALSE if line is not folded.
 * Return MAYBE if the line is folded when next to a folded line.
 */
    int
lineFolded(win, lnum)
    win_t	*win;
    linenr_t	lnum;
{
    return foldedCount(win, lnum, NULL) != 0;
}

/* foldedCount() {{{2 */
/*
 * Count the number of lines that are folded at line number "lnum".
 * Normally "lnum" is the first line of a possible fold, and the returned
 * number is the number of lines in the fold.
 * Doesn't use caching from the displayed window.
 * Returns number of folded lines from "lnum", or 0 if line is not folded.
 * When "levelp" is not NULL, sets *levelp to the fold level.  Negative if
 * it's at the start of the fold.
 */
    long
foldedCount(win, lnum, levelp)
    win_t	*win;
    linenr_t	lnum;
    int		*levelp;
{
    linenr_t	last;

    if (hasFoldingWin(win, lnum, NULL, &last, FALSE, levelp))
	return (long)(last - lnum + 1);
    return 0;
}

/* foldmethodIsManual() {{{2 */
/*
 * Return TRUE if 'foldmethod' is "manual"
 */
    int
foldmethodIsManual(wp)
    win_t	*wp;
{
    return (wp->w_p_fdm[3] == 'u');
}

/* foldmethodIsIndent() {{{2 */
/*
 * Return TRUE if 'foldmethod' is "indent"
 */
    int
foldmethodIsIndent(wp)
    win_t	*wp;
{
    return (wp->w_p_fdm[0] == 'i');
}

/* foldmethodIsExpr() {{{2 */
/*
 * Return TRUE if 'foldmethod' is "expr"
 */
    int
foldmethodIsExpr(wp)
    win_t	*wp;
{
    return (wp->w_p_fdm[1] == 'x');
}

/* foldmethodIsMarker() {{{2 */
/*
 * Return TRUE if 'foldmethod' is "marker"
 */
    int
foldmethodIsMarker(wp)
    win_t	*wp;
{
    return (wp->w_p_fdm[2] == 'r');
}

/* foldmethodIsSyntax() {{{2 */
/*
 * Return TRUE if 'foldmethod' is "syntax"
 */
    int
foldmethodIsSyntax(wp)
    win_t	*wp;
{
    return (wp->w_p_fdm[0] == 's');
}

/* closeFold() {{{2 */
/*
 * Close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    void
closeFold(lnum, count)
    linenr_t	lnum;
    long	count;
{
    setFoldRepeat(lnum, count, FALSE);
}

/* closeFoldRecurse() {{{2 */
/*
 * Close fold for current window at line "lnum" recursively.
 */
    void
closeFoldRecurse(lnum)
    linenr_t	lnum;
{
    (void)setManualFold(lnum, FALSE, TRUE, NULL);
}

/* opFoldRange() {{{2 */
/*
 * Open or Close folds for current window in lines "first" to "last".
 * Used for "zo", "zO", "zc" and "zC" in Visual mode.
 */
    void
opFoldRange(first, last, opening, recurse)
    linenr_t	first;
    linenr_t	last;
    int		opening;	/* TRUE to open, FALSE to close */
    int		recurse;	/* TRUE to do it recursively */
{
    int		done = DONE_NOTHING;	/* avoid error messages */
    linenr_t	lnum;
    linenr_t	lnum_next;

    for (lnum = first; lnum <= last; lnum = lnum_next + 1)
    {
	lnum_next = lnum;
	/* Opening one level only: next fold to open is after the one going to
	 * be opened. */
	if (opening && !recurse)
	    (void)hasFolding(lnum, NULL, &lnum_next);
	(void)setManualFold(lnum, opening, recurse, &done);
	/* Closing one level only: next line to close a fold is after just
	 * closed fold. */
	if (!opening && !recurse)
	    (void)hasFolding(lnum, NULL, &lnum_next);
    }
    if (done == DONE_NOTHING)
	EMSG(_(e_nofold));
}

/* openFold() {{{2 */
/*
 * Open fold for current window at line "lnum".
 * Repeat "count" times.
 */
    void
openFold(lnum, count)
    linenr_t	lnum;
    long	count;
{
    setFoldRepeat(lnum, count, TRUE);
}

/* openFoldRecurse() {{{2 */
/*
 * Open fold for current window at line "lnum" recursively.
 */
    void
openFoldRecurse(lnum)
    linenr_t	lnum;
{
    (void)setManualFold(lnum, TRUE, TRUE, NULL);
}

/* foldOpenCursor() {{{2 */
/*
 * Open folds until the cursor line is not in a closed fold.
 */
    void
foldOpenCursor()
{
    int		done;

    checkupdate(curwin);
    if (hasAnyFolding(curwin))
	for (;;)
	{
	    done = DONE_NOTHING;
	    (void)setManualFold(curwin->w_cursor.lnum, TRUE, FALSE, &done);
	    if (!(done & DONE_ACTION))
		break;
	}
}

/* newFoldLevel() {{{2 */
/*
 * Set new foldlevel for current window.
 */
    void
newFoldLevel()
{
    fold_t	*fp = (fold_t *)curwin->w_folds.ga_data;
    int		i;

    checkupdate(curwin);
    if (curwin->w_fold_manual)
    {
	/* Set all flags for the first level of folds to FD_LEVEL.  Following
	 * manual open/close will then change the flags to FD_OPEN or
	 * FD_CLOSED for those folds that don't use 'foldlevel'. */
	for (i = 0; i < curwin->w_folds.ga_len; ++i)
	    fp[i].fd_flags = FD_LEVEL;
	curwin->w_fold_manual = FALSE;
    }
    changed_window_setting();
}

/* foldCheckClose() {{{2 */
/*
 * Apply 'foldlevel' to all folds that don't contain the cursor.
 */
    void
foldCheckClose()
{
    if (*p_fcl != NUL)	/* can only be "all" right now */
    {
	checkupdate(curwin);
	if (checkCloseRec(&curwin->w_folds, curwin->w_cursor.lnum,
							(int)curwin->w_p_fdl))
	    changed_window_setting();
    }
}

/* checkCloseRec() {{{2 */
    static int
checkCloseRec(gap, lnum, level)
    garray_t	*gap;
    linenr_t	lnum;
    int		level;
{
    fold_t	*fp;
    int		retval = FALSE;
    int		i;

    fp = (fold_t *)gap->ga_data;
    for (i = 0; i < gap->ga_len; ++i)
    {
	/* Only manually opened folds may need to be closed. */
	if (fp[i].fd_flags == FD_OPEN)
	{
	    if (level <= 0 && (lnum < fp[i].fd_top
				      || lnum >= fp[i].fd_top + fp[i].fd_len))
	    {
		fp[i].fd_flags = FD_LEVEL;
		retval = TRUE;
	    }
	    else
		retval |= checkCloseRec(&fp[i].fd_nested, lnum - fp[i].fd_top,
								   level - 1);
	}
    }
    return retval;
}

/* foldCreate() {{{2 */
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window.
 */
    void
foldCreate(start, end)
    linenr_t	start;
    linenr_t	end;
{
    fold_t	*fp;
    garray_t	*gap;
    garray_t	fold_ga;
    int		i, j;
    int		cont;
    int		use_level = FALSE;
    int		closed = FALSE;
    int		level = 0;
    linenr_t	start_rel = start;
    linenr_t	end_rel = end;

    if (start > end)
    {
	/* reverse the range */
	end = start_rel;
	start = end_rel;
	start_rel = start;
	end_rel = end;
    }

    /* When 'foldmethod' is "marker" add markers, which creates the folds. */
    if (foldmethodIsMarker(curwin))
    {
	foldCreateMarkers(start, end);
	return;
    }

    checkupdate(curwin);

    /* Find the place to insert the new fold. */
    gap = &curwin->w_folds;
    for (;;)
    {
	if (!foldFind(gap, start_rel, &fp))
	    break;
	if (fp->fd_top + fp->fd_len > end_rel)
	{
	    /* New fold is completely inside this fold: Go one level deeper. */
	    gap = &fp->fd_nested;
	    start_rel -= fp->fd_top;
	    end_rel -= fp->fd_top;
	    if (use_level || fp->fd_flags == FD_LEVEL)
	    {
		use_level = TRUE;
		if (level >= curwin->w_p_fdl)
		    closed = TRUE;
	    }
	    else if (fp->fd_flags == FD_CLOSED)
		closed = TRUE;
	    ++level;
	}
	else
	{
	    /* This fold and new fold overlap: Insert here and move some folds
	     * inside the new fold. */
	    break;
	}
    }

    i = fp - (fold_t *)gap->ga_data;
    if (ga_grow(gap, 1) == OK)
    {
	fp = (fold_t *)gap->ga_data + i;
	ga_init2(&fold_ga, (int)sizeof(fold_t), 10);

	/* Count number of folds that will be contained in the new fold. */
	for (cont = 0; i + cont < gap->ga_len; ++cont)
	    if (fp[cont].fd_top > end_rel)
		break;
	if (cont > 0 && ga_grow(&fold_ga, cont) == OK)
	{
	    /* When last contained fold isn't completely contained, adjust end
	     * of new fold. */
	    if (end_rel < fp[cont - 1].fd_top + fp[cont - 1].fd_len - 1)
		end_rel = fp[cont - 1].fd_top + fp[cont - 1].fd_len - 1;
	    /* Move contained folds to inside new fold. */
	    mch_memmove(fold_ga.ga_data, fp, sizeof(fold_t) * cont);
	    fold_ga.ga_len += cont;
	    fold_ga.ga_room -= cont;
	    i += cont;

	    /* Adjust line numbers in contained folds to be relative to the
	     * new fold. */
	    for (j = 0; j < cont; ++j)
		((fold_t *)fold_ga.ga_data)[j].fd_top -= start_rel;
	}
	/* Move remaining entries to after the new fold. */
	if (i < gap->ga_len)
	    mch_memmove(fp + 1, (fold_t *)gap->ga_data + i,
				     sizeof(fold_t) * (gap->ga_len - i));
	gap->ga_len = gap->ga_len + 1 - cont;
	gap->ga_room = gap->ga_room - 1 + cont;

	/* insert new fold */
	fp->fd_nested = fold_ga;
	fp->fd_top = start_rel;
	fp->fd_len = end_rel - start_rel + 1;

	/* We want the new fold to be closed.  If it would remain open because
	 * of using 'foldlevel', need to adjust fd_flags of containing folds.
	 */
	if (use_level && !closed && level < curwin->w_p_fdl)
	    closeFold(start, 1L);
	if (!use_level)
	    curwin->w_fold_manual = TRUE;
	fp->fd_flags = FD_CLOSED;
	fp->fd_small = MAYBE;

	/* redraw */
	changed_window_setting();
    }
}

/* deleteFold() {{{2 */
/*
 * Delete a fold at line "start" in the current window.
 * When "end" is not 0, delete all folds from "start" to "end".
 * When "recursive" is TRUE delete recursively.
 */
    void
deleteFold(start, end, recursive)
    linenr_t	start;
    linenr_t	end;
    int		recursive;
{
    garray_t	*gap;
    fold_t	*fp;
    garray_t	*found_ga;
    fold_t	*found_fp = NULL;
    linenr_t	found_off = 0;
    int		use_level = FALSE;
    int		maybe_small = FALSE;
    int		level = 0;
    linenr_t	lnum = start;
    linenr_t	lnum_off;
    int		did_one = FALSE;
    linenr_t	first_lnum = MAXLNUM;
    linenr_t	last_lnum = 0;

    checkupdate(curwin);

    while (lnum <= end)
    {
	/* Find the deepest fold for "start". */
	gap = &curwin->w_folds;
	found_ga = NULL;
	lnum_off = 0;
	for (;;)
	{
	    if (!foldFind(gap, lnum - lnum_off, &fp))
		break;
	    /* lnum is inside this fold, remember info */
	    found_ga = gap;
	    found_fp = fp;
	    found_off = lnum_off;

	    /* if "lnum" is folded, don't check nesting */
	    if (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		break;

	    /* check nested folds */
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_top;
	    ++level;
	}
	if (found_ga == NULL)
	{
	    ++lnum;
	}
	else
	{
	    lnum = found_fp->fd_top + found_fp->fd_len + found_off;
	    did_one = TRUE;

	    if (foldmethodIsManual(curwin))
		deleteFoldEntry(found_ga,
		    (int)(found_fp - (fold_t *)found_ga->ga_data), recursive);
	    else
	    {
		if (found_fp->fd_top + found_off < first_lnum)
		    first_lnum = found_fp->fd_top;
		if (lnum > last_lnum)
		    last_lnum = lnum;
		parseMarker(curwin);
		deleteFoldMarkers(found_fp, recursive, found_off);
	    }

	    /* redraw window */
	    changed_window_setting();
	}
    }
    if (!did_one)
	EMSG(_(e_nofold));
    if (last_lnum > 0)
	changed_lines(first_lnum, (colnr_t)0, last_lnum, 0L);
}

/* clearFolding() {{{2 */
/*
 * Remove all folding for window "win".
 */
    void
clearFolding(win)
    win_t	*win;
{
    deleteFoldRecurse(&win->w_folds);
    win->w_foldinvalid = FALSE;
}

/* foldUpdate() {{{2 */
/*
 * Update folds for changes in the buffer of a window.
 * Note that inserted/deleted lines must have already been taken care of by
 * calling foldMarkAdjust().
 * The changes in lines from top to bot (inclusive).
 */
    void
foldUpdate(wp, top, bot)
    win_t	*wp;
    linenr_t	top;
    linenr_t	bot;
{
    fold_t	*fp;

    /* Mark all folds from top to bot as maybe-small. */
    (void)foldFind(&curwin->w_folds, curwin->w_cursor.lnum, &fp);
    while (fp < (fold_t *)curwin->w_folds.ga_data + curwin->w_folds.ga_len
	    && fp->fd_top < bot)
    {
	fp->fd_small = MAYBE;
	++fp;
    }

    if (foldmethodIsIndent(wp)
	    || foldmethodIsExpr(wp)
	    || foldmethodIsMarker(wp)
	    || foldmethodIsSyntax(wp))
	foldUpdateIEMS(wp, top, bot);
}

/* foldUpdateAll() {{{2 */
/*
 * Update all lines in the current window for folding.
 * Used when a fold setting changes or after reloading the buffer.
 * The actual updating is postponed until fold info is used, to avoid doing
 * every time a setting is changed or a syntax item is added.
 */
    void
foldUpdateAll(win)
    win_t	*win;
{
    win->w_foldinvalid = TRUE;
}

/* foldMoveTo() {{{2 */
/*
 * If "updown" is FALSE: Move to the start or end of the fold.
 * If "updown" is TRUE: move to fold at the same level.
 * If not moved return FAIL.
 */
    int
foldMoveTo(updown, dir, count)
    int		updown;
    int		dir;	    /* FORWARD or BACKWARD */
    long	count;
{
    long	n;
    int		retval = FAIL;
    linenr_t	lnum_off;
    linenr_t	lnum_found;
    linenr_t	lnum;
    int		use_level;
    int		maybe_small;
    garray_t	*gap;
    fold_t	*fp;
    int		level;
    int		last;

    /* Repeat "count" times. */
    for (n = 0; n < count; ++n)
    {
	/* Find nested folds.  Stop when a fold is closed.  The deepest fold
	 * that moves the cursor is used. */
	lnum_off = 0;
	gap = &curwin->w_folds;
	use_level = FALSE;
	maybe_small = FALSE;
	lnum_found = curwin->w_cursor.lnum;
	level = 0;
	last = FALSE;
	for (;;)
	{
	    if (!foldFind(gap, curwin->w_cursor.lnum - lnum_off, &fp))
	    {
		if (!updown)
		    break;

		/* When moving up, consider a fold above the cursor; when
		 * moving down consider a fold below the cursor. */
		if (dir == FORWARD)
		{
		    if (fp - (fold_t *)gap->ga_data >= gap->ga_len)
			break;
		    --fp;
		}
		else
		{
		    if (fp == (fold_t *)gap->ga_data)
			break;
		}
		/* don't look for contained folds, they will always move
		 * the cursor too far. */
		last = TRUE;
	    }

	    if (!last)
	    {
		/* Check if this fold is closed. */
		if (check_closed(curwin, fp, &use_level, level,
						      &maybe_small, lnum_off))
		    last = TRUE;

		/* "[z" and "]z" stop at closed fold */
		if (last && !updown)
		    break;
	    }

	    if (updown)
	    {
		if (dir == FORWARD)
		{
		    /* to start of next fold if there is one */
		    if (fp + 1 - (fold_t *)gap->ga_data < gap->ga_len)
		    {
			lnum = fp[1].fd_top + lnum_off;
			if (lnum > curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
		else
		{
		    /* to end of previous fold if there is one */
		    if (fp > (fold_t *)gap->ga_data)
		    {
			lnum = fp[-1].fd_top + lnum_off + fp[-1].fd_len - 1;
			if (lnum < curwin->w_cursor.lnum)
			    lnum_found = lnum;
		    }
		}
	    }
	    else
	    {
		/* Open fold found, set cursor to its start/end and then check
		 * nested folds. */
		if (dir == FORWARD)
		{
		    lnum = fp->fd_top + lnum_off + fp->fd_len - 1;
		    if (lnum > curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
		else
		{
		    lnum = fp->fd_top + lnum_off;
		    if (lnum < curwin->w_cursor.lnum)
			lnum_found = lnum;
		}
	    }

	    if (last)
		break;

	    /* Check nested folds (if any). */
	    gap = &fp->fd_nested;
	    lnum_off += fp->fd_top;
	    ++level;
	}
	if (lnum_found != curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.lnum = lnum_found;
	    curwin->w_cursor.col = 0;
	    retval = OK;
	}
	else
	    break;
    }

    return retval;
}

/* foldInitWin() {{{2 */
/*
 * Init the fold info in a new window.
 */
    void
foldInitWin(newwin)
    win_t	*newwin;
{
    ga_init2(&newwin->w_folds, (int)sizeof(fold_t), 10);
}

/* find_wl_entry() {{{2 */
/*
 * Find an entry in the win->w_lines[] array for buffer line "lnum".
 * Only valid entries are considered (for entries where wl_valid is FALSE the
 * line number can be wrong).
 * Returns index of entry or -1 if not found.
 */
    int
find_wl_entry(win, lnum)
    win_t	*win;
    linenr_t	lnum;
{
    int		i;

    if (win->w_lines_valid > 0)
	for (i = 0; i < win->w_lines_valid; ++i)
	    if (win->w_lines[i].wl_valid)
	    {
		if (lnum < win->w_lines[i].wl_lnum)
		    return -1;
		if (lnum <= win->w_lines[i].wl_lastlnum)
		    return i;
	    }
    return -1;
}

/* foldAdjustVisual() {{{2 */
#ifdef FEAT_VISUAL
/*
 * Adjust the Visual area to include any fold at the start or end completely.
 */
    void
foldAdjustVisual()
{
    pos_t	*start, *end;
    char_u	*ptr;

    if (!VIsual_active || !hasAnyFolding(curwin))
	return;

    if (ltoreq(VIsual, curwin->w_cursor))
    {
	start = &VIsual;
	end = &curwin->w_cursor;
    }
    else
    {
	start = &curwin->w_cursor;
	end = &VIsual;
    }
    if (hasFolding(start->lnum, &start->lnum, NULL))
	start->col = 0;
    if (hasFolding(end->lnum, NULL, &end->lnum))
    {
	ptr = ml_get(end->lnum);
	end->col = STRLEN(ptr);
	if (end->col > 0 && *p_sel == 'o')
	    --end->col;
#ifdef FEAT_MBYTE
	/* prevent cursor from moving on the trail byte */
	if (has_mbyte)
	    mb_adjust_cursor();
#endif
    }
}
#endif

/* cursor_foldstart() {{{2 */
/*
 * Move the cursor to the first line of a closed fold.
 */
    void
foldAdjustCursor()
{
    (void)hasFolding(curwin->w_cursor.lnum, &curwin->w_cursor.lnum, NULL);
}

/* Internal functions for "fold_t" {{{1 */
/* cloneFoldGrowArray() {{{2 */
/*
 * Will "clone" (i.e deep copy) a growarray_t of folds.
 *
 * Return FAIL if the operation cannot be completed, otherwise OK.
 */
    static void
cloneFoldGrowArray(from, to)
    garray_t	*from;
    garray_t	*to;
{
    int		i;
    fold_t	*from_p;
    fold_t	*to_p;

    ga_init2(to, from->ga_itemsize, from->ga_growsize);
    if (from->ga_len == 0 || ga_grow(to, from->ga_len) == FAIL)
	return;

    from_p = (fold_t *)from->ga_data;
    to_p = (fold_t *)to->ga_data;

    for (i = 0; i < from->ga_len; i++)
    {
	to_p->fd_top = from_p->fd_top;
	to_p->fd_len = from_p->fd_len;
	to_p->fd_flags = from_p->fd_flags;
	to_p->fd_small = from_p->fd_small;
	cloneFoldGrowArray(&from_p->fd_nested, &to_p->fd_nested);
	++to->ga_len;
	--to->ga_room;
	++from_p;
	++to_p;
    }
}

/* foldFind() {{{2 */
/*
 * Search for line "lnum" in folds of growarray "gap".
 * Set *fpp to the fold struct for the fold that contains "lnum" or
 * the first fold below it (careful: it can be beyond the end of the array!).
 * Returns FALSE when there is no fold that contains "lnum".
 */
    static int
foldFind(gap, lnum, fpp)
    garray_t	*gap;
    linenr_t	lnum;
    fold_t	**fpp;
{
    linenr_t	low, high;
    fold_t	*fp;
    int		i;

    /*
     * Perform a binary search.
     * "low" is lowest index of possible match.
     * "high" is highest index of possible match.
     */
    fp = (fold_t *)gap->ga_data;
    low = 0;
    high = gap->ga_len - 1;
    while (low <= high)
    {
	i = (low + high) / 2;
	if (fp[i].fd_top > lnum)
	    /* fold below lnum, adjust high */
	    high = i - 1;
	else if (fp[i].fd_top + fp[i].fd_len <= lnum)
	    /* fold above lnum, adjust low */
	    low = i + 1;
	else
	{
	    /* lnum is inside this fold */
	    *fpp = fp + i;
	    return TRUE;
	}
    }
    *fpp = fp + low;
    return FALSE;
}

/* checkupdate() {{{2 */
/*
 * Check if the folds in window "wp" are invalid and update them if needed.
 */
    static void
checkupdate(wp)
    win_t	*wp;
{
    if (wp->w_foldinvalid)
	foldUpdate(wp, (linenr_t)1, (linenr_t)MAXLNUM); /* will update all */
}

/* setFoldRepeat() {{{2 */
/*
 * Open or close fold for current window at line "lnum".
 * Repeat "count" times.
 */
    static void
setFoldRepeat(lnum, count, open)
    linenr_t	lnum;
    long	count;
    int		open;
{
    int		done;
    long	n;

    for (n = 0; n < count; ++n)
    {
	done = DONE_NOTHING;
	(void)setManualFold(lnum, open, FALSE, &done);
	if (!(done & DONE_ACTION))
	{
	    /* Only give an error message when no fold could be opened. */
	    if (n == 0 && !(done & DONE_FOLD))
		EMSG(_(e_nofold));
	    break;
	}
    }
}

/* setManualFold() {{{2 */
/*
 * Open or close the fold in the current window which contains "lnum".
 * "donep", when not NULL, points to flag that is set to DONE_FOLD when some
 * fold was found and to DONE_ACTION when some fold was opened or closed.
 * When "donep" is NULL give an error message when no fold was found for
 * "lnum".
 * Return the line number of the next line that could be closed.
 * It's only valid when "opening" is TRUE!
 */
    static linenr_t
setManualFold(lnum, opening, recurse, donep)
    linenr_t	lnum;
    int		opening;    /* TRUE when opening, FALSE when closing */
    int		recurse;    /* TRUE when closing/opening recursive */
    int		*donep;
{
    fold_t	*fp;
    fold_t	*fp2;
    fold_t	*found = NULL;
    int		j;
    int		level = 0;
    int		use_level = FALSE;
    int		found_fold = FALSE;
    garray_t	*gap;
    linenr_t	next = MAXLNUM;
    linenr_t	off = 0;

    checkupdate(curwin);

    /*
     * Find the fold, open or close it.
     */
    gap = &curwin->w_folds;
    for (;;)
    {
	if (!foldFind(gap, lnum, &fp))
	{
	    /* If there is a following fold, continue there next time. */
	    if (fp < (fold_t *)gap->ga_data + gap->ga_len)
		next = fp->fd_top + off;
	    break;
	}

	/* lnum is inside this fold */
	found_fold = TRUE;

	/* If there is a following fold, continue there next time. */
	if (fp + 1 < (fold_t *)gap->ga_data + gap->ga_len)
	    next = fp[1].fd_top + off;

	/* Change from level-dependent folding to manual. */
	if (use_level || fp->fd_flags == FD_LEVEL)
	{
	    use_level = TRUE;
	    if (level >= curwin->w_p_fdl)
		fp->fd_flags = FD_CLOSED;
	    else
		fp->fd_flags = FD_OPEN;
	    fp2 = (fold_t *)fp->fd_nested.ga_data;
	    for (j = 0; j < fp->fd_nested.ga_len; ++j)
		fp2[j].fd_flags = FD_LEVEL;
	}

	/* Simple case: Close recursively means closing the topmost fold. */
	if (!opening && recurse)
	{
	    if (fp->fd_flags != FD_CLOSED && donep != NULL)
		*donep |= DONE_ACTION;
	    fp->fd_flags = FD_CLOSED;
	    break;
	}

	if (fp->fd_flags == FD_CLOSED)
	{
	    /* When opening, open topmost closed fold. */
	    if (opening)
	    {
		fp->fd_flags = FD_OPEN;
		if (donep != NULL)
		    *donep |= DONE_ACTION;
		if (recurse)
		    foldOpenNested(fp);
	    }
	    break;
	}

	/* fold is open, check nested folds */
	found = fp;
	gap = &fp->fd_nested;
	lnum -= fp->fd_top;
	off += fp->fd_top;
	++level;
    }
    if (found_fold)
    {
	/* When closing and not recurse, close deepest open fold. */
	if (!opening && found != NULL)
	{
	    found->fd_flags = FD_CLOSED;
	    if (donep != NULL)
		*donep |= DONE_ACTION;
	}
	curwin->w_fold_manual = TRUE;
	changed_window_setting();
	if (donep != NULL)
	    *donep |= DONE_FOLD;
    }
    else if (donep == NULL)
	EMSG(_(e_nofold));

    return next;
}

/* foldOpenNested() {{{2 */
/*
 * Open all nested folds in fold "fpr" recursively.
 */
    static void
foldOpenNested(fpr)
    fold_t	*fpr;
{
    int		i;
    fold_t	*fp;

    fp = (fold_t *)fpr->fd_nested.ga_data;
    for (i = 0; i < fpr->fd_nested.ga_len; ++i)
    {
	foldOpenNested(&fp[i]);
	fp[i].fd_flags = FD_OPEN;
    }
}

/* deleteFoldEntry() {{{2 */
/*
 * Delete fold "idx" from growarray "gap".
 * When "recursive" is TRUE also delete all the folds contained in it.
 * When "recursive" is FALSE contained folds are moved one level up.
 */
    static void
deleteFoldEntry(gap, idx, recursive)
    garray_t	*gap;
    int		idx;
    int		recursive;
{
    fold_t	*fp;
    int		i;
    long	moved;
    fold_t	*nfp;

    fp = (fold_t *)gap->ga_data + idx;
    if (recursive || fp->fd_nested.ga_len == 0)
    {
	/* recursively delete the contained folds */
	deleteFoldRecurse(&fp->fd_nested);
	--gap->ga_len;
	++gap->ga_room;
	if (idx < gap->ga_len)
	    mch_memmove(fp, fp + 1, sizeof(fold_t) * (gap->ga_len - idx));
    }
    else
    {
	/* move nested folds one level up, to overwrite the fold that is
	 * deleted. */
	moved = fp->fd_nested.ga_len;
	if (ga_grow(gap, (int)(moved - 1)) == OK)
	{
	    /* adjust fd_top and fd_flags for the moved folds */
	    nfp = (fold_t *)fp->fd_nested.ga_data;
	    for (i = 0; i < moved; ++i)
	    {
		nfp[i].fd_top += fp->fd_top;
		if (fp->fd_flags == FD_LEVEL)
		    nfp[i].fd_flags = FD_LEVEL;
		if (fp->fd_small == MAYBE)
		    nfp[i].fd_small = MAYBE;
	    }

	    /* move the existing folds down to make room */
	    if (idx < gap->ga_len)
		mch_memmove(fp + moved, fp + 1,
					sizeof(fold_t) * (gap->ga_len - idx));
	    /* move the contained folds one level up */
	    mch_memmove(fp, nfp, (size_t)(sizeof(fold_t) * moved));
	    vim_free(nfp);
	    gap->ga_len += moved - 1;
	    gap->ga_room -= moved - 1;
	}
    }
}

/* deleteFoldRecurse() {{{2 */
/*
 * Delete nested folds in a fold.
 */
    static void
deleteFoldRecurse(gap)
    garray_t	*gap;
{
    int		i;

    for (i = 0; i < gap->ga_len; ++i)
	deleteFoldRecurse(&(((fold_t *)(gap->ga_data))[i].fd_nested));
    ga_clear(gap);
}

/* foldMarkAdjust() {{{2 */
/*
 * Update line numbers of folds for inserted/deleted lines.
 */
    void
foldMarkAdjust(wp, line1, line2, amount, amount_after)
    win_t	*wp;
    linenr_t	line1;
    linenr_t	line2;
    long	amount;
    long	amount_after;
{
    /* If deleting marks from line1 to line2, but not deleting all those
     * lines, set line2 so that only deleted lines have their folds removed. */
    if (amount == MAXLNUM && line2 >= line1 && line2 - line1 >= -amount_after)
	line2 = line1 - amount_after - 1;
    /* If appending a line in Insert mode, it should be included in the fold
     * just above the line. */
    if ((State & INSERT) && amount == (linenr_t)1 && line2 == MAXLNUM)
	--line1;
    foldMarkAdjustRecurse(&wp->w_folds, line1, line2, amount, amount_after);
}

/* foldMarkAdjustRecurse() {{{2 */
    static void
foldMarkAdjustRecurse(gap, line1, line2, amount, amount_after)
    garray_t	*gap;
    linenr_t	line1;
    linenr_t	line2;
    long	amount;
    long	amount_after;
{
    fold_t	*fp;
    int		i;
    linenr_t	last;
    linenr_t	top;

    /* In Insert mode an inserted line at the top of a fold is considered part
     * of the fold, otherwise it isn't. */
    if ((State & INSERT) && amount == (linenr_t)1 && line2 == MAXLNUM)
	top = line1 + 1;
    else
	top = line1;

    /* Find the fold containing or just below "line1". */
    (void)foldFind(gap, line1, &fp);

    /*
     * Adjust all folds below "line1" that are affected.
     */
    for (i = fp - (fold_t *)gap->ga_data; i < gap->ga_len; ++i, ++fp)
    {
	/*
	 * Check for these situations:
	 *        1  2  3
	 *        1  2  3
	 * line1     2  3  4  5
	 *           2  3  4  5
	 *           2  3  4  5
	 * line2     2  3  4  5
	 *              3     5  6
	 *              3     5  6
	 */

	last = fp->fd_top + fp->fd_len - 1; /* last line of fold */

	/* 1. fold completely above line1: nothing to do */
	if (last < line1)
	    continue;

	/* 6. fold below line2: only adjust for amount_after */
	if (fp->fd_top > line2)
	{
	    if (amount_after == 0)
		break;
	    fp->fd_top += amount_after;
	}
	else
	{
	    if (fp->fd_top >= top && last <= line2)
	    {
		/* 4. fold completely contained in range */
		if (amount == MAXLNUM)
		{
		    /* Deleting lines: delete the fold completely */
		    deleteFoldEntry(gap, i, TRUE);
		    --i;    /* adjust index for deletion */
		    --fp;
		}
		else
		    fp->fd_top += amount;
	    }
	    else
	    {
		/* 2, 3, or 5: need to correct nested folds too */
		foldMarkAdjustRecurse(&fp->fd_nested, line1 - fp->fd_top,
				  line2 - fp->fd_top, amount, amount_after);
		if (fp->fd_top < top)
		{
		    if (last <= line2)
		    {
			/* 2. fold contains line1, line2 is below fold */
			if (amount == MAXLNUM)
			    fp->fd_len = line1 - fp->fd_top;
			else
			    fp->fd_len += amount;
		    }
		    else
		    {
			/* 3. fold contains line1 and line2 */
			fp->fd_len += amount_after;
		    }
		}
		else
		{
		    /* 5. fold is below line1 and contains line2 */
		    if (amount == MAXLNUM)
		    {
			fp->fd_len -= line2 - fp->fd_top + 1;
			fp->fd_top = line1;
		    }
		    else
		    {
			fp->fd_len += amount_after - amount;
			fp->fd_top += amount;
		    }
		}
	    }
	}
    }
}

/* getDeepestNesting() {{{2 */
/*
 * Get the lowest 'foldlevel' value that makes the deepest nested fold in the
 * current window open.
 */
    int
getDeepestNesting()
{
    checkupdate(curwin);
    return getDeepestNestingRecurse(&curwin->w_folds);
}

    static int
getDeepestNestingRecurse(gap)
    garray_t	*gap;
{
    int		i;
    int		level;
    int		maxlevel = 0;
    fold_t	*fp;

    fp = (fold_t *)gap->ga_data;
    for (i = 0; i < gap->ga_len; ++i)
    {
	level = getDeepestNestingRecurse(&fp[i].fd_nested) + 1;
	if (level > maxlevel)
	    maxlevel = level;
    }

    return maxlevel;
}

/* check_closed() {{{2 */
/*
 * Check if a fold is closed and update the info needed to check nested folds.
 */
    static int
check_closed(win, fp, use_levelp, level, maybe_smallp, lnum_off)
    win_t	*win;
    fold_t	*fp;
    int		*use_levelp;	    /* TRUE: outer fold had FD_LEVEL */
    int		level;		    /* folding depth */
    int		*maybe_smallp;	    /* TRUE: outer this had fd_small == MAYBE */
    linenr_t	lnum_off;	    /* line number offset for fp->fd_top */
{
    int		closed = FALSE;

    /* Check if this fold is closed.  If the flag is FD_LEVEL this
     * fold and all folds it contains depend on 'foldlevel'. */
    if (*use_levelp || fp->fd_flags == FD_LEVEL)
    {
	*use_levelp = TRUE;
	if (level >= win->w_p_fdl)
	    closed = TRUE;
    }
    else if (fp->fd_flags == FD_CLOSED)
	closed = TRUE;

    /* Small fold isn't closed anyway. */
    if (fp->fd_small == MAYBE)
	*maybe_smallp = TRUE;
    if (closed)
    {
	if (*maybe_smallp)
	    fp->fd_small = MAYBE;
	checkSmall(win, fp, lnum_off);
	if (fp->fd_small == TRUE)
	    closed = FALSE;
    }
    return closed;
}

/* checkSmall() {{{2 */
/*
 * Update fd_small field of fold "fp".
 */
    static void
checkSmall(wp, fp, lnum_off)
    win_t	*wp;
    fold_t	*fp;
    linenr_t	lnum_off;	/* offset for fp->fd_top */
{
    int		count;
    int		n;

    if (fp->fd_small == MAYBE)
    {
	/* Mark any nested folds to maybe-small */
	setSmallMaybe(&fp->fd_nested);

	if (fp->fd_len > curwin->w_p_fml)
	    fp->fd_small = FALSE;
	else
	{
	    count = 0;
	    for (n = 0; n < fp->fd_len; ++n)
	    {
		count += plines_win_nofold(wp, fp->fd_top + lnum_off + n);
		if (count > curwin->w_p_fml)
		{
		    fp->fd_small = FALSE;
		    return;
		}
	    }
	    fp->fd_small = TRUE;
	}
    }
}

/* setSmallMaybe() {{{2 */
/*
 * Set small flags in "gap" to MAYBE.
 */
    static void
setSmallMaybe(gap)
    garray_t	*gap;
{
    int		i;
    fold_t	*fp;

    fp = (fold_t *)gap->ga_data;
    for (i = 0; i < gap->ga_len; ++i)
	fp[i].fd_small = MAYBE;
}

/* foldCreateMarkers() {{{2 */
/*
 * Create a fold from line "start" to line "end" (inclusive) in the current
 * window by adding markers.
 */
    static void
foldCreateMarkers(start, end)
    linenr_t	start;
    linenr_t	end;
{
    if (!curbuf->b_p_ma)
    {
	EMSG(_(e_modifiable));
	return;
    }
    if (*curbuf->b_p_cms == NUL)
    {
	EMSG(_("'commentstring' is empty"));
	return;
    }
    parseMarker(curwin);

    foldAddMarker(start, curwin->w_p_fmr, foldstartmarkerlen);
    foldAddMarker(end, foldendmarker, foldendmarkerlen);

    /* Update both changes here, to avoid all folds after the start are
     * changed when the start marker is inserted and the end isn't. */
    changed_lines(start, (colnr_t)0, end, 0L);
}

/* foldAddMarker() {{{2 */
/*
 * Add "marker[markerlen]" in 'commentstring' to line "lnum".
 */
    static void
foldAddMarker(lnum, marker, markerlen)
    linenr_t	lnum;
    char_u	*marker;
    int		markerlen;
{
    char_u	*cms = curbuf->b_p_cms;
    char_u	*line;
    int		line_len;
    char_u	*newline;
    char_u	*p = (char_u *)strstr((char *)curbuf->b_p_cms, "%s");

    /* Allocate a new line: old-line + 'cms'-start + marker + 'cms'-end */
    line = ml_get(lnum);
    line_len = STRLEN(line);
    newline = alloc((unsigned)(line_len + markerlen + STRLEN(cms) - 1));
    if (newline == NULL)
	return;

    if (u_save(lnum - 1, lnum + 1) == OK)
    {
	STRCPY(newline, line);
	STRCPY(newline + line_len, cms);
	STRNCPY(newline + line_len + (p - cms), marker, markerlen);
	STRCPY(newline + line_len + (p - cms) + markerlen, p + 2);

	ml_replace(lnum, newline, FALSE);
    }
}

/* deleteFoldMarkers() {{{2 */
/*
 * Delete the markers for a fold, causing it to be deleted.
 */
    static void
deleteFoldMarkers(fp, recursive, lnum_off)
    fold_t	*fp;
    int		recursive;
    linenr_t	lnum_off;	/* offset for fp->fd_top */
{
    int		i;

    if (recursive)
	for (i = 0; i < fp->fd_nested.ga_len; ++i)
	    deleteFoldMarkers((fold_t *)fp->fd_nested.ga_data + i, TRUE,
						       lnum_off + fp->fd_top);
    foldDelMarker(fp->fd_top + lnum_off, curwin->w_p_fmr, foldstartmarkerlen);
    foldDelMarker(fp->fd_top + lnum_off + fp->fd_len - 1,
					     foldendmarker, foldendmarkerlen);
}

/* foldDelMarker() {{{2 */
/*
 * Delete marker "marker[markerlen]" at the end of line "lnum".
 * Delete 'commentstring' if it matches.
 * If the marker is not found, there is no error message.  Could a missing
 * close-marker.
 */
    static void
foldDelMarker(lnum, marker, markerlen)
    linenr_t	lnum;
    char_u	*marker;
    int		markerlen;
{
    char_u	*line;
    char_u	*newline;
    char_u	*p;
    int		len;
    char_u	*cms = curbuf->b_p_cms;
    char_u	*cms2;

    line = ml_get(lnum);
    for (p = line; *p != NUL; ++p)
	if (STRNCMP(p, marker, markerlen) == 0)
	{
	    /* Found the marker, include a digit if it's there. */
	    len = markerlen;
	    if (isdigit(p[len]))
		++len;
	    if (*cms != NUL)
	    {
		/* Also delete 'commentstring' if it matches. */
		cms2 = (char_u *)strstr((char *)cms, "%s");
		if (p - line >= cms2 - cms
			&& STRNCMP(p - (cms2 - cms), cms, cms2 - cms) == 0
			&& STRNCMP(p + len, cms2 + 2, STRLEN(cms2 + 2)) == 0)
		{
		    p -= cms2 - cms;
		    len += STRLEN(cms) - 2;
		}
	    }
	    if (u_save(lnum - 1, lnum + 1) == OK)
	    {
		/* Make new line: text-before-marker + text-after-marker */
		newline = alloc((unsigned)(STRLEN(line) - len + 1));
		if (newline != NULL)
		{
		    STRNCPY(newline, line, p - line);
		    STRCPY(newline + (p - line), p + len);
		    ml_replace(lnum, newline, FALSE);
		}
	    }
	    break;
	}
}

/* Folding by indent, expr, marker and syntax. {{{1 */
/* Define "fline_t", passed to get fold level for a line. {{{2 */
typedef struct
{
    win_t	*wp;		/* window */
    linenr_t	lnum;		/* current line number */
    linenr_t	off;		/* offset between lnum and real line number */
    linenr_t	lnum_save;	/* line nr used by foldUpdateIEMSRecurse() */
    int		lvl;		/* current level (-1 for undefined) */
    int		lvl_next;	/* level used for next line */
    int		start;		/* number of folds that are forced to start at
				   this line. */
    int		end;		/* level of fold that is forced to end below
				   this line */
    int		had_end;	/* level of fold that is forced to end above
				   this line (copy of "end" of prev. line) */
} fline_t;

/* Flag is set when redrawing is needed. */
static int fold_changed;

/* Function declarations. {{{2 */
static linenr_t foldUpdateIEMSRecurse __ARGS((garray_t *gap, int level, linenr_t startlnum, fline_t *flp, void (*getlevel)__ARGS((fline_t *)), linenr_t bot, int topflags));
static int foldInsert __ARGS((garray_t *gap, int i));
static void foldSplit __ARGS((garray_t *gap, int i, linenr_t top, linenr_t bot));
static void foldRemove __ARGS((garray_t *gap, linenr_t top, linenr_t bot));
static void foldMerge __ARGS((fold_t *fp1, garray_t *gap, fold_t *fp2));
static void foldlevelIndent __ARGS((fline_t *flp));
static void foldlevelExpr __ARGS((fline_t *flp));
static void foldlevelMarker __ARGS((fline_t *flp));
static void foldlevelSyntax __ARGS((fline_t *flp));

/* foldUpdateIEMS() {{{2 */
/*
 * Update the folding for window "wp", at least from lines "top" to "bot".
 * Return TRUE if any folds did change.
 */
    static void
foldUpdateIEMS(wp, top, bot)
    win_t	*wp;
    linenr_t	top;
    linenr_t	bot;
{
    linenr_t	lnum;
    linenr_t	start;
    linenr_t	end;
    fline_t	fline;
    void	(*getlevel)__ARGS((fline_t *));
    int		level;
    garray_t	*gap;
    fold_t	*fp;

    /* Avoid problems when being called recursively. */
    if (invalid_top != (linenr_t)0)
	return;

    if (wp->w_foldinvalid)
    {
	/* Need to update all folds. */
	top = 1;
	bot = wp->w_buffer->b_ml.ml_line_count;
	wp->w_foldinvalid = FALSE;

	/* Mark all folds a maybe-small. */
	setSmallMaybe(&wp->w_folds);
    }

    /* When deleting lines at the end of the buffer "top" can be past the end
     * of the buffer. */
    if (top > wp->w_buffer->b_ml.ml_line_count)
	top = wp->w_buffer->b_ml.ml_line_count;

    fold_changed = FALSE;
    fline.wp = wp;
    fline.off = 0;
    fline.lvl = 0;
    fline.lvl_next = -1;
    fline.start = 0;
    fline.end = MAX_LEVEL + 1;
    fline.had_end = MAX_LEVEL + 1;

    invalid_top = top;
    invalid_bot = bot;

    if (foldmethodIsMarker(wp))
    {
	getlevel = foldlevelMarker;

	/* Init marker variables to speed up foldlevelMarker(). */
	parseMarker(wp);

	/* Need to get the level of the line above top, it is used if there is
	 * no marker at the top. */
	if (top > 1)
	{
	    /*
	     * Recursively search to get the fold level at top - 1.
	     */
	    gap = &wp->w_folds;
	    lnum = top - 1;
	    level = 0;
	    for (;;)
	    {
		if (!foldFind(gap, lnum, &fp))
		    break;
		gap = &fp->fd_nested;
		lnum -= fp->fd_top;	/* lnum is relative to start of fold */
		++level;
	    }
	    /* The fold may end just above the top, check for that. */
	    fline.lnum = top - 1;
	    fline.lvl = level;
	    getlevel(&fline);
	    /* If a fold started here, we already had the level, if it stops
	     * here, we need to use lvl_next. */
	    if (fline.lvl > level)
		fline.lvl = level;
	    else
		fline.lvl = fline.lvl_next;
	}
	fline.lnum = top;
	getlevel(&fline);
    }
    else
    {
	fline.lnum = top;
	if (foldmethodIsExpr(wp))
	{
	    getlevel = foldlevelExpr;
	    /* start one line back, because a "<1" may indicate the end of a
	     * fold in the topline */
	    if (top > 1)
		--fline.lnum;
	}
	else if (foldmethodIsSyntax(wp))
	    getlevel = foldlevelSyntax;
	else
	    getlevel = foldlevelIndent;

	/* Backup to a line for which the fold level is defined.  Since it's
	 * always defined for line one, we will stop there. */
	fline.lvl = -1;
	for ( ; !got_int; --fline.lnum)
	{
	    /* Reset lvl_next each time, because it will be set to a value for
	     * the next line, but we search backwards here. */
	    fline.lvl_next = -1;
	    getlevel(&fline);
	    if (fline.lvl >= 0)
		break;
	}
    }

    start = fline.lnum;
    end = bot;
    /* Do at least one line. */
    if (start > end && end < wp->w_buffer->b_ml.ml_line_count)
	end = start;
    while (!got_int)
    {
	if (fline.lnum > end)
	{
	    /* For "marker", "expr"  and "syntax"  methods: If a change caused
	     * a fold to be removed, we need to continue at least until where
	     * it ended. */
	    if (getlevel != foldlevelMarker
		    && getlevel != foldlevelSyntax
		    && getlevel != foldlevelExpr)
		break;
	    if ((start <= end
			&& foldFind(&wp->w_folds, end, &fp)
			&& fp->fd_top + fp->fd_len - 1 > end)
		    || (fline.lvl == 0
			&& foldFind(&wp->w_folds, fline.lnum, &fp)
			&& fp->fd_top < fline.lnum))
		end = fp->fd_top + fp->fd_len - 1;
	    else
		break;
	}

	/* A level 1 fold starts at a line with foldlevel > 0. */
	if (fline.lvl > 0)
	{
	    invalid_top = fline.lnum;
	    invalid_bot = end;
	    end = foldUpdateIEMSRecurse(&wp->w_folds,
				   1, start, &fline, getlevel, end, FD_LEVEL);
	    start = fline.lnum;
	}
	else
	{
	    if (fline.lnum == wp->w_buffer->b_ml.ml_line_count)
		break;
	    ++fline.lnum;
	    fline.lvl = fline.lvl_next;
	    getlevel(&fline);
	}
    }

    /* There can't be any folds from start until end now. */
    foldRemove(&wp->w_folds, start, end);

    /* If some fold changed, need to redraw and position cursor. */
    if (fold_changed && wp->w_p_fen)
	changed_window_setting();

    /* If we updated folds past "bot", need to redraw more lines.  Don't do
     * this in other situations, the changed lines will be redrawn anyway and
     * this method can cause the whole window to be updated. */
    if (end != bot)
    {
	if (wp->w_redraw_top == 0 || wp->w_redraw_top > top)
	    wp->w_redraw_top = top;
	if (wp->w_redraw_bot < end)
	    wp->w_redraw_bot = end;
    }

    invalid_top = (linenr_t)0;
}

/* foldUpdateIEMSRecurse() {{{2 */
/*
 * Update a fold that starts at "flp->lnum".  At this line there is always a
 * valid foldlevel, and its level >= "level".
 * "flp" is valid for "flp->lnum" when called and it's valid when returning.
 * "flp->lnum" is set to the lnum just below the fold, if it ends before
 * "bot", it's "bot" plus one if the fold continues and it's bigger when using
 * the marker method and a text change made following folds to change.
 * When returning, "flp->lnum_save" is the line number that was used to get
 * the level when the level at "flp->lnum" is invalid.
 * Remove any folds from "startlnum" up to here at this level.
 * Recursively update nested folds.
 * Below line "bot" there are no changes in the text.
 * "flp->lnum", "flp->lnum_save" and "bot" are relative to the start of the
 * outer fold.
 * "flp->off" is the offset to the real line number in the buffer.
 *
 * All this would be a lot simpler if all folds in the range would be deleted
 * and then created again.  But we would loose all information about the
 * folds, even when making changes that don't affect the folding (e.g. "vj~").
 *
 * Returns bot, which may have been increased for lines that also need to be
 * updated as a result of a detected change in the fold.
 */
    static linenr_t
foldUpdateIEMSRecurse(gap, level, startlnum, flp, getlevel, bot, topflags)
    garray_t	*gap;
    int		level;
    linenr_t	startlnum;
    fline_t	*flp;
    void	(*getlevel)__ARGS((fline_t *));
    linenr_t	bot;
    int		topflags;	/* flags used by containing fold */
{
    linenr_t	ll;
    fold_t	*fp = NULL;
    fold_t	*fp2;
    int		lvl = level;
    linenr_t	startlnum2 = startlnum;
    linenr_t	firstlnum = flp->lnum;	/* first lnum we got */
    int		i;
    int		finish = FALSE;
    linenr_t	linecount = flp->wp->w_buffer->b_ml.ml_line_count - flp->off;
    int		concat;

    /*
     * If using the marker method, the start line is not the start of a fold
     * and the level is non-zero, we must use the previous fold.
     */
    if (getlevel == foldlevelMarker && flp->start == 0 && flp->lvl > 0)
	foldFind(gap, startlnum - 1, &fp);

    /*
     * Loop over all lines in this fold, or until "bot" is hit.
     * Handle nested folds inside of this fold.
     * "flp->lnum" is the current line.  When finding the end of the fold, it
     * is just below the end of the fold.
     * "*flp" contains the level of the line "flp->lnum" or a following one if
     * there are lines with an invalid fold level.  "flp->lnum_save" is the
     * line number that was used to get the fold level (below "flp->lnum" when
     * it has an invalid fold level).  When called the fold level is always
     * valid, thus "flp->lnum_save" is equal to "flp->lnum".
     */
    flp->lnum_save = flp->lnum;
    while (!got_int)
    {
	/* Updating folds can be slow, check for CTRL-C. */
	line_breakcheck();

	/* Set "lvl" to the level of line "flp->lnum".  When flp->start is set
	 * and after the first line of the fold, set the level to zero to
	 * force the fold to end.  Do the same when had_end is set: Previous
	 * line was marked as end of a fold. */
	lvl = flp->lvl;
	if (lvl > MAX_LEVEL)
	    lvl = MAX_LEVEL;
	if (flp->lnum > firstlnum
		&& (level > lvl - flp->start || level >= flp->had_end))
	    lvl = 0;

	if (flp->lnum > bot && !finish)
	{
	    /* For "marker" and "syntax" methods:
	     * - If a change caused a nested fold to be removed, we need to
	     *   delete it and continue at least until where it ended.
	     * - If a change caused a nested fold to be created, or this fold
	     *   to continue below its original end, need to finish this fold.
	     */
	    if (getlevel != foldlevelMarker
		    && getlevel != foldlevelExpr
		    && getlevel != foldlevelSyntax)
		break;
	    if (lvl == level
		    && foldFind(&fp->fd_nested, flp->lnum - fp->fd_top, &fp2))
		bot = fp2->fd_top + fp2->fd_len - 1 + fp->fd_top;
	    else if (fp->fd_top + fp->fd_len <= flp->lnum && lvl >= level)
		finish = TRUE;
	    else
		break;
	}

	/* At the start of the first nested fold and at the end of the current
	 * fold: check if existing folds at this level, before the current
	 * one, need to be deleted or truncated. */
	if (fp == NULL
		&& (lvl != level
		    || flp->lnum_save >= bot
		    || flp->start != 0
		    || flp->had_end <= MAX_LEVEL
		    || flp->lnum == linecount))
	{
	    /*
	     * Remove or update folds that have lines between startlnum and
	     * firstlnum.
	     */
	    while (!got_int)
	    {
		/* set concat to 1 if it's allowed to concatenated this fold
		 * with a previous one that touches it. */
		if (flp->start != 0 || flp->had_end <= MAX_LEVEL)
		    concat = 0;
		else
		    concat = 1;

		/* Find an existing fold to re-use.  Preferably one that
		 * includes startlnum, otherwise one that ends just before
		 * startlnum or starts after it. */
		if (foldFind(gap, startlnum, &fp)
			|| (fp < ((fold_t *)gap->ga_data) + gap->ga_len
			    && fp->fd_top <= firstlnum)
			|| foldFind(gap, firstlnum - concat, &fp)
			|| (fp < ((fold_t *)gap->ga_data) + gap->ga_len
			    && ((lvl < level && fp->fd_top < flp->lnum)
				|| (lvl >= level
					   && fp->fd_top <= flp->lnum_save))))
		{
		    if (fp->fd_top + fp->fd_len + concat > firstlnum)
		    {
			/* Use existing fold for the new fold.  If it starts
			 * before where we started looking, extend it.  If it
			 * starts at another line, update nested folds to keep
			 * their position, compensating for the new fd_top. */
			if (fp->fd_top >= startlnum && fp->fd_top != firstlnum)
			{
			    if (fp->fd_top > firstlnum)
				/* like lines are inserted */
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_t)0, (linenr_t)MAXLNUM,
					(long)(fp->fd_top - firstlnum), 0L);
			    else
				/* like lines are deleted */
				foldMarkAdjustRecurse(&fp->fd_nested,
					(linenr_t)0,
					(long)(firstlnum - fp->fd_top - 1),
					(linenr_t)MAXLNUM,
					(long)(fp->fd_top - firstlnum));
			    fp->fd_len += fp->fd_top - firstlnum;
			    fp->fd_top = firstlnum;
			    fold_changed = TRUE;
			}
			else if (flp->start != 0 && lvl == level
						   && fp->fd_top != firstlnum)
			{
			    /* Existing fold that includes startlnum must stop
			     * if we find the start of a new fold at the same
			     * level.  Split it.  Delete contained folds at
			     * this point to split them too. */
			    foldRemove(&fp->fd_nested, flp->lnum - fp->fd_top,
						      flp->lnum - fp->fd_top);
			    i = fp - (fold_t *)gap->ga_data;
			    foldSplit(gap, i, flp->lnum, flp->lnum - 1);
			    fp = (fold_t *)gap->ga_data + i + 1;
			    /* If using the "marker" or "syntax" method, we
			     * need to continue until the end of the fold is
			     * found. */
			    if (getlevel == foldlevelMarker
				    || getlevel == foldlevelExpr
				    || getlevel == foldlevelSyntax)
				finish = TRUE;
			}
			break;
		    }
		    if (fp->fd_top >= startlnum)
		    {
			/* A fold that starts at or after startlnum and stops
			 * before the new fold must be deleted.  Continue
			 * looking for the next one. */
			deleteFoldEntry(gap,
				     (int)(fp - (fold_t *)gap->ga_data), TRUE);
		    }
		    else
		    {
			/* A fold has some lines above startlnum, truncate it
			 * to stop just above startlnum.  */
			fp->fd_len = startlnum - fp->fd_top;
			foldMarkAdjustRecurse(&fp->fd_nested,
				(linenr_t)fp->fd_len, (linenr_t)MAXLNUM,
						       (linenr_t)MAXLNUM, 0L);
			fold_changed = TRUE;
		    }
		}
		else
		{
		    /* Insert new fold.  Careful: ga_data may be NULL and it
		     * may change! */
		    i = fp - (fold_t *)gap->ga_data;
		    if (foldInsert(gap, i) != OK)
			return bot;
		    fp = (fold_t *)gap->ga_data + i;
		    /* The new fold continues until bot, unless we find the
		     * end earlier. */
		    fp->fd_top = firstlnum;
		    fp->fd_len = bot - firstlnum + 1;
		    /* When the containing fold is open, the new fold is open.
		     * The new fold is closed if the fold above it is closed.
		     * The first fold depends on the containing fold. */
		    if (topflags == FD_OPEN)
		    {
			flp->wp->w_fold_manual = TRUE;
			fp->fd_flags = FD_OPEN;
		    }
		    else if (i <= 0)
		    {
			fp->fd_flags = topflags;
			if (topflags != FD_LEVEL)
			    flp->wp->w_fold_manual = TRUE;
		    }
		    else
			fp->fd_flags = (fp - 1)->fd_flags;
		    /* If using the "marker", "expr" or "syntax" method, we
		     * need to continue until the end of the fold is found. */
		    if (getlevel == foldlevelMarker
			    || getlevel == foldlevelExpr
			    || getlevel == foldlevelSyntax)
			finish = TRUE;
		    fold_changed = TRUE;
		    break;
		}
	    }
	}

	if (lvl < level || flp->lnum > linecount)
	{
	    /*
	     * Found a line with a lower foldlevel, this fold ends just above
	     * "flp->lnum".
	     */
	    break;
	}

	/*
	 * The fold includes the line "flp->lnum" and "flp->lnum_save".
	 */
	if (lvl > level)
	{
	    /*
	     * There is a nested fold, handle it recursively.
	     */
	    /* At least do one line (can happen when finish is TRUE). */
	    if (bot < flp->lnum)
		bot = flp->lnum;

	    /* Line numbers in the nested fold are relative to the start of
	     * this fold. */
	    flp->lnum = flp->lnum_save - fp->fd_top;
	    flp->off += fp->fd_top;
	    i = fp - (fold_t *)gap->ga_data;
	    bot = foldUpdateIEMSRecurse(&fp->fd_nested, level + 1,
				       startlnum2 - fp->fd_top, flp, getlevel,
					      bot - fp->fd_top, fp->fd_flags);
	    fp = (fold_t *)gap->ga_data + i;
	    flp->lnum += fp->fd_top;
	    flp->lnum_save += fp->fd_top;
	    flp->off -= fp->fd_top;
	    bot += fp->fd_top;
	    startlnum2 = flp->lnum;

	    /* This fold may end at the same line, don't incr. flp->lnum. */
	}
	else
	{
	    /*
	     * Get the level of the next line, then continue the loop to check
	     * if it ends there.
	     * Skip over undefined lines, to find the foldlevel after it.
	     * For the last line in the file the foldlevel is always valid.
	     */
	    flp->lnum = flp->lnum_save;
	    ll = flp->lnum + 1;
	    while (!got_int)
	    {
		if (++flp->lnum > linecount)
		    break;
		flp->lvl = flp->lvl_next;
		getlevel(flp);
		if (flp->lvl >= 0 || flp->had_end <= MAX_LEVEL)
		    break;
	    }
	    if (flp->lnum > linecount)
		break;

	    /* leave flp->lnum_save to lnum of the line that was used to get
	     * the level, flp->lnum to the lnum of the next line. */
	    flp->lnum_save = flp->lnum;
	    flp->lnum = ll;
	}
    }

    if (fp == NULL)	/* only happens when got_int is set */
	return bot;

    /*
     * Get here when:
     * lvl < level: the folds ends just above "flp->lnum"
     * lvl >= level: fold continues below "bot"
     */

    /* Current fold at least extends until lnum. */
    if (fp->fd_len < flp->lnum - fp->fd_top)
    {
	fp->fd_len = flp->lnum - fp->fd_top;
	fold_changed = TRUE;
    }

    /* Delete contained folds from end of last one found until bot. */
    foldRemove(&fp->fd_nested, startlnum2 - fp->fd_top, bot - fp->fd_top);

    if (lvl < level)
    {
	/* End of fold found, update the length when it got shorter. */
	if (fp->fd_len != flp->lnum - fp->fd_top)
	{
	    if (fp->fd_top + fp->fd_len > bot + 1)
	    {
		/* fold coninued below bot */
		if (getlevel == foldlevelMarker
			|| getlevel == foldlevelExpr
			|| getlevel == foldlevelSyntax)
		{
		    /* marker method: truncate the fold and make sure the
		     * previously included lines are processed again */
		    bot = fp->fd_top + fp->fd_len - 1;
		    fp->fd_len = flp->lnum - fp->fd_top;
		}
		else
		{
		    /* indent or expr method: split fold to create a new one
		     * below bot */
		    i = fp - (fold_t *)gap->ga_data;
		    foldSplit(gap, i, flp->lnum, bot);
		    fp = (fold_t *)gap->ga_data + i;
		}
	    }
	    else
		fp->fd_len = flp->lnum - fp->fd_top;
	    fold_changed = TRUE;
	}
    }

    /* delete following folds that end before the current line */
    for (;;)
    {
	fp2 = fp + 1;
	if (fp2 >= (fold_t *)gap->ga_data + gap->ga_len
						  || fp2->fd_top > flp->lnum)
	    break;
	if (fp2->fd_top + fp2->fd_len > flp->lnum)
	{
	    if (fp2->fd_top < flp->lnum)
	    {
		/* Make fold that includes lnum start at lnum. */
		foldMarkAdjustRecurse(&fp2->fd_nested,
			(linenr_t)0, (long)(flp->lnum - fp2->fd_top - 1),
			(linenr_t)MAXLNUM, (long)(fp2->fd_top - flp->lnum));
		fp2->fd_len -= flp->lnum - fp2->fd_top;
		fp2->fd_top = flp->lnum;
		fold_changed = TRUE;
	    }

	    if (lvl >= level)
	    {
		/* merge new fold with existing fold that follows */
		foldMerge(fp, gap, fp2);
	    }
	    break;
	}
	fold_changed = TRUE;
	deleteFoldEntry(gap, (int)(fp2 - (fold_t *)gap->ga_data), TRUE);
    }

    /* Need to redraw the lines we inspected, which might be further down than
     * was asked for. */
    if (bot < flp->lnum - 1)
	bot = flp->lnum - 1;

    return bot;
}

/* foldInsert() {{{2 */
/*
 * Insert a new fold in "gap" at position "i".
 * Returns OK for success, FAIL for failure.
 */
    static int
foldInsert(gap, i)
    garray_t	*gap;
    int		i;
{
    fold_t	*fp;

    if (ga_grow(gap, 1) != OK)
	return FAIL;
    fp = (fold_t *)gap->ga_data + i;
    if (i < gap->ga_len)
	mch_memmove(fp + 1, fp, sizeof(fold_t) * (gap->ga_len - i));
    ++gap->ga_len;
    --gap->ga_room;
    ga_init2(&fp->fd_nested, (int)sizeof(fold_t), 10);
    return OK;
}

/* foldSplit() {{{2 */
/*
 * Split the "i"th fold in "gap", which starts before "top" and ends below
 * "bot" in two pieces, one ending above "top" and the other starting below
 * "bot".
 * The caller must first have taken care of any nested folds from "top" to
 * "bot"!
 */
    static void
foldSplit(gap, i, top, bot)
    garray_t	*gap;
    int		i;
    linenr_t	top;
    linenr_t	bot;
{
    fold_t	*fp;
    fold_t	*fp2;
    garray_t	*gap1;
    garray_t	*gap2;
    int		idx;
    int		len;

    /* The fold continues below bot, need to split it. */
    if (foldInsert(gap, i + 1) == FAIL)
	return;
    fp = (fold_t *)gap->ga_data + i;
    fp[1].fd_top = bot + 1;
    fp[1].fd_len = fp->fd_len - (fp[1].fd_top - fp->fd_top);
    fp[1].fd_flags = fp->fd_flags;

    /* Move nested folds below bot to new fold.  There can't be
     * any between top and bot, they have been removed by the caller. */
    gap1 = &fp->fd_nested;
    gap2 = &fp[1].fd_nested;
    (void)(foldFind(gap1, bot + 1 - fp->fd_top, &fp2));
    len = (fold_t *)gap1->ga_data + gap1->ga_len - fp2;
    if (len > 0 && ga_grow(gap2, len) == OK)
    {
	for (idx = 0; idx < len; ++idx)
	{
	    ((fold_t *)gap2->ga_data)[idx] = fp2[idx];
	    ((fold_t *)gap2->ga_data)[idx].fd_top
						 -= fp[1].fd_top - fp->fd_top;
	}
	gap2->ga_len = len;
	gap2->ga_room -= len;
	gap1->ga_len -= len;
	gap1->ga_room += len;
    }
    fp->fd_len = top - fp->fd_top;
    fold_changed = TRUE;
}

/* foldRemove() {{{2 */
/*
 * Remove folds within the range "top" to and including "bot".
 * Check for these situations:
 *      1  2  3
 *      1  2  3
 * top     2  3  4  5
 *         2  3  4  5
 * bot     2  3  4  5
 *            3     5  6
 *            3     5  6
 *
 * 1: not changed
 * 2: trunate to stop above "top"
 * 3: split in two parts, one stops above "top", other starts below "bot".
 * 4: deleted
 * 5: made to start below "bot".
 * 6: not changed
 */
    static void
foldRemove(gap, top, bot)
    garray_t	*gap;
    linenr_t	top;
    linenr_t	bot;
{
    fold_t	*fp = NULL;

    if (bot < top)
	return;		/* nothing to do */

    for (;;)
    {
	/* Find fold that includes top or a following one. */
	if (foldFind(gap, top, &fp) && fp->fd_top < top)
	{
	    /* 2: or 3: need to delete nested folds */
	    foldRemove(&fp->fd_nested, top - fp->fd_top, bot - fp->fd_top);
	    if (fp->fd_top + fp->fd_len > bot + 1)
	    {
		/* 3: need to split it. */
		foldSplit(gap, (int)(fp - (fold_t *)gap->ga_data), top, bot);
	    }
	    else
	    {
		/* 2: truncate fold at "top". */
		fp->fd_len = top - fp->fd_top;
	    }
	    fold_changed = TRUE;
	    continue;
	}
	if (fp >= (fold_t *)(gap->ga_data) + gap->ga_len
		|| fp->fd_top > bot)
	{
	    /* 6: Found a fold below bot, can stop looking. */
	    break;
	}
	if (fp->fd_top >= top)
	{
	    /* Found an entry below top. */
	    fold_changed = TRUE;
	    if (fp->fd_top + fp->fd_len > bot)
	    {
		/* 5: Make fold that includes bot start below bot. */
		foldMarkAdjustRecurse(&fp->fd_nested,
			(linenr_t)0, (long)(bot - fp->fd_top),
			(linenr_t)MAXLNUM, (long)(fp->fd_top - bot - 1));
		fp->fd_len -= bot - fp->fd_top + 1;
		fp->fd_top = bot + 1;
		break;
	    }

	    /* 4: Delete completely contained fold. */
	    deleteFoldEntry(gap, (int)(fp - (fold_t *)gap->ga_data), TRUE);
	}
    }
}

/* foldMerge() {{{2 */
/*
 * Merge two adjecent folds (and the nested ones in them).
 * This only works correctly when the folds are really adjecent!  Thus "fp1"
 * must end just above "fp2".
 * The resulting fold is "fp1", nested folds are moved from "fp2" to "fp1".
 * Fold entry "fp2" in "gap" is deleted.
 */
    static void
foldMerge(fp1, gap, fp2)
    fold_t	*fp1;
    garray_t	*gap;
    fold_t	*fp2;
{
    fold_t	*fp3;
    fold_t	*fp4;
    int		idx;
    garray_t	*gap1 = &fp1->fd_nested;
    garray_t	*gap2 = &fp2->fd_nested;

    /* If the last nested fold in fp1 touches the first nested fold in fp2,
     * merge them recursively. */
    if (foldFind(gap1, fp1->fd_len - 1L, &fp3) && foldFind(gap2, 0L, &fp4))
	foldMerge(fp3, gap2, fp4);

    /* Move nested folds in fp2 to the end of fp1. */
    if (gap2->ga_len > 0 && ga_grow(gap1, gap2->ga_len) == OK)
    {
	for (idx = 0; idx < gap2->ga_len; ++idx)
	{
	    ((fold_t *)gap1->ga_data)[gap1->ga_len]
					= ((fold_t *)gap2->ga_data)[idx];
	    ((fold_t *)gap1->ga_data)[gap1->ga_len].fd_top += fp1->fd_len;
	    ++gap1->ga_len;
	    --gap1->ga_room;
	}
	gap2->ga_len = 0;
	/* fp2->fd_nested.ga_room isn't updated, we delete it below */
    }

    fp1->fd_len += fp2->fd_len;
    deleteFoldEntry(gap, (int)(fp2 - (fold_t *)gap->ga_data), TRUE);
    fold_changed = TRUE;
}

/* foldlevelIndent() {{{2 */
/*
 * Low level function to get the foldlevel for the "indent" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    static void
foldlevelIndent(flp)
    fline_t	*flp;
{
    char_u	*s;
    buf_t	*buf;
    linenr_t	lnum = flp->lnum + flp->off;

    buf = flp->wp->w_buffer;
    s = skipwhite(ml_get_buf(buf, lnum, FALSE));

    /* empty line or lines starting with a character in 'foldignore': level
     * depends on surrounding lines */
    if (*s == NUL || vim_strchr(flp->wp->w_p_fdi, *s) != NULL)
    {
	/* first and last line can't be undefined, use level 0 */
	if (lnum == 1 || lnum == buf->b_ml.ml_line_count)
	    flp->lvl = 0;
	else
	    flp->lvl = -1;
    }
    else
	flp->lvl = get_indent_buf(buf, lnum) / buf->b_p_sw;
    if (flp->lvl > flp->wp->w_p_fdn)
	flp->lvl = flp->wp->w_p_fdn;
}

/* foldlevelExpr() {{{2 */
/*
 * Low level function to get the foldlevel for the "expr" method.
 * Doesn't use any caching.
 * Returns a level of -1 if the foldlevel depends on surrounding lines.
 */
    static void
foldlevelExpr(flp)
    fline_t	*flp;
{
    win_t	*win;
    int		n;
    int		c;
    linenr_t	lnum = flp->lnum + flp->off;

    win = curwin;
    curwin = flp->wp;
    curbuf = flp->wp->w_buffer;
    set_vim_var_nr(VV_LNUM, lnum);

    flp->start = 0;
    flp->had_end = flp->end;
    flp->end = MAX_LEVEL + 1;
    if (lnum <= 1)
	flp->lvl = 0;

    n = eval_foldexpr(flp->wp->w_p_fde, &c);
    switch (c)
    {
	/* "a1", "a2", .. : add to the fold level */
	case 'a': if (flp->lvl >= 0)
		  {
		      flp->lvl += n;
		      flp->lvl_next = flp->lvl;
		  }
		  flp->start = n;
		  break;

	/* "s1", "s2", .. : subtract from the fold level */
	case 's': if (flp->lvl >= 0)
		  {
		      if (n > flp->lvl)
			  flp->lvl_next = 0;
		      else
			  flp->lvl_next = flp->lvl - n;
		      flp->end = flp->lvl_next + 1;
		  }
		  break;

	/* ">1", ">2", .. : start a fold with a certain level */
	case '>': flp->lvl = n;
		  flp->lvl_next = n;
		  flp->start = 1;
		  break;

	/* "<1", "<2", .. : end a fold with a certain level */
	case '<': flp->lvl = n;
		  flp->lvl_next = n - 1;
		  flp->end = n;
		  break;

	/* "=": No change in level */
	case '=': flp->lvl_next = flp->lvl;
		  break;

	/* "-1", "0", "1", ..: set fold level */
	default:  if (n < 0)
		      /* Use the current level for the next line, so that "a1"
		       * will work there. */
		      flp->lvl_next = flp->lvl;
		  else
		      flp->lvl_next = n;
		  flp->lvl = n;
		  break;
    }

    /* If the level is unknown for the first or the last line in the file, use
     * level 0. */
    if (flp->lvl < 0)
    {
	if (lnum <= 1)
	{
	    flp->lvl = 0;
	    flp->lvl_next = 0;
	}
	if (lnum == curbuf->b_ml.ml_line_count)
	    flp->lvl_next = 0;
    }

    curwin = win;
    curbuf = curwin->w_buffer;
}

/* parseMarker() {{{2 */
/*
 * Parse 'foldmarker' and set "foldendmarker", "foldstartmarkerlen" and
 * "foldendmarkerlen".
 * Relies on the option value to have been checked for correctness already.
 */
    static void
parseMarker(wp)
    win_t	*wp;
{
    foldendmarker = vim_strchr(wp->w_p_fmr, ',');
    foldstartmarkerlen = foldendmarker++ - wp->w_p_fmr;
    foldendmarkerlen = STRLEN(foldendmarker);
}

/* foldlevelMarker() {{{2 */
/*
 * Low level function to get the foldlevel for the "marker" method.
 * "foldendmarker", "foldstartmarkerlen" and "foldendmarkerlen" must have been
 * set before calling this.
 * Requires that flp->lvl is set to the fold level of the previous line!
 * Careful: This means you can't call this function twice on the same line.
 * Doesn't use any caching.
 * Sets flp->start when a start marker was found.
 */
    static void
foldlevelMarker(flp)
    fline_t	*flp;
{
    char_u	*startmarker;
    int		cstart;
    int		cend;
    char_u	*s;
    int		n;

    /* cache a few values for speed */
    startmarker = flp->wp->w_p_fmr;
    cstart = *startmarker;
    ++startmarker;
    cend = *foldendmarker;

    /* Default: no start found, next level is same as current level */
    flp->start = 0;
    flp->lvl_next = flp->lvl;

    s = ml_get_buf(flp->wp->w_buffer, flp->lnum + flp->off, FALSE);
    while (*s)
    {
	if (*s == cstart
		  && STRNCMP(s + 1, startmarker, foldstartmarkerlen - 1) == 0)
	{
	    /* found startmarker: set flp->lvl */
	    s += foldstartmarkerlen;
	    if (isdigit(*s))
	    {
		n = atoi((char *)s);
		if (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n;
		    ++flp->start;
		}
	    }
	    else
	    {
		++flp->lvl;
		++flp->lvl_next;
		++flp->start;
	    }
	}
	else if (*s == cend
	      && STRNCMP(s + 1, foldendmarker + 1, foldendmarkerlen - 1) == 0)
	{
	    /* found endmarker: set flp->lvl_next */
	    s += foldendmarkerlen;
	    if (isdigit(*s))
	    {
		n = atoi((char *)s);
		if (n > 0)
		{
		    flp->lvl = n;
		    flp->lvl_next = n - 1;
		    /* never start a fold with an end marker */
		    if (flp->lvl_next > flp->lvl)
			flp->lvl_next = flp->lvl;
		}
	    }
	    else
		--flp->lvl_next;
	}
	else
	    ++s;
    }
}

/* foldlevelSyntax() {{{2 */
/*
 * Low level function to get the foldlevel for the "syntax" method.
 * Doesn't use any caching.
 */
    static void
foldlevelSyntax(flp)
    fline_t	*flp;
{
    linenr_t	lnum = flp->lnum + flp->off;
    int		n;

    /* Use the maximum fold level at the start of this line and the next. */
    flp->lvl = syn_get_foldlevel(flp->wp, lnum);
    flp->start = FALSE;
    if (lnum < flp->wp->w_buffer->b_ml.ml_line_count)
    {
	n = syn_get_foldlevel(flp->wp, lnum + 1);
	if (n > flp->lvl)
	{
	    flp->start = n - flp->lvl;	/* fold(s) start here */
	    flp->lvl = n;
	}
    }
}

/* functions for storing the fold state in a View {{{1 */
/* put_folds() {{{2 */
#if defined(FEAT_SESSION) || defined(PROTO)
static int put_folds_recurse __ARGS((FILE *fd, garray_t *gap, linenr_t off));
static int put_foldopen_recurse __ARGS((FILE *fd, garray_t *gap, linenr_t off));

/*
 * Write commands to "fd" to restore the manual folds in window "wp".
 * Return FAIL if writing fails.
 */
    int
put_folds(fd, wp)
    FILE	*fd;
    win_t	*wp;
{
    if (foldmethodIsManual(wp))
    {
	if (put_line(fd, "silent! normal zE") == FAIL
		|| put_folds_recurse(fd, &wp->w_folds, (linenr_t)0) == FAIL)
	    return FAIL;
    }

    /* If some folds are manually opened/closed, need to restore that. */
    if (wp->w_fold_manual)
	return put_foldopen_recurse(fd, &wp->w_folds, (linenr_t)0);

    return OK;
}

/* put_folds_recurse() {{{2 */
/*
 * Write commands to "fd" to recreate manually created folds.
 * Returns FAIL when writing failed.
 */
    static int
put_folds_recurse(fd, gap, off)
    FILE	*fd;
    garray_t	*gap;
    linenr_t	off;
{
    int		i;
    fold_t	*fp;

    fp = (fold_t *)gap->ga_data;
    for (i = 0; i < gap->ga_len; i++)
    {
	/* Do nested folds first, they will be created closed. */
	if (put_folds_recurse(fd, &fp->fd_nested, off + fp->fd_top) == FAIL)
	    return FAIL;
	if (fprintf(fd, "%ld,%ldfold", fp->fd_top + off,
					fp->fd_top + off + fp->fd_len - 1) < 0
		|| put_eol(fd) == FAIL)
	    return FAIL;
	++fp;
    }
    return OK;
}

/* put_folds_recurse() {{{2 */
/*
 * Write commands to "fd" to open and close manually opened/closed folds.
 * Returns FAIL when writing failed.
 */
    static int
put_foldopen_recurse(fd, gap, off)
    FILE	*fd;
    garray_t	*gap;
    linenr_t	off;
{
    int		i;
    fold_t	*fp;

    fp = (fold_t *)gap->ga_data;
    for (i = 0; i < gap->ga_len; i++)
    {
	if (fp->fd_flags != FD_LEVEL)
	{
	    if (fp->fd_nested.ga_len > 0)
	    {
		/* open/close nested folds while this fold is open */
		if (fprintf(fd, "%ld", fp->fd_top + off) < 0
			|| put_eol(fd) == FAIL
			|| put_line(fd, "normal zo") == FAIL)
		    return FAIL;
		if (put_foldopen_recurse(fd, &fp->fd_nested, off + fp->fd_top)
			== FAIL)
		    return FAIL;
	    }
	    if (fprintf(fd, "%ld", fp->fd_top + off) < 0
		    || put_eol(fd) == FAIL
		    || fprintf(fd, "normal z%c",
				    fp->fd_flags == FD_CLOSED ? 'c' : 'o') < 0
		    || put_eol(fd) == FAIL)
		return FAIL;
	}
	++fp;
    }

    return OK;
}
#endif /* FEAT_SESSION */

/* }}}1 */
#endif /* defined(FEAT_FOLDING) || defined(PROTO) */
