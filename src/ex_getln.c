/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * ex_getln.c: Functions for entering and editing an Ex command line.
 */

#include "vim.h"

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
struct cmdline_info
{
    char_u  *cmdbuff;	    /* pointer to command line buffer */
    int	     cmdbufflen;    /* length of cmdbuff */
    int	     cmdlen;	    /* number of chars on command line */
    int	     cmdpos;	    /* current cursor position */
    int	     cmdspos;	    /* cursor column on screen */
    int	     cmdfirstc;	    /* ':', '/', '?', '=' or NUL */
    int	     cmdindent;	    /* number of spaces before cmdline */
    int	     overstrike;    /* Typing mode on the command line.  Shared by
			       getcmdline() and put_on_cmdline(). */
};

static struct cmdline_info ccline;	/* current cmdline_info */

static int	cmd_numfiles = -1;	/* number of files found by
						    file name completion */
static char_u	**(history[HIST_COUNT]) = {NULL, NULL, NULL};
static int	hisidx[HIST_COUNT] = {-1, -1, -1};  /* last entered entry */
static int	hislen = 0;		/* actual length of history tables */

#ifdef RIGHTLEFT
static int	cmd_hkmap = 0;	    /* Hebrew mapping during command line */
#endif

#ifdef FKMAP
static int	    cmd_fkmap = 0;	/* Farsi mapping during command line */
#endif

static int	hist_char2type __ARGS((int c));
static void	init_history __ARGS((void));

static int	in_history __ARGS((int, char_u *, int));
static void	alloc_cmdbuff __ARGS((int len));
static int	realloc_cmdbuff __ARGS((int len));
static void	putcmdline __ARGS((int));
static void	redrawcmd __ARGS((void));
static void	cursorcmd __ARGS((void));
static int	ccheck_abbr __ARGS((int));
static int	nextwild __ARGS((int));
static int	showmatches __ARGS((void));
static void	set_expand_context __ARGS((void));
static int	ExpandFromContext __ARGS((char_u *, int *, char_u ***, int, int));

/*
 * getcmdline() - accept a command line starting with firstc.
 *
 * firstc == ':'	    get ":" command line.
 * firstc == '/' or '?'	    get search pattern
 * firstc == '='	    get expression
 * firstc == NUL	    get text for :insert command
 *
 * The line is collected in ccline.cmdbuff, which is reallocated to fit the
 * command line.
 *
 * Careful: getcmdline() can be called recursively!
 *
 * Return pointer to allocated string if there is a commandline, NULL
 * otherwise.
 */
    char_u *
