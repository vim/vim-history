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
#include "ops.h"	/* for operator */

extern u_char *get_inserted();
static void insertchar __ARGS((unsigned));

	void
edit(count)
	long count;
{
	u_char		 c;
	u_char		*ptr;
	linenr_t	 lnum;
	bool_t		 literal_next_flag = FALSE;
	int 		 temp;
	static u_char	*last_insert = NULL;
	int			 oldstate;

	for (;;)
	{
		cursupdate();		/* Figure out where the cursor is based on Curpos. */
		windgoto(Cursrow, Curscol);
		c = vgetc();

		if (c == CTRL('V') && !literal_next_flag)
		{
				literal_next_flag = TRUE;
				outchar('^');
				AppendToRedobuff("\x16");
				oldstate = State;
				State = NOMAPPING;		/* next character not mapped */
				continue;
		}
		if (literal_next_flag)
		{
				u_char cc;

				literal_next_flag = FALSE;
				State = oldstate;
				if ((cc = gcharCurpos()) == NUL)
					outchar(' ');
				else
					outstr(transchar(cc));	/* erase the '^' */
				insertchar(c);
				continue;
		}
		switch (c)		/* We're in insert mode */
		{
			  case ESC: /* an escape ends input mode */
		doESCkey:
				AppendToRedobuff(ESC_STR);

				if (--count > 0)		/* repeat what was typed */
				{
						start_redo_ins();
						continue;
				}
				stop_redo_ins();

				/*
				 * save the inserted text for later redo with ^@
				 */
				free(last_insert);
				last_insert = get_inserted();

				/*
				 * If we just did an auto-indent, truncate the line, and put
				 * the cursor back.
				 */
				if (did_ai)
				{
					*nr2ptr(Curpos.lnum) = NUL;
					canincrease(0);
					Curpos.col = 0;
					did_ai = FALSE;
					did_si = FALSE;
					can_si = FALSE;
				}
				set_want_col = TRUE;

				/*
				 * The cursor should end up on the last inserted character.
				 * This is an attempt to match the real 'vi', but it may not
				 * be quite right yet.
				 */
				if (Curpos.col != 0)
						decCurpos();
				State = NORMAL;
				script_winsize_pp();
				if (Recording)
						showmode();
				else
						msg("");
				return;

			  	/*
				 * Insert the previously inserted text.
				 * Last_insert actually is a copy of the redo buffer, so we
				 * first have to remove the command.
				 * The trailing ESC will end the insert.
				 */
			  case K_ZERO:
				if (last_insert == NULL)
				{
					beep();
					break;
				}
				ptr = last_insert;
				while (*ptr && strchr("AIROaio", *ptr++) == NULL)
					;
				stuffReadbuff((char *)ptr);
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
			  	if (inindent())
			  	{
			  		shift_line(0);
					AppendToRedobuff("\024");
					did_si = FALSE;
					can_si = FALSE;
			  		goto redraw;
			  	}
				c = TAB;
				goto normalchar;

			  case CTRL('D'): 	/* make indent one shiftwidth smaller */

				if (inindent())			/* delete one shiftwidth */
				{
					AppendToRedobuff("\04");
					shift_line((bool_t)TRUE);
					did_si = FALSE;
					can_si = FALSE;
					if (Curpos.col < 1)
						did_ai = FALSE;
					goto redraw;
				}
				/*FALLTHROUGH*/

			  case BS:
			  case DEL:
nextbs:
				/* can't backup past starting point */
				/* can backup to a previous line if backspace option is set */
				if (Curpos.lnum == Insstart.lnum && Curpos.col <= Insstart.col
							|| Curpos.col <= 0 && !P(P_BS))
				{
					beep();
					goto redraw;
				}
				did_si = FALSE;
				can_si = FALSE;
				if (Curpos.col <= 1)
						did_ai = FALSE;
				if (Curpos.col <= 0)		/* delete newline! */
				{
					temp = gcharCurpos();		/* remember current char */
					--Curpos.lnum;
					dojoin((bool_t)FALSE);
					if (temp == NUL)
						++Curpos.col;
				}
				else
				{
					decCurpos();
					if (State != REPLACE || P(P_RD))
						delchar((bool_t)TRUE);
				}
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
			  	if (Curpos.col == 0)	/* delete current line */
			  		goto nextbs;

			  	c = 1;
			  	/*FALLTHROUGH*/

			  case CTRL('U'):
				/* delete upto starting point, previous line or previous word */
				while ((Curpos.lnum == Insstart.lnum) ? (Curpos.col > Insstart.col) :
								(Curpos.col > 0))
				{
						decCurpos();
						if (c == 1 && !isspace(gcharCurpos()))
						{
							c = 2;
							temp = isidchar(gcharCurpos());
						}
						if (c == 2 && isidchar(gcharCurpos()) != temp)
						{
							incCurpos();
							break;
						}
						delchar((bool_t)TRUE);
				}
				did_ai = FALSE;
				did_si = FALSE;
				can_si = FALSE;
				/*
				 * It's a little strange to put ctrl-U into the redo
				 * buffer, but it makes auto-indent a lot easier to deal
				 * with.
				 */
				AppendToRedobuff(mkstr(c));
				goto redraw;

			  case TAB:
			  	if (!P(P_ET))
					goto normalchar;
										/* expand a tab into spaces */
				did_ai = FALSE;
				did_si = FALSE;
				can_si = FALSE;
				insstr("                " + 16 - (P(P_TS) - Curpos.col % P(P_TS)));
				AppendToRedobuff("\t");
				goto redraw;

			  case CR:
			  case NL:
				if (State == REPLACE)           /* DMT added, 12/89 */
					delchar((bool_t)FALSE);
				AppendToRedobuff(NL_STR);
				if (!Opencmd(FORWARD))
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
				if (Curpos.col > 0 && (can_si && c == '}' || did_si && c == '{'))
					shift_line((bool_t)TRUE);
				insertchar(c);
				break;
			}
	}
}

