/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_cmds2.c: some more functions for command line commands
 */

#include "vim.h"

#if defined(FEAT_EVAL) || defined(PROTO)

/*
 * do_debug(): Debug mode.
 * Repeatedly get Ex commands, until told to continue normal execution.
 */
    void
do_debug(cmd)
    char_u	*cmd;
{
    int		save_msg_scroll = msg_scroll;
    int		save_State = State;
    int		save_did_emsg = did_emsg;
    int		n;
    char_u	*cmdline = NULL;
    char_u	*p;
    char	*tail = NULL;
    static int	last_cmd = 0;
#define CMD_CONT	1
#define CMD_NEXT	2
#define CMD_STEP	3
#define CMD_FINISH	4

#ifdef ALWAYS_USE_GUI
    /* Can't do this when there is no terminal for input/output. */
    if (!gui.in_use)
    {
	/* Break as soon as possible. */
	debug_break_level = 9999;
	return;
    }
#endif

    /* Make sure we are in raw mode and start termcap mode.  Might have side
     * effects... */
    settmode(TMODE_RAW);
    starttermcap();

    ++RedrawingDisabled;	    /* don't redisplay the window */
    ++no_wait_return;		    /* don't wait for return */
    did_emsg = FALSE;		    /* don't use error from debugged stuff */

    State = NORMAL;
#ifdef FEAT_SNIFF
    want_sniff_request = 0;    /* No K_SNIFF wanted */
#endif

    if (!debug_did_msg)
	MSG(_("Entering Debug mode.  Type \"cont\" to leave."));
    if (sourcing_name != NULL)
	msg(sourcing_name);
    if (sourcing_lnum != 0)
	smsg((char_u *)_("line %ld: %s"), (long)sourcing_lnum, cmd);
    else
	smsg((char_u *)_("cmd: %s"), cmd);

    /*
     * Repeat getting a command and executing it.
     */
    for (;;)
    {
	msg_scroll = TRUE;
	need_wait_return = FALSE;
#ifdef FEAT_SNIFF
	ProcessSniffRequests();
#endif
	cmdline = getcmdline('>', 0L, 0);
	cmdline_row = msg_row;
	if (cmdline != NULL)
	{
	    /* If this is a debug command, set "last_cmd".
	     * If not, reset "last_cmd".
	     * For a blank line use previous command. */
	    p = skipwhite(cmdline);
	    if (*p != NUL)
	    {
		switch (*p)
		{
		    case 'c': last_cmd = CMD_CONT;
			      tail = "ont";
			      break;
		    case 'n': last_cmd = CMD_NEXT;
			      tail = "ext";
			      break;
		    case 's': last_cmd = CMD_STEP;
			      tail = "tep";
			      break;
		    case 'f': last_cmd = CMD_FINISH;
			      tail = "inish";
			      break;
		    default: last_cmd = 0;
		}
		if (last_cmd != 0)
		{
		    /* Check that the tail matches. */
		    ++p;
		    while (*p != NUL && *p == *tail)
		    {
			++p;
			++tail;
		    }
		    if (ASCII_ISALPHA(*p))
			last_cmd = 0;
		}
	    }
	}
	if (last_cmd != 0)
	{
	    /* Execute debug command: decided where to break next and return. */
	    switch (last_cmd)
	    {
		case CMD_CONT:
		    debug_break_level = -1;
		    break;
		case CMD_NEXT:
		    debug_break_level = debug_level;
		    break;
		case CMD_STEP:
		    debug_break_level = 9999;
		    break;
		case CMD_FINISH:
		    debug_break_level = debug_level - 1;
		    break;
	    }
	    break;
	}
	if (cmdline != NULL)
	{
	    /* don't debug this command */
	    n = debug_break_level;
	    debug_break_level = -1;
	    (void)do_cmdline(cmdline, getexline, NULL, DOCMD_VERBOSE);
	    debug_break_level = n;

	    vim_free(cmdline);
	}
	lines_left = Rows - 1;
    }
    vim_free(cmdline);

    --RedrawingDisabled;
    --no_wait_return;
    redraw_all_later(NOT_VALID);
    need_wait_return = FALSE;
    msg_scroll = save_msg_scroll;
    lines_left = Rows - 1;
    State = save_State;
    did_emsg = save_did_emsg;

    /* Only print the message again when typing a command before coming back
     * here. */
    debug_did_msg = TRUE;
}

/*
 * ":debug".
 */
    void
ex_debug(eap)
    exarg_T	*eap;
{
    int		debug_break_level_save = debug_break_level;

    debug_break_level = 9999;
    do_cmdline_cmd(eap->arg);
    debug_break_level = debug_break_level_save;
}

static char_u	*debug_breakpoint_name = NULL;
static linenr_T	debug_breakpoint_lnum;

/*
 * Go to debug mode when a breakpoint was encountered or "debug_level" is
 * at or below the break level.  But only when the line is actually
 * executed.
 * Called from do_one_cmd() before executing a command.
 */
    void
dbg_check_breakpoint(eap)
    exarg_T	*eap;
{
    char_u	*p;

    if (debug_breakpoint_name != NULL)
    {
	if (!eap->skip)
	{
	    /* replace K_SNR with "<SNR>" */
	    if (debug_breakpoint_name[0] == K_SPECIAL
		    && debug_breakpoint_name[1] == KS_EXTRA
		    && debug_breakpoint_name[2] == (int)KE_SNR)
		p = (char_u *)"<SNR>";
	    else
		p = (char_u *)"";
	    smsg((char_u *)_("Breakpoint in \"%s%s\" line %ld"), p,
		    debug_breakpoint_name + (*p == NUL ? 0 : 3),
		    (long)debug_breakpoint_lnum);
	    debug_breakpoint_name = NULL;
	    do_debug(eap->cmd);
	}
	else
	    debug_breakpoint_name = NULL;
    }
    else if (!eap->skip && debug_level <= debug_break_level)
	do_debug(eap->cmd);
}

/*
 * The list of breakpoints: dbg_breakp.
 * This is a grow-array of structs.
 */
struct debuggy
{
    int		dbg_nr;		/* breakpoint number */
    int		dbg_type;	/* DBG_FUNC or DBG_FILE */
    char_u	*dbg_name;	/* function or file name */
    regprog_T	*dbg_prog;	/* regexp program */
    linenr_T	dbg_lnum;	/* line number in function or file */
};

static garray_T dbg_breakp = {0, 0, sizeof(struct debuggy), 4, NULL};
#define BREAKP(idx)	(((struct debuggy *)dbg_breakp.ga_data)[idx])
static int last_breakp = 0;	/* nr of last defined breakpoint */

#define DBG_FUNC	1
#define DBG_FILE	2

static int dbg_parsearg __ARGS((char_u *arg));

/*
 * Parse the arguments of ":breakadd" or ":breakdel" and put them in the entry
 * just after the last one in dbg_breakp.  Note that "dbg_name" is allocated.
 * Returns FAIL for failure.
 */
    static int
dbg_parsearg(arg)
    char_u	*arg;
{
    char_u	*p = arg;
    struct debuggy *bp;

    if (ga_grow(&dbg_breakp, 1) == FAIL)
	return FAIL;
    bp = &BREAKP(dbg_breakp.ga_len);

    /* Find "func" or "file". */
    if (STRNCMP(p, "func", 4) == 0)
	bp->dbg_type = DBG_FUNC;
    else if (STRNCMP(p, "file", 4) == 0)
	bp->dbg_type = DBG_FILE;
    else
    {
	EMSG2(_(e_invarg2), p);
	return FAIL;
    }
    p = skipwhite(p + 4);

    /* Find optional line number. */
    if (isdigit(*p))
    {
	bp->dbg_lnum = getdigits(&p);
	p = skipwhite(p);
    }
    else
	bp->dbg_lnum = 0;

    /* Find the function or file name.  Don't accept a function name with (). */
    if (*p == NUL
	    || (bp->dbg_type == DBG_FUNC && strstr((char *)p, "()") != NULL))
    {
	EMSG2(_(e_invarg2), arg);
	return FAIL;
    }
    if ((bp->dbg_name = vim_strsave(p)) == NULL)
	return FAIL;
    return OK;
}

/*
 * ":breakadd".
 */
    void
ex_breakadd(eap)
    exarg_T	*eap;
{
    struct debuggy *bp;
    char_u	*pat;

    if (dbg_parsearg(eap->arg) == OK)
    {
	bp = &BREAKP(dbg_breakp.ga_len);
	pat = file_pat_to_reg_pat(bp->dbg_name, NULL, NULL, FALSE);
	if (pat != NULL)
	{
	    bp->dbg_prog = vim_regcomp(pat, TRUE);
	    vim_free(pat);
	}
	if (pat == NULL || bp->dbg_prog == NULL)
	    vim_free(bp->dbg_name);
	else
	{
	    if (bp->dbg_lnum == 0)	/* default line number is 1 */
		bp->dbg_lnum = 1;
	    BREAKP(dbg_breakp.ga_len++).dbg_nr = ++last_breakp;
	    --dbg_breakp.ga_room;
	    ++debug_tick;
	}
    }
}

/*
 * ":breakdel".
 */
    void
ex_breakdel(eap)
    exarg_T	*eap;
{
    struct debuggy *bp, *bpi;
    int		nr;
    int		todel = -1;
    int		i;
    linenr_T	best_lnum = 0;

    if (isdigit(*eap->arg))
    {
	/* ":breakdel {nr}" */
	nr = atol((char *)eap->arg);
	for (i = 0; i < dbg_breakp.ga_len; ++i)
	    if (BREAKP(i).dbg_nr == nr)
	    {
		todel = i;
		break;
	    }
    }
    else
    {
	/* ":breakdel {func|file} [lnum] {name}" */
	if (dbg_parsearg(eap->arg) == FAIL)
	    return;
	bp = &BREAKP(dbg_breakp.ga_len);
	for (i = 0; i < dbg_breakp.ga_len; ++i)
	{
	    bpi = &BREAKP(i);
	    if (bp->dbg_type == bpi->dbg_type
		    && STRCMP(bp->dbg_name, bpi->dbg_name) == 0
		    && (bp->dbg_lnum == bpi->dbg_lnum
			|| (bp->dbg_lnum == 0
			    && (best_lnum == 0
				|| bpi->dbg_lnum < best_lnum))))
	    {
		todel = i;
		best_lnum = bpi->dbg_lnum;
	    }
	}
	vim_free(bp->dbg_name);
    }

    if (todel < 0)
	EMSG2(_("E161: Breakpoint not found: %s"), eap->arg);
    else
    {
	vim_free(BREAKP(todel).dbg_name);
	vim_free(BREAKP(todel).dbg_prog);
	--dbg_breakp.ga_len;
	++dbg_breakp.ga_room;
	if (todel < dbg_breakp.ga_len)
	    mch_memmove(&BREAKP(todel), &BREAKP(todel + 1),
		    (dbg_breakp.ga_len - todel) * sizeof(struct debuggy));
	++debug_tick;
    }
}

