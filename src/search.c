/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
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

static int inmacro __ARGS((char_u *, char_u *));
static int cls __ARGS((void));

static char_u *top_bot_msg = (char_u *)"search hit TOP, continuing at BOTTOM";
static char_u *bot_top_msg = (char_u *)"search hit BOTTOM, continuing at TOP";

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
 * lowest:	searchit(); called by dosearch() and edit().
 * Highest: dosearch(); changes curwin->w_cursor, called by normal().
 *
 * The last search pattern is remembered for repeating the same search.
 * This pattern is shared between the :g, :s, ? and / commands.
 * This is in myregcomp().
 *
 * The actual string matching is done using a heavily modified version of
 * Henry Spencer's regular expression library.
 */

/*
 * Two search patterns are remembered: One for the :substitute command and
 * one for other searches. last_pattern points to the one that was
 * used the last time.
 */
static char_u 	*search_pattern = NULL;
static char_u 	*subst_pattern = NULL;
static char_u 	*last_pattern = NULL;

static int		want_start;				/* looking for start of line? */

/*
 * translate search pattern for regcomp()
 *
 * sub_cmd == 0: save pat in search_pattern (normal search command)
 * sub_cmd == 1: save pat in subst_pattern (:substitute command)
 * sub_cmd == 2: save pat in both patterns (:global command)
 * which_pat == 0: use previous search pattern if "pat" is NULL
 * which_pat == 1: use previous sustitute pattern if "pat" is NULL
 * which_pat == 2: use last used pattern if "pat" is NULL
 * 
 */
	regexp *
myregcomp(pat, sub_cmd, which_pat)
	char_u	*pat;
	int		sub_cmd;
	int		which_pat;
{
	regexp *retval;

	if (pat == NULL || *pat == NUL)     /* use previous search pattern */
	{
		if (which_pat == 0)
		{
			if (search_pattern == NULL)
			{
				emsg(e_noprevre);
				return (regexp *) NULL;
			}
			pat = search_pattern;
		}
		else if (which_pat == 1)
		{
			if (subst_pattern == NULL)
			{
				emsg(e_nopresub);
				return (regexp *) NULL;
			}
			pat = subst_pattern;
		}
		else	/* which_pat == 2 */
		{
			if (last_pattern == NULL)
			{
				emsg(e_noprevre);
				return (regexp *) NULL;
			}
			pat = last_pattern;
		}
	}

	/*
	 * save the currently used pattern in the appropriate place,
	 * unless the pattern should not be remembered
	 */
	if (!keep_old_search_pattern)
	{
		if (sub_cmd == 0 || sub_cmd == 2)	/* search or global command */
		{
			if (search_pattern != pat)
			{
				free(search_pattern);
				search_pattern = strsave(pat);
				last_pattern = search_pattern;
				reg_magic = p_magic;		/* Magic sticks with the r.e. */
			}
		}
		if (sub_cmd == 1 || sub_cmd == 2)	/* substitute or global command */
		{
			if (subst_pattern != pat)
			{
				free(subst_pattern);
				subst_pattern = strsave(pat);
				last_pattern = subst_pattern;
				reg_magic = p_magic;		/* Magic sticks with the r.e. */
			}
		}
	}

	want_start = (*pat == '^');		/* looking for start of line? */
	reg_ic = p_ic;					/* tell the regexec routine how to search */
	retval = regcomp(pat);
	return retval;
}

/*
 * lowest level search function.
 * Search for 'count'th occurrence of 'str' in direction 'dir'.
 * Start at position 'pos' and return the found position in 'pos'.
 * Return OK for success, FAIL for failure.
 */
	int
