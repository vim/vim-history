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
#include "globals.h"
#include "proto.h"
#include "mark.h"

/*
 * This file contains routines to maintain and manipulate marks.
 */

#define NMARKS			26			/* max. # of named marks */
#define JUMPLISTSIZE	50			/* max. # of marks in jump list */

static struct mark pcmark;					/* previous context mark */
static struct mark namedm[NMARKS];			/* original vi marks */
static struct filemark namedfm[NMARKS];		/* new marks with file nr */
static struct filemark jumplist[JUMPLISTSIZE];	/* list of old pcmarks */

static int jumplistlen = 0;
static int jumplistidx = 0;

static FPOS *mark2pos __ARGS((struct mark *));

/*
 * setmark(c) - set named mark 'c' at current cursor position
 *
 * Returns TRUE on success, FALSE if no room for mark or bad name given.
 */
	int
setmark(c)
	int			c;
{
	int 			i;

	if (islower(c))
	{
		i = c - 'a';
		namedm[i].ptr = nr2ptr(Curpos.lnum);
		namedm[i].col = Curpos.col;
		return TRUE;
	}
	if (isupper(c))
	{
		i = c - 'A';
		namedfm[i].mark.ptr = nr2ptr(Curpos.lnum);
		namedfm[i].mark.col = Curpos.col;
		namedfm[i].lnum = Curpos.lnum;
		namedfm[i].fnum = 0;
		return TRUE;
	}
	return FALSE;
}

/*
 * setpcmark() - set the previous context mark to the current position
 *				 and insert it into the jump list
 */
	void
setpcmark()
{
	int i;
#ifdef ROTATE
	struct filemark tempmark;
#endif

	pcmark.ptr = nr2ptr(Curpos.lnum);
	pcmark.col = Curpos.col;

#ifndef ROTATE
	/*
	 * simply add the new entry at the end of the list
	 */
	jumplistidx = jumplistlen;
#else
	/*
	 * If last used entry is not at the top, put it at the top by rotating
	 * the stack until it is (the newer entries will be at the bottom).
	 * Keep one entry (the last used one) at the top.
	 */
	if (jumplistidx < jumplistlen)
		++jumplistidx;
	while (jumplistidx < jumplistlen)
	{
		tempmark = jumplist[jumplistlen - 1];
		for (i = jumplistlen - 1; i > 0; --i)
			jumplist[i] = jumplist[i - 1];
		jumplist[0] = tempmark;
		++jumplistidx;
	}
#endif

		/* only add new entry if it differs from the last one */
	if (jumplistlen == 0 || jumplist[jumplistidx - 1].mark.ptr != pcmark.ptr)
	{
			/* if jumplist is full: remove oldest entry */
		if (++jumplistlen > JUMPLISTSIZE)
		{
			jumplistlen = JUMPLISTSIZE;
			for (i = 1; i < jumplistlen; ++i)
				jumplist[i - 1] = jumplist[i];
			--jumplistidx;
		}

		jumplist[jumplistidx].mark = pcmark;
		jumplist[jumplistidx].lnum = Curpos.lnum;
		jumplist[jumplistidx].fnum = 0;
		++jumplistidx;
	}
}

/*
 * move "count" positions in the jump list (count may be negative)
 */
	FPOS *
movemark(count)
	int count;
{
	FPOS		*pos;

	if (jumplistlen == 0)			/* nothing to jump to */
		return (FPOS *)NULL;

	if (jumplistidx + count < 0 || jumplistidx + count >= jumplistlen)
		return (FPOS *)NULL;

	/*
	 * if first CTRL-O or CTRL-I command after a jump, add cursor position to list
	 */
	if (jumplistidx == jumplistlen)
	{
		setpcmark();
		--jumplistidx;		/* skip the new entry */
	}

	jumplistidx += count;
	if (jumplist[jumplistidx].mark.ptr == NULL)	/* jump to other file */
	{
		if (getaltfile(jumplist[jumplistidx].fnum - 1, jumplist[jumplistidx].lnum, FALSE))
			return (FPOS *)NULL;
		Curpos.col = jumplist[jumplistidx].mark.col;
		jumplist[jumplistidx].fnum = 0;
		jumplist[jumplistidx].mark.ptr = nr2ptr(Curpos.lnum);
		pos = (FPOS *)-1;
	}
	else
		pos = mark2pos(&jumplist[jumplistidx].mark);
	return pos;
}

/*
 * getmark(c) - find mark for char 'c'
 *
 * Return pointer to FPOS if found
 *        NULL if no such mark.
 *        -1 if mark is in other file (only if changefile is TRUE)
 */
	FPOS *
getmark(c, changefile)
	int			c;
	int			changefile;
{
	FPOS	*posp;

	posp = NULL;
	if (c == '\'' || c == '`')			/* previous context mark */
		posp = mark2pos(&pcmark);
	else if (islower(c))				/* normal named mark */
		posp = mark2pos(&(namedm[c - 'a']));
	else if (isupper(c))				/* named file mark */
	{
		c -= 'A';
		posp = mark2pos(&(namedfm[c].mark));
		if (posp == NULL && namedfm[c].lnum != 0 && (changefile || samealtfile(namedfm[c].fnum - 1)))
		{
			if (!getaltfile(namedfm[c].fnum - 1, namedfm[c].lnum, TRUE))
			{
				Curpos.col = namedfm[c].mark.col;
				namedfm[c].fnum = 0;
				namedfm[c].mark.ptr = nr2ptr(Curpos.lnum);
				posp = (FPOS *)-1;
			}
		}
	}
	return posp;
}

	static FPOS *
