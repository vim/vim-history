/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
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
	char_u		**y_array;		/* pointer to array of line pointers */
	linenr_t 	y_size; 		/* number of lines in y_array */
	char_u		y_type; 		/* MLINE, MCHAR or MBLOCK */
} y_buf[36];					/* 0..9 = number buffers, 10..35 = char buffers */

static struct	yankbuf *y_current;		/* ptr to current yank buffer */
static int		yankappend;				/* TRUE when appending */
static struct	yankbuf *y_previous = NULL; /* ptr to last written yank buffer */

static void		get_yank_buffer __ARGS((int));
static int		stuff_yank __ARGS((int, char_u *));
static void		free_yank __ARGS((long));
static void		free_yank_all __ARGS((void));
static void		block_prep __ARGS((linenr_t, int));

/* variables use by block_prep, dodelete and doyank */
static int		startspaces;
static int		endspaces;
static int		textlen;
static char_u		*textstart;
static colnr_t	textcol;

/*
 * doshift - handle a shift operation
 */
	void
doshift(op, curs_top, amount)
	int 			op;
	int				curs_top;
	int				amount;
{
	register long	i;
	int				first_char;

	if (!u_save((linenr_t)(curwin->w_cursor.lnum - 1), (linenr_t)(curwin->w_cursor.lnum + nlines)))
		return;

	for (i = nlines; --i >= 0; )
	{
		first_char = *ml_get(curwin->w_cursor.lnum);
		if (first_char == NUL)							/* empty line */
			curwin->w_cursor.col = 0;
		/*
		 * Don't move the line right if it starts with # and p_si is set.
		 */
		else if (!curbuf->b_p_si || first_char != '#')
		{
			/* if (Visual_block)
					shift the block, not the whole line
			else */
				shift_line(op == LSHIFT, p_sr, amount);
		}
		++curwin->w_cursor.lnum;
	}

	if (curs_top)			/* put cursor on first line, for ">>" */
		curwin->w_cursor.lnum -= nlines;
	else
		--curwin->w_cursor.lnum;		/* put cursor on last line, for ":>" */
	updateScreen(CURSUPD);

	if (nlines > p_report)
		smsg((char_u *)"%ld line%s %ced", nlines, plural(nlines),
									(op == RSHIFT) ? '>' : '<');
}

/*
 * shift the current line one shiftwidth left (if left != 0) or right
 * leaves cursor on first blank in the line
 */
	void
shift_line(left, round, amount)
	int left;
	int	round;
	int	amount;
{
	register int count;
	register int i, j;
	int p_sw = (int)curbuf->b_p_sw;

	count = get_indent();			/* get current indent */

	if (round)						/* round off indent */
	{
		i = count / p_sw;			/* number of p_sw rounded down */
		j = count % p_sw;			/* extra spaces */
		if (j && left)				/* first remove extra spaces */
			--amount;
		if (left)
		{
			i -= amount;
			if (i < 0)
				i = 0;
		}
		else
			i += amount;
		count = i * p_sw;
	}
	else				/* original vi indent */
	{
		if (left)
		{
			count -= p_sw * amount;
			if (count < 0)
				count = 0;
		}
		else
			count += p_sw * amount;
	}
	set_indent(count, TRUE);		/* set new indent */
}

/*
 * check if character is name of yank buffer
 * Note: There is no check for 0 (default register), caller should do this
 */
 	int
is_yank_buffer(c, write)
	int		c;
	int		write;		/* if TRUE check for writable buffers */
{
	if (isalnum(c) || (!write && strchr(".%:", c) != NULL) || c == '"')
		return TRUE;
	return FALSE;
}

/*
 * Set y_current and yankappend, according to the value of yankbuffer.
 *
 * If yankbuffer is 0 and writing, use buffer 0
 * If yankbuffer is 0 and reading, use previous buffer
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
 * start or stop recording into a yank buffer
 *
 * return FAIL for failure, OK otherwise
 */
	int
dorecord(c)
	int c;
{
	char_u		*p;
	static int	bufname;
	int			retval;

	if (Recording == FALSE) 		/* start recording */
	{
		if (!isalnum(c) && c != '"')	/* registers 0-9, a-z and " are allowed */
			retval = FAIL;
		else
		{
			Recording = TRUE;
			showmode();
			bufname = c;
			retval = OK;
		}
	}
	else							/* stop recording */
	{
		Recording = FALSE;
		MSG("");
			/* the trailing 'q' command will not have been put in the buffer */
		p = get_recorded();
		if (p == NULL)
			retval = FAIL;
		else
			retval = (stuff_yank(bufname, p));
	}
	return retval;
}

/*
 * stuff string 'p' into yank buffer 'bufname' (append if uppercase)
 * 'p' is assumed to be alloced.
 *
 * return FAIL for failure, OK otherwise
 */
	static int
