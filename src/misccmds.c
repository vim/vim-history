/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * misccmds.c: functions that didn't seem to fit elsewhere
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static void check_status __ARGS((BUF *));

static char_u *(si_tab[]) = {(char_u *)"if", (char_u *)"else", (char_u *)"while", (char_u *)"for", (char_u *)"do"};

/*
 * count the size of the indent in the current line
 */
	int
get_indent()
{
	register char_u *ptr;
	register int count = 0;

	for (ptr = ml_get(curwin->w_cursor.lnum); *ptr; ++ptr)
	{
		if (*ptr == TAB)	/* count a tab for what it is worth */
			count += (int)curbuf->b_p_ts - (count % (int)curbuf->b_p_ts);
		else if (*ptr == ' ')
			++count;			/* count a space for one */
		else
			break;
	}
	return (count);
}

/*
 * set the indent of the current line
 * leaves the cursor on the first non-blank in the line
 */
	void
set_indent(size, delete)
	register int size;
	int delete;
{
	int				oldstate = State;
	register int	c;

	State = INSERT;		/* don't want REPLACE for State */
	curwin->w_cursor.col = 0;
	if (delete)							/* delete old indent */
	{
		while ((c = gchar_cursor()), iswhite(c))
			(void)delchar(FALSE);
	}
	if (!curbuf->b_p_et)			/* if 'expandtab' is set, don't use TABs */
		while (size >= (int)curbuf->b_p_ts)
		{
			inschar(TAB);
			size -= (int)curbuf->b_p_ts;
		}
	while (size)
	{
		inschar(' ');
		--size;
	}
	State = oldstate;
}

/*
 * Opencmd
 *
 * Add a blank line below or above the current line.
 *
 * Return TRUE for success, FALSE for failure
 */

	int
