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
 * undo.c: multi level undo facility
 *
 * The saved lines are stored in a list of lists:
 *
 * u_oldhead----------------------------------------------+
 *                                                        |
 *                                                        V
 *              +--------------+    +--------------+    +--------------+
 * u_newhead--->| u_header     |    | u_header     |    | u_header     |
 *              |     uh_next------>|     uh_next------>|     uh_next---->NULL
 *       NULL<--------uh_prev  |<---------uh_prev  |<---------uh_prev  |
 *              |     uh_entry |    |     uh_entry |    |     uh_entry |
 *              +--------|-----+    +--------|-----+    +--------|-----+
 *                       |                   |                   |
 *                       V                   V                   V
 *              +--------------+    +--------------+    +--------------+
 *              | u_entry      |    | u_entry      |    | u_entry      |
 *              |     ue_next  |    |     ue_next  |    |     ue_next  |
 *              +--------|-----+    +--------|-----+    +--------|-----+
 *                       |                   |                   |
 *                       V                   V                   V
 *              +--------------+            NULL                NULL
 *              | u_entry      |
 *              |     ue_next  |
 *              +--------|-----+
 *                       |
 *                       V
 *                      etc.
 *
 * Each u_entry list contains the information for one undo or redo.
 * u_curhead points to the header of the last undo (the next redo), or is
 * NULL if nothing has been undone.
 *
 * All data is allocated with alloc_line(), thus it will be freed as soon as
 * we switch files!
 */

#include "vim.h"

struct u_entry
{
	struct u_entry	*ue_next;	/* pointer to next entry in list */
	linenr_t		ue_top;		/* number of line above undo block */
	linenr_t		ue_bot;		/* number of line below undo block */
	char			*ue_botptr;	/* pointer to line below undo block */
	char			**ue_array;	/* array of lines in undo block */
	long			ue_size;	/* number of lines in ue_array */
};

struct u_header
{
	struct u_header	*uh_next;	/* pointer to next header in list */
	struct u_header	*uh_prev;	/* pointer to previous header in list */
	struct u_entry	*uh_entry;	/* pointer to first entry */
	FPOS			 uh_curpos;	/* cursor position before saving */
};

static struct u_header *u_oldhead = NULL;	/* pointer to oldest header */
static struct u_header *u_newhead = NULL;	/* pointer to newest header */
static struct u_header *u_curhead = NULL;	/* pointer to current header */
static int				u_numhead = 0;		/* current number of headers */
static bool_t			u_synced = TRUE;	/* entry lists are synced */

/*
 * variables for "U" command
 */
static char	   *u_line_ptr = NULL;		/* saved line for "U" command */
static linenr_t u_line_lnum;			/* line number of line in u_line */
static colnr_t	u_line_colnr;			/* optional column number */

static void u_getbot();
static int u_savecommon __ARGS((linenr_t, linenr_t, int, char *));
static void u_undoredo();
static void u_freelist __ARGS((struct u_header *));
static void u_freeentry __ARGS((struct u_entry *, long));

/*
 * save the current line for both the "u" and "U" command
 */
	bool_t