stuff_yank(bufname, p)
	int bufname;
	char_u *p;
{
	char_u *lp;
	char_u **pp;

	yankbuffer = bufname;
											/* check for read-only buffer */
	if (yankbuffer != 0 && !is_yank_buffer(yankbuffer, TRUE))
		return FAIL;
	get_yank_buffer(TRUE);
	if (yankappend && y_current->y_array != NULL)
	{
		pp = &(y_current->y_array[y_current->y_size - 1]);
		lp = lalloc((long_u)(STRLEN(*pp) + STRLEN(p) + 1), TRUE);
		if (lp == NULL)
		{
			free(p);
			return FAIL;
		}
		STRCPY(lp, *pp);
		STRCAT(lp, p);
		free(p);
		free(*pp);
		*pp = lp;
	}
	else
	{
		free_yank_all();
		if ((y_current->y_array = (char_u **)alloc((unsigned)sizeof(char_u *))) == NULL)
		{
			free(p);
			return FAIL;
		}
		y_current->y_array[0] = p;
		y_current->y_size = 1;
		y_current->y_type = MCHAR;	/* used to be MLINE, why? */
	}
	return OK;
}

/*
 * execute a yank buffer (register): copy it into the stuff buffer
 *
 * return FAIL for failure, OK otherwise
 */
	int
doexecbuf(c)
	int c;
{
	static int lastc = NUL;
	long i;

	if (c == '@')					/* repeat previous one */
		c = lastc;
	if (!is_yank_buffer(c, FALSE))	/* check for valid buffer */
		return FAIL;
	lastc = c;

	if (c == ':')					/* use last command line */
	{
		if (last_cmdline == NULL)
		{
			EMSG(e_nolastcmd);
			return FAIL;
		}
		free(new_last_cmdline);		/* don't keep the command line containing @: */
		new_last_cmdline = NULL;
		if (ins_typestr((char_u *)"\n", FALSE) == FAIL)
			return FAIL;
		if (ins_typestr(last_cmdline, FALSE) == FAIL)
			return FAIL;
	}
	else
	{
		yankbuffer = c;
		get_yank_buffer(FALSE);
		if (y_current->y_array == NULL)
			return FAIL;

		for (i = y_current->y_size; --i >= 0; )
		{
		/* insert newline between lines and after last line if type is MLINE */
			if (y_current->y_type == MLINE || i < y_current->y_size - 1)
			{
				if (ins_typestr((char_u *)"\n", FALSE) == FAIL)
					return FAIL;
			}
			if (ins_typestr(y_current->y_array[i], FALSE) == FAIL)
				return FAIL;
		}
		Exec_reg = TRUE;		/* disable the 'q' command */
	}
	return OK;
}

/*
 * insert a yank buffer: copy it into the Read buffer
 * used by CTRL-R command in insert mode
 *
 * return FAIL for failure, OK otherwise
 */
	int
insertbuf(c)
	int c;
{
	long i;

	/*
	 * It is possible to get into an endless loop by having CTRL-R a in
	 * register a and then, in insert mode, doing CTRL-R a.
	 * If you hit CTRL-C, the loop will be broken here.
	 */
	breakcheck();
	if (got_int)
		return FAIL;

	if (!is_yank_buffer(c, FALSE))		/* check for valid buffer */
		return FAIL;

	if (c == '.')						/* insert last inserted text */
	{
		stuff_inserted(NUL, 1L, TRUE);
		return OK;
	}

	if (c == '%')						/* insert file name */
	{
		if (check_fname() == FAIL)
			return FAIL;
		stuffReadbuff(curbuf->b_xfilename);
		return OK;
	}

	if (c == ':')						/* insert last command line */
	{
		if (last_cmdline == NULL)
		{
			EMSG(e_nolastcmd);
			return FAIL;
		}
		stuffReadbuff(last_cmdline);
		return OK;
	}

	yankbuffer = c;
	get_yank_buffer(FALSE);
	if (y_current->y_array == NULL)
		return FAIL;

	for (i = 0; i < y_current->y_size; ++i)
	{
		stuffReadbuff(y_current->y_array[i]);
	/* insert newline between lines and after last line if type is MLINE */
		if (y_current->y_type == MLINE || i < y_current->y_size - 1)
			stuffReadbuff((char_u *)"\n");
	}
	return OK;
}

/*
 * dodelete - handle a delete operation
 */
	void
