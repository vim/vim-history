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
 * cmdline.c: functions for reading in the command line and executing it
 */

#include "vim.h"
#include "cmdtab.h"
#include "ops.h"			/* included because we call functions in ops.c */

#ifdef LATTICE
# define mktemp(a)	tmpnam(a)
#endif

/*
 * the history list of alternate files
 */
#define NUMALTFILES 20

static char    *altfiles[NUMALTFILES];	/* alternate files */
static linenr_t altlnum[NUMALTFILES];	/* line # in alternate file */
static linenr_t doecmdlnum = 0;			/* line # in new file for doecmd() */

static char 	invarg[] = "Invalid argument";
static char 	nomore[] = "No more files to edit";
static char 	nowrtmsg[] = "No write since last change (use ! to override)";
static char 	toolong[] = "Command too long";
static char 	invcmd[] = "Invalid command";
char 			notopen[] = "Cannot open file";

/*
 * variables shared between getcmdline() and redrawcmdline()
 */
static int		 cmdlen;		/* number of chars on command line */
static int		 cmdpos;		/* current cursor position */
static int		 cmdslen;		/* lenght of command line on screen */
static int		 cmdspos;		/* cursor position on screen */
static int		 cmdredraw; 	/* max. number of lines of the command - 1 */
static int		 cmdfirstc; 	/* ':', '/' or '?' */
static u_char	*cmdbuff;		/* pointer to command line buffer */

/*
 * The next two variables contain the bounds of any range given in a command.
 * They are set by docmdline().
 */
static linenr_t 	line1, line2;
static bool_t		forceit;
static int			regname;

static void		cmdchecklen __ARGS((void));
static void		cursorcmd __ARGS((void));
static u_char	*DoOneCmd __ARGS((u_char *));
static void		dobang __ARGS((int, u_char *));
static bool_t	autowrite __ARGS((void));
static int		dowrite __ARGS((u_char *, bool_t));
static bool_t	doecmd __ARGS((char *));
static void		doshell __ARGS((char *, bool_t));
static void		dofilter __ARGS((u_char *, bool_t, bool_t));
static bool_t	check_readonly __ARGS((void));
static bool_t	check_changed __ARGS((bool_t));
static bool_t	check_fname __ARGS((void));
static bool_t	check_more __ARGS((void));
#ifdef WILD_CARDS
static char		*ExpandOne __ARGS((u_char *, int, bool_t));
#endif
static void		showmatches __ARGS((char *, int));
static linenr_t get_address __ARGS((u_char **, linenr_t));

extern char		*mktemp __ARGS((char *));

extern int global_busy, global_wait;	/* shared with cmdsearch.c */

/*
 * variable shared with quickfix.c
 */
extern int qf_index;

/*
 * getcmdline() - accept a command line starting with ':', '!', '/', or '?'
 *
 * For searches the optional matching '?' or '/' is removed.
 */

	bool_t
