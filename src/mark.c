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
 * mark.c: functions for setting marks and jumping to them
 */

#include "vim.h"

#ifdef	MEGAMAX
overlay "mark"
#endif

/*
 * This file contains routines to maintain and manipulate marks.
 */

#define NMARKS	26				/* max. # of marks that can be saved */

struct mark {
	char		   *ptr;
	colnr_t 		col;
};

static struct mark mlist[NMARKS];
static struct mark pcmark;		/* previous context mark */

/*
 * setmark(c) - set mark 'c' at current cursor position
 *
 * Returns TRUE on success, FALSE if no room for mark or bad name given.
 */
bool_t
setmark(c)
	int			c;
{
	int 			i;

	if (!islower(c))
		return FALSE;

	i = c - 'a';
	mlist[i].ptr = nr2ptr(Curpos.lnum);
	mlist[i].col = Curpos.col;
	return TRUE;
}

/*
 * setpcmark() - set the previous context mark to the current position
 */
void
setpcmark()
{
	pcmark.ptr = nr2ptr(Curpos.lnum);
	pcmark.col = Curpos.col;
}

/*
 * getmark(c) - find mark for char 'c'
 *
 * Return pointer to LPtr or NULL if no such mark.
 */
	FPOS *
getmark(c)
	int			c;
{
	int 			i;
	static FPOS pos;

	if (c == '\'' || c == '`')	/* previous context mark */
	{
		if (pcmark.ptr != NULL && (pos.lnum = ptr2nr(pcmark.ptr, 1)) != 0)
		{
				pos.col = pcmark.col;
				return (&pos);
		}
	}
	else if (islower(c))
	{
		i = c - 'a';
		if ((pos.lnum = ptr2nr(mlist[i].ptr, 1)) != 0)
		{
				pos.col = mlist[i].col;
				return (&pos);
		}
	}
	return (FPOS *)NULL;
}

/*
 * clrallmarks() - clear all marks
 *
 * Used mainly when trashing the entire buffer during ":e" type commands
 */
void
clrallmarks()
{
	int 			i;

	for (i = 0; i < NMARKS; i++)
		mlist[i].ptr = NULL;
	pcmark.ptr = NULL;
	qf_clrallmarks();
}

/*
 * adjustmark: set new ptr for a mark
 * if new == NULL the mark is effectively deleted
 */
   void
adjustmark(old, new)
		char *old, *new;
{
		register int i;

		for (i = 0; i < NMARKS; ++i)
				if (mlist[i].ptr == old)
						mlist[i].ptr = new;
		if (pcmark.ptr == old)
				pcmark.ptr = new;
		qf_adjustmark(old, new);
}
