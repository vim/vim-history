/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * eval.c: Expression evaluation.
 */

#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>	/* contains setenv() declaration for Amiga */
#endif
#ifdef AMIGA
# include <time.h>	/* for strftime() */
#endif

#ifdef WANT_EVAL

typedef struct
{
    char_u	*var_name;	/* name of variable */
    char	var_type;	/* VAR_NUMBER or VAR_STRING */
    union
    {
#if SIZEOF_INT <= 3		/* use long if int is smaller than 32 bits */
	long	var_number;	/* number value */
#else
	int	var_number;	/* number value */
#endif
	char_u	*var_string;	/* string value (Careful: can be NULL!) */
    }		var_val;
} var;

#define VAR_UNKNOWN 0
#define VAR_NUMBER  1
#define VAR_STRING  2

typedef var *	VAR;

/*
 * All user-defined internal variables are stored in variables.
 */
struct growarray    variables;
#define VAR_ENTRY(idx)	(((VAR)(variables.ga_data))[idx])
#define VAR_GAP_ENTRY(idx, gap)	(((VAR)(gap->ga_data))[idx])
#define BVAR_ENTRY(idx)	(((VAR)(curbuf->b_vars.ga_data))[idx])
#define WVAR_ENTRY(idx)	(((VAR)(curwin->w_vars.ga_data))[idx])

static int eval0 __ARGS((char_u *arg,  VAR retvar, char_u **nextcmd));
static int eval1 __ARGS((char_u **arg, VAR retvar));
static int eval2 __ARGS((char_u **arg, VAR retvar));
static int eval3 __ARGS((char_u **arg, VAR retvar));
static int eval4 __ARGS((char_u **arg, VAR retvar));
static int eval5 __ARGS((char_u **arg, VAR retvar));
static int eval6 __ARGS((char_u **arg, VAR retvar));
static int get_option_var __ARGS((char_u **arg, VAR retvar));
static int get_string_var __ARGS((char_u **arg, VAR retvar));
static int get_lit_string_var __ARGS((char_u **arg, VAR retvar));
static int get_env_var __ARGS((char_u **arg, VAR retvar));
static int get_func_var __ARGS((char_u *name, int len, VAR retvar, char_u **arg));
static void f_buffer_exists __ARGS((VAR argvars, VAR retvar));
static void f_char2nr __ARGS((VAR argvars, VAR retvar));
static void f_col __ARGS((VAR argvars, VAR retvar));
static void f_delete __ARGS((VAR argvars, VAR retvar));
static void f_exists __ARGS((VAR argvars, VAR retvar));
static void f_expand __ARGS((VAR argvars, VAR retvar));
static void f_file_readable __ARGS((VAR argvars, VAR retvar));
static void f_getline __ARGS((VAR argvars, VAR retvar));
static void f_has __ARGS((VAR argvars, VAR retvar));
static void f_highlight_exists __ARGS((VAR argvars, VAR retvar));
static void f_highlightID __ARGS((VAR argvars, VAR retvar));
static void f_hostname __ARGS((VAR argvars, VAR retvar));
static void f_isdirectory __ARGS((VAR argvars, VAR retvar));
static void f_last_buffer_nr __ARGS((VAR argvars, VAR retvar));
static void f_line __ARGS((VAR argvars, VAR retvar));
static void f_match __ARGS((VAR argvars, VAR retvar));
static void f_matchend __ARGS((VAR argvars, VAR retvar));
static void f_nr2char __ARGS((VAR argvars, VAR retvar));
static void f_some_match __ARGS((VAR argvars, VAR retvar, int start));
static void f_strftime __ARGS((VAR argvars, VAR retvar));
static void f_strlen __ARGS((VAR argvars, VAR retvar));
static void f_strpart __ARGS((VAR argvars, VAR retvar));
static void f_synID __ARGS((VAR argvars, VAR retvar));
static void f_synIDattr __ARGS((VAR argvars, VAR retvar));
static void f_synIDtrans __ARGS((VAR argvars, VAR retvar));
static void f_substitute __ARGS((VAR argvars, VAR retvar));
static void f_tempname __ARGS((VAR argvars, VAR retvar));
static void f_virtcol __ARGS((VAR argvars, VAR retvar));
static FPOS *var2fpos __ARGS((VAR varp));
static int get_env_len __ARGS((char_u **arg));
char_u *get_env_string __ARGS((char_u **arg));
static int get_id_len __ARGS((char_u **arg));
static int eval_isnamec __ARGS((int c));
static int get_var_var __ARGS((char_u *name, int len, VAR retvar));
static VAR alloc_var __ARGS((void));
static VAR alloc_string_var __ARGS((char_u *string));
static void free_var __ARGS((VAR varp));
static void clear_var __ARGS((VAR varp));
static long get_var_number __ARGS((VAR varp));
static char_u *get_var_string __ARGS((VAR varp));
static char_u *get_var_string_buf __ARGS((VAR varp, char_u *buf));
static VAR find_var __ARGS((char_u *name));
static struct growarray *find_var_ga __ARGS((char_u *name, char_u **varname));
static void var_free_one __ARGS((VAR v));
static void list_one_var __ARGS((VAR v, char_u *prefix));
static void set_var __ARGS((char_u *name, VAR varp));
static char_u *find_option_end __ARGS((char_u *p));

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    void
set_internal_string_var(name, value)
    char_u	*name;
    char_u	*value;
{
    char_u  *val;
    VAR	    varp;

    val = vim_strsave(value);
    if (val != NULL)
    {
	varp = alloc_string_var(val);
	if (varp != NULL)
	{
	    set_var(name, varp);
	    free_var(varp);
	}
    }
}

/*
 * Top level evaluation function, returning a boolean.
 * Sets "error" to TRUE if there was an error.
 * Return TRUE or FALSE.
 */
    int
eval_to_bool(arg, error, nextcmd)
    char_u	*arg;
    int		*error;
    char_u	**nextcmd;
{
    var		retvar;
    int		retval;

    if (eval0(arg, &retvar, nextcmd) == FAIL)
    {
	*error = TRUE;
	retval = FALSE;
    }
    else
    {
	*error = FALSE;
	retval = (get_var_number(&retvar) != 0);
	clear_var(&retvar);
    }

    return retval;
}

/*
 * Top level evaluation function, returning a string.
 * Return pointer to allocated memory, or NULL for failure.
 */
    char_u *
eval_to_string(arg, nextcmd)
    char_u	*arg;
    char_u	**nextcmd;
{
    var		retvar;
    char_u	*retval;

    if (eval0(arg, &retvar, nextcmd) == FAIL)
	retval = NULL;
    else
    {
	retval = vim_strsave(get_var_string(&retvar));
	clear_var(&retvar);
    }

    return retval;
}

/*
 * ":let var = expr"	assignment command.
 * ":let var"		list one variable value
 * ":let"		list all variable values
 */
    void