getcmdline(firstc, buff)
	int			firstc; 	/* either ':', '/', or '?' */
	u_char		*buff;	 	/* buffer for command string */
{
	register u_char 	c;
	register int		i;
			 int		retval;
			 int		hiscnt;				/* current history line in use */
	static	 char 		**history = NULL;	/* history table */
	static	 int		hislen = 0; 		/* actual lengt of history table */
			 int		newlen;				/* new length of history table */
	static	 int		hisidx = -1;		/* last entered entry */
			 char		**temp;
			 int		j;
			 int		gotesc = FALSE;		/* TRUE when last char typed was <ESC> */


/*
 * set some variables for redrawcmd()
 */
	cmdfirstc = firstc;
	cmdbuff = buff;
	cmdlen = cmdpos = 0;
	cmdslen = cmdspos = 1;
	cmdredraw = 0;
	State = CMDLINE;
	gotocmdline(YES, firstc);

/*
 * if size of history table changed, reallocate it
 */
	newlen = P(P_HI);
	if (newlen != hislen)						/* history length changed */
	{
		temp = (char **)alloc((int)(newlen * sizeof(char *)));
		if (temp != NULL)
		{
				if (newlen > hislen)			/* array becomes bigger */
				{
						for (i = 0; i <= hisidx; ++i)
								temp[i] = history[i];
						j = i;
						for ( ; i <= newlen - (hislen - hisidx); ++i)
								temp[i] = NULL;
						for ( ; j < hislen; ++i, ++j)
								temp[i] = history[j];
				}
				else							/* array becomes smaller */
				{
						j = hisidx;
						hisidx = newlen - 1;
						for (i = hisidx; i >= 0; --i)
						{
								temp[i] = history[j];
								if (--j < 0)
										j = hislen - 1;
						}
				}
				free(history);
				history = temp;
				hislen = newlen;
		}
	}
	hiscnt = hislen;			/* set hiscnt to impossible history value */

	/* collect the command string, handling '\b', @ and much more */
	for (;;)
	{
		cursorcmd();	/* set the cursor on the right spot */
		c = vgetc();

		if (c == '\n' || c == '\r')
		{
				outchar('\r');
				flushbuf();
				break;
		}
		if (c != ESC)
			gotesc = FALSE;
		switch (c)
		{
		case BS:
		case DEL:
				/*
				 * delete current character is the same as backspace on next
				 * character, except at end of line
				 */
				if (c == DEL && cmdpos != cmdlen)
					++cmdpos;
				if (cmdpos > 0)
				{
					--cmdpos;
					--cmdlen;
					for (i = cmdpos; i < cmdlen; ++i)
						buff[i] = buff[i + 1];
					redrawcmd();
				}
				else if (cmdlen == 0)
				{
					retval = FALSE;
					msg("");
					goto returncmd; 	/* back to cmd mode */
				}
				continue;

/*		case '@':	only in very old vi */
		case CTRL('U'):
clearline:
				cmdpos = 0;
				cmdlen = 0;
				cmdslen = 1;
				cmdspos = 1;
				gotocmdline(YES, firstc);
				continue;

		case ESC:
#ifndef WILD_CARDS
				retval = FALSE;
				msg("");
				goto returncmd; 	/* back to cmd mode */
#else
			{
				char *pp;
				int oldlen;
				int difflen;

				/*
				 * hitting <ESC> twice means: abandon command line
				 */
				if (gotesc)
				{
					retval = FALSE;
					msg("");
					goto returncmd; 	/* back to cmd mode */
				}
				gotesc = TRUE;
				for (i = cmdpos; i > 0 && buff[i - 1] != ' '; --i)
					;
				oldlen = cmdpos - i;
				if ((pp = ExpandOne(&buff[i], oldlen, (bool_t)FALSE)) != NULL)
				{
					if (cmdlen + (difflen = strlen(pp) - oldlen) > CMDBUFFSIZE - 4)
						emsg(toolong);
					else
					{
						strncpy((char *)&buff[cmdpos + difflen], (char *)&buff[cmdpos], (size_t)(cmdlen - cmdpos));
						strncpy((char *)&buff[i], pp, strlen(pp));
						cmdlen += difflen;
						cmdpos += difflen;
					}
					free(pp);
				}
				redrawcmd();
				continue;
			}

		case CTRL('D'):
			{
				for (i = cmdpos; i > 0 && buff[i - 1] != ' '; --i)
						;
				showmatches((char *)&buff[i], cmdpos - i);

				redrawcmd();
				continue;
			}
#endif

		case K_RARROW:
		case K_SRARROW:
				do
				{
						if (cmdpos >= cmdlen)
								break;
						cmdspos += charsize(buff[cmdpos]);
						++cmdpos;
				}
				while (c == K_SRARROW && buff[cmdpos] != ' ');
				continue;

		case K_LARROW:
		case K_SLARROW:
				do
				{
						if (cmdpos <= 0)
								break;
						--cmdpos;
						cmdspos -= charsize(buff[cmdpos]);
				}
				while (c == K_SLARROW && buff[cmdpos - 1] != ' ');
				continue;

		case K_UARROW:
		case K_DARROW:
				i = hiscnt;
				if (c == K_UARROW)
				{
						if (hiscnt == hislen)
								hiscnt = hisidx;
						else if (hiscnt == 0 && hisidx != hislen - 1)
								hiscnt = hislen - 1;
						else if (hiscnt != hisidx + 1)
								--hiscnt;
				}
				else
				{
						if (hiscnt == hisidx)	/* on last entry, clear the line */
						{
								hiscnt = hislen;
								goto clearline;
						}
						if (hiscnt == hislen)	/* not on a history line, nothing to do */
								continue;
						if (hiscnt == hislen - 1)
								hiscnt = 0;
						else
								++hiscnt;
				}
				if (hiscnt < 0 || history[hiscnt] == NULL)
						hiscnt = i;
				else
				{
						strcpy((char *)buff, history[hiscnt]);
						cmdpos = cmdlen = strlen((char *)buff);
						redrawcmd();
				}
				continue;

		case CTRL('V'):
				outchar('^');
				outtrans((char *)(buff + cmdpos), cmdlen - cmdpos);
				++cmdslen;
				cmdchecklen();
				--cmdslen;
				cursorcmd();
				State = NOMAPPING;		/* no mapping for the next char */
				c = vgetc();
				State = CMDLINE;
		}

		/* we come here if we have entered a normal character */
		if (cmdlen < CMDBUFFSIZE - 2)
		{
				for (i = cmdlen++; i > cmdpos; --i)
						buff[i] = buff[i - 1];
				buff[cmdpos] = c;
				outtrans((char *)(buff + cmdpos), cmdlen - cmdpos);
				++cmdpos;
				i = charsize(c);
				cmdslen += i;
				cmdspos += i;
		}
		cmdchecklen();
	}
	buff[cmdlen] = NUL;

	if (++hisidx == hislen)
		hisidx = 0;
	free(history[hisidx]);
	history[hisidx] = strsave((char *)buff);
	retval = TRUE;

returncmd:
	if (cmdredraw)
	{
		screenclear();
		updateScreen(NOT_VALID);
	}
	State = NORMAL;
	script_winsize_pp();
	return retval;
}

/*
 * Check if the command line spans more than one screen line.
 * The maximum number of lines is remembered.
 */
	static void
cmdchecklen()
{
		if (cmdslen / Columns > cmdredraw)
				cmdredraw = cmdslen / Columns;
}

/*
 * this fuction is called when the screen size changes
 */
	void
redrawcmdline()
{
		cmdredraw = 0;
		redrawcmd();
		cursorcmd();
}

/*
 * Redraw what is currently on the command line.
 */
	void
redrawcmd()
{
	register int i;

	windgoto(Rows - 1 - cmdredraw, 0);
	outchar(cmdfirstc);
	cmdslen = 1;
	cmdspos = 1;
	outtrans((char *)cmdbuff, cmdlen);
	for (i = 0; i < cmdlen; )
	{
		cmdslen += charsize(cmdbuff[i]);
		if (++i == cmdpos)
				cmdspos = cmdslen;
	}
	for (i = (cmdredraw + 1) * Columns - cmdslen; --i > 0; )
		outchar(' ');
	cmdchecklen();
}

	static void
cursorcmd()
{
	windgoto(Rows - 1 - cmdredraw + (cmdspos / Columns), cmdspos % Columns);
}

/*
 * docmdline(): execute an Ex command line
 *
 * 1. If no line given, get one.
 * 2. Split up in parts separated with '|'.
 *
 * This function may be called recursively!
 */
	void
docmdline(cmdline)
	u_char		*cmdline;
{
	u_char		buff[CMDBUFFSIZE];		/* command line */
	u_char		*nextcomm;

/*
 * 1. If no line given: get one.
 */
	if (cmdline == NULL)
	{
		if (!getcmdline(':', buff))
				return;
	}
	else
	{
		if (strlen((char *)cmdline) > CMDBUFFSIZE - 2)
		{
				emsg(toolong);
				return;
		}
		/* Make a copy of the command so we can mess with it. */
		strcpy((char *)buff, (char *)cmdline);
	}

/*
 * 2. Loop for each '|' separated command.
 *    DoOneCmd will set nextcommand to NULL if there is no trailing '|'.
 */
	for (;;)
	{
		nextcomm = DoOneCmd(buff);
		if (nextcomm == NULL)
			break;
		strcpy((char *)buff, (char *)nextcomm);
	}
}

/*
 * Execute one Ex command.
 *
 * 2. skip comment lines and leading space
 * 3. parse range
 * 4. parse command
 * 5. parse arguments
 * 6. switch on command name
 *
 * This function may be called recursively!
 */
	static u_char *
