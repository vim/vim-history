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
 * Code to handle user-settable parameters. This is all pretty much table-
 * driven. To add a new parameter, put it in the params array, and add a
 * variable for it in param.h. If it's a numeric parameter, add any necessary
 * bounds checks to doset().
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

struct param
{
	char		*fullname;		/* full parameter name */
	char		*shortname; 	/* permissible abbreviation */
	int 		flags;			/* see below */
	char		*var;			/* pointer to variable */
};

/*
 * Flags
 */
#define P_BOOL			0x01	/* the parameter is boolean */
#define P_NUM			0x02	/* the parameter is numeric */
#define P_STRING		0x04	/* the parameter is a string */
#define P_CHANGED		0x08	/* the parameter has been changed */
#define P_EXPAND		0x10	/* environment expansion */

/*
 * The param structure is initialized here.
 * The order of the parameters should be alfabetic
 * The parameters with a NULL variable are 'hidden': a set command for
 * them is ignored and they are not printed.
 */
struct param params[] =
{
		{"autoindent",	"ai",	P_BOOL,		(char *)&p_ai},
		{"autoprint",	"ap",	P_BOOL,		(char *)NULL},
		{"autowrite",	"aw",	P_BOOL,		(char *)&p_aw},
		{"backspace",	"bs",	P_NUM,		(char *)&p_bs},
		{"backup",		"bk",	P_BOOL,		(char *)&p_bk},
#ifdef UNIX
 		{"backupdir",	"bdir",	P_STRING|P_EXPAND,	(char *)&p_bdir},
#endif
		{"beautify",	"bf",	P_BOOL,		(char *)NULL},
		{"binary",		"bin",	P_BOOL,		(char *)&p_bin},
#ifdef MSDOS
		{"bioskey",		"biosk",P_BOOL,		(char *)&p_biosk},
#endif
		{"columns",		"co",	P_NUM,		(char *)&Columns},
		{"compatible",	"cp",	P_BOOL,		(char *)&p_cp},
#ifdef DIGRAPHS
		{"digraph",		"dg",	P_BOOL,		(char *)&p_dg},
#endif /* DIGRAPHS */
 		{"directory",	"dir",	P_STRING|P_EXPAND,	(char *)&p_dir},
		{"edcompatible",NULL,	P_BOOL,		(char *)&p_ed},
		{"equalprg",	"ep",  	P_STRING|P_EXPAND,	(char *)&p_ep},
		{"errorbells",	"eb",	P_BOOL,		(char *)&p_eb},
		{"errorfile",	"ef",  	P_STRING|P_EXPAND,	(char *)&p_ef},
		{"errorformat",	"efm", 	P_STRING,	(char *)&p_efm},
		{"esckeys",		"ek",	P_BOOL,		(char *)&p_ek},
		{"expandtab",	"et",	P_BOOL,		(char *)&p_et},
		{"exrc",		NULL,	P_BOOL,		(char *)&p_exrc},
		{"formatprg",	"fp",  	P_STRING|P_EXPAND,	(char *)&p_fp},
		{"graphic",		"gr",	P_BOOL,		(char *)&p_gr},
		{"hardtabs",	"ht",	P_NUM,		(char *)NULL},
		{"helpfile",	"hf",  	P_STRING|P_EXPAND,	(char *)&p_hf},
		{"history", 	"hi", 	P_NUM,		(char *)&p_hi},
		{"ignorecase",	"ic",	P_BOOL,		(char *)&p_ic},
		{"insertmode",	"im",	P_BOOL,		(char *)&p_im},
		{"joinspaces", 	"js",	P_BOOL,		(char *)&p_js},
		{"keywordprg",	"kp",  	P_STRING|P_EXPAND,	(char *)&p_kp},
		{"lines",		NULL, 	P_NUM,		(char *)&Rows},
		{"lisp",		NULL,	P_BOOL,		(char *)NULL},
		{"list",		NULL,	P_BOOL,		(char *)&p_list},
		{"magic",		NULL,	P_BOOL,		(char *)&p_magic},
		{"makeprg",		"mp",  	P_STRING|P_EXPAND,	(char *)&p_mp},
		{"mesg",		NULL,	P_BOOL,		(char *)NULL},
		{"modeline",	"ml",	P_BOOL,		(char *)&p_ml},
		{"modelines",	"mls",	P_NUM,		(char *)&p_mls},
		{"number",		"nu",	P_BOOL,		(char *)&p_nu},
		{"open",		NULL,	P_BOOL,		(char *)NULL},
		{"optimize",	"opt",	P_BOOL,		(char *)NULL},
		{"paragraphs",	"para",	P_STRING,	(char *)&p_para},
		{"paste",		NULL,	P_BOOL,		(char *)&p_paste},
		{"prompt",		NULL,	P_BOOL,		(char *)NULL},
		{"readonly",	"ro",	P_BOOL,		(char *)&p_ro},
		{"redraw",		NULL,	P_BOOL,		(char *)NULL},
		{"remap",		NULL,	P_BOOL,		(char *)&p_remap},
		{"report",		NULL,	P_NUM,		(char *)&p_report},
		{"revins",		"ri",	P_BOOL,		(char *)&p_ri},
		{"ruler",		"ru",	P_BOOL,		(char *)&p_ru},
		{"secure",		NULL,	P_BOOL,		(char *)&p_secure},
		{"scroll",		NULL, 	P_NUM,		(char *)&p_scroll},
		{"scrolljump",	"sj", 	P_NUM,		(char *)&p_sj},
		{"sections",	"sect",	P_STRING,	(char *)&p_sections},
		{"shell",		"sh",	P_STRING|P_EXPAND,	(char *)&p_sh},
		{"shelltype",	"st",	P_NUM,		(char *)&p_st},
		{"shiftround",	"sr",	P_BOOL,		(char *)&p_sr},
		{"shiftwidth",	"sw",	P_NUM,		(char *)&p_sw},
#ifndef MSDOS
		{"shortname",	"sn",	P_BOOL,		(char *)&p_sn},
#endif
		{"showcmd",		"sc",	P_BOOL,		(char *)&p_sc},
		{"showmatch",	"sm",	P_BOOL,		(char *)&p_sm},
		{"showmode",	"smd",	P_BOOL,		(char *)&p_smd},
		{"sidescroll",	"ss",	P_NUM,		(char *)&p_ss},
		{"slowopen",	"slow",	P_BOOL,		(char *)NULL},
		{"smartindent", "si",	P_BOOL,		(char *)&p_si},
		{"sourceany", NULL,	P_BOOL,		(char *)NULL},
		{"suffixes",	"su",	P_STRING,	(char *)&p_su},
		{"tabstop", 	"ts",	P_NUM,		(char *)&p_ts},
		{"taglength",	"tl",	P_NUM,		(char *)&p_tl},
		{"tags",		NULL,	P_STRING|P_EXPAND,	(char *)&p_tags},
		{"term",		NULL,	P_STRING|P_EXPAND,	(char *)&term_strings.t_name},
		{"terse",		NULL,	P_BOOL,		(char *)NULL},
		{"textauto",	"ta",	P_BOOL,		(char *)&p_ta},
		{"textmode",	"tx",	P_BOOL,		(char *)&p_tx},
		{"textwidth",	"tw",	P_NUM,		(char *)&p_tw},
		{"tildeop", 	"to",	P_BOOL,		(char *)&p_to},
		{"timeout", 	NULL,	P_BOOL,		(char *)&p_timeout},
		{"timeoutlen",	"tm",	P_NUM,		(char *)&p_tm},
		{"ttimeout", 	NULL,	P_BOOL,		(char *)&p_ttimeout},
		{"ttytype",		NULL,	P_STRING,	(char *)NULL},
		{"undolevels",	"ul",	P_NUM,		(char *)&p_ul},
		{"updatecount",	"uc",	P_NUM,		(char *)&p_uc},
		{"updatetime",	"ut",	P_NUM,		(char *)&p_ut},
		{"visualbell",	"vb",	P_BOOL,		(char *)&p_vb},
		{"warn",		NULL,	P_BOOL,		(char *)&p_warn},
		{"wildchar",	"wc", 	P_NUM,		(char *)&p_wc},
		{"window",		NULL, 	P_NUM,		(char *)NULL},
		{"w300",		NULL, 	P_NUM,		(char *)NULL},
		{"w1200",		NULL, 	P_NUM,		(char *)NULL},
		{"w9600",		NULL, 	P_NUM,		(char *)NULL},
		{"wrap",		NULL,	P_BOOL,		(char *)&p_wrap},
		{"wrapscan",	"ws",	P_BOOL,		(char *)&p_ws},
		{"wrapmargin",	"wm",	P_NUM,		(char *)&p_wm},
		{"writeany",	"wa",	P_BOOL,		(char *)&p_wa},
		{"writebackup",	"wb",	P_BOOL,		(char *)&p_wb},
		{"yankendofline", "ye",	P_BOOL,		(char *)&p_ye},

/* terminal output codes */
		{"t_el",		NULL,	P_STRING,	(char *)&term_strings.t_el},
		{"t_il",		NULL,	P_STRING,	(char *)&term_strings.t_il},
		{"t_cil",		NULL,	P_STRING,	(char *)&term_strings.t_cil},
		{"t_dl",		NULL,	P_STRING,	(char *)&term_strings.t_dl},
		{"t_cdl",		NULL,	P_STRING,	(char *)&term_strings.t_cdl},
		{"t_ed",		NULL,	P_STRING,	(char *)&term_strings.t_ed},
		{"t_ci",		NULL,	P_STRING,	(char *)&term_strings.t_ci},
		{"t_cv",		NULL,	P_STRING,	(char *)&term_strings.t_cv},
		{"t_tp",		NULL,	P_STRING,	(char *)&term_strings.t_tp},
		{"t_ti",		NULL,	P_STRING,	(char *)&term_strings.t_ti},
		{"t_cm",		NULL,	P_STRING,	(char *)&term_strings.t_cm},
		{"t_sr",		NULL,	P_STRING,	(char *)&term_strings.t_sr},
		{"t_cri",		NULL,	P_STRING,	(char *)&term_strings.t_cri},
		{"t_vb",		NULL,	P_STRING,	(char *)&term_strings.t_vb},
		{"t_ks",		NULL,	P_STRING,	(char *)&term_strings.t_ks},
		{"t_ke",		NULL,	P_STRING,	(char *)&term_strings.t_ke},
		{"t_ts",		NULL,	P_STRING,	(char *)&term_strings.t_ts},
		{"t_te",		NULL,	P_STRING,	(char *)&term_strings.t_te},

/* terminal key codes */
		{"t_ku",		NULL,	P_STRING,	(char *)&term_strings.t_ku},
		{"t_kd",		NULL,	P_STRING,	(char *)&term_strings.t_kd},
		{"t_kr",		NULL,	P_STRING,	(char *)&term_strings.t_kr},
		{"t_kl",		NULL,	P_STRING,	(char *)&term_strings.t_kl},
		{"t_sku",		NULL,	P_STRING,	(char *)&term_strings.t_sku},
		{"t_skd",		NULL,	P_STRING,	(char *)&term_strings.t_skd},
		{"t_skr",		NULL,	P_STRING,	(char *)&term_strings.t_skr},
		{"t_skl",		NULL,	P_STRING,	(char *)&term_strings.t_skl},
		{"t_f1",		NULL,	P_STRING,	(char *)&term_strings.t_f1},
		{"t_f2",		NULL,	P_STRING,	(char *)&term_strings.t_f2},
		{"t_f3",		NULL,	P_STRING,	(char *)&term_strings.t_f3},
		{"t_f4",		NULL,	P_STRING,	(char *)&term_strings.t_f4},
		{"t_f5",		NULL,	P_STRING,	(char *)&term_strings.t_f5},
		{"t_f6",		NULL,	P_STRING,	(char *)&term_strings.t_f6},
		{"t_f7",		NULL,	P_STRING,	(char *)&term_strings.t_f7},
		{"t_f8",		NULL,	P_STRING,	(char *)&term_strings.t_f8},
		{"t_f9",		NULL,	P_STRING,	(char *)&term_strings.t_f9},
		{"t_f10",		NULL,	P_STRING,	(char *)&term_strings.t_f10},
		{"t_sf1",		NULL,	P_STRING,	(char *)&term_strings.t_sf1},
		{"t_sf2",		NULL,	P_STRING,	(char *)&term_strings.t_sf2},
		{"t_sf3",		NULL,	P_STRING,	(char *)&term_strings.t_sf3},
		{"t_sf4",		NULL,	P_STRING,	(char *)&term_strings.t_sf4},
		{"t_sf5",		NULL,	P_STRING,	(char *)&term_strings.t_sf5},
		{"t_sf6",		NULL,	P_STRING,	(char *)&term_strings.t_sf6},
		{"t_sf7",		NULL,	P_STRING,	(char *)&term_strings.t_sf7},
		{"t_sf8",		NULL,	P_STRING,	(char *)&term_strings.t_sf8},
		{"t_sf9",		NULL,	P_STRING,	(char *)&term_strings.t_sf9},
		{"t_sf10",		NULL,	P_STRING,	(char *)&term_strings.t_sf10},
		{"t_help",		NULL,	P_STRING,	(char *)&term_strings.t_help},
		{"t_undo",		NULL,	P_STRING,	(char *)&term_strings.t_undo},
		{NULL, NULL, 0, NULL}			/* end marker */
};

