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
 * ops.c: implementation of various operators: doshift, dodelete, dotilde,
 *		  dochange, doyank, doput, dojoin
 */

#include "vim.h"
#include "ops.h"

/*
 * We have one yank buffer for normal yanks and puts, nine yank buffers for
 * deletes and 26 yank buffers for use by name.
 * Each yank buffer is an array of pointers to lines.
 */
static struct yankbuf
{
	char		**y_array;		/* pointer to array of line pointers */
	linenr_t 	y_size; 		/* number of lines in y_array */
	char		y_type; 		/* MLINE or MCHAR */
} y_buf[36];					/* 0..9 = number buffers, 10..35 = char buffers */

static struct yankbuf *y_current;		/* pointer to current yank buffer */
static bool_t yankappend;				/* TRUE when appending */
static struct yankbuf *y_delete = NULL; /* pointer to yank buffer of last delete */

static void		get_yank_buffer __ARGS((void));
static bool_t	stuff_yank __ARGS((int, char *));
static void		free_yank __ARGS((long));
static void		free_yank_all __ARGS((void));

/*
 * doshift - handle a shift operation
 */
	void
doshift(op)
	int 			op;
{
	register int i;

	if (!u_save((linenr_t)(Curpos.lnum - 1), (linenr_t)(Curpos.lnum + nlines)))
		return;

	Curpos.lnum += nlines;		/* start with last line, leave cursor on first */
	for (i = nlines; --i >= 0; )
		if (lineempty(--Curpos.lnum))
			Curpos.col = 0;
		else
			shift_line(op == LSHIFT);

	cursupdate();
	updateScreen(NOT_VALID);

	if (nlines > P(P_RP))
		smsg("%ld lines %ced", nlines, (op == RSHIFT) ? '>' : '<');
}

/*
 * shift the current line one shiftwidth left (if left != 0) or right
 * leaves cursor on first blank in the line
 */
	void
shift_line(left)
	int left;
{
	register int count;
	register int i, j;

	count = get_indent();			/* get current indent */

	if (P(P_SR))		/* round off indent */
	{
		i = count / P(P_SW);			/* compute new indent */
		j = count % P(P_SW);
		if (j)
		{
			if (!left)
				++i;
		}
		else if (left)
		{
			if (i)
				--i;
		}
		else
			++i;
		count = i * P(P_SW);
	}
	else				/* original vi indent */
	{
		if (left)
		{
			count -= P(P_SW);
			if (count < 0)
				count = 0;
		}
		else
			count += P(P_SW);
	}
	set_indent(count, (bool_t)TRUE);		/* set new indent */
}

/*
 * Set y_current and yankappend, according to the value of yankbuffer.
 */
	static void
get_yank_buffer()
{
		register int i;

		yankappend = FALSE;
		if (i = yankbuffer) 	/* yes, it's an assign */
		{
				if (isdigit(i))
						i -= '0';
				else if (islower(i))
						i -= 'a' - 10;
				else if (isupper(i))
				{
						i -= 'A' - 10;
						yankappend = TRUE;
				}
				else
						i = 0;
		}
		y_current = &(y_buf[i]);
}

/*
 * (stop) recording into a yank buffer
 */
	bool_t
dorecord(c)
	int c;
{
		char *p, *lp;
		static int bufname;

		if (Recording == FALSE) 		/* start recording */
		{
				if (!isalpha(c))
						return FALSE;
				Recording = TRUE;
				if (P(P_MO))
						showmode();
				bufname = c;
				return TRUE;
		}
		else							/* stop recording */
		{
				Recording = FALSE;
				if (P(P_MO))
						msg("");
				p = (char *)get_recorded();
				if (p == NULL)
						return FALSE;
				lp = strrchr(p, 'v');	/* delete the trailing 'v' */
				if (lp != NULL)
						*lp = NUL;
				return (stuff_yank(bufname, p));
		}
}

/*
 * stuff string 'p' into yank buffer 'bufname' (append if uppercase)
 * 'p' is assumed to be alloced.
 */
	static bool_t
