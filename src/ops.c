/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * ops.c: implementation of various operators: op_shift, op_delete, op_tilde,
 *	  op_change, op_yank, do_put, do_join
 */

#include "vim.h"

/*
 * Number of registers.
 *	0 = unnamed register, for normal yanks and puts
 *   1..9 = number registers, for deletes
 * 10..35 = named registers
 *     36 = delete register (-)
 *     37 = Clipboard register (*). Only if USE_CLIPBOARD defined
 */
#ifdef USE_CLIPBOARD
# define NUM_REGISTERS		38
#else
# define NUM_REGISTERS		37
#endif

/*
 * Symbolic names for some registers.
 */
#define DELETION_REGISTER	36
#ifdef USE_CLIPBOARD
# define CLIPBOARD_REGISTER	37
#endif

/*
 * Each yank register is an array of pointers to lines.
 */
static struct yankreg
{
    char_u	**y_array;	/* pointer to array of line pointers */
    linenr_t	y_size;		/* number of lines in y_array */
    char_u	y_type;		/* MLINE, MCHAR or MBLOCK */
} y_regs[NUM_REGISTERS];

static struct yankreg	*y_current;	    /* ptr to current yankreg */
static int		y_append;	    /* TRUE when appending */
static struct yankreg	*y_previous = NULL; /* ptr to last written yankreg */

/*
 * structure used by block_prep, op_delete and op_yank for blockwise operators
 */
struct block_def
{
    int		startspaces;
    int		endspaces;
    int		textlen;
    char_u	*textstart;
    colnr_t	textcol;
};

#ifdef WANT_EVAL
static char_u	*get_expr_line __ARGS((void));
#endif
static void	get_yank_register __ARGS((int regname, int writing));
static int	stuff_yank __ARGS((int, char_u *));
static int	put_in_typebuf __ARGS((char_u *s, int colon));
static int	get_spec_reg __ARGS((int regname, char_u **argp, int *allocated));
static void	free_yank __ARGS((long));
static void	free_yank_all __ARGS((void));
static void	block_prep __ARGS((OPARG *oap, struct block_def *, linenr_t, int));
#if defined(USE_CLIPBOARD) || defined(WANT_EVAL)
static void	str_to_reg __ARGS((struct yankreg *y_ptr, int type, char_u *str, long len));
#endif
static int	same_leader __ARGS((int, char_u *, int, char_u *));
static int	fmt_end_block __ARGS((linenr_t, int *, char_u **));

/*
 * op_shift - handle a shift operation
 */
    void
op_shift(oap, curs_top, amount)
    OPARG	    *oap;
    int		    curs_top;
    int		    amount;
{
    long	    i;
    int		    first_char;

    if (u_save((linenr_t)(curwin->w_cursor.lnum - 1),
		 (linenr_t)(curwin->w_cursor.lnum + oap->line_count)) == FAIL)
	return;
    for (i = oap->line_count; --i >= 0; )
    {
	first_char = *ml_get_curline();
	if (first_char == NUL)				/* empty line */
	    curwin->w_cursor.col = 0;
	/*
	 * Don't move the line right if it starts with # and p_si is set.
	 */
	else
#if defined(SMARTINDENT) || defined(CINDENT)
	    if (first_char != '#' || (
# ifdef SMARTINDENT
			 !curbuf->b_p_si
# endif
# if defined(SMARTINDENT) && defined(CINDENT)
			    &&
# endif
# ifdef CINDENT
			 (!curbuf->b_p_cin || !in_cinkeys('#', ' ', TRUE))
# endif
					))
#endif
	{
	    /* if (oap->block_mode)
		    shift the block, not the whole line
	    else */
		shift_line(oap->op_type == OP_LSHIFT, p_sr, amount);
	}
	++curwin->w_cursor.lnum;
    }

    if (curs_top)	    /* put cursor on first line, for ">>" */
    {
	curwin->w_cursor.lnum -= oap->line_count;
	beginline(BL_SOL | BL_FIX);   /* shift_line() may have set cursor.col */
    }
    else
	--curwin->w_cursor.lnum;	/* put cursor on last line, for ":>" */
    update_topline();
    update_screen(NOT_VALID);

    if (oap->line_count > p_report)
       smsg((char_u *)"%ld line%s %ced %d time%s", oap->line_count,
	      plural(oap->line_count), (oap->op_type == OP_RSHIFT) ? '>' : '<',
			 amount, plural((long)amount));

    /*
     * Set "'[" and "']" marks.
     */
    curbuf->b_op_start = oap->start;
    curbuf->b_op_end = oap->end;
}

/*
 * shift the current line one shiftwidth left (if left != 0) or right
 * leaves cursor on first blank in the line
 */
    void
shift_line(left, round, amount)
    int left;
    int	round;
    int	amount;
{
    int	    count;
    int	    i, j;
    int	    p_sw = (int)curbuf->b_p_sw;

    count = get_indent();	    /* get current indent */

    if (round)			    /* round off indent */
    {
	i = count / p_sw;	    /* number of p_sw rounded down */
	j = count % p_sw;	    /* extra spaces */
	if (j && left)		    /* first remove extra spaces */
	    --amount;
	if (left)
	{
	    i -= amount;
	    if (i < 0)
		i = 0;
	}
	else
	    i += amount;
	count = i * p_sw;
    }
    else		/* original vi indent */
    {
	if (left)
	{
	    count -= p_sw * amount;
	    if (count < 0)
		count = 0;
	}
	else
	    count += p_sw * amount;
    }
    set_indent(count, TRUE);	    /* set new indent */
}

#if defined(LISPINDENT) || defined(CINDENT)
/*
 * op_reindent - handle reindenting a block of lines for C or lisp.
 *
 * mechanism copied from op_shift, above
 */
    void
op_reindent(oap, how)
    OPARG	*oap;
    int		(*how) __ARGS((void));
{
    long	i;
    char_u	*l;
    int		count;

    if (u_save((linenr_t)(curwin->w_cursor.lnum - 1),
		 (linenr_t)(curwin->w_cursor.lnum + oap->line_count)) == FAIL)
	return;

    for (i = oap->line_count; --i >= 0 && !got_int; )
    {
	/* it's a slow thing to do, so give feedback so there's no worry that
	 * the computer's just hung. */

	if (	   (i % 50 == 0
		    || i == oap->line_count - 1)
		&& oap->line_count > p_report)
	    smsg((char_u *)"%ld line%s to indent... ", i, plural(i));

	/*
	 * Be vi-compatible: For lisp indenting the first line is not
	 * indented, unless there is only one line.
	 */
#ifdef LISPINDENT
	if (i != oap->line_count - 1 || oap->line_count == 1 ||
						       how != get_lisp_indent)
#endif
	{
	    l = skipwhite(ml_get_curline());
	    if (*l == NUL)		    /* empty or blank line */
		count = 0;
	    else
		count = how();		    /* get the indent for this line */

	    set_indent(count, TRUE);
	}
	++curwin->w_cursor.lnum;
    }

    /* put cursor on first non-blank of indented line */
    curwin->w_cursor.lnum -= oap->line_count;
    beginline(BL_SOL | BL_FIX);

    update_topline();
    update_screen(NOT_VALID);

    if (oap->line_count > p_report)
    {
	i = oap->line_count - (i + 1);
	smsg((char_u *)"%ld line%s indented ", i, plural(i));
    }
    /* set '[ and '] marks */
    curbuf->b_op_start = oap->start;
    curbuf->b_op_end = oap->end;
}
#endif /* defined(LISPINDENT) || defined(CINDENT) */

#ifdef WANT_EVAL
/*
 * Keep the last expression line here, for repeating.
 */
static char_u	*expr_line = NULL;

/*
 * Get an expression for the "\"=expr1" or "CTRL-R =expr1"
 * Returns '=' when OK, NUL otherwise.
 */
    int
get_expr_register()
{
    char_u	*new_line;

    new_line = getcmdline('=', 0L, 0);
    if (new_line == NULL)
	return NUL;
    if (*new_line == NUL)	/* use previous line */
	vim_free(new_line);
    else
	set_expr_line(new_line);
    return '=';
}

/*
 * Set the expression for the '=' register.
 * Argument must be an allocated string.
 */
    void
set_expr_line(new_line)
    char_u	*new_line;
{
    vim_free(expr_line);
    expr_line = new_line;
}

/*
 * Get the result of the '=' register expression.
 * Returns a pointer to allocated memory, or NULL for failure.
 */
    static char_u *
get_expr_line()
{
    if (expr_line == NULL)
	return NULL;
    return eval_to_string(expr_line, NULL);
}
#endif /* WANT_EVAL */

/*
 * Check if 'regname' is a valid name of a yank register.
 * Note: There is no check for 0 (default register), caller should do this
 */
    int
valid_yank_reg(regname, writing)
    int	    regname;
    int	    writing;	    /* if TRUE check for writable registers */
{
    if (regname > '~')
	return FALSE;
    if (       isalnum(regname)
	    || (!writing && vim_strchr((char_u *)
#ifdef WANT_EVAL
				    ".%#:="
#else
				    ".%#:"
#endif
					, regname) != NULL)
	    || regname == '"'
	    || regname == '-'
#ifdef USE_CLIPBOARD
	    || (clipboard.available && regname == '*')
#endif
							)
	return TRUE;
    return FALSE;
}

/*
 * Set y_current and y_append, according to the value of "regname".
 *
 * If regname is 0 and writing, use register 0
 * If regname is 0 and reading, use previous register
 */
    static void
get_yank_register(regname, writing)
    int	    regname;
    int	    writing;
{
    int	    i;

    y_append = FALSE;
    if (((regname == 0 && !writing) || regname == '"') && y_previous != NULL)
    {
	y_current = y_previous;
	return;
    }
    i = regname;
    if (isdigit(i))
	i -= '0';
    else if (islower(i))
	i -= 'a' - 10;
    else if (isupper(i))
    {
	i -= 'A' - 10;
	y_append = TRUE;
    }
    else if (regname == '-')
	i = DELETION_REGISTER;
#ifdef USE_CLIPBOARD
    else if (clipboard.available && regname == '*')
	i = CLIPBOARD_REGISTER;
#endif
    else		/* not 0-9, a-z, A-Z or '-': use register 0 */
	i = 0;
    y_current = &(y_regs[i]);
    if (writing)	/* remember the register we write into for do_put() */
	y_previous = y_current;
}

/*
 * return TRUE if the current yank register has type MLINE
 */
    int
yank_register_mline(regname)
    int	    regname;
{
    if (regname != 0 && !valid_yank_reg(regname, FALSE))
	return FALSE;
    get_yank_register(regname, FALSE);
    return (y_current->y_type == MLINE);
}

/*
 * start or stop recording into a yank register
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_record(c)
    int c;
{
    char_u	*p;
    static int	regname;
    struct yankreg *old_y_previous, *old_y_current;
    int		retval;

    if (Recording == FALSE)	    /* start recording */
    {
			/* registers 0-9, a-z and " are allowed */
	if (c > '~' || (!isalnum(c) && c != '"'))
	    retval = FAIL;
	else
	{
	    Recording = TRUE;
	    showmode();
	    regname = c;
	    retval = OK;
	}
    }
    else			    /* stop recording */
    {
	Recording = FALSE;
	MSG("");
	p = get_recorded();
	if (p == NULL)
	    retval = FAIL;
	else
	{
	    /*
	     * We don't want to change the default register here, so save and
	     * restore the current register name.
	     */
	    old_y_previous = y_previous;
	    old_y_current = y_current;

	    retval = stuff_yank(regname, p);

	    y_previous = old_y_previous;
	    y_current = old_y_current;
	}
    }
    return retval;
}