static void param_expand __ARGS((int, int));
static int findparam __ARGS((char *));
static void	showparams __ARGS((int));
static void showonep __ARGS((struct param *));
static int  istermparam __ARGS((struct param *));

/*
 * Initialize the shell parameter and scroll size.
 */
	void
set_init()
{
	char	*p;
	int		i;

	if ((p = (char *)vimgetenv("SHELL")) != NULL
#ifdef MSDOS
			|| (p = (char *)vimgetenv("COMSPEC")) != NULL
#endif
															)
	{
		p = strsave(p);
		if (p != NULL)		/* we don't want a NULL */
			p_sh = p;
	}

	p_scroll = (Rows >> 1);
	comp_col();

	/*
	 * expand environment variables in some string options
	 */
	for (i = 0; params[i].fullname != NULL; i++)
		param_expand(i, FALSE);
}

/*
 * parse 'arg' for option settings
 * 'arg' may be IObuff, but only when no errors can be present.
 */
	int
doset(arg)
	char		*arg;	/* parameter string (may be written to!) */
{
	register int i;
	char		*s;
	char		*errmsg;
	char		*startarg;
	int			prefix;	/* 0: nothing, 1: "no", 2: "inv" in front of name */
	int 		nextchar;
	int 		len;
	int 		flags;
	long		value;
	long		olduc = p_uc;		/* remember old update count */
	long		oldRows = Rows;		/* remember old Rows */
	int			errcnt = 0;			/* number of errornous entries */
	static long	save_tw = 0;		/* saved options for 'paste' */
	static int	save_ai = 0;
	static int	save_si = 0;
	static int	save_sm = 0;
	static int	save_ru = 0;
	static int	save_ri = 0;
	static int	did_paste = FALSE;	/* when TRUE saved values have been set */
	int			do_wait = FALSE;	/* call wait_return() at the end */
	int			did_show = FALSE;	/* already showed one value */

	if (*arg == NUL)
	{
		showparams(0);
		return 0;
	}

	while (*arg)		/* loop to process all parameters */
	{
		errmsg = NULL;
		startarg = arg;		/* remember for error message */
		if (strncmp(arg, "all", (size_t)3) == 0)
			showparams(1);
		else if (strncmp(arg, "termcap", (size_t)7) == 0)
			showparams(2);
		else
		{
			prefix = 1;
			if (strncmp(arg, "no", (size_t)2) == 0)
			{
				prefix = 0;
				arg += 2;
			}
			else if (strncmp(arg, "inv", (size_t)3) == 0)
			{
				prefix = 2;
				arg += 3;
			}
				/* find end of name */
			for (len = 0; isalnum(arg[len]) || arg[len] == '_'; ++len)
				;
			nextchar = arg[len];
			arg[len] = 0;								/* name ends with 0 */
			i = findparam(arg);
			arg[len] = nextchar;						/* restore nextchar */

			if (i == -1)		/* found a mismatch: skip the rest */
			{
				errmsg = "Unknown option";
				goto skip;
			}

			if (!params[i].var)			/* hidden option */
				goto skip;

			flags = params[i].flags;
			/*
			 * allow '=' and ':' as MSDOS command.com allows only one
			 * '=' character per "set" command line. grrr. (jw)
			 */
			if (nextchar == '?' || 
			    (prefix == 1 && nextchar != '=' &&
				 nextchar != ':' && !(flags & P_BOOL)))
			{										/* print value */
				if (did_show)
				{
					outchar('\n');			/* cursor below last one */
					do_wait = TRUE;			/* have to call wait_return() */
				}
				else
				{
					gotocmdline(TRUE, NUL);	/* cursor at status line */
					did_show = TRUE;		/* remember that we did a line */
				}
				showonep(&params[i]);

					/* check if the message is too long to fit in one line */
				if ((flags & P_STRING) && strlen(params[i].fullname) +
							(*(char **)(params[i].var) == NULL ? 0 :
							strsize(*(char **)(params[i].var))) + 1 >= (unsigned)sc_col)
					do_wait = TRUE;
			}
			else
			{
				if (nextchar != NUL && strchr("=: \t", nextchar) == NULL)
				{
					errmsg = e_invarg;
					goto skip;
				}
				else if (flags & P_BOOL)					/* boolean */
				{
						if (nextchar == '=' || nextchar == ':')
						{
							errmsg = e_invarg;
							goto skip;
						}
						/*
						 * in secure mode, setting of the secure option is not allowed
						 */
						if (secure && (int *)params[i].var == &p_secure)
						{
							errmsg = "not allowed here";
							goto skip;
						}
						if (prefix == 2)
							*(int *)(params[i].var) ^= 1;	/* invert it */
						else
							*(int *)(params[i].var) = prefix;
							/* handle compatbile option here */
						if ((int *)params[i].var == &p_cp && p_cp)
						{
							p_bs = 0;		/* normal backspace */
							p_bk = 0;		/* no backup file */
#ifdef DIGRAPHS
							p_dg = 0;		/* no digraphs */
#endif /* DIGRAPHS */
							p_ek = 0;		/* no ESC keys in insert mode */
							p_et = 0;		/* no expansion of tabs */
							p_hi = 0;		/* no history */
							p_im = 0;		/* do not start in insert mode */
							p_js = 1;		/* insert 2 spaces after period */
							p_ml = 0;		/* no modelines */
							p_ru = 0;		/* no ruler */
							p_ri = 0;		/* no reverse insert */
							p_sj = 1;		/* no scrolljump */
							p_sr = 0;		/* do not round indent to shiftwidth */
							p_sc = 0;		/* no showcommand */
							p_smd = 0;		/* no showmode */
							p_si = 0;		/* no smartindent */
							p_ta = 0;		/* no automatic textmode detection */
							p_tw = 0;		/* no automatic line wrap */
							p_to = 0;		/* no tilde operator */
							p_ttimeout = 0;	/* no terminal timeout */
							p_ul = 0;		/* no multilevel undo */
							p_uc = 0;		/* no autoscript file */
							p_wb = 0;		/* no backup file */
							if (p_wc == TAB)
								p_wc = Ctrl('E');	/* normal use for TAB */
							p_ye = 0;		/* no yank to end of line */
						}
						if ((int *)params[i].var == &p_bin && p_bin)	/* handle bin */
						{
							p_tw = 0;		/* no automatic line wrap */
							p_tx = 0;		/* no text mode */
							p_ta = 0;		/* no text auto */
							p_ml = 0;		/* no modelines */
							p_et = 0;		/* no expandtab */
						}
						if ((int *)params[i].var == &p_paste)	/* handle paste here */
						{
							if (p_paste)
							{
								save_tw = p_tw;		/* save current values */
								save_ai = p_ai;
								save_si = p_si;
								save_sm = p_sm;
								save_ru = p_ru;
								save_ri = p_ri;
								p_tw = 0;			/* textwidth is 0 */
								p_ai = 0;			/* no auto-indent */
								p_si = 0;			/* no smart-indent */
								p_sm = 0;			/* no showmatch */
								p_ru = 0;			/* no ruler */
								p_ri = 0;			/* no reverse insert */
								did_paste = TRUE;
							}
							else if (did_paste)		/* restore old values */
							{
								p_tw = save_tw;
								p_ai = save_ai;
								p_si = save_si;
								p_sm = save_sm;
								p_ru = save_ru;
								p_ri = save_ri;
							}
						}
				}
				else								/* numeric or string */
				{
					if ((nextchar != '=' && nextchar != ':') || prefix != 1)
					{
						errmsg = e_invarg;
						goto skip;
					}
					if (flags & P_NUM)				/* numeric */
					{
						value = atol(arg + len + 1);
							/* wrapmargin is translated into textwidth */
						if ((long *)params[i].var == &p_wm)
						{
							if (value == 0)		/* switch it off */
								p_tw = 0;
							else
							{
								if (value >= (int)Columns)
									value = (int)Columns - 1;
								p_tw = Columns - value;
							}
						}
						*(long *)(params[i].var) = value;
					}
					else							/* string */
					{
						arg += len + 1;		/* jump to after the '=' */
						prefix = *arg;		/* remember first char of arg */
						s = alloc((unsigned)(strlen(arg) + 1)); /* get a bit too much */
						if (s == NULL)
							break;
						if (flags & P_CHANGED)
							free(*(char **)(params[i].var));
						*(char **)(params[i].var) = s;
									/* copy the string */
						while (*arg && *arg != ' ')
						{
							if (*arg == '\\' && *(arg + 1)) /* skip over escaped chars */
									++arg;
							*s++ = *arg++;
						}
						*s = NUL;
						if (prefix == '$')
							param_expand(i, TRUE);	/* expand environment variables */
						/*
						 * options that need some action
						 * to perform when changed (jw)
						 */
						if (params[i].var == (char *)&term_strings.t_name)
							set_term(term_strings.t_name);
						else if (istermparam(&params[i]))
						{
							ttest(FALSE);
							if (params[i].var == (char *)&term_strings.t_tp)
							{
								outstr(T_TP);
								updateScreen(CLEAR);
							}
						}
					}
				}
				params[i].flags |= P_CHANGED;
			}
		}

skip:
		/*
		 * Check the bounds for numeric parameters here
		 */
		if (Rows < 2)
		{
			Rows = 2;
			errmsg = "Need at least 2 lines";
		}
		/*
		 * If the screenheight has been changed, assume it is the physical
		 * screenheight, set Rows_max.
		 */
		if (oldRows != Rows)
		{
			Rows_max = Rows;
#ifdef MSDOS
			set_window();		/* active window may have changed */
#endif
		}

		if (p_ts <= 0)
		{
			errmsg = e_positive;
			p_ts = 8;
		}
		if (p_tm < 0)
		{
			errmsg = e_positive;
			p_tm = 0;
		}
		if (p_scroll <= 0 || p_scroll > Rows)
		{
			if (p_scroll != 0)
				errmsg = e_scroll;
			p_scroll = Rows >> 1;
		}
		if (p_report < 0)
		{
			errmsg = e_positive;
			p_report = 1;
		}
		if (p_sj < 0 || p_sj >= Rows)
		{
			errmsg = e_scroll;
			p_sj = 1;
		}
		if (p_uc < 0)
		{
			errmsg = e_positive;
			p_uc = 100;
		}
		if (p_ut < 0)
		{
			errmsg = e_positive;
			p_ut = 2000;
		}
		if (p_ss < 0)
		{
			errmsg = e_positive;
			p_ss = 0;
		}
		if (errmsg)
		{
			strcpy(IObuff, errmsg);
			strcat(IObuff, ": ");
			s = IObuff + strlen(IObuff);
			while (*startarg && !isspace(*startarg))
				*s++ = *startarg++;
			*s = NUL;
			emsg(IObuff);
			arg = startarg;		/* skip to next argument */
			++errcnt;			/* count number of errors */
		}
		skiptospace(&arg);				/* skip to next white space */
		skipspace(&arg);				/* skip spaces */
	}

	if (p_uc == 0 && olduc != 0)		/* p_uc changed from on to off */
		stopscript();
	if (p_uc > 0 && olduc == 0)			/* p_uc changed from off to on */
		startscript();
	comp_col();

	/*
	 * Update the screen in case we changed something like "tabstop" or
	 * "lines" or "list" that will change its appearance.
	 * If we messed up the screen by showing more than one line of param
	 * values call wait_return(), which will also update the screen..
	 */
	if (do_wait)
	{
		outchar('\n');
		wait_return(TRUE);
	}
	else
		updateScreen(NOT_VALID);
	return errcnt;
}

