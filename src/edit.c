/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * edit.c: functions for insert mode
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "ops.h"	/* for operator */

extern u_char *get_inserted();
static void start_arrow __ARGS((void));
static void stop_arrow __ARGS((void));
static void stop_insert __ARGS((void));
static int echeck_abbr __ARGS((int));

int arrow_used;				/* Normally FALSE, set to TRUE after hitting
							 * cursor key in insert mode. Used by vgetorpeek()
							 * to decide when to call u_sync() */
int restart_edit = 0;		/* call edit when next command finished */
static u_char	*last_insert = NULL;
							/* the text of the previous insert */
static int		last_insert_skip;
							/* number of chars in front of the previous insert */
static int		new_insert_skip;
							/* number of chars in front of the current insert */

	void
edit(count)
	long count;
{
	u_char		 c;
	u_char		 cc;
	u_char		*ptr;
	u_char		*saved_line = NULL;		/* saved line for replace mode */
	linenr_t	 saved_lnum = 0;		/* lnum of saved line */
	int			 saved_char = NUL;		/* char replaced by NL */
	linenr_t	 lnum;
	int 		 temp = 0;
	int			 mode;
	int			 nextc = 0;
	int			 lastc = 0;
	colnr_t		 mincol;

	if (restart_edit)
	{
		arrow_used = TRUE;
		restart_edit = 0;
	}
	else
		arrow_used = FALSE;

#ifdef DIGRAPHS
	dodigraph(-1);					/* clear digraphs */
#endif

/*
 * Get the current length of the redo buffer, those characters have to be
 * skipped if we want to get to the inserted characters.
 */

	ptr = get_inserted();
	new_insert_skip = strlen((char *)ptr);
	free(ptr);

	old_indent = 0;

	for (;;)
	{
		if (arrow_used)		/* don't repeat insert when arrow key used */
			count = 0;

		set_want_col = TRUE;	/* set Curswant in case of K_DARROW or K_UARROW */
		cursupdate();		/* Figure out where the cursor is based on Curpos. */
		showruler(0);
		setcursor();
		if (nextc)			/* character remaining from CTRL-V */
		{
			c = nextc;
			nextc = 0;
		}
		else
		{
			c = vgetc();
			if (c == Ctrl('C') && got_int)
				got_int = FALSE;
		}
		if (c != Ctrl('D'))			/* remember to detect ^^D and 0^D */
			lastc = c;

/*
 * In replace mode a backspace puts the original text back.
 * We save the current line to be able to do that.
 * If characters are appended to the line, they will be deleted.
 * If we start a new line (with CR) the saved line will be empty, thus
 * the characters will be deleted.
 * If we backspace over the new line, that line will be saved.
 */
		if (State == REPLACE && saved_lnum != Curpos.lnum)
		{
			free(saved_line);
			saved_line = (u_char *)strsave((char *)nr2ptr(Curpos.lnum));
			saved_lnum = Curpos.lnum;
		}

#ifdef DIGRAPHS
		c = dodigraph(c);
#endif /* DIGRAPHS */

		if (c == Ctrl('V'))
		{
			outchar('^');
			AppendToRedobuff("\026");	/* CTRL-V */
			cursupdate();
			setcursor();

			c = get_literal(&nextc);

		/* erase the '^' */
			if ((cc = gcharCurpos()) == NUL || (cc == TAB && !p_list))
				outchar(' ');
			else
				outstrn(transchar(cc));

			if (isidchar(c) || !echeck_abbr(c))
				insertchar(c);
			continue;
		}
		switch (c)		/* handle character in insert mode */
		{
			  case Ctrl('O'):		/* execute one command */
			    if (echeck_abbr(Ctrl('O') + 0x100))
					break;
			  	count = 0;
				if (State == INSERT)
					restart_edit = 'I';
				else
					restart_edit = 'R';
				goto doESCkey;

			  case ESC: 			/* an escape ends input mode */
			    if (echeck_abbr(ESC + 0x100))
					break;
				/*FALLTHROUGH*/

			  case Ctrl('C'):
doESCkey:
				if (!arrow_used)
				{
					AppendToRedobuff(ESC_STR);

					if (--count > 0)		/* repeat what was typed */
					{
							start_redo_ins();
							continue;
					}
					stop_insert();
				}
				set_want_col = TRUE;

				/*
				 * The cursor should end up on the last inserted character.
				 */
				if (Curpos.col != 0 && (!restart_edit || gcharCurpos() == NUL) && !p_ri)
					decCurpos();
				if (extraspace)			/* did reverse replace in column 0 */
				{
					delchar(FALSE);
					updateline();
					extraspace = FALSE;
				}
				State = NORMAL;
				script_winsize_pp();	/* may need to put :winsize in script */
					/* inchar() may have deleted the "INSERT" message */
				if (Recording)
					showmode();
				else if (p_smd)
					msg("");
				free(saved_line);
				old_indent = 0;
				return;

			  	/*
				 * Insert the previously inserted text.
				 * Last_insert actually is a copy of the redo buffer, so we
				 * first have to remove the command.
				 * For ^@ the trailing ESC will end the insert.
				 */
			  case K_ZERO:
			  case Ctrl('A'):
				stuff_inserted(NUL, 1L, (c == Ctrl('A')));
				break;

			  	/*
				 * insert the contents of a register
				 */
			  case Ctrl('R'):
			  	if (!insertbuf(vgetc()))
					beep();
				break;

			  case Ctrl('P'):			/* toggle reverse insert mode */
			  	p_ri = !p_ri;
				showmode();
				break;

				/*
				 * If the cursor is on an indent, ^T/^D insert/delete one
				 * shiftwidth. Otherwise ^T/^D behave like a TAB/backspace.
				 * This isn't completely compatible with
				 * vi, but the difference isn't very noticeable and now you can
				 * mix ^D/backspace and ^T/TAB without thinking about which one
				 * must be used.
				 */
			  case Ctrl('T'):		/* make indent one shiftwidth greater */
			  case Ctrl('D'): 		/* make indent one shiftwidth smaller */
				stop_arrow();
				AppendCharToRedobuff(c);
				if ((lastc == '0' || lastc == '^') && Curpos.col)
				{
					--Curpos.col;
					delchar(FALSE);			/* delete the '^' or '0' */
					if (lastc == '^')
						old_indent = get_indent();	/* remember current indent */

						/* determine offset from first non-blank */
					temp = Curpos.col;
					beginline(TRUE);
					temp -= Curpos.col;
					set_indent(0, TRUE);	/* remove all indent */
				}
				else
				{
						/* determine offset from first non-blank */
					temp = Curpos.col;
					beginline(TRUE);
					temp -= Curpos.col;

					shift_line(c == Ctrl('D'), TRUE);

						/* try to put cursor on same character */
					temp += Curpos.col;
				}
				if (temp <= 0)
					Curpos.col = 0;
				else
					Curpos.col = temp;
				did_ai = FALSE;
				did_si = FALSE;
				can_si = FALSE;
		  		goto redraw;

			  case BS:
			  case DEL:
nextbs:
				mode = 0;
dodel:
				/* can't delete anything in an empty file */
				/* can't backup past first character in buffer */
				/* can't backup past starting point unless 'backspace' > 1 */
				/* can backup to a previous line if 'backspace' == 0 */
				if (bufempty() || (!p_ri &&
						((Curpos.lnum == 1 && Curpos.col <= 0) ||
						(p_bs < 2 && (arrow_used ||
							(Curpos.lnum == Insstart.lnum &&
							Curpos.col <= Insstart.col) ||
							(Curpos.col <= 0 && p_bs == 0))))))
				{
					beep();
					goto redraw;
				}

				stop_arrow();
				if (p_ri)
					incCurpos();
				if (Curpos.col <= 0)		/* delete newline! */
				{
					lnum = Insstart.lnum;
					if (Curpos.lnum == Insstart.lnum || p_ri)
					{
						if (!u_save((linenr_t)(Curpos.lnum - 2), (linenr_t)(Curpos.lnum + 1)))
							goto redraw;
						--Insstart.lnum;
						Insstart.col = 0;
					}
				/* in replace mode, in the line we started replacing, we
														only move the cursor */
					if (State != REPLACE || Curpos.lnum > lnum)
					{
						temp = gcharCurpos();		/* remember current char */
						--Curpos.lnum;
						dojoin(FALSE, TRUE);
						if (temp == NUL && gcharCurpos() != NUL)
							++Curpos.col;
						if (saved_char)				/* restore what NL replaced */
						{
							State = NORMAL;			/* no replace for this char */
							inschar(saved_char);	/* but no showmatch */
							State = REPLACE;
							saved_char = NUL;
							if (!p_ri)
								decCurpos();
						}
						else if (p_ri)				/* in reverse mode */
							saved_lnum = 0;			/* save this line again */
					}
					else
						decCurpos();
					did_ai = FALSE;
				}
				else
				{
					if (p_ri && State != REPLACE)
						decCurpos();
					mincol = 0;
					if (mode == 3 && !p_ri && p_ai)	/* keep indent */
					{
						temp = Curpos.col;
						beginline(TRUE);
						if (Curpos.col < temp)
							mincol = Curpos.col;
						Curpos.col = temp;
					}

					/* delete upto starting point, start of line or previous word */
					do
					{
						if (!p_ri)
							decCurpos();

								/* start of word? */
						if (mode == 1 && !isspace(gcharCurpos()))
						{
							mode = 2;
							temp = isidchar(gcharCurpos());
						}
								/* end of word? */
						else if (mode == 2 && (isspace(cc = gcharCurpos()) || isidchar(cc) != temp))
						{
							if (!p_ri)
								incCurpos();
							else if (State == REPLACE)
								decCurpos();
							break;
						}
						if (State == REPLACE)
						{
							if (saved_line)
							{
								if (extraspace)
								{
									if ((int)strlen(nr2ptr(Curpos.lnum)) - 1 > (int)strlen((char *)saved_line))
										delchar(FALSE);
									else
									{
										decCurpos();
										delchar(FALSE);
										extraspace = FALSE;
										pcharCurpos(*saved_line);
									}
								}
								else if (Curpos.col < strlen((char *)saved_line))
									pcharCurpos(saved_line[Curpos.col]);
								else if (!p_ri)
									delchar(FALSE);
							}
						}
						else  /* State != REPLACE */
						{
							delchar(FALSE);
							if (p_ri && gcharCurpos() == NUL)
								break;
						}
						if (mode == 0)		/* just a single backspace */
							break;
						if (p_ri && State == REPLACE && incCurpos())
							break;
					} while (p_ri || (Curpos.col > mincol && (Curpos.lnum != Insstart.lnum ||
							Curpos.col != Insstart.col)));
					if (extraspace)
						decCurpos();
				}
				did_si = FALSE;
				can_si = FALSE;
				if (Curpos.col <= 1)
					did_ai = FALSE;
				/*
				 * It's a little strange to put backspaces into the redo
				 * buffer, but it makes auto-indent a lot easier to deal
				 * with.
				 */
				AppendCharToRedobuff(c);
				if (vpeekc() == BS)
				{
						c = vgetc();
						goto nextbs;	/* speedup multiple backspaces */
				}
redraw:
				cursupdate();
				updateline();
				break;

			  case Ctrl('W'):		/* delete word before cursor */
			  	mode = 1;
			  	goto dodel;

			  case Ctrl('U'):		/* delete inserted text in current line */
				mode = 3;
			  	goto dodel;

			  case K_LARROW:
			  	if (oneleft())
					start_arrow();
				else
					beep();
				break;

			  case K_SLARROW:
			  	if (Curpos.lnum > 1 || Curpos.col > 0)
				{
					bck_word(1L, 0);
					start_arrow();
				}
				else
					beep();
				break;

			  case K_RARROW:
				if (gcharCurpos() != NUL)
				{
					set_want_col = TRUE;
					start_arrow();
					++Curpos.col;
				}
				else
					beep();
				break;

			  case K_SRARROW:
			  	if (Curpos.lnum < line_count || gcharCurpos() != NUL)
				{
					fwd_word(1L, 0, 0);
					start_arrow();
				}
				else
					beep();
				break;

			  case K_UARROW:
			  	if (oneup(1L))
					start_arrow();
				else
					beep();
				break;

			  case K_SUARROW:
			  	if (onepage(BACKWARD, 1L))
					start_arrow();
				else
					beep();
				break;

			  case K_DARROW:
			  	if (onedown(1L))
					start_arrow();
				else
					beep();
				break;

			  case K_SDARROW:
			  	if (onepage(FORWARD, 1L))
					start_arrow();
				else
					beep();
				break;

			  case TAB:
			    if (echeck_abbr(TAB + 0x100))
					break;
			  	if (!p_et || (p_ri && State == REPLACE))
					goto normalchar;
										/* expand a tab into spaces */
				stop_arrow();
				did_ai = FALSE;
				did_si = FALSE;
				can_si = FALSE;
				temp = (int)p_ts - Curpos.col % (int)p_ts;
				inschar(' ');			/* delete one char in replace mode */
				while (--temp)
					insstr(" ");		/* insstr does not delete chars */
				AppendToRedobuff("\t");
				goto redraw;

			  case CR:
			  case NL:
			    if (echeck_abbr(c + 0x100))
					break;
				stop_arrow();
				if (State == REPLACE)
				{
					saved_char = gcharCurpos();
					delchar(FALSE);
				}
				AppendToRedobuff(NL_STR);
				if (!Opencmd(FORWARD, TRUE, State == INSERT))
					goto doESCkey;		/* out of memory */
				if (p_ri)
				{
					decCurpos();
					if (State == REPLACE && Curpos.col > 0)
						decCurpos();
				}
				break;

#ifdef DIGRAPHS
			  case Ctrl('K'):
				outchar('?');
				AppendToRedobuff("\026");	/* CTRL-V */
				setcursor();
			  	c = vgetc();
				outstrn(transchar(c));
				setcursor();
				c = getdigraph(c, vgetc());
				goto normalchar;
#endif /* DIGRAPHS */

			  case Ctrl('Y'):				/* copy from previous line */
				lnum = Curpos.lnum - 1;
				goto copychar;

			  case Ctrl('E'):				/* copy from next line */
				lnum = Curpos.lnum + 1;
copychar:
				if (lnum < 1 || lnum > line_count)
				{
					beep();
					break;
				}

				/* try to advance to the cursor column */
				temp = 0;
				ptr = (u_char *)nr2ptr(lnum);
				while (temp < Cursvcol && *ptr)
						temp += chartabsize(*ptr++, temp);

				if (temp > Cursvcol)
						--ptr;
				if ((c = *ptr) == NUL)
				{
					beep();
					break;
				}

				/*FALLTHROUGH*/
			  default:
normalchar:
				if (Curpos.col > 0 && ((can_si && c == '}') || (did_si && c == '{')))
					shift_line(TRUE, TRUE);

				if (isidchar(c) || !echeck_abbr(c))
					insertchar(c);
				break;
			}
	}
}

