/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * cmdcmds.c: functions for command line commands
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

#if defined(LATTICE) || defined(NT)
# define mktemp(a)	tmpnam(a)
#endif

extern char		*mktemp __ARGS((char *));

/*
 * align text:
 * type = -1  left aligned
 * type = 0   centered
 * type = 1   right aligned
 */
	void
do_align(start, end, width, type)
	linenr_t	start;
	linenr_t	end;
	int			width;
	int			type;
{
	FPOS	pos;
	int		len;
	int		indent = 0;

	pos = curwin->w_cursor;
	if (type == -1)		/* left align: width is used for new indent */
	{
		if (width >= 0)
			indent = width;
	}
	else
	{
		/*
		 * if 'textwidth' set, use it
		 * else if 'wrapmargin' set, use it
		 * if invalid value, use 80
		 */
		if (width <= 0)
			width = curbuf->b_p_tw;
		if (width == 0 && curbuf->b_p_wm > 0)
			width = Columns - curbuf->b_p_wm;
		if (width <= 0)
			width = 80;
	}

	if (!u_save((linenr_t)(start - 1), (linenr_t)(end + 1)))
		return;
	for (curwin->w_cursor.lnum = start; curwin->w_cursor.lnum <= end; ++curwin->w_cursor.lnum)
	{
		set_indent(indent, TRUE);				/* remove existing indent */
		if (type == -1)							/* left align */
			continue;
		len = strsize(ml_get(curwin->w_cursor.lnum));		/* get line lenght */
		if (len < width)
			switch (type)
			{
			case 0:		set_indent((width - len) / 2, FALSE);	/* center */
						break;
			case 1:		set_indent(width - len, FALSE);			/* right */
						break;
			}
	}
	curwin->w_cursor = pos;
	beginline(TRUE);
	updateScreen(NOT_VALID);
}

/*
 * :move command - move lines line1-line2 to line n
 *
 * return FAIL for failure, OK otherwise
 */
	int
do_move(line1, line2, n)
	linenr_t	line1;
	linenr_t	line2;
	linenr_t	n;
{
	char_u		*q;
	int			has_mark;

	if (n >= line1 && n < line2 && line2 > line1)
	{
		EMSG("Move lines into themselves");
		return FAIL;
	}

	/*
	 * adjust line marks (global marks done below)
	 * if the lines are moved down, the marks in the moved lines
	 * move down and the marks in the lines between the old and
	 * new position move up.
	 * If the lines are moved up it is just the other way round
	 */
	if (n >= line2)			/* move down */
	{
		mark_adjust(line1, line2, n - line2);
		mark_adjust(line2 + 1, n,  -(line2 - line1 + 1));
	}
	else					/* move up */
	{
		mark_adjust(line1, line2, -(line1 - n - 1));
		mark_adjust(n + 1, line1 - 1, line2 - line1 + 1);
	}

	if (n >= line1)
	{
		--n;
		curwin->w_cursor.lnum = n - (line2 - line1) + 1;
	}
	else
		curwin->w_cursor.lnum = n + 1;
	while (line1 <= line2)
	{
			/* this undo is not efficient, but it works */
		u_save(line1 - 1, line1 + 1);
		q = strsave(ml_get(line1));
		if (q != NULL)
		{
			/*
			 * marks from global command go with the line
			 */
			has_mark = ml_has_mark(line1);
			ml_delete(line1);
			u_save(n, n + 1);
			ml_append(n, q, (colnr_t)0, FALSE);
			free(q);
			if (has_mark)
				ml_setmarked(n + 1);
		}
		if (n < line1)
		{
			++n;
			++line1;
		}
		else
			--line2;
	}
	CHANGED;
	return OK;
}

/*
 * :copy command - copy lines line1-line2 to line n
 */
	void
do_copy(line1, line2, n)
	linenr_t	line1;
	linenr_t	line2;
	linenr_t	n;
{
	linenr_t		lnum;
	char_u			*p;

	mark_adjust(n + 1, MAXLNUM, line2 - line1 + 1);

	/*
	 * there are three situations:
	 * 1. destination is above line1
	 * 2. destination is between line1 and line2
	 * 3. destination is below line2
	 *
	 * n = destination (when starting)
	 * curwin->w_cursor.lnum = destination (while copying)
	 * line1 = start of source (while copying)
	 * line2 = end of source (while copying)
	 */
	u_save(n, n + 1);
	curwin->w_cursor.lnum = n;
	lnum = line2 - line1 + 1;
	while (line1 <= line2)
	{
		/* need to use strsave() because the line will be unlocked
			within ml_append */
		p = strsave(ml_get(line1));
		if (p != NULL)
		{
			ml_append(curwin->w_cursor.lnum, p, (colnr_t)0, FALSE);
			free(p);
		}
				/* situation 2: skip already copied lines */
		if (line1 == n)
			line1 = curwin->w_cursor.lnum;
		++line1;
		if (curwin->w_cursor.lnum < line1)
			++line1;
		if (curwin->w_cursor.lnum < line2)
			++line2;
		++curwin->w_cursor.lnum;
	}
	CHANGED;
	msgmore((long)lnum);
}

