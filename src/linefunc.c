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
 * linefunc.c: some functions to move to the next/previous line and
 *			   to the next/previous character
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

/*
 * coladvance(col)
 *
 * Try to advance the Cursor to the specified column.
 */

	void
coladvance(wcol)
	colnr_t 		wcol;
{
	int 				index;
	register u_char		*ptr;
	register colnr_t	col;

	ptr = (u_char *)nr2ptr(Curpos.lnum);

	/* try to advance to the specified column */
	index = -1;
	col = 0;
	while (col <= wcol && *ptr)
	{
		++index;
		/* Count a tab for what it's worth (if list mode not on) */
		col += chartabsize(*ptr, col);
		++ptr;
	}
	if (index < 0)
		Curpos.col = 0;
	else
		Curpos.col = index;
}

/*
 * inc(p)
 *
 * Increment the line pointer 'p' crossing line boundaries as necessary. Return
 * 1 when crossing a line, -1 when at end of file, 0 otherwise.
 */
	int
inc(lp)
	register FPOS  *lp;
{
	register char  *p = pos2ptr(lp);

	if (*p != NUL)
	{			/* still within line */
		lp->col++;
		return ((p[1] != NUL) ? 0 : 1);
	}
	if (lp->lnum != line_count)
	{			/* there is a next line */
		lp->col = 0;
		lp->lnum++;
		return 1;
	}
	return -1;
}

	int
incCurpos()
{
	return inc(&Curpos);
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
	int
incl(lp)
		register FPOS *lp;
{
		register int r;

		if ((r = inc(lp)) == 1 && lp->col)
				r = inc(lp);
		return r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary. Return
 * 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
	int
dec(lp)
	register FPOS  *lp;
{
	if (lp->col > 0)
	{			/* still within line */
		lp->col--;
		return 0;
	}
	if (lp->lnum > 1)
	{			/* there is a prior line */
		lp->lnum--;
		lp->col = strlen(nr2ptr(lp->lnum));
		return 1;
	}
	return -1;					/* at start of file */
}

	int
decCurpos()
{
	return dec(&Curpos);
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
	int
decl(lp)
		register FPOS *lp;
{
		register int r;

		if ((r = dec(lp)) == 1 && lp->col)
				r = dec(lp);
		return r;
}

/*
 * make sure Curpos in on a valid character
 */
	void
adjustCurpos()
{
	int len;

	if (Curpos.lnum == 0)
		Curpos.lnum = 1;
	if (Curpos.lnum > line_count)
		Curpos.lnum = line_count;

	len = strlen(nr2ptr(Curpos.lnum));
	if (len == 0)
		Curpos.col = 0;
	else if (Curpos.col >= len)
		Curpos.col = len - 1;
}