/*
 * Next character is interpreted literally.
 * A one, two or three digit decimal number is interpreted as its byte value.
 * If one or two digits are entered, *nextc is set to the next character.
 */
	int
get_literal(nextc)
	int *nextc;
{
	u_char		 cc;
	u_char		 nc;
	int			 oldstate;
	int			 i;

	oldstate = State;
	State = NOMAPPING;		/* next characters not mapped */

	if (got_int)
	{
		*nextc = NUL;
		return Ctrl('C');
	}
	cc = 0;
	for (i = 0; i < 3; ++i)
	{
		nc = vgetc();
		if (!isdigit(nc))
			break;
		cc = cc * 10 + nc - '0';
		nc = 0;
	}
	if (i == 0)		/* no number entered */
	{
		cc = nc;
		nc = 0;
		if (cc == K_ZERO)	/* NUL is stored as NL */
			cc = '\n';
	}
	else if (cc == 0)		/* NUL is stored as NL */
		cc = '\n';

	State = oldstate;
	*nextc = nc;
	got_int = FALSE;		/* CTRL-C typed after CTRL-V is not an interrupt */
	return cc;
}

/*
 * Special characters in this context are those that need processing other
 * than the simple insertion that can be performed here. This includes ESC
 * which terminates the insert, and CR/NL which need special processing to
 * open up a new line. This routine tries to optimize insertions performed by
 * the "redo", "undo" or "put" commands, so it needs to know when it should
 * stop and defer processing to the "normal" mechanism.
 */
