/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * edit.c: functions for insert mode
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"
#include "ops.h"	/* for operator */

extern char_u *get_inserted();
static void start_arrow __ARGS((void));
static void stop_arrow __ARGS((void));
static void stop_insert __ARGS((void));
static int echeck_abbr __ARGS((int));

int arrow_used;				/* Normally FALSE, set to TRUE after hitting
							 * cursor key in insert mode. Used by vgetorpeek()
							 * to decide when to call u_sync() */
int restart_edit = 0;		/* call edit when next command finished */
static char_u	*last_insert = NULL;
							/* the text of the previous insert */
static int		last_insert_skip;
							/* number of chars in front of the previous insert */
static int		new_insert_skip;
							/* number of chars in front of the current insert */

	void
edit(count)
	long count;
{
	int			 c;
	int			 cc;
	char_u		*ptr;
	char_u		*saved_line = NULL;		/* saved line for replace mode */
	linenr_t	 saved_lnum = 0;		/* lnum of saved line */
	int			 saved_char = NUL;		/* char replaced by NL */
	linenr_t	 lnum;
	int 		 temp = 0;
	int			 mode;
	int			 nextc = 0;
	int			 lastc = 0;
	colnr_t		 mincol;
	static linenr_t o_lnum = 0;
	static int	 o_eol = FALSE;
#ifdef WEBB_KEYWORD_COMPL
	FPOS		 complete_pos;
	FPOS		 first_match;
	char_u		 *complete_pat = NULL;
	char_u		 *completion_str = NULL;
	char_u		 *last_completion_str = NULL;
	char_u		 *tmp_ptr;
	int			 previous_c = 0;
	int			 complete_col = 0;			/* init for gcc */
	int			 complete_direction;
	int			 complete_any_word = 0;		/* true -> ^N/^P hit with no prefix
											 * init for gcc */
	int			 done;
	int			 found_error = FALSE;
	char_u		 backup_char = 0;			/* init for gcc */

	c = NUL;
#endif /* WEBB_KEYWORD_COMPL */

	if (restart_edit)
	{
		arrow_used = TRUE;
		restart_edit = 0;
		/*
		 * If the cursor was after the end-of-line before the CTRL-O
		 * and it is now at the end-of-line, put it after the end-of-line
		 * (this is not correct in very rare cases).
		 */
		if (o_eol && curwin->w_cursor.lnum == o_lnum &&
				*((ptr = ml_get(curwin->w_cursor.lnum)) + curwin->w_cursor.col) != NUL &&
				*(ptr + curwin->w_cursor.col + 1) == NUL)
			++curwin->w_cursor.col;
	}
	else
	{
		arrow_used = FALSE;
		o_eol = FALSE;
	}

#ifdef DIGRAPHS
	dodigraph(-1);					/* clear digraphs */
#endif

/*
 * Get the current length of the redo buffer, those characters have to be
 * skipped if we want to get to the inserted characters.
 */

	ptr = get_inserted();
	new_insert_skip = STRLEN(ptr);
	free(ptr);

	old_indent = 0;

	for (;;)
	{
		if (arrow_used)		/* don't repeat insert when arrow key used */
			count = 0;

		if (!arrow_used)
			curwin->w_set_curswant = TRUE;	/* set curwin->w_curswant for next K_DARROW or K_UARROW */
		cursupdate();		/* Figure out where the cursor is based on curwin->w_cursor. */
		showruler(0);
		setcursor();
#ifdef WEBB_KEYWORD_COMPL
		previous_c = c;
#endif /* WEBB_KEYWORD_COMPL */
		if (nextc)			/* character remaining from CTRL-V */
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
#ifdef WEBB_KEYWORD_COMPL
		if (previous_c == Ctrl('N') || previous_c == Ctrl('P'))
		{
			/* Show error message from attempted keyword completion (probably
			 * 'Pattern not found') until another key is hit, then go back to
			 * showing what mode we are in.
			 */
			showmode();
			if (c != Ctrl('N') && c != Ctrl('P'))
			{
				/* Get here when we have finished typing a sequence of ^N and
				 * ^P. Free up memory that was used, and make sure we can redo
				 * the insert.
				 */
				if (completion_str != NULL)
					AppendToRedobuff(completion_str);
				free(complete_pat);
				free(completion_str);
				free(last_completion_str);
				complete_pat = completion_str = last_completion_str = NULL;
			}
		}
#endif /* WEBB_KEYWORD_COMPL */
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
		if (State == REPLACE && saved_lnum != curwin->w_cursor.lnum)
		{
			free(saved_line);
			saved_line = strsave(ml_get(curwin->w_cursor.lnum));
			saved_lnum = curwin->w_cursor.lnum;
		}

#ifdef DIGRAPHS
		c = dodigraph(c);
#endif /* DIGRAPHS */

		if (c == Ctrl('V'))
		{
			screen_start();
			screen_outchar('^', curwin->w_row, curwin->w_col);
			AppendToRedobuff((char_u *)"\026");	/* CTRL-V */
			cursupdate();
			setcursor();

			c = get_literal(&nextc);

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
				o_lnum = curwin->w_cursor.lnum;
				o_eol = (gchar_cursor() == NUL);
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
							(void)start_redo_ins();
							continue;
					}
					stop_insert();
				}
				if (!restart_edit)
					curwin->w_set_curswant = TRUE;

				/*
				 * The cursor should end up on the last inserted character.
				 */
				if (curwin->w_cursor.col != 0 && (!restart_edit || gchar_cursor() == NUL) && !p_ri)
					dec_cursor();
				if (extraspace)			/* did reverse replace in column 0 */
				{
					(void)delchar(FALSE);
					updateline();
					extraspace = FALSE;
				}
				State = NORMAL;
					/* inchar() may have deleted the "INSERT" message */
				if (Recording)
					showmode();
				else if (p_smd)
					MSG("");
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
			  	if (insertbuf(vgetc()) == FAIL)
					beep();
				break;

			  case Ctrl('B'):			/* toggle reverse insert mode */
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
				if ((lastc == '0' || lastc == '^') && curwin->w_cursor.col)
				{
					--curwin->w_cursor.col;
					(void)delchar(FALSE);			/* delete the '^' or '0' */
					if (lastc == '^')
						old_indent = get_indent();	/* remember current indent */

						/* determine offset from first non-blank */
					temp = curwin->w_cursor.col;
					beginline(TRUE);
					temp -= curwin->w_cursor.col;
					set_indent(0, TRUE);	/* remove all indent */
				}
				else
				{
ins_indent:
						/* determine offset from first non-blank */
					temp = curwin->w_cursor.col;
					beginline(TRUE);
					temp -= curwin->w_cursor.col;

					shift_line(c == Ctrl('D'), TRUE, 1);

						/* try to put cursor on same character */
					temp += curwin->w_cursor.col;
				}
				if (temp <= 0)
					curwin->w_cursor.col = 0;
				else
					curwin->w_cursor.col = temp;
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
						((curwin->w_cursor.lnum == 1 && curwin->w_cursor.col <= 0) ||
						(p_bs < 2 && (arrow_used ||
							(curwin->w_cursor.lnum == Insstart.lnum &&
							curwin->w_cursor.col <= Insstart.col) ||
							(curwin->w_cursor.col <= 0 && p_bs == 0))))))
				{
					beep();
					goto redraw;
				}

				stop_arrow();
				if (p_ri)
					inc_cursor();
				if (curwin->w_cursor.col <= 0)		/* delete newline! */
				{
					lnum = Insstart.lnum;
					if (curwin->w_cursor.lnum == Insstart.lnum || p_ri)
					{
						if (!u_save((linenr_t)(curwin->w_cursor.lnum - 2), (linenr_t)(curwin->w_cursor.lnum + 1)))
							goto redraw;
						--Insstart.lnum;
						Insstart.col = 0;
						/* this is in xvim, why?
						if (curbuf->b_p_ai)
							for (ptr = ml_get(Insstart.lnum);
											iswhite(*ptr++); Insstart.col++)
								; */
					}
				/* in replace mode, in the line we started replacing, we
														only move the cursor */
					if (State != REPLACE || curwin->w_cursor.lnum > lnum)
					{
						temp = gchar_cursor();		/* remember current char */
						--curwin->w_cursor.lnum;
						(void)dojoin(FALSE, TRUE);
						if (temp == NUL && gchar_cursor() != NUL)
							++curwin->w_cursor.col;
						if (saved_char)				/* restore what NL replaced */
						{
							State = NORMAL;			/* no replace for this char */
							inschar(saved_char);	/* but no showmatch */
							State = REPLACE;
							saved_char = NUL;
							if (!p_ri)
								dec_cursor();
						}
						else if (p_ri)				/* in reverse mode */
							saved_lnum = 0;			/* save this line again */
					}
					else
						dec_cursor();
					did_ai = FALSE;
				}
				else
				{
					if (p_ri && State != REPLACE)
						dec_cursor();
					mincol = 0;
					if (mode == 3 && !p_ri && curbuf->b_p_ai)	/* keep indent */
					{
						temp = curwin->w_cursor.col;
						beginline(TRUE);
						if (curwin->w_cursor.col < temp)
							mincol = curwin->w_cursor.col;
						curwin->w_cursor.col = temp;
					}

					/* delete upto starting point, start of line or previous word */
					do
					{
						if (!p_ri)
							dec_cursor();

								/* start of word? */
						if (mode == 1 && !isspace(gchar_cursor()))
						{
							mode = 2;
							temp = isidchar(gchar_cursor());
						}
								/* end of word? */
						else if (mode == 2 && (isspace(cc = gchar_cursor()) || isidchar(cc) != temp))
						{
							if (!p_ri)
								inc_cursor();
							else if (State == REPLACE)
								dec_cursor();
							break;
						}
						if (State == REPLACE)
						{
							if (saved_line)
							{
								if (extraspace)
								{
									if ((int)STRLEN(ml_get(curwin->w_cursor.lnum)) - 1 > (int)STRLEN(saved_line))
										(void)delchar(FALSE);
									else
									{
										dec_cursor();
										(void)delchar(FALSE);
										extraspace = FALSE;
										pchar_cursor(*saved_line);
									}
								}
								else if (curwin->w_cursor.col < STRLEN(saved_line))
									pchar_cursor(saved_line[curwin->w_cursor.col]);
								else if (!p_ri)
									(void)delchar(FALSE);
							}
						}
						else  /* State != REPLACE */
						{
							(void)delchar(FALSE);
							if (p_ri && gchar_cursor() == NUL)
								break;
						}
						if (mode == 0)		/* just a single backspace */
							break;
						if (p_ri && State == REPLACE && inc_cursor())
							break;
					} while (p_ri || (curwin->w_cursor.col > mincol && (curwin->w_cursor.lnum != Insstart.lnum ||
							curwin->w_cursor.col != Insstart.col)));
					if (extraspace)
						dec_cursor();
				}
				did_si = FALSE;
				can_si = FALSE;
				if (curwin->w_cursor.col <= 1)
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
			  	if (oneleft() == OK)
				{
					start_arrow();
				}
					/* if 'whichwrap' set for cursor in insert mode may go
					 * to previous line */
				else if ((p_ww & 16) && curwin->w_cursor.lnum > 1)
				{
					start_arrow();
					--(curwin->w_cursor.lnum);
					coladvance(MAXCOL);
					curwin->w_curswant = MAXCOL;	/* so we stay at the end */
				}
				else
					beep();
				break;

			  case K_SLARROW:
			  	if (curwin->w_cursor.lnum > 1 || curwin->w_cursor.col > 0)
				{
					bck_word(1L, 0);
					start_arrow();
				}
				else
					beep();
				break;

			  case K_RARROW:
				if (gchar_cursor() != NUL)
				{
					curwin->w_set_curswant = TRUE;
					start_arrow();
					++curwin->w_cursor.col;
				}
					/* if 'whichwrap' set for cursor in insert mode may go
					 * to next line */
				else if ((p_ww & 16) && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
				{
					curwin->w_set_curswant = TRUE;
					start_arrow();
					++curwin->w_cursor.lnum;
					curwin->w_cursor.col = 0;
				}
				else
					beep();
				break;

			  case K_SRARROW:
			  	if (curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count || gchar_cursor() != NUL)
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
			  	if ((!curbuf->b_p_et && !(p_sta && inindent())) || (p_ri && State == REPLACE))
					goto normalchar;

				AppendToRedobuff((char_u *)"\t");

				if (!curbuf->b_p_et)				/* insert smart tab */
					goto ins_indent;

										/* p_te set: expand a tab into spaces */
				stop_arrow();
				did_ai = FALSE;
				did_si = FALSE;
				can_si = FALSE;
				if (p_sta && inindent())
					temp = (int)curbuf->b_p_sw;
				else
					temp = (int)curbuf->b_p_ts;
				temp -= curwin->w_cursor.col % temp;
				inschar(' ');			/* delete one char in replace mode */
				while (--temp)
					insstr((char_u *)" ");		/* insstr does not delete chars */
				goto redraw;

			  case CR:
			  case NL:
			    if (echeck_abbr(c + 0x100))
					break;
				stop_arrow();
				if (State == REPLACE)
				{
					saved_char = gchar_cursor();
					(void)delchar(FALSE);
				}
				/*
				 * When 'autoindent' set delete white space after the cursor.
				 * Vi does this, although it probably does it implicitly due
				 * to the way it does auto-indent -- webb.
				 */
				if (curbuf->b_p_ai || curbuf->b_p_si)
					while ((c = gchar_cursor()) == ' ' || c == TAB)
						(void)delchar(FALSE);

				AppendToRedobuff(NL_STR);
				if (!Opencmd(FORWARD, TRUE, State == INSERT))
					goto doESCkey;		/* out of memory */
				if (p_ri)
				{
					dec_cursor();
					if (State == REPLACE && curwin->w_cursor.col > 0)
						dec_cursor();
				}
				break;

#ifdef DIGRAPHS
			  case Ctrl('K'):
				screen_start();
				screen_outchar('?', curwin->w_row, curwin->w_col);
				setcursor();
			  	c = vgetc();
				if (c != ESC)
				{
					if (charsize(c) == 1)
					{
						screen_start();
						screen_outchar(c, curwin->w_row, curwin->w_col);
					}
					setcursor();
					cc = vgetc();
					if (cc != ESC)
					{
						AppendToRedobuff((char_u *)"\026");	/* CTRL-V */
						c = getdigraph(c, cc, TRUE);
						goto normalchar;
					}
				}
				updateline();
				goto doESCkey;
#endif /* DIGRAPHS */

#ifdef WEBB_KEYWORD_COMPL
			  case Ctrl('P'):			/* Do previous pattern completion */
			  case Ctrl('N'):			/* Do next pattern completion */
				if (c == Ctrl('P'))
					complete_direction = BACKWARD;
				else
					complete_direction = FORWARD;
				if (previous_c != Ctrl('N') && previous_c != Ctrl('P'))
				{
					/* First time we hit ^N or ^P (in a row, I mean) */
					complete_pos = curwin->w_cursor;
					ptr = ml_get(complete_pos.lnum);
					complete_col = complete_pos.col;
					temp = complete_col - 1;
					if (temp < 0 || !isidchar(ptr[temp]))
					{
						complete_pat = strsave((char_u *)"\\<[a-zA-Z_]");
						complete_any_word = TRUE;
					}
					else
					{
						while (temp >= 0 && isidchar(ptr[temp]))
							temp--;
						temp++;
						complete_pat = alloc(curwin->w_cursor.col - temp + 3);
						if (complete_pat != NULL)
							sprintf((char *)complete_pat, "\\<%.*s",
								(int)(curwin->w_cursor.col - temp), ptr + temp);
						complete_any_word = FALSE;
					}
					last_completion_str = strsave((char_u *)" ");
				}
				else
				{
					/* This is not the first ^N or ^P we have hit in a row */
					while (curwin->w_cursor.col != complete_col)
					{
						curwin->w_cursor.col--;
						delchar(FALSE);
					}
					if (completion_str != NULL)
					{
						free(last_completion_str);
						last_completion_str = strsave(completion_str);
					}
				}
				if (complete_pat == NULL || last_completion_str == NULL)
				{
					found_error = TRUE;
					break;
				}
				if (!complete_any_word)
				{
					ptr = ml_get(curwin->w_cursor.lnum);
					backup_char = ptr[complete_col - 1];
					ptr[complete_col - 1] = ' ';
				}
				done = FALSE;
				found_error = FALSE;
				first_match.lnum = 0;
				keep_old_search_pattern = TRUE;
				while (!done)
				{
					if (complete_direction == BACKWARD)
					{
						ptr = ml_get(complete_pos.lnum);
						while (isidchar(ptr[complete_pos.col]))
							complete_pos.col--;
						complete_pos.col++;
					}
					if (!searchit(&complete_pos, complete_direction,
						complete_pat, 1L, TRUE, TRUE))
					{
						found_error = TRUE;
						break;
					}
					if (complete_any_word)
						ptr = ml_get_pos(&complete_pos);
					else
						ptr = ml_get_pos(&complete_pos) + 1;
					tmp_ptr = ptr;
					temp = 1;
					while (*tmp_ptr != NUL && isidchar(*tmp_ptr++))
						temp++;
					free (completion_str);
					tmp_ptr = completion_str = alloc(temp);
					if (completion_str == NULL)
					{
						found_error = TRUE;
						break;
					}
					while (*ptr != NUL && isidchar(*ptr))
						*(tmp_ptr++) = *(ptr++);
					*tmp_ptr = NUL;
					if (completion_str[0] != NUL &&
							STRCMP(completion_str, last_completion_str) != 0)
						done = TRUE;
					else if (first_match.lnum == 0)
					{
						first_match.lnum = complete_pos.lnum;
						first_match.col = complete_pos.col;
					}
					else if (complete_pos.lnum == first_match.lnum
						 && complete_pos.col == first_match.col)
					{
						if (completion_str[0] == NUL)
							EMSG("Exact match only");
						else
							EMSG("No other matches");
						done = TRUE;
					}
				}
				if (!found_error)
					insstr(completion_str);
				if (!complete_any_word)
				{
					ptr = ml_get(curwin->w_cursor.lnum);
					ptr[complete_col - 1] = backup_char;
				}
				keep_old_search_pattern = FALSE;
				updateline();
				break;
#endif /* WEBB_KEYWORD_COMPL */

			  case Ctrl('Y'):				/* copy from previous line */
				lnum = curwin->w_cursor.lnum - 1;
				goto copychar;

			  case Ctrl('E'):				/* copy from next line */
				lnum = curwin->w_cursor.lnum + 1;
copychar:
				if (lnum < 1 || lnum > curbuf->b_ml.ml_line_count)
				{
					beep();
					break;
				}

				/* try to advance to the cursor column */
				temp = 0;
				ptr = ml_get(lnum);
				while (temp < curwin->w_virtcol && *ptr)
						temp += chartabsize(*ptr++, (long)temp);

				if (temp > curwin->w_virtcol)
						--ptr;
				if ((c = *ptr) == NUL)
				{
					beep();
					break;
				}

				/*FALLTHROUGH*/
			  default:
normalchar:
				/*
				 * do some very smart indenting when entering '{' or '}' or '#'
				 */
				if (curwin->w_cursor.col > 0 && ((can_si && c == '}') || (did_si && c == '{')))
				{
					FPOS	*pos, old_pos;
					int		i;

						/* for '}' set indent equal to matching '{' */
					if (c == '}' && (pos = showmatch('{')) != NULL)
					{
						old_pos = curwin->w_cursor;
						curwin->w_cursor = *pos;
						i = get_indent();
						curwin->w_cursor = old_pos;
						set_indent(i, TRUE);
					}
					else
						shift_line(TRUE, TRUE, 1);
				}
					/* set indent of '#' always to 0 */
				if (curwin->w_cursor.col > 0 && can_si && c == '#')
				{
								/* remember current indent for next line */
					old_indent = get_indent();
					set_indent(0, TRUE);
				}

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
	int			 cc;
	int			 nc;
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
	int		textwidth;

	stop_arrow();

	/*
	 * find out textwidth to be used:
	 *	if 'textwidth' option is set, use it
	 *	else if 'wrapmargin' option is set, use Columns - 'wrapmargin'
	 *	if invalid value, us 0.
	 */
	textwidth = curbuf->b_p_tw;
	if (textwidth == 0 && curbuf->b_p_wm)
		textwidth = Columns - curbuf->b_p_wm;
	if (textwidth < 0)
		textwidth = 0;

	/*
	 * If the cursor is past 'textwidth' and we are inserting a non-space,
	 * try to break the line in two or more pieces. If c == NUL then we have
	 * been called to do formatting only. If textwidth == 0 it does nothing.
	 * Don't do this if an existing character is being replaced.
	 */
	if (c == NUL || !(isspace(c) || (State == REPLACE && *ml_get_cursor() != NUL)))
	{
		while (textwidth && curwin->w_virtcol >= textwidth)
		{
			int		startcol;		/* Cursor column at entry */
			int		wantcol;		/* column at textwidth border */
			int		foundcol;		/* column for start of word */

			if ((startcol = curwin->w_cursor.col) == 0)
				break;
			coladvance(textwidth);			/* find column of textwidth border */
			wantcol = curwin->w_cursor.col;

			curwin->w_cursor.col = startcol - 1;
			foundcol = 0;
			while (curwin->w_cursor.col > 0)			/* find position to break at */
			{
				if (isspace(gchar_cursor()))
				{
					while (curwin->w_cursor.col > 0 && isspace(gchar_cursor()))
						--curwin->w_cursor.col;
					if (curwin->w_cursor.col == 0)	/* only spaces in front of text */
						break;
					foundcol = curwin->w_cursor.col + 1;
					if (curwin->w_cursor.col < wantcol)
						break;
				}
				--curwin->w_cursor.col;
			}

			if (foundcol == 0)			/* no spaces, cannot break line */
			{
				curwin->w_cursor.col = startcol;
				break;
			}
			curwin->w_cursor.col = foundcol;		/* put cursor after pos. to break line */
			startcol -= foundcol;
			Opencmd(FORWARD, FALSE, FALSE);
			while (isspace(gchar_cursor()) && startcol)		/* delete blanks */
			{
				(void)delchar(FALSE);
				--startcol;				/* adjust cursor pos. */
			}
			curwin->w_cursor.col += startcol;
			curs_columns(FALSE);		/* update curwin->w_virtcol */
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
 			if (curwin->w_cursor.lnum == curwin->w_botline && !curwin->w_empty_rows)
				win_del_lines(curwin, 0, 1, TRUE, TRUE);
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
		char_u			p[MAX_COLUMNS + 1];
		int 			i;

		p[0] = c;
		i = 1;
		while ((c = vpeekc()) != NUL && !ISSPECIAL(c) && i < MAX_COLUMNS &&
					(textwidth == 0 || (curwin->w_virtcol += charsize(p[i - 1])) < textwidth) &&
					!(!no_abbr && !isidchar(c) && isidchar(p[i - 1])))
			p[i++] = vgetc();
#ifdef DIGRAPHS
		dodigraph(-1);					/* clear digraphs */
		dodigraph(p[i-1]);				/* may be the start of a digraph */
#endif
		p[i] = '\0';
		insstr(p);
		AppendToRedobuff(p);
	}
	else
	{
		inschar(c);
		AppendCharToRedobuff(c);
	}

	/*
	 * TODO: If the cursor has shifted past the end of the screen, should
	 * adjust the screen display. Avoids extra redraw.
	 */

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
		u_save_cursor();			/* errors are ignored! */
		Insstart = curwin->w_cursor;		/* new insertion starts here */
		ResetRedobuff();
		AppendToRedobuff((char_u *)"1i");	/* pretend we start an insertion */
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
		ml_replace(curwin->w_cursor.lnum, (char_u *)"", TRUE);
		curwin->w_cursor.col = 0;
		if (curwin->w_p_list)			/* the deletion is only seen in list mode */
			updateline();
	}
	did_ai = FALSE;
	did_si = FALSE;
	can_si = FALSE;
}

