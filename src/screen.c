/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * screen.c: code for displaying on the screen
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

char *tgoto __PARMS((char *cm, int col, int line));

static char_u 	*Nextscreen = NULL; 	/* What is currently on the screen. */
static char_u 	**LinePointers = NULL;	/* array of pointers into Nextscreen */

/*
 * Cline_height is set (in cursupdate) to the number of physical
 * lines taken by the line the cursor is on. We use this to avoid extra calls
 * to plines(). The optimized routine updateline()
 * makes sure that the size of the cursor line hasn't changed. If so, lines
 * below the cursor will move up or down and we need to call the routine
 * updateScreen() to examine the entire screen.
 */
static int		Cline_height;			/* current size of cursor line */

static int		Cline_row;				/* starting row of the cursor line on screen */

static FPOS		old_cursor = {0, 0};	/* last known end of visual part */
static int		oldCurswant = 0;		/* last known value of Curswant */
static int		canopt;					/* TRUE when cursor goto can be optimized */
static int		invert = 0;				/* set to INVERTCODE when inverting */

#define INVERTCODE		0x80

static int win_line __ARGS((WIN *, linenr_t, int, int));
static void screen_char __ARGS((char_u *, int, int));
static void screenalloc __ARGS((int));
static void screenclear2 __ARGS((void));
static int screen_ins_lines __ARGS((int, int, int, int));

/*
 * updateline() - like updateScreen() but only for cursor line
 *
 * This determines whether or not we need to call updateScreen() to examine
 * the entire screen for changes. This occurs if the size of the cursor line
 * (in rows) hasn't changed.
 */
	void
updateline()
{
	int 		row;
	int 		n;

	if (must_redraw)		/* must redraw whole screen */
	{
		updateScreen(must_redraw);
		return;
	}

	screenalloc(TRUE);		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL || RedrawingDisabled)
		return;

	screen_start();			/* init cursor position of screen_char() */
	cursor_off();

	(void)set_highlight('v');
	row = win_line(curwin, curwin->w_cursor.lnum, Cline_row, curwin->w_height);

	if (row == curwin->w_height + 1)			/* line too long for window */
		updateScreen(VALID_TO_CURSCHAR);
	else
	{
		n = row - Cline_row;
		if (n != Cline_height)		/* line changed size */
		{
			if (n < Cline_height) 	/* got smaller: delete lines */
				win_del_lines(curwin, row, Cline_height - n, FALSE, TRUE);
			else					/* got bigger: insert lines */
				win_ins_lines(curwin, Cline_row + Cline_height, n - Cline_height, FALSE, TRUE);
			updateScreen(VALID_TO_CURSCHAR);
		}
	}
}

/*
 * updateScreen()
 *
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to Nextscreen, and update curwin->w_botline.
 */

	void
updateScreen(type)
	int 			type;
{
	WIN				*wp;

	screenalloc(TRUE);		/* allocate screen buffers if size changed */
	if (Nextscreen == NULL)
		return;

	if (must_redraw)
	{
		if (type < must_redraw)		/* use maximal type */
			type = must_redraw;
		must_redraw = 0;
	}

	if (type == CURSUPD)		/* update cursor and then redraw NOT_VALID*/
	{
		curwin->w_lsize_valid = 0;
		cursupdate();			/* will call updateScreen() */
		return;
	}
	if (curwin->w_lsize_valid == 0 && type != CLEAR)
		type = NOT_VALID;

 	if (RedrawingDisabled)
	{
		must_redraw = type;		/* remember type for next time */
		return;
	}

	/*
	 * if the screen was scrolled up when displaying a message, scroll it down
	 */
	if (msg_scrolled)
	{
		clear_cmdline = TRUE;
		if (msg_scrolled > Rows - 5)		/* clearing is faster */
			type = CLEAR;
		else if (type != CLEAR)
		{
			if (screen_ins_lines(0, 0, msg_scrolled, (int)Rows) == FAIL)
				type = CLEAR;
			win_rest_invalid(firstwin);		/* should do only first/last few */
		}
		msg_scrolled = 0;
	}

	/*
	 * reset cmdline_row now (may have been changed temporarily)
	 */
	compute_cmdrow();

	if (type == CLEAR)			/* first clear screen */
	{
		screenclear();			/* will reset clear_cmdline */
		type = NOT_VALID;
	}

	if (clear_cmdline)
		gotocmdline(TRUE, NUL);	/* first clear cmdline */

/* return if there is nothing to do */
	if ((type == VALID && curwin->w_topline == curwin->w_lsize_lnum[0]) ||
			(type == INVERTED && old_cursor.lnum == curwin->w_cursor.lnum &&
					old_cursor.col == curwin->w_cursor.col && curwin->w_curswant == oldCurswant))
		return;

	curwin->w_redr_type = type;

/*
 * go from top to bottom through the windows, redrawing the ones that need it
 */
	cursor_off();
	for (wp = firstwin; wp; wp = wp->w_next)
	{
		if (wp->w_redr_type)
			win_update(wp);
		if (wp->w_redr_status)
			win_redr_status(wp);
	}
	if (redraw_cmdline)
		showmode();
}

/*
 * update a single window
 *
 * This may cause the windows below it also to be redrawn
 */
	void