#define ISSPECIAL(c)	((c) < ' ' || (c) >= DEL)

	void
insertchar(c)
	unsigned	c;
{
	int		haveto_redraw = FALSE;

	stop_arrow();
	/*
	 * If the cursor is past 'textwidth' and we are inserting a non-space,
	 * try to break the line in two or more pieces. If c == NUL then we have
	 * been called to do formatting only. If p_tw == 0 it does nothing.
	 */
	if (c == NUL || !isspace(c))
	{
		while (p_tw && Cursvcol >= p_tw)
		{
			int		startcol;		/* Cursor column at entry */
			int		wantcol;		/* column at textwidth border */
			int		foundcol;		/* column for start of word */

			if ((startcol = Curpos.col) == 0)
				break;
			coladvance((int)p_tw);			/* find column of textwidth border */
			wantcol = Curpos.col;

			Curpos.col = startcol - 1;
			foundcol = 0;
			while (Curpos.col > 0)			/* find position to break at */
			{
				if (isspace(gcharCurpos()))
				{
					while (Curpos.col > 0 && isspace(gcharCurpos()))
						--Curpos.col;
					if (Curpos.col == 0)	/* only spaces in front of text */
						break;
					foundcol = Curpos.col + 1;
					if (Curpos.col < wantcol)
						break;
				}
				--Curpos.col;
			}

			if (foundcol == 0)			/* no spaces, cannot break line */
			{
				Curpos.col = startcol;
				break;
			}
			Curpos.col = foundcol;		/* put cursor after pos. to break line */
			startcol -= foundcol;
			Opencmd(FORWARD, FALSE, FALSE);
			while (isspace(gcharCurpos()) && startcol)		/* delete blanks */
			{
				delchar(FALSE);
				--startcol;				/* adjust cursor pos. */
			}
			Curpos.col += startcol;
			curs_columns(FALSE);		/* update Cursvcol */
			haveto_redraw = TRUE;
		}
		if (c == NUL)					/* formatting only */
			return;
		if (haveto_redraw)
		{
			/*
			 * If the cursor ended up just below the screen we scroll up here
			 * to avoid a redraw of the whole screen in the most common cases.
			 */
 			if (Curpos.lnum == Botline && !emptyrows)
				s_del(0, 1, TRUE);
			updateScreen(CURSUPD);
		}
	}

	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;

	/*
	 * If there's any pending input, grab up to MAX_COLUMNS at once.
	 * This speeds up normal text input considerably.
	 */
	if (vpeekc() != NUL && State != REPLACE && !p_ri)
	{
		char			p[MAX_COLUMNS + 1];
		int 			i;

		p[0] = c;
		i = 1;
		while ((c = vpeekc()) != NUL && !ISSPECIAL(c) && i < MAX_COLUMNS &&
					(!p_tw || (Cursvcol += charsize(p[i - 1])) < p_tw) &&
					!(!no_abbr && !isidchar(c) && isidchar(p[i - 1])))
			p[i++] = vgetc();
		p[i] = '\0';
		insstr(p);
		AppendToRedobuff(p);
	}
	else
	{
		inschar(c);
		AppendCharToRedobuff(c);
	}

	updateline();
}