/*
 * expand environment variable at the start of some string options
 */
	static void
param_expand(i, dofree)
	int		i;
	int		dofree;
{
	char *p;

	if ((params[i].flags & P_EXPAND) && (p = *(char **)(params[i].var)) != NULL && *p == '$')
	{
		expand_env(*(char **)(params[i].var), IObuff, IOSIZE);
		p = strsave(IObuff);
		if (p)
		{
			if (dofree)
				free(*(char **)(params[i].var));
			*(char **)(params[i].var) = p;
		}
	}
}

/*
 * find index for option 'arg'
 * return -1 if not found
 */
	static int
findparam(arg)
	char *arg;
{
	int		i;
	char	*s;

	for (i = 0; (s = params[i].fullname) != NULL; i++)
	{
		if (strcmp(arg, s) == 0) /* match full name */
			break;
	}
	if (s == NULL)
	{
		for (i = 0; params[i].fullname != NULL; i++)
		{
			s = params[i].shortname;
			if (s != NULL && strcmp(arg, s) == 0) /* match short name */
				break;
			s = NULL;
		}
	}
	if (s == NULL)
		i = -1;
	return i;
}

/*
 * mark option 'arg' changed
 */
	void
paramchanged(arg)
	char *arg;
{
	int i;

	i = findparam(arg);
	if (i >= 0)
		params[i].flags |= P_CHANGED;
}

