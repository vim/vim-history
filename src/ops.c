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
#include "globals.h"
#include "proto.h"
#include "param.h"
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
	char		y_type; 		/* MLINE, MCHAR or MBLOCK */
} y_buf[36];					/* 0..9 = number buffers, 10..35 = char buffers */

static struct	yankbuf *y_current;		/* ptr to current yank buffer */
static int		yankappend;				/* TRUE when appending */
static struct	yankbuf *y_previous = NULL; /* ptr to last written yank buffer */

static void		get_yank_buffer __ARGS((int));
static int		stuff_yank __ARGS((int, char *));
static void		free_yank __ARGS((long));
static void		free_yank_all __ARGS((void));
static void		block_prep __ARGS((linenr_t, int));

/* variables use by block_prep, dodelete and doyank */
static int		startspaces;
static int		endspaces;
static int		textlen;
static char		*textstart;
static colnr_t	textcol;

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

	updateScreen(CURSUPD);

	if (nlines > p_report)
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

	if (p_sr)		/* round off indent */
	{
		i = count / p_sw;			/* compute new indent */
		j = count % p_sw;
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
		count = i * p_sw;
	}
	else				/* original vi indent */
	{
		if (left)
		{
			count -= p_sw;
			if (count < 0)
				count = 0;
		}
		else
			count += p_sw;
	}
	set_indent(count, TRUE);		/* set new indent */
}

/*
 * Set y_current and yankappend, according to the value of yankbuffer.
 */
	static void
get_yank_buffer(writing)
	int		writing;
{
	register int i;

	yankappend = FALSE;
	if (yankbuffer == 0 && y_previous != NULL && !writing)
	{
		y_current = y_previous;
		return;
	}
	i = yankbuffer;
	if (isdigit(i))
		i -= '0';
	else if (islower(i))
		i -= 'a' - 10;
	else if (isupper(i))
	{
		i -= 'A' - 10;
		yankappend = TRUE;
	}
	else			/* not 0-9, a-z or A-Z: use buffer 0 */
		i = 0;
	y_current = &(y_buf[i]);
	if (writing)		/* remember the buffer we write into for doput() */
		y_previous = y_current;
}