/*
 * start_arrow() is called when an arrow key is used in insert mode.
 * It resembles hitting the <ESC> key.
 */
	static void
start_arrow()
{
	if (!arrow_used)		/* something has been inserted */
	{
		AppendToRedobuff(ESC_STR);
		arrow_used = TRUE;		/* this means we stopped the current insert */
		stop_insert();
	}
}

/*
 * stop_arrow() is called before a change is made in insert mode.
 * If an arrow key has been used, start a new insertion.
 */
	static void
stop_arrow()
{
	if (arrow_used)
	{
		u_saveCurpos();			/* errors are ignored! */
		Insstart = Curpos;		/* new insertion starts here */
		ResetRedobuff();
		AppendToRedobuff("1i");	/* pretend we start an insertion */
		arrow_used = FALSE;
	}
}

/*
 * do a few things to stop inserting
 */
	static void
stop_insert()
{
	stop_redo_ins();

	/*
	 * save the inserted text for later redo with ^@
	 */
	free(last_insert);
	last_insert = get_inserted();
	last_insert_skip = new_insert_skip;

	/*
	 * If we just did an auto-indent, truncate the line, and put
	 * the cursor back.
	 */
	if (did_ai && !arrow_used)
	{
		*nr2ptr(Curpos.lnum) = NUL;
		canincrease(0);
		Curpos.col = 0;
		if (p_list)			/* the deletion is only seen in list mode */
			updateline();
	}
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
}