Opencmd(dir, redraw, delspaces)
	int 		dir;
	int			redraw;
	int			delspaces;
{
	char_u   *ptr, *p_extra;
	FPOS	old_cursor; 			/* old cursor position */
	int		newcol = 0;			/* new cursor column */
	int 	newindent = 0;		/* auto-indent of the new line */
	int		n;
	int		truncate = FALSE;	/* truncate current line afterwards */
	int		no_si = FALSE;		/* reset did_si afterwards */
	int		retval = FALSE;		/* return value, default is FAIL */

	ptr = strsave(ml_get(curwin->w_cursor.lnum));
	if (ptr == NULL)			/* out of memory! */
		return FALSE;

	u_clearline();				/* cannot do "U" command when adding lines */
	did_si = FALSE;
	if (curbuf->b_p_ai || curbuf->b_p_si)
	{
		/*
		 * count white space on current line
		 */
		newindent = get_indent();
		if (newindent == 0)
			newindent = old_indent;		/* for ^^D command in insert mode */
		old_indent = 0;

			/*
			 * If we just did an auto-indent, then we didn't type anything on the
			 * prior line, and it should be truncated.
			 */
		if (dir == FORWARD && did_ai)
			truncate = TRUE;
		else if (curbuf->b_p_si && *ptr != NUL)
		{
			char_u	*p;
			char_u	*pp;
			int		i, save;

			if (dir == FORWARD)
			{
				p = ptr + STRLEN(ptr) - 1;
				while (p > ptr && isspace(*p))	/* find last non-blank in line */
					--p;
				if (*p == '{')					/* line ends in '{': do indent */
				{
					did_si = TRUE;
					no_si = TRUE;
				}
				else							/* look for "if" and the like */
				{
					p = ptr;
					skipspace(&p);
					for (pp = p; islower(*pp); ++pp)
						;
					if (!isidchar(*pp))			/* careful for vars starting with "if" */
					{
						save = *pp;
						*pp = NUL;
						for (i = sizeof(si_tab)/sizeof(char_u *); --i >= 0; )
							if (STRCMP(p, si_tab[i]) == 0)
							{
								did_si = TRUE;
								break;
							}
						*pp = save;
					}
				}
			}
			else
			{
				p = ptr;
				skipspace(&p);
				if (*p == '}')			/* if line starts with '}': do indent */
					did_si = TRUE;
			}
		}
		did_ai = TRUE;
		if (curbuf->b_p_si)
			can_si = TRUE;
	}
	if (State == INSERT || State == REPLACE)	/* only when dir == FORWARD */
	{
		p_extra = ptr + curwin->w_cursor.col;
		if (curbuf->b_p_ai && delspaces)
			skipspace(&p_extra);
		if (*p_extra != NUL)
			did_ai = FALSE; 		/* append some text, don't trucate now */
	}
	else
		p_extra = (char_u *)"";				/* append empty line */

	old_cursor = curwin->w_cursor;
	if (dir == BACKWARD)
		--curwin->w_cursor.lnum;
	if (ml_append(curwin->w_cursor.lnum, p_extra, (colnr_t)0, FALSE) == FAIL)
		goto theend;
	mark_adjust(curwin->w_cursor.lnum + 1, MAXLNUM, 1L);
	if (newindent || did_si)
	{
		++curwin->w_cursor.lnum;
		if (did_si)
		{
			if (p_sr)
				newindent -= newindent % (int)curbuf->b_p_sw;
			newindent += (int)curbuf->b_p_sw;
		}
		set_indent(newindent, FALSE);
		newcol = curwin->w_cursor.col;
		if (no_si)
			did_si = FALSE;
	}
	curwin->w_cursor = old_cursor;

	if (dir == FORWARD)
	{
		if (truncate || State == INSERT || State == REPLACE)
		{
			if (truncate)
				*ptr = NUL;
			else
				*(ptr + curwin->w_cursor.col) = NUL;	/* truncate current line at cursor */
			ml_replace(curwin->w_cursor.lnum, ptr, FALSE);
			ptr = NULL;
		}

		/*
		 * Get the cursor to the start of the line, so that 'curwin->w_row' gets
		 * set to the right physical line number for the stuff that
		 * follows...
		 */
		curwin->w_cursor.col = 0;

		if (redraw)
		{
			n = RedrawingDisabled;
			RedrawingDisabled = TRUE;
			cursupdate();				/* don't want it to update srceen */
			RedrawingDisabled = n;

			/*
			 * If we're doing an open on the last logical line, then go ahead and
			 * scroll the screen up. Otherwise, just insert a blank line at the
			 * right place. We use calls to plines() in case the cursor is
			 * resting on a long line.
			 */
			n = curwin->w_row + plines(curwin->w_cursor.lnum);
			if (n == curwin->w_height)
				scrollup(1L);
			else
				win_ins_lines(curwin, n, 1, TRUE, TRUE);
		}
		++curwin->w_cursor.lnum;	/* cursor moves down */
	}
	else if (redraw) 				/* insert physical line above current line */
		win_ins_lines(curwin, curwin->w_row, 1, TRUE, TRUE);

	curwin->w_cursor.col = newcol;
	if (redraw)
	{
		updateScreen(VALID_TO_CURSCHAR);
		cursupdate();			/* update curwin->w_row */
	}
	CHANGED;

	retval = TRUE;				/* success! */
theend:
	free(ptr);
	return retval;
}

/*
 * plines(p) - return the number of physical screen lines taken by line 'p'
 */
	int
plines(p)
	linenr_t	p;
{
	return plines_win(curwin, p);
}
	
	int
plines_win(wp, p)
	WIN			*wp;
	linenr_t	p;
{
	register long		col = 0;
	register char_u		*s;
	register int		lines;

	if (!wp->w_p_wrap)
		return 1;

	s = ml_get_buf(wp->w_buffer, p, FALSE);
	if (*s == NUL)				/* empty line */
		return 1;

	while (*s != NUL)
		col += chartabsize(*s++, col);

	/*
	 * If list mode is on, then the '$' at the end of the line takes up one
	 * extra column.
	 */
	if (wp->w_p_list)
		col += 1;

	/*
	 * If 'number' mode is on, add another 8.
	 */
	if (wp->w_p_nu)
		col += 8;

	lines = (col + (Columns - 1)) / Columns;
	if (lines <= wp->w_height)
		return lines;
	return (int)(wp->w_height);		/* maximum length */
}