do_let(eap)
    EXARG	*eap;
{
    char_u	*arg = eap->arg;
    char_u	*expr;
    char_u	*name;
    VAR		varp;
    var		retvar;
    char_u	*p;
    int		c1, c2;
    int		i;
#ifndef HAVE_SETENV
    char_u	*envbuf;
#endif

    expr = vim_strchr(arg, '=');
    if (expr == NULL)
    {
	if (ends_excmd(*arg))
	{
	    if (!eap->skip)
	    {
		/*
		 * List all variables.
		 */
		for (i = 0; i < variables.ga_len; ++i)
		    if (VAR_ENTRY(i).var_name != NULL)
			list_one_var(&VAR_ENTRY(i), (char_u *)"");
		for (i = 0; i < curbuf->b_vars.ga_len; ++i)
		    if (BVAR_ENTRY(i).var_name != NULL)
			list_one_var(&BVAR_ENTRY(i), (char_u *)"b:");
		for (i = 0; i < curwin->w_vars.ga_len; ++i)
		    if (WVAR_ENTRY(i).var_name != NULL)
			list_one_var(&WVAR_ENTRY(i), (char_u *)"w:");
	    }
	}
	else
	{
	    /*
	     * List variables.
	     */
	    while (!ends_excmd(*arg))
	    {
		for (p = arg; eval_isnamec(*p); ++p)
		    ;
		if (!vim_iswhite(*p) && !ends_excmd(*p))
		{
		    EMSG(e_trailing);
		    break;
		}
		if (!eap->skip)
		{
		    c1 = *p;
		    *p = NUL;
		    varp = find_var(arg);
		    if (varp == NULL)
			EMSG2("Unknown variable: \"%s\"", arg);
		    else
		    {
			name = vim_strchr(arg, ':');
			if (name != NULL)
			{
			    c2 = *++name;
			    *name = NUL;
			    list_one_var(varp, arg);
			    *name = c2;
			}
			else
			    list_one_var(varp, (char_u *)"");
		    }
		    *p = c1;
		}
		arg = skipwhite(p);
	    }
	}
	eap->nextcmd = check_nextcmd(arg);
    }
    else
    {
	if (eap->skip)
	    ++emsg_off;
	i = eval0(expr + 1, &retvar, &eap->nextcmd);
	if (eap->skip)
	    --emsg_off;
	else if (i != FAIL)
	{
	    /*
	     * ":let $VAR = expr": Set environment variable.
	     */
	    if (*arg == '$')
	    {
		int	len;
		int	cc;

		/* Find the end of the name. */
		++arg;
		name = arg;
		len = get_env_len(&arg);
		if (name != NULL)
		{
		    if (*skipwhite(arg) != '=')
			EMSG(e_letunexp);
		    else
		    {
			cc = name[len];
			name[len] = NUL;
			p = get_var_string(&retvar);
#ifdef HAVE_SETENV
# if defined(AMIGA) || defined(VMS)
			vim_setenv((char *)name, (char *)p);
# else
			setenv((char *)name, (char *)p, 1);
# endif
#else
			/*
			 * Putenv does not copy the string, it has to remain
			 * valid.  The allocated memory will never be freed.
			 */
			envbuf = alloc((unsigned)(STRLEN(p) +
							   STRLEN(name) + 2));
			if (envbuf != NULL)
			{
			    sprintf((char *)envbuf, "%s=%s", name, p);
			    putenv((char *)envbuf);
			}
#endif
			if (STRICMP(name, "home") == 0)
			    init_homedir();
			name[len] = cc;
		    }
		}
	    }

	    /*
	     * ":let &option = expr": Set option value.
	     */
	    else if (*arg == '&')
	    {
		/*
		 * Find the end of the name;
		 */
		++arg;
		p = find_option_end(arg);
		if (*skipwhite(p) != '=')
		    EMSG(e_letunexp);
		else
		{
		    c1 = *p;
		    *p = NUL;
		    set_option_value(arg, get_var_number(&retvar),
						     get_var_string(&retvar));
		    *p = c1;		    /* put back for error messages */
		}
	    }

	    /*
	     * ":let @r = expr": Set register contents.
	     */
	    else if (*arg == '@')
	    {
		++arg;
		if (*skipwhite(arg + 1) != '=')
		    EMSG(e_letunexp);
		else
		    write_reg_contents(*arg == '@' ? '"' : *arg,
						     get_var_string(&retvar));
	    }

	    /*
	     * ":let var = expr": Set internal variable.
	     */
	    else if (eval_isnamec(*arg) && !isdigit(*arg))
	    {
		/*
		 * Find the end of the name;
		 */
		for (p = arg; eval_isnamec(*p); ++p)
		    ;
		if (*skipwhite(p) != '=')
		    EMSG(e_letunexp);
		else
		{
		    c1 = *p;
		    *p = NUL;
		    set_var(arg, &retvar);
		    *p = c1;		/* put char back for error messages */
		}
	    }

	    else
	    {
		EMSG2(e_invarg2, arg);
	    }

	}
	clear_var(&retvar);
    }
}

/*
 * ":unlet var" command.
 */
    void
do_unlet(arg)
    char_u	*arg;
{
    char_u	    *name_end;
    VAR		    v;
    int		    cc;

    name_end = skiptowhite(arg);
    cc = *name_end;
    *name_end = NUL;

    v = find_var(arg);
    if (v != NULL)	    /* existing variable, may need to free string */
	var_free_one(v);
    else		    /* non-existing variable */
	EMSG2("No such variable: \"%s\"", arg);

    *name_end = cc;
}

/*
 * types for expressions.
 */
enum exp_type
{
    TYPE_UNKNOWN = 0,
    TYPE_EQUAL,		/* == */
    TYPE_NEQUAL,	/* != */
    TYPE_GREATER,	/* >  */
    TYPE_GEQUAL,	/* >= */
    TYPE_SMALLER,	/* <  */
    TYPE_SEQUAL,	/* <= */
    TYPE_MATCH,		/* =~ */
    TYPE_NOMATCH	/* !~ */
};

/*
 * Handle zero level expression.
 * This calls eval1() and handles error message and nextcmd.
 * Return OK or FAIL.
 */
    static int
eval0(arg, retvar, nextcmd)
    char_u	*arg;
    VAR		retvar;
    char_u	**nextcmd;
{
    int		ret = OK;
    char_u	*p;

    p = skipwhite(arg);
    if (eval1(&p, retvar) == FAIL || !ends_excmd(*p))
    {
	EMSG2(e_invexpr2, arg);
	ret = FAIL;
    }
    if (nextcmd != NULL)
	*nextcmd = check_nextcmd(p);

    return ret;
}

/*
 * Handle first level expression:
 *	expr2 || expr2 || expr2	    logical OR
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to the next non-white after the recognized expression.
 *
 * Return OK or FAIL.
 */
    static int
eval1(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    var		var2;
    long	n1, n2;

    /*
     * Get the first variable.
     */
    if (eval2(arg, retvar) == FAIL)
	return FAIL;

    /*
     * Repeat until there is no following "||".
     */
    while ((*arg)[0] == '|' && (*arg)[1] == '|')
    {
	n1 = get_var_number(retvar);
	clear_var(retvar);

	/*
	 * Get the second variable.
	 */
	*arg = skipwhite(*arg + 2);
	if (eval2(arg, &var2) == FAIL)
	    return FAIL;

	/*
	 * Compute the result.
	 */
	n2 = get_var_number(&var2);
	clear_var(&var2);
	retvar->var_type = VAR_NUMBER;
	retvar->var_val.var_number = (n1 || n2);
    }

    return OK;
}

/*
 * Handle second level expression:
 *	expr3 && expr3 && expr3	    logical AND
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to the next non-white after the recognized expression.
 *
 * Return OK or FAIL.
 */
    static int
eval2(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    var		var2;
    long	n1, n2;

    /*
     * Get the first variable.
     */
    if (eval3(arg, retvar) == FAIL)
	return FAIL;

    /*
     * Repeat until there is no following "&&".
     */
    while ((*arg)[0] == '&' && (*arg)[1] == '&')
    {
	n1 = get_var_number(retvar);
	clear_var(retvar);

	/*
	 * Get the second variable.
	 */
	*arg = skipwhite(*arg + 2);
	if (eval3(arg, &var2) == FAIL)
	    return FAIL;

	/*
	 * Compute the result.
	 */
	n2 = get_var_number(&var2);
	clear_var(&var2);
	retvar->var_type = VAR_NUMBER;
	retvar->var_val.var_number = (n1 && n2);
    }

    return OK;
}

/*
 * Handle third level expression:
 *	var1 == var2
 *	var1 =~ var2
 *	var1 != var2
 *	var1 !~ var2
 *	var1 > var2
 *	var1 >= var2
 *	var1 < var2
 *	var1 <= var2
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to the next non-white after the recognized expression.
 *
 * Return OK or FAIL.
 */
    static int