/*
 * move cursor to start of line
 * if flag == TRUE move to first non-white
 */
	void
beginline(flag)
	int			flag;
{
	curwin->w_cursor.col = 0;
	if (flag)
	{
		register char_u *ptr;

		for (ptr = ml_get(curwin->w_cursor.lnum); iswhite(*ptr); ++ptr)
			++curwin->w_cursor.col;
	}
	curwin->w_set_curswant = TRUE;
}

/*
 * oneright oneleft onedown oneup
 *
 * Move one char {right,left,down,up}.
 * Return OK when sucessful, FAIL when we hit a line of file boundary.
 */

	int
oneright()
{
	char_u *ptr;

	ptr = ml_get_cursor();
	if (*ptr++ == NUL || *ptr == NUL)
		return FAIL;
	curwin->w_set_curswant = TRUE;
	++curwin->w_cursor.col;
	return OK;
}

	int
oneleft()
{
	if (curwin->w_cursor.col == 0)
		return FAIL;
	curwin->w_set_curswant = TRUE;
	--curwin->w_cursor.col;
	return OK;
}

	int
oneup(n)
	long n;
{
	if (n != 0 && curwin->w_cursor.lnum == 1)
		return FAIL;
	if (n >= curwin->w_cursor.lnum)
		curwin->w_cursor.lnum = 1;
	else
		curwin->w_cursor.lnum -= n;

	if (operator == NOP)
		cursupdate();				/* make sure curwin->w_topline is valid */

	/* try to advance to the column we want to be at */
	coladvance(curwin->w_curswant);
	return OK;
}

	int
