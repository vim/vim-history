/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * undo.c: multi level undo facility
 *
 * The saved lines are stored in a list of lists (one for each buffer):
 *
 * b_u_oldhead------------------------------------------------+
 *                                                            |
 *                                                            V
 *                +--------------+    +--------------+    +--------------+
 * b_u_newhead--->| u_header     |    | u_header     |    | u_header     |
 *                |     uh_next------>|     uh_next------>|     uh_next---->NULL
 *         NULL<--------uh_prev  |<---------uh_prev  |<---------uh_prev  |
 *                |     uh_entry |    |     uh_entry |    |     uh_entry |
 *                +--------|-----+    +--------|-----+    +--------|-----+
 *                         |                   |                   |
 *                         V                   V                   V
 *                +--------------+    +--------------+    +--------------+
 *                | u_entry      |    | u_entry      |    | u_entry      |
 *                |     ue_next  |    |     ue_next  |    |     ue_next  |
 *                +--------|-----+    +--------|-----+    +--------|-----+
 *                         |                   |                   |
 *                         V                   V                   V
 *                +--------------+            NULL                NULL
 *                | u_entry      |
 *                |     ue_next  |
 *                +--------|-----+
 *                         |
 *                         V
 *                        etc.
 *
 * Each u_entry list contains the information for one undo or redo.
 * curbuf->b_u_curhead points to the header of the last undo (the next redo), or is
 * NULL if nothing has been undone.
 *
 * All data is allocated with u_alloc_line(), thus it will be freed as soon as
 * we switch files!
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void u_getbot __ARGS((void));
static int u_savecommon __ARGS((linenr_t, linenr_t, linenr_t));
static void u_undoredo __ARGS((void));
static void u_undo_end __ARGS((void));
static void u_freelist __ARGS((struct u_header *));
static void u_freeentry __ARGS((struct u_entry *, long));

static char_u *u_blockalloc __ARGS((long_u));
static void u_free_line __ARGS((char_u *));
static char_u *u_alloc_line __ARGS((unsigned));
static char_u *u_save_line __ARGS((linenr_t));

static long		u_newcount, u_oldcount;

/*
 * save the current line for both the "u" and "U" command
 */
	int