eval3(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    var			var2;
    char_u		*p;
    int			i = 0;
    enum exp_type	type = TYPE_UNKNOWN;
    int			len = 2;
    long		n1 = FALSE, n2;
    char_u		*s1, *s2;
    char_u		buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    vim_regexp		*prog;

    /*
     * Get the first variable.
     */
    if (eval4(arg, retvar) == FAIL)
	return FAIL;

    p = *arg;
    switch (p[0])
    {
	case '=':   if (p[1] == '=')
			type = TYPE_EQUAL;
		    else if (p[1] == '~')
			type = TYPE_MATCH;
		    break;
	case '!':   if (p[1] == '=')
			type = TYPE_NEQUAL;
		    else if (p[1] == '~')
			type = TYPE_NOMATCH;
		    break;
	case '>':   if (p[1] != '=')
		    {
			type = TYPE_GREATER;
			len = 1;
		    }
		    else
			type = TYPE_GEQUAL;
		    break;
	case '<':   if (p[1] != '=')
		    {
			type = TYPE_SMALLER;
			len = 1;
		    }
		    else
			type = TYPE_SEQUAL;
		    break;
    }

    /*
     * If there is a comparitive operator, use it.
     */
    if (type != TYPE_UNKNOWN)
    {
	/*
	 * Get the second variable.
	 */
	*arg = skipwhite(p + len);
	if (eval4(arg, &var2) == FAIL)
	{
	    clear_var(retvar);
	    return FAIL;
	}

	/*
	 * If one of the two variables is a number, compare as a number.
	 * When using "=~" or "!~", always compare as string.
	 */
	if ((retvar->var_type == VAR_NUMBER || var2.var_type == VAR_NUMBER)
		&& type != TYPE_MATCH && type != TYPE_NOMATCH)
	{
	    n1 = get_var_number(retvar);
	    n2 = get_var_number(&var2);
	    switch (type)
	    {
		case TYPE_EQUAL:    n1 = (n1 == n2); break;
		case TYPE_NEQUAL:   n1 = (n1 != n2); break;
		case TYPE_GREATER:  n1 = (n1 > n2); break;
		case TYPE_GEQUAL:   n1 = (n1 >= n2); break;
		case TYPE_SMALLER:  n1 = (n1 < n2); break;
		case TYPE_SEQUAL:   n1 = (n1 <= n2); break;
		case TYPE_UNKNOWN:
		case TYPE_MATCH:
		case TYPE_NOMATCH:  break;  /* avoid gcc warning */
	    }
	}
	else
	{
	    s1 = get_var_string_buf(retvar, buf1);
	    s2 = get_var_string_buf(&var2, buf2);
	    if (type != TYPE_MATCH && type != TYPE_NOMATCH)
		i = STRCMP(s1, s2);
	    switch (type)
	    {
		case TYPE_EQUAL:    n1 = (i == 0); break;
		case TYPE_NEQUAL:   n1 = (i != 0); break;
		case TYPE_GREATER:  n1 = (i > 0); break;
		case TYPE_GEQUAL:   n1 = (i >= 0); break;
		case TYPE_SMALLER:  n1 = (i < 0); break;
		case TYPE_SEQUAL:   n1 = (i <= 0); break;
		case TYPE_MATCH:
		case TYPE_NOMATCH:  reg_ic = p_ic;
				    prog = vim_regcomp(s2, TRUE);
				    if (prog != NULL)
				    {
					n1 = vim_regexec(prog, s1, TRUE);
					vim_free(prog);
					if (type == TYPE_NOMATCH)
					    n1 = !n1;
				    }
				    break;
		case TYPE_UNKNOWN:  break;  /* avoid gcc warning */
	    }
	}
	clear_var(retvar);
	clear_var(&var2);
	retvar->var_type = VAR_NUMBER;
	retvar->var_val.var_number = n1;
    }

    return OK;
}

/*
 * Handle fourth level expression:
 *	+	number addition
 *	-	number subtraction
 *	.	string concatenation
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to the next non-white after the recognized expression.
 *
 * Return OK or FAIL.
 */
    static int
eval4(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    var		var2;
    int		op;
    long	n1, n2;
    char_u	*s1, *s2;
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*p;

    /*
     * Get the first variable.
     */
    if (eval5(arg, retvar) == FAIL)
	return FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    for (;;)
    {
	op = **arg;
	if (op != '+' && op != '-' && op != '.')
	    break;

	/*
	 * Get the second variable.
	 */
	*arg = skipwhite(*arg + 1);
	if (eval5(arg, &var2) == FAIL)
	{
	    clear_var(retvar);
	    return FAIL;
	}

	/*
	 * Compute the result.
	 */
	if (op == '.')
	{
	    s1 = get_var_string_buf(retvar, buf1);
	    s2 = get_var_string_buf(&var2, buf2);
	    p = alloc((unsigned)(STRLEN(s1) + STRLEN(s2) + 1));
	    if (p != NULL)
	    {
		STRCPY(p, s1);
		STRCAT(p, s2);
	    }
	    clear_var(retvar);
	    retvar->var_type = VAR_STRING;
	    retvar->var_val.var_string = p;
	}
	else
	{
	    n1 = get_var_number(retvar);
	    n2 = get_var_number(&var2);
	    clear_var(retvar);
	    if (op == '+')
		n1 = n1 + n2;
	    else
		n1 = n1 - n2;
	    retvar->var_type = VAR_NUMBER;
	    retvar->var_val.var_number = n1;
	}
	clear_var(&var2);
    }
    return OK;
}

/*
 * Handle fifth level expression:
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to the next non-white after the recognized expression.
 *
 * Return OK or FAIL.
 */
    static int
eval5(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    var		var2;
    int		op;
    long	n1, n2;

    /*
     * Get the first variable.
     */
    if (eval6(arg, retvar) == FAIL)
	return FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    for (;;)
    {
	op = **arg;
	if (op != '*' && op != '/' && op != '%')
	    break;

	n1 = get_var_number(retvar);
	clear_var(retvar);

	/*
	 * Get the second variable.
	 */
	*arg = skipwhite(*arg + 1);
	if (eval6(arg, &var2) == FAIL)
	    return FAIL;
	n2 = get_var_number(&var2);
	clear_var(&var2);

	/*
	 * Compute the result.
	 */
	if (op == '*')
	    n1 = n1 * n2;
	else if (op == '/')
	{
	    if (n2 == 0)	/* give an error message? */
		n1 = 0x7fffffff;
	    else
		n1 = n1 / n2;
	}
	else
	{
	    if (n2 == 0)	/* give an error message? */
		n1 = 0;
	    else
		n1 = n1 % n2;
	}
	retvar->var_type = VAR_NUMBER;
	retvar->var_val.var_number = n1;
    }

    return OK;
}

/*
 * Handle sixth level expression:
 *  number		number constant
 *  "string"		string contstant
 *  *option-name	option value
 *  @r			register contents
 *  identifier		variable value
 *  $VAR		environment variable
 *  (expression)	nested expression
 *
 *  Also handle:
 *  ! in front		logical NOT
 *  - in front		unary minus
 *  trailing []		subscript in String
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to the next non-white after the recognized expression.
 *
 * Return OK or FAIL.
 */
    static int