onedown(n)
	long n;
{
	if (n != 0 && curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
		return FAIL;
	curwin->w_cursor.lnum += n;
	if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
		curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;

	if (operator == NOP)
		cursupdate();				/* make sure curwin->w_topline is valid */

	/* try to advance to the column we want to be at */
	coladvance(curwin->w_curswant);
	return OK;
}

/*
 * move screen 'count' pages up or down and update screen
 *
 * return FAIL for failure, OK otherwise
 */
	int
onepage(dir, count)
	int		dir;
	long	count;
{
	linenr_t		lp;
	long			n;

	if (curbuf->b_ml.ml_line_count == 1)	/* nothing to do */
		return FAIL;
	for ( ; count > 0; --count)
	{
		if (dir == FORWARD ? (curwin->w_topline >= curbuf->b_ml.ml_line_count - 1) : (curwin->w_topline == 1))
		{
			beep();
			return FAIL;
		}
		if (dir == FORWARD)
		{
			if (curwin->w_botline > curbuf->b_ml.ml_line_count)				/* at end of file */
				curwin->w_topline = curbuf->b_ml.ml_line_count;
			else if (plines(curwin->w_botline) >= curwin->w_height - 2 ||	/* next line is big */
					curwin->w_botline - curwin->w_topline <= 3)		/* just three lines on screen */
				curwin->w_topline = curwin->w_botline;
			else
				curwin->w_topline = curwin->w_botline - 2;
			curwin->w_cursor.lnum = curwin->w_topline;
			if (count != 1)
				comp_Botline(curwin);
		}
		else	/* dir == BACKWARDS */
		{
			lp = curwin->w_topline;
			/*
			 * If the first two lines on the screen are not too big, we keep
			 * them on the screen.
			 */
			if ((n = plines(lp)) > curwin->w_height / 2)
				--lp;
			else if (lp < curbuf->b_ml.ml_line_count && n + plines(lp + 1) < curwin->w_height / 2)
				++lp;
			curwin->w_cursor.lnum = lp;
			n = 0;
			while (n <= curwin->w_height && lp >= 1)
			{
				n += plines(lp);
				--lp;
			}
			if (n <= curwin->w_height)				/* at begin of file */
				curwin->w_topline = 1;
			else if (lp >= curwin->w_topline - 2)		/* happens with very long lines */
			{
				--curwin->w_topline;
				comp_Botline(curwin);
				curwin->w_cursor.lnum = curwin->w_botline - 1;
			}
			else
				curwin->w_topline = lp + 2;
		}
	}
	beginline(TRUE);
	updateScreen(VALID);
	return OK;
}

	void
stuff_inserted(c, count, no_esc)
	int		c;
	long	count;
	int		no_esc;
{
	char_u		*esc_ptr = NULL;
	char_u		*ptr;

	if (last_insert == NULL)
	{
		EMSG("No inserted text yet");
		return;
	}
	if (c)
		stuffcharReadbuff(c);
	if (no_esc && (esc_ptr = (char_u *)STRRCHR(last_insert, 27)) != NULL)
		*esc_ptr = NUL;		/* remove the ESC */

			/* skip the command */
	ptr = last_insert + last_insert_skip;

	do
		stuffReadbuff(ptr);
	while (--count > 0);

	if (no_esc && esc_ptr)
		*esc_ptr = 27;		/* put the ESC back */
}

	char_u *
get_last_insert()
{
	if (last_insert == NULL)
		return NULL;
	return last_insert + last_insert_skip;
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

	return check_abbr(c, ml_get(curwin->w_cursor.lnum), curwin->w_cursor.col,
				curwin->w_cursor.lnum == Insstart.lnum ? Insstart.col : 0);
}
