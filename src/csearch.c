/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 *
 * csearch.c: dosub() and doglob() for :s, :g and :v
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

/* we use modified Henry Spencer's regular expression routines */
#include "regexp.h"

/* dosub(lp, up, cmd)
 *
 * Perform a substitution from line 'lp' to line 'up' using the
 * command pointed to by 'cmd' which should be of the form:
 *
 * /pattern/substitution/gc
 *
 * The trailing 'g' is optional and, if present, indicates that multiple
 * substitutions should be performed on each line, if applicable.
 * The trailing 'c' is optional and, if present, indicates that a confirmation
 * will be asked for each replacement.
 * The usual escapes are supported as described in the regexp docs.
 *
 * use_old == 0 for :substitute
 * use_old == 1 for :&
 * use_old == 2 for :~
 */

	void
dosub(lp, up, cmd, nextcommand, use_old)
	linenr_t	lp;
	linenr_t	up;
	char_u		*cmd;
	char_u		**nextcommand;
	int			use_old;
{
	linenr_t		lnum;
	long			i;
	char_u		   *ptr;
	char_u		   *old_line;
	regexp		   *prog;
	long			nsubs = 0;
	linenr_t		nlines = 0;
	static int		do_all = FALSE; 	/* do multiple substitutions per line */
	static int		do_ask = FALSE; 	/* ask for confirmation */
	char_u		   *pat = NULL, *sub = NULL;
	static char_u   *old_sub = NULL;
	int 			delimiter;
	int 			sublen;
	int				got_quit = FALSE;
	int				got_match = FALSE;
	int				temp;
	int				which_pat;
	
	if (use_old == 2)
		which_pat = 2;		/* use last used regexp */
	else
		which_pat = 1;		/* use last substitute regexp */

								   /* new pattern and substitution */
	if (use_old == 0 && *cmd != NUL && strchr("0123456789gcr|\"", *cmd) == NULL)
	{
		if (isalpha(*cmd))			/* don't accept alpha for separator */
		{
			emsg(e_invarg);
			return;
		}
		/*
		 * undocumented vi feature:
		 *	"\/sub/" and "\?sub?" use last used search pattern (almost like //sub/r).
		 *  "\&sub&" use last substitute pattern (like //sub/).
		 */
		if (*cmd == '\\')
		{
			++cmd;
			if (strchr("/?&", *cmd) == NULL)
			{
				emsg(e_backslash);
				return;
			}
			if (*cmd != '&')
				which_pat = 0;				/* use last '/' pattern */
			pat = (char_u *)"";				/* empty search pattern */
			delimiter = *cmd++;				/* remember delimiter character */
		}
		else			/* find the end of the regexp */
		{
			delimiter = *cmd++;				/* remember delimiter character */
			pat = cmd;						/* remember start of search pattern */
			cmd = skip_regexp(cmd, delimiter);
			if (cmd[0] == delimiter)		/* end delimiter found */
				*cmd++ = NUL;				/* replace it by a NUL */
		}

		/*
		 * Small incompatibility: vi sees '\n' as end of the command, but in
		 * Vim we want to use '\n' to find/substitute a NUL.
		 */
		sub = cmd;			/* remember the start of the substitution */

		while (cmd[0])
		{
			if (cmd[0] == delimiter)			/* end delimiter found */
			{
				*cmd++ = NUL;					/* replace it by a NUL */
				break;
			}
			if (cmd[0] == '\\' && cmd[1] != 0)	/* skip escaped characters */
				++cmd;
			++cmd;
		}

		free(old_sub);
		old_sub = strsave(sub);
	}
	else								/* use previous pattern and substitution */
	{
		if (old_sub == NULL)    /* there is no previous command */
		{
			emsg(e_nopresub);
			return;
		}
		pat = NULL; 			/* myregcomp() will use previous pattern */
		sub = old_sub;
	}

	/*
	 * find trailing options
	 */
	if (!p_ed)
	{
		if (p_gd)				/* default is global on */
			do_all = TRUE;
		else
			do_all = FALSE;
		do_ask = FALSE;
	}
	while (*cmd)
	{
		/*
		 * Note that 'g' and 'c' are always inverted, also when p_ed is off
		 * 'r' is never inverted.
		 */
		if (*cmd == 'g')
			do_all = !do_all;
		else if (*cmd == 'c')
			do_ask = !do_ask;
		else if (*cmd == 'r')		/* use last used regexp */
			which_pat = 2;
		else
			break;
		++cmd;
	}

	/*
	 * check for a trailing count
	 */
	skipspace(&cmd);
	if (isdigit(*cmd))
	{
		i = getdigits(&cmd);
		if (i <= 0)
		{
			emsg(e_zerocount);
			return;
		}
		lp = up;
		up += i - 1;
	}

	/*
	 * check for trailing '|', '"' or '\n'
	 */
	skipspace(&cmd);
	if (*cmd)
	{
		if (strchr("|\"\n", *cmd) == NULL)
		{
			emsg(e_trailing);
			return;
		}
		else
			*nextcommand = cmd;
	}

	if ((prog = myregcomp(pat, 1, which_pat)) == NULL)
	{
		emsg(e_invcmd);
		return;
	}

	/*
	 * ~ in the substitute pattern is replaced by the old pattern.
	 * We do it here once to avoid it to be replaced over and over again.
	 */
	sub = regtilde(sub, (int)p_magic);

	old_line = NULL;
	for (lnum = lp; lnum <= up && !(got_int || got_quit); ++lnum)
	{
		ptr = ml_get(lnum);
		if (regexec(prog, ptr, TRUE))  /* a match on this line */
		{
			char_u		*new_end, *new_start = NULL;
			char_u		*old_match, *old_copy;
			char_u		*prev_old_match = NULL;
			char_u		*p1;
			int			did_sub = FALSE;
			int			match, lastone;

			/* make a copy of the line, so it won't be taken away when updating
				the screen */
			if ((old_line = strsave(ptr)) == NULL)
				continue;
			regexec(prog, old_line, TRUE);  /* match again on this line to update the pointers. TODO: remove extra regexec() */
			if (!got_match)
			{
				setpcmark();
				got_match = TRUE;
			}

			old_copy = old_match = old_line;
			for (;;)			/* loop until nothing more to replace */
			{
				/*
				 * Save the position of the last change for the final cursor
				 * position (just like the real vi).
				 */
				curwin->w_cursor.lnum = lnum;
				curwin->w_cursor.col = (int)(prog->startp[0] - old_line);

				/*
				 * Match empty string does not count, except for first match.
				 * This reproduces the strange vi behaviour.
				 * This also catches endless loops.
				 */
				if (old_match == prev_old_match && old_match == prog->endp[0])
				{
					++old_match;
					goto skip;
				}
				old_match = prog->endp[0];
				prev_old_match = old_match;

				while (do_ask)		/* loop until 'y', 'n' or 'q' typed */
				{
					temp = RedrawingDisabled;
					RedrawingDisabled = FALSE;
					comp_Botline(curwin);
					updateScreen(CURSUPD);
									/* same highlighting as for wait_return */
					(void)set_highlight('r');
					msg_highlight = TRUE;
					smsg((char_u *)"replace by %s (y/n/q)?", sub);
					showruler(TRUE);
					setcursor();
					RedrawingDisabled = temp;
					if ((i = vgetc()) == 'q' || i == ESC || i == Ctrl('C'))
					{
						got_quit = TRUE;
						break;
					}
					else if (i == 'n')
						goto skip;
					else if (i == 'y')
						break;
				}
				if (got_quit)
					break;

						/* get length of substitution part */
				sublen = regsub(prog, sub, old_line, 0, (int)p_magic);
				if (new_start == NULL)
				{
					/*
					 * Get some space for a temporary buffer to do the substitution
					 * into.
					 */
					if ((new_start = alloc((unsigned)(STRLEN(old_line) + sublen + 5))) == NULL)
						goto outofmem;
					*new_start = NUL;
				}
				else
				{
					/*
					 * extend the temporary buffer to do the substitution into.
					 */
					if ((p1 = alloc((unsigned)(STRLEN(new_start) + STRLEN(old_copy) + sublen + 1))) == NULL)
						goto outofmem;
					STRCPY(p1, new_start);
					free(new_start);
					new_start = p1;
				}

				for (new_end = new_start; *new_end; new_end++)
					;
				/*
				 * copy up to the part that matched
				 */
				while (old_copy < prog->startp[0])
					*new_end++ = *old_copy++;

				regsub(prog, sub, new_end, 1, (int)p_magic);
				nsubs++;
				did_sub = TRUE;

				/*
				 * Now the trick is to replace CTRL-Ms with a real line break.
				 * This would make it impossible to insert CTRL-Ms in the text.
				 * That is the way vi works. In Vim the line break can be
				 * avoided by preceding the CTRL-M with a CTRL-V. Now you can't
				 * precede a line break with a CTRL-V, big deal.
				 */
				while ((p1 = STRCHR(new_end, CR)) != NULL)
				{
					if (p1 == new_end || p1[-1] != Ctrl('V'))
					{
						if (u_inssub(lnum))				/* prepare for undo */
						{
							*p1 = NUL;					/* truncate up to the CR */
							mark_adjust(lnum, MAXLNUM, 1L);
							ml_append(lnum - 1, new_start, (colnr_t)(p1 - new_start + 1), FALSE);
							++lnum;
							++up;					/* number of lines increases */
							STRCPY(new_start, p1 + 1);	/* copy the rest */
							new_end = new_start;
						}
					}
					else							/* remove CTRL-V */
					{
						STRCPY(p1 - 1, p1);
						new_end = p1;
					}
				}

				old_copy = prog->endp[0];	/* remember next character to be copied */
				/*
				 * continue searching after the match
				 * prevent endless loop with patterns that match empty strings,
				 * e.g. :s/$/pat/g or :s/[a-z]* /(&)/g
				 */
skip:
				match = -1;
				lastone = (*old_match == NUL || got_int || got_quit || !do_all);
				if (lastone || do_ask || (match = regexec(prog, old_match, (int)FALSE)) == 0)
				{
					if (new_start)
					{
						/*
						 * Copy the rest of the line, that didn't match.
						 * Old_match has to be adjusted, we use the end of the line
						 * as reference, because the substitute may have changed
						 * the number of characters.
						 */
						STRCAT(new_start, old_copy);
						i = old_line + STRLEN(old_line) - old_match;
						if (u_savesub(lnum))
							ml_replace(lnum, new_start, TRUE);

						free(old_line);			/* free the temp buffer */
						old_line = new_start;
						new_start = NULL;
						old_match = old_line + STRLEN(old_line) - i;
						if (old_match < old_line)		/* safety check */
						{
							EMSG("dosub internal error: old_match < old_line");
							old_match = old_line;
						}
						old_copy = old_line;
					}
					if (match == -1 && !lastone)
						match = regexec(prog, old_match, (int)FALSE);
					if (match <= 0)		/* quit loop if there is no more match */
						break;
				}
					/* breakcheck is slow, don't call it too often */
				if ((nsubs & 15) == 0)
					breakcheck();

			}
			if (did_sub)
				++nlines;
			free(old_line);		/* free the copy of the original line */
			old_line = NULL;
		}
			/* breakcheck is slow, don't call it too often */
		if ((lnum & 15) == 0)
			breakcheck();
	}

outofmem:
	free(old_line);		/* may have to free an allocated copy of the line */
	if (nsubs)
	{
		CHANGED;
		updateScreen(CURSUPD); /* need this to update LineSizes */
		beginline(TRUE);
		if (nsubs > p_report)
			smsg((char_u *)"%s%ld substitution%s on %ld line%s",
								got_int ? "(Interrupted) " : "",
								nsubs, plural(nsubs),
								(long)nlines, plural((long)nlines));
		else if (got_int)
				emsg(e_interr);
		else if (do_ask)
				MSG("");
	}
	else if (got_int)		/* interrupted */
		emsg(e_interr);
	else if (got_match)		/* did find something but nothing substituted */
		MSG("");
	else					/* nothing found */
		emsg(e_nomatch);

	free(prog);
}