eval6(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    var		var2;
    long	n;
    int		len;
    char_u	*s;
    int		val;
    char_u	*start_leader, *end_leader;
    int		ret = OK;

    /*
     * Skip '!' and '-' characters.  They are handled later.
     */
    start_leader = *arg;
    while (**arg == '!' || **arg == '-')
	*arg = skipwhite(*arg + 1);
    end_leader = *arg;

    switch (**arg)
    {
    /*
     * Number constant.
     */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
		retvar->var_type = VAR_NUMBER;
		retvar->var_val.var_number =
				     vim_str2nr(*arg, NULL, &len, TRUE, TRUE);
		*arg += len;
		break;

    /*
     * String constant: "string".
     */
    case '\"':	ret = get_string_var(arg, retvar);
		break;

    /*
     * Literal string constant: 'string'.
     */
    case '\'':	ret = get_lit_string_var(arg, retvar);
		break;

    /*
     * Option value: &name
     */
    case '&':	ret = get_option_var(arg, retvar);
		break;

    /*
     * Environment variable: $VAR.
     */
    case '$':	ret = get_env_var(arg, retvar);
		break;

    /*
     * Register contents: @r.
     */
    case '@':	retvar->var_type = VAR_STRING;
		retvar->var_val.var_string = get_reg_contents(*++*arg);
		if (**arg != NUL)
		    ++*arg;
		break;

    /*
     * nested expression: (expression).
     */
    case '(':	*arg = skipwhite(*arg + 1);
		ret = eval1(arg, retvar);	/* recursive! */
		if (**arg != ')')
		    EMSG("Missing ')'");
		else
		    ++*arg;
		break;

    /*
     * Must be a variable or function name then.
     */
    default:	s = *arg;
		len = get_id_len(arg);
		if (len)
		{
		    if (**arg == '(')		/* recursive! */
			ret = get_func_var(s, len, retvar, arg);
		    else
			ret = get_var_var(s, len, retvar);
		}
		else
		    ret = FAIL;
		break;
    }
    *arg = skipwhite(*arg);

    /*
     * Handle expr[expr] subscript.
     */
    if (**arg == '[' && ret == OK)
    {
	/*
	 * Get the variable from inside the [].
	 */
	*arg = skipwhite(*arg + 1);
	if (eval1(arg, &var2) == FAIL)		/* recursive! */
	{
	    clear_var(retvar);
	    return FAIL;
	}
	n = get_var_number(&var2);

	/* Check for the ']'. */
	if (**arg != ']')
	{
	    EMSG("Missing ']'");
	    clear_var(retvar);
	    return FAIL;
	}

	/*
	 * The resulting variable is a string of a single character.
	 * If the index is too big or negative, the result is empty.
	 */
	s = get_var_string(retvar);
	if (n >= (long)STRLEN(s) || n < 0)
	    s = NULL;
	else
	    s = vim_strnsave(s + n, 1);
	clear_var(retvar);
	retvar->var_type = VAR_STRING;
	retvar->var_val.var_string = s;

	*arg = skipwhite(*arg + 1);	/* skip the ']' */
    }

    /*
     * Apply logical NOT and unary '-', from right to left.
     */
    if (ret == OK && end_leader > start_leader)
    {
	val = get_var_number(retvar);
	while (end_leader > start_leader)
	{
	    --end_leader;
	    if (*end_leader == '!')
		val = !val;
	    else if (*end_leader == '-')
		val = -val;
	}
	clear_var(retvar);
	retvar->var_type = VAR_NUMBER;
	retvar->var_val.var_number = val;
    }

    return ret;
}

/*
 * Get an option value.
 * "arg" points to the '&' before the option name.
 * "arg" is advanced to character after the option name.
 * Return OK or FAIL.
 */
    static int
get_option_var(arg, retvar)
    char_u	**arg;
    VAR		retvar;		/* when NULL, only check if option exists */
{
    char_u	*option_end;
    long	numval;
    char_u	*stringval;
    int		opt_type;
    int		c;
    int		ret = OK;

    /*
     * Isolate the option name and find its value.
     */
    option_end = find_option_end(*arg + 1);
    if (option_end == *arg + 1)
    {
	if (retvar != NULL)
	    EMSG2("Option name missing: %s", *arg);
	return FAIL;
    }

    c = *option_end;
    *option_end = NUL;
    opt_type = get_option_value(*arg + 1, &numval,
					  retvar == NULL ? NULL : &stringval);

    if (opt_type < 0)		    /* invalid name */
    {
	if (retvar != NULL)
	    EMSG2("Unknown option: %s", *arg + 1);
	ret = FAIL;
    }
    else if (retvar != NULL)
    {
	if (opt_type == 1)		/* number option */
	{
	    retvar->var_type = VAR_NUMBER;
	    retvar->var_val.var_number = numval;
	}
	else				/* string option */
	{
	    retvar->var_type = VAR_STRING;
	    retvar->var_val.var_string = stringval;
	}
    }

    *option_end = c;		    /* put back for error messages */
    *arg = option_end;

    return ret;
}

/*
 * Allocate a variable for an string constant.
 * Return OK or FAIL.
 */
    static int
get_string_var(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    char_u	*p;
    char_u	*name;
    int		i;
    int		mod;

    /*
     * Find the end of the string, skipping backslashed characters.
     */
    for (p = *arg + 1; *p && *p != '\"'; ++p)
	if (*p == '\\' && p[1] != NUL)
	    ++p;
    if (*p != '\"')
    {
	EMSG2("Missing quote: %s", *arg);
	return FAIL;
    }

    /*
     * Copy the string into allocated memory, handling backslashed
     * characters.
     */
    name = alloc((unsigned)(p - *arg));
    if (name == NULL)
	return FAIL;

    i = 0;
    for (p = *arg + 1; *p && *p != '\"'; ++p)
    {
	if (*p == '\\')
	{
	    switch (*++p)
	    {
		case 'b': name[i++] = BS; break;
		case 'e': name[i++] = ESC; break;
		case 'f': name[i++] = FF; break;
		case 'n': name[i++] = NL; break;
		case 'r': name[i++] = CR; break;
		case 't': name[i++] = TAB; break;

			  /* hex: "\x1", "\x12" */
		case 'X':
		case 'x': if (isxdigit(p[1]))
			  {
			      ++p;
			      name[i] = hex2nr(*p);
			      if (isxdigit(p[1]))
			      {
				  ++p;
				  name[i] = (name[i] << 4) + hex2nr(*p);
			      }
			      ++i;
			  }
			  else
			      name[i++] = *p;
			  break;

			  /* octal: "\1", "\12", "\123" */
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7': name[i] = *p - '0';
			  if (p[1] >= '0' && p[1] <= '7')
			  {
			      ++p;
			      name[i] = (name[i] << 3) + *p - '0';
			      if (p[1] >= '0' && p[1] <= '7')
			      {
				  ++p;
				  name[i] = (name[i] << 3) + *p - '0';
			      }
			  }
			  ++i;
			  break;

			    /* Special key, e.g.: <C-W> */
		case '<': name[i] = find_special_key(&p, &mod);
			  if (name[i])
			  {
			      ++i;
			      --p;
			      break;
			  }
			  /* FALLTHROUGH */

		default:  name[i++] = *p;
			  break;
	    }
	}
	else
	    name[i++] = *p;
    }
    name[i] = NUL;
    *arg = p + 1;

    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = name;

    return OK;
}

/*
 * Allocate a variable for an backtick-string constant.
 * Return OK or FAIL.
 */
    static int
get_lit_string_var(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    char_u	*p;
    char_u	*name;

    /*
     * Find the end of the string.
     */
    p = vim_strchr(*arg + 1, '\'');
    if (p == NULL)
    {
	EMSG2("Missing quote: %s", *arg);
	return FAIL;
    }

    /*
     * Copy the string into allocated memory.
     */
    name = vim_strnsave(*arg + 1, (int)(p - (*arg + 1)));
    if (name == NULL)
	return FAIL;

    *arg = p + 1;

    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = name;

    return OK;
}

/*
 * Get the value of an environment variable.
 * Return OK or FAIL.
 */
    static int
get_env_var(arg, retvar)
    char_u	**arg;
    VAR		retvar;
{
    char_u	*string;

    string = get_env_string(arg);

    /*
     * Allocate a variable.  If the environment variable was not set, silently
     * assume it is empty.
     */
    if (string != NULL)
	string = vim_strsave(string);
    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = string;

    return OK;
}

/*
 * Allocate a variable for the result of a function.
 * Return OK or FAIL.
 */
    static int