dodelete()
{
	register int	n;
	linenr_t		lnum;
	char_u			*ptr;
	char_u			*new, *old;
	linenr_t		old_lcount = curbuf->b_ml.ml_line_count;
	int				did_yank = FALSE;

	/*
	 * Imitate the strange Vi behaviour: If the delete spans more than one line
	 * and mtype == MCHAR and the result is a blank line, make the delete
	 * linewise. Don't do this for the change command.
	 */
	if (mtype == MCHAR && nlines > 1 && operator == DELETE)
	{
		ptr = ml_get(curbuf->b_endop.lnum) + curbuf->b_endop.col + mincl;
		skipspace(&ptr);
		if (*ptr == NUL && inindent())
			mtype = MLINE;
	}

/*
 * If a yank buffer was specified, put the deleted text into that buffer
 */
	if (yankbuffer != 0)
	{
										/* check for read-only buffer */
		if (!is_yank_buffer(yankbuffer, TRUE))
		{
			beep();
			return;
		}
		get_yank_buffer(TRUE);			/* yank into specified buffer */
		if (doyank(TRUE) == OK)
			did_yank = TRUE;
	}

/*
 * Put deleted text into register 1 and shift number buffers if
 * the delete contains a line break.
 * Overruled when a yankbuffer has been specified!
 */
	if (yankbuffer != 0 || mtype == MLINE || nlines > 1)
	{
		y_current = &y_buf[9];
		free_yank_all();				/* free buffer nine */
		for (n = 9; n > 1; --n)
			y_buf[n] = y_buf[n - 1];
		y_previous = y_current = &y_buf[1];
		y_buf[1].y_array = NULL;		/* set buffer one to empty */
		yankbuffer = 0;
	}
	else if (yankbuffer == 0)			/* yank into unnamed buffer */
		get_yank_buffer(TRUE);

	/*
	 * Do a yank of whatever we're about to delete. If there's too much stuff
	 * to fit in the yank buffer, then get a confirmation before doing the
	 * delete. This is crude, but simple. And it avoids doing a delete of
	 * something we can't put back if we want.
	 */
	if (yankbuffer == 0 && doyank(TRUE) == OK)
		did_yank = TRUE;

	if (!did_yank)
	{
		if (ask_yesno((char_u *)"cannot yank; delete anyway") != 'y')
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
		if (!u_save((linenr_t)(curbuf->b_startop.lnum - 1), (linenr_t)(curbuf->b_endop.lnum + 1)))
			return;

		for (lnum = curwin->w_cursor.lnum; curwin->w_cursor.lnum <= curbuf->b_endop.lnum; ++curwin->w_cursor.lnum)
		{
			block_prep(curwin->w_cursor.lnum, TRUE);
			if (textlen == 0)		/* nothing to delete */
				continue;

		/*
		 * If we delete a TAB, it may be replaced by several characters.
		 * Thus the number of characters may increase!
		 */
			n = textlen - startspaces - endspaces;		/* number of chars deleted */
			old = ml_get(curwin->w_cursor.lnum);
			new = alloc((unsigned)STRLEN(old) + 1 - n);
			if (new == NULL)
				continue;
		/* copy up to deleted part */
			memmove((char *)new, (char *)old, (size_t)textcol);
		/* insert spaces */
			copy_spaces(new + textcol, (size_t)(startspaces + endspaces));
		/* copy the part after the deleted part */
			old += textcol + textlen;
			memmove((char *)new + textcol + startspaces + endspaces,
									(char *)old, STRLEN(old) + 1);
		/* replace the line */
			ml_replace(curwin->w_cursor.lnum, new, FALSE);
		}
		curwin->w_cursor.lnum = lnum;
		CHANGED;
		updateScreen(VALID_TO_CURSCHAR);
		nlines = 0;		/* no lines deleted */
	}
	else if (mtype == MLINE)
	{
		if (operator == CHANGE)
		{
			dellines((long)(nlines - 1), TRUE, TRUE);
			if (!u_save_cursor())
				return;
			if (curbuf->b_p_ai)				/* don't delete indent */
			{
				beginline(TRUE);			/* put cursor on first non-white */
				did_ai = TRUE;				/* delete the indent when ESC hit */
			}
			while (delchar(FALSE) == OK)	/* slow but simple */
				;
			if (curwin->w_cursor.col > 0)
				--curwin->w_cursor.col;		/* put cursor on last char in line */
		}
		else
		{
			dellines(nlines, TRUE, TRUE);
		}
		u_clearline();	/* "U" command should not be possible after "dd" */
		beginline(TRUE);
	}
	else if (nlines == 1)		/* delete characters within one line */
	{
		if (!u_save_cursor())
			return;
		n = curbuf->b_endop.col - curbuf->b_startop.col + 1 - !mincl;
		while (n-- > 0)
			if (delchar(TRUE) == FAIL)
				break;
	}
	else						/* delete characters between lines */
	{
		if (!u_save_cursor())	/* save first line for undo */
			return;
		n = curwin->w_cursor.col;
		while (curwin->w_cursor.col >= n)	/* delete from cursor to end of line */
			if (delchar(TRUE) == FAIL)
				break;

		curbuf->b_startop = curwin->w_cursor;		/* remember curwin->w_cursor */
		++curwin->w_cursor.lnum;
		dellines((long)(nlines - 2), TRUE, TRUE);	/* includes save for undo */

		if (!u_save_cursor())	/* save last line for undo */
			return;
		n = curbuf->b_endop.col - !mincl;
		curwin->w_cursor.col = 0;
		while (n-- >= 0)		/* delete from start of line until endop */
			if (delchar(TRUE) == FAIL)
				break;
		curwin->w_cursor = curbuf->b_startop;		/* restore curwin->w_cursor */
		(void)dojoin(FALSE, TRUE);
	}

	if ((mtype == MCHAR && nlines == 1) || operator == CHANGE)
	{
		cursupdate();
		updateline();
	}
	else
		updateScreen(CURSUPD);

	msgmore(curbuf->b_ml.ml_line_count - old_lcount);

		/* correct endop for deleted text (for "']" command) */
	if (Visual_block)
		curbuf->b_endop.col = curbuf->b_startop.col;
	else
		curbuf->b_endop = curbuf->b_startop;
}