u_save_cursor()
{
	return (u_save((linenr_t)(curwin->w_cursor.lnum - 1), (linenr_t)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and bot may be curbuf->b_ml.ml_line_count + 1.
 * Returns FALSE when lines could not be saved.
 */
	int
u_save(top, bot)
	linenr_t top, bot;
{
	if (top > curbuf->b_ml.ml_line_count || top >= bot || bot > curbuf->b_ml.ml_line_count + 1)
		return FALSE;	/* rely on caller to do error messages */

	if (top + 2 == bot)
		u_saveline((linenr_t)(top + 1));

	return (u_savecommon(top, bot, (linenr_t)0));
}

/*
 * save the line "lnum" (used by :s command)
 * The line is replaced, so the new bottom line is lnum + 1.
 */
	int
u_savesub(lnum)
	linenr_t	lnum;
{
	return (u_savecommon(lnum - 1, lnum + 1, lnum + 1));
}

/*
 * a new line is inserted before line "lnum" (used by :s command)
 * The line is inserted, so the new bottom line is lnum + 1.
 */
 	int
u_inssub(lnum)
	linenr_t	lnum;
{
	return (u_savecommon(lnum - 1, lnum, lnum + 1));
}

/*
 * save the lines "lnum" - "lnum" + nlines (used by delete command)
 * The lines are deleted, so the new bottom line is lnum.
 */
	int
u_savedel(lnum, nlines)
	linenr_t	lnum;
	long		nlines;
{
	return (u_savecommon(lnum - 1, lnum + nlines, lnum));
}

	static int 
u_savecommon(top, bot, newbot)
	linenr_t top, bot;
	linenr_t newbot;
{
	linenr_t		lnum;
	long			i;
	struct u_header *uhp;
	struct u_entry	*uep;
	long			size;

	/*
	 * if curbuf->b_u_synced == TRUE make a new header
	 */
	if (curbuf->b_u_synced)
	{
		/*
		 * if we undid more than we redid, free the entry lists before and
		 * including curbuf->b_u_curhead
		 */
		while (curbuf->b_u_curhead != NULL)
			u_freelist(curbuf->b_u_newhead);

		/*
		 * free headers to keep the size right
		 */
		while (curbuf->b_u_numhead > p_ul && curbuf->b_u_oldhead != NULL)
			u_freelist(curbuf->b_u_oldhead);

		if (p_ul < 0)			/* no undo at all */
			return TRUE;

		/*
		 * make a new header entry
		 */
		uhp = (struct u_header *)u_alloc_line((unsigned)sizeof(struct u_header));
		if (uhp == NULL)
			goto nomem;
		uhp->uh_prev = NULL;
		uhp->uh_next = curbuf->b_u_newhead;
		if (curbuf->b_u_newhead != NULL)
			curbuf->b_u_newhead->uh_prev = uhp;
		uhp->uh_entry = NULL;
		uhp->uh_cursor = curwin->w_cursor;		/* save cursor position for undo */
		uhp->uh_changed = curbuf->b_changed;	/* save changed flag for undo */
												/* save named marks for undo */
		memmove((char *)uhp->uh_namedm, (char *)curbuf->b_namedm, sizeof(FPOS) * NMARKS); 
		curbuf->b_u_newhead = uhp;
		if (curbuf->b_u_oldhead == NULL)
			curbuf->b_u_oldhead = uhp;
		++curbuf->b_u_numhead;
	}
	else	/* find line number for ue_bot for previous u_save() */
		u_getbot();

	size = bot - top - 1;
#ifndef UNIX
		/*
		 * With Amiga and MSDOS we can't handle big undo's, because then
		 * u_alloc_line would have to allocate a block larger than 32K
		 */
	if (size >= 8000)
		goto nomem;
#endif

	/*
	 * add lines in front of entry list
	 */
	uep = (struct u_entry *)u_alloc_line((unsigned)sizeof(struct u_entry));
	if (uep == NULL)
		goto nomem;

	uep->ue_size = size;
	uep->ue_top = top;
	uep->ue_lcount = 0;
	if (newbot)
		uep->ue_bot = newbot;
		/*
		 * use 0 for ue_bot if bot is below last line or if the buffer is empty, in
		 * which case the last line may be replaced (e.g. with 'O' command).
		 */
	else if (bot > curbuf->b_ml.ml_line_count || bufempty())
		uep->ue_bot = 0;
	else
		uep->ue_lcount = curbuf->b_ml.ml_line_count;	/* we have to compute ue_bot later */

	if (size)
	{
		if ((uep->ue_array = (char_u **)u_alloc_line((unsigned)(sizeof(char_u *) * size))) == NULL)
		{
			u_freeentry(uep, 0L);
			goto nomem;
		}
		for (i = 0, lnum = top + 1; i < size; ++i)
		{
			if ((uep->ue_array[i] = u_save_line(lnum++)) == NULL)
			{
				u_freeentry(uep, i);
				goto nomem;
			}
		}
	}
	uep->ue_next = curbuf->b_u_newhead->uh_entry;
	curbuf->b_u_newhead->uh_entry = uep;
	curbuf->b_u_synced = FALSE;
	return TRUE;

nomem:
	if (ask_yesno((char_u *)"no undo possible; continue anyway") == 'y')
		return TRUE;
	return FALSE;
}

	void
u_undo(count)
	int count;
{
	/*
	 * If we get an undo command while executing a macro, we behave like the 
	 * original vi. If this happens twice in one macro the result will not
	 * be compatible.
	 */
	if (curbuf->b_u_synced == FALSE)
	{
		u_sync();
		count = 1;
	}

	curbuf->b_startop.lnum = 0;			/* unset '[ mark */
	curbuf->b_endop.lnum = 0;				/* unset '] mark */
	u_newcount = 0;
	u_oldcount = 0;
	while (count--)
	{
		if (curbuf->b_u_curhead == NULL)						/* first undo */
			curbuf->b_u_curhead = curbuf->b_u_newhead;
		else if (p_ul > 0)							/* multi level undo */
			curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next;			/* get next undo */

		if (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == NULL)	/* nothing to undo */
		{
			curbuf->b_u_curhead = curbuf->b_u_oldhead;					/* stick curbuf->b_u_curhead at end */
			beep();
			break;
		}

		u_undoredo();
	}
	u_undo_end();
}

	void
u_redo(count)
	int count;
{
	u_newcount = 0;
	u_oldcount = 0;
	while (count--)
	{
		if (curbuf->b_u_curhead == NULL || p_ul <= 0)		/* nothing to redo */
		{
			beep();
			break;
		}

		u_undoredo();

		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev;			/* advance for next redo */
	}
	u_undo_end();
}

/*
 * u_undoredo: common code for undo and redo
 *
 * The lines in the file are replaced by the lines in the entry list at curbuf->b_u_curhead.
 * The replaced lines in the file are saved in the entry list for the next undo/redo.
 */
	static void
u_undoredo()
{
	char_u		**newarray = NULL;
	linenr_t	oldsize;
	linenr_t	newsize;
	linenr_t	top, bot;
	linenr_t	lnum;
	linenr_t	newlnum = MAXLNUM;
	long		i;
	struct u_entry *uep, *nuep;
	struct u_entry *newlist = NULL;
	int			changed = curbuf->b_changed;
	FPOS		namedm[NMARKS];

	if (curbuf->b_u_curhead->uh_changed)
		CHANGED;
	else
		UNCHANGED(curbuf);
	/*
	 * save marks before undo/redo
	 */
	memmove((char *)namedm, (char *)curbuf->b_namedm, sizeof(FPOS) * NMARKS); 

	for (uep = curbuf->b_u_curhead->uh_entry; uep != NULL; uep = nuep)
	{
		top = uep->ue_top;
		bot = uep->ue_bot;
		if (bot == 0)
			bot = curbuf->b_ml.ml_line_count + 1;
		if (top > curbuf->b_ml.ml_line_count || top >= bot || bot > curbuf->b_ml.ml_line_count + 1)
		{
			EMSG("u_undo: line numbers wrong");
			CHANGED;		/* don't want UNCHANGED now */
			return;
		}

		if (top < newlnum)
		{
			newlnum = top;
			curwin->w_cursor.lnum = top + 1;
		}
		oldsize = bot - top - 1;	/* number of lines before undo */

		newsize = uep->ue_size;		/* number of lines after undo */

		/* delete the lines between top and bot and save them in newarray */
		if (oldsize)
		{
			if ((newarray = (char_u **)u_alloc_line((unsigned)(sizeof(char_u *) * oldsize))) == NULL)
			{
				/*
				 * We have messed up the entry list, repair is impossible.
				 * we have to free the rest of the list.
				 */
				while (uep != NULL)
				{
					nuep = uep->ue_next;
					u_freeentry(uep, uep->ue_size);
					uep = nuep;
				}
				break;
			}
			/* delete backwards, it goes faster in most cases */
			for (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
			{
				newarray[i] = u_save_line(lnum);
				ml_delete(lnum);
			}
		}

		/* insert the lines in u_array between top and bot */
		if (newsize)
		{
			for (lnum = top, i = 0; i < newsize; ++i, ++lnum)
			{
				ml_append(lnum, uep->ue_array[i], (colnr_t)0, FALSE);
				u_free_line(uep->ue_array[i]);
			}
			u_free_line((char_u *)uep->ue_array);
		}

		/* adjust marks */
		if (oldsize != newsize)
		{
			mark_adjust(top, top + oldsize - 1, MAXLNUM);
			mark_adjust(top + oldsize, MAXLNUM, (long)newsize - (long)oldsize);
		}

		u_newcount += newsize;
		u_oldcount += oldsize;
		uep->ue_size = oldsize;
		uep->ue_array = newarray;
		uep->ue_bot = top + newsize + 1;

		/*
		 * insert this entry in front of the new entry list
		 */
		nuep = uep->ue_next;
		uep->ue_next = newlist;
		newlist = uep;
	}

	curbuf->b_u_curhead->uh_entry = newlist;
	curbuf->b_u_curhead->uh_changed = changed;

	/*
	 * restore marks from before undo/redo
	 */
	for (i = 0; i < NMARKS; ++i)
		if (curbuf->b_u_curhead->uh_namedm[i].lnum)
		{
			curbuf->b_namedm[i] = curbuf->b_u_curhead->uh_namedm[i];
			curbuf->b_u_curhead->uh_namedm[i] = namedm[i];
		}

	if (curbuf->b_u_curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
		curwin->w_cursor.col = curbuf->b_u_curhead->uh_cursor.col;
	else
		curwin->w_cursor.col = 0;
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
	static void
u_undo_end()
{
	if ((u_oldcount -= u_newcount) != 0)
		msgmore(-u_oldcount);
	else if (u_newcount > p_report)
		smsg((char_u *)"%ld change%s", u_newcount, plural(u_newcount));

	updateScreen(CURSUPD);
}

/*
 * u_sync: stop adding to the current entry list
 */
	void
u_sync()
{
	if (curbuf->b_u_synced)
		return;				/* already synced */
	u_getbot();				/* compute ue_bot of previous u_save */
	curbuf->b_u_curhead = NULL;
}

/*
 * Called after writing the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
	void
u_unchanged(buf)
	BUF		*buf;
{
	register struct u_header *uh;

	for (uh = buf->b_u_newhead; uh; uh = uh->uh_next)
		uh->uh_changed = 1;
	buf->b_did_warn = FALSE;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 * 				It is called only when b_u_synced is FALSE.
 */
	static void
u_getbot()
{
	register struct u_entry *uep;

	if (curbuf->b_u_newhead == NULL || (uep = curbuf->b_u_newhead->uh_entry) == NULL)
	{
		EMSG("undo list corrupt");
		return;
	}

	if (uep->ue_lcount != 0)
	{
		/*
		 * the new ue_bot is computed from the number of lines that has been
		 * inserted (0 - deleted) since calling u_save. This is equal to the old
		 * line count subtracted from the current line count.
		 */
		uep->ue_bot = uep->ue_top + uep->ue_size + 1 + (curbuf->b_ml.ml_line_count - uep->ue_lcount);
		if (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_line_count)
		{
			EMSG("undo line missing");
			uep->ue_bot = uep->ue_top + 1;	/* assume all lines deleted, will get
											 * all the old lines back without
											 * deleting the current ones */
		}
		uep->ue_lcount = 0;
	}

	curbuf->b_u_synced = TRUE;
}

/*
 * u_freelist: free one entry list and adjust the pointers
 */
	static void
u_freelist(uhp)
	struct u_header *uhp;
{
	register struct u_entry *uep, *nuep;

	for (uep = uhp->uh_entry; uep != NULL; uep = nuep)
	{
		nuep = uep->ue_next;
		u_freeentry(uep, uep->ue_size);
	}

	if (curbuf->b_u_curhead == uhp)
		curbuf->b_u_curhead = NULL;

	if (uhp->uh_next == NULL)
		curbuf->b_u_oldhead = uhp->uh_prev;
	else
		uhp->uh_next->uh_prev = uhp->uh_prev;

	if (uhp->uh_prev == NULL)
		curbuf->b_u_newhead = uhp->uh_next;
	else
		uhp->uh_prev->uh_next = uhp->uh_next;

	u_free_line((char_u *)uhp);
	--curbuf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
	static void
u_freeentry(uep, n)
	struct u_entry *uep;
	register long n;
{
	while (n)
		u_free_line(uep->ue_array[--n]);
	u_free_line((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
	void
u_clearall(buf)
	BUF		*buf;
{
	buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = NULL;
	buf->b_u_synced = TRUE;
	buf->b_u_numhead = 0;
	buf->b_u_line_ptr = NULL;
	buf->b_u_line_lnum = 0;
}

/*
 * save the line "lnum" for the "U" command
 */
	void
u_saveline(lnum)
	linenr_t lnum;
{
	if (lnum == curbuf->b_u_line_lnum)		/* line is already saved */
		return;
	if (lnum < 1 || lnum > curbuf->b_ml.ml_line_count)	/* should never happen */
		return;
	u_clearline();
	curbuf->b_u_line_lnum = lnum;
	if (curwin->w_cursor.lnum == lnum)
		curbuf->b_u_line_colnr = curwin->w_cursor.col;
	else
		curbuf->b_u_line_colnr = 0;
	curbuf->b_u_line_ptr = u_save_line(lnum);	/* when out of mem alloc() will give a warning */
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
	void
u_clearline()
{
	if (curbuf->b_u_line_ptr != NULL)
	{
		u_free_line(curbuf->b_u_line_ptr);
		curbuf->b_u_line_ptr = NULL;
		curbuf->b_u_line_lnum = 0;
	}
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 */
	void
u_undoline()
{
	colnr_t t;
	char_u	*old;

	if (curbuf->b_u_line_ptr == NULL || curbuf->b_u_line_lnum > curbuf->b_ml.ml_line_count)
	{
		beep();
		return;
	}
		/* first save the line for the 'u' command */
	u_savecommon(curbuf->b_u_line_lnum - 1, curbuf->b_u_line_lnum + 1, (linenr_t)0);
	old = u_save_line(curbuf->b_u_line_lnum);
	if (old == NULL)
		return;
	ml_replace(curbuf->b_u_line_lnum, curbuf->b_u_line_ptr, TRUE);
	u_free_line(curbuf->b_u_line_ptr);
	curbuf->b_u_line_ptr = old;

	t = curbuf->b_u_line_colnr;
	if (curwin->w_cursor.lnum == curbuf->b_u_line_lnum)
		curbuf->b_u_line_colnr = curwin->w_cursor.col;
	curwin->w_cursor.col = t;
	curwin->w_cursor.lnum = curbuf->b_u_line_lnum;
	cursupdate();
	updateScreen(VALID_TO_CURSCHAR);
}

/*
 * storage allocation for the undo lines and blocks of the current file
 */

/*
 * Memory is allocated in relatively large blocks. These blocks are linked
 * in the allocated block list, headed by curbuf->b_block_head. They are all freed
 * when abandoning a file, so we don't have to free every single line. The
 * list is kept sorted on memory address.
 * block_alloc() allocates a block.
 * m_blockfree() frees all blocks.
 *
 * The available chunks of memory are kept in free chunk lists. There is
 * one free list for each block of allocated memory. The list is kept sorted
 * on memory address.
 * u_alloc_line() gets a chunk from the free lists.
 * u_free_line() returns a chunk to the free lists.
 * curbuf->b_m_search points to the chunk before the chunk that was
 * freed/allocated the last time.
 * curbuf->b_mb_current points to the b_head where curbuf->b_m_search
 * points into the free list.
 *
 *
 *  b_block_head     /---> block #1     /---> block #2
 *       mb_next ---/       mb_next ---/       mb_next ---> NULL
 *       mb_info            mb_info            mb_info
 *          |                  |                  |
 *          V                  V                  V
 *        NULL          free chunk #1.1      free chunk #2.1
 *                             |                  |
 *                             V                  V
 *                      free chunk #1.2          NULL
 *                             |
 *                             V
 *                            NULL
 *
 * When a single free chunk list would have been used, it could take a lot
 * of time in u_free_line() to find the correct place to insert a chunk in the
 * free list. The single free list would become very long when many lines are
 * changed (e.g. with :%s/^M$//).
 */

	/*
	 * this blocksize is used when allocating new lines
	 */
#define MEMBLOCKSIZE 2044

/*
 * The size field contains the size of the chunk, including the size field itself.
 *
 * When the chunk is not in-use it is preceded with the m_info structure.
 * The m_next field links it in one of the free chunk lists.
 *
 * On most unix systems structures have to be longword (32 or 64 bit) aligned.
 * On most other systems they are short (16 bit) aligned.
 */

/* the structure definitions are now in structs.h */

#ifdef ALIGN_LONG
	/* size of m_size */
# define M_OFFSET (sizeof(long_u))
#else
	/* size of m_size */
# define M_OFFSET (sizeof(short_u))
#endif

/*
 * Allocate a block of memory and link it in the allocated block list.
 */
	static char_u *
u_blockalloc(size)
	long_u	size;
{
	struct m_block *p;
	struct m_block *mp, *next;

	p = (struct m_block *)lalloc(size + sizeof(struct m_block), TRUE);
	if (p != NULL)
	{
		 /* Insert the block into the allocated block list, keeping it
		 			sorted on address. */
		for (mp = &curbuf->b_block_head; (next = mp->mb_next) != NULL && next < p; mp = next)
			;
		p->mb_next = next;				/* link in block list */
		mp->mb_next = p;
		p->mb_info.m_next = NULL;		/* clear free list */
		p->mb_info.m_size = 0;
		curbuf->b_mb_current = p;		/* remember current block */
		curbuf->b_m_search = NULL;
		p++;							/* return usable memory */
	}
	return (char_u *)p;
}

/*
 * free all allocated memory blocks for the buffer 'buf'
 */
	void
u_blockfree(buf)
	BUF		*buf;
{
	struct m_block	*p, *np;

	for (p = buf->b_block_head.mb_next; p != NULL; p = np)
	{
		np = p->mb_next;
		free(p);
	}
	buf->b_block_head.mb_next = NULL;
	buf->b_m_search = NULL;
	buf->b_mb_current = NULL;
}

/*
 * Free a chunk of memory.
 * Insert the chunk into the correct free list, keeping it sorted on address.
 */
	static void
u_free_line(ptr)
	char_u *ptr;
{
	register info_t		*next;
	register info_t		*prev, *curr;
	register info_t		*mp;
	struct m_block		*nextb;

	if (ptr == NULL || ptr == IObuff)
		return;	/* illegal address can happen in out-of-memory situations */

	mp = (info_t *)(ptr - M_OFFSET);

		/* find block where chunk could be a part off */
		/* if we change curbuf->b_mb_current, curbuf->b_m_search is set to NULL */
	if (curbuf->b_mb_current == NULL || mp < (info_t *)curbuf->b_mb_current)
	{
		curbuf->b_mb_current = curbuf->b_block_head.mb_next;
		curbuf->b_m_search = NULL;
	}
	if ((nextb = curbuf->b_mb_current->mb_next) != NULL && (info_t *)nextb < mp)
	{
		curbuf->b_mb_current = nextb;
		curbuf->b_m_search = NULL;
	}
	while ((nextb = curbuf->b_mb_current->mb_next) != NULL && (info_t *)nextb < mp)
		curbuf->b_mb_current = nextb;

	curr = NULL;
	/*
	 * If mp is smaller than curbuf->b_m_search->m_next go to the start of
	 * the free list
	 */
	if (curbuf->b_m_search == NULL || mp < (curbuf->b_m_search->m_next))
		next = &(curbuf->b_mb_current->mb_info);
	else
		next = curbuf->b_m_search;
	/*
	 * The following loop is executed very often.
	 * Therefore it has been optimized at the cost of readability.
	 * Keep it fast!
	 */
#ifdef SLOW_BUT_EASY_TO_READ
	do
	{
		prev = curr;
		curr = next;
		next = next->m_next;
	}
	while (mp > next && next != NULL);
#else
	do										/* first, middle, last */
	{
		prev = next->m_next;				/* curr, next, prev */
		if (prev == NULL || mp <= prev)
		{
			prev = curr;
			curr = next;
			next = next->m_next;
			break;
		}
		curr = prev->m_next;				/* next, prev, curr */
		if (curr == NULL || mp <= curr)
		{
			prev = next;
			curr = prev->m_next;
			next = curr->m_next;
			break;
		}
		next = curr->m_next;				/* prev, curr, next */
	}
	while (mp > next && next != NULL);
#endif

/* if *mp and *next are concatenated, join them into one chunk */
	if ((char_u *)mp + mp->m_size == (char_u *)next)
	{
		mp->m_size += next->m_size;
		mp->m_next = next->m_next;
	}
	else
		mp->m_next = next;

/* if *curr and *mp are concatenated, join them */
	if (prev != NULL && (char_u *)curr + curr->m_size == (char_u *)mp)
	{
		curr->m_size += mp->m_size;
		curr->m_next = mp->m_next;
		curbuf->b_m_search = prev;
	}
	else
	{
		curr->m_next = mp;
		curbuf->b_m_search = curr;	/* put curbuf->b_m_search before freed chunk */
	}
}

/*
 * Allocate and initialize a new line structure with room for at least
 * 'size' characters plus a terminating NUL.
 */
	static char_u *
u_alloc_line(size)
	register unsigned size;
{
	register info_t *mp, *mprev, *mp2;
	struct m_block	*mbp;
	int		 		size_align;

/*
 * Add room for size field and trailing NUL byte.
 * Adjust for minimal size (must be able to store info_t
 * plus a trailing NUL, so the chunk can be released again)
 */
	size += M_OFFSET + 1;
	if (size < sizeof(info_t) + 1)
	  size = sizeof(info_t) + 1;

/*
 * round size up for alignment
 */
	size_align = (size + ALIGN_MASK) & ~ALIGN_MASK;

/*
 * If curbuf->b_m_search is NULL (uninitialized free list) start at
 * curbuf->b_block_head
 */
	if (curbuf->b_mb_current == NULL || curbuf->b_m_search == NULL)
	{
		curbuf->b_mb_current = &curbuf->b_block_head;
		curbuf->b_m_search = &(curbuf->b_block_head.mb_info);
	}

/* search for space in free list */
	mprev = curbuf->b_m_search;
	mbp = curbuf->b_mb_current;
	mp = curbuf->b_m_search->m_next;
	if (mp == NULL)
	{
		if (mbp->mb_next)
			mbp = mbp->mb_next;
		else
			mbp = &curbuf->b_block_head;
		mp = curbuf->b_m_search = &(mbp->mb_info);
	}
	while (mp->m_size < size)
	{
		if (mp == curbuf->b_m_search)		/* back where we started in free chunk list */
		{
			if (mbp->mb_next)
				mbp = mbp->mb_next;
			else
				mbp = &curbuf->b_block_head;
			mp = curbuf->b_m_search = &(mbp->mb_info);
			if (mbp == curbuf->b_mb_current)	/* back where we started in block list */
			{
				int		n = (size_align > (MEMBLOCKSIZE / 4) ? size_align : MEMBLOCKSIZE);

				mp = (info_t *)u_blockalloc((long_u)n);
				if (mp == NULL)
					return (NULL);
				mp->m_size = n;
				u_free_line((char_u *)mp + M_OFFSET);
				mp = curbuf->b_m_search;
				mbp = curbuf->b_mb_current;
			}
		}
		mprev = mp;
		if ((mp = mp->m_next) == NULL)		/* at end of the list */
			mp = &(mbp->mb_info);			/* wrap around to begin */
	}

/* if the chunk we found is large enough, split it up in two */
	if ((long)mp->m_size - size_align >= (long)(sizeof(info_t) + 1))
	{
		mp2 = (info_t *)((char_u *)mp + size_align);
		mp2->m_size = mp->m_size - size_align;
		mp2->m_next = mp->m_next;
		mprev->m_next = mp2;
		mp->m_size = size_align;
	}
	else					/* remove *mp from the free list */
	{
		mprev->m_next = mp->m_next;
	}
	curbuf->b_m_search = mprev;
	curbuf->b_mb_current = mbp;

	mp = (info_t *)((char_u *)mp + M_OFFSET);
	*(char_u *)mp = NUL;					/* set the first byte to NUL */

	return ((char_u *)mp);
}

/*
 * u_save_line(): allocate memory with u_alloc_line() and copy line 'lnum' into it.
 */
	static char_u *
u_save_line(lnum)
	linenr_t	lnum;
{
	register char_u *src;
	register char_u *dst;
	register unsigned len;

	src = ml_get(lnum);
	len = STRLEN(src);
	if ((dst = u_alloc_line(len)) != NULL)
		memmove((char *)dst, (char *)src, (size_t)(len + 1));
	return (dst);
}