get_func_var(name, len, retvar, arg)
    char_u	*name;		/* name of the function */
    int		len;		/* length of "name" */
    VAR		retvar;
    char_u	**arg;		/* argument, pointing to the '(' */
{
    char_u	*argp;
    int		ret = FAIL;
#define MAX_FUNC_ARGS	4
    var		argvars[MAX_FUNC_ARGS];	/* vars for arguments */
    int		argcount = 0;		/* number of arguments found */
#define ERROR_NONE	0
#define ERROR_INVARG	1
#define ERROR_UNKOWN	2
#define ERROR_OTHER	3
    int		error = ERROR_NONE;
    int		i;
    int		cc;
    static struct fst
    {
	char	*f_name;	/* function name */
	char	f_argcount;	/* number of arguments */
	void	(*f_func) __ARGS((VAR args, VAR rvar));    /* impl. function */
    } functions[] =
    {{"buffer_exists",	    1, f_buffer_exists},
     {"char2nr",	    1, f_char2nr},
     {"col",		    1, f_col},
     {"delete",		    1, f_delete},
     {"exists",		    1, f_exists},
     {"expand",		    1, f_expand},
     {"file_readable",	    1, f_file_readable},
     {"getline",	    1, f_getline},
     {"has",		    1, f_has},
     {"highlight_exists",   1, f_highlight_exists},
     {"highlightID",	    1, f_highlightID},
     {"hostname",	    0, f_hostname},
     {"isdirectory",	    1, f_isdirectory},
     {"last_buffer_nr",	    0, f_last_buffer_nr},
     {"line",		    1, f_line},
     {"match",		    2, f_match},
     {"matchend",	    2, f_matchend},
     {"nr2char",	    1, f_nr2char},
#ifdef HAVE_STRFTIME
     {"strftime",	    1, f_strftime},
#endif
     {"strlen",		    1, f_strlen},
     {"strpart",	    3, f_strpart},
     {"synID",		    3, f_synID},
     {"synIDattr",	    2, f_synIDattr},
     {"synIDtrans",	    1, f_synIDtrans},
     {"substitute",	    4, f_substitute},
     {"tempname",	    0, f_tempname},
     {"virtcol",	    1, f_virtcol},
    };

    cc = name[len];
    name[len] = NUL;

    /*
     * Get the arguments.
     */
    argp = *arg;
    while (argcount < MAX_FUNC_ARGS)
    {
	argp = skipwhite(argp + 1);	    /* skip the '(' or ',' */
	if (*argp == ')')
	    break;
	if (eval1(&argp, &argvars[argcount]) == FAIL)
	{
	    error = ERROR_OTHER;
	    break;
	}
	++argcount;
	if (*argp != ',')
	    break;
    }
    if (*argp != ')' && error == ERROR_NONE)
	error = ERROR_INVARG;

    if (error == ERROR_NONE)
    {
	retvar->var_type = VAR_NUMBER;	/* default is number retvar */

	/*
	 * Find the function name in the table, call its implementation.
	 */
	error = ERROR_UNKOWN;
	for (i = 0; i < (int)(sizeof(functions) / sizeof(struct fst)); ++i)
	    if (STRCMP(name, functions[i].f_name) == 0)
	    {
		if (argcount != functions[i].f_argcount)
		    error = ERROR_INVARG;
		else
		{
		    functions[i].f_func(argvars, retvar);
		    error = ERROR_NONE;
		}
		break;
	    }

	if (error == ERROR_UNKOWN)
	    EMSG2("Unknown function: %s", name);

	*arg = skipwhite(argp + 1);
	if (error == ERROR_NONE)
	    ret = OK;
    }

    while (--argcount >= 0)
	clear_var(&argvars[argcount]);

    if (error == ERROR_INVARG)
	EMSG2("Invalid arguments for function %s", name);

    name[len] = cc;

    return ret;
}

/*
 * "buffer_exists()" function.
 */
    static void
f_buffer_exists(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    int		n = FALSE;
    char_u	*name;

    if (argvars[0].var_type == VAR_NUMBER)
	n = (buflist_findnr((int)argvars[0].var_val.var_number) != NULL);
    else if (argvars[0].var_val.var_string != NULL)
    {
	/* First make the name into a full path name */
	name = FullName_save(argvars[0].var_val.var_string,
#ifdef UNIX
		TRUE	    /* force expansion, get rid of symbolic links */
#else
		FALSE
#endif
		);
	if (name != NULL)
	{
	    n = (buflist_findname(name) != NULL);
	    vim_free(name);
	}
    }

    retvar->var_val.var_number = n;
}

/*
 * "char2nr()" function
 */
    static void
f_char2nr(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_val.var_number = get_var_string(&argvars[0])[0];
}

/*
 * "col(string)" function
 */
    static void
f_col(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    colnr_t	col = 0;
    FPOS	*fp;

    fp = var2fpos(&argvars[0]);
    if (fp != NULL && fp->lnum > 0)
	col = fp->col + 1;

    retvar->var_val.var_number = col;
}

/*
 * "delete()" function
 */
    static void
f_delete(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_val.var_number = vim_remove(get_var_string(&argvars[0]));
}

/*
 * "exists()" function
 */
    static void
f_exists(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u	*p;
    char_u	*name;
    int		n = FALSE;
    int		len;

    p = get_var_string(&argvars[0]);
    if (*p == '$')			/* environment variable */
	n = (get_env_string(&p) != NULL);
    else if (*p == '&')			/* option */
	n = (get_option_var(&p, NULL) == OK);
    else				/* internal variable */
    {
	name = p;
	len = get_id_len(&p);
	if (len != 0)
	    n = (get_var_var(name, len, NULL) == OK);
    }

    retvar->var_val.var_number = n;
}

/*
 * "expand()" function
 */
    static void
f_expand(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u	*s;
    int		len;
    char_u	*errormsg;

    retvar->var_type = VAR_STRING;
    s = get_var_string(&argvars[0]);
    if (*s == '%' || *s == '#' || *s == '<')
	retvar->var_val.var_string = eval_vars(s, &len, NULL, &errormsg);
    else
	retvar->var_val.var_string = ExpandOne(s, NULL, 0, WILD_ALL);
}

/*
 * "file_readable()" function
 */
    static void
f_file_readable(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    FILE	*fd;
    char_u	*p;
    int		n;

    p = get_var_string(&argvars[0]);
    if (!mch_isdir(p) && (fd = fopen((char *)p, "r")) != NULL)
    {
	n = TRUE;
	fclose(fd);
    }
    else
	n = FALSE;

    retvar->var_val.var_number = n;
}

/*
 * "getline(lnum)" function
 */
    static void
f_getline(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    linenr_t	lnum;
    char_u	*p;

    lnum = get_var_number(&argvars[0]);
    if (lnum == 0)  /* no valid number, try using line() */
    {
	f_line(argvars, retvar);
	lnum = retvar->var_val.var_number;
	clear_var(retvar);
    }

    if (lnum >= 1 && lnum <= curbuf->b_ml.ml_line_count)
	p = ml_get(lnum);
    else
	p = (char_u *)"";

    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = vim_strsave(p);
}

/*
 * "has()" function
 */
    static void