/*
 * Special characters in this context are those that need processing other
 * than the simple insertion that can be performed here. This includes ESC
 * which terminates the insert, and CR/NL which need special processing to
 * open up a new line. This routine tries to optimize insertions performed by
 * the "redo", "undo" or "put" commands, so it needs to know when it should
 * stop and defer processing to the "normal" mechanism.
 */
#define ISSPECIAL(c)	((c) < ' ')

	static void
insertchar(c)
	unsigned	c;
{
	/*
	 * If the cursor is past 'textwidth' and we are inserting a non-space,
	 * try to break the line in two or more pieces.
	 */
	if (!isspace(c))
		while (Cursvcol > P(P_TW))
		{
			int		startcol;		/* Cursor column at entry */
			int		wantcol;		/* column at textwidth border */
			int		foundcol;		/* column for start of word */
			int		mincol;			/* minimum column for break */

			if ((startcol = Curpos.col) == 0)
				break;
			coladvance((int)P(P_TW));	/* find column of textwidth border */
			wantcol = Curpos.col;
			beginline((int)P(P_AI));			/* find start of text */
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
			Opencmd(FORWARD);
			Curpos.col += startcol;
			cursupdate();			/* update Cursvcol */
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
					(Cursvcol += charsize(p[i - 1])) < P(P_TW))
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
 * oneright oneleft onedown oneup
 *
 * Move one char {right,left,down,up}.	Return TRUE when sucessful, FALSE when
 * we hit a boundary (of a line, or the file).
 */

	bool_t
oneright()
{
	char *ptr;

	ptr = nr2ptr(Curpos.lnum) + Curpos.col;
	set_want_col = TRUE;

	if (*ptr++ == NUL || *ptr == NUL)
		return FALSE;
	++Curpos.col;
	return TRUE;
}

	bool_t
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
	bool_t			flag;
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

	bool_t
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

	bool_t
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