win_update(wp)
	WIN		*wp;
{
	int				type = wp->w_redr_type;
	register int	row;
	register int	endrow;
	linenr_t		lnum;
	linenr_t		lastline = 0; /* only valid if endrow != Rows -1 */
	int				done;		/* if TRUE, we hit the end of the file */
	int				didline;	/* if TRUE, we finished the last line */
	int 			srow = 0;	/* starting row of the current line */
	int 			idx;
	int 			i;
	long 			j;

	if (type == NOT_VALID)
	{
		wp->w_redr_status = TRUE;
		wp->w_lsize_valid = 0;
	}

	idx = 0;
	row = 0;
	lnum = wp->w_topline;

	/* The number of rows shown is w_height. */
	/* The default last row is the status/command line. */
	endrow = wp->w_height;

	if (type == VALID || type == VALID_TO_CURSCHAR)
	{
		/*
		 * We handle two special cases:
		 * 1: we are off the top of the screen by a few lines: scroll down
		 * 2: wp->w_topline is below wp->w_lsize_lnum[0]: may scroll up
		 */
		if (wp->w_topline < wp->w_lsize_lnum[0])	/* may scroll down */
		{
			j = wp->w_lsize_lnum[0] - wp->w_topline;
			if (j < wp->w_height - 2)				/* not too far off */
			{
				lastline = wp->w_lsize_lnum[0] - 1;
				i = plines_m_win(wp, wp->w_topline, lastline);
				if (i < wp->w_height - 2)		/* less than a screen off */
				{
					/*
					 * Try to insert the correct number of lines.
					 * If not the last window, delete the lines at the bottom.
					 * win_ins_lines may fail.
					 */
					if (win_ins_lines(wp, 0, i, FALSE, wp == firstwin) == OK &&
													wp->w_lsize_valid)
					{
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
				else if (lastwin == firstwin)	/* far off: clearing the screen is faster */
					screenclear();
			}
			else if (lastwin == firstwin)	/* far off: clearing the screen is faster */
				screenclear();
		}
		else							/* may scroll up */
		{
			j = -1;
			for (i = 0; i < wp->w_lsize_valid; i++) /* try to find wp->w_topline in wp->w_lsize_lnum[] */
			{
				if (wp->w_lsize_lnum[i] == wp->w_topline)
				{
					j = i;
					break;
				}
				row += wp->w_lsize[i];
			}
			if (j == -1)	/* wp->w_topline is not in wp->w_lsize_lnum */
			{
				row = 0;
				if (lastwin == firstwin)
					screenclear();   /* far off: clearing the screen is faster */
			}
			else
			{
				/*
				 * Try to delete the correct number of lines.
				 * wp->w_topline is at wp->w_lsize_lnum[i].
				 */
				if ((row == 0 || win_del_lines(wp, 0, row, FALSE, wp == firstwin) == OK) && wp->w_lsize_valid)
				{
					srow = row;
					row = 0;
					for (;;)
					{
						if (type == VALID_TO_CURSCHAR && lnum == wp->w_cursor.lnum)
								break;
						if (row + srow + (int)wp->w_lsize[j] >= wp->w_height)
								break;
						wp->w_lsize[idx] = wp->w_lsize[j];
						wp->w_lsize_lnum[idx] = lnum++;

						row += wp->w_lsize[idx++];
						if ((int)++j >= wp->w_lsize_valid)
							break;
					}
					wp->w_lsize_valid = idx;
				}
				else
					row = 0;		/* update all lines */
			}
		}
		if (endrow == wp->w_height && idx == 0) 	/* no scrolling */
				wp->w_lsize_valid = 0;
	}

	done = didline = FALSE;
	screen_start();	/* init cursor position of screen_char() */

	if (VIsual.lnum)				/* check if we are updating the inverted part */
	{
		linenr_t	from, to;

	/* find the line numbers that need to be updated */
		if (wp->w_cursor.lnum < old_cursor.lnum)
		{
			from = wp->w_cursor.lnum;
			to = old_cursor.lnum;
		}
		else
		{
			from = old_cursor.lnum;
			to = wp->w_cursor.lnum;
		}
	/* if in block mode and changed column or wp->w_curswant: update all lines */
		if (Visual_block && (wp->w_cursor.col != old_cursor.col || wp->w_curswant != oldCurswant))
		{
			if (from > VIsual.lnum)
				from = VIsual.lnum;
			if (to < VIsual.lnum)
				to = VIsual.lnum;
		}

		if (from < wp->w_topline)
			from = wp->w_topline;
		if (to >= wp->w_botline)
			to = wp->w_botline - 1;

	/* find the minimal part to be updated */
		if (type == INVERTED)
		{
			while (lnum < from)						/* find start */
			{
				row += wp->w_lsize[idx++];
				++lnum;
			}
			srow = row;
			for (j = idx; j < wp->w_lsize_valid; ++j)	/* find end */
			{
				if (wp->w_lsize_lnum[j] == to + 1)
				{
					endrow = srow;
					break;
				}
				srow += wp->w_lsize[j];
			}
			old_cursor = wp->w_cursor;
			oldCurswant = wp->w_curswant;
		}
	/* if we update the lines between from and to set old_cursor */
		else if (lnum <= from && (endrow == wp->w_height || lastline >= to))
		{
			old_cursor = wp->w_cursor;
			oldCurswant = wp->w_curswant;
		}
	}

	(void)set_highlight('v');

	/*
	 * Update the screen rows from "row" to "endrow".
	 * Start at line "lnum" which is at wp->w_lsize_lnum[idx].
	 */
	for (;;)
	{
		if (lnum > wp->w_buffer->b_ml.ml_line_count)		/* hit the end of the file */
		{
			done = TRUE;
			break;
		}
		srow = row;
		row = win_line(wp, lnum, srow, endrow);
		if (row > endrow)	/* past end of screen */
		{
			wp->w_lsize[idx] = plines_win(wp, lnum);	/* we may need the size of that */
			wp->w_lsize_lnum[idx++] = lnum;		/* too long line later on */
			break;
		}

		wp->w_lsize[idx] = row - srow;
		wp->w_lsize_lnum[idx++] = lnum;
		if (++lnum > wp->w_buffer->b_ml.ml_line_count)
		{
			done = TRUE;
			break;
		}

		if (row == endrow)
		{
			didline = TRUE;
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
		else if (row > wp->w_height)		/* Need to blank out the last line */
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
			screen_fill(wp->w_winpos + wp->w_height - 1, wp->w_winpos + wp->w_height, 0, (int)Columns, '@', '@');
			wp->w_botline = lnum + 1;
		}
		else
		{
			/*
			 * Clear the rest of the screen and mark the unused lines.
			 */
			screen_fill(wp->w_winpos + srow, wp->w_winpos + wp->w_height, 0, (int)Columns, '@', ' ');
			wp->w_botline = lnum;
			wp->w_empty_rows = wp->w_height - srow;
		}
	}
	else
	{
		/* make sure the rest of the screen is blank */
		/* put '~'s on rows that aren't part of the file. */
		screen_fill(wp->w_winpos + row, wp->w_winpos + wp->w_height, 0, (int)Columns, '~', ' ');
		wp->w_empty_rows = wp->w_height - row;

		if (done)				/* we hit the end of the file */
			wp->w_botline = wp->w_buffer->b_ml.ml_line_count + 1;
		else
			wp->w_botline = lnum;
	}

	wp->w_redr_type = 0;
}

/*
 * mark all status lines for redraw; used after first :cd
 */
	void
status_redraw_all()
{
	WIN		*wp;

	for (wp = firstwin; wp; wp = wp->w_next)
		wp->w_redr_status = TRUE;
	updateScreen(NOT_VALID);
}

/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
	void
win_redr_status(wp)
	WIN		*wp;
{
	int		row;
	int		col;
	char_u	*p;
	int		len;
	int		fillchar;

	if (wp->w_status_height)					/* if there is a status line */
	{
		if (set_highlight('s') == OK)			/* can highlight */
		{
			fillchar = ' ';
			start_highlight();
		}
		else									/* can't highlight, use '=' */
			fillchar = '=';

		screen_start();			/* init cursor position */
		row = wp->w_winpos + wp->w_height;
		col = 0;
		p = wp->w_buffer->b_xfilename;
		if (p == NULL)
			p = (char_u *)"[No File]";
		else
		{
			home_replace(p, NameBuff, MAXPATHL);
			p = NameBuff;
		}
		len = STRLEN(p);
		if (wp->w_buffer->b_changed)
			len += 4;
		if (len > ru_col - 1)
		{
			screen_outchar('<', row, 0);
			p += len - (ru_col - 1) + 1;
			len = (ru_col - 1);
			col = 1;
		}
		screen_msg(p, row, col);
		if (wp->w_buffer->b_changed)
			screen_msg((char_u *)" [+]", row, len - 4);
		screen_fill(row, row + 1, len, ru_col, fillchar, fillchar);

		stop_highlight();
		win_redr_ruler(wp, TRUE);
	}
	else	/* no status line, can only be last window */
		redraw_cmdline = TRUE;
	wp->w_redr_status = FALSE;
}

/*
 * display line "lnum" of window 'wp' on the screen
 * Start at row "startrow", stop when "endrow" is reached.
 * Return the number of last row the line occupies.
 */

	static int
win_line(wp, lnum, startrow, endrow)
		WIN				*wp;
		linenr_t		lnum;
		int 			startrow;
		int 			endrow;
{
	char_u 			*screenp;
	int				c;
	int				col;				/* visual column on screen */
	long			vcol;				/* visual column for tabs */
	int				row;				/* row in the window, excluding w_winpos */
	int				screen_row;			/* row on the screen, including w_winpos */
	char_u			*ptr;
	char_u			extra[16];			/* "%ld" must fit in here */
	char_u			*p_extra;
	int 			n_extra;
	int				n_spaces = 0;

	int				fromcol, tocol;		/* start/end of inverting */
	int				noinvcur = FALSE;	/* don't invert the cursor */
	int				temp;
	FPOS			*top, *bot;

	row = startrow;
	screen_row = row + wp->w_winpos;
	col = 0;
	vcol = 0;
	fromcol = -10;
	tocol = MAXCOL;
	canopt = TRUE;
	if (VIsual.lnum && wp == curwin)			/* visual active in this window */
	{
		if (ltoreq(wp->w_cursor, VIsual))		/* Visual is after wp->w_cursor */
		{
			top = &wp->w_cursor;
			bot = &VIsual;
		}
		else							/* Visual is before wp->w_cursor */
		{
			top = &VIsual;
			bot = &wp->w_cursor;
		}
		if (Visual_block)						/* block mode */
		{
			if (lnum >= top->lnum && lnum <= bot->lnum)
			{
				fromcol = getvcol(wp, top, 2);
				temp = getvcol(wp, bot, 2);
				if (temp < fromcol)
					fromcol = temp;

				if (wp->w_curswant != MAXCOL)
				{
					tocol = getvcol(wp, top, 3);
					temp = getvcol(wp, bot, 3);
					if (temp > tocol)
						tocol = temp;
					++tocol;
				}
			}
		}
		else							/* non-block mode */
		{
			if (lnum > top->lnum && lnum <= bot->lnum)
				fromcol = 0;
			else if (lnum == top->lnum)
				fromcol = getvcol(wp, top, 2);
			if (lnum == bot->lnum)
				tocol = getvcol(wp, bot, 3) + 1;

			if (VIsual.col == VISUALLINE)		/* linewise */
			{
				if (fromcol > 0)
					fromcol = 0;
				tocol = VISUALLINE;
			}
		}
			/* if the cursor can't be switched off, don't invert the character
						where the cursor is */
		if ((T_CI == NULL || *T_CI == NUL) && lnum == wp->w_cursor.lnum)
			noinvcur = TRUE;

		if (tocol <= wp->w_leftcol)			/* inverting is left of screen */
			fromcol = 0;
		else if (fromcol >= 0 && fromcol < wp->w_leftcol)	/* start of invert is left of screen */
			fromcol = wp->w_leftcol;

		/* if inverting in this line, can't optimize cursor positioning */
		if (fromcol >= 0)
			canopt = FALSE;
	}

	ptr = ml_get_buf(wp->w_buffer, lnum, FALSE);
	if (!wp->w_p_wrap)		/* advance to first character to be displayed */
	{
		while (vcol < wp->w_leftcol && *ptr)
			vcol += chartabsize(*ptr++, vcol);
		if (vcol > wp->w_leftcol)
		{
			n_spaces = vcol - wp->w_leftcol;	/* begin with some spaces */
			vcol = wp->w_leftcol;
		}
	}
	screenp = LinePointers[screen_row];
	if (wp->w_p_nu)
	{
		sprintf((char *)extra, "%7ld ", (long)lnum);
		p_extra = extra;
		n_extra = 8;
		vcol -= 8;		/* so vcol is 0 when line number has been printed */
	}
	else
	{
		p_extra = NULL;
		n_extra = 0;
	}
	for (;;)
	{
		if (!canopt)	/* Visual in this line */
		{
			if (((vcol == fromcol && !(noinvcur && vcol == wp->w_virtcol)) ||
					(noinvcur && vcol == wp->w_virtcol + 1 && vcol >= fromcol)) &&
					vcol < tocol)
				start_highlight();		/* start highlighting */
			else if (invert && (vcol == tocol || (noinvcur && vcol == wp->w_virtcol)))
				stop_highlight();		/* stop highlighting */
		}

		/* Get the next character to put on the screen. */
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
			if ((c = *ptr++) < ' ' || (c > '~' && c <= 0xa0))
			{
				/*
				 * when getting a character from the file, we may have to turn it
				 * into something else on the way to putting it into 'Nextscreen'.
				 */
				if (c == TAB && !wp->w_p_list)
				{
					/* tab amount depends on current column */
					n_spaces = (int)wp->w_buffer->b_p_ts - vcol % (int)wp->w_buffer->b_p_ts - 1;
					c = ' ';
				}
				else if (c == NUL && wp->w_p_list)
				{
					p_extra = (char_u *)"";
					n_extra = 1;
					c = '$';
				}
				else if (c != NUL)
				{
					p_extra = transchar(c);
					n_extra = charsize(c) - 1;
					c = *p_extra++;
				}
			}
		}

		if (c == NUL)
		{
			if (invert)
			{
				if (vcol == 0)	/* invert first char of empty line */
				{
					if (*screenp != (' ' ^ INVERTCODE))
					{
							*screenp = (' ' ^ INVERTCODE);
							screen_char(screenp, screen_row, col);
					}
					++screenp;
					++col;
				}
				stop_highlight();
			}
			/* 
			 * blank out the rest of this row, if necessary
			 */
			while (col < Columns && *screenp == ' ')
			{
				++screenp;
				++col;
			}
			if (col < Columns)
			{
				screen_fill(screen_row, screen_row + 1, col, (int)Columns, ' ', ' ');
				col = Columns;
			}
			row++;
			screen_row++;
			break;
		}
		if (col >= Columns)
		{
			col = 0;
			++row;
			++screen_row;
			if (!wp->w_p_wrap)
				break;
			if (row == endrow)		/* line got too long for screen */
			{
				++row;
				break;
			}
			screenp = LinePointers[screen_row];
		}

		/*
		 * Store the character in Nextscreen.
		 * Be careful with characters where (c ^ INVERTCODE == ' '), they may be
		 * confused with spaces inserted by scrolling.
		 */
		if (*screenp != (c ^ invert) || c == (' ' ^ INVERTCODE))
		{
			*screenp = (c ^ invert);
			screen_char(screenp, screen_row, col);
		}
		++screenp;
		col++;
		vcol++;
	}

	if (invert)
		stop_highlight();
	return (row);
}

/*
 * output a single character directly to the screen
 * update NextScreen
 * Note: must do screen_start() before this!
 */
	void
screen_outchar(c, row, col)
	int		c;
	int		row, col;
{
	char_u		buf[2];

	buf[0] = c;
	buf[1] = NUL;
	screen_msg(buf, row, col);
}
	
/*
 * put string '*msg' on the screen at position 'row' and 'col'
 * update NextScreen
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: must do screen_start() before this!
 * Note: caller must make sure that row is valid!
 */
	void
screen_msg(msg, row, col)
	char_u	*msg;
	int		row;
	int		col;
{
	char_u	*screenp;

	screenp = LinePointers[row] + col;
	while (*msg && col < Columns)
	{
		if (*screenp != (*msg ^ invert) || *msg == (' ' ^ INVERTCODE))
		{
			*screenp = (*msg ^ invert);
			screen_char(screenp, row, col);
		}
		++screenp;
		++col;
		++msg;
	}
}

/*
 * last cursor position known by screen_char
 */
static int	oldrow, oldcol;		/* old cursor position */

/*
 * reset cursor position. Use whenever cursor moved before calling screen_char.
 */
	void
screen_start()
{
	oldcol = 9999;
}

/*
 * set_highlight - set highlight depending on 'highlight' option and context.
 *
 * return FAIL if highlighting is not possible, OK otherwise
 */
	int
set_highlight(context)
	int		context;
{
	int		len;
	int		i;
	int		mode;

	len = STRLEN(p_hl);
	for (i = 0; i < len; i += 3)
		if (p_hl[i] == context)
			break;
	if (i < len)
		mode = p_hl[i + 1];
	else
		mode = 'i';
	switch (mode)
	{
		case 'b':	highlight = T_TB;		/* bold */
					unhighlight = T_TP;
					break;
		case 's':	highlight = T_SO;		/* standout */
					unhighlight = T_SE;
					break;
		case 'n':	highlight = NULL;		/* no highlighting */
					unhighlight = NULL;
					break;
		default:	highlight = T_TI;		/* invert/reverse */
					unhighlight = T_TP;
					break;
	}
	if (highlight == NULL || *highlight == NUL ||
						unhighlight == NULL || *unhighlight == NUL)
	{
		highlight = NULL;
		return FAIL;
	}
	return OK;
}

	void
start_highlight()
{
	if (highlight != NULL)
	{
		outstr(highlight);
		invert = INVERTCODE;
	}
}

	void
stop_highlight()
{
	if (invert)
	{
		outstr(unhighlight);
		invert = 0;
	}
}

/*
 * put character '*p' on the screen at position 'row' and 'col'
 */
	static void
screen_char(p, row, col)
		char_u	*p;
		int 	row;
		int 	col;
{
	int			c;
	int			noinvcurs;

	/*
	 * Outputting the last character on the screen may scrollup the screen.
	 * Don't to it!
	 */
	if (row == Rows - 1 && col == Columns - 1)
		return;
	if (oldcol != col || oldrow != row)
	{
		/* check if no cursor movement is allowed in standout mode */
		if (invert && !p_wi && (T_MS == NULL || *T_MS == NUL))
			noinvcurs = 7;
		else
			noinvcurs = 0;

		/*
		 * If we're on the same row (which happens a lot!), try to
		 * avoid a windgoto().
		 * If we are only a few characters off, output the
		 * characters. That is faster than cursor positioning.
		 * This can't be used when inverting (a part of) the line.
		 */
		if (oldrow == row && oldcol < col)
		{
			register int i;

			i = col - oldcol;
			if (i <= 4 + noinvcurs && canopt)
			{
				while (i)
				{
					c = *(p - i--);
					outchar(c ^ invert);
				}
			}
			else
			{
				if (noinvcurs)
					stop_highlight();
			
				if (T_CRI && *T_CRI)	/* use tgoto interface! jw */
					OUTSTR(tgoto((char *)T_CRI, 0, i));
				else
					windgoto(row, col);
			
				if (noinvcurs)
					start_highlight();
			}
			oldcol = col;
		}
		else
		{
			if (noinvcurs)
				stop_highlight();
			windgoto(oldrow = row, oldcol = col);
			if (noinvcurs)
				start_highlight();
		}
	}
	/*
	 * For weird invert mechanism: output (un)highlight before every char
	 * Lots of extra output, but works.
	 */
	if (p_wi)
	{
		if (invert)                                      
			outstr(highlight);                            
		else                                             
			outstr(unhighlight);
	}
	outchar(*p ^ invert);
	oldcol++;
}

/*
 * Fill the screen from 'start_row' to 'end_row', from 'start_col' to 'end_col'
 * with character 'c1' in first column followed by 'c2' in the other columns.
 */
	void
screen_fill(start_row, end_row, start_col, end_col, c1, c2)
	int 	start_row, end_row;
	int		start_col, end_col;
	int		c1, c2;
{
	int				row;
	int				col;
	char_u			*screenp;
	int				did_delete = FALSE;
	int				c;

	if (start_row >= end_row || start_col >= end_col)	/* nothing to do */
		return;

	c1 ^= invert;
	c2 ^= invert;
	for (row = start_row; row < end_row; ++row)
	{
			/* try to use delete-line termcap code */
		if (c2 == ' ' && end_col == Columns && T_EL != NULL && *T_EL != NUL)
		{
			/*
			 * check if we really need to clear something
			 */
			col = start_col;
			screenp = LinePointers[row] + start_col;
			if (c1 != ' ')						/* don't clear first char */
			{
				++col;
				++screenp;
			}
			while (col < end_col && *screenp == ' ')	/* skip blanks */
			{
				++col;
				++screenp;
			}
			if (col < end_col)					/* something to be cleared */
			{
				windgoto(row, col);
				outstr(T_EL);
			}
			did_delete = TRUE;
		}

		screen_start();			/* init cursor position of screen_char() */
		screenp = LinePointers[row] + start_col;
		c = c1;
		for (col = start_col; col < end_col; ++col)
		{
			if (*screenp != c)
			{
				*screenp = c;
				if (!did_delete || c != ' ')
					screen_char(screenp, row, col);
			}
			++screenp;
			c = c2;
		}
		if (row == Rows - 1)
		{
			redraw_cmdline = TRUE;
			if (c1 == ' ' && c2 == ' ')
				clear_cmdline = FALSE;
		}
	}
}

/*
 * recompute all w_botline's. Called after Rows changed.
 */
	void
comp_Botline_all()
{
	WIN		*wp;

	for (wp = firstwin; wp; wp = wp->w_next)
		comp_Botline(wp);
}

/*
 * compute wp->w_botline. Can be called after wp->w_topline changed.
 */
	void
comp_Botline(wp)
	WIN			*wp;
{
	linenr_t	lnum;
	int			done = 0;

	for (lnum = wp->w_topline; lnum <= wp->w_buffer->b_ml.ml_line_count; ++lnum)
	{
		if ((done += plines_win(wp, lnum)) > wp->w_height)
			break;
	}
	wp->w_botline = lnum;		/* wp->w_botline is the line that is just below the window */
}

	static void
screenalloc(clear)
	int		clear;
{
	static int		old_Rows = 0;
	static int		old_Columns = 0;
	register int	i;
	WIN				*wp;
	int				outofmem = FALSE;

	/*
	 * Allocation of the screen buffers is done only when the size changes
	 * and when Rows and Columns have been set.
	 */
	if ((Nextscreen != NULL && Rows == old_Rows && Columns == old_Columns) || Rows == 0 || Columns == 0)
		return;

	comp_col();			/* recompute columns for shown command and ruler */
	old_Rows = Rows;
	old_Columns = Columns;

	/*
	 * If we're changing the size of the screen, free the old arrays
	 */
	free(Nextscreen);
	free(LinePointers);
	for (wp = firstwin; wp; wp = wp->w_next)
		win_free_lsize(wp);

	Nextscreen = (char_u *)malloc((size_t) (Rows * Columns));
	LinePointers = (char_u **)malloc(sizeof(char_u *) * Rows);
	for (wp = firstwin; wp; wp = wp->w_next)
	{
		if (win_alloc_lsize(wp) == FAIL)
		{
			outofmem = TRUE;
			break;
		}
	}

	if (Nextscreen == NULL || LinePointers == NULL || outofmem)
	{
		emsg(e_outofmem);
		free(Nextscreen);
		Nextscreen = NULL;
	}
	else
	{
		for (i = 0; i < Rows; ++i)
			LinePointers[i] = Nextscreen + i * Columns;
	}

	if (clear)
		screenclear2();
}

	void
screenclear()
{
	screenalloc(FALSE);			/* allocate screen buffers if size changed */
	screenclear2();
}

	static void
screenclear2()
{
	if (starting || Nextscreen == NULL)
		return;

	outstr(T_ED);				/* clear the display */

								/* blank out Nextscreen */
	memset((char *)Nextscreen, ' ', (size_t)(Rows * Columns));

	win_rest_invalid(firstwin);
	clear_cmdline = FALSE;
	if (must_redraw == CLEAR)		/* no need to clear again */
		must_redraw = NOT_VALID;
}

/*
 * check cursor for a valid lnum
 */
	void
check_cursor()
{
	if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	if (curwin->w_cursor.lnum <= 0)
		curwin->w_cursor.lnum = 1;
}

	void
cursupdate()
{
	linenr_t		p;
	long 			nlines;
	int 			i;
	int 			temp;

	screenalloc(TRUE);		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return;

	check_cursor();
	if (bufempty()) 			/* special case - file is empty */
	{
		curwin->w_topline = 1;
		curwin->w_cursor.lnum = 1;
		curwin->w_cursor.col = 0;
		curwin->w_lsize[0] = 0;
		if (curwin->w_lsize_valid == 0)		/* don't know about screen contents */
			updateScreen(NOT_VALID);
		curwin->w_lsize_valid = 1;
	}
	else if (curwin->w_cursor.lnum < curwin->w_topline)
	{
/*
 * If the cursor is above the top of the screen, scroll the screen to
 * put it at the top of the screen.
 * If we weren't very close to begin with, we scroll more, so that
 * the line is close to the middle.
 */
		temp = curwin->w_height / 2 - 1;
		if (temp < 2)
			temp = 2;
		if (curwin->w_topline - curwin->w_cursor.lnum >= temp)		/* not very close */
		{
			p = curwin->w_cursor.lnum;
			i = plines(p);
			temp += i;
								/* count lines for 1/2 screenheight */
			while (i < curwin->w_height + 1 && i < temp && p > 1)
				i += plines(--p);
			curwin->w_topline = p;
			if (i > curwin->w_height)		/* cursor line won't fit, backup one line */
				++curwin->w_topline;
		}
		else if (p_sj > 1)		/* scroll at least p_sj lines */
		{
			for (i = 0; i < p_sj && curwin->w_topline > 1; i += plines(--curwin->w_topline))
				;
		}
		if (curwin->w_topline > curwin->w_cursor.lnum)
			curwin->w_topline = curwin->w_cursor.lnum;
		updateScreen(VALID);
	}
	else if (curwin->w_cursor.lnum >= curwin->w_botline)
	{
/*
 * If the cursor is below the bottom of the screen, scroll the screen to
 * put the cursor on the screen.
 * If the cursor is less than a screenheight down
 * compute the number of lines at the top which have the same or more
 * rows than the rows of the lines below the bottom
 */
		nlines = curwin->w_cursor.lnum - curwin->w_botline + 1;
		if (nlines <= curwin->w_height + 1)
		{
				/* get the number or rows to scroll minus the number of
								free '~' rows */
			temp = plines_m(curwin->w_botline, curwin->w_cursor.lnum) - curwin->w_empty_rows;
			if (temp <= 0)				/* curwin->w_empty_rows is larger, no need to scroll */
				nlines = 0;
			else if (temp > curwin->w_height)		/* more than a screenfull, don't scroll */
				nlines = temp;
			else
			{
					/* scroll minimal number of lines */
				if (temp < p_sj)
					temp = p_sj;
				for (i = 0, p = curwin->w_topline; i < temp && p < curwin->w_botline; ++p)
					i += plines(p);
				if (i >= temp)				/* it's possible to scroll */
					nlines = p - curwin->w_topline;
				else						/* below curwin->w_botline, don't scroll */
					nlines = 9999;
			}
		}

		/*
		 * Scroll up if the cursor is off the bottom of the screen a bit.
		 * Otherwise put it at 1/2 of the screen.
		 */
		if (nlines >= curwin->w_height / 2 && nlines > p_sj)
		{
			p = curwin->w_cursor.lnum;
			temp = curwin->w_height / 2 + 1;
			nlines = 0;
			i = 0;
			do				/* this loop could win a contest ... */
				i += plines(p);
			while (i < temp && (nlines = 1) != 0 && --p != 0);
			curwin->w_topline = p + nlines;
		}
		else
			scrollup(nlines);
		updateScreen(VALID);
	}
	else if (curwin->w_lsize_valid == 0)		/* don't know about screen contents */
		updateScreen(NOT_VALID);
	curwin->w_row = curwin->w_col = curwin->w_virtcol = i = 0;
	for (p = curwin->w_topline; p != curwin->w_cursor.lnum; ++p)
		if (RedrawingDisabled)		/* curwin->w_lsize[] invalid */
			curwin->w_row += plines(p);
		else
			curwin->w_row += curwin->w_lsize[i++];

	Cline_row = curwin->w_row;
	if (!RedrawingDisabled && i > curwin->w_lsize_valid)
								/* Should only happen with a line that is too */
								/* long to fit on the last screen line. */
		Cline_height = 0;
	else
	{
		if (RedrawingDisabled)      		/* curwin->w_lsize[] invalid */
		    Cline_height = plines(curwin->w_cursor.lnum);
        else
			Cline_height = curwin->w_lsize[i];

		curs_columns(!RedrawingDisabled);	/* compute curwin->w_virtcol and curwin->w_col */
		if (must_redraw)
			updateScreen(must_redraw);
	}

	if (curwin->w_set_curswant)
	{
		curwin->w_curswant = curwin->w_virtcol;
		curwin->w_set_curswant = FALSE;
	}
}

/*
 * compute curwin->w_col and curwin->w_virtcol
 */
	void
curs_columns(scroll)
	int scroll;			/* when TRUE, may scroll horizontally */
{
	int diff;

	curwin->w_virtcol = getvcol(curwin, &curwin->w_cursor, 1);
	curwin->w_col = curwin->w_virtcol;
	if (curwin->w_p_nu)
		curwin->w_col += 8;

	curwin->w_row = Cline_row;
	if (curwin->w_p_wrap)			/* long line wrapping, adjust curwin->w_row */
		while (curwin->w_col >= Columns)
		{
			curwin->w_col -= Columns;
			curwin->w_row++;
		}
	else if (scroll)	/* no line wrapping, compute curwin->w_leftcol if scrolling is on */
						/* if scrolling is off, curwin->w_leftcol is assumed to be 0 */
	{
						/* If Cursor is in columns 0, start in column 0 */
						/* If Cursor is left of the screen, scroll rightwards */
						/* If Cursor is right of the screen, scroll leftwards */
		if (curwin->w_cursor.col == 0)
		{
						/* screen has to be redrawn with new curwin->w_leftcol */
			if (curwin->w_leftcol != 0 && must_redraw < NOT_VALID)
				must_redraw = NOT_VALID;
			curwin->w_leftcol = 0;
		}
		else if (((diff = curwin->w_leftcol + (curwin->w_p_nu ? 8 : 0)
					- curwin->w_col) > 0 ||
					(diff = curwin->w_col - (curwin->w_leftcol + Columns) + 1) > 0))
		{
			if (p_ss == 0 || diff >= Columns / 2)
				curwin->w_leftcol = curwin->w_col - Columns / 2;
			else
			{
				if (diff < p_ss)
					diff = p_ss;
				if (curwin->w_col < curwin->w_leftcol + 8)
					curwin->w_leftcol -= diff;
				else
					curwin->w_leftcol += diff;
			}
			if (curwin->w_leftcol < 0)
				curwin->w_leftcol = 0;
			if (must_redraw < NOT_VALID)
				must_redraw = NOT_VALID;	/* screen has to be redrawn with new curwin->w_leftcol */
		}
		curwin->w_col -= curwin->w_leftcol;
	}
	if (curwin->w_row > curwin->w_height - 1)	/* Cursor past end of screen */
		curwin->w_row = curwin->w_height - 1;	/* happens with line that does not fit on screen */
}

/*
 * get virtual column number of pos
 * type = 1: where the cursor is on this character
 * type = 2: on the first position of this character (TAB)
 * type = 3: on the last position of this character (TAB)
 */
	int
getvcol(wp, pos, type)
	WIN		*wp;
	FPOS	*pos;
	int		type;
{
	int				col;
	int				vcol;
	char_u		   *ptr;
	int 			incr;
	int				c;

	vcol = 0;
	ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
	for (col = pos->col; col >= 0; --col)
	{
		c = *ptr++;
		if (c == NUL)		/* make sure we don't go past the end of the line */
			break;

		/* A tab gets expanded, depending on the current column */
		incr = chartabsize(c, (long)vcol);

		if (col == 0)		/* character at pos.col */
		{
			if (type == 3 || (type == 1 && c == TAB && State == NORMAL && !wp->w_p_list))
				--incr;
			else
				break;
		}
		vcol += incr;
	}
	return vcol;
}

	void
scrolldown(nlines)
	long	nlines;
{
	register long	done = 0;	/* total # of physical lines done */

	/* Scroll up 'nlines' lines. */
	while (nlines--)
	{
		if (curwin->w_topline == 1)
			break;
		done += plines(--curwin->w_topline);
	}
	/*
	 * Compute the row number of the last row of the cursor line
	 * and move it onto the screen.
	 */
	curwin->w_row += done;
	if (curwin->w_p_wrap)
		curwin->w_row += plines(curwin->w_cursor.lnum) - 1 - curwin->w_virtcol / Columns;
	while (curwin->w_row >= curwin->w_height && curwin->w_cursor.lnum > 1)
		curwin->w_row -= plines(curwin->w_cursor.lnum--);
}

	void
scrollup(nlines)
	long	nlines;
{
#ifdef NEVER
	register long	done = 0;	/* total # of physical lines done */

	/* Scroll down 'nlines' lines. */
	while (nlines--)
	{
		if (curwin->w_topline == curbuf->b_ml.ml_line_count)
			break;
		done += plines(curwin->w_topline);
		if (curwin->w_cursor.lnum == curwin->w_topline)
			++curwin->w_cursor.lnum;
		++curwin->w_topline;
	}
	win_del_lines(curwin, 0, done, TRUE, TRUE);
#endif
	curwin->w_topline += nlines;
	if (curwin->w_topline > curbuf->b_ml.ml_line_count)
		curwin->w_topline = curbuf->b_ml.ml_line_count;
	if (curwin->w_cursor.lnum < curwin->w_topline)
		curwin->w_cursor.lnum = curwin->w_topline;
}

/*
 * insert 'nlines' lines at 'row' in window 'wp'
 * if 'invalid' is TRUE the wp->w_lsize_lnum[] is invalidated.
 * if 'mayclear' is TRUE the screen will be cleared if it is faster than scrolling
 * Returns FAIL if the lines are not inserted, OK for success.
 */
	int
win_ins_lines(wp, row, nlines, invalid, mayclear)
	WIN		*wp;
	int		row;
	int		nlines;
	int		invalid;
	int		mayclear;
{
	int		did_delete;
	int		nextrow;
	int		lastrow;
	int		retval;

	if (invalid)
		wp->w_lsize_valid = 0;

	if (RedrawingDisabled || nlines <= 0 || wp->w_height < 5)
		return FAIL;
	
	if (nlines > wp->w_height - row)
		nlines = wp->w_height - row;

	if (mayclear && Rows - nlines < 5)	/* only a few lines left: redraw is faster */
	{
		screenclear();		/* will set wp->w_lsize_valid to 0 */
		return FAIL;
	}

	if (nlines == wp->w_height)	/* will delete all lines */
		return FAIL;

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
		scroll_region_set(wp);
		retval = screen_ins_lines(wp->w_winpos, row, nlines, wp->w_height);
		scroll_region_reset();
		return retval;
	}

	if (wp->w_next && p_tf)		/* don't delete/insert on fast terminal */
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
		if (screen_del_lines(0, wp->w_winpos + wp->w_height - nlines, nlines, (int)Rows) == OK)
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
		lastrow = nextrow + nlines;
		if (lastrow > Rows)
			lastrow = Rows;
		screen_fill(nextrow - nlines, lastrow - nlines, 0, (int)Columns, ' ', ' ');
	}

	if (screen_ins_lines(0, wp->w_winpos + row, nlines, (int)Rows) == FAIL)
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
 * delete 'nlines' lines at 'row' in window 'wp'
 * If 'invalid' is TRUE curwin->w_lsize_lnum[] is invalidated.
 * If 'mayclear' is TRUE the screen will be cleared if it is faster than scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
	int