f_has(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    int		i;
    char_u	*name;
    int		n = FALSE;
    static char	*(has_list[]) =
    {
#ifdef MSDOS
# ifdef DJGPP
	"dos32",
# else
	"dos16",
# endif
#endif
#ifdef WIN32
	"win32",
#endif
#ifdef AMIGA
	"amiga",
# ifndef NO_ARP
	"arp",
# endif
#endif
#ifdef __BEOS__
	"beos",
#endif
#ifdef macintosh
	"mac",
#endif
#ifdef UNIX
	"unix",
#endif
#ifndef CASE_INSENSITIVE_FILENAME
	"fname_case",
#endif
#ifdef AUTOCMD
	"autocmd",
#endif
#if defined(SOME_BUILTIN_TCAPS) || defined(ALL_BUILTIN_TCAPS)
	"builtin_terms",
# ifdef ALL_BUILTIN_TCAPS
	"all_builtin_terms",
# endif
#endif
#ifdef CINDENT
	"cindent",
#endif
#ifdef DEBUG
	"debug",
#endif
#ifdef DIGRAPHS
	"digraphs",
#endif
#ifdef EMACS_TAGS
	"emacs_tags",
#endif
	"eval",	    /* always present, of course! */
#ifdef EX_EXTRA
	"ex_extra",
#endif
#ifdef EXTRA_SEARCH
	"extra_search",
#endif
#ifdef FKMAP
	"farsi",
#endif
#ifdef FILE_IN_PATH
	"file_in_path",
#endif
#ifdef FIND_IN_PATH
	"find_in_path",
#endif
#if !defined(USE_SYSTEM) && defined(UNIX)
	"fork",
#endif
#ifdef USE_GUI
	"gui",
#endif
#ifdef USE_GUI_ATHENA
	"gui_athena",
#endif
#ifdef USE_GUI_BEOS
	"gui_beos",
#endif
#ifdef USE_GUI_MAC
	"gui_mac",
#endif
#ifdef USE_GUI_MOTIF
	"gui_motif",
#endif
#ifdef USE_GUI_WIN32
	"gui_win32",
#endif
#ifdef INSERT_EXPAND
	"insert_expand",
#endif
#ifdef HAVE_LANGMAP
	"langmap",
#endif
#ifdef LISPINDENT
	"lispindent",
#endif
#ifdef UNIX
# ifdef DEC_MOUSE
	"mouse_dec",
# endif
# ifdef NETTERM_MOUSE
	"mouse_netterm",
# endif
# ifdef XTERM_MOUSE
	"mouse_xterm",
# endif
#endif
#ifdef HAVE_OLE
	"ole",
#endif
#ifdef HAVE_PERL_INTERP
	"perl",
#endif
#ifdef HAVE_PYTHON
	"python",
#endif
#ifdef QUICKFIX
	"quickfix",
#endif
#ifdef RIGHTLEFT
	"rightleft",
#endif
#ifdef SHOWCMD
	"showcmd",
#endif
#ifdef SMARTINDENT
	"smartindent",
#endif
#ifdef SYNTAX_HL
	"syntax",
#endif
#if defined(USE_SYSTEM) || !defined(UNIX)
	"system",
#endif
#ifdef BINARY_TAGS
	"tag_binary",
#endif
#ifdef OLD_STATIC_TAGS
	"tag_old_static",
#endif
#ifdef TAG_ANY_WHITE
	"tag_any_white",
#endif
#ifdef TERMINFO
	"terminfo",
#endif
#ifdef TEXT_OBJECTS
	"textobjects",
#endif
#ifdef HAVE_TGETENT
	"tgetent",
#endif
#ifdef VIMINFO
	"viminfo",
#endif
#ifdef WRITEBACKUP
	"writebackup",
#endif
#ifdef SAVE_XTERM_SCREEN
	"xterm_save",
#endif
#if defined(UNIX) && defined(WANT_X11) && defined(HAVE_X11)
	"X11",
#endif
	NULL
    };

    name = get_var_string(&argvars[0]);
    for (i = 0; has_list[i] != NULL; ++i)
	if (STRICMP(name, has_list[i]) == 0)
	{
	    n = TRUE;
	    break;
	}

    if (n == FALSE)
    {
#ifdef USE_GUI
	if (STRICMP(name, "gui_running") == 0)
	{
	    if (gui.in_use || gui.starting)
		n = TRUE;
	}
#endif
#ifdef SYNTAX_HL
# ifdef USE_GUI
	else
# endif
	    if (STRICMP(name, "syntax_items") == 0)
	{
	    if (syntax_present(curbuf))
		n = TRUE;
	}
#endif
    }

    retvar->var_val.var_number = n;
}

/*
 * "highlight_exists()" function
 */
    static void
f_highlight_exists(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_val.var_number = highlight_exists(get_var_string(&argvars[0]));
}

/*
 * "highlightID(name)" function
 */
    static void
f_highlightID(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_val.var_number = syn_name2id(get_var_string(&argvars[0]));
}

/*
 * "hostname()" function
 */
/*ARGSUSED*/
    static void
f_hostname(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u hostname[256];

    mch_get_host_name(hostname, 256);
    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = vim_strsave(hostname);
}

/*
 * "isdirectory()" function
 */
    static void
f_isdirectory(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_val.var_number = mch_isdir(get_var_string(&argvars[0]));
}

/*
 * "last_buffer_nr()" function.
 */
/*ARGSUSED*/
    static void
f_last_buffer_nr(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    int		n = 0;
    BUF		*buf;

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	if (n < buf->b_fnum)
	    n = buf->b_fnum;

    retvar->var_val.var_number = n;
}

/*
 * "line(string)" function
 */
    static void
f_line(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    linenr_t	lnum = 0;
    FPOS	*fp;

    fp = var2fpos(&argvars[0]);
    if (fp != NULL)
	lnum = fp->lnum;
    else if (get_var_string(&argvars[0])[0] == '$')   /* last line in buffer */
	lnum = curbuf->b_ml.ml_line_count;

    retvar->var_val.var_number = lnum;
}

/*
 * "match()" function
 */
    static void
f_match(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    f_some_match(argvars, retvar, TRUE);
}

/*
 * "matchend()" function
 */
    static void
f_matchend(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    f_some_match(argvars, retvar, FALSE);
}

    static void
f_some_match(argvars, retvar, start)
    VAR		argvars;
    VAR		retvar;
    int		start;
{
    char_u		*str;
    char_u		*pat;
    int			n = -1;
    vim_regexp		*prog;
    char_u		patbuf[NUMBUFLEN];

    str = get_var_string(&argvars[0]);
    pat = get_var_string_buf(&argvars[1], patbuf);

    reg_ic = p_ic;
    prog = vim_regcomp(pat, TRUE);
    if (prog != NULL)
    {
	if (vim_regexec(prog, str, TRUE))
	{
	    if (start)
		n = prog->startp[0] - str;
	    else
		n = prog->endp[0] - str;
	}
	vim_free(prog);
    }
    retvar->var_val.var_number = n;
}

/*
 * "nr2char()" function
 */
    static void
f_nr2char(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u	buf[2];

    buf[0] = (char_u)get_var_number(&argvars[0]);
    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = vim_strnsave(buf, 1);
}

#ifdef HAVE_STRFTIME
/*
 * "strftime()" function
 */
    static void
f_strftime(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u	result_buf[80];
    struct tm	*curtime;
    time_t	seconds;
    char_u	*p;

    p = get_var_string(&argvars[0]);
    seconds = time(NULL);
    curtime = localtime(&seconds);
    (void)strftime((char *)result_buf, (size_t)80, (char *)p, curtime);

    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = vim_strsave(result_buf);
}
#endif

/*
 * "strlen()" function
 */
    static void
f_strlen(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_val.var_number = STRLEN(get_var_string(&argvars[0]));
}

/*
 * "strpart()" function
 */
    static void
f_strpart(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u	*p;
    int		n;
    int		len;
    int		slen;

    p = get_var_string(&argvars[0]);
    n = get_var_number(&argvars[1]);
    len = get_var_number(&argvars[2]);
    slen = STRLEN(p);
    /*
     * Only return the overlap between the specified part and the actual
     * string.
     */
    if (n < 0)
    {
	len += n;
	n = 0;
    }
    else if (n > slen)
	n = slen;
    if (len < 0)
	len = 0;
    else if (n + len > slen)
	len = slen - n;

    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = vim_strnsave(p + n, len);
}

/*
 * "synID(line, col, trans)" function
 */
    static void
f_synID(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    int		id = 0;
#ifdef SYNTAX_HL
    long	line;
    long	col;
    int		trans;

    line = get_var_number(&argvars[0]);
    col = get_var_number(&argvars[1]) - 1;
    trans = get_var_number(&argvars[2]);

    if (line >= 1 && line <= curbuf->b_ml.ml_line_count
	    && col >= 0 && col < (long)STRLEN(ml_get(line)))
	id = syn_get_id(line, col, trans);
#endif

    retvar->var_val.var_number = id;
}

/*
 * "synIDattr(id, what)" function
 */
    static void
