/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * ex_docmd.c: functions for executing an Ex command line.
 */

#include "vim.h"

#define DO_DECLARE_EXCMD
#include "ex_cmds.h"	/* Declare the cmdnames struct. */

#ifdef HAVE_FCNTL_H
# include <fcntl.h>	    /* for chdir() */
#endif

static int	    quitmore = 0;
static int	    ex_pressedreturn = FALSE;

#ifdef WANT_EVAL
/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#define CSTACK_LEN	50

struct condstack
{
    char    cs_flags[CSTACK_LEN];   /* CSF_ flags */
    int	    cs_line[CSTACK_LEN];    /* line number of ":while" line */
    int	    cs_idx;		    /* current entry, or -1 if none */
    int	    cs_whilelevel;	    /* number of nested ":while"s */
    char    cs_had_while;	    /* just found ":while" */
    char    cs_had_continue;	    /* just found ":continue" */
    char    cs_had_endwhile;	    /* just found ":endwhile" */
};

#define CSF_TRUE	1	/* condition was TRUE */
#define CSF_ACTIVE	2	/* current state is active */
#define CSF_WHILE	4	/* is a ":while" */
#endif

#ifdef WANT_EVAL
static void free_cmdlines __ARGS((struct growarray *gap));
static char_u	*do_one_cmd __ARGS((char_u **, int, struct condstack *, char_u *(*getline)(int, void *, int), void *cookie));
#else
static char_u	*do_one_cmd __ARGS((char_u **, int, char_u *(*getline)(int, void *, int), void *cookie));
#endif
static int	buf_write_all __ARGS((BUF *));
static int	do_write __ARGS((EXARG *eap));
static char_u	*getargcmd __ARGS((char_u **));
static char_u	*skip_cmd_arg __ARGS((char_u *p));
#ifdef QUICKFIX
static void	do_make __ARGS((char_u *));
static char_u	*get_mef_name __ARGS((int newname));
static void	do_cfile __ARGS((EXARG *eap));
#endif
static int	do_arglist __ARGS((char_u *));
static int	rem_backslash __ARGS((char_u *str));
static int	check_readonly __ARGS((int));
static int	check_changed __ARGS((BUF *, int, int, int));
static int	check_more __ARGS((int, int));
static linenr_t get_address __ARGS((char_u **));
static void	do_quit __ARGS((EXARG *eap));
static void	do_quit_all __ARGS((int forceit));
static void	do_close __ARGS((EXARG *eap));
static void	do_suspend __ARGS((int forceit));
static void	do_exit __ARGS((EXARG *eap));
static void	do_wqall __ARGS((EXARG *eap));
static void	do_print __ARGS((EXARG *eap));
static void	do_argfile __ARGS((EXARG *eap, int argn));
static void	do_next __ARGS((EXARG *eap));
static void	do_recover __ARGS((EXARG *eap));
static void	do_args __ARGS((EXARG *eap));
static void	do_wnext __ARGS((EXARG *eap));
static void	do_resize __ARGS((EXARG *eap));
static void	do_splitview __ARGS((EXARG *eap));
static void	do_exedit __ARGS((EXARG *eap, WIN *old_curwin));
#ifdef USE_GUI
static void	do_gui __ARGS((EXARG *eap));
#endif
static void	do_swapname __ARGS((void));
static void	do_read __ARGS((EXARG *eap));
static void	do_cd __ARGS((EXARG *eap));
static void	do_pwd __ARGS((void));
static void	do_sleep __ARGS((EXARG *eap));
static void	do_exmap __ARGS((EXARG *eap, int isabbrev));
static void	do_winsize __ARGS((char_u *arg));
static void	do_exops __ARGS((EXARG *eap));
static void	do_copymove __ARGS((EXARG *eap));
static void	do_exjoin __ARGS((EXARG *eap));
static void	do_exat __ARGS((EXARG *eap));
static void	do_redir __ARGS((EXARG *eap));
static void	close_redir __ARGS((void));
static void	do_mkrc __ARGS((EXARG *eap));
static FILE	*open_exfile __ARGS((EXARG *eap, char *mode));
static void	do_setmark __ARGS((EXARG *eap));
#ifdef EX_EXTRA
static void	do_normal __ARGS((EXARG *eap));
#endif
#ifdef FIND_IN_PATH
static char_u	*do_findpat __ARGS((EXARG *eap, int action));
#endif
static void	do_ex_tag __ARGS((EXARG *eap, int dt));
#ifdef WANT_EVAL
static char_u	*do_if __ARGS((EXARG *eap, struct condstack *cstack));
static char_u	*do_else __ARGS((EXARG *eap, struct condstack *cstack));
static char_u	*do_while __ARGS((EXARG *eap, struct condstack *cstack));
static char_u	*do_continue __ARGS((struct condstack *cstack));
static char_u	*do_break __ARGS((struct condstack *cstack));
static char_u	*do_endwhile __ARGS((struct condstack *cstack));
static int	has_while_cmd __ARGS((char_u *p));
#endif

/*
 * Table used to quickly search for a command, based on its first character.
 */
CMDIDX cmdidxs[27] =
{
	CMD_append,
	CMD_buffer,
	CMD_change,
	CMD_delete,
	CMD_edit,
	CMD_file,
	CMD_global,
	CMD_help,
	CMD_insert,
	CMD_join,
	CMD_k,
	CMD_list,
	CMD_move,
	CMD_next,
	CMD_open,
	CMD_print,
	CMD_quit,
	CMD_read,
	CMD_substitute,
	CMD_t,
	CMD_undo,
	CMD_vglobal,
	CMD_write,
	CMD_xit,
	CMD_yank,
	CMD_z,
	CMD_Next
};

/*
 * do_exmode(): Repeatedly get commands for the "Ex" mode, until the ":vi"
 * command is given.
 */
    void
do_exmode()
{
    int		save_msg_scroll;
    int		prev_msg_row;
    linenr_t	prev_line;

    save_msg_scroll = msg_scroll;
    ++RedrawingDisabled;	    /* don't redisplay the window */
    ++no_wait_return;		    /* don't wait for return */
    settmode(TMODE_COOK);

    State = NORMAL;
    exmode_active = TRUE;
#ifdef USE_SNIFF
    want_sniff_request = 0;    /* No K_SNIFF wanted */
#endif

    MSG("Entering Ex mode.  Type \"visual\" to get out.");
    while (exmode_active)
    {

	msg_scroll = TRUE;
	need_wait_return = FALSE;
	ex_pressedreturn = FALSE;
	ex_no_reprint = FALSE;
	prev_msg_row = msg_row;
	prev_line = curwin->w_cursor.lnum;
#ifdef USE_SNIFF
	ProcessSniffRequests();
#endif
	do_cmdline(NULL, getexmodeline, NULL, DOCMD_NOWAIT);
	lines_left = Rows - 1;

	if (prev_line != curwin->w_cursor.lnum && !ex_no_reprint)
	{
	    if (ex_pressedreturn)
	    {
		/* go up one line, to overwrite the ":<CR>" line, so the
		 * output doensn't contain empty lines. */
		msg_row = prev_msg_row;
		if (prev_msg_row == Rows - 1)
		    msg_row--;
	    }
	    msg_col = 0;
	    print_line_no_prefix(curwin->w_cursor.lnum, FALSE);
	    msg_clr_eos();
	}
	else if (ex_pressedreturn)	/* must be at EOF */
	    EMSG("At end-of-file");
    }

    settmode(TMODE_RAW);
    --RedrawingDisabled;
    --no_wait_return;
    update_screen(CLEAR);
    need_wait_return = FALSE;
    msg_scroll = save_msg_scroll;
}

/*
 * do_cmdline(): execute one Ex command line
 *
 * 1. Execute "cmdline" when it is not NULL.
 *    If "cmdline" is NULL, or more lines are needed, getline() is used.
 * 2. Split up in parts separated with '|'.
 *
 * This function can be called recursively!
 *
 * flags:
 * DOCMD_VERBOSE - The command will be included in the error message.
 * DOCMD_NOWAIT  - Don't call wait_return() and friends.
 * DOCMD_REPEAT  - Repeat execution until getline() returns NULL.
 *
 * return FAIL if cmdline could not be executed, OK otherwise
 */
    int
do_cmdline(cmdline, getline, cookie, flags)
    char_u	*cmdline;
    char_u	*(*getline) __ARGS((int, void *, int));
    void	*cookie;		/* argument for getline() */
    int		flags;
{
    char_u	*next_cmdline;		/* next cmd to execute */
    char_u	*cmdline_copy = NULL;	/* copy of cmd line */
    static int	recursive = 0;		/* recursive depth */
    int		msg_didout_before_start = 0;
    int		count = 0;		/* line number count */
    int		did_inc = FALSE;	/* incremented RedrawingDisabled */
    int		retval = OK;
#ifdef WANT_EVAL
    struct condstack cstack;		/* conditional stack */
    struct growarray lines_ga;		/* keep lines for ":while" */
    int		current_line = 0;	/* active line in lines_ga */
    int		did_endwhile = FALSE;	/* ended with ":endwhile" */
#endif

#ifdef WANT_EVAL
    cstack.cs_idx = -1;
    cstack.cs_whilelevel = 0;
    cstack.cs_had_while = FALSE;
    cstack.cs_had_endwhile = FALSE;
    cstack.cs_had_continue = FALSE;
    ga_init(&lines_ga);
    lines_ga.ga_itemsize = sizeof(char_u *);
    lines_ga.ga_growsize = 10;
#endif

    /*
     * "did_emsg" will be set to TRUE when emsg() is used, in which case we
     * cancel the whole command line, and any if/endif while/endwhile loop.
     */
    did_emsg = FALSE;

    /*
     * Continue executing command lines:
     * - when inside an ":if" or ":while"
     * - for multiple commands on one line, separated with '|'
     * - when repeating until there are no more lines (for ":source")
     */
    next_cmdline = cmdline;
    do
    {
	/* stop skipping cmds for an error msg after all endifs and endwhiles */
	if (next_cmdline == NULL
#ifdef WANT_EVAL
				&& cstack.cs_idx < 0
#endif
							)
	    did_emsg = FALSE;

	/*
	 * 1. If repeating a line with ":while", get a line from lines_ga.
	 * 2. If no line given: Get an allocated line with getline().
	 * 3. If a line is given: Make a copy, so we can mess with it.
	 */

#ifdef WANT_EVAL
	/* 1. If repeating, get a previous line from lines_ga. */
	if (cstack.cs_whilelevel && current_line < lines_ga.ga_len)
	{
	    /* Each '|' separated command is stored separately in lines_ga, to
	     * be able to jump to it.  Don't use next_cmdline now. */
	    vim_free(cmdline_copy);
	    cmdline_copy = NULL;
	    next_cmdline = ((char_u **)(lines_ga.ga_data))[current_line];
	    line_breakcheck();		/* check if CTRL-C typed */
	}
#endif

	/* 2. If no line given, get an allocated line with getline(). */
	if (next_cmdline == NULL)
	{
	    /*
	     * Need to set msg_didout for the first line after an ":if",
	     * otherwise the ":if" will be overwritten.
	     */
	    if (count == 1 && getline == getexline)
		msg_didout = TRUE;
	    if (getline == NULL || (next_cmdline = getline(':', cookie,
#ifdef WANT_EVAL
		    cstack.cs_idx < 0 ? 0 : (cstack.cs_idx + 1) * 2
#else
		    0
#endif
			    )) == NULL)
	    {
		/* don't call wait_return for aborted command line */
		if (KeyTyped)
		    need_wait_return = FALSE;
		retval = FAIL;
		break;
	    }
	}

	/* 3. Make a copy of the command so we can mess with it. */
	else if (cmdline_copy == NULL)
	{
	    next_cmdline = vim_strsave(next_cmdline);
	    if (next_cmdline == NULL)
	    {
		retval = FAIL;
		break;
	    }
	}
	cmdline_copy = next_cmdline;

#ifdef WANT_EVAL
	/*
	 * Save the current line when inside a ":while", and when the command
	 * looks like a ":while", because we may need it later.
	 * When there is a '|' and another command, it is stored separately,
	 * because we need to be able to jump back to it from an :endwhile.
	 */
	if (	   current_line == lines_ga.ga_len
		&& (cstack.cs_whilelevel || has_while_cmd(next_cmdline))
		&& ga_grow(&lines_ga, 1) == OK)
	{
	    ((char_u **)(lines_ga.ga_data))[current_line] =
						    vim_strsave(next_cmdline);
	    ++lines_ga.ga_len;
	    --lines_ga.ga_room;
	}
	did_endwhile = FALSE;
#endif

	if (count++ == 0)
	{
	    /*
	     * All output from the commands is put below each other, without
	     * waiting for a return. Don't do this when executing commands
	     * from a script or when being called recursive (e.g. for ":e
	     * +command file").
	     */
	    if (!(flags & DOCMD_NOWAIT) && !recursive)
	    {
		msg_didout_before_start = msg_didout;
		msg_didany = FALSE; /* no output yet */
		msg_start();
		msg_scroll = TRUE;  /* put messages below each other */
		++no_wait_return;   /* dont wait for return until finished */
		++RedrawingDisabled;
		did_inc = TRUE;
	    }
	}

	/*
	 * 2. Execute one '|' separated command.
	 *    do_one_cmd() will return NULL if there is no trailing '|'.
	 *    "cmdline_copy" can change, e.g. for '%' and '#' expansion.
	 */
	++recursive;
	next_cmdline = do_one_cmd(&cmdline_copy, flags & DOCMD_VERBOSE,
#ifdef WANT_EVAL
				&cstack,
#endif
				getline, cookie);
	--recursive;
	if (next_cmdline == NULL)
	{
	    vim_free(cmdline_copy);
	    cmdline_copy = NULL;

	    /*
	     * If the command was typed, remember it for the ':' register.
	     * Do this AFTER executing the command to make :@: work.
	     */
	    if (getline == getexline && new_last_cmdline != NULL)
	    {
		vim_free(last_cmdline);
		last_cmdline = new_last_cmdline;
		new_last_cmdline = NULL;
	    }
	}
	else
	{
	    /* need to copy the command after the '|' to cmdline_copy, for the
	     * next do_one_cmd() */
	    STRCPY(cmdline_copy, next_cmdline);
	    next_cmdline = cmdline_copy;
	}


#ifdef WANT_EVAL
	if (cstack.cs_whilelevel)
	{
	    ++current_line;

	    /*
	     * An ":endwhile" and ":continue" is handled here.
	     * If we were executing commands, jump back to the ":while".
	     * If we were not executing commands, decrement whilelevel.
	     */
	    if (cstack.cs_had_endwhile || cstack.cs_had_continue)
	    {
		if (cstack.cs_had_endwhile)
		{
		    cstack.cs_had_endwhile = FALSE;
		    did_endwhile = TRUE;
		}
		else
		    cstack.cs_had_continue = FALSE;

		/* jump back to the matching ":while"? */
		if (!did_emsg && cstack.cs_idx >= 0
			&& (cstack.cs_flags[cstack.cs_idx] & CSF_ACTIVE))
		{
		    current_line = cstack.cs_line[cstack.cs_idx];
		    cstack.cs_had_while = TRUE;	    /* note we jumped there */
		}
		else /* can only get here with ":endwhile" */
		{
		    --cstack.cs_whilelevel;
		    if (cstack.cs_idx >= 0)
			--cstack.cs_idx;
		}
	    }

	    /*
	     * For a ":while" we need to remember the line number.
	     */
	    else if (cstack.cs_had_while)
	    {
		cstack.cs_had_while = FALSE;
		cstack.cs_line[cstack.cs_idx] = current_line - 1;
	    }
	}

	/*
	 * When not inside a ":while", clear remembered lines.
	 */
	if (!cstack.cs_whilelevel)
	{
	    free_cmdlines(&lines_ga);
	    current_line = 0;
	}
#endif /* WANT_EVAL */

    }
    /*
     * Continue executing command lines when:
     * - no CTRL-C typed
     * - didn't get an error message or lines are not typed
     * - there is a command after '|', inside a :if or :while, or looping for
     *	 ":source" command.
     */
    while (!got_int
	    && !(did_emsg && (getline == getexmodeline || getline == getexline))
	    && (next_cmdline != NULL
#ifdef WANT_EVAL
			|| cstack.cs_idx >= 0
#endif
			|| (flags & DOCMD_REPEAT)));

    vim_free(cmdline_copy);
#ifdef WANT_EVAL
    free_cmdlines(&lines_ga);
#endif

    /*
     * If there was too much output to fit on the command line, ask the user to
     * hit return before redrawing the screen. With the ":global" command we do
     * this only once after the command is finished.
     */
    if (did_inc)
    {
	--RedrawingDisabled;
	--no_wait_return;
	msg_scroll = FALSE;

	/*
	 * When just finished an ":if"-":else" which was typed, no need to
	 * wait for hit-return.  Also for an error situation.
	 */
	if ((count > 1 && KeyTyped && !did_emsg
#ifdef WANT_EVAL
			    && !did_endwhile
#endif
					    ) || retval == FAIL)
	{
	    need_wait_return = FALSE;
	    msg_didany = FALSE;		/* don't wait when restarting edit */
	    redraw_later(NOT_VALID);
	}
	else if ((need_wait_return || (msg_check() && !dont_wait_return)))
	{
	    /*
	     * The msg_start() above clears msg_didout. The wait_return we do
	     * here should not overwrite the command that may be shown before
	     * doing that.
	     */
	    msg_didout = msg_didout_before_start;
	    wait_return(FALSE);
	}
    }

    return retval;
}

#ifdef WANT_EVAL
    static void
free_cmdlines(gap)
    struct growarray *gap;
{
    while (gap->ga_len)
    {
	vim_free(((char_u **)(gap->ga_data))[gap->ga_len - 1]);
	--gap->ga_len;
	++gap->ga_room;
    }
}
#endif

/*
 * Execute one Ex command.
 *
 * If 'sourcing' is TRUE, the command will be included in the error message.
 *
 * 2. skip comment lines and leading space
 * 3. parse range
 * 4. parse command
 * 5. parse arguments
 * 6. switch on command name
 *
 * Note: "getline" can be NULL.
 *
 * This function may be called recursively!
 */
    static char_u *
do_one_cmd(cmdlinep, sourcing,
#ifdef WANT_EVAL
			    cstack,
#endif
				    getline, cookie)
    char_u		**cmdlinep;
    int			sourcing;
#ifdef WANT_EVAL
    struct condstack	*cstack;