win_del_lines(wp, row, nlines, invalid, mayclear)
	WIN				*wp;
	int 			row;
	int 			nlines;
	int				invalid;
	int				mayclear;
{
	int			retval;

	if (invalid)
		wp->w_lsize_valid = 0;

	if (RedrawingDisabled || nlines <= 0)
		return FAIL;
	
	if (nlines > wp->w_height - row)
		nlines = wp->w_height - row;

	if (mayclear && Rows - nlines < 5)	/* only a few lines left: redraw is faster */
	{
		screenclear();		/* will set wp->w_lsize_valid to 0 */
		return FAIL;
	}

	if (nlines == wp->w_height)	/* will delete all lines */
		return FAIL;

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
		scroll_region_set(wp);
		retval = screen_del_lines(wp->w_winpos, row, nlines, wp->w_height);
		scroll_region_reset();
		return retval;
	}

	if (wp->w_next && p_tf)		/* don't delete/insert on fast terminal */
		return FAIL;

	if (screen_del_lines(0, wp->w_winpos + row, nlines, (int)Rows) == FAIL)
		return FAIL;

	/*
	 * If there are windows or status lines below, try to put them at the
	 * correct place. If we can't do that, they have to be redrawn.
	 */
	if (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
	{
		if (screen_ins_lines(0, wp->w_winpos + wp->w_height - nlines, nlines, (int)Rows) == FAIL)
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
	WIN			*wp;
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
 * insert lines on the screen and update Nextscreen
 * 'end' is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used 'off' is the offset from the top for the region.
 * 'row' and 'end' are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
	static int
screen_ins_lines(off, row, nlines, end)
	int			off;
	int 		row;
	int 		nlines;
	int			end;
{
	int 		i;
	int 		j;
	char_u		*temp;
	int			cursor_row;

	if (T_CSC != NULL && *T_CSC != NUL)		/* cursor relative to region */
		cursor_row = row;
	else
		cursor_row = row + off;

	screenalloc(TRUE);		/* allocate screen buffers if size changed */
	if (Nextscreen == NULL)
		return FAIL;

	if (nlines <= 0 ||  ((T_CIL == NULL || *T_CIL == NUL) &&
						(T_IL == NULL || *T_IL == NUL) &&
						(T_SR == NULL || *T_SR == NUL || row != 0)))
		return FAIL;
	
	/*
	 * It "looks" better if we do all the inserts at once
	 */
    if (T_CIL && *T_CIL) 
    {
        windgoto(cursor_row, 0);
		if (nlines == 1 && T_IL && *T_IL)
			outstr(T_IL);
		else
			OUTSTR(tgoto((char *)T_CIL, 0, nlines));
    }
    else
    {
        for (i = 0; i < nlines; i++) 
        {
            if (i == 0 || cursor_row != 0)
				windgoto(cursor_row, 0);
			if (T_IL && *T_IL)
				outstr(T_IL);
			else
				outstr(T_SR);
        }
    }
	/*
	 * Now shift LinePointers nlines down to reflect the inserted lines.
	 * Clear the inserted lines.
	 */
	row += off;
	end += off;
	for (i = 0; i < nlines; ++i)
	{
		j = end - 1 - i;
		temp = LinePointers[j];
		while ((j -= nlines) >= row)
				LinePointers[j + nlines] = LinePointers[j];
		LinePointers[j + nlines] = temp;
		memset((char *)temp, ' ', (size_t)Columns);
	}
	return OK;
}

/*
 * delete lines on the screen and update Nextscreen
 * 'end' is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used 'off' is the offset from the top for the region.
 * 'row' and 'end' are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
	int
screen_del_lines(off, row, nlines, end)
	int				off;
	int 			row;
	int 			nlines;
	int				end;
{
	int 		j;
	int 		i;
	char_u		*temp;
	int			cursor_row;
	int			cursor_end;

	if (T_CSC != NULL && *T_CSC != NUL)		/* cursor relative to region */
	{
		cursor_row = row;
		cursor_end = end;
	}
	else
	{
		cursor_row = row + off;
		cursor_end = end + off;
	}

	screenalloc(TRUE);		/* allocate screen buffers if size changed */
	if (Nextscreen == NULL)
		return FAIL;

	if (nlines <= 0 ||  ((T_DL == NULL || *T_DL == NUL) &&
						(T_CDL == NULL || *T_CDL == NUL) &&
						row != 0))
		return FAIL;

	/* delete the lines */
	if (T_CDL && *T_CDL) 
	{
		windgoto(cursor_row, 0);
		if (nlines == 1 && T_DL && *T_DL)
			outstr(T_DL);
		else
			OUTSTR(tgoto((char *)T_CDL, 0, nlines));
	} 
	else
	{
		if (row == 0)
		{
			windgoto(cursor_end - 1, 0);
			for (i = 0; i < nlines; i++) 
				outchar('\n');
		}
		else
		{
			for (i = 0; i < nlines; i++) 
			{
				windgoto(cursor_row, 0);
				outstr(T_DL);           /* delete a line */
			}
		}
	}

	/*
	 * Now shift LinePointers nlines up to reflect the deleted lines.
	 * Clear the deleted lines.
	 */
	row += off;
	end += off;
	for (i = 0; i < nlines; ++i)
	{
		j = row + i;
		temp = LinePointers[j];
		while ((j += nlines) <= end - 1)
			LinePointers[j - nlines] = LinePointers[j];
		LinePointers[j - nlines] = temp;
		memset((char *)temp, ' ', (size_t)Columns);
	}
	return OK;
}

/*
 * show the current mode and ruler
 *
 * If clear_cmdline is TRUE, clear it first.
 * If clear_cmdline is FALSE there may be a message there that needs to be
 * cleared only if a mode is shown.
 */
	void
showmode()
{
	int		did_clear = clear_cmdline;
	int		need_clear = FALSE;

	if ((p_smd && (State & INSERT)) || Recording)
	{
		gotocmdline(clear_cmdline, NUL);
		if (p_smd)
		{
			if (State & INSERT)
			{
				msg_outstr((char_u *)"-- ");
				if (p_ri)
					msg_outstr((char_u *)"REVERSE ");
				if (State == INSERT)
					msg_outstr((char_u *)"INSERT --");
				else
					msg_outstr((char_u *)"REPLACE --");
				need_clear = TRUE;
			}
		}
		if (Recording)
		{
			msg_outstr((char_u *)"recording");
			need_clear = TRUE;
		}
		if (need_clear && !did_clear)
			msg_ceol();
	}
	win_redr_ruler(lastwin, TRUE);
	redraw_cmdline = FALSE;
}

/*
 * delete mode message
 */
	void
delmode()
{
	if (Recording)
		MSG("recording");
	else
		MSG("");
}

/*
 * if ruler option is set: show current cursor position
 * if always is FALSE, only print if position has changed
 */
	void
showruler(always)
	int		always;
{
	win_redr_ruler(curwin, always);
}

	void
win_redr_ruler(wp, always)
	WIN		*wp;
	int		always;
{
	static linenr_t	oldlnum = 0;
	static colnr_t	oldcol = 0;
	char_u			buffer[30];
	int				row;
	int				fillchar;

	if (p_ru && (redraw_cmdline || always || wp->w_cursor.lnum != oldlnum || wp->w_virtcol != oldcol))
	{
		cursor_off();
		if (wp->w_status_height)
		{
			row = wp->w_winpos + wp->w_height;
			if (set_highlight('s') == OK)		/* can use highlighting */
			{
				fillchar = ' ';
				start_highlight();
			}
			else
				fillchar = '=';
		}
		else
		{
			row = Rows - 1;
			fillchar = ' ';
		}
		/*
		 * Some sprintfs return the lenght, some return a pointer.
		 * To avoid portability problems we use strlen here.
		 */
		sprintf((char *)buffer, "%ld,%d", wp->w_cursor.lnum, (int)wp->w_cursor.col + 1);
		if (wp->w_cursor.col != wp->w_virtcol)
			sprintf((char *)buffer + STRLEN(buffer), "-%d", wp->w_virtcol + 1);

		screen_start();			/* init cursor position */
		screen_msg(buffer, row, ru_col);
		screen_fill(row, row + 1, ru_col + (int)STRLEN(buffer), (int)Columns, fillchar, fillchar);
		oldlnum = wp->w_cursor.lnum;
		oldcol = wp->w_virtcol;
		stop_highlight();
	}
}

/*
 * screen_valid: Returns TRUE if there is a valid screen to write to.
 * 				 Returns FALSE when starting up and screen not initialized yet.
 * Used by msg() to decide to use either screen_msg() or printf().
 */
	int
screen_valid()
{
	screenalloc(FALSE);		/* allocate screen buffers if size changed */
	return (Nextscreen != NULL);
}
