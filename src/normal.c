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
 * Contains the main routine for processing characters in command mode.
 * Communicates closely with the code in ops.c to handle the operators.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

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
#define CHECKCLEAROPQ	if (checkclearopq()) break;

/*
 * If a count is given before the operator, it is saved in opnum.
 */
static linenr_t	opnum = 0;
static linenr_t	Prenum; 		/* The (optional) number before a command. */
int				redo_Visual_busy = FALSE;	/* TRUE when redo-ing a visual */

static void		prep_redo __ARGS((long, int, int, int));
static int		checkclearop __ARGS((void));
static int		checkclearopq __ARGS((void));
static void		clearopbeep __ARGS((void));
static void		premsg __ARGS((int, int));

extern int		restart_edit;	/* this is in edit.c */

/*
 * normal
 *
 * Execute a command in normal mode.
 *
 * This is basically a big switch with the cases arranged in rough categories
 * in the following order:
 *
 *	  0. Macros (q, @)
 *	  1. Screen positioning commands (^U, ^D, ^F, ^B, ^E, ^Y, z)
 *	  2. Control commands (:, <help>, ^L, ^G, ^^, ZZ, *, ^], ^T)
 *	  3. Cursor motions (G, H, M, L, l, K_RARROW,  , h, K_LARROW, ^H, k, K_UARROW, ^P, +, CR, LF, j, K_DARROW, ^N, _, |, B, b, W, w, E, e, $, ^, 0)
 *	  4. Searches (?, /, n, N, T, t, F, f, ,, ;, ], [, %, (, ), {, })
 *	  5. Edits (., u, K_UNDO, ^R, U, r, J, p, P, ^A, ^S)
 *	  6. Inserts (A, a, I, i, o, O, R)
 *	  7. Operators (~, d, c, y, >, <, !, =, Q)
 *	  8. Abbreviations (x, X, D, C, s, S, Y, &)
 *	  9. Marks (m, ', `, ^O, ^I)
 *	 10. Buffer setting (")
 *	 11. Visual (v, V, ^V)
 *   12. Suspend (^Z)
 */

	void
