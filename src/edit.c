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

int arrow_used;			/* Normally FALSE, set to TRUE after hitting
							 * cursor key in insert mode. Used by vgetorpeek()
							 * to decide when to call u_sync() */
int restart_edit;		/* call edit when next command finished */
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
	linenr_t	 lnum;
	int 		 temp = 0, mode;
	int			 nextc = 0;

#ifdef DIGRAPHS
	int			 inserted = 0;		/* last 'normal' inserted char */
	int			 backspaced = 0;	/* last backspace char */
#endif /* DIGRAPHS */

	if (restart_edit)
	{
		arrow_used = TRUE;
		restart_edit = FALSE;
	}
	else
		arrow_used = FALSE;

/*
 * Get the current length of the redo buffer, those characters have to be
 * skipped if we want to get to the inserted characters.
 */

	ptr = get_inserted();
	new_insert_skip = strlen((char *)ptr);
	free(ptr);

	for (;;)
	{
		if (arrow_used)		/* don't repeat insert when arrow key used */
			count = 0;

		cursupdate();		/* Figure out where the cursor is based on Curpos. */
		setcursor();
		if (nextc)			/* character remaining from CTRL-V */
		{
			c = nextc;
			nextc = 0;
		}
		else
			c = vgetc();

nextchar:
#ifdef DIGRAPHS
		if (p_dg)
		{
			if (backspaced)
				c = getdigraph(backspaced, c);
			backspaced = 0;
			if (c == BS && inserted)
				backspaced = inserted;
			else
				inserted = c;
		}
#endif /* DIGRAPHS */

		if (c == CTRL('V'))
		{
				outchar('^');
				AppendToRedobuff("\026");	/* CTRL-V */
				cursupdate();
				setcursor();

				c = get_literal(&nextc);

			/* erase the '^' */
				if ((cc = gcharCurpos()) == NUL)
					outchar(' ');
				else
					outstrn(transchar(cc));

				insertchar(c);
				continue;
		}
		switch (c)		/* handle character in insert mode */
		{
			  case CTRL('O'):		/* execute one command */
			  	count = 0;
				restart_edit = TRUE;
				/*FALLTHROUGH*/

			  case ESC: /* an escape ends input mode */
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
				if (Curpos.col != 0 && (!restart_edit || gcharCurpos() == NUL))
					decCurpos();
				State = NORMAL;
				script_winsize_pp();	/* may need to put :winsize in script */
					/* inchar() may have deleted the "INSERT" message */
				if (Recording)
					showmode();
				else if (p_mo)
					msg("");
				return;

			  	/*
				 * Insert the previously inserted text.
				 * Last_insert actually is a copy of the redo buffer, so we
				 * first have to remove the command.
				 * For ^@ the trailing ESC will end the insert.
				 */
			  case K_ZERO:
			  case CTRL('A'):
				stuff_inserted(NUL, 1L, (c == CTRL('A')));
				break;

			  	/*
				 * insert the contents of a register
				 */
			  case CTRL('B'):
			  	if (!insertbuf(vgetc()))
					beep();
				break;

				/*
				 * If the cursor is on an indent, ^T/^D insert/delete one
				 * shiftwidth. Otherwise ^T/^D behave like a TAB/backspace.
				 * This isn't completely compatible with
				 * vi, but the difference isn't very noticeable and now you can
				 * mix ^D/backspace and ^T/TAB without thinking about which one
				 * must be used.
				 */
			  case CTRL('T'):		/* make indent one shiftwidth greater */
			  case CTRL('D'): 		/* make indent one shiftwidth smaller */
				stop_arrow();
				AppendToRedobuff(mkstr(c));

					/* determine offset from first non-blank */
				temp = Curpos.col;
				beginline(TRUE);
				temp -= Curpos.col;

		  		shift_line(c == CTRL('D'));

					/* try to put cursor on same character */
				temp += Curpos.col;
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
				/* can't backup past first character in buffer */
				/* can't backup past starting point unless "backspace" > 1 */
				/* can backup to a previous line if "backspace" == 0 */
				if ((Curpos.lnum == 1 && Curpos.col <= 0) ||
						(p_bs < 2 && (arrow_used ||
							(Curpos.lnum == Insstart.lnum &&
							Curpos.col <= Insstart.col) ||
							(Curpos.col <= 0 && p_bs == 0))))
				{
					beep();
					goto redraw;
				}

				stop_arrow();
				if (Curpos.col <= 0)		/* delete newline! */
				{
					if (Curpos.lnum == Insstart.lnum)
					{
						if (!u_save((linenr_t)(Curpos.lnum - 2), (linenr_t)(Curpos.lnum + 1)))
							goto redraw;
						--Insstart.lnum;
						Insstart.col = 0;
					}
				/* in replace mode with 'repdel' off we only move the cursor */
					if (State != REPLACE || p_rd)
					{
						temp = gcharCurpos();		/* remember current char */
						--Curpos.lnum;
						dojoin(FALSE);
						if (temp == NUL)
							++Curpos.col;
					}
					else
						decCurpos();
					did_ai = FALSE;
				}
				else
				{
					/* delete upto starting point, start of line or previous word */
					do
					{
						decCurpos();
								/* start of word? */
						if (mode == 1 && !isspace(gcharCurpos()))
						{
							mode = 2;
							temp = isidchar(gcharCurpos());
						}
								/* end of word? */
						if (mode == 2 && isidchar(gcharCurpos()) != temp)
						{
							incCurpos();
							break;
						}
						if (State != REPLACE || p_rd)
							delchar(TRUE);
						if (mode == 0)		/* just a single backspace */
							break;
					} while (Curpos.col > 0 && (Curpos.lnum != Insstart.lnum ||
							Curpos.col != Insstart.col));
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
				AppendToRedobuff(mkstr(c));
				if (vpeekc() == BS)
				{
						c = vgetc();
						goto nextbs;	/* speedup multiple backspaces */
				}
redraw:
				cursupdate();
				updateline();
				break;

			  case CTRL('W'):
			  	/* delete word before cursor */
			  	mode = 1;
			  	goto dodel;

			  case CTRL('U'):
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
					fwd_word(1L, 0);
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
			  	if (!p_et)
					goto normalchar;
										/* expand a tab into spaces */
				stop_arrow();
				did_ai = FALSE;
				did_si = FALSE;
				can_si = FALSE;
				insstr("                " + 16 - (p_ts - Curpos.col % p_ts));
				AppendToRedobuff("\t");
				goto redraw;

			  case CR:
			  case NL:
				stop_arrow();
				if (State == REPLACE)           /* DMT added, 12/89 */
					delchar(FALSE);
				AppendToRedobuff(NL_STR);
				if (!Opencmd(FORWARD, TRUE))
					goto doESCkey;		/* out of memory */
				break;

			  case CTRL('R'):
				/*
				 * addition by mool: copy from previous line
				 */
				lnum = Curpos.lnum - 1;
				goto copychar;

			  case CTRL('E'):
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
					shift_line(TRUE);
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
	}
	else if (cc == 0)		/* NUL is stored as NL */
		cc = '\n';

	State = oldstate;
	*nextc = nc;
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
	int		must_redraw = FALSE;

	stop_arrow();
	/*
	 * If the cursor is past 'textwidth' and we are inserting a non-space,
	 * try to break the line in two or more pieces. If c == NUL then we have
	 * been called to do formatting only.
	 */
	if (c == NUL || !isspace(c))
	{
		while (Cursvcol >= p_tw)
		{
			int		startcol;		/* Cursor column at entry */
			int		wantcol;		/* column at textwidth border */
			int		foundcol;		/* column for start of word */
			int		mincol;			/* minimum column for break */

			if ((startcol = Curpos.col) == 0)
				break;
			coladvance((int)p_tw);	/* find column of textwidth border */
			wantcol = Curpos.col;
			beginline((int)p_ai);			/* find start of text */
			mincol = Curpos.col;

			Curpos.col = startcol - 1;
			foundcol = 0;
			while (Curpos.col > mincol)	/* find position to break at */
			{
				if (isspace(gcharCurpos()))
				{
					foundcol = Curpos.col + 1;
					while (Curpos.col > 1 && isspace(gcharCurpos()))
						--Curpos.col;
					if (Curpos.col < wantcol)
						break;
				}
				--Curpos.col;
			}

			if (foundcol == 0)	/* no spaces, cannot break line */
			{
				Curpos.col = startcol;
				break;
			}
			Curpos.col = foundcol;
			startcol -= Curpos.col;
			Opencmd(FORWARD, FALSE);
			Curpos.col += startcol;
			curs_columns();			/* update Cursvcol */
			must_redraw = TRUE;
		}
		if (c == NUL)		/* formatting only */
			return;
		if (must_redraw)
			updateScreen(CURSUPD);
	}

	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;

	/*
	 * If there's any pending input, grab up to MAX_COLUMNS at once.
	 * This speeds up normal text input considerably.
	 */
	if (vpeekc() != NUL && State != REPLACE)
	{
		char			p[MAX_COLUMNS + 1];
		int 			i;

		p[0] = c;
		i = 1;
		while ((c = vpeekc()) != NUL && !ISSPECIAL(c) && i < MAX_COLUMNS &&
					(Cursvcol += charsize(p[i - 1])) < p_tw)
			p[i++] = vgetc();
		p[i] = '\0';
		insstr(p);
		AppendToRedobuff(p);
	}
	else
	{
		inschar(c);
		AppendToRedobuff(mkstr(c));
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
		ResetBuffers();
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

	ptr = pos2ptr(&Curpos);
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
		lp = Topline;
		n = 0;
		if (dir == BACKWARD)
		{
			if (lp < line_count)
				++lp;
			Curpos.lnum = lp;
		}
		while (n < Rows - 1 && lp >= 1 && lp <= line_count)
		{
			n += plines(lp);
			lp += dir;
		}
		if (dir == FORWARD)
		{
			if (--lp > 1)
				--lp;
			Topline = Curpos.lnum = lp;
		}
		else
			Topline = lp + 1;
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
		beep();
		return;
	}
	if (c)
		stuffReadbuff(mkstr(c));
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
