/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * screen.c: code for displaying on the screen
 */

#include "vim.h"

static char 	*Nextscreen = NULL; 	/* What's to be put on the screen. */
static int		 NumLineSizes = 0;		/* # of active LineSizes */
static linenr_t *LineNumbers = NULL;	/* Pointer to the line for LineSizes */
static char 	*LineSizes = NULL;		/* Size of a line (pline output) */
static char 	**LinePointers = NULL;	/* array of pointers into Netscreen */

/*
 * The following variable is set (in cursupdate) to the number of physical
 * lines taken by the line the cursor is on. We use this to avoid extra calls
 * to plines(). The optimized routine updateline()
 * makes sure that the size of the cursor line hasn't changed. If so, lines below
 * the cursor will move up or down and we need to call the routine
 * updateScreen() to examine the entire screen.
 */
static int		Cline_size; 			/* size (in rows) of the cursor line */
static int		Cline_row;				/* starting row of the cursor line */
static int		redraw_msg = TRUE;		/* TRUE when "insert mode" needs updating */

static int screenline __ARGS((linenr_t, int, int));
static void screenchar __ARGS((int, int, int));
static void screenfill __ARGS((int, int));

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

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL || RedrawingDisabled)
		return;

	screenchar(' ', -1, -1);	/* init cursor position of screenchar() */
#ifdef AUX
	if (!Aux_Device)
#endif
		outstr(T_CI);				/* disable cursor */

	row = screenline(Curpos.lnum, Cline_row, Rows - 1);

#ifdef AUX
	if (!Aux_Device)
#endif
		outstr(T_CV);				/* enable cursor again */

	n = row - Cline_row;
	if (n != Cline_size)		/* line changed size */
	{
		if (n < Cline_size) 	/* got smaller: delete lines */
				s_del(row, Cline_size - n, (bool_t)FALSE);
		else					/* got bigger: insert lines */
				s_ins(Cline_row + Cline_size, n - Cline_size, (bool_t)FALSE);

		updateScreen(VALID_TO_CURSCHAR);
	}
}

/*
 * updateScreen()
 *
 * Based on the current value of Topline, transfer a screenfull of stuff from
 * Filemem to Nextscreen, and update Botline.
 */

	void
updateScreen(type)
	int 			type;
{
	register int	row;
	register int	endrow;
	linenr_t		lnum;
	bool_t			done;		/* if TRUE, we hit the end of the file */
	bool_t			didline;	/* if TRUE, we finished the last line */
	int 			srow;		/* starting row of the current line */
	int 			idx;
	int 			i;
	long 			j;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL || RedrawingDisabled)
		return;

	if (NumLineSizes == 0)
		type = NOT_VALID;
	if (type == VALID && Topline == LineNumbers[0]) 	/* nothing to do */
		return;

	if (type == NOT_VALID)
	{
		redraw_msg = TRUE;
		NumLineSizes = 0;
	}

	idx = 0;
	row = 0;
	lnum = Topline;
#ifdef AUX
	if (!Aux_Device)