DoOneCmd(buff)
	u_char *buff;
{
	u_char				cmdbuf[CMDBUFFSIZE];	/* for '%' and '#' expansion */
	u_char				c;
	register u_char		*p;
	char				*q;
	u_char				*cmd, *arg;
	int 				i;
	int					cmdidx;
	int					argt;
	register linenr_t	lnum;
	long				n;
	int					addr_count;	/* number of address specifications */
	FPOS				pos;
	bool_t				append = FALSE;			/* write with append */
	u_char				*nextcomm;


/*
 * 2. skip comment lines and leading space, colons or bars
 */
	for (cmd = buff; *cmd && strchr(" \t:|", *cmd) != NULL; cmd++)
		;

	nextcomm = NULL;		/* default: no next command */
	if (strchr("#\"", *cmd) != NULL)	/* ignore comment and empty lines */
		goto doend;

/*
 * 3. parse a range specifier of the form: addr [,addr] [;addr] ..
 *
 * where 'addr' is:
 *
 * %		  (entire file)
 * $  [+-NUM]
 * 'x [+-NUM] (where x denotes a currently defined mark)
 * .  [+-NUM]
 * [+-NUM]..
 * NUM
 *
 * The cmd pointer is updated to point to the first character following the
 * range spec. If an initial address is found, but no second, the upper bound
 * is equal to the lower.
 */

	addr_count = 0;
	--cmd;
	do {
		++cmd;							/* skip ',' or ';' */
		line1 = line2;
		line2 = Curpos.lnum;			/* default is current line number */
		skipspace((char **)&cmd);
		lnum = get_address(&cmd, Curpos.lnum);
		if (lnum == INVLNUM)
		{
			if (*cmd == '%')            /* '%' - all lines */
			{
				++cmd;
				line1 = 1;
				line2 = line_count;
				++addr_count;
			}
		} else
			line2 = lnum;
		addr_count++;

		if (*cmd == ';')
		{
			if (line2 == 0)
				Curpos.lnum = 1;
			else
				Curpos.lnum = line2;
		}
	} while (*cmd == ',' || *cmd == ';');

	/* One address given: set start and end lines */
	if (addr_count == 1) {
		line1 = line2;
		/* ... but only implicit: really no address given */
		if (lnum == INVLNUM) {
			addr_count = 0;
		}
	}

	if (line1 > line2 || line2 > line_count)
	{
		emsg("Invalid range");
		goto doend;
	}

/*
 * 4. parse command
 */

	skipspace((char **)&cmd);

	/*
	 * If we got a line, but no command, then go to the line.
	 */
	if (*cmd == NUL || *cmd == '"' || *cmd == '|' && (nextcomm = cmd))
	{
		if (addr_count != 0)
		{
			if (line2 == 0)
				Curpos.lnum = 1;
			else
				Curpos.lnum = line2;
			Curpos.col = 0;
			cursupdate();
		}
		goto doend;
	}

	/*
	 * isolate the command and search for it in the command table
	 */
	p = cmd;
	if (*cmd != 'k')
		while (isalpha(*p))
			++p;
	if (p == cmd && strchr("!=><&k", *p) != NULL)	/* non-alpha or 'k' command */
		++p;
	i = p - cmd;

	for (cmdidx = 0; cmdidx < CMD_SIZE; ++cmdidx)
		if (strncmp(cmdnames[cmdidx].cmd_name, (char *)cmd, (size_t)i) == 0)
			break;

	if (i == 0 || cmdidx == CMD_SIZE)
	{
		emsg(invcmd);
		goto doend;
	}

	if (*p == '!')					/* forced commands */
	{
		++p;
		forceit = TRUE;
	}
	else
		forceit = FALSE;

/*
 * 5. parse arguments
 */
	argt = cmdnames[cmdidx].cmd_argt;

	if (!(argt & RANGE) && addr_count)
	{
		emsg("No range allowed");
		goto doend;
	}

	if (!(argt & ZEROR))			/* zero in range not allowed */
	{
		if (line1 == 0)
			line1 = 1;
		if (line2 == 0)
			line2 = 1;
	}

	arg = p;						/* remember start of argument */
	skipspace((char **)&arg);

	if ((argt & NEEDARG) && *arg == NUL)
	{
		emsg("Argument required");
		goto doend;
	}

	/*
	 * check for '|' to separate commands and '"' to start comments
	 */
	if (argt & TRLBAR)
	{
		while (*p)
		{
			if (*p == '"' && !(argt & NOTRLCOM) || *p == '|')
			{						/* remove the backslash or ^V */
				if (*(p - 1) == '\\' || *(p - 1) == CTRL('V'))
				{
					strcpy((char *)p - 1, (char *)p);
					--p;
				}
				else
				{
					if (*p == '|')
						nextcomm = p + 1;
					*p = NUL;
					break;
				}
			}
			++p;
		}
	}

	if ((argt & DFLALL) && addr_count == 0)
	{
		line1 = 1;
		line2 = line_count;
	}

	regname = 0;
		/* accept numbered register only when no count allowed (:put) */
	if ((argt & REGSTR) && (isalpha(*arg) || !(argt & COUNT) && isdigit(*arg)))
	{
		regname = *arg;
		++arg;
		skipspace((char **)&arg);
	}

	if ((argt & COUNT) && isdigit(*arg))
	{
		i = getdigits((char **)&arg);
		skipspace((char **)&arg);
		if (i <= 0)
		{
			emsg("zero count");
			goto doend;
		}
		line1 = line2;
		line2 += i - 1;
	}

	if (!(argt & EXTRA) && strchr("|\"#", *arg) == NULL)	/* no arguments allowed */
	{
		emsg("Trailing characters");
		goto doend;
	}

	if (cmdidx == CMD_write && *arg == '>' && *(arg + 1) == '>')	/* append */
	{
		arg += 2;
		skipspace((char **)&arg);
		append = TRUE;
	}

	/*
	 * change '%' to Filename, '#' to altfile
	 */
	if (argt & XFILE)
	{
		for (p = arg; *p; ++p)
		{
			c = *p;
			if (c != '%' && c != '#')	/* nothing to expand */
				continue;
			if (*(p - 1) == '\\')		/* remove escaped char */
			{
				strcpy((char *)p - 1, (char *)p);
				--p;
				continue;
			}

			n = 1;				/* length of what we expand */
			if (c == '%')
			{
				if (check_fname())
						goto doend;
				q = Filename;
			}
			else
			{
				q = (char *)p + 1;
				i = getdigits(&q);
				n = q - p;

				if (i >= NUMALTFILES || altfiles[i] == NULL)
				{
						emsg("no alternate file");
						goto doend;
				}
				doecmdlnum = altlnum[i];
				q = altfiles[i];
			}
			i = strlen((char *)arg) + strlen(q) + 3;
			if (nextcomm)
				i += strlen((char *)nextcomm);
			if (i > CMDBUFFSIZE)
			{
				emsg(toolong);
				goto doend;
			}
			/*
			 * we built the new argument in cmdbuf[], then copy it back to buff[]
			 */
			*p = NUL;
			strcpy((char *)cmdbuf, (char *)arg);
			strcat((char *)cmdbuf, q);
			i = strlen((char *)cmdbuf);
			strcat((char *)cmdbuf, (char *)p+n);
			p = buff + i - 1;
			if (nextcomm)
			{
				i = strlen((char *)cmdbuf) + 1;
				strcpy((char *)cmdbuf + i, (char *)nextcomm);
				nextcomm = buff + i;
			}
			strcpy((char *)buff, (char *)cmdbuf);
			arg = buff;
		}
#ifdef WILD_CARDS
		if (argt & NOSPC)		/* one file argument: expand wildcards */
		{
			for (p = arg; *p; ++p)
				if (strchr("*?[{}`", *p) != NULL)
					break;
			if (*p)					/* has a wildcard */
			{
				if ((p = (u_char *)ExpandOne(arg, (int)strlen((char *)arg), (bool_t)TRUE)) == NULL)
					goto doend;
				if (strlen((char *)p) + arg - buff < CMDBUFFSIZE - 2)
					strcpy((char *)arg, (char *)p);
				else
					emsg(toolong);
				free(p);
			}
		}
#endif
	}

/*
 * 6. switch on command name
 */
	switch (cmdidx)
	{
		case CMD_quit:
				if (check_changed((bool_t)FALSE) || check_more())
						break;
				getout(0);

		case CMD_xit:
		case CMD_wq:
				if ((cmdidx == CMD_wq || Changed) && (check_readonly() || !dowrite(arg, (bool_t)FALSE)))
					break;
				if (check_more())
					break;
				getout(0);

		case CMD_args:
				gotocmdline((bool_t)TRUE, NUL);
				for (i = 0; i < numfiles; ++i)
				{
					if (i == curfile)
						outchar('[');
					outstr(files[i]);
					if (i == curfile)
						outchar(']');
					outchar(' ');
				}
				outchar('\n');
				wait_return((bool_t)TRUE);
				break;

		case CMD_next:
				if (check_changed((bool_t)TRUE))
						break;
				if (*arg != NUL)		/* redefine file list */
				{
					if (doarglist((char *)arg))
						break;
					i = 0;
				}
				else
				{
					if (addr_count == 0)
						i = curfile + 1;
					else
						i = curfile + line2;
				}

donextfile:		if (i < 0 || i >= numfiles)
				{
					emsg(nomore);
					break;
				}
				if (check_changed((bool_t)TRUE))
						break;
				curfile = i;
				doecmd(files[curfile]);
				break;

		case CMD_previous:
		case CMD_Next:
				if (addr_count == 0)
					i = curfile - 1;
				else
					i = curfile - line2;
				goto donextfile;

		case CMD_rewind:
				i = 0;
				goto donextfile;

		case CMD_write:
				if (*arg == '!')		/* input lines to shell command */
					dofilter(arg + 1, (bool_t)TRUE, (bool_t)FALSE);
				else
					dowrite(arg, append);
				break;

		case CMD_edit:
		case CMD_ex:
		case CMD_visual:
				doecmd((char *)arg);
				break;

		case CMD_file:
				if (*arg == NUL)
						fileinfo();
				else
				{
						setfname((char *)arg);
						filemess(Filename, "");
						maketitle();
				}
				break;

		case CMD_files:
				setmode(0);
				for (i = 0; i < NUMALTFILES; ++i)
				{
					if (altfiles[i])
					{
						sprintf(IObuff, "%2d \"%s\" line %ld\n", i, altfiles[i], (long)altlnum[i]);
						outstr(IObuff);
					}
					flushbuf();
				}
				setmode(1);
				wait_return((bool_t)TRUE);
				break;

		case CMD_read:
				if (forceit || *arg == '!' && ++arg)
				{
						dofilter(arg, (bool_t)FALSE, (bool_t)TRUE);			/* :r!cmd */
						break;
				}
				if (!u_save(line2, (linenr_t)(line2 + 1)))
						break;
				if (readfile((char *)arg, line2))
				{
					emsg(notopen);
					break;
				}
				updateScreen(NOT_VALID);
				CHANGED;
				break;

		case CMD_cd:
		case CMD_chdir:
				if (*arg == NUL)
				{
					if (dirname(IObuff, IOSIZE))
						msg(IObuff);
					else
						emsg("unknown");
				}
				else
				{
					if (chdir(arg))
						emsg("chdir unsuccesful");
				}
				break;

		case CMD_equal:
				smsg("line %ld", (long)line2);
				break;

		case CMD_print:
				setmode(0);			/* set cooked mode, so output can be halted */
				do
				{
					if (P(P_NU))
					{
						sprintf(IObuff, "%7ld ", (long)line1);
						outstr(IObuff);
					}
					prt_line(nr2ptr(line1));
					outchar('\n');
					flushbuf();
				} while (++line1 <= line2);
				setmode(1);

				if (global_busy)
					global_wait = 1;
				else
					wait_return((bool_t)TRUE);
				break;

		case CMD_shell:
				doshell(NULL, (bool_t)FALSE);
				break;

		case CMD_tag:
				dotag((char *)arg, 0, addr_count ? (int)line2 : 1);
				break;

		case CMD_pop:
				dotag("", 1, addr_count ? (int)line2 : 1);
				break;

		case CMD_tags:
				dotags();
				break;

		case CMD_marks:
				domarks();
				break;

		case CMD_jumps:
				dojumps();
				break;

		case CMD_set:
				doset((char *)arg);
				break;

		case CMD_map:
		case CMD_unmap:
				switch (domap(*cmd == 'u', (char *)arg, forceit ? INSERT : NORMAL))
				{
					case 1: emsg(invarg);
							break;
					case 2: emsg("no such mapping");
							break;
					case 3: emsg("ambiguous mapping");
							break;
				}
				break;

		case CMD_display:
				dodis();		/* display buffer contents */
				break;

		case CMD_help:
				help();
				break;

		case CMD_version:
				msg(longVersion);
				break;

		case CMD_winsize:
				line1 = getdigits((char **)&arg);
				skipspace((char **)&arg);
				line2 = getdigits((char **)&arg);
				set_winsize((int)line1, (int)line2);
				break;

		case CMD_delete:
		case CMD_yank:
		case CMD_rshift:
		case CMD_lshift:
				yankbuffer = regname;
				startop.lnum = line1;
				endop.lnum = line2;
				nlines = line2 - line1 + 1;
				mtype = MLINE;
				Curpos.lnum = line1;
				switch (cmdidx)
				{
				case CMD_delete:
					dodelete();
					break;
				case CMD_yank:
					doyank((bool_t)FALSE);
					break;
				case CMD_rshift:
					doshift(RSHIFT);
					break;
				case CMD_lshift:
					doshift(LSHIFT);
					break;
				}
				break;

		case CMD_put:
				yankbuffer = regname;
				Curpos.lnum = line2;
				doput(FORWARD, -1L);
				break;

		case CMD_t:
		case CMD_copy:		/* copy: first yank, then put */
		case CMD_move:		/* move: first delete, then put */
				n = get_address(&arg, line2);
				if (n == INVLNUM)
				{
					emsg("invalid address");
					break;
				}

				yankbuffer = 0;		/* use default yank buffer */
				startop.lnum = line1;
				endop.lnum = line2;
				nlines = line2 - line1 + 1;
				mtype = MLINE;
				Curpos.lnum = line1;
				if (cmdidx == CMD_move)
				{
					if (n > line2)		/* correct n for the deleted lines */
						n -= nlines;
					else if (n >= line1)
						n = line1 - 1;
					dodelete();
				}
				else
					doyank((bool_t)FALSE);

				if (n == 0)				/* put in above first line */
				{
					Curpos.lnum = 1;
					doput(BACKWARD, -1L);
				}
				else
				{
					Curpos.lnum = n;
					doput(FORWARD, -1L);
				}
				break;

		case CMD_and:
		case CMD_substitute:
				dosub(line1, line2, (char *)arg, &nextcomm);
				break;

		case CMD_join:
				Curpos.lnum = line1;
				if (line1 == line2)
				{
					if (line2 == line_count)
					{
						beep();
						break;
					}
					++line2;
				}
				dodojoin(line2 - line1 + 1, !forceit);
				break;

		case CMD_global:
				if (forceit)
					*cmd = 'v';
		case CMD_vglobal:
				doglob(*cmd, line1, line2, (char *)arg);
				break;

		case CMD_bang:
				dobang(addr_count, arg);
				break;

		case CMD_undo:
				u_undo(1);
				break;

		case CMD_source:
				if (forceit)	/* :so! read vi commands */
					openscript((char *)arg);
				else if (dosource((char *)arg))		/* :so read ex commands */
					emsg(notopen);
				break;

		case CMD_mkexrc:
				{
					FILE	*fd;

					if (*arg == NUL)
						arg = (u_char *)".exrc";
					if (!forceit && (fd = fopen((char *)arg, "r")) != NULL)
					{
						fclose(fd);
						emsg("file exists; use ! to overwrite");
						break;
					}

					if ((fd = fopen((char *)arg, "w")) == NULL)
					{
						emsg("cannot create file");
						break;
					}
					if (makemap(fd) || makeset(fd))
						emsg("error while writing");
					fclose(fd);
					break;
				}

		case CMD_cc:
					qf_jump(atoi((char *)arg));
					break;

		case CMD_cf:
					if (*arg == NUL)
						arg = (u_char *)PS(P_EF);
					if (!qf_init((char *)arg))
						qf_jump(0);
					break;

		case CMD_cl:
					qf_list();
					break;

		case CMD_cn:
					qf_jump(qf_index + 1);
					break;

		case CMD_cp:
					qf_jump(qf_index - 1);
					break;

		case CMD_cq:
					getout(4);

		case CMD_mark:
		case CMD_k:
					pos = Curpos;			/* save Curpos */
					Curpos.lnum = line2;
					Curpos.col = 0;
					setmark(*arg);			/* set mark */
					Curpos = pos;			/* restore Curpos */
					break;

#ifdef SETKEYMAP
		case CMD_setkeymap:
					set_keymap(arg);
					break;
#endif

		default:
					emsg(invcmd);
	}


doend:
	return nextcomm;
}

