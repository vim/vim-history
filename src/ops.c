/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
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
	register long i;

	if (!u_save((linenr_t)(Curpos.lnum - 1), (linenr_t)(Curpos.lnum + nlines)))
		return;

	Curpos.lnum += nlines;		/* start with last line, leave cursor on first */
	for (i = nlines; --i >= 0; )
		if (lineempty(--Curpos.lnum))
			Curpos.col = 0;
		else
		{
			/* if (Visual_block)
					shift the block, not the whole line
			else */
				shift_line(op == LSHIFT, p_sr);
		}

	updateScreen(CURSUPD);

	if (nlines > p_report)
		smsg("%ld line%s %ced", nlines, plural(nlines),
									(op == RSHIFT) ? '>' : '<');
}

/*
 * shift the current line one shiftwidth left (if left != 0) or right
 * leaves cursor on first blank in the line
 */
	void
shift_line(left, round)
	int left;
	int	round;
{
	register int count;
	register int i, j;

	count = get_indent();			/* get current indent */

	if (round)						/* round off indent */
	{
		i = count / (int)p_sw;		/* compute new indent */
		j = count % (int)p_sw;
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
		count = i * (int)p_sw;
	}
	else				/* original vi indent */
	{
		if (left)
		{
			count -= (int)p_sw;
			if (count < 0)
				count = 0;
		}
		else
			count += (int)p_sw;
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
	if (((yankbuffer == 0 && !writing) || yankbuffer == '"') && y_previous != NULL)
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
	char *p;
	static int bufname;

	if (Recording == FALSE) 		/* start recording */
	{
		if (!isalnum(c) && c != '"')	/* registers 0-9, a-z and " are allowed */
			return FALSE;
		Recording = TRUE;
		showmode();
		bufname = c;
		return TRUE;
	}
	else							/* stop recording */
	{
		Recording = FALSE;
		msg("");
			/* the trailing 'q' command will not have been put in the buffer */
		p = (char *)get_recorded();
		if (p == NULL)
			return FALSE;
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
	if (yankbuffer == '.' || yankbuffer == '%')		/* read-only buffer */
		return FALSE;
	get_yank_buffer(TRUE);
	if (yankappend && y_current->y_array != NULL)
	{
		pp = &(y_current->y_array[y_current->y_size - 1]);
		lp = lalloc((u_long)(strlen(*pp) + strlen(p) + 1), TRUE);
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
 * execute a yank buffer (register): copy it into the stuff buffer
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
	if (!isalnum(c) && c != '"')		/* registers 0-9, a-z and " are allowed */
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
			if (ins_typestr("\n", FALSE) < 0)
				return FALSE;
		}
		if (ins_typestr(y_current->y_array[i], FALSE) < 0)
			return FALSE;
	}
	Exec_reg = TRUE;		/* disable the 'q' command */
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

	if (c == '%')						/* insert file name */
	{
		if (check_fname())
			return FALSE;
		stuffReadbuff(xFilename);
		return TRUE;
	}

	if (!isalnum(c) && c != '"')		/* registers 0-9, a-z and " are allowed */
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
	linenr_t		old_lcount = line_count;

	/*
	 * Imitate the strange Vi behaviour: If the delete spans more than one line
	 * and mtype == MCHAR and the result is a blank line, make the delete
	 * linewise. Don't do this for the change command.
	 */
	if (mtype == MCHAR && nlines > 1 && operator == DELETE)
	{
		ptr = nr2ptr(endop.lnum) + endop.col + mincl;
		skipspace(&ptr);
		if (*ptr == NUL && startinmargin())
			mtype = MLINE;
	}

		/*
		 * Shift number buffers if there is no yankbuffer defined and we do a
		 * delete that contains a line break.
		 */
	if (yankbuffer == 0 && (mtype == MLINE || nlines > 1))
	{
		y_current = &y_buf[9];
		free_yank_all();				/* free buffer nine */
		for (n = 9; n > 1; --n)
			y_buf[n] = y_buf[n - 1];
		y_previous = y_current = &y_buf[1];
		y_buf[1].y_array = NULL;		/* set buffer one to empty */
	}
	else if (yankbuffer == '.' || yankbuffer == '%')	/* read-only buffer */
	{
		beep();
		return;
	}
	else								/* yank into specified buffer */
		get_yank_buffer(TRUE);

	/*
	 * Do a yank of whatever we're about to delete. If there's too much stuff
	 * to fit in the yank buffer, then get a confirmation before doing the
	 * delete. This is crude, but simple. And it avoids doing a delete of
	 * something we can't put back if we want.
	 */
	if (!doyank(TRUE))
	{
		if (ask_yesno("cannot yank; delete anyway") != 'y')
		{
			emsg(e_abort);
			return;
		}
	}

/*
 * block mode
 */
	if (Visual_block)
	{
		if (!u_save((linenr_t)(startop.lnum - 1), (linenr_t)(endop.lnum + 1)))
			return;

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
		if (operator == CHANGE)
		{
			dellines((long)(nlines - 1), TRUE, TRUE);
			if (!u_saveCurpos())
				return;
			while (delchar(TRUE));		/* slow but simple */
		}
		else
		{
			dellines(nlines, TRUE, TRUE);
		}
		u_clearline();	/* "U" command should not be possible after "dd" */
	}
	else if (nlines == 1)		/* delete characters within one line */
	{
		if (!u_saveCurpos())
			return;
		n = endop.col - startop.col + 1 - !mincl;
		while (n-- > 0)
			if (!delchar(TRUE))
				break;
	}
	else						/* delete characters between lines */
	{
		if (!u_saveCurpos())	/* save first line for undo */
			return;
		n = Curpos.col;
		while (Curpos.col >= n)	/* delete from cursor to end of line */
			if (!delchar(TRUE))
				break;

		startop = Curpos;		/* remember Curpos */
		++Curpos.lnum;
		dellines((long)(nlines - 2), TRUE, TRUE);	/* includes save for undo */

		if (!u_saveCurpos())	/* save last line for undo */
			return;
		n = endop.col - !mincl;
		Curpos.col = 0;
		while (n-- >= 0)		/* delete from start of line until endop */
			if (!delchar(TRUE))
				break;
		Curpos = startop;		/* restore Curpos */
		dojoin(FALSE, TRUE);
	}

	if ((mtype == MCHAR && nlines == 1) || operator == CHANGE)
	{
		cursupdate();
		updateline();
	}
	else
		updateScreen(CURSUPD);

	msgmore(line_count - old_lcount);

		/* correct endop for deleted text (for "']" command) */
	if (Visual_block)
		endop.col = startop.col;
	else
		endop = startop;
}

/*
 * dotilde - handle the (non-standard vi) tilde operator
 */
	void
dotilde()
{
	FPOS pos;

	if (!u_save((linenr_t)(startop.lnum - 1), (linenr_t)(endop.lnum + 1)))
		return;

	pos = startop;
	if (Visual_block)		/* block mode */
	{
		for (; pos.lnum <= endop.lnum; ++pos.lnum)
		{
			block_prep(pos.lnum, FALSE);
			pos.col = textcol;
			while (--textlen >= 0)
			{
				swapchar(&pos);
				if (inc(&pos) == -1)	/* at end of file */
					break;
			}
		}
	}
	else			/* not block mode */
	{
		if (mtype == MLINE)
		{
				pos.col = 0;
				endop.col = strlen(nr2ptr(endop.lnum));
				if (endop.col)
						--endop.col;
		}
		else if (!mincl)
			dec(&endop);

		while (ltoreq(pos, endop))
		{
			swapchar(&pos);
			if (inc(&pos) == -1)	/* at end of file */
				break;
		}
	}

	if (mtype == MCHAR && nlines == 1 && !Visual_block)
	{
		cursupdate();
		updateline();
	}
	else
		updateScreen(CURSUPD);

	if (nlines > p_report)
			smsg("%ld line%s ~ed", nlines, plural(nlines));
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
	if (islower(c) && operator != LOWER)
	{
		pchar(*pos, toupper(c));
		CHANGED;
	}
	else if (isupper(c) && operator != UPPER)
	{
		pchar(*pos, tolower(c));
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

	if (!no_op)
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
	struct yankbuf		*curr;			/* copy of y_current */
	struct yankbuf		new; 			/* new yank buffer when appending */
	char				**new_ptr;
	register linenr_t	lnum;			/* current line number */
	long 				j;
	int					yanktype = mtype;
	long				yanklines = nlines;
	linenr_t			yankendlnum = endop.lnum;

	char				*pnew;

	if (yankbuffer == '.' || yankbuffer == '%')			/* read-only buffer */
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

/*
 * If the cursor was in column 1 before and after the movement, the
 * yank is always linewise.
 */
	if (mtype == MCHAR && startop.col == 0 && endop.col == 0 && nlines > 1)
	{
		yanktype = MLINE;
		if (mincl == FALSE && yankendlnum > startop.lnum)
		{
			--yankendlnum;
			--yanklines;
		}
	}

	y_current->y_size = yanklines;
	y_current->y_type = yanktype;	/* set the yank buffer type */
	y_current->y_array = (char **)lalloc((u_long)(sizeof(char *) * yanklines), TRUE);

	if (y_current->y_array == NULL)
	{
		y_current = curr;
		return FALSE;
	}

	i = 0;
	lnum = startop.lnum;

	if (Visual_block)
	{
/*
 * block mode
 */
		y_current->y_type = MBLOCK;	/* set the yank buffer type */
		for ( ; lnum <= yankendlnum; ++lnum)
		{
			block_prep(lnum, FALSE);
			if ((pnew = alloc(startspaces + endspaces + textlen + 1)) == NULL)
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
 * 1. if yanktype != MLINE yank last part of the top line
 * 2. yank the lines between startop and endop, inclusive when yanktype == MLINE
 * 3. if yanktype != MLINE yank first part of the bot line
 */
		if (yanktype != MLINE)
		{
			if (yanklines == 1)		/* startop and endop on same line */
			{
					j = endop.col - startop.col + 1 - !mincl;
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

		while (yanktype == MLINE ? (lnum <= yankendlnum) : (lnum < yankendlnum))
		{
			if ((y_current->y_array[i] = strsave(nr2ptr(lnum++))) == NULL)
					goto fail;
			++i;
		}
		if (yanktype != MLINE)
		{
			if ((y_current->y_array[i] = strnsave(nr2ptr(yankendlnum), endop.col + 1 - !mincl)) == NULL)
					goto fail;
		}
	}

success:
	if (curr != y_current)		/* append the new block to the old block */
	{
		new_ptr = (char **)lalloc((u_long)(sizeof(char *) * (curr->y_size + y_current->y_size)), TRUE);
		if (new_ptr == NULL)
				goto fail;
		for (j = 0; j < curr->y_size; ++j)
				new_ptr[j] = curr->y_array[j];
		free(curr->y_array);
		curr->y_array = new_ptr;

		if (yanktype == MLINE) 	/* MLINE overrides MCHAR and MBLOCK */
				curr->y_type = MLINE;
		if (curr->y_type == MCHAR)		/* concatenate the last line of the old
										block with the first line of the new block */
		{
				new_ptr = (char **)lalloc((u_long)(strlen(curr->y_array[curr->y_size - 1]) + strlen(y_current->y_array[0]) + 1), TRUE);
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
	if (operator == YANK)		/* don't do this when deleting */
	{
		if (yanktype == MCHAR && !Visual_block)
			--yanklines;
		if (yanklines > p_report)
		{
			cursupdate();		/* redisplay now, so message is not deleted */
			smsg("%ld line%s yanked", yanklines, plural(yanklines));
		}
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
	int			totlen = 0;		/* init for gcc */
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
	int			commandchar;
	char		temp[2];

	startop = Curpos;			/* default for "'[" command */
	if (dir == FORWARD)
		startop.col++;
	endop = Curpos;				/* default for "']" command */
	commandchar = (dir == FORWARD ? (count == -1 ? 'o' : 'a') : (count == -1 ? 'O' : 'i'));
	if (yankbuffer == '.')		/* use inserted text */
	{
		stuff_inserted(commandchar, count, FALSE);
		return;
	}
	else if (yankbuffer == '%')	/* use file name */
	{
		if (!check_fname())
		{
			stuffcharReadbuff(commandchar);
			stuffReadbuff(xFilename);
			stuffcharReadbuff(ESC);
		}
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
		temp[0] = yankbuffer;
		temp[1] = NUL;
		emsg2("Nothing in register %s", temp);
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
		endop.lnum = Curpos.lnum - 1;		/* for "']" command */
		endop.col = textcol + totlen - 1;
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
				{
					++Curpos.col;
					++endop.col;
				}
			}
			newCurpos = Curpos;
		}
		else if (dir == BACKWARD)
	/* if type is MLINE, BACKWARD is the same as FORWARD on the previous line */
			--lnum;
		else	/* type == MLINE, dir == FORWARD */
		{
			startop.col = 0;
			startop.lnum++;
		}

/*
 * simple case: insert into current line
 */
		if (y_type == MCHAR && y_size == 1)
		{
			i = count * newlen;
			if (i)
			{
				if (!canincrease((int)i))
					return; 				/* alloc() will give error message */
				ep = nr2ptr(lnum) + col;
				memmove(ep + i, ep, strlen(ep) + 1);
					Curpos.col += (colnr_t)(i - 1);	/* put cursor on last putted char */
				for (i = 0; i < count; ++i)
				{
					strncpy(ep, y_array[0], (size_t)newlen);
					ep += newlen;
				}
			}
			endop = Curpos;
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

			endop.lnum = lnum;		/* for "']" command */
			if (y_type == MLINE)
			{
				Curpos.col = 0;
				endop.col = 0;
				if (dir == FORWARD)
				{
					updateScreen(NOT_VALID);		/* recompute Botline */
					++Curpos.lnum;
				}
					/* put cursor on first non-blank in last inserted line */
				beginline(TRUE);
			}
			else		/* put cursor on first inserted character */
			{
				if (col > 1)
					endop.col = col - 1;
				else
					endop.col = 0;
				Curpos = newCurpos;
			}

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

#ifdef AMIGA
	settmode(0);			/* set cooked mode so output can be halted */
#endif
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
				outstrn("\"\"");
			else
			{
				outchar('"');
				if (i < 10)
					outchar(i + '0');
				else
					outchar(i + 'a' - 10);
			}
			outchar(' ');

			n = (int)Columns - 4;
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
#ifdef AMIGA
	settmode(1);
#endif
	wait_return(TRUE);
}

/*
 * join 'count' lines (minimal 2), including u_save()
 */
	void
dodojoin(count, insert_space, redraw)
	long	count;
	int		insert_space;
	int		redraw;
{
	if (!u_save((linenr_t)(Curpos.lnum - 1), (linenr_t)(Curpos.lnum + count)))
		return;

	while (--count > 0)
		if (!dojoin(insert_space, redraw))
		{
				beep();
				break;
		}

	if (redraw)
		updateScreen(VALID_TO_CURSCHAR);
}

	int
dojoin(insert_space, redraw)
	int			insert_space;
	int			redraw;
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
				spaces = 2;
		}
	}
	nextsize = strlen(next);
	if (!canincrease(nextsize + spaces))
		return FALSE;

	/*
	 * Append the spaces and the next line. Curr has to be obtained again,
	 * because canincrease() will have changed the pointer.
	 */
	curr = nr2ptr(Curpos.lnum) + currsize;
	while (spaces--)
		*curr++ = ' ';
	strcpy(curr, next);

	/*
	 * Delete the following line. To do this we move the cursor there
	 * briefly, and then move it back. After dellines() the cursor may
	 * have moved up (last line deleted), so the current lnum is kept in t.
	 */
	t = Curpos.lnum;
	++Curpos.lnum;
	dellines(1L, FALSE, FALSE);
	Curpos.lnum = t;

	/*
	 * the number of rows on the screen is reduced by the difference
	 * in number of rows of the two old lines and the one new line
	 */
	if (redraw)
	{
		rows_to_del -= plines(Curpos.lnum);
		if (rows_to_del > 0)
			s_del(Cursrow, rows_to_del, TRUE);
	}

 	/*
	 * go to first character of the joined line
	 */
	if (currsize == 0)
		Curpos.col = 0;
	else
	{
		Curpos.col = currsize - 1;
		oneright();
	}
	CHANGED;

	return TRUE;
}

/*
 * implementation of the format operator 'Q'
 */
	void
doformat()
{
		/* prepare undo and join the lines */
	dodojoin((long)nlines, TRUE, FALSE);

		/* put cursor on last non-space */
	coladvance(MAXCOL);
	while (Curpos.col && isspace(gcharCurpos()))
		decCurpos();
	curs_columns(FALSE);			/* update Cursvcol */

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
			ResetRedobuff();
			AppendNumberToRedobuff(count);
			AppendCharToRedobuff(initstr);
	}

	if (initstr == 'R')
		State = REPLACE;
	else
		State = INSERT;

	if (p_smd)
		showmode();

	change_warning();		/* give a warning if readonly */
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
		textlen = (int)(pend - textstart);
	}
}

	int
doaddsub(c, Prenum1)
	int			c;
	linenr_t	Prenum1;
{
	register int 	col;
	char			buf[30];
	int				hex;		/* 'x' or 'X': hexadecimal; '0': octal */
	static int		hexupper = FALSE;	/* 0xABC */
	long			n;
	char			*ptr;

	ptr = nr2ptr(Curpos.lnum);
	col = Curpos.col;

		/* first check if we are on a hexadecimal number */
	while (col > 0 && isxdigit(ptr[col]))
		--col;
	if (col > 0 && (ptr[col] == 'X' || ptr[col] == 'x') &&
						ptr[col - 1] == '0' && isxdigit(ptr[col + 1]))
		--col;		/* found hexadecimal number */
	else
	{
		/* first search forward and then backward for start of number */
		col = Curpos.col;

		while (ptr[col] != NUL && !isdigit(ptr[col]))
			++col;

		while (col > 0 && isdigit(ptr[col - 1]))
			--col;
	}

	if (isdigit(ptr[col]) && u_saveCurpos())
	{
		set_want_col = TRUE;

		if (ptr[col] != '0')
			hex = 0;				/* decimal */
		else
		{
			hex = TO_UPPER(ptr[col + 1]);		/* assume hexadecimal */
			if (hex != 'X' || !isxdigit(ptr[col + 2]))
			{
				if (isdigit(hex))
					hex = '0';		/* octal */
				else
					hex = 0;		/* 0 by itself is decimal */
			}
		}

		if (!hex && col > 0 && ptr[col - 1] == '-')
			--col;

		ptr += col;
		if (hex == '0')
			sscanf(ptr, "%lo", &n);
		else if (hex)
			sscanf(ptr, "%lx", &n);	/* "%X" doesn't work! */
		else
			n = atol(ptr);

		if (c == Ctrl('A'))
			n += Prenum1;
		else
			n -= Prenum1;

		if (hex == 'X')					/* skip the '0x' */
			col += 2;
		Curpos.col = col;
		do								/* delete the old number */
		{
			if (isalpha(c))
			{
				if (isupper(c))
					hexupper = TRUE;
				else
					hexupper = FALSE;
			}
			delchar(FALSE);
			c = gcharCurpos();
		}
		while (hex ? (hex == '0' ? c >= '0' && c <= '7' : isxdigit(c)) : isdigit(c));

		if (hex == '0')
			sprintf(buf, "0%lo", n);
		else if (hexupper)
			sprintf(buf, "%lX", n);
		else if (hex)
			sprintf(buf, "%lx", n);
		else
			sprintf(buf, "%ld", n);
		insstr(buf);					/* insert the new number */
		--Curpos.col;
		updateline();
		return TRUE;
	}
	else
	{
		beep();
		return FALSE;
	}
}

/*
 * Return TRUE if startop is on or before the first non-blank character in the line
 */
	int
startinmargin()
{
	int		n;
	char	*ptr;

	n = 0;
	for (ptr = nr2ptr(startop.lnum); *ptr && isspace(*ptr); ++ptr)
		++n;
	return (n >= startop.col);
}
