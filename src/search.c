/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony
 *							G. R. (Fred) Walter     watmath!watcgl!grwalter
 */
/*
 * search.c: code for normal mode searching commands
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "ops.h"		/* for mincl */

/* modified Henry Spencer's regular expression routines */
#include "regexp.h"

static int inmacro __ARGS((char *, char *));
static int cls __ARGS((void));

/*
 * This file contains various searching-related routines. These fall into
 * three groups:
 * 1. string searches (for /, ?, n, and N)
 * 2. character searches within a single line (for f, F, t, T, etc)
 * 3. "other" kinds of searches like the '%' command, and 'word' searches.
 */

/*
 * String searches
 *
 * The string search functions are divided into two levels:
 * lowest:	searchit(); called by dosearch() and docmdline().
 * Highest: dosearch(); changes Curpos, called by normal().
 *
 * The last search pattern is remembered for repeating the same search.
 * This pattern is shared between the :g, :s, ? and / commands.
 * This is in myregcomp().
 *
 * The actual string matching is done using a heavily modified version of
 * Henry Spencer's regular expression library.
 */

static char 	*search_pattern = NULL;  /* previous search pattern */
static int		want_start; 			 /* looking for start of line? */

/*
 * translate search pattern for regcomp()
 */
	regexp *
myregcomp(pat)
	char *pat;
{
	regexp *retval;

	if (pat == NULL || *pat == NUL)     /* use previous search pattern */
	{
		if (search_pattern == NULL)
		{
			emsg(e_noprevre);
			return (regexp *) NULL;
		}
		pat = search_pattern;
	}
	else
	{
		if (search_pattern != NULL)
			free(search_pattern);
		search_pattern = strsave(pat);
		reg_magic = p_magic;		/* Magic sticks with the r.e. */
	}
	want_start = (*pat == '^');		/* looking for start of line? */
	reg_ic = p_ic;				/* tell the regexec routine how to search */
	retval = regcomp(pat);
	return retval;
}

/*
 * lowest level search function.
 * Search for 'count'th occurrence of 'str' in direction 'dir'.
 * Start at position 'pos' and return the found position in 'pos'.
 * Return 1 for success, 0 for failure.
 */
	int
searchit(pos, dir, str, count, end)
	FPOS	*pos;
	int 	dir;
	char	*str;
	long	count;
	int		end;
{
	int 			found;
	linenr_t		lnum;
	linenr_t		startlnum;
	regexp		   *prog;
	register char  *s;
	char		   *ptr;
	register int	i;
	register char  *match, *matchend;
	int 			loop;

	if ((prog = myregcomp(str)) == NULL)
	{
		emsg(e_invstring);
		return 0;
	}
/*
 * find the string
 */
	found = 1;
	while (count-- && found)    /* stop after count matches, or no more matches */
	{
		startlnum = pos->lnum;	/* remember start of search for detecting no match */
		found = 0;				/* default: not found */

		i = pos->col + dir; 	/* search starts one postition away */
		lnum = pos->lnum;

		if (dir == BACKWARD)
		{
			if (i < 0)
				--lnum;
		}

		for (loop = 0; loop != 2; ++loop)   /* do this twice if 'wrapscan' is set */
		{
			for ( ; lnum > 0 && lnum <= line_count; lnum += dir, i = -1)
			{
				s = ptr = nr2ptr(lnum);
				if (dir == FORWARD && i > 0)    /* first line for forward search */
				{
					if (want_start || strlen(s) <= (size_t)i)   /* match not possible */
						continue;
					s += i;
				}

				if (regexec(prog, s, dir == BACKWARD || i <= 0))
				{							/* match somewhere on line */
					match = prog->startp[0];
					matchend = prog->endp[0];
					if (dir == BACKWARD && !want_start)
					{
						/*
						 * Now, if there are multiple matches on this line, we have to
						 * get the last one. Or the last one before the cursor, if we're
						 * on that line.
						 */
						while (regexec(prog, prog->startp[0] + 1, (int)FALSE))
						{
							if ((i >= 0) && ((prog->startp[0] - s) > i))
								break;
							match = prog->startp[0];
							matchend = prog->endp[0];
						}

						if ((i >= 0) && ((match - s) > i))
							continue;
					}

					pos->lnum = lnum;
					if (end)
						pos->col = (int) (matchend - ptr - 1);
					else
						pos->col = (int) (match - ptr);
					found = 1;
					break;
				}
				/* breakcheck is slow, do it only once in 16 lines */
				if ((lnum & 15) == 0)
					breakcheck();       /* stop if ctrl-C typed */
				if (got_int)
					break;

				if (loop && lnum == startlnum)  /* if second loop stop where started */
					break;
			}
	/* stop the search if wrapscan isn't set, after an interrupt and after a match */
			if (!p_ws || got_int || found)
				break;

			if (dir == BACKWARD)    /* start second loop at the other end */
				lnum = line_count;
			else
				lnum = 1;
		}
		if (got_int)
			break;
	}

	free((char *) prog);

	if (!found)             /* did not find it */
	{
		if (got_int)
				emsg(e_interr);
		else
				emsg(e_patnotf);
		return 0;
	}

	return 1;
}