#endif
    char_u		*(*getline) __ARGS((int, void *, int));
    void		*cookie;		/* argument for getline() */
{
    char_u		*p;
    char_u		*new_cmdline;
    int			i;
    int			len;
    long		argt;
    linenr_t		lnum;
    long		n;
    char_u		*errormsg = NULL;	/* error message */
    EXARG		ea;			/* Ex command arguments */

    vim_memset(&ea, 0, sizeof(ea));
    ea.line1 = 1;
    ea.line2 = 1;

	/* when not editing the last file :q has to be typed twice */
    if (quitmore)
	--quitmore;
/*
 * 2. skip comment lines and leading space and colons
 */
    for (ea.cmd = *cmdlinep; *ea.cmd == ' ' || *ea.cmd == '\t'
						  || *ea.cmd == ':'; ea.cmd++)
	;

    /* in ex mode, an empty line works like :+ */
    if (*ea.cmd == NUL && exmode_active && getline == getexmodeline)
    {
	ea.cmd = (char_u *)"+";
	ex_pressedreturn = TRUE;
    }

    if (*ea.cmd == '"' || *ea.cmd == NUL)   /* ignore comment and empty lines */
	goto doend;

#ifdef WANT_EVAL
    ea.skip = did_emsg || (cstack->cs_idx >= 0
			 && !(cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE));
#endif

/*
 * 3. parse a range specifier of the form: addr [,addr] [;addr] ..
 *
 * where 'addr' is:
 *
 * %	      (entire file)
 * $  [+-NUM]
 * 'x [+-NUM] (where x denotes a currently defined mark)
 * .  [+-NUM]
 * [+-NUM]..
 * NUM
 *
 * The ea.cmd pointer is updated to point to the first character following the
 * range spec. If an initial address is found, but no second, the upper bound
 * is equal to the lower.
 */

#ifdef WANT_EVAL
    if (ea.skip)
	goto skip_address;
#endif

    /* repeat for all ',' or ';' separated addresses */
    for (;;)
    {
	ea.line1 = ea.line2;
	ea.line2 = curwin->w_cursor.lnum;   /* default is current line number */
	ea.cmd = skipwhite(ea.cmd);
	lnum = get_address(&ea.cmd);
	if (ea.cmd == NULL)		    /* error detected */
	    goto doend;
	if (lnum == MAXLNUM)
	{
	    if (*ea.cmd == '%')		    /* '%' - all lines */
	    {
		++ea.cmd;
		ea.line1 = 1;
		ea.line2 = curbuf->b_ml.ml_line_count;
		++ea.addr_count;
	    }
	    else if (*ea.cmd == '*')	    /* '*' - visual area */
	    {
		FPOS	    *fp;

		++ea.cmd;
		fp = getmark('<', FALSE);
		if (check_mark(fp) == FAIL)
		    goto doend;
		ea.line1 = fp->lnum;
		fp = getmark('>', FALSE);
		if (check_mark(fp) == FAIL)
		    goto doend;
		ea.line2 = fp->lnum;
		++ea.addr_count;
	    }
	}
	else
	    ea.line2 = lnum;
	ea.addr_count++;

	if (*ea.cmd == ';')
	    curwin->w_cursor.lnum = ea.line2;
	else if (*ea.cmd != ',')
	    break;
	++ea.cmd;
    }

    /* One address given: set start and end lines */
    if (ea.addr_count == 1)
    {
	ea.line1 = ea.line2;
	    /* ... but only implicit: really no address given */
	if (lnum == MAXLNUM)
	    ea.addr_count = 0;
    }

    /* Don't leave the cursor on an illegal line (caused by ';') */
    check_cursor_lnum();

#ifdef WANT_EVAL
skip_address:
#endif

/*
 * 4. parse command
 */

    /*
     * Skip ':' and any white space
     */
    ea.cmd = skipwhite(ea.cmd);
    while (*ea.cmd == ':')
	ea.cmd = skipwhite(ea.cmd + 1);

    /*
     * If we got a line, but no command, then go to the line.
     * If we find a '|' or '\n' we set ea.nextcmd.
     */
    if (*ea.cmd == NUL || *ea.cmd == '"' ||
			       (ea.nextcmd = check_nextcmd(ea.cmd)) != NULL)
    {
	/*
	 * strange vi behaviour:
	 * ":3"		jumps to line 3
	 * ":3|..."	prints line 3
	 * ":|"		prints current line
	 */
#ifdef WANT_EVAL
	if (ea.skip)	    /* skip this if inside :if */
	    goto doend;
#endif
	if (*ea.cmd == '|')
	{
	    ea.cmdidx = CMD_print;
	    do_print(&ea);
	}
	else if (ea.addr_count != 0)
	{
	    if (ea.line2 == 0)
		curwin->w_cursor.lnum = 1;
	    else if (ea.line2 > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
	    else
		curwin->w_cursor.lnum = ea.line2;
	    beginline(BL_SOL | BL_FIX);
	}
	goto doend;
    }

    /*
     * Isolate the command and search for it in the command table.
     * Exeptions:
     * - the 'k' command can directly be followed by any character.
     * - the 's' command can be followed directly by 'c', 'g' or 'r'
     *	    but :sre[wind] is another command.
     */
    if (*ea.cmd == 'k')
    {
	ea.cmdidx = CMD_k;
	p = ea.cmd + 1;
    }
    else if (ea.cmd[0] == 's'
	    && (ea.cmd[1] == 'c'
		|| ea.cmd[1] == 'g'
		|| (ea.cmd[1] == 'r' && ea.cmd[2] != 'e')))
    {
	ea.cmdidx = CMD_substitute;
	p = ea.cmd + 1;
    }
    else
    {
	p = ea.cmd;
	while (isalpha(*p))
	    ++p;
	/* check for non-alpha command */
	if (p == ea.cmd && vim_strchr((char_u *)"@!=><&~#", *p) != NULL)
	    ++p;
	i = (int)(p - ea.cmd);

	if (*ea.cmd >= 'a' && *ea.cmd <= 'z')
	    ea.cmdidx = cmdidxs[*ea.cmd - 'a'];
	else
	    ea.cmdidx = cmdidxs[26];

	for ( ; ea.cmdidx < CMD_SIZE; ea.cmdidx = (CMDIDX)((int)ea.cmdidx + 1))
	    if (STRNCMP(cmdnames[ea.cmdidx].cmd_name, (char *)ea.cmd,
							      (size_t)i) == 0)
		break;
	if (i == 0 || ea.cmdidx == CMD_SIZE)
	{
#ifdef WANT_EVAL
	    if (!ea.skip)
#endif
	    {
		STRCPY(IObuff, "Not an editor command");
		if (!sourcing)
		{
		    STRCAT(IObuff, ": ");
		    STRNCAT(IObuff, *cmdlinep, 40);
		}
		errormsg = IObuff;
	    }
	    goto doend;
	}
    }

    if (*p == '!' && ea.cmdidx != CMD_substitute)    /* forced commands */
    {
	++p;
	ea.forceit = TRUE;
    }
    else
	ea.forceit = FALSE;

/*
 * 5. parse arguments
 */
    argt = cmdnames[ea.cmdidx].cmd_argt;

    if (!(argt & RANGE) && ea.addr_count)	/* no range allowed */
    {
	errormsg = e_norange;
	goto doend;
    }

    if (!(argt & BANG) && ea.forceit)		/* no <!> allowed */
    {
	errormsg = e_nobang;
	if (ea.cmdidx == CMD_help)
	    errormsg = (char_u *)"Don't panic!";
	goto doend;
    }

    /*
     * If the range is backwards, ask for confirmation and, if given, swap
     * ea.line1 & ea.line2 so it's forwards again.
     * When global command is busy, don't ask, will fail below.
     */
    if (!global_busy && ea.line1 > ea.line2)
    {
	if (sourcing)
	{
	    errormsg = (char_u *)"Backwards range given";
	    goto doend;
	}
	else if (ask_yesno((char_u *)
			   "Backwards range given, OK to swap", FALSE) != 'y')
	    goto doend;
	lnum = ea.line1;
	ea.line1 = ea.line2;
	ea.line2 = lnum;
    }
    /*
     * don't complain about the range if it is not used
     * (could happen if line_count is accidently set to 0)
     */
    if (       ea.line1 < 0
	    || ea.line2 < 0
	    || ea.line1 > ea.line2
	    || ((argt & RANGE)
		&& !(argt & NOTADR)
		&& ea.line2 > curbuf->b_ml.ml_line_count))
    {
	errormsg = e_invrange;
	goto doend;
    }

    if ((argt & NOTADR) && ea.addr_count == 0)	/* default is 1, not cursor */
	ea.line2 = 1;

    if (!(argt & ZEROR))	    /* zero in range not allowed */
    {
	if (ea.line1 == 0)
	    ea.line1 = 1;
	if (ea.line2 == 0)
	    ea.line2 = 1;
    }

#ifdef QUICKFIX
    /*
     * For the :make command we insert the 'makeprg' option here,
     * so things like % get expanded.
     */
    if (ea.cmdidx == CMD_make)
    {
	if ((new_cmdline = alloc((int)(STRLEN(p_mp) + STRLEN(p) + 2))) == NULL)
	    goto doend;		    /* out of memory */
	STRCPY(new_cmdline, p_mp);
	STRCAT(new_cmdline, " ");
	STRCAT(new_cmdline, p);
	msg_make(p);
	/* 'ea.cmd' is not set here, because it is not used at CMD_make */
	vim_free(*cmdlinep);
	*cmdlinep = new_cmdline;
	p = new_cmdline;
    }
#endif

    /*
     * Skip to start of argument.
     * Don't do this for the ":!" command, because ":!! -l" needs the space.
     */
    if (ea.cmdidx == CMD_bang)
	ea.arg = p;
    else
	ea.arg = skipwhite(p);

    if (ea.cmdidx == CMD_write || ea.cmdidx == CMD_update)
    {
	if (*ea.arg == '>')			/* append */
	{
	    if (*++ea.arg != '>')		/* typed wrong */
	    {
		errormsg = (char_u *)"Use w or w>>";
		goto doend;
	    }
	    ea.arg = skipwhite(ea.arg + 1);
	    ea.append = TRUE;
	}
	else if (*ea.arg == '!' && ea.cmdidx == CMD_write)  /* :w !filter */
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    if (ea.cmdidx == CMD_read)
    {
	if (ea.forceit)
	{
	    ea.usefilter = TRUE;		/* :r! filter if ea.forceit */
	    ea.forceit = FALSE;
	}
	else if (*ea.arg == '!')		/* :r !filter */
	{
	    ++ea.arg;
	    ea.usefilter = TRUE;
	}
    }

    if (ea.cmdidx == CMD_lshift || ea.cmdidx == CMD_rshift)
    {
	ea.amount = 1;
	while (*ea.arg == *ea.cmd)		/* count number of '>' or '<' */
	{
	    ++ea.arg;
	    ++ea.amount;
	}
	ea.arg = skipwhite(ea.arg);
    }

    /*
     * Check for "+command" argument, before checking for next command.
     * Don't do this for ":read !cmd" and ":write !cmd".
     */
    if ((argt & EDITCMD) && !ea.usefilter)
	ea.do_ecmd_cmd = getargcmd(&ea.arg);

    /*
     * Check for '|' to separate commands and '"' to start comments.
     * Don't do this for ":read !cmd" and ":write !cmd".
     */
    if ((argt & TRLBAR) && !ea.usefilter)
    {
	for (p = ea.arg; *p; ++p)
	{
	    if (*p == Ctrl('V'))
	    {
		if (argt & (USECTRLV | XFILE))
		    ++p;		/* skip CTRL-V and next char */
		else
		    STRCPY(p, p + 1);	/* remove CTRL-V and skip next char */
		if (*p == NUL)		/* stop at NUL after CTRL-V */
		    break;
	    }
	    else if ((*p == '"' && !(argt & NOTRLCOM)) ||
						      *p == '|' || *p == '\n')
	    {
		/*
		 * We remove the '\' before the '|', unless USECTRLV is used
		 * AND 'b' is present in 'cpoptions'.
		 */
		if ((vim_strchr(p_cpo, CPO_BAR) == NULL ||
				       !(argt & USECTRLV)) && *(p - 1) == '\\')
		{
		    STRCPY(p - 1, p);	/* remove the backslash */
		    --p;
		}
		else
		{
		    ea.nextcmd = check_nextcmd(p);
		    *p = NUL;
		    break;
		}
	    }
	}
	if (!(argt & NOTRLCOM))		/* remove trailing spaces */
	    del_trailing_spaces(ea.arg);
    }

    /*
     * Check for <newline> to end a shell command.
     * Also do this for ":read !cmd" and ":write !cmd".
     */
    else if (ea.cmdidx == CMD_bang || ea.usefilter)
    {
	for (p = ea.arg; *p; ++p)
	{
	    if (*p == '\\' && p[1])
		++p;
	    else if (*p == '\n')
	    {
		ea.nextcmd = p + 1;
		*p = NUL;
		break;
	    }
	}
    }

    if ((argt & DFLALL) && ea.addr_count == 0)
    {
	ea.line1 = 1;
	ea.line2 = curbuf->b_ml.ml_line_count;
    }

    /* accept numbered register only when no count allowed (:put) */
    if (       (argt & REGSTR)
	    && *ea.arg != NUL
	    && valid_yank_reg(*ea.arg, ea.cmdidx != CMD_put)
	    && !((argt & COUNT) && isdigit(*ea.arg)))
    {
	ea.regname = *ea.arg++;
#ifdef WANT_EVAL
	/* for '=' register: accept the rest of the line as an expression */
	if (ea.arg[-1] == '=' && ea.arg[0] != NUL)
	{
	    set_expr_line(vim_strsave(ea.arg));
	    ea.arg += STRLEN(ea.arg);
	}
#endif
	ea.arg = skipwhite(ea.arg);
    }

    if ((argt & COUNT) && isdigit(*ea.arg))
    {
	n = getdigits(&ea.arg);
	ea.arg = skipwhite(ea.arg);
	if (n <= 0)
	{
	    errormsg = e_zerocount;
	    goto doend;
	}
	if (argt & NOTADR)	/* e.g. :buffer 2, :sleep 3 */
	{
	    ea.line2 = n;
	    if (ea.addr_count == 0)
		ea.addr_count = 1;
	}
	else
	{
	    ea.line1 = ea.line2;
	    ea.line2 += n - 1;
	    ++ea.addr_count;
	    /*
	     * Be vi compatible: no error message for out of range.
	     */
	    if (ea.line2 > curbuf->b_ml.ml_line_count)
		ea.line2 = curbuf->b_ml.ml_line_count;
	}
    }
						/* no arguments allowed */
    if (!(argt & EXTRA) && *ea.arg != NUL &&
				 vim_strchr((char_u *)"|\"", *ea.arg) == NULL)
    {
	errormsg = e_trailing;
	goto doend;
    }

    if ((argt & NEEDARG) && *ea.arg == NUL)
    {
	errormsg = e_argreq;
	goto doend;
    }

    if (argt & XFILE)
    {
	int	    has_wildcards;	/* need to expand wildcards */
	char_u	    *repl;
	int	    srclen;

	/*
	 * Decide to expand wildcards *before* replacing '%', '#', etc.  If
	 * the file name contains a wildcard it should not cause expanding.
	 * (it will be expanded anyway if there is a wildcard before replacing).
	 */
	has_wildcards = mch_has_wildcard(ea.arg);
	for (p = ea.arg; *p; )
	{
	    /*
	     * Quick check if this cannot be the start of a special string.
	     */
	    if (vim_strchr((char_u *)"%#<", *p) == NULL)
	    {
		++p;
		continue;
	    }

	    /*
	     * Try to find a match at this position.
	     */
	    repl = eval_vars(p, &srclen, &(ea.do_ecmd_lnum), &errormsg);
	    if (errormsg != NULL)   /* error detected */
		goto doend;
	    if (repl == NULL)	    /* no match found */
	    {
		p += srclen;
		continue;
	    }

	    /*
	     * The new command line is build in new_cmdline[].
	     * First allocate it.
	     */
	    len = STRLEN(repl);
	    i = STRLEN(*cmdlinep) + len + 3;
	    if (ea.nextcmd)
		i += STRLEN(ea.nextcmd);   /* add space for next command */
	    if ((new_cmdline = alloc(i)) == NULL)
	    {
		vim_free(repl);
		goto doend;		    /* out of memory! */
	    }

	    /*
	     * Copy the stuff before the expanded part.
	     * Copy the expanded stuff.
	     * Copy what came after the expanded part.
	     * Copy the next commands, if there are any.
	     */
	    i = p - *cmdlinep;		    /* length of part before match */
	    vim_memmove(new_cmdline, *cmdlinep, (size_t)i);
	    vim_memmove(new_cmdline + i, repl, (size_t)len);
	    vim_free(repl);
	    i += len;			    /* remember the end of the string */
	    STRCPY(new_cmdline + i, p + srclen);
	    p = new_cmdline + i;	    /* remember where to continue */

	    if (ea.nextcmd)		    /* append next command */
	    {
		i = STRLEN(new_cmdline) + 1;
		STRCPY(new_cmdline + i, ea.nextcmd);
		ea.nextcmd = new_cmdline + i;
	    }
	    ea.cmd = new_cmdline + (ea.cmd - *cmdlinep);
	    ea.arg = new_cmdline + (ea.arg - *cmdlinep);
	    if (ea.do_ecmd_cmd != NULL)
		ea.do_ecmd_cmd = new_cmdline + (ea.do_ecmd_cmd - *cmdlinep);
	    vim_free(*cmdlinep);
	    *cmdlinep = new_cmdline;
	}

	/*
	 * One file argument: Expand wildcards.
	 * Don't do this with ":r !command" or ":w !command".
	 */
	if ((argt & NOSPC) && !ea.usefilter)
	{
	    /*
	     * May do this twice:
	     * 1. Replace environment variables.
	     * 2. Replace any other wildcards, remove backslashes.
	     */
	    for (n = 1; n <= 2; ++n)
	    {
		if (n == 2)
		{
#if defined(UNIX)
		    /*
		     * Only for Unix we check for more than one file name.
		     * For other systems spaces are considered to be part
		     * of the file name.
		     * Only check here if there is no wildcard, otherwise
		     * ExpandOne will check for errors. This allows
		     * ":e `ls ve*.c`" on Unix.
		     */
		    if (!has_wildcards)
			for (p = ea.arg; *p; ++p)
			{
			    /* skip escaped characters */
			    if (p[1] && (*p == '\\' || *p == Ctrl('V')))
				++p;
			    else if (vim_iswhite(*p))
			    {
				errormsg = (char_u *)
						 "Only one file name allowed";
				goto doend;
			    }
			}
#endif
		    /*
		     * halve the number of backslashes (this is Vi compatible)
		     */
		    backslash_halve(ea.arg, has_wildcards);
#ifdef macintosh
		    /*
		     * translate unix-like path components
		     */
		    slash_n_colon_adjust (ea.arg);
#endif
		}

		if (has_wildcards)
		{
		    if (n == 1)
		    {
			/*
			 * First loop: May expand environment variables.  This
			 * can be done much faster with expand_env() than with
			 * something else (e.g., calling a shell).
			 * After expanding environment variables, check again
			 * if there are still wildcards present.
			 */
			if (vim_strchr(ea.arg, '$') || vim_strchr(ea.arg, '~'))
			{
			    expand_env(ea.arg, NameBuff, MAXPATHL);
			    has_wildcards = mch_has_wildcard(NameBuff);
			    p = NameBuff;
			}
			else
			    p = NULL;
		    }
		    else /* n == 2 */
		    {
			if ((p = ExpandOne(ea.arg, NULL, WILD_LIST_NOTFOUND,
						   WILD_EXPAND_FREE)) == NULL)
			    goto doend;
		    }
		    if (p != NULL)
		    {
			/*
			 * The tricky bit here is to replace the argument,
			 * while keeping the "ea.cmd" and "ea.nextcmd" the
			 * pointers correct.
			 */
			len = ea.arg - *cmdlinep;
			i = STRLEN(p) + len;
			if (ea.nextcmd)
			    i += STRLEN(ea.nextcmd);
			if ((new_cmdline = alloc((unsigned)i + 2)) != NULL)
			{
			    STRNCPY(new_cmdline, *cmdlinep, len);
			    STRCPY(new_cmdline + len, p);
			    if (ea.nextcmd)	    /* append next command */
			    {
				i = STRLEN(new_cmdline) + 1;
				STRCPY(new_cmdline + i, ea.nextcmd);
				ea.nextcmd = new_cmdline + i;
			    }
			    ea.cmd = new_cmdline + (ea.cmd - *cmdlinep);
			    ea.arg = new_cmdline + len;
			    vim_free(*cmdlinep);
			    *cmdlinep = new_cmdline;
			}
			if (n == 2)	/* p came from ExpandOne() */
			    vim_free(p);
		    }
		}
	    }
	}
    }

#ifdef WANT_EVAL
    /*
     * Skip the command when it's not going to be executed.
     * The commands like :if, :endif, etc. always need to be executed.
     * Also make an exception for commands that handle a trailing command
     * themselves.
     */
    if (ea.skip)
    {
	switch (ea.cmdidx)
	{
	    /* commands that need evaluation */
	    case CMD_while:
	    case CMD_endwhile:
	    case CMD_if:
	    case CMD_elseif:
	    case CMD_else:
	    case CMD_endif:	break;

	    /* commands that handle '|' themselves */
	    case CMD_djump:
	    case CMD_dlist:
	    case CMD_dsearch:
	    case CMD_dsplit:
	    case CMD_echo:
	    case CMD_echon:
	    case CMD_execute:
	    case CMD_help:
	    case CMD_ijump:
	    case CMD_ilist:
	    case CMD_isearch:
	    case CMD_isplit:
	    case CMD_let:
	    case CMD_substitute:
	    case CMD_syntax:
	    case CMD_and:
	    case CMD_tilde:	break;

	    default:		goto doend;
	}
    }
#endif

    /*
     * Accept buffer name.  Cannot be used at the same time with a buffer
     * number.
     */
    if ((argt & BUFNAME) && *ea.arg && ea.addr_count == 0)
    {
	/*
	 * :bdelete and :bunload take several arguments, separated by spaces:
	 * find next space (skipping over escaped characters).
	 * The others take one argument: ignore trailing spaces.
	 */
	if (ea.cmdidx == CMD_bdelete || ea.cmdidx == CMD_bunload)
	    p = skiptowhite_esc(ea.arg);
	else
	{
	    p = ea.arg + STRLEN(ea.arg);
	    while (p > ea.arg && vim_iswhite(p[-1]))
		--p;
	}
	ea.line2 = buflist_findpat(ea.arg, p);
	if (ea.line2 < 0)	    /* failed */
	    goto doend;
	ea.addr_count = 1;
	ea.arg = skipwhite(p);
    }

/*
 * 6. switch on command name
 *
 * The "ea" structure holds the arguments that can be used.
 */
    switch (ea.cmdidx)
    {
	case CMD_quit:
		do_quit(&ea);
		break;

	case CMD_qall:
		do_quit_all(ea.forceit);
		break;

	case CMD_close:
		do_close(&ea);
		break;

	case CMD_hide:
		close_window(curwin, FALSE);	/* don't free buffer */
		break;

	case CMD_only:
		close_others(TRUE, ea.forceit);
		break;

	case CMD_stop:
	case CMD_suspend:
		do_suspend(ea.forceit);
		break;

	case CMD_exit:
	case CMD_xit:
	case CMD_wq:
		do_exit(&ea);
		break;

	case CMD_xall:
	case CMD_wqall:
		exiting = TRUE;
		/* FALLTHROUGH */

	case CMD_wall:
		do_wqall(&ea);
		break;

	case CMD_preserve:
		ml_preserve(curbuf, TRUE);
		break;

	case CMD_recover:
		do_recover(&ea);
		break;

	case CMD_args:
		do_args(&ea);
		break;

	case CMD_wnext:
	case CMD_wNext:
	case CMD_wprevious:
		do_wnext(&ea);
		break;

	case CMD_next:
	case CMD_snext:
		do_next(&ea);
		break;

	case CMD_previous:
	case CMD_sprevious:
	case CMD_Next:
	case CMD_sNext:
		do_argfile(&ea, curwin->w_arg_idx - (int)ea.line2);
		break;

	case CMD_rewind:
	case CMD_srewind:
		do_argfile(&ea, 0);
		break;

	case CMD_last:
	case CMD_slast:
		do_argfile(&ea, arg_file_count - 1);
		break;

	case CMD_argument:
	case CMD_sargument:
		if (ea.addr_count)
		    i = ea.line2 - 1;
		else
		    i = curwin->w_arg_idx;
		do_argfile(&ea, i);
		break;

	case CMD_all:
	case CMD_sall:
		if (ea.addr_count == 0)
		    ea.line2 = 9999;
		do_arg_all((int)ea.line2, ea.forceit);
		break;

	case CMD_buffer:	/* :[N]buffer [N]	to buffer N */
	case CMD_sbuffer:	/* :[N]sbuffer [N]	to buffer N */
		if (*ea.arg)
		    errormsg = e_trailing;
		else
		{
		    if (ea.addr_count == 0)	/* default is current buffer */
			(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT
						       : DOBUF_GOTO,
				       DOBUF_CURRENT, FORWARD, 0, ea.forceit);
		    else
			(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT
						       : DOBUF_GOTO,
			     DOBUF_FIRST, FORWARD, (int)ea.line2, ea.forceit);
		}
		break;

	case CMD_bmodified:	/* :[N]bmod [N]		to next mod. buffer */
	case CMD_sbmodified:	/* :[N]sbmod [N]	to next mod. buffer */
		(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO,
			       DOBUF_MOD, FORWARD, (int)ea.line2, ea.forceit);
		break;

	case CMD_bnext:		/* :[N]bnext [N]	to next buffer */
	case CMD_sbnext:	/* :[N]sbnext [N]	to next buffer */
		(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO,
			   DOBUF_CURRENT, FORWARD, (int)ea.line2, ea.forceit);
		break;

	case CMD_bNext:		/* :[N]bNext [N]	to previous buffer */
	case CMD_bprevious:	/* :[N]bprevious [N]	to previous buffer */
	case CMD_sbNext:	/* :[N]sbNext [N]	to previous buffer */
	case CMD_sbprevious:	/* :[N]sbprevious [N]	to previous buffer */
		(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO,
			  DOBUF_CURRENT, BACKWARD, (int)ea.line2, ea.forceit);
		break;

	case CMD_brewind:	/* :brewind		to first buffer */
	case CMD_sbrewind:	/* :sbrewind		to first buffer */
		(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO,
					 DOBUF_FIRST, FORWARD, 0, ea.forceit);
		break;

	case CMD_blast:		/* :blast		to last buffer */
	case CMD_sblast:	/* :sblast		to last buffer */
		(void)do_buffer(*ea.cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO,
					  DOBUF_LAST, FORWARD, 0, ea.forceit);
		break;

	case CMD_bunload:	/* :[N]bunload[!] [N] [bufname] unload buffer */
	case CMD_bdelete:	/* :[N]bdelete[!] [N] [bufname] delete buffer */
		errormsg = do_bufdel(ea.cmdidx == CMD_bdelete ? DOBUF_DEL
							      : DOBUF_UNLOAD,
					 ea.arg, ea.addr_count, (int)ea.line1,
						   (int)ea.line2, ea.forceit);
		break;

	case CMD_unhide:
	case CMD_sunhide:
		if (ea.addr_count == 0)
		    ea.line2 = 9999;
		(void)do_buffer_all((int)ea.line2, FALSE);
		break;

	case CMD_ball:
	case CMD_sball:
		if (ea.addr_count == 0)
		    ea.line2 = 9999;
		(void)do_buffer_all((int)ea.line2, TRUE);
		break;

	case CMD_buffers:
	case CMD_files:
	case CMD_ls:
		buflist_list();
		break;

	case CMD_update:
		if (curbuf_changed())
		    (void)do_write(&ea);
		break;

	case CMD_write:
		if (ea.usefilter)	/* input lines to shell command */
		    do_bang(1, ea.line1, ea.line2, FALSE, ea.arg, TRUE, FALSE);
		else
		    (void)do_write(&ea);
		break;

	/*
	 * set screen mode
	 * if no argument given, just get the screen size and redraw
	 */
	case CMD_mode:
		if (*ea.arg == NUL || mch_screenmode(ea.arg) != FAIL)
		    set_winsize(0, 0, FALSE);
		break;

	case CMD_resize:
		do_resize(&ea);
		break;

	case CMD_sview:
	case CMD_split:
	case CMD_new:
		do_splitview(&ea);
		break;

	case CMD_edit:
	case CMD_ex:
	case CMD_visual:
	case CMD_view:
		do_exedit(&ea, NULL);
		break;

#ifdef USE_GUI
	/*
	 * Change from the terminal version to the GUI version.  File names
	 * may be given to redefine the args list -- webb
	 */
	case CMD_gvim:
	case CMD_gui:
		do_gui(&ea);
		break;
#endif

	case CMD_file:
		do_file(ea.arg, ea.forceit);
		break;

	case CMD_swapname:
		do_swapname();
		break;

	case CMD_read:
		do_read(&ea);
		break;

	case CMD_cd:
	case CMD_chdir:
		do_cd(&ea);
		break;

	case CMD_pwd:
		do_pwd();
		break;

	case CMD_equal:
		smsg((char_u *)"line %ld", (long)ea.line2);
		break;

	case CMD_list:
		i = curwin->w_p_list;
		curwin->w_p_list = 1;
		do_print(&ea);
		curwin->w_p_list = i;
		break;

	case CMD_number:
	case CMD_pound:			/* :# */
	case CMD_print:
		do_print(&ea);
		break;

	case CMD_shell:
		do_shell(NULL, 0);
		break;

	case CMD_sleep:
		do_sleep(&ea);
		break;

	case CMD_stag:
		postponed_split = -1;
		/*FALLTHROUGH*/
	case CMD_tag:
		do_tag(ea.arg, DT_TAG,
			ea.addr_count ? (int)ea.line2 : 1, ea.forceit);
		break;

	case CMD_stselect:
		postponed_split = -1;
		/*FALLTHROUGH*/
	case CMD_tselect:
		do_tag(ea.arg, DT_SELECT, 0, ea.forceit);
		break;

	case CMD_pop:		do_ex_tag(&ea, DT_POP); break;
	case CMD_tnext:		do_ex_tag(&ea, DT_NEXT); break;
	case CMD_tNext:
	case CMD_tprevious:	do_ex_tag(&ea, DT_PREV); break;
	case CMD_trewind:	do_ex_tag(&ea, DT_FIRST); break;
	case CMD_tlast:		do_ex_tag(&ea, DT_LAST); break;

	case CMD_tags:
		do_tags();
		break;

	case CMD_marks:
		do_marks(ea.arg);
		break;

	case CMD_jumps:
		do_jumps();
		break;

	case CMD_ascii:
		do_ascii();
		break;

#ifdef FIND_IN_PATH
	case CMD_checkpath:
		find_pattern_in_path(NULL, 0, 0, FALSE, FALSE, CHECK_PATH, 1L,
				   ea.forceit ? ACTION_SHOW_ALL : ACTION_SHOW,
					      (linenr_t)1, (linenr_t)MAXLNUM);
		break;
#endif

	case CMD_digraphs:
#ifdef DIGRAPHS
		if (*ea.arg)
		    putdigraph(ea.arg);
		else
		    listdigraphs();
#else
		EMSG("No digraphs in this version");
#endif
		break;

	case CMD_set:
		(void)do_set(ea.arg);
		break;

	case CMD_fixdel:
		do_fixdel();
		break;

#ifdef AUTOCMD
	case CMD_augroup:
	case CMD_autocmd:
		/*
		 * Disallow auto commands from .exrc and .vimrc in current
		 * directory for security reasons.
		 */
		if (secure)
		{
		    secure = 2;
		    errormsg = e_curdir;
		}
		else if (ea.cmdidx == CMD_autocmd)
		    do_autocmd(ea.arg, ea.forceit);
		else
		    do_augroup(ea.arg);
		break;

	/*
	 * Apply the automatic commands to all loaded buffers.
	 */
	case CMD_doautoall:
		do_autoall(ea.arg);
		break;

	/*
	 * Apply the automatic commands to the current buffer.
	 */
	case CMD_doautocmd:
		(void)do_doautocmd(ea.arg, TRUE);
		do_modelines();
		break;
#endif

	case CMD_abbreviate:
	case CMD_noreabbrev:
	case CMD_unabbreviate:
	case CMD_cabbrev:
	case CMD_cnoreabbrev:
	case CMD_cunabbrev:
	case CMD_iabbrev:
	case CMD_inoreabbrev:
	case CMD_iunabbrev:
		do_exmap(&ea, TRUE);	    /* almost the same as mapping */
		break;

	case CMD_map:
	case CMD_nmap:
	case CMD_vmap:
	case CMD_omap:
	case CMD_cmap:
	case CMD_imap:
	case CMD_noremap:
	case CMD_nnoremap:
	case CMD_vnoremap:
	case CMD_onoremap:
	case CMD_cnoremap:
	case CMD_inoremap:
		/*
		 * If we are sourcing .exrc or .vimrc in current directory we
		 * print the mappings for security reasons.
		 */
		if (secure)
		{
		    secure = 2;
		    msg_outtrans(ea.cmd);
		    msg_putchar('\n');
		}
	case CMD_unmap:
	case CMD_nunmap:
	case CMD_vunmap:
	case CMD_ounmap:
	case CMD_cunmap:
	case CMD_iunmap:
		do_exmap(&ea, FALSE);
		break;

	case CMD_mapclear:
	case CMD_nmapclear:
	case CMD_vmapclear:
	case CMD_omapclear:
	case CMD_cmapclear:
	case CMD_imapclear:
		map_clear(ea.cmd, ea.forceit, FALSE);
		break;

	case CMD_abclear:
	case CMD_iabclear:
	case CMD_cabclear:
		map_clear(ea.cmd, TRUE, TRUE);
		break;

#ifdef USE_GUI
	case CMD_menu:	    case CMD_noremenu:	    case CMD_unmenu:
	case CMD_amenu:	    case CMD_anoremenu:	    case CMD_aunmenu:
	case CMD_nmenu:	    case CMD_nnoremenu:	    case CMD_nunmenu:
	case CMD_vmenu:	    case CMD_vnoremenu:	    case CMD_vunmenu:
	case CMD_omenu:	    case CMD_onoremenu:	    case CMD_ounmenu:
	case CMD_imenu:	    case CMD_inoremenu:	    case CMD_iunmenu:
	case CMD_cmenu:	    case CMD_cnoremenu:	    case CMD_cunmenu:
		gui_do_menu(&ea);
		break;
#endif

	case CMD_display:
	case CMD_registers:
		do_dis(ea.arg);
		break;

	case CMD_help:
		do_help(&ea);
		break;

	case CMD_version:
		do_version(ea.arg);
		break;

	case CMD_winsize:
		do_winsize(ea.arg);
		break;

	case CMD_delete:
	case CMD_yank:
	case CMD_rshift:
	case CMD_lshift:
		do_exops(&ea);
		break;

	case CMD_put:
		/* ":0put" works like ":1put!". */
		if (ea.line2 == 0)
		{
		    ea.line2 = 1;
		    ea.forceit = TRUE;
		}
		curwin->w_cursor.lnum = ea.line2;
		do_put(ea.regname, ea.forceit ? BACKWARD : FORWARD, -1L, FALSE);
		break;

	case CMD_t:
	case CMD_copy:
	case CMD_move:
		do_copymove(&ea);
		break;

	case CMD_coffee:
		MSG("Insert the beans please.");
		break;

	case CMD_and:		/* :& */
	case CMD_tilde:		/* :~ */
	case CMD_substitute:	/* :s */
		do_sub(&ea);
		break;

	case CMD_join:
		do_exjoin(&ea);
		break;

	case CMD_global:
		if (ea.forceit)
		    *ea.cmd = 'v';
	case CMD_vglobal:
		do_glob(&ea);
		break;

	case CMD_at:		    /* :[addr]@r */
		do_exat(&ea);
		break;

	case CMD_bang:
		do_bang(ea.addr_count, ea.line1, ea.line2,
					      ea.forceit, ea.arg, TRUE, TRUE);
		break;

	case CMD_undo:
		u_undo(1);
		break;

	case CMD_redo:
		u_redo(1);
		break;

	case CMD_source:
		if (ea.forceit)			/* :so! read vi commands */
		    (void)openscript(ea.arg);
						/* :so read ex commands */
		else if (do_source(ea.arg, FALSE, FALSE) == FAIL)
		    emsg2(e_notopen, ea.arg);
		break;

#ifdef VIMINFO
	case CMD_rviminfo:
		p = p_viminfo;
		if (*p_viminfo == NUL)
		    p_viminfo = (char_u *)"'100";
		if (read_viminfo(ea.arg, TRUE, TRUE, ea.forceit) == FAIL)
		    EMSG("Cannot open viminfo file for reading");
		p_viminfo = p;
		break;

	case CMD_wviminfo:
		p = p_viminfo;
		if (*p_viminfo == NUL)
		    p_viminfo = (char_u *)"'100";
		write_viminfo(ea.arg, ea.forceit);
		p_viminfo = p;
		break;
#endif /* VIMINFO */

	case CMD_redir:
		do_redir(&ea);
		break;

	case CMD_mkvimrc:
		if (*ea.arg == NUL)
		    ea.arg = (char_u *)VIMRC_FILE;
		/*FALLTHROUGH*/

	case CMD_mkexrc:
		do_mkrc(&ea);
		break;

#ifdef QUICKFIX
	case CMD_cc:
		qf_jump(0, ea.addr_count ? (int)ea.line2 : 0, ea.forceit);
		break;

	case CMD_cfile:
		do_cfile(&ea);
		break;

	case CMD_clist:
		qf_list(ea.forceit);
		break;

	case CMD_crewind:
		qf_jump(0, ea.addr_count ? (int)ea.line2 : 1, ea.forceit);
		break;

	case CMD_clast:
		qf_jump(0, ea.addr_count ? (int)ea.line2 : 32767, ea.forceit);
		break;

	case CMD_cnext:
		qf_jump(FORWARD, ea.addr_count ? (int)ea.line2 : 1, ea.forceit);
		break;

	case CMD_cNext:
	case CMD_cprevious:
		qf_jump(BACKWARD, ea.addr_count ? (int)ea.line2 : 1,
								  ea.forceit);
		break;
#endif

	case CMD_cquit:
		getout(1);	/* this does not always pass on the exit
				   code to the Manx compiler. why? */

	case CMD_mark:
	case CMD_k:
		do_setmark(&ea);
		break;

#ifdef EX_EXTRA
	case CMD_center:
	case CMD_right:
	case CMD_left:
		do_align(&ea);
		break;

	case CMD_retab:
		do_retab(&ea);
		break;

	case CMD_normal:
		do_normal(&ea);
		break;
#endif

#ifdef QUICKFIX
	case CMD_make:
		do_make(ea.arg);
		break;
#endif

#ifdef FIND_IN_PATH
	case CMD_isearch:
	case CMD_dsearch:
		errormsg = do_findpat(&ea, ACTION_SHOW);
		break;

	case CMD_ilist:
	case CMD_dlist:
		errormsg = do_findpat(&ea, ACTION_SHOW_ALL);
		break;

	case CMD_ijump:
	case CMD_djump:
		errormsg = do_findpat(&ea, ACTION_GOTO);
		break;

	case CMD_isplit:
	case CMD_dsplit:
		errormsg = do_findpat(&ea, ACTION_SPLIT);
		break;
#endif

#ifdef SYNTAX_HL
	case CMD_syntax:
		do_syntax(&ea);
		break;
#endif

	case CMD_highlight:
		do_highlight(ea.arg, ea.forceit);
		break;

#ifdef WANT_EVAL
	case CMD_echo:
	case CMD_echon:
		do_echo(&ea, ea.cmdidx == CMD_echo);
		break;

	case CMD_execute:
		do_execute(&ea, getline, cookie);
		break;

	case CMD_if:
		errormsg = do_if(&ea, cstack);
		break;

	case CMD_elseif:
	case CMD_else:
		errormsg = do_else(&ea, cstack);
		break;

	case CMD_endif:
		if (cstack->cs_idx < 0
			|| (cstack->cs_flags[cstack->cs_idx] & CSF_WHILE))
		    errormsg = (char_u *)":endif without :if";
		else
		    --cstack->cs_idx;
		break;

	case CMD_while:
		errormsg = do_while(&ea, cstack);
		break;

	case CMD_continue:
		errormsg = do_continue(cstack);
		break;

	case CMD_break:
		errormsg = do_break(cstack);
		break;

	case CMD_endwhile:
		errormsg = do_endwhile(cstack);
		break;

	case CMD_let:
		do_let(&ea);
		break;

	case CMD_unlet:
		do_unlet(ea.arg);
		break;
#endif /* WANT_EVAL */

	case CMD_insert:
		do_append(ea.line2 - 1, getline, cookie);
		ex_no_reprint = TRUE;
		break;

	case CMD_append:
		do_append(ea.line2, getline, cookie);
		ex_no_reprint = TRUE;
		break;

	case CMD_change:
		do_change(ea.line1, ea.line2, getline, cookie);
		ex_no_reprint = TRUE;
		break;

	case CMD_z:
		do_z(ea.line2, ea.arg);
		ex_no_reprint = TRUE;
		break;

	case CMD_intro:
		do_intro();
		break;

#ifdef HAVE_PERL_INTERP
	case CMD_perl:
		do_perl(&ea);
		break;

	case CMD_perldo:
		do_perldo(&ea);
		break;
#endif

#ifdef HAVE_PYTHON
	case CMD_python:
		do_python(&ea);
		break;

	case CMD_pyfile:
		do_pyfile(&ea);
		break;
#endif

#ifdef USE_SNIFF
	case CMD_sniff:
		do_sniff(ea.arg);
		break;
#endif

	default:
		/* Illegal commands have already been handled */
		errormsg = (char_u *)"Sorry, this command is not implemented";
    }


doend:
    if (curwin->w_cursor.lnum == 0)	/* can happen with zero line number */
	curwin->w_cursor.lnum = 1;

    if (errormsg != NULL && *errormsg != NUL && !did_emsg)
    {
	emsg(errormsg);
	if (sourcing)
	{
	    MSG_PUTS(": ");
	    msg_outtrans(*cmdlinep);
	}
    }
    if (ea.nextcmd && *ea.nextcmd == NUL)	/* not really a next command */
	ea.nextcmd = NULL;
    return ea.nextcmd;
}

/*
 * This is all pretty much copied from do_one_cmd(), with all the extra stuff
 * we don't need/want deleted.	Maybe this could be done better if we didn't
 * repeat all this stuff.  The only problem is that they may not stay perfectly
 * compatible with each other, but then the command line syntax probably won't
 * change that much -- webb.
 */
    char_u *
set_one_cmd_context(buff)
    char_u	*buff;	    /* buffer for command string */
{
    char_u		*p;
    char_u		*cmd, *arg;
    int			i;
    CMDIDX		cmdidx;
    long		argt;
    char_u		delim;
    int			forceit = FALSE;
    int			usefilter = FALSE;  /* filter instead of file name */

    expand_pattern = buff;
    expand_context = EXPAND_COMMANDS;	/* Default until we get past command */

/*
 * 2. skip comment lines and leading space, colons or bars
 */
    for (cmd = buff; vim_strchr((char_u *)" \t:|", *cmd) != NULL; cmd++)
	;
    expand_pattern = cmd;

    if (*cmd == NUL)
	return NULL;
    if (*cmd == '"')	    /* ignore comment lines */
    {
	expand_context = EXPAND_NOTHING;
	return NULL;
    }

/*
 * 3. parse a range specifier of the form: addr [,addr] [;addr] ..
 */
    /*
     * Backslashed delimiters after / or ? will be skipped, and commands will
     * not be expanded between /'s and ?'s or after "'". -- webb
     */
    while (*cmd != NUL && (vim_isspace(*cmd) || isdigit(*cmd) ||
			    vim_strchr((char_u *)".$%*'/?-+,;", *cmd) != NULL))
    {
	if (*cmd == '\'')
	{
	    if (*++cmd == NUL)
		expand_context = EXPAND_NOTHING;
	}
	else if (*cmd == '/' || *cmd == '?')
	{
	    delim = *cmd++;
	    while (*cmd != NUL && *cmd != delim)
		if (*cmd++ == '\\' && *cmd != NUL)
		    ++cmd;
	    if (*cmd == NUL)
		expand_context = EXPAND_NOTHING;
	}
	if (*cmd != NUL)
	    ++cmd;
    }

/*
 * 4. parse command
 */

    cmd = skipwhite(cmd);
    expand_pattern = cmd;
    if (*cmd == NUL)
	return NULL;
    if (*cmd == '"')
    {
	expand_context = EXPAND_NOTHING;
	return NULL;
    }

    if (*cmd == '|' || *cmd == '\n')
	return cmd + 1;			/* There's another command */

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
    else
    {
	p = cmd;
	while (isalpha(*p) || *p == '*')    /* Allow * wild card */
	    ++p;
	    /* check for non-alpha command */
	if (p == cmd && vim_strchr((char_u *)"@!=><&~#", *p) != NULL)
	    ++p;
	i = (int)(p - cmd);

	if (i == 0)
	{
	    expand_context = EXPAND_UNSUCCESSFUL;
	    return NULL;
	}
	for (cmdidx = (CMDIDX)0; cmdidx < CMD_SIZE;
					   cmdidx = (CMDIDX)((int)cmdidx + 1))
	    if (STRNCMP(cmdnames[cmdidx].cmd_name, cmd, (size_t)i) == 0)
		break;
    }

    /*
     * If the cursor is touching the command, and it ends in an alphabetic
     * character, complete the command name.
     */
    if (*p == NUL && isalpha(p[-1]))
	return NULL;

    if (cmdidx == CMD_SIZE)
    {
	if (*cmd == 's' && vim_strchr((char_u *)"cgr", cmd[1]) != NULL)
	{
	    cmdidx = CMD_substitute;
	    p = cmd + 1;
	}
	else
	{
	    /* Not still touching the command and it was an illegal command */
	    expand_context = EXPAND_UNSUCCESSFUL;
	    return NULL;
	}
    }

    expand_context = EXPAND_NOTHING; /* Default now that we're past command */

    if (*p == '!')		    /* forced commands */
    {
	forceit = TRUE;
	++p;
    }

/*
 * 5. parse arguments
 */
    argt = cmdnames[cmdidx].cmd_argt;

    arg = skipwhite(p);

    if (cmdidx == CMD_write || cmdidx == CMD_update)
    {
	if (*arg == '>')			/* append */
	{
	    if (*++arg == '>')
		++arg;
	    arg = skipwhite(arg);
	}
	else if (*arg == '!' && cmdidx == CMD_write)	/* :w !filter */
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    if (cmdidx == CMD_read)
    {
	usefilter = forceit;			/* :r! filter if forced */
	if (*arg == '!')			/* :r !filter */
	{
	    ++arg;
	    usefilter = TRUE;
	}
    }

    if (cmdidx == CMD_lshift || cmdidx == CMD_rshift)
    {
	while (*arg == *cmd)	    /* allow any number of '>' or '<' */
	    ++arg;
	arg = skipwhite(arg);
    }

    /* Does command allow "+command"? */
    if ((argt & EDITCMD) && !usefilter && *arg == '+')
    {
	/* Check if we're in the +command */
	p = arg + 1;
	arg = skip_cmd_arg(arg);

	/* Still touching the command after '+'? */
	if (*arg == NUL)
	    return p;

	/* Skip space(s) after +command to get to the real argument */
	arg = skipwhite(arg);
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
		if (p[1] != NUL)
		    ++p;
	    }
	    else if ( (*p == '"' && !(argt & NOTRLCOM))
		    || *p == '|' || *p == '\n')
	    {
		if (*(p - 1) != '\\')
		{
		    if (*p == '|' || *p == '\n')
			return p + 1;
		    return NULL;    /* It's a comment */
		}
	    }
	    ++p;
	}
    }

						/* no arguments allowed */
    if (!(argt & EXTRA) && *arg != NUL &&
				    vim_strchr((char_u *)"|\"", *arg) == NULL)
	return NULL;

    /* Find start of last argument (argument just before cursor): */
    p = buff + STRLEN(buff);
    while (p != arg && *p != ' ' && *p != TAB)
	p--;
    if (*p == ' ' || *p == TAB)
	p++;
    expand_pattern = p;

    if (argt & XFILE)
    {
	int in_quote = FALSE;
	char_u *bow = NULL;	/* Beginning of word */

	/*
	 * Allow spaces within back-quotes to count as part of the argument
	 * being expanded.
	 */
	expand_pattern = skipwhite(arg);
	for (p = expand_pattern; *p; ++p)
	{
	    if (*p == '\\' && p[1])
		++p;
#ifdef SPACE_IN_FILENAME
	    else if (vim_iswhite(*p) && (!(argt & NOSPC) || usefilter))
#else
	    else if (vim_iswhite(*p))
#endif
	    {
		p = skipwhite(p);
		if (in_quote)
		    bow = p;
		else
		    expand_pattern = p;
		--p;
	    }
	    else if (*p == '`')
	    {
		if (!in_quote)
		{
		    expand_pattern = p;
		    bow = p + 1;
		}
		in_quote = !in_quote;
	    }
	}

	/*
	 * If we are still inside the quotes, and we passed a space, just
	 * expand from there.
	 */
	if (bow != NULL && in_quote)
	    expand_pattern = bow;
	expand_context = EXPAND_FILES;
    }

/*
 * 6. switch on command name
 */
    switch (cmdidx)
    {
	case CMD_cd:
	case CMD_chdir:
	    expand_context = EXPAND_DIRECTORIES;
	    break;
	case CMD_global:
	case CMD_vglobal:
	    delim = *arg;	    /* get the delimiter */
	    if (delim)
		++arg;		    /* skip delimiter if there is one */

	    while (arg[0] != NUL && arg[0] != delim)
	    {
		if (arg[0] == '\\' && arg[1] != NUL)
		    ++arg;
		++arg;
	    }
	    if (arg[0] != NUL)
		return arg + 1;
	    break;
	case CMD_and:
	case CMD_substitute:
	    delim = *arg;
	    if (delim)
		++arg;
	    for (i = 0; i < 2; i++)
	    {
		while (arg[0] != NUL && arg[0] != delim)
		{
		    if (arg[0] == '\\' && arg[1] != NUL)
			++arg;
		    ++arg;
		}
		if (arg[0] != NUL)	/* skip delimiter */
		    ++arg;
	    }
	    while (arg[0] && vim_strchr((char_u *)"|\"#", arg[0]) == NULL)
		++arg;
	    if (arg[0] != NUL)
		return arg;
	    break;
	case CMD_isearch:
	case CMD_dsearch:
	case CMD_ilist:
	case CMD_dlist:
	case CMD_ijump:
	case CMD_djump:
	case CMD_isplit:
	case CMD_dsplit:
	    arg = skipwhite(skipdigits(arg));	    /* skip count */
	    if (*arg == '/')	/* Match regexp, not just whole words */
	    {
		for (++arg; *arg && *arg != '/'; arg++)
		    if (*arg == '\\' && arg[1] != NUL)
			arg++;
		if (*arg)
		{
		    arg = skipwhite(arg + 1);

		    /* Check for trailing illegal characters */
		    if (*arg && vim_strchr((char_u *)"|\"\n", *arg) == NULL)
			expand_context = EXPAND_NOTHING;
		    else
			return arg;
		}
	    }
	    break;
#ifdef AUTOCMD
	case CMD_autocmd:
	    return set_context_in_autocmd(arg, FALSE);

	case CMD_doautocmd:
	    return set_context_in_autocmd(arg, TRUE);
#endif
	case CMD_set:
	    set_context_in_set_cmd(arg);
	    break;
	case CMD_stag:
	case CMD_tag:
	case CMD_stselect:
	case CMD_tselect:
	    expand_context = EXPAND_TAGS;
	    expand_pattern = arg;
	    break;
	case CMD_help:
	    expand_context = EXPAND_HELP;
	    expand_pattern = arg;
	    break;
	case CMD_augroup:
	    expand_context = EXPAND_AUGROUP;
	    expand_pattern = arg;
	    break;
#ifdef SYNTAX_HL
	case CMD_syntax:
	    set_context_in_syntax_cmd(arg);
	    break;
#endif
	case CMD_highlight:
	    set_context_in_highlight_cmd(arg);
	    break;
	case CMD_bdelete:
	case CMD_bunload:
	    while ((expand_pattern = vim_strchr(arg, ' ')) != NULL)
		arg = expand_pattern + 1;
	case CMD_buffer:
	case CMD_sbuffer:
	    expand_context = EXPAND_BUFFERS;
	    expand_pattern = arg;
	    break;
#ifdef USE_GUI
	case CMD_menu:	    case CMD_noremenu:	    case CMD_unmenu:
	case CMD_amenu:	    case CMD_anoremenu:	    case CMD_aunmenu:
	case CMD_nmenu:	    case CMD_nnoremenu:	    case CMD_nunmenu:
	case CMD_vmenu:	    case CMD_vnoremenu:	    case CMD_vunmenu:
	case CMD_omenu:	    case CMD_onoremenu:	    case CMD_ounmenu:
	case CMD_imenu:	    case CMD_inoremenu:	    case CMD_iunmenu:
	case CMD_cmenu:	    case CMD_cnoremenu:	    case CMD_cunmenu:
	    return gui_set_context_in_menu_cmd(cmd, arg, forceit);
#endif
	default:
	    break;
    }
    return NULL;
}

/*
 * get a single EX address
 *
 * Set ptr to the next character after the part that was interpreted.
 * Set ptr to NULL when an error is encountered.
 *
 * Return MAXLNUM when no Ex address was found.
 */
    static linenr_t
get_address(ptr)
    char_u	**ptr;
{
    int		c;
    int		i;
    long	n;
    char_u	*cmd;
    FPOS	pos;
    FPOS	*fp;
    linenr_t	lnum;

    cmd = skipwhite(*ptr);
    lnum = MAXLNUM;
    do
    {
	switch (*cmd)
	{
	    case '.':			    /* '.' - Cursor position */
			++cmd;
			lnum = curwin->w_cursor.lnum;
			break;

	    case '$':			    /* '$' - last line */
			++cmd;
			lnum = curbuf->b_ml.ml_line_count;
			break;

	    case '\'':			    /* ''' - mark */
			if (*++cmd == NUL || (check_mark(
					fp = getmark(*cmd++, FALSE)) == FAIL))
			{
			    cmd = NULL;
			    goto error;
			}
			lnum = fp->lnum;
			break;

	    case '/':
	    case '?':			    /* '/' or '?' - search */
			c = *cmd++;
			pos = curwin->w_cursor;	    /* save curwin->w_cursor */
			/*
			 * When '/' or '?' follows another address, start from
			 * there.
			 */
			if (lnum != MAXLNUM)
			    curwin->w_cursor.lnum = lnum;
			/*
			 * Start a forward search at the end of the line.
			 * Start a backward search at the start of the line.
			 * This makes sure we never match in the current line,
			 * and can match anywhere in the next/previous line.
			 */
			if (c == '/')
			    curwin->w_cursor.col = MAXCOL;
			else
			    curwin->w_cursor.col = 0;
			searchcmdlen = 0;
			if (!do_search(NULL, c, cmd, 1L,
				      SEARCH_HIS + SEARCH_MSG + SEARCH_START))
			{
			    curwin->w_cursor = pos;
			    cmd = NULL;
			    goto error;
			}
			lnum = curwin->w_cursor.lnum;
			curwin->w_cursor = pos;
					    /* adjust command string pointer */
			cmd += searchcmdlen;
			break;

	    case '\\':		    /* "\?", "\/" or "\&", repeat search */
			++cmd;
			if (*cmd == '&')
			    i = RE_SUBST;
			else if (*cmd == '?' || *cmd == '/')
			    i = RE_SEARCH;
			else
			{
			    emsg(e_backslash);
			    cmd = NULL;
			    goto error;
			}

			/*
			 * When search follows another address, start from
			 * there.
			 */
			if (lnum != MAXLNUM)
			    pos.lnum = lnum;
			else
			    pos.lnum = curwin->w_cursor.lnum;

			/*
			 * Start the search just like for the above do_search().
			 */
			if (*cmd != '?')
			    pos.col = MAXCOL;
			else
			    pos.col = 0;
			if (searchit(curbuf, &pos,
				    *cmd == '?' ? BACKWARD : FORWARD,
				    (char_u *)"", 1L,
				    SEARCH_MSG + SEARCH_START, i) == OK)
			    lnum = pos.lnum;
			else
			{
			    cmd = NULL;
			    goto error;
			}
			++cmd;
			break;

	    default:
			if (isdigit(*cmd))	/* absolute line number */
			    lnum = getdigits(&cmd);
	}

	for (;;)
	{
	    cmd = skipwhite(cmd);
	    if (*cmd != '-' && *cmd != '+' && !isdigit(*cmd))
		break;

	    if (lnum == MAXLNUM)
		lnum = curwin->w_cursor.lnum;	/* "+1" is same as ".+1" */
	    if (isdigit(*cmd))
		i = '+';		/* "number" is same as "+number" */
	    else
		i = *cmd++;
	    if (!isdigit(*cmd))		/* '+' is '+1', but '+0' is not '+1' */
		n = 1;
	    else
		n = getdigits(&cmd);
	    if (i == '-')
		lnum -= n;
	    else
		lnum += n;
	}
    } while (*cmd == '/' || *cmd == '?');

error:
    *ptr = cmd;
    return lnum;
}

/*
 * If 'autowrite' option set, try to write the file.
 *
 * return FAIL for failure, OK otherwise
 */
    int
autowrite(buf, forceit)
    BUF	    *buf;
    int	    forceit;
{
    if (!p_aw || (!forceit && buf->b_p_ro) || buf->b_ffname == NULL)
	return FAIL;
    return buf_write_all(buf);
}

/*
 * flush all buffers, except the ones that are readonly
 */
    void
autowrite_all()
{
    BUF	    *buf;

    if (!p_aw)
	return;
    for (buf = firstbuf; buf; buf = buf->b_next)
	if (buf_changed(buf) && !buf->b_p_ro)
	{
	    (void)buf_write_all(buf);
#ifdef AUTOCMD
	    /* an autocommand may have deleted the buffer */
	    if (!buf_valid(buf))
		buf = firstbuf;
#endif
	}
}

    static int
check_readonly(forceit)
    int	    forceit;
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
check_changed(buf, checkaw, mult_win, forceit)
    BUF	    *buf;
    int	    checkaw;	    /* do autowrite if buffer was changed */
    int	    mult_win;	    /* check also when several windows for the buffer */
    int	    forceit;
{
    if (    !forceit &&
	    buf_changed(buf) && (mult_win || buf->b_nwindows <= 1) &&
	    (!checkaw || autowrite(buf, forceit) == FAIL))
    {
	emsg(e_nowrtmsg);
	return TRUE;
    }
    return FALSE;
}

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    int
can_abandon(buf, forceit)
    BUF	    *buf;
    int	    forceit;
{
    return (	   p_hid
		|| !buf_changed(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 */
    int
check_changed_any()
{
    BUF	    *buf;
    int	    save;

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
    {
	if (buf_changed(buf))
	{
	    exiting = FALSE;
	    /* There must be a wait_return for this message, do_buffer()
	     * may cause a redraw.  But wait_return() is a no-op when vgetc()
	     * is busy (Quit used from window menu), then make sure we don't
	     * cause a scroll up. */
	    if (vgetc_busy)
	    {
		msg_row = cmdline_row;
		msg_col = 0;
		msg_didout = FALSE;
	    }
	    if (EMSG2("No write since last change for buffer \"%s\"",
		    buf->b_fname == NULL ? (char_u *)"No File" :
		    buf->b_fname))
	    {
		save = no_wait_return;
		no_wait_return = FALSE;
		wait_return(FALSE);
		no_wait_return = save;
	    }
	    (void)do_buffer(DOBUF_GOTO, DOBUF_FIRST, FORWARD, buf->b_fnum, 0);
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    int
check_fname()
{
    if (curbuf->b_ffname == NULL)
    {
	emsg(e_noname);
	return FAIL;
    }
    return OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    static int
buf_write_all(buf)
    BUF	    *buf;
{
    int	    retval;
#ifdef AUTOCMD
    BUF	    *old_curbuf = curbuf;
#endif

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
					 (linenr_t)1, buf->b_ml.ml_line_count,
						  FALSE, FALSE, TRUE, FALSE));
#ifdef AUTOCMD
    if (curbuf != old_curbuf)
	MSG("Warning: Entered other buffer unexpectedly (check autocommands)");
#endif
    return retval;
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

    if (*arg == '+')	    /* +[command] */
    {
	++arg;
	if (vim_isspace(*arg))
	    command = (char_u *)"$";
	else
	{
	    command = arg;
	    arg = skip_cmd_arg(command);
	    if (*arg)
		*arg++ = NUL;	/* terminate command with NUL */
	}

	arg = skipwhite(arg);	/* skip over spaces */
	*argp = arg;
    }
    return command;
}

/*
 * Find end of "+command" argument.  Skip over "\ " and "\\".
 */
    static char_u *
skip_cmd_arg(p)
    char_u *p;
{
    while (*p && !vim_isspace(*p))
    {
	if (*p == '\\' && p[1] != NUL)
	    ++p;
	++p;
    }
    return p;
}

/*
 * Return TRUE if "str" starts with a backslash that should be removed.
 * For MS-DOS, WIN32 and OS/2 this is only done when the character after the
 * backslash is not a normal file name character.
 * Although '$' is a valid file name character, we remove the backslash before
 * it, to be able to disginguish between a file name that starts with '$' and
 * the name of an environment variable.
 * Make sure that "\\mch\file" isn't translated into "\mch\file".
 */
    static int
rem_backslash(str)
    char_u  *str;
{
#ifdef BACKSLASH_IN_FILENAME
    return (str[0] == '\\'
	    && (str[1] == '$'
		|| (str[1] != NUL
		    && str[1] != '*'
		    && str[1] != '?'
		    && !vim_isfilec(str[1]))));
#else
    return (str[0] == '\\' && str[1] != NUL);
#endif
}

/*
 * Halve the number of backslashes in a file name argument.
 * For MS-DOS we only do this if the character after the backslash
 * is not a normal file character.
 * For Unix, when wildcards are going to be expanded, don't remove
 * backslashes before special characters.
 */
    void
backslash_halve(p, has_wildcards)
    char_u  *p;
    int	    has_wildcards;	/* going to expand wildcards later */
{
    for ( ; *p; ++p)
	if (rem_backslash(p)
#if defined(UNIX) || defined(OS2)
		&& !(has_wildcards &&
			vim_strchr((char_u *)" *?[{`$\\", p[1]))
#endif
					       )
	    STRCPY(p, p + 1);
}

/*
 * write current buffer to file 'eap->arg'
 * if 'eap->append' is TRUE, append to the file
 *
 * if *eap->arg == NUL write to current file
 * if b_notedited is TRUE, check for overwriting current file
 *
 * return FAIL for failure, OK otherwise
 */
    static int
do_write(eap)
    EXARG	*eap;
{
    int	    other;
    char_u  *fname = NULL;		/* init to shut up gcc */
    char_u  *ffname;
    int	    retval = FAIL;
    char_u  *free_fname = NULL;

    ffname = eap->arg;
    if (*ffname == NUL)
	other = FALSE;
    else
    {
	fname = ffname;
	free_fname = fix_fname(ffname);
	/*
	 * When out-of-memory, keep unexpanded file name, because we MUST be
	 * able to write the file in this situation.
	 */
	if (free_fname != NULL)
	    ffname = free_fname;
	other = otherfile(ffname);
    }

    /*
     * If we have a new file, name put it in the list of alternate file names.
     */
    if (other && vim_strchr(p_cpo, CPO_ALTWRITE) != NULL)
	setaltfname(ffname, fname, (linenr_t)1);

    /*
     * writing to the current file is not allowed in readonly mode
     * and need a file name
     */
    if (!other && (check_readonly(eap->forceit) || check_fname() == FAIL))
	goto theend;

    if (!other)
    {
	ffname = curbuf->b_ffname;
	fname = curbuf->b_fname;
	/*
	 * Not writing the whole file is only allowed with '!'.
	 */
	if (	   (eap->line1 != 1
		    || eap->line2 != curbuf->b_ml.ml_line_count)
		&& !eap->forceit
		&& !eap->append
		&& !p_wa)
	{
	    EMSG("Use ! to write partial buffer");
	    goto theend;
	}
    }

    /*
     * write to other file or b_notedited set or not writing the whole file:
     * overwriting only allowed with '!'
     */
    if (       (other
		|| curbuf->b_notedited)
	    && !eap->forceit
	    && !eap->append
	    && !p_wa
	    && vim_fexists(ffname))
    {
#ifdef UNIX
	    /* with UNIX it is possible to open a directory */
	if (mch_isdir(ffname))
	    EMSG2("\"%s\" is a directory", ffname);
	else
#endif
	    emsg(e_exists);
	goto theend;
    }
    retval = (buf_write(curbuf, ffname, fname, eap->line1, eap->line2,
				     eap->append, eap->forceit, TRUE, FALSE));
theend:
    vim_free(free_fname);
    return retval;
}

/*
 * try to abandon current file and edit a new or existing file
 * 'fnum' is the number of the file, if zero use ffname/sfname
 *
 * return 1 for "normal" error, 2 for "not written" error, 0 for success
 * -1 for succesfully opening another file
 * 'lnum' is the line number for the cursor in the new file (if non-zero).
 */
    int
getfile(fnum, ffname, sfname, setpm, lnum, forceit)
    int		fnum;
    char_u	*ffname;
    char_u	*sfname;
    int		setpm;
    linenr_t	lnum;
    int		forceit;
{
    int		other;
    int		retval;
    char_u	*free_me = NULL;

    if (fnum == 0)
    {
	fname_expand(&ffname, &sfname);	/* make ffname full path, set sfname */
	other = otherfile(ffname);
	free_me = ffname;		/* has been allocated, free() later */
    }
    else
	other = (fnum != curbuf->b_fnum);

    if (other)
	++no_wait_return;	    /* don't wait for autowrite message */
    if (other && !forceit && curbuf->b_nwindows == 1 &&
	    !p_hid && curbuf_changed() && autowrite(curbuf, forceit) == FAIL)
    {
	if (other)
	    --no_wait_return;
	emsg(e_nowrtmsg);
	retval = 2;	/* file has been changed */
	goto theend;
    }
    if (other)
	--no_wait_return;
    if (setpm)
	setpcmark();
    if (!other)
    {
	if (lnum != 0)
	    curwin->w_cursor.lnum = lnum;
	check_cursor_lnum();
	beginline(BL_SOL | BL_FIX);
	retval = 0;	/* it's in the same file */
    }
    else if (do_ecmd(fnum, ffname, sfname, NULL, lnum,
		(p_hid ? ECMD_HIDE : 0) + (forceit ? ECMD_FORCEIT : 0)) == OK)
	retval = -1;	/* opened another file */
    else
	retval = 1;	/* error encountered */

theend:
    vim_free(free_me);
    return retval;
}

/*
 * start editing a new file
 *
 *     fnum: file number; if zero use ffname/sfname
 *   ffname: the file name
 *		- full path if sfname used,
 *		- any file name if sfname is NULL
 *		- empty string to re-edit with the same file name (but may be
 *		    in a different directory)
 *		- NULL to start an empty buffer
 *   sfname: the short file name (or NULL)
 *  command: the command to be executed after loading the file
 *  newlnum: put cursor on this line number (if possible)
 *    flags:
 *	   ECMD_HIDE: if TRUE don't free the current buffer
 *     ECMD_SET_HELP: set b_help flag of (new) buffer before opening file
 *	 ECMD_OLDBUF: use existing buffer if it exists
 *	ECMD_FORCEIT: ! used for Ex command
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_ecmd(fnum, ffname, sfname, command, newlnum, flags)
    int		fnum;
    char_u	*ffname;
    char_u	*sfname;
    char_u	*command;
    linenr_t	newlnum;
    int		flags;
{
    int		other_file;		/* TRUE if editing another file */
    int		oldbuf;			/* TRUE if using existing buffer */
#ifdef AUTOCMD
    int		auto_buf = FALSE;	/* TRUE if autocommands brought us
					   into the buffer unexpectedly */
#endif
    BUF		*buf;
    char_u	*free_fname = NULL;
    int		retval = FAIL;
    long	n;

    if (fnum != 0)
    {
	if (fnum == curbuf->b_fnum)	/* file is already being edited */
	    return OK;			/* nothing to do */
	other_file = TRUE;
    }
    else
    {
	    /* if no short name given, use ffname for short name */
	if (sfname == NULL)
	    sfname = ffname;
#ifdef USE_FNAME_CASE
# ifdef USE_LONG_FNAME
	if (USE_LONG_FNAME)
# endif
	    fname_case(sfname);	    /* set correct case for short file name */
#endif

	if (ffname == NULL)
	    other_file = TRUE;
					    /* there is no file name */
	else if (*ffname == NUL && curbuf->b_ffname == NULL)
	    other_file = FALSE;
	else
	{
	    if (*ffname == NUL)		    /* re-edit with same file name */
	    {
		ffname = curbuf->b_ffname;
		sfname = curbuf->b_fname;
	    }
	    free_fname = fix_fname(ffname); /* may expand to full path name */
	    if (free_fname != NULL)
		ffname = free_fname;
	    other_file = otherfile(ffname);
	}
    }
/*
 * if the file was changed we may not be allowed to abandon it
 * - if we are going to re-edit the same file
 * - or if we are the only window on this file and if ECMD_HIDE is FALSE
 */
    if (((!other_file && !(flags & ECMD_OLDBUF)) ||
	    (curbuf->b_nwindows == 1 && !(flags & ECMD_HIDE))) &&
	    check_changed(curbuf, FALSE, !other_file, (flags & ECMD_FORCEIT)))
    {
	if (fnum == 0 && other_file && ffname != NULL)
	    setaltfname(ffname, sfname, newlnum);
	goto theend;
    }

/*
 * End Visual mode before switching to another buffer, so the text can be
 * copied into the GUI selection buffer.
 */
    if (VIsual_active)
	end_visual_mode();

/*
 * If we are starting to edit another file, open a (new) buffer.
 * Otherwise we re-use the current buffer.
 */
    if (other_file)
    {
	curwin->w_alt_fnum = curbuf->b_fnum;
	buflist_altlnum();

	if (fnum)
	    buf = buflist_findnr(fnum);
	else
	    buf = buflist_new(ffname, sfname, 1L, TRUE);
	if (buf == NULL)
	    goto theend;
	if (buf->b_ml.ml_mfp == NULL)	    /* no memfile yet */
	{
	    oldbuf = FALSE;
	    buf->b_nwindows = 0;
	}
	else				    /* existing memfile */
	{
	    oldbuf = TRUE;
	    buf_check_timestamp(buf);
	}

	/*
	 * Make the (new) buffer the one used by the current window.
	 * If the old buffer becomes unused, free it if ECMD_HIDE is FALSE.
	 * If the current buffer was empty and has no file name, curbuf
	 * is returned by buflist_new().
	 */
	if (buf != curbuf)
	{
#ifdef AUTOCMD
	    BUF	    *old_curbuf;
	    char_u  *new_name = NULL;

	    /*
	     * Be careful: The autocommands may delete any buffer and change
	     * the current buffer.
	     * - If the buffer we are going to edit is deleted, give up.
	     * - If we ended up in the new buffer already, need to skip a few
	     *	 things, set auto_buf.
	     */
	    old_curbuf = curbuf;
	    if (buf->b_fname != NULL)
		new_name = vim_strsave(buf->b_fname);
	    apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE);
	    if (!buf_valid(buf))	/* new buffer has been deleted */
	    {
		EMSG2("Autocommands unexpectedly deleted new buffer %s",
			new_name == NULL ? (char_u *)"" : new_name);
		vim_free(new_name);
		goto theend;
	    }
	    vim_free(new_name);
	    if (buf == curbuf)		/* already in new buffer */
		auto_buf = TRUE;
	    else
	    {
		if (curbuf == old_curbuf)
#endif
		    buf_copy_options(curbuf, buf, BCO_ENTER);
		close_buffer(curwin, curbuf, !(flags & ECMD_HIDE), FALSE);
		curwin->w_buffer = buf;
		curbuf = buf;
		++curbuf->b_nwindows;
		/* set 'fileformat' */
		if (*p_ffs && !oldbuf)
		    set_fileformat(default_fileformat());
#ifdef AUTOCMD
	    }
#endif
	}
	else
	    ++curbuf->b_nwindows;

	curwin->w_pcmark.lnum = 1;
	curwin->w_pcmark.col = 0;
    }
    else
    {
	if (check_fname() == FAIL)
	    goto theend;
	oldbuf = (flags & ECMD_OLDBUF);
    }

/*
 * If we get here we are sure to start editing
 */
    /* don't redraw until the cursor is in the right line */
    ++RedrawingDisabled;
    if (flags & ECMD_SET_HELP)
	curbuf->b_help = TRUE;

/*
 * other_file	oldbuf
 *  FALSE	FALSE	    re-edit same file, buffer is re-used
 *  FALSE	TRUE	    re-edit same file, nothing changes
 *  TRUE	FALSE	    start editing new file, new buffer
 *  TRUE	TRUE	    start editing in existing buffer (nothing to do)
 */
    if (!other_file && !oldbuf)		/* re-use the buffer */
    {
	if (newlnum == 0)
	    newlnum = curwin->w_cursor.lnum;
	buf_freeall(curbuf);		/* free all things for buffer */
	buf_clear(curbuf);
	curbuf->b_op_start.lnum = 0;	/* clear '[ and '] marks */
	curbuf->b_op_end.lnum = 0;
    }

    /*
     * Reset cursor position, could be used by autocommands.
     */
    adjust_cursor();

    /*
     * Check if we are editing the w_arg_idx file in the argument list.
     */
    check_arg_idx();

#ifdef AUTOCMD
    if (!auto_buf)
#endif
    {
	/*
	 * Set cursor and init window before reading the file and executing
	 * autocommands.  This allows for the autocommands to position the
	 * cursor.
	 */
	win_init(curwin);

	/*
	 * Careful: open_buffer() and apply_autocmds() may change the current
	 * buffer and window.
	 */
	if (!oldbuf)			    /* need to read the file */
	{
	    curbuf->b_flags |= BF_CHECK_RO; /* set/reset 'ro' flag */
	    (void)open_buffer(FALSE);
	}
#ifdef AUTOCMD
	else
	    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE);
	check_arg_idx();
#endif
	maketitle();
    }

    if (command == NULL)
    {
	if (newlnum)
	{
	    curwin->w_cursor.lnum = newlnum;
	    check_cursor_lnum();
	    beginline(BL_SOL | BL_FIX);
	}
	else
	{
	    if (exmode_active)
	    {
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
		check_cursor_lnum();
	    }
	    beginline(BL_WHITE | BL_FIX);
	}
    }

    /* Check if cursors in other windows on the same buffer are still valid */
    check_lnums(FALSE);

    /*
     * Did not read the file, need to show some info about the file.
     * Do this after setting the cursor.
     */
    if (oldbuf
#ifdef AUTOCMD
		&& !auto_buf
#endif
			    )
	fileinfo(FALSE, TRUE, FALSE);

    if (command != NULL)
	do_cmdline(command, NULL, NULL, DOCMD_VERBOSE);
    --RedrawingDisabled;
    if (!skip_redraw)
    {
	n = p_so;
	if (command == NULL)
	    p_so = 999;			/* force cursor halfway the window */
	update_topline();
	p_so = n;
	update_curbuf(NOT_VALID);	/* redraw now */
    }

    if (p_im)
	need_start_insertmode = TRUE;
    retval = OK;

theend:
    vim_free(free_fname);
    return retval;
}

#ifdef QUICKFIX
    static void
do_make(arg)
    char_u *arg;
{
    char_u	*name;

    autowrite_all();
    name = get_mef_name(TRUE);
    if (name == NULL)
	return;
    vim_remove(name);	    /* in case it's not unique */

    /*
     * If 'shellpipe' empty: don't redirect to 'errorfile'.
     */
    if (*p_sp == NUL)
	sprintf((char *)IObuff, "%s%s%s", p_shq, arg, p_shq);
    else
	sprintf((char *)IObuff, "%s%s%s %s %s", p_shq, arg, p_shq, p_sp, name);
    /*
     * Output a newline if there's something else than the :make command that
     * was typed (in which case the cursor is in column 0).
     */
    if (msg_col != 0)
	msg_putchar('\n');
    MSG_PUTS(":!");
    msg_outtrans(IObuff);		/* show what we are doing */

    /* let the shell know if we are redirecting output or not */
    do_shell(IObuff, *p_sp ? SHELL_DOOUT : 0);

#ifdef AMIGA
    out_flush();
		/* read window status report and redraw before message */
    (void)char_avail();
#endif

    if (qf_init(name) > 0)
	qf_jump(0, 0, FALSE);		/* display first error */

    vim_remove(name);
    vim_free(name);
}

/*
 * Return the name for the errorfile, in allocated memory.
 * When "newname" is TRUE, find a new unique name when 'makeef' contains
 * "##".  Returns NULL for error.
 */
    static char_u *
get_mef_name(newname)
    int		newname;
{
    char_u	*p;
    char_u	*name;
    static int	start = -1;
    static int	off = 0;

    if (*p_mef == NUL)
    {
	EMSG("makeef option not set");
	return NULL;
    }

    for (p = p_mef; *p; ++p)
	if (p[0] == '#' && p[1] == '#')
	    break;

    if (*p == NUL)
	return vim_strsave(p_mef);

    /* When "newname" set: keep trying until the name doesn't exist yet. */
    for (;;)
    {
	if (newname)
	{
	    if (start == -1)
		start = mch_get_pid();
	    ++off;
	}

	name = alloc((unsigned)STRLEN(p_mef) + 30);
	if (name == NULL)
	    break;
	STRCPY(name, p_mef);
	sprintf((char *)name + (p - p_mef), "%d%d", start, off);
	STRCAT(name, p + 2);
	if (!newname || mch_getperm(name) < 0)
	    break;
	vim_free(name);
    }
    return name;
}

/*
 * ":cfile" command.
 */
    static void
do_cfile(eap)
    EXARG	*eap;
{
    if (*eap->arg != NUL)
	set_string_option_direct((char_u *)"ef", -1, eap->arg, TRUE);
    if (qf_init(p_ef) > 0)
	qf_jump(0, 0, eap->forceit);		/* display first error */
}
#endif /* QUICKFIX */

/*
 * Redefine the argument list to 'str'.
 *
 * Return FAIL for failure, OK otherwise.
 */
    static int
do_arglist(str)
    char_u *str;
{
    int	    new_count = 0;
    char_u  **new_files = NULL;
    int	    exp_count;
    char_u  **exp_files;
    char_u  **t;
    char_u  *p;
    int	    inquote;
    int	    i;

    while (*str)
    {
	/*
	 * create a new entry in new_files[]
	 */
	t = (char_u **)lalloc((long_u)(sizeof(char_u *) * (new_count + 1)),
									TRUE);
	if (t != NULL)
	    for (i = new_count; --i >= 0; )
		t[i] = new_files[i];
	vim_free(new_files);
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
	     * for MSDOS et.al. a backslash is part of a file name.
	     * Only skip ", space and tab.
	     */
	    if (rem_backslash(str))
		*p++ = *++str;
	    else
	    {
		if (!inquote && vim_isspace(*str))
		    break;
		if (*str == '"')
		    inquote ^= TRUE;
		else
		    *p++ = *str;
	    }
	}
	str = skipwhite(str);
	*p = NUL;
    }

    i = expand_wildcards(new_count, new_files, &exp_count, &exp_files,
						  EW_DIR|EW_FILE|EW_NOTFOUND);
    vim_free(new_files);
    if (i == FAIL)
	return FAIL;
    if (exp_count == 0)
    {
	emsg(e_nomatch);
	return FAIL;
    }
    FreeWild(arg_file_count, arg_files);
    arg_files = exp_files;
    arg_file_count = exp_count;
    arg_had_last = FALSE;

    /*
     * put all file names in the buffer list
     */
    for (i = 0; i < arg_file_count; ++i)
	(void)buflist_add(arg_files[i]);

    return OK;
}

/*
 * Check if we are editing the w_arg_idx file in the argument list.
 */
    void
check_arg_idx()
{
    if (arg_file_count > 1
	    && (curbuf->b_ffname == NULL
		|| curwin->w_arg_idx >= arg_file_count
		|| !(fullpathcmp(arg_files[curwin->w_arg_idx],
					 curbuf->b_ffname, TRUE) & FPC_SAME)))
	curwin->w_arg_idx_invalid = TRUE;
    else
	curwin->w_arg_idx_invalid = FALSE;
}

    int
ends_excmd(c)
    int	    c;
{
    return (c == NUL || c == '|' || c == '\"' || c == '\n');
}

/*
 * Return the next command, after the first '|' or '\n'.
 * Return NULL if not found.
 */
    char_u *
find_nextcmd(p)
    char_u	*p;
{
    while (*p != '|' && *p != '\n')
    {
	if (*p == NUL)
	    return NULL;
	++p;
    }
    return (p + 1);
}

/*
 * Check if *p is a separator between Ex commands.
 * Return NULL if it isn't, (p + 1) if it is.
 */
    char_u *
check_nextcmd(p)
    char_u	*p;
{
    p = skipwhite(p);
    if (*p == '|' || *p == '\n')
	return (p + 1);
    else
	return NULL;
}

/*
 * - if there are more files to edit
 * - and this is the last window
 * - and forceit not used
 * - and not repeated twice on a row
 *    return FAIL and give error message if 'message' TRUE
 * return OK otherwise
 */
    static int
check_more(message, forceit)
    int message;	    /* when FALSE check only, no messages */
    int forceit;
{
    if (!forceit && only_one_window() && arg_file_count > 1 && !arg_had_last &&
				    quitmore == 0)
    {
	if (message)
	{
	    EMSGN("%ld more files to edit",
				      arg_file_count - curwin->w_arg_idx - 1);
	    quitmore = 2;	    /* next try to quit is allowed */
	}
	return FAIL;
    }
    return OK;
}

/*
 * Structure used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is required, because it needs to be handed to do_cmdline() and
 * sourcing can be done recursively.
 */
struct source_cookie
{
    FILE	*fp;		/* opened file for sourcing */
#ifdef USE_CRNL
    int		fileformat;	/* EOL_UNKNOWN, EOL_UNIX or EOL_DOS */
    int		error;		/* TRUE if LF found after CR-LF */
#endif
};

/*
 * do_source: Read the file "fname" and execute its lines as EX commands.
 *
 * This function may be called recursively!
 *
 * return FAIL if file could not be opened, OK otherwise
 */
    int
do_source(fname, check_other, is_vimrc)
    char_u	*fname;
    int		check_other;	    /* check for .vimrc and _vimrc */
    int		is_vimrc;	    /* call vimrc_found() when file exists */
{
    struct source_cookie    cookie;
    char_u		    *save_sourcing_name;
    linenr_t		    save_sourcing_lnum;
    char_u		    *p;
    char_u		    *fname_exp;
    int			    retval = FAIL;

    fname_exp = expand_env_save(fname);
    if (fname_exp == NULL)
	goto theend;
#ifdef macintosh
    slash_n_colon_adjust(fname_exp);
#endif
    cookie.fp = fopen((char *)fname_exp, READBIN);
    if (cookie.fp == NULL && check_other)
    {
	/*
	 * Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	 * and ".exrc" by "_exrc" or vice versa.
	 */
	p = gettail(fname_exp);
	if ((*p == '.' || *p == '_') &&
		(STRICMP(p + 1, "vimrc") == 0 ||
		 STRICMP(p + 1, "gvimrc") == 0 ||
		 STRICMP(p + 1, "exrc") == 0))
	{
	    if (*p == '_')
		*p = '.';
	    else
		*p = '_';
	    cookie.fp = fopen((char *)fname_exp, READBIN);
	}
    }

    if (cookie.fp == NULL)
    {
	if (p_verbose > 0)
	    smsg((char_u *)"could not source \"%s\"", fname);
	goto theend;
    }

    /*
     * The file exists.
     * - In verbose mode, give a message.
     * - For a vimrc file, may want to set 'compatible', call vimrc_found().
     */
    if (p_verbose > 0)
	smsg((char_u *)"sourcing \"%s\"", fname);
    if (is_vimrc)
	vimrc_found();


#ifdef USE_CRNL
    /* If no automatic file format: Set default to CR-NL. */
    if (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    else
	cookie.fileformat = EOL_UNKNOWN;
    cookie.error = FALSE;
#endif

    /*
     * Keep the sourcing name, for recursive calls.
     */
    save_sourcing_name = sourcing_name;
    save_sourcing_lnum = sourcing_lnum;
    sourcing_name = fname_exp;
    sourcing_lnum = 0;

    /*
     * Call do_cmdline, which will call getsourceline() to get the lines.
     */
    do_cmdline(NULL, getsourceline, (void *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);

    fclose(cookie.fp);
    if (got_int)
	emsg(e_interr);
    sourcing_name = save_sourcing_name;
    sourcing_lnum = save_sourcing_lnum;
    retval = OK;

theend:
    vim_free(fname_exp);
    return retval;
}

/*
 * Get one full line from a sourced file.
 * Called by do_source() and do_cmdline().
 *
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
/* ARGSUSED */
    char_u *
getsourceline(c, cookie, indent)
    int	    c;		    /* not used */
    void    *cookie;
    int	    indent;	    /* not used */
{
    struct source_cookie    *sp = (struct source_cookie *)cookie;
    struct growarray	    ga;
    int			    len;
    char_u		    *buf;
#ifdef USE_CRNL
    int			    has_cr;	    /* CR-LF found */
#endif
    int			    have_read = FALSE;

    /* use a growarray to store the sourced line */
    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 200;

    /*
     * Loop until there is a finished line (or end-of-file).
     */
    sourcing_lnum++;
    for (;;)
    {
	/* make room to read at least 80 (more) characters */
	if (ga_grow(&ga, 80) == FAIL)
	    break;
	buf = (char_u *)ga.ga_data;

	if (fgets((char *)buf + ga.ga_len, ga.ga_room, sp->fp) == NULL
								   || got_int)
	    break;

	len = STRLEN(buf);
#ifdef USE_CRNL
	/* Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	 * CTRL-Z by its own, or after a NL. */
	if (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl('Z'))
	{
	    buf[len - 1] = NUL;
	    break;
	}
#endif
	have_read = TRUE;
	ga.ga_room -= len - ga.ga_len;
	ga.ga_len = len;

	/* If the line was longer than the buffer, read more. */
	if (ga.ga_room == 1 && buf[len - 1] != '\n')
	    continue;

	if (len >= 1 && buf[len - 1] == '\n')	/* remove trailing NL */
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    if (sp->fileformat == EOL_UNKNOWN)
	    {
		if (has_cr)
		    sp->fileformat = EOL_DOS;
		else
		    sp->fileformat = EOL_UNIX;
	    }

	    if (sp->fileformat == EOL_DOS)
	    {
		if (has_cr)	    /* replace trailing CR */
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		    ++ga.ga_room;
		}
		else	    /* lines like ":map xx yy^M" will have failed */
		{
		    if (!sp->error)
			EMSG("Warning: Wrong line separator, ^M may be missing");
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#endif
	    /* The '\n' is escaped if there is an odd number of ^V's just
	     * before it, first set "c" just before the 'V's and then check
	     * len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo */
	    for (c = len - 2; c >= 0 && buf[c] == Ctrl('V'); c--)
		;
	    if ((len & 1) != (c & 1))	/* escaped NL, read more */
	    {
		sourcing_lnum++;
		continue;
	    }

	    buf[len - 1] = NUL;		/* remove the NL */
	}

	/*
	 * Check for ^C here now and then, so recursive :so can be broken.
	 */
	line_breakcheck();
	break;
    }

    if (have_read)
	return (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    return NULL;
}

/*
 * Function given to ExpandGeneric() to obtain the list of command names.
 */
    char_u *
get_command_name(idx)
    int	    idx;
{
    if (idx >= (int)CMD_SIZE)
	return NULL;
    return cmdnames[idx].cmd_name;
}

/*
 * Call this function if we thought we were going to exit, but we won't
 * (because of an error).  May need to restore the terminal mode.
 */
    void
not_exiting()
{
    exiting = FALSE;
    if (!exmode_active)
	settmode(TMODE_RAW);
}

/*
 * ":quit": quit current window, quit Vim if closed the last window.
 */
    static void
do_quit(eap)
    EXARG	*eap;
{
    /*
     * If there are more files or windows we won't exit.
     */
    if (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    if ((!p_hid && check_changed(curbuf, FALSE, FALSE, eap->forceit))
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window()
		&& !eap->forceit && check_changed_any()))
    {
	not_exiting();
    }
    else
    {
	if (only_one_window())	    /* quit last window */
	    getout(0);
	close_window(curwin, !p_hid || eap->forceit); /* may free buffer */
    }
}

/*
 * ":qall": try to quit all windows
 */
    static void
do_quit_all(forceit)
    int		forceit;
{
    exiting = TRUE;
    if (forceit || !check_changed_any())
	getout(0);
    not_exiting();
}

/*
 * ":close": close current window, unless it is the last one
 */
    static void
do_close(eap)
    EXARG	*eap;
{
    int	    need_hide;

    need_hide = (buf_changed(curbuf) && curbuf->b_nwindows <= 1);
    if (need_hide && !p_hid && !eap->forceit)
	emsg(e_nowrtmsg);
    else
	close_window(curwin, !need_hide);	    /* may free buffer */
}

/*
 * ":stop" and ":suspend": Suspend Vim.
 */
    static void
do_suspend(forceit)
    int		forceit;
{
    /*
     * Disallow suspending for "rvim".
     */
    if (!check_restricted()
#ifdef WIN32
	/*
	 * Check if external commands are allowed now.
	 */
	&& can_end_termcap_mode(TRUE)
#endif
					)
    {
	if (!forceit)
	    autowrite_all();
	windgoto((int)Rows - 1, 0);
	out_char('\n');
	out_flush();
	stoptermcap();
	mch_restore_title(3);	/* restore window titles */
	ui_suspend();		/* call machine specific function */
	maketitle();
	starttermcap();
	scroll_start();		/* scroll screen before redrawing */
	must_redraw = CLEAR;
	set_winsize(0, 0, FALSE); /* May have resized window */
    }
}

/*
 * ":exit", ":xit" and ":wq": Write file and exit Vim.
 */
    static void
do_exit(eap)
    EXARG	*eap;
{
    /*
     * if more files or windows we won't exit
     */
    if (check_more(FALSE, eap->forceit) == OK && only_one_window())
	exiting = TRUE;
    if (       ((eap->cmdidx == CMD_wq
		    || curbuf_changed())
		&& do_write(eap) == FAIL)
	    || check_more(TRUE, eap->forceit) == FAIL
	    || (only_one_window()
		&& !eap->forceit && check_changed_any()))
    {
	not_exiting();
    }
    else
    {
	if (only_one_window())	    /* quit last window, exit Vim */
	    getout(0);
	close_window(curwin, !p_hid); /* quit current window, may free buffer */
    }
}

/*
 * ":wall", ":wqall" and ":xall": Write all changed files (and exit).
 */
    static void
do_wqall(eap)
    EXARG	*eap;
{
    BUF	    *buf;
    int	    error = 0;

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
    {
	if (buf_changed(buf))
	{
	    if (buf->b_ffname == NULL)
	    {
		emsg(e_noname);
		++error;
	    }
	    else if (!eap->forceit && buf->b_p_ro)
	    {
		EMSG2("\"%s\" is readonly, use ! to write anyway",
								buf->b_fname);
		++error;
	    }
	    else
	    {
		if (buf_write_all(buf) == FAIL)
		    ++error;
#ifdef AUTOCMD
		/* an autocommand may have deleted the buffer */
		if (!buf_valid(buf))
		    buf = firstbuf;
#endif
	    }
	}
    }
    if (exiting)
    {
	if (!error)
	    getout(0);		/* exit Vim */
	not_exiting();
    }
}

    static void
do_print(eap)
    EXARG	*eap;
{
    for ( ;!got_int; ui_breakcheck())
    {
	print_line(eap->line1,
		   (eap->cmdidx == CMD_number || eap->cmdidx == CMD_pound));
	if (++eap->line1 > eap->line2)
	    break;
	out_flush();	    /* show one line at a time */
    }
    setpcmark();
    /* put cursor at last line */
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_SOL | BL_FIX);

    ex_no_reprint = TRUE;
}

/*
 * Edit file "argn" from the arguments.
 */
    static void
do_argfile(eap, argn)
    EXARG   *eap;
    int	    argn;
{
    int		other;
    char_u	*p;
    char_u	*ffname;
    BUF		*buf;

    if (argn < 0 || argn >= arg_file_count)
    {
	if (arg_file_count <= 1)
	    EMSG("There is only one file to edit");
	else if (argn < 0)
	    EMSG("Cannot go before first file");
	else
	    EMSG("Cannot go beyond last file");
    }
    else
    {
	setpcmark();
	if (*eap->cmd == 's')	    /* split window first */
	{
	    if (win_split(0, FALSE, FALSE) == FAIL)
		return;
	}
	else
	{
	    /*
	     * if 'hidden' set, only check for changed file when re-editing
	     * the same buffer
	     */
	    other = TRUE;
	    if (p_hid)
	    {
		p = fix_fname(arg_files[argn]);
		other = otherfile(p);
		vim_free(p);
	    }
	    if ((!p_hid || !other)
			 && check_changed(curbuf, TRUE, !other, eap->forceit))
		return;
	}

	curwin->w_arg_idx = argn;
	if (argn == arg_file_count - 1)
	    arg_had_last = TRUE;

	/*
	 * If no line number given, use the last known line number.
	 */
	if (eap->do_ecmd_lnum == 0)
	{
	    ffname = fix_fname(arg_files[curwin->w_arg_idx]);
	    if (ffname != NULL)
	    {
		buf = buflist_findname(ffname);
		if (buf != NULL)
		    eap->do_ecmd_lnum = buflist_findlnum(buf);
		vim_free(ffname);
	    }
	}

	(void)do_ecmd(0, arg_files[curwin->w_arg_idx],
		      NULL, eap->do_ecmd_cmd, eap->do_ecmd_lnum,
		      (p_hid ? ECMD_HIDE : 0) +
					   (eap->forceit ? ECMD_FORCEIT : 0));
    }
}

/*
 * Do ":next" command, and commands that behave like it.
 */
    static void
do_next(eap)
    EXARG	*eap;
{
    int	    i;

    /*
     * check for changed buffer now, if this fails the argument list is not
     * redefined.
     */
    if (       p_hid
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, TRUE, FALSE, eap->forceit))
    {
	if (*eap->arg != NUL)		    /* redefine file list */
	{
	    if (do_arglist(eap->arg) == FAIL)
		return;
	    i = 0;
	}
	else
	    i = curwin->w_arg_idx + (int)eap->line2;
	do_argfile(eap, i);
    }
}

#if defined(USE_GUI_WIN32) || defined(USE_GUI_BEOS) || defined(PROTO)
/*
 * Handle a file drop. The code is here because a drop is *nearly* like an
 * :args command, but not quite (we have a list of exact filenames, so we
 * don't want to (a) parse a command line, or (b) expand wildcards. So the
 * code is very similar to :args and hence needs access to a lot of the static
 * functions in this file.
 *
 * Arguments:
 *	FILEC => the number of files dropped
 *	FILEV => the list of files dropped
 *
 * The list should be allocated using vim_alloc(), as should each item in the
 * list. This function takes over responsibility for freeing the list.
 *
 * XXX The list is made into the arg_files list. This is freed using
 * FreeWild(), which does a series of vim_free() calls, unless the two defines
 * __EMX__ and __ALWAYS_HAS_TRAILING_NUL_POINTER are set. In this case, a
 * routine _fnexplodefree() is used. This may cause problems, but as the drop
 * file functionality is (currently) Win32-specific (where these defines are
 * not set), this is not presently a problem.
 */

void
handle_drop(filec, filev)
    int	     filec;
    char_u **filev;
{
    EXARG ea;
    int i;
    int split = FALSE;

    /* Check whether the current buffer is changed. If so, we will need
     * to split the current window or data could be lost.
     * We don't need to check if the 'hidden' option is set, as in this
     * case the buffer won't be lost.
     */
    if (!p_hid)
    {
	int old_emsg = emsg_off;

	emsg_off = TRUE;
	split = check_changed(curbuf, TRUE, FALSE, FALSE);
	emsg_off = old_emsg;
    }

    /*
     * Set up the new argument list.
     * This code is copied from the tail end of do_arglist()
     */
    FreeWild(arg_file_count, arg_files);
    arg_file_count = filec;
    arg_files = filev;
    arg_had_last = FALSE;

    for (i = 0; i < arg_file_count; ++i)
	(void)buflist_add(arg_files[i]);

    /*
     * Move to the first file.
     */

    /* Fake up a minimal "[s]next" command for do_argfile() */
    ea.cmd = (char_u *)(split ? "snext" : "next");
    ea.forceit = FALSE;
    ea.do_ecmd_cmd = NULL;
    ea.do_ecmd_lnum = 0;

    do_argfile(&ea, 0);
}
#endif

/*
 * Handle ":recover" command.
 */
    static void
do_recover(eap)
    EXARG	*eap;
{
    recoverymode = TRUE;
    if (!check_changed(curbuf, FALSE, TRUE, eap->forceit)
		&& (*eap->arg == NUL || setfname(eap->arg, NULL, TRUE) == OK))
	ml_recover();
    recoverymode = FALSE;
}

/*
 * Handle ":args" command.
 */
    static void
do_args(eap)
    EXARG	*eap;
{
    int	    i;

    /* ":args file": handle like :next */
    if (!ends_excmd(*eap->arg))
	do_next(eap);
    else
    {
	if (arg_file_count == 0)	    /* no file name list */
	{
	    if (check_fname() == OK)	    /* check for no file name */
		smsg((char_u *)"[%s]", curbuf->b_ffname);
	}
	else
	{
	    /*
	     * Overwrite the command, in most cases there is no scrolling
	     * required and no wait_return().
	     */
	    gotocmdline(TRUE);
	    for (i = 0; i < arg_file_count; ++i)
	    {
		if (i == curwin->w_arg_idx)
		    msg_putchar('[');
		msg_outtrans(arg_files[i]);
		if (i == curwin->w_arg_idx)
		    msg_putchar(']');
		msg_putchar(' ');
	    }
	}
    }
}

/*
 * Handle ":wnext", ":wNext" and ":wprevious" commands.
 */
    static void
do_wnext(eap)
    EXARG	*eap;
{
    int		i;

    if (eap->cmd[1] == 'n')
	i = curwin->w_arg_idx + (int)eap->line2;
    else
	i = curwin->w_arg_idx - (int)eap->line2;
    eap->line1 = 1;
    eap->line2 = curbuf->b_ml.ml_line_count;
    if (do_write(eap) != FAIL)
	do_argfile(eap, i);
}

/*
 * :sview [+command] file   split window with new file, read-only
 * :split [[+command] file] split window with current or new file
 * :new [[+command] file]   split window with no or new file
 */
    static void
do_splitview(eap)
    EXARG   *eap;
{
    WIN	    *old_curwin;

    old_curwin = curwin;
    if (win_split(eap->addr_count ? (int)eap->line2 : 0, FALSE, FALSE) != FAIL)
	do_exedit(eap, old_curwin);
}

/*
 * Handle ":resize" command.
 * set, increment or decrement current window height
 */
    static void
do_resize(eap)
    EXARG	*eap;
{
    int		n;

    n = atol((char *)eap->arg);
    if (*eap->arg == '-' || *eap->arg == '+')
	n += curwin->w_height;
    else if (n == 0)	    /* default is very high */
	n = 9999;
    win_setheight((int)n);
}

    static void
do_exedit(eap, old_curwin)
    EXARG	*eap;
    WIN		*old_curwin;
{
    int	    n;

    /*
     * ":vi" command ends Ex mode.
     */
    if (eap->cmdidx == CMD_visual || eap->cmdidx == CMD_view)
    {
	exmode_active = FALSE;
	if (*eap->arg == NUL)
	    return;
    }

    if ((eap->cmdidx == CMD_new) && *eap->arg == NUL)
    {
	setpcmark();
	(void)do_ecmd(0, NULL, NULL, eap->do_ecmd_cmd, (linenr_t)1,
			       ECMD_HIDE + (eap->forceit ? ECMD_FORCEIT : 0));
    }
    else if (eap->cmdidx != CMD_split || *eap->arg != NUL)
    {
	n = readonlymode;
	if (eap->cmdidx == CMD_view || eap->cmdidx == CMD_sview)
	    readonlymode = TRUE;
	setpcmark();
	(void)do_ecmd(0, eap->arg, NULL, eap->do_ecmd_cmd, eap->do_ecmd_lnum,
					 (p_hid ? ECMD_HIDE : 0) +
				    (eap->forceit ? ECMD_FORCEIT : 0));
	readonlymode = n;
    }
    else
    {
	if (eap->do_ecmd_cmd != NULL)
	    do_cmdline(eap->do_ecmd_cmd, NULL, NULL, DOCMD_VERBOSE);
	update_screen(NOT_VALID);
    }

    /*
     * if ":split file" worked, set alternate file name in old window to new
     * file
     */
    if (       (eap->cmdidx == CMD_new
		|| eap->cmdidx == CMD_split)
	    && *eap->arg != NUL
	    && curwin != old_curwin
	    && win_valid(old_curwin)
	    && old_curwin->w_buffer != curbuf)
	old_curwin->w_alt_fnum = curbuf->b_fnum;

    ex_no_reprint = TRUE;
}

#ifdef USE_GUI
/*
 * Handle ":gui" or ":gvim" command.
 */
    static void
do_gui(eap)
    EXARG	*eap;
{
    /*
     * Check for "-f" argument: foreground, don't fork.
     */
    if (eap->arg[0] == '-' && eap->arg[1] == 'f' &&
			     (eap->arg[2] == NUL || vim_iswhite(eap->arg[2])))
    {
	gui.dofork = FALSE;
	eap->arg = skipwhite(eap->arg + 2);
    }
    else
	gui.dofork = TRUE;
    if (!gui.in_use)
    {
	/* Clear the command.  Needed for when forking+exiting, to avoid part
	 * of the argument ending up after the shell prompt. */
	msg_clr_eos();
	gui_start();
    }
    if (!ends_excmd(*eap->arg))
	do_next(eap);
}
#endif

    static void
do_swapname()
{
    if (curbuf->b_ml.ml_mfp == NULL || curbuf->b_ml.ml_mfp->mf_fname == NULL)
	MSG("No swap file");
    else
	msg(curbuf->b_ml.ml_mfp->mf_fname);
}

    static void
do_read(eap)
    EXARG	*eap;
{
    int	    i;

    if (eap->usefilter)			/* :r!cmd */
	do_bang(1, eap->line1, eap->line2, FALSE, eap->arg, FALSE, TRUE);
    else
    {
	if (u_save(eap->line2, (linenr_t)(eap->line2 + 1)) == FAIL)
	    return;

	if (*eap->arg == NUL)
	{
	    if (check_fname() == FAIL)	/* check for no file name */
		return;
	    i = readfile(curbuf->b_ffname, curbuf->b_fname,
			       eap->line2, (linenr_t)0, (linenr_t)MAXLNUM, 0);
	}
	else
	{
	    if (vim_strchr(p_cpo, CPO_ALTREAD) != NULL)
		setaltfname(eap->arg, eap->arg, (linenr_t)1);
	    i = readfile(eap->arg, NULL,
			       eap->line2, (linenr_t)0, (linenr_t)MAXLNUM, 0);

	}
	if (i == FAIL)
	    emsg2(e_notopen, eap->arg);
	else
	    update_screen(NOT_VALID);
    }
}

    static void
do_cd(eap)
    EXARG	*eap;
{
    BUF		*buf;
    char_u	*p;

#ifdef UNIX
    /*
     * for UNIX ":cd" means: go to home directory
     */
    if (*eap->arg == NUL)    /* use NameBuff for home directory name */
    {
	expand_env((char_u *)"$HOME", NameBuff, MAXPATHL);
	eap->arg = NameBuff;
    }
#endif
    if (*eap->arg == NUL)
	do_pwd();
    else
    {
	if (vim_chdir((char *)eap->arg))
	    emsg(e_failed);
	else
	{
	    /*
	     * Use full path from now on for files currently being
	     * edited, both for file name and swap file name.  Try
	     * to shorten the file names a bit if safe to do so.
	     */
	    mch_dirname(IObuff, IOSIZE);
	    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	    {
		if (buf->b_fname != NULL)
		{
		    vim_free(buf->b_sfname);
		    buf->b_sfname = NULL;
		    p = shorten_fname(buf->b_ffname, IObuff);
		    if (p != NULL)
		    {
			buf->b_sfname = vim_strsave(p);
			buf->b_fname = buf->b_sfname;
		    }
		    if (p == NULL || buf->b_fname == NULL)
			buf->b_fname = buf->b_ffname;
		    mf_fullname(buf->b_ml.ml_mfp);
		}
	    }
	    status_redraw_all();
	}
    }
}

    static void
do_pwd()
{
    if (mch_dirname(NameBuff, MAXPATHL) == OK)
	msg(NameBuff);
    else
	emsg(e_unknown);
}

    static void
do_sleep(eap)
    EXARG	*eap;
{
    int	    n;

    if (cursor_valid())
    {
	n = curwin->w_winpos + curwin->w_wrow - msg_scrolled;
	if (n >= 0)
	{
	    windgoto((int)n, curwin->w_wcol);
	    out_flush();
	}
    }
    ui_delay(eap->line2 * 1000L, TRUE);
}

    static void
do_exmap(eap, isabbrev)
    EXARG	*eap;
    int		isabbrev;
{
    int	    mode;
    char_u  *cmdp;

    cmdp = eap->cmd;
    mode = get_map_mode(&cmdp, eap->forceit || isabbrev);

    switch (do_map((*cmdp == 'n') ? 2 : (*cmdp == 'u'),
						    eap->arg, mode, isabbrev))
    {
	case 1: emsg(e_invarg);
		break;
	case 2: emsg(e_nomap);
		break;
	case 3: emsg(e_ambmap);
		break;
    }
}

/*
 * ":winsize" command (obsolete).
 */
    static void
do_winsize(arg)
    char_u	*arg;
{
    int	    w, h;

    w = getdigits(&arg);
    arg = skipwhite(arg);
    h = getdigits(&arg);
    set_winsize(w, h, TRUE);
}

/*
 * Handle command that work like operators: ":delete", ":yank", ":>" and ":<".
 */
    static void
do_exops(eap)
    EXARG	*eap;
{
    OPARG	oa;

    clear_oparg(&oa);
    oa.regname = eap->regname;
    oa.start.lnum = eap->line1;
    oa.end.lnum = eap->line2;
    oa.line_count = eap->line2 - eap->line1 + 1;
    oa.motion_type = MLINE;
    if (eap->cmdidx != CMD_yank)	/* position cursor for undo */
    {
	setpcmark();
	curwin->w_cursor.lnum = eap->line1;
	beginline(BL_SOL | BL_FIX);
    }

    switch (eap->cmdidx)
    {
	case CMD_delete:
	    oa.op_type = OP_DELETE;
	    op_delete(&oa);
	    break;

	case CMD_yank:
	    oa.op_type = OP_YANK;
	    (void)op_yank(&oa, FALSE, TRUE);
	    break;

	default:    /* CMD_rshift or CMD_lshift */
	    if ((eap->cmdidx == CMD_rshift)
#ifdef RIGHTLEFT
				    ^ curwin->w_p_rl
#endif
						    )
		oa.op_type = OP_RSHIFT;
	    else
		oa.op_type = OP_LSHIFT;
	    op_shift(&oa, FALSE, eap->amount);
	    break;
    }
}

/*
 * Handle ":copy" and ":move".
 */
    static void
do_copymove(eap)
    EXARG	*eap;
{
    long	n;

    n = get_address(&eap->arg);
    if (eap->arg == NULL)	    /* error detected */
    {
	eap->nextcmd = NULL;
	return;
    }

    /*
     * move or copy lines from 'eap->line1'-'eap->line2' to below line 'n'
     */
    if (n == MAXLNUM || n < 0 || n > curbuf->b_ml.ml_line_count)
    {
	emsg(e_invaddr);
	return;
    }

    if (eap->cmdidx == CMD_move)
    {
	if (do_move(eap->line1, eap->line2, n) == FAIL)
	    return;
    }
    else
	do_copy(eap->line1, eap->line2, n);
    u_clearline();
    beginline(BL_SOL | BL_FIX);
    update_screen(NOT_VALID);
}

/*
 * Handle ":join" command.
 */
    static void
do_exjoin(eap)
    EXARG	*eap;
{
    curwin->w_cursor.lnum = eap->line1;
    if (eap->line1 == eap->line2)
    {
	if (eap->addr_count >= 2)   /* :2,2join does nothing */
	    return;
	if (eap->line2 == curbuf->b_ml.ml_line_count)
	{
	    beep_flush();
	    return;
	}
	++eap->line2;
    }
    do_do_join(eap->line2 - eap->line1 + 1, !eap->forceit, FALSE);
    beginline(BL_WHITE | BL_FIX);
}

/*
 * Handle ":@" command, execute from register.
 */
    static void
do_exat(eap)
    EXARG	*eap;
{
    curwin->w_cursor.lnum = eap->line2;

#ifdef USE_GUI_WIN32
    dont_scroll = TRUE;		/* disallow scrolling here */
#endif

    /* put the register in mapbuf */
    if (do_execreg(*eap->arg, TRUE,
			      vim_strchr(p_cpo, CPO_EXECBUF) != NULL) == FAIL)
	beep_flush();
    else
    {
	/* execute from the mapbuf */
	while (vpeekc() == ':')
	{
	    (void)vgetc();
	    (void)do_cmdline(NULL, getexline, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);
	}
    }
}

/*
 * Handle ":redir" command, start/stop redirection.
 */
    static void
do_redir(eap)
    EXARG	*eap;
{
    char	*mode;

    if (STRICMP(eap->arg, "END") == 0)
	close_redir();
    else
    {
	if (*eap->arg == '>')
	{
	    ++eap->arg;
	    if (*eap->arg == '>')
	    {
		++eap->arg;
		mode = "a";
	    }
	    else
		mode = "w";
	    eap->arg = skipwhite(eap->arg);

	    close_redir();
	    redir_fd = open_exfile(eap, mode);
	}

	/* TODO: redirect to a buffer */

	/* TODO: redirect to an internal variable */

	else
	    EMSG(e_invarg);
    }
}

    static void
close_redir()
{
    if (redir_fd != NULL)
    {
	fclose(redir_fd);
	redir_fd = NULL;
    }
}

/*
 * Handle ":mkexrc" and ":mkvimrc" commands.
 */
    static void
do_mkrc(eap)
    EXARG	*eap;
{
    FILE    *fd;

    if (*eap->arg == NUL)
	eap->arg = (char_u *)EXRC_FILE;
    fd = open_exfile(eap, WRITEBIN);
    if (fd == NULL)
	return;

    /* Write the version command for :mkvimrc */
    if (eap->cmdidx == CMD_mkvimrc)
    {
#ifdef USE_CRNL
	fprintf(fd, "version 5.0\r\n");
#else
	fprintf(fd, "version 5.0\n");
#endif
    }

    if (makemap(fd) == FAIL || makeset(fd) == FAIL || fclose(fd))
	emsg(e_write);
}

/*
 * Open a file for writing for an Ex command, with some checks.
 * Return file descriptor, or NULL on failure.
 */
    static FILE	*
open_exfile(eap, mode)
    EXARG	*eap;
    char	*mode;	    /* "w" for create new file or "a" for append */
{
    FILE	*fd;

#ifdef UNIX
    /* with Unix it is possible to open a directory */
    if (mch_isdir(eap->arg))
    {
	EMSG2("\"%s\" is a directory", eap->arg);
	return NULL;
    }
#endif
    if (!eap->forceit && *mode != 'a' && vim_fexists(eap->arg))
    {
	EMSG2("\"%s\" exists (use ! to override)", eap->arg);
	return NULL;
    }

    if ((fd = fopen((char *)eap->arg, mode)) == NULL)
    {
	EMSG2("Cannot open \"%s\" for writing", eap->arg);
	return NULL;
    }
    return fd;
}

/*
 * Handle ":mark" or ":k" command.
 */
    static void
do_setmark(eap)
    EXARG	*eap;
{
    FPOS	pos;

    pos = curwin->w_cursor;	    /* save curwin->w_cursor */
    curwin->w_cursor.lnum = eap->line2;
    beginline(BL_WHITE | BL_FIX);
    (void)setmark(*eap->arg);	    /* set mark */
    curwin->w_cursor = pos;	    /* restore curwin->w_cursor */
}

#ifdef EX_EXTRA
/*
 * Handle ":normal[!] {commands}" - execute normal mode commands
 * Often used for ":autocmd".
 */
    static void
do_normal(eap)
    EXARG	*eap;
{
    OPARG	oa;
    int		len;
    int		save_msg_scroll = msg_scroll;
    int		save_restart_edit = restart_edit;
    int		save_msg_didout = msg_didout;

    msg_scroll = FALSE;	    /* no msg scrolling in Normal mode */
    restart_edit = 0;	    /* don't go to Insert mode */

    /*
     * Repeat the :normal command for each line in the range.  When no range
     * given, execute it just once, without positioning the cursor first.
     */
    do
    {
	clear_oparg(&oa);
	if (eap->addr_count != 0)
	{
	    curwin->w_cursor.lnum = eap->line1++;
	    curwin->w_cursor.col = 0;
	}

	/*
	 * Stuff the argument into the typeahead buffer.
	 * Execute normal_cmd() until there is no more
	 * typeahead than there was before this command.
	 */
	len = typelen;
	ins_typebuf(eap->arg, eap->forceit ? -1 : 0, 0, TRUE);
	while (	   (!stuff_empty()
			|| (!typebuf_typed()
			    && typelen > len))
		&& !got_int)
	{
	    adjust_cursor();		/* put cursor on valid line */
	    /* Make sure w_topline and w_leftcol are correct. */
	    update_topline();
	    if (!curwin->w_p_wrap)
		validate_cursor();
	    update_curswant();

	    normal_cmd(&oa, FALSE);	/* execute a Normal mode cmd */
	}
    }
    while (eap->addr_count > 0 && eap->line1 <= eap->line2 && !got_int);

    msg_scroll = save_msg_scroll;
    restart_edit = save_restart_edit;
    msg_didout |= save_msg_didout;	/* don't reset msg_didout now */
}
#endif

#ifdef FIND_IN_PATH
    static char_u *
do_findpat(eap, action)
    EXARG	*eap;
    int		action;
{
    int		whole = TRUE;
    long	n;
    char_u	*p;
    char_u	*errormsg = NULL;

    n = 1;
    if (isdigit(*eap->arg))	/* get count */
    {
	n = getdigits(&eap->arg);
	eap->arg = skipwhite(eap->arg);
    }
    if (*eap->arg == '/')   /* Match regexp, not just whole words */
    {
	whole = FALSE;
	++eap->arg;
	p = skip_regexp(eap->arg, '/', p_magic);
	if (*p)
	{
	    *p++ = NUL;
	    p = skipwhite(p);

	    /* Check for trailing illegal characters */
	    if (!ends_excmd(*p))
		errormsg = e_trailing;
	    else
		eap->nextcmd = check_nextcmd(p);
	}
    }
    if (!eap->skip)
	find_pattern_in_path(eap->arg, 0, (int)STRLEN(eap->arg),
			    whole, !eap->forceit,
			    *eap->cmd == 'd' ?	FIND_DEFINE : FIND_ANY,
			    n, action, eap->line1, eap->line2);

    return errormsg;
}
#endif

    static void
do_ex_tag(eap, dt)
    EXARG	*eap;
    int		dt;
{
    do_tag((char_u *)"", dt, eap->addr_count ? (int)eap->line2
					     : 1, eap->forceit);
}

#ifdef WANT_EVAL

    static char_u *
do_if(eap, cstack)
    EXARG		*eap;
    struct condstack	*cstack;
{
    char_u	*errormsg = NULL;
    int		error;

    if (cstack->cs_idx == CSTACK_LEN - 1)
	errormsg = (char_u *)":if nesting too deep";
    else
    {
	++cstack->cs_idx;
	cstack->cs_flags[cstack->cs_idx] = 0;
	if (cstack->cs_idx == 0
		|| (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))
	{
	    if (eval_to_bool(eap->arg, &error, &eap->nextcmd))
		cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE | CSF_TRUE;
	    if (error)
		--cstack->cs_idx;
	}
    }

    return errormsg;
}

/*
 * Handle ":else" and ":elseif" commands.
 */
    static char_u *
do_else(eap, cstack)
    EXARG		*eap;
    struct condstack	*cstack;
{
    char_u	*errormsg = NULL;
    int		error;

    if (cstack->cs_idx < 0 || (cstack->cs_flags[cstack->cs_idx] & CSF_WHILE))
    {
	if (eap->cmdidx == CMD_else)
	    errormsg = (char_u *)":else without :if";
	else
	    errormsg = (char_u *)":elseif without :if";
    }
    else
    {
	/*
	 * Only do something when there is no surrounding conditional, or the
	 * surrounding conditional was active.
	 */
	if (cstack->cs_idx == 0
		     || (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))
	{
	    /* if the ":if" was TRUE, reset active, otherwise set it */
	    if (cstack->cs_flags[cstack->cs_idx] & CSF_TRUE)
		cstack->cs_flags[cstack->cs_idx] = CSF_TRUE;
	    else
		cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE;

	    if (eap->cmdidx == CMD_elseif
		    && (cstack->cs_flags[cstack->cs_idx] & CSF_ACTIVE))
	    {
		if (eval_to_bool(eap->arg, &error, &eap->nextcmd))
		    cstack->cs_flags[cstack->cs_idx] = CSF_ACTIVE | CSF_TRUE;
		else
		    cstack->cs_flags[cstack->cs_idx] = 0;
		if (error)
		    --cstack->cs_idx;
	    }
	}
    }

    return errormsg;
}

/*
 * Handle ":while".
 */
    static char_u *
do_while(eap, cstack)
    EXARG		*eap;
    struct condstack	*cstack;
{
    char_u	*errormsg = NULL;
    int		error;

    if (cstack->cs_idx == CSTACK_LEN - 1)
	errormsg = (char_u *)":while nesting too deep";
    else
    {
	/*
	 * cs_had_while is set when we have jumped back from the matching
	 * ":endwhile".  When not set, need to init this cstack entry.
	 */
	if (!cstack->cs_had_while)
	{
	    ++cstack->cs_idx;
	    ++cstack->cs_whilelevel;
	}
	cstack->cs_flags[cstack->cs_idx] = CSF_WHILE;

	if (cstack->cs_idx == 0
		|| (cstack->cs_flags[cstack->cs_idx - 1] & CSF_ACTIVE))
	{
	    if (eval_to_bool(eap->arg, &error, &eap->nextcmd))
		cstack->cs_flags[cstack->cs_idx] |= CSF_ACTIVE | CSF_TRUE;
	    if (error)
		--cstack->cs_idx;
	    else
		/*
		 * Set cs_had_while flag, so do_cmdline() will set the line
		 * number in cs_line[].
		 */
		cstack->cs_had_while = TRUE;
	}
    }

    return errormsg;
}

/*
 * Handle ":continue".
 */
    static char_u *
do_continue(cstack)
    struct condstack	*cstack;
{
    char_u	*errormsg = NULL;

    if (cstack->cs_whilelevel <= 0 || cstack->cs_idx < 0)
	errormsg = (char_u *)":continue without :while";
    else
    {
	/* Find the matching ":while". */
	while (cstack->cs_idx > 0
		    && !(cstack->cs_flags[cstack->cs_idx] & CSF_WHILE))
	    --cstack->cs_idx;

	/*
	 * Set cs_had_continue, so do_cmdline() will jump back to the matching
	 * ":while".
	 */
	cstack->cs_had_continue = TRUE;	    /* let do_cmdline() handle it */
    }

    return errormsg;
}

/*
 * Handle ":break".
 */
    static char_u *
do_break(cstack)
    struct condstack	*cstack;
{
    char_u	*errormsg = NULL;
    int		idx;

    if (cstack->cs_whilelevel <= 0 || cstack->cs_idx < 0)
	errormsg = (char_u *)":break without :while";
    else
    {
	/* Find the matching ":while". */
	for (idx = cstack->cs_idx; idx >= 0; --idx)
	{
	    cstack->cs_flags[idx] &= ~CSF_ACTIVE;
	    if (cstack->cs_flags[idx] & CSF_WHILE)
		break;
	}
    }

    return errormsg;
}

/*
 * Handle ":endwhile".
 */
    static char_u *
do_endwhile(cstack)
    struct condstack	*cstack;
{
    char_u	*errormsg = NULL;

    if (cstack->cs_whilelevel <= 0 || cstack->cs_idx < 0)
	errormsg = (char_u *)":endwhile without :while";
    else
    {
	if (!(cstack->cs_flags[cstack->cs_idx] & CSF_WHILE))
	{
	    errormsg = (char_u *)":enwhile without :while";
	    while (cstack->cs_idx > 0
		    && !(cstack->cs_flags[cstack->cs_idx] & CSF_WHILE))
		--cstack->cs_idx;
	}
	/*
	 * Set cs_had_endwhile, so do_cmdline() will jump back to the matching
	 * ":while".
	 */
	cstack->cs_had_endwhile = TRUE;
    }

    return errormsg;
}

/*
 * Return TRUE if the string "p" looks like a ":while" command.
 */
    static int
has_while_cmd(p)
    char_u	*p;
{
    p = skipwhite(p);
    while (*p == ':')
	++p;
    p = skipwhite(p);
    if (p[0] == 'w' && p[1] == 'h')
	return TRUE;
    return FALSE;
}

#endif /* WANT_EVAL */

/*
 * Evaluate cmdline variables.
 *
 * change '%'	    to curbuf->b_ffname
 *	  '#'	    to curwin->w_altfile
 *	  '<cword>' to word under the cursor
 *	  '<cWORD>' to WORD under the cursor
 *	  '<cfile>' to path name under the cursor
 *	  '<sfile>" to sourced file name
 *	  '<afile>' to file name for autocommand
 *
 * When an error is detected, "errormsg" is set to a non-NULL pointer (may be
 * "" for error without a message) and NULL is returned.
 * Returns an allocated string if a valid match was found.
 * Returns NULL if no match was found.	"usedlen" then still contains the
 * number of characters to skip.
 */
    char_u *
eval_vars(src, usedlen, lnump, errormsg)
    char_u	*src;		/* pointer into commandline */
    int		*usedlen;	/* characters after src that are used */
    linenr_t	*lnump;		/* line number for :e command, or NULL */
    char_u	**errormsg;	/* error message, or NULL */
{
    int		i;
    char_u	*s;
    char_u	*tail;
    char_u	*result;
    int		resultlen;
    char_u	*buf = NULL;
    BUF		*buffer;
#define VALID_P	    1
#define VALID_H	    2
    int		valid = VALID_H + VALID_P;    /* assume valid result */
    int		spec_idx;
    static char *(spec_str[]) =
		{
		    "%",
#define SPEC_PERC   0
		    "#",
#define SPEC_HASH   1
		    "<cword>",		/* cursor word */
#define SPEC_CWORD  2
		    "<cWORD>",		/* cursor WORD */
#define SPEC_CCWORD 3
		    "<cfile>",		/* cursor path name */
#define SPEC_CFILE  4
		    "<sfile>",		/* ":so" file name */
#define SPEC_SFILE  5
#ifdef AUTOCMD
		    "<afile>"		/* autocommand file name */
# define SPEC_AFILE 6
#endif
		};
#define SPEC_COUNT  (sizeof(spec_str) / sizeof(char *))

    *errormsg = NULL;

    /*
     * Check if there is something to do.
     */
    for (spec_idx = 0; spec_idx < SPEC_COUNT; ++spec_idx)
    {
	*usedlen = strlen(spec_str[spec_idx]);
	if (STRNCMP(src, spec_str[spec_idx], *usedlen) == 0)
	    break;
    }
    if (spec_idx == SPEC_COUNT)	    /* no match */
    {
	*usedlen = 1;
	return NULL;
    }

    /*
     * Skip when preceded with a backslash "\%" and "\#".
     * Note: In "\\%" the % is also not recognized!
     */
    if (*(src - 1) == '\\')
    {
	*usedlen = 0;
	STRCPY(src - 1, src);		/* remove backslash */
	return NULL;
    }

    /*
     * word or WORD under cursor
     */
    if (spec_idx == SPEC_CWORD || spec_idx == SPEC_CCWORD)
    {
	resultlen = find_ident_under_cursor(&result, spec_idx == SPEC_CWORD ?
				      (FIND_IDENT|FIND_STRING) : FIND_STRING);
	if (resultlen == 0)
	{
	    *errormsg = (char_u *)"";
	    return NULL;
	}
    }

    /*
     * '#': Alternate file name
     * '%': Current file name
     *	    File name under the cursor
     *	    File name for autocommand
     *	and following modifiers
     */
    else
    {
	switch (spec_idx)
	{
	case SPEC_PERC:		    /* '%': current file */
		if (curbuf->b_fname == NULL)
		{
		    result = (char_u *)"";
		    valid = 0;	    /* Must have ":p:h" to be valid */
		}
		else
		    result = curbuf->b_fname;
		break;

	case SPEC_HASH:		/* '#' or "#99": alternate file */
		s = src + 1;
		i = (int)getdigits(&s);
		*usedlen = s - src;	/* length of what we expand */

		buffer = buflist_findnr(i);
		if (buffer == NULL)
		{
		    *errormsg = (char_u *)"No alternate file name to substitute for '#'";
		    return NULL;
		}
		if (lnump != NULL)
		    *lnump = buflist_findlnum(buffer);
		if (buffer->b_fname == NULL)
		{
		    result = (char_u *)"";
		    valid = 0;	    /* Must have ":p:h" to be valid */
		}
		else
		    result = buffer->b_fname;
		break;

#ifdef FILE_IN_PATH
	case SPEC_CFILE:	    /* file name under cursor */
		result = file_name_at_cursor(FNAME_MESS|FNAME_HYP, 1L);
		if (result == NULL)
		{
		    *errormsg = (char_u *)"";
		    return NULL;
		}
		buf = result;	    /* remember allocated string */
		break;
#endif

#ifdef AUTOCMD
	case SPEC_AFILE:	    /* file name for autocommand */
		result = autocmd_fname;
		if (result == NULL)
		{
		    *errormsg = (char_u *)"no autocommand file name to substitute for \"<afile>\"";
		    return NULL;
		}
		break;
#endif
	case SPEC_SFILE:	    /* file name for ":so" command */
		result = sourcing_name;
		if (result == NULL)
		{
		    *errormsg = (char_u *)"no :soure file name to substitute for \"<sfile>\"";
		    return NULL;
		}
		break;
	}

	resultlen = STRLEN(result);	/* length of new string */
	if (src[*usedlen] == '<')	/* remove the file name extension */
	{
	    ++*usedlen;
	    if ((s = vim_strrchr(result, '.')) != NULL && s >= gettail(result))
		resultlen = s - result;
	}
	else
	{
	    /* ":p" - full path/file_name */
	    if (src[*usedlen] == ':' && src[*usedlen + 1] == 'p')
	    {
		valid |= VALID_P;
		*usedlen += 2;
		result = FullName_save(result, FALSE);
		vim_free(buf);	    /* free any allocated file name */
		if (result == NULL)
		{
		    *errormsg = (char_u *)"";
		    return NULL;
		}
		resultlen = STRLEN(result);
		buf = result;
	    }

	    tail = gettail(result);

	    /* ":h" - head, remove "/file_name", can be repeated  */
	    /* Don't remove the first "/" or "c:\" */
	    while (src[*usedlen] == ':' && src[*usedlen + 1] == 'h')
	    {
		valid |= VALID_H;
		*usedlen += 2;
		s = get_past_head(result);
		while (tail > s && vim_ispathsep(tail[-1]))
		    --tail;
		resultlen = tail - result;
		while (tail > s && !vim_ispathsep(tail[-1]))
		    --tail;
	    }

	    /* ":t" - tail, just the basename */
	    if (src[*usedlen] == ':' && src[*usedlen + 1] == 't')
	    {
		*usedlen += 2;
		resultlen -= tail - result;
		result = tail;
	    }

	    /* ":e" - extension, can be repeated */
	    /* ":r" - root, without extension, can be repeated */
	    while (src[*usedlen] == ':' &&
		       (src[*usedlen + 1] == 'e' || src[*usedlen + 1] == 'r'))
	    {
		/* find a '.' in the tail:
		 * - for second :e: before the current fname
		 * - otherwise: The last '.'
		 */
		if (src[*usedlen + 1] == 'e' && result > tail)
		    s = result - 2;
		else
		    s = result + resultlen - 1;
		for ( ; s > tail; --s)
		    if (s[0] == '.')
			break;
		if (src[*usedlen + 1] == 'e')		/* :e */
		{
		    if (s > tail)
		    {
			resultlen += result - (s + 1);
			result = s + 1;
		    }
		    else if (result <= tail)
			resultlen = 0;
		}
		else				/* :r */
		{
		    if (s > tail)	/* remove one extension */
			resultlen = s - result;
		}
		*usedlen += 2;
	    }
	}

	/* TODO - ":s/pat/foo/" - substitute */
	/* if (src[*usedlen] == ':' && src[*usedlen + 1] == 's') */
    }

    if (resultlen == 0 || valid != VALID_H + VALID_P)
    {
	if (valid != VALID_H + VALID_P)
	    *errormsg = (char_u *)"Empty file name for '%' or '#', only works with \":p:h\"";
	else
	    *errormsg = (char_u *)"Evaluates to an empty string";
	result = NULL;
    }
    else
	result = vim_strnsave(result, resultlen);
    vim_free(buf);
    return result;
}

/*
 * Expand the <sfile> string in "arg".
 *
 * Returns an allocated string, or NULL for any error.
 */
    char_u *
expand_sfile(arg)
    char_u	*arg;
{
    char_u	*errormsg;
    int		len;
    char_u	*result;
    char_u	*newres;
    char_u	*repl;
    int		srclen;
    char_u	*p;
    linenr_t	dummy;

    result = vim_strsave(arg);
    if (result == NULL)
	return NULL;

    for (p = result; *p; )
    {
	if (STRNCMP(p, "<sfile>", 7))
	    ++p;
	else
	{
	    /* replace "<sfile>" with the sourced file name, and do ":" stuff */
	    repl = eval_vars(p, &srclen, &dummy, &errormsg);
	    if (errormsg != NULL)
	    {
		if (*errormsg)
		    emsg(errormsg);
		vim_free(result);
		return NULL;
	    }
	    if (repl == NULL)		/* no match (cannot happen) */
	    {
		p += srclen;
		continue;
	    }
	    len = STRLEN(result) - srclen + STRLEN(repl) + 1;
	    newres = alloc(len);
	    if (newres == NULL)
	    {
		vim_free(repl);
		vim_free(result);
		return NULL;
	    }
	    vim_memmove(newres, result, (size_t)(p - result));
	    STRCPY(newres + (p - result), repl);
	    len = STRLEN(newres);
	    STRCAT(newres, p + srclen);
	    vim_free(result);
	    result = newres;
	    p = newres + len;		/* continue after the match */
	}
    }

    return result;
}
