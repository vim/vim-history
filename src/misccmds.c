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
 * misccmds.c: functions that didn't seem to fit elsewhere
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static char *(si_tab[]) = {"if", "else", "while", "for", "do"};

/*
 * count the size of the indent in the current line
 */
	int
get_indent()
{
	register char *ptr;
	register int count = 0;

	for (ptr = nr2ptr(Curpos.lnum); *ptr; ++ptr)
	{
		if (*ptr == TAB)	/* count a tab for what it is worth */
			count += p_ts - (count % p_ts);
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
	int		oldstate = State;

	State = INSERT;		/* don't want REPLACE for State */
	Curpos.col = 0;
	if (delete)
	{
		while (isspace(gcharCurpos()))	/* delete old indent */
			delchar(FALSE);
	}
	while (size >= p_ts)
	{
		inschar(TAB);
		size -= p_ts;
	}
	while (size)
	{
		inschar(' ');
		--size;
	}
	State = oldstate;
	script_winsize_pp();
}

/*
 * Opencmd
 *
 * Add a blank line below or above the current line.
 */

	int
Opencmd(dir, redraw)
	int 		dir;
	int			redraw;
{
	char   *l;
	char   *ptr;
	FPOS	oldCurpos; 			/* old cursor position */
	int		newcol = 0;			/* new cursor column */
	int 	newindent = 0;		/* auto-indent of the new line */
	int 	extra = 0;			/* number of bytes to be copied from current line */
	int		n;
	int		truncate = FALSE;	/* truncate current line afterwards */
	int		no_si = FALSE;		/* reset did_si afterwards */

	ptr = nr2ptr(Curpos.lnum);
	u_clearline();		/* cannot do "U" command when adding lines */
	did_si = FALSE;
	if (p_ai || p_si)
	{
		/*
		 * count white space on current line
		 */
		newindent = get_indent();

				/*
				 * If we just did an auto-indent, then we didn't type anything on the
				 * prior line, and it should be truncated.
				 */
		if (dir == FORWARD && did_ai)
				truncate = TRUE;
		else if (p_si)
		{
			char	*p;
			char	*pp;
			int		i, save;

			p = ptr;
			skipspace(&p);
			if (dir == FORWARD)
			{
				if (*p == '{')
				{
					did_si = TRUE;
					no_si = TRUE;
				}
				else
				{
					for (pp = p; islower(*pp); ++pp) ;
					if (!isidchar(*pp))
					{
						save = *pp;
						*pp = NUL;
						for (i = sizeof(si_tab)/sizeof(char *); --i >= 0; )
							if (strcmp(p, si_tab[i]) == 0)
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
				if (*p == '}')
					did_si = TRUE;
			}
		}
		did_ai = TRUE;
		if (p_si)
			can_si = TRUE;
	}
	if (State == INSERT || State == REPLACE)	/* only when dir == FORWARD */
		extra = strlen(ptr + Curpos.col);
	if ((l = alloc_line(extra)) == NULL)
		return (FALSE);
	if (extra)
	{
		strcpy(l, ptr + Curpos.col);
		did_ai = FALSE; 		/* don't trucate now */
	}

	oldCurpos = Curpos;
	if (dir == BACKWARD)
		--Curpos.lnum;
	if (appendline(Curpos.lnum, l) == FALSE)
		return FALSE;
	if (newindent || did_si)
	{
		++Curpos.lnum;
		if (did_si)
		{
			if (p_sr)
				newindent -= newindent % p_sw;
			newindent += p_sw;
		}
		set_indent(newindent, FALSE);
		newcol = Curpos.col;
		if (no_si)
			did_si = FALSE;
	}
	Curpos = oldCurpos;

	if (dir == FORWARD)
	{
		if (truncate)
			*ptr = NUL;
		else if (extra)
		{
			truncate = TRUE;
			*(ptr + Curpos.col) = NUL;		/* truncate current line at cursor */
		}
		if (truncate)
			canincrease(0);

		/*
		 * Get the cursor to the start of the line, so that 'Cursrow' gets
		 * set to the right physical line number for the stuff that
		 * follows...
		 */
		Curpos.col = 0;

		if (redraw)
		{
			cursupdate();

			/*
			 * If we're doing an open on the last logical line, then go ahead and
			 * scroll the screen up. Otherwise, just insert a blank line at the
			 * right place. We use calls to plines() in case the cursor is
			 * resting on a long line.
			 */
			n = Cursrow + plines(Curpos.lnum);
			if (n == (Rows - 1))
				scrollup(1L);
			else
				s_ins(n, 1, TRUE);
		}
		++Curpos.lnum;	/* cursor moves down */
	}
	else if (redraw)
		s_ins(Cursrow, 1, TRUE); /* insert physical line */

	Curpos.col = newcol;
	if (redraw)
	{
		updateScreen(VALID_TO_CURSCHAR);
		cursupdate();		/* update Cursrow */
	}
	CHANGED;

	return (TRUE);
}

/*
 * plines(p) - return the number of physical screen lines taken by line 'p'
 */
	int
plines(p)
	linenr_t p;
{
	register int	col = 0;
	register u_char  *s;

#ifdef DEBUG
	if (p == 0)
	{
		emsg("plines(0) ????");
		return (0);
	}
#endif
	s = (u_char *)nr2ptr(p);

	if (*s == NUL)				/* empty line */
		return 1;

	while (*s != NUL)
		col += chartabsize(*s++, col);

	/*
	 * If list mode is on, then the '$' at the end of the line takes up one
	 * extra column.
	 */
	if (p_list)
		col += 1;

	/*
	 * If 'number' mode is on, add another 8.
	 */
	if (p_nu)
		col += 8;

	return ((col + ((int)Columns - 1)) / (int)Columns);
}

/*
 * Count the physical lines (rows) for the lines "first" to "last" inclusive.
 */
	int
plines_m(first, last)
	linenr_t		first, last;
{
		int count = 0;

		while (first <= last)
				count += plines(first++);
		return (count);
}

	void
fileinfo()
{
	if (bufempty())
	{
		msg("Buffer Empty");
		return;
	}
	sprintf(IObuff, "\"%s\"%s line %ld of %ld -- %d %% --",
			(Filename != NULL) ? Filename : "No File",
			Changed ? " [Modified]" : "",
			(long)Curpos.lnum,
			(long)line_count,
			(int)(((long)Curpos.lnum * 100L) / (long)line_count));

	if (numfiles > 1)
		sprintf(IObuff + strlen(IObuff), " (file %d of %d)", curfile + 1, numfiles);
	msg(IObuff);
}

	void
setfname(s)
	char *s;
{
		if (Filename != NULL)
				free(Filename);
		if (s == NULL || *s == NUL)
				Filename = NULL;
		else
		{
				FullName(s, IObuff, IOSIZE);
				Filename = (char *)strsave(IObuff);
		}
}

/*
 * put filename in title bar of window
 */
	void
maketitle()
{
#ifdef AMIGA
		if (Filename == NULL)
				settitle("");
		else
		{
			if (numfiles == 1)
				settitle(Filename);
			else
			{
					sprintf(IObuff, "%s (%d of %d)", Filename, curfile + 1, numfiles);
					settitle(IObuff);
			}
		}
#endif
}

	void
inschar(c)
	int			c;
{
	register char  *p;
	register char  *pend;

	p = nr2ptr(Curpos.lnum);
	pend = p + Curpos.col;
	if (State != REPLACE || *pend == NUL)
	{
			/* make room for the new char. */
			if (!canincrease(1))
				return;

			p = nr2ptr(Curpos.lnum);
			pend = p + Curpos.col;
			p += strlen(p) + 1;

			for (; p > pend; p--)
				*p = *(p - 1);
	}
	*pend = c;

	/*
	 * If we're in insert mode and showmatch mode is set, then check for
	 * right parens and braces. If there isn't a match, then beep. If there
	 * is a match AND it's on the screen, then flash to it briefly. If it
	 * isn't on the screen, don't do anything.
	 */
	if (p_sm && State == INSERT && (c == ')' || c == '}' || c == ']'))
	{
		FPOS		   *lpos, csave;

		if ((lpos = showmatch()) == NULL)		/* no match, so beep */
			beep();
		else if (lpos->lnum >= Topline)
		{
			updateScreen(VALID_TO_CURSCHAR); /* show the new char first */
			csave = Curpos;
			Curpos = *lpos; 	/* move to matching char */
			cursupdate();
			setcursor();
			flushbuf();
			vim_delay();		/* brief pause */
			Curpos = csave; 	/* restore cursor position */
			cursupdate();
		}
	}
	++Curpos.col;

	CHANGED;
}

	void
insstr(s)
	register char  *s;
{
	register char  *p;
	register char  *pend;
	register int	n = strlen(s);

	/* Move everything in the file over to make */
	/* room for the new string. */
	if (!canincrease(n))
		return;

	p = nr2ptr(Curpos.lnum);
	pend = p + Curpos.col;
	p += strlen(p) + n;

	for (; p > pend; p--)
		*p = *(p - n);

	Curpos.col += n;
	while (--n >= 0)
		*p++ = *s++;

	CHANGED;
}

	int
delchar(fixpos)
	int			fixpos; 	/* if TRUE fix the cursor position when done */
{
	char		*ptr;
	int			lastchar;

	ptr = pos2ptr(&Curpos);

	if (*ptr == NUL)	/* can't do anything */
		return FALSE;

	lastchar = (*++ptr == NUL);
	/* Delete the char. at Curpos by shifting everything in the line down. */
	do
		*(ptr - 1) = *ptr;
	while (*ptr++);

	/*
	 * If we just took off the last character of a non-blank line, we don't
	 * want to end up positioned at the newline.
	 */
	if (fixpos && Curpos.col > 0 && lastchar && State != INSERT)
			--Curpos.col;

	(void)canincrease(0);
	CHANGED;
	return TRUE;
}

	void
dellines(nlines, can_update)
	long 			nlines;
	int				can_update;
{
	int 			doscreen;	/* if true, update the screen */
	int 			num_plines = 0;

	doscreen = can_update;
	/*
	 * There's no point in keeping the screen updated if we're deleting more
	 * than a screen's worth of lines.
	 */
	if (nlines > (Rows - 1) && can_update)
	{
		doscreen = FALSE;
		/* flaky way to clear rest of screen */
		s_del(Cursrow, (int)Rows - 1, TRUE);
	}
	while (nlines-- > 0)
	{
		if (bufempty()) 		/* nothing to delete */
			break;

		/*
		 * Set up to delete the correct number of physical lines on the
		 * screen
		 */
		if (doscreen)
			num_plines += plines(Curpos.lnum);

		free_line(delsline(Curpos.lnum));

		CHANGED;

		/* If we delete the last line in the file, stop */
		if (Curpos.lnum > line_count)
		{
			Curpos.lnum = line_count;
			break;
		}
	}
	Curpos.col = 0;
	/*
	 * Delete the correct number of physical lines on the screen
	 */
	if (doscreen && num_plines > 0)
		s_del(Cursrow, num_plines, TRUE);
}

	int
gchar(pos)
	FPOS *pos;
{
	return (int)(*(pos2ptr(pos)));
}

	int
gcharCurpos()
{
	return (int)(*(pos2ptr(&Curpos)));
}

/*
 * return TRUE if the cursor is before or on the first non-blank in the line
 */
	int
inindent()
{
	register char *ptr;
	register int col;

	for (col = 0, ptr = nr2ptr(Curpos.lnum); isspace(*ptr++); ++col)
		;
	if (col >= Curpos.col)
		return TRUE;
	else
		return FALSE;
}

/*
 * skipspace: skip over ' ' and '\t'.
 *
 * note: you must give a pointer to a char pointer!
 */
	void
skipspace(pp)
	char **pp;
{
    register char *p;
    
    for (p = *pp; *p == ' ' || *p == '\t'; ++p)	/* skip to next non-white */
    	;
    *pp = p;
}

/*
 * skiptospace: skip over text until ' ' or '\t'.
 *
 * note: you must give a pointer to a char pointer!
 */
	void
skiptospace(pp)
	char **pp;
{
	register char *p;

	for (p = *pp; *p != ' ' && *p != '\t' && *p != NUL; ++p)
		;
	*pp = p;
}

/*
 * getdigits: get a number from a string and skip over it
 *
 * note: you must give a pointer to a char pointer!
 */

	long
getdigits(pp)
	char **pp;
{
    register char *p;
	long retval;
    
	p = *pp;
	retval = atol(p);
    while (isdigit(*p))	/* skip to next non-digit */
    	++p;
    *pp = p;
	return retval;
}

	char *
plural(n)
	long n;
{
	static char buf[2] = "s";

	if (n == 1)
		return &(buf[1]);
	return &(buf[0]);
}

/*
 * set_Changed is called whenever something in the file is changed
 * If the file is readonly, give a warning message with the first change.
 */
	void
set_Changed()
{
	if (Changed == 0 && p_ro)
	{
		emsg("Warning: Changing a readonly file");
		sleep(2);		/* give the user some time to think about it */
	}
	Changed = 1;
	Updated = 1;
}

	int
ask_yesno(str)
	char *str;
{
	int r = ' ';

	while (r != 'y' && r != 'n')
	{
		smsg("%s (y/n)? ", str);
		r = vgetc();
		outchar(r);		/* show what you typed */
		flushbuf();
	}
	return r;
}

	void
msgmore(n)
	long n;
{
	long pn;

	if (n > 0)
		pn = n;
	else
		pn = -n;

	if (pn > p_report)
		smsg("%ld %s lines %s", pn, n > 0 ? "more" : "fewer", got_int ? "(Interrupted)" : "");
}

/*
 * give a warning for an error
 */
	void
beep()
{
	flush_buffers();
	if (p_vb)
	{
		if (T_VB && *T_VB)
		    outstr(T_VB);
		else
		{			/* very primitive visual bell */
	        msg("    ^G");
	        msg("     ^G");
	        msg("    ^G ");
	        msg("     ^G");
	        msg("       ");
		}
	}
	else
	    outchar('\007');
}

/* 
 * Expand environment variable with path name.
 * If anything fails no expansion is done and dst equals src.
 */
	void
expand_env(src, dst, dstlen)
	char	*src;			/* input string e.g. "$HOME/vim.hlp" */
	char	*dst;			/* where to put the result */
	int		dstlen;			/* maximum length of the result */
{
	char	*tail;
	int		c;
	char	*var;

	if (*src == '$')
	{
		for (tail = src + 1; *tail; ++tail)
			if (*tail == PATHSEP)
				break;
		c = *tail;
		*tail = NUL;
		var = getenv(src + 1);
		*tail = c;
		if (*tail)
			++tail;
		if (var && strlen(var) + strlen(tail) + 1 < dstlen)
		{
			strcpy(dst, var);
			strcat(dst, PATHSEPSTR);
			strcat(dst, tail);
			return;
		}
	}
	strncpy(dst, src, (size_t)dstlen);
}