/*
 * Stuff string 'p' into yank register 'regname' as a single line (append if
 * uppercase).	'p' must have been alloced.
 *
 * return FAIL for failure, OK otherwise
 */
    static int
stuff_yank(regname, p)
    int	    regname;
    char_u  *p;
{
    char_u *lp;
    char_u **pp;

					    /* check for read-only register */
    if (regname != 0 && !valid_yank_reg(regname, TRUE))
	return FAIL;
    get_yank_register(regname, TRUE);
    if (y_append && y_current->y_array != NULL)
    {
	pp = &(y_current->y_array[y_current->y_size - 1]);
	lp = lalloc((long_u)(STRLEN(*pp) + STRLEN(p) + 1), TRUE);
	if (lp == NULL)
	{
	    vim_free(p);
	    return FAIL;
	}
	STRCPY(lp, *pp);
	STRCAT(lp, p);
	vim_free(p);
	vim_free(*pp);
	*pp = lp;
    }
    else
    {
	free_yank_all();
	if ((y_current->y_array =
			(char_u **)alloc((unsigned)sizeof(char_u *))) == NULL)
	{
	    vim_free(p);
	    return FAIL;
	}
	y_current->y_array[0] = p;
	y_current->y_size = 1;
	y_current->y_type = MCHAR;  /* used to be MLINE, why? */
    }
    return OK;
}

/*
 * execute a yank register: copy it into the stuff buffer
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_execreg(regname, colon, addcr)
    int	    regname;
    int	    colon;		/* insert ':' before each line */
    int	    addcr;		/* always add '\n' to end of line */
{
    static int	lastc = NUL;
    long	i;
    char_u	*p;
    int		retval;


    if (regname == '@')			/* repeat previous one */
	regname = lastc;
					/* check for valid regname */
    if (regname == '%' || regname == '#' || !valid_yank_reg(regname, FALSE))
	return FAIL;
    lastc = regname;

    if (regname == ':')			/* use last command line */
    {
	if (last_cmdline == NULL)
	{
	    EMSG(e_nolastcmd);
	    return FAIL;
	}
	vim_free(new_last_cmdline); /* don't keep the cmdline containing @: */
	new_last_cmdline = NULL;
	retval = put_in_typebuf(last_cmdline, TRUE);
    }
#ifdef WANT_EVAL
    else if (regname == '=')
    {
	p = get_expr_line();
	if (p == NULL)
	    return FAIL;
	retval = put_in_typebuf(p, colon);
	vim_free(p);
	return retval;
    }
#endif
    else if (regname == '.')		/* use last inserted text */
    {
	p = get_last_insert_save();
	if (p == NULL)
	{
	    EMSG(e_noinstext);
	    return FAIL;
	}
	retval = put_in_typebuf(p, colon);
	vim_free(p);
	return retval;
    }
    else
    {
	get_yank_register(regname, FALSE);
	if (y_current->y_array == NULL)
	    return FAIL;

	/*
	 * Insert lines into typeahead buffer, from last one to first one.
	 */
	for (i = y_current->y_size; --i >= 0; )
	{
	/* insert newline between lines and after last line if type is MLINE */
	    if (y_current->y_type == MLINE || i < y_current->y_size - 1
								     || addcr)
	    {
		if (ins_typebuf((char_u *)"\n", FALSE, 0, TRUE) == FAIL)
		    return FAIL;
	    }
	    if (ins_typebuf(y_current->y_array[i], FALSE, 0, TRUE) == FAIL)
		return FAIL;
	    if (colon && ins_typebuf((char_u *)":", FALSE, 0, TRUE) == FAIL)
		return FAIL;
	}
	Exec_reg = TRUE;	/* disable the 'q' command */
    }
    return OK;
}

    static int
put_in_typebuf(s, colon)
    char_u	*s;
    int		colon;	    /* add ':' before the line */
{
    int		retval = OK;

    if (colon)
	retval = ins_typebuf((char_u *)"\n", FALSE, 0, TRUE);
    if (retval == OK)
	retval = ins_typebuf(s, FALSE, 0, TRUE);
    if (colon && retval == OK)
	retval = ins_typebuf((char_u *)":", FALSE, 0, TRUE);
    return retval;
}

/*
 * Insert a yank register: copy it into the Read buffer.
 * Used by CTRL-R command and middle mouse button in insert mode.
 *
 * return FAIL for failure, OK otherwise
 */
    int
insert_reg(regname)
    int regname;
{
    long    i;
    int	    retval = OK;
    char_u  *arg;
    int	    allocated;

    /*
     * It is possible to get into an endless loop by having CTRL-R a in
     * register a and then, in insert mode, doing CTRL-R a.
     * If you hit CTRL-C, the loop will be broken here.
     */
    ui_breakcheck();
    if (got_int)
	return FAIL;

    /* check for valid regname */
    if (regname != NUL && !valid_yank_reg(regname, FALSE))
	return FAIL;

#ifdef USE_CLIPBOARD
    if (regname == '*')
	clip_get_selection();		/* may fill * register */
#endif

    if (regname == '.')			/* insert last inserted text */
	retval = stuff_inserted(NUL, 1L, TRUE);
    else if (get_spec_reg(regname, &arg, &allocated))
    {
	if (arg == NULL)
	    return FAIL;
	stuffReadbuff(arg);
	if (allocated)
	    vim_free(arg);
    }
    else				/* name or number register */
    {
	get_yank_register(regname, FALSE);
	if (y_current->y_array == NULL)
	    retval = FAIL;
	else
	{
	    for (i = 0; i < y_current->y_size; ++i)
	    {
		stuffReadbuff(y_current->y_array[i]);
		/*
		 * Insert a newline between lines and after last line if
		 * y_type is MLINE.
		 */
		if (y_current->y_type == MLINE || i < y_current->y_size - 1)
		    stuffReadbuff((char_u *)"\n");
	    }
	}
    }

    return retval;
}

/*
 * If "regname" is a special register, return a pointer to its value.
 */
    static int
get_spec_reg(regname, argp, allocated)
    int	    regname;
    char_u  **argp;
    int	    *allocated;
{
    *argp = NULL;
    *allocated = FALSE;
    if (regname == '%')		/* file name */
    {
	if (check_fname() != FAIL)  /* will give emsg if not set */
	    *argp = curbuf->b_fname;
	return TRUE;
    }
    if (regname == '#')		/* alternate file name */
    {
	*argp = getaltfname();	/* will give emsg if not set */
	return TRUE;
    }
#ifdef WANT_EVAL
    if (regname == '=')		/* result of expression */
    {
	*argp = get_expr_line();
	*allocated = TRUE;
	return TRUE;
    }
#endif
    if (regname == ':')		/* last command line */
    {
	if (last_cmdline == NULL)
	    EMSG(e_nolastcmd);
	else
	    *argp = last_cmdline;
	return TRUE;
    }
    if (regname == '.')		/* last inserted text */
    {
	*argp = get_last_insert_save();
	*allocated = TRUE;
	if (*argp == NULL)
	    EMSG(e_noinstext);
	return TRUE;
    }

    return FALSE;
}

/*
 * paste a yank register into the command line.
 * used by CTRL-R command in command-line mode
 * insert_reg() can't be used here, because special characters from the
 * register contents will be interpreted as commands.
 *
 * return FAIL for failure, OK otherwise
 */
    int
