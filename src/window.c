/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static int win_comp_pos __ARGS((void));
static void win_exchange __ARGS((long));
static void win_rotate __ARGS((int, int));
static void win_append __ARGS((WIN *, WIN *));
static void win_remove __ARGS((WIN *));

static WIN		*prevwin = NULL;		/* previous window */

/*
 * all CTRL-W window commands are handled here, called from normal().
 */
	void
do_window(nchar, Prenum)
	int		nchar;
	long	Prenum;
{
	long	Prenum1;
	WIN		*wp;
	char_u	*ptr;

	if (Prenum == 0)
		Prenum1 = 1;
	else
		Prenum1 = Prenum;

	switch (nchar)
	{
/* split current window in two parts */
	case 'S':
	case Ctrl('S'):
	case 's':	VIsual.lnum = 0;		/* stop Visual mode */
				win_split(Prenum, TRUE);
				break;

/* open new window */
	case Ctrl('N'):
	case 'n':	VIsual.lnum = 0;					/* stop Visual mode */
				stuffcharReadbuff(':');
				if (Prenum)
					stuffnumReadbuff(Prenum);		/* window height */
				stuffReadbuff((char_u *)"new\n");	/* it is cmdline.c */
				break;

/* quit current window */
	case Ctrl('Q'):
	case 'q':	VIsual.lnum = 0;		/* stop Visual mode */
				stuffReadbuff((char_u *)":quit\n");	/* it is cmdline.c */
				break;

/* close current window */
	case Ctrl('C'):
	case 'c':	VIsual.lnum = 0;		/* stop Visual mode */
				stuffReadbuff((char_u *)":close\n");	/* it is cmdline.c */
				break;

/* close all but current window */
	case Ctrl('O'):
	case 'o':	VIsual.lnum = 0;		/* stop Visual mode */
				stuffReadbuff((char_u *)":only\n");	/* it is cmdline.c */
				break;

/* cursor to next window */
	case 'j':
	case K_DARROW:
	case Ctrl('J'):
				VIsual.lnum = 0;		/* stop Visual mode */
				for (wp = curwin; wp->w_next != NULL && Prenum1-- > 0;
															wp = wp->w_next)
					;
				win_enter(wp, TRUE);
				cursupdate();
				break;

/* cursor to next window with wrap around */
	case Ctrl('W'):
	case 'w':
				VIsual.lnum = 0;		/* stop Visual mode */
				if (lastwin == firstwin)		/* just one window */
					beep();
				else
				{
					if (Prenum)					/* go to specified window */
					{
						for (wp = firstwin; --Prenum > 0; )
						{
							if (wp->w_next == NULL)
								break;
							else
								wp = wp->w_next;
						}
					}
					else						/* go to next window */
					{
						wp = curwin->w_next;
						if (wp == NULL)
							wp = firstwin;		/* wrap around */
					}
					win_enter(wp, TRUE);
					cursupdate();
				}
				break;

/* cursor to window above */
	case 'k':
	case K_UARROW:
	case Ctrl('K'):
				VIsual.lnum = 0;		/* stop Visual mode */
				for (wp = curwin; wp->w_prev != NULL && Prenum1-- > 0;
															wp = wp->w_prev)
					;
				win_enter(wp, TRUE);
				cursupdate();
				break;

/* cursor to last accessed (previous) window */
	case 'p':
	case Ctrl('P'):
				VIsual.lnum = 0;		/* stop Visual mode */
				if (prevwin == NULL)
					beep();
				else
				{
					win_enter(prevwin, TRUE);
					cursupdate();
				}
				break;

/* exchange current and next window */
	case 'x':
	case Ctrl('X'):
				win_exchange(Prenum);
				break;

/* rotate windows downwards */
	case Ctrl('R'):
	case 'r':	VIsual.lnum = 0;					/* stop Visual mode */
				win_rotate(FALSE, (int)Prenum1);	/* downwards */
				break;

/* rotate windows upwards */
	case 'R':	VIsual.lnum = 0;					/* stop Visual mode */
				win_rotate(TRUE, (int)Prenum1);		/* upwards */
				break;

/* make all windows the same height */
	case '=':	win_equal(NULL, TRUE);
				break;

/* increase current window height */
	case '+':	win_setheight(curwin->w_height + (int)Prenum1);
				break;

/* decrease current window height */
	case '-':	win_setheight(curwin->w_height - (int)Prenum1);
				break;

/* set current window height */
	case Ctrl('_'):
	case '_':	win_setheight(Prenum ? (int)Prenum : 9999);
				break;

/* jump to tag and split window if tag exists */
	case ']':
	case Ctrl(']'):
				VIsual.lnum = 0;		/* stop Visual mode */
				postponed_split = TRUE;
				stuffcharReadbuff(Ctrl(']'));
				break;

/* edit file name under cursor in a new window */
	case 'f':
	case Ctrl('F'):
				VIsual.lnum = 0;		/* stop Visual mode */
				ptr = file_name_at_cursor();
				if (ptr == NULL)
					beep();
				else
				{
					stuffReadbuff((char_u *) ":split ");
					stuffReadbuff(ptr);
					stuffReadbuff((char_u *) "\n");
					free(ptr);
				}
				break;

	default:	beep();
				break;
	}
}

