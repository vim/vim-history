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

/* cursor to next window */
	case 'j':
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

/* make all windows the same height */
	case '=':	win_equal();
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

/*
 * check if we are able to split the current window and compute its height
 */
 	if (curwin->w_height < 2 * MIN_ROWS + STATUS_HEIGHT)
	{
		EMSG(e_noroom);
		return FAIL;
	}
	if (new_height == 0)
		new_height = curwin->w_height / 2;

	if (new_height < MIN_ROWS)
		new_height = MIN_ROWS;
	
	if (new_height > curwin->w_height - MIN_ROWS - STATUS_HEIGHT)
		new_height = curwin->w_height - MIN_ROWS - STATUS_HEIGHT;

/*
 * allocate new window structure and link it in the window list
 */
	wp = win_alloc(curwin->w_prev, curwin);
	if (wp == NULL)
		return FAIL;
/*
 * compute the new screen positions
 */
	wp->w_height = new_height;
	win_comp_scroll(wp);
	wp->w_status_height = STATUS_HEIGHT;
	curwin->w_height -= new_height + STATUS_HEIGHT;
	win_comp_scroll(curwin);
	wp->w_winpos = curwin->w_winpos;
	curwin->w_winpos = wp->w_winpos + wp->w_height + STATUS_HEIGHT;
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
 	win_enter(wp, FALSE);
	if (redraw)
		updateScreen(NOT_VALID);
	return OK;
}

/*
 * make all windows the same height
 */
	void
win_equal()
{
	int		total;
	int		wincount;
	int		winpos;
	int		temp;
	WIN		*wp;

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
 * spread the available lines over the windows
 */
	winpos = 0;
	for (wp = firstwin; wp; wp = wp->w_next)
	{
		wp->w_winpos = winpos;
		temp = total / wincount;
		wp->w_height = MIN_ROWS + temp;
		win_comp_scroll(wp);
		total -= temp;
		--wincount;
		winpos += wp->w_height + wp->w_status_height;
	}
	cursupdate();
	updateScreen(CLEAR);
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

	if (lastwin == firstwin)		/* "Cannot happen" */
	{
		EMSG("Cannot quit last window");
		return;
	}

/*
 * Close the link to the buffer.
 */
	close_buffer(curbuf, free_buf);

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
	win_enter(wp, FALSE);
	/*
	 * if last window has status line now and 'laststatus' not set,
	 * remove the status line
	 */
	if (!p_ls && lastwin->w_status_height)
	{
		lastwin->w_height += lastwin->w_status_height;
		lastwin->w_status_height = 0;
		win_comp_scroll(lastwin);
	}
	win_comp_scroll(curwin);
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
win_alloc(prev, next)
	WIN		*prev, *next;
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
		new->w_prev = prev;
		new->w_next = next;
		if (prev == NULL)
			firstwin = new;
		else
			prev->w_next = new;
		if (next == NULL)
			lastwin = new;
		else
			next->w_prev = new;

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

	if (wp->w_prev)
		wp->w_prev->w_next = wp->w_next;
	else
		firstwin = wp->w_next;
	if (wp->w_next)
		wp->w_next->w_prev = wp->w_prev;
	else
		lastwin = wp->w_prev;

	free(wp);
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
			/* update the w_winpos fields */
		for (wp = firstwin->w_next; wp; wp = wp->w_next)
			wp->w_winpos = wp->w_prev->w_winpos + wp->w_prev->w_height + wp->w_prev->w_status_height;
	}
	else if (extra_lines > 0)					/* increase height of last window */
	{
		lastwin->w_height += extra_lines;
		win_comp_scroll(lastwin);
	}

	compute_cmdrow();
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

/*
 * recompute the window positions
 */
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
		if (!p_ls)		/* remove status line */
		{
			lastwin->w_status_height = 0;
			lastwin->w_height++;
			win_comp_scroll(lastwin);
			lastwin->w_redr_status = TRUE;
		}
	}
	else
	{
		if (p_ls)		/* add status line */
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