stuff_yank(bufname, p)
	int bufname;
	char *p;
{
		char *lp;
		char **pp;

		yankbuffer = bufname;
		get_yank_buffer();
		if (yankappend && y_current->y_array != NULL)
		{
				pp = &(y_current->y_array[y_current->y_size - 1]);
				lp = alloc((unsigned)(strlen(*pp) + strlen(p) + 1));
				if (lp == NULL)
				{
						free(p);
						return FALSE;
				}
				strcpy(lp, *pp);
				strcat(lp, p);
				free(p);
				free(*pp);
				*pp = lp;
		}
		else
		{
				free_yank_all();
				if ((y_current->y_array = (char **)alloc((unsigned)sizeof(char *))) == NULL)
				{
						free(p);
						return FALSE;
				}
				y_current->y_array[0] = p;
				y_current->y_size = 1;
				y_current->y_type = MCHAR;	/* used to be MLINE, why? */
		}
		return TRUE;
}

/*
 * execute a yank buffer: copy it into the stuff buffer
 */
doexecbuf(c)
	int c;
{
		static int lastc = NUL;
		long i;

		if (c == '@')			/* repeat previous one */
				c = lastc;

		lastc = c;
		if (!isalpha(c))
				return FALSE;

		yankbuffer = c;
		get_yank_buffer();
		if (y_current->y_array == NULL)
				return FALSE;

		for (i = y_current->y_size; --i >= 0; )
		{
					/* insert newline after last line if type is MLINE and
																between lines */
				if (y_current->y_type == MLINE || i < y_current->y_size - 1)
				{
					if (ins_mapbuf("\n") < 0)
						return FALSE;
				}
				if (ins_mapbuf(y_current->y_array[i]) < 0)
					return FALSE;
		}

		return TRUE;
}

/*
 * dodelete - handle a delete operation
 */
	void
dodelete()
{
	register int	n;

	/*
	 * Do a yank of whatever we're about to delete. If there's too much stuff
	 * to fit in the yank buffer, then get a confirmation before doing the
	 * delete. This is crude, but simple. And it avoids doing a delete of
	 * something we can't put back if we want.
	 */
	if (yankbuffer == 0)				/* normal delete: shift number buffers */
	{
		y_current = &y_buf[9];
		free_yank_all();				/* free buffer nine */
		for (n = 9; n > 1; --n)
				y_buf[n] = y_buf[n - 1];
		y_current = &y_buf[1];
		y_buf[1].y_array = NULL;		/* set buffer one to empty */
	}
	else								/* yank into specified buffer */
		get_yank_buffer();
	y_delete = y_current;		/* remember the buffer we delete into for doput() */

	if (!doyank((bool_t)TRUE))
	{
		if (ask_yesno("cannot yank; delete anyway") != 'y')
		{
				emsg("command aborted");
				return;
		}
	}

	if (!u_save((linenr_t)(startop.lnum - 1), (linenr_t)(endop.lnum + 1)))
		return;

	if (mtype == MLINE)
	{
		u_clearline();	/* "U" command should not be possible after "dd" */
		if (operator == CHANGE)
		{
			delline((long)(nlines - 1), (bool_t)TRUE);
			Curpos.col = 0;
			while (delchar((bool_t)TRUE));
		}
		else
		{
			delline(nlines, (bool_t)TRUE);
		}
	}
	else if (nlines == 1)		/* del. within line */
	{
		n = endop.col - startop.col + 1 - oneless;
		while (n--)
			if (!delchar((bool_t)TRUE))
				break;
	}
	else						/* del. between lines */
	{
		n = Curpos.col;
		while (Curpos.col >= n)
			if (!delchar((bool_t)TRUE))
				break;

		startop = Curpos;		/* remember Curpos */
		++Curpos.lnum;
		delline((long)(nlines - 2), (bool_t)TRUE);
		Curpos.col = 0;
		n = endop.col - oneless;

		while (n-- >= 0)
			if (!delchar((bool_t)TRUE))
				break;
		Curpos = startop;		/* restore Curpos */
		dojoin((bool_t)FALSE);
	}

	cursupdate();
	if (mtype == MCHAR && nlines == 1 /* && P(P_NU) == FALSE */ || operator == CHANGE)
		updateline();
	/* already done by cursupdate()
	else
		updateScreen(NOT_VALID); */

	msgmore(-nlines);
}

/*
 * dotilde - handle the (non-standard vi) tilde operator
 */
	void
