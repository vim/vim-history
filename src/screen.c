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
#include "globals.h"
#include "proto.h"
#include "param.h"

static u_char 	*Nextscreen = NULL; 	/* What's to be put on the screen. */
static int		 NumLineSizes = 0;		/* # of active LineSizes */
static linenr_t *LineNumbers = NULL;	/* Pointer to the line for LineSizes */
static u_char 	*LineSizes = NULL;		/* Number of rows the lines occupy */
static u_char 	**LinePointers = NULL;	/* array of pointers into Netscreen */

/*
 * The following variable is set (in cursupdate) to the number of physical
 * lines taken by the line the cursor is on. We use this to avoid extra calls
 * to plines(). The optimized routine updateline()
 * makes sure that the size of the cursor line hasn't changed. If so, lines
 * below the cursor will move up or down and we need to call the routine
 * updateScreen() to examine the entire screen.
 */
static int		Cline_size; 			/* size (in rows) of the cursor line */
static int		Cline_row;				/* starting row of the cursor line */
int				redraw_msg = TRUE;		/* TRUE when "insert mode" needs updating */
static FPOS		oldCurpos = {0, 0};		/* last known end of quoted part */
static int		oldCurswant = 0;		/* last known value of Curswant */
static int		canopt;					/* TRUE when cursor goto can be optimized */
static int		emptyrows = 0;			/* number of '~' rows on screen */

static int screenline __ARGS((linenr_t, int, int));
static void screenchar __ARGS((u_char *, int, int));
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

	screenchar(NULL, 0, 0);	/* init cursor position of screenchar() */
	outstr(T_CI);				/* disable cursor */

	row = screenline(Curpos.lnum, Cline_row, (int)Rows - 1);

	outstr(T_CV);				/* enable cursor again */

	n = row - Cline_row;
	if (n != Cline_size)		/* line changed size */
	{
		if (n < Cline_size) 	/* got smaller: delete lines */
				s_del(row, Cline_size - n, FALSE);
		else					/* got bigger: insert lines */
				s_ins(Cline_row + Cline_size, n - Cline_size, FALSE);

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
	linenr_t		lastline = 0; /* only valid if endrow != Rows -1 */
	int				done;		/* if TRUE, we hit the end of the file */
	int				didline;	/* if TRUE, we finished the last line */
	int 			srow = 0;	/* starting row of the current line */
	int 			idx;
	int 			i;
	long 			j;
	static int		postponed_not_valid = FALSE;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return;

	if (type == CLEAR)		/* first clear screen */
	{
		screenclear();
		type = NOT_VALID;
	}
	if (type == CURSUPD)	/* update cursor and then redraw */
	{
		NumLineSizes = 0;
		cursupdate();		/* will call updateScreen(VALID) */
		return;
	}
	if (NumLineSizes == 0)
		type = NOT_VALID;

 	if (RedrawingDisabled)
	{
		if (type == NOT_VALID)
			postponed_not_valid = TRUE;		/* use NOT_VALID next time */
		return;
	}

	if (postponed_not_valid)
	{
		type = NOT_VALID;
		postponed_not_valid = FALSE;
	}

/* return if there is nothing to do */
	if ((type == VALID && Topline == LineNumbers[0]) ||
			(type == INVERTED && oldCurpos.lnum == Curpos.lnum &&
					oldCurpos.col == Curpos.col))
		return;

	if (type == NOT_VALID)
	{
		redraw_msg = TRUE;
		NumLineSizes = 0;
	}

	idx = 0;
	row = 0;
	lnum = Topline;
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
				lastline = LineNumbers[0] - 1;
				i = plines_m(Topline, lastline);
				if (i < Rows - 3)		/* less than a screen off */
				{
					/*
					 * Try to insert the correct number of lines.
					 * This may fail and the screen may have been cleared.
					 */
					if (s_ins(0, i, FALSE) && NumLineSizes)
					{
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
				else		/* far off: clearing the screen is faster */
					screenclear();
			}
			else		/* far off: clearing the screen is faster */
				screenclear();
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
				screenclear();   /* far off: clearing the screen is faster */
			}
			else
			{
				/*
				 * Try to delete the correct number of lines.
				 * Topline is at LineNumbers[i].
				 */
				if ((row == 0 || s_del(0, row, FALSE)) && NumLineSizes)
				{
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
				else
					row = 0;		/* update all lines */
			}
		}
		if (endrow == Rows - 1 && idx == 0) 	/* no scrolling */
				NumLineSizes = 0;
	}

	done = didline = FALSE;
	screenchar(NULL, 0, 0);	/* init cursor position of screenchar() */

	if (Quote.lnum)				/* check if we are updating the inverted part */
	{
		linenr_t	from, to;

	/* find the line numbers that need to be updated */
		if (Curpos.lnum < oldCurpos.lnum)
		{
			from = Curpos.lnum;
			to = oldCurpos.lnum;
		}
		else
		{
			from = oldCurpos.lnum;
			to = Curpos.lnum;
		}
	/* if in block mode and changed column or Curswant: update all lines */
		if (Quote_block && (Curpos.col != oldCurpos.col || Curswant != oldCurswant))
		{
			if (from > Quote.lnum)
				from = Quote.lnum;
			if (to < Quote.lnum)
				to = Quote.lnum;
		}

		if (from < Topline)
			from = Topline;
		if (to >= Botline)
			to = Botline - 1;

	/* find the minimal part to be updated */
		if (type == INVERTED)
		{
			while (lnum < from)						/* find start */
			{
				row += LineSizes[idx++];
				++lnum;
			}
			srow = row;
			for (j = idx; j < NumLineSizes; ++j)	/* find end */
			{
				if (LineNumbers[j] == to + 1)
				{
					endrow = srow;
					break;
				}
				srow += LineSizes[j];
			}
			oldCurpos = Curpos;
			oldCurswant = Curswant;
		}
	/* if we update the lines between from and to set oldCurpos */
		else if (lnum <= from && (endrow == Rows - 1 || lastline >= to))
		{
			oldCurpos = Curpos;
			oldCurswant = Curswant;
		}
	}

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

	emptyrows = 0;
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
		emptyrows = Rows - row - 1;

		if (done)				/* we hit the end of the file */
			Botline = line_count + 1;
		else
			Botline = lnum;
	}

	if (redraw_msg)
	{
		showmode();
		redraw_msg = FALSE;
	}

	outstr(T_CV);				/* enable cursor again */
}