getcmdline(firstc, count, indent)
    int		firstc;
    long	count;		/* only used for incremental search */
    int		indent;		/* indent for inside conditionals */
{
    int		c;
#ifdef DIGRAPHS
    int		cc;
#endif
    int		i;
    int		j;
    char_u	*p;
    int		hiscnt;			/* current history line in use */
    char_u	*lookfor = NULL;	/* string to match */
    int		gotesc = FALSE;		/* TRUE when <ESC> just typed */
    int		do_abbr;		/* when TRUE check for abbr. */
    int		histype;		/* history type to be used */
#ifdef EXTRA_SEARCH
    FPOS	old_cursor;
    colnr_t	old_curswant;
    colnr_t	old_leftcol;
    linenr_t	old_topline;
    linenr_t	old_botline;
    int		did_incsearch = FALSE;
    int		incsearch_postponed = FALSE;
#endif
    int		save_msg_scroll = msg_scroll;
    int		save_State = State;	/* remember State when called */
    int		some_key_typed = FALSE;	/* one of the keys was typed */
#ifdef USE_MOUSE
    /* mouse drag and release events are ignored, unless they are
     * preceded with a mouse down event */
    int		ignore_drag_release = TRUE;
#endif
#ifdef USE_SNIFF
    want_sniff_request = 0;
#endif

    ccline.overstrike = FALSE;		    /* always start in insert mode */
#ifdef EXTRA_SEARCH
    old_cursor = curwin->w_cursor;	    /* needs to be restored later */
    old_curswant = curwin->w_curswant;
    old_leftcol = curwin->w_leftcol;
    old_topline = curwin->w_topline;
    old_botline = curwin->w_botline;
#endif

    /*
     * set some variables for redrawcmd()
     */
    ccline.cmdfirstc = firstc;
    ccline.cmdindent = indent;
    alloc_cmdbuff(exmode_active ? 250 : 0); /* alloc initial ccline.cmdbuff */
    if (ccline.cmdbuff == NULL)
	return NULL;			    /* out of memory */
    ccline.cmdlen = ccline.cmdpos = 0;
    if (firstc)
	ccline.cmdspos = 1 + indent;
    else
	ccline.cmdspos = 0 + indent;

    redir_off = TRUE;		/* don't redirect the typed command */
    i = msg_scrolled;
    msg_scrolled = 0;		/* avoid wait_return message */
    gotocmdline(TRUE);
    msg_scrolled += i;
    if (firstc)
	msg_putchar(firstc);
    while (indent-- > 0)
	msg_putchar(' ');

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when doing
     * ":@0" when register 0 doesn't contain a CR.
     */
    msg_scroll = FALSE;

    State = CMDLINE;
#ifdef USE_MOUSE
    setmouse();
#endif

    init_history();
    hiscnt = hislen;		/* set hiscnt to impossible history value */
    histype = hist_char2type(firstc);

#ifdef DIGRAPHS
    do_digraph(-1);		/* init digraph typahead */
#endif

    /* collect the command string, handling editing keys */
    for (;;)
    {
#ifdef USE_GUI_WIN32
	dont_scroll = FALSE;	/* allow scrolling here */
#endif
	cursorcmd();		/* set the cursor on the right spot */
	c = vgetc();
	if (KeyTyped)
	{
	    some_key_typed = TRUE;
#ifdef RIGHTLEFT
	    if (cmd_hkmap)
		c = hkmap(c);
# ifdef FKMAP
	    if (cmd_fkmap)
		c = cmdl_fkmap(c);
# endif
#endif
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 */
	if ((c == Ctrl('C')
#ifdef UNIX
		|| c == intr_char
#endif
				) && !global_busy)
	    got_int = FALSE;

	/* free old command line when finished moving around in the history
	 * list */
	if (lookfor
		&& c != K_S_DOWN && c != K_S_UP && c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_LEFT && c != K_RIGHT
		&& (cmd_numfiles > 0 || (c != Ctrl('P') && c != Ctrl('N'))))
	{
	    vim_free(lookfor);
	    lookfor = NULL;
	}

	/*
	 * <S-Tab> works like CTRL-P (unless 'wc' is <S-Tab>).
	 */
	if (c != p_wc && c == K_S_TAB && cmd_numfiles != -1)
	    c = Ctrl('P');

	/* free expanded names when finished walking through matches */
	if (cmd_numfiles != -1 && !(c == p_wc && KeyTyped) && c != Ctrl('N') &&
			c != Ctrl('P') && c != Ctrl('A') && c != Ctrl('L'))
	    (void)ExpandOne(NULL, NULL, 0, WILD_FREE);

#ifdef DIGRAPHS
	c = do_digraph(c);
#endif

	if (c == '\n' || c == '\r' || (c == ESC && (!KeyTyped ||
					 vim_strchr(p_cpo, CPO_ESC) != NULL)))
	{
	    gotesc = FALSE;	/* Might have typed ESC previously, don't
				   truncate the cmdline now. */
	    if (ccheck_abbr(c + ABBR_OFF))
		goto cmdline_changed;
	    windgoto(msg_row, 0);
	    out_flush();
	    break;
	}

	/* hitting <ESC> twice means: abandon command line */
	/* wildcard expansion is only done when the key is really typed,
	 * not when it comes from a macro */
	if (c == p_wc && !gotesc && KeyTyped)
	{
	    if (cmd_numfiles > 0)   /* typed p_wc twice */
		i = nextwild(WILD_NEXT);
	    else		    /* typed p_wc first time */
		i = nextwild(WILD_EXPAND_KEEP);
	    if (c == ESC)
		gotesc = TRUE;
	    if (i == OK)
		goto cmdline_changed;
	}
	gotesc = FALSE;

	/* <S-Tab> goes to last match, in a clumsy way */
	if (c == K_S_TAB && KeyTyped)
	{
	    if (nextwild(WILD_EXPAND_KEEP) == OK
		    && nextwild(WILD_PREV) == OK
		    && nextwild(WILD_PREV) == OK)
		goto cmdline_changed;
	}

	if (c == NUL || c == K_ZERO)	    /* NUL is stored as NL */
	    c = NL;

	do_abbr = TRUE;		/* default: check for abbreviation */
	switch (c)
	{
	case K_BS:
	case Ctrl('H'):
	case K_DEL:
	case Ctrl('W'):
#ifdef FKMAP
		if (cmd_fkmap && c == K_BS)
		    c = K_DEL;
#endif
		/*
		 * delete current character is the same as backspace on next
		 * character, except at end of line
		 */
		if (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
		    ++ccline.cmdpos;
		if (ccline.cmdpos > 0)
		{
		    j = ccline.cmdpos;
		    if (c == Ctrl('W'))
		    {
			while (ccline.cmdpos &&
			       vim_isspace(ccline.cmdbuff[ccline.cmdpos - 1]))
			    --ccline.cmdpos;
			i = vim_iswordc(ccline.cmdbuff[ccline.cmdpos - 1]);
			while (ccline.cmdpos && !vim_isspace(
					 ccline.cmdbuff[ccline.cmdpos - 1]) &&
				vim_iswordc(
				      ccline.cmdbuff[ccline.cmdpos - 1]) == i)
			    --ccline.cmdpos;
		    }
		    else
			--ccline.cmdpos;
		    ccline.cmdlen -= j - ccline.cmdpos;
		    i = ccline.cmdpos;
		    while (i < ccline.cmdlen)
			ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		    redrawcmd();
		}
		else if (ccline.cmdlen == 0 && c != Ctrl('W'))
		{
		    vim_free(ccline.cmdbuff);	/* no commandline to return */
		    ccline.cmdbuff = NULL;
		    msg_col = 0;
		    msg_putchar(' ');		/* delete ':' */
		    redraw_cmdline = TRUE;
		    goto returncmd;		/* back to cmd mode */
		}
		goto cmdline_changed;

	case K_INS:
#ifdef FKMAP
		/* if Farsi mode set, we are in reverse insert mode -
		   Do not change the mode */
		if (cmd_fkmap)
		    beep_flush();
		else
#endif
		ccline.overstrike = !ccline.overstrike;
#ifdef USE_GUI
		if (gui.in_use)
		    gui_upd_cursor_shape();	/* change shape of cursor */
#endif
		goto cmdline_not_changed;

/*	case '@':   only in very old vi */
	case Ctrl('U'):
		ccline.cmdpos = 0;
		ccline.cmdlen = 0;
		if (firstc)
		    ccline.cmdspos = 1 + ccline.cmdindent;
		else
		    ccline.cmdspos = 0 + ccline.cmdindent;
		redrawcmd();
		goto cmdline_changed;

	case ESC:	/* get here if p_wc != ESC or when ESC typed twice */
	case Ctrl('C'):
		gotesc = TRUE;	    /* will free ccline.cmdbuff after putting
				       it in history */
		goto returncmd;	    /* back to cmd mode */

	case Ctrl('R'):		    /* insert register */
#ifdef USE_GUI_WIN32
		dont_scroll = TRUE; /* disallow scrolling here */
#endif
		putcmdline('"');
		++no_mapping;
		c = vgetc();
		--no_mapping;
#ifdef WANT_EVAL
		/*
		 * Insert the result of an expression.
		 * Need to save the current command line, to be able to enter
		 * a new one...
		 */
		if (c == '=')
		{
		    struct cmdline_info	    save_ccline;

		    if (firstc == '=')	/* can't do this recursively */
		    {
			beep_flush();
			c = ESC;
		    }
		    else
		    {
			save_ccline = ccline;
			c = get_expr_register();
			ccline = save_ccline;
		    }
		}
#endif
		if (c != ESC)	    /* use ESC to cancel inserting register */
		    cmdline_paste(c);
		redrawcmd();
		goto cmdline_changed;

	case Ctrl('D'):
	    {
		if (showmatches() == FAIL)
		    break;	/* Use ^D as normal char instead */

		redrawcmd();
		continue;	/* don't do incremental search now */
	    }

	case K_RIGHT:
	case K_S_RIGHT:
		do
		{
		    if (ccline.cmdpos >= ccline.cmdlen)
			break;
		    ccline.cmdspos += charsize(ccline.cmdbuff[ccline.cmdpos]);
		    ++ccline.cmdpos;
		}
		while ((c == K_S_RIGHT || (mod_mask & MOD_MASK_CTRL))
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
		goto cmdline_not_changed;

	case K_LEFT:
	case K_S_LEFT:
		do
		{
		    if (ccline.cmdpos <= 0)
			break;
		    --ccline.cmdpos;
		    ccline.cmdspos -= charsize(ccline.cmdbuff[ccline.cmdpos]);
		}
		while ((c == K_S_LEFT || (mod_mask & MOD_MASK_CTRL))
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
		goto cmdline_not_changed;

#if defined(USE_MOUSE) || defined(FKMAP)
	case K_IGNORE:
#endif
#ifdef USE_MOUSE
	case K_MIDDLEDRAG:
	case K_MIDDLERELEASE:
		goto cmdline_not_changed;   /* Ignore mouse */

	case K_MIDDLEMOUSE:
# ifdef USE_GUI
		/* When GUI is active, also paste when 'mouse' is empty */
		if (!gui.in_use)
# endif
		    if (!mouse_has(MOUSE_COMMAND))
			goto cmdline_not_changed;   /* Ignore mouse */
# ifdef USE_GUI
		if (gui.in_use)
		    cmdline_paste('*');
		else
# endif
		    cmdline_paste(0);
		redrawcmd();
		goto cmdline_changed;

	case K_LEFTDRAG:
	case K_LEFTRELEASE:
	case K_RIGHTDRAG:
	case K_RIGHTRELEASE:
		if (ignore_drag_release)
		    goto cmdline_not_changed;
		/* FALLTHROUGH */
	case K_LEFTMOUSE:
	case K_RIGHTMOUSE:
		if (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		else
		    ignore_drag_release = FALSE;
# ifdef USE_GUI
		/* When GUI is active, also move when 'mouse' is empty */
		if (!gui.in_use)
# endif
		    if (!mouse_has(MOUSE_COMMAND))
			goto cmdline_not_changed;   /* Ignore mouse */
		if (firstc)
		    ccline.cmdspos = 1 + ccline.cmdindent;
		else
		    ccline.cmdspos = 0 + ccline.cmdindent;
		for (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
							      ++ccline.cmdpos)
		{
		    i = charsize(ccline.cmdbuff[ccline.cmdpos]);
		    if (mouse_row <= cmdline_row + ccline.cmdspos / Columns &&
				     mouse_col < ccline.cmdspos % Columns + i)
			break;
		    ccline.cmdspos += i;
		}
		goto cmdline_not_changed;
#endif	/* USE_MOUSE */

#ifdef USE_GUI
	case K_SCROLLBAR:
		if (!msg_scrolled)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		goto cmdline_not_changed;

	case K_HORIZ_SCROLLBAR:
		if (!msg_scrolled)
		{
		    gui_do_horiz_scroll();
		    redrawcmd();
		}
		goto cmdline_not_changed;
#endif

	case Ctrl('B'):	    /* begin of command line */
	case K_HOME:
	case K_KHOME:
		ccline.cmdpos = 0;
		if (firstc)
		    ccline.cmdspos = 1 + ccline.cmdindent;
		else
		    ccline.cmdspos = 0 + ccline.cmdindent;
		goto cmdline_not_changed;

	case Ctrl('E'):	    /* end of command line */
	case K_END:
	case K_KEND:
		ccline.cmdpos = ccline.cmdlen;
		ccline.cmdbuff[ccline.cmdlen] = NUL;
		if (firstc)
		    ccline.cmdspos = 1 + ccline.cmdindent;
		else
		    ccline.cmdspos = 0 + ccline.cmdindent;
		ccline.cmdspos += vim_strsize(ccline.cmdbuff);
		goto cmdline_not_changed;

	case Ctrl('A'):	    /* all matches */
		if (nextwild(WILD_ALL) == FAIL)
		    break;
		goto cmdline_changed;

	case Ctrl('L'):	    /* longest common part */
		if (nextwild(WILD_LONGEST) == FAIL)
		    break;
		goto cmdline_changed;

	case Ctrl('N'):	    /* next match */
	case Ctrl('P'):	    /* previous match */
		if (cmd_numfiles > 0)
		{
		    if (nextwild((c == Ctrl('P')) ? WILD_PREV : WILD_NEXT)
								      == FAIL)
			break;
		    goto cmdline_changed;
		}

	case K_UP:
	case K_DOWN:
	case K_S_UP:
	case K_S_DOWN:
	case K_PAGEUP:
	case K_KPAGEUP:
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
		if (hislen == 0 || firstc == NUL)	/* no history */
		    goto cmdline_not_changed;

		i = hiscnt;

		/* save current command string so it can be restored later */
		ccline.cmdbuff[ccline.cmdlen] = NUL;
		if (lookfor == NULL)
		{
		    if ((lookfor = vim_strsave(ccline.cmdbuff)) == NULL)
			goto cmdline_not_changed;
		    lookfor[ccline.cmdpos] = NUL;
		}

		j = STRLEN(lookfor);
		for (;;)
		{
		    /* one step backwards */
		    if (c == K_UP || c == K_S_UP || c == Ctrl('P') ||
			    c == K_PAGEUP || c == K_KPAGEUP)
		    {
			if (hiscnt == hislen)	/* first time */
			    hiscnt = hisidx[histype];
			else if (hiscnt == 0 && hisidx[histype] != hislen - 1)
			    hiscnt = hislen - 1;
			else if (hiscnt != hisidx[histype] + 1)
			    --hiscnt;
			else			/* at top of list */
			{
			    hiscnt = i;
			    break;
			}
		    }
		    else    /* one step forwards */
		    {
			/* on last entry, clear the line */
			if (hiscnt == hisidx[histype])
			{
			    hiscnt = hislen;
			    break;
			}

			/* not on a history line, nothing to do */
			if (hiscnt == hislen)
			    break;
			if (hiscnt == hislen - 1)   /* wrap around */
			    hiscnt = 0;
			else
			    ++hiscnt;
		    }
		    if (hiscnt < 0 || history[histype][hiscnt] == NULL)
		    {
			hiscnt = i;
			break;
		    }
		    if ((c != K_UP && c != K_DOWN) || hiscnt == i ||
			    STRNCMP(history[histype][hiscnt],
						    lookfor, (size_t)j) == 0)
			break;
		}

		if (hiscnt != i)	/* jumped to other entry */
		{
		    vim_free(ccline.cmdbuff);
		    if (hiscnt == hislen)
			p = lookfor;	/* back to the old one */
		    else
			p = history[histype][hiscnt];

		    alloc_cmdbuff((int)STRLEN(p));
		    if (ccline.cmdbuff == NULL)
			goto returncmd;
		    STRCPY(ccline.cmdbuff, p);

		    ccline.cmdpos = ccline.cmdlen = STRLEN(ccline.cmdbuff);
		    redrawcmd();
		    goto cmdline_changed;
		}
		beep_flush();
		goto cmdline_not_changed;

	case Ctrl('V'):
	case Ctrl('Q'):
#ifdef USE_MOUSE
		ignore_drag_release = TRUE;
#endif
		putcmdline('^');
		c = get_literal();	    /* get next (two) character(s) */
		do_abbr = FALSE;	    /* don't do abbreviation now */
		break;

#ifdef DIGRAPHS
	case Ctrl('K'):
#ifdef USE_MOUSE
		ignore_drag_release = TRUE;
#endif
		putcmdline('?');
#ifdef USE_GUI_WIN32
		dont_scroll = TRUE;	    /* disallow scrolling here */
#endif
		++no_mapping;
		++allow_keys;
		c = vgetc();
		--no_mapping;
		--allow_keys;
		if (c != ESC)		    /* ESC cancels CTRL-K */
		{
		    if (IS_SPECIAL(c))	    /* insert special key code */
			break;
		    if (charsize(c) == 1)
			putcmdline(c);
		    ++no_mapping;
		    ++allow_keys;
		    cc = vgetc();
		    --no_mapping;
		    --allow_keys;
		    if (cc != ESC)	    /* ESC cancels CTRL-K */
		    {
			c = getdigraph(c, cc, TRUE);
			break;
		    }
		}
		redrawcmd();
		goto cmdline_not_changed;
#endif /* DIGRAPHS */

#ifdef RIGHTLEFT
	case Ctrl('_'):	    /* CTRL-_: switch language mode */
#ifdef FKMAP
		if (p_altkeymap)
		{
		    cmd_fkmap = !cmd_fkmap;
		    if (cmd_fkmap)	/* in Farsi always in Insert mode */
			ccline.overstrike = FALSE;
		}
		else			    /* Hebrew is default */
#endif
		    cmd_hkmap = !cmd_hkmap;
		goto cmdline_not_changed;
#endif

	default:
#ifdef UNIX
		if (c == intr_char)
		{
		    gotesc = TRUE;	/* will free ccline.cmdbuff after
					   putting it in history */
		    goto returncmd;	/* back to cmd mode */
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		if (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		break;
	}

	/* we come here if we have a normal character */

	if (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c)) && ccheck_abbr(c))
	    goto cmdline_changed;

	/*
	 * put the character in the command line
	 */
	if (IS_SPECIAL(c) || mod_mask != 0x0)
	    put_on_cmdline(get_special_key_name(c, mod_mask), -1, TRUE);
	else
	{
	    IObuff[0] = c;
	    put_on_cmdline(IObuff, 1, TRUE);
	}
	goto cmdline_changed;

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_changed:
#ifdef EXTRA_SEARCH
	if (!incsearch_postponed)
	    continue;
#endif

cmdline_changed:
#ifdef EXTRA_SEARCH
	if (p_is && (firstc == '/' || firstc == '?'))
	{
	    /* if there is a character waiting, search and redraw later */
	    if (char_avail())
	    {
		incsearch_postponed = TRUE;
		continue;
	    }
	    incsearch_postponed = FALSE;
	    curwin->w_cursor = old_cursor;  /* start at old position */

	    /* If there is no command line, don't do anything */
	    if (ccline.cmdlen == 0)
		i = 0;
	    else
	    {
		ccline.cmdbuff[ccline.cmdlen] = NUL;
		emsg_off = TRUE;    /* So it doesn't beep if bad expr */
		i = do_search(NULL, firstc, ccline.cmdbuff, count,
				      SEARCH_KEEP + SEARCH_OPT + SEARCH_NOOF);
		emsg_off = FALSE;
	    }
	    if (i)
		highlight_match = TRUE;		/* highlight position */
	    else
		highlight_match = FALSE;	    /* don't highlight */

	    /* first restore the old curwin values, so the screen is
	     * positioned in the same way as the actual search command */
	    curwin->w_leftcol = old_leftcol;
	    curwin->w_topline = old_topline;
	    curwin->w_botline = old_botline;
	    update_topline();
	    /*
	     * First move cursor to end of match, then to start.  This moves
	     * the whole match onto the screen when 'nowrap' is set.
	     */
	    curwin->w_cursor.col += search_match_len;
	    validate_cursor();
	    curwin->w_cursor.col -= search_match_len;
	    validate_cursor();

	    update_screen(NOT_VALID);
	    redrawcmdline();
	    did_incsearch = TRUE;
	}
#endif
    }

returncmd:

#ifdef FKMAP
    cmd_fkmap = 0;
#endif

#ifdef EXTRA_SEARCH
    if (did_incsearch)
    {
	curwin->w_cursor = old_cursor;
	curwin->w_curswant = old_curswant;
	curwin->w_leftcol = old_leftcol;
	curwin->w_topline = old_topline;
	curwin->w_botline = old_botline;
	highlight_match = FALSE;
	validate_cursor();	/* needed for TAB */
	redraw_later(NOT_VALID);
    }
#endif

    if (ccline.cmdbuff != NULL)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
	ccline.cmdbuff[ccline.cmdlen] = NUL;
	if (ccline.cmdlen && firstc &&
				   (some_key_typed || histype == HIST_SEARCH))
	{
	    add_to_history(histype, ccline.cmdbuff);
	    if (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strsave(ccline.cmdbuff);
	    }
	}

	if (gotesc)	    /* abandon command line */
	{
	    vim_free(ccline.cmdbuff);
	    ccline.cmdbuff = NULL;
	    MSG("");
	    redraw_cmdline = TRUE;
	}
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    msg_check();
    msg_scroll = save_msg_scroll;
    redir_off = FALSE;

    State = save_State;
#ifdef USE_MOUSE
    setmouse();
#endif

    return ccline.cmdbuff;
}

/*
 * Get an Ex command line for the ":" command.
 */
/* ARGSUSED */
    char_u *
getexline(c, dummy, indent)
    int		c;		/* normally ':', NUL for ":append" */
    void	*dummy;		/* cookie not used */
    int		indent;		/* indent for inside conditionals */
{
    return getcmdline(c, 1L, indent);
}

/*
 * Get an Ex command line for Ex mode.
 * In Ex mode we only use the OS supplied line editing features and no
 * mappings or abbreviations.
 */
/* ARGSUSED */
    char_u *
getexmodeline(c, dummy, indent)
    int		c;		/* normally ':', NUL for ":append" */
    void	*dummy;		/* cookie not used */
    int		indent;		/* indent for inside conditionals */
{
    struct growarray	line_ga;
    int			len;
    int			off = 0;
    char_u		*p;
    int			finished = FALSE;
#if defined(USE_GUI) || defined(NO_COOKED_INPUT)
    int			startcol = 0;
    int			c1;
    int			escaped = FALSE;	/* CTRL-V typed */
#endif

    /* always start in column 0; write a newline if necessary */
    compute_cmdrow();
    if (msg_col)
	msg_putchar('\n');
    if (c == ':')
    {
	msg_putchar(':');
	while (indent-- > 0)
	    msg_putchar(' ');
#if defined(USE_GUI) || defined(NO_COOKED_INPUT)
	startcol = msg_col;
#endif
    }

    ga_init(&line_ga);
    line_ga.ga_itemsize = 1;
    line_ga.ga_growsize = 30;

    /*
     * Get the line, one character at a time.
     */
    got_int = FALSE;
    while (!got_int && !finished)
    {
	if (ga_grow(&line_ga, 40) == FAIL)
	    break;
	p = (char_u *)line_ga.ga_data + line_ga.ga_len;

	/* Get one character (inchar gets a third of maxlen characters!) */
	len = inchar(p + off, 3, -1L);
	if (len < 0)
	    continue;	    /* end of input script reached */
	/* for a special character, we need at least three characters */
	if ((*p == K_SPECIAL || *p == CSI) && off + len < 3)
	{
	    off += len;
	    continue;
	}
	len += off;
	off = 0;

	/*
	 * When using the GUI, and for systems that don't have cooked input,
	 * handle line editing here.
	 */
#if defined(USE_GUI) || defined(NO_COOKED_INPUT)
# ifndef NO_COOKED_INPUT
	if (gui.in_use)
# endif
	{
	    if (got_int)
	    {
		msg_putchar('\n');
		break;
	    }

	    while (len > 0)
	    {
		c1 = *p++;
		--len;
# ifndef NO_COOKED_INPUT
		if ((c1 == K_SPECIAL || c1 == CSI) && len >= 2)
# else
#  ifdef USE_GUI
		if ((c1 == K_SPECIAL || (c1 == CSI && gui.in_use)) && len >= 2)
#  else
		if (c1 == K_SPECIAL && len >= 2)
#  endif
# endif
		{
		    c1 = TO_SPECIAL(p[0], p[1]);
		    p += 2;
		    len -= 2;
		}

		if (!escaped)
		{
		    if (c1 == BS || c1 == K_BS || c1 == DEL || c1 == K_DEL)
		    {
			if (line_ga.ga_len > 0)
			{
			    msg_putchar('\b');
			    msg_putchar(' ');
			    msg_putchar('\b');
			    --line_ga.ga_len;
			    ++line_ga.ga_room;
			}
			continue;
		    }

		    if (c1 == Ctrl('U'))
		    {
			msg_col = startcol;
			msg_clr_eos();
			line_ga.ga_room += line_ga.ga_len;
			line_ga.ga_len = 0;
			continue;
		    }

		    if (c1 == Ctrl('V'))
		    {
			escaped = TRUE;
			continue;
		    }
		}

		((char_u *)line_ga.ga_data)[line_ga.ga_len] = c1;
		if (c1 == '\r')
		    msg_putchar('\n');
		else
		    msg_putchar(c1);
		++line_ga.ga_len;
		--line_ga.ga_room;
		escaped = FALSE;
	    }
	    windgoto(msg_row, msg_col);
	}
# ifndef NO_COOKED_INPUT
	else
# endif
#endif
#ifndef NO_COOKED_INPUT
	{
	    line_ga.ga_len += len;
	    line_ga.ga_room -= len;
	}
#endif
	p = (char_u *)(line_ga.ga_data) + line_ga.ga_len;
	while (line_ga.ga_len && (p[-1] == '\n' || p[-1] == '\r'))
	{
	    finished = TRUE;
	    --line_ga.ga_len;
	    --p;
	    *p = NUL;
	}
    }

    /* note that cursor has moved, because of the echoed <CR> */
    screen_down();
    /* make following messages go to the next line */
    msg_didout = FALSE;
    msg_col = 0;
    if (msg_row < Rows - 1)
	++msg_row;
    emsg_on_display = FALSE;		/* don't want ui_delay() */

    if (got_int)
	ga_clear(&line_ga);

    return (char_u *)line_ga.ga_data;
}

#ifdef USE_GUI
/*
 * Return TRUE if ccline.overstrike is on.
 */
    int
cmdline_overstrike()
{
    return ccline.overstrike;
}

/*
 * Return TRUE if the cursor is at the end of the cmdline.
 */
    int
cmdline_at_end()
{
    return (ccline.cmdpos >= ccline.cmdlen);
}
#endif

/*
 * Allocate a new command line buffer.
 * Assigns the new buffer to ccline.cmdbuff and ccline.cmdbufflen.
 * Returns the new value of ccline.cmdbuff and ccline.cmdbufflen.
 */
    static void
alloc_cmdbuff(len)
    int	    len;
{
    /*
     * give some extra space to avoid having to allocate all the time
     */
    if (len < 80)
	len = 100;
    else
	len += 20;

    ccline.cmdbuff = alloc(len);    /* caller should check for out-of-memory */
    ccline.cmdbufflen = len;
}

/*
 * Re-allocate the command line to length len + something extra.
 * return FAIL for failure, OK otherwise
 */
    static int
realloc_cmdbuff(len)
    int	    len;
{
    char_u	*p;

    p = ccline.cmdbuff;
    alloc_cmdbuff(len);			/* will get some more */
    if (ccline.cmdbuff == NULL)		/* out of memory */
    {
	ccline.cmdbuff = p;		/* keep the old one */
	return FAIL;
    }
    vim_memmove(ccline.cmdbuff, p, (size_t)ccline.cmdlen);
    vim_free(p);
    return OK;
}

/*
 * put a character on the command line.
 * Used for CTRL-V and CTRL-K
 */
    static void
putcmdline(c)
    int	    c;
{
    char_u  buf[1];

    buf[0] = c;
    msg_outtrans_len(buf, 1);
    msg_outtrans_len(ccline.cmdbuff + ccline.cmdpos,
					       ccline.cmdlen - ccline.cmdpos);
    cursorcmd();
}

/*
 * Put the given string, of the given length, onto the command line.
 * If len is -1, then STRLEN() is used to calculate the length.
 * If 'redraw' is TRUE then the new part of the command line, and the remaining
 * part will be redrawn, otherwise it will not.  If this function is called
 * twice in a row, then 'redraw' should be FALSE and redrawcmd() should be
 * called afterwards.
 */
    int
put_on_cmdline(str, len, redraw)
    char_u  *str;
    int	    len;
    int	    redraw;
{
    int	    i;

    if (len < 0)
	len = STRLEN(str);

    /* Check if ccline.cmdbuff needs to be longer */
    if (ccline.cmdlen + len + 1 >= ccline.cmdbufflen)
	i = realloc_cmdbuff(ccline.cmdlen + len);
    else
	i = OK;
    if (i == OK)
    {
	if (!ccline.overstrike)
	{
	    vim_memmove(ccline.cmdbuff + ccline.cmdpos + len,
					       ccline.cmdbuff + ccline.cmdpos,
				     (size_t)(ccline.cmdlen - ccline.cmdpos));
	    ccline.cmdlen += len;
	}
	else if (ccline.cmdpos + len > ccline.cmdlen)
	    ccline.cmdlen = ccline.cmdpos + len;
	vim_memmove(ccline.cmdbuff + ccline.cmdpos, str, (size_t)len);
	if (redraw)
	    msg_outtrans_len(ccline.cmdbuff + ccline.cmdpos,
					       ccline.cmdlen - ccline.cmdpos);
#ifdef FKMAP
	/*
	 ** If we are in Farsi command mode, the character input must be in
	 ** insert mode. So do not advance the cmdpos.
	 */
	if (!cmd_fkmap)
#endif
	{
	    ccline.cmdpos += len;
	    while (len--)
		ccline.cmdspos += charsize(str[len]);
	}
    }
    if (redraw)
	msg_check();
    return i;
}

/*
 * this fuction is called when the screen size changes and with incremental
 * search
 */
    void
redrawcmdline()
{
    msg_scrolled = 0;
    need_wait_return = FALSE;
    compute_cmdrow();
    redrawcmd();
    cursorcmd();
}

/*
 * Redraw what is currently on the command line.
 */
    static void
redrawcmd()
{
    int	    i;

    msg_start();
    if (ccline.cmdfirstc)
	msg_putchar(ccline.cmdfirstc);
    for (i = ccline.cmdindent; i > 0; --i)
	msg_putchar(' ');
    msg_outtrans_len(ccline.cmdbuff, ccline.cmdlen);
    msg_clr_eos();

    if (ccline.cmdfirstc)
	ccline.cmdspos = 1 + ccline.cmdindent;
    else
	ccline.cmdspos = 0 + ccline.cmdindent;
    for (i = 0; i < ccline.cmdlen && i < ccline.cmdpos; ++i)
	ccline.cmdspos += charsize(ccline.cmdbuff[i]);
    /*
     * An emsg() before may have set msg_scroll. This is used in normal mode,
     * in cmdline mode we can reset them now.
     */
    msg_scroll = FALSE;		/* next message overwrites cmdline */
}

    void
compute_cmdrow()
{
    if (exmode_active)
	cmdline_row = Rows - 1;
    else
	cmdline_row = lastwin->w_winpos + lastwin->w_height +
					lastwin->w_status_height;
}

    static void
cursorcmd()
{
    msg_row = cmdline_row + (ccline.cmdspos / (int)Columns);
    msg_col = ccline.cmdspos % (int)Columns;
    if (msg_row >= Rows)
	msg_row = Rows - 1;
    windgoto(msg_row, msg_col);
}

    void
gotocmdline(clr)
    int		    clr;
{
    msg_start();
    if (clr)		    /* clear the bottom line(s) */
	msg_clr_eos();	    /* will reset clear_cmdline */
    windgoto(cmdline_row, 0);
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
    if (p_paste || no_abbr)	    /* no abbreviations or in paste mode */
	return FALSE;

    return check_abbr(c, ccline.cmdbuff, ccline.cmdpos, 0);
}

/*
 * Return FAIL if this is not an appropriate context in which to do
 * completion of anything, return OK if it is (even if there are no matches).
 * For the caller, this means that the character is just passed through like a
 * normal character (instead of being expanded).  This allows :s/^I^D etc.
 */
    static int
nextwild(type)
    int	    type;
{
    int	    i;
    char_u  *p1;
    char_u  *p2;
    int	    oldlen;
    int	    difflen;
    int	    v;

    if (cmd_numfiles == -1)
	set_expand_context();
    if (expand_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	return OK;  /* Something illegal on command line */
    }
    if (expand_context == EXPAND_NOTHING)
    {
	/* Caller can use the character as a normal char instead */
	return FAIL;
    }
    expand_interactively = TRUE;

    MSG_PUTS("...");	    /* show that we are busy */
    out_flush();

    i = expand_pattern - ccline.cmdbuff;
    oldlen = ccline.cmdpos - i;

    if (type == WILD_NEXT || type == WILD_PREV)
    {
	/*
	 * Get next/previous match for a previous expanded pattern.
	 */
	p2 = ExpandOne(NULL, NULL, 0, type);
    }
    else
    {
	/*
	 * Translate string into pattern and expand it.
	 */
	if ((p1 = addstar(&ccline.cmdbuff[i], oldlen)) == NULL)
	    p2 = NULL;
	else
	{
	    p2 = ExpandOne(p1, vim_strnsave(&ccline.cmdbuff[i], oldlen),
						     WILD_HOME_REPLACE, type);
	    vim_free(p1);
	}
    }

    if (p2 != NULL)
    {
	if (ccline.cmdlen + (difflen = STRLEN(p2) - oldlen) >
							ccline.cmdbufflen - 4)
	    v = realloc_cmdbuff(ccline.cmdlen + difflen);
	else
	    v = OK;
	if (v == OK)
	{
	    vim_strncpy(&ccline.cmdbuff[ccline.cmdpos + difflen],
					       &ccline.cmdbuff[ccline.cmdpos],
		    ccline.cmdlen - ccline.cmdpos);
	    STRNCPY(&ccline.cmdbuff[i], p2, STRLEN(p2));
	    ccline.cmdlen += difflen;
	    ccline.cmdpos += difflen;
	}
	vim_free(p2);
    }

    redrawcmd();
    if (cmd_numfiles <= 0 && p2 == NULL)
	beep_flush();
    else if (cmd_numfiles == 1)
	(void)ExpandOne(NULL, NULL, 0, WILD_FREE);  /* free expanded pattern */

    expand_interactively = FALSE;	    /* reset for next call */
    return OK;
}

#define MAXSUFLEN 30	    /* maximum length of a file suffix */

/*
 * Do wildcard expansion on the string 'str'.
 * Return a pointer to alloced memory containing the new string.
 * Return NULL for failure.
 *
 * mode = WILD_FREE:	    just free previously expanded matches
 * mode = WILD_EXPAND_FREE: normal expansion, do not keep matches
 * mode = WILD_EXPAND_KEEP: normal expansion, keep matches
 * mode = WILD_NEXT:	    use next match in multiple match, wrap to first
 * mode = WILD_PREV:	    use previous match in multiple match, wrap to first
 * mode = WILD_ALL:	    return all matches concatenated
 * mode = WILD_LONGEST:	    return longest matched part
 *
 * options = WILD_LIST_NOTFOUND:    list entries without a match
 * options = WILD_HOME_REPLACE:	    do home_replace() for buffer names
 */
    char_u *
ExpandOne(str, orig, options, mode)
    char_u  *str;
    char_u  *orig;	    /* original string which is expanded */
    int	    options;
    int	    mode;
{
    char_u	*ss = NULL;
    static char_u **cmd_files = NULL;	/* list of input files */
    static int	findex;
    static char_u *orig_save = NULL;	/* kept value of orig */
    int		i, j;
    int		non_suf_match;		/* number without matching suffix */
    long_u	len;
    char_u	*setsuf;
    int		fnamelen, setsuflen;
    char_u	suf_buf[MAXSUFLEN];
    char_u	*p;

/*
 * first handle the case of using an old match
 */
    if (mode == WILD_NEXT || mode == WILD_PREV)
    {
	if (cmd_numfiles > 0)
	{
	    if (mode == WILD_PREV)
	    {
		if (findex == -1)
		    findex = cmd_numfiles;
		--findex;
	    }
	    else    /* mode == WILD_NEXT */
		++findex;

	    /*
	     * When wrapping around, return the original string, set findex to
	     * -1.
	     */
	    if (findex < 0)
	    {
		if (orig_save == NULL)
		    findex = cmd_numfiles - 1;
		else
		    findex = -1;
	    }
	    if (findex >= cmd_numfiles)
	    {
		if (orig_save == NULL)
		    findex = 0;
		else
		    findex = -1;
	    }
	    if (findex == -1)
		return vim_strsave(orig_save);
	    return vim_strsave(cmd_files[findex]);
	}
	else
	    return NULL;
    }

/* free old names */
    if (cmd_numfiles != -1 && mode != WILD_ALL && mode != WILD_LONGEST)
    {
	FreeWild(cmd_numfiles, cmd_files);
	cmd_numfiles = -1;
	vim_free(orig_save);
	orig_save = NULL;
    }
    findex = 0;

    if (mode == WILD_FREE)	/* only release file name */
	return NULL;

    if (cmd_numfiles == -1)
    {
	vim_free(orig_save);
	orig_save = orig;
	if (ExpandFromContext(str, &cmd_numfiles, &cmd_files, FALSE,
							     options) == FAIL)
	    /* error: do nothing */;
	else if (cmd_numfiles == 0)
	{
	    if (!expand_interactively)
		emsg(e_nomatch);
	}
	else
	{
	    /*
	     * May change home directory back to "~"
	     */
	    if (options & WILD_HOME_REPLACE)
		tilde_replace(str, cmd_numfiles, cmd_files);

	    /*
	     * Insert backslashes into a file name before a space, \, %, # and
	     * wildmatch characters, except '~'.
	     */
	    if (expand_interactively &&
		    (expand_context == EXPAND_FILES ||
		     expand_context == EXPAND_BUFFERS ||
		     expand_context == EXPAND_DIRECTORIES))
	    {
		for (i = 0; i < cmd_numfiles; ++i)
		{
		    p = vim_strsave_escaped(cmd_files[i],
#ifdef BACKSLASH_IN_FILENAME
						    (char_u *)" *?[{`$%#"
#else
# ifdef COLON_AS_PATHSEP
						    (char_u *)" *?[{`$%#/"
# else
						    (char_u *)" *?[{`$\\%#"
# endif
#endif
									    );
		    if (p != NULL)
		    {
			vim_free(cmd_files[i]);
			cmd_files[i] = p;
		    }
		}
	    }

	    if (mode != WILD_ALL && mode != WILD_LONGEST)
	    {
		non_suf_match = 1;
		if (cmd_numfiles > 1)	/* more than one match; check suffix */
		{
		    non_suf_match = 0;
		    for (i = 0; i < cmd_numfiles; ++i)
		    {
			fnamelen = STRLEN(cmd_files[i]);
			setsuflen = 0;
			for (setsuf = p_su; *setsuf; )
			{
			    setsuflen = copy_option_part(&setsuf, suf_buf,
							      MAXSUFLEN, ".,");
			    if (fnamelen >= setsuflen && STRNCMP(suf_buf,
					  cmd_files[i] + fnamelen - setsuflen,
						      (size_t)setsuflen) == 0)
				break;
			    setsuflen = 0;
			}
			if (setsuflen)	    /* suffix matched: ignore file */
			    continue;
			/*
			 * Move the name without matching suffix to the front
			 * of the list.  This makes CTRL-N work nice.
			 */
			p = cmd_files[i];
			for (j = i; j > non_suf_match; --j)
			    cmd_files[j] = cmd_files[j - 1];
			cmd_files[non_suf_match++] = p;
		    }
		}
		if (non_suf_match != 1)
		{
		    /* Can we ever get here unless it's while expanding
		     * interactively?  If not, we can get rid of this all
		     * together. Don't really want to wait for this message
		     * (and possibly have to hit return to continue!).
		     */
		    if (!expand_interactively)
			emsg(e_toomany);
		    else
			beep_flush();
		}
		if (!(non_suf_match != 1 && mode == WILD_EXPAND_FREE))
		    ss = vim_strsave(cmd_files[0]);
	    }
	}
    }

    /* Find longest common part */
    if (mode == WILD_LONGEST && cmd_numfiles > 0)
    {
	for (len = 0; cmd_files[0][len]; ++len)
	{
	    for (i = 0; i < cmd_numfiles; ++i)
	    {
#ifdef CASE_INSENSITIVE_FILENAME
		if (expand_context == EXPAND_DIRECTORIES
			|| expand_context == EXPAND_FILES
			|| expand_context == EXPAND_BUFFERS)
		{
		    if (TO_LOWER(cmd_files[i][len]) !=
						   TO_LOWER(cmd_files[0][len]))
			break;
		}
		else
#endif
		     if (cmd_files[i][len] != cmd_files[0][len])
		    break;
	    }
	    if (i < cmd_numfiles)
	    {
		vim_beep();
		break;
	    }
	}
	ss = alloc((unsigned)len + 1);
	if (ss)
	{
	    STRNCPY(ss, cmd_files[0], len);
	    ss[len] = NUL;
	}
	findex = -1;			    /* next p_wc gets first one */
    }

    /* Concatenate all matching names */
    if (mode == WILD_ALL && cmd_numfiles > 0)
    {
	len = 0;
	for (i = 0; i < cmd_numfiles; ++i)
	    len += STRLEN(cmd_files[i]) + 1;
	ss = lalloc(len, TRUE);
	if (ss != NULL)
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

    if (mode == WILD_EXPAND_FREE || mode == WILD_ALL)
    {
	FreeWild(cmd_numfiles, cmd_files);
	cmd_numfiles = -1;
    }
    return ss;
}

/*
 * For each file name in files[num_files]:
 * If 'orig_pat' starts with "~/", replace the home directory with "~".
 */
    void
tilde_replace(orig_pat, num_files, files)
    char_u  *orig_pat;
    int	    num_files;
    char_u  **files;
{
    int	    i;
    char_u  *p;

    if (orig_pat[0] == '~' && vim_ispathsep(orig_pat[1]))
    {
	for (i = 0; i < num_files; ++i)
	{
	    p = home_replace_save(NULL, files[i]);
	    if (p != NULL)
	    {
		vim_free(files[i]);
		files[i] = p;
	    }
	}
    }
}

/*
 * show all matches for completion on the command line
 */
    static int
showmatches()
{
    char_u	*file_str;
    int		num_files;
    char_u	**files_found;
    int		i, j, k;
    int		maxlen;
    int		lines;
    int		columns;
    char_u	*p;
    int		lastlen;
    int		attr;

    set_expand_context();
    if (expand_context == EXPAND_UNSUCCESSFUL)
    {
	beep_flush();
	return OK;  /* Something illegal on command line */
    }
    if (expand_context == EXPAND_NOTHING)
    {
	/* Caller can use the character as a normal char instead */
	return FAIL;
    }
    expand_interactively = TRUE;

    /* add star to file name, or convert to regexp if not expanding files! */
    file_str = addstar(expand_pattern,
		      (int)(ccline.cmdbuff + ccline.cmdpos - expand_pattern));
    if (file_str == NULL)
    {
	expand_interactively = FALSE;
	return OK;
    }

    msg_didany = FALSE;			/* lines_left will be set */
    msg_start();			/* prepare for paging */
    msg_putchar('\n');
    out_flush();
    cmdline_row = msg_row;
    msg_didany = FALSE;			/* lines_left will be set again */
    msg_start();			/* prepare for paging */

    /* find all files that match the description */
    if (ExpandFromContext(file_str, &num_files, &files_found, FALSE, 0) == FAIL)
    {
	num_files = 0;
	files_found = (char_u **)"";
    }

    /* find the length of the longest file name */
    maxlen = 0;
    for (i = 0; i < num_files; ++i)
    {
	if (expand_context == EXPAND_FILES || expand_context == EXPAND_BUFFERS)
	{
	    home_replace(NULL, files_found[i], NameBuff, MAXPATHL);
	    j = vim_strsize(NameBuff);
	}
	else
	    j = vim_strsize(files_found[i]);
	if (j > maxlen)
	    maxlen = j;
    }

    /* compute the number of columns and lines for the listing */
    maxlen += 2;    /* two spaces between file names */
    columns = ((int)Columns + 2) / maxlen;
    if (columns < 1)
	columns = 1;
    lines = (num_files + columns - 1) / columns;

    attr = highlight_attr[HLF_D];   /* find out highlighting for directories */

    /* list the files line by line */
    for (i = 0; i < lines; ++i)
    {
	lastlen = 999;
	for (k = i; k < num_files; k += lines)
	{
	    for (j = maxlen - lastlen; --j >= 0; )
		msg_putchar(' ');
	    if (expand_context == EXPAND_FILES ||
					     expand_context == EXPAND_BUFFERS)
	    {
			/* highlight directories */
		j = (mch_isdir(files_found[k]));
		home_replace(NULL, files_found[k], NameBuff, MAXPATHL);
		p = NameBuff;
	    }
	    else
	    {
		j = FALSE;
		p = files_found[k];
	    }
	    lastlen = msg_outtrans_attr(p, j ? attr : 0);
	}
	msg_putchar('\n');
	out_flush();		    /* show one line at a time */
	if (got_int)
	{
	    got_int = FALSE;
	    break;
	}
    }
    vim_free(file_str);
    FreeWild(num_files, files_found);

/*
 * we redraw the command below the lines that we have just listed
 * This is a bit tricky, but it saves a lot of screen updating.
 */
    cmdline_row = msg_row;	/* will put it back later */

    expand_interactively = FALSE;
    return OK;
}

/*
 * Prepare a string for expansion.
 * When expanding file names:  The string will be used with expand_wildcards().
 * Copy the file name into allocated memory and add a '*' at the end.
 * When expanding other names:	The string will be used with regcomp().  Copy
 * the name into allocated memory and add ".*" at the end.
 */
    char_u *
addstar(fname, len)
    char_u  *fname;
    int	    len;
{
    char_u  *retval;
    int	    i, j;
    int	    new_len;
    char_u  *tail;

    if (expand_interactively && expand_context != EXPAND_FILES &&
					 expand_context != EXPAND_DIRECTORIES)
    {
	/*
	 * Matching will be done internally (on something other than files).
	 * So we convert the file-matching-type wildcards into our kind for
	 * use with vim_regcomp().  First work out how long it will be:
	 */

	/* for help tags the translation is done in find_help_tags() */
	if (expand_context == EXPAND_HELP)
	    retval = vim_strnsave(fname, len);
	else
	{
	    new_len = len + 2;		/* +2 for '^' at start, NUL at end */
	    for (i = 0; i < len; i++)
	    {
		if (fname[i] == '*' || fname[i] == '~')
		    new_len++;		/* '*' needs to be replaced by ".*"
					   '~' needs to be replaced by "\~" */

		/* Buffer names are like file names.  "." should be literal */
		if (expand_context == EXPAND_BUFFERS && fname[i] == '.')
		    new_len++;		/* "." becomes "\." */
	    }
	    retval = alloc(new_len);
	    if (retval != NULL)
	    {
		retval[0] = '^';
		j = 1;
		for (i = 0; i < len; i++, j++)
		{
		    if (fname[i] == '\\' && ++i == len)	/* skip backslash */
			break;

		    switch (fname[i])
		    {
			case '*':   retval[j++] = '.';
				    break;
			case '~':   retval[j++] = '\\';
				    break;
			case '?':   retval[j] = '.';
				    continue;
			case '.':   if (expand_context == EXPAND_BUFFERS)
					retval[j++] = '\\';
				    break;
		    }
		    retval[j] = fname[i];
		}
		retval[j] = NUL;
	    }
	}
    }
    else
    {
	retval = alloc(len + 4);
	if (retval != NULL)
	{
	    STRNCPY(retval, fname, len);
	    retval[len] = NUL;
	    backslash_halve(retval, TRUE);	/* remove some backslashes */
	    len = STRLEN(retval);

	    /*
	     * Don't add a star to ~, ~user, $var or `cmd`.
	     * ~ would be at the start of the tail.
	     * $ could be anywhere in the tail.
	     * ` could be anywhere in the file name.
	     */
	    tail = gettail(retval);
	    if (*tail != '~' && vim_strchr(tail, '$') == NULL
					   && vim_strchr(retval, '`') == NULL)
	    {
#ifdef MSDOS
		/*
		 * if there is no dot in the file name, add "*.*" instead of
		 * "*".
		 */
		for (i = len - 1; i >= 0; --i)
		    if (vim_strchr((char_u *)".\\/:", retval[i]) != NULL)
			break;
		if (i < 0 || retval[i] != '.')
		{
		    retval[len++] = '*';
		    retval[len++] = '.';
		}
#endif
		retval[len++] = '*';
	    }
	    retval[len] = NUL;
	}
    }
    return retval;
}

/*
 * Must parse the command line so far to work out what context we are in.
 * Completion can then be done based on that context.
 * This routine sets two global variables:
 *  char_u *expand_pattern  The start of the pattern to be expanded within
 *				the command line (ends at the cursor).
 *  int expand_context	    The type of thing to expand.  Will be one of:
 *
 *  EXPAND_UNSUCCESSFUL	    Used sometimes when there is something illegal on
 *			    the command line, like an unknown command.	Caller
 *			    should beep.
 *  EXPAND_NOTHING	    Unrecognised context for completion, use char like
 *			    a normal char, rather than for completion.	eg
 *			    :s/^I/
 *  EXPAND_COMMANDS	    Cursor is still touching the command, so complete
 *			    it.
 *  EXPAND_BUFFERS	    Complete file names for :buf and :sbuf commands.
 *  EXPAND_FILES	    After command with XFILE set, or after setting
 *			    with P_EXPAND set.	eg :e ^I, :w>>^I
 *  EXPAND_DIRECTORIES	    In some cases this is used instead of the latter
 *			    when we know only directories are of interest.  eg
 *			    :set dir=^I
 *  EXPAND_SETTINGS	    Complete variable names.  eg :set d^I
 *  EXPAND_BOOL_SETTINGS    Complete boolean variables only,  eg :set no^I
 *  EXPAND_TAGS		    Complete tags from the files in p_tags.  eg :ta a^I
 *  EXPAND_HELP		    Complete tags from the file 'helpfile'/tags
 *  EXPAND_EVENTS	    Complete event names
 *  EXPAND_SYNTAX	    Complete :syntax command arguments
 *  EXPAND_HIGHLIGHT	    Complete highlight (syntax) group names
 *  EXPAND_AUGROUP	    Complete autocommand group names
 *
 * -- webb.
 */
    static void
set_expand_context()
{
    char_u	*nextcomm;
    int		old_char = NUL;

    if (ccline.cmdfirstc != ':')	/* only expansion for ':' commands */
    {
	expand_context = EXPAND_NOTHING;
	return;
    }

    /*
     * Avoid a UMR warning from Purify, only save the character if it has been
     * written before.
     */
    if (ccline.cmdpos < ccline.cmdlen)
	old_char = ccline.cmdbuff[ccline.cmdpos];
    ccline.cmdbuff[ccline.cmdpos] = NUL;
    nextcomm = ccline.cmdbuff;
    while (nextcomm != NULL)
	nextcomm = set_one_cmd_context(nextcomm);
    ccline.cmdbuff[ccline.cmdpos] = old_char;
}

/*
 * Do the expansion based on the global variables expand_context and
 * expand_pattern -- webb.
 */
    static int
ExpandFromContext(pat, num_file, file, files_only, options)
    char_u  *pat;
    int	    *num_file;
    char_u  ***file;
    int	    files_only;
    int	    options;
{
    vim_regexp	*prog;
    int		ret;
    int		flags;

    flags = 0;
    if (!files_only)
	flags |= EW_DIR;
    if (options & WILD_LIST_NOTFOUND)
	flags |= EW_NOTFOUND;

    if (!expand_interactively || expand_context == EXPAND_FILES)
	return expand_wildcards(1, &pat, num_file, file, flags|EW_FILE);
    else if (expand_context == EXPAND_DIRECTORIES)
	return expand_wildcards(1, &pat, num_file, file,
						 (flags | EW_DIR) & ~EW_FILE);

    *file = (char_u **)"";
    *num_file = 0;
    if (expand_context == EXPAND_OLD_SETTING)
	return ExpandOldSetting(num_file, file);

    if (expand_context == EXPAND_HELP)
	return find_help_tags(pat, num_file, file);

    set_reg_ic(pat);	    /* set reg_ic according to p_ic, p_scs and pat */

    if (expand_context == EXPAND_BUFFERS)
	return ExpandBufnames(pat, num_file, file, options);
    else if (expand_context == EXPAND_TAGS)
	return find_tags(pat, num_file, file, TAG_WILD | TAG_NAMES, MAXCOL);

    prog = vim_regcomp(pat, (int)p_magic);
    if (prog == NULL)
	return FAIL;

    if (expand_context == EXPAND_COMMANDS)
	ret = ExpandGeneric(prog, num_file, file, get_command_name);
    else if (expand_context == EXPAND_SETTINGS ||
				       expand_context == EXPAND_BOOL_SETTINGS)
	ret = ExpandSettings(prog, num_file, file);
#ifdef USE_GUI
    else if (expand_context == EXPAND_MENUS)
	ret = ExpandGeneric(prog, num_file, file, get_menu_name);
#endif
#ifdef SYNTAX_HL
    else if (expand_context == EXPAND_SYNTAX)
    {
	reg_ic = TRUE;
	ret = ExpandGeneric(prog, num_file, file, get_syntax_name);
    }
#endif
    else if (expand_context == EXPAND_HIGHLIGHT)
    {
	reg_ic = TRUE;
	ret = ExpandGeneric(prog, num_file, file, get_highlight_name);
    }
#ifdef AUTOCMD
    else if (expand_context == EXPAND_EVENTS)
    {
	reg_ic = TRUE;
	ret = ExpandGeneric(prog, num_file, file, get_event_name);
    }
    else if (expand_context == EXPAND_AUGROUP)
    {
	reg_ic = TRUE;
	ret = ExpandGeneric(prog, num_file, file, get_augroup_name);
    }
#endif
    else
	ret = FAIL;

    vim_free(prog);
    return ret;
}

/*
 * Expand a list of names.
 *
 * Generic function for command line completion.  It calls a function to
 * obtain strings, one by one.	The strings are matched against a regexp
 * program.  Matching strings are copied into an array, which is returned.
 *
 * Returns OK when no problems encountered, FAIL for error (out of memory).
 */
    int
ExpandGeneric(prog, num_file, file, func)
    vim_regexp	*prog;
    int		*num_file;
    char_u	***file;
    char_u	*((*func)__ARGS((int))); /* returns a string from the list */
{
    int i;
    int count = 0;
    int	loop;
    char_u  *str;

    /* do this loop twice:
     * loop == 0: count the number of matching names
     * loop == 1: copy the matching names into allocated memory
     */
    for (loop = 0; loop <= 1; ++loop)
    {
	for (i = 0; ; ++i)
	{
	    str = (*func)(i);
	    if (str == NULL)	    /* end of list */
		break;
	    if (*str == NUL)	    /* skip empty strings */
		continue;

	    if (vim_regexec(prog, str, TRUE))
	    {
		if (loop)
		    (*file)[count] = vim_strsave_escaped(str,
							  (char_u *)" \t\\.");
		++count;
	    }
	}
	if (loop == 0)
	{
	    if (count == 0)
		return OK;
	    *num_file = count;
	    *file = (char_u **)alloc((unsigned)(count * sizeof(char_u *)));
	    if (*file == NULL)
	    {
		*file = (char_u **)"";
		return FAIL;
	    }
	    count = 0;
	}
    }
    return OK;
}

/*********************************
 *  Command line history stuff	 *
 *********************************/

/*
 * Translate a history character to the associated type number.
 */
    static int
hist_char2type(c)
    int	    c;
{
    if (c == ':')
	return HIST_CMD;
    if (c == '=')
	return HIST_EXPR;
    return HIST_SEARCH;	    /* must be '?' or '/' */
}

/*
 * init_history() - Initialize the command line history.
 * Also used to re-allocate the history when the size changes.
 */
    static void
init_history()
{
    int	    newlen;	    /* new length of history table */
    char_u  **temp;
    int	    i;
    int	    j;
    int	    type;

    /*
     * If size of history table changed, reallocate it
     */
    newlen = (int)p_hi;
    if (newlen != hislen)			/* history length changed */
    {
	for (type = 0; type < HIST_COUNT; ++type)   /* adjust three tables */
	{
	    if (newlen)
	    {
		temp = (char_u **)lalloc((long_u)(newlen * sizeof(char_u *)),
									TRUE);
		if (temp == NULL)   /* out of memory! */
		{
		    if (type == 0)  /* first one: just keep the old length */
		    {
			newlen = hislen;
			break;
		    }
		    /* Already changed one table, now we can only have zero
		     * length for all tables. */
		    newlen = 0;
		    type = -1;
		    continue;
		}
	    }
	    else
		temp = NULL;
	    if (newlen == 0 || temp != NULL)
	    {
		if (hisidx[type] < 0)		/* there are no entries yet */
		{
		    for (i = 0; i < newlen; ++i)
			temp[i] = NULL;
		}
		else if (newlen > hislen)	/* array becomes bigger */
		{
		    for (i = 0; i <= hisidx[type]; ++i)
			temp[i] = history[type][i];
		    j = i;
		    for ( ; i <= newlen - (hislen - hisidx[type]); ++i)
			temp[i] = NULL;
		    for ( ; j < hislen; ++i, ++j)
			temp[i] = history[type][j];
		}
		else				/* array becomes smaller or 0 */
		{
		    j = hisidx[type];
		    for (i = newlen - 1; ; --i)
		    {
			if (i >= 0)		/* copy newest entries */
			    temp[i] = history[type][j];
			else			/* remove older entries */
			    vim_free(history[type][j]);
			if (--j < 0)
			    j = hislen - 1;
			if (j == hisidx[type])
			    break;
		    }
		    hisidx[type] = newlen - 1;
		}
		vim_free(history[type]);
		history[type] = temp;
	    }
	}
	hislen = newlen;
    }
}

/*
 * Check if command line 'str' is already in history.
 * If 'move_to_front' is TRUE, matching entry is moved to end of history.
 */
    static int
in_history(type, str, move_to_front)
    int	    type;
    char_u  *str;
    int	    move_to_front;	/* Move the entry to the front if it exists */
{
    int	    i;
    int	    last_i = -1;

    if (hisidx[type] < 0)
	return FALSE;
    i = hisidx[type];
    do
    {
	if (history[type][i] == NULL)
	    return FALSE;
	if (STRCMP(str, history[type][i]) == 0)
	{
	    if (!move_to_front)
		return TRUE;
	    last_i = i;
	    break;
	}
	if (--i < 0)
	    i = hislen - 1;
    } while (i != hisidx[type]);

    if (last_i >= 0)
    {
	str = history[type][i];
	while (i != hisidx[type])
	{
	    if (++i >= hislen)
		i = 0;
	    history[type][last_i] = history[type][i];
	    last_i = i;
	}
	history[type][i] = str;
	return TRUE;
    }
    return FALSE;
}

/*
 * Add the given string to the given history.  If the string is already in the
 * history then it is moved to the front.  "histype" may be HIST_CMD,
 * HIST_SEARCH or HIST_EXPR.
 */
    void
add_to_history(histype, new_entry)
    int		histype;
    char_u	*new_entry;
{
    static int	last_maptick = -1;	/* last seen maptick */

    if (hislen == 0)		/* no history */
	return;

    /*
     * Searches inside the same mapping overwrite each other, so that only
     * the last line is kept.  Be careful not to remove a line that was moved
     * down, only lines that were added.
     */
    if (histype == HIST_SEARCH)
    {
	if (maptick == last_maptick)
	{
	    /* Current line is from the same mapping, remove it */
	    vim_free(history[HIST_SEARCH][hisidx[HIST_SEARCH]]);
	    history[HIST_SEARCH][hisidx[HIST_SEARCH]] = NULL;
	    if (--hisidx[HIST_SEARCH] < 0)
		hisidx[HIST_SEARCH] = hislen - 1;
	}
	last_maptick = -1;
    }
    if (!in_history(histype, new_entry, TRUE))
    {
	if (++hisidx[histype] == hislen)
	    hisidx[histype] = 0;
	vim_free(history[histype][hisidx[histype]]);
	history[histype][hisidx[histype]] = vim_strsave(new_entry);
	if (histype == HIST_SEARCH)
	    last_maptick = maptick;
    }
}

#ifdef VIMINFO
static char_u **viminfo_history[HIST_COUNT] = {NULL, NULL, NULL};
static int	viminfo_hisidx[HIST_COUNT] = {0, 0, 0};
static int	viminfo_hislen[HIST_COUNT] = {0, 0, 0};
static int	viminfo_add_at_front = FALSE;

static int	hist_type2char __ARGS((int type, int use_question));

/*
 * Translate a history type number to the associated character.
 */
    static int
hist_type2char(type, use_question)
    int	    type;
    int	    use_question;	    /* use '?' instead of '/' */
{
    if (type == HIST_CMD)
	return ':';
    if (type == HIST_SEARCH)
    {
	if (use_question)
	    return '?';
	else
	    return '/';
    }
    return '=';
}

/*
 * Prepare for reading the history from the viminfo file.
 * This allocates history arrays to store the read history lines.
 */
    void
prepare_viminfo_history(asklen)
    int	    asklen;
{
    int	    i;
    int	    num;
    int	    type;
    int	    len;

    init_history();
    viminfo_add_at_front = (asklen != 0);
    if (asklen > hislen)
	asklen = hislen;

    for (type = 0; type < HIST_COUNT; ++type)
    {
	/*
	 * Count the number of empty spaces in the history list.  If there are
	 * more spaces available than we request, then fill them up.
	 */
	for (i = 0, num = 0; i < hislen; i++)
	    if (history[type][i] == NULL)
		num++;
	len = asklen;
	if (num > len)
	    len = num;
	if (len <= 0)
	    viminfo_history[type] = NULL;
	else
	    viminfo_history[type] =
		   (char_u **)lalloc((long_u)(len * sizeof(char_u *)), FALSE);
	if (viminfo_history[type] == NULL)
	    len = 0;
	viminfo_hislen[type] = len;
	viminfo_hisidx[type] = 0;
    }
}

/*
 * Accept a line from the viminfo, store it in the history array when it's
 * new.
 */
    int
read_viminfo_history(line, fp)
    char_u  *line;
    FILE    *fp;
{
    int	    type;

    type = hist_char2type(line[0]);
    if (viminfo_hisidx[type] < viminfo_hislen[type])
    {
	viminfo_readstring(line);
	if (!in_history(type, line + 1, viminfo_add_at_front))
	    viminfo_history[type][viminfo_hisidx[type]++] =
							vim_strsave(line + 1);
    }
    return vim_fgets(line, LSIZE, fp);
}

    void
finish_viminfo_history()
{
    int idx;
    int i;
    int	type;

    for (type = 0; type < HIST_COUNT; ++type)
    {
	if (history[type] == NULL)
	    return;
	idx = hisidx[type] + viminfo_hisidx[type];
	if (idx >= hislen)
	    idx -= hislen;
	else if (idx < 0)
	    idx = hislen - 1;
	if (viminfo_add_at_front)
	    hisidx[type] = idx;
	else
	{
	    if (hisidx[type] == -1)
		hisidx[type] = hislen - 1;
	    do
	    {
		if (history[type][idx] != NULL)
		    break;
		if (++idx == hislen)
		    idx = 0;
	    } while (idx != hisidx[type]);
	    if (idx != hisidx[type] && --idx < 0)
		idx = hislen - 1;
	}
	for (i = 0; i < viminfo_hisidx[type]; i++)
	{
	    vim_free(history[type][idx]);
	    history[type][idx] = viminfo_history[type][i];
	    if (--idx < 0)
		idx = hislen - 1;
	}
	vim_free(viminfo_history[type]);
	viminfo_history[type] = NULL;
    }
}

    void
write_viminfo_history(fp)
    FILE    *fp;
{
    int	    i;
    int	    type;
    int	    num_saved;

    init_history();
    if (hislen == 0)
	return;
    for (type = 0; type < HIST_COUNT; ++type)
    {
	num_saved = get_viminfo_parameter(hist_type2char(type, FALSE));
	if (num_saved == 0)
	    continue;
	if (num_saved < 0)  /* Use default */
	    num_saved = hislen;
	fprintf(fp, "\n# %s History (newest to oldest):\n",
			    type == HIST_CMD ? "Command Line" :
			    type == HIST_SEARCH ? "Search String" :
						    "Expression");
	if (num_saved > hislen)
	    num_saved = hislen;
	i = hisidx[type];
	if (i >= 0)
	    while (num_saved--)
	    {
		if (history[type][i] != NULL)
		{
		    putc(hist_type2char(type, TRUE), fp);
		    viminfo_writestring(fp, history[type][i]);
		}
		if (--i < 0)
		    i = hislen - 1;
	    }
    }
}
#endif /* VIMINFO */

#ifdef FKMAP
/*
 * Write a character at the current cursor+offset position.
 * It is directly written into the command buffer block.
 */
    void
cmd_pchar(c, offset)
    int	    c, offset;
{
    if (ccline.cmdpos + offset >= ccline.cmdlen || ccline.cmdpos + offset < 0)
    {
	EMSG("cmd_pchar beyond the command length");
	return;
    }
    ccline.cmdbuff[ccline.cmdpos + offset] = (char_u)c;
}

    int
cmd_gchar(offset)
    int	    offset;
{
    if (ccline.cmdpos + offset >= ccline.cmdlen || ccline.cmdpos + offset < 0)
    {
	/*  EMSG("cmd_gchar beyond the command length"); */
	return NUL;
    }
    return (int)ccline.cmdbuff[ccline.cmdpos + offset];
}
#endif