cmdline_paste(regname)
    int regname;
{
    long	i;
    char_u	*arg;
    int		allocated;

    if (!valid_yank_reg(regname, FALSE))	/* check for valid regname */
	return FAIL;

#ifdef USE_CLIPBOARD
    if (regname == '*')
	clip_get_selection();
#endif

    if (regname == '.')			/* insert last inserted text */
	return FAIL;			/* Unimplemented */

    if (get_spec_reg(regname, &arg, &allocated))
    {
	if (arg == NULL)
	    return FAIL;
	i = put_on_cmdline(arg, -1, TRUE);
	if (allocated)
	    vim_free(arg);
	return (int)i;
    }

    get_yank_register(regname, FALSE);
    if (y_current->y_array == NULL)
	return FAIL;

    for (i = 0; i < y_current->y_size; ++i)
    {
	put_on_cmdline(y_current->y_array[i], -1, FALSE);

	/* insert ^M between lines and after last line if type is MLINE */
	if (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    put_on_cmdline((char_u *)"\r", 1, FALSE);
    }
    return OK;
}

/*
 * op_delete - handle a delete operation
 *
 * return FAIL if undo failed, OK otherwise.
 */
    int
op_delete(oap)
    OPARG   *oap;
{
    int			n;
    linenr_t		lnum;
    char_u		*ptr;
    char_u		*newp, *oldp;
    linenr_t		old_lcount = curbuf->b_ml.ml_line_count;
    int			did_yank = FALSE;
    struct block_def	bd;

    if (curbuf->b_ml.ml_flags & ML_EMPTY)	    /* nothing to do */
	return OK;

    /* Nothing to delete, return here.	Do prepare undo, for op_change(). */
    if (oap->empty)
    {
	return u_save_cursor();
    }

/*
 * Imitate the strange Vi behaviour: If the delete spans more than one line
 * and motion_type == MCHAR and the result is a blank line, make the delete
 * linewise.  Don't do this for the change command or Visual mode.
 */
    if (       oap->motion_type == MCHAR
	    && !oap->is_VIsual
	    && oap->line_count > 1
	    && oap->op_type == OP_DELETE)
    {
	ptr = ml_get(oap->end.lnum) + oap->end.col + oap->inclusive;
	ptr = skipwhite(ptr);
	if (*ptr == NUL && inindent(0))
	    oap->motion_type = MLINE;
    }

/*
 * Check for trying to delete (e.g. "D") in an empty line.
 * Note: For the change operator it is ok.
 */
    if (       oap->motion_type == MCHAR
	    && oap->line_count == 1
	    && oap->op_type == OP_DELETE
	    && *ml_get(oap->start.lnum) == NUL)
    {
	/*
	 * It's an error to operate on an empty region, when 'E' inclucded in
	 * 'cpoptions' (Vi compatible).
	 */
	if (vim_strchr(p_cpo, CPO_EMPTYREGION) != NULL)
	    beep_flush();
	return OK;
    }

/*
 * Do a yank of whatever we're about to delete.
 * If a yank register was specified, put the deleted text into that register
 */
    if (oap->regname != 0)
    {
					/* check for read-only register */
	if (!valid_yank_reg(oap->regname, TRUE))
	{
	    beep_flush();
	    return OK;
	}
	get_yank_register(oap->regname, TRUE);	/* yank into specified reg. */
	if (op_yank(oap, TRUE, FALSE) == OK)	/* yank without message */
	    did_yank = TRUE;
    }

/*
 * Put deleted text into register 1 and shift number registers if
 * the delete contains a line break, or when a regname has been specified!
 */
    if (oap->regname != 0 || oap->motion_type == MLINE || oap->line_count > 1)
    {
	y_current = &y_regs[9];
	free_yank_all();		/* free register nine */
	for (n = 9; n > 1; --n)
	    y_regs[n] = y_regs[n - 1];
	y_previous = y_current = &y_regs[1];
	y_regs[1].y_array = NULL;	/* set register one to empty */
	oap->regname = 0;
    }
    else if (oap->regname == 0)		/* yank into unnamed register */
    {
	oap->regname = '-';		/* use special delete register */
	get_yank_register(oap->regname, TRUE);
	oap->regname = 0;
    }

    if (oap->regname == 0 && op_yank(oap, TRUE, FALSE) == OK)
	did_yank = TRUE;

/*
 * If there's too much stuff to fit in the yank register, then get a
 * confirmation before doing the delete. This is crude, but simple. And it
 * avoids doing a delete of something we can't put back if we want.
 */
    if (!did_yank)
    {
	if (ask_yesno((char_u *)"cannot yank; delete anyway", TRUE) != 'y')
	{
	    emsg(e_abort);
	    return FAIL;
	}
    }

/*
 * block mode delete
 */
    if (oap->block_mode)
    {
	if (u_save((linenr_t)(oap->start.lnum - 1),
			       (linenr_t)(oap->end.lnum + 1)) == FAIL)
	    return FAIL;

	for (lnum = curwin->w_cursor.lnum;
			       curwin->w_cursor.lnum <= oap->end.lnum;
						      ++curwin->w_cursor.lnum)
	{
	    block_prep(oap, &bd, curwin->w_cursor.lnum, TRUE);
	    if (bd.textlen == 0)	/* nothing to delete */
		continue;

	    /* n == number of chars deleted
	     * If we delete a TAB, it may be replaced by several characters.
	     * Thus the number of characters may increase!
	     */
	    n = bd.textlen - bd.startspaces - bd.endspaces;
	    oldp = ml_get_curline();
	    newp = alloc_check((unsigned)STRLEN(oldp) + 1 - n);
	    if (newp == NULL)
		continue;
	    /* copy up to deleted part */
	    vim_memmove(newp, oldp, (size_t)bd.textcol);
	    /* insert spaces */
	    copy_spaces(newp + bd.textcol,
				     (size_t)(bd.startspaces + bd.endspaces));
	    /* copy the part after the deleted part */
	    oldp += bd.textcol + bd.textlen;
	    vim_memmove(newp + bd.textcol + bd.startspaces + bd.endspaces,
						      oldp, STRLEN(oldp) + 1);
	    /* replace the line */
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);
	}

	curwin->w_cursor.lnum = lnum;
	changed_cline_bef_curs();	/* recompute cursor pos. on screen */
	approximate_botline();		/* w_botline may be wrong now */
	adjust_cursor();

	CHANGED;
	update_screen(VALID_TO_CURSCHAR);
	oap->line_count = 0;	    /* no lines deleted */
    }
    else if (oap->motion_type == MLINE)
    {
	if (oap->op_type == OP_CHANGE)
	{
	    /* Delete the lines except the first one.  Temporarily move the
	     * cursor to the next line.  Save the current line number, if the
	     * last line is deleted it may be changed.
	     */
	    if (oap->line_count > 1)
	    {
		lnum = curwin->w_cursor.lnum;
		++curwin->w_cursor.lnum;
		del_lines((long)(oap->line_count - 1), TRUE, TRUE);
		curwin->w_cursor.lnum = lnum;
	    }
	    if (u_save_cursor() == FAIL)
		return FAIL;
	    if (curbuf->b_p_ai)		    /* don't delete indent */
	    {
		beginline(BL_WHITE);	    /* cursor on first non-white */
		did_ai = TRUE;		    /* delete the indent when ESC hit */
	    }
	    else
		beginline(0);		    /* cursor in column 0 */
	    truncate_line(FALSE);   /* delete the rest of the line */
				    /* leave cursor past last char in line */
	}
	else
	{
	    del_lines(oap->line_count, TRUE, TRUE);
	    beginline(BL_WHITE | BL_FIX);
	}
	u_clearline();	/* "U" command should not be possible after "dd" */
    }
    else if (oap->line_count == 1)	/* delete characters within one line */
    {
	if (u_save_cursor() == FAIL)
	    return FAIL;
	    /* if 'cpoptions' contains '$', display '$' at end of change */
	if (	   vim_strchr(p_cpo, CPO_DOLLAR) != NULL
		&& oap->op_type == OP_CHANGE
		&& oap->end.lnum == curwin->w_cursor.lnum
		&& !oap->is_VIsual)
	    display_dollar(oap->end.col - !oap->inclusive);
	n = oap->end.col - oap->start.col + 1 - !oap->inclusive;
	(void)del_chars((long)n, TRUE);
    }
    else				/* delete characters between lines */
    {
	if (u_save_cursor() == FAIL)	/* save first line for undo */
	    return FAIL;
	truncate_line(TRUE);		/* delete from cursor to end of line */

	oap->start = curwin->w_cursor;	/* remember curwin->w_cursor */
	++curwin->w_cursor.lnum;
					/* includes save for undo */
	del_lines((long)(oap->line_count - 2), TRUE, TRUE);

	if (u_save_cursor() == FAIL)	/* save last line for undo */
	    return FAIL;
	/* delete from start of line until op_end */
	curwin->w_cursor.col = 0;
	(void)del_chars((long)(oap->end.col + 1 - !oap->inclusive), TRUE);
	curwin->w_cursor = oap->start;	/* restore curwin->w_cursor */
	(void)do_join(FALSE, TRUE);
    }

    /*
     * For a change within one line, the screen is updated differently (to
     * take care of 'dollar').
     */
    if (oap->motion_type == MCHAR && oap->line_count == 1)
    {
	if (dollar_vcol)
	    must_redraw = 0;	    /* don't want a redraw now */
	else
	    update_screenline();
    }
    else if (!global_busy)	    /* no need to update screen for :global */
    {
	update_topline();
	update_screen(NOT_VALID);
    }

    msgmore(curbuf->b_ml.ml_line_count - old_lcount);

    /*
     * Set "'[" and "']" marks.
     */
    curbuf->b_op_start = oap->start;
    if (oap->block_mode)
    {
	curbuf->b_op_end.lnum = oap->end.lnum;
	curbuf->b_op_end.col = oap->start.col;
    }
    else
	curbuf->b_op_end = oap->start;

    return OK;
}

/*
 * op_tilde - handle the (non-standard vi) tilde operator
 */
    void
op_tilde(oap)
    OPARG	*oap;
{
    FPOS		pos;
    struct block_def	bd;

    if (u_save((linenr_t)(oap->start.lnum - 1),
				       (linenr_t)(oap->end.lnum + 1)) == FAIL)
	return;

    /*
     * Set '[ and '] marks.
     */
    curbuf->b_op_start = oap->start;
    curbuf->b_op_end = oap->end;

    pos = oap->start;
    if (oap->block_mode)		    /* Visual block mode */
    {
	for (; pos.lnum <= oap->end.lnum; ++pos.lnum)
	{
	    block_prep(oap, &bd, pos.lnum, FALSE);
	    pos.col = bd.textcol;
	    while (--bd.textlen >= 0)
	    {
		swapchar(oap->op_type, &pos);
		if (inc(&pos) == -1)	    /* at end of file */
		    break;
	    }
	}
    }
    else				    /* not block mode */
    {
	if (oap->motion_type == MLINE)
	{
	    pos.col = 0;
	    oap->end.col = STRLEN(ml_get(oap->end.lnum));
	    if (oap->end.col)
		--oap->end.col;
	}
	else if (!oap->inclusive)
	    dec(&(oap->end));

	while (ltoreq(pos, oap->end))
	{
	    swapchar(oap->op_type, &pos);
	    if (inc(&pos) == -1)    /* at end of file */
		break;
	}
    }

    if (oap->motion_type == MCHAR && oap->line_count == 1 && !oap->block_mode)
	update_screenline();
    else
    {
	update_topline();
	update_screen(NOT_VALID);
    }

    if (oap->line_count > p_report)
	smsg((char_u *)"%ld line%s ~ed",
				    oap->line_count, plural(oap->line_count));
}

/*
 * If op_type == OP_UPPER: make uppercase,
 * if op_type == OP_LOWER: make lowercase,
 * else swap case of character at 'pos'
 */
    void
swapchar(op_type, pos)
    int	    op_type;
    FPOS    *pos;
{
    int	    c;

    c = gchar(pos);
    if (islower(c) && op_type != OP_LOWER)
    {
	pchar(*pos, TO_UPPER(c));
	CHANGED;
    }
    else if (isupper(c) && op_type != OP_UPPER)
    {
	pchar(*pos, TO_LOWER(c));
	CHANGED;
    }
}

/*
 * op_change - handle a change operation
 *
 * return TRUE if edit() returns because of a CTRL-O command
 */
    int
op_change(oap)
    OPARG	*oap;
{
    colnr_t	       l;

    l = oap->start.col;
    if (oap->motion_type == MLINE)
    {
	l = 0;
#ifdef SMARTINDENT
	if (curbuf->b_p_si)
	    can_si = TRUE;	/* It's like opening a new line, do si */
#endif
    }

    if (op_delete(oap) == FAIL)
	return FALSE;

    if ((l > curwin->w_cursor.col) && !lineempty(curwin->w_cursor.lnum))
	inc_cursor();

#if defined(LISPINDENT) || defined(CINDENT)
    if (oap->motion_type == MLINE)
    {
# ifdef LISPINDENT
	if (curbuf->b_p_lisp && curbuf->b_p_ai)
	    fixthisline(get_lisp_indent);
# endif
# if defined(LISPINDENT) && defined(CINDENT)
	else
# endif
# ifdef CINDENT
	if (curbuf->b_p_cin)
	    fixthisline(get_c_indent);
# endif
    }
#endif

    return edit(NUL, FALSE, (linenr_t)1);
}

/*
 * set all the yank registers to empty (called from main())
 */
    void
init_yank()
{
    int		i;

    for (i = 0; i < NUM_REGISTERS; ++i)
	y_regs[i].y_array = NULL;
}

/*
 * Free "n" lines from the current yank register.
 * Called for normal freeing and in case of error.
 */
    static void
free_yank(n)
    long n;
{
    if (y_current->y_array != NULL)
    {
	long	    i;

	for (i = n; --i >= 0; )
	{
	    if ((i & 1023) == 1023)		    /* this may take a while */
	    {
		/*
		 * This message should never cause a hit-return message.
		 * Overwrite this message with any next message.
		 */
		++no_wait_return;
		smsg((char_u *)"freeing %ld lines", i + 1);
		--no_wait_return;
		msg_didout = FALSE;
		msg_col = 0;
	    }
	    vim_free(y_current->y_array[i]);
	}
	vim_free(y_current->y_array);
	y_current->y_array = NULL;
	if (n >= 1000)
	    MSG("");
    }
}

    static void
free_yank_all()
{
    free_yank(y_current->y_size);
}

/*
 * Yank the text between curwin->w_cursor and startpos into a yank register.
 * If we are to append (uppercase register), we first yank into a new yank
 * register and then concatenate the old and the new one (so we keep the old
 * one in case of out-of-memory).
 *
 * return FAIL for failure, OK otherwise
 */
    int