normal()
{
	register u_char	c;
	long 			n;
	int				flag = FALSE;
	int 			type = 0;		/* used in some operations to modify type */
	int 			dir = FORWARD;	/* search direction */
	u_char			nchar = NUL;
	int				finish_op;
	linenr_t		Prenum1;
	char			searchbuff[CMDBUFFSIZE];		/* buffer for search string */
	FPOS			*pos;
	register char	*ptr;
	int				command_busy = FALSE;
	static int		didwarn = FALSE;		/* warned for broken inversion */

		/* the visual area is remembered for reselection */
	static linenr_t	resel_Visual_nlines;		/* number of lines */
	static int		resel_Visual_type = 0;	/* type 'v', 'V' or CTRL-V */
	static colnr_t	resel_Visual_col;		/* number of columns or end column */
		/* the visual area is remembered for redo */
	static linenr_t	redo_Visual_nlines;		/* number of lines */
	static int		redo_Visual_type = 0;	/* type 'v', 'V' or CTRL-V */
	static colnr_t	redo_Visual_col;			/* number of columns or end column */

	Prenum = 0;
	/*
	 * If there is an operator pending, then the command we take this time
	 * will terminate it. Finish_op tells us to finish the operation before
	 * returning this time (unless the operation was cancelled).
	 */
	finish_op = (operator != NOP);

	if (!finish_op && !yankbuffer)
		opnum = 0;

	if (p_sc && (vpeekc() == NUL || KeyTyped == TRUE))
		premsg(NUL, NUL);
	State = NORMAL_BUSY;
	c = vgetc();

	/* Pick up any leading digits and compute 'Prenum' */
	while ((c >= '1' && c <= '9') || (Prenum != 0 && (c == DEL || c == '0')))
	{
		if (c == DEL)
				Prenum /= 10;
		else
				Prenum = Prenum * 10 + (c - '0');
		if (Prenum < 0)			/* got too large! */
			Prenum = 999999999;
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
	if (strchr("@zZtTfF[]m'`\"", c) || (c == 'q' && !Recording && !Exec_reg) ||
										(c == 'r' && !Visual.lnum))
	{
		State = NOMAPPING;
		nchar = vgetc();		/* no macro mapping for this char */
		premsg(c, nchar);
	}
	if (p_sc)
		flushbuf();		/* flush the premsg() characters onto the screen so we can
							see them while the command is being executed */

	if (c != 'z')	/* the 'z' command gets another character */
	{
		State = NORMAL;
		script_winsize_pp();
	}
	if (nchar == ESC)
	{
		CLEAROP;
		goto normal_end;
	}
	switch (c)
	{

/*
 * 0: Macros
 */
	  case 'q': 		/* (stop) recording into a named register */
		CHECKCLEAROP;
						/* command is ignored while executing a register */
		if (!Exec_reg && !dorecord(nchar))
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
	  case Ctrl('D'):
		flag = TRUE;

	  case Ctrl('U'):
		CHECKCLEAROP;
		if (Prenum)
			p_scroll = (Prenum > Rows - 1) ? Rows - 1 : Prenum;
		n = (p_scroll < Rows) ? p_scroll : Rows - 1;
		if (flag)
		{
				Topline += n;
				if (Topline > line_count)
					Topline = line_count;
				comp_Botline();		/* compute Botline */
				onedown(n);
		}
		else
		{
				if (n >= Curpos.lnum)
					n = Curpos.lnum - 1;
				Prenum1 = Curpos.lnum - n;
				scrolldown(n);
				if (Prenum1 < Curpos.lnum)
					Curpos.lnum = Prenum1;
		}
		beginline(TRUE);
		updateScreen(VALID);
		break;

	  case Ctrl('B'):
	  case K_SUARROW:
		dir = BACKWARD;

	  case Ctrl('F'):
	  case K_SDARROW:
		CHECKCLEAROP;
		onepage(dir, Prenum1);
		break;

	  case Ctrl('E'):
		CHECKCLEAROP;
		scrollup(Prenum1);
		updateScreen(VALID);
		break;

	  case Ctrl('Y'):
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
					set_winheight((int)Prenum);
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

		if (Prenum)		/* line number given */
		{
			setpcmark();
			if (Prenum > line_count)
				Curpos.lnum = line_count;
			else
				Curpos.lnum = Prenum;
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

		  case '.': 			/* put Curpos in middle of screen */
			n = (Rows + plines(Curpos.lnum)) / 2;
			goto dozcmd;

		  case '-': 			/* put Curpos at bottom of screen */
			n = Rows - 1;
			/* FALLTHROUGH */

	dozcmd:
			{
				register linenr_t	lp = Curpos.lnum;
				register long		l = plines(lp);

				do
				{
					Topline = lp;
					if (--lp == 0)
						break;
					l += plines(lp);
				} while (l <= n);
			}
			updateScreen(VALID);
			beginline(TRUE);
			break;

		  default:
			CLEAROPBEEP;
		}
		break;

/*
 *	  2: Control commands
 */
	  case ':':
	    if (Visual.lnum)
			goto dooperator;
		CHECKCLEAROP;
		docmdline(NULL);
		break;

	  case K_HELP:
		CHECKCLEAROP;
		help();
		break;

	  case Ctrl('L'):
		CHECKCLEAROP;
		updateScreen(CLEAR);
		break;

	  case Ctrl('G'):
		CHECKCLEAROP;
		fileinfo(did_cd || Prenum);	/* print full name if count given or :cd used */
		break;

	  case K_CCIRCM:			/* shorthand command */
		CHECKCLEAROPQ;
		getaltfile((int)Prenum, (linenr_t)0, TRUE);
		break;

	  case 'Z': 		/* write, if changed, and exit */
		CHECKCLEAROPQ;
		if (nchar != 'Z')
		{
			CLEAROPBEEP;
			break;
		}
		stuffReadbuff(":x\n");
		break;

	  case Ctrl(']'):			/* :ta to current identifier */
		CHECKCLEAROPQ;
	  case '*': 				/* / to current identifier */
	  case '#': 				/* ? to current identifier */
	  case 'K':					/* run program for current identifier */
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
			switch (c)
			{
				case '*':
					stuffReadbuff("/");
					break;
				case '#':
					stuffReadbuff("?");
					break;
				case 'K':
					stuffReadbuff(":! ");
					stuffReadbuff(p_kp);
					stuffReadbuff(" ");
					break;
				default:
					stuffReadbuff(":ta ");
			}

			/*
			 * Now grab the chars in the identifier
			 */
			while (isidchar(ptr[col]))
			{
				stuffcharReadbuff(ptr[col]);
				++col;
			}
			stuffReadbuff("\n");
		}
		break;

	  case Ctrl('T'):		/* backwards in tag stack */
			CHECKCLEAROPQ;
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
		beginline(TRUE);
		break;

	  case 'H':
	  case 'M':
		if (c == 'M')
				n = (Rows - emptyrows - 1) / 2;
		else
				n = Prenum;
		mtype = MLINE;
		setpcmark();
		Curpos.lnum = Topline;
		while (n && onedown((long)1))
				--n;
		beginline(TRUE);
		break;

	  case 'L':
		mtype = MLINE;
		setpcmark();
		Curpos.lnum = Botline - 1;
		for (n = Prenum; n && oneup((long)1); n--)
				;
		beginline(TRUE);
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
	  case Ctrl('H'):
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
	  case Ctrl('P'):
		mtype = MLINE;
		if (!oneup(Prenum1))
			CLEAROPBEEP;
		else if (flag)
			beginline(TRUE);
		break;

	  case '+':
	  case CR:
		flag = TRUE;
		/* FALLTHROUGH */

	  case 'j':
	  case K_DARROW:
	  case Ctrl('N'):
	  case NL:
		mtype = MLINE;
		if (!onedown(Prenum1))
			CLEAROPBEEP;
		else if (flag)
			beginline(TRUE);
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
		if (operator == CHANGE && p_ai)		/* do not delete the indent */
		{
			beginline(TRUE);
			startop = Curpos;
			mtype = MCHAR;
			mincl = TRUE;
		}
		else
			mtype = MLINE;
		if (!onedown((long)(Prenum1 - 1)))
			CLEAROPBEEP;
		else if (mtype == MCHAR)	/* 'c' with autoindent */
		{
			Curpos.col = MAXCOL;	/* put cursor on last char in line */
			adjustCurpos();
		}
		else if (operator != YANK)	/* 'Y' does not move cursor */
			beginline(TRUE);
		break;

	  case '|':
		mtype = MCHAR;
		mincl = TRUE;
		beginline(FALSE);
		if (Prenum > 0)
			coladvance((colnr_t)(Prenum - 1));
		Curswant = (colnr_t)(Prenum - 1);
		break;

		/*
		 * Word Motions
		 */

	  case 'B':
		type = 1;
		/* FALLTHROUGH */

	  case 'b':
	  case K_SLARROW:
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
	  case K_SRARROW:
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
			n = fwd_word(Prenum1, type, operator != NOP);
		else
			n = end_word(Prenum1, type, operator == CHANGE);
		if (n)
		{
			CLEAROPBEEP;
			break;
		}
#if 0
		/*
		 * If we do a 'dw' for the last word in a line, we only delete the rest
		 * of the line, not joining the two lines, unless the current line is empty.
		 */
		if (operator == DELETE && Prenum1 == 1 && startop.lnum != Curpos.lnum &&
			!lineempty(startop.lnum))
		{
				Curpos = startop;
				while (oneright())
					;
				mincl = TRUE;
		}
#endif
		break;

	  case '$':
		mtype = MCHAR;
		mincl = TRUE;
		Curswant = MAXCOL;				/* so we stay at the end */
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
		mincl = FALSE;
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

		n = dosearch(c, searchbuff, FALSE, Prenum1, TRUE);
		if (n == 0)
			CLEAROP;
		else if (n == 2)
			mtype = MLINE;
		break;

	  case 'N':
		flag = 1;

	  case 'n':
		mtype = MCHAR;
		mincl = FALSE;
		set_want_col = TRUE;
		if (!dosearch(0, NULL, flag, Prenum1, TRUE))
			CLEAROP;
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
			CLEAROPBEEP;
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
		mincl = TRUE;
	    if (Prenum)		/* {cnt}% : goto {cnt} percentage in file */
		{
			if (Prenum > 100)
				CLEAROPBEEP;
			else
			{
				mtype = MLINE;
				setpcmark();
						/* round up, so CTRL-G will give same value */
				Curpos.lnum = (line_count * Prenum + 99) / 100;
				Curpos.col = 0;
			}
		}
		else			/* % : go to matching paren */
		{
			mtype = MCHAR;
			if ((pos = showmatch()) == NULL)
				CLEAROPBEEP;
			else
			{
				setpcmark();
				Curpos = *pos;
				set_want_col = TRUE;
			}
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
		CHECKCLEAROPQ;
		if (!start_redo(Prenum))
			CLEAROPBEEP;
		break;

	  case 'u':
	    if (Visual.lnum)
			goto dooperator;
	  case K_UNDO:
		CHECKCLEAROPQ;
		u_undo((int)Prenum1);
		set_want_col = TRUE;
		break;

	  case Ctrl('R'):
		CHECKCLEAROPQ;
	  	u_redo((int)Prenum1);
		set_want_col = TRUE;
		break;

	  case 'U':
	    if (Visual.lnum)
			goto dooperator;
		CHECKCLEAROPQ;
		u_undoline();
		set_want_col = TRUE;
		break;

	  case 'r':
	    if (Visual.lnum)
		{
			c = 'c';
			goto dooperator;
		}
		CHECKCLEAROPQ;
		ptr = nr2ptr(Curpos.lnum) + Curpos.col;
		if (strlen(ptr) < (unsigned)Prenum1)	/* not enough characters to replace */
		{
			CLEAROPBEEP;
			break;
		}
		/*
		 * Replacing with a line break or tab is done by edit(), because it
         * is complicated.
		 * Other characters are done below to avoid problems with things like
		 * CTRL-V 048 (for edit() this would be R CTRL-V 0 ESC).
		 */
		if (nchar == '\r' || nchar == '\n' || nchar == '\t')
		{
			prep_redo(Prenum1, 'r', nchar, NUL);
			stuffnumReadbuff(Prenum1);
			stuffcharReadbuff('R');
			stuffcharReadbuff(nchar);
			stuffcharReadbuff(ESC);
			break;
		}

		if (nchar == Ctrl('V'))				/* get another character */
		{
			c = Ctrl('V');
			nchar = get_literal(&type);
			if (type)						/* typeahead */
				stuffcharReadbuff(type);
		}
		else
			c = NUL;
		prep_redo(Prenum1, 'r', c, nchar);
		if (!u_saveCurpos())				/* save line for undo */
			break;
		Curpos.col += Prenum1 - 1;
		while (Prenum1--)					/* replace the characters */
			*ptr++ = nchar;
		set_want_col = TRUE;
		CHANGED;
		updateline();
		break;

	  case 'J':
	    if (Visual.lnum)		/* join the visual lines */
		{
			if (Curpos.lnum > Visual.lnum)
			{
				Prenum = Curpos.lnum - Visual.lnum + 1;
				Curpos.lnum = Visual.lnum;
			}
			else
				Prenum = Visual.lnum - Curpos.lnum + 1;
			Visual.lnum = 0;
		}
		CHECKCLEAROP;
		if (Prenum <= 1)
			Prenum = 2; 			/* default for join is two lines! */
		if (Curpos.lnum + Prenum - 1 > line_count)	/* beyond last line */
		{
			CLEAROPBEEP;
			break;
		}

		prep_redo(Prenum, 'J', NUL, NUL);
		dodojoin(Prenum, TRUE, TRUE);
		break;

	  case 'P':
		dir = BACKWARD;
		/* FALLTHROUGH */

	  case 'p':
		CHECKCLEAROPQ;
		prep_redo(Prenum, c, NUL, NUL);
		doput(dir, Prenum1);
		break;

	  case Ctrl('A'):			/* add to number */
	  case Ctrl('S'):			/* subtract from number */
		CHECKCLEAROPQ;
		if (doaddsub((int)c, Prenum1))
			prep_redo(Prenum1, c, NUL, NUL);
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
		CHECKCLEAROPQ;
		/* Works just like an 'i'nsert on the next character. */
		if (u_saveCurpos())
		{
			if (!lineempty(Curpos.lnum))
				incCurpos();
			startinsert(c, FALSE, Prenum1);
			command_busy = TRUE;
		}
		break;

	  case 'I':
		beginline(TRUE);
		/* FALLTHROUGH */

	  case 'i':
		CHECKCLEAROPQ;
		if (u_saveCurpos())
		{
			startinsert(c, FALSE, Prenum1);
			command_busy = TRUE;
		}
		break;

	  case 'o':
	  	if (Visual.lnum)	/* switch start and end of visual */
		{
			Prenum = Visual.lnum;
			Visual.lnum = Curpos.lnum;
			Curpos.lnum = Prenum;
			if (Visual.col != VISUALLINE)
			{
				n = Visual.col;
				Visual.col = Curpos.col;
				Curpos.col = (int)n;
				set_want_col = TRUE;
			}
			break;
		}
		CHECKCLEAROP;
		if (u_save(Curpos.lnum, (linenr_t)(Curpos.lnum + 1)) && Opencmd(FORWARD, TRUE, TRUE))
		{
			startinsert('o', TRUE, Prenum1);
			command_busy = TRUE;
		}
		break;

	  case 'O':
		CHECKCLEAROPQ;
		if (u_save((linenr_t)(Curpos.lnum - 1), Curpos.lnum) && Opencmd(BACKWARD, TRUE, TRUE))
		{
			startinsert('O', TRUE, Prenum1);
			command_busy = TRUE;
		}
		break;

	  case 'R':
	    if (Visual.lnum)
		{
			c = 'c';
			Visual.col = VISUALLINE;
			goto dooperator;
		}
		CHECKCLEAROPQ;
		if (u_saveCurpos())
		{
			startinsert('R', FALSE, Prenum1);
			command_busy = TRUE;
		}
		break;

/*
 * 7: Operators
 */
	  case '~': 		/* swap case */
	  /*
	   * if tilde is not an operator and Visual is off: swap case
	   * of a single character
	   */
		if (!p_to && !Visual.lnum)
		{
			CHECKCLEAROPQ;
			if (lineempty(Curpos.lnum))
			{
				CLEAROPBEEP;
				break;
			}
			prep_redo(Prenum, '~', NUL, NUL);

			if (!u_saveCurpos())
				break;

			for (; Prenum1 > 0; --Prenum1)
			{
				if (gcharCurpos() == NUL)
					break;
				swapchar(&Curpos);
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
	  case 'Q':
dooperator:
		n = strchr(opchars, c) - opchars + 1;
		if (n == operator)		/* double operator works on lines */
			goto lineop;
		CHECKCLEAROP;
		if (Prenum != 0)
			opnum = Prenum;
		startop = Curpos;
		operator = (int)n;
		break;

/*
 * 8: Abbreviations
 */

	 /* when Visual the next commands are operators */
	  case 'S':
	  case 'Y':
	  case 'D':
	  case 'C':
	  case 'x':
	  case 'X':
	  case 's':
	  	if (Visual.lnum)
		{
			static char trans[] = "ScYyDdCcxdXdsc";

			if (isupper(c) && !Visual_block)		/* uppercase means linewise */
				Visual.col = VISUALLINE;
			c = *(strchr(trans, c) + 1);
			goto dooperator;
		}

	  case '&':
		CHECKCLEAROPQ;
		if (Prenum)
			stuffnumReadbuff(Prenum);

		if (c == 'Y' && p_ye)
			c = 'Z';
		{
				static char *(ar[9]) = {"dl", "dh", "d$", "c$", "cl", "cc", "yy", "y$", ":s\r"};
				static char *str = "xXDCsSYZ&";

				stuffReadbuff(ar[(int)(strchr(str, c) - str)]);
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
		pos = getmark(nchar, (operator == NOP));
		if (pos == (FPOS *)-1)	/* jumped to other file */
		{
			if (flag)
				beginline(TRUE);
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
				beginline(TRUE);
		}
		mtype = flag ? MLINE : MCHAR;
		mincl = FALSE;		/* ignored if not MCHAR */
		set_want_col = TRUE;
		break;

	case Ctrl('O'):			/* goto older pcmark */
		Prenum1 = -Prenum1;
		/* FALLTHROUGH */

	case Ctrl('I'):			/* goto newer pcmark */
		CHECKCLEAROPQ;
		pos = movemark((int)Prenum1);
		if (pos == (FPOS *)-1)	/* jump to other file */
		{
			set_want_col = TRUE;
			break;
		}
		goto cursormark;

/*
 * 10. Buffer setting
 */
	  case '"':
		CHECKCLEAROP;
		if (isalnum(nchar) || nchar == '.' || nchar == '%' || nchar == '"')
		{
			yankbuffer = nchar;
			opnum = Prenum;		/* remember count before '"' */
		}
		else
			CLEAROPBEEP;
		break;

/*
 * 11. Visual
 */
 	  case 'v':
	  case 'V':
	  case Ctrl('V'):
		CHECKCLEAROP;
		Visual_block = FALSE;

			/* stop Visual */
		if (Visual.lnum)
		{
			Visual.lnum = 0;
			updateScreen(NOT_VALID);		/* delete the inversion */
		}
			/* start Visual */
		else
		{
			if (!didwarn && (T_TI == NULL || *T_TI == NUL))	/* cannot invert */
			{
				emsg("Warning: terminal cannot invert");
				didwarn = TRUE;
			}
			if (Prenum)						/* use previously selected part */
			{
				if (!resel_Visual_type)		/* there is none */
				{
					beep();
					break;
				}
				Visual = Curpos;
				if (resel_Visual_nlines > 1)
					Curpos.lnum += resel_Visual_nlines * Prenum - 1;
				switch (resel_Visual_type)
				{
				case 'V':	Visual.col = VISUALLINE;
							break;

				case Ctrl('V'):
							Visual_block = TRUE;
							break;

				case 'v':		
							if (resel_Visual_nlines <= 1)
								Curpos.col += resel_Visual_col * Prenum - 1;
							else
								Curpos.col = resel_Visual_col;
							break;
				}
				if (resel_Visual_col == MAXCOL)
				{
					Curswant = MAXCOL;
					coladvance(MAXCOL);
				}
				else if (Visual_block)
					coladvance((colnr_t)(Cursvcol + resel_Visual_col * Prenum - 1));
				updateScreen(NOT_VALID);	/* show the inversion */
			}
			else
			{
				Visual = Curpos;
				if (c == 'V')				/* linewise */
					Visual.col = VISUALLINE;
				else if (c == Ctrl('V'))	/* blockwise */
					Visual_block = TRUE;
				updateline();				/* start the inversion */
			}
		}
		break;

/*
 * 12. Suspend
 */

 	case Ctrl('Z'):
		CLEAROP;
		Visual.lnum = 0;					/* stop Visual */
		stuffReadbuff(":st\r");				/* with autowrite */
		break;

/*
 * The end
 */
	  case ESC:
	    if (Visual.lnum)
		{
			Visual.lnum = 0;			/* stop Visual */
			updateScreen(NOT_VALID);
		}

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
	if ((Visual.lnum || finish_op) && operator != NOP)
	{
		if (operator != YANK && !Visual.lnum)		/* can't redo yank */
		{
				prep_redo(Prenum, opchars[operator - 1], c, nchar);
				if (c == '/' || c == '?')		/* was a search */
				{
						AppendToRedobuff(searchbuff);
						AppendToRedobuff(NL_STR);
				}
		}

		if (redo_Visual_busy)
		{
			startop = Curpos;
			Curpos.lnum += redo_Visual_nlines - 1;
			switch (redo_Visual_type)
			{
			case 'V':	Visual.col = VISUALLINE;
						break;

			case Ctrl('V'):
						Visual_block = TRUE;
						break;

			case 'v':		
						if (redo_Visual_nlines <= 1)
							Curpos.col += redo_Visual_col - 1;
						else
							Curpos.col = redo_Visual_col;
						break;
			}
			if (redo_Visual_col == MAXCOL)
			{
				Curswant = MAXCOL;
				coladvance(MAXCOL);
			}
		}
		else if (Visual.lnum)
			startop = Visual;

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
		nlines = endop.lnum - startop.lnum + 1;

		if (Visual.lnum || redo_Visual_busy)
		{
			if (Visual_block)				/* block mode */
			{
				startvcol = getvcol(&startop, 2);
				n = getvcol(&endop, 2);
				if (n < startvcol)
					startvcol = (colnr_t)n;

			/* if '$' was used, get endvcol from longest line */
				if (Curswant == MAXCOL)
				{
					Curpos.col = MAXCOL;
					endvcol = 0;
					for (Curpos.lnum = startop.lnum; Curpos.lnum <= endop.lnum; ++Curpos.lnum)
						if ((n = getvcol(&Curpos, 3)) > endvcol)
							endvcol = (colnr_t)n;
					Curpos = startop;
				}
				else if (redo_Visual_busy)
					endvcol = startvcol + redo_Visual_col - 1;
				else
				{
					endvcol = getvcol(&startop, 3);
					n = getvcol(&endop, 3);
					if (n > endvcol)
						endvcol = (colnr_t)n;
				}
				coladvance(startvcol);
			}

	/*
	 * prepare to reselect and redo Visual: this is based on the size
	 * of the Visual text
	 */
			if (Visual_block)
				resel_Visual_type = Ctrl('V');
			else if (Visual.col == VISUALLINE)
				resel_Visual_type = 'V';
			else
				resel_Visual_type = 'v';
			if (Curswant == MAXCOL)
				resel_Visual_col = MAXCOL;
			else if (Visual_block)
				resel_Visual_col = endvcol - startvcol + 1;
			else if (nlines > 1)
				resel_Visual_col = endop.col;
			else
				resel_Visual_col = endop.col - startop.col + 1;
			resel_Visual_nlines = nlines;
			if (operator != YANK && operator != COLON)	/* can't redo yank and : */
			{
				prep_redo(0L, 'v', opchars[operator - 1], NUL);
				redo_Visual_type = resel_Visual_type;
				redo_Visual_col = resel_Visual_col;
				redo_Visual_nlines = resel_Visual_nlines;
			}

			mincl = TRUE;
			if (Visual.col == VISUALLINE)
				mtype = MLINE;
			else
				mtype = MCHAR;

			redo_Visual_busy = FALSE;
			/*
			 * Switch Visual off now, so screen updating does
			 * not show inverted text when the screen is redrawn.
			 * With YANK and sometimes with COLON and FILTER there is no screen
			 * redraw, so it is done here to remove the inverted part.
			 */
			Visual.lnum = 0;
			if (operator == YANK || operator == COLON || operator == FILTER)
				updateScreen(NOT_VALID);
		}

		set_want_col = 1;

			/* no_op is set when start and end are the same */
		no_op = (mtype == MCHAR && !mincl && equal(startop, endop));

	/*
	 * If the end of an operator is in column one while mtype is MCHAR and mincl
	 * is FALSE, we put endop after the last character in the previous line.
	 * If startop is on or before the first non-blank in the line, the operator
	 * becomes linewise (strange, but that's the way vi does it).
	 */
		if (mtype == MCHAR && mincl == FALSE && endop.col == 0 && nlines > 1)
		{
			--nlines;
			--endop.lnum;
			if (startinmargin())
				mtype = MLINE;
			else
			{
				endop.col = strlen(nr2ptr(endop.lnum));
				if (endop.col)
				{
					--endop.col;
					mincl = TRUE;
				}
			}
		}
		switch (operator)
		{
		  case LSHIFT:
		  case RSHIFT:
			doshift(operator);
			break;

		  case DELETE:
			if (!no_op)
				dodelete();
			break;

		  case YANK:
			if (!no_op)
				doyank(FALSE);
			break;

		  case CHANGE:
			dochange();
			command_busy = TRUE;
			break;

		  case FILTER:
			bangredo = TRUE;			/* dobang() will put cmd in redo buffer */

		  case INDENT:
		  case COLON:
dofilter:
			sprintf(IObuff, ":%ld,%ld", (long)startop.lnum, (long)endop.lnum);
			stuffReadbuff(IObuff);
			if (operator != COLON)
				stuffReadbuff("!");
			if (operator == INDENT)
			{
				stuffReadbuff(p_ep);
				stuffReadbuff("\n");
			}
			else if (operator == FORMAT)
			{
				stuffReadbuff(p_fp);
				stuffReadbuff("\n");
			}
				/*	docmdline() does the rest */
			break;

		  case TILDE:
		  case UPPER:
		  case LOWER:
			if (!no_op)
				dotilde();
			break;

		  case FORMAT:
			if (*p_fp != NUL)
				goto dofilter;		/* use external command */
			doformat();				/* use internal function */
			break;

		  default:
			CLEAROPBEEP;
		}
		operator = NOP;
		Visual_block = FALSE;
		yankbuffer = 0;
	}

normal_end:
	premsg(-1, NUL);
	if (restart_edit && operator == NOP && Visual.lnum == 0 && !command_busy && stuff_empty() && yankbuffer == 0)
		startinsert(restart_edit, FALSE, 1L);
}

	static void
prep_redo(num, cmd, c, nchar)
	long 	num;
	int		cmd;
	int		c;
	int		nchar;
{
	ResetRedobuff();
	if (yankbuffer != 0)	/* yank from specified buffer */
	{
		AppendCharToRedobuff('\"');
		AppendCharToRedobuff(yankbuffer);
	}
	if (num)
		AppendNumberToRedobuff(num);
	AppendCharToRedobuff(cmd);
	if (c != NUL)
		AppendCharToRedobuff(c);
	if (nchar != NUL)
		AppendCharToRedobuff(nchar);
}

/*
 * check for operator active
 */
	static int
checkclearop()
{
	if (operator == NOP)
		return (FALSE);
	clearopbeep();
	return (TRUE);
}

/*
 * check for operator or Visual active
 */
	static int
checkclearopq()
{
	if (operator == NOP && Visual.lnum == 0)
		return (FALSE);
	clearopbeep();
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
 * the last command character, plus 'c1' and 'c2'
 */
	static void
premsg(c1, c2)
	int c1, c2;
{
	char	buf[40];
	char	*p;

	if (!p_sc || !(KeyTyped || c1 == -1 || c1 == ' '))
		return;

	cursor_off();
	windgoto((int)Rows - 1, sc_col);
	if (c1 == -1)
		outstrn("          ");	/* look in comp_col() for the number of spaces */
	else
	{
		p = buf;
		if (opnum)
		{
			sprintf(p, "%ld", (long)opnum);
			p = p + strlen(buf);
		}
		if (yankbuffer)
		{
			*p++ = '"';
			*p++ = yankbuffer;
		}
		if (operator == 'z')
			*p++ = 'z';
		else if (operator)
			*p++ = opchars[operator - 1];
		if (Prenum)
		{
			sprintf(p, "%ld", (long)Prenum);
			p = p + strlen(p);
		}
		*p = NUL;
		if (c1)
			strcpy(p, transchar(c1));
		if (c2)
			strcat(p, transchar(c2));
		buf[10] = NUL;			/* truncate at maximal length */
		outstrn(buf);
	}
	setcursor();
	cursor_on();
}