/*
 * split the current window, implements CTRL-W s and :split
 *
 * new_height is the height for the new window, 0 to make half of current height
 * redraw is TRUE when redraw now
 *
 * return FAIL for failure, OK otherwise
 */
	int
win_split(new_height, redraw)
	long	new_height;
	int		redraw;
{
	WIN			*wp;
	linenr_t	lnum;
	int			h;
	int			i;
	int			need_status;
	int			do_equal = (p_ea && new_height == 0);
	int			needed;
	int			available;
	
		/* add a status line when p_ls == 1 and splitting the first window */
	if (lastwin == firstwin && p_ls == 1 && curwin->w_status_height == 0)
		need_status = STATUS_HEIGHT;
	else
		need_status = 0;

/*
 * check if we are able to split the current window and compute its height
 */
	available = curwin->w_height;
 	needed = 2 * MIN_ROWS + STATUS_HEIGHT + need_status;
	if (p_ea)
	{
		for (wp = firstwin; wp != NULL; wp = wp->w_next)
			if (wp != curwin)
			{
				available += wp->w_height;
				needed += MIN_ROWS;
			}
	}
 	if (available < needed)
	{
		EMSG(e_noroom);
		return FAIL;
	}
	if (need_status)
	{
		curwin->w_status_height = STATUS_HEIGHT;
		curwin->w_height -= STATUS_HEIGHT;
	}
	if (new_height == 0)
		new_height = curwin->w_height / 2;

	if (new_height > curwin->w_height - MIN_ROWS - STATUS_HEIGHT)
		new_height = curwin->w_height - MIN_ROWS - STATUS_HEIGHT;

	if (new_height < MIN_ROWS)
		new_height = MIN_ROWS;

		/* if it doesn't fit in the current window, need win_equal() */
	if (curwin->w_height - new_height - STATUS_HEIGHT < MIN_ROWS)
		do_equal = TRUE;
/*
 * allocate new window structure and link it in the window list
 */
	if (p_sb)		/* new window below current one */
		wp = win_alloc(curwin);
	else
		wp = win_alloc(curwin->w_prev);
	if (wp == NULL)
		return FAIL;
/*
 * compute the new screen positions
 */
	wp->w_height = new_height;
	win_comp_scroll(wp);
	curwin->w_height -= new_height + STATUS_HEIGHT;
	win_comp_scroll(curwin);
	if (p_sb)		/* new window below current one */
	{
		wp->w_winpos = curwin->w_winpos + curwin->w_height + STATUS_HEIGHT;
		wp->w_status_height = curwin->w_status_height;
		curwin->w_status_height = STATUS_HEIGHT;
	}
	else			/* new window above current one */
	{
		wp->w_winpos = curwin->w_winpos;
		wp->w_status_height = STATUS_HEIGHT;
		curwin->w_winpos = wp->w_winpos + wp->w_height + STATUS_HEIGHT;
	}
/*
 * make the contents of the new window the same as the current one
 */
	wp->w_buffer = curbuf;
	curbuf->b_nwindows++;
	wp->w_cursor = curwin->w_cursor;
	wp->w_row = curwin->w_row;
	wp->w_col = curwin->w_col;
	wp->w_virtcol = curwin->w_virtcol;
	wp->w_curswant = curwin->w_curswant;
	wp->w_set_curswant = curwin->w_set_curswant;
	wp->w_empty_rows = curwin->w_empty_rows;
	wp->w_leftcol = curwin->w_leftcol;
	wp->w_pcmark = curwin->w_pcmark;
	wp->w_prev_pcmark = curwin->w_prev_pcmark;

	wp->w_arg_idx = curwin->w_arg_idx;
	/*
	 * copy tagstack and options from existing window
	 */
	for (i = 0; i < curwin->w_tagstacklen; i++)
	{
		wp->w_tagstack[i].fmark = curwin->w_tagstack[i].fmark;
		wp->w_tagstack[i].tagname = strsave(curwin->w_tagstack[i].tagname);
	}
	wp->w_tagstackidx = curwin->w_tagstackidx;
	wp->w_tagstacklen = curwin->w_tagstacklen;
	win_copy_options(curwin, wp);
/*
 * Both windows need redrawing
 */
 	wp->w_redr_type = NOT_VALID;
	wp->w_redr_status = TRUE;
 	curwin->w_redr_type = NOT_VALID;
	curwin->w_redr_status = TRUE;
/*
 * Cursor is put in middle of window in both windows
 */
	if (wp->w_height < curwin->w_height)	/* use smallest of two heights */
		h = wp->w_height;
	else
		h = curwin->w_height;
	h >>= 1;
	for (lnum = wp->w_cursor.lnum; lnum > 1; --lnum)
	{
		h -= plines(lnum);
		if (h <= 0)
			break;
	}
	wp->w_topline = lnum;
	curwin->w_topline = lnum;
/*
 * make the new window the current window and redraw
 */
	if (do_equal)
		win_equal(wp, FALSE);
 	win_enter(wp, FALSE);
	if (redraw)
		updateScreen(NOT_VALID);
	return OK;
}