op_yank(oap, deleting, mess)
    OPARG   *oap;
    int	    deleting;
    int	    mess;
{
    long		y_idx;		/* index in y_array[] */
    struct yankreg	*curr;		/* copy of y_current */
    struct yankreg	newreg;		/* new yank register when appending */
    char_u		**new_ptr;
    linenr_t		lnum;		/* current line number */
    long		j;
    long		len;
    int			yanktype = oap->motion_type;
    long		yanklines = oap->line_count;
    linenr_t		yankendlnum = oap->end.lnum;
    char_u		*p;
    char_u		*pnew;
    struct block_def	bd;

				    /* check for read-only register */
    if (oap->regname != 0 && !valid_yank_reg(oap->regname, TRUE))
    {
	beep_flush();
	return FAIL;
    }
    if (!deleting)		    /* op_delete() already set y_current */
	get_yank_register(oap->regname, TRUE);

    curr = y_current;
				    /* append to existing contents */
    if (y_append && y_current->y_array != NULL)
	y_current = &newreg;
    else
	free_yank_all();	    /* free previously yanked lines */

/*
 * If the cursor was in column 1 before and after the movement, and the
 * operator is not inclusive, the yank is always linewise.
 */
    if (       oap->motion_type == MCHAR
	    && oap->start.col == 0
	    && !oap->inclusive
	    && oap->end.col == 0
	    && yanklines > 1)
    {
	yanktype = MLINE;
	--yankendlnum;
	--yanklines;
    }

    y_current->y_size = yanklines;
    y_current->y_type = yanktype;   /* set the yank register type */
    y_current->y_array = (char_u **)lalloc_clear((long_u)(sizeof(char_u *) *
							    yanklines), TRUE);

    if (y_current->y_array == NULL)
    {
	y_current = curr;
	return FAIL;
    }

    y_idx = 0;
    lnum = oap->start.lnum;

/*
 * Visual block mode
 */
    if (oap->block_mode)
    {
	y_current->y_type = MBLOCK;	    /* set the yank register type */
	for ( ; lnum <= yankendlnum; ++lnum)
	{
	    block_prep(oap, &bd, lnum, FALSE);

	    if ((pnew = alloc(bd.startspaces + bd.endspaces +
					  bd.textlen + 1)) == NULL)
		goto fail;
	    y_current->y_array[y_idx++] = pnew;

	    copy_spaces(pnew, (size_t)bd.startspaces);
	    pnew += bd.startspaces;

	    vim_memmove(pnew, bd.textstart, (size_t)bd.textlen);
	    pnew += bd.textlen;

	    copy_spaces(pnew, (size_t)bd.endspaces);
	    pnew += bd.endspaces;

	    *pnew = NUL;
	}
    }
    else
    {
	/*
	 * there are three parts for non-block mode:
	 * 1. if yanktype != MLINE yank last part of the top line
	 * 2. yank the lines between op_start and op_end, inclusive when
	 *    yanktype == MLINE
	 * 3. if yanktype != MLINE yank first part of the bot line
	 */
	if (yanktype != MLINE)
	{
	    if (yanklines == 1)	    /* op_start and op_end on same line */
	    {
		j = oap->end.col - oap->start.col + 1 - !oap->inclusive;
		/* Watch out for very big endcol (MAXCOL) */
		p = ml_get(lnum) + oap->start.col;
		len = STRLEN(p);
		if (j > len || j < 0)
		    j = len;
		if ((y_current->y_array[0] = vim_strnsave(p, (int)j)) == NULL)
		{
fail:
		    free_yank(y_idx);	/* free the allocated lines */
		    y_current = curr;
		    return FAIL;
		}
		goto success;
	    }
	    if ((y_current->y_array[0] =
			vim_strsave(ml_get(lnum++) + oap->start.col)) == NULL)
		goto fail;
	    ++y_idx;
	}

	while (yanktype == MLINE ? (lnum <= yankendlnum) : (lnum < yankendlnum))
	{
	    if ((y_current->y_array[y_idx] =
					 vim_strsave(ml_get(lnum++))) == NULL)
		goto fail;
	    ++y_idx;
	}
	if (yanktype != MLINE)
	{
	    if ((y_current->y_array[y_idx] = vim_strnsave(ml_get(yankendlnum),
				 oap->end.col + 1 - !oap->inclusive)) == NULL)
		goto fail;
	}
    }

success:
    if (curr != y_current)	/* append the new block to the old block */
    {
	new_ptr = (char_u **)lalloc((long_u)(sizeof(char_u *) *
				   (curr->y_size + y_current->y_size)), TRUE);
	if (new_ptr == NULL)
	    goto fail;
	for (j = 0; j < curr->y_size; ++j)
	    new_ptr[j] = curr->y_array[j];
	vim_free(curr->y_array);
	curr->y_array = new_ptr;

	if (yanktype == MLINE)	/* MLINE overrides MCHAR and MBLOCK */
	    curr->y_type = MLINE;

	/* concatenate the last line of the old block with the first line of
	 * the new block */
	if (curr->y_type == MCHAR)
	{
	    pnew = lalloc((long_u)(STRLEN(curr->y_array[curr->y_size - 1])
			      + STRLEN(y_current->y_array[0]) + 1), TRUE);
	    if (pnew == NULL)
	    {
		    y_idx = y_current->y_size - 1;
		    goto fail;
	    }
	    STRCPY(pnew, curr->y_array[--j]);
	    STRCAT(pnew, y_current->y_array[0]);
	    vim_free(curr->y_array[j]);
	    vim_free(y_current->y_array[0]);
	    curr->y_array[j++] = pnew;
	    y_idx = 1;
	}
	else
	    y_idx = 0;
	while (y_idx < y_current->y_size)
	    curr->y_array[j++] = y_current->y_array[y_idx++];
	curr->y_size = j;
	vim_free(y_current->y_array);
	y_current = curr;
    }
    if (mess)			/* Display message about yank? */
    {
	if (yanktype == MCHAR && !oap->block_mode && yanklines == 1)
	    yanklines = 0;
	/* Some versions of Vi use ">=" here, some don't...  */
	if (yanklines > p_report)
	{
	    /* redisplay now, so message is not deleted */
	    update_topline_redraw();
	    smsg((char_u *)"%ld line%s yanked", yanklines, plural(yanklines));
	}
    }

    /*
     * Set "'[" and "']" marks.
     */
    curbuf->b_op_start = oap->start;
    curbuf->b_op_end = oap->end;

#ifdef USE_CLIPBOARD
    /*
     * If we were yanking to the clipboard register, send result to clipboard.
     */
    if (curr == &(y_regs[CLIPBOARD_REGISTER]))
    {
	clip_own_selection();
	clip_mch_set_selection();
    }
#endif

    return OK;
}

/*
 * put contents of register "regname" into the text
 * For ":put" command count == -1.
 */
    void