/*
 * Count the physical lines (rows) for the lines "first" to "last" inclusive.
 */
	int
plines_m(first, last)
	linenr_t		first, last;
{
	return plines_m_win(curwin, first, last);
}

	int
plines_m_win(wp, first, last)
	WIN				*wp;
	linenr_t		first, last;
{
	int count = 0;

	while (first <= last)
		count += plines_win(wp, first++);
	return (count);
}

/*
 * insert or replace a single character at the cursor position
 */
	void
inschar(c)
	int			c;
{
	register char_u  *p;
	int				rir0;		/* reverse replace in column 0 */
	char_u			*new;
	char_u			*old;
	int				oldlen;
	int				extra;
	colnr_t			col = curwin->w_cursor.col;
	linenr_t		lnum = curwin->w_cursor.lnum;

	old = ml_get(lnum);
	oldlen = STRLEN(old) + 1;

	rir0 = (State == REPLACE && p_ri && col == 0);
	if (rir0 || State != REPLACE || *(old + col) == NUL)
		extra = 1;
	else
		extra = 0;

	new = alloc((unsigned)(oldlen + extra));
	if (new == NULL)
		return;
	memmove((char *)new, (char *)old, (size_t)col);
	p = new + col;
	memmove((char *)p + extra, (char *)old + col, (size_t)(oldlen - col));
	if (rir0)					/* reverse replace in column 0 */
	{
		*(p + 1) = c;			/* replace the char that was in column 0 */
		c = ' ';				/* insert a space */
		extraspace = TRUE;
	}
	*p = c;
	ml_replace(lnum, new, FALSE);

	/*
	 * If we're in insert mode and showmatch mode is set, then check for
	 * right parens and braces. If there isn't a match, then beep. If there
	 * is a match AND it's on the screen, then flash to it briefly. If it
	 * isn't on the screen, don't do anything.
	 */
	if (p_sm && State == INSERT && (c == ')' || c == '}' || c == ']'))
	{
		FPOS		   *lpos, csave;

		if ((lpos = showmatch(NUL)) == NULL)		/* no match, so beep */
			beep();
		else if (lpos->lnum >= curwin->w_topline)
		{
			updateScreen(VALID_TO_CURSCHAR); /* show the new char first */
			csave = curwin->w_cursor;
			curwin->w_cursor = *lpos; 	/* move to matching char */
			cursupdate();
			showruler(0);
			setcursor();
			cursor_on();		/* make sure that the cursor is shown */
			flushbuf();
			vim_delay();		/* brief pause */
			curwin->w_cursor = csave; 	/* restore cursor position */
			cursupdate();
		}
	}
	if (!p_ri)							/* normal insert: cursor right */
		++curwin->w_cursor.col;
	else if (State == REPLACE && !rir0)	/* reverse replace mode: cursor left */
		--curwin->w_cursor.col;
	CHANGED;
}

/*
 * insert a string at the cursor position
 */
	void
insstr(s)
	register char_u  *s;
{
	register char_u		*old, *new;
	register int		newlen = STRLEN(s);
	int					oldlen;
	colnr_t				col = curwin->w_cursor.col;
	linenr_t			lnum = curwin->w_cursor.lnum;

	old = ml_get(lnum);
	oldlen = STRLEN(old);
	new = alloc((unsigned)(oldlen + newlen + 1));
	if (new == NULL)
		return;
	memmove((char *)new, (char *)old, (size_t)col);
	memmove((char *)new + col, (char *)s, (size_t)newlen);
	memmove((char *)new + col + newlen, (char *)old + col, (size_t)(oldlen - col + 1));
	ml_replace(lnum, new, FALSE);
	curwin->w_cursor.col += newlen;
	CHANGED;
}

/*
 * delete one character under the cursor
 *
 * return FAIL for failure, OK otherwise
 */
	int