/*
 * ":breaklist".
 */
/*ARGSUSED*/
    void
ex_breaklist(eap)
    exarg_T	*eap;
{
    struct debuggy *bp;
    int		i;

    if (dbg_breakp.ga_len == 0)
	MSG(_("No breakpoints defined"));
    else
	for (i = 0; i < dbg_breakp.ga_len; ++i)
	{
	    bp = &BREAKP(i);
	    smsg((char_u *)_("%3d  %s %s  line %ld"),
		    bp->dbg_nr,
		    bp->dbg_type == DBG_FUNC ? "func" : "file",
		    bp->dbg_name,
		    (long)bp->dbg_lnum);
	}
}

/*
 * Find a breakpoint for a function or sourced file.
 * Returns line number at which to break; zero when no matching breakpoint.
 */
    linenr_T
dbg_find_breakpoint(file, fname, after)
    int		file;	    /* TRUE for a file, FALSE for a function */
    char_u	*fname;	    /* file or function name */
    linenr_T	after;	    /* after this line number */
{
    struct debuggy *bp;
    int		i;
    linenr_T	lnum = 0;
    regmatch_T	regmatch;
    char_u	*name = fname;

    /* Replace K_SNR in function name with "<SNR>". */
    if (!file && fname[0] == K_SPECIAL)
    {
	name = alloc((unsigned)STRLEN(fname) + 3);
	if (name == NULL)
	    name = fname;
	else
	{
	    STRCPY(name, "<SNR>");
	    STRCPY(name + 5, fname + 3);
	}
    }

    for (i = 0; i < dbg_breakp.ga_len; ++i)
    {
	/* skip entries that are not useful or are for a line that is beyond
	 * an already found breakpoint */
	bp = &BREAKP(i);
	if ((bp->dbg_type == DBG_FILE) == file
		&& bp->dbg_lnum > after
		&& (lnum == 0 || bp->dbg_lnum < lnum))
	{
	    regmatch.regprog = bp->dbg_prog;
	    regmatch.rm_ic = FALSE;
	    if (vim_regexec(&regmatch, name, (colnr_T)0))
		lnum = bp->dbg_lnum;
	}
    }
    if (name != fname)
	vim_free(name);

    return lnum;
}

/*
 * Called when a breakpoint was encountered.
 */
    void
dbg_breakpoint(name, lnum)
    char_u	*name;
    linenr_T	lnum;
{
    /* We need to check if this line is actually executed in do_one_cmd() */
    debug_breakpoint_name = name;
    debug_breakpoint_lnum = lnum;
}
#endif

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    int
autowrite(buf, forceit)
    buf_T	*buf;
    int		forceit;
{
    if (!(p_aw || p_awa) || !p_write
#ifdef FEAT_QUICKFIX
	/* never autowrite a "nofile" or "nowrite" buffer */
	|| bt_dontwrite(buf)
#endif
	|| (!forceit && buf->b_p_ro) || buf->b_ffname == NULL)
	return FAIL;
    return buf_write_all(buf, forceit);
}

/*
 * flush all buffers, except the ones that are readonly
 */
    void
autowrite_all()
{
    buf_T	*buf;

    if (!(p_aw || p_awa) || !p_write)
	return;
    for (buf = firstbuf; buf; buf = buf->b_next)
	if (bufIsChanged(buf) && !buf->b_p_ro)
	{
	    (void)buf_write_all(buf, FALSE);
#ifdef FEAT_AUTOCMD
	    /* an autocommand may have deleted the buffer */
	    if (!buf_valid(buf))
		buf = firstbuf;
#endif
	}
}

/*
 * return TRUE if buffer was changed and cannot be abandoned.
 */
/*ARGSUSED*/
    int
check_changed(buf, checkaw, mult_win, forceit, allbuf)
    buf_T	*buf;
    int		checkaw;	/* do autowrite if buffer was changed */
    int		mult_win;	/* check also when several wins for the buf */
    int		forceit;
    int		allbuf;		/* may write all buffers */
{
    if (       !forceit
	    && bufIsChanged(buf)
	    && (mult_win || buf->b_nwindows <= 1)
	    && (!checkaw || autowrite(buf, forceit) == FAIL))
    {
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	if ((p_confirm || cmdmod.confirm) && p_write)
	{
	    buf_T	*buf2;
	    int		count = 0;

	    if (allbuf)
		for (buf2 = firstbuf; buf2 != NULL; buf2 = buf2->b_next)
		    if (bufIsChanged(buf2)
				     && (buf2->b_ffname != NULL
# ifdef FEAT_BROWSE
					 || cmdmod.browse
# endif
					))
			++count;
#ifdef FEAT_AUTOCMD
	    if (!buf_valid(buf))
		/* Autocommand deleted buffer, oops!  It's not changed now. */
		return FALSE;
#endif
	    dialog_changed(buf, count > 1);
#ifdef FEAT_AUTOCMD
	    if (!buf_valid(buf))
		/* Autocommand deleted buffer, oops!  It's not changed now. */
		return FALSE;
#endif
	    return bufIsChanged(buf);
	}
#endif
	EMSG(_(e_nowrtmsg));
	return TRUE;
    }
    return FALSE;
}

#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG) || defined(PROTO)

#ifdef FEAT_BROWSE
static void	browse_save_fname __ARGS((buf_T *buf));

/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    static void
browse_save_fname(buf)
    buf_T	*buf;
{
    if (buf->b_fname == NULL)
    {
	char_u *fname;

	fname = do_browse(TRUE, (char_u *)_("Save As"), NULL, NULL, NULL,
								   NULL, buf);
	if (fname != NULL)
	{
	    setfname(fname, NULL, TRUE);
	    vim_free(fname);
	}
    }
}
#endif

/*
 * Ask the user what to do when abondoning a changed buffer.
 */
    void