/*
 * handle the :! command.
 * We replace the extra bangs by the previously entered command and remember
 * the command.
 */
	void
dobang(addr_count, line1, line2, forceit, arg)
	int			addr_count;
	linenr_t	line1, line2;
	int			forceit;
	char_u		*arg;
{
	static	char_u	*prevcmd = NULL;		/* the previous command */
	char_u			*t;
	char_u			*trailarg;
	int 			len;

	/*
	 * Disallow shell commands from .exrc and .vimrc in current directory for
	 * security reasons.
	 */
	if (secure)
	{
		secure = 2;
		emsg(e_curdir);
		return;
	}
	len = STRLEN(arg) + 1;

	autowrite_all();
	/*
	 * try to find an embedded bang, like in :!<cmd> ! [args]
	 * (:!! is indicated by the 'forceit' variable)
	 */
	trailarg = arg;
	skiptospace(&trailarg);
	skipspace(&trailarg);
	if (*trailarg == '!')
		*trailarg++ = NUL;
	else
		trailarg = NULL;

	if (forceit || trailarg != NULL)			/* use the previous command */
	{
		if (prevcmd == NULL)
		{
			emsg(e_noprev);
			return;
		}
		len += STRLEN(prevcmd) * (trailarg != NULL && forceit ? 2 : 1);
	}

	if (len > CMDBUFFSIZE)
	{
		emsg(e_toolong);
		return;
	}
	if ((t = alloc(len)) == NULL)
		return;
	*t = NUL;
	if (forceit)
		STRCPY(t, prevcmd);
	STRCAT(t, arg);
	if (trailarg != NULL)
	{
		STRCAT(t, prevcmd);
		STRCAT(t, trailarg);
	}
	free(prevcmd);
	prevcmd = t;

	if (bangredo)			/* put cmd in redo buffer for ! command */
	{
		AppendToRedobuff(prevcmd);
		AppendToRedobuff((char_u *)"\n");
		bangredo = FALSE;
	}
		/* echo the command */
	msg_start();
	msg_outchar(':');
	if (addr_count)						/* :range! */
	{
		msg_outnum((long)line1);
		msg_outchar(',');
		msg_outnum((long)line2);
	}
	msg_outchar('!');
	msg_outtrans(prevcmd, -1);
	msg_ceol();

	if (addr_count == 0)				/* :! */
		doshell(prevcmd); 
	else								/* :range! */
		dofilter(line1, line2, prevcmd, TRUE, TRUE);
}

/*
 * call a shell to execute a command
 */
	void