/*
 * dotilde - handle the (non-standard vi) tilde operator
 */
	void
dotilde()
{
	FPOS pos;

	if (!u_save((linenr_t)(curbuf->b_startop.lnum - 1), (linenr_t)(curbuf->b_endop.lnum + 1)))
		return;

	pos = curbuf->b_startop;
	if (Visual_block)		/* block mode */
	{
		for (; pos.lnum <= curbuf->b_endop.lnum; ++pos.lnum)
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
				curbuf->b_endop.col = STRLEN(ml_get(curbuf->b_endop.lnum));
				if (curbuf->b_endop.col)
						--curbuf->b_endop.col;
		}
		else if (!mincl)
			dec(&(curbuf->b_endop));

		while (ltoreq(pos, curbuf->b_endop))
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
			smsg((char_u *)"%ld line%s ~ed", nlines, plural(nlines));
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

	l = curbuf->b_startop.col;

	if (!no_op)
		dodelete();

	if ((l > curwin->w_cursor.col) && !lineempty(curwin->w_cursor.lnum))
		inc_cursor();

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
				smsg((char_u *)"freeing %ld lines", i + 1);
			free(y_current->y_array[i]);
		}
		free(y_current->y_array);
		y_current->y_array = NULL;
		if (n >= 1000)
			MSG("");
	}
}

	static void
free_yank_all()
{
		free_yank(y_current->y_size);
}

/*
 * Yank the text between curwin->w_cursor and startpos into a yank buffer.
 * If we are to append ("uppercase), we first yank into a new yank buffer and
 * then concatenate the old and the new one (so we keep the old one in case
 * of out-of-memory).
 *
 * return FAIL for failure, OK otherwise
 */
	int
doyank(deleting)
	int deleting;
{
	long 				i;				/* index in y_array[] */
	struct yankbuf		*curr;			/* copy of y_current */
	struct yankbuf		new; 			/* new yank buffer when appending */
	char_u				**new_ptr;
	register linenr_t	lnum;			/* current line number */
	long 				j;
	int					yanktype = mtype;
	long				yanklines = nlines;
	linenr_t			yankendlnum = curbuf->b_endop.lnum;

	char_u				*pnew;

									/* check for read-only buffer */
	if (yankbuffer != 0 && !is_yank_buffer(yankbuffer, TRUE))
	{
		beep();
		return FAIL;
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
	if (mtype == MCHAR && curbuf->b_startop.col == 0 && curbuf->b_endop.col == 0 && nlines > 1)
	{
		yanktype = MLINE;
		if (mincl == FALSE && yankendlnum > curbuf->b_startop.lnum)
		{
			--yankendlnum;
			--yanklines;
		}
	}

	y_current->y_size = yanklines;
	y_current->y_type = yanktype;	/* set the yank buffer type */
	y_current->y_array = (char_u **)lalloc((long_u)(sizeof(char_u *) * yanklines), TRUE);

	if (y_current->y_array == NULL)
	{
		y_current = curr;
		return FAIL;
	}

	i = 0;
	lnum = curbuf->b_startop.lnum;

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
			STRNCPY(pnew, textstart, (size_t)textlen);
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
					j = curbuf->b_endop.col - curbuf->b_startop.col + 1 - !mincl;
					if ((y_current->y_array[0] = strnsave(ml_get(lnum) + curbuf->b_startop.col, (int)j)) == NULL)
					{
	fail:
							free_yank(i);	/* free the lines that we allocated */
							y_current = curr;
							return FAIL;
					}
					goto success;
			}
			if ((y_current->y_array[0] = strsave(ml_get(lnum++) + curbuf->b_startop.col)) == NULL)
					goto fail;
			++i;
		}

		while (yanktype == MLINE ? (lnum <= yankendlnum) : (lnum < yankendlnum))
		{
			if ((y_current->y_array[i] = strsave(ml_get(lnum++))) == NULL)
					goto fail;
			++i;
		}
		if (yanktype != MLINE)
		{
			if ((y_current->y_array[i] = strnsave(ml_get(yankendlnum), curbuf->b_endop.col + 1 - !mincl)) == NULL)
					goto fail;
		}
	}

success:
	if (curr != y_current)		/* append the new block to the old block */
	{
		new_ptr = (char_u **)lalloc((long_u)(sizeof(char_u *) * (curr->y_size + y_current->y_size)), TRUE);
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
				pnew = lalloc((long_u)(STRLEN(curr->y_array[curr->y_size - 1]) + STRLEN(y_current->y_array[0]) + 1), TRUE);
				if (pnew == NULL)
				{
						i = y_current->y_size - 1;
						goto fail;
				}
				STRCPY(pnew, curr->y_array[--j]);
				STRCAT(pnew, y_current->y_array[0]);
				free(curr->y_array[j]);
				free(y_current->y_array[0]);
				curr->y_array[j++] = pnew;
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
			smsg((char_u *)"%ld line%s yanked", yanklines, plural(yanklines));
		}
	}

	return OK;
}