/*
 * handle the :! command.
 * We replace the extra bangs by the previously entered command and remember
 * the command.
 */
	static void
dobang(addr_count, arg)
	int addr_count;
	u_char *arg;
{
	static	char	*prevcmd = NULL;		/* the previous command */
	char			*t;
	char			*trailarg;
	int 			len;

	len = strlen((char *)arg) + 1;

	if (Changed)
		autowrite();
	/*
	 * try to find an embedded bang, like in :!<cmd> ! [args]
	 * (:!! is indicated by the 'forceit' variable)
	 */
	for (trailarg = (char *)arg; *trailarg && !isspace(*trailarg); ++trailarg)
		;
	skipspace(&trailarg);
	if (*trailarg == '!')
		*trailarg++ = NUL;
	else
		trailarg = NULL;

	if (forceit || trailarg != NULL)			/* use the previous command */
	{
		if (prevcmd == NULL)
		{
			emsg("No previous command");
			return;
		}
		len += strlen(prevcmd) * (trailarg != NULL && forceit ? 2 : 1);
	}

	if (len > CMDBUFFSIZE)
	{
		emsg("line too long");
		return;
	}
	if ((t = alloc(len)) == NULL)
		return;
	*t = NUL;
	if (forceit)
		strcpy(t, prevcmd);
	strcat(t, (char *)arg);
	if (trailarg != NULL)
	{
		strcat(t, prevcmd);
		strcat(t, trailarg);
	}
	free(prevcmd);
	prevcmd = t;

	if (addr_count == 0)
	{
		smsg(":!%s", prevcmd);
		doshell(prevcmd, (bool_t)FALSE); 				/* :! */
	}
	else
	{
		smsg(":%ld,%ld!%s", (long)line1, (long)line2, prevcmd);
		dofilter((u_char *)prevcmd, (bool_t)TRUE, (bool_t)TRUE);		/* :range! */
	}
}

	static bool_t