/*
 * Highest level string search function.
 * Search for the 'count'th occurence of string 'str' in direction 'dirc'
 *					If 'dirc' is 0: use previous dir.
 * If 'str' is 0 or 'str' is empty: use previous string.
 *			  If 'reverse' is TRUE: go in reverse of previous dir.
 *				 If 'echo' is TRUE: echo the search command
 */
	int
dosearch(dirc, str, reverse, count, echo)
	int				dirc;
	char		   *str;
	int				reverse;
	long			count;
	int				echo;
{
	FPOS			pos;		/* position of the last match */
	char			*searchstr;
	static int		lastsdir = '/';	/* previous search direction */
	static int		lastoffline;/* previous/current search has line offset */
	static int		lastend;	/* previous/current search set cursor at end */
	static long 	lastoff;	/* previous/current line or char offset */
	static int		nosetpm;    /* do not call setpcmark() */
	register char	*p;
	register long	c;
	char			*dircp = NULL;

	if (dirc == 0)
		dirc = lastsdir;
	else
		lastsdir = dirc;
	if (reverse)
	{
		if (dirc == '/')
			dirc = '?';
		else
			dirc = '/';
	}
	searchstr = str;
									/* use previous string */
	if (str == NULL || *str == NUL || *str == dirc)
	{
		if (search_pattern == NULL)
		{
			emsg(e_noprevre);
			return 0;
		}
		searchstr = "";				/* will use search_pattern in myregcomp() */
	}
	if (str != NULL && *str != NUL)	/* look for (new) offset */
	{
		/* If there is a matching '/' or '?', toss it */
		for (p = str; *p; ++p)
		{
			if (*p == dirc)
			{
				dircp = p;		/* remember where we put the NUL */
				*p++ = NUL;
				break;
			}
			if (*p == '\\' && p[1] != NUL)
				++p;	/* skip next character */
		}

		lastoffline = FALSE;
		lastend = FALSE;
		nosetpm = FALSE;
		lastoff = 0;
		switch (*p)
		{
			case 'n': 					/* do not call setpcmark() */
						nosetpm = TRUE;
						++p;
						break;
			case '+':
			case '-':                   /* got a line offset */
						lastoffline = TRUE;
						break;
			case 'e':                   /* position cursor at end */
						lastend = TRUE;
			case 's':                   /* got a character offset from start */
						++p;
		}
		if (*p == '+' || *p == '-')     /* got an offset */
		{
			if (isdigit(*(p + 1)))
				lastoff = atol(p);		/* '+nr' or '-nr' */
			else if (*p == '-')			/* single '-' */
				lastoff = -1;
			else						/* single '+' */
				lastoff = 1;
			++p;
			while (isdigit(*p))			/* skip number */
				++p;
		}
		searchcmdlen = p - str;			/* compute lenght of search command
														for get_address() */
	}

	if (echo)
	{
		start_msg();
		outchar(dirc);
		outtrans(*searchstr == NUL ? search_pattern : searchstr, -1);
		if (lastoffline || lastend || lastoff || nosetpm)
		{
			outchar(dirc);
			if (nosetpm)
				outchar('n');
			else if (lastend)
				outchar('e');
			else if (!lastoffline)
				outchar('s');
			if (lastoff < 0)
			{
				outchar('-');
				outnum((long)-lastoff);
			}
			else if (lastoff > 0 || lastoffline)
			{
				outchar('+');
				outnum((long)lastoff);
			}
		}
		check_msg();

		gotocmdline(FALSE, NUL);
		flushbuf();
	}

	pos = Curpos;

	c = searchit(&pos, dirc == '/' ? FORWARD : BACKWARD, searchstr, count, lastend);
	if (dircp)
		*dircp = dirc;		/* put second '/' or '?' back for normal() */
	if (!c)
		return 0;

	if (!lastoffline)           /* add the character offset to the column */
	{
		if (lastoff > 0)        /* offset to the right, check for end of line */
		{
			p = pos2ptr(&pos) + 1;
			c = lastoff;
			while (c-- && *p++ != NUL)
				++pos.col;
		}
		else					/* offset to the left, check for start of line */
		{
			if ((c = pos.col + lastoff) < 0)
				c = 0;
			pos.col = c;
		}
	}

	if (!nosetpm)
		setpcmark();
	Curpos = pos;
	set_want_col = TRUE;

	if (!lastoffline)
		return 1;

/*
 * add the offset to the line number.
 */
	c = Curpos.lnum + lastoff;
	if (c < 1)
		Curpos.lnum = 1;
	else if (c > line_count)
		Curpos.lnum = line_count;
	else
		Curpos.lnum = c;
	Curpos.col = 0;

	return 2;
}