#endif
	outstr(T_CI);				/* disable cursor */

	/* The number of rows shown is Rows-1. */
	/* The default last row is the status/command line. */
	endrow = Rows - 1;

	if (type == VALID || type == VALID_TO_CURSCHAR)
	{
		/*
		 * We handle two special cases:
		 * 1: we are off the top of the screen by a few lines: scroll down
		 * 2: Topline is below LineNumbers[0]: may scroll up
		 */
		if (Topline < LineNumbers[0])	/* may scroll down */
		{
				j = LineNumbers[0] - Topline;
				if (j < Rows - 3)				/* not too far off */
				{
						i = plines_m(Topline, (linenr_t)(LineNumbers[0] - 1));
						if (i < Rows - 3)		/* less than a screen off */
						{
							s_ins(0, i, (bool_t)FALSE);

							endrow = i;

							if ((NumLineSizes += j) > Rows - 1)
								NumLineSizes = Rows - 1;
							for (idx = NumLineSizes; idx - j >= 0; idx--)
							{
								LineNumbers[idx] = LineNumbers[idx - j];
								LineSizes[idx] = LineSizes[idx - j];
							}
							idx = 0;
						}
				}
		}
		else							/* may scroll up */
		{
				j = -1;
				for (i = 0; i < NumLineSizes; i++) /* try to find Topline in LineNumbers[] */
				{
					if (LineNumbers[i] == Topline)
					{
						j = i;
						break;
					}
					row += LineSizes[i];
				}
				if (j == -1)	/* Topline is not in LineNumbers */
				{
					row = 0;
				}
				else
				{
					if (row)	/* Topline is at LineNumbers[i] */
						s_del(0, row, (bool_t)FALSE);
					srow = row;
					row = 0;
					for (;;)
					{
						if (type == VALID_TO_CURSCHAR && lnum == Curpos.lnum)
								break;
						if (row + srow + LineSizes[j] >= Rows - 1)
								break;
						LineSizes[idx] = LineSizes[j];
						LineNumbers[idx] = lnum++;

						row += LineSizes[idx++];
						if (++j >= NumLineSizes)
							break;
					}
					NumLineSizes = idx;
				}
		}
		if (endrow == Rows - 1 && idx == 0) 	/* no scrolling */
				NumLineSizes = 0;
	}

	done = didline = FALSE;
	screenchar(' ', -1, -1);	/* init cursor position of screenchar() */

	/*
	 * Update the screen rows from "row" to "endrow".
	 * Start at line "lnum" which is at LineNumbers[idx].
	 */
	for (;;)
	{
			if (lnum > line_count)		/* hit the end of the file */
			{
				done = TRUE;
				break;
			}
			srow = row;
			row = screenline(lnum, srow, endrow);
			if (row > endrow)	/* past end of screen */
			{
				LineSizes[idx] = plines(lnum);	/* we may need the size of that */
				LineNumbers[idx++] = lnum;		/* too long line later on */
				break;
			}

			LineSizes[idx] = row - srow;
			LineNumbers[idx++] = lnum;
			if (++lnum > line_count)
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
	if (idx > NumLineSizes)
		NumLineSizes = idx;

	/* Do we have to do off the top of the screen processing ? */
	if (endrow != Rows - 1)
	{
		row = 0;
		for (idx = 0; idx < NumLineSizes && row < (Rows - 1); idx++)
			row += LineSizes[idx];

		if (row < (Rows - 1))
		{
			done = TRUE;
		}
		else if (row > (Rows - 1))		/* Need to blank out the last line */
		{
			lnum = LineNumbers[idx - 1];
			srow = row - LineSizes[idx - 1];
			didline = FALSE;
		}
		else
		{
			lnum = LineNumbers[idx - 1] + 1;
			didline = TRUE;
		}
	}

	/*
	 * If we didn't hit the end of the file, and we didn't finish the last
	 * line we were working on, then the line didn't fit.
	 */
	if (!done && !didline)
	{
		/*
		 * Clear the rest of the screen and mark the unused lines.
		 */
		screenfill(srow, '@');

		Botline = lnum;
	}
	else
	{
		/* make sure the rest of the screen is blank */
		/* put '~'s on rows that aren't part of the file. */
		screenfill(row, '~');

		if (done)				/* we hit the end of the file */
			Botline = line_count + 1;
		else
			Botline = lnum; 	/* FIX - prev? */
	}

	if (redraw_msg && P(P_MO))
	{
		showmode();
		redraw_msg = FALSE;
	}

#ifdef AUX
	if (!Aux_Device)
#endif
	outstr(T_CV);				/* enable cursor again */
}

/*
 * Move line "lnum" to the screen.
 * Start at row "startrow", stop when "endrow" is reached.
 * Return the number of rows the line occupies.
 */
	static int