delchar(fixpos)
	int			fixpos; 	/* if TRUE fix the cursor position when done */
{
	char_u		*old, *new;
	int			oldlen;
	linenr_t	lnum = curwin->w_cursor.lnum;
	colnr_t		col = curwin->w_cursor.col;
	int			was_alloced;

	old = ml_get(lnum);
	oldlen = STRLEN(old);

	if (col >= oldlen)	/* can't do anything (happens with replace mode) */
		return FAIL;

/*
 * If the old line has been allocated the deleteion can be done in the
 * existing line. Otherwise a new line has to be allocated
 */
	was_alloced = ml_line_alloced();		/* check if old was allocated */
	if (was_alloced)
		new = old;							/* use same allocated memory */
	else
	{
		new = alloc((unsigned)oldlen);		/* need to allocated a new line */
		if (new == NULL)
			return FAIL;
		memmove((char *)new, (char *)old, (size_t)col);
	}
	memmove((char *)new + col, (char *)old + col + 1, (size_t)(oldlen - col));
	if (!was_alloced)
		ml_replace(lnum, new, FALSE);

	/*
	 * If we just took off the last character of a non-blank line, we don't
	 * want to end up positioned at the newline.
	 */
	if (fixpos && curwin->w_cursor.col > 0 && col == oldlen - 1)
		--curwin->w_cursor.col;

	CHANGED;
	return OK;
}

	void
dellines(nlines, dowindow, undo)
	long 			nlines;			/* number of lines to delete */
	int 			dowindow;		/* if true, update the window */
	int				undo;			/* if true, prepare for undo */
{
	int 			num_plines = 0;

	if (nlines <= 0)
		return;
	/*
	 * There's no point in keeping the window updated if we're deleting more
	 * than a window's worth of lines.
	 */
	if (nlines > (curwin->w_height - curwin->w_row) && dowindow)
	{
		dowindow = FALSE;
		/* flaky way to clear rest of window */
		win_del_lines(curwin, curwin->w_row, curwin->w_height, TRUE, TRUE);
	}
	if (undo && !u_savedel(curwin->w_cursor.lnum, nlines))
		return;

	mark_adjust(curwin->w_cursor.lnum, curwin->w_cursor.lnum + nlines - 1, MAXLNUM);
	mark_adjust(curwin->w_cursor.lnum + nlines, MAXLNUM, -nlines);

	while (nlines-- > 0)
	{
		if (bufempty()) 		/* nothing to delete */
			break;

		/*
		 * Set up to delete the correct number of physical lines on the
		 * window
		 */
		if (dowindow)
			num_plines += plines(curwin->w_cursor.lnum);

		ml_delete(curwin->w_cursor.lnum);

		CHANGED;

		/* If we delete the last line in the file, stop */
		if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		{
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
			break;
		}
	}
	curwin->w_cursor.col = 0;
	/*
	 * Delete the correct number of physical lines on the window
	 */
	if (dowindow && num_plines > 0)
		win_del_lines(curwin, curwin->w_row, num_plines, TRUE, TRUE);
}

	int
gchar(pos)
	FPOS *pos;
{
	return (int)(*(ml_get_pos(pos)));
}

	int
gchar_cursor()
{
	return (int)(*(ml_get_cursor()));
}

/*
 * Write a character at the current cursor position.
 * It is directly written into the block.
 */
	void
pchar_cursor(c)
	int c;
{
	*(ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE) + curwin->w_cursor.col) = c;
}

/*
 * return TRUE if the cursor is before or on the first non-blank in the line
 */
	int
inindent()
{
	register char_u *ptr;
	register int col;

	for (col = 0, ptr = ml_get(curwin->w_cursor.lnum); iswhite(*ptr); ++col)
		++ptr;
	if (col >= curwin->w_cursor.col)
		return TRUE;
	else
		return FALSE;
}

/*
 * skipspace: skip over ' ' and '\t'.
 *
 * note: you must give a pointer to a char_u pointer!
 */
	void
skipspace(pp)
	char_u **pp;
{
    register char_u *p;
    
    for (p = *pp; *p == ' ' || *p == '\t'; ++p)	/* skip to next non-white */
    	;
    *pp = p;
}

/*
 * skiptospace: skip over text until ' ' or '\t'.
 *
 * note: you must give a pointer to a char_u pointer!
 */
	void
skiptospace(pp)
	char_u **pp;
{
	register char_u *p;

	for (p = *pp; *p != ' ' && *p != '\t' && *p != NUL; ++p)
		;
	*pp = p;
}