searchit(pos, dir, str, count, end, message)
	FPOS	*pos;
	int 	dir;
	char_u	*str;
	long	count;
	int		end;
	int		message;
{
	int 				found;
	linenr_t			lnum = 0;			/* init to shut up gcc */
	linenr_t			startlnum;
	regexp				*prog;
	register char_u		*s;
	char_u				*ptr;
	register int		i;
	register char_u		*match, *matchend;
	int 				loop;

	if ((prog = myregcomp(str, 0, 2)) == NULL)
	{
		if (message)
			emsg(e_invstring);
		return FAIL;
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

		if (dir == BACKWARD && i < 0)
			--lnum;

		for (loop = 0; loop != 2; ++loop)   /* do this twice if 'wrapscan' is set */
		{
			for ( ; lnum > 0 && lnum <= curbuf->b_ml.ml_line_count; lnum += dir, i = -1)
			{
				s = ptr = ml_get(lnum);
				if (dir == FORWARD && i > 0)    /* first line for forward search */
				{
					if (want_start || STRLEN(s) <= (size_t)i)   /* match not possible */
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
						 * Now, if there are multiple matches on this line,
						 * we have to get the last one. Or the last one before
						 * the cursor, if we're on that line.
						 */
						while (*match != NUL && regexec(prog, match + 1, (int)FALSE))
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

			/*
			 * If 'wrapscan' is set we continue at the other end of the file.
			 * If 'terse' is not set, we give a message.
			 * This message is also remembered in keep_msg for when the screen
			 * is redrawn. The keep_msg is cleared whenever another message is
			 * written.
			 */
			if (dir == BACKWARD)    /* start second loop at the other end */
			{
				lnum = curbuf->b_ml.ml_line_count;
				if (!p_terse && message)
				{
					msg(top_bot_msg);
					keep_msg = top_bot_msg;
				}
			}
			else
			{
				lnum = 1;
				if (!p_terse && message)
				{
					msg(bot_top_msg);
					keep_msg = bot_top_msg;
				}
			}
		}
		if (got_int)
			break;
	}

	free(prog);

	if (!found)             /* did not find it */
	{
		if (got_int)
			emsg(e_interr);
		else if (message)
		{
			if (p_ws)
				emsg(e_patnotf);
			else if (lnum == 0)
				EMSG("search hit TOP without match");
			else
				EMSG("search hit BOTTOM without match");
		}
		return FAIL;
	}

	return OK;
}

/*
 * Highest level string search function.
 * Search for the 'count'th occurence of string 'str' in direction 'dirc'
 *					If 'dirc' is 0: use previous dir.
 * If 'str' is 0 or 'str' is empty: use previous string.
 *			  If 'reverse' is TRUE: go in reverse of previous dir.
 *				 If 'echo' is TRUE: echo the search command and handle options
 *			  If 'message' is TRUE: may give error message
 *
 * return 0 for failure, 1 for found, 2 for found and line offset added
 */
	int
dosearch(dirc, str, reverse, count, echo, message)
	int				dirc;
	char_u		   *str;
	int				reverse;
	long			count;
	int				echo;
	int				message;
{
	FPOS			pos;		/* position of the last match */
	char_u			*searchstr;
	static int		lastsdir = '/';	/* previous search direction */
	static int		lastoffline;/* previous/current search has line offset */
	static int		lastend;	/* previous/current search set cursor at end */
	static long 	lastoff;	/* previous/current line or char offset */
	int				old_lastsdir;
	int				old_lastoffline;
	int				old_lastend;
	long			old_lastoff;
	int				ret;		/* Return value */
	register char_u	*p;
	register long	c;
	char_u			*dircp = NULL;

	/*
	 * save the values for when keep_old_search_pattern is set
	 * (no if around this because gcc wants them initialized)
	 */
	old_lastsdir = lastsdir;
	old_lastoffline = lastoffline;
	old_lastend = lastend;
	old_lastoff = lastoff;

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
			ret = 0;
			goto end_dosearch;
		}
		searchstr = (char_u *)"";	/* will use search_pattern in myregcomp() */
	}
	if (str != NULL && *str != NUL)	/* look for (new) offset */
	{
		/*
		 * Find end of regular expression.
		 * If there is a matching '/' or '?', toss it.
		 */
		p = skip_regexp(str, dirc);
		if (*p == dirc)
		{
			dircp = p;		/* remember where we put the NUL */
			*p++ = NUL;
		}
		lastoffline = FALSE;
		lastend = FALSE;
		lastoff = 0;
		/*
		 * Check for a line offset or a character offset.
		 * for get_address (echo off) we don't check for a character offset,
		 * because it is meaningless and the 's' could be a substitute command.
		 */
		if (*p == '+' || *p == '-' || isdigit(*p))
			lastoffline = TRUE;
		else if (echo && (*p == 'e' || *p == 's' || *p == 'b'))
		{
			if (*p == 'e')			/* end */
				lastend = TRUE;
			++p;
		}
		if (isdigit(*p) || *p == '+' || *p == '-')     /* got an offset */
		{
			if (isdigit(*p) || isdigit(*(p + 1)))
				lastoff = atol((char *)p);		/* 'nr' or '+nr' or '-nr' */
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
		msg_start();
		msg_outchar(dirc);
		msg_outtrans(*searchstr == NUL ? search_pattern : searchstr, -1);
		if (lastoffline || lastend || lastoff)
		{
			msg_outchar(dirc);
			if (lastend)
				msg_outchar('e');
			else if (!lastoffline)
				msg_outchar('s');
			if (lastoff < 0)
			{
				msg_outchar('-');
				msg_outnum((long)-lastoff);
			}
			else if (lastoff > 0 || lastoffline)
			{
				msg_outchar('+');
				msg_outnum((long)lastoff);
			}
		}
		msg_ceol();
		(void)msg_check();

		gotocmdline(FALSE, NUL);
		flushbuf();
	}

	pos = curwin->w_cursor;

	c = searchit(&pos, dirc == '/' ? FORWARD : BACKWARD, searchstr, count, lastend, message);
	if (dircp)
		*dircp = dirc;			/* put second '/' or '?' back for normal() */
	if (c == FAIL)
	{
		ret = 0;
		goto end_dosearch;
	}
	if (lastend)
		mincl = TRUE;			/* 'e' includes last character */

	if (!lastoffline)           /* add the character offset to the column */
	{
		if (lastoff > 0)        /* offset to the right, check for end of line */
		{
			p = ml_get_pos(&pos) + 1;
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

	if (!tag_busy)
		setpcmark();
	curwin->w_cursor = pos;
	curwin->w_set_curswant = TRUE;

	if (!lastoffline)
	{
		ret = 1;
		goto end_dosearch;
	}

/*
 * add the offset to the line number.
 */
	c = curwin->w_cursor.lnum + lastoff;
	if (c < 1)
		curwin->w_cursor.lnum = 1;
	else if (c > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	else
		curwin->w_cursor.lnum = c;
	curwin->w_cursor.col = 0;

	ret = 2;

end_dosearch:
	if (keep_old_search_pattern)
	{
		lastsdir = old_lastsdir;
		lastoffline = old_lastoffline;
		lastend = old_lastend;
		lastoff = old_lastoff;
	}
	return ret;
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
	static int	 	lastc = NUL;	/* last character searched for */
	static int		lastcdir;		/* last direction of character search */
	static int		lastctype;		/* last type of search ("find" or "to") */
	register int	col;
	char_u			*p;
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

	p = ml_get(curwin->w_cursor.lnum);
	col = curwin->w_cursor.col;
	len = STRLEN(p);

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
	curwin->w_cursor.col = col;
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
showmatch(initc)
	int		initc;
{
	static FPOS		pos;				/* current search position */
	int				findc;				/* matching brace */
	int				c;
	int 			count = 0;			/* cumulative number of braces */
	int 			idx = 0;			/* init for gcc */
	static char_u 	table[6] = {'(', ')', '[', ']', '{', '}'};
	int 			inquote = 0;		/* non-zero when inside quotes */
	register char_u	*linep;				/* pointer to current line */
	register char_u	*ptr;
	int				do_quotes;			/* check for quotes in current line */
	int				hash_dir = 0;		/* Direction searched for # things */
	int				comment_dir = 0;	/* Direction searched for comments */

	pos = curwin->w_cursor;
	linep = ml_get(pos.lnum); 

	/*
	 * if initc given, look in the table for the matching character
	 */
	if (initc != NUL)
	{
		for (idx = 0; idx < 6; ++idx)
			if (table[idx] == initc)
			{
				initc = table[idx = idx ^ 1];
				break;
			}
		if (idx == 6)			/* invalid initc! */
			return NULL;
	}
	/*
	 * no initc given, look under the cursor
	 */
	else
	{
		if (linep[0] == '#' && pos.col == 0)
			hash_dir = 1;

		/*
		 * Are we on a comment?
		 */
		if (linep[pos.col] == '/')
		{
			if (linep[pos.col + 1] == '*')
			{
				comment_dir = 1;
				idx = 0;
			}
			else if (pos.col > 0 && linep[pos.col - 1] == '*')
			{
				comment_dir = -1;
				idx = 1;
			}
		}
		if (linep[pos.col] == '*')
		{
			if (linep[pos.col + 1] == '/')
			{
				comment_dir = -1;
				idx = 1;
			}
			else if (pos.col > 0 && linep[pos.col - 1] == '/')
			{
				comment_dir = 1;
				idx = 0;
			}
		}

		/*
		 * If we are not on a comment or the # at the start of a line, then
		 * look for brace anywhere on this line after the cursor.
		 */
		if (!hash_dir && !comment_dir)
		{
			/*
			 * find the brace under or after the cursor
			 */
			linep = ml_get(pos.lnum); 
			for (;;)
			{
				initc = linep[pos.col];
				if (initc == NUL)
					break;

				for (idx = 0; idx < 6; ++idx)
					if (table[idx] == initc)
						break;
				if (idx != 6)
					break;
				++pos.col;
			}
			if (idx == 6)
			{
				if (linep[0] == '#')
					hash_dir = 1;
				else
					return NULL;
			}
		}
		if (hash_dir)
		{
			/*
			 * Look for matching #if, #else, #elif, or #endif
			 */
			mtype = MLINE;		/* Linewise for this case only */
			ptr = linep + 1;
			while (*ptr == ' ' || *ptr == TAB)
				ptr++;
			if (STRNCMP(ptr, "if", (size_t)2) == 0 || STRNCMP(ptr, "el", (size_t)2) == 0)
				hash_dir = 1;
			else if (STRNCMP(ptr, "endif", (size_t)5) == 0)
				hash_dir = -1;
			else
				return NULL;
			pos.col = 0;
			while (!got_int)
			{
				if (hash_dir > 0)
				{
					if (pos.lnum == curbuf->b_ml.ml_line_count)
						break;
				}
				else if (pos.lnum == 1)
					break;
				pos.lnum += hash_dir;
				linep = ml_get(pos.lnum);
				if ((pos.lnum & 15) == 0)
					breakcheck();
				if (linep[0] != '#')
					continue;
				ptr = linep + 1;
				while (*ptr == ' ' || *ptr == TAB)
					ptr++;
				if (hash_dir > 0)
				{
					if (STRNCMP(ptr, "if", (size_t)2) == 0)
						count++;
					else if (STRNCMP(ptr, "el", (size_t)2) == 0)
					{
						if (count == 0)
							return &pos;
					}
					else if (STRNCMP(ptr, "endif", (size_t)5) == 0)
					{
						if (count == 0)
							return &pos;
						count--;
					}
				}
				else
				{
					if (STRNCMP(ptr, "if", (size_t)2) == 0)
					{
						if (count == 0)
							return &pos;
						count--;
					}
					else if (STRNCMP(ptr, "endif", (size_t)5) == 0)
						count++;
				}
			}
			return NULL;
		}
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
				linep = ml_get(pos.lnum);
				pos.col = STRLEN(linep);	/* put pos.col on trailing NUL */
				do_quotes = -1;
					/* we only do a breakcheck() once for every 16 lines */
				if ((pos.lnum & 15) == 0)
					breakcheck();
			}
			else
				--pos.col;
		}
		else							/* forward search */
		{
			if (linep[pos.col] == NUL)  /* at end of line, go to next one */
			{
				if (pos.lnum == curbuf->b_ml.ml_line_count) /* end of file */
					break;
				++pos.lnum;
				linep = ml_get(pos.lnum);
				pos.col = 0;
				do_quotes = -1;
					/* we only do a breakcheck() once for every 16 lines */
				if ((pos.lnum & 15) == 0)
					breakcheck();
			}
			else
				++pos.col;
		}

		if (comment_dir)
		{
			/* Note: comments do not nest, and we ignore quotes in them */
			if (linep[pos.col] != '/' ||
							(comment_dir == 1 && pos.col == 0) ||
							linep[pos.col - comment_dir] != '*')
				continue;
			return &pos;
		}

		if (do_quotes == -1)		/* count number of quotes in this line */
		{
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
					ptr = ml_get(pos.lnum - 1);
					if (*ptr && *(ptr + STRLEN(ptr) - 1) == '\\')
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

	curr = curwin->w_cursor.lnum;

	for (;;)
	{
		if (dir == FORWARD)
		{
				if (curr++ == curbuf->b_ml.ml_line_count)
						break;
		}
		else
		{
				if (curr-- == 1)
						break;
		}

		if (*ml_get(curr) == what)
		{
			if (--count > 0)
				continue;
			setpcmark();
			curwin->w_cursor.lnum = curr;
			curwin->w_cursor.col = 0;
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

	pos = curwin->w_cursor;
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
		else if (dir == FORWARD && pos.col == 0 && startPS(pos.lnum, NUL, FALSE))
		{
			if (pos.lnum == curbuf->b_ml.ml_line_count)
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
							(pos.col == 0 && startPS(pos.lnum, NUL, FALSE)))
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

	setpcmark();
	curwin->w_cursor = pos;
	return TRUE;
}

/*
 * findpar(dir, count, what) - Find the next paragraph in direction 'dir'
 * Paragraphs are currently supposed to be separated by empty lines.
 * Return TRUE if the next paragraph was found.
 * If 'what' is '{' or '}' we go to the next section.
 * If 'both' is TRUE also stop at '}'.
 */
	int
findpar(dir, count, what, both)
	register int	dir;
	long			count;
	int 			what;
	int				both;
{
	register linenr_t	curr;
	int					did_skip;		/* TRUE after separating lines have
												been skipped */
	int					first;			/* TRUE on first line */

	curr = curwin->w_cursor.lnum;

	while (count--)
	{
		did_skip = FALSE;
		for (first = TRUE; ; first = FALSE)
		{
				if (*ml_get(curr) != NUL)
					did_skip = TRUE;

				if (!first && did_skip && startPS(curr, what, both))
					break;

				if ((curr += dir) < 1 || curr > curbuf->b_ml.ml_line_count)
				{
						if (count)
								return FALSE;
						curr -= dir;
						break;
				}
		}
	}
	setpcmark();
	if (both && *ml_get(curr) == '}')	/* include line with '}' */
		++curr;
	curwin->w_cursor.lnum = curr;
	if (curr == curbuf->b_ml.ml_line_count)
	{
		if ((curwin->w_cursor.col = STRLEN(ml_get(curr))) != 0)
			--curwin->w_cursor.col;
		mincl = TRUE;
	}
	else
		curwin->w_cursor.col = 0;
	return TRUE;
}

/*
 * check if the string 's' is a nroff macro that is in option 'opt'
 */
	static int
inmacro(opt, s)
		char_u *opt;
		register char_u *s;
{
		register char_u *macro;

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
 * If 'both' is TRUE also stop at '}'
 */
	int
startPS(lnum, para, both)
	linenr_t	lnum;
	int 		para;
	int			both;
{
	register char_u *s;

	s = ml_get(lnum);
	if (*s == para || *s == '\f' || (both && *s == '}'))
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
 * cls() - returns the class of character at curwin->w_cursor
 *
 * The 'type' of the current search modifies the classes of characters if a 'W',
 * 'B', or 'E' motion is being done. In this case, chars. from class 2 are
 * reported as class 1 since only white space boundaries are of interest.
 */
	static int
cls()
{
	register int c;

	c = gchar_cursor();
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
		i = inc_cursor();
		if (i == -1)
			return TRUE;
		if (i == 1 && eol && count == 0)	/* started at last char in line */
			return FALSE;

		if (sclass != 0)
			while (cls() == sclass)
			{
				i = inc_cursor();
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
			if (curwin->w_cursor.col == 0 && *ml_get(curwin->w_cursor.lnum) == NUL)
				break;

			i = inc_cursor();
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

		if (dec_cursor() == -1)		/* started at start of file */
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
				if (curwin->w_cursor.col == 0 && *ml_get(curwin->w_cursor.lnum) == NUL)
					goto finished;

				if (dec_cursor() == -1)		/* hit start of file, stop here */
					return FALSE;
			}
			sclass = cls();
		}

		/*
		 * Move backward to start of this word.
		 */
		if (skip_chars(sclass, BACKWARD))
				return FALSE;

		inc_cursor();                    /* overshot - forward one */
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
		if (inc_cursor() == -1)
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
		dec_cursor();                    /* overshot - backward one */
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
			if ((dir == FORWARD ? inc_cursor() : dec_cursor()) == -1)
				return TRUE;
		return FALSE;
}