dialog_changed(buf, checkall)
    buf_T	*buf;
    int		checkall;	/* may abandon all changed buffers */
{
    char_u	buff[IOSIZE];
    int		ret;
    buf_T	*buf2;

    dialog_msg(buff, _("Save changes to \"%.*s\"?"),
			(buf->b_fname != NULL) ?
			buf->b_fname : (char_u *)_("Untitled"));
    if (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, NULL, buff, 1);
    else
	ret = vim_dialog_yesnocancel(VIM_QUESTION, NULL, buff, 1);

    if (ret == VIM_YES)
    {
#ifdef FEAT_BROWSE
	/* May get file name, when there is none */
	browse_save_fname(buf);
#endif
	if (buf->b_fname != NULL)   /* didn't hit Cancel */
	    (void)buf_write_all(buf, FALSE);
    }
    else if (ret == VIM_NO)
    {
	unchanged(buf, TRUE);
    }
    else if (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	for (buf2 = firstbuf; buf2 != NULL; buf2 = buf2->b_next)
	{
	    if (bufIsChanged(buf2)
		    && (buf2->b_ffname != NULL
#ifdef FEAT_BROWSE
			|| cmdmod.browse
#endif
			)
		    && !buf2->b_p_ro)
	    {
#ifdef FEAT_BROWSE
		/* May get file name, when there is none */
		browse_save_fname(buf2);
#endif
		if (buf2->b_fname != NULL)   /* didn't hit Cancel */
		    (void)buf_write_all(buf2, FALSE);
#ifdef FEAT_AUTOCMD
		/* an autocommand may have deleted the buffer */
		if (!buf_valid(buf2))
		    buf2 = firstbuf;
#endif
	    }
	}
    }
    else if (ret == VIM_DISCARDALL)
    {
	/*
	 * mark all buffers as unchanged
	 */
	for (buf2 = firstbuf; buf2 != NULL; buf2 = buf2->b_next)
	    unchanged(buf2, TRUE);
    }
}
#endif

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    int
can_abandon(buf, forceit)
    buf_T	*buf;
    int		forceit;
{
    return (	   P_HID(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 */
    int
check_changed_any(hidden)
    int		hidden;		/* Only check hidden buffers */
{
    buf_T	*buf;
    int		save;
#ifdef FEAT_WINDOWS
    win_T	*wp;
#endif

#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    for (;;)
    {
#endif
	/* check curbuf first: if it was changed we can't abandon it */
	if (!hidden && curbufIsChanged())
	    buf = curbuf;
	else
	{
	    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
		if ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
		    break;
	}
	if (buf == NULL)    /* No buffers changed */
	    return FALSE;

#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	if (p_confirm || cmdmod.confirm)
	{
	    if (check_changed(buf, p_awa, TRUE, FALSE, TRUE) && buf_valid(buf))
		break;	    /* didn't save - still changes */
	}
	else
	    break;	    /* confirm not active - has changes */
    }
#endif

    exiting = FALSE;
#if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    if (!(p_confirm || cmdmod.confirm))
#endif
    {
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
	if (EMSG2(_("E162: No write since last change for buffer \"%s\""),
		    buf_spname(buf) != NULL ? (char_u *)buf_spname(buf) :
		    buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

#ifdef FEAT_WINDOWS
    /* Try to find a window that contains the buffer. */
    if (buf != curbuf)
	for (wp = firstwin; wp != NULL; wp = wp->w_next)
	    if (wp->w_buffer == buf)
	    {
		win_goto(wp);
# ifdef FEAT_AUTOCMD
		/* Paranoia: did autocms wipe out the buffer with changes? */
		if (!buf_valid(buf))
		    return TRUE;
# endif
		break;
	    }
#endif

    /* Open the changed buffer in the current window. */
    if (buf != curbuf)
	set_curbuf(buf, DOBUF_GOTO);

    return TRUE;
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
	EMSG(_(e_noname));
	return FAIL;
    }
    return OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    int
buf_write_all(buf, forceit)
    buf_T	*buf;
    int		forceit;
{
    int	    retval;
#ifdef FEAT_AUTOCMD
    buf_T	*old_curbuf = curbuf;
#endif

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_line_count, NULL,
						  FALSE, forceit, TRUE, FALSE));
#ifdef FEAT_AUTOCMD
    if (curbuf != old_curbuf)
	MSG(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
#endif
    return retval;
}

/*
 * Code to handle the argument list.
 */
static int do_arglist __ARGS((char_u *str, int what, int after));
static void alist_check_arg_idx __ARGS((void));
#ifdef FEAT_LISTCMDS
static int alist_add_list __ARGS((int count, char_u **files, int after));
#endif
#define AL_SET	1
#define AL_ADD	2
#define AL_DEL	3

/*
 * "what" == AL_SET: Redefine the argument list to 'str'.
 * "what" == AL_ADD: add files in 'str' to the argument list after "after".
 * "what" == AL_DEL: remove files in 'str' from the argument list.
 *
 * Return FAIL for failure, OK otherwise.
 */
/*ARGSUSED*/
    static int
do_arglist(str, what, after)
    char_u	*str;
    int		what;
    int		after;		/* 0 means before first one */
{
    garray_T	new_ga;
    int		exp_count;
    char_u	**exp_files;
    char_u	*p;
    int		inquote;
    int		inbacktick;
    int		i;
#ifdef FEAT_LISTCMDS
    int		match;
#endif

    /*
     * Collect all file name arguments in "new_ga".
     */
    ga_init2(&new_ga, (int)sizeof(char_u *), 20);
    while (*str)
    {
	if (ga_grow(&new_ga, 1) == FAIL)
	{
	    ga_clear(&new_ga);
	    return FAIL;
	}
	((char_u **)new_ga.ga_data)[new_ga.ga_len++] = str;
	--new_ga.ga_room;

	/*
	 * Isolate one argument, taking quotes and backticks.
	 * Quotes are removed, backticks remain.
	 */
	inquote = FALSE;
	inbacktick = FALSE;
	for (p = str; *str; ++str)
	{
	    /*
	     * for MSDOS et.al. a backslash is part of a file name.
	     * Only skip ", space and tab.
	     */
	    if (rem_backslash(str))
	    {
		*p++ = *str++;
		*p++ = *str;
	    }
	    else
	    {
		/* An item ends at a space not in quotes or backticks */
		if (!inquote && !inbacktick && vim_isspace(*str))
		    break;
		if (!inquote && *str == '`')
		    inbacktick ^= TRUE;
		if (!inbacktick && *str == '"')
		    inquote ^= TRUE;
		else
		    *p++ = *str;
	    }
	}
	str = skipwhite(str);
	*p = NUL;
    }

#ifdef FEAT_LISTCMDS
    if (what == AL_DEL)
    {
	regmatch_T	regmatch;
	int		didone;

	/*
	 * Delete the items: use each item as a regexp and find a match in the
	 * argument list.
	 */
#ifdef CASE_INSENSITIVE_FILENAME
	regmatch.rm_ic = TRUE;		/* Always ignore case */
#else
	regmatch.rm_ic = FALSE;		/* Never ignore case */
#endif
	for (i = 0; i < new_ga.ga_len && !got_int; ++i)
	{
	    p = ((char_u **)new_ga.ga_data)[i];
	    p = file_pat_to_reg_pat(p, NULL, NULL, FALSE);
	    if (p == NULL)
		break;
	    regmatch.regprog = vim_regcomp(p, (int)p_magic);
	    if (regmatch.regprog == NULL)
	    {
		vim_free(p);
		break;
	    }

	    didone = FALSE;
	    for (match = 0; match < ARGCOUNT; ++match)
		if (vim_regexec(&regmatch, alist_name(&ARGLIST[match]),
								  (colnr_T)0))
		{
		    didone = TRUE;
		    vim_free(ARGLIST[match].ae_fname);
		    mch_memmove(ARGLIST + match, ARGLIST + match + 1,
			    (ARGCOUNT - match - 1) * sizeof(aentry_T));
		    --ALIST(curwin)->al_ga.ga_len;
		    ++ALIST(curwin)->al_ga.ga_room;
		    if (curwin->w_arg_idx > match)
			--curwin->w_arg_idx;
		    --match;
		}

	    vim_free(regmatch.regprog);
	    vim_free(p);
	    if (!didone)
		EMSG2(_(e_nomatch2), ((char_u **)new_ga.ga_data)[i]);
	}
	ga_clear(&new_ga);
    }
    else
#endif
    {
	i = expand_wildcards(new_ga.ga_len, (char_u **)new_ga.ga_data,
		&exp_count, &exp_files, EW_DIR|EW_FILE|EW_ADDSLASH|EW_NOTFOUND);
	ga_clear(&new_ga);
	if (i == FAIL)
	    return FAIL;
	if (exp_count == 0)
	{
	    EMSG(_(e_nomatch));
	    return FAIL;
	}

#ifdef FEAT_LISTCMDS
	if (what == AL_ADD)
	{
	    (void)alist_add_list(exp_count, exp_files, after);
	    vim_free(exp_files);
	}
	else /* what == AL_SET */
#endif
	    alist_set(ALIST(curwin), exp_count, exp_files, FALSE);
    }

    alist_check_arg_idx();

    return OK;
}

/*
 * Check the validity of the arg_idx for each other window.
 */
    static void
alist_check_arg_idx()
{
#ifdef FEAT_WINDOWS
    win_T	*win;

    for (win = firstwin; win != NULL; win = win->w_next)
	if (win->w_alist == curwin->w_alist)
	    check_arg_idx(win);
#else
    check_arg_idx(curwin);
#endif
}

/*
 * Check if window "win" is editing the w_arg_idx file in its argument list.
 */
    void
check_arg_idx(win)
    win_T	*win;
{
    if (WARGCOUNT(win) > 1
	    && (win->w_arg_idx >= WARGCOUNT(win)
		|| (win->w_buffer->b_fnum
				      != WARGLIST(win)[win->w_arg_idx].ae_fnum
		    && (win->w_buffer->b_ffname == NULL
			 || !(fullpathcmp(
				 alist_name(&WARGLIST(win)[win->w_arg_idx]),
				win->w_buffer->b_ffname, TRUE) & FPC_SAME)))))
    {
	/* We are not editing the current entry in the argument list.
	 * Set "arg_had_last" if we are editing the last one. */
	win->w_arg_idx_invalid = TRUE;
	if (win->w_arg_idx != WARGCOUNT(win) - 1
		&& arg_had_last == FALSE
#ifdef FEAT_WINDOWS
		&& ALIST(win) == &global_alist
#endif
		&& GARGCOUNT > 0
		&& win->w_arg_idx < GARGCOUNT
		&& (win->w_buffer->b_fnum == GARGLIST[GARGCOUNT - 1].ae_fnum
		    || (win->w_buffer->b_ffname != NULL
			&& (fullpathcmp(alist_name(&GARGLIST[GARGCOUNT - 1]),
				win->w_buffer->b_ffname, TRUE) & FPC_SAME))))
	    arg_had_last = TRUE;
    }
    else
    {
	/* We are editing the current entry in the argument list.
	 * Set "arg_had_last" if it's also the last one */
	win->w_arg_idx_invalid = FALSE;
	if (win->w_arg_idx == WARGCOUNT(win) - 1
#ifdef FEAT_WINDOWS
		&& win->w_alist == &global_alist
#endif
		)
	    arg_had_last = TRUE;
    }
}

/*
 * ":args", ":argslocal" and ":argsglobal".
 */
    void
ex_args(eap)
    exarg_T	*eap;
{
    int		i;

    if (eap->cmdidx != CMD_args)
    {
#if defined(FEAT_WINDOWS) && defined(FEAT_LISTCMDS)
	alist_unlink(ALIST(curwin));
	if (eap->cmdidx == CMD_argglobal)
	    ALIST(curwin) = &global_alist;
	else /* eap->cmdidx == CMD_arglocal */
	    alist_new();
#else
	ex_ni(eap);
	return;
#endif
    }

    if (!ends_excmd(*eap->arg))
    {
	/*
	 * ":args file ..": define new argument list, handle like ":next"
	 * Also for ":argslocal file .." and ":argsglobal file ..".
	 */
	ex_next(eap);
    }
    else
#if defined(FEAT_WINDOWS) && defined(FEAT_LISTCMDS)
	if (eap->cmdidx == CMD_args)
#endif
    {
	/*
	 * ":args": list arguments.
	 */
	if (ARGCOUNT > 0)
	{
	    /* Overwrite the command, for a short list there is no scrolling
	     * required and no wait_return(). */
	    gotocmdline(TRUE);
	    for (i = 0; i < ARGCOUNT; ++i)
	    {
		if (i == curwin->w_arg_idx)
		    msg_putchar('[');
		msg_outtrans(alist_name(&ARGLIST[i]));
		if (i == curwin->w_arg_idx)
		    msg_putchar(']');
		msg_putchar(' ');
	    }
	}
    }
#if defined(FEAT_WINDOWS) && defined(FEAT_LISTCMDS)
    else if (eap->cmdidx == CMD_arglocal)
    {
	garray_T	*gap = &curwin->w_alist->al_ga;

	/*
	 * ":argslocal": make a local copy of the global argument list.
	 */
	if (ga_grow(gap, GARGCOUNT) == OK)
	    for (i = 0; i < GARGCOUNT; ++i)
		if (GARGLIST[i].ae_fname != NULL)
		{
		    AARGLIST(curwin->w_alist)[gap->ga_len].ae_fname =
					    vim_strsave(GARGLIST[i].ae_fname);
		    AARGLIST(curwin->w_alist)[gap->ga_len].ae_fnum =
							  GARGLIST[i].ae_fnum;
		    ++gap->ga_len;
		    --gap->ga_room;
		}
    }
#endif
}

/*
 * ":previous", ":sprevious", ":Next" and ":sNext".
 */
    void
ex_previous(eap)
    exarg_T	*eap;
{
    /* If past the last one already, go to the last one. */
    if (curwin->w_arg_idx - (int)eap->line2 >= ARGCOUNT)
	do_argfile(eap, ARGCOUNT - 1);
    else
	do_argfile(eap, curwin->w_arg_idx - (int)eap->line2);
}

/*
 * ":rewind", ":first", ":sfirst" and ":srewind".
 */
    void
ex_rewind(eap)
    exarg_T	*eap;
{
    do_argfile(eap, 0);
}

/*
 * ":last" and ":slast".
 */
    void
ex_last(eap)
    exarg_T	*eap;
{
    do_argfile(eap, ARGCOUNT - 1);
}

/*
 * ":argument" and ":sargument".
 */
    void
ex_argument(eap)
    exarg_T	*eap;
{
    int		i;

    if (eap->addr_count > 0)
	i = eap->line2 - 1;
    else
	i = curwin->w_arg_idx;
    do_argfile(eap, i);
}

/*
 * Edit file "argn" of the argument lists.
 */
    void
do_argfile(eap, argn)
    exarg_T	*eap;
    int		argn;
{
    int		other;
    char_u	*p;

    if (argn < 0 || argn >= ARGCOUNT)
    {
	if (ARGCOUNT <= 1)
	    EMSG(_("E163: There is only one file to edit"));
	else if (argn < 0)
	    EMSG(_("E164: Cannot go before first file"));
	else
	    EMSG(_("E165: Cannot go beyond last file"));
    }
    else
    {
	setpcmark();
#ifdef FEAT_GUI
	need_mouse_correct = TRUE;
#endif

#ifdef FEAT_WINDOWS
	if (*eap->cmd == 's')	    /* split window first */
	{
	    if (win_split(0, 0) == FAIL)
		return;
	}
	else
#endif
	{
	    /*
	     * if 'hidden' set, only check for changed file when re-editing
	     * the same buffer
	     */
	    other = TRUE;
	    if (P_HID(curbuf))
	    {
		p = fix_fname(alist_name(&ARGLIST[argn]));
		other = otherfile(p);
		vim_free(p);
	    }
	    if ((!P_HID(curbuf) || !other)
		  && check_changed(curbuf, TRUE, !other, eap->forceit, FALSE))
		return;
	}

	curwin->w_arg_idx = argn;
	if (argn == ARGCOUNT - 1
#ifdef FEAT_WINDOWS
		&& curwin->w_alist == &global_alist
#endif
	   )
	    arg_had_last = TRUE;

	/* Edit the file; always use the last known line number. */
	(void)do_ecmd(0, alist_name(&ARGLIST[curwin->w_arg_idx]), NULL,
		      eap, ECMD_LAST,
		      (P_HID(curwin->w_buffer) ? ECMD_HIDE : 0) +
					   (eap->forceit ? ECMD_FORCEIT : 0));
    }
}

/*
 * ":next", and commands that behave like it.
 */
    void
ex_next(eap)
    exarg_T	*eap;
{
    int		i;

    /*
     * check for changed buffer now, if this fails the argument list is not
     * redefined.
     */
    if (       P_HID(curbuf)
	    || eap->cmdidx == CMD_snext
	    || !check_changed(curbuf, TRUE, FALSE, eap->forceit, FALSE))
    {
	if (*eap->arg != NUL)		    /* redefine file list */
	{
	    if (do_arglist(eap->arg, AL_SET, 0) == FAIL)
		return;
	    i = 0;
	}
	else
	    i = curwin->w_arg_idx + (int)eap->line2;
	do_argfile(eap, i);
    }
}

#ifdef FEAT_LISTCMDS
/*
 * ":argedit"
 */
    void
ex_argedit(eap)
    exarg_T	*eap;
{
    int		fnum;
    int		i;
    char_u	*s;

    /* Add the argument to the buffer list and get the buffer number. */
    fnum = buflist_add(eap->arg, BLN_LISTED);

    /* Check if this argument is already in the argument list. */
    for (i = 0; i < ARGCOUNT; ++i)
	if (ARGLIST[i].ae_fnum == fnum)
	    break;
    if (i == ARGCOUNT)
    {
	/* Can't find it, add it to the argument list. */
	s = vim_strsave(eap->arg);
	if (s == NULL)
	    return;
	i = alist_add_list(1, &s,
	       eap->addr_count > 0 ? (int)eap->line2 : curwin->w_arg_idx + 1);
	if (i < 0)
	    return;
	curwin->w_arg_idx = i;
    }

    alist_check_arg_idx();

    /* Edit the argument. */
    do_argfile(eap, i);
}

/*
 * ":argadd"
 */
    void
ex_argadd(eap)
    exarg_T	*eap;
{
    do_arglist(eap->arg, AL_ADD,
	       eap->addr_count > 0 ? (int)eap->line2 : curwin->w_arg_idx + 1);
#ifdef FEAT_TITLE
    maketitle();
#endif
}

/*
 * ":argdelete"
 */
    void
ex_argdelete(eap)
    exarg_T	*eap;
{
    int		i;
    int		n;

    if (eap->addr_count > 0)
    {
	/* ":1,4argdel": Delete all arguments in the range. */
	if (eap->line2 > ARGCOUNT)
	    eap->line2 = ARGCOUNT;
	n = eap->line2 - eap->line1 + 1;
	if (*eap->arg != NUL || n <= 0)
	    EMSG(_(e_invarg));
	else
	{
	    for (i = eap->line1; i <= eap->line2; ++i)
		vim_free(ARGLIST[i - 1].ae_fname);
	    mch_memmove(ARGLIST + eap->line1 - 1, ARGLIST + eap->line2,
			(size_t)((ARGCOUNT - eap->line2) * sizeof(aentry_T)));
	    ALIST(curwin)->al_ga.ga_len -= n;
	    ALIST(curwin)->al_ga.ga_room += n;
	    if (curwin->w_arg_idx >= eap->line2)
		curwin->w_arg_idx -= n;
	    else if (curwin->w_arg_idx > eap->line1)
		curwin->w_arg_idx = eap->line1;
	}
    }
    else if (*eap->arg == NUL)
	EMSG(_(e_argreq));
    else
	do_arglist(eap->arg, AL_DEL, 0);
#ifdef FEAT_TITLE
    maketitle();
#endif
}

/*
 * ":argdo", ":windo", ":bufdo"
 */
    void
ex_listdo(eap)
    exarg_T	*eap;
{
    int		i;
#ifdef FEAT_WINDOWS
    win_T	*win;
#endif
    buf_T	*buf;
#if defined(FEAT_AUTOCMD) && defined(FEAT_SYN_HL)
    char_u	*save_ei = vim_strsave(p_ei);
    char_u	*new_ei;
#endif

#ifndef FEAT_WINDOWS
    if (eap->cmdidx == CMD_windo)
    {
	ex_ni(eap);
	return;
    }
#endif

#if defined(FEAT_AUTOCMD) && defined(FEAT_SYN_HL)
    new_ei = vim_strnsave(p_ei, (int)STRLEN(p_ei) + 8);
    if (new_ei != NULL)
    {
	STRCAT(new_ei, ",Syntax");
	set_string_option_direct((char_u *)"ei", -1, new_ei, OPT_FREE);
	vim_free(new_ei);
    }
#endif

    if (eap->cmdidx == CMD_windo
	    || P_HID(curbuf)
	    || !check_changed(curbuf, TRUE, FALSE, eap->forceit, FALSE))
    {
	/* start at the first argument/window/buffer */
	i = 0;
#ifdef FEAT_WINDOWS
	win = firstwin;
#endif
	if (eap->cmdidx == CMD_bufdo)
	    goto_buffer(eap, DOBUF_FIRST, FORWARD, 0);
	while (!got_int)
	{
	    if (eap->cmdidx == CMD_argdo)
	    {
		/* go to argument "i" */
		if (i == ARGCOUNT)
		    break;
		do_argfile(eap, i);
		if (curwin->w_arg_idx != i)
		    break;
		++i;
	    }
#ifdef FEAT_WINDOWS
	    else if (eap->cmdidx == CMD_windo)
	    {
		/* go to window "win" */
		if (!win_valid(win))
		    break;
		win_goto(win);
		win = win->w_next;
	    }
#endif

	    /* execute the command */
	    do_cmdline(eap->arg, eap->getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    if (eap->cmdidx == CMD_bufdo)
	    {
		/* go to the next buffer */
		buf = curbuf->b_next;
		if (!buf_valid(buf))
		    break;
		goto_buffer(eap, DOBUF_CURRENT, FORWARD, 1);
		if (curbuf != buf)
		    break;
	    }
	}
    }
#if defined(FEAT_AUTOCMD) && defined(FEAT_SYN_HL)
    if (new_ei != NULL)
    {
	set_string_option_direct((char_u *)"ei", -1, save_ei, OPT_FREE);
	apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					     curbuf->b_fname, TRUE, curbuf);
    }
    vim_free(save_ei);
#endif
}

/*
 * Add files[count] to the arglist of the current window after arg "after".
 * The file names in files[count] must have been allocated and are taken over.
 * Files[] itself is not taken over.
 * Returns index of first added argument.  Returns -1 when failed (out of mem).
 */
    static int
alist_add_list(count, files, after)
    int		count;
    char_u	**files;
    int		after;	    /* where to add: 0 = before first one */
{
    int		i;

    if (ga_grow(&ALIST(curwin)->al_ga, count) == OK)
    {
	if (after < 0)
	    after = 0;
	if (after > ARGCOUNT)
	    after = ARGCOUNT;
	if (after < ARGCOUNT)
	    mch_memmove(&(ARGLIST[after + count]), &(ARGLIST[after]),
				       (ARGCOUNT - after) * sizeof(aentry_T));
	for (i = 0; i < count; ++i)
	{
	    ARGLIST[after + i].ae_fname = files[i];
	    ARGLIST[after + i].ae_fnum = buflist_add(files[i], BLN_LISTED);
	}
	ALIST(curwin)->al_ga.ga_len += count;
	ALIST(curwin)->al_ga.ga_room -= count;
	if (curwin->w_arg_idx >= after)
	    ++curwin->w_arg_idx;
	return after;
    }

    for (i = 0; i < count; ++i)
	vim_free(files[i]);
    return -1;
}

#endif /* FEAT_LISTCMDS */

/*
 * ":source" and associated commands.
 */
/*
 * Structure used to store info for each sourced file.
 * It is shared between do_source() and getsourceline().
 * This is required, because it needs to be handed to do_cmdline() and
 * sourcing can be done recursively.
 */
struct source_cookie
{
    FILE	*fp;		/* opened file for sourcing */
    char_u      *nextline;      /* if not NULL: line that was read ahead */
    int		finished;	/* ":finish" used */
#if defined (USE_CRNL) || defined (USE_CR)
    int		fileformat;	/* EOL_UNKNOWN, EOL_UNIX or EOL_DOS */
    int		error;		/* TRUE if LF found after CR-LF */
#endif
#ifdef FEAT_EVAL
    linenr_T	breakpoint;	/* next line with breakpoint or zero */
    char_u	*fname;		/* name of sourced file */
    int		dbg_tick;	/* debug_tick when breakpoint was set */
#endif
#ifdef FEAT_MBYTE
    vimconv_T	conv;		/* type of conversion */
#endif
};

static char_u *get_one_sourceline __ARGS((struct source_cookie *sp));

#ifdef FEAT_EVAL
/* Growarray to store the names of sourced scripts. */
static garray_T script_names = {0, 0, sizeof(char_u *), 4, NULL};
#define SCRIPT_NAME(id) (((char_u **)script_names.ga_data)[(id) - 1])
#endif

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
    linenr_T		    save_sourcing_lnum;
    char_u		    *p;
    char_u		    *fname_exp;
    int			    retval = FAIL;
#ifdef FEAT_EVAL
    scid_T		    save_current_SID;
    static scid_T	    last_current_SID = 0;
    void		    *save_funccalp;
    int			    save_debug_break_level = debug_break_level;
#endif

#ifdef RISCOS
    fname_exp = mch_munge_fname(fname);
#else
    fname_exp = expand_env_save(fname);
#endif
    if (fname_exp == NULL)
	goto theend;
#ifdef MACOS_CLASSIC
    slash_n_colon_adjust(fname_exp);
#endif
    if (mch_isdir(fname_exp))
    {
	smsg((char_u *)_("Cannot source a directory: \"%s\""), fname);
	goto theend;
    }

    cookie.fp = mch_fopen((char *)fname_exp, READBIN);
    if (cookie.fp == NULL && check_other)
    {
	/*
	 * Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	 * and ".exrc" by "_exrc" or vice versa.
	 */
	p = gettail(fname_exp);
	if ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    if (*p == '_')
		*p = '.';
	    else
		*p = '_';
	    cookie.fp = mch_fopen((char *)fname_exp, READBIN);
	}
    }

    if (cookie.fp == NULL)
    {
	if (p_verbose > 0)
	{
	    if (sourcing_name == NULL)
		smsg((char_u *)_("could not source \"%s\""), fname);
	    else
		smsg((char_u *)_("line %ld: could not source \"%s\""),
			sourcing_lnum, fname);
	}
	goto theend;
    }

    /*
     * The file exists.
     * - In verbose mode, give a message.
     * - For a vimrc file, may want to set 'compatible', call vimrc_found().
     */
    if (p_verbose > 1)
    {
	if (sourcing_name == NULL)
	    smsg((char_u *)_("sourcing \"%s\""), fname);
	else
	    smsg((char_u *)_("line %ld: sourcing \"%s\""),
		    sourcing_lnum, fname);
    }
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

#ifdef USE_CR
    /* If no automatic file format: Set default to CR. */
    if (*p_ffs == NUL)
	cookie.fileformat = EOL_MAC;
    else
	cookie.fileformat = EOL_UNKNOWN;
    cookie.error = FALSE;
#endif

    cookie.nextline = NULL;
    cookie.finished = FALSE;

#ifdef FEAT_EVAL
    /*
     * Check if this script has a breakpoint.
     */
    cookie.breakpoint = dbg_find_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;
#endif
#ifdef FEAT_MBYTE
    cookie.conv.vc_type = CONV_NONE;		/* no conversion */
# ifdef USE_ICONV
    cookie.conv.vc_fd = (iconv_t)-1;
# endif
#endif

    /*
     * Keep the sourcing name/lnum, for recursive calls.
     */
    save_sourcing_name = sourcing_name;
    sourcing_name = fname_exp;
    save_sourcing_lnum = sourcing_lnum;
    sourcing_lnum = 0;

#ifdef FEAT_EVAL
    /*
     * Check if this script was sourced before to finds its SID.
     * If it's new, generate a new SID.
     */
    save_current_SID = current_SID;
    for (current_SID = script_names.ga_len; current_SID > 0; --current_SID)
	if (SCRIPT_NAME(current_SID) != NULL
		&& fnamecmp(SCRIPT_NAME(current_SID), fname_exp) == 0)
	    break;
    if (current_SID == 0)
    {
	current_SID = ++last_current_SID;
	if (ga_grow(&script_names, (int)(current_SID - script_names.ga_len))
									== OK)
	{
	    while (script_names.ga_len < current_SID)
	    {
		SCRIPT_NAME(script_names.ga_len + 1) = NULL;
		++script_names.ga_len;
		--script_names.ga_room;
	    }
	    SCRIPT_NAME(current_SID) = fname_exp;
	    fname_exp = NULL;
	}
	/* Allocate the local script variables to use for this script. */
	new_script_vars(current_SID);
    }

    /* Don't use local function variables, if called from a function */
    save_funccalp = save_funccal();
#endif

    /*
     * Call do_cmdline, which will call getsourceline() to get the lines.
     */
    do_cmdline(NULL, getsourceline, (void *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);

    retval = OK;
    fclose(cookie.fp);
    vim_free(cookie.nextline);
    if (got_int)
	EMSG(_(e_interr));
    sourcing_name = save_sourcing_name;
    sourcing_lnum = save_sourcing_lnum;
#ifdef FEAT_EVAL
    current_SID = save_current_SID;
    restore_funccal(save_funccalp);
#endif
    if (p_verbose > 1)
    {
	smsg((char_u *)_("finished sourcing %s"), fname);
	if (sourcing_name != NULL)
	    smsg((char_u *)_("continuing in %s"), sourcing_name);
    }
#ifdef STARTUPTIME
    sprintf(IObuff, "sourcing %s", fname);
    TIME_MSG(IObuff);
#endif

#ifdef FEAT_EVAL
    /*
     * After a "finish" in debug mode, need to break at first command of next
     * sourced file.
     */
    if (save_debug_break_level > debug_level
	    && debug_break_level == debug_level)
	++debug_break_level;
#endif

theend:
    vim_free(fname_exp);
    return retval;
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * ":scriptnames"
 */
/*ARGSUSED*/
    void
ex_scriptnames(eap)
    exarg_T	*eap;
{
    int i;

    for (i = 1; i <= script_names.ga_len && !got_int; ++i)
	if (SCRIPT_NAME(i) != NULL)
	    smsg((char_u *)"%3d: %s", i, SCRIPT_NAME(i));
}

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 */
    char_u *
get_scriptname(id)
    scid_T	id;
{
    if (id == SID_MODELINE)
	return (char_u *)"modeline";
    return SCRIPT_NAME(id);
}
#endif

#if defined(USE_CR) || defined(PROTO)
/*
 * Version of fgets() which also works for lines ending in a <CR> only
 * (Macintosh format).
 */
    char *
fgets_cr(s, n, stream)
    char	*s;
    int		n;
    FILE	*stream;
{
    int	c = 0;
    int char_read = 0;

    while (!feof(stream) && c != '\r' && c != '\n' && char_read < n - 1)
    {
	c = fgetc(stream);
	s[char_read++] = c;
	/* If the file is in DOS format, we need to skip a NL after a CR.  I
	 * thought it was the other way around, but this appears to work... */
	if (c == '\n')
	{
	    c = fgetc(stream);
	    if (c != '\r')
		ungetc(c, stream);
	}
    }

    s[char_read] = 0;
    if (char_read == 0)
	return NULL;

    if (feof(stream) && char_read == 1)
	return NULL;

    return s;
}
#endif

/*
 * Get one full line from a sourced file.
 * Called by do_cmdline() when it's called from do_source().
 *
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
/* ARGSUSED */
    char_u *
getsourceline(c, cookie, indent)
    int		c;		/* not used */
    void	*cookie;
    int		indent;		/* not used */
{
    struct source_cookie *sp = (struct source_cookie *)cookie;
    char_u		*line;
    char_u		*p, *s;

#ifdef FEAT_EVAL
    /* If breakpoints have been added/deleted need to check for it. */
    if (sp->dbg_tick < debug_tick)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, sourcing_lnum);
	sp->dbg_tick = debug_tick;
    }
#endif
    /*
     * Get current line.  If there is a read-ahead line, use it, otherwise get
     * one now.
     */
    if (sp->finished)
	line = NULL;
    else if (sp->nextline == NULL)
	line = get_one_sourceline(sp);
    else
    {
	line = sp->nextline;
	sp->nextline = NULL;
	++sourcing_lnum;
    }

    /* Only concatenate lines starting with a \ when 'cpoptions' doesn't
     * contain the 'C' flag. */
    if (line != NULL && (vim_strchr(p_cpo, CPO_CONCAT) == NULL))
    {
	/* compensate for the one line read-ahead */
	--sourcing_lnum;
	for (;;)
	{
	    sp->nextline = get_one_sourceline(sp);
	    if (sp->nextline == NULL)
		break;
	    p = skipwhite(sp->nextline);
	    if (*p != '\\')
		break;
	    s = alloc((int)(STRLEN(line) + STRLEN(p)));
	    if (s == NULL)	/* out of memory */
		break;
	    STRCPY(s, line);
	    STRCAT(s, p + 1);
	    vim_free(line);
	    line = s;
	    vim_free(sp->nextline);
	}
    }

#ifdef FEAT_MBYTE
    if (line != NULL && sp->conv.vc_type != CONV_NONE)
    {
	/* Convert the encoding of the script line. */
	s = string_convert(&sp->conv, line, NULL);
	if (s != NULL)
	{
	    vim_free(line);
	    line = s;
	}
    }
#endif

#ifdef FEAT_EVAL
    /* Did we encounter a breakpoint? */
    if (sp->breakpoint != 0 && sp->breakpoint <= sourcing_lnum)
    {
	dbg_breakpoint(sp->fname, sourcing_lnum);
	/* Find next breakpoint. */
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, sourcing_lnum);
	sp->dbg_tick = debug_tick;
    }
#endif

    return line;
}

    static char_u *
get_one_sourceline(sp)
    struct source_cookie    *sp;
{
    garray_T		ga;
    int			len;
    int			c;
    char_u		*buf;
#ifdef USE_CRNL
    int			has_cr;		/* CR-LF found */
#endif
#ifdef USE_CR
    char_u		*scan;
#endif
    int			have_read = FALSE;

    /* use a growarray to store the sourced line */
    ga_init2(&ga, 1, 200);

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

#ifdef USE_CR
	if (sp->fileformat == EOL_MAC)
	{
	    if (fgets_cr((char *)buf + ga.ga_len, ga.ga_room, sp->fp) == NULL
		    || got_int)
		break;
	}
	else
#endif
	    if (fgets((char *)buf + ga.ga_len, ga.ga_room, sp->fp) == NULL
		    || got_int)
		break;
	len = (int)STRLEN(buf);
#ifdef USE_CRNL
	/* Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	 * CTRL-Z by its own, or after a NL. */
	if (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    break;
	}
#endif

#ifdef USE_CR
	/* If the read doesn't stop on a new line, and there's
	 * some CR then we assume a Mac format */
	if (sp->fileformat == EOL_UNKNOWN)
	{
	    if (buf[len - 1] != '\n' && vim_strchr(buf, '\r') != NULL)
		sp->fileformat = EOL_MAC;
	    else
		sp->fileformat = EOL_UNIX;
	}

	if (sp->fileformat == EOL_MAC)
	{
	    scan = vim_strchr(buf, '\r');

	    if (scan != NULL)
	    {
		*scan = '\n';
		if (*(scan + 1) != 0)
		{
		    *(scan + 1) = 0;
		    fseek(sp->fp, (long)(scan - buf - len + 1), SEEK_CUR);
		}
	    }
	    len = STRLEN(buf);
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
			EMSG(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#endif
	    /* The '\n' is escaped if there is an odd number of ^V's just
	     * before it, first set "c" just before the 'V's and then check
	     * len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo */
	    for (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
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
 * ":scriptencoding": Set encoding conversion for a sourced script.
 * Without the multi-byte feature it's simply ignored.
 */
/*ARGSUSED*/
    void
ex_scriptencoding(eap)
    exarg_T	*eap;
{
#ifdef FEAT_MBYTE
    struct source_cookie	*sp;
    char_u			*name;

    if (eap->getline != getsourceline)
    {
	EMSG(_("E167: :scriptencoding used outside of a sourced file"));
	return;
    }

    if (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	if (name == NULL)	/* out of memory */
	    return;
    }
    else
	name = eap->arg;

    /* Setup for conversion from the specified encoding to 'encoding'. */
    sp = (struct source_cookie *)eap->cookie;
    convert_setup(&sp->conv, name, p_enc);

    if (name != eap->arg)
	vim_free(name);
#endif
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * ":finish": Mark a sourced file as finished.
 */
    void
ex_finish(eap)
    exarg_T	*eap;
{
    if (eap->getline == getsourceline)
	((struct source_cookie *)eap->cookie)->finished = TRUE;
    else
	EMSG(_("E168: :finish used outside of a sourced file"));
}

/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 */
    int
source_finished(cookie)
    void	*cookie;
{
    return ((struct source_cookie *)cookie)->finished == TRUE;
}
#endif

#if defined(FEAT_LISTCMDS) || defined(PROTO)
/*
 * ":checktime [buffer]"
 */
    void
ex_checktime(eap)
    exarg_T	*eap;
{
    buf_T	*buf;

    if (eap->addr_count == 0)	/* default is all buffers */
	check_timestamps(FALSE);
    else
    {
	buf = buflist_findnr((int)eap->line2);
	if (buf != NULL)	/* cannot happen? */
	    (void)buf_check_timestamp(buf, FALSE);
    }
}
#endif

#if defined(FEAT_PRINTER) || defined(PROTO)
/*
 * Printing code (Machine-independent.)
 * To implement printing on a platform, the following functions must be
 * defined:
 *
 * int mch_print_init(prt_settings_T *settings, char_u *jobname, int forceit)
 * Called once. Code should display printer dialogue (if appropriate) and
 * determine printer font and margin settings.
 * Return FALSE to abort.
 *
 * int mch_print_begin(prt_settings_T *settings)
 * Called once. Start print job.
 * Return FALSE to abort.
 *
 * int mch_print_begin_page()
 * Called at the start of each page. If you want to do a header, do it
 * here.
 * Return FALSE to abort.
 *
 * int mch_print_end_page()
 * Called at the end of each page. Do a footer here!
 * Return FALSE to abort.
 *
 * void mch_print_end()
 * Called at end of print job
 *
 * void mch_print_cleanup()
 * Called if print job is abandoned. Free any memory, close devices and
 * handles.
 *
 * void mch_print_setfont(int iBold, int iItalic, int iUnderline);
 * Called whenever the font style changes.
 *
 * void mch_print_set_bg(long bgcol);
 * Called whenever the background color changes. Parameter is an RGB
 * value.
 *
 * void mch_print_set_fg(long fgcol);
 * Called whenever the foreground color changes. Parameter is an RGB
 * value.
 *
 * int mch_print_text_out(int x, int y, char_u *p, int len, int *must_break);
 * Output text 'p' of 'len' characters at position x,y in device units.
 * set 'must_break' to TRUE if there is not room for another character after
 * the string printed. Return the width of the string 'p' printed in device
 * units.
 * x = 0 is the left margin.
 *
 * Note that the generic code has no idea of margins. The machine code should
 * simply make the page look smaller!
 *
 */

#ifdef FEAT_SYN_HL
static const unsigned long  cterm_color_8[8] =
{
    0x808080UL, 0x6060ffUL, 0x00ff00UL, 0x00ffffUL,
    0xff8080UL, 0xff40ffUL, 0xffff00UL, 0xffffffUL
};

static const unsigned long  cterm_color_16[16] =
{
    0x000000UL, 0x0000c0UL, 0x008000UL, 0x004080UL,
    0xc00000UL, 0xc000c0UL, 0x808000UL, 0xc0c0c0UL,
    0x808080UL, 0x6060ffUL, 0x00ff00UL, 0x00ffffUL,
    0xff8080UL, 0xff40ffUL, 0xffff00UL, 0xffffffUL
};

# ifdef FEAT_GUI
static int		curr_italic;
static int		curr_bold;
static int		curr_underline;
# endif
static unsigned long	current_bg;
static unsigned long	current_fg;
static int		current_id;
#endif
static int		page_count;

static unsigned long darken_rgb __ARGS((unsigned long rgb));
static unsigned long prt_get_term_color __ARGS((int colorindex));
static void ex_print_number __ARGS((int offset, int y_pos, linenr_T lnum, int format, int wrapped));
static void ex_print_header __ARGS((int offset, int width, int pagenum, linenr_T lnum, int format));

/*
 * If using a dark background, the colors will probably be too bright to show
 * up well on white paper, so reduce their brightness.
 */
    static unsigned long
darken_rgb(rgb)
    unsigned long	rgb;
{
    return	((rgb >> 17) << 16)
	    +	(((rgb & 0xff00) >> 9) << 8)
	    +	((rgb & 0xff) >> 1);
}

    static unsigned long
prt_get_term_color(colorindex)
    int	    colorindex;
{
    /* TODO: Should check for xterm with 88 or 256 colors. */
    if (t_colors > 8)
	return cterm_color_16[colorindex % 16];
    return cterm_color_8[colorindex % 8];
}

    static void
ex_print_number(offset, y_pos, lnum, format, wrapped)
    int		offset;
    int		y_pos;
    linenr_T	lnum;
    int		format;
    int		wrapped;
{
    /*
     * A simple header for demos
     */
    int	x_pos = 0 - (offset);
    char_u tbuf[8];
    int needbreak;

    if (!wrapped)
	sprintf((char *)tbuf, "%6ld", (long)lnum);
    else
	STRCPY(tbuf, "      ");

#ifdef FEAT_SYN_HL
    if (format)
    {
	current_fg = 0x808080UL;
	current_bg = 0xFFFFFFUL;
	mch_print_set_fg(current_fg);
	mch_print_set_bg(current_bg);

# ifdef FEAT_GUI
	curr_underline = FALSE;
	curr_italic = TRUE;
	curr_bold = FALSE;
	mch_print_setfont(FALSE, TRUE, FALSE);
# endif
	current_id = -1;
    }
#endif /*FEAT_SYN_HL*/

    x_pos += mch_print_text_out(x_pos, y_pos,
	    tbuf,
	    6,
	    &needbreak);
}

    int
get_printer_page_num()
{
    return page_count + 1;
}

/*ARGSUSED*/
    static void
ex_print_header(offset, width, pagenum, lnum, format)
    int		offset;
    int		width;
    int		pagenum;
    linenr_T	lnum;
    int		format;
{
    /*
     * A simple header for demos
     */
    int		needbreak;
    int		x_pos = 0;
    int		y_pos = 0 - (offset
			       * printer_opts[OPT_PRINT_HEADERHEIGHT].number);
    char_u	*tbuf;
    char_u	*p;
#ifdef FEAT_STL_OPT
    linenr_T	tmp_lnum, tmp_topline, tmp_botline;
#endif
#ifdef FEAT_MBYTE
    int		l;
#endif

    p = tbuf = alloc(width + 1);

    if (p == NULL)
	return;

#ifdef FEAT_STL_OPT
    /*
     * Need to (temporarily) set current line number and first/last line
     * number on the 'window'.  Since we don't know how long the page is, set
     * the first and current line number to the top line, and guess that the
     * page length is 64.
     */
    tmp_lnum = curwin->w_cursor.lnum;
    tmp_topline = curwin->w_topline;
    tmp_botline = curwin->w_botline;
    curwin->w_cursor.lnum = lnum;
    curwin->w_topline = lnum;
    curwin->w_botline = lnum + 63;

    build_stl_str_hl(curwin, p, p_headerfmt, ' ', width, NULL);

    /* Reset line numbers */
    curwin->w_cursor.lnum = tmp_lnum;
    curwin->w_topline = tmp_topline;
    curwin->w_botline = tmp_botline;
#else
    sprintf((char *)tbuf, "Page %d", pagenum);
#endif


#ifdef FEAT_SYN_HL
    if (format)
    {
	current_fg = 0;
	current_bg = 0xFFFFFFUL;
	mch_print_set_fg(current_fg);
	mch_print_set_bg(current_bg);

#ifdef FEAT_GUI
	curr_underline = FALSE;
	curr_italic = FALSE;
	curr_bold = TRUE;
	mch_print_setfont(TRUE, FALSE, FALSE);
#endif
	current_id = -1;
    }
#endif /*FEAT_SYN_HL*/

    /*
     * We can print to a -ve offset since the top margin
     * has been moved down 2 lines from the printable area
     * extent
     */
    while (*p != NUL)
    {
	x_pos += mch_print_text_out(x_pos, y_pos,
		p,
#ifdef FEAT_MBYTE
		(l = (*mb_ptr2len_check)(p)),
#else
		1,
#endif
		&needbreak);
	if (needbreak)
	{
	    y_pos += offset;
	    x_pos = 0;

	    if (y_pos >= 0) /* out of room in header */
		break;
	}
#ifdef FEAT_MBYTE
	p += l;
#else
	p++;
#endif
    }

    vim_free(tbuf);
}

static colnr_T hardcopy_line __ARGS((colnr_T column, linenr_T file_line, int page_line, int mono, prt_settings_T *settingsp, int *lead_spaces, int wrapped));

    void
ex_hardcopy(eap)
    exarg_T	*eap;
{
    linenr_T		lnum;
    int			collated_copies, uncollated_copies;
    prt_settings_T	settings;
    unsigned long	bytes_to_print = 0;
    int			page_line;
    int			wrapped = FALSE;
#ifdef FEAT_SYN_HL
    int			mono = FALSE;
#endif

#ifdef FEAT_SYN_HL
    if (printer_opts[OPT_PRINT_MONO].present
	    && printer_opts[OPT_PRINT_MONO].string[0] == 'y')
	mono = TRUE;
#endif

    /*
     * Initialise printer. The mch_ code should set up margins
     * if applicable. (It may not be a real printer - for example
     * the engine might generate HTML or PS.)
     */
    if (!mch_print_init(&settings,
			curbuf->b_fname == NULL
			    ? (char_u *)buf_spname(curbuf)
			    : curbuf->b_sfname == NULL
				? curbuf->b_fname
				: curbuf->b_sfname,
			eap->forceit))
	return;

    /*
     * Estimate the total lines to be printed
     */
    for (lnum = eap->line1; lnum <= eap->line2; lnum++)
	bytes_to_print += (unsigned long)STRLEN(ml_get(lnum));

#ifdef FEAT_SYN_HL
# ifdef FEAT_GUI
    curr_italic = FALSE;
    curr_bold = FALSE;
    curr_underline = FALSE;
# endif
    current_bg = 0xffffffffUL;
    current_fg = 0xffffffffUL;
    current_id = -1;
#endif

    if (!mch_print_begin(&settings))
	goto print_fail_no_begin;

    /*
     * Loop over collated copies: 1 2 3, 1 2 3, ...
     */
    for (collated_copies = 0;
	    collated_copies < settings.n_collated_copies;
	    collated_copies++)
    {
	int		lead_spaces;
	int		prev_lead_spaces = 0;
	colnr_T		column = 0;
	colnr_T		prev_column = 0;
	linenr_T	file_line = 0;
	linenr_T	prev_file_line = eap->line1;
	unsigned long	bytes_printed;
	unsigned long	prev_bytes_printed = 0;

	bytes_printed = 0;

	/*
	 * Loop over all pages in the print job: 1 2 3 ...
	 */
	for (page_count = 0; file_line <= eap->line2; ++page_count)
	{
	    /*
	     * Loop over uncollated copies: 1 1 1, 2 2 2, 3 3 3, ...
	     */
	    for (uncollated_copies = 0;
		    uncollated_copies < settings.n_uncollated_copies;
		    uncollated_copies++)
	    {
		/*
		 * Print one page.
		 */
		lead_spaces = prev_lead_spaces;
		column = prev_column;
		file_line = prev_file_line;
		bytes_printed = prev_bytes_printed;

		/* Check for interrupt character every page. */
		ui_breakcheck();
		if (got_int || !mch_print_begin_page())
		    goto print_fail;

		sprintf((char *)IObuff, _("Printing page %d (%d%%)"),
			page_count + 1,
			(int)((bytes_printed * 100) / bytes_to_print));
		if (settings.n_collated_copies > 1)
		    sprintf((char *)IObuff + STRLEN(IObuff),
			    _(" Copy %d of %d"),
			    collated_copies + 1,
			    settings.n_collated_copies);
		screen_fill((int)Rows - 1, (int)Rows, 0, (int)Columns,
								 ' ', ' ', 0);
		screen_puts(IObuff, (int)Rows - 1, 0, hl_attr(HLF_R));
		out_flush();

		/*
		 * Output header if required
		 */
		if (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
		    ex_print_header(settings.line_height,
					settings.chars_per_line,
					page_count + 1,
					file_line,
					!mono);

		for (page_line = 0 ; page_line < settings.lines_per_page;
								  ++page_line)
		{
		    column = hardcopy_line(column, file_line, page_line,
					       mono, &settings, &lead_spaces,
					       wrapped);
		    if (column == 0)
		    {
			/* finished a file line */
			wrapped = FALSE;
			bytes_printed += STRLEN(ml_get(file_line));
			++file_line;
			if (file_line > eap->line2)
			    break; /* reached the end */
		    }
		    else
			wrapped = TRUE;
		}

		if (!mch_print_end_page())
		    goto print_fail;
	    }

	    prev_lead_spaces = lead_spaces;
	    prev_column = column;
	    prev_file_line = file_line;
	    prev_bytes_printed = bytes_printed;
	}

	sprintf((char *)IObuff, _("Printed: %s"), settings.jobname);
	screen_fill((int)Rows - 1, (int)Rows, 0, (int)Columns, ' ', ' ', 0);
	screen_puts(IObuff, (int)Rows - 1, 0, hl_attr(HLF_R));
	out_flush();
    }

print_fail:
    mch_print_end();

print_fail_no_begin:
    mch_print_cleanup();
}

/*
 * Print one page line.
 * Return the next column to print, or zero if the line is finished.
 */
    static colnr_T
hardcopy_line(column, file_line, page_line, mono, settingsp, lead_spaces, wrapped)
    colnr_T		column;
    linenr_T		file_line;
    int			page_line;
    int			mono;
    prt_settings_T	*settingsp;
    int			*lead_spaces;
    int			wrapped;
{
    char_u		*p;
    char_u		*start_line;
    int			x_position = 0;
    int			need_break = FALSE;
    int			outputlen;
    int			colorindex;
    char		*color;
    int			id;
    int			tab_spaces;
    unsigned long	print_pos = 0;
    unsigned long	this_color;
#ifdef FEAT_SYN_HL
    char		modec = NUL;
#endif

    start_line = p = ml_get(file_line);
    p += column;
    print_pos = 0;
    if (column == 0)
	tab_spaces = 0;
    else
	tab_spaces = *lead_spaces;   /* left over from wrap halfway a tab */

#ifdef FEAT_SYN_HL
# ifdef  FEAT_GUI
    if (!gui.in_use)
# endif
    {
	if (t_colors > 1)
	    modec = 'c';
	else
	    modec = 't';
    }
#endif

    if (printer_opts[OPT_PRINT_NUMBER].present)
	ex_print_number(settingsp->number_width,
			settingsp->line_height * page_line,
			file_line,
			!mono,
			wrapped);

    /*
     * Loop over the columns until the end of the file line or right margin.
     */
    for ( ; *p != NUL && !need_break; p++)
    {
	outputlen = 1;
#ifdef FEAT_MBYTE
	if (has_mbyte && (outputlen = (*mb_ptr2len_check)(p)) < 1)
	    outputlen = 1;
#endif
#ifdef FEAT_SYN_HL
	/*
	 * syntax highlighting stuff.
	 */
	if (!mono)
	{
	    id = syn_get_id(file_line, (long)(p - start_line), 1);
	    if (id > 0)
		id = syn_get_final_id(id);
	    else
		id = 0;

	    if (id != current_id)
	    {
		current_id = id;
#ifdef	FEAT_GUI
		if (gui.in_use)
		{
		    int ifItalic = (highlight_has_attr(id,
						     HL_ITALIC, 'g')) ? 1 : 0;
		    int ifBold = (highlight_has_attr(id,
						       HL_BOLD, 'g')) ? 1 : 0;
		    int ifUnderline = (highlight_has_attr(id,
						  HL_UNDERLINE, 'g')) ? 1 : 0;
		    if (curr_underline != ifUnderline
			    || curr_bold != ifBold
			    || curr_italic != ifItalic)
		    {
			curr_underline = ifUnderline;
			curr_italic = ifItalic;
			curr_bold = ifBold;
			mch_print_setfont(curr_bold, curr_italic,
							      curr_underline);
		    }

		    this_color = highlight_gui_color_rgb(id, FALSE);
		    if (this_color == 0)
			this_color = 0xffffffUL;

		    if (this_color != current_bg)
		    {
			current_bg = this_color;
			mch_print_set_bg(this_color);
		    }

		    this_color = highlight_gui_color_rgb(id, TRUE);
		    if (this_color == 0xffffffUL)
			this_color = 0;
		    else if (*p_bg == 'd')
			this_color = darken_rgb(this_color);

		    if (this_color != current_fg)
		    {
			current_fg = this_color;
			mch_print_set_fg(this_color);
		    }
		}
		else
#endif /* FEAT_GUI */
		{
		    color = (char *)highlight_color(id, (char_u *)"fg", modec);
		    if (color == NULL)
			colorindex = 0;
		    else
			colorindex = atoi(color);

		    if (colorindex >= 0 && colorindex < t_colors)
		    {
			this_color = prt_get_term_color(colorindex);
			if (this_color == 0xffffffUL)
			    this_color = 0;
			else if (*p_bg == 'd')
			    this_color = darken_rgb(this_color);

			if (this_color != current_fg)
			{
			    current_fg = this_color;
			    mch_print_set_fg(this_color);
			}
		    }
		}
	    }
	}
#endif /* FEAT_SYN_HL */

	/*
	 * Appropriately expand any tabs to spaces.
	 */
	if (*p == TAB || tab_spaces != 0)
	{
	    if (tab_spaces == 0)
		tab_spaces = curbuf->b_p_ts - (print_pos % curbuf->b_p_ts);

	    while (tab_spaces > 0)
	    {
		x_position += mch_print_text_out(x_position,
			 settingsp->line_height * page_line,
				    (char_u *)" ",
				    1,
				    &need_break);
		print_pos++;
		tab_spaces--;
		if (need_break)
		    break;
	    }
	}
	else
	{
	    x_position += mch_print_text_out(x_position,
			    settingsp->line_height * page_line,
			    p,
			    outputlen,
			    &need_break);
	    print_pos++;
	}
	p += outputlen - 1;
    }

    *lead_spaces = tab_spaces;

    /*
     * Start next line of file if we clip lines, or have reached end of the
     * line.
     */
    if (*p == NUL || (printer_opts[OPT_PRINT_WRAP].present
		&& printer_opts[OPT_PRINT_WRAP].string[0] == 'n'))
	return 0;
    return p - start_line;
}

# if defined(FEAT_POSTSCRIPT) || defined(PROTO)

/*
 * PS printer stuff
 */

#define CHAR_HEIGHT_IN_POINTS	(12)
#define CHAR_WIDTH_IN_POINTS	(7) /*<VN> */

static FILE *s_ps_file;

static int s_left_margin;
static int s_right_margin;
static int s_top_margin;

    void
mch_print_cleanup()
{
    if (s_ps_file != NULL)
	fclose(s_ps_file);
}

static int to_device_units __ARGS((int idx, int dpi, int physsize, int offset));
static int mch_print_get_cpl __ARGS((int *yChar_out, int *number_width_out));
static int mch_print_get_lpp __ARGS((int yCharsize));

    static int
to_device_units(idx, dpi, physsize, offset)
    int		idx;
    int		dpi;
    int		physsize;
    int		offset;
{
    int		ret;

    if (printer_opts[idx].string[0] == 'i')
	ret = (printer_opts[idx].number * dpi);
    else if (printer_opts[idx].string[0] == 'm')
	ret = (printer_opts[idx].number * 10 * dpi) / 254;
    else
	ret = (physsize * printer_opts[idx].number) / 100;

    if (ret < offset)
	return 0;
    else
	return ret - offset;
}

    static int
mch_print_get_cpl(yChar_out, number_width_out)
    int *yChar_out;
    int *number_width_out;
{
    int cpl;
    int hr;
    int phyw;
    int dvoff;
    int rev_offset;
    int dpi = 72;

    *yChar_out = CHAR_HEIGHT_IN_POINTS;

    hr	    = 595;
    phyw    = 595;
    dvoff   = 0;

    rev_offset = phyw - (dvoff + hr);

    s_left_margin = to_device_units(OPT_PRINT_LEFT, dpi, phyw, dvoff);
    if (printer_opts[OPT_PRINT_NUMBER].present)
		*number_width_out = 8 * CHAR_WIDTH_IN_POINTS;

    s_right_margin = hr - to_device_units(OPT_PRINT_RIGHT, dpi, phyw,
								  rev_offset);

    cpl	= (s_right_margin - s_left_margin) / CHAR_WIDTH_IN_POINTS;

    return cpl;
}

/*
 * Get number of lines of text that fit on a page (excluding the header).
 */
    static int
mch_print_get_lpp(yCharsize)
    int		yCharsize;
{
    int		vr;
    int		phyw;
    int		dvoff;
    int		rev_offset;
    int		bottom_margin;
    int		dpi;

    vr	    = 842;
    phyw    = 842;
    dvoff   = 0;
    dpi	    = 72;

    rev_offset = phyw - (dvoff + vr);
    s_top_margin = to_device_units(OPT_PRINT_TOP, dpi, phyw, dvoff);

    /* adjust top margin if there is a header */
    if (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	s_top_margin += (yCharsize
			       * printer_opts[OPT_PRINT_HEADERHEIGHT].number);

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw, rev_offset);


    return (bottom_margin - s_top_margin) / yCharsize ;
}

    int
mch_print_init(psettings, jobname, forceit)
    prt_settings_T *psettings;
    char_u	*jobname;
    int		forceit;
{
#if 0
    /*
     * If bang present, return default printer setup with no dialogue
     */
    if (forceit)
	s_pd.Flags |= PD_RETURNDEFAULT;
#endif

    s_ps_file = open_exfile((char_u *)"test.ps", forceit, WRITEBIN);
    if (s_ps_file == NULL)
    {
	EMSG(_("E999: Can't open PostScript output file"));
	mch_print_cleanup();
	return FALSE;
    }

    (void)put_line(s_ps_file, "%!");
    (void)put_line(s_ps_file, "/Courier findfont");
    (void)put_line(s_ps_file, "12 scalefont");
    (void)put_line(s_ps_file, "setfont");

    /*
     * Fill in the settings struct
     */
    psettings->chars_per_line = mch_print_get_cpl(&psettings->line_height,
						    &psettings->number_width);
    psettings->lines_per_page = mch_print_get_lpp(psettings->line_height);
    psettings->n_collated_copies = 1;
    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

    return TRUE;
}

/* ARGSUSED */
    int
mch_print_begin(psettings)
    prt_settings_T *psettings;
{
    return TRUE;
}

    void
mch_print_end()
{
    fclose(s_ps_file);
}

    int
mch_print_end_page()
{
    (void)put_line(s_ps_file, "showpage");
    return TRUE;
}

    int
mch_print_begin_page()
{
    return TRUE;
}

/*ARGSUSED*/
    int
mch_print_text_out(x, y, p, len, must_break)
    int		x;
    int		y;
    char_u	*p;
    int		len;
    int		*must_break;
{
    int		sz;

    (void)fprintf(s_ps_file, "%d %d moveto\n",
			    x + s_left_margin,
			    842 - (y + s_top_margin));

    if (*p == '(' || *p == ')' || *p == '\\')
	(void)fprintf(s_ps_file, "(\\%c) show\n", *p);
    else
	(void)fprintf(s_ps_file, "(%c) show\n", *p);

#ifndef FEAT_PROPORTIONAL_FONTS
	sz = CHAR_WIDTH_IN_POINTS;
#else
	sz = CHAR_WIDTH_IN_POINTS;
#endif
    *must_break = ((x + s_left_margin + sz) > s_right_margin);

    return sz;
}

#if defined(FEAT_GUI) || defined(PROTO)
/* ARGSUSED */
    void
mch_print_setfont(iBold, iItalic, iUnderline)
    int iBold;
    int iItalic;
    int iUnderline;
{
    /*<VN> write 'findfont', 'scalefont', 'setfont'*/
}
#endif

/* ARGSUSED */
    void
mch_print_set_bg(bgcol)
    unsigned long bgcol;
{
    /*<VN> write ?????*/
}
    void
mch_print_set_fg(fgcol)
    unsigned long fgcol;
{
    int	    r, g, b;

    r = (fgcol & 0xff0000) >> 16;
    g = (fgcol & 0xff00) >> 8;
    b = fgcol & 0xff;
    (void)fprintf(s_ps_file, "%d.%02d %d.%02d %d.%02d setrgbcolor\n",
		  r / 255, (r * 100 + 127) / 255 % 100,
		  g / 255, (g * 100 + 127) / 255 % 100,
		  b / 255, (b * 100 + 127) / 255 % 100);
}


# endif /*FEAT_POSTSCRIPT*/
#endif /*FEAT_PRINTER*/


#ifdef FEAT_EVAL

# if defined(HAVE_LOCALE_H) || defined(X_LOCALE)
static char *get_locale_val __ARGS((int what));

    static char *
get_locale_val(what)
    int		what;
{
    char	*loc;

    /* Obtain the locale value from the libraries.  For DJGPP this is
     * redefined and it doesn't use the arguments. */
    loc = setlocale(what, NULL);

#  if defined(__BORLANDC__)
    if (loc != NULL)
    {
	char_u	*p;

	/* Borland returns something like "LC_CTYPE=<name>\n"
	 * Let's try to fix that bug here... */
	p = vim_strchr(loc, '=');
	if (p != NULL)
	{
	    loc = ++p;
	    while (*p != NUL)	/* remove trailing newline */
	    {
		if (*p < ' ')
		{
		    *p = NUL;
		    break;
		}
		++p;
	    }
	}
    }
#  endif

    return loc;
}
# endif

/*
 * Set the "v:lang" variable according to the current locale setting.
 * Also do "v:lc_time"and "v:ctype".
 */
    void
set_lang_var()
{
    char_u	*loc;

# if defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    loc = (char_u *)get_locale_val(LC_CTYPE);
# else
    /* setlocale() not supported: use the default value */
    loc = (char_u *)"C";
# endif
    set_vim_var_string(VV_CTYPE, loc, -1);

    /* When LC_MESSAGES isn't defined use the value from LC_CTYPE. */
# if (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) && defined(LC_MESSAGES)
    loc = (char_u *)get_locale_val(LC_MESSAGES);
# endif
    set_vim_var_string(VV_LANG, loc, -1);

# if defined(HAVE_LOCALE_H) || defined(X_LOCALE)
    loc = (char_u *)get_locale_val(LC_TIME);
# endif
    set_vim_var_string(VV_LC_TIME, loc, -1);
}
#endif

#if (defined(HAVE_LOCALE_H) || defined(X_LOCALE)) \
	&& (defined(FEAT_GETTEXT) || defined(FEAT_MBYTE))
/*
 * ":language":  Set the language (locale).
 */
    void
ex_language(eap)
    exarg_T	*eap;
{
    char	*loc;
    char_u	*p;
    char_u	*name;
    int		what = LC_ALL;
    char	*whatstr = "";

    name = eap->arg;

    /* Check for "messages {name}", "ctype {name}" or "time {name}" argument.
     * Allow abbreviation, but require at least 3 characters to avoid
     * confusion with a two letter language name "me" or "ct". */
    p = skiptowhite(eap->arg);
    if ((*p == NUL || vim_iswhite(*p)) && p - eap->arg >= 3)
    {
	if (STRNICMP(eap->arg, "messages", p - eap->arg) == 0)
	{
#ifdef LC_MESSAGES
	    what = LC_MESSAGES;
#else
	    what = LC_CTYPE;
#endif
	    name = skipwhite(p);
	    whatstr = "messages ";
	}
	else if (STRNICMP(eap->arg, "ctype", p - eap->arg) == 0)
	{
	    what = LC_CTYPE;
	    name = skipwhite(p);
	    whatstr = "ctype ";
	}
	else if (STRNICMP(eap->arg, "time", p - eap->arg) == 0)
	{
	    what = LC_TIME;
	    name = skipwhite(p);
	    whatstr = "time ";
	}
    }

    if (*name == NUL)
    {
	smsg((char_u *)_("Current %slanguage: \"%s\""),
		whatstr, setlocale(what, NULL));
    }
    else
    {
	loc = setlocale(what, (char *)name);
	if (loc == NULL)
	    EMSG2(_("E197: Cannot set language to \"%s\""), name);
	else
	{
#ifdef HAVE_NL_MSG_CAT_CNTR
	    /* Need to do this for GNU gettext, otherwise cached translations
	     * will be used again. */
	    extern int _nl_msg_cat_cntr;

	    ++_nl_msg_cat_cntr;
#endif
	    /* Reset $LC_ALL, otherwise it would overrule everyting. */
	    vim_setenv((char_u *)"LC_ALL", (char_u *)"");

	    if (what != LC_TIME)
	    {
		/* Tell gettext() what to translate to.  It apparently doesn't
		 * use the currently effective locale.  Also do this when
		 * FEAT_GETTEXT isn't defined, so that shell commands use this
		 * value. */
		if (what == LC_ALL)
		    vim_setenv((char_u *)"LANG", name);
		if (what != LC_CTYPE)
		    vim_setenv((char_u *)"LC_MESSAGES", name);

		/* Set $LC_CTYPE, because it overrules $LANG, and
		 * gtk_set_locale() calls setlocale() again.  gnome_init()
		 * sets $LC_CTYPE to "en_US" (that's a bug!). */
#ifdef LC_MESSAGES
		if (what != LC_MESSAGES)
#endif
		    vim_setenv((char_u *)"LC_CTYPE", name);
# ifdef FEAT_GUI_GTK
		/* Let GTK know what locale we're using.  Not sure this is
		 * really needed... */
		if (gui.in_use)
		    (void)gtk_set_locale();
# endif
	    }

# ifdef FEAT_EVAL
	    /* Set v:lang, v:lc_time and v:ctype to the final result. */
	    set_lang_var();
# endif
	}
    }
}

# if defined(FEAT_CMDL_COMPL) || defined(PROTO)
/*
 * Function given to ExpandGeneric() to obtain the possible arguments of the
 * ":language" command.
 */
/*ARGSUSED*/
    char_u *
get_lang_arg(xp, idx)
    expand_T	*xp;
    int		idx;
{
    if (idx == 0)
	return (char_u *)"messages";
    if (idx == 1)
	return (char_u *)"ctype";
    return NULL;
}
# endif

#endif
