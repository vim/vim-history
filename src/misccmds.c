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
			count += (int)p_ts - (count % (int)p_ts);
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
	if (!p_et)			/* if 'expandtab' is set, don't use TABs */
		while (size >= (int)p_ts)
		{
			inschar(TAB);
			size -= (int)p_ts;
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
Opencmd(dir, redraw, delspaces)
	int 		dir;
	int			redraw;
	int			delspaces;
{
	char   *l;
	char   *ptr, *pp;
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
		if (newindent == 0)
			newindent = old_indent;		/* for ^^D command in insert mode */
		old_indent = 0;

			/*
			 * If we just did an auto-indent, then we didn't type anything on the
			 * prior line, and it should be truncated.
			 */
		if (dir == FORWARD && did_ai)
			truncate = TRUE;
		else if (p_si && *ptr != NUL)
		{
			char	*p;
			char	*pp;
			int		i, save;

			if (dir == FORWARD)
			{
				p = ptr + strlen(ptr) - 1;
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
				p = ptr;
				skipspace(&p);
				if (*p == '}')			/* if line starts with '}': do indent */
					did_si = TRUE;
			}
		}
		did_ai = TRUE;
		if (p_si)
			can_si = TRUE;
	}
	if (State == INSERT || State == REPLACE)	/* only when dir == FORWARD */
	{
		pp = ptr + Curpos.col;
		if (p_ai && delspaces)
			skipspace(&pp);
		extra = strlen(pp);
	}
	if ((l = alloc_line(extra)) == NULL)
		return (FALSE);
	if (extra)
	{
		strcpy(l, pp);
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
				newindent -= newindent % (int)p_sw;
			newindent += (int)p_sw;
		}
		set_indent(newindent, FALSE);
		newcol = Curpos.col;
		if (no_si)
			did_si = FALSE;
	}
	Curpos = oldCurpos;

	if (dir == FORWARD)
	{
		if (truncate || State == INSERT || State == REPLACE)
		{
			if (truncate)
				*ptr = NUL;
			else
				*(ptr + Curpos.col) = NUL;	/* truncate current line at cursor */
			canincrease(0);
		}

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
		cursupdate();			/* update Cursrow */
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
	register int		col = 0;
	register u_char		*s;

	if (!p_wrap)
		return 1;

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

	col = (col + ((int)Columns - 1)) / (int)Columns;
	if (col < Rows)
		return col;
	return (int)(Rows - 1);		/* maximum length */
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
fileinfo(fullname)
	int fullname;
{
	if (bufempty())
	{
		msg("Buffer Empty");
		return;
	}
	sprintf(IObuff, "\"%s\"%s%s%s line %ld of %ld -- %d %% --",
			(!fullname && sFilename != NULL) ? sFilename :
				((Filename != NULL) ? Filename : "No File"),
			Changed ? " [Modified]" : "",
			NotEdited ? " [Not edited]" : "",
			p_ro ? " [readonly]" : "",
			(long)Curpos.lnum,
			(long)line_count,
			(int)(((long)Curpos.lnum * 100L) / (long)line_count));

	if (numfiles > 1)
		sprintf(IObuff + strlen(IObuff), " (file %d of %d)", curfile + 1, numfiles);
	msg(IObuff);
}

/*
 * Set the current file name to 's'.
 * The file name with the full path is also remembered, for when :cd is used.
 */
	void
setfname(s, ss)
	char *s, *ss;
{
	free(Filename);
	free(sFilename);
	if (s == NULL || *s == NUL)
	{
		Filename = NULL;
		sFilename = NULL;
	}
	else
	{
		if (ss == NULL)
			ss = s;
		sFilename = (char *)strsave(ss);
		FullName(s, IObuff, IOSIZE);
		Filename = (char *)strsave(IObuff);
	}
	if (did_cd)
		xFilename = Filename;
	else
		xFilename = sFilename;

#ifndef MSDOS
	thisfile_sn = FALSE;
#endif
}

/*
 * return nonzero if "s" is not the same file as current file
 */
	int
otherfile(s)
	char *s;
{
	if (s == NULL || *s == NUL || Filename == NULL)		/* no name is different */
		return TRUE;
	FullName(s, IObuff, IOSIZE);
	return fnamecmp(IObuff, Filename);
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
		if (numfiles <= 1)
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
	int				rir0;		/* reverse replace in column 0 */

	p = Curpos2ptr();
	rir0 = (State == REPLACE && p_ri && Curpos.col == 0);
	if (rir0 || State != REPLACE || *p == NUL)
	{
			/* make room for the new char. */
		if (!canincrease(1))	/* make room for the new char */
			return;

		p = Curpos2ptr();		/* get p again, canincrease() may have changed it */
		memmove(p + 1, p, strlen(p) + 1);	/* move following text and NUL */
	}
	if (rir0)					/* reverse replace in column 0 */
	{
		*(p + 1) = c;			/* replace the char that was in column 0 */
		c = ' ';				/* insert a space */
		extraspace = TRUE;
	}
	*p = c;

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
			showruler(0);
			setcursor();
			flushbuf();
			vim_delay();		/* brief pause */
			Curpos = csave; 	/* restore cursor position */
			cursupdate();
		}
	}
	if (!p_ri)							/* normal insert: cursor right */
		++Curpos.col;
	else if (State == REPLACE && !rir0)	/* reverse replace mode: cursor left */
		--Curpos.col;
	CHANGED;
}

	void