screenline(lnum, startrow, endrow)
		linenr_t		lnum;
		int 			startrow;
		int 			endrow;
{
	register u_char  *screenp;
	register u_char   c;
	register int	col;
	register int	vcol;				/* virtual column for tabs */
	register int	row;
	register u_char *ptr;
	char			extra[16];
	char			*p_extra;
	int 			n_extra;

	row = startrow;
	col = 0;
	vcol = 0;
	ptr = (u_char *)nr2ptr(lnum);
	screenp = (u_char *)LinePointers[row];
	if (P(P_NU))
	{
		sprintf(extra, "%7ld ", (long)lnum);
		p_extra = extra;
		n_extra = 8;
		vcol = -8;		/* so vcol is 0 when line number has been printed */

	}
	else
	{
		p_extra = NULL;
		n_extra = 0;
	}
	for (;;)
	{
		/* Get the next character to put on the screen. */

		/*
		 * The 'extra' array contains the extra stuff that is inserted to
		 * represent special characters (tabs, and other non-printable stuff.
		 * The order in the 'extra' array is reversed.
		 */

		if (n_extra > 0)
		{
			c = (u_char)*p_extra++;
			n_extra--;
		}
		else if ((c = *ptr++) < ' ' || c > '~' && c < 0xa0)
		{
			/*
			 * when getting a character from the file, we may have to turn it
			 * into something else on the way to putting it into 'Nextscreen'.
			 */
			if (c == TAB && !P(P_LS))
			{
				p_extra = "               ";
				/* tab amount depends on current column */
				n_extra = (int)P(P_TS) - vcol % (int)P(P_TS) - 1;
				c = ' ';
			}
			else if (c == NUL && P(P_LS))
			{
				extra[0] = NUL;
				p_extra = extra;
				n_extra = 1;
				c = '$';
			}
			else if (c != NUL)
			{
				p_extra = (char *)transchar(c);
				c = (u_char)*p_extra++;
				n_extra = 1;
			}
		}

		if (c == NUL)
		{
			/* blank out the rest of this row */
			while (col < Columns)
			{
				if (*screenp != ' ')
				{
						screenchar(' ', row, col);
						*screenp = ' ';
				}
				++screenp;
				++col;
			}
			row++;
			break;
		}
		if (col >= Columns)
		{
			col = 0;
			if (++row == endrow)		/* line got too long for screen */
			{
				++row;
				break;
			}
			screenp = (u_char *)LinePointers[row];
		}
		/* store the character in Nextscreen */
		if (*screenp != c)
				screenchar(*screenp = c, row, col);
		++screenp;
		col++;
		vcol++;
	}

	return (row);
}

/*
 * put character 'c' on the screen at position 'row' and 'col'
 */
	static void
screenchar(c, row, col)
		int		c;
		int 	row;
		int 	col;
{
		static int oldrow, oldcol;		/* old cursor position */

		if (row == -1)					/* initialize cursor position */
		{
				oldrow = oldcol = -1;
				return;
		}
		if (oldcol != col || oldrow != row)
		{
				/*
				 * If we're on the same row, don't do a windgoto()
				 * (this happens a lot!)
				 */
				if (oldrow == row && oldcol < col)
				{
					register int i;

#ifdef AMIGA
# ifdef AUX
					if (Aux_Device)
						outstr("\033[");
					else
# endif /* AUX */
						outchar(CSI);
#else
					outstr("\033[");
#endif /* AMIGA */
					i = col - oldcol;
					if (i > 1)
						outnum((long)i);
					outchar('C');
					oldcol = col;
				}
				else
					windgoto(oldrow = row, oldcol = col);
		}
		outchar(c);
		oldcol++;
}

/*
 * Fill the screen at 'srow' with character 'c' followed by blanks.
 */
	static void
screenfill(srow, c)
		int 	srow;
		int		c;
{
		register int row;
		register int col;
		register char *screenp;

		for (row = srow; row < (Rows - 1); ++row)
		{
			screenp = LinePointers[row];
			if (*screenp != c)
				screenchar(c, row, 0);
			*screenp++ = c;
			for (col = 1; col < Columns; ++col)
			{
				if (*screenp != ' ')
				{
						screenchar(' ', row, col);
						*screenp = ' ';
				}
				++screenp;
			}
		}
}