/*
 * put contents of register into the text
 */
	void
doput(dir, count, fix_indent)
	int		dir;				/* BACKWARD for 'P', FORWARD for 'p' */
	long	count;
	int		fix_indent;			/* make indent look nice */
{
	char_u		*ptr;
	char_u		*new, *old;
	int 		yanklen;
	int			oldlen;
	int			totlen = 0;		/* init for gcc */
	linenr_t	lnum;
	int			col;
	long 		i;		/* index in y_array[] */
	int 		y_type;
	long 		y_size;
	char_u		**y_array;
	long 		nlines = 0;
	int			vcol;
	int			delchar;
	int			incr = 0;
	long		j;
	FPOS		new_cursor;
	int			commandchar;
	char_u		temp[2];
	int			indent;
	int			orig_indent = 0;			/* init for gcc */
	int			indent_diff = 0;			/* init for gcc */
	int			first_indent = TRUE;

	if (fix_indent)
		orig_indent = get_indent();

	curbuf->b_startop = curwin->w_cursor;			/* default for "'[" command */
	if (dir == FORWARD)
		curbuf->b_startop.col++;
	curbuf->b_endop = curwin->w_cursor;				/* default for "']" command */
	commandchar = (dir == FORWARD ? (count == -1 ? 'o' : 'a') : (count == -1 ? 'O' : 'i'));
	if (yankbuffer == '.')		/* use inserted text */
	{
		stuff_inserted(commandchar, count, FALSE);
		return;
	}
	else if (yankbuffer == '%')	/* use file name */
	{
		if (check_fname() == OK)
		{
			stuffcharReadbuff(commandchar);
			stuffReadbuff(curbuf->b_xfilename);
			stuffcharReadbuff(ESC);
		}
		return;
	}
	else if (yankbuffer == ':')	/* use last command line */
	{
		if (last_cmdline == NULL)
			EMSG(e_nolastcmd);
		else
		{
			stuffcharReadbuff(commandchar);
			stuffReadbuff(last_cmdline);
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
		EMSG2("Nothing in register %s", temp);
		return;
	}

	if (y_type == MBLOCK)
	{
		lnum = curwin->w_cursor.lnum + y_size + 1;
		if (lnum > curbuf->b_ml.ml_line_count)
			lnum = curbuf->b_ml.ml_line_count + 1;
		if (!u_save(curwin->w_cursor.lnum - 1, lnum))
			return;
	}
	else if (!u_save_cursor())
		return;

	yanklen = STRLEN(y_array[0]);
	CHANGED;

	lnum = curwin->w_cursor.lnum;
	col = curwin->w_cursor.col;

/*
 * block mode
 */
	if (y_type == MBLOCK)
	{
		if (dir == FORWARD && gchar_cursor() != NUL)
		{
			col = getvcol(curwin, &curwin->w_cursor, 3) + 1;
			++curwin->w_cursor.col;
		}
		else
			col = getvcol(curwin, &curwin->w_cursor, 2);
		for (i = 0; i < y_size; ++i)
		{
			startspaces = 0;
			endspaces = 0;
			textcol = 0;
			vcol = 0;
			delchar = 0;

		/* add a new line */
			if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
			{
				ml_append(curbuf->b_ml.ml_line_count, (char_u *)"", (colnr_t)1, FALSE);
				++nlines;
			}
			old = ml_get(curwin->w_cursor.lnum);
			oldlen = STRLEN(old);
			for (ptr = old; vcol < col && *ptr; ++ptr)
			{
				/* Count a tab for what it's worth (if list mode not on) */
				incr = chartabsize(*ptr, (long)vcol);
				vcol += incr;
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
			yanklen = STRLEN(y_array[i]);
			totlen = count * yanklen + startspaces + endspaces;
			new = alloc((unsigned)totlen + oldlen + 1);
			if (new == NULL)
				break;
		/* copy part up to cursor to new line */
			ptr = new;
			memmove((char *)ptr, (char *)old, (size_t)textcol);
			ptr += textcol;
		/* may insert some spaces before the new text */
			copy_spaces(ptr, (size_t)startspaces);
			ptr += startspaces;
		/* insert the new text */
			for (j = 0; j < count; ++j)
			{
					STRNCPY(ptr, y_array[i], (size_t)yanklen);
					ptr += yanklen;
			}
		/* may insert some spaces after the new text */
			copy_spaces(ptr, (size_t)endspaces);
			ptr += endspaces;
		/* move the text after the cursor to the end of the line. */
			memmove((char *)ptr, (char *)old + textcol + delchar,
							(size_t)(oldlen - textcol - delchar + 1));
			ml_replace(curwin->w_cursor.lnum, new, FALSE);

			++curwin->w_cursor.lnum;
			if (i == 0)
				curwin->w_cursor.col += startspaces;
		}
		curbuf->b_endop.lnum = curwin->w_cursor.lnum - 1;		/* for "']" command */
		curbuf->b_endop.col = textcol + totlen - 1;
		curwin->w_cursor.lnum = lnum;
		cursupdate();
		updateScreen(VALID_TO_CURSCHAR);
	}
	else		/* not block mode */
	{
		if (y_type == MCHAR)
		{
	/* if type is MCHAR, FORWARD is the same as BACKWARD on the next character */
			if (dir == FORWARD && gchar_cursor() != NUL)
			{
				++col;
				if (yanklen)
				{
					++curwin->w_cursor.col;
					++curbuf->b_endop.col;
				}
			}
			new_cursor = curwin->w_cursor;
		}
		else if (dir == BACKWARD)
	/* if type is MLINE, BACKWARD is the same as FORWARD on the previous line */
			--lnum;

/*
 * simple case: insert into current line
 */
		if (y_type == MCHAR && y_size == 1)
		{
			totlen = count * yanklen;
			if (totlen)
			{
				old = ml_get(lnum);
				new = alloc((unsigned)(STRLEN(old) + totlen + 1));
				if (new == NULL)
					return; 				/* alloc() will give error message */
				memmove((char *)new, (char *)old, (size_t)col);
				ptr = new + col;
				for (i = 0; i < count; ++i)
				{
					memmove((char *)ptr, (char *)y_array[0], (size_t)yanklen);
					ptr += yanklen;
				}
				memmove((char *)ptr, (char *)old + col, STRLEN(old + col) + 1);
				ml_replace(lnum, new, FALSE);
				curwin->w_cursor.col += (colnr_t)(totlen - 1);	/* put cursor on last putted char */
			}
			curbuf->b_endop = curwin->w_cursor;
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
					 * First insert y_array[size - 1] in front of second line.
					 * Then append y_array[0] to first line.
					 */
					ptr = ml_get(lnum) + col;
					totlen = STRLEN(y_array[y_size]);
					new = alloc((unsigned)(STRLEN(ptr) + totlen + 1));
					if (new == NULL)
						goto error;
					STRCPY(new, y_array[y_size]);
					STRCAT(new, ptr);
					ml_append(lnum, new, (colnr_t)0, FALSE);	/* insert second line */
					free(new);
					++nlines;

					old = ml_get(lnum);
					new = alloc((unsigned)(col + yanklen + 1));
					if (new == NULL)
						goto error;
											/* copy first part of line */
					memmove((char *)new, (char *)old, (size_t)col);
											/* append to first line */
					memmove((char *)new + col, (char *)y_array[0],
											(size_t)(yanklen + 1));
					ml_replace(lnum, new, FALSE);

					curwin->w_cursor.lnum = lnum;
					i = 1;
				}

				while (i < y_size)
				{
					if (ml_append(lnum++, y_array[i++], (colnr_t)0, FALSE) == FAIL)
						goto error;
					if (fix_indent)
					{
						curwin->w_cursor.lnum = lnum;
						if (curbuf->b_p_si && *ml_get(lnum) == '#')
							indent = 0;		/* Leave # lines at start */
						else if (first_indent)
						{
							indent_diff = orig_indent - get_indent();
							indent = orig_indent;
							first_indent = FALSE;
						}
						else if ((indent = get_indent() + indent_diff) < 0)
							indent = 0;
						set_indent(indent, TRUE);
					}
					++nlines;
				}
				if (y_type == MCHAR)
					++lnum; 	/* lnum is now number of line below inserted lines */
			}

			curbuf->b_endop.lnum = lnum;		/* for "']" command */
			if (y_type == MLINE)
			{
				curwin->w_cursor.col = 0;
				curbuf->b_endop.col = 0;
				if (dir == FORWARD)
				{
					updateScreen(NOT_VALID);		/* recompute curwin->w_botline */
					++curwin->w_cursor.lnum;
				}
					/* put cursor on first non-blank in last inserted line */
				beginline(TRUE);
			}
			else		/* put cursor on first inserted character */
			{
				if (col > 1)
					curbuf->b_endop.col = col - 1;
				else
					curbuf->b_endop.col = 0;
				curwin->w_cursor = new_cursor;
			}

error:
			if (y_type == MLINE)		/* for '[ */
			{
				curbuf->b_startop.col = 0;
				if (dir == FORWARD)
					curbuf->b_startop.lnum++;
			}
			mark_adjust(curbuf->b_startop.lnum + (y_type == MCHAR), MAXLNUM, nlines);
			updateScreen(CURSUPD);
		}
	}

	msgmore(nlines);
	curwin->w_set_curswant = TRUE;
}