insstr(s)
	register char  *s;
{
	register char  *p;
	register int	n = strlen(s);

	if (!canincrease(n))	/* make room for the new string */
		return;

	p = Curpos2ptr();
	memmove(p + n, p, strlen(p) + 1);
	memmove(p, s, (size_t)n);
	Curpos.col += n;
	CHANGED;
}

	int
delchar(fixpos)
	int			fixpos; 	/* if TRUE fix the cursor position when done */
{
	char		*ptr;
	int			lastchar;

	ptr = Curpos2ptr();

	if (*ptr == NUL)	/* can't do anything (happens with replace mode) */
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
	if (fixpos && Curpos.col > 0 && lastchar)
		--Curpos.col;

	(void)canincrease(0);
	CHANGED;
	return TRUE;
}

	void
dellines(nlines, doscreen, undo)
	long 			nlines;			/* number of lines to delete */
	int 			doscreen;		/* if true, update the screen */
	int				undo;			/* if true, prepare for undo */
{
	int 			num_plines = 0;
	char			*ptr;

	if (nlines <= 0)
		return;
	/*
	 * There's no point in keeping the screen updated if we're deleting more
	 * than a screen's worth of lines.
	 */
	if (nlines > (Rows - 1 - Cursrow) && doscreen)
	{
		doscreen = FALSE;
		/* flaky way to clear rest of screen */
		s_del(Cursrow, (int)Rows - 1, TRUE);
	}
	if (undo && !u_savedel(Curpos.lnum, nlines))
		return;
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

		ptr = delsline(Curpos.lnum, TRUE);
		if (!undo)
			free_line(ptr);

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
	return (int)(*(Curpos2ptr()));
}

	void
pcharCurpos(c)
	int c;
{
	*(Curpos2ptr()) = c;
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
 * skiptodigit: skip over text until digit found
 *
 * note: you must give a pointer to a char pointer!
 */
	void
skiptodigit(pp)
	char **pp;
{
	register char *p;

	for (p = *pp; !isdigit(*p) && *p != NUL; ++p)
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
 * Don't use emsg(), because it flushes the macro buffer.
 */
	void
set_Changed()
{
	change_warning();
	Changed = 1;
	Updated = 1;
}

	void
change_warning()
{
	if (Changed == 0 && p_ro)
	{
		msg("Warning: Changing a readonly file");
		sleep(1);			/* give him some time to think about it */
	}
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
		if (r == Ctrl('C'))
			r = 'n';
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
		smsg("%ld %s line%s %s", pn, n > 0 ? "more" : "fewer", plural(pn),
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
	        msg("    ^G");
	        msg("     ^G");
	        msg("    ^G ");
	        msg("     ^G");
	        msg("       ");
			showmode();			/* may have deleted the mode message */
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
/*
 * The variable name is copied into dst temporarily, because it may be
 * a string in read-only memory.
 */
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
		var = (char *)vimgetenv(dst);
		if (var && (strlen(var) + strlen(tail) + 1 < (unsigned)dstlen))
		{
			strcpy(dst, var);
			strcat(dst, tail);
			return;
		}
	}
	strncpy(dst, src, (size_t)dstlen);
}

/*
 * Compare two file names and return TRUE if they are different files.
 * For the first name environment variables are expanded
 */
	int
fullpathcmp(s1, s2)
	char *s1, *s2;
{
#ifdef UNIX
	struct stat st1, st2;
	char buf1[MAXPATHL];

	expand_env(s1, buf1, MAXPATHL);
	if (stat(buf1, &st1) == 0 && stat(s2, &st2) == 0 &&
				st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
		return FALSE;
	return TRUE;
#else
	char buf1[MAXPATHL];
	char buf2[MAXPATHL];

	expand_env(s1, buf2, MAXPATHL);
	if (FullName(buf2, buf1, MAXPATHL) && FullName(s2, buf2, MAXPATHL))
		return strcmp(buf1, buf2);
	/*
	 * one of the FullNames() failed, file probably doesn't exist.
	 */
	return TRUE;
#endif
}

/*
 * get the tail of a path: the file name.
 */
	char *
gettail(fname)
	char *fname;
{
	register char *p1, *p2;

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
