/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * cmdline.c: functions for reading in the command line and executing it
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "cmdtab.h"
#include "ops.h"			/* included because we call functions in ops.c */
#include "fcntl.h"			/* for chdir() */

	/* TODO: remove this ugly global variable */
static linenr_t doecmdlnum = 0;			/* line # in new file for doecmd() */

/*
 * variables shared between getcmdline() and redrawcmdline()
 */
static int		 cmdlen;		/* number of chars on command line */
static int		 cmdpos;		/* current cursor position */
static int		 cmdspos;		/* cursor column on screen */
static int		 cmdfirstc; 	/* ':', '/' or '?' */
static char_u	*cmdbuff;		/* pointer to command line buffer */

/*
 * The next two variables contain the bounds of any range given in a command.
 * They are set by docmdline().
 */
static linenr_t 	line1, line2;

static int			forceit;
static int			regname;
static int			quitmore = 0;
static int  		cmd_numfiles = -1;	  /* number of files found by
													filename completion */

static void		putcmdline __ARGS((int, char_u *));
static void		cursorcmd __ARGS((void));
static int		ccheck_abbr __ARGS((int));
static char_u	*DoOneCmd __ARGS((char_u *));
static int		autowrite __ARGS((BUF *));
static int		buf_write_all __ARGS((BUF *));
static int		dowrite __ARGS((char_u *, int));
static int		doecmd __ARGS((char_u *, char_u *, char_u *, int));
static char_u	*getargcmd __ARGS((char_u **));
static char_u	*checknextcomm __ARGS((char_u *));
static void		domake __ARGS((char_u *));
static int		doarglist __ARGS((char_u *));
static int		check_readonly __ARGS((void));
static int		check_changed __ARGS((BUF *, int, int));
static int		check_changed_any __ARGS((int));
static int		check_more __ARGS((int));
static void		nextwild __ARGS((char_u *, int));
static void		showmatches __ARGS((char_u *, int));
static char_u	*addstar __ARGS((char_u *, int));
static linenr_t get_address __ARGS((char_u **));

/*
 * getcmdline() - accept a command line starting with ':', '!', '/', or '?'
 *
 * For searches the optional matching '?' or '/' is removed.
 *
 * Return OK if there is a commandline, FAIL if not
 */

	int
getcmdline(firstc, buff)
	int			firstc; 	/* either ':', '/', or '?' */
	char_u		*buff;	 	/* buffer for command string */
{
	register char_u 	c;
			 int		nextc = 0;
	register int		i;
			 int		retval;
			 int		hiscnt;				/* current history line in use */
	static	 char_u		**history = NULL;	/* history table */
	static	 int		hislen = 0; 		/* actual lengt of history table */
			 int		newlen;				/* new length of history table */
	static	 int		hisidx = -1;		/* last entered entry */
			 char_u		**temp;
			 char_u		*lookfor = NULL;	/* string to match */
			 int		j = -1;
			 int		gotesc = FALSE;		/* TRUE when last char typed was <ESC> */
			 int		do_abbr;			/* when TRUE check for abbr. */

/*
 * set some variables for redrawcmd()
 */
	cmdfirstc = firstc;
	cmdbuff = buff;
	cmdlen = cmdpos = 0;
	cmdspos = 1;
	State = CMDLINE;
	gotocmdline(TRUE, firstc);

/*
 * if size of history table changed, reallocate it
 */
	newlen = (int)p_hi;
	if (newlen != hislen)						/* history length changed */
	{
		if (newlen)
			temp = (char_u **)lalloc((long_u)(newlen * sizeof(char_u *)), TRUE);
		else
			temp = NULL;
		if (newlen == 0 || temp != NULL)
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
				for (i = newlen - 1; ; --i)
				{
					if (i >= 0)
						temp[i] = history[j];	/* copy newest entries */
					else
						free(history[j]);		/* remove older entries */
					if (--j < 0)
						j = hislen - 1;
					if (j == hisidx)
						break;
				}
				hisidx = newlen - 1;
			}
			free(history);
			history = temp;
			hislen = newlen;
		}
	}
	hiscnt = hislen;			/* set hiscnt to impossible history value */

#ifdef DIGRAPHS
	dodigraph(-1);				/* init digraph typahead */
#endif

	/* collect the command string, handling '\b', @ and much more */
	for (;;)
	{
		cursorcmd();	/* set the cursor on the right spot */
		if (nextc)		/* character remaining from CTRL-V */
		{
			c = nextc;
			nextc = 0;
		}
		else
		{
			c = vgetc();
			if (c == Ctrl('C'))
				got_int = FALSE;
		}

		if (lookfor && c != K_SDARROW && c != K_SUARROW)
		{
			free(lookfor);
			lookfor = NULL;
		}

		if (cmd_numfiles > 0 && !(c == p_wc && KeyTyped) && c != Ctrl('N') &&
						c != Ctrl('P') && c != Ctrl('A') && c != Ctrl('L'))
			(void)ExpandOne(NULL, FALSE, -2);	/* may free expanded file names */

#ifdef DIGRAPHS
		c = dodigraph(c);
#endif

		if (c == '\n' || c == '\r' || (c == ESC && !KeyTyped))
		{
			if (ccheck_abbr(c + 0x100))
				continue;
			outchar('\r');		/* show that we got the return */
			flushbuf();
			break;
		}

			/* hitting <ESC> twice means: abandon command line */
			/* wildcard expansion is only done when the key is really typed, not
			   when it comes from a macro */
		if (c == p_wc && !gotesc && KeyTyped)
		{
			if (cmd_numfiles > 0)	/* typed p_wc twice */
				nextwild(buff, 3);
			else					/* typed p_wc first time */
				nextwild(buff, 0);
			if (c == ESC)
				gotesc = TRUE;
			continue;
		}
		gotesc = FALSE;

		if (c == K_ZERO)		/* NUL is stored as NL */
			c = '\n';

		do_abbr = TRUE;			/* default: check for abbreviation */
		switch (c)
		{
		case BS:
		case DEL:
		case Ctrl('W'):
				/*
				 * delete current character is the same as backspace on next
				 * character, except at end of line
				 */
				if (c == DEL && cmdpos != cmdlen)
					++cmdpos;
				if (cmdpos > 0)
				{
					j = cmdpos;
					if (c == Ctrl('W'))
					{
						while (cmdpos && isspace(buff[cmdpos - 1]))
							--cmdpos;
						i = isidchar(buff[cmdpos - 1]);
						while (cmdpos && !isspace(buff[cmdpos - 1]) && isidchar(buff[cmdpos - 1]) == i)
							--cmdpos;
					}
					else
						--cmdpos;
					cmdlen -= j - cmdpos;
					i = cmdpos;
					while (i < cmdlen)
						buff[i++] = buff[j++];
					redrawcmd();
				}
				else if (cmdlen == 0 && c != Ctrl('W'))
				{
					retval = FAIL;
					msg_pos(-1, 0);
					msg_outchar(' ');	/* delete ':' */
					goto returncmd; 	/* back to cmd mode */
				}
				continue;

/*		case '@':	only in very old vi */
		case Ctrl('U'):
clearline:
				cmdpos = 0;
				cmdlen = 0;
				cmdspos = 1;
				redrawcmd();
				continue;

		case ESC:			/* get here if p_wc != ESC or when ESC typed twice */
		case Ctrl('C'):
				retval = FAIL;
				MSG("");
				goto returncmd; 	/* back to cmd mode */

		case Ctrl('D'):
			{
				for (i = cmdpos; i > 0 && buff[i - 1] != ' '; --i)
						;
				showmatches(&buff[i], cmdpos - i);

				redrawcmd();
				continue;
			}

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

		case Ctrl('B'):		/* begin of command line */
				cmdpos = 0;
				cmdspos = 1;
				continue;

		case Ctrl('E'):		/* end of command line */
				cmdpos = cmdlen;
				buff[cmdlen] = NUL;
				cmdspos = strsize(buff) + 1;
				continue;

		case Ctrl('A'):		/* all matches */
				nextwild(buff, 4);
				continue;

		case Ctrl('L'):		/* longest common part */
				nextwild(buff, 5);
				continue;

		case Ctrl('N'):		/* next match */
		case Ctrl('P'):		/* previous match */
				if (cmd_numfiles > 0)
				{
					nextwild(buff, (c == Ctrl('P')) ? 2 : 1);
					continue;
				}

		case K_UARROW:
		case K_DARROW:
		case K_SUARROW:
		case K_SDARROW:
				if (hislen == 0)		/* no history */
					continue;

				i = hiscnt;
			
					/* save current command string */
				if (c == K_SUARROW || c == K_SDARROW)
				{
					buff[cmdpos] = NUL;
					if (lookfor == NULL && (lookfor = strsave(buff)) == NULL)
						continue;

					j = STRLEN(lookfor);
				}
				for (;;)
				{
						/* one step backwards */
					if (c == K_UARROW || c == K_SUARROW || c == Ctrl('P'))
					{
						if (hiscnt == hislen)	/* first time */
							hiscnt = hisidx;
						else if (hiscnt == 0 && hisidx != hislen - 1)
							hiscnt = hislen - 1;
						else if (hiscnt != hisidx + 1)
							--hiscnt;
						else					/* at top of list */
							break;
					}
					else	/* one step forwards */
					{
						if (hiscnt == hisidx)	/* on last entry, clear the line */
						{
							hiscnt = hislen;
							goto clearline;
						}
						if (hiscnt == hislen)	/* not on a history line, nothing to do */
							break;
						if (hiscnt == hislen - 1)	/* wrap around */
							hiscnt = 0;
						else
							++hiscnt;
					}
					if (hiscnt < 0 || history[hiscnt] == NULL)
					{
						hiscnt = i;
						break;
					}
					if ((c != K_SUARROW && c != K_SDARROW) || hiscnt == i ||
							STRNCMP(history[hiscnt], lookfor, (size_t)j) == 0)
						break;
				}

				if (hiscnt != i)		/* jumped to other entry */
				{
					STRCPY(buff, history[hiscnt]);
					cmdpos = cmdlen = STRLEN(buff);
					redrawcmd();
				}
				continue;

		case Ctrl('V'):
				putcmdline('^', buff);
				c = get_literal(&nextc);	/* get next (two) character(s) */
				do_abbr = FALSE;			/* don't do abbreviation now */
				break;

#ifdef DIGRAPHS
		case Ctrl('K'):
				putcmdline('?', buff);
			  	c = vgetc();
				putcmdline(c, buff);
				c = getdigraph(c, vgetc());
				break;
#endif /* DIGRAPHS */
		}

		/* we come here if we have a normal character */

		if (do_abbr && !isidchar(c) && ccheck_abbr(c))
			continue;

		if (cmdlen < CMDBUFFSIZE - 2)
		{
				for (i = cmdlen++; i > cmdpos; --i)
						buff[i] = buff[i - 1];
				buff[cmdpos] = c;
				msg_outtrans(buff + cmdpos, cmdlen - cmdpos);
				++cmdpos;
				i = charsize(c);
				cmdspos += i;
		}
		msg_check();
	}
	retval = OK;				/* when we get here we have a valid command line */