do_put(regname, dir, count, fix_indent)
    int	    regname;
    int	    dir;		/* BACKWARD for 'P', FORWARD for 'p' */
    long    count;
    int	    fix_indent;		/* make indent look nice */
{
    char_u	*ptr;
    char_u	*newp, *oldp;
    int		yanklen;
    int		oldlen;
    int		totlen = 0;		    /* init for gcc */
    linenr_t	lnum;
    colnr_t	col;
    long	i;			    /* index in y_array[] */
    int		y_type;
    long	y_size;
    char_u	**y_array;
    long	nr_lines = 0;
    colnr_t	vcol;
    int		delcount;
    int		incr = 0;
    long	j;
    FPOS	new_cursor;
    int		indent;
    int		orig_indent = 0;	    /* init for gcc */
    int		indent_diff = 0;	    /* init for gcc */
    int		first_indent = TRUE;
    FPOS	old_pos;
    struct block_def bd;
    char_u	*insert_string = NULL;
    int		allocated = FALSE;

#ifdef USE_CLIPBOARD
    if (regname == '*')
	clip_get_selection();
#endif

    if (fix_indent)
	orig_indent = get_indent();

    curbuf->b_op_start = curwin->w_cursor;	/* default for '[ mark */
    if (dir == FORWARD)
	curbuf->b_op_start.col++;
    curbuf->b_op_end = curwin->w_cursor;	/* default for '] mark */

    /*
     * Using inserted text works differently, because the register includes
     * special characters (newlines, etc.).
     */
    if (regname == '.')
    {
	(void)stuff_inserted((dir == FORWARD ? (count == -1 ? 'o' : 'a') :
				    (count == -1 ? 'O' : 'i')), count, FALSE);
	return;
    }

    /*
     * For special registers '%' (file name), '#' (alternate file name) and
     * ':' (last command line), etc. we have to create a fake yank register.
     */
    if (get_spec_reg(regname, &insert_string, &allocated))
    {
	if (insert_string == NULL)
	    return;
    }

    if (insert_string != NULL)
    {
	y_type = MCHAR;			/* use fake one-line yank register */
	y_size = 1;
	y_array = &insert_string;
    }
    else
    {
	get_yank_register(regname, FALSE);

	y_type = y_current->y_type;
	y_size = y_current->y_size;
	y_array = y_current->y_array;
    }

    if (count == -1)	    /* :put command */
    {
	y_type = MLINE;
	count = 1;
    }

    if (y_size == 0 || y_array == NULL)
    {
	EMSG2("Nothing in register %s",
		  regname == 0 ? (char_u *)"\"" : transchar(regname));
	goto end;
    }

    if (y_type == MBLOCK)
    {
	lnum = curwin->w_cursor.lnum + y_size + 1;
	if (lnum > curbuf->b_ml.ml_line_count)
	    lnum = curbuf->b_ml.ml_line_count + 1;
	if (u_save(curwin->w_cursor.lnum - 1, lnum) == FAIL)
	    goto end;
    }
    else if (u_save_cursor() == FAIL)
	goto end;

    yanklen = STRLEN(y_array[0]);
    CHANGED;

    lnum = curwin->w_cursor.lnum;
    col = curwin->w_cursor.col;
    approximate_botline();	    /* w_botline might not be valid now */
    changed_cline_bef_curs();	    /* cursor posn on screen may change */

/*
 * block mode
 */
    if (y_type == MBLOCK)
    {
	if (dir == FORWARD && gchar_cursor() != NUL)
	{
	    getvcol(curwin, &curwin->w_cursor, NULL, NULL, &col);
	    ++col;
	    ++curwin->w_cursor.col;
	}
	else
	    getvcol(curwin, &curwin->w_cursor, &col, NULL, NULL);
	for (i = 0; i < y_size; ++i)
	{
	    bd.startspaces = 0;
	    bd.endspaces = 0;
	    bd.textcol = 0;
	    vcol = 0;
	    delcount = 0;

	/* add a new line */
	    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	    {
		ml_append(curbuf->b_ml.ml_line_count, (char_u *)"",
							   (colnr_t)1, FALSE);
		++nr_lines;
	    }
	    oldp = ml_get_curline();
	    oldlen = STRLEN(oldp);
	    for (ptr = oldp; vcol < col && *ptr; ++ptr)
	    {
		/* Count a tab for what it's worth (if list mode not on) */
		incr = lbr_chartabsize(ptr, (colnr_t)vcol);
		vcol += incr;
		++bd.textcol;
	    }
	    if (vcol < col) /* line too short, padd with spaces */
	    {
		bd.startspaces = col - vcol;
	    }
	    else if (vcol > col)
	    {
		bd.endspaces = vcol - col;
		bd.startspaces = incr - bd.endspaces;
		--bd.textcol;
		delcount = 1;
	    }
	    yanklen = STRLEN(y_array[i]);
	    totlen = count * yanklen + bd.startspaces + bd.endspaces;
	    newp = alloc_check((unsigned)totlen + oldlen + 1);
	    if (newp == NULL)
		break;
	/* copy part up to cursor to new line */
	    ptr = newp;
	    vim_memmove(ptr, oldp, (size_t)bd.textcol);
	    ptr += bd.textcol;
	/* may insert some spaces before the new text */
	    copy_spaces(ptr, (size_t)bd.startspaces);
	    ptr += bd.startspaces;
	/* insert the new text */
	    for (j = 0; j < count; ++j)
	    {
		vim_memmove(ptr, y_array[i], (size_t)yanklen);
		ptr += yanklen;
	    }
	/* may insert some spaces after the new text */
	    copy_spaces(ptr, (size_t)bd.endspaces);
	    ptr += bd.endspaces;
	/* move the text after the cursor to the end of the line. */
	    vim_memmove(ptr, oldp + bd.textcol + delcount,
				(size_t)(oldlen - bd.textcol - delcount + 1));
	    ml_replace(curwin->w_cursor.lnum, newp, FALSE);

	    ++curwin->w_cursor.lnum;
	    if (i == 0)
		curwin->w_cursor.col += bd.startspaces;
	}
						/* adjust '] mark */
	curbuf->b_op_end.lnum = curwin->w_cursor.lnum - 1;
	curbuf->b_op_end.col = bd.textcol + totlen - 1;
	curwin->w_cursor.lnum = lnum;
	update_topline();
	update_screen(VALID_TO_CURSCHAR);
    }
    else	/* not block mode */
    {
	if (y_type == MCHAR)
	{
    /* if type is MCHAR, FORWARD is the same as BACKWARD on the next char */
	    if (dir == FORWARD && gchar_cursor() != NUL)
	    {
		++col;
		if (yanklen)
		{
		    ++curwin->w_cursor.col;
		    ++curbuf->b_op_end.col;
		}
	    }
	    new_cursor = curwin->w_cursor;
	}
	else if (dir == BACKWARD)
    /* if type is MLINE, BACKWARD is the same as FORWARD on the previous line */
	    --lnum;

/*
 * simple case: insert into current line
 */
	if (y_type == MCHAR && y_size == 1)
	{
	    totlen = count * yanklen;
	    if (totlen)
	    {
		oldp = ml_get(lnum);
		newp = alloc_check((unsigned)(STRLEN(oldp) + totlen + 1));
		if (newp == NULL)
		    goto end;		/* alloc() will give error message */
		vim_memmove(newp, oldp, (size_t)col);
		ptr = newp + col;
		for (i = 0; i < count; ++i)
		{
		    vim_memmove(ptr, y_array[0], (size_t)yanklen);
		    ptr += yanklen;
		}
		vim_memmove(ptr, oldp + col, STRLEN(oldp + col) + 1);
		ml_replace(lnum, newp, FALSE);
		/* Put cursor on last putted char. */
		curwin->w_cursor.col += (colnr_t)(totlen - 1);
	    }
	    curbuf->b_op_end = curwin->w_cursor;
	    /* For "CTRL-O p" in Insert mode, put cursor after last char */
	    if (totlen && restart_edit)
		++curwin->w_cursor.col;
	    update_screenline();
	}
	else
	{
	    while (--count >= 0)
	    {
		i = 0;
		if (y_type == MCHAR)
		{
		    /*
		     * Split the current line in two at the insert position.
		     * First insert y_array[size - 1] in front of second line.
		     * Then append y_array[0] to first line.
		     */
		    ptr = ml_get(lnum) + col;
		    totlen = STRLEN(y_array[y_size - 1]);
		    newp = alloc_check((unsigned)(STRLEN(ptr) + totlen + 1));
		    if (newp == NULL)
			goto error;
		    STRCPY(newp, y_array[y_size - 1]);
		    STRCAT(newp, ptr);
			/* insert second line */
		    ml_append(lnum, newp, (colnr_t)0, FALSE);
		    vim_free(newp);

		    oldp = ml_get(lnum);
		    newp = alloc_check((unsigned)(col + yanklen + 1));
		    if (newp == NULL)
			goto error;
					    /* copy first part of line */
		    vim_memmove(newp, oldp, (size_t)col);
					    /* append to first line */
		    vim_memmove(newp + col, y_array[0], (size_t)(yanklen + 1));
		    ml_replace(lnum, newp, FALSE);

		    curwin->w_cursor.lnum = lnum;
		    i = 1;
		}

		while (i < y_size)
		{
		    if ((y_type != MCHAR || i < y_size - 1) &&
			ml_append(lnum, y_array[i], (colnr_t)0, FALSE) == FAIL)
			    goto error;
		    lnum++;
		    i++;
		    if (fix_indent)
		    {
			old_pos = curwin->w_cursor;
			curwin->w_cursor.lnum = lnum;
			ptr = ml_get(lnum);
#if defined(SMARTINDENT) || defined(CINDENT)
			if (*ptr == '#'
# ifdef SMARTINDENT
			   && curbuf->b_p_si
# endif
# ifdef CINDENT
			   && curbuf->b_p_cin && in_cinkeys('#', ' ', TRUE)
# endif
					    )
			    indent = 0;     /* Leave # lines at start */
			else
#endif
			     if (*ptr == NUL)
			    indent = 0;     /* Ignore empty lines */
			else if (first_indent)
			{
			    indent_diff = orig_indent - get_indent();
			    indent = orig_indent;
			    first_indent = FALSE;
			}
			else if ((indent = get_indent() + indent_diff) < 0)
			    indent = 0;
			set_indent(indent, TRUE);
			curwin->w_cursor = old_pos;
		    }
		    ++nr_lines;
		}
	    }

	    /* put '] mark at last inserted character */
	    curbuf->b_op_end.lnum = lnum;
	    col = STRLEN(y_array[y_size - 1]);
	    if (col > 1)
		curbuf->b_op_end.col = col - 1;
	    else
		curbuf->b_op_end.col = 0;

	    if (y_type == MLINE)
	    {
		/* put cursor onfirst non-blank in first inserted line */
		curwin->w_cursor.col = 0;
		if (dir == FORWARD)
		    ++curwin->w_cursor.lnum;
		beginline(BL_WHITE | BL_FIX);
	    }
	    else	/* put cursor on first inserted character */
	    {
		curwin->w_cursor = new_cursor;
	    }

error:
	    if (y_type == MLINE)	/* adjust '[ mark */
	    {
		curbuf->b_op_start.col = 0;
		if (dir == FORWARD)
		    curbuf->b_op_start.lnum++;
	    }
	    mark_adjust(curbuf->b_op_start.lnum + (y_type == MCHAR),
					     (linenr_t)MAXLNUM, nr_lines, 0L);
	    update_topline();
	    update_screen(NOT_VALID);
	}
    }

    msgmore(nr_lines);
    curwin->w_set_curswant = TRUE;

end:
    if (allocated)
	vim_free(insert_string);
}

/* Return the character name of the register with the given number */
    int
get_register_name(num)
    int num;
{
    if (num == -1)
	return '"';
    else if (num < 10)
	return num + '0';
    else if (num == DELETION_REGISTER)
	return '-';
#ifdef USE_CLIPBOARD
    else if (num == CLIPBOARD_REGISTER)
	return '*';
#endif
    else
	return num + 'a' - 10;
}

/*
 * display the contents of the yank registers
 */
    void
do_dis(arg)
    char_u *arg;
{
    int		    i, n;
    long	    j;
    char_u	    *p;
    struct yankreg  *yb;
    char_u	    name;
    int		    attr;

    if (arg != NULL && *arg == NUL)
	arg = NULL;
    attr = highlight_attr[HLF_8];

    /* Highlight title */
    MSG_PUTS_TITLE("\n--- Registers ---");
    for (i = -1; i < NUM_REGISTERS; ++i)
    {
	if (i == -1)
	{
	    if (y_previous != NULL)
		yb = y_previous;
	    else
		yb = &(y_regs[0]);
	}
	else
	    yb = &(y_regs[i]);
	name = get_register_name(i);
	if (yb->y_array != NULL && (arg == NULL ||
					       vim_strchr(arg, name) != NULL))
	{
	    msg_putchar('\n');
	    msg_putchar('"');
	    msg_putchar(name);
	    MSG_PUTS("   ");

	    n = (int)Columns - 6;
	    for (j = 0; j < yb->y_size && n > 1; ++j)
	    {
		if (j)
		{
		    MSG_PUTS_ATTR("^J", attr);
		    n -= 2;
		}
		for (p = yb->y_array[j]; *p && (n -= charsize(*p)) >= 0; ++p)
		    msg_outtrans_len(p, 1);
	    }
	    if (n > 1 && yb->y_type == MLINE)
		MSG_PUTS_ATTR("^J", attr);
	    out_flush();		    /* show one line at a time */
	}
    }

    /*
     * display last inserted text
     */
    if ((p = get_last_insert()) != NULL &&
	(arg == NULL || vim_strchr(arg, '.') != NULL))
    {
	MSG_PUTS("\n\".   ");
	dis_msg(p, TRUE);
    }

    /*
     * display last command line
     */
    if (last_cmdline != NULL && (arg == NULL || vim_strchr(arg, ':') != NULL))
    {
	MSG_PUTS("\n\":   ");
	dis_msg(last_cmdline, FALSE);
    }

    /*
     * display current file name
     */
    if (curbuf->b_fname != NULL &&
				(arg == NULL || vim_strchr(arg, '%') != NULL))
    {
	MSG_PUTS("\n\"%   ");
	dis_msg(curbuf->b_fname, FALSE);
    }

    /*
     * display alternate file name
     */
    if (arg == NULL || vim_strchr(arg, '%') != NULL)
    {
	char_u	    *fname;
	linenr_t    dummy;

	if (buflist_name_nr(0, &fname, &dummy) != FAIL)
	{
	    MSG_PUTS("\n\"#   ");
	    dis_msg(fname, FALSE);
	}
    }

#ifdef WANT_EVAL
    /*
     * display last used expression
     */
    if (expr_line != NULL && (arg == NULL || vim_strchr(arg, '=') != NULL))
    {
	MSG_PUTS("\n\"=   ");
	dis_msg(expr_line, FALSE);
    }
#endif
}

/*
 * display a string for do_dis()
 * truncate at end of screen line
 */
    void
dis_msg(p, skip_esc)
    char_u	*p;
    int		skip_esc;	    /* if TRUE, ignore trailing ESC */
{
    int	    n;

    n = (int)Columns - 6;
    while (*p && !(*p == ESC && skip_esc && *(p + 1) == NUL) &&
			(n -= charsize(*p)) >= 0)
	msg_outtrans_len(p++, 1);
}

/*
 * join 'count' lines (minimal 2), including u_save()
 */
    void
do_do_join(count, insert_space, redraw)
    long    count;
    int	    insert_space;
    int	    redraw;		    /* can redraw, curwin->w_wcol valid */
{
    if (u_save((linenr_t)(curwin->w_cursor.lnum - 1),
		    (linenr_t)(curwin->w_cursor.lnum + count)) == FAIL)
	return;

    if (count > 10)
	redraw = FALSE;		    /* don't redraw each small change */
    while (--count > 0)
    {
	line_breakcheck();
	if (got_int || do_join(insert_space, redraw) == FAIL)
	{
	    beep_flush();
	    break;
	}
    }
    redraw_later(VALID_TO_CURSCHAR);

    /*
     * Need to update the screen if the line where the cursor is became too
     * long to fit on the screen.
     */
    update_topline_redraw();
}

