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
 * Contains the main routine for processing characters in command mode.
 * Communicates closely with the code in ops.c to handle the operators.
 */

#include "vim.h"

#undef EXTERN
#undef INIT
#define EXTERN
#define INIT(x) x
#include "ops.h"

/*
 * Generally speaking, every command in normal() should either clear any
 * pending operator (with CLEAROP), or set the motion type variable.
 */

#define CLEAROP (operator = NOP)		/* clear any pending operator */
#define CLEAROPBEEP 	clearopbeep()	/* CLEAROP plus a beep() */
#define CHECKCLEAROP	if (checkclearop()) break;

/*
 * Operators can have counts either before the operator, or between the
 * operator and the following cursor motion as in:
 *
 * d3w or 3dw
 *
 * If a count is given before the operator, it is saved in opnum. If normal() is
 * called with a pending operator, the count in opnum (if present) overrides
 * any count that came later.
 */
static linenr_t	opnum = 0;
static linenr_t	Prenum; 		/* The (optional) number before a command. */
static bool_t	substituting = FALSE;	/* TRUE when in 'S' command */

static void		prep_redo __ARGS((long, int, int, int));
static int		checkclearop();
static void		clearopbeep();
static void		premsg __ARGS((int, int));

/*
 * normal
 *
 * Execute a command in normal mode.
 *
 * This is basically a big switch with the cases arranged in rough categories
 * in the following order:
 *
 *	  0. Macros (v, @)
 *	  1. Screen positioning commands (^U, ^D, ^F, ^B, ^E, ^Y, z)
 *	  2. Control commands (:, <help>, ^L, ^G, ^^, ZZ, *, ^], ^T)
 *	  3. Cursor motions (G, H, M, L, l, K_RARROW,  , h, K_LARROW, ^H, k, K_UARROW, ^P, +, CR, LF, j, K_DARROW, ^N, _, |, B, b, W, w, E, e, $, ^, 0)
 *	  4. Searches (?, /, n, N, T, t, F, f, ,, ;, ], [, %, (, ), {, })
 *	  5. Edits (., u, K_UNDO, ^R, U, r, J, p, P)
 *	  6. Inserts (A, a, I, i, o, O, R)
 *	  7. Operators (~, d, c, y, >, <, !, =)
 *	  8. Abbreviations (x, X, D, C, s, S, Y, &)
 *	  9. Marks (m, ', `, ^O, ^I)
 *	 10. Buffer setting (")
 */

	void