/*
 * skiptodigit: skip over text until digit found
 *
 * note: you must give a pointer to a char_u pointer!
 */
	void
skiptodigit(pp)
	char_u **pp;
{
	register char_u *p;

	for (p = *pp; !isdigit(*p) && *p != NUL; ++p)
		;
	*pp = p;
}

/*
 * getdigits: get a number from a string and skip over it
 *
 * note: you must give a pointer to a char_u pointer!
 */

	long
getdigits(pp)
	char_u **pp;
{
    register char_u *p;
	long retval;
    
	p = *pp;
	retval = atol((char *)p);
    while (isdigit(*p))	/* skip to next non-digit */
    	++p;
    *pp = p;
	return retval;
}

	char_u *
plural(n)
	long n;
{
	static char_u buf[2] = "s";

	if (n == 1)
		return &(buf[1]);
	return &(buf[0]);
}

/*
 * set_Changed is called when something in the current buffer is changed
 */
	void
set_Changed()
{
	if (!curbuf->b_changed)
	{
		change_warning();
		curbuf->b_changed = TRUE;
		check_status(curbuf);
	}
}

/*
 * unset_Changed is called when the changed flag must be reset for buffer 'buf'
 */
	void
unset_Changed(buf)
	BUF		*buf;
{
	if (buf->b_changed)
	{
		buf->b_changed = 0;
		check_status(buf);
	}
}

/*
 * check_status: called when the status bars for the buffer 'buf'
 *				 need to be updated
 */
	static void
check_status(buf)
	BUF		*buf;
{
	WIN		*wp;
	int		i;

	i = 0;
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
		if (wp->w_buffer == buf && wp->w_status_height)
		{
			wp->w_redr_status = TRUE;
			++i;
		}
	if (i && must_redraw < NOT_VALID)		/* redraw later */
		must_redraw = NOT_VALID;
}

/*
 * If the file is readonly, give a warning message with the first change.
 * Don't use emsg(), because it flushes the macro buffer.
 * If we have undone all changes b_changed will be FALSE, but b_did_warn
 * will be TRUE.
 */
	void
change_warning()
{
	if (curbuf->b_did_warn == FALSE && curbuf->b_changed == 0 && curbuf->b_p_ro)
	{
		curbuf->b_did_warn = TRUE;
		MSG("Warning: Changing a readonly file");
		sleep(1);			/* give him some time to think about it */
	}
}

/*
 * ask for a reply from the user, a 'y' or a 'n'.
 * No other characters are accepted, the message is repeated until a valid
 * reply is entered or CTRL-C is hit.
 *
 * return the 'y' or 'n'
 */
	int
ask_yesno(str)
	char_u *str;
{
	int r = ' ';

	while (r != 'y' && r != 'n')
	{
		(void)set_highlight('r');		/* same highlighting as for wait_return */
		msg_highlight = TRUE;
		smsg((char_u *)"%s (y/n)?", str);
		r = vgetc();
		if (r == Ctrl('C'))
			r = 'n';
		msg_outchar(r);		/* show what you typed */
		flushbuf();
	}
	return r;
}

	void
msgmore(n)
	long n;
{
	long pn;

	if (global_busy)		/* no messages now, wait until global is finished */
		return;

	if (n > 0)
		pn = n;
	else
		pn = -n;

	if (pn > p_report)
		smsg((char_u *)"%ld %s line%s %s", pn, n > 0 ? "more" : "fewer", plural(pn),
											got_int ? "(Interrupted)" : "");
}

/*
 * give a warning for an error
 */
	void
beep()
{
	flush_buffers(FALSE);		/* flush internal buffers */
	if (p_vb)
	{
		if (T_VB && *T_VB)
		    outstr(T_VB);
		else
		{						/* very primitive visual bell */
	        MSG("    ^G");
	        MSG("     ^G");
	        MSG("    ^G ");
	        MSG("     ^G");
	        MSG("       ");
			showmode();			/* may have deleted the mode message */
		}
	}
	else
	    outchar('\007');
}