/*
 * display the contents of the yank buffers
 */
	void
dodis()
{
	register int			i, n;
	register long			j;
	register char_u			*p;
	register struct yankbuf *yb;

	gotocmdline(TRUE, NUL);

	msg_outstr((char_u *)"--- Registers ---");
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
			msg_outchar('\n');
			if (i == -1)
				msg_outstr((char_u *)"\"\"");
			else
			{
				msg_outchar('"');
				if (i < 10)
					msg_outchar(i + '0');
				else
					msg_outchar(i + 'a' - 10);
			}
			msg_outstr((char_u *)"   ");

			n = (int)Columns - 6;
			for (j = 0; j < yb->y_size && n > 1; ++j)
			{
				if (j)
				{
					msg_outstr((char_u *)"^J");
					n -= 2;
				}
				for (p = yb->y_array[j]; *p && (n -= charsize(*p)) >= 0; ++p)
					msg_outtrans(p, 1);
			}
			flushbuf();				/* show one line at a time */
		}
	}

	/*
	 * display last inserted text
	 */
	if ((p = get_last_insert()) != NULL)
	{
		msg_outstr((char_u *)"\n\".   ");
		dis_msg(p, TRUE);
	}

	/*
	 * display last command line
	 */
	if (last_cmdline != NULL)
	{
		msg_outstr((char_u *)"\n\":   ");
		dis_msg(last_cmdline, FALSE);
	}

	/*
	 * display current file name
	 */
	if (curbuf->b_xfilename != NULL)
	{
		msg_outstr((char_u *)"\n\"%   ");
		dis_msg(curbuf->b_xfilename, FALSE);
	}

	msg_end();
}