/*
 * (stop) recording into a yank buffer
 */
	int
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
		showmode();
		bufname = c;
		return TRUE;
	}
	else							/* stop recording */
	{
		Recording = FALSE;
		if (p_mo)
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
	static int
stuff_yank(bufname, p)
	int bufname;
	char *p;
{
	char *lp;
	char **pp;

	yankbuffer = bufname;
	if (yankbuffer == '.')		/* read-only buffer */
		return FALSE;
	get_yank_buffer(TRUE);
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
	int
doexecbuf(c)
	int c;
{
	static int lastc = NUL;
	long i;

	if (c == '@')			/* repeat previous one */
		c = lastc;

	lastc = c;
	if (!isalnum(c))		/* registers 0-9 and a-z are allowed */
		return FALSE;

	yankbuffer = c;
	get_yank_buffer(FALSE);
	if (y_current->y_array == NULL)
		return FALSE;

	for (i = y_current->y_size; --i >= 0; )
	{
	/* insert newline between lines and after last line if type is MLINE */
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
 * insert a yank buffer: copy it into the Read buffer
 */
	int
insertbuf(c)
	int c;
{
	long i;

	if (!isalnum(c))		/* registers 0-9 and a-z are allowed */
		return FALSE;

	yankbuffer = c;
	get_yank_buffer(FALSE);
	if (y_current->y_array == NULL)
		return FALSE;

	for (i = 0; i < y_current->y_size; ++i)
	{
		stuffReadbuff(y_current->y_array[i]);
	/* insert newline between lines and after last line if type is MLINE */
		if (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffReadbuff("\n");
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
	linenr_t		lnum;
	char			*ptr;

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
		y_previous = y_current = &y_buf[1];
		y_buf[1].y_array = NULL;		/* set buffer one to empty */
	}
	else if (yankbuffer == '.')			/* read-only buffer */
	{
		beep();
		return;
	}
	else								/* yank into specified buffer */
		get_yank_buffer(TRUE);

	if (!doyank(TRUE))
	{
		if (ask_yesno("cannot yank; delete anyway") != 'y')
		{
			emsg(e_abort);
			return;
		}
	}

	if (!u_save((linenr_t)(startop.lnum - 1), (linenr_t)(endop.lnum + 1)))
		return;

/*
 * block mode
 */
	if (Quote_block)
	{
		for (lnum = Curpos.lnum; Curpos.lnum <= endop.lnum; ++Curpos.lnum)
		{
			block_prep(Curpos.lnum, TRUE);
			if (textlen == 0)		/* nothing to delete */
				continue;

		/*
		 * If we delete a TAB, it may be replaced by several characters.
		 * Thus the number of characters may increase!
		 */
			n = textlen - startspaces - endspaces;
		/* number of characters increases - make room */
			if (n < 0 && !canincrease(-n))
				continue;
			ptr = nr2ptr(Curpos.lnum) + textcol;
		/* copy the part after the deleted part */
			memmove(ptr + startspaces + endspaces, ptr + textlen, strlen(ptr + textlen) + 1);
		/* insert spaces */
			copy_spaces(ptr, (size_t)(startspaces + endspaces));
			if (n > 0)
				canincrease(0);
		}
		Curpos.lnum = lnum;
		CHANGED;
		updateScreen(VALID_TO_CURSCHAR);
		nlines = 0;		/* no lines deleted */
	}
	else if (mtype == MLINE)
	{
		u_clearline();	/* "U" command should not be possible after "dd" */
		if (operator == CHANGE)
		{
			dellines((long)(nlines - 1), TRUE);
			Curpos.col = 0;
			while (delchar(TRUE));
		}
		else
		{
			dellines(nlines, TRUE);
		}
	}
	else if (nlines == 1)		/* del. within line */
	{
		n = endop.col - startop.col + 1 - oneless;
		while (n--)
			if (!delchar(TRUE))
				break;
	}
	else						/* del. between lines */
	{
		n = Curpos.col;
		while (Curpos.col >= n)
			if (!delchar(TRUE))
				break;

		startop = Curpos;		/* remember Curpos */
		++Curpos.lnum;
		dellines((long)(nlines - 2), TRUE);
		n = endop.col - oneless;

		while (n-- >= 0)
			if (!delchar(TRUE))
				break;
		Curpos = startop;		/* restore Curpos */
		dojoin(FALSE);
	}

	if ((mtype == MCHAR && nlines <= 2 /* && p_nu == FALSE */) || operator == CHANGE)
	{
		cursupdate();
		updateline();
	}
	else
		updateScreen(CURSUPD);

	msgmore(-nlines);
}

/*
 * dotilde - handle the (non-standard vi) tilde operator
 */
	void
dotilde()
{
		if (!u_save((linenr_t)(startop.lnum - 1), (linenr_t)(endop.lnum + 1)))
				return;

		if (Quote_block)		/* block mode */
		{
			for (; startop.lnum <= endop.lnum; ++startop.lnum)
			{
				block_prep(startop.lnum, FALSE);
				startop.col = textcol;
				for (; --textlen >= 0; inc(&startop))
					swapchar(&startop);
			}
		}
		else			/* not block mode */
		{
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
				swapchar(&startop);
		}

		if (mtype == MCHAR && nlines == 1 && !Quote_block)
		{
			cursupdate();
			updateline();
		}
		else
			updateScreen(CURSUPD);

		if (nlines > p_report)
				smsg("%ld lines ~ed", nlines);
}

/*
 * If operator == UPPER: make uppercase,
 * if operator == LOWER: make lowercase,
 * else swap case of character at 'pos'
 */
	void
swapchar(pos)
	FPOS	*pos;
{
	int		c;

	c = gchar(pos);
	if (operator == UPPER ? islower(c) :
			(operator == LOWER ? isupper(c) :
				isalpha(c)))
	{
		pchar(*pos, c ^ 0x20);	/* Change current character */
		CHANGED;
	}
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

	startinsert(NUL, FALSE, (linenr_t)1);
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
	int
doyank(deleting)
	int deleting;
{
	long 				i;				/* index in y_array[] */
	struct yankbuf		*curr;		/* copy of y_current */
	struct yankbuf		new; 		/* new yank buffer when appending */
	char				**new_ptr;
	register linenr_t	lnum;			/* current line number */
	long 				j;

	char				*pnew;

	if (yankbuffer == '.')			/* read-only buffer */
	{
		beep();
		return FALSE;
	}
	if (!deleting)					/* dodelete() already set y_current */
		get_yank_buffer(TRUE);

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

	i = 0;
	lnum = startop.lnum;

	if (Quote_block)
	{
/*
 * block mode
 */
		y_current->y_type = MBLOCK;	/* set the yank buffer type */
		for ( ; lnum <= endop.lnum; ++lnum)
		{
			block_prep(lnum, FALSE);
			if ((pnew= alloc(startspaces + endspaces + textlen + 1)) == NULL)
				goto fail;
			y_current->y_array[i++] = pnew;
			copy_spaces(pnew, (size_t)startspaces);
			pnew += startspaces;
			strncpy(pnew, textstart, (size_t)textlen);
			pnew += textlen;
			copy_spaces(pnew, (size_t)endspaces);
			pnew += endspaces;
			*pnew = NUL;
		}
	}
	else
	{
/*
 * there are three parts for non-block mode:
 * 1. if mtype != MLINE yank last part of the top line
 * 2. yank the lines between startop and endop, inclusive when mtype == MLINE
 * 3. if mtype != MLINE yank first part of the bot line
 */
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

		if (mtype == MLINE) 	/* MLINE overrides MCHAR and MBLOCK */
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
	if (operator == YANK && nlines > p_report)
	{
		cursupdate();		/* redisplay now, so message is not deleted */
		smsg("%ld lines yanked", (long)(y_current->y_size));
	}

	return TRUE;
}

	void
doput(dir, count)
	int dir;
	long count;
{
	char		*ptr, *ep;
	int 		newlen;
	int			totlen;
	linenr_t	lnum;
	int			col;
	long 		i;		/* index in y_array[] */
	int 		y_type;
	long 		y_size;
	char		**y_array;
	long 		nlines = 0;
	int			vcol;
	int			delchar;
	int			incr = 0;
	long		j;
	FPOS		newCurpos;

	if (yankbuffer == '.')		/* use inserted text */
	{
		stuff_inserted(dir == FORWARD ? (count == -1 ? 'o' : 'a') : (count == -1 ? 'O' : 'i'), count, FALSE);
		return;
	}
	get_yank_buffer(FALSE);

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

	if (y_type == MBLOCK)
	{
		lnum = Curpos.lnum + y_size + 1;
		if (lnum > line_count)
			lnum = line_count + 1;
		if (!u_save(Curpos.lnum - 1, lnum))
			return;
	}
	else if (!u_saveCurpos())
		return;

	newlen = strlen(y_array[0]);
	CHANGED;

	lnum = Curpos.lnum;
	col = Curpos.col;

/*
 * block mode
 */
	if (y_type == MBLOCK)
	{
		if (dir == FORWARD && gcharCurpos() != NUL)
		{
			col = getvcol(&Curpos, 3) + 1;
			++Curpos.col;
		}
		else
			col = getvcol(&Curpos, 2);
		for (i = 0; i < y_size; ++i)
		{
			startspaces = 0;
			endspaces = 0;
			textcol = 0;
			vcol = 0;
			delchar = 0;

		/* add a new line */
			if (Curpos.lnum > line_count)
			{
				ep = alloc_line(0);
				if (ep == NULL)
						goto error;
				appendline(line_count, ep);
				++nlines;
			}
			ptr = nr2ptr(Curpos.lnum);
			while (vcol < col && *ptr)
			{
				/* Count a tab for what it's worth (if list mode not on) */
				incr = chartabsize(*ptr, vcol);
				vcol += incr;
				++ptr;
				++textcol;
			}
			if (vcol < col)	/* line too short, padd with spaces */
			{
				startspaces = col - vcol;
			}
			else if (vcol > col)
			{
				endspaces = vcol - col;
				startspaces = incr - endspaces;
				--textcol;
				delchar = 1;
			}
			newlen = strlen(y_array[i]);
			totlen = count * newlen + startspaces + endspaces;
			if (!canincrease(totlen))
				break;
			ptr = nr2ptr(Curpos.lnum) + textcol;

		/* move the text after the cursor to the end of the line. */
			memmove(ptr + totlen - delchar, ptr, strlen(ptr) + 1);
		/* may insert some spaces before the new text */
			copy_spaces(ptr, (size_t)startspaces);
			ptr += startspaces;
		/* insert the new text */
			for (j = 0; j < count; ++j)
			{
					strncpy(ptr, y_array[i], (size_t)newlen);
					ptr += newlen;
			}
		/* may insert some spaces after the new text */
			copy_spaces(ptr, (size_t)endspaces);

			++Curpos.lnum;
			if (i == 0)
				Curpos.col += startspaces;
		}
		Curpos.lnum = lnum;
		cursupdate();
		updateScreen(VALID_TO_CURSCHAR);
	}
	else		/* not block mode */
	{
		if (y_type == MCHAR)
		{
	/* if type is MCHAR, FORWARD is the same as BACKWARD on the next character */
			if (dir == FORWARD && gcharCurpos() != NUL)
			{
				++col;
				if (newlen)
					++Curpos.col;
			}
			newCurpos = Curpos;
		}
		else if (dir == BACKWARD)
	/* if type is MLINE, BACKWARD is the same as FORWARD on the previous line */
			--lnum;

/*
 * simple case: insert into current line
 */
		if (y_type == MCHAR && y_size == 1)
		{
			i = count * newlen;
			if (!canincrease((int)i))
					return; 				/* alloc() will give error message */
			ep = nr2ptr(lnum) + col;
			memmove(ep + i, ep, strlen(ep) + 1);
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
			if (y_type == MCHAR)
				--y_size;
			while (--count >= 0)
			{
				i = 0;
				if (y_type == MCHAR)
				{
					/*
					 * Split the current line in two at the insert position.
					 * Append y_array[0] to first line.
					 * Insert y_array[size - 1] in front of second line.
					 */
					ptr = nr2ptr(lnum) + col;
					col = strlen(y_array[y_size]);
					ep = alloc_line((unsigned)(strlen(ptr) + col));
					if (ep == NULL)
						goto error;
					strcpy(ep, y_array[y_size]);
					strcat(ep, ptr);
					appendline(lnum, ep);			/* insert in second line */
					++nlines;
					*ptr = NUL;
					Curpos.lnum = lnum;
					if (!canincrease(newlen))		/* lnum == Curpos.lnum! */
						goto error;
					strcat(nr2ptr(lnum), y_array[0]);/* append to first line */
					i = 1;
				}

				while (i < y_size)
				{
					ep = save_line(y_array[i++]);
					if (ep == NULL)
						goto error;
					appendline(lnum++, ep);
					++nlines;
				}
				if (y_type == MCHAR)
					++lnum; 	/* lnum is now number of line below inserted lines */
			}

			if (y_type == MLINE)
			{
				Curpos.col = 0;
				if (dir == FORWARD)
				{
					updateScreen(NOT_VALID);		/* recompute Botline */
					++Curpos.lnum;
				}
					/* put cursor on first non-blank in last inserted line */
				beginline(TRUE);
			}
			else		/* put cursor on first inserted character */
				Curpos = newCurpos;

error:
			updateScreen(CURSUPD);
		}
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

	settmode(0);			/* set cooked mode so output can be halted */
	for (i = -1; i < 36; ++i)
	{
		if (i == -1)
		{
			if (y_previous != NULL)
				yb = y_previous;
			else
				yb = &(y_buf[0]);
		}
		else
			yb = &(y_buf[i]);
		if (yb->y_array != NULL)
		{
			if (i == -1)
				outstrn("pP");
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
					outstrn("^J");
					n -= 2;
				}
				for (p = yb->y_array[j]; *p && n > 0; ++p)
				{
					outstrn(transchar(*p));
					n -= charsize(*p);
				}
			}
			outchar('\n');
			flushbuf();
		}
	}
	settmode(1);
	wait_return(TRUE);
}

/*
 * join 'count' lines (minimal 2), including u_save()
 */
	void
dodojoin(count, flag, redraw)
	long	count;
	int		flag;
	int		redraw;
{
	if (!u_save((linenr_t)(Curpos.lnum - 1), (linenr_t)(Curpos.lnum + count)))
		return;

	while (--count > 0)
		if (!dojoin(flag))
		{
				beep();
				break;
		}

	if (redraw)
		updateScreen(VALID_TO_CURSCHAR);
}

	int
dojoin(insert_space)
	int			insert_space;
{
	char		*curr;
	char		*next;
	char		*endcurr;
	int 		currsize;		/* size of the current line */
	int 		nextsize;		/* size of the next line */
	int			spaces;			/* number of spaces to insert */
	int			rows_to_del;	/* number of rows on screen to delete */
	linenr_t	t;

	if (Curpos.lnum == line_count)		/* on last line */
		return FALSE;

	rows_to_del = plines_m(Curpos.lnum, Curpos.lnum + 1);
	curr = nr2ptr(Curpos.lnum);
	currsize = strlen(curr);
	next = nr2ptr((linenr_t)(Curpos.lnum + 1));
	spaces = 0;
	if (insert_space)
	{
		skipspace(&next);
		spaces = 1;
		if (*next == ')' || currsize == 0)
			spaces = 0;
		else
		{
			endcurr = curr + currsize - 1;
			if (*endcurr == ' ' || *endcurr == TAB)
			{
				spaces = 0;
				if (currsize > 1)
					--endcurr;
			}
			if (p_js && strchr(".!?", *endcurr) != NULL)
				++spaces;
		}
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
	 * briefly, and then move it back.
	 */
	t = Curpos.lnum;
	++Curpos.lnum;
	dellines(1L, FALSE);
	Curpos.lnum = t;

	/*
	 * the number of rows on the screen is reduced by the difference
	 * in number of rows of the two old lines and the one new line
	 */
	rows_to_del -= plines(Curpos.lnum);
	if (rows_to_del > 0)
		s_del(Cursrow, rows_to_del, TRUE);

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

/*
 * implementation of the format operator 'V'
 */
	void
doformat()
{
		/* prepare undo and join the lines */
	dodojoin((long)endop.lnum - (long)startop.lnum + 1, TRUE, FALSE);

		/* put cursor on last non-space */
	coladvance(29999);
	while (Curpos.col && isspace(gcharCurpos()))
		decCurpos();
	curs_columns();			/* update Cursvcol */

		/* do the formatting */
	State = INSERT;		/* for Opencmd() */
	insertchar(NUL);
	State = NORMAL;
	updateScreen(NOT_VALID);
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

	if (p_mo)
		showmode();

	edit(count);
}

/*
 * prepare a few things for block mode yank/delete/tilde
 *
 * for delete:
 * - textlen includes the first/last char to be (partly) deleted
 * - start/endspaces is the number of columns that are taken by the
 *	 first/last deleted char minus the number of columns that have to be deleted.
 * for yank and tilde:
 * - textlen includes the first/last char to be wholly yanked
 * - start/endspaces is the number of columns of the first/last yanked char
 *   that are to be yanked.
 */
	static void
block_prep(lnum, delete)
	linenr_t	lnum;
	int			delete;
{
	int			vcol;
	int			incr = 0;
	char		*pend;

	startspaces = 0;
	endspaces = 0;
	textlen = 0;
	textcol = 0;
	vcol = 0;
	textstart = nr2ptr(lnum);
	while (vcol < startvcol && *textstart)
	{
		/* Count a tab for what it's worth (if list mode not on) */
		incr = chartabsize(*textstart, vcol);
		vcol += incr;
		++textstart;
		++textcol;
	}
	if (vcol < startvcol)	/* line too short */
	{
		if (!delete)
			endspaces = endvcol - startvcol + 1;
	}
	else /* vcol >= startvcol */
	{
		startspaces = vcol - startvcol;
		if (delete && vcol > startvcol)
			startspaces = incr - startspaces;
		pend = textstart;
		while (vcol <= endvcol && *pend)
		{
			/* Count a tab for what it's worth (if list mode not on) */
			incr = chartabsize(*pend, vcol);
			vcol += incr;
			++pend;
		}
		if (vcol < endvcol && !delete)	/* line too short */
		{
			endspaces = endvcol - vcol;
		}
		else if (vcol > endvcol)
		{
			if (delete)
				endspaces = vcol - endvcol - 1;
			else if (pend != textstart)
			{
				endspaces = incr - (vcol - endvcol);
				if (endspaces)
					--pend;
			}
		}
		if (delete && startspaces)
		{
			--textstart;
			--textcol;
		}
		textlen = pend - textstart;
	}
}