autowrite()
{
	if (!P(P_AW) || check_readonly() || check_fname())
		return FALSE;
	return (writeit(Filename, (linenr_t)1, line_count, 0));
}

	static int
dowrite(arg, append)
	u_char *arg;
	bool_t append;
{
	FILE *f;

	if (check_readonly())
		return FALSE;
	if (*arg == NUL)
	{
		if (check_fname())
			return FALSE;
		return (writeit(Filename, line1, line2, append));
	}
	if (!forceit && !append && !P(P_WA) && (f = fopen((char *)arg, "r")) != NULL)
	{								/* don't overwrite existing file */
			fclose(f);
			emsg("File exists (use ! to override)");
			return 0;
	}
	return (writeit((char *)arg, line1, line2, append));
}

	static bool_t
doecmd(arg)
	char		*arg;
{
	bool_t		setalt;
	char		*command = NULL;
	int			i;
	linenr_t	newlnum;

	newlnum = doecmdlnum;
	doecmdlnum = 0;						/* reset it for next time */

	if (*arg == '+')
	{
		++arg;
#if 0
		if (isdigit(*arg))				/* :e +num file */
			newlnum = getdigits(&arg);
		else if (*arg == NUL || isspace(*arg))			/* :e + file */
			newlnum = INVLNUM;
		else							/* :e +command file */
		{
			command = arg;
			while (*arg && !isspace(*arg))
				++arg;
			if (*arg)
				*arg++ = NUL;
		}
#endif
		if (isspace(*arg))
			command = "$";
		else
		{
			command = arg;
			while (*arg && !isspace(*arg))
				++arg;
		}
		if (*arg)
			*arg++ = NUL;
		
		skipspace(&arg);
	}

#ifdef AMIGA
	fname_case(arg);		/* set correct case for filename */
#endif
	FullName(arg, IObuff, IOSIZE);
	setalt = (*arg != NUL && (Filename == NULL || stricmp(IObuff, Filename)));
	if (setalt)
	{
		free(altfiles[NUMALTFILES - 1]);
		for (i = NUMALTFILES - 1; i > 0; --i)
		{
			altfiles[i] = altfiles[i - 1];
			altlnum[i] = altlnum[i - 1];
		}
		incrmarks();		/* increment file number for all jumpmarks */
		incrtags();			/* increment file number for all tags */
	}
	if (check_changed((bool_t)FALSE))
	{
		if (setalt)
		{
			altfiles[0] = strsave(arg);
			altlnum[0] = 1;
		}
		decrmarks();		/* decrement file number for jumpmarks in current file */
		decrtags();			/* decrement file number for tags in current file */
		return FALSE;
	}
	if (setalt)
	{
		altfiles[0] = Filename;
		Filename = NULL;
		altlnum[0] = Curpos.lnum;
		setfname(arg);
	}
	else if (newlnum == 0)
		newlnum = Curpos.lnum;
	maketitle();
	if (check_fname())
		return FALSE;

	/* clear mem and read file */
	freeall();
	filealloc();
	UNCHANGED;
	startscript();		/* re-start auto script file */

	readfile(Filename, (linenr_t)0);
	if (newlnum && command == NULL)
	{
		if (newlnum != INVLNUM)
			Curpos.lnum = newlnum;
		else
			Curpos.lnum = line_count;
		Curpos.col = 0;
	}
	updateScreen(NOT_VALID);
	if (command)
		docmdline(command);
	return TRUE;
}

	static void