/*
 * make 'count' windows on the screen
 * return actual number of windows on the screen
 * called when there is just one window, filling the whole screen.
 */
	int
make_windows(count)
	int		count;
{
	int		maxcount;
	int		todo;
	int		p_sb_save;

/*
 * each window needs at least MIN_ROWS lines and a status line
 */
	maxcount = (curwin->w_height + curwin->w_status_height) /
											(MIN_ROWS + STATUS_HEIGHT);
	if (count > maxcount)
		count = maxcount;

	/*
	 * add status line now, otherwise first window will be too big
	 */
	if ((p_ls == 2 || (count > 1 && p_ls == 1)) && curwin->w_status_height == 0)
	{
		curwin->w_status_height = STATUS_HEIGHT;
		curwin->w_height -= STATUS_HEIGHT;
	}

/*
 * set 'splitbelow' off for a moment, don't what that now
 */
	p_sb_save = p_sb;
	p_sb = FALSE;
		/* todo is number of windows left to create */
	for (todo = count - 1; todo > 0; --todo)
		if (win_split((long)(curwin->w_height - (curwin->w_height - todo
				* STATUS_HEIGHT) / (todo + 1) - STATUS_HEIGHT), FALSE) == FAIL)
			break;
	p_sb = p_sb_save;

		/* return actual number of windows */
	return (count - todo);
}

/*
 * Exchange current and next window
 */
	static void