f_synIDattr(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u	*p = NULL;
#ifdef SYNTAX_HL
    int		id;
    char_u	*what;

    id = get_var_number(&argvars[0]);
    what = get_var_string(&argvars[1]);

    switch (TO_LOWER(what[0]))
    {
	case 'b':
		if (TO_LOWER(what[1]) == 'g')		/* bg[#] */
		    p = highlight_color(id, what);
		else					/* bold */
		    p = highlight_has_attr(id, HL_BOLD);
		break;

	case 'f':					/* fg[#] */
		p = highlight_color(id, what);
		break;

	case 'i':
		if (TO_LOWER(what[1]) == 'n')		/* inverse */
		    p = highlight_has_attr(id, HL_INVERSE);
		else					/* italic */
		    p = highlight_has_attr(id, HL_ITALIC);
		break;

	case 'n':					/* name */
		p = get_highlight_name(id - 1);
		break;

	case 'r':					/* reverse */
		p = highlight_has_attr(id, HL_INVERSE);
		break;

	case 's':					/* standout */
		p = highlight_has_attr(id, HL_STANDOUT);
		break;

	case 'u':					/* underline */
		p = highlight_has_attr(id, HL_UNDERLINE);
		break;
    }

    if (p != NULL)
	p = vim_strsave(p);
#endif
    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = p;
}

/*
 * "synIDtrans(id)" function
 */
    static void
f_synIDtrans(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    int		id;

#ifdef SYNTAX_HL
    id = get_var_number(&argvars[0]);

    if (id > 0)
	id = syn_get_final_id(id);
    else
#endif
	id = 0;

    retvar->var_val.var_number = id;
}

/*
 * "substitute()" function
 */
    static void
f_substitute(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    char_u		*str;
    char_u		*pat;
    char_u		*sub;
    char_u		*result = NULL;
    int			sublen;
    vim_regexp		*prog;
    int			i;
    char_u		patbuf[NUMBUFLEN];
    char_u		subbuf[NUMBUFLEN];

    str = get_var_string(&argvars[0]);
    pat = get_var_string_buf(&argvars[1], patbuf);
    sub = get_var_string_buf(&argvars[2], subbuf);
    /* flags = get_var_string_buf(&argvars[3], flagsbuf); */

    reg_ic = p_ic;
    prog = vim_regcomp(pat, TRUE);
    if (prog != NULL)
    {
	if (vim_regexec(prog, str, TRUE))
	{
	    /*
	     * Get some space for a temporary buffer to do the substitution
	     * into.  It will contain:
	     * - The text up to where the match is.
	     * - The substituted text.
	     * - The text after the match.
	     */
	    sublen = vim_regsub(prog, sub, str, FALSE, TRUE);
	    if ((result = alloc_check((unsigned)(STRLEN(str) + sublen
			       - (prog->endp[0] - prog->startp[0])))) != NULL)
	    {
		i = prog->startp[0] - str;
		vim_memmove(result, str, (size_t)i);
		(void)vim_regsub(prog, sub, result + i, TRUE, TRUE);
		STRCAT(result, prog->endp[0]);
	    }
	}
	vim_free(prog);
    }
    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = (result == NULL ? vim_strsave(str) : result);
}

/*
 * "tempname()" function
 */
/*ARGSUSED*/
    static void
f_tempname(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    retvar->var_type = VAR_STRING;
    retvar->var_val.var_string = vim_tempname('X');
}

/*
 * "virtcol(string)" function
 */
    static void
f_virtcol(argvars, retvar)
    VAR		argvars;
    VAR		retvar;
{
    colnr_t	vcol = 0;
    FPOS	*fp;

    fp = var2fpos(&argvars[0]);
    if (fp != NULL)
    {
	getvcol(curwin, fp, NULL, NULL, &vcol);
	++vcol;
    }

    retvar->var_val.var_number = vcol;
}

/*
 * Translate a String variable into a position (for col() and virtcol()).
 */
    static FPOS *
var2fpos(varp)
    VAR		varp;
{
    char_u	*name;

    name = get_var_string(varp);
    if (name[0] == '.')		/* cursor */
	return &curwin->w_cursor;
    if (name[0] == '\'')	/* mark */
	return getmark(name[1], FALSE);
    return NULL;
}

/*
 * Get the lenght of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    static int
get_env_len(arg)
    char_u	**arg;
{
    char_u	*p;
    int		len;

    for (p = *arg; vim_isIDc(*p); ++p)
	;
    if (p == *arg)	    /* no name found */
	return 0;

    len = p - *arg;
    *arg = p;
    return len;
}

/*
 * Get the value of an environment variable.
 * "arg" points to the '$' before the env var name.
 * "arg" is advanced to the first character after the name.
 * Return NULL for failure.
 */
    char_u *