/*
 * Character Searches
 */

/*
 * searchc(c, dir, type, count)
 *
 * Search for character 'c', in direction 'dir'. If 'type' is 0, move to the
 * position of the character, otherwise move to just before the char.
 * Repeat this 'count' times.
 */
	int
searchc(c, dir, type, count)
	int 			c;
	register int	dir;
	int 			type;
	long			count;
{
	static char 	lastc = NUL;	/* last character searched for */
	static int		lastcdir;		/* last direction of character search */
	static int		lastctype;		/* last type of search ("find" or "to") */
	register int	col;
	char			*p;
	int 			len;

	if (c != NUL)       /* normal search: remember args for repeat */
	{
		lastc = c;
		lastcdir = dir;
		lastctype = type;
	}
	else				/* repeat previous search */
	{
		if (lastc == NUL)
			return FALSE;
		if (dir)        /* repeat in opposite direction */
			dir = -lastcdir;
		else
			dir = lastcdir;
	}

	p = nr2ptr(Curpos.lnum);
	col = Curpos.col;
	len = strlen(p);

	/*
	 * On 'to' searches, skip one to start with so we can repeat searches in
	 * the same direction and have it work right.
	 * REMOVED to get vi compatibility
	 * if (lastctype)
	 *	col += dir;
	 */

	while (count--)
	{
			for (;;)
			{
				if ((col += dir) < 0 || col >= len)
					return FALSE;
				if (p[col] == lastc)
						break;
			}
	}
	if (lastctype)
		col -= dir;
	Curpos.col = col;
	return TRUE;
}

/*
 * "Other" Searches
 */

/*
 * showmatch - move the cursor to the matching paren or brace
 *
 * Improvement over vi: Braces inside quotes are ignored.
 */
	FPOS		   *