win_exchange(Prenum)
	long		Prenum;
{
	WIN		*wp;
	WIN		*wp2;
	int		temp;

	if (lastwin == firstwin)		/* just one window */
	{
		beep();
		return;
	}

/*
 * find window to exchange with
 */
	if (Prenum)
	{
		wp = firstwin;
		while (wp != NULL && --Prenum > 0)
			wp = wp->w_next;
	}
	else if (curwin->w_next != NULL)	/* Swap with next */
		wp = curwin->w_next;
	else	/* Swap last window with previous */
		wp = curwin->w_prev;

	if (wp == curwin || wp == NULL)
		return;

/*
 * 1. remove curwin from the list. Remember after which window it was in wp2
 * 2. insert curwin before wp in the list
 * if wp != wp2
 *    3. remove wp from the list
 *    4. insert wp after wp2
 * 5. exchange the status line height
 */
	wp2 = curwin->w_prev;
	win_remove(curwin);
	win_append(wp->w_prev, curwin);
	if (wp != wp2)
	{
		win_remove(wp);
		win_append(wp2, wp);
	}
	temp = curwin->w_status_height;
	curwin->w_status_height = wp->w_status_height;
	wp->w_status_height = temp;

	win_comp_pos();				/* recompute window positions */

	win_enter(wp, TRUE);
	cursupdate();
	updateScreen(CLEAR);
}

/*
 * rotate windows: if upwards TRUE the second window becomes the first one
 *				   if upwards FALSE the first window becomes the second one
 */
	static void
win_rotate(upwards, count)
	int		upwards;
	int		count;
{
	WIN			 *wp;
	int			 height;

	if (firstwin == lastwin)			/* nothing to do */
	{
		beep();
		return;
	}
	while (count--)
	{
		if (upwards)			/* first window becomes last window */
		{
			wp = firstwin;
			win_remove(wp);
			win_append(lastwin, wp);
			wp = lastwin->w_prev;			/* previously last window */
		}
		else					/* last window becomes first window */
		{
			wp = lastwin;
			win_remove(lastwin);
			win_append(NULL, wp);
			wp = firstwin;					/* previously last window */
		}
			/* exchange status height of old and new last window */
		height = lastwin->w_status_height;
		lastwin->w_status_height = wp->w_status_height;
		wp->w_status_height = height;

			/* recompute w_winpos for all windows */
		(void) win_comp_pos();
	}

	cursupdate();
	updateScreen(CLEAR);
}

/*
 * make all windows the same height
 */
	void
win_equal(next_curwin, redraw)
	WIN		*next_curwin;			/* pointer to current window to be */
	int		redraw;
{
	int		total;
	int		less;
	int		wincount;
	int		winpos;
	int		temp;
	WIN		*wp;
	int		new_height;

/*
 * count the number of lines available
 */
	total = 0;
	wincount = 0;
	for (wp = firstwin; wp; wp = wp->w_next)
	{
		total += wp->w_height - MIN_ROWS;
		wincount++;
	}

/*
 * if next_curwin given and 'winheight' set, make next_curwin p_wh lines
 */
	if (next_curwin != NULL && p_wh)
	{
		if (p_wh - MIN_ROWS > total)	/* all lines go to current window */
			less = total;
		else
		{
			less = p_wh - MIN_ROWS - total / wincount;
			if (less < 0)
				less = 0;
		}
	}
	else
		less = 0;
		

/*
 * spread the available lines over the windows
 */
	winpos = 0;
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
	{
		if (wp == next_curwin && less)
		{
			less = 0;
			temp = p_wh - MIN_ROWS;
			if (temp > total)
				temp = total;
		}
		else
			temp = (total - less + (wincount >> 1)) / wincount;
		new_height = MIN_ROWS + temp;
		if (wp->w_winpos != winpos || wp->w_height != new_height)
		{
			wp->w_redr_type = NOT_VALID;
			wp->w_redr_status = TRUE;
		}
		wp->w_winpos = winpos;
		wp->w_height = new_height;
		win_comp_scroll(wp);
		total -= temp;
		--wincount;
		winpos += wp->w_height + wp->w_status_height;
	}
	if (redraw)
	{
		cursupdate();
		updateScreen(CLEAR);
	}
}