/*
 * Join two lines at the cursor position.
 * "redraw" is TRUE when the screen should be updated.
 *
 * return FAIL for failure, OK ohterwise
 */
    int
do_join(insert_space, redraw)
    int		insert_space;
    int		redraw;
{
    char_u	*curr;
    char_u	*next;
    char_u	*newp;
    int		endcurr1, endcurr2;
    int		currsize;	/* size of the current line */
    int		nextsize;	/* size of the next line */
    int		spaces;		/* number of spaces to insert */
    int		rows_to_del = 0;/* number of rows on screen to delete */
    linenr_t	t;

    if (curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count)
	return FAIL;		/* can't join on last line */

    if (redraw)
    {
	/*
	 * Check if we can really redraw:  w_cline_row and w_cline_height need
	 * to be valid.  Try to make them valid by calling may_validate_crow()
	 */
	if (may_validate_crow() == OK)
	    rows_to_del = plines_m(curwin->w_cursor.lnum,
						   curwin->w_cursor.lnum + 1);
	else
	    redraw = FALSE;
    }

    curr = ml_get_curline();
    currsize = STRLEN(curr);
    endcurr1 = endcurr2 = NUL;
    if (currsize > 0)
    {
	endcurr1 = *(curr + currsize - 1);
	if (currsize > 1)
	    endcurr2 = *(curr + currsize - 2);
    }

    next = ml_get((linenr_t)(curwin->w_cursor.lnum + 1));
    spaces = 0;
    if (insert_space)
    {
	next = skipwhite(next);
	if (*next != ')' && currsize != 0 && endcurr1 != TAB)
	{
	    /* don't add a space if the line is inding in a space */
	    if (endcurr1 == ' ')
		endcurr1 = endcurr2;
	    else
		++spaces;
	    /* extra space when 'joinspaces' set and line ends in '.' */
	    if (       p_js
		    && (endcurr1 == '.'
			|| (vim_strchr(p_cpo, CPO_JOINSP) == NULL
			    && (endcurr1 == '?' || endcurr1 == '!'))))
		++spaces;
	}
    }
    nextsize = STRLEN(next);

    newp = alloc_check((unsigned)(currsize + nextsize + spaces + 1));
    if (newp == NULL)
	return FAIL;

    /*
     * Insert the next line first, because we already have that pointer.
     * Curr has to be obtained again, because getting next will have
     * invalidated it.
     */
    vim_memmove(newp + currsize + spaces, next, (size_t)(nextsize + 1));

    curr = ml_get_curline();
    vim_memmove(newp, curr, (size_t)currsize);

    copy_spaces(newp + currsize, (size_t)spaces);

    ml_replace(curwin->w_cursor.lnum, newp, FALSE);

    /*
     * Delete the following line. To do this we move the cursor there
     * briefly, and then move it back. After del_lines() the cursor may
     * have moved up (last line deleted), so the current lnum is kept in t.
     */
    t = curwin->w_cursor.lnum;
    ++curwin->w_cursor.lnum;
    del_lines(1L, FALSE, FALSE);
    curwin->w_cursor.lnum = t;

    /*
     * the number of rows on the screen is reduced by the difference
     * in number of rows of the two old lines and the one new line
     */
    if (redraw)
    {
	rows_to_del -= plines(curwin->w_cursor.lnum);
	if (rows_to_del > 0)
	    win_del_lines(curwin, curwin->w_cline_row + curwin->w_cline_height,
						     rows_to_del, TRUE, TRUE);
    }

    /*
     * go to first character of the joined line
     */
    if (currsize == 0)
	curwin->w_cursor.col = 0;
    else
    {
	curwin->w_cursor.col = currsize - 1;
	(void)oneright();
    }
    CHANGED;

    return OK;
}

/*
 * Return TRUE if the two comment leaders given are the same.  The cursor is
 * in the first line.  White-space is ignored.	Note that the whole of
 * 'leader1' must match 'leader2_len' characters from 'leader2' -- webb
 */
    static int
same_leader(leader1_len, leader1_flags, leader2_len, leader2_flags)
    int	    leader1_len;
    char_u  *leader1_flags;
    int	    leader2_len;
    char_u  *leader2_flags;
{
    int	    idx1 = 0, idx2 = 0;
    char_u  *p;
    char_u  *line1;
    char_u  *line2;

    if (leader1_len == 0)
	return (leader2_len == 0);

    /*
     * If first leader has 'f' flag, the lines can be joined only if the
     * second line does not have a leader.
     * If first leader has 'e' flag, the lines can never be joined.
     * If fist leader has 's' flag, the lines can only be joined if there is
     * some text after it and the second line has the 'm' flag.
     */
    if (leader1_flags != NULL)
    {
	for (p = leader1_flags; *p && *p != ':'; ++p)
	{
	    if (*p == COM_FIRST)
		return (leader2_len == 0);
	    if (*p == COM_END)
		return FALSE;
	    if (*p == COM_START)
	    {
		if (*(ml_get_curline() + leader1_len) == NUL)
		    return FALSE;
		if (leader2_flags == NULL || leader2_len == 0)
		    return FALSE;
		for (p = leader2_flags; *p && *p != ':'; ++p)
		    if (*p == COM_MIDDLE)
			return TRUE;
		return FALSE;
	    }
	}
    }

    /*
     * Get current line and next line, compare the leaders.
     * The first line has to be saved, only one line can be locked at a time.
     */
    line1 = vim_strsave(ml_get_curline());
    if (line1 != NULL)
    {
	for (idx1 = 0; vim_iswhite(line1[idx1]); ++idx1)
	    ;
	line2 = ml_get(curwin->w_cursor.lnum + 1);
	for (idx2 = 0; idx2 < leader2_len; ++idx2)
	{
	    if (!vim_iswhite(line2[idx2]))
	    {
		if (line1[idx1++] != line2[idx2])
		    break;
	    }
	    else
		while (vim_iswhite(line1[idx1]))
		    ++idx1;
	}
	vim_free(line1);
    }
    return (idx2 == leader2_len && idx1 == leader1_len);
}

/*
 * implementation of the format operator 'gq'
 */
    void
op_format(oap)
    OPARG	*oap;
{
    long	old_line_count = curbuf->b_ml.ml_line_count;
    int		prev_is_blank = FALSE;
    int		is_end_block = TRUE;
    int		next_is_end_block;
    int		leader_len = 0;	    /* init for gcc */
    int		next_leader_len;
    char_u	*leader_flags = NULL;
    char_u	*next_leader_flags;
    int		advance = TRUE;
    int		second_indent = -1;
    int		do_second_indent;
    int		first_par_line = TRUE;
    int		smd_save;
    long	count;

    if (u_save((linenr_t)(oap->start.lnum - 1),
				       (linenr_t)(oap->end.lnum + 1)) == FAIL)
	return;

    /* Set '[ mark at the start of the formatted area */
    curbuf->b_op_start = oap->start;

    /* check for 'q' and '2' in 'formatoptions' */
    fo_do_comments = has_format_option(FO_Q_COMS);
    do_second_indent = has_format_option(FO_Q_SECOND);

    /*
     * get info about the previous and current line.
     */
    if (curwin->w_cursor.lnum > 1)
	is_end_block = fmt_end_block(curwin->w_cursor.lnum - 1,
					&next_leader_len, &next_leader_flags);
    next_is_end_block = fmt_end_block(curwin->w_cursor.lnum,
					&next_leader_len, &next_leader_flags);

    curwin->w_cursor.lnum--;
    for (count = oap->line_count; count > 0; --count)
    {
	/*
	 * Advance to next block.
	 */
	if (advance)
	{
	    curwin->w_cursor.lnum++;
	    prev_is_blank = is_end_block;
	    is_end_block = next_is_end_block;
	    leader_len = next_leader_len;
	    leader_flags = next_leader_flags;
	}

	/*
	 * The last line to be formatted.
	 */
	if (count == 1)
	{
	    next_is_end_block = TRUE;
	    next_leader_len = 0;
	    next_leader_flags = NULL;
	}
	else
	    next_is_end_block = fmt_end_block(curwin->w_cursor.lnum + 1,
					&next_leader_len, &next_leader_flags);
	advance = TRUE;

	/*
	 * For the first line of a paragraph, check indent of second line.
	 * Don't do this for comments and empty lines.
	 */
	if (first_par_line && do_second_indent &&
		prev_is_blank && !is_end_block &&
		curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count &&
		leader_len == 0 && next_leader_len == 0 &&
		!lineempty(curwin->w_cursor.lnum + 1))
	    second_indent = get_indent_lnum(curwin->w_cursor.lnum + 1);

	/*
	 * Skip end-of-block (blank) lines
	 */
	if (is_end_block)
	{
	}
	/*
	 * If we have got to the end of a paragraph, format it.
	 */
	else if (next_is_end_block || !same_leader(leader_len, leader_flags,
					  next_leader_len, next_leader_flags))
	{
	    /* replace indent in first line with minimal number of tabs and
	     * spaces, according to current options */
	    set_indent(get_indent(), TRUE);

	    /* put cursor on last non-space */
	    coladvance(MAXCOL);
	    while (curwin->w_cursor.col && vim_isspace(gchar_cursor()))
		dec_cursor();

	    /* do the formatting, without 'showmode' */
	    State = INSERT;	/* for open_line() */
	    smd_save = p_smd;
	    p_smd = FALSE;
	    insertchar(NUL, TRUE, second_indent, FALSE);
	    State = NORMAL;
	    p_smd = smd_save;
	    first_par_line = TRUE;
	    second_indent = -1;
	}
	else
	{
	    /*
	     * Still in same paragraph, so join the lines together.
	     * But first delete the comment leader from the second line.
	     */
	    advance = FALSE;
	    curwin->w_cursor.lnum++;
	    curwin->w_cursor.col = 0;
	    (void)del_chars((long)next_leader_len, FALSE);
	    curwin->w_cursor.lnum--;
	    if (do_join(TRUE, FALSE) == FAIL)
	    {
		beep_flush();
		break;
	    }
	    first_par_line = FALSE;
	}
    }
    fo_do_comments = FALSE;
    /*
     * Leave the cursor at the first non-blank of the last formatted line.
     * If the cursor was move one line back (e.g. with "Q}") go to the next
     * line, so "." will do the next lines.
     */
    if (oap->end_adjusted && curwin->w_cursor.lnum < curbuf->b_ml.ml_line_count)
	++curwin->w_cursor.lnum;
    beginline(BL_WHITE | BL_FIX);
    update_screen(NOT_VALID);
    msgmore(curbuf->b_ml.ml_line_count - old_line_count);

    /* put '] mark on the end of the formatted area */
    curbuf->b_op_end = curwin->w_cursor;
}

/*
 * Blank lines, and lines containing only the comment leader, are left
 * untouched by the formatting.  The function returns TRUE in this
 * case.  It also returns TRUE when a line starts with the end of a comment
 * ('e' in comment flags), so that this line is skipped, and not joined to the
 * previous line.  A new paragraph starts after a blank line, or when the
 * comment leader changes -- webb.
 */
    static int