/*
 * if 'all' == 0: show changed parameters
 * if 'all' == 1: show all normal parameters
 * if 'all' == 2: show all terminal parameters
 */
	static void
showparams(all)
	int			all;
{
	struct param   *p;
	int				col = 0;
	int				inc;
	int				isterm;

	gotocmdline(TRUE, NUL);
	outstrn("Parameters:\n");

#ifdef AMIGA
	settmode(0);				/* set cooked mode so output can be halted */
#endif
	for (p = &params[0]; p->fullname != NULL; p++)
	{
		isterm = istermparam(p);
		if (p->var && (
			(all == 2 && isterm) ||
			(all == 1 && !isterm) ||
			(all == 0 && (p->flags & P_CHANGED))))
		{
			if ((p->flags & P_STRING) && *(char **)(p->var) != NULL)
			{
				inc = strlen(p->fullname) + strsize(*(char **)(p->var)) + 1;
				if (inc < 18)
					inc = 18;
			}
			else
				inc = 18;
			if (col + inc >= Columns)
			{
				col = 0;
				if (Rows != Rows_max)
					windgoto((int)Rows_max - 1, 0);
				outchar('\n');				/* scroll screen one line up */
			}
			windgoto((int)Rows - 1, col); /* make columns */
			showonep(p);
			col += inc;
			col += 19 - col % 19;
			flushbuf();
		}
	}

	if (col)
		outchar('\n');
#ifdef AMIGA
	settmode(1);
#endif
	wait_return(TRUE);
}