mark2pos(markp)
	struct mark *markp;
{
	static FPOS pos;

	if (markp->ptr != NULL && (pos.lnum = ptr2nr(markp->ptr, (linenr_t)1)) != 0)
	{
		pos.col = markp->col;
		return (&pos);
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
	static int 			i = -1;

	if (i == -1)		/* first call ever: initialize */
		for (i = 0; i < NMARKS; i++)
			namedfm[i].lnum = 0;

	for (i = 0; i < NMARKS; i++)
	{
		namedm[i].ptr = NULL;
		namedfm[i].mark.ptr = NULL;
	}
	pcmark.ptr = NULL;
	qf_clrallmarks();
	for (i = 0; i < jumplistlen; ++i)
		jumplist[i].mark.ptr = NULL;
}

/*
 * increment the file number for all filemarks
 * called when adding a file to the file stack
 */
	void
incrmarks()
{
	int			i;

	for (i = 0; i < NMARKS; i++)
		++namedfm[i].fnum;

	for (i = 0; i < jumplistlen; ++i)
	{
#if 0		/* this would take too much time */
		if (jumplist[i].fnum == 0)	/* current file */
			jumplist[i].lnum = ptr2nr(jumplist[i].mark.ptr, 1);
#endif
		++jumplist[i].fnum;
	}
}

/*
 * decrement the file number for the filemarks of the current file
 * called when not adding the current file name to the file stack
 */
	void
decrmarks()
{
	int			i;

	for (i = 0; i < NMARKS; i++)
		if (namedfm[i].fnum == 1)
			namedfm[i].fnum = 0;

	for (i = 0; i < jumplistlen; ++i)
		if (jumplist[i].fnum == 1)
			jumplist[i].fnum = 0;
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
		{
			if (namedm[i].ptr == old)
				namedm[i].ptr = new;
			if (namedfm[i].mark.ptr == old)
			{
				namedfm[i].mark.ptr = new;
				if (new == NULL)
					namedfm[i].lnum = 0;		/* delete this mark */
			}
		}
		if (pcmark.ptr == old)
				pcmark.ptr = new;
		for (i = 0; i < jumplistlen; ++i)
				if (jumplist[i].mark.ptr == old)
						jumplist[i].mark.ptr = new;
		qf_adjustmark(old, new);
}

/*
 * get name of file from a filemark (use the occasion to update the lnum)
 */
	char *
fm_getname(fmark)
	struct filemark *fmark;
{
	linenr_t	nr;
	char		*name;

	if (fmark->fnum != 0)						/* maybe not current file */
	{
		name = getaltfname(fmark->fnum - 1);
		if (fnamecmp(name, Filename) != 0)		/* not current file */
			return name;
		fmark->fnum = 0;
	}
	if (fmark->mark.ptr == NULL)
		fmark->mark.ptr = nr2ptr(fmark->lnum);	/* update ptr */
	else
	{
		nr = ptr2nr(fmark->mark.ptr, (linenr_t)1);
		if (nr != 0)
			fmark->lnum = nr;					/* update lnum */
	}
	return "current";
}

/*
 * print the marks (use the occasion to update the line numbers)
 */
	void
domarks()
{
	int			i;
	char		*name;

	settmode(0);
	outstrn("\nmark line  file\n");
	for (i = 0; i < NMARKS; ++i)
	{
		if (namedm[i].ptr != NULL)
		{
			sprintf(IObuff, " %c %5ld\n",
				i + 'a',
				ptr2nr(namedm[i].ptr, (linenr_t)1));
			outstrn(IObuff);
		}
		flushbuf();
	}
	for (i = 0; i < NMARKS; ++i)
	{
		if (namedfm[i].lnum != 0)
		{
			name = fm_getname(&namedfm[i]);
			if (name == NULL)		/* file name not available */
				continue;

			sprintf(IObuff, " %c %5ld  %s\n",
				i + 'A',
				namedfm[i].lnum,
				name);
			outstrn(IObuff);
		}
		flushbuf();
	}
	settmode(1);
	wait_return(TRUE);
}

/*
 * print the jumplist (use the occasion to update the line numbers)
 */
	void
dojumps()
{
	int			i;
	char		*name;

	settmode(0);
	outstrn("\n jump line  file\n");
	for (i = 0; i < jumplistlen; ++i)
	{
		if (jumplist[i].lnum != 0)
		{
			name = fm_getname(&jumplist[i]);
			if (name == NULL)		/* file name not available */
				continue;

			sprintf(IObuff, "%c %2d %5ld  %s\n",
				i == jumplistidx ? '>' : ' ',
				i + 1,
				jumplist[i].lnum,
				name);
			outstrn(IObuff);
		}
		flushbuf();
	}
	if (jumplistidx == jumplistlen)
		outstrn(">\n");
	settmode(1);
	wait_return(TRUE);
}