/*
 * close current window
 * If "free_buf" is TRUE related buffer may be freed.
 *
 * called by :quit, :close, :xit, :wq and findtag()
 */
	void
close_window(free_buf)
	int		free_buf;
{
	WIN 	*wp;

	if (lastwin == firstwin)
	{
		EMSG("Cannot close last window");
		return;
	}

/*
 * Close the link to the buffer.
 */
	close_buffer(curbuf, free_buf, FALSE);

/*
 * Remove the window.
 */
	if (curwin->w_prev == NULL)		/* freed space goes to next window */
	{
		wp = curwin->w_next;
		wp->w_winpos = curwin->w_winpos;
	}
	else							/* freed space goes to previous window */
		wp = curwin->w_prev;
	wp->w_height += curwin->w_height + curwin->w_status_height;

	win_free(curwin);
	curwin = NULL;
	if (p_ea)
		win_equal(wp, FALSE);
	win_enter(wp, FALSE);
	/*
	 * if last window has status line now and we don't want one,
	 * remove the status line
	 */
	if (lastwin->w_status_height &&
						(p_ls == 0 || (p_ls == 1 && firstwin == lastwin)))
	{
		lastwin->w_height += lastwin->w_status_height;
		lastwin->w_status_height = 0;
		win_comp_scroll(lastwin);
	}
	win_comp_scroll(curwin);
	updateScreen(NOT_VALID);
}

/*
 * close all windows except current one
 * buffers in the windows become hidden
 *
 * called by :only and do_arg_all();
 */
	void
close_others(message)
	int		message;
{
	WIN 	*wp;
	WIN 	*nextwp;

	if (lastwin == firstwin)
	{
		if (message)
			EMSG("Already only one window");
		return;
	}

	for (wp = firstwin; wp != NULL; wp = nextwp)
	{
		nextwp = wp->w_next;
		if (wp == curwin)				/* don't close current window */
			continue;
	/*
	 * Close the link to the buffer.
	 */
		close_buffer(wp->w_buffer, FALSE, FALSE);

	/*
	 * Remove the window. All lines go to current window.
	 */
		curwin->w_height += wp->w_height + wp->w_status_height;

		win_free(wp);
	}
	/*
	 * if current window has status line and we don't want one,
	 * remove the status line
	 */
	if (curwin->w_status_height && p_ls != 2)
	{
		curwin->w_height += curwin->w_status_height;
		curwin->w_status_height = 0;
	}
	curwin->w_winpos = 0;			/* put current window at top of the screen */
	win_comp_scroll(curwin);
	if (message)
		updateScreen(NOT_VALID);
}

/*
 * init the cursor in the window
 *
 * called when a new file is being edited
 */
	void
win_init(wp)
	WIN		*wp;
{
	wp->w_redr_type = NOT_VALID;
	wp->w_cursor.lnum = 1;
	wp->w_curswant = wp->w_cursor.col = 0;
	wp->w_pcmark.lnum = 1;		/* pcmark not cleared but set to line 1 */
	wp->w_pcmark.col = 0;
	wp->w_prev_pcmark.lnum = 0;
	wp->w_prev_pcmark.col = 0;
	wp->w_topline = 1;
	wp->w_botline = 2;
}

/*
 * make window wp the current window
 */
	void
win_enter(wp, undo_sync)
	WIN		*wp;
	int		undo_sync;
{
	if (wp == curwin)			/* nothing to do */
		return;

		/* sync undo before leaving the current buffer */
	if (undo_sync && curbuf != wp->w_buffer)
		u_sync();
	if (curwin != NULL)
		prevwin = curwin;		/* remember for CTRL-W p */
	curwin = wp;
	curbuf = wp->w_buffer;
	maketitle();
			/* set window height to desired minimal value */
	if (p_wh && curwin->w_height < p_wh)
		win_setheight((int)p_wh);
}