doshell(cmd)
	char_u	*cmd;
{
	BUF		*buf;

	/*
	 * Disallow shell commands from .exrc and .vimrc in current directory for
	 * security reasons.
	 */
	if (secure)
	{
		secure = 2;
		emsg(e_curdir);
		msg_end();
		return;
	}
	stoptermcap();
	msg_outchar('\n');					/* may shift screen one line up */

		/* warning message before calling the shell */
	if (p_warn)
		for (buf = firstbuf; buf; buf = buf->b_next)
			if (buf->b_changed)
			{
				msg_outstr((char_u *)"[No write since last change]\n");
				break;
			}

	windgoto((int)Rows - 1, 0);
	cursor_on();
	(void)call_shell(cmd, 0, TRUE);

#ifdef AMIGA
	wait_return(term_console ? -1 : TRUE);		/* see below */
#else
	wait_return(TRUE);				/* includes starttermcap() */
#endif

	/*
	 * In an Amiga window redrawing is caused by asking the window size.
	 * If we got an interrupt this will not work. The chance that the window
	 * size is wrong is very small, but we need to redraw the screen.
	 * Don't do this if ':' hit in wait_return().
	 * THIS IS UGLY but it save an extra redraw.
	 */
#ifdef AMIGA
	if (skip_redraw)				/* ':' hit in wait_return() */
		must_redraw = CLEAR;
	else if (term_console)
	{
		OUTSTR("\033[0 q"); 		/* get window size */
		if (got_int)
			must_redraw = CLEAR;	/* if got_int is TRUE we have to redraw */
		else
			must_redraw = 0;		/* no extra redraw needed */
	}
#endif /* AMIGA */
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
	void
dofilter(line1, line2, buff, do_in, do_out)
	linenr_t	line1, line2;
	char_u		*buff;
	int			do_in, do_out;
{
#ifdef LATTICE
	char_u		itmp[L_tmpnam];		/* use tmpnam() */
	char_u		otmp[L_tmpnam];
#else
	char_u		itmp[TMPNAMELEN];
	char_u		otmp[TMPNAMELEN];
#endif
	linenr_t 	linecount;

	/*
	 * Disallow shell commands from .exrc and .vimrc in current directory for
	 * security reasons.
	 */
	if (secure)
	{
		secure = 2;
		emsg(e_curdir);
		return;
	}
	if (*buff == NUL)		/* no filter command */
		return;
	linecount = line2 - line1 + 1;
	curwin->w_cursor.lnum = line1;
	curwin->w_cursor.col = 0;
	/* cursupdate(); */

	/*
	 * 1. Form temp file names
	 * 2. Write the lines to a temp file
	 * 3. Run the filter command on the temp file
	 * 4. Read the output of the command into the buffer
	 * 5. Delete the original lines to be filtered
	 * 6. Remove the temp files
	 */

#ifndef LATTICE
	/* for lattice we use tmpnam(), which will make its own name */
	STRCPY(itmp, TMPNAME1);
	STRCPY(otmp, TMPNAME2);
#endif

	if ((do_in && *mktemp((char *)itmp) == NUL) || (do_out && *mktemp((char *)otmp) == NUL))
	{
		emsg(e_notmp);
		return;
	}

/*
 * ! command will be overwritten by next mesages
 * This is a trade off between showing the command and not scrolling the
 * text one line up (problem on slow terminals).
 */
	must_redraw = CLEAR;		/* screen has been shifted up one line */
	++no_wait_return;			/* don't call wait_return() while busy */
	if (do_in && buf_write(curbuf, itmp, NULL, line1, line2, FALSE, 0, FALSE) == FAIL)
	{
		msg_outchar('\n');					/* keep message from writeit() */
		--no_wait_return;
		(void)emsg2(e_notcreate, itmp);		/* will call wait_return */
		return;
	}
	if (!do_out)
		outchar('\n');

#if defined(UNIX) && !defined(ARCHIE)
/*
 * put braces around the command (for concatenated commands)
 */
 	sprintf((char *)IObuff, "(%s)", (char *)buff);
	if (do_in)
	{
		STRCAT(IObuff, " < ");
		STRCAT(IObuff, itmp);
	}
	if (do_out)
	{
		STRCAT(IObuff, " > ");
		STRCAT(IObuff, otmp);
	}
#else
/*
 * for shells that don't understand braces around commands, at least allow
 * the use of commands in a pipe.
 */
	STRCPY(IObuff, buff);
	if (do_in)
	{
		char_u		*p;
	/*
	 * If there is a pipe, we have to put the '<' in front of it
	 */
		p = STRCHR(IObuff, '|');
		if (p)
			*p = NUL;
		STRCAT(IObuff, " < ");
		STRCAT(IObuff, itmp);
		p = STRCHR(buff, '|');
		if (p)
			STRCAT(IObuff, p);
	}
	if (do_out)
	{
		STRCAT(IObuff, " > ");
		STRCAT(IObuff, otmp);
	}
#endif

	windgoto((int)Rows - 1, 0);
	cursor_on();
			/* errors are ignored, so you can see the error
			   messages from the command; use 'u' to fix the text */
	(void)call_shell(IObuff, 1, FALSE);

	if (do_out)
	{
		if (!u_save((linenr_t)(line2), (linenr_t)(line2 + 1)))
		{
			linecount = 0;
			goto error;
		}
		if (readfile(otmp, NULL, line2, FALSE, (linenr_t)0, MAXLNUM) == FAIL)
		{
			outchar('\n');
			emsg2(e_notread, otmp);
			linecount = 0;
			goto error;
		}

		if (do_in)
		{
			curwin->w_cursor.lnum = line1;
			dellines(linecount, TRUE, TRUE);
		}
		--no_wait_return;
	}
	else
	{
error:
		--no_wait_return;
		wait_return(FALSE);
	}
	updateScreen(CLEAR);		/* do this before messages below */

	if (linecount > p_report)
	{
		if (!do_in && do_out)
			msgmore(linecount);
		else
			smsg((char_u *)"%ld lines filtered", (long)linecount);
	}
	remove((char *)itmp);
	remove((char *)otmp);
}