/*
 * doglob(cmd)
 *
 * Execute a global command of the form:
 *
 * g/pattern/X : execute X on all lines where pattern matches
 * v/pattern/X : execute X on all lines where pattern does not match
 *
 * where 'X' is an EX command
 *
 * The command character (as well as the trailing slash) is optional, and
 * is assumed to be 'p' if missing.
 *
 * This is implemented in two passes: first we scan the file for the pattern and
 * set a mark for each line that (not) matches. secondly we execute the command
 * for each line that has a mark. This is required because after deleting
 * lines we do not know where to search for the next match.
 */

	void
doglob(type, lp, up, cmd)
	int 		type;
	linenr_t	lp, up;
	char_u		*cmd;
{
	linenr_t		lnum;		/* line number according to old situation */
	linenr_t		old_lcount; /* curbuf->b_ml.ml_line_count before the command */
	int 			ndone;

	char_u			delim;		/* delimiter, normally '/' */
	char_u		   *pat;
	regexp		   *prog;
	int				match;
	int				which_pat;

	if (global_busy)
	{
		EMSG("Cannot do :global recursive");
		++global_busy;
		return;
	}

	which_pat = 2;			/* default: use last used regexp */

	/*
	 * undocumented vi feature:
	 *	"\/" and "\?": use previous search pattern.
	 *  	     "\&": use previous substitute pattern.
	 */
	if (*cmd == '\\')
	{
		++cmd;
		if (strchr("/?&", *cmd) == NULL)
		{
			emsg(e_backslash);
			return;
		}
		if (*cmd == '&')
			which_pat = 1;		/* use previous substitute pattern */
		else
			which_pat = 0;		/* use previous search pattern */
		++cmd;
		pat = (char_u *)"";
	}
	else
	{
		delim = *cmd; 			/* get the delimiter */
		if (delim)
			++cmd;				/* skip delimiter if there is one */
		pat = cmd;				/* remember start of pattern */
		cmd = skip_regexp(cmd, delim);
		if (cmd[0] == delim)				/* end delimiter found */
			*cmd++ = NUL;					/* replace it by a NUL */
	}

	reg_ic = p_ic;           /* set "ignore case" flag appropriately */

	if ((prog = myregcomp(pat, 2, which_pat)) == NULL)
	{
		emsg(e_invcmd);
		return;
	}
	MSG("");

/*
 * pass 1: set marks for each (not) matching line
 */
	ndone = 0;
	for (lnum = lp; lnum <= up && !got_int; ++lnum)
	{
		match = regexec(prog, ml_get(lnum), (int)TRUE);     /* a match on this line? */
		if ((type == 'g' && match) || (type == 'v' && !match))
		{
			ml_setmarked(lnum);
			ndone++;
		}
			/* breakcheck is slow, don't call it too often */
		if ((lnum & 15) == 0)
			breakcheck();
	}

/*
 * pass 2: execute the command for each line that has been marked
 */
	if (got_int)
		MSG("Interrupted");
	else if (ndone == 0)
		msg(e_nomatch);
	else
	{
		global_busy = 1;
		dont_sleep = 1;			/* don't sleep in emsg() */
		no_wait_return = 1;		/* dont wait for return until finished */
		need_wait_return = FALSE;
		RedrawingDisabled = TRUE;
		old_lcount = curbuf->b_ml.ml_line_count;
		did_msg = FALSE;
		while (!got_int && (lnum = ml_firstmarked()) != 0 && global_busy == 1)
		{
			/*
			 * If there was a message from the previous command, scroll
			 * the lines up for the next, otherwise it will be overwritten.
			 * did_msg is set by msg_start().
			 */
			if (did_msg)
			{
				cmdline_row = msg_row;
				did_msg = FALSE;
			}
			curwin->w_cursor.lnum = lnum;
			curwin->w_cursor.col = 0;
			if (*cmd == NUL || *cmd == '\n')
				docmdline((char_u *)"p");
			else
				docmdline(cmd);
			breakcheck();
		}

		RedrawingDisabled = FALSE;
		global_busy = 0;
		dont_sleep = 0;
		no_wait_return = 0;
		if (need_wait_return)                /* wait for return now */
			wait_return(FALSE);

		screenclear();
		updateScreen(CURSUPD);
		msgmore(curbuf->b_ml.ml_line_count - old_lcount);
	}

	ml_clearmarked();      /* clear rest of the marks */
	free(prog);
}