/*
 * allocate a window structure and link it in the window list
 */
	WIN *
win_alloc(after)
	WIN		*after;
{
	WIN		*new;

/*
 * allocate window structure and linesizes arrays
 */
	new = (WIN *)alloc((unsigned)sizeof(WIN));
	if (new)
	{
/*
 * most stucture members have to be zero
 */
 		memset((char *)new, 0, sizeof(WIN));
/*
 * link the window in the window list
 */
		win_append(after, new);

		win_alloc_lsize(new);

		/* position the display and the cursor at the top of the file. */
		new->w_topline = 1;
		new->w_cursor.lnum = 1;
	}
	return new;
}

/*
 * remove window 'wp' from the window list and free the structure
 */
	void
win_free(wp)
	WIN		*wp;
{
	if (prevwin == wp)
		prevwin = NULL;
	win_free_lsize(wp);
	win_remove(wp);
	free(wp);
}

	static void
win_append(after, wp)
	WIN		*after, *wp;
{
	WIN 	*before;

	if (after == NULL)		/* after NULL is in front of the first */
		before = firstwin;
	else
		before = after->w_next;

	wp->w_next = before;
	wp->w_prev = after;
	if (after == NULL)
		firstwin = wp;
	else
		after->w_next = wp;
	if (before == NULL)
		lastwin = wp;
	else
		before->w_prev = wp;
}

/*
 * remove window from the window list
 */
	static void
win_remove(wp)
	WIN		*wp;
{
	if (wp->w_prev)
		wp->w_prev->w_next = wp->w_next;
	else
		firstwin = wp->w_next;
	if (wp->w_next)
		wp->w_next->w_prev = wp->w_prev;
	else
		lastwin = wp->w_prev;
}

/*
 * allocate lsize arrays for a window
 * return FAIL for failure, OK for success
 */
	int
win_alloc_lsize(wp)
	WIN		*wp;
{
	wp->w_lsize_valid = 0;
	wp->w_lsize_lnum = (linenr_t *) malloc((size_t) (Rows * sizeof(linenr_t)));
	wp->w_lsize = (char_u *)malloc((size_t) Rows);
	if (wp->w_lsize_lnum == NULL || wp->w_lsize == NULL)
	{
		win_free_lsize(wp);		/* one of the two may have worked */
		wp->w_lsize_lnum = NULL;
		wp->w_lsize = NULL;
		return FAIL;
	}
	return OK;
}

/*
 * free lsize arrays for a window
 */
 	void
win_free_lsize(wp)
	WIN		*wp;
{
	free(wp->w_lsize_lnum);
	free(wp->w_lsize);
}

/*
 * call this fuction whenever Rows changes value
 */
	void
screen_new_rows()
{
	WIN		*wp;
	int		extra_lines;

	if (firstwin == NULL)		/* not initialized yet */
		return;
/*
 * the number of extra lines is the difference between the position where
 * the command line should be and where it is now
 */
	compute_cmdrow();
	extra_lines = Rows - p_ch - cmdline_row;
	if (extra_lines < 0)						/* reduce windows height */
	{
		for (wp = lastwin; wp; wp = wp->w_prev)
		{
			if (wp->w_height - MIN_ROWS < -extra_lines)
			{
				extra_lines += wp->w_height - MIN_ROWS;
				wp->w_height = MIN_ROWS;
				win_comp_scroll(wp);
			}
			else
			{
				wp->w_height += extra_lines;
				win_comp_scroll(wp);
				break;
			}
		}
		(void)win_comp_pos();					/* compute w_winpos */
	}
	else if (extra_lines > 0)					/* increase height of last window */
	{
		lastwin->w_height += extra_lines;
		win_comp_scroll(lastwin);
	}

	compute_cmdrow();
}

/*
 * update the w_winpos field for all windows
 * returns the row just after the last window
 */
	static int
