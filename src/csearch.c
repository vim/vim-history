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
 *
 * csearch.c: command line searching commands
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

/* we use modified Henry Spencer's regular expression routines */
#include "regexp.h"

int global_busy = 0;			/* set to 1 if global busy, 2 if global has
									been called during a global command */
int global_wait;				/* set to 1 if wait_return has to be called
									after global command */
extern regexp *myregcomp __ARGS((char *));

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
 */

	void
dosub(lp, up, cmd, nextcommand)
	linenr_t	lp;
	linenr_t	up;
	char		*cmd;
	u_char		**nextcommand;
{
	linenr_t		lnum;
	long			i;
	char		   *ptr;
	regexp		   *prog;
	long			nsubs = 0;
	linenr_t		nlines = 0;
	static int		do_all = FALSE; 	/* do multiple substitutions per line */
	static int		do_ask = FALSE; 	/* ask for confirmation */
	char		   *pat, *sub = NULL;
	static char    *old_sub = NULL;
	int 			delimiter;
	int 			sublen;
	int				got_quit = FALSE;
	int				got_match = FALSE;
	int				temp;

	if (strchr("0123456789gc|\"#", *cmd) == NULL)       /* new pattern and substitution */
	{
		delimiter = *cmd++;			/* remember delimiter character */
		pat = cmd;					/* remember the start of the regexp */

		/*
		 * do the next loop twice:
		 *  i == 0: find the end of the regexp
		 *  i == 1: find the end of the substitution
		 */
		for (i = 0; ; ++i)
		{
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
			if (i == 1)
				break;
			sub = cmd;				/* remember the start of the substitution */
		}
		free(old_sub);
		old_sub = strsave(sub);
	}
	else								/* use previous pattern and substitution */
	{
		if (old_sub == NULL)    /* there is no previous command */
		{
			beep();
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
		do_all = FALSE;
		do_ask = FALSE;
	}
	while (*cmd)
	{
		if (*cmd == 'g')
			do_all = !do_all;
		else if (*cmd == 'c')
			do_ask = !do_ask;
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
	 * check for trailing '|', '"' or '#'
	 */
	skipspace(&cmd);
	if (*cmd)
	{
		if (strchr("|\"#", *cmd) != NULL)
		{
			*nextcommand = (u_char *)cmd;
		}
		else
		{
			emsg(e_trailing);
			return;
		}
	}

	if ((prog = myregcomp(pat)) == NULL)
	{
		emsg(e_invcmd);
		return;
	}

	/*
	 * ~ in the substitute pattern is replaced by the old pattern.
	 * We do it here once to avoid it to be replaced over and over again.
	 */
	sub = regtilde(sub, (int)p_magic);

	for (lnum = lp; lnum <= up && !(got_int || got_quit); ++lnum)
	{
		ptr = nr2ptr(lnum);
		if (regexec(prog, ptr, TRUE))  /* a match on this line */
		{
			char		*new_end, *new_start = NULL;
			char		*old_match, *old_copy;
			char		*prev_old_match = NULL;
			char		*p1, *p2;
			int			did_sub = FALSE;
			int			match, lastone;

			if (!got_match)
			{
				setpcmark();
				got_match = TRUE;
			}

			/*
			 * Save the line that was last changed for the final cursor
			 * position (just like the real vi).
			 */
			Curpos.lnum = lnum;

			old_copy = old_match = ptr;
			for (;;)			/* loop until nothing more to replace */
			{
				Curpos.col = (int)(prog->startp[0] - ptr);
				/*
				 * Match empty string does not count, except for first match.
				 * This reproduces the strange vi behaviour.
				 * This also catches endless loops.
				 */
				if (old_match == prev_old_match && old_match == prog->endp[0])
				{
					++old_match;
					goto skip2;
				}
				while (do_ask)		/* loop until 'y', 'n' or 'q' typed */
				{
					temp = RedrawingDisabled;
					RedrawingDisabled = FALSE;
					updateScreen(CURSUPD);
					smsg("replace by %s (y/n/q)? ", sub);
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
				sublen = regsub(prog, sub, ptr, 0, (int)p_magic);
				if (new_start == NULL)
				{
					/*
					 * Get some space for a temporary buffer to do the substitution
					 * into.
					 */
					if ((new_start = alloc((unsigned)(strlen(ptr) + sublen + 5))) == NULL)
						goto outofmem;
					*new_start = NUL;
				}
				else
				{
					/*
					 * extend the temporary buffer to do the substitution into.
					 */
					if ((p1 = alloc((unsigned)(strlen(new_start) + strlen(old_copy) + sublen + 1))) == NULL)
						goto outofmem;
					strcpy(p1, new_start);
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
				while ((p1 = strchr(new_end, CR)) != NULL)
				{
					if (p1 == new_end || p1[-1] != Ctrl('V'))
					{
						if (u_inssub(lnum))				/* prepare for undo */
						{
							*p1 = NUL;					/* truncate up to the CR */
							if ((p2 = save_line(new_start)) != NULL)
							{
								appendline(lnum - 1, p2);
								++lnum;
								++up;					/* number of lines increases */
							}
							strcpy(new_start, p1 + 1);	/* copy the rest */
							new_end = new_start;
						}
					}
					else							/* remove CTRL-V */
					{
						strcpy(p1 - 1, p1);
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
				old_match = prog->endp[0];
				prev_old_match = old_match;
skip2:
				match = -1;
				lastone = (*old_match == NUL || got_int || got_quit || !do_all);
				if (lastone || do_ask || (match = regexec(prog, old_match, (int)FALSE)) == 0)
				{
					if (new_start)
					{
						/*
						 * copy the rest of the line, that didn't match
						 */
						strcat(new_start, old_copy);
						i = old_match - ptr;

						if ((ptr = save_line(new_start)) != NULL && u_savesub(lnum))
							replaceline(lnum, ptr);

						free(new_start);          /* free the temp buffer */
						new_start = NULL;
						old_match = ptr + i;
						old_copy = ptr;
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
		}
			/* breakcheck is slow, don't call it too often */
		if ((lnum & 15) == 0)
			breakcheck();
	}

outofmem:
	if (nsubs)
	{
		CHANGED;
		updateScreen(CURSUPD); /* need this to update LineSizes */
		beginline(TRUE);
		if (nsubs > p_report)
			smsg("%s%ld substitution%s on %ld line%s",
								got_int ? "(Interrupted) " : "",
								nsubs, plural(nsubs),
								(long)nlines, plural((long)nlines));
		else if (got_int)
				msg(e_interr);
		else if (do_ask)
				msg("");
	}
	else if (got_int)		/* interrupted */
		msg(e_interr);
	else if (got_match)		/* did find something but nothing substituted */
		msg("");
	else					/* nothing found */
		msg(e_nomatch);

	free((char *) prog);
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
	char		*cmd;
{
	linenr_t		lnum;		/* line number according to old situation */
	linenr_t		old_lcount; /* line_count before the command */
	int 			ndone;

	char			delim;		/* delimiter, normally '/' */
	char		   *pat;
	regexp		   *prog;
	int				match;

	if (global_busy)
	{
		emsg("Cannot do :global recursive");
		++global_busy;
		return;
	}

	delim = *cmd; 			/* get the delimiter */
	if (delim)
		++cmd;				/* skip delimiter if there is one */
	pat = cmd;

	while (cmd[0])
	{
		if (cmd[0] == delim)				/* end delimiter found */
		{
			*cmd++ = NUL;					/* replace it by a NUL */
			break;
		}
		if (cmd[0] == '\\' && cmd[1] != 0)	/* skip escaped characters */
			++cmd;
		++cmd;
	}

	reg_ic = p_ic;           /* set "ignore case" flag appropriately */

	if ((prog = myregcomp(pat)) == NULL)
	{
		emsg(e_invcmd);
		return;
	}
	msg("");

/*
 * pass 1: set marks for each (not) matching line
 */
	ndone = 0;
	for (lnum = lp; lnum <= up && !got_int; ++lnum)
	{
		match = regexec(prog, nr2ptr(lnum), (int)TRUE);     /* a match on this line? */
		if ((type == 'g' && match) || (type == 'v' && !match))
		{
			setmarked(lnum);
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
		msg("Interrupted");
	else if (ndone == 0)
		msg(e_nomatch);
	else
	{
		global_busy = 1;
		global_wait = 0;
		RedrawingDisabled = TRUE;
		old_lcount = line_count;
		while (!got_int && (lnum = firstmarked()) != 0 && global_busy == 1)
		{
			Curpos.lnum = lnum;
			Curpos.col = 0;
			if (*cmd == NUL)
				docmdline((u_char *)"p");
			else
				docmdline((u_char *)cmd);
			breakcheck();
		}

		RedrawingDisabled = FALSE;
		global_busy = 0;
		if (global_wait)                /* wait for return */
			wait_return(FALSE);
		screenclear();
		updateScreen(CURSUPD);
		msgmore(line_count - old_lcount);
	}

	clearmarked();      /* clear rest of the marks */
	free((char *) prog);
}