returncmd:
	buff[cmdlen] = NUL;
	/*
	 * put line in history buffer
	 */
	if (cmdlen != 0)
	{
		if (hislen != 0)
		{
			if (++hisidx == hislen)
				hisidx = 0;
			free(history[hisidx]);
			history[hisidx] = strsave(buff);
		}
		if (firstc == ':')
		{
			free(last_cmdline);
			last_cmdline = strsave(buff);
		}
	}

	/*
	 * If the screen was shifted up, redraw the whole screen (later).
	 * If the line is too long, clear it, so ruler and shown command do
	 * not get printed in the middle of it.
	 */
	msg_check();
	State = NORMAL;
	return retval;
}

/*
 * put a character on the command line.
 * Used for CTRL-V and CTRL-K
 */
	static void
putcmdline(c, buff)
	int		c;
	char_u	*buff;
{
	char_u	buf[2];

	buf[0] = c;
	buf[1] = 0;
	msg_outtrans(buf, 1);
	msg_outtrans(buff + cmdpos, cmdlen - cmdpos);
	cursorcmd();
}

/*
 * this fuction is called when the screen size changes
 */
	void
redrawcmdline()
{
	msg_scrolled = 0;
	compute_cmdrow();
	redrawcmd();
	cursorcmd();
}

	void
compute_cmdrow()
{
	cmdline_row = lastwin->w_winpos + lastwin->w_height + lastwin->w_status_height;
}

/*
 * Redraw what is currently on the command line.
 */
	void
redrawcmd()
{
	register int	i;

	msg_start();
	msg_outchar(cmdfirstc);
	msg_outtrans(cmdbuff, cmdlen);
	msg_ceol();

	cmdspos = 1;
	for (i = 0; i < cmdlen && i < cmdpos; ++i)
		cmdspos += charsize(cmdbuff[i]);
}

	static void