win_comp_pos()
{
	WIN		*wp;
	int		row;

	row = 0;
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
	{
		if (wp->w_winpos != row)		/* if position changes, redraw */
		{
			wp->w_winpos = row;
			wp->w_redr_type = NOT_VALID;
			wp->w_redr_status = TRUE;
		}
		row += wp->w_height + wp->w_status_height;
	}
	return row;
}

/*
 * set current window height
 */
	void
win_setheight(height)
	int		height;
{
	WIN		*wp;
	int		room;				/* total number of lines available */
	int		take;				/* number of lines taken from other windows */
	int		room_cmdline;		/* lines available from cmdline */
	int		row;

	if (height < MIN_ROWS)		/* need at least some lines */
		height = MIN_ROWS;
/*
 * compute the room we have from all the windows
 */
	room = MIN_ROWS;			/* count the MIN_ROWS for the current window */
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
		room += wp->w_height - MIN_ROWS;
/*
 * compute the room available from the command line
 */
	room_cmdline = Rows - p_ch - cmdline_row;
/*
 * limit new height to the room available
 */
	if (height > room + room_cmdline)			/* can't make it that large */
		height = room + room_cmdline;			/* use all available room */
/*
 * compute the number of lines we will take from the windows (can be negative)
 */
	take = height - curwin->w_height;
	if (take == 0)								/* no change, nothing to do */
		return;

	if (take > 0)
	{
		take -= room_cmdline;					/* use lines from cmdline first */
		if (take < 0)
			take = 0;
	}
/*
 * set the current window to the new height
 */
	curwin->w_height = height;
	win_comp_scroll(curwin);
/*
 * take lines from the windows below the current window
 */
	for (wp = curwin->w_next; wp != NULL && take != 0; wp = wp->w_next)
	{
		if (wp->w_height - take < MIN_ROWS)
		{
			take -= wp->w_height - MIN_ROWS;
			wp->w_height = MIN_ROWS;
		}
		else
		{
			wp->w_height -= take;
			take = 0;
		}
		win_comp_scroll(wp);				/* recompute p_scroll */
		wp->w_redr_type = NOT_VALID;		/* need to redraw this window */
		wp->w_redr_status = TRUE;
	}
/*
 * take lines from the windows above the current window
 */
	for (wp = curwin->w_prev; wp != NULL && take != 0; wp = wp->w_prev)
	{
		if (wp->w_height - take < MIN_ROWS)
		{
			take -= wp->w_height - MIN_ROWS;
			wp->w_height = MIN_ROWS;
		}
		else
		{
			wp->w_height -= take;
			take = 0;
		}
		win_comp_scroll(wp);				/* recompute p_scroll */
		wp->w_redr_type = NOT_VALID;		/* need to redraw this window */
		wp->w_redr_status = TRUE;
	}

/* recompute the window positions */
	row = win_comp_pos();

/*
 * If there is extra space created between the last window and the command line,
 * clear it.
 */
 	screen_fill(row, cmdline_row, 0, (int)Columns, ' ', ' ');
	cmdline_row = row;

	updateScreen(NOT_VALID);
}

	void
win_comp_scroll(wp)
	WIN		*wp;
{
	wp->w_p_scroll = (wp->w_height >> 1);
	if (wp->w_p_scroll == 0)
		wp->w_p_scroll = 1;
}

/*
 * command_height: called whenever p_ch has been changed
 */
	void
command_height()
{
	int		current;

	current = Rows - cmdline_row;
	if (current > p_ch)				/* p_ch got smaller */
		lastwin->w_height += current - p_ch;
	else							/* p_ch got bigger */
	{
		if (lastwin->w_height - (p_ch - current) < MIN_ROWS)
		{
			emsg(e_noroom);
			p_ch = lastwin->w_height - MIN_ROWS + current;
		}
		lastwin->w_height -= p_ch - current;
									/* clear the lines added to cmdline */
		screen_fill((int)(Rows - p_ch), (int)Rows, 0, (int)Columns, ' ', ' ');
	}
	win_comp_scroll(lastwin);
	cmdline_row = Rows - p_ch;
	lastwin->w_redr_type = NOT_VALID;
	lastwin->w_redr_status = TRUE;
	redraw_cmdline = TRUE;
}

	void