fmt_end_block(lnum, leader_len, leader_flags)
    linenr_t	lnum;
    int		*leader_len;
    char_u	**leader_flags;
{
    char_u	*flags = NULL;	    /* init for GCC */
    char_u	*ptr;

    ptr = ml_get(lnum);
    *leader_len = get_leader_len(ptr, leader_flags);

    if (*leader_len > 0)
    {
	/*
	 * Search for 'e' flag in comment leader flags.
	 */
	flags = *leader_flags;
	while (*flags && *flags != ':' && *flags != COM_END)
	    ++flags;
    }

    return (ptr[*leader_len] == NUL ||
	    (*leader_len > 0 && *flags == COM_END) ||
	     startPS(lnum, NUL, FALSE));
}

/*
 * prepare a few things for block mode yank/delete/tilde
 *
 * for delete:
 * - textlen includes the first/last char to be (partly) deleted
 * - start/endspaces is the number of columns that are taken by the
 *   first/last deleted char minus the number of columns that have to be
 *   deleted.  for yank and tilde:
 * - textlen includes the first/last char to be wholly yanked
 * - start/endspaces is the number of columns of the first/last yanked char
 *   that are to be yanked.
 */
    static void
block_prep(oap, bd, lnum, is_del)
    OPARG		*oap;
    struct block_def	*bd;
    linenr_t		lnum;
    int			is_del;
{
    colnr_t	vcol;
    int		incr = 0;
    char_u	*pend;
    char_u	*pstart;

    bd->startspaces = 0;
    bd->endspaces = 0;
    bd->textlen = 0;
    bd->textcol = 0;
    vcol = 0;
    pstart = ml_get(lnum);
    while (vcol < oap->start_vcol && *pstart)
    {
	/* Count a tab for what it's worth (if list mode not on) */
	incr = lbr_chartabsize(pstart, (colnr_t)vcol);
	vcol += incr;
	++pstart;
	++bd->textcol;
    }
    if (vcol < oap->start_vcol)	/* line too short */
    {
	if (!is_del)
	    bd->endspaces = oap->end_vcol - oap->start_vcol + 1;
    }
    else /* vcol >= oap->start_vcol */
    {
	bd->startspaces = vcol - oap->start_vcol;
	if (is_del && vcol > oap->start_vcol)
	    bd->startspaces = incr - bd->startspaces;
	pend = pstart;
	if (vcol > oap->end_vcol)	/* it's all in one character */
	{
	    bd->startspaces = oap->end_vcol - oap->start_vcol + 1;
	    if (is_del)
		bd->startspaces = incr - bd->startspaces;
	}
	else
	{
	    while (vcol <= oap->end_vcol && *pend)
	    {
		/* Count a tab for what it's worth (if list mode not on) */
		incr = lbr_chartabsize(pend, (colnr_t)vcol);
		vcol += incr;
		++pend;
	    }
	    if (vcol < oap->end_vcol && !is_del)    /* line too short */
	    {
		bd->endspaces = oap->end_vcol - vcol;
	    }
	    else if (vcol > oap->end_vcol)
	    {
		bd->endspaces = vcol - oap->end_vcol - 1;
		if (!is_del && pend != pstart && bd->endspaces)
		    --pend;
	    }
	}
	if (is_del && bd->startspaces)
	{
	    --pstart;
	    --bd->textcol;
	}
	bd->textlen = (int)(pend - pstart);
    }
    bd->textstart = pstart;
}

#ifdef RIGHTLEFT
static void reverse_line __ARGS((char_u *s));

    static void
reverse_line(s)
    char_u *s;
{
    int	    i, j;
    char_u  c;

    if ((i = STRLEN(s) - 1) <= 0)
	return;

    curwin->w_cursor.col = i - curwin->w_cursor.col;
    for (j = 0; j < i; j++, i--)
    {
	c = s[i]; s[i] = s[j]; s[j] = c;
    }
}

# define RLADDSUBFIX() if (curwin->w_p_rl) reverse_line(ptr);
#else
# define RLADDSUBFIX()
#endif

/*
 * add or subtract 'Prenum1' from a number in a line
 * 'command' is CTRL-A for add, CTRL-X for subtract
 *
 * return FAIL for failure, OK otherwise
 */
    int
do_addsub(command, Prenum1)
    int		command;
    linenr_t	Prenum1;
{
    int		    col;
    char_u	    buf1[NUMBUFLEN];
    char_u	    buf2[NUMBUFLEN];
    int		    hex;		/* 'X' or 'x': hex; '0': octal */
    static int	    hexupper = FALSE;	/* 0xABC */
    long	    n;
    char_u	    *ptr;
    int		    c;
    int		    length = 0;		/* character length of the number */
    int		    todel;
    int		    dohex;
    int		    dooct;
    int		    firstdigit;

    dohex = (vim_strchr(curbuf->b_p_nf, 'x') != NULL);
    dooct = (vim_strchr(curbuf->b_p_nf, 'o') != NULL);

    ptr = ml_get_curline();
    RLADDSUBFIX();

    /*
     * First check if we are on a hexadecimal number, after the "0x".
     */
    col = curwin->w_cursor.col;
    if (dohex)
	while (col > 0 && isxdigit(ptr[col]))
	    --col;
    if (       dohex
	    && col > 0
	    && (ptr[col] == 'X'
		|| ptr[col] == 'x')
	    && ptr[col - 1] == '0'
	    && isxdigit(ptr[col + 1]))
    {
	/*
	 * Found hexadecimal number, move to its start.
	 */
	--col;
    }
    else
    {
	/*
	 * Search forward and then backward to find the start of number.
	 */
	col = curwin->w_cursor.col;

	while (ptr[col] != NUL && !isdigit(ptr[col]))
	    ++col;

	while (col > 0 && isdigit(ptr[col - 1]))
	    --col;
    }

    /*
     * If a number was found, and saving for undo works, replace the number.
     */
    firstdigit = ptr[col];
    RLADDSUBFIX();
    if (!isdigit(firstdigit) || u_save_cursor() != OK)
    {
	beep_flush();
	return FAIL;
    }

    /* get ptr again, because u_save() may have changed it */
    ptr = ml_get_curline();
    RLADDSUBFIX();

    if (col > 0 && ptr[col - 1] == '-')	    /* negative number */
	--col;
					    /* get the number value */
    n = vim_str2nr(ptr + col, &hex, &length, dooct, dohex);

    if (command == Ctrl('A'))		    /* add or subtract */
	n += Prenum1;
    else
	n -= Prenum1;

    /*
     * Delete the old number.
     */
    curwin->w_cursor.col = col;
    todel = length;
    c = gchar_cursor();
    /*
     * Don't include the '-' in the length, only the length of the part
     * after it is kept the same.
     */
    if (c == '-')
	--length;
    while (todel-- > 0)
    {
	if (isalpha(c))
	{
	    if (isupper(c))
		hexupper = TRUE;
	    else
		hexupper = FALSE;
	}
	(void)del_char(FALSE);
	c = gchar_cursor();
    }

    /*
     * Prepare the leading characters in buf1[].
     */
    ptr = buf1;
    if (n < 0)
    {
	*ptr++ = '-';
	n = -n;
    }
    if (hex)
    {
	*ptr++ = '0';
	--length;
    }
    if (hex == 'x' || hex == 'X')
    {
	*ptr++ = hex;
	--length;
    }

    /*
     * Put the number characters in buf2[].
     */
    if (hex == 0)
	sprintf((char *)buf2, "%ld", n);
    else if (hex == '0')
	sprintf((char *)buf2, "%lo", n);
    else if (hex && hexupper)
	sprintf((char *)buf2, "%lX", n);
    else
	sprintf((char *)buf2, "%lx", n);
    length -= STRLEN(buf2);

    /*
     * adjust number of zeros to the new number of digits, so the
     * total length of the number remains the same
     */
    if (firstdigit == '0')
	while (length-- > 0)
	    *ptr++ = '0';
    *ptr = NUL;
    STRCAT(buf1, buf2);
    ins_str(buf1);		    /* insert the new number */
    --curwin->w_cursor.col;
    curwin->w_set_curswant = TRUE;
#ifdef RIGHTLEFT
    ptr = ml_get_buf(curbuf, curwin->w_cursor.lnum, TRUE);
    RLADDSUBFIX();
#endif
    update_screenline();
    return OK;
}

#ifdef VIMINFO
    int
read_viminfo_register(line, fp, force)
    char_u  *line;
    FILE    *fp;
    int	    force;
{
    int	    eof;
    int	    do_it = TRUE;
    int	    size;
    int	    limit;
    int	    i;
    int	    set_prev = FALSE;
    char_u  *str;
    char_u  **array = NULL;

    /* We only get here (hopefully) if line[0] == '"' */
    str = line + 1;
    if (*str == '"')
    {
	set_prev = TRUE;
	str++;
    }
    if (!isalnum(*str) && *str != '-')
    {
	if (viminfo_error("Illegal register name", line))
	    return TRUE;	/* too many errors, pretend end-of-file */
	do_it = FALSE;
    }
    get_yank_register(*str++, FALSE);
    if (!force && y_current->y_array != NULL)
	do_it = FALSE;
    size = 0;
    limit = 100;	/* Optimized for registers containing <= 100 lines */
    if (do_it)
    {
	if (set_prev)
	    y_previous = y_current;
	vim_free(y_current->y_array);
	array = y_current->y_array =
		       (char_u **)alloc((unsigned)(limit * sizeof(char_u *)));
	str = skipwhite(str);
	if (STRNCMP(str, "CHAR", 4) == 0)
	    y_current->y_type = MCHAR;
	else if (STRNCMP(str, "BLOCK", 5) == 0)
	    y_current->y_type = MBLOCK;
	else
	    y_current->y_type = MLINE;
    }
    while (!(eof = vim_fgets(line, LSIZE, fp)) && line[0] == TAB)
    {
	if (do_it)
	{
	    if (size >= limit)
	    {
		y_current->y_array = (char_u **)
			      alloc((unsigned)(limit * 2 * sizeof(char_u *)));
		for (i = 0; i < limit; i++)
		    y_current->y_array[i] = array[i];
		vim_free(array);
		limit *= 2;
		array = y_current->y_array;
	    }
	    viminfo_readstring(line);
	    str = vim_strsave(line + 1);
	    if (str != NULL)
		array[size++] = str;
	    else
		do_it = FALSE;
	}
    }
    if (do_it)
    {
	if (size == 0)
	{
	    vim_free(array);
	    y_current->y_array = NULL;
	}
	else if (size < limit)
	{
	    y_current->y_array =
			(char_u **)alloc((unsigned)(size * sizeof(char_u *)));
	    for (i = 0; i < size; i++)
		y_current->y_array[i] = array[i];
	    vim_free(array);
	}
	y_current->y_size = size;
    }
    return eof;
}

    void