dotilde()
{
		register char	c;

		if (!u_save((linenr_t)(startop.lnum - 1), (linenr_t)(endop.lnum + 1)))
				return;

		if (mtype == MLINE)
		{
				startop.col = 0;
				endop.col = strlen(nr2ptr(endop.lnum));
				if (endop.col)
						--endop.col;
		}
		else if (oneless)
				dec(&endop);

		for ( ; ltoreq(startop, endop); inc(&startop))
		{
				/*
				 * Swap case through the range
				 */
				c = gchar(&startop);
				if (isalpha(c))
				{
						pchar(startop, c ^ 0x20);	/* Change current character */
						CHANGED;
				}
		}

		cursupdate();
		if (mtype == MCHAR && nlines == 1)
				updateline();
		else
				updateScreen(NOT_VALID);

		if (nlines > P(P_RP))
				smsg("%ld lines ~ed", nlines);
}

/*
 * dochange - handle a change operation
 */
	void
dochange()
{
	register colnr_t 		   l;

	l = startop.col;

	dodelete();

	if ((l > Curpos.col) && !lineempty(Curpos.lnum))
		incCurpos();

	startinsert(NUL, (bool_t)FALSE, (linenr_t)1);
}

/*
 * set all the yank buffers to empty (called from main())
 */
	void
init_yank()
{
		register int i;

		for (i = 0; i < 36; ++i)
				y_buf[i].y_array = NULL;
}

/*
 * Free "n" lines from the current yank buffer.
 * Called for normal freeing and in case of error.
 */
	static void
free_yank(n)
	long n;
{
	if (y_current->y_array != NULL)
	{
		register long i;

		for (i = n; --i >= 0; )
		{
			if (i % 1000 == 999)					/* this may take a while */
				smsg("freeing %ld lines", i + 1);
			free(y_current->y_array[i]);
		}
		free((char *)y_current->y_array);
		y_current->y_array = NULL;
		if (n >= 1000)
			msg("");
	}
}

	static void
free_yank_all()
{
		free_yank(y_current->y_size);
}

/*
 * Yank the text between Curpos and startpos into a yank buffer.
 * If we are to append ("uppercase), we first yank into a new yank buffer and
 * then concatenate the old and the new one (so we keep the old one in case
 * of out-of-memory).
 */
	bool_t
doyank(deleting)
	bool_t deleting;
{
	long 				i;				/* index in y_array[] */
	struct yankbuf		*curr;		/* copy of y_current */
	struct yankbuf		new; 		/* new yank buffer when appending */
	char				**new_ptr;
	register linenr_t	lnum;			/* current line number */
	long 				j;

	if (!deleting)
	{
		y_delete = NULL;
		get_yank_buffer();
	}

	curr = y_current;
	if (yankappend && y_current->y_array != NULL) /* append to existing contents */
		y_current = &new;
	else
		free_yank_all();		/* free previously yanked lines */

	y_current->y_size = nlines;
	y_current->y_type = mtype;	/* set the yank buffer type */
	y_current->y_array = (char **)alloc((unsigned)(sizeof(char *) * nlines));

	if (y_current->y_array == NULL)
	{
		y_current = curr;
		return FALSE;
	}

/*
 * there are three parts:
 * 1. if mtype != MLINE yank last part of the top line
 * 2. yank the lines between startop and endop, inclusive when mtype == MLINE
 * 3. if mtype != MLINE yank first part of the bot line
 */
	i = 0;
	lnum = startop.lnum;
	if (mtype != MLINE)
	{
		if (nlines == 1)		/* startop and endop on same line */
		{
				j = endop.col - startop.col + 1 - oneless;
				if ((y_current->y_array[0] = strnsave(nr2ptr(lnum) + startop.col, (int)j)) == NULL)
				{
fail:
						free_yank(i);	/* free the lines that we allocated */
						y_current = curr;
						return FALSE;
				}
				goto success;
		}
		if ((y_current->y_array[0] = strsave(nr2ptr(lnum++) + startop.col)) == NULL)
				goto fail;
		++i;
	}

	while (mtype == MLINE ? (lnum <= endop.lnum) : (lnum < endop.lnum))
	{
		if ((y_current->y_array[i] = strsave(nr2ptr(lnum++))) == NULL)
				goto fail;
		++i;
	}
	if (mtype != MLINE)
	{
		if ((y_current->y_array[i] = strnsave(nr2ptr(endop.lnum), endop.col + 1 - oneless)) == NULL)
				goto fail;
	}

success:
	if (curr != y_current)		/* append the new block to the old block */
	{
		new_ptr = (char **)alloc((unsigned)(sizeof(char *) * (curr->y_size + y_current->y_size)));
		if (new_ptr == NULL)
				goto fail;
		for (j = 0; j < curr->y_size; ++j)
				new_ptr[j] = curr->y_array[j];
		free(curr->y_array);
		curr->y_array = new_ptr;

		if (mtype == MLINE) 	/* MLINE overrides MCHAR */
				curr->y_type = MLINE;
		if (curr->y_type == MCHAR)		/* concatenate the last line of the old
										block with the first line of the new block */
		{
				new_ptr = (char **)alloc((unsigned)(strlen(curr->y_array[curr->y_size - 1]) + strlen(y_current->y_array[0]) + 1));
				if (new_ptr == NULL)
				{
						i = y_current->y_size - 1;
						goto fail;
				}
				strcpy((char *)new_ptr, curr->y_array[--j]);
				strcat((char *)new_ptr, y_current->y_array[0]);
				free(curr->y_array[j]);
				free(y_current->y_array[0]);
				curr->y_array[j++] = (char *)new_ptr;
				i = 1;
		}
		else
				i = 0;
		while (i < y_current->y_size)
				curr->y_array[j++] = y_current->y_array[i++];
		curr->y_size = j;
		free(y_current->y_array);
		y_current = curr;
	}
	if (operator == YANK && nlines > P(P_RP))
			smsg("%ld lines yanked", (long)(y_current->y_size));

	return TRUE;
}

	void