/*
 * oneright oneleft onedown oneup
 *
 * Move one char {right,left,down,up}.	Return TRUE when sucessful, FALSE when
 * we hit a boundary (of a line, or the file).
 */

	int
oneright()
{
	char *ptr;

	ptr = Curpos2ptr();
	set_want_col = TRUE;

	if (*ptr++ == NUL || *ptr == NUL)
		return FALSE;
	++Curpos.col;
	return TRUE;
}

	int
oneleft()
{
	set_want_col = TRUE;

	if (Curpos.col == 0)
		return FALSE;
	--Curpos.col;
	return TRUE;
}

	void
beginline(flag)
	int			flag;
{
	Curpos.col = 0;
	if (flag)
	{
		register char *ptr;

		for (ptr = nr2ptr(Curpos.lnum); isspace(*ptr); ++ptr)
			++Curpos.col;
	}
	set_want_col = TRUE;
}

	int
oneup(n)
	long n;
{
	if (n != 0 && Curpos.lnum == 1)
		return FALSE;
	if (n >= Curpos.lnum)
		Curpos.lnum = 1;
	else
		Curpos.lnum -= n;

	if (operator == NOP)
		cursupdate();				/* make sure Topline is valid */

	/* try to advance to the column we want to be at */
	coladvance(Curswant);
	return TRUE;
}

	int