/*
 * showonep: show the value of one option
 * must not be called with a hidden option!
 */
	static void
showonep(p)
		struct param *p;
{
	char			buf[64];

	if ((p->flags & P_BOOL) && !*(int *)(p->var))
		outstrn("no");
	outstrn(p->fullname);
	if (!(p->flags & P_BOOL))
	{
		outchar('=');
		if (p->flags & P_NUM)
		{
			sprintf(buf, "%ld", *(long *)(p->var));
			outstrn(buf);
		}
		else if (*(char **)(p->var) != NULL)
			outtrans(*(char **)(p->var), -1);
	}
}

/*
 * Write modified parameters as set command to a file.
 * Return 1 on error.
 */
	int
makeset(fd)
	FILE *fd;
{
	struct param	*p;
	char			*s;
	int				e;

	for (p = &params[0]; p->fullname != NULL; p++)
		if ((p->flags & P_CHANGED) && p->var)
		{
			if (p->flags & P_BOOL)
				fprintf(fd, "set %s%s", *(int *)(p->var) ? "" : "no", p->fullname);
			else if (p->flags & P_NUM)
				fprintf(fd, "set %s=%ld", p->fullname, *(long *)(p->var));
			else
			{
				fprintf(fd, "set %s=", p->fullname);
				s = *(char **)(p->var);
					/* some characters hav to be escaped with CTRL-V or backslash */
				if (s != NULL && putescstr(fd, s, TRUE) < 0)
					return 1;
			}
#ifdef MSDOS
			putc('\r', fd);
#endif
				/*
				 * Only check error for this putc, should catch at least
				 * the "disk full" situation.
				 */
			e = putc('\n', fd);
			if (e < 0)
				return 1;
		}
	return 0;
}

/*
 * Clear all the terminal parameters.
 * If the parameter has been changed, free the allocated memory.
 * Reset the "changed" flag, so the new value will not be freed.
 */
	void
clear_termparam()
{
	struct param   *p;

	for (p = &params[0]; p->fullname != NULL; p++)
		if (istermparam(p))			/* terminal parameters must never be hidden */
		{
			if (p->flags & P_CHANGED)
				free(*(char **)(p->var));
			*(char **)(p->var) = NULL;
			p->flags &= ~P_CHANGED;
		}
}

	static int
istermparam(p)
	struct param *p;
{
	return (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 */

#define COL_SHOWCMD 10		/* columns needed by shown command */
#define COL_RULER 17		/* columns needed by ruler */

	void
comp_col()
{
	sc_col = 0;
	ru_col = 0;
	if (p_ru)
		ru_col = sc_col = COL_RULER + 1;
	if (p_sc)
	{
		sc_col += COL_SHOWCMD;
		if (!p_ru)
			++sc_col;
	}
	sc_col = Columns - sc_col;
	ru_col = Columns - ru_col;
}