u_saveCurpos()
{
	return (u_save((linenr_t)(Curpos.lnum - 1), (linenr_t)(Curpos.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and bot may be line_count + 1.
 * Returns FALSE when lines could not be saved.
 */
	bool_t
u_save(top, bot)
	linenr_t top, bot;
{
	if (top > line_count || top >= bot || bot > line_count + 1)
		return FALSE;	/* rely on caller to do error messages */

	if (top + 2 == bot)
		u_saveline((linenr_t)(top + 1));

	return (u_savecommon(top, bot, 0, (char *)0));
}

/*
 * save the line "lnum", pointed at by "ptr" (used by :g//s commands)
 * "ptr" is handed over to the undo routines
 */
	bool_t
u_savesub(lnum, ptr)
	linenr_t	lnum;
	char		*ptr;
{
	return (u_savecommon(lnum - 1, lnum + 1, 1, ptr));
}

/*
 * save the line "lnum", pointed at by "ptr" (used by :g//d commands)
 * "ptr" is handed over to the undo routines
 */
	bool_t
u_savedel(lnum, ptr)
	linenr_t	lnum;
	char		*ptr;
{
	return (u_savecommon(lnum - 1, lnum, 1, ptr));
}

	static int 
u_savecommon(top, bot, flag, ptr)
	linenr_t top, bot;
	int flag;
	char *ptr;
{
	linenr_t		lnum;
	long			i;
	struct u_header *uhp;
	struct u_entry	*uep;
	long			size;

	/*
	 * if u_synced == TRUE make a new header
	 */
	if (u_synced)
	{
		/*
		 * if we undid more than we redid, free the entry lists before and
		 * including u_curhead
		 */
		while (u_curhead != NULL)
			u_freelist(u_newhead);

		/*
		 * free headers to keep the size right
		 */
		while (u_numhead > P(P_UL) && u_oldhead != NULL)
			u_freelist(u_oldhead);

		/*
		 * make a new header entry
		 */
		uhp = (struct u_header *)alloc_line((unsigned)sizeof(struct u_header));
		if (uhp == NULL)
			goto nomem;
		uhp->uh_prev = NULL;
		uhp->uh_next = u_newhead;
		if (u_newhead != NULL)
			u_newhead->uh_prev = uhp;
		uhp->uh_entry = NULL;
		uhp->uh_curpos = Curpos;	/* save cursor position for undo */
		u_newhead = uhp;
		if (u_oldhead == NULL)
			u_oldhead = uhp;
		++u_numhead;
	}
	else	/* find line number for ue_botptr for previous u_save() */
		u_getbot();

	/*
	 * add lines in front of entry list
	 */
	uep = (struct u_entry *)alloc_line((unsigned)sizeof(struct u_entry));
	if (uep == NULL)
		goto nomem;

	if (flag)
		size = 1;
	else
		size = bot - top - 1;
	uep->ue_size = size;
	uep->ue_top = top;
	uep->ue_botptr = NULL;
	if (flag)
		uep->ue_bot = bot;
	else if (bot > line_count)
		uep->ue_bot = 0;
	else
		uep->ue_botptr = nr2ptr(bot);	/* we have to do ptr2nr(ue_botptr) later */

	if (size)
	{
		if ((uep->ue_array = (char **)alloc_line((unsigned)(sizeof(char *) * size))) == NULL)
		{
			u_freeentry(uep, 0L);
			goto nomem;
		}
		if (flag)
			uep->ue_array[0] = ptr;
		else
			for (i = 0, lnum = top + 1; i < size; ++i)
				if ((uep->ue_array[i] = save_line(nr2ptr(lnum++))) == NULL)
				{
					u_freeentry(uep, i);
					goto nomem;
				}
	}
	uep->ue_next = u_newhead->uh_entry;
	u_newhead->uh_entry = uep;
	u_synced = FALSE;
	return TRUE;

nomem:
	if (flag)
		free_line(ptr);
	else if (ask_yesno("no undo possible; continue anyway") == 'y')
		return TRUE;
	return FALSE;
}

	void
u_undo(count)
	int count;
{
	while (count--)
	{
		if (u_curhead == NULL)						/* first undo */
			u_curhead = u_newhead;
		else if (P(P_UL) != 0)						/* multi level undo */
			u_curhead = u_curhead->uh_next;			/* get next undo */

		if (u_numhead == 0 || u_curhead == NULL)	/* nothing to undo */
		{
			u_curhead = u_oldhead;					/* stick u_curhead at end */
			beep();
			return;
		}

		u_undoredo();
	}
}

	void
u_redo(count)
	int count;
{
	while (count--)
	{
		if (u_curhead == NULL || P(P_UL) == 0)		/* nothing to redo */
		{
			beep();
			return;
		}

		u_undoredo();

		u_curhead = u_curhead->uh_prev;			/* advance for next redo */
	}
}

/*
 * u_undoredo: common code for undo and redo
 *
 * The lines in the file are replaced by the lines in the entry list at u_curhead.
 * The replaced lines in the file are saved in the entry list for the next undo/redo.
 */
	static void
u_undoredo()
{
	char		**newarray;
	linenr_t	oldsize;
	linenr_t	newsize;
	linenr_t	top, bot;
	linenr_t	lnum;
	linenr_t	newlnum = INVLNUM;
	long		i;
	long		count = 0;
	struct u_entry *uep, *nuep;
	struct u_entry *newlist = NULL;

	if (u_synced == FALSE)
	{
		emsg("undo not synced");
		return;
	}

	CHANGED;
	for (uep = u_curhead->uh_entry; uep != NULL; uep = nuep)
	{
			top = uep->ue_top;
			bot = uep->ue_bot;
			if (bot == 0)
				bot = line_count + 1;
			if (top > line_count || top >= bot)
			{
				emsg("u_undo: line numbers wrong");
				return;
			}

			if (top < newlnum)
			{
				newlnum = top;
				Curpos.lnum = top + 1;
			}
			oldsize = bot - top - 1;	/* number of lines before undo */

			newsize = uep->ue_size;		/* number of lines after undo */

			/* delete the lines between top and bot and save them in newarray */
			if (oldsize)
			{
				if ((newarray = (char **)alloc_line((unsigned)(sizeof(char *) * oldsize))) == NULL)
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
				/* delete backwards, it goes faster in some cases */
				for (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
					newarray[i] = delsline(lnum);
			}
			/* insert the lines in u_array between top and bot */
			if (newsize)
			{
				for (lnum = top, i = 0; i < newsize; ++i, ++lnum)
					appendline(lnum, uep->ue_array[i]);
				free_line((char *)uep->ue_array);
			}
			count += newsize - oldsize;
			uep->ue_size = oldsize;
			uep->ue_array = newarray;
			uep->ue_bot = lnum + 1;

			/*
			 * insert this entry in front of the new entry list
			 */
			nuep = uep->ue_next;
			uep->ue_next = newlist;
			newlist = uep;
	}

	u_curhead->uh_entry = newlist;

	msgmore(count);

	if (u_curhead->uh_curpos.lnum == Curpos.lnum)
		Curpos.col = u_curhead->uh_curpos.col;
	else
		Curpos.col = 0;
	cursupdate();
	updateScreen(NOT_VALID);
}

/*
 * u_sync: stop adding to the current entry list
 */
	void
u_sync()
{
	if (u_synced)
		return;				/* already synced */
	u_getbot();				/* compute ue_bot of previous u_undo */
	u_curhead = NULL;
}

/*
 * u_getbot(): compute the line number of the previous u_undo
 */
	static void
u_getbot()
{
	register struct u_entry *uep;

	if (u_newhead == NULL || (uep = u_newhead->uh_entry) == NULL)
	{
		emsg("undo list corrupt");
		return;
	}

	if (uep->ue_botptr != NULL)
		if ((uep->ue_bot = ptr2nr(uep->ue_botptr, uep->ue_top)) == 0)
		{
			emsg("undo line missing");
			return;
		}

	u_synced = TRUE;
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

	if (u_curhead == uhp)
		u_curhead = NULL;

	if (uhp->uh_next == NULL)
		u_oldhead = uhp->uh_prev;
	else
		uhp->uh_next->uh_prev = uhp->uh_prev;

	if (uhp->uh_prev == NULL)
		u_newhead = uhp->uh_next;
	else
		uhp->uh_prev->uh_next = uhp->uh_next;

	free_line((char *)uhp);
	--u_numhead;
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
		free_line(uep->ue_array[--n]);
	free_line((char *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */

	void
u_clearall()
{
	u_newhead = u_oldhead = u_curhead = NULL;
	u_synced = TRUE;
	u_numhead = 0;
	u_line_ptr = NULL;
	u_line_lnum = 0;
}

/*
 * save the line "lnum" for the "U" command
 */
	void
u_saveline(lnum)
	linenr_t lnum;
{
	if (lnum == u_line_lnum)		/* line is already saved */
		return;
	if (lnum < 1 || lnum > line_count)	/* should never happen */
		return;
	u_clearline();
	u_line_lnum = lnum;
	if (Curpos.lnum == lnum)
		u_line_colnr = Curpos.col;
	else
		u_line_colnr = 0;
	u_line_ptr = save_line(nr2ptr(lnum));	/* when out of mem alloc() will give a warning */
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
	void
u_clearline()
{
	if (u_line_ptr != NULL)
	{
		free_line(u_line_ptr);
		u_line_ptr = NULL;
		u_line_lnum = 0;
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

	if (u_line_ptr == NULL || u_line_lnum > line_count)
	{
		beep();
		return;
	}
		/* first save the line for the 'u' command */
	u_savecommon(u_line_lnum - 1, u_line_lnum + 1, 0, (char *)0);
	u_line_ptr = replaceline(u_line_lnum, u_line_ptr);

	t = u_line_colnr;
	if (Curpos.lnum == u_line_lnum)
		u_line_colnr = Curpos.col;
	Curpos.col = t;
	Curpos.lnum = u_line_lnum;
	cursupdate();
	updateScreen(VALID_TO_CURSCHAR);
}