doshell(cmd, redraw_now)
	char	*cmd;
	bool_t	redraw_now;
{
	gotocmdline((bool_t)FALSE, '\n');

	call_shell(cmd, 0);

	if (global_busy)
		global_wait = 1;
	else
		wait_return(redraw_now);
#ifdef AUX
	if (!Aux_Device && !redraw_now)
#else
	if (!redraw_now)
#endif
		outstr("\033[0 q"); 			/* get window size */
}

/*
 * dofilter: filter lines through a command given by the user
 *
 * We use temp files and the call_shell() routine here. This would normally
 * be done using pipes on a UNIX machine, but this is more portable to
 * the machines we usually run on. The call_shell() routine needs to be able
 * to deal with redirection somehow, and should handle things like looking
 * at the PATH env. variable, and adding reasonable extensions to the
 * command name given by the user. All reasonable versions of call_shell()
 * do this.
 * We use input redirection if do_in is TRUE.
 * We use output redirection if do_out is TRUE.
 */
	static void
dofilter(buff, do_in, do_out)
	u_char		*buff;
	bool_t		do_in, do_out;
{
		char		itmp[15];
		char		otmp[15];
		linenr_t 	linecount;

		if (*buff == NUL)		/* no filter command */
				return;
		linecount = line2 - line1 + 1;
		Curpos.lnum = line1;
		Curpos.col = 0;
		cursupdate();
		gotocmdline((bool_t)FALSE, '\n');

		/*
		 * 1. Form temp file names
		 * 2. Write the lines to a temp file
		 * 3. Run the filter command on the temp file
		 * 4. Read the output of the command into the buffer
		 * 5. Delete the original lines to be filtered
		 * 6. Remove the temp files
		 */

		strcpy(itmp, "t:viXXXXXX");
		strcpy(otmp, "t:voXXXXXX");

		if ((do_in && *mktemp(itmp) == NUL) || (do_out && *mktemp(otmp) == NUL))
		{
				emsg("Can't get temp file names");
				return;
		}

		if (do_in && !writeit(itmp, line1, line2, (bool_t)FALSE))
		{
				emsg("Can't create input temp file");
				return;
		}
		if (!do_out)
			outchar('\n');

		sprintf(IObuff, "%s %c %s %c %s", buff,
						do_in ? '<' : ' ', do_in ? itmp : "",
						do_out ? '>' : ' ', do_out ? otmp : "");

		call_shell(IObuff, 1);

		if (do_out)
		{
				if (!u_save((linenr_t)(line1 - 1), (linenr_t)(line2 + 1)))
				{
						linecount = 0;
						goto error;
				}
				if (readfile(otmp, line2))
				{
						emsg("Can't read filter output");
						linecount = 0;
						goto error;
				}

				if (do_in)
						delline(linecount, (bool_t)TRUE);
		}
		else
		{
error:
				if (global_busy)
					global_wait = 1;
				else
					wait_return((bool_t)FALSE);
		}
		screenclear();
		updateScreen(NOT_VALID);

		if (linecount > P(P_RP))
		{
				if (!do_in && do_out)
						msgmore(linecount);
				else
						smsg("%ld lines filtered", (long)linecount);
		}
#ifdef AMIGA
		unlink(itmp);
		unlink(otmp);
#else
		remove(itmp);
		remove(otmp);
#endif
		return;
	}