cursorcmd()
{
	msg_pos(cmdline_row + (cmdspos / (int)Columns), cmdspos % (int)Columns);
	windgoto(msg_row, msg_col);
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text with
 * backspaces and the replacement string is inserted, followed by "c".
 */
	static int
ccheck_abbr(c)
	int c;
{
	if (p_paste || no_abbr)			/* no abbreviations or in paste mode */
		return FALSE;
	
	return check_abbr(c, cmdbuff, cmdpos, 0);
}

/*
 * docmdline(): execute an Ex command line
 *
 * 1. If no line given, get one.
 * 2. Split up in parts separated with '|'.
 *
 * This function may be called recursively!
 *
 * return FAIL if commandline could not be executed, OK otherwise
 */
	int
docmdline(cmdline)
	char_u		*cmdline;
{
	char_u		buff[CMDBUFFSIZE];		/* command line */
	char_u		*nextcomm;

/*
 * 1. If no line given: get one.
 */
	if (cmdline == NULL)
	{
		if (!getcmdline(':', buff))
			return FAIL;
	}
	else
	{
		if (STRLEN(cmdline) > (size_t)(CMDBUFFSIZE - 2))
		{
			emsg(e_toolong);
			return FAIL;
		}
		/* Make a copy of the command so we can mess with it. */
		STRCPY(buff, cmdline);
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
		STRCPY(buff, nextcomm);
	}
	return OK;
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
	static char_u *
DoOneCmd(buff)
	char_u *buff;
{
	char_u				cmdbuf[CMDBUFFSIZE];	/* for '%' and '#' expansion */
	char_u				c;
	register char_u		*p;
	char_u				*q;
	char_u				*cmd, *arg;
	char_u				*editcmd = NULL;		/* +command argument for doecmd */
	int 				i = 0;					/* init to shut up gcc */
	int					cmdidx;
	int					argt;
	register linenr_t	lnum;
	long				n;
	int					addr_count;	/* number of address specifications */
	FPOS				pos;
	int					append = FALSE;			/* write with append */
	int					usefilter = FALSE;		/* filter instead of file name */
	char_u				*nextcomm = NULL;		/* no next command yet */
	int					amount = 0;				/* for ":>" and ":<"; init for gcc */

	if (quitmore)
		--quitmore;		/* when not editing the last file :q has to be typed twice */
/*
 * 2. skip comment lines and leading space, colons or bars
 */
	for (cmd = buff; *cmd && strchr(" \t:|", *cmd) != NULL; cmd++)
		;

	if (*cmd == '"' || *cmd == NUL)	/* ignore comment and empty lines */
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
	do
	{
		++cmd;							/* skip ',' or ';' */
		line1 = line2;
		line2 = curwin->w_cursor.lnum;			/* default is current line number */
		skipspace(&cmd);
		lnum = get_address(&cmd);
		if (lnum == MAXLNUM)
		{
			if (*cmd == '%')            /* '%' - all lines */
			{
				++cmd;
				line1 = 1;
				line2 = curbuf->b_ml.ml_line_count;
				++addr_count;
			}
		}
		else
			line2 = lnum;
		addr_count++;

		if (*cmd == ';')
		{
			if (line2 == 0)
				curwin->w_cursor.lnum = 1;
			else
				curwin->w_cursor.lnum = line2;
		}
	} while (*cmd == ',' || *cmd == ';');

	/* One address given: set start and end lines */
	if (addr_count == 1)
	{
		line1 = line2;
			/* ... but only implicit: really no address given */
		if (lnum == MAXLNUM)
			addr_count = 0;
	}

/*
 * 4. parse command
 */

	skipspace(&cmd);

	/*
	 * If we got a line, but no command, then go to the line.
	 * If we find a '|' or '\n' we set nextcomm.
	 */
	if (*cmd == NUL || *cmd == '"' ||
			((*cmd == '|' || *cmd == '\n') &&
					(nextcomm = cmd + 1) != NULL))		/* just an assignment */
	{
		if (addr_count != 0)
		{
			/*
			 * strange vi behaviour: ":3" jumps to line 3
			 * ":3|..." prints line 3
			 */
			if (*cmd == '|')
			{
				cmdidx = CMD_print;
				goto cmdswitch;			/* UGLY goto */
			}
			if (line2 == 0)
				curwin->w_cursor.lnum = 1;
			else if (line2 > curbuf->b_ml.ml_line_count)
				curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
			else
				curwin->w_cursor.lnum = line2;
			curwin->w_cursor.col = 0;
			cursupdate();
		}
		goto doend;
	}

	/*
	 * Isolate the command and search for it in the command table.
	 * Exeptions:
	 * - the 'k' command can directly be followed by any character.
	 * - the 's' command can be followed directly by 'c', 'g' or 'r'
	 */
	if (*cmd == 'k')
	{
		cmdidx = CMD_k;
		p = cmd + 1;
	}
	if (*cmd == 's' && strchr("cgr", cmd[1]) != NULL)
	{
		cmdidx = CMD_substitute;
		p = cmd + 1;
	}
	else
	{
		p = cmd;
		while (isalpha(*p))
			++p;
		if (p == cmd && strchr("@!=><&~#", *p) != NULL)	/* non-alpha command */
			++p;
		i = (int)(p - cmd);

		for (cmdidx = 0; cmdidx < CMD_SIZE; ++cmdidx)
			if (STRNCMP(cmdnames[cmdidx].cmd_name, (char *)cmd, (size_t)i) == 0)
				break;
		if (i == 0 || cmdidx == CMD_SIZE)
		{
			emsg(e_invcmd);
			goto doend;
		}
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
		emsg(e_norange);
		goto doend;
	}

	if (line1 > line2 || line1 < 0 || line2 < 0  ||
					(!(argt & NOTADR) && line2 > curbuf->b_ml.ml_line_count))
	{
		emsg(e_invrange);
		goto doend;
	}

	if ((argt & NOTADR) && addr_count == 0)		/* default is 1, not cursor */
		line2 = 1;

	if (!(argt & ZEROR))			/* zero in range not allowed */
	{
		if (line1 == 0)
			line1 = 1;
		if (line2 == 0)
			line2 = 1;
	}

	/*
	 * for the :make command we insert the 'makeprg' option here,
	 * so things like % get expanded
	 */
	if (cmdidx == CMD_make)
	{
		if (STRLEN(p_mp) + STRLEN(p) + 2 >= (unsigned)CMDBUFFSIZE)
		{
			emsg(e_toolong);
			goto doend;
		}
		STRCPY(cmdbuf, p_mp);
		STRCAT(cmdbuf, " ");
		STRCAT(cmdbuf, p);
		STRCPY(buff,   cmdbuf);
		p = buff;
	}

	arg = p;						/* remember start of argument */
	skipspace(&arg);

	if ((argt & NEEDARG) && *arg == NUL)
	{
		emsg(e_argreq);
		goto doend;
	}

	if (cmdidx == CMD_write)
	{
		if (*arg == '>')						/* append */
		{
			if (*++arg != '>')				/* typed wrong */
			{
				EMSG("Use w or w>>");
				goto doend;
			}
			++arg;
			skipspace(&arg);
			append = TRUE;
		}
		else if (*arg == '!')					/* :w !filter */
		{
			++arg;
			usefilter = TRUE;
		}
	}

	if (cmdidx == CMD_read)
	{
		usefilter = forceit;					/* :r! filter if forceit */
		if (*arg == '!')						/* :r !filter */
		{
			++arg;
			usefilter = TRUE;
		}
	}

	if (cmdidx == CMD_lshift || cmdidx == CMD_rshift)
	{
		amount = 1;
		while (*arg == *cmd)		/* count number of '>' or '<' */
		{
			++arg;
			++amount;
		}
		skipspace(&arg);
	}

	/*
	 * Check for '|' to separate commands and '"' to start comments.
	 * Don't do this for ":read !cmd" and ":write !cmd".
	 */
	if ((argt & TRLBAR) && !usefilter)
	{
		p = arg;
		while (*p)
		{
			if (*p == Ctrl('V'))
			{
				if ((argt & USECTRLV) && p[1] != NUL)	/* skip CTRL-V and next char */
					++p;
				else					/* remove CTRL-V and skip next char */
					STRCPY(p, p + 1);
			}
			else if ((*p == '"' && !(argt & NOTRLCOM)) || *p == '|' || *p == '\n')
			{
				if (*(p - 1) == '\\')	/* remove the backslash */
				{
					STRCPY(p - 1, p);
					--p;
				}
				else
				{
					if (*p == '|' || *p == '\n')
						nextcomm = p + 1;
					*p = NUL;
					break;
				}
			}
			++p;
		}
		if (!(argt & NOTRLCOM))			/* remove trailing spaces */
			del_spaces(arg);
	}

	if ((argt & DFLALL) && addr_count == 0)
	{
		line1 = 1;
		line2 = curbuf->b_ml.ml_line_count;
	}

	regname = 0;
		/* accept numbered register only when no count allowed (:put) */
	if ((argt & REGSTR) && *arg != NUL && is_yank_buffer(*arg, FALSE) && !((argt & COUNT) && isdigit(*arg)))
	{
		regname = *arg;
		++arg;
		skipspace(&arg);
	}

	if ((argt & COUNT) && isdigit(*arg))
	{
		n = getdigits(&arg);
		skipspace(&arg);
		if (n <= 0)
		{
			emsg(e_zerocount);
			goto doend;
		}
		line1 = line2;
		line2 += n - 1;
	}

	if (!(argt & EXTRA) && strchr("|\"", *arg) == NULL)	/* no arguments allowed */
	{
		emsg(e_trailing);
		goto doend;
	}

	/*
	 * change '%' to curbuf->b_filename, '#' to curwin->w_altfile
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
				STRCPY(p - 1, p);
				--p;
				continue;
			}

			if (c == '%')				/* current file */
			{
				if (check_fname() == FAIL)
					goto doend;
				q = curbuf->b_xfilename;
				n = 1;					/* length of what we expand */
			}
			else						/* '#': alternate file */
			{
				q = p + 1;
				i = (int)getdigits(&q);
				n = q - p;				/* length of what we expand */

				if (filelist_name_nr(i, &q, &doecmdlnum) == FAIL)
				{
					emsg(e_noalt);
					goto doend;
				}
			}
			i = STRLEN(arg) + STRLEN(q) + 3;
			if (nextcomm)
				i += STRLEN(nextcomm);
			if (i > CMDBUFFSIZE)
			{
				emsg(e_toolong);
				goto doend;
			}
			/*
			 * we built the new argument in cmdbuf[], then copy it back to buff[]
			 */
			*p = NUL;							/* truncate at the '#' or '%' */
			STRCPY(cmdbuf, arg);				/* copy up to there */
			i = p - arg;						/* remember the lenght */
			STRCAT(cmdbuf, q);					/* append the file name */
			if (*(p + n) == '<')				/* may remove extension */
			{
				++n;
				if ((arg = (char_u *)strrchr((char *)q, '.')) != NULL &&
								arg >= gettail(q))
					*(cmdbuf + (arg - q) + i) = NUL;
			}
			i = STRLEN(cmdbuf);					/* remember the end of the filename */
			STRCAT(cmdbuf, p+n);				/* append what is after '#' or '%' */
			p = buff + i - 1;					/* remember where to continue */
			if (nextcomm)						/* append next command */
			{
				i = STRLEN(cmdbuf) + 1;
				STRCPY(cmdbuf + i, nextcomm);
				nextcomm = buff + i;
			}
			STRCPY(buff, cmdbuf);				/* copy back to buff[] */
			arg = buff;
		}

		/*
		 * One file argument: expand wildcards.
		 * Don't do this with ":r !command" or ":w !command".
		 */
		if (argt & NOSPC)
		{
			if (has_wildcard(arg) && !usefilter)
			{
				if ((p = ExpandOne(arg, TRUE, -1)) == NULL)
					goto doend;
				if (STRLEN(p) + arg - buff < CMDBUFFSIZE - 2)
					STRCPY(arg, p);
				else
					emsg(e_toolong);
				free(p);
			}
		}
	}

/*
 * 6. switch on command name
 */
cmdswitch:
	switch (cmdidx)
	{
		/*
		 * quit current window, quit Vim if closed the last window
		 */
		case CMD_quit:
						/* if more files or windows we won't exit */
				if (check_more(FALSE) == OK && firstwin == lastwin)
					exiting = TRUE;
				if (check_changed(curbuf, FALSE, FALSE) ||
							check_more(TRUE) == FAIL ||
							(firstwin == lastwin && check_changed_any(FALSE)))
				{
					exiting = FALSE;
					settmode(1);
					break;
				}
				if (firstwin == lastwin)	/* quit last window */
					getout(0);
				close_window(TRUE);			/* may free buffer */
				break;

		/*
		 * try to quit all windows
		 */
		case CMD_qall:
				exiting = TRUE;
				if (!check_changed_any(FALSE))
					getout(0);
				exiting = FALSE;
				settmode(1);
				break;

		/*
		 * close current window, unless it is the last one
		 */
		case CMD_close:
				if (firstwin == lastwin)
				{
					beep();
					break;
				}
				close_window(FALSE);		/* don't free buffer */
				break;

		case CMD_stop:
		case CMD_suspend:
				if (!forceit)
					autowrite_all();
				gotocmdend();
				flushbuf();
				stoptermcap();
				mch_suspend();		/* call machine specific function */
				starttermcap();
				must_redraw = CLEAR;
				break;

		case CMD_exit:
		case CMD_xit:
		case CMD_wq:
							/* if more files or windows we won't exit */
				if (check_more(FALSE) == OK && firstwin == lastwin)
					exiting = TRUE;
				if (((cmdidx == CMD_wq ||
						(curbuf->b_nwindows == 1 && curbuf->b_changed)) &&
						(check_readonly() || dowrite(arg, FALSE) == FAIL)) ||
					check_more(TRUE) == FAIL || 
					(firstwin == lastwin && check_changed_any(FALSE)))
				{
					exiting = FALSE;
					settmode(1);
					break;
				}
				if (firstwin == lastwin)	/* quit last window, exit Vim */
					getout(0);
				close_window(TRUE);			/* quit current window, may free buffer */
				break;

		case CMD_xall:		/* write all changed files and exit */
				exiting = TRUE;
				/* FALLTHROUGH */

		case CMD_wall:		/* write all changed files */
				{
					BUF		*buf;
					int		error = 0;

					for (buf = firstbuf; buf != NULL; buf = buf->b_next)
					{
						if (buf->b_changed)
						{
							if (buf->b_filename == NULL)
							{
								emsg(e_noname);
								++error;
							}
							else if (!forceit && buf->b_p_ro)
							{
								EMSG2("\"%s\" is readonly, use ! to write anyway", buf->b_xfilename);
								++error;
							}
							else if (buf_write_all(buf) == FAIL)
								++error;
						}
					}
					if (cmdidx == CMD_xall)
					{
						if (!error)
							getout(0);			/* exit Vim */
						exiting = FALSE;
						settmode(1);
					}
				}
				break;

		case CMD_preserve:					/* put everything in .swp file */
				ml_preserve(curbuf, TRUE);
				break;

		case CMD_args:
				if (arg_count == 0)			/* no file name list */
				{
					if (check_fname() == OK)		/* check for no file name at all */
						smsg((char_u *)"[%s]", curbuf->b_filename);
					break;
				}
				gotocmdline(TRUE, NUL);
				for (i = 0; i < arg_count; ++i)
				{
					if (i == arg_current)
						msg_outchar('[');
					msg_outstr(arg_files[i]);
					if (i == arg_current)
						msg_outchar(']');
					msg_outchar(' ');
				}
				if (msg_check())		/* if message too long */
				{
					msg_outchar('\n');
					wait_return(FALSE);
				}
				break;

		case CMD_wnext:
				n = line2;
				line1 = 1;
				line2 = curbuf->b_ml.ml_line_count;
				(void)dowrite(arg, FALSE);
				line2 = n;
				arg = (char_u *)"";		/* no file list */
				/*FALLTHROUGH*/

		case CMD_next:
				if (check_changed(curbuf, TRUE, FALSE))
					break;
				/*FALLTHROUGH*/

		case CMD_snext:
				editcmd = getargcmd(&arg);		/* get +command argument */
				nextcomm = checknextcomm(arg);	/* check for trailing command */
				if (*arg != NUL)				/* redefine file list */
				{
					if (doarglist(arg) == FAIL)
						break;
					i = 0;
				}
				else
					i = arg_current + (int)line2;

donextfile:		if (i < 0)
				{
					EMSG("Cannot go before first file");
					break;
				}
				if (i >= arg_count)
				{
					EMSG2("No more than %d files to edit", arg_count);
					break;
				}
				if (*cmd == 's')		/* split window first */
				{
					if (win_split(0L, FALSE) == FAIL)
						break;
				}
				else if (check_changed(curbuf, TRUE, FALSE))
					break;
				arg_current = i;
				(void)doecmd(arg_files[arg_current], NULL, editcmd, p_hid);
				break;

		case CMD_previous:
		case CMD_sprevious:
		case CMD_Next:
		case CMD_sNext:
				if (addr_count == 0)
					i = arg_current - 1;
				else
					i = arg_current - (int)line2;
				/*FALLTHROUGH*/

		case CMD_rewind:
		case CMD_srewind:
				if (cmdidx == CMD_rewind || cmdidx == CMD_srewind)
					i = 0;
				editcmd = getargcmd(&arg);		/* get +command argument */
				nextcomm = checknextcomm(arg);	/* check for trailing command */
				goto donextfile;

			/*
			 * :[N]buffer [[+command] fname]
			 */
		case CMD_buffer:
				if (*arg || addr_count == 0)	/* file name argument */
					goto do_edit;

				/*FALLTHROUGH*/

		case CMD_bmod:				/* :bmod         go to next modified buffer */
		case CMD_bnext:				/* :[N]bnext     go to next buffer */
		case CMD_bNext:				/* :[N]bNext     go to previous buffer */
		case CMD_bprevious:			/* :[N]bprevious go to previous buffer */
		case CMD_brewind:			/* :brewind      go to first buffer */
		case CMD_bdelete:			/* :[N]bdelete   delete buffer */
				{
					BUF		*buf;

					n = line2;
					if (cmdidx == CMD_bmod)		/* find next modified buffer */
					{
						buf = curbuf;
						while (n-- > 0)
						{
							do
							{
								buf = buf->b_next;
								if (buf == NULL)
									buf = firstbuf;
							}
							while (buf != curbuf && !buf->b_changed);
						}
						if (!buf->b_changed)
						{
							EMSG("No modified buffer found");
							break;
						}
					}
					else if (cmdidx == CMD_brewind)
						buf = firstbuf;
					else if (cmdidx == CMD_bdelete && addr_count == 0)
						buf = curbuf;
					else
					{
						if (cmdidx == CMD_buffer)	/* find buffer N */
							buf = firstbuf;
						else
							buf = curbuf;
						while (buf != NULL && n-- > 0)
						{
							if (cmdidx == CMD_bnext || cmdidx == CMD_buffer)
								buf = buf->b_next;
							else
								buf = buf->b_prev;
						}
					}
					if (buf == NULL)		/* could not find it */
					{
						if (cmdidx == CMD_bnext)
							EMSG("Cannot go beyond last buffer");
						else if (cmdidx == CMD_buffer || cmdidx == CMD_bdelete)
							EMSG2("Cannot go to buffer %ld", (char_u *)line2);
						else
							EMSG("Cannot go before first buffer");
					}
					else
					{
					/*
					 * delete buffer buf from the list
					 */
						if (cmdidx == CMD_bdelete)
						{
							if (buf->b_nwindows > 1)
							{
								EMSG("Other window editing this buffer");
								break;
							}
							if (!forceit && buf->b_changed)
							{
								emsg(e_nowrtmsg);
								break;
							}
							/*
							 * if deleting last buffer, make it empty
							 */
							if (firstbuf->b_next == NULL)
							{
								doecmd(NULL, NULL, NULL, FALSE);
								break;
							}
							/*
							 * If deleted buffer is not current one, delete it here.
							 * Otherwise find buffer to go to and delete it below.
							 */
							{
								if (buf != curbuf)
								{
									close_buffer(buf, TRUE);
									break;
								}
								if (buf->b_next != NULL)
									buf = buf->b_next;
								else
									buf = buf->b_prev;
							}
						}
					/*
					 * make buf current buffer
					 */
						filelist_altlnum();		/* remember curpos.lnum */
						close_buffer(curbuf, cmdidx == CMD_bdelete);
						curwin->w_buffer = buf;
						curbuf = buf;
						++curbuf->b_nwindows;
						filelist_getlnum();		/* restore curpos.lnum */
						maketitle();
						updateScreen(NOT_VALID);
					}
					break;
				}

		case CMD_buffers:
				do_buffers();
				break;

		case CMD_write:
				if (usefilter)		/* input lines to shell command */
					dofilter(line1, line2, arg, TRUE, FALSE);
				else
					(void)dowrite(arg, append);
				break;

			/*
			 * set screen mode
			 * if no argument given, just get the screen size and redraw
			 */
		case CMD_mode:
				if (*arg == NUL || mch_screenmode(arg) != FAIL)
					set_winsize(0, 0, FALSE);
				break;

				/*
				 * set, increment or decrement current window height
				 */
		case CMD_resize:
				n = atol((char *)arg);
				if (*arg == '-' || *arg == '+')
					win_setheight(curwin->w_height + (int)n);
				else
				{
					if (n == 0)		/* default is very high */
						n = 9999;
					win_setheight((int)n);
				}
				break;

				/*
				 * :split [[+command] file]  split window with current or new file
				 * :new [[+command] file]    split window with no or new file
				 */
		case CMD_split:
		case CMD_new:
				if (win_split(addr_count ? line2 : 0L, FALSE) == FAIL)
					break;
				/*FALLTHROUGH*/

do_edit:
		case CMD_edit:
		case CMD_ex:
		case CMD_visual:
				editcmd = getargcmd(&arg);		/* get +command argument */
				nextcomm = checknextcomm(arg);	/* check for trailing command */
				if ((cmdidx == CMD_new || cmdidx == CMD_buffer) && *arg == NUL)
					(void)doecmd(NULL, NULL, editcmd, TRUE);
				else if (cmdidx != CMD_split || *arg != NUL)
					(void)doecmd(arg, NULL, editcmd, cmdidx == CMD_buffer || p_hid);
				else
					updateScreen(NOT_VALID);
				break;

		case CMD_file:
				if (*arg != NUL)
				{
					setfname(arg, NULL);
					curbuf->b_notedited = TRUE;
					maketitle();
				}
				fileinfo(did_cd);		/* print full filename if :cd used */
				break;

		case CMD_swapname:
				p = curbuf->b_ml.ml_mfp->mf_fname;
				if (p == NULL)
					MSG("No swap file");
				else
					msg(p);
				break;

		case CMD_mfstat:		/* print memfile statistics, for debugging */
				mf_statistics();
				break;

		case CMD_files:
				filelist_list();
				break;

		case CMD_read:
				if (usefilter)
				{
					dofilter(line1, line2, arg, FALSE, TRUE);			/* :r!cmd */
					break;
				}
				if (!u_save(line2, (linenr_t)(line2 + 1)))
					break;
				if (*arg == NUL)
				{
					if (check_fname() == FAIL)	/* check for no file name at all */
						break;
					i = readfile(curbuf->b_filename, curbuf->b_sfilename, line2, FALSE, (linenr_t)0, MAXLNUM);
				}
				else
					i = readfile(arg, NULL, line2, FALSE, (linenr_t)0, MAXLNUM);
				if (i == FAIL)
				{
					emsg2(e_notopen, arg);
					break;
				}
				updateScreen(NOT_VALID);
				break;

		case CMD_cd:
		case CMD_chdir:
#ifdef UNIX
				/*
				 * for UNIX ":cd" means: go to home directory
				 */
				if (*arg == NUL)	 /* use cmdbuf for home directory name */
				{
					expand_env("$HOME", cmdbuf, CMDBUFFSIZE);
					arg = cmdbuf;
				}
#endif
				if (*arg != NUL)
				{
					if (!did_cd)
					{
						BUF		*buf;

							/* use full path from now on for names of files
							 * being edited and swap files */
						for (buf = firstbuf; buf != NULL; buf = buf->b_next)
						{
							buf->b_xfilename = buf->b_filename;
							mf_fullname(buf->b_ml.ml_mfp);
						}
						status_redraw_all();
					}
					did_cd = TRUE;
					if (chdir((char *)arg))
						emsg(e_failed);
					break;
				}
				/*FALLTHROUGH*/

		case CMD_pwd:
				if (dirname(IObuff, IOSIZE) == OK)
					msg(IObuff);
				else
					emsg(e_unknown);
				break;

		case CMD_equal:
				smsg((char_u *)"line %ld", (long)line2);
				break;

		case CMD_list:
				i = curwin->w_p_list;
				curwin->w_p_list = 1;
		case CMD_number:				/* :nu */
		case CMD_pound:					/* :# */
		case CMD_print:					/* :p */
				mch_start_listing();		/* may set cooked mode, so output can be halted */
				gotocmdline(TRUE, NUL);
				cursor_off();
				for ( ;!got_int; breakcheck())
				{
					if (curwin->w_p_nu || cmdidx == CMD_number || cmdidx == CMD_pound)
					{
						sprintf((char *)IObuff, "%7ld ", (long)line1);
						msg_outstr(IObuff);
					}
					msg_prt_line(ml_get(line1));
					if (++line1 > line2)
						break;
					msg_outchar('\n');
					flushbuf();			/* show one line at a time */
				}
				mch_stop_listing();

				if (cmdidx == CMD_list)
					curwin->w_p_list = i;

					/*
					 * if we have one line that runs into the shown command,
					 * or more than one line, call wait_return()
					 * also do this when global_busy, so we remember to call
					 * wait_return at the end of the global command.
					 */
				if (msg_check() || global_busy)
				{
					msg_outchar('\n');
					wait_return(FALSE);
				}
				break;

		case CMD_shell:
				doshell(NULL);
				break;

		case CMD_sleep:
				sleep((int)line2);
				break;

		case CMD_tag:
				dotag(arg, 0, addr_count ? (int)line2 : 1);
				break;

		case CMD_pop:
				dotag((char_u *)"", 1, addr_count ? (int)line2 : 1);
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

		case CMD_digraphs:
#ifdef DIGRAPHS
				if (*arg)
					putdigraph(arg);
				else
					listdigraphs();
#else
				EMSG("No digraphs in this version");
#endif /* DIGRAPHS */
				break;

		case CMD_set:
				(void)doset(arg);
				break;

		case CMD_abbreviate:
		case CMD_cabbrev:
		case CMD_iabbrev:
		case CMD_cnoreabbrev:
		case CMD_inoreabbrev:
		case CMD_noreabbrev:
		case CMD_unabbreviate:
		case CMD_cunabbrev:
		case CMD_iunabbrev:
				i = ABBREV;
				goto doabbr;		/* almost the same as mapping */

		case CMD_cmap:
		case CMD_imap:
		case CMD_map:
		case CMD_cnoremap:
		case CMD_inoremap:
		case CMD_noremap:
				/*
				 * If we are sourcing .exrc or .vimrc in current directory we
				 * print the mappings for security reasons.
				 */
				if (secure)
				{
					secure = 2;
					msg_outtrans(cmd, -1);
					msg_outchar('\n');
				}
		case CMD_cunmap:
		case CMD_iunmap:
		case CMD_unmap:
				i = 0;
doabbr:
				if (*cmd == 'c')		/* cmap, cunmap, cnoremap, etc. */
				{
					i += CMDLINE;
					++cmd;
				}
				else if (*cmd == 'i')	/* imap, iunmap, inoremap, etc. */
				{
					i += INSERT;
					++cmd;
				}
				else if (forceit || i)	/* map!, unmap!, noremap!, abbrev */
					i += INSERT + CMDLINE;
				else
					i += NORMAL;			/* map, unmap, noremap */
				switch (domap((*cmd == 'n') ? 2 : (*cmd == 'u'), arg, i))
				{
					case 1: emsg(e_invarg);
							break;
					case 2: emsg(e_nomap);
							break;
					case 3: emsg(e_ambmap);
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

		case CMD_winsize:					/* obsolete command */
				line1 = getdigits(&arg);
				skipspace(&arg);
				line2 = getdigits(&arg);
				set_winsize((int)line1, (int)line2, TRUE);
				break;

		case CMD_delete:
		case CMD_yank:
		case CMD_rshift:
		case CMD_lshift:
				yankbuffer = regname;
				curbuf->b_startop.lnum = line1;
				curbuf->b_endop.lnum = line2;
				nlines = line2 - line1 + 1;
				mtype = MLINE;
				curwin->w_cursor.lnum = line1;
				switch (cmdidx)
				{
				case CMD_delete:
					dodelete();
					break;
				case CMD_yank:
					(void)doyank(FALSE);
					curwin->w_cursor.lnum = line2;		/* put cursor on last line */
					break;
				case CMD_rshift:
					doshift(RSHIFT, FALSE, amount);
					break;
				case CMD_lshift:
					doshift(LSHIFT, FALSE, amount);
					break;
				}
				break;

		case CMD_put:
				yankbuffer = regname;
				curwin->w_cursor.lnum = line2;
				doput(forceit ? BACKWARD : FORWARD, -1L);
				break;

		case CMD_t:
		case CMD_copy:
		case CMD_move:
				n = get_address(&arg);
				/*
				 * move or copy lines from 'line1'-'line2' to below line 'n'
				 */
				if (n == MAXLNUM || n < 0)
				{
					emsg(e_invaddr);
					break;
				}

				if (cmdidx == CMD_move)
				{
					if (do_move(line1, line2, n) == FAIL)
						break;
				}
				else
					do_copy(line1, line2, n);
				u_clearline();
				curwin->w_cursor.col = 0;
				updateScreen(NOT_VALID);
				break;

		case CMD_and:			/* :& */
		case CMD_tilde:			/* :~ */
		case CMD_substitute:	/* :s */
				dosub(line1, line2, arg, &nextcomm,
							cmdidx == CMD_substitute ? 0 :
							cmdidx == CMD_and ? 1 : 2);
				break;

		case CMD_join:
				curwin->w_cursor.lnum = line1;
				if (line1 == line2)
				{
					if (line2 == curbuf->b_ml.ml_line_count)
					{
						beep();
						break;
					}
					++line2;
				}
				dodojoin(line2 - line1 + 1, !forceit, TRUE);
				break;

		case CMD_global:
				if (forceit)
					*cmd = 'v';
		case CMD_vglobal:
				doglob(*cmd, line1, line2, arg);
				break;

		case CMD_at:				/* :[addr]@r */
				curwin->w_cursor.lnum = line2;
				if (doexecbuf(*arg) == FAIL)			/* put the register in mapbuf */
					beep();
				else
					(void)docmdline((char_u *)NULL);	/* execute from the mapbuf */
				break;

		case CMD_bang:
				dobang(addr_count, line1, line2, forceit, arg);
				break;

		case CMD_undo:
				u_undo(1);
				break;

		case CMD_redo:
				u_redo(1);
				break;

		case CMD_source:
				if (forceit)	/* :so! read vi commands */
					(void)openscript(arg);
				else
				{
					++no_wait_return;
					if (dosource(arg) == FAIL)		/* :so read ex commands */
						emsg2(e_notopen, arg);
					--no_wait_return;
					if (need_wait_return)
						wait_return(FALSE);
				}
				break;

		case CMD_mkvimrc:
				if (*arg == NUL)
					arg = (char_u *)VIMRC_FILE;
				/*FALLTHROUGH*/

		case CMD_mkexrc:
				{
					FILE	*fd;

					if (*arg == NUL)
						arg = (char_u *)EXRC_FILE;
#ifdef UNIX
						/* with Unix it is possible to open a directory */
					if (isdir(arg) == TRUE)
					{
						EMSG2("\"%s\" is a directory", arg);
						break;
					}
#endif
					if (!forceit && (fd = fopen((char *)arg, "r")) != NULL)
					{
						fclose(fd);
						EMSG2("\"%s\" exists (use ! to override)", arg);
						break;
					}

					if ((fd = fopen((char *)arg, "w")) == NULL)
					{
						EMSG2("Cannot open \"%s\" for writing", arg);
						break;
					}
					if (makemap(fd) == FAIL || makeset(fd) == FAIL || fclose(fd))
						emsg(e_write);
					break;
				}

		case CMD_cc:
					qf_jump(0, atoi((char *)arg));
					break;

		case CMD_cf:
					if (*arg != NUL)
					{
						/*
						 * Great trick: Insert 'ef=' before arg.
						 * Always ok, because "cf " must be there.
						 */
						arg -= 3;
						arg[0] = 'e';
						arg[1] = 'f';
						arg[2] = '=';
						(void)doset(arg);
					}
					(void)qf_init();
					break;

		case CMD_cl:
					qf_list();
					break;

		case CMD_cn:
					qf_jump(FORWARD, *arg == NUL ? 1 : atoi((char *)arg));
					break;

		case CMD_cp:
					qf_jump(BACKWARD, *arg == NUL ? 1 : atoi((char *)arg));
					break;

		case CMD_cq:
					getout(1);		/* this does not always work. why? */

		case CMD_mark:
		case CMD_k:
					pos = curwin->w_cursor;			/* save curwin->w_cursor */
					curwin->w_cursor.lnum = line2;
					curwin->w_cursor.col = 0;
					(void)setmark(*arg);			/* set mark */
					curwin->w_cursor = pos;			/* restore curwin->w_cursor */
					break;

#ifdef SETKEYMAP
		case CMD_setkeymap:
					set_keymap(arg);
					break;
#endif

		case CMD_center:
		case CMD_right:
		case CMD_left:
					do_align(line1, line2, atoi((char *)arg),
							cmdidx == CMD_center ? 0 : cmdidx == CMD_right ? 1 : -1);
					break;

		case CMD_make:
					domake(arg);
					break;

		default:
					emsg(e_invcmd);
	}


doend:
	forceit = FALSE;		/* reset now so it can be used in getfile() */
	if (nextcomm && *nextcomm == NUL)		/* not really a next command */
		nextcomm = NULL;
	return nextcomm;
}

/*
 * if 'autowrite' option set, try to write the file
 *
 * return FAIL for failure, OK otherwise
 */
	static int
autowrite(buf)
	BUF		*buf;
{
	if (!p_aw || (!forceit && buf->b_p_ro) || buf->b_filename == NULL)
		return FAIL;
	return buf_write_all(buf);
}

/*
 * flush all buffers, except the ones that are readonly
 */
	void
autowrite_all()
{
	BUF		*buf;

	if (!p_aw)
		return;
	for (buf = firstbuf; buf; buf = buf->b_next)
		if (buf->b_changed && !buf->b_p_ro)
			(void)buf_write_all(buf);
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
	static int
buf_write_all(buf)
	BUF		*buf;
{
	return (buf_write(buf, buf->b_filename, buf->b_sfilename, (linenr_t)1, buf->b_ml.ml_line_count, 0, 0, TRUE));
}

/*
 * write current buffer to file 'fname'
 * if 'append' is TRUE, append to the file
 *
 * if *fname == NUL write to current file
 * if b_notedited is TRUE, check for overwriting current file
 *
 * return FAIL for failure, OK otherwise
 */
	static int
dowrite(fname, append)
	char_u	*fname;
	int		append;
{
	FILE	*fd;
	int		other;
	char_u	*sfname = NULL;				/* init to shut up gcc */

	if (*fname == NUL)
		other = FALSE;
	else
	{
		sfname = fname;
		if (fname != IObuff)		/* if not already expanded */
		{
			(void)FullName(fname, IObuff, IOSIZE);
			fname = IObuff;
		}
		other = otherfile(fname);
	}

	/*
	 * if we have a new file name put it in the list of alternate file names
	 */
	if (other)
		setaltfname(fname, sfname, (linenr_t)1);

	/*
	 * writing to the current file is not allowed in readonly mode
	 * and need a file name
	 */
	if (!other && (check_readonly() || check_fname() == FAIL))
		return FAIL;

	/*
	 * write to other file or b_notedited set: overwriting only allowed with '!'
	 */
	if ((other || curbuf->b_notedited) && !forceit && !append && !p_wa && (fd = fopen((char *)fname, "r")) != NULL)
	{								/* don't overwrite existing file */
		fclose(fd);
#ifdef UNIX
			/* with UNIX it is possible to open a directory */
		if (isdir(fname) == TRUE)
			EMSG2("\"%s\" is a directory", fname);
		else
#endif
			emsg(e_exists);
		return FAIL;
	}
	if (!other)
	{
		fname = curbuf->b_filename;
		sfname = curbuf->b_sfilename;
	}
	return (buf_write(curbuf, fname, sfname, line1, line2, append, forceit, TRUE));
}

/*
 * start editing a new file
 *
 *    fname: the file name
 *				- full path if sfname used,
 *				- any file name if sfname is NULL
 *				- empty string to re-edit with the same file name (but may be
 *					in a different directory)
 *				- NULL to start an empty buffer
 *   sfname: the short file name (or NULL)
 *  command: the command to be executed after loading the file
 *     hide: if TRUE don't free the current buffer
 *
 * return FAIL for failure, OK otherwise
 */
	static int
doecmd(fname, sfname, command, hide)
	char_u		*fname;
	char_u		*sfname;
	char_u		*command;
	int			hide;
{
	int			other_file;				/* TRUE if editing another file */
	linenr_t	newlnum;
	int			oldbuf = FALSE;			/* TRUE if using existing buffer */

	newlnum = doecmdlnum;
	doecmdlnum = 0;						/* reset it for next time */

		/* if no short name given, use fname for short name */
	if (sfname == NULL)
		sfname = fname;

	if (fname == NULL)
		other_file = TRUE;
	else if (*fname == NUL && curbuf->b_filename == NULL)	/* there is no file name */
		other_file = FALSE;
	else
	{
		if (*fname == NUL)				/* re-edit with same file name */
		{
			fname = curbuf->b_filename;
			sfname = curbuf->b_sfilename;
		}
		fname = fix_fname(fname);		/* may expand to full path name */
		other_file = otherfile(fname);
	}
/*
 * if the file was changed we may not be allowed to abandon it
 * - if we are going to re-edit the same file
 * - or if we are the only window on this file and if hide is FALSE
 */
	if ((!other_file || (curbuf->b_nwindows == 1 && !hide)) &&
						check_changed(curbuf, FALSE, FALSE))
	{
		if (other_file && fname != NULL)
			setaltfname(fname, sfname, (linenr_t)1);
		return FAIL;
	}
/*
 * If we are starting to edit another file, open a (new) buffer.
 * Otherwise we re-use the current buffer.
 */
	if (other_file)
	{
		curwin->w_alt_fnum = curbuf->b_fnum;
		filelist_altlnum();
		if (open_buffer(fname, &oldbuf, !hide) == FAIL)
			return FAIL;
		curbuf->b_pcmark.lnum = 1;
		curbuf->b_pcmark.col = 0;
	}
	else if (check_fname() == FAIL)
		return FAIL;

/*
 * If we get here we are sure to start editing
 */
		/* don't redraw until the cursor is in the right line */
	++RedrawingDisabled;

/*
 * other_file	oldbuf
 *	FALSE		FALSE		re-edit same file, buffer is re-used
 *	FALSE		TRUE		not posible
 *  TRUE		FALSE		start editing new file, new buffer
 *  TRUE		TRUE		start editing in existing buffer (nothing to do)
 */
	if (!other_file)					/* re-use the buffer */
	{
		if (newlnum == 0)
			newlnum = curwin->w_cursor.lnum;
		buf_freeall(curbuf);			/* free all things for buffer */
		buf_init(curbuf);
		curbuf->b_startop.lnum = 0;		/* clear '[ and '] marks */
		curbuf->b_endop.lnum = 0;
	}
	else
		setfname(fname, sfname);

	if (!oldbuf)						/* need to read the file */
	{
		if (readonlymode && fname != NULL)
			curbuf->b_p_ro = TRUE;
		ml_open(curbuf);
		if (fname != NULL)
			(void)readfile(curbuf->b_filename, curbuf->b_sfilename, (linenr_t)0, TRUE, (linenr_t)0, MAXLNUM);
		UNCHANGED;
	}
	else
		fileinfo(did_cd);				/* message about the file */
	win_init(curwin);
	maketitle();

	if (newlnum && command == NULL)
	{
		if (newlnum != MAXLNUM)
			curwin->w_cursor.lnum = newlnum;
		else
			curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		curwin->w_cursor.col = 0;
	}
	check_cursor();

	if (command != NULL)
		docmdline(command);
	--RedrawingDisabled;
	if (!skip_redraw)
		updateScreen(CURSUPD);			/* redraw now */

	if (p_im)
		stuffReadbuff((char_u *)"i");	/* start editing in insert mode */
	return OK;
}

/*
 * get + command from ex argument
 */
	static char_u *
getargcmd(argp)
	char_u **argp;
{
	char_u *arg = *argp;
	char_u *command = NULL;

	if (*arg == '+')		/* +[command] */
	{
		++arg;
		if (isspace(*arg))
			command = (char_u *)"$";
		else
		{
			command = arg;
			/*
			 * should check for "\ " (but vi has a bug that prevents it to work)
			 */
			skiptospace(&arg);
		}
		if (*arg)
			*arg++ = NUL;	/* terminate command with NUL */
		
		skipspace(&arg);	/* skip over spaces */
		*argp = arg;
	}
	return command;
}

/*
 * look for command separator '|' or '\n'
 */
	static char_u *
checknextcomm(arg)
	char_u *arg;
{
	char_u *p;
	char_u *nextcomm = NULL;

	for (p = arg; *p; ++p)
	{
		if (*p == '\\' && p[1])
			++p;
		else if (*p == '|' || *p == '\n')
		{
			nextcomm = p + 1;	/* remember start of next command */
			*p = NUL;			/* delete '|' or '\n' */
			del_spaces(arg);	/* delete spaces in front of '|' or '\n' */
			break;
		}
	}
	return nextcomm;
}

	static void
domake(arg)
	char_u *arg;
{
	if (*p_ef == NUL)
	{
		EMSG("errorfile option not set");
		return;
	}
	if (curbuf->b_changed)
		(void)autowrite(curbuf);
	remove((char *)p_ef);
	outchar(':');
	outstr(arg);		/* show what we are doing */
	sprintf((char *)IObuff, "%s %s %s", arg, p_sp, p_ef);
	doshell(IObuff);
#ifdef AMIGA
	flushbuf();
	vpeekc();		/* read window status report and redraw before message */
#endif
	(void)qf_init();
	remove((char *)p_ef);
}

/* 
 * Redefine the argument list to 'str'.
 *
 * Return FAIL for failure, OK otherwise.
 */
	static int
doarglist(str)
	char_u *str;
{
	int		new_count = 0;
	char_u	**new_files = NULL;
	int		exp_count;
	char_u	**exp_files;
	char_u	**t;
	char_u	*p;
	int		inquote;
	int		i;

	while (*str)
	{
		/*
		 * create a new entry in new_files[]
		 */
		t = (char_u **)lalloc((long_u)(sizeof(char_u *) * (new_count + 1)), TRUE);
		if (t != NULL)
			for (i = new_count; --i >= 0; )
				t[i] = new_files[i];
		free(new_files);
		if (t == NULL)
			return FAIL;
		new_files = t;
		new_files[new_count++] = str;

		/*
		 * isolate one argument, taking quotes
		 */
		inquote = FALSE;
		for (p = str; *str; ++str)
		{
			/*
			 * for MSDOS a backslash is part of a file name.
			 * Only skip ", space and tab.
			 */
#ifdef MSDOS
			if (*str == '\\' && strchr("\" \t", *(str + 1)) != NULL)
#else
			if (*str == '\\' && *(str + 1) != NUL)
#endif
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
	
	i = ExpandWildCards(new_count, new_files, &exp_count, &exp_files, FALSE, TRUE);
	free(new_files);
	if (i == FAIL)
		return FAIL;
	if (exp_count == 0)
	{
		emsg(e_nomatch);
		return FAIL;
	}
	if (arg_exp)				/* arg_files[] has been allocated, free it */
		FreeWild(arg_count, arg_files);
	else
		arg_exp = TRUE;
	arg_files = exp_files;
	arg_count = exp_count;

	return OK;
}

	void
gotocmdline(clr, firstc)
	int				clr;
	int				firstc;
{
	msg_start();
	if (clr)			/* clear the bottom line(s) */
		msg_ceol();		/* will reset clear_cmdline */
	windgoto(cmdline_row, 0);
	if (firstc)
		msg_outchar(firstc);
}

	void
gotocmdend()
{
	windgoto((int)Rows - 1, 0);
	outchar('\n');
}

	static int
check_readonly()
{
	if (!forceit && curbuf->b_p_ro)
	{
		emsg(e_readonly);
		return TRUE;
	}
	return FALSE;
}

/*
 * return TRUE if buffer was changed and cannot be abandoned.
 */
	static int
check_changed(buf, checkaw, mult_win)
	BUF		*buf;
	int		checkaw;		/* do autowrite if buffer was changed */
	int		mult_win;		/* check also when several windows for this buffer */
{
	if (	!forceit &&
			buf->b_changed && (mult_win || buf->b_nwindows <= 1) &&
			!p_hid &&
			(!checkaw || autowrite(buf) == FAIL))
	{
		emsg(e_nowrtmsg);
		return TRUE;
	}
	return FALSE;
}

/*
 * return TRUE if any buffer was changed and cannot be abandoned.
 */
	static int
check_changed_any(checkaw)
	int		checkaw;		/* do autowrite if buffer was changed */
{
	BUF		*buf;

	if (!forceit)
	{
		for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		{
			if (buf->b_changed && (!checkaw || autowrite(buf) == FAIL))
			{
				EMSG2("No write since last change for buffer \"%s\"",
						buf->b_xfilename == NULL ? (char_u *)"No File" : buf->b_xfilename);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*
 * return FAIL if there is no filename, OK if there is one
 * give error message for FAIL
 */
	int
check_fname()
{
	if (curbuf->b_filename == NULL)
	{
		emsg(e_noname);
		return FAIL;
	}
	return OK;
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *	  return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
	static int
check_more(message)
	int message;			/* when FALSE check only, no messages */
{
	if (!forceit && firstwin == lastwin && arg_current + 1 < arg_count &&
									quitmore == 0)
	{
		if (message)
		{
			emsg(e_more);
			quitmore = 2;			/* next try to quit is allowed */
		}
		return FAIL;
	}
	return OK;
}

/*
 * try to abandon current file and edit "fname"
 * return 1 for "normal" error, 2 for "not written" error, 0 for success
 * -1 for succesfully opening another file
 */
	int
getfile(fname, sfname, setpm)
	char_u	*fname;
	char_u	*sfname;
	int		setpm;
{
	int other;

	if (sfname == NULL)
		sfname = fname;
	
	if (fname != IObuff)		/* if not expanded already */
	{
		(void)FullName(fname, IObuff, IOSIZE);
		fname = IObuff;
	}
	other = otherfile(fname);
	if (other && !forceit && curbuf->b_nwindows == 1 && curbuf->b_changed &&
										autowrite(curbuf) == FAIL)
	{
		emsg(e_nowrtmsg);
		return 2;		/* file has been changed */
	}
	if (setpm)
		setpcmark();
	if (!other)
		return 0;		/* it's in the same file */
	if (doecmd(fname, sfname, NULL, p_hid) == OK)
		return -1;		/* opened another file */
	return 1;			/* error encountered */
}

	static void
nextwild(buff, type)
	char_u *buff;
	int		type;
{
	int		i;
	char_u	*p1, *p2;
	int		oldlen;
	int		difflen;

	msg_outstr((char_u *)"...");		/* show that we are busy */
	flushbuf();

	for (i = cmdpos; i > 0 && buff[i - 1] != ' '; --i)
		;
	oldlen = cmdpos - i;

		/* add a "*" to the file name and expand it */
	if ((p1 = addstar(&buff[i], oldlen)) != NULL)
	{
		if ((p2 = ExpandOne(p1, FALSE, type)) != NULL)
		{
			if (cmdlen + (difflen = STRLEN(p2) - oldlen) > CMDBUFFSIZE - 4)
				emsg(e_toolong);
			else
			{
				STRNCPY(&buff[cmdpos + difflen], &buff[cmdpos], (size_t)(cmdlen - cmdpos));
				STRNCPY(&buff[i], p2, STRLEN(p2));
				cmdlen += difflen;
				cmdpos += difflen;
			}
			free(p2);
		}
		free(p1);
	}
	redrawcmd();
}

/*
 * Do wildcard expansion on the string 'str'.
 * Return a pointer to alloced memory containing the new string.
 * Return NULL for failure.
 *
 * mode = -2: only release file names
 * mode = -1: normal expansion, do not keep file names
 * mode =  0: normal expansion, keep file names
 * mode =  1: use next match in multiple match
 * mode =  2: use previous match in multiple match
 * mode =  3: use next match in multiple match and wrap to first
 * mode =  4: return all matches concatenated
 * mode =  5: return longest matched part
 */
	char_u *
ExpandOne(str, list_notfound, mode)
	char_u	*str;
	int		list_notfound;
	int		mode;
{
	char_u		*ss = NULL;
	static char_u **cmd_files = NULL;	  /* list of input files */
	static int	findex;
	int			i, found = 0;
	int			multmatch = FALSE;
	long_u		len;
	char_u		*filesuf, *setsuf, *nextsetsuf;
	int			filesuflen, setsuflen;

/*
 * first handle the case of using an old match
 */
	if (mode >= 1 && mode < 4)
	{
		if (cmd_numfiles > 0)
		{
			if (mode == 2)
				--findex;
			else	/* mode == 1 || mode == 3 */
				++findex;
			if (findex < 0)
				findex = 0;
			if (findex > cmd_numfiles - 1)
			{
				if (mode == 3)
					findex = 0;
				else
					findex = cmd_numfiles - 1;
			}
			return strsave(cmd_files[findex]);
		}
		else
			return NULL;
	}

/* free old names */
	if (cmd_numfiles != -1 && mode < 4)
	{
		FreeWild(cmd_numfiles, cmd_files);
		cmd_numfiles = -1;
	}
	findex = 0;

	if (mode == -2)		/* only release file name */
		return NULL;

	if (cmd_numfiles == -1)
	{
		if (ExpandWildCards(1, (char_u **)&str, &cmd_numfiles, &cmd_files, FALSE, list_notfound) == FAIL)
			/* error: do nothing */;
		else if (cmd_numfiles == 0)
			emsg(e_nomatch);
		else if (mode < 4)
		{
			if (cmd_numfiles > 1)		/* more than one match; check suffixes */
			{
				found = -2;
				for (i = 0; i < cmd_numfiles; ++i)
				{
					if ((filesuf = STRRCHR(cmd_files[i], '.')) != NULL)
					{
						filesuflen = STRLEN(filesuf);
						for (setsuf = p_su; *setsuf; setsuf = nextsetsuf)
						{
							if ((nextsetsuf = STRCHR(setsuf + 1, '.')) == NULL)
								nextsetsuf = setsuf + STRLEN(setsuf);
							setsuflen = (int)(nextsetsuf - setsuf);
							if (filesuflen == setsuflen &&
										STRNCMP(setsuf, filesuf, (size_t)setsuflen) == 0)
								break;
						}
						if (*setsuf)				/* suffix matched: ignore file */
							continue;
					}
					if (found >= 0)
					{
						multmatch = TRUE;
						break;
					}
					found = i;
				}
			}
			if (multmatch || found < 0)
			{
				emsg(e_toomany);
				found = 0;				/* return first one */
				multmatch = TRUE;		/* for found < 0 */
			}
			if (found >= 0 && !(multmatch && mode == -1))
				ss = strsave(cmd_files[found]);
		}
	}

	if (mode == 5 && cmd_numfiles > 0)		/* find longest common part */
	{
		for (len = 0; cmd_files[0][len]; ++len)
		{
			for (i = 0; i < cmd_numfiles; ++i)
			{
#ifdef AMIGA
				if (toupper(cmd_files[i][len]) != toupper(cmd_files[0][len]))
#else
				if (cmd_files[i][len] != cmd_files[0][len])
#endif
					break;
			}
			if (i < cmd_numfiles)
				break;
		}
		ss = alloc((unsigned)len + 1);
		if (ss)
		{
			STRNCPY(ss, cmd_files[0], (size_t)len);
			ss[len] = NUL;
		}
		multmatch = TRUE;					/* don't free the names */
		findex = -1;						/* next p_wc gets first one */
	}

	if (mode == 4 && cmd_numfiles > 0)		/* concatenate all file names */
	{
		len = 0;
		for (i = 0; i < cmd_numfiles; ++i)
			len += STRLEN(cmd_files[i]) + 1;
		ss = lalloc(len, TRUE);
		if (ss)
		{
			*ss = NUL;
			for (i = 0; i < cmd_numfiles; ++i)
			{
				STRCAT(ss, cmd_files[i]);
				if (i != cmd_numfiles - 1)
					STRCAT(ss, " ");
			}
		}
	}

	if (!multmatch || mode == -1 || mode == 4)
	{
		FreeWild(cmd_numfiles, cmd_files);
		cmd_numfiles = -1;
	}
	return ss;
}

/*
 * show all filenames that match the string "file" with length "len"
 */
	static void
showmatches(file, len)
	char_u *file;
	int	len;
{
	char_u *file_str;
	int num_files;
	char_u **files_found;
	int i, j, k;
	int maxlen;
	int lines;
	int columns;

	file_str = addstar(file, len);		/* add star to file name */
	if (file_str == NULL)
		return;

	msg_outchar('\n');
	flushbuf();

	/* find all files that match the description */
	if (ExpandWildCards(1, &file_str, &num_files, &files_found, FALSE, FALSE) == FAIL)
		return;

	/* find the maximum length of the file names */
	maxlen = 0;
	for (i = 0; i < num_files; ++i)
	{
		j = STRLEN(files_found[i]);
		if (j > maxlen)
			maxlen = j;
	}

	/* compute the number of columns and lines for the listing */
	maxlen += 2;	/* two spaces between file names */
	columns = ((int)Columns + 2) / maxlen;
	if (columns < 1)
		columns = 1;
	lines = (num_files + columns - 1) / columns;

	/* list the files line by line */
	mch_start_listing();		/* allow output to be halted */
	for (i = 0; i < lines; ++i)
	{
		for (k = i; k < num_files; k += lines)
		{
			if (k > i)
				for (j = maxlen - STRLEN(files_found[k - lines]); --j >= 0; )
					msg_outchar(' ');
			j = isdir(files_found[k]);	/* highlight directories */
			if (j == TRUE)
			{
				outstr(T_SO);
				screen_start();		/* don't output spaces to position cursor */
			}
			msg_outstr(files_found[k]);
			if (j == TRUE)
				outstr(T_SE);
		}
		msg_outchar('\n');
		flushbuf();					/* show one line at a time */
	}
	free(file_str);
	FreeWild(num_files, files_found);
	mch_stop_listing();

/*
* we redraw the command below the lines that we have just listed
* This is a bit tricky, but it saves a lot of screen updating.
*/
	cmdline_row = msg_row;		/* will put it back later */
}

/*
 * copy the file name into allocated memory and add a '*' at the end
 */
	static char_u *
addstar(fname, len)
	char_u	*fname;
	int		len;
{
	char_u	*retval;
#ifdef MSDOS
	int		i;
#endif

	retval = alloc(len + 4);
	if (retval != NULL)
	{
		STRNCPY(retval, fname, (size_t)len);
#ifdef MSDOS
	/*
	 * if there is no dot in the file name, add "*.*" instead of "*".
	 */
		for (i = len - 1; i >= 0; --i)
			if (strchr(".\\/:", retval[i]))
				break;
		if (i < 0 || retval[i] != '.')
		{
			retval[len++] = '*';
			retval[len++] = '.';
		}
#endif
		retval[len] = '*';
		retval[len + 1] = 0;
	}
	return retval;
}

/*
 * dosource: read the file "fname" and execute its lines as EX commands
 *
 * This function may be called recursively!
 *
 * return FAIL if file could not be opened, OK otherwise
 */
	int
dosource(fname)
	register char_u *fname;
{
	register FILE	*fp;
	register int	len;
#ifdef MSDOS
	int				error = FALSE;
#endif

	expand_env(fname, IObuff, IOSIZE);		/* use IObuff for expanded name */
	if ((fp = fopen((char *)IObuff, READBIN)) == NULL)
		return FAIL;

	++dont_sleep;			/* don't call sleep() in emsg() */
	len = 0;
	while (fgets((char *)IObuff + len, IOSIZE - len, fp) != NULL && !got_int)
	{
		len = STRLEN(IObuff) - 1;
		if (len >= 0 && IObuff[len] == '\n')	/* remove trailing newline */
		{
#ifdef MSDOS
			if (len > 0 && IObuff[len - 1] == '\r') /* trailing CR-LF */
				--len;
			else
			{
				if (!error)
					EMSG("Warning: Wrong line separator, ^M may be missing");
				error = TRUE;		/* lines like ":map xx yy^M" will fail */
			}
#endif
				/* escaped newline, read more */
			if (len > 0 && len < IOSIZE && IObuff[len - 1] == Ctrl('V'))
			{
				IObuff[len - 1] = '\n';		/* remove CTRL-V */
				continue;
			}
			IObuff[len] = NUL;
		}
		breakcheck();		/* check for ^C here, so recursive :so will be broken */
		docmdline(IObuff);
		len = 0;
	}
	fclose(fp);
	if (got_int)
		emsg(e_interr);
	--dont_sleep;
	return OK;
}

/*
 * get single EX address
 */
	static linenr_t
get_address(ptr)
	char_u		**ptr;
{
	linenr_t	cursor_lnum = curwin->w_cursor.lnum;
	int			c;
	int			i;
	long		n;
	char_u  	*cmd;
	FPOS		pos;
	FPOS		*fp;
	linenr_t	lnum;

	cmd = *ptr;
	skipspace(&cmd);
	lnum = MAXLNUM;
	do
	{
		switch (*cmd)
		{
			case '.': 						/* '.' - Cursor position */
						++cmd;
						lnum = cursor_lnum;
						break;

			case '$': 						/* '$' - last line */
						++cmd;
						lnum = curbuf->b_ml.ml_line_count;
						break;

			case '\'': 						/* ''' - mark */
						if (*++cmd == NUL || (fp = getmark(*cmd++, FALSE)) == NULL)
						{
							emsg(e_umark);
							goto error;
						}
						lnum = fp->lnum;
						break;

			case '/':
			case '?':						/* '/' or '?' - search */
						c = *cmd++;
						pos = curwin->w_cursor;		/* save curwin->w_cursor */
						curwin->w_cursor.col = -1;	/* searchit() will increment the col */
						if (c == '/')
						{
						 	if (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)	/* :/pat on last line */
								curwin->w_cursor.lnum = 1;
							else
								++curwin->w_cursor.lnum;
						}
						searchcmdlen = 0;
						if (dosearch(c, cmd, FALSE, (long)1, FALSE))
							lnum = curwin->w_cursor.lnum;
						curwin->w_cursor = pos;
				
						cmd += searchcmdlen;	/* adjust command string pointer */
						break;

			default:
						if (isdigit(*cmd))				/* absolute line number */
							lnum = getdigits(&cmd);
		}
		
		while (*cmd == '-' || *cmd == '+')
		{
			if (lnum == MAXLNUM)
				lnum = cursor_lnum;
			i = *cmd++;
			if (!isdigit(*cmd))	/* '+' is '+1', but '+0' is not '+1' */
				n = 1;
			else 
				n = getdigits(&cmd);
			if (i == '-')
				lnum -= n;
			else
				lnum += n;
		}

		cursor_lnum = lnum;
	} while (*cmd == '/' || *cmd == '?');

error:
	*ptr = cmd;
	return lnum;
}