normal()
{
	register u_char	c;
	long 			n;
	bool_t			flag = FALSE;
	int 			type = 0;		/* used in some operations to modify type */
	int 			dir = FORWARD;	/* search direction */
	u_char			nchar = NUL;
	bool_t			finish_op;
	linenr_t		Prenum1;
	char			searchbuff[CMDBUFFSIZE];		/* buffer for search string */
	FPOS			*pos;
	register char	*ptr;


	Prenum = 0;
	/*
	 * If there is an operator pending, then the command we take this time
	 * will terminate it. Finish_op tells us to finish the operation before
	 * returning this time (unless the operation was cancelled).
	 */
	finish_op = (operator != NOP);

	if (!finish_op && !yankbuffer)
		opnum = 0;

	if (vpeekc() == NUL || KeyTyped == TRUE)
		premsg(NUL, NUL);
	State = NORMAL_BUSY;
	c = vgetc();

	/* Pick up any leading digits and compute 'Prenum' */
	while (c >= '1' && c <= '9' || (Prenum > 0 && (c == DEL || c == '0')))
	{
		if (c == DEL)
				Prenum /= 10;
		else
				Prenum = Prenum * 10 + (c - '0');
		premsg(' ', NUL);
		c = vgetc();
	}

	/*
	 * If we're in the middle of an operator (including after entering a yank
	 * buffer with ") AND we had a count before the
	 * operator, then that count overrides the current value of Prenum. What
	 * this means effectively, is that commands like "3dw" get turned into
	 * "d3w" which makes things fall into place pretty neatly.
	 * If you give a count before AND after the operator, they are multiplied.
	 */
	if (opnum != 0)
	{
			if (Prenum)
				Prenum *= opnum;
			else
				Prenum = opnum;
			opnum = 0;
	}

	Prenum1 = (Prenum == 0 ? 1 : Prenum);		/* Prenum often defaults to 1 */
	premsg(c, NUL);

	/*
	 * get an additional character if we need one
	 */
	if (strchr("@zZtTfF[]rm'`\"", c) || c == 'v' && Recording == FALSE)
	{
		State = NOMAPPING;
		nchar = vgetc();		/* no macro mapping for this char */
		premsg(c, nchar);
	}
	flushbuf();			/* flush the premsg() characters onto the screen so we can
							see them while the command is being executed */

	if (c != 'z')	/* the 'z' command gets another character */
	{
		State = NORMAL;
		script_winsize_pp();
	}
	if (nchar == ESC)
	{
		CLEAROP;
		premsg(-1, NUL);
		return;
	}
	switch (c)
	{

/*
 * 0: Macros
 */
	  case 'v': 		/* (stop) recording into a named buffer */
		CHECKCLEAROP;
		if (!dorecord(nchar))
				CLEAROPBEEP;
		break;

	 case '@':			/* execute a named buffer */
		CHECKCLEAROP;
		while (Prenum1--)
			if (!doexecbuf(nchar))
			{
				CLEAROPBEEP;
				break;
			}
		break;

/*
 * 1: Screen positioning commands
 */
	  case CTRL('D'):
		flag = TRUE;

	  case CTRL('U'):
		CHECKCLEAROP;
		if (Prenum)
			P(P_SS) = (Prenum > Rows - 1) ? Rows - 1 : Prenum;
		n = (P(P_SS) < Rows) ? P(P_SS) : Rows - 1;
		if (flag)
		{
				scrollup(n);
				onedown(n);
		}
		else
		{
				scrolldown(n);
				oneup(n);
		}
		updateScreen(VALID);
		break;

	  case CTRL('B'):
		dir = BACKWARD;

	  case CTRL('F'):
		CHECKCLEAROP;
		if (line_count == 1)	/* nothing to do */
				break;
		for ( ; Prenum1 > 0; --Prenum1)
		{
			linenr_t		lp;

			if (dir == FORWARD ? (Topline >= line_count - 1) : (Topline == 1))
			{
				beep();
				break;
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
		beginline((bool_t)TRUE);
		updateScreen(VALID);
		break;

	  case CTRL('E'):
		CHECKCLEAROP;
		scrollup(Prenum1);
		updateScreen(VALID);
		break;

	  case CTRL('Y'):
		CHECKCLEAROP;
		scrolldown(Prenum1);
		updateScreen(VALID);
		break;

	  case 'z':
		CHECKCLEAROP;
		if (isdigit(nchar))
		{
			/*
			 * we misuse some variables to be able to call premsg()
			 */
			operator = c;
			opnum = Prenum;
			Prenum = nchar - '0';
			for (;;)
			{
				premsg(' ', NUL);
				nchar = vgetc();
				State = NORMAL;
				script_winsize_pp();
				if (nchar == DEL)
					Prenum /= 10;
				else if (isdigit(nchar))
					Prenum = Prenum * 10 + (nchar - '0');
				else if (nchar == CR)
				{
					set_winsize(Columns, (int)Prenum);
					break;
				}
				else
				{
					CLEAROPBEEP;
					break;
				}
			}
			operator = NOP;
			break;
		}
		State = NORMAL;
		script_winsize_pp();
		switch (nchar)
		{
		  case NL:				/* put Curpos at top of screen */
		  case CR:
			Topline = Curpos.lnum;
			updateScreen(VALID);
			break;

		  case '.': 			/* put Curspos in middle of screen */
			n = Rows / 2;
			goto dozcmd;

		  case '-': 			/* put Curpos at bottom of screen */
			n = Rows - 1;
			/* FALLTHROUGH */

	dozcmd:
			{
				register linenr_t	lp = Curpos.lnum;
				register long		l = 0;

				while ((l < n) && (lp != 0))
				{
					l += plines(lp);
					Topline = lp;
					--lp;
				}
			}
			updateScreen(VALID);
			break;

		  default:
			CLEAROPBEEP;
		}
		break;

/*
 *	  2: Control commands
 */
	  case ':':
		CHECKCLEAROP;
		docmdline(NULL);
		break;

	  case K_HELP:
		CHECKCLEAROP;
		help();
		break;

	  case CTRL('L'):
		CHECKCLEAROP;
		screenclear();
		updateScreen(NOT_VALID);
		break;

	  case CTRL('G'):
		CHECKCLEAROP;
		fileinfo();
		break;

	  case K_CCIRCM:			/* shorthand command */
		CHECKCLEAROP;
		getaltfile((int)Prenum, (linenr_t)0, (bool_t)TRUE);
		break;

	  case 'Z': 		/* write, if changed, and exit */
		CHECKCLEAROP;
		if (nchar != 'Z')
		{
			CLEAROPBEEP;
			break;
		}
		stuffReadbuff(":x\n");
		break;

	  case CTRL(']'):			/* :ta to current identifier */
		CHECKCLEAROP;
	  case '*': 				/* / to current identifier */
	  case '#': 				/* ? to current identifier */
		{
			register int 	col;

			ptr = nr2ptr(Curpos.lnum);
			col = Curpos.col;

			/*
			 * skip to start of identifier.
			 */
			while (ptr[col] != NUL && !isidchar(ptr[col]))
				++col;

			/*
			 * Back up to start of identifier. This doesn't match the
			 * real vi but I like it a little better and it shouldn't bother
			 * anyone.
			 */
			while (col > 0 && isidchar(ptr[col - 1]))
				--col;

			if (!isidchar(ptr[col]))
			{
				CLEAROPBEEP;
				break;
			}

			if (Prenum)
				stuffnumReadbuff(Prenum);
			if (c == '*')
				stuffReadbuff("/");
			else if (c == '#')
				stuffReadbuff("?");
			else
				stuffReadbuff(":ta ");

			/*
			 * Now grab the chars in the identifier
			 */
			while (isidchar(ptr[col]))
			{
				stuffReadbuff(mkstr(ptr[col]));
				++col;
			}
			stuffReadbuff("\n");
		}
		break;

	  case CTRL('T'):		/* backwards in tag stack */
			CHECKCLEAROP;
	  		dotag("", 2, (int)Prenum1);
			break;

/*
 * Cursor motions
 */
	  case 'G':
		mtype = MLINE;
		setpcmark();
		if (Prenum == 0 || Prenum > line_count)
				Curpos.lnum = line_count;
		else
				Curpos.lnum = Prenum;
		beginline((bool_t)TRUE);
		break;

	  case 'H':
	  case 'M':
		if (c == 'M')
				n = Rows / 2;
		else
				n = Prenum;
		mtype = MLINE;
		Curpos.lnum = Topline;
		while (n && onedown((long)1))
				--n;
		beginline((bool_t)TRUE);
		break;

	  case 'L':
		mtype = MLINE;
		Curpos.lnum = Botline - 1;
		for (n = Prenum; n && oneup((long)1); n--)
				;
		beginline((bool_t)TRUE);
		break;

	  case 'l':
	  case K_RARROW:
	  case ' ':
		mtype = MCHAR;
		mincl = FALSE;
		n = Prenum1;
		while (n--)
		{
			if (!oneright())
			{
				if (operator == NOP)
					beep();
				else
				{
					if (lineempty(Curpos.lnum))
						CLEAROPBEEP;
					else
					{
						mincl = TRUE;
						if (n)
							beep();
					}
				}
				break;
			}
		}
		set_want_col = TRUE;
		break;

	  case 'h':
	  case K_LARROW:
	  case CTRL('H'):
	  case DEL:
		mtype = MCHAR;
		mincl = FALSE;
		n = Prenum1;
		while (n--)
		{
			if (!oneleft())
			{
				if (operator != DELETE && operator != CHANGE)
					beep();
				else if (Prenum1 == 1)
					CLEAROPBEEP;
				break;
			}
		}
		set_want_col = TRUE;
		break;

	  case '-':
		flag = TRUE;
		/* FALLTHROUGH */

	  case 'k':
	  case K_UARROW:
	  case CTRL('P'):
		mtype = MLINE;
		if (!oneup(Prenum1))
			CLEAROPBEEP;
		else if (flag)
			beginline((bool_t)TRUE);
		break;

	  case '+':
	  case CR:
		flag = TRUE;
		/* FALLTHROUGH */

	  case 'j':
	  case K_DARROW:
	  case CTRL('N'):
	  case NL:
		mtype = MLINE;
		if (!onedown(Prenum1))
			CLEAROPBEEP;
		else if (flag)
			beginline((bool_t)TRUE);
		break;

		/*
		 * This is a strange motion command that helps make operators more
		 * logical. It is actually implemented, but not documented in the
		 * real 'vi'. This motion command actually refers to "the current
		 * line". Commands like "dd" and "yy" are really an alternate form of
		 * "d_" and "y_". It does accept a count, so "d3_" works to delete 3
		 * lines.
		 */
	  case '_':
lineop:
		mtype = MLINE;
		if (!onedown((long)(Prenum1 - 1)))
			CLEAROPBEEP;
		else
			beginline((bool_t)TRUE);
		break;

	  case '|':
		mtype = MCHAR;
		mincl = TRUE;
		beginline((bool_t)FALSE);
		if (Prenum > 0)
			coladvance((colnr_t)(Prenum - 1));
		Curswant = Prenum - 1;
		break;

		/*
		 * Word Motions
		 */

	  case 'B':
		type = 1;
		/* FALLTHROUGH */

	  case 'b':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		if (bck_word(Prenum1, type))
			CLEAROPBEEP;
		break;

	  case 'E':
		type = 1;
		/* FALLTHROUGH */

	  case 'e':
		mincl = TRUE;
		goto dowrdcmd;

	  case 'W':
		type = 1;
		/* FALLTHROUGH */

	  case 'w':
		mincl = FALSE;
		flag = TRUE;
		/*
		 * This is a little strange. To match what the real vi does, we
		 * effectively map 'cw' to 'ce', and 'cW' to 'cE', provided that we are
		 * not on a space or a TAB. This seems
		 * impolite at first, but it's really more what we mean when we say
		 * 'cw'.
		 */
		if (operator == CHANGE && (n = gcharCurpos()) != ' ' && n != TAB &&
																n != NUL)
		{
			mincl = TRUE;
			flag = FALSE;
		}

dowrdcmd:
		mtype = MCHAR;
		set_want_col = TRUE;
		if (flag)
			n = fwd_word(Prenum1, type);
		else
			n = end_word(Prenum1, type, operator == CHANGE);
		if (n)
		{
			CLEAROPBEEP;
			break;
		}
		/*
		 * if we do a 'dw' for the last word in a line, we only delete the rest
		 * of the line, not joining the two lines.
		 */
		if (operator == DELETE && Prenum1 == 1 && startop.lnum != Curpos.lnum)
		{
				Curpos = startop;
				while (oneright())
					;
				mincl = TRUE;
		}
		break;

	  case '$':
		mtype = MCHAR;
		mincl = TRUE;
		Curswant = 9999;				/* so we stay at the end */
		if (!onedown((long)(Prenum1 - 1)))
		{
				CLEAROPBEEP;
				break;
		}
		break;

	  case '^':
		flag = TRUE;
		/* FALLTHROUGH */

	  case '0':
		mtype = MCHAR;
		mincl = TRUE;
		beginline(flag);
		break;

/*
 * 4: Searches
 */
	  case '?':
	  case '/':
		if (!getcmdline(c, (u_char *)searchbuff))
		{
				CLEAROP;
				break;
		}
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;

		n = dosearch(c == '/' ? FORWARD : BACKWARD, searchbuff, (bool_t)FALSE, Prenum1);
		if (n == 0)
				CLEAROPBEEP;
		else if (n == 2)
				mtype = MLINE;
		break;

	  case 'N':
		flag = 1;

	  case 'n':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		if (!dosearch(0, NULL, flag, Prenum1))
			CLEAROPBEEP;
		break;

		/*
		 * Character searches
		 */
	  case 'T':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case 't':
		type = 1;
		goto docsearch;

	  case 'F':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case 'f':
docsearch:
		mtype = MCHAR;
		mincl = TRUE;
		set_want_col = TRUE;
		if (!searchc(nchar, dir, type, Prenum1))
		{
			CLEAROPBEEP;
		}
		break;

	  case ',':
		flag = 1;
		/* FALLTHROUGH */

	  case ';':
	    dir = flag;
	    goto docsearch;		/* nchar == NUL, thus repeat previous search */

		/*
		 * section or C function searches
		 */

	  case '[':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case ']':
		mtype = MLINE;
		set_want_col = TRUE;
		flag = '{';
		if (nchar != c)
		{
			if (nchar == '[' || nchar == ']')
				flag = '}';
			else
			{
				CLEAROPBEEP;
				break;
			}
		}
		if (dir == FORWARD && operator != NOP)	/* e.g. y]] searches for '}' */
			flag = '}';
		if (!findpar(dir, Prenum1, flag))
		{
			CLEAROPBEEP;
		}
		break;

	  case '%':
		mtype = MCHAR;
		mincl = TRUE;
		if ((pos = showmatch()) == NULL)
			CLEAROPBEEP;
		else
		{
			setpcmark();
			Curpos = *pos;
			set_want_col = TRUE;
		}
		break;

	  case '(':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case ')':
		mtype = MCHAR;
		if (c == ')')
			mincl = FALSE;
		else
			mincl = TRUE;
		set_want_col = TRUE;

		if (!findsent(dir, Prenum1))
			CLEAROPBEEP;
		break;

	  case '{':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case '}':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;

		if (!findpar(dir, Prenum1, NUL))
			CLEAROPBEEP;
		break;

/*
 * 5: Edits
 */
	  case '.':
		CHECKCLEAROP;
		if (!start_redo(Prenum))
			CLEAROPBEEP;
		break;

	  case 'u':
	  case K_UNDO:
		CHECKCLEAROP;
		u_undo((int)Prenum1);
		break;

	  case CTRL('R'):
		CHECKCLEAROP;
	  	u_redo((int)Prenum1);
		break;

	  case 'U':
		CHECKCLEAROP;
		u_undoline();
		break;

	  case 'r':
		CHECKCLEAROP;
		n = strlen(nr2ptr(Curpos.lnum)) - Curpos.col;
		if (n < Prenum1)			/* not enough characters to replace */
		{
			CLEAROPBEEP;
			break;
		}

		prep_redo(Prenum1, 'r', NUL, nchar);
		stuffnumReadbuff(Prenum1);
		stuffReadbuff("R");
		stuffReadbuff(mkstr(nchar));
		stuffReadbuff("\033");
		break;

	  case 'J':
		CHECKCLEAROP;
		if (Prenum <= 1)
				Prenum = 2; 	/* default for join is two lines! */
		if (Curpos.lnum + Prenum - 1 > line_count)	/* beyond last line */
		{
			CLEAROPBEEP;
			break;
		}

		prep_redo(Prenum, 'J', NUL, NUL);
		dodojoin(Prenum, (bool_t)TRUE);
		break;

	  case 'P':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case 'p':
		CHECKCLEAROP;
		prep_redo(Prenum, c, NUL, NUL);
		doput(dir, Prenum1);
		break;

/*
 * 6: Inserts
 */
	  case 'A':
		set_want_col = TRUE;
		while (oneright())
				;
		/* FALLTHROUGH */

	  case 'a':
		CHECKCLEAROP;
		/* Works just like an 'i'nsert on the next character. */
		if (u_saveCurpos())
		{
				if (!lineempty(Curpos.lnum))
					incCurpos();
				startinsert(c, (bool_t)FALSE, Prenum1);
		}
		break;

	  case 'I':
		beginline((bool_t)TRUE);
		/* FALLTHROUGH */

	  case 'i':
	  case K_INSERT:
		CHECKCLEAROP;
		if (u_saveCurpos())
				startinsert(c, (bool_t)FALSE, Prenum1);
		break;

	  case 'o':
		CHECKCLEAROP;
		if (u_save(Curpos.lnum, (linenr_t)(Curpos.lnum + 1)) && Opencmd(FORWARD))
				startinsert('o', (bool_t)TRUE, Prenum1);
		break;

	  case 'O':
		CHECKCLEAROP;
		if (u_save((linenr_t)(Curpos.lnum - 1), Curpos.lnum) && Opencmd(BACKWARD))
				startinsert('O', (bool_t)TRUE, Prenum1);
		break;

	  case 'R':
		CHECKCLEAROP;
		if (u_saveCurpos())
				startinsert('R', (bool_t)FALSE, Prenum1);
		break;

/*
 * 7: Operators
 */
	  case '~': 		/* swap case */
		if (!P(P_TO))	/* if tilde is not an operator */
		{
				CHECKCLEAROP;
				if (lineempty(Curpos.lnum)) {
					CLEAROPBEEP;
					break;
				}
				prep_redo(Prenum, '~', NUL, NUL);

				if (!u_saveCurpos())
						break;

				ptr = nr2ptr(Curpos.lnum) + Curpos.col;
				for ( ; (c = *ptr) != NUL && Prenum1 > 0; --Prenum1, ++ptr)
				{
						if (isalpha(c))
						{
								if (islower(c))
										*ptr = toupper(c);
								else
										*ptr = tolower(c);
						}
						incCurpos();
				}

				set_want_col = TRUE;
				CHANGED;
				updateline();
				break;
		}
		/*FALLTHROUGH*/

	  case 'd':
	  case 'c':
	  case 'y':
	  case '>':
	  case '<':
	  case '!':
	  case '=':
		n = strchr(opchars, c) - opchars + 1;
		if (n == operator)		/* double operator works on lines */
			goto lineop;
		CHECKCLEAROP;
		if (Prenum != 0)
			opnum = Prenum;
		startop = Curpos;
		operator = n;
		break;

/*
 * 8: Abbreviations
 */

	  case 'x':
	  case 'X':
	  case 'D':
	  case 'C':
	  case 's':
	  case 'S':
	  case 'Y':
	  case '&':
		CHECKCLEAROP;
		if (Prenum)
			stuffnumReadbuff(Prenum);

		if (c == 'S')
		{
			beginline((int)P(P_AI));
			substituting = TRUE;
		}
		else if (c == 'Y' && P(P_YE))
			c = 'Z';
		{
				static char *(ar[9]) = {"dl", "dh", "d$", "c$", "cl", "c$", "yy", "y$", ":s\r"};
				static char *str = "xXDCsSYZ&";

				stuffReadbuff(ar[strchr(str, c) - str]);
		}
		break;

/*
 * 9: Marks
 */

	  case 'm':
		CHECKCLEAROP;
		if (!setmark(nchar))
			CLEAROPBEEP;
		break;

	  case '\'':
		flag = TRUE;
		/* FALLTHROUGH */

	  case '`':
		pos = getmark(nchar, (bool_t)(operator == NOP));
		if (pos == (FPOS *)-1)	/* jumped to other file */
		{
			if (flag)
				beginline((bool_t)TRUE);
			break;
		}

		if (pos != NULL)
			setpcmark();

cursormark:
		if (pos == NULL)
			CLEAROPBEEP;
		else
		{
			Curpos = *pos;
			if (flag)
				beginline((bool_t)TRUE);
		}
		mtype = flag ? MLINE : MCHAR;
		mincl = TRUE;		/* ignored if not MCHAR */
		set_want_col = TRUE;
		break;

	case CTRL('O'):			/* goto older pcmark */
		Prenum1 = -Prenum1;
		/* FALLTHROUGH */

	case CTRL('I'):			/* goto newer pcmark */
		CHECKCLEAROP;
		pos = movemark((int)Prenum1);
		if (pos == (FPOS *)-1)	/* jump to other file */
			break;
		goto cursormark;

/*
 * 10. Buffer setting
 */
	  case '"':
		CHECKCLEAROP;
		if (isalnum(nchar))
		{
			yankbuffer = nchar;
			opnum = Prenum;		/* remember count before '"' */
		}
		else
			CLEAROPBEEP;
		break;

	  default:					/* not a known command */
		CLEAROPBEEP;
		break;

	}	/* end of switch on command character */

/*
 * if we didn't start or finish an operator, reset yankbuffer, unless we
 * need it later.
 */
	if (!finish_op && !operator && strchr("\"DCYSsXx", c) == NULL)
		yankbuffer = 0;

	/*
	 * If an operation is pending, handle it...
	 */
	if (finish_op && operator != NOP)	/* we just finished an operator */
	{
		if (operator != YANK)
		{
				prep_redo(Prenum, opchars[operator - 1], c, nchar);
				if (c == '/' || c == '?')		/* was a search */
				{
						AppendToRedobuff(searchbuff);
						AppendToRedobuff(NL_STR);
				}
		}

		if (lt(startop, Curpos))
		{
			endop = Curpos;
			Curpos = startop;
		}
		else
		{
			endop = startop;
			startop = Curpos;
		}
		set_want_col = 1;
		nlines = endop.lnum - startop.lnum + 1;
		if (!mincl && !equal(startop, endop))
			oneless = 1;
		else
			oneless = 0;

		switch (operator)
		{
		  case LSHIFT:
		  case RSHIFT:
			doshift(operator);
			break;

		  case DELETE:
			dodelete();
			break;

		  case YANK:
			doyank((bool_t)FALSE);
			break;

		  case CHANGE:
			dochange();
			break;

		  case FILTER:
			AppendToRedobuff("!\n");	/* strange but necessary */

		  case INDENT:
			{
				if (mincl == FALSE && endop.col == 0 && endop.lnum > startop.lnum)
					--endop.lnum;
				sprintf(IObuff, ":%ld,%ld!", (long)startop.lnum, (long)endop.lnum);
				stuffReadbuff(IObuff);
				if (operator == INDENT)
						stuffReadbuff("indent\n");
			}
				/*	docmdline() does the rest */
			break;

		  case TILDE:
			dotilde();
			break;

		  default:
			CLEAROPBEEP;
		}
		operator = NOP;
		yankbuffer = 0;
	}
	premsg(-1, NUL);
}

	static void
prep_redo(num, cmd, c, nchar)
	long 	num;
	int		cmd;
	int		c;
	int		nchar;
{
		if (substituting)	/* special case: 'S' command is done like 'c$' */
		{
			substituting = FALSE;
			cmd = 'S';
			c = NUL;
			nchar = NUL;
		}
		ResetBuffers();
		if (yankbuffer != 0)	/* yank from specified buffer */
		{
				AppendToRedobuff("\"");
				AppendToRedobuff(mkstr(yankbuffer));
		}
		if (num)
				AppendNumberToRedobuff(num);
		AppendToRedobuff(mkstr(cmd));
		if (c != NUL)
				AppendToRedobuff(mkstr(c));
		if (nchar != NUL)
				AppendToRedobuff(mkstr(nchar));
}

	static int
checkclearop()
{
		if (operator == NOP)
				return (FALSE);
		CLEAROP;
		beep();
		return (TRUE);
}

	static void
clearopbeep()
{
		CLEAROP;
		beep();
}

/*
 * display, on the last line of the window, the characters typed before
 * the last command character, plus 'c'
 */
	static void
premsg(c1, c2)
		int c1, c2;
{
		char c;

		if (!P(P_SC) || !KeyTyped)
				return;

#ifdef AUX
		if (!Aux_Device)
#endif
			outstr(T_CI);			/* disable cursor */
		windgoto(Rows - 1, Columns - 12);
		if (c1 == -1)
				outstr("           ");
		else
		{
			if (opnum)
				outnum(opnum);
			if (yankbuffer)
			{
				outchar('"');
				outchar(yankbuffer);
			}
			if (operator == 'z')
				outchar('z');
			else if (operator)
				outchar(opchars[operator - 1]);
			if (Prenum)
				outnum(Prenum);
			if (c1)
			{
				c = c1;
				outtrans(&c, 1);
			}
			if (c2)
			{
				c = c2;
				outtrans(&c, 1);
			}
		}
		windgoto(Cursrow, Curscol);
#ifdef AUX
		if (!Aux_Device)
#endif
			outstr(T_CV);			/* enable cursor */
}