onedown(n)
	long n;
{
	if (n != 0 && Curpos.lnum == line_count)
		return FALSE;
	Curpos.lnum += n;
	if (Curpos.lnum > line_count)
		Curpos.lnum = line_count;

	if (operator == NOP)
		cursupdate();				/* make sure Topline is valid */

	/* try to advance to the column we want to be at */
	coladvance(Curswant);
	return TRUE;
}

	int
onepage(dir, count)
	int		dir;
	long	count;
{
	linenr_t		lp;
	long			n;

	if (line_count == 1)	/* nothing to do */
		return FALSE;
	for ( ; count > 0; --count)
	{
		if (dir == FORWARD ? (Topline >= line_count - 1) : (Topline == 1))
		{
			beep();
			return FALSE;
		}
		if (dir == FORWARD)
		{
			if (Botline > line_count)				/* at end of file */
				Topline = line_count;
			else if (plines(Botline) >= Rows - 3 ||	/* next line is big */
					Botline - Topline <= 3)		/* just three lines on screen */
				Topline = Botline;
			else
				Topline = Botline - 2;
			Curpos.lnum = Topline;
			if (count != 1)
				comp_Botline();
		}
		else	/* dir == BACKWARDS */
		{
			lp = Topline;
			/*
			 * If the first two lines on the screen are not too big, we keep
			 * them on the screen.
			 */
			if ((n = plines(lp)) > Rows / 2)
				--lp;
			else if (lp < line_count && n + plines(lp + 1) < Rows / 2)
				++lp;
			Curpos.lnum = lp;
			n = 0;
			while (n <= Rows - 1 && lp >= 1)
			{
				n += plines(lp);
				--lp;
			}
			if (n <= Rows - 1)				/* at begin of file */
				Topline = 1;
			else if (lp >= Topline - 2)		/* happens with very long lines */
			{
				--Topline;
				comp_Botline();
				Curpos.lnum = Botline - 1;
			}
			else
				Topline = lp + 2;
		}
	}
	beginline(TRUE);
	updateScreen(VALID);
	return TRUE;
}

	void
stuff_inserted(c, count, no_esc)
	int		c;
	long	count;
	int		no_esc;
{
	u_char		*esc_ptr = NULL;
	u_char		*ptr;

	if (last_insert == NULL)
	{
		emsg("No inserted text yet");
		return;
	}
	if (c)
		stuffcharReadbuff(c);
	if (no_esc && (esc_ptr = (u_char *)strrchr((char *)last_insert, 27)) != NULL)
		*esc_ptr = NUL;		/* remove the ESC */

			/* skip the command */
	ptr = last_insert + last_insert_skip;

	do
		stuffReadbuff((char *)ptr);
	while (--count > 0);

	if (no_esc && esc_ptr)
		*esc_ptr = 27;		/* put the ESC back */
}

/*
 * Check the word in front of the cursor for an abbreviation.
 * Called when the non-id character "c" has been entered.
 * When an abbreviation is recognized it is removed from the text and
 * the replacement string is inserted in typestr, followed by "c".
 */
	static int
echeck_abbr(c)
	int c;
{
	if (p_paste || no_abbr)			/* no abbreviations or in paste mode */
		return FALSE;

	return check_abbr(c, nr2ptr(Curpos.lnum), Curpos.col,
				Curpos.lnum == Insstart.lnum ? Insstart.col : 0);
}