showmatch()
{
	static FPOS		pos;			/* current search position */
	char			initc;			/* brace under or after the cursor */
	char			findc;			/* matching brace */
	char			c;
	int 			count = 0;		/* cumulative number of braces */
	int 			idx;
	static char 	table[6] = {'(', ')', '[', ']', '{', '}'};
	int 			inquote = 0;	/* non-zero when inside quotes */
	register char	*linep;			/* pointer to current line */
	register char	*ptr;
	int				do_quotes;		/* check for quotes in current line */

	pos = Curpos;

	/*
	 * find the brace under or after the cursor
	 */
	linep = nr2ptr(pos.lnum); 
	for (;;)
	{
		initc = linep[pos.col];
		if (initc == NUL)
			return (FPOS *) NULL;

		for (idx = 0; idx < 6; ++idx)
			if (table[idx] == initc)
				break;
		if (idx != 6)
			break;
		++pos.col;
	}

	findc = table[idx ^ 1];		/* get matching brace */
	idx &= 1;

	do_quotes = -1;
	while (!got_int)
	{
		/*
		 * Go to the next position, forward or backward. We could use
		 * inc() and dec() here, but that is much slower
		 */
		if (idx)              			/* backward search */
		{
			if (pos.col == 0)   		/* at start of line, go to previous one */
			{
				if (pos.lnum == 1)      /* start of file */
					break;
				--pos.lnum;
				linep = nr2ptr(pos.lnum);
				pos.col = strlen(linep);	/* put pos.col on trailing NUL */
				do_quotes = -1;
			}
			else
				--pos.col;
		}
		else							/* forward search */
		{
			if (linep[pos.col] == NUL)  /* at end of line, go to next one */
			{
				if (pos.lnum == line_count) /* end of file */
					break;
				++pos.lnum;
				linep = nr2ptr(pos.lnum);
				pos.col = 0;
				do_quotes = -1;
			}
			else
				++pos.col;
		}

		if (do_quotes == -1)		/* count number of quotes in this line */
		{
				/* we only do a breakcheck() once for every 16 lines */
			if ((pos.lnum & 15) == 0)
				breakcheck();

			/*
			 * count the number of quotes in the line, skipping \" and '"'
			 */
			for (ptr = linep; *ptr; ++ptr)
				if (*ptr == '"' && (ptr == linep || ptr[-1] != '\\') &&
							(ptr == linep || ptr[-1] != '\'' || ptr[1] != '\''))
					++do_quotes;
			do_quotes &= 1;			/* result is 1 with even number of quotes */

			/*
			 * If we find an uneven count, check current line and previous
			 * one for a '\' at the end.
			 */
			if (!do_quotes)
			{
				inquote = FALSE;
				if (ptr[-1] == '\\')
				{
					do_quotes = 1;
					if (idx)					/* backward search */
						inquote = TRUE;
				}
				if (pos.lnum > 1)
				{
					ptr = nr2ptr(pos.lnum - 1);
					if (*ptr && *(ptr + strlen(ptr) - 1) == '\\')
					{
						do_quotes = 1;
						if (!idx)				/* forward search */
							inquote = TRUE;
					}
				}
			}
		}

		/*
		 * Things inside quotes are ignored by setting 'inquote'.
		 * If we find a quote without a preceding '\' invert 'inquote'.
		 * At the end of a line not ending in '\' we reset 'inquote'.
		 *
		 * In lines with an uneven number of quotes (without preceding '\')
		 * we do not know which part to ignore. Therefore we only set
		 * inquote if the number of quotes in a line is even,
		 * unless this line or the previous one ends in a '\'.
		 * Complicated, isn't it?
		 */
		switch (c = linep[pos.col])
		{
		case NUL:
			inquote = FALSE;
			break;

		case '"':
				/* a quote that is preceded with a backslash is ignored */
			if (do_quotes && (pos.col == 0 || linep[pos.col - 1] != '\\'))
				inquote = !inquote;
			break;

		/*
		 * Skip things in single quotes: 'x' or '\x'.
		 * Be careful for single single quotes, eg jon's.
		 * Things like '\233' or '\x3f' are not skipped, there is never a
		 * brace in them.
		 */
		case '\'':
			if (idx)						/* backward search */
			{
				if (pos.col > 1)
				{
					if (linep[pos.col - 2] == '\'')
						pos.col -= 2;
					else if (linep[pos.col - 2] == '\\' && pos.col > 2 && linep[pos.col - 3] == '\'')
						pos.col -= 3;
				}
			}
			else if (linep[pos.col + 1])	/* forward search */
			{
				if (linep[pos.col + 1] == '\\' && linep[pos.col + 2] && linep[pos.col + 3] == '\'')
					pos.col += 3;
				else if (linep[pos.col + 2] == '\'')
					pos.col += 2;
			}
			break;

		default:
			if (!inquote)      /* only check for match outside of quotes */
			{
				if (c == initc)
					count++;
				else if (c == findc)
				{
					if (count == 0)
						return &pos;
					count--;
				}
			}
		}
	}
	return (FPOS *) NULL;       /* never found it */
}