/* 
 * Redefine the argument list to 'str'.
 * Return TRUE for failure.
 */
doarglist(str)
	char *str;
{
	int new_numfiles = 0;
	char **new_files = NULL;
	int exp_numfiles;
	char **exp_files;
	char **t;
	char *p;
	bool_t inquote;
	int i;

	while (*str)
	{
		/*
		 * create a new entry in new_files[]
		 */
		t = (char **)alloc((unsigned)(sizeof(char *) * (new_numfiles + 1)));
		if (t != NULL)
			for (i = new_numfiles; --i >= 0; )
				t[i] = new_files[i];
		free(new_files);
		if (t == NULL)
			return TRUE;
		new_files = t;
		new_files[new_numfiles++] = str;

		/*
		 * isolate one argument, taking quotes
		 */
		inquote = FALSE;
		for (p = str; *str; ++str)
		{
			if (*str == '\\' && *(str + 1) != NUL)
				*p++ = *++str;
			else
			{
				if (!inquote && isspace(*str))
					break;
				if (*str == '"')
					inquote ^= TRUE;
				else
					*p++ = *str;
			}
		}
		skipspace(&str);
		*p = NUL;
	}
	
#ifdef WILD_CARDS
	if (ExpandWildCards(new_numfiles, new_files, &exp_numfiles, &exp_files, (bool_t)FALSE, (bool_t)TRUE) != 0)
	{
		emsg((char *)exp_files);
		return TRUE;
	}
	else if (exp_numfiles == 0)
	{
		emsg("No match");
		return TRUE;
	}
	FreeWild(numfiles, files);
	files = exp_files;
	numfiles = exp_numfiles;

#else
	files = new_files;
	numfiles = new_numfiles;
#endif

	return FALSE;
}

	void
gotocmdline(clr, firstc)
	bool_t			clr;
	int				firstc;
{
	windgoto(Rows - 1, 0);
	if (clr)
		outstr(T_EL);			/* clear the bottom line */
	if (firstc)
		outchar(firstc);
}

	static bool_t
check_readonly()
{
	if (!forceit && P(P_RO))
	{
		emsg("file is readonly");
		return TRUE;
	}
	return FALSE;
}

	static bool_t
check_changed(checkaw)
	bool_t	checkaw;
{
		if (!forceit && Changed && (!checkaw || !autowrite()))
		{
				emsg(nowrtmsg);
				return TRUE;
		}
		return FALSE;
}

	static bool_t
check_fname()
{
		if (Filename == NULL)
		{
				emsg("No file name");
				return TRUE;
		}
		return FALSE;
}

	static bool_t
check_more()
{
		if (!forceit && curfile + 1 < numfiles)
		{
				emsg("still more files to edit");
				return TRUE;
		}
		return FALSE;
}

/*
 * try to abandon current file and edit "fname"
 * return 1 for normal error, 2 for "not written" error, 0 for success
 */
getfile(fname, setpm)
	char	*fname;
	bool_t	setpm;
{
	int other;

	FullName(fname, IObuff, IOSIZE);
	if (Filename == NULL)
		other = TRUE;
	else
		other = stricmp(IObuff, Filename);	/* ignore upper/lower case difference */
	if (other && !forceit && Changed && !autowrite())
	{
		emsg(nowrtmsg);
		return 2;
	}
	if (setpm)
		setpcmark();
	if (!other || doecmd(fname))
		return 0;
	return 1;
}

/*
 * return TRUE if alternate file n is the same as the current file
 */
	bool_t
samealtfile(n)
	int			n;
{
	if (n < NUMALTFILES && altfiles[n] != NULL && Filename != NULL &&
					stricmp(altfiles[n], Filename) == 0)
		return TRUE;
	return FALSE;
}

/*
 * get alternate file n
 * set linenr to lnum or altlnum if lnum == 0
 * if (setpm) setpcmark
 * return 1 for failure, 0 for success
 */
	int
getaltfile(n, lnum, setpm)
	int			n;
	linenr_t	lnum;
	bool_t		setpm;
{
	if (n >= NUMALTFILES || altfiles[n] == NULL)
		return 1;
	if (lnum == 0)
		lnum = altlnum[n];		/* altlnum may be changed by getfile() */
	if (!getfile(altfiles[n], setpm))
	{
		Curpos.lnum = lnum;
		Curpos.col = 0;
		return 0;
	}
	return 1;
}

/*
 * get name of alternate file
 */
 	char *
getaltfname(n)
	int n;
{
	if (n >= NUMALTFILES)
		return NULL;
	return altfiles[n];
}

#ifdef WILD_CARDS
/*
 * Do wildcard expansion on the string 'str' of length 'len'.
 * Return a pointer to alloced memory containing the new string.
 * Return NULL for failure.
 */
	static char *