doput(dir, count)
	int dir;
	long count;
{
	char		*ptr, *ep;
	int 		newlen;
	linenr_t	startlnum;
	int 		startcol;
	linenr_t	lnum;
	long 		i;		/* index in y_array[] */
	int 		y_type;
	long 		y_size;
	char		**y_array;
	long 		nlines = 0;

	if (!u_saveCurpos())
		return;
	get_yank_buffer();
	if (yankbuffer == 0 && y_delete != NULL)
		y_current = y_delete;

	y_type = y_current->y_type;
	y_size = y_current->y_size;
	y_array = y_current->y_array;

	if (count == -1)		/* :put command */
	{
		y_type = MLINE;
		count = 1;
	}

	if (y_size == 0 || y_array == NULL)
	{
		beep();
		return;
	}
	newlen = strlen(y_array[0]);
	CHANGED;

	startlnum = Curpos.lnum;
	if (y_type == MCHAR)
	{
		/*
		 * if type is MCHAR, FORWARD is the same as BACKWARD on the next character
		 */
		startcol = Curpos.col;
		if (dir == FORWARD && gcharCurpos() != NUL)
		{
				++startcol;
				if (newlen)
						++Curpos.col;
		}
	}
	else if (dir == BACKWARD)
		/*
		 * if type is MLINE, BACKWARD is the same as FORWARD on the previous line
		 */
		--startlnum;

	/*
	 * simple case: insert into current line
	 */
	if (y_type == MCHAR && y_size == 1)
	{
		i = count * newlen;
		if (!canincrease((int)i))
				return; 				/* alloc() will give error message */
		ptr = nr2ptr(startlnum);
		ep = ptr + startcol;
		for (ptr += strlen(ptr); ptr >= ep; --ptr)
				*(ptr + i) = *ptr;
		Curpos.col += i - 1;		/* put cursor on last putted char */
		for (i = 0; i < count; ++i)
		{
				strncpy(ep, y_array[0], (size_t)newlen);
				ep += newlen;
		}
		updateline();
	}
	else
	{
		while (--count >= 0)
		{
			i = 0;
			lnum = startlnum;
			if (y_type == MCHAR)
			{
				/*
				 * split the current line in two at the insert position
				 */
				ptr = nr2ptr(lnum) + startcol;
				ep = save_line(ptr);
				if (ep == NULL)
						goto error;
				appendline(lnum, ep);
				*ptr = NUL;
				if (!canincrease(newlen))		/* lnum == Curpos.lnum! */
						goto error;
				strcat(nr2ptr(lnum), y_array[0]);
				i = 1;
				++nlines;
			}

			while (y_type == MCHAR ? (i < y_size - 1) : (i < y_size))
			{
				ep = save_line(y_array[i++]);
				if (ep == NULL)
						goto error;
				appendline(lnum++, ep);
				++nlines;
			}
			++lnum; 	/* lnum is now number of line below inserted lines */
			if (y_type == MCHAR)		/* insert last line */
			{
				ptr = nr2ptr(lnum);
				ep = alloc_line((unsigned)(strlen(ptr) + strlen(y_array[i])));
				if (ep == NULL)
						goto error;
				strcpy(ep, y_array[i]);
				strcat(ep, ptr);
				free_line(replaceline(lnum, ep));
			}
		}

		if (y_type == MLINE)
		{
				Curpos.col = 0;
				if (dir == FORWARD)
				{
						updateScreen(NOT_VALID);		/* recompute Botline */
						++Curpos.lnum;
				}
		}

error:
		beginline((bool_t)TRUE);
		cursupdate();
		updateScreen(NOT_VALID);
	}

	msgmore(nlines);
	set_want_col = TRUE;
}