/*
 * findfunc(dir, what) - Find the next line starting with 'what' in direction 'dir'
 *
 * Return TRUE if a line was found.
 */
	int
findfunc(dir, what, count)
	int 		dir;
	int			what;
	long		count;
{
	linenr_t	curr;

	curr = Curpos.lnum;

	for (;;)
	{
		if (dir == FORWARD)
		{
				if (curr++ == line_count)
						break;
		}
		else
		{
				if (curr-- == 1)
						break;
		}

		if (*nr2ptr(curr) == what)
		{
			if (--count > 0)
				continue;
			setpcmark();
			Curpos.lnum = curr;
			Curpos.col = 0;
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * findsent(dir, count) - Find the start of the next sentence in direction 'dir'
 * Sentences are supposed to end in ".", "!" or "?" followed by white space or
 * a line break. Also stop at an empty line.
 * Return TRUE if the next sentence was found.
 */
	int
findsent(dir, count)
		int 	dir;
		long	count;
{
	FPOS			pos, tpos;
	register int	c;
	int 			(*func) __PARMS((FPOS *));
	int 			startlnum;
	int				noskip = FALSE;			/* do not skip blanks */

	pos = Curpos;
	if (dir == FORWARD)
		func = incl;
	else
		func = decl;

	while (count--)
	{
		/* if on an empty line, skip upto a non-empty line */
		if (gchar(&pos) == NUL)
		{
			do
				if ((*func)(&pos) == -1)
					break;
			while (gchar(&pos) == NUL);
			if (dir == FORWARD)
				goto found;
		}
		/* if on the start of a paragraph or a section and searching
		 * forward, go to the next line */
		else if (dir == FORWARD && pos.col == 0 && startPS(pos.lnum, NUL))
		{
			if (pos.lnum == line_count)
				return FALSE;
			++pos.lnum;
			goto found;
		}
		else if (dir == BACKWARD)
			decl(&pos);

		/* go back to the previous non-blank char */
		while ((c = gchar(&pos)) == ' ' || c == '\t' ||
					(dir == BACKWARD && strchr(".!?)]\"'", c) != NULL && c != NUL))
			if (decl(&pos) == -1)
				break;

		/* remember the line where the search started */
		startlnum = pos.lnum;

		for (;;)                /* find end of sentence */
		{
			if ((c = gchar(&pos)) == NUL ||
							(pos.col == 0 && startPS(pos.lnum, NUL)))
			{
				if (dir == BACKWARD && pos.lnum != startlnum)
					++pos.lnum;
				break;
			}
			if (c == '.' || c == '!' || c == '?')
			{
				tpos = pos;
				do
					if ((c = inc(&tpos)) == -1)
						break;
				while (strchr(")}\"'", c = gchar(&tpos)) != NULL && c != NUL);
				if (c == -1  || c == ' ' || c == '\t' || c == NUL)
				{
					pos = tpos;
					if (gchar(&pos) == NUL) /* skip NUL at EOL */
						inc(&pos);
					break;
				}
			}
			if ((*func)(&pos) == -1)
			{
				if (count)
					return FALSE;
				noskip = TRUE;
				break;
			}
		}
found:
			/* skip white space */
		while (!noskip && ((c = gchar(&pos)) == ' ' || c == '\t'))
			if (incl(&pos) == -1)
				break;
	}

	Curpos = pos;
	setpcmark();
	return TRUE;
}

/*
 * findpar(dir, count, what) - Find the next paragraph in direction 'dir'
 * Paragraphs are currently supposed to be separated by empty lines.
 * Return TRUE if the next paragraph was found.
 * If 'what' is '{' or '}' we go to the next section.
 */
	int
findpar(dir, count, what)
	register int	dir;
	long			count;
	int 			what;
{
	register linenr_t	curr;
	int					did_skip;		/* TRUE after separating lines have
												been skipped */
	int					first;			/* TRUE on first line */

	curr = Curpos.lnum;

	while (count--)
	{
		did_skip = FALSE;
		for (first = TRUE; ; first = FALSE)
		{
				if (*nr2ptr(curr) != NUL)
					did_skip = TRUE;

				if (!first && did_skip && startPS(curr, what))
					break;

				if ((curr += dir) < 1 || curr > line_count)
				{
						if (count)
								return FALSE;
						curr -= dir;
						break;
				}
		}
	}
	setpcmark();
	Curpos.lnum = curr;
	if (curr == line_count)
	{
		if ((Curpos.col = strlen(nr2ptr(curr))) != 0)
			--Curpos.col;
		mincl = TRUE;
	}
	else
		Curpos.col = 0;
	return TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
	static int
inmacro(opt, s)
		char *opt;
		register char *s;
{
		register char *macro;

		for (macro = opt; macro[0]; ++macro)
		{
				if (macro[0] == s[0] && (((s[1] == NUL || s[1] == ' ')
						&& (macro[1] == NUL || macro[1] == ' ')) || macro[1] == s[1]))
						break;
				++macro;
				if (macro[0] == NUL)
						break;
		}
		return (macro[0] != NUL);
}

/*
 * startPS: return TRUE if line 'lnum' is the start of a section or paragraph.
 * If 'para' is '{' or '}' only check for sections.
 */
	int
startPS(lnum, para)
	linenr_t	lnum;
	int 		para;
{
	register char *s;

	s = nr2ptr(lnum);
	if (*s == para || *s == '\f')
		return TRUE;
	if (*s == '.' && (inmacro(p_sections, s + 1) || (!para && inmacro(p_para, s + 1))))
		return TRUE;
	return FALSE;
}

/*
 * The following routines do the word searches performed by the 'w', 'W',
 * 'b', 'B', 'e', and 'E' commands.
 */

/*
 * To perform these searches, characters are placed into one of three
 * classes, and transitions between classes determine word boundaries.
 *
 * The classes are:
 *
 * 0 - white space
 * 1 - letters, digits and underscore
 * 2 - everything else
 */

static int		stype;			/* type of the word motion being performed */

/*
 * cls() - returns the class of character at Curpos
 *
 * The 'type' of the current search modifies the classes of characters if a 'W',
 * 'B', or 'E' motion is being done. In this case, chars. from class 2 are
 * reported as class 1 since only white space boundaries are of interest.
 */
	static int
cls()
{
	register int c;

	c = gcharCurpos();
	if (c == ' ' || c == '\t' || c == NUL)
		return 0;

	if (isidchar(c))
		return 1;

	/*
	 * If stype is non-zero, report these as class 1.
	 */
	return (stype == 0) ? 2 : 1;
}


/*
 * fwd_word(count, type, eol) - move forward one word
 *
 * Returns TRUE if the cursor was already at the end of the file.
 * If eol is TRUE, last word stops at end of line (for operators).
 */
	int
fwd_word(count, type, eol)
	long		count;
	int 		type;
	int			eol;
{
	int 		sclass; 	/* starting class */
	int			i;

	stype = type;
	while (--count >= 0)
	{
		sclass = cls();

		/*
		 * We always move at least one character.
		 */
		i = incCurpos();
		if (i == -1)
			return TRUE;
		if (i == 1 && eol && count == 0)	/* started at last char in line */
			return FALSE;

		if (sclass != 0)
			while (cls() == sclass)
			{
				i = incCurpos();
				if (i == -1 || (i == 1 && eol && count == 0))
					return FALSE;
			}

		/*
		 * go to next non-white
		 */
		while (cls() == 0)
		{
			/*
			 * We'll stop if we land on a blank line
			 */
			if (Curpos.col == 0 && *nr2ptr(Curpos.lnum) == NUL)
				break;

			i = incCurpos();
			if (i == -1 || (i == 1 && eol && count == 0))
				return FALSE;
		}
	}
	return FALSE;
}

/*
 * bck_word(count, type) - move backward 'count' words
 *
 * Returns TRUE if top of the file was reached.
 */
	int
bck_word(count, type)
	long		count;
	int 		type;
{
	int 		sclass; 	/* starting class */

	stype = type;
	while (--count >= 0)
	{
		sclass = cls();

		if (decCurpos() == -1)		/* started at start of file */
			return TRUE;

		if (cls() != sclass || sclass == 0)
		{
			/*
			 * We were at the start of a word. Go back to the end of the prior
			 * word.
			 */
			while (cls() == 0)  /* skip any white space */
			{
				/*
				 * We'll stop if we land on a blank line
				 */
				if (Curpos.col == 0 && *nr2ptr(Curpos.lnum) == NUL)
					goto finished;

				if (decCurpos() == -1)		/* hit start of file, stop here */
					return FALSE;
			}
			sclass = cls();
		}

		/*
		 * Move backward to start of this word.
		 */
		if (skip_chars(sclass, BACKWARD))
				return FALSE;

		incCurpos();                    /* overshot - forward one */
finished:
		;
	}
	return FALSE;
}

/*
 * end_word(count, type, stop) - move to the end of the word
 *
 * There is an apparent bug in the 'e' motion of the real vi. At least on the
 * System V Release 3 version for the 80386. Unlike 'b' and 'w', the 'e'
 * motion crosses blank lines. When the real vi crosses a blank line in an
 * 'e' motion, the cursor is placed on the FIRST character of the next
 * non-blank line. The 'E' command, however, works correctly. Since this
 * appears to be a bug, I have not duplicated it here.
 *
 * Returns TRUE if end of the file was reached.
 *
 * If stop is TRUE and we are already on the end of a word, move one less.
 */
	int
end_word(count, type, stop)
	long		count;
	int 		type;
	int			stop;
{
	int 		sclass; 	/* starting class */

	stype = type;
	while (--count >= 0)
	{
		sclass = cls();
		if (incCurpos() == -1)
			return TRUE;

		/*
		 * If we're in the middle of a word, we just have to move to the end of it.
		 */
		if (cls() == sclass && sclass != 0)
		{
			/*
			 * Move forward to end of the current word
			 */
			if (skip_chars(sclass, FORWARD))
					return TRUE;
		}
		else if (!stop || sclass == 0)
		{
			/*
			 * We were at the end of a word. Go to the end of the next word.
			 */

			if (skip_chars(0, FORWARD))     /* skip any white space */
				return TRUE;

			/*
			 * Move forward to the end of this word.
			 */
			if (skip_chars(cls(), FORWARD))
				return TRUE;
		}
		decCurpos();                    /* overshot - backward one */
		stop = FALSE;					/* we move only one word less */
	}
	return FALSE;
}

	int
skip_chars(class, dir)
	int class;
	int dir;
{
		while (cls() == class)
			if ((dir == FORWARD ? incCurpos() : decCurpos()) == -1)
				return TRUE;
		return FALSE;
}