/* 
 * Expand environment variable with path name.
 * "~/" is also expanded, like $HOME.
 * If anything fails no expansion is done and dst equals src.
 */
	void
expand_env(src, dst, dstlen)
	char_u	*src;			/* input string e.g. "$HOME/vim.hlp" */
	char_u	*dst;			/* where to put the result */
	int		dstlen;			/* maximum length of the result */
{
	char_u	*tail;
	int		c;
	char_u	*var;

	if (*src == '$' || (*src == '~' && STRCHR("/ \t\n", src[1]) != NULL))
	{
/*
 * The variable name is copied into dst temporarily, because it may be
 * a string in read-only memory.
 */
		if (*src == '$')
		{
			tail = src + 1;
			var = dst;
			c = dstlen - 1;
			while (c-- > 0 && *tail && isidchar(*tail))
				*var++ = *tail++;
			*var = NUL;
/*
 * It is possible that vimgetenv() uses IObuff for the expansion, and that the
 * 'dst' is also IObuff. This works, as long as 'var' is the first to be copied
 * to 'dst'!
 */
			var = vimgetenv(dst);
		}
		else
		{
			var = vimgetenv((char_u *)"HOME");
			tail = src + 1;
		}
		if (var && (STRLEN(var) + STRLEN(tail) + 1 < (unsigned)dstlen))
		{
			STRCPY(dst, var);
			STRCAT(dst, tail);
			return;
		}
	}
	STRNCPY(dst, src, (size_t)dstlen);
}

/* 
 * Replace home directory by "~/"
 * If anything fails dst equals src.
 */
	void
home_replace(src, dst, dstlen)
	char_u	*src;			/* input file name */
	char_u	*dst;			/* where to put the result */
	int		dstlen;			/* maximum length of the result */
{
	char_u	*home;
	size_t	len;

	/*
	 * If there is no "HOME" environment variable, or when it
	 * is very short, don't replace anything.
	 */
	if ((home = vimgetenv((char_u *)"HOME")) == NULL || (len = STRLEN(home)) <= 1)
		STRNCPY(dst, src, (size_t)dstlen);
	else
	{
		skipspace(&src);
		while (*src && dstlen > 0)
		{
			if (STRNCMP(src, home, len) == 0)
			{
				src += len;
				if (--dstlen > 0)
					*dst++ = '~';
			}
			while (*src && *src != ' ' && --dstlen > 0)
				*dst++ = *src++;
			while (*src == ' ' && --dstlen > 0)
				*dst++ = *src++;
		}
		*dst = NUL;
	}
}

/*
 * Compare two file names and return TRUE if they are different files.
 * For the first name environment variables are expanded
 */
	int
fullpathcmp(s1, s2)
	char_u *s1, *s2;
{
#ifdef UNIX
	struct stat st1, st2;
	char_u buf1[MAXPATHL];

	expand_env(s1, buf1, MAXPATHL);
	if (stat((char *)buf1, &st1) == 0 && stat((char *)s2, &st2) == 0 &&
				st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
		return FALSE;
	return TRUE;
#else
	char_u buf1[MAXPATHL];
	char_u buf2[MAXPATHL];

	expand_env(s1, buf2, MAXPATHL);
	if (FullName(buf2, buf1, MAXPATHL) == OK && FullName(s2, buf2, MAXPATHL) == OK)
		return STRCMP(buf1, buf2);
	/*
	 * one of the FullNames() failed, file probably doesn't exist.
	 */
	return TRUE;
#endif
}

/*
 * get the tail of a path: the file name.
 */
	char_u *
gettail(fname)
	char_u *fname;
{
	register char_u *p1, *p2;

	for (p1 = p2 = fname; *p2; ++p2)	/* find last part of path */
	{
		if (ispathsep(*p2))
			p1 = p2 + 1;
	}
	return p1;
}

/*
 * return TRUE if 'c' is a path separator.
 */
	int
ispathsep(c)
	int c;
{
#ifdef UNIX
	return (c == PATHSEP);		/* UNIX has ':' inside file names */
#else
# ifdef MSDOS
	return (c == ':' || c == PATHSEP || c == '/');
# else
	return (c == ':' || c == PATHSEP);
# endif
#endif
}