get_env_string(arg)
    char_u	**arg;
{
    int	    len;
    int	    cc;
    char_u  *name;
    char_u  *s;

    ++*arg;
    name = *arg;
    len = get_env_len(arg);
    if (len == 0)
	return NULL;
    cc = name[len];
    name[len] = NUL;
    s = vim_getenv(name);
    name[len] = cc;

    return s;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to the first non-white character after the name.
 * Return 0 if something is wrong.
 */
    static int
get_id_len(arg)
    char_u	**arg;
{
    char_u	*p;
    int		len;

    /* Find the end of the name. */
    for (p = *arg; eval_isnamec(*p); ++p)
	;
    if (p == *arg)	    /* no name found */
	return 0;

    len = p - *arg;
    *arg = skipwhite(p);

    return len;
}

    static int
eval_isnamec(c)
    int	    c;
{
    return (isalpha(c) || isdigit(c) || c == '_' || c == ':');
}

/*
 * Get the value of internal variable "name".
 * Return OK or FAIL.
 */
    static int
get_var_var(name, len, retvar)
    char_u	*name;
    int		len;		/* length of "name" */
    VAR		retvar;		/* NULL when only checking existence */
{
    int		ret = OK;
    int		type = VAR_UNKNOWN;
    long	number = 1;
    char_u	*string = NULL;
    VAR		v;
    int		cc;

    cc = name[len];
    name[len] = NUL;

    /*
     * Check for built-in variables.
     */
    if (len == 7 && STRCMP(name, "version") == 0)
    {
	type = VAR_NUMBER;
	number = get_version();
    }
    else if (len == 5 && STRCMP(name, "count") == 0)
    {
	type = VAR_NUMBER;
	number = global_opnum;
    }

    /*
     * Check for user-defined variables.
     */
    else
    {
	v = find_var(name);
	if (v != NULL)
	{
	    type = v->var_type;
	    number = v->var_val.var_number;
	    string = v->var_val.var_string;
	}
    }

    if (type == VAR_UNKNOWN)
    {
	if (retvar != NULL)
	    EMSG2("Undefined variable: %s", name);
	ret = FAIL;
    }
    else if (retvar != NULL)
    {
	retvar->var_type = type;
	if (type == VAR_NUMBER)
	    retvar->var_val.var_number = number;
	else
	{
	    if (string != NULL)
		string = vim_strsave(string);
	    retvar->var_val.var_string = string;
	}
    }

    name[len] = cc;

    return ret;
}

/*
 * Allocate memory for a variable, and make it emtpy (0 or NULL value).
 */
    static VAR
alloc_var()
{
    return (VAR)alloc_clear((unsigned)sizeof(var));
}

/*
 * Allocate memory for a variable, and assign a string to it.
 * The string "s" must have been allocated, it is consumed.
 * Return NULL for out of memory, the variable otherwise.
 */
    static VAR
alloc_string_var(s)
    char_u	*s;
{
    VAR	    retvar;

    retvar = alloc_var();
    if (retvar != NULL)
    {
	retvar->var_type = VAR_STRING;
	retvar->var_val.var_string = s;
    }
    else
	vim_free(s);
    return retvar;
}

/*
 * Free the memory for a variable.
 */
    static void
free_var(varp)
    VAR	    varp;
{
    if (varp != NULL)
    {
	if (varp->var_type == VAR_STRING)
	    vim_free(varp->var_val.var_string);
	vim_free(varp->var_name);
	vim_free(varp);
    }
}

/*
 * Free the memory for a variable value and set the value to NULL or 0.
 */
    static void
clear_var(varp)
    VAR	    varp;
{
    if (varp != NULL)
    {
	if (varp->var_type == VAR_STRING)
	{
	    vim_free(varp->var_val.var_string);
	    varp->var_val.var_string = NULL;
	}
	else
	    varp->var_val.var_number = 0;
    }
}

/*
 * Get the number value of a variable.
 * If it is a String variable, use vim_str2nr().
 */
    static long
get_var_number(varp)
    VAR	    varp;
{
    if (varp->var_type == VAR_NUMBER)
	return (long)(varp->var_val.var_number);
    else if (varp->var_val.var_string == NULL)
	return 0L;
    else
	return vim_str2nr(varp->var_val.var_string, NULL, NULL, TRUE, TRUE);
}

/*
 * Get the string value of a variable.
 * If it is a Number variable, the number is converted into a string.
 * get_var_string() uses a single, static buffer.  You can only use it once!
 * get_var_string_buf() uses a given buffer.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    static char_u *
get_var_string(varp)
    VAR	    varp;
{
    static char_u   mybuf[NUMBUFLEN];

    return get_var_string_buf(varp, mybuf);
}

    static char_u *
get_var_string_buf(varp, buf)
    VAR	    varp;
    char_u  *buf;
{
    if (varp->var_type == VAR_NUMBER)
    {
	sprintf((char *)buf, "%ld", (long)varp->var_val.var_number);
	return buf;
    }
    else if (varp->var_val.var_string == NULL)
	return (char_u *)"";
    else
	return varp->var_val.var_string;
}

/*
 * Find variable "name" in the list of variables.
 * Return a pointer to it if found, NULL if not found.
 */
    static VAR
find_var(name)
    char_u	*name;
{
    int			i;
    char_u		*varname;
    struct growarray	*gap;

    gap = find_var_ga(name, &varname);
    if (gap == NULL)
	return NULL;

    for (i = gap->ga_len; --i >= 0; )
	if (VAR_GAP_ENTRY(i, gap).var_name != NULL
		&& STRCMP(VAR_GAP_ENTRY(i, gap).var_name, varname) == 0)
	    break;
    if (i < 0)
	return NULL;
    return &VAR_GAP_ENTRY(i, gap);
}

/*
 * Find the growarray and start of acutal variable name for a variable name.
 */
    static struct growarray *
find_var_ga(name, varname)
    char_u  *name;
    char_u  **varname;
{
    char_u	*p;

    p = vim_strchr(name, ':');
    if (p == NULL)			/* internal variable */
    {
	*varname = name;
	var_init(&variables);
	return &variables;
    }
    *varname = p + 1;
    if (*name == 'b')			/* local buffer variable */
	return &curbuf->b_vars;
    if (*name == 'w')			/* local window variable */
	return &curwin->w_vars;
    return NULL;
}

/*
 * Initialize internal variables for use.
 */
    void
var_init(gap)
    struct growarray *gap;
{
    gap->ga_itemsize = sizeof(var);
    gap->ga_growsize = 4;
}

/*
 * Clean up a list of internal variables.
 */
    void
var_clear(gap)
    struct growarray *gap;
{
    int	    i;

    for (i = gap->ga_len; --i >= 0; )
	var_free_one(&VAR_GAP_ENTRY(i, gap));
    ga_clear(gap);
}

    static void
var_free_one(v)
    VAR	    v;
{
    vim_free(v->var_name);
    v->var_name = NULL;
    if (v->var_type == VAR_STRING)
	vim_free(v->var_val.var_string);
    v->var_val.var_string = NULL;
}

/*
 * List the value of one internal variable.
 */
    static void
list_one_var(v, prefix)
    VAR	    v;
    char_u  *prefix;
{
    msg(prefix);
    msg_puts(v->var_name);
    msg_putchar(' ');
    msg_advance(22);
    if (v->var_type == VAR_NUMBER)
	msg_putchar('#');
    else
	msg_putchar(' ');
    msg_puts(get_var_string(v));
}

/*
 * Set variable "name" to value in "varp".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    static void
set_var(name, varp)
    char_u	*name;
    VAR		varp;
{
    int			i;
    VAR			v;
    char_u		*varname;
    struct growarray	*gap;

    v = find_var(name);
    if (v != NULL)	    /* existing variable, only need to free string */
    {
	if (v->var_type == VAR_STRING)
	    vim_free(v->var_val.var_string);
    }
    else		    /* add a new variable */
    {
	gap = find_var_ga(name, &varname);
	if (gap == NULL)    /* illegal name */
	    return;

	/* Try to use an empty entry */
	for (i = gap->ga_len; --i >= 0; )
	    if (VAR_GAP_ENTRY(i, gap).var_name == NULL)
		break;
	if (i < 0)	    /* need to allocated more room */
	{
	    if (ga_grow(gap, 1) == FAIL)
		return;
	    i = gap->ga_len;
	}
	v = &VAR_GAP_ENTRY(i, gap);
	if ((v->var_name = vim_strsave(varname)) == NULL)
	    return;
	if (i == gap->ga_len)
	{
	    ++gap->ga_len;
	    --gap->ga_room;
	}
    }

    v->var_type = varp->var_type;
    if (varp->var_type == VAR_STRING)
	v->var_val.var_string = vim_strsave(get_var_string(varp));
    else
	v->var_val.var_number = varp->var_val.var_number;
}

/*
 * Implementation of
 * ":echo expr1 .."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 .."	print each argument plain.
 */
    void
do_echo(eap, echo)
    EXARG	*eap;
    int		echo;	    /* TRUE for ":echo" command, FALSE for ":echon" */
{
    char_u	*arg = eap->arg;
    var		retvar;
    char_u	*p;
    int		first = TRUE;

    if (eap->skip)
	++emsg_off;
    else if (echo)
	msg_start();
    while (*arg != NUL && *arg != '|' && *arg != '\n')
    {
	if (eval1(&arg, &retvar) == FAIL)
	    break;
	if (!eap->skip)
	{
	    if (arg != eap->arg && echo)
		msg_putchar(' ');
	    for (p = get_var_string(&retvar); *p != NUL; ++p)
		if (*p == '\n' || *p == '\r' || *p == TAB)
		{
		    if (*p != TAB && first)
		    {
			/* remove any text still there from the command */
			msg_clr_eos();
			first = FALSE;
		    }
		    msg_putchar(*p);
		}
		else
		    msg_puts(transchar(*p));
	}
	clear_var(&retvar);
	arg = skipwhite(arg);
    }
    eap->nextcmd = check_nextcmd(arg);

    if (eap->skip)
	--emsg_off;
    else
    {
	/* remove text that may still be there from the command */
	if (first)
	    msg_clr_eos();
	if (echo)
	    msg_end();
    }
}

/*
 * Implementation of
 * ":execute expr1 .."	execute the result of an expression.
 */
    void
do_execute(eap, getline, cookie)
    EXARG	*eap;
    char_u	*(*getline) __ARGS((int, void *, int));
    void	*cookie;		/* argument for getline() */
{
    char_u	*arg = eap->arg;
    var		retvar;
    int		ret = OK;
    char_u	*p;
    struct growarray ga;
    int		len;

    ga_init(&ga);
    ga.ga_itemsize = 1;
    ga.ga_growsize = 80;

    if (eap->skip)
	++emsg_off;
    while (*arg != NUL && *arg != '|' && *arg != '\n')
    {
	if (eval1(&arg, &retvar) == FAIL)
	{
	    ret = FAIL;
	    break;
	}

	if (!eap->skip)
	{
	    p = get_var_string(&retvar);
	    len = STRLEN(p);
	    if (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_var(&retvar);
		ret = FAIL;
		break;
	    }
	    if (ga.ga_len)
	    {
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
		--ga.ga_room;
	    }
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_room -= len;
	    ga.ga_len += len;
	}

	clear_var(&retvar);
	arg = skipwhite(arg);
    }

    if (ret != FAIL && ga.ga_data != NULL)
	do_cmdline((char_u *)ga.ga_data,
				 getline, cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);

    ga_clear(&ga);

    if (eap->skip)
	--emsg_off;

    eap->nextcmd = check_nextcmd(arg);
}

    static char_u *
find_option_end(p)
    char_u  *p;
{
    while (isalnum(*p) || *p == '_')
	++p;
    return p;
}

#endif /* WANT_EVAL */