ExpandOne(str, len, list_notfound)
	u_char *str;
	int	  len;
	bool_t	list_notfound;
{
	char *pp;
	char *ss = NULL;
	char **cmd_files = NULL;	  /* list of input files */
	int  cmd_numfiles = 0;	  /* number of input files */
	int	 i, found;
	char *filesuf, *setsuf, *nextsetsuf;
	int	 filesuflen, setsuflen;

	if ((pp = strnsave((char *)str, len + 1)) == NULL)
		return NULL;
	pp[len] = '*';
	pp[len + 1] = NUL;

	if (ExpandWildCards(1, &pp, &cmd_numfiles, &cmd_files, (bool_t)FALSE, list_notfound) != 0)
		emsg((char *)cmd_files);
	else if (cmd_numfiles == 0)
		emsg("No match");
	else
	{
		found = 0;
		if (cmd_numfiles > 1)		/* more than one match; check suffixes */
		{
			found = -1;
			for (i = 0; i < cmd_numfiles; ++i)
			{
				if ((filesuf = strrchr(cmd_files[i], '.')) != NULL)
				{
					filesuflen = strlen(filesuf);
					for (setsuf = PS(P_SU); *setsuf; setsuf = nextsetsuf)
					{
						if ((nextsetsuf = strchr(setsuf + 1, '.')) == NULL)
							nextsetsuf = setsuf + strlen(setsuf);
						setsuflen = nextsetsuf - setsuf;
						if (filesuflen == setsuflen &&
									strncmp(setsuf, filesuf, (size_t)setsuflen) == 0)
							break;
					}
					if (*setsuf)				/* suffix matched: ignore file */
						continue;
				}
				if (found >= 0)
				{
					found = -1;
					break;
				}
				found = i;
			}
		}
		if (found < 0)
			emsg("Too many file names");
		else
			ss = strsave(cmd_files[found]);
	}
		
	FreeWild(cmd_numfiles, cmd_files);
	free(pp);
	return ss;
}

/*
 * show all filenames that match the string "file" with length "len"
 */
	static void
showmatches(file, len)
	char *file;
	int	len;
{
	char *file_str;
	int num_files;
	char **files_found;
	int i, j, k, c;
	int maxlen;
	int lines;
	int columns;

	file_str = alloc(len + 2);
	if (file_str != NULL)
	{
		outchar('\n');
		flushbuf();

		/* find all files that match the description */
		strncpy(file_str, file, (size_t)len);
		file_str[len] = '*';
		file_str[len + 1] = 0;
		ExpandWildCards(1, &file_str, &num_files, &files_found, (bool_t)FALSE, (bool_t)FALSE);

		/* find the maximum length of the file names */
		maxlen = 0;
		for (i = 0; i < num_files; ++i)
		{
			j = strlen(files_found[i]);
			if (j > maxlen)
				maxlen = j;
		}

		/* compute the number of columns and lines for the listing */
		maxlen += 2;	/* two spaces between file names */
		columns = (Columns + 2) / maxlen;
		if (columns < 1)
			columns = 1;
		lines = (num_files + columns - 1) / columns;

		/* list the files line by line */
		setmode(0);		/* allow output to be halted */
		for (i = 0; i < lines; ++i)
		{
			for (k = i; k < num_files; k += lines)
			{
				if (k > i)
					for (j = maxlen - strlen(files_found[k - lines]); --j >= 0; )
						outchar(' ');
				j = isdir(files_found[k]);	/* highlight directories */
				if (j)
#ifdef AMIGA
					outstr("\033[33m");		/* use other color (mosly red) */
#else
					outstr(T_TI);
#endif
				outstr(files_found[k]);
				if (j)
#ifdef AMIGA
					outstr("\033[0m");		/* use normal color */
#else
					outstr(T_TP);
#endif
			}
			outchar('\n');
			flushbuf();
		}
		free(file_str);
		FreeWild(num_files, files_found);
		setmode(1);

		State = HITRETURN;	/* prevent redraws because of window resizes */
		outstr("Press RETURN to continue");
		do {
			c = vgetc();
		} while (c != '\r' && c != '\n' && c != ' ');
		if (State == SETWINSIZE)	/* now do the resizing */
		{
			State = CMDLINE;
			set_winsize(0, 0);
		}
		State = CMDLINE;
	}
	screenclear();
	updateScreen(NOT_VALID);
}
#endif

/*
 * dosource: read the file "fname" and execute its lines as EX commands
 *
 * This function may be called recursively!
 */
dosource(fname)
	register char *fname;
{
	register FILE *fp;
	register char *s;

	if ((fp = fopen(fname, "r")) == NULL)
		return 1;

	while (fgets(IObuff, IOSIZE, fp) != NULL && !got_int)
	{
		s = IObuff + strlen(IObuff) - 1;
		if (*s == '\n')	/* remove trailing newline */
			*s = NUL;
		docmdline((u_char *)IObuff);
		breakcheck();
	}
	fclose(fp);
	return 0;
}

/*
 * get single EX address
 */
	static linenr_t
get_address(ptr, curpos_lnum)
	u_char		**ptr;
	linenr_t	curpos_lnum;
{
	int			c;
	int			i;
	long		n;
	u_char		*p;
	u_char  	*cmd;
	FPOS		pos;
	FPOS		*fp;
	linenr_t	lnum;

	cmd = *ptr;
	skipspace((char **)&cmd);
	lnum = INVLNUM;
	do
	{
		switch (*cmd)
		{
			case '.': 						/* '.' - Cursor position */
						++cmd;
						lnum = curpos_lnum;
						break;

			case '$': 						/* '$' - last line */
						++cmd;
						lnum = line_count;
						break;

			case '\'': 						/* ''' - mark */
						if (*++cmd == NUL || (fp = getmark(*cmd++, (bool_t)FALSE)) == NULL)
						{
							emsg("Unknown mark");
							goto error;
						}
						lnum = fp->lnum;
						break;

			case '/':
			case '?':						/* '/' or '?' - search */
						c = *cmd++;

						for (p = cmd; *p && (*p != c || *(p - 1) == '\\'); ++p)
							;
						i = *p;
						*p = NUL;	/* put a '\0' at the end of the pattern */
				
						/* search for the pattern */
						pos.lnum = curpos_lnum;
						pos.col = -1;	/* searchit() will increment the col */
						if (c == '/')
							++pos.lnum;
						if (searchit(&pos, c == '/' ? FORWARD : BACKWARD, (char *)cmd, (long)1, 0))
							lnum = pos.lnum;
				
						/* adjust command string pointer */
						cmd = p;
						if (i != NUL)
							++cmd;

						break;

			default:
						if (isdigit(*cmd))				/* absolute line number */
							lnum = getdigits((char **)&cmd);
		}
		
		while (*cmd == '-' || *cmd == '+')
		{
			if (lnum == INVLNUM)
				lnum = curpos_lnum;
			i = *cmd++;
			if (!isdigit(*cmd))	/* '+' is '+1', but '+0' is not '+1' */
				n = 1;
			else 
				n = getdigits((char **)&cmd);
			if (i == '-')
				lnum -= n;
			else
				lnum += n;
		}

		curpos_lnum = lnum;
	} while (*cmd == '/' || *cmd == '?');

error:
	*ptr = cmd;
	return lnum;
}