static int		invert;		/* shared by screenline() and screenchar() */

/*
 * Move line "lnum" to the screen.
 * Start at row "startrow", stop when "endrow" is reached.
 * Return the number of last row the line occupies.
 */

	static int
screenline(lnum, startrow, endrow)
		linenr_t		lnum;
		int 			startrow;
		int 			endrow;
{
	register u_char  *screenp;
	register u_char   c;
	register int	col;				/* visual column on screen */
	register int	vcol;				/* visual column for tabs */
	register int	row;
	register u_char *ptr;
	char			extra[16];
	char			*p_extra;
	int 			n_extra;

	int				fromcol, tocol;		/* start/end of inverting */
	int				temp;
	FPOS			*top, *bot;

	row = startrow;
	col = 0;
	vcol = 0;
	invert = FALSE;
	fromcol = tocol = -10;
	ptr = (u_char *)nr2ptr(lnum);
	canopt = TRUE;
	if (Quote.lnum)					/* quoting active */
	{
		if (ltoreq(Curpos, Quote))		/* Quote is after Curpos */
		{
			top = &Curpos;
			bot = &Quote;
		}
		else							/* Quote is before Curpos */
		{
			top = &Quote;
			bot = &Curpos;
		}
		if (Quote_block)						/* block mode */
		{
			if (lnum >= top->lnum && lnum <= bot->lnum)
			{
				fromcol = getvcol(top, 2);
				temp = getvcol(bot, 2);
				if (temp < fromcol)
					fromcol = temp;

				if (Curswant == 29999)
					tocol = 29999;
				else
				{
					tocol = getvcol(top, 3);
					temp = getvcol(bot, 3);
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
			if (lnum == top->lnum)
				fromcol = getvcol(top, 2);
			if (lnum == bot->lnum)
				tocol = getvcol(bot, 3) + 1;

			if (Quote.col == QUOTELINE)		/* linewise */
			{
				if (fromcol > 0)
					fromcol = 0;
				if (tocol > 0)
					tocol = QUOTELINE;
			}
		}
		/* if inverting in this line, can't optimize cursor positioning */
		if (fromcol >= 0 || tocol >= 0)
			canopt = FALSE;
	}
	screenp = LinePointers[row];
	if (p_nu)
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
		if (vcol == fromcol)	/* start inverting */
		{
			invert = TRUE;
			outstr(T_TI);
		}
		if (vcol == tocol)		/* stop inverting */
		{
			invert = FALSE;
			outstr(T_TP);
		}

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
		else
		{
			if ((c = *ptr++) < ' ' || (c > '~' && c < 0xa0))
			{
				/*
				 * when getting a character from the file, we may have to turn it
				 * into something else on the way to putting it into 'Nextscreen'.
				 */
				if (c == TAB && !p_list)
				{
					p_extra = spaces;
					/* tab amount depends on current column */
					n_extra = (int)p_ts - vcol % (int)p_ts - 1;
					c = ' ';
				}
				else if (c == NUL && p_list)
				{
					extra[0] = NUL;
					p_extra = extra;
					n_extra = 1;
					c = '$';
				}
				else if (c != NUL)
				{
					p_extra = (char *)transchar(c);
					n_extra = charsize(c) - 1;
					c = (u_char)*p_extra++;
				}
			}
		}

		if (c == NUL)
		{
			if (invert)
			{
				if (vcol == 0)	/* invert first char of empty line */
				{
					if (*screenp != (' ' ^ 0x80))
					{
							*screenp = (' ' ^ 0x80);
							screenchar(screenp, row, col);
					}
					++screenp;
					++col;
				}
				outstr(T_TP);
				invert = FALSE;
			}
			/* 
			 * blank out the rest of this row
			 * could also use clear-to-end-of-line, but it is slower
			 * on an Amiga
			 */
			while (col < Columns)
			{
				if (*screenp != ' ')
				{
						*screenp = ' ';
						screenchar(screenp, row, col);
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
			screenp = LinePointers[row];
		}
		/* store the character in Nextscreen */
		if (!invert)
		{
			if (*screenp != c)
			{
				*screenp = c;
				screenchar(screenp, row, col);
			}
		}
		else
		{
			if (*screenp != (c ^ 0x80))
			{
				*screenp = c ^ 0x80;
				screenchar(screenp, row, col);
			}
		}
		++screenp;
		col++;
		vcol++;
	}

	if (invert)
	{
		outstr(T_TP);
		invert = FALSE;
	}
	return (row);
}

/*
 * put character '*p' on the screen at position 'row' and 'col'
 */
	static void
screenchar(p, row, col)
		u_char	*p;
		int 	row;
		int 	col;
{
	static int	oldrow, oldcol;		/* old cursor position */
	int			c;

	if (p == NULL)					/* initialize cursor position */
	{
		oldrow = oldcol = -1;
		return;
	}
	if (oldcol != col || oldrow != row)
	{
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
			if (i <= 4 && canopt)
			{
				while (i)
				{
					c = *(p - i--);
					outchar(c);
				}
			}
			else if (T_CRI && *T_CRI)	/* use tgoto interface! jw */
				outstr(tgoto(T_CRI, 0, i));
			else
				windgoto(row, col);
			
			oldcol = col;
		}
		else
			windgoto(oldrow = row, oldcol = col);
	}
	if (invert)
		outchar(*p ^ 0x80);
	else
		outchar(*p);
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
		register u_char *screenp;

		for (row = srow; row < (Rows - 1); ++row)
		{
			screenp = LinePointers[row];
			if (*screenp != c)
			{
				*screenp = c;
				screenchar(screenp, row, 0);
			}
			++screenp;
			for (col = 1; col < Columns; ++col)
			{
				if (*screenp != ' ')
				{
					*screenp = ' ';
					screenchar(screenp, row, col);
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
			if (c == TAB && !p_list) {
				strcpy(extra, "                ");
				/* tab amount depends on current column */
				n_extra = (p_ts - 1) - col % p_ts;
				c = ' ';
			} else if (c == NUL && p_list) {
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
		free((char *)Nextscreen);
	if (LinePointers != NULL)
		free((char *)LinePointers);
	if (LineNumbers != NULL)
		free((char *) LineNumbers);
	if (LineSizes != NULL)
		free(LineSizes);

	Nextscreen = (u_char *)malloc((size_t) (Rows * Columns));
	LineNumbers = (linenr_t *) malloc((size_t) (Rows * sizeof(linenr_t)));
	LineSizes = (u_char *)malloc((size_t) Rows);
	LinePointers = (u_char **)malloc(sizeof(u_char *) * Rows);

	if (Nextscreen == NULL || LineNumbers == NULL || LineSizes == NULL ||
												LinePointers == NULL)
	{
		emsg(e_outofmem);
		if (Nextscreen != NULL)
			free((char *)Nextscreen);
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
	register u_char  *np;
	register u_char  *end;

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
	long 			nlines;
	int 			i;
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
		if (NumLineSizes == 0)		/* don't know about screen contents */
			updateScreen(NOT_VALID);
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
		else if (p_sj > 1)		/* scroll at least p_sj lines */
		{
			for (i = 0; i < p_sj && Topline > 1; i += plines(--Topline))
				;

		}
		if (Topline > Curpos.lnum)
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
		if (nlines <= Rows)
		{
				/* get the number or rows to scroll minus the number of
								free '~' rows */
			temp = plines_m(Botline, Curpos.lnum) - emptyrows;
			if (temp <= 0)
				nlines = 0;
			else
			{
					/* scroll minimal number of lines */
				if (temp < p_sj)
					temp = p_sj;
				for (i = 0, p = Topline; i < temp && p <= line_count; ++p)
					i += plines(p);
				nlines = p - Topline;
			}
		}

		/*
		 * Scroll up if the cursor is off the bottom of the screen a bit.
		 * Otherwise put it at 2/3 of the screen.
		 */
		if (nlines > Rows / 3 && nlines > p_sj)
		{
			p = Curpos.lnum;
			temp = (2 * Rows) / 3;
			nlines = 0;
			i = 0;
			do				/* this loop could win a contest ... */
				i += plines(p);
			while (i < temp && (nlines = 1) != 0 && --p != 0);
			Topline = p + nlines;
		}
		else
			scrollup(nlines);
		updateScreen(VALID);
	}
	else if (NumLineSizes == 0)		/* don't know about screen contents */
		updateScreen(NOT_VALID);
	Cursrow = Curscol = Cursvcol = i = 0;
	for (p = Topline; p != Curpos.lnum; ++p)
		if (RedrawingDisabled)		/* LineSizes[] invalid */
			Cursrow += plines(p);
		else
			Cursrow += LineSizes[i++];

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

		curs_columns();		/* compute Cursvcol and Curscol */
	}

	if (set_want_col)
	{
		Curswant = Cursvcol;
		set_want_col = FALSE;
	}
	showruler(0);
}

/*
 * compute Curscol and Cursvcol
 */
	void
curs_columns()
{
	Cursvcol = getvcol(&Curpos, 1);
	Curscol = Cursvcol;
	if (p_nu)
		Curscol += 8;

	Cursrow = Cline_row;
	while (Curscol >= Columns)
	{
		Curscol -= Columns;
		Cursrow++;
	}
}

/*
 * get virtual column number of pos
 * type = 1: where the cursor is on this character
 * type = 2: on the first position of this character
 * type = 3: on the last position of this character
 */
	int
getvcol(pos, type)
	FPOS	*pos;
	int		type;
{
	int				col;
	int				vcol;
	u_char		   *ptr;
	int 			incr;
	u_char			c;

	vcol = 0;
	ptr = (u_char *)nr2ptr(pos->lnum);
	for (col = pos->col; col >= 0; --col)
	{
		c = *ptr++;
		if (c == NUL)		/* make sure we don't go past the end of the line */
			break;

		/* A tab gets expanded, depending on the current column */
		incr = chartabsize(c, vcol);

		if (col == 0)		/* character at pos.col */
		{
			if (type == 3 || (type == 1 && c == TAB && State == NORMAL && !p_list))
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
 * Returns 0 if the lines are not inserted, 1 for success.
 */
	int
s_ins(row, nlines, invalid)
	int 		row;
	int 		nlines;
	int			invalid;
{
	int 		i;
	int 		j;
	u_char		*temp;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return 0;

	if (invalid)
		NumLineSizes = 0;

	if (nlines > (Rows - 1 - row))
		nlines = Rows - 1 - row;

	if (RedrawingDisabled || nlines <= 0 ||
						((T_CIL == NULL || *T_CIL == NUL) &&
						(T_IL == NULL || *T_IL == NUL) &&
						(T_SR == NULL || *T_SR == NUL || row != 0)))
		return 0;
	
	if (Rows - nlines < 5)	/* only a few lines left: redraw is faster */
	{
		screenclear();		/* will set NumLineSizes to 0 */
		return 0;
	}

	/*
	 * It "looks" better if we do all the inserts at once
	 */
    if (T_CIL && *T_CIL) 
    {
        windgoto(row, 0);
		if (nlines == 1 && T_IL && *T_IL)
			outstr(T_IL);
		else
			outstr(tgoto(T_CIL, 0, nlines));
    }
    else
    {
        for (i = 0; i < nlines; i++) 
        {
            if (i == 0 || row != 0)
				windgoto(row, 0);
			if (T_IL && *T_IL)
				outstr(T_IL);
			else
				outstr(T_SR);
        }
    }
	windgoto((int)Rows - 1, 0);		/* delete any garbage that may have */
	clear_line();					/* been shifted to the bottom line */
	redraw_msg = TRUE;

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
	return 1;
}

/*
 * s_del(row, nlines, invalid) - delete 'nlines' lines at 'row'
 * If 'invalid' is TRUE LineNumbers[] is ivalidated.
 * Return 1 for success, 0 if the lines are not deleted.
 */
	int
s_del(row, nlines, invalid)
	int 			row;
	int 			nlines;
	int			invalid;
{
	int 			j;
	int 			i;
	u_char		*temp;

	screenalloc();		/* allocate screen buffers if size changed */

	if (Nextscreen == NULL)
		return 0;

	if (invalid)
		NumLineSizes = 0;

	if (nlines > (Rows - 1 - row))
		nlines = Rows - 1 - row;

	if (RedrawingDisabled || nlines <= 0 ||
				((T_DL == NULL || *T_DL == NUL) &&
				(T_CDL == NULL || *T_CDL == NUL) &&
				row != 0))
		return 0;

	if (Rows - nlines < 5)	/* only a few lines left: redraw is faster */
	{
		screenclear();		/* will set NumLineSizes to 0 */
		return 0;
	}

	windgoto((int)Rows - 1, 0);		/* delete any garbage that may be */
	clear_line();					/* on the bottom line */
	redraw_msg = TRUE;

	/* delete the lines */
	if (T_CDL && *T_CDL) 
	{
		windgoto(row, 0);
		if (nlines == 1 && T_DL && *T_DL)
			outstr(T_DL);
		else
			outstr(tgoto(T_CDL, 0, nlines));
	} 
	else
	{
		if (row == 0)
		{
			for (i = 0; i < nlines; i++) 
				outchar('\n');
		}
		else
		{
			for (i = 0; i < nlines; i++) 
			{
				windgoto(row, 0);
				outstr(T_DL);           /* delete a line */
			}
		}
	}

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
	return 1;
}

	void
showmode()
{
		if ((p_mo && (State == INSERT || State == REPLACE)) || Recording)
		{
				gotocmdline(TRUE, NUL);
				if (p_mo)
				{
					if (State == INSERT)
						outstrn("-- INSERT --");
					if (State == REPLACE)
						outstrn("-- REPLACE --");
				}
				if (Recording)
						outstrn("recording");
		}
		showruler(1);
}

/*
 * delete mode message
 */
	void
delmode()
{
	if (Recording)
		msg("recording");
	else
		msg("");
}

/*
 * if ruler option is set: show current cursor position
 * if always is FALSE, only print if position has changed
 */
	void
showruler(always)
	int		always;
{
	static linenr_t	oldlnum = 0;
	static colnr_t	oldcol = 0;
	static int		oldlen = 0;
	int				newlen;
	char			buffer[20];

	if (p_ru && (redraw_msg || always || Curpos.lnum != oldlnum || Cursvcol != oldcol))
	{
		windgoto((int)Rows - 1, (int)Columns - 22);
#if defined(SYSV) || defined(linux) || defined(MSDOS) || defined(AMIGA)
		newlen = sprintf(buffer, "%ld,%d", Curpos.lnum, Cursvcol + 1);
#else
		newlen = strlen(sprintf(buffer, "%ld,%d", Curpos.lnum, Cursvcol + 1));
#endif
		outstrn(buffer);
		while (newlen < oldlen)
		{
			outchar(' ');
			--oldlen;
		}
		oldlen = newlen;
		oldlnum = Curpos.lnum;
		oldcol = Cursvcol;
		redraw_msg = FALSE;
	}
}

/*
 * Clear a line. The cursor must be at the first char of the line.
 */
	void
clear_line()
{
	register int i;

	if (T_EL != NULL && *T_EL != NUL)
		outstr(T_EL);
	else
		for (i = 1; i < Columns; ++i)
			outchar(' ');
}