/*
 * display a string for dodis()
 * truncate at end of screen line
 */
	void
dis_msg(p, skip_esc)
	char_u		*p;
	int			skip_esc;			/* if TRUE, ignore trailing ESC */
{
	int		n;

	n = (int)Columns - 6;
	while (*p && !(*p == ESC && skip_esc && *(p + 1) == NUL) &&
						(n -= charsize(*p)) >= 0)
		msg_outtrans(p++, 1);
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
	if (!u_save((linenr_t)(curwin->w_cursor.lnum - 1), (linenr_t)(curwin->w_cursor.lnum + count)))
		return;

	while (--count > 0)
		if (dojoin(insert_space, redraw) == FAIL)
		{
				beep();
				break;
		}

	if (redraw)
		updateScreen(VALID_TO_CURSCHAR);
}

/*
 * join two lines at the cursor position
 *
 * return FAIL for failure, OK ohterwise
 */
	int
dojoin(insert_space, redraw)
	int			insert_space;
	int			redraw;
{
	char_u		*curr;
	char_u		*next;
	char_u		*new;
	int			endcurr1, endcurr2;
	int 		currsize;		/* size of the current line */
	int 		nextsize;		/* size of the next line */
	int			spaces;			/* number of spaces to insert */
	int			rows_to_del;	/* number of rows on screen to delete */
	linenr_t	t;

	if (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)		/* on last line */
		return FAIL;

	rows_to_del = plines_m(curwin->w_cursor.lnum, curwin->w_cursor.lnum + 1);

	curr = ml_get(curwin->w_cursor.lnum);
	currsize = STRLEN(curr);
	endcurr1 = endcurr2 = NUL;
	if (currsize > 0)
	{
		endcurr1 = *(curr + currsize - 1);
		if (currsize > 1)
			endcurr2 = *(curr + currsize - 2);
	}

	next = ml_get((linenr_t)(curwin->w_cursor.lnum + 1));
	spaces = 0;
	if (insert_space)
	{
		skipspace(&next);
		spaces = 1;
		if (*next == ')' || currsize == 0)
			spaces = 0;
		else
		{
			if (endcurr1 == ' ' || endcurr1 == TAB)
			{
				spaces = 0;
				if (currsize > 1)
					endcurr1 = endcurr2;
			}
			if (p_js && strchr(".!?", endcurr1) != NULL)
				spaces = 2;
		}
	}
	nextsize = STRLEN(next);

	new = alloc((unsigned)(currsize + nextsize + spaces + 1));
	if (new == NULL)
		return FAIL;

	/*
	 * Insert the next line first, because we already have that pointer.
	 * Curr has to be obtained again, because getting next will have
	 * invalidated it.
	 */
	memmove((char *)new + currsize + spaces, (char *)next, (size_t)(nextsize + 1));

	curr = ml_get(curwin->w_cursor.lnum);
	memmove((char *)new, (char *)curr, (size_t)currsize);

	copy_spaces(new + currsize, (size_t)spaces);

	ml_replace(curwin->w_cursor.lnum, new, FALSE);

	/*
	 * Delete the following line. To do this we move the cursor there
	 * briefly, and then move it back. After dellines() the cursor may
	 * have moved up (last line deleted), so the current lnum is kept in t.
	 */
	t = curwin->w_cursor.lnum;
	++curwin->w_cursor.lnum;
	dellines(1L, FALSE, FALSE);
	curwin->w_cursor.lnum = t;

	/*
	 * the number of rows on the screen is reduced by the difference
	 * in number of rows of the two old lines and the one new line
	 */
	if (redraw)
	{
		rows_to_del -= plines(curwin->w_cursor.lnum);
		if (rows_to_del > 0)
			win_del_lines(curwin, curwin->w_row, rows_to_del, TRUE, TRUE);
	}

 	/*
	 * go to first character of the joined line
	 */
	if (currsize == 0)
		curwin->w_cursor.col = 0;
	else
	{
		curwin->w_cursor.col = currsize - 1;
		(void)oneright();
	}
	CHANGED;

	return OK;
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
	while (curwin->w_cursor.col && isspace(gchar_cursor()))
		dec_cursor();
	curs_columns(FALSE);			/* update curwin->w_virtcol */

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
	Insstart = curwin->w_cursor;
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
	char_u		*pend;

	startspaces = 0;
	endspaces = 0;
	textlen = 0;
	textcol = 0;
	vcol = 0;
	textstart = ml_get(lnum);
	while (vcol < startvcol && *textstart)
	{
		/* Count a tab for what it's worth (if list mode not on) */
		incr = chartabsize(*textstart, (long)vcol);
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
		if (vcol > endvcol)		/* it's all in one character */
		{
			startspaces = endvcol - startvcol + 1;
			if (delete)
				startspaces = incr - startspaces;
		}
		else
		{
			while (vcol <= endvcol && *pend)
			{
				/* Count a tab for what it's worth (if list mode not on) */
				incr = chartabsize(*pend, (long)vcol);
				vcol += incr;
				++pend;
			}
			if (vcol < endvcol && !delete)	/* line too short */
			{
				endspaces = endvcol - vcol;
			}
			else if (vcol > endvcol)
			{
				endspaces = vcol - endvcol - 1;
				if (!delete && pend != textstart && endspaces)
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

#define NUMBUFLEN 30

/*
 * add or subtract 'Prenum1' from a number in a line
 * 'command' is CTRL-A for add, CTRL-X for subtract
 *
 * return FAIL for failure, OK otherwise
 */
	int
doaddsub(command, Prenum1)
	int			command;
	linenr_t	Prenum1;
{
	register int 	col;
	char_u			buf[NUMBUFLEN];
	int				hex;		/* 'x' or 'X': hexadecimal; '0': octal */
	static int		hexupper = FALSE;	/* 0xABC */
	long			n;
	char_u			*ptr;
	int				i;
	int				c;

	ptr = ml_get(curwin->w_cursor.lnum);
	col = curwin->w_cursor.col;

		/* first check if we are on a hexadecimal number */
	while (col > 0 && isxdigit(ptr[col]))
		--col;
	if (col > 0 && (ptr[col] == 'X' || ptr[col] == 'x') &&
						ptr[col - 1] == '0' && isxdigit(ptr[col + 1]))
		--col;		/* found hexadecimal number */
	else
	{
		/* first search forward and then backward for start of number */
		col = curwin->w_cursor.col;

		while (ptr[col] != NUL && !isdigit(ptr[col]))
			++col;

		while (col > 0 && isdigit(ptr[col - 1]))
			--col;
	}

	if (isdigit(ptr[col]) && u_save_cursor())
	{
		ptr = ml_get(curwin->w_cursor.lnum);	/* get it again, because of undo */
		curwin->w_set_curswant = TRUE;

		hex = 0;								/* default is decimal */
		if (ptr[col] == '0')					/* could be hex or octal */
		{
			hex = TO_UPPER(ptr[col + 1]);		/* assume hexadecimal */
			if (hex != 'X' || !isxdigit(ptr[col + 2]))
			{
				if (isdigit(hex))
					hex = '0';					/* octal */
				else
					hex = 0;					/* 0 by itself is decimal */
			}
		}

		if (!hex && col > 0 && ptr[col - 1] == '-')
			--col;

		ptr += col;
		/*
		 * we copy the number into a buffer because some versions of sscanf
		 * cannot handle characters with the upper bit set, making some special
		 * characters handled like digits.
		 */
		for (i = 0; *ptr && !(*ptr & 0x80) && i < NUMBUFLEN - 1; ++i)
			buf[i] = *ptr++;
		buf[i] = NUL;

		if (hex == '0')
			sscanf((char *)buf, "%lo", &n);
		else if (hex)
			sscanf((char *)buf + 2, "%lx", &n);	/* "%X" doesn't work! */
		else
			n = atol((char *)buf);

		if (command == Ctrl('A'))
			n += Prenum1;
		else
			n -= Prenum1;

		if (hex == 'X')					/* skip the '0x' */
			col += 2;
		curwin->w_cursor.col = col;
		c = gchar_cursor();
		do								/* delete the old number */
		{
			if (isalpha(c))
			{
				if (isupper(c))
					hexupper = TRUE;
				else
					hexupper = FALSE;
			}
			(void)delchar(FALSE);
			c = gchar_cursor();
		}
		while (hex ? (hex == '0' ? c >= '0' && c <= '7' : isxdigit(c)) : isdigit(c));

		if (hex == '0')
			sprintf((char *)buf, "0%lo", n);
		else if (hex && hexupper)
			sprintf((char *)buf, "%lX", n);
		else if (hex)
			sprintf((char *)buf, "%lx", n);
		else
			sprintf((char *)buf, "%ld", n);
		insstr(buf);					/* insert the new number */
		--curwin->w_cursor.col;
		updateline();
		return OK;
	}
	else
	{
		beep();
		return FAIL;
	}
}