/*
 * display the contents of the yank buffers
 */
	void
dodis()
{
		register int i, n;
		register long j;
		register char *p;
		register struct yankbuf *yb;

		setmode(0);			/* set cooked mode so output can be halted */
		for (i = 0; i < 36; ++i)
		{
				if (i == 0 && y_delete != NULL)
						yb = y_delete;
				else
						yb = &(y_buf[i]);
				if (yb->y_array != NULL)
				{
						if (i == 0)
								outstr("pP");
						else
						{
								outchar('"');
								if (i < 10)
										outchar(i + '0');
								else
										outchar(i + 'a' - 10);
						}
						outchar(' ');

						n = Columns - 4;
						for (j = 0; j < yb->y_size && n > 0; ++j)
						{
								if (j)
								{
										outstr("^J");
										n -= 2;
								}
								for (p = yb->y_array[j]; *p && n > 0; ++p)
								{
										outstr(transchar(*p));
										n -= charsize(*p);
								}
						}
						outchar('\n');
						flushbuf();
				}
		}
		setmode(1);
		wait_return((bool_t)TRUE);
}

/*
 * join 'count' lines (minimal 2), including u_save()
 */
	void
dodojoin(count, flag)
	long	count;
	bool_t	flag;
{
		if (!u_save((linenr_t)(Curpos.lnum - 1), (linenr_t)(Curpos.lnum + count)))
				return;

		while (--count)
				if (!dojoin(flag))
				{
						beep();
						break;
				}

		updateScreen(VALID_TO_CURSCHAR);
}

	bool_t
dojoin(insert_space)
	bool_t			insert_space;
{
	char		*curr;
	char		*next;
	int 		currsize;		/* size of the current line */
	int 		nextsize;		/* size of the next line */
	int			spaces;			/* number of spaces to insert */

	if (Curpos.lnum == line_count)		/* on last line */
		return FALSE;

	curr = nr2ptr(Curpos.lnum);
	currsize = strlen(curr);
	next = nr2ptr((linenr_t)(Curpos.lnum + 1));
	spaces = 0;
	if (insert_space)
	{
		skipspace(&next);
		if (P(P_JS))
			spaces = 2;		/* vi compatible */
		else
			spaces = 1;		/* what I like */
		if (currsize && (*(curr + currsize - 1) == ' ' ||
						*(curr + currsize - 1) == TAB) ||
						*next == ')')
				spaces = 0;
	}
	nextsize = strlen(next);
	if (!canincrease(nextsize + spaces))
		return FALSE;

	curr = nr2ptr(Curpos.lnum); /* canincrease() will have changed the pointer */

	while (spaces)
	{
		*(curr + currsize++) = ' ';
		--spaces;
	}
	strcpy(curr + currsize, next);

	/*
	 * Delete the following line. To do this we move the cursor there
	 * briefly, and then move it back. Don't back up if the delete made us
	 * the last line.
	 */
	++Curpos.lnum;

	if (Curpos.lnum != line_count)
	{
		delline(1L, (bool_t)TRUE);
		--Curpos.lnum;
	}
	else
		delline(1L, (bool_t)TRUE);

	if (currsize == 0)
		Curpos.col = 0;
	else
	{
		Curpos.col = currsize - 1;
		oneright(); 	/* go to first char. of joined line */
	}
	CHANGED;

	return TRUE;
}

	void
startinsert(initstr, startln, count)
	int			initstr;
	int 		startln;		/* if set, insert at start of line */
	long 		count;
{
	Insstart = Curpos;
	if (startln)
		Insstart.col = 0;

	if (initstr != NUL)
	{
			ResetBuffers();
			AppendNumberToRedobuff(count);
			AppendToRedobuff(mkstr(initstr));
	}

	if (initstr == 'R')
		State = REPLACE;
	else
		State = INSERT;

	if (P(P_MO))
		showmode();

	edit(count);
}