/*
 * prt_line() - print the given line
 */
	void
prt_line(s)
	char		   *s;
{
	register int	si = 0;
	register char	 c;
	register int	col = 0;

	char			extra[16];
	int 			n_extra = 0;
	int 			n;

	for (;;) {

		if (n_extra > 0)
			c = extra[--n_extra];
		else {
			c = s[si++];
			if (c == TAB && !P(P_LS)) {
				strcpy(extra, "                ");
				/* tab amount depends on current column */
				n_extra = (P(P_TS) - 1) - col % P(P_TS);
				c = ' ';
			} else if (c == NUL && P(P_LS)) {
				extra[0] = NUL;
				n_extra = 1;
				c = '$';
			} else if (c != NUL && (n = charsize(c)) > 1) {
				char			 *p;

				n_extra = 0;
				p = transchar(c);
				/* copy 'ch-str'ing into 'extra' in reverse */
				while (n > 1)
					extra[n_extra++] = p[--n];
				c = p[0];
			}
		}

		if (c == NUL)
			break;

		outchar(c);
		col++;
	}
}

	void
screenalloc()
{
	static int		old_Rows = 0;
	static int		old_Columns = 0;
	register int	i;

	/*
	 * Allocation of the sceen buffers is done only when the size changes
	 */
	if (Nextscreen != NULL && Rows == old_Rows && Columns == old_Columns)
		return;

	old_Rows = Rows;
	old_Columns = Columns;

	/*
	 * If we're changing the size of the screen, free the old arrays
	 */
	if (Nextscreen != NULL)
		free(Nextscreen);
	if (LinePointers != NULL)
		free((char *)LinePointers);
	if (LineNumbers != NULL)
		free((char *) LineNumbers);
	if (LineSizes != NULL)
		free(LineSizes);

	Nextscreen = malloc((size_t) (Rows * Columns));
	LineNumbers = (linenr_t *) malloc((size_t) (Rows * sizeof(linenr_t)));
	LineSizes = malloc((size_t) Rows);
	LinePointers = (char **)malloc(sizeof(char *) * Rows);

	if (Nextscreen == NULL || LineNumbers == NULL || LineSizes == NULL ||
												LinePointers == NULL)
	{
		emsg("Out of memory");
		if (Nextscreen != NULL)
			free(Nextscreen);
		Nextscreen = NULL;
	}
	else
	{
		for (i = 0; i < Rows; ++i)
				LinePointers[i] = Nextscreen + i * Columns;
	}

	screenclear();
}

	void
screenclear()
{
	register char  *np;
	register char  *end;

	if (Nextscreen == NULL)
		return;

	outstr(T_ED);				/* clear the display */

	np = Nextscreen;
	end = Nextscreen + Rows * Columns;

	/* blank out Nextscreen */
	while (np != end)
		*np++ = ' ';

	/* clear screen info */
	NumLineSizes = 0;
}

	void
