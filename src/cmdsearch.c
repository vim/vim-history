/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony
 *							G. R. (Fred) Walter     watmath!watcgl!grwalter
 */

/*
 *
 * cmdsearch.c: command line searching commands
 */

#include "vim.h"

/* we use modified Henry Spencer's regular expression routines */
#include "regexp.h"

extern char emsg_inval[];
extern char emsg_interrupted[];
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
	bool_t			do_all; 		/* do multiple substitutions per line */
	bool_t			do_ask; 		/* ask for confirmation */
	char		   *pat, *sub;
	static char    *old_sub = NULL;
	int 			delimiter;
	int 			sublen;

	if (strchr("0123456789gc|\"#", *cmd) == NULL)       /* new pattern and substitution */
	{
		delimiter = *cmd;			/* remember delimiter character */
		++cmd;						/* skip the delimiter */
		pat = cmd;					/* note the start of the regexp */
		while (*cmd)                /* find the end of the regexp */
		{
			if (cmd[0] == delimiter && cmd[-1] != '\\')
			{
				*cmd++ = NUL;
				break;
			}
			++cmd;
		}

		sub = cmd;					/* note the start of the substitution */
		for (;;)                    /* find the end of the substitution */
		{
			if (*cmd == NUL)        /* no end delimiter */
				break;
			if (cmd[0] == delimiter && cmd[-1] != '\\')
			{
				*cmd++ = NUL;
				break;
			}
			cmd++;
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
	do_all = FALSE;
	do_ask = FALSE;
	while (*cmd)
	{
		if (*cmd == 'g')
			do_all = TRUE;
		else if (*cmd == 'c')
			do_ask = TRUE;
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
			emsg("zero count");
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
			emsg("Trailing characters");
			return;
		}
	}

	if ((prog = myregcomp(pat)) == NULL)
	{
		emsg(emsg_inval);
		return;
	}

	for (lnum = lp; lnum <= up && !got_int; ++lnum)
	{
		ptr = nr2ptr(lnum);
		if (regexec(prog, ptr, (int)TRUE))  /* a match on this line */
		{
			char		   *ns, *sns, *p, *prevp, *oldp;
			bool_t		did_sub = FALSE;

#ifdef LATTICE
			oldp = NULL;	/* Lattice gives an error message without this */
#endif
			if (nsubs == 0)
					setpcmark();
			/*
			 * Save the line that was last changed for the final cursor
			 * position (just like the real vi).
			 */
			Curpos.lnum = lnum;

			prevp = p = ptr;
			do
			{
				Curpos.col = prog->startp[0] - ptr;
				/*
				 * First match empty string does not count, except for first match.
				 * This reproduces the strange vi behaviour.
				 * This also catches endless loops.
				 */
				if (did_sub && p == oldp && p == prog->endp[0])
				{
					++p;
					goto skip2;
				}
				if (do_ask)
				{
						cursupdate();
						updateScreen(NOT_VALID);
						smsg("replace by %s (y/n/q)? ", sub);
						windgoto(Cursrow, Curscol);
						if ((i = vgetc()) == 'q')
						{
							got_int = TRUE;
							break;
						}
						else if (i != 'y')
							goto skip;
				}

						/* get length of substitution part */
				sublen = regsub(prog, sub, ptr, 0, (int)P(P_MAGIC));
				if (did_sub == FALSE)
				{
					/*
					 * Get some space for a temporary buffer to do the substitution
					 * into.
					 */
					if ((sns = alloc((unsigned)(strlen(ptr) + sublen + 5))) == NULL)
						goto outofmem;
					*sns = NUL;
					did_sub = TRUE;
				}
				else
				{
					/*
					 * extend the temporary buffer to do the substitution into.
					 */
					if ((ns = alloc((unsigned)(strlen(sns) + strlen(prevp) + sublen + 1))) == NULL)
						goto outofmem;
					strcpy(ns, sns);
					free(sns);
					sns = ns;
				}

				for (ns = sns; *ns; ns++)
					;
				/*
				 * copy up to the part that matched
				 */
				while (prevp < prog->startp[0])
					*ns++ = *prevp++;

				regsub(prog, sub, ns, 1, (int)P(P_MAGIC));
				nsubs++;

				prevp = prog->endp[0];	/* remember last copied character */
				/*
				 * continue searching after the match
				 * prevent endless loop with patterns that match empty strings,
				 * e.g. :s/$/pat/g or :s/[a-z]* /(&)/g
				 */
skip:
				p = prog->endp[0];
				oldp = p;
				if (*p == NUL)      /* end of line: quit here */
					break;

skip2:
				breakcheck();

			} while (!got_int && do_all && regexec(prog, p, (int)FALSE));

			if (did_sub)
			{
					/*
					 * copy the rest of the line, that didn't match
					 */
					strcat(sns, prevp);

					if ((ptr = save_line(sns)) != NULL)
							u_savesub(lnum, replaceline(lnum, ptr));

					free(sns);          /* free the temp buffer */
					++nlines;
			}
		}
		breakcheck();
	}

outofmem:
	if (nsubs)
	{
		CHANGED;
		updateScreen(NOT_VALID);        /* need this to update LineSizes */
		beginline((bool_t)TRUE);
		if (nsubs >= P(P_RP))
			smsg("%s%ld substitution%s on %ld line%s",
								got_int ? "(Interrupted) " : "",
								nsubs, plural(nsubs),
								(long)nlines, plural((long)nlines));
		else if (got_int)
				msg(emsg_interrupted);
		else if (do_ask)
				msg("");
	}
	else if (got_int)
		msg(emsg_interrupted);
	else
		msg("No match");

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
	bool_t			match;

	if (global_busy)
	{
		emsg("Cannot do :global recursive");
		++global_busy;
		return;
	}

	delim = *cmd++; 			/* skip the delimiter */
	pat = cmd;

	while (*cmd)
	{
		if (cmd[0] == delim && cmd[-1] != '\\')
		{
			*cmd++ = NUL;
			break;
		}
		cmd++;
	}

	reg_ic = P(P_IC);           /* set "ignore case" flag appropriately */

	if ((prog = myregcomp(pat)) == NULL)
	{
		emsg(emsg_inval);
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
		if (type == 'g' && match || type == 'v' && !match)
		{
			setmarked(lnum);
			ndone++;
		}
		breakcheck();
	}

/*
 * pass 2: execute the command for each line that has been marked
 */
	if (got_int)
		msg("Interrupted");
	else if (ndone == 0)
		msg("No match");
	else
	{
		global_busy = 1;
		global_wait = 0;
		RedrawingDisabled = TRUE;
		old_lcount = line_count;
		while (!got_int && (lnum = firstmarked()) && global_busy == 1)
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
		if (global_wait)                /* wait for return */
			wait_return((bool_t)FALSE);
		screenclear();                  /* redraw */
		updateScreen(NOT_VALID);
		msgmore(line_count - old_lcount);
	}

	clearmarked();      /* clear rest of the marks */
	global_busy = 0;
	free((char *) prog);
}