write_viminfo_registers(fp)
    FILE    *fp;
{
    int	    i, j;
    char_u  *type;
    char_u  c;
    int	    num_lines;
    int	    max_num_lines;

    fprintf(fp, "\n# Registers:\n");

    max_num_lines = get_viminfo_parameter('"');
    if (max_num_lines == 0)
	return;
    for (i = 0; i < NUM_REGISTERS; i++)
    {
	if (y_regs[i].y_array == NULL)
	    continue;
#ifdef USE_CLIPBOARD
	/* Skip '*' register, we don't want it back next time */
	if (i == CLIPBOARD_REGISTER)
	    continue;
#endif
	switch (y_regs[i].y_type)
	{
	    case MLINE:
		type = (char_u *)"LINE";
		break;
	    case MCHAR:
		type = (char_u *)"CHAR";
		break;
	    case MBLOCK:
		type = (char_u *)"BLOCK";
		break;
	    default:
		sprintf((char *)IObuff, "Unknown register type %d",
		    y_regs[i].y_type);
		emsg(IObuff);
		type = (char_u *)"LINE";
		break;
	}
	if (y_previous == &y_regs[i])
	    fprintf(fp, "\"");
	if (i == DELETION_REGISTER)
	    c = '-';
	else if (i < 10)
	    c = '0' + i;
	else
	    c = 'a' + i - 10;
	fprintf(fp, "\"%c\t%s\n", c, type);
	num_lines = y_regs[i].y_size;

	/* If max_num_lines < 0, then we save ALL the lines in the register */
	if (max_num_lines > 0 && num_lines > max_num_lines)
	    num_lines = max_num_lines;
	for (j = 0; j < num_lines; j++)
	{
	    putc('\t', fp);
	    viminfo_writestring(fp, y_regs[i].y_array[j]);
	}
    }
}
#endif /* VIMINFO */

#if defined(USE_CLIPBOARD) || defined(PROTO)
/*
 * Text selection stuff that uses the GUI selection register '*'.  When using a
 * GUI this may be text from another window, otherwise it is the last text we
 * had highlighted with VIsual mode.  With mouse support, clicking the middle
 * button performs the paste, otherwise you will need to do <"*p>.
 */

    void
clip_free_selection()
{
    struct yankreg *y_ptr = y_current;

    y_current = &y_regs[CLIPBOARD_REGISTER];	    /* '*' register */
    free_yank_all();
    y_current->y_size = 0;
    y_current = y_ptr;
}

/*
 * Get the selected text and put it in the gui text register '*'.
 */
    void
clip_get_selection()
{
    struct yankreg *old_y_previous, *old_y_current;
    FPOS    old_cursor, old_visual;
    colnr_t old_curswant;
    int	    old_set_curswant;
    OPARG   oa;
    CMDARG  ca;

    if (clipboard.owned)
    {
	if (y_regs[CLIPBOARD_REGISTER].y_array != NULL)
	    return;

	/* Get the text between clipboard.start & clipboard.end */
	old_y_previous = y_previous;
	old_y_current = y_current;
	old_cursor = curwin->w_cursor;
	old_curswant = curwin->w_curswant;
	old_set_curswant = curwin->w_set_curswant;
	old_visual = VIsual;
	clear_oparg(&oa);
	oa.regname = '*';
	oa.op_type = OP_YANK;
	vim_memset(&ca, 0, sizeof(ca));
	ca.oap = &oa;
	ca.cmdchar = 'y';
	ca.count1 = 1;
	do_pending_operator(&ca, NULL, NULL, 0, TRUE, TRUE);
	y_previous = old_y_previous;
	y_current = old_y_current;
	curwin->w_cursor = old_cursor;
	curwin->w_curswant = old_curswant;
	curwin->w_set_curswant = old_set_curswant;
	VIsual = old_visual;
    }
    else
    {
	clip_free_selection();

	/* Try to get selected text from another window */
	clip_mch_request_selection();
    }
}

/* Convert from the GUI selection string into the '*' register */
    void
clip_yank_selection(type, str, len)
    int	    type;
    char_u  *str;
    long    len;
{
    struct yankreg *y_ptr = &y_regs[CLIPBOARD_REGISTER];    /* '*' register */

    clip_free_selection();

    str_to_reg(y_ptr, type, str, len);
}

/*
 * Convert the '*' register into a GUI selection string returned in *str with
 * length *len.
 */
    int
clip_convert_selection(str, len)
    char_u  **str;
    long_u  *len;
{
    struct yankreg *y_ptr = &y_regs[CLIPBOARD_REGISTER];    /* '*' register */
    char_u  *p;
    int	    lnum;
    int	    i, j;
    int_u   eolsize;

#ifdef USE_CRNL
    eolsize = 2;
#else
    eolsize = 1;
#endif

    *str = NULL;
    *len = 0;
    if (y_ptr->y_array == NULL)
	return -1;

    for (i = 0; i < y_ptr->y_size; i++)
	*len += STRLEN(y_ptr->y_array[i]) + eolsize;

    /*
     * Don't want newline character at end of last line if we're in MCHAR mode.
     */
    if (y_ptr->y_type == MCHAR && *len > eolsize)
	*len -= eolsize;

    p = *str = lalloc(*len, TRUE);
    if (p == NULL)
	return -1;
    lnum = 0;
    for (i = 0, j = 0; i < (int)*len; i++, j++)
    {
	if (y_ptr->y_array[lnum][j] == '\n')
	    p[i] = NUL;
	else if (y_ptr->y_array[lnum][j] == NUL)
	{
#ifdef USE_CRNL
	    p[i++] = '\r';
#endif
#ifdef USE_CR
	    p[i] = '\r';
#else
	    p[i] = '\n';
#endif
	    lnum++;
	    j = -1;
	}
	else
	    p[i] = y_ptr->y_array[lnum][j];
    }
    return y_ptr->y_type;
}
#endif /* USE_CLIPBOARD || PROTO */

#ifdef WANT_EVAL
/*
 * Return the contents of a register as a single allocated string.
 * Used for "@r" in expressions.
 * Returns NULL for error.
 */
    char_u *
get_reg_contents(regname)
    int	    regname;
{
    long    i;
    char_u  *retval;
    int	    allocated;
    long    len;

    /* Don't allow using an expression register inside an expression */
    if (regname == '=')
	return NULL;

    if (regname == '@')	    /* "@@" is used for unnamed register */
	regname = '"';

    /* check for valid regname */
    if (regname != NUL && !valid_yank_reg(regname, FALSE))
	return NULL;

#ifdef USE_CLIPBOARD
    if (regname == '*')
	clip_get_selection();		/* may fill * register */
#endif

    if (get_spec_reg(regname, &retval, &allocated))
    {
	if (retval == NULL)
	    return NULL;
	if (!allocated)
	    retval = vim_strsave(retval);
	return retval;
    }

    get_yank_register(regname, FALSE);
    if (y_current->y_array == NULL)
	return NULL;

    /*
     * Compute length of resulting string.
     */
    len = 0;
    for (i = 0; i < y_current->y_size; ++i)
    {
	len += STRLEN(y_current->y_array[i]);
	/*
	 * Insert a newline between lines and after last line if
	 * y_type is MLINE.
	 */
	if (y_current->y_type == MLINE || i < y_current->y_size - 1)
	    ++len;
    }

    retval = lalloc(len + 1, TRUE);

    /*
     * Copy the lines of the yank register into the string.
     */
    if (retval != NULL)
    {
	len = 0;
	for (i = 0; i < y_current->y_size; ++i)
	{
	    STRCPY(retval + len, y_current->y_array[i]);
	    len += STRLEN(retval + len);

	    /*
	     * Insert a NL between lines and after the last line if y_type is
	     * MLINE.
	     */
	    if (y_current->y_type == MLINE || i < y_current->y_size - 1)
		retval[len++] = '\n';
	}
	retval[len] = NUL;
    }

    return retval;
}

/*
 * Store string 'str' in register 'name'.
 * Careful: 'str' is modified, you may have to use a copy!
 * If 'str' ends in '\n' or '\r', use linewise, otherwise use characterwise.
 */
    void
write_reg_contents(name, str)
    int	    name;
    char_u  *str;
{
    struct yankreg  *old_y_previous, *old_y_current;
    long	    len;

    if (!valid_yank_reg(name, TRUE))	    /* check for valid reg name */
    {
	EMSG2("Invalid register name: '%s'", transchar(name));
	return;
    }

    /* Don't want to change the current (unnamed) register */
    old_y_previous = y_previous;
    old_y_current = y_current;

    get_yank_register(name, TRUE);
    if (!y_append)
	free_yank_all();
    len = STRLEN(str);
    str_to_reg(y_current,
	    (len > 0 && (str[len - 1] == '\n' || str[len -1] == '\r'))
	     ? MLINE : MCHAR, str, len);
    y_previous = old_y_previous;
    y_current = old_y_current;
}
#endif	/* WANT_EVAL */

#if defined(USE_CLIPBOARD) || defined(WANT_EVAL)
/*
 * Put a string into a register.  When the register is not empty, the string
 * is appended.
 */
    static void
str_to_reg(y_ptr, type, str, len)
    struct yankreg	*y_ptr;		/* pointer to yank register */
    int			type;		/* MCHAR or MLINE */
    char_u		*str;		/* string to put in register */
    long		len;		/* lenght of string */
{
    int	    lnum;
    long    start;
    long    i;
    int	    extra;
    int	    newlines;			/* number of lines added */
    int	    extraline = 0;		/* extra line at the end */
    int	    append = FALSE;		/* append to last line in register */
    char_u  *s;
    char_u  **pp;

    if (y_ptr->y_array == NULL)		/* NULL means emtpy register */
	y_ptr->y_size = 0;

    /*
     * Count the number of lines within the string
     */
    newlines = 0;
    for (i = 0; i < len; i++)
	if (str[i] == '\n')
	    ++newlines;
    if (type == MCHAR || len == 0 || str[len - 1] != '\n')
    {
	extraline = 1;
	++newlines;	/* count extra newline at the end */
    }
    if (y_ptr->y_size > 0 && y_ptr->y_type == MCHAR)
    {
	append = TRUE;
	--newlines;	/* uncount newline when appending first line */
    }

    /*
     * Allocate an array to hold the pointers to the new register lines.
     * If the register was not empty, move the existing lines to the new array.
     */
    pp = (char_u **)lalloc_clear((y_ptr->y_size + newlines)
						    * sizeof(char_u *), TRUE);
    if (pp == NULL)	/* out of memory */
	return;
    for (lnum = 0; lnum < y_ptr->y_size; ++lnum)
	pp[lnum] = y_ptr->y_array[lnum];
    vim_free(y_ptr->y_array);
    y_ptr->y_array = pp;

    /*
     * Find the end of each line and save it into the array.
     */
    for (start = 0; start < len + extraline; start += i + 1)
    {
	for (i = start; i < len; ++i)	/* find the end of the line */
	    if (str[i] == '\n')
		break;
	i -= start;			/* i is now length of line */
	if (append)
	{
	    --lnum;
	    extra = STRLEN(y_ptr->y_array[lnum]);
	}
	else
	    extra = 0;
	s = alloc((unsigned)(i + extra + 1));
	if (s == NULL)
	    break;
	if (extra)
	{
	    vim_memmove(s, y_ptr->y_array[lnum], (size_t)extra);
	    vim_free(y_ptr->y_array[lnum]);
	}
	if (i)
	    vim_memmove(s + extra, str + start, (size_t)i);
	extra += i;
	s[extra] = NUL;
	y_ptr->y_array[lnum++] = s;
	while (--extra >= 0)
	{
	    if (*s == NUL)
		*s = '\n';	    /* replace NUL with newline */
	    ++s;
	}
	append = FALSE;		    /* only first line is appended */
    }
    y_ptr->y_type = type;
    y_ptr->y_size = lnum;
}
#endif /* USE_CLIPBOARD || WANT_EVAL || PROTO */

    void
clear_oparg(oap)
    OPARG	*oap;
{
    vim_memset(oap, 0, sizeof(OPARG));
}