cursupdate()
{
	linenr_t		p;
	u_char			c;
	u_char		   *ptr;
	int 			incr;
	long 			nlines;
	int 			i;
	int 			didincr;
	int 			temp;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return;

	if (Curpos.lnum > line_count)
		Curpos.lnum = line_count;
	if (bufempty()) 			/* special case - file is empty */
	{
		Topline = 1;
		Curpos.lnum = 1;
		Curpos.col = 0;
		for (i = 0; i < Rows; i++)
			LineSizes[i] = 0;
		NumLineSizes = 1;
	}
	else if (Curpos.lnum < Topline)
	{
		/*
		 * if the cursor is above the top of the screen, put it at the top of
		 * the screen, and if we weren't very close to begin with, we scroll so that
		 * the line is close to the middle.
		 */
		temp = Rows / 3;
		if (Topline - Curpos.lnum >= temp)
		{
			p = Curpos.lnum;
			for (i = 0; i < temp && p > 1; i += plines(--p))
				;
			Topline = p;
		}
		else
				Topline = Curpos.lnum;
		updateScreen(VALID);
	}
	else if (Curpos.lnum >= Botline)
	{
		nlines = Curpos.lnum - Botline + 1;
		/*
		 * compute the number of lines at the top which have the same or more
		 * rows than the rows of the lines below the bottom
		 */
		if (nlines <= Rows / 3)
		{
				temp = plines_m(Botline, Curpos.lnum);
				for (i = 0, p = Topline; i < temp && p <= line_count; ++p)
						i += plines(p);
				nlines = p - Topline;
		}

		/*
		 * Scroll up if the cursor is off the bottom of the screen a bit.
		 * Otherwise put it at 2/3 of the screen.
		 */
		if (nlines > Rows / 3)
		{
			p = Curpos.lnum;
			temp = (2 * Rows) / 3;
			nlines = 0;
			i = 0;
			do				/* this loop could win a contest ... */
				i += plines(p);
			while (i < temp && (nlines = 1) && --p != 0);
			Topline = p + nlines;
		}
		else
			scrollup(nlines);
		updateScreen(VALID);
	}
	if (NumLineSizes == 0)		/* don't know about screen contents */
		updateScreen(NOT_VALID);
	Cursrow = Curscol = Cursvcol = i = 0;
	for (p = Topline; p != Curpos.lnum; ++p)
		if (RedrawingDisabled)		/* LineSizes[] invalid */
			Cursrow += plines(p);
		else
			Cursrow += LineSizes[i++];

	if (P(P_NU))
		Curscol = 8;

	Cline_row = Cursrow;
	if (!RedrawingDisabled && i > NumLineSizes)
								/* Should only happen with a line that is too */
								/* long to fit on the last screen line. */
		Cline_size = 0;
	else
	{
		if (RedrawingDisabled)      /* LineSizes[] invalid */
		    Cline_size = plines(Curpos.lnum);
        else
			Cline_size = LineSizes[i];

		ptr = (u_char *)nr2ptr(Curpos.lnum);
		for (i = 0; i <= Curpos.col; i++)
		{
			c = *ptr++;
			if (c == NUL)		/* make sure we don't go past the end of the line */
				Curpos.col = i;
			/* A tab gets expanded, depending on the current column */
			incr = chartabsize(c, Cursvcol);
			Curscol += incr;
			Cursvcol += incr;
			if (Curscol >= Columns)
			{
				Curscol -= Columns;
				Cursrow++;
				didincr = TRUE;
			}
			else
				didincr = FALSE;
		}

		if (c == TAB && State == NORMAL && !P(P_LS))
		{
			Curscol--;
			Cursvcol--;
		}
		else
		{
			Curscol -= incr;
			Cursvcol -= incr;
		}
		if (Curscol < 0)
		{
			Curscol += Columns;
			Cursrow--;
		}
	}

	if (set_want_col)
	{
		Curswant = Cursvcol;
		set_want_col = FALSE;
	}
}

	void
scrolldown(nlines)
	long	nlines;
{
	register long	done = 0;	/* total # of physical lines done */

	/* Scroll up 'nlines' lines. */
	while (nlines--)
	{
		if (Topline == 1)
			break;
		done += plines(--Topline);
	}
	/*
	 * Compute the row number of the last row of the cursor line
	 * and move it onto the screen.
	 */
	Cursrow += done + plines(Curpos.lnum) - 1 - Cursvcol / Columns;
	while (Cursrow >= Rows - 1 && Curpos.lnum > 1)
		Cursrow -= plines(Curpos.lnum--);
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
		if (Topline == line_count)
			break;
		done += plines(Topline);
		if (Curpos.lnum == Topline)
			++Curpos.lnum;
		++Topline;
	}
	s_del(0, done, TRUE);