last_status()
{
	if (lastwin->w_status_height)
	{
					/* remove status line */
		if (p_ls == 0 || (p_ls == 1 && firstwin == lastwin))
		{
			lastwin->w_status_height = 0;
			lastwin->w_height++;
			win_comp_scroll(lastwin);
			lastwin->w_redr_status = TRUE;
		}
	}
	else
	{
					/* add status line */
		if (p_ls == 2 || (p_ls == 1 && firstwin != lastwin))
		{
			if (lastwin->w_height <= MIN_ROWS)		/* can't do it */
				emsg(e_noroom);
			else
			{
				lastwin->w_status_height = 1;
				lastwin->w_height--;
				win_comp_scroll(lastwin);
				lastwin->w_redr_status = TRUE;
			}
		}
	}
}

/*
 * file_name_at_cursor()
 *
 * Return the name of the file under (or to the right of) the cursor.  The
 * p_path variable is searched if the file name does not start with '/'.
 * The string returned has been alloc'ed and should be freed by the caller.
 * NULL is returned if the file name or file is not found.
 */
	char_u *
file_name_at_cursor()
{
	char_u	*ptr;
	char_u	*dir;
	char_u	*file_name;
	char_u	save_char;
	int		col;
	int		len;

		/* characters in a file name besides alfa-num */
#ifdef UNIX
	char_u	*file_chars = (char_u *)"/.-_+,~$";
#endif
#ifdef AMIGA
	char_u	*file_chars = (char_u *)"/.-_+,$:";
#endif
#ifdef MSDOS
	char_u	*file_chars = (char_u *)"/.-_+,$\\:";
#endif

	ptr = ml_get(curwin->w_cursor.lnum);
	col = curwin->w_cursor.col;

		/* search forward for what could be the start of a file name */
	while (!isalnum((char) ptr[col]) && STRCHR(file_chars, ptr[col]) == NULL)
		++col;
	if (ptr[col] == NUL)			/* nothing found */
		return NULL;

		/* search backward for char that cannot be in a file name */
	while (col >= 0 &&
	  (isalnum((char) ptr[col]) || STRCHR(file_chars, ptr[col]) != NULL))
		--col;
	ptr += col + 1;
	col = 0;

		/* search forward for a char that cannot be in a file name */
	while (ptr[col] != NUL
	  && (isalnum((char) ptr[col]) || STRCHR(file_chars, ptr[col]) != NULL))
		++col;

		/* copy file name into NameBuff, expanding environment variables */
	save_char = ptr[col];
	ptr[col] = NUL;
	expand_env(ptr, NameBuff, MAXPATHL);
	ptr[col] = save_char;

	if (isFullName(NameBuff))			/* absolute path */
	{
		if ((file_name = strsave(NameBuff)) == NULL)
			return NULL;
		if (getperm(file_name) >= 0)
			return file_name;
	}
	else							/* relative path, use 'path' option */
	{
		if ((file_name = alloc((int)(STRLEN(p_path) + STRLEN(NameBuff) + 2))) == NULL)
			return NULL;
		dir = p_path;
		for (;;)
		{
			skipspace(&dir);
			for (len = 0; dir[len] != NUL && dir[len] != ' '; len++)
				;
			if (len == 0)
				break;
			if (len == 1 && dir[0] == '.')		/* current dir */
				STRCPY(file_name, NameBuff);
			else
			{
				STRNCPY(file_name, dir, (size_t)len);
#ifdef AMIGA			/* Amiga doesn't like c:/file */
				if (file_name[len - 1] != ':')
#endif
					file_name[len] = '/';
				STRCPY(file_name + len + 1, NameBuff);
			}
			if (getperm(file_name) >= 0)
				return file_name;
			dir += len;
		}
	}
	free(file_name);			/* file doesn't exist */
	return NULL;
}