#endif
		Topline += nlines;
		if (Topline > line_count)
				Topline = line_count;
		if (Curpos.lnum < Topline)
				Curpos.lnum = Topline;
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
 * s_ins(row, nlines, invalid) - insert 'nlines' lines at 'row'
 * if 'invalid' is TRUE the LineNumbers[] is invalidated.
 */
	void
s_ins(row, nlines, invalid)
	int 			row;
	int 			nlines;
	bool_t		invalid;
{
	int 			i;
	int 			j;
	char		*temp;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return;

	if (invalid)
		NumLineSizes = 0;

	if (nlines > (Rows - 1 - row))
		nlines = Rows - 1 - row;

	if ((T_IL[0] == NUL) || RedrawingDisabled || nlines <= 0)
		return;

	/*
	 * It "looks" better if we do all the inserts at once
	 */
#ifndef AMIGA
	outstr(T_SC);				/* save position */

	if (T_IL_B[0] == NUL) {
		for (i = 0; i < nlines; i++) {
			windgoto(row, 0);
			outstr(T_IL);
		}
	} else
#endif
	{
		windgoto(row, 0);
		outstr(T_IL);
		outnum((long)nlines);
		outstr(T_IL_B);
	}

	windgoto(Rows - 1, 0);		/* delete any garbage that may have */
	outstr(T_EL);				/* been shifted to the bottom line */
	redraw_msg = TRUE;

#ifndef AMIGA
	outstr(T_RC);				/* restore position */
#endif

	/*
	 * Now shift LinePointers nlines down to reflect the inserted lines.
	 * Clear the inserted lines.
	 */
	for (i = 0; i < nlines; ++i)
	{
		j = Rows - 2 - i;
		temp = LinePointers[j];
		while ((j -= nlines) >= row)
				LinePointers[j + nlines] = LinePointers[j];
		LinePointers[j + nlines] = temp;
		memset(temp, ' ', (size_t)Columns);
	}
}

/*
 * s_del(row, nlines, invalid) - delete 'nlines' lines at 'row'
 * If 'invalid' is TRUE LineNumbers[] is ivalidated.
 */
	void
s_del(row, nlines, invalid)
	int 			row;
	int 			nlines;
	bool_t		invalid;
{
	int 			j;
	int 			i;
	char		*temp;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return;

	if (invalid)
		NumLineSizes = 0;

	if (nlines > (Rows - 1 - row))
		nlines = Rows - 1 - row;

	if ((T_DL[0] == NUL) || RedrawingDisabled || nlines <= 0)
		return;

#ifndef AMIGA
	outstr(T_SC);				/* save position */
#endif

	windgoto(Rows - 1, 0);		/* delete any garbage that */
	outstr(T_EL);				/* was on the status line */
	redraw_msg = TRUE;

	/* delete the lines */
#ifndef AMIGA
	if (T_DL_B[0] == NUL) {
		for (i = 0; i < nlines; i++) {
			windgoto(row, 0);
			outstr(T_DL);		/* delete a line */
		}
	} else
#endif
	{
		windgoto(row, 0);
		outstr(T_DL);
		outnum((long)nlines);
		outstr(T_DL_B);
	}
#ifndef AMIGA
	outstr(T_RC);				/* restore position */
#endif

	/*
	 * Now shift LinePointers nlines up to reflect the deleted lines.
	 * Clear the deleted lines.
	 */
	for (i = 0; i < nlines; ++i)
	{
		j = row + i;
		temp = LinePointers[j];
		while ((j += nlines) < Rows - 1)
				LinePointers[j - nlines] = LinePointers[j];
		LinePointers[j - nlines] = temp;
		memset(temp, ' ', (size_t)Columns);
	}
}

	void
showmode()
{
		if (State == INSERT || State == REPLACE || Recording)
		{
				gotocmdline(YES, NUL);
				if (State == INSERT)
						outstr("-- INSERT --");
				if (State == REPLACE)
						outstr("-- REPLACE --");
				if (Recording)
						outstr("recording");
		}
}
