/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
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
	short 		flags;			/* see below */
	char_u		*var;			/* pointer to variable */
};

/*
 * Flags
 * Note: Don't use P_STRING and P_IND at the same time
 */
#define P_BOOL			0x01	/* the parameter is boolean */
#define P_NUM			0x02	/* the parameter is numeric */
#define P_STRING		0x04	/* the parameter is a string */
#define P_CHANGED		0x08	/* the parameter has been changed */
#define P_EXPAND		0x10	/* environment expansion */
#define P_IND			0x20	/* indirect, is in curwin or curbuf */

/*
 * The options that are in curwin or curbuf have P_IND set and a var field
 * that contains one of the values below. The values are odd to make it very
 * unlikely that another variable has the same value.
 * Note: P_EXPAND and P_IND can never be used at the same time.
 * Note: P_IND cannot be used for a terminal option.
 */
#define PV_LIST		1
#define PV_NU		3
#define PV_SCROLL	5
#define PV_WRAP		7

#define PV_AI		11
#define PV_BIN		13
#define PV_EOL		14
#define PV_ET		15
#define PV_ML		17
#define PV_RO		19
#define PV_SI		21
#define PV_SN		23
#define PV_SW		25
#define PV_TS		27
#define PV_TW		29
#define PV_TX		31
#define PV_WM		33

/*
 * The param structure is initialized here.
 * The order of the parameters should be alfabetic
 * The parameters with a NULL variable are 'hidden': a set command for
 * them is ignored and they are not printed.
 */
static struct param params[] =
{
		{"autoindent",	"ai",	P_BOOL|P_IND,		(char_u *)PV_AI},
		{"autoprint",	"ap",	P_BOOL,				(char_u *)NULL},
		{"autowrite",	"aw",	P_BOOL,				(char_u *)&p_aw},
		{"backspace",	"bs",	P_NUM,				(char_u *)&p_bs},
		{"backup",		"bk",	P_BOOL,				(char_u *)&p_bk},
#ifdef UNIX
 		{"backupdir",	"bdir",	P_STRING|P_EXPAND,	(char_u *)&p_bdir},
#endif
		{"beautify",	"bf",	P_BOOL,				(char_u *)NULL},
		{"binary",		"bin",	P_BOOL|P_IND,		(char_u *)PV_BIN},
#ifdef MSDOS
		{"bioskey",		"biosk",P_BOOL,				(char_u *)&p_biosk},
#endif
		{"cmdheight",	"ch",	P_NUM,				(char_u *)&p_ch},
		{"columns",		"co",	P_NUM,				(char_u *)&Columns},
		{"compatible",	"cp",	P_BOOL,				(char_u *)&p_cp},
#ifdef DIGRAPHS
		{"digraph",		"dg",	P_BOOL,				(char_u *)&p_dg},
#endif /* DIGRAPHS */
 		{"directory",	"dir",	P_STRING|P_EXPAND,	(char_u *)&p_dir},
		{"edcompatible",NULL,	P_BOOL,				(char_u *)&p_ed},
		{"endofline",	"eol",	P_BOOL|P_IND,		(char_u *)PV_EOL},
		{"equalalways",	"ea",  	P_BOOL,				(char_u *)&p_ea},
		{"equalprg",	"ep",  	P_STRING|P_EXPAND,	(char_u *)&p_ep},
		{"errorbells",	"eb",	P_BOOL,				(char_u *)&p_eb},
		{"errorfile",	"ef",  	P_STRING|P_EXPAND,	(char_u *)&p_ef},
		{"errorformat",	"efm", 	P_STRING,			(char_u *)&p_efm},
		{"esckeys",		"ek",	P_BOOL,				(char_u *)&p_ek},
		{"expandtab",	"et",	P_BOOL|P_IND,		(char_u *)PV_ET},
		{"exrc",		NULL,	P_BOOL,				(char_u *)&p_exrc},
		{"formatprg",	"fp",  	P_STRING|P_EXPAND,	(char_u *)&p_fp},
		{"gdefault",	"gd",	P_BOOL,				(char_u *)&p_gd},
		{"graphic",		"gr",	P_BOOL,				(char_u *)&p_gr},
		{"hardtabs",	"ht",	P_NUM,				(char_u *)NULL},
		{"helpfile",	"hf",  	P_STRING|P_EXPAND,	(char_u *)&p_hf},
		{"hidden",		"hid",	P_BOOL,				(char_u *)&p_hid},
		{"highlight",	"hl",	P_STRING,			(char_u *)&p_hl},
		{"history", 	"hi", 	P_NUM,				(char_u *)&p_hi},
		{"icon",	 	NULL,	P_BOOL,				(char_u *)&p_icon},
		{"ignorecase",	"ic",	P_BOOL,				(char_u *)&p_ic},
		{"insertmode",	"im",	P_BOOL,				(char_u *)&p_im},
		{"joinspaces", 	"js",	P_BOOL,				(char_u *)&p_js},
		{"keywordprg",	"kp",  	P_STRING|P_EXPAND,	(char_u *)&p_kp},
		{"laststatus",	"ls", 	P_NUM,				(char_u *)&p_ls},
		{"lines",		NULL, 	P_NUM,				(char_u *)&Rows},
		{"lisp",		NULL,	P_BOOL,				(char_u *)NULL},
		{"list",		NULL,	P_BOOL|P_IND,		(char_u *)PV_LIST},
		{"magic",		NULL,	P_BOOL,				(char_u *)&p_magic},
		{"makeprg",		"mp",  	P_STRING|P_EXPAND,	(char_u *)&p_mp},
		{"maxmem",		"mm",	P_NUM,				(char_u *)&p_mm},
		{"maxmemtot",	"mmt",	P_NUM,				(char_u *)&p_mmt},
		{"mesg",		NULL,	P_BOOL,				(char_u *)NULL},
		{"modeline",	"ml",	P_BOOL|P_IND,		(char_u *)PV_ML},
		{"modelines",	"mls",	P_NUM,				(char_u *)&p_mls},
		{"more",		NULL,	P_BOOL,				(char_u *)&p_more},
		{"nobuf",		"nb",	P_BOOL,				(char_u *)&p_nb},
		{"number",		"nu",	P_BOOL|P_IND,		(char_u *)PV_NU},
		{"open",		NULL,	P_BOOL,				(char_u *)NULL},
		{"optimize",	"opt",	P_BOOL,				(char_u *)NULL},
		{"paragraphs",	"para",	P_STRING,			(char_u *)&p_para},
		{"paste",		NULL,	P_BOOL,				(char_u *)&p_paste},
		{"patchmode",   "pm",   P_STRING,			(char_u *)&p_pm},
		{"path",		"pa",  	P_STRING|P_EXPAND,	(char_u *)&p_path},
		{"prompt",		NULL,	P_BOOL,				(char_u *)NULL},
		{"readonly",	"ro",	P_BOOL|P_IND,		(char_u *)PV_RO},
		{"redraw",		NULL,	P_BOOL,				(char_u *)NULL},
		{"remap",		NULL,	P_BOOL,				(char_u *)&p_remap},
		{"report",		NULL,	P_NUM,				(char_u *)&p_report},
		{"revins",		"ri",	P_BOOL,				(char_u *)&p_ri},
		{"ruler",		"ru",	P_BOOL,				(char_u *)&p_ru},
		{"scroll",		NULL, 	P_NUM|P_IND,		(char_u *)PV_SCROLL},
		{"scrolljump",	"sj", 	P_NUM,				(char_u *)&p_sj},
		{"sections",	"sect",	P_STRING,			(char_u *)&p_sections},
		{"secure",		NULL,	P_BOOL,				(char_u *)&p_secure},
		{"shell",		"sh",	P_STRING|P_EXPAND,	(char_u *)&p_sh},
		{"shellpipe",	"sp",	P_STRING,			(char_u *)&p_sp},
		{"shelltype",	"st",	P_NUM,				(char_u *)&p_st},
		{"shiftround",	"sr",	P_BOOL,				(char_u *)&p_sr},
		{"shiftwidth",	"sw",	P_NUM|P_IND,		(char_u *)PV_SW},
#ifndef MSDOS
		{"shortname",	"sn",	P_BOOL|P_IND,		(char_u *)PV_SN},
#endif
		{"showcmd",		"sc",	P_BOOL,				(char_u *)&p_sc},
		{"showmatch",	"sm",	P_BOOL,				(char_u *)&p_sm},
		{"showmode",	"smd",	P_BOOL,				(char_u *)&p_smd},
		{"sidescroll",	"ss",	P_NUM,				(char_u *)&p_ss},
		{"slowopen",	"slow",	P_BOOL,				(char_u *)NULL},
		{"smartindent", "si",	P_BOOL|P_IND,		(char_u *)PV_SI},
		{"smarttab",	"sta",	P_BOOL,				(char_u *)&p_sta},
		{"sourceany",	NULL,	P_BOOL,				(char_u *)NULL},
		{"splitbelow",	"sb",	P_BOOL,				(char_u *)&p_sb},
		{"suffixes",	"su",	P_STRING,			(char_u *)&p_su},
		{"tabstop", 	"ts",	P_NUM|P_IND,		(char_u *)PV_TS},
		{"taglength",	"tl",	P_NUM,				(char_u *)&p_tl},
		{"tagrelative",	"tr",	P_BOOL,				(char_u *)&p_tr},
		{"tags",		NULL,	P_STRING|P_EXPAND,	(char_u *)&p_tags},
		{"term",		NULL,	P_STRING|P_EXPAND,	(char_u *)&term_strings.t_name},
		{"terse",		NULL,	P_BOOL,				(char_u *)&p_terse},
		{"textauto",	"ta",	P_BOOL,				(char_u *)&p_ta},
		{"textmode",	"tx",	P_BOOL|P_IND,		(char_u *)PV_TX},
		{"textwidth",	"tw",	P_NUM|P_IND,		(char_u *)PV_TW},
		{"tildeop", 	"to",	P_BOOL,				(char_u *)&p_to},
		{"timeout", 	NULL,	P_BOOL,				(char_u *)&p_timeout},
		{"timeoutlen",	"tm",	P_NUM,				(char_u *)&p_tm},
		{"title",	 	NULL,	P_BOOL,				(char_u *)&p_title},
		{"ttimeout", 	NULL,	P_BOOL,				(char_u *)&p_ttimeout},
		{"ttyfast", 	"tf",	P_BOOL,				(char_u *)&p_tf},
		{"ttytype",		NULL,	P_STRING,			(char_u *)NULL},
		{"undolevels",	"ul",	P_NUM,				(char_u *)&p_ul},
		{"updatecount",	"uc",	P_NUM,				(char_u *)&p_uc},
		{"updatetime",	"ut",	P_NUM,				(char_u *)&p_ut},
		{"visualbell",	"vb",	P_BOOL,				(char_u *)&p_vb},
		{"w300",		NULL, 	P_NUM,				(char_u *)NULL},
		{"w1200",		NULL, 	P_NUM,				(char_u *)NULL},
		{"w9600",		NULL, 	P_NUM,				(char_u *)NULL},
		{"warn",		NULL,	P_BOOL,				(char_u *)&p_warn},
		{"weirdinvert",	"wi",	P_BOOL,				(char_u *)&p_wi},
		{"whichwrap",	"ww",	P_NUM,				(char_u *)&p_ww},
		{"wildchar",	"wc", 	P_NUM,				(char_u *)&p_wc},
		{"window",		NULL, 	P_NUM,				(char_u *)NULL},
		{"winheight",	"wh",	P_NUM,				(char_u *)&p_wh},
		{"wrap",		NULL,	P_BOOL|P_IND,		(char_u *)PV_WRAP},
		{"wrapmargin",	"wm",	P_NUM|P_IND,		(char_u *)PV_WM},
		{"wrapscan",	"ws",	P_BOOL,				(char_u *)&p_ws},
		{"writeany",	"wa",	P_BOOL,				(char_u *)&p_wa},
		{"writebackup",	"wb",	P_BOOL,				(char_u *)&p_wb},
		{"yankendofline", "ye",	P_BOOL,				(char_u *)&p_ye},

/* terminal output codes */
		{"t_cdl",		NULL,	P_STRING,	(char_u *)&term_strings.t_cdl},
		{"t_ci",		NULL,	P_STRING,	(char_u *)&term_strings.t_ci},
		{"t_cil",		NULL,	P_STRING,	(char_u *)&term_strings.t_cil},
		{"t_cm",		NULL,	P_STRING,	(char_u *)&term_strings.t_cm},
		{"t_cri",		NULL,	P_STRING,	(char_u *)&term_strings.t_cri},
		{"t_cv",		NULL,	P_STRING,	(char_u *)&term_strings.t_cv},
		{"t_cvv",		NULL,	P_STRING,	(char_u *)&term_strings.t_cvv},
		{"t_dl",		NULL,	P_STRING,	(char_u *)&term_strings.t_dl},
		{"t_cs",		NULL,	P_STRING,	(char_u *)&term_strings.t_cs},
		{"t_ed",		NULL,	P_STRING,	(char_u *)&term_strings.t_ed},
		{"t_el",		NULL,	P_STRING,	(char_u *)&term_strings.t_el},
		{"t_il",		NULL,	P_STRING,	(char_u *)&term_strings.t_il},
		{"t_ke",		NULL,	P_STRING,	(char_u *)&term_strings.t_ke},
		{"t_ks",		NULL,	P_STRING,	(char_u *)&term_strings.t_ks},
		{"t_ms",		NULL,	P_STRING,	(char_u *)&term_strings.t_ms},
		{"t_se",		NULL,	P_STRING,	(char_u *)&term_strings.t_se},
		{"t_so",		NULL,	P_STRING,	(char_u *)&term_strings.t_so},
		{"t_ti",		NULL,	P_STRING,	(char_u *)&term_strings.t_ti},
		{"t_tb",		NULL,	P_STRING,	(char_u *)&term_strings.t_tb},
		{"t_tp",		NULL,	P_STRING,	(char_u *)&term_strings.t_tp},
		{"t_sr",		NULL,	P_STRING,	(char_u *)&term_strings.t_sr},
		{"t_te",		NULL,	P_STRING,	(char_u *)&term_strings.t_te},
		{"t_ts",		NULL,	P_STRING,	(char_u *)&term_strings.t_ts},
		{"t_vb",		NULL,	P_STRING,	(char_u *)&term_strings.t_vb},

/* terminal key codes */
		{"t_ku",		NULL,	P_STRING,	(char_u *)&term_strings.t_ku},
		{"t_kd",		NULL,	P_STRING,	(char_u *)&term_strings.t_kd},
		{"t_kr",		NULL,	P_STRING,	(char_u *)&term_strings.t_kr},
		{"t_kl",		NULL,	P_STRING,	(char_u *)&term_strings.t_kl},
		{"t_sku",		NULL,	P_STRING,	(char_u *)&term_strings.t_sku},
		{"t_skd",		NULL,	P_STRING,	(char_u *)&term_strings.t_skd},
		{"t_skr",		NULL,	P_STRING,	(char_u *)&term_strings.t_skr},
		{"t_skl",		NULL,	P_STRING,	(char_u *)&term_strings.t_skl},
		{"t_f1",		NULL,	P_STRING,	(char_u *)&term_strings.t_f1},
		{"t_f2",		NULL,	P_STRING,	(char_u *)&term_strings.t_f2},
		{"t_f3",		NULL,	P_STRING,	(char_u *)&term_strings.t_f3},
		{"t_f4",		NULL,	P_STRING,	(char_u *)&term_strings.t_f4},
		{"t_f5",		NULL,	P_STRING,	(char_u *)&term_strings.t_f5},
		{"t_f6",		NULL,	P_STRING,	(char_u *)&term_strings.t_f6},
		{"t_f7",		NULL,	P_STRING,	(char_u *)&term_strings.t_f7},
		{"t_f8",		NULL,	P_STRING,	(char_u *)&term_strings.t_f8},
		{"t_f9",		NULL,	P_STRING,	(char_u *)&term_strings.t_f9},
		{"t_f10",		NULL,	P_STRING,	(char_u *)&term_strings.t_f10},
		{"t_sf1",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf1},
		{"t_sf2",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf2},
		{"t_sf3",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf3},
		{"t_sf4",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf4},
		{"t_sf5",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf5},
		{"t_sf6",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf6},
		{"t_sf7",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf7},
		{"t_sf8",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf8},
		{"t_sf9",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf9},
		{"t_sf10",		NULL,	P_STRING,	(char_u *)&term_strings.t_sf10},
		{"t_help",		NULL,	P_STRING,	(char_u *)&term_strings.t_help},
		{"t_undo",		NULL,	P_STRING,	(char_u *)&term_strings.t_undo},
		{"t_csc",		NULL,	P_STRING,	(char_u *)&term_strings.t_csc},
		{NULL, NULL, 0, NULL}			/* end marker */
};

#define PARAM_COUNT (sizeof(params) / sizeof(struct param))

static void param_expand __ARGS((int, int));
static int findparam __ARGS((char_u *));
static void	showparams __ARGS((int));
static void showonep __ARGS((struct param *));
static int  istermparam __ARGS((struct param *));
static char_u *get_varp __ARGS((struct param *));

/*
 * Initialize the shell parameter and scroll size.
 */
	void
set_init()
{
	char_u	*p;
	int		i;

	if ((p = vimgetenv((char_u *)"SHELL")) != NULL
#ifdef MSDOS
			|| (p = vimgetenv((char_u *)"COMSPEC")) != NULL
#endif
															)
	{
		p = strsave(p);
		if (p != NULL)		/* we don't want a NULL */
			p_sh = p;
	}

#ifdef UNIX
	/*
	 * Default for p_sp is "| tee".
	 * For known shells it is changed here to include stderr.
	 */
	p = gettail(p_sh);
	if (	 STRCMP(p, "csh") == 0 ||
			 STRCMP(p, "tcsh") == 0 ||
			 STRCMP(p, "zsh") == 0)
		p_sp = (char_u *)"|& tee";
	else if (STRCMP(p, "sh") == 0 ||
			 STRCMP(p, "ksh") == 0 ||
			 STRCMP(p, "bash") == 0)
		p_sp = (char_u *)"2>&1| tee";
#endif

	curwin->w_p_scroll = (Rows >> 1);
	comp_col();

/*
 * set the options in curwin and curbuf that are non-zero
 */
	curwin->w_p_wrap = TRUE;
	curbuf->b_p_ml = TRUE;
	curbuf->b_p_sw = 8;
	curbuf->b_p_ts = 8;
#ifdef MSDOS
	curbuf->b_p_tx = TRUE;		/* texmode is default for MSDOS */
#endif

	/*
	 * expand environment variables in some string options
	 */
	for (i = 0; params[i].fullname != NULL; i++)
		param_expand(i, FALSE);
	
	/*
	 * may adjust p_mmt and p_mm for available memory
	 */
	if (p_mmt == 0)
	{
		p_mmt = (mch_avail_mem(FALSE) >> 11);
		if (p_mm > p_mmt)
			p_mm = p_mmt;
	}
}

/*
 * parse 'arg' for option settings
 * 'arg' may be IObuff, but only when no errors can be present and parameter
 * does not need to be expanded with param_expand().
 *
 * return FAIL if errors are detected, OK otherwise
 */
	int
doset(arg)
	char_u		*arg;	/* parameter string (may be written to!) */
{
	register int i;
	char_u		*s;
	char_u		*errmsg;
	char_u		*startarg;
	int			prefix;	/* 0: nothing, 1: "no", 2: "inv" in front of name */
	int 		nextchar;
	int 		len;
	int 		flags;				/* flags for current option */
	char_u		*varp;				/* pointer to variable for current option */
	int			errcnt = 0;			/* number of errornous entries */
	long		oldRows = Rows;		/* remember old Rows */
	int			oldpaste = p_paste;	/* remember old paste option */
	long		oldch = p_ch;		/* remember old command line height */
	int			oldea = p_ea;		/* remember old 'equalalways' */
	long		olduc = p_uc;		/* remember old 'updatecount' */
	static int	save_sm = 0;		/* saved options for 'paste' */
	static int	save_ru = 0;
	static int	save_ri = 0;
	int			did_show = FALSE;	/* already showed one value */
	WIN			*wp;

	if (*arg == NUL)
	{
		showparams(0);
		return OK;
	}
	if (STRNCMP(arg, "all", (size_t)3) == 0)
	{
		showparams(1);
		return OK;
	}
	if (STRNCMP(arg, "termcap", (size_t)7) == 0)
	{
		showparams(2);
		return OK;
	}

	while (*arg)		/* loop to process all parameters */
	{
		errmsg = NULL;
		startarg = arg;		/* remember for error message */
		prefix = 1;
		if (STRNCMP(arg, "no", (size_t)2) == 0)
		{
			prefix = 0;
			arg += 2;
		}
		else if (STRNCMP(arg, "inv", (size_t)3) == 0)
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
			errmsg = (char_u *)"Unknown option";
			goto skip;
		}

		if (!params[i].var)			/* hidden option */
			goto skip;

		flags = params[i].flags;
		varp = get_varp(&(params[i]));

		/*
		 * allow '=' and ':' as MSDOS command.com allows only one
		 * '=' character per "set" command line. grrr. (jw)
		 */
		if (nextchar == '?' || 
			(prefix == 1 && nextchar != '=' &&
			 nextchar != ':' && !(flags & P_BOOL)))
		{										/* print value */
			if (did_show)
				msg_outchar('\n');		/* cursor below last one */
			else
			{
				gotocmdline(TRUE, NUL);	/* cursor at status line */
				did_show = TRUE;		/* remember that we did a line */
			}
			showonep(&params[i]);
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
				if (secure && (int *)varp == &p_secure)
				{
					errmsg = (char_u *)"not allowed here";
					goto skip;
				}
				if (prefix == 2)
					*(int *)(varp) ^= 1;	/* invert it */
				else
					*(int *)(varp) = prefix;
					/* handle compatible option here */
				if ((int *)varp == &p_cp && p_cp)
				{
					p_bs = 0;		/* normal backspace */
					p_ww = 0;		/* backspace and space do not wrap */
					p_bk = 0;		/* no backup file */
#ifdef DIGRAPHS
					p_dg = 0;		/* no digraphs */
#endif /* DIGRAPHS */
					p_ek = 0;		/* no ESC keys in insert mode */
					curbuf->b_p_et = 0;		/* no expansion of tabs */
					p_gd = 0;		/* /g is not default for :s */
					p_hi = 0; 		/* no history */
					p_im = 0;		/* do not start in insert mode */
					p_js = 1;		/* insert 2 spaces after period */
					curbuf->b_p_ml = 0;		/* no modelines */
					p_more = 0;		/* no -- more -- for listings */
					p_ru = 0;		/* no ruler */
					p_ri = 0;		/* no reverse insert */
					p_sj = 1;		/* no scrolljump */
					p_sr = 0;		/* do not round indent to shiftwidth */
					p_sc = 0;		/* no showcommand */
					p_smd = 0;		/* no showmode */
					curbuf->b_p_si = 0;		/* no smartindent */
					p_sta = 0;		/* no smarttab */
					p_ta = 0;		/* no automatic textmode detection */
					curbuf->b_p_tw = 0;		/* no automatic line wrap */
					p_to = 0;		/* no tilde operator */
					p_ttimeout = 0;	/* no terminal timeout */
					p_tr = 0;		/* tag file names not relative */
					p_ul = 0;		/* no multilevel undo */
					p_uc = 0;		/* no autoscript file */
					p_wb = 0;		/* no backup file */
					if (p_wc == TAB)
						p_wc = Ctrl('E');	/* normal use for TAB */
					p_ye = 0;		/* no yank to end of line */
				}
				if ((int *)varp == &curbuf->b_p_bin && curbuf->b_p_bin)	/* handle bin */
				{
					curbuf->b_p_tw = 0;		/* no automatic line wrap */
					curbuf->b_p_wm = 0;		/* no automatic line wrap */
					curbuf->b_p_tx = 0;		/* no text mode */
					p_ta = 0;				/* no text auto */
					curbuf->b_p_ml = 0;		/* no modelines */
					curbuf->b_p_et = 0;		/* no expandtab */
				}
				if ((int *)varp == &p_paste)	/* handle paste here */
				{
					BUF		*buf;

					if (p_paste && !oldpaste)	/* paste switched on */
					{
							/* save and set options for all buffers */
						for (buf = firstbuf; buf != NULL; buf = buf->b_next)
						{
							buf->b_p_tw_save = buf->b_p_tw;
							buf->b_p_wm_save = buf->b_p_wm;
							buf->b_p_ai_save = buf->b_p_ai;
							buf->b_p_si_save = buf->b_p_si;
							buf->b_p_tw = 0;		/* textwidth is 0 */
							buf->b_p_wm = 0;		/* wrapmargin is 0 */
							buf->b_p_ai = 0;		/* no auto-indent */
							buf->b_p_si = 0;		/* no smart-indent */
						}
							/* save and set global options */
						save_sm = p_sm;
						save_ru = p_ru;
						save_ri = p_ri;
						p_sm = 0;				/* no showmatch */
						p_ru = 0;				/* no ruler */
						p_ri = 0;				/* no reverse insert */
					}
					else if (!p_paste && oldpaste)	/* paste switched off */
					{
							/* restore options for all buffers */
						for (buf = firstbuf; buf != NULL; buf = buf->b_next)
						{
							buf->b_p_tw = buf->b_p_tw_save;
							buf->b_p_ai = buf->b_p_ai_save;
							buf->b_p_si = buf->b_p_si_save;
						}
							/* restore global options */
						p_sm = save_sm;
						p_ru = save_ru;
						p_ri = save_ri;
					}
				}
				if (!starting && ((int *)varp == &p_title ||
										(int *)varp == &p_icon))
				{
					if (*(int *)varp)			/* Set window title NOW */
						maketitle();
					else						/* Reset window title NOW */
						mch_restore_title((int *)varp == &p_title ? 1 : 2);
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
					*(long *)(varp) = atol((char *)arg + len + 1);

					if ((long *)varp == &p_wh)
					{
						if (p_wh < 0)
						{
							errmsg = e_positive;
							p_wh = 0;
						}
							/* Change window height NOW */
						if (p_wh && lastwin != firstwin)
						{
							win_equal(curwin, FALSE);
							must_redraw = CLEAR;
						}
					}
					if ((long *)varp == &p_ls)
						last_status();		/* (re)set last window status line */
				}
				else							/* string */
				{
					arg += len + 1;		/* jump to after the '=' */
					s = alloc((unsigned)(STRLEN(arg) + 1)); /* get a bit too much */
					if (s == NULL)
						break;
					if (flags & P_CHANGED)
						free(*(char **)(varp));
					*(char_u **)(varp) = s;
								/* copy the string */
					while (*arg && *arg != ' ')
					{
						if (*arg == '\\' && *(arg + 1)) /* skip over escaped chars */
								++arg;
						*s++ = *arg++;
					}
					*s = NUL;
					param_expand(i, TRUE);	/* expand environment variables and ~ */
					/*
					 * options that need some action
					 * to perform when changed (jw)
					 */
					if (varp == (char_u *)&term_strings.t_name)
						set_term(term_strings.t_name);
					else if (istermparam(&params[i]))
					{
						ttest(FALSE);
						if (varp == (char_u *)&term_strings.t_tp)
						{
							outstr(T_TP);
							updateScreen(CLEAR);
						}
					}
				}
			}
			params[i].flags |= P_CHANGED;
		}

skip:
		/*
		 * Check the bounds for numeric parameters here
		 */
		if (Rows < 2)
		{
			Rows = 2;
			errmsg = (char_u *)"Need at least 2 lines";
		}
		/*
		 * If the screenheight has been changed, assume it is the physical
		 * screenheight.
		 */
		if (oldRows != Rows)
		{
			screen_new_rows();
#ifdef MSDOS
			set_window();		/* active window may have changed */
#endif
		}

		if (curbuf->b_p_ts <= 0)
		{
			errmsg = e_positive;
			curbuf->b_p_ts = 8;
		}
		if (p_tm < 0)
		{
			errmsg = e_positive;
			p_tm = 0;
		}
		if (curwin->w_p_scroll <= 0 || curwin->w_p_scroll > curwin->w_height)
		{
			if (curwin->w_p_scroll != 0)
				errmsg = e_scroll;
			win_comp_scroll(curwin);
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
		if (p_ch < 1)
		{
			errmsg = e_positive;
			p_ch = 1;
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
			STRCPY(IObuff, errmsg);
			STRCAT(IObuff, ": ");
			s = IObuff + STRLEN(IObuff);
			while (*startarg && !isspace(*startarg))
				*s++ = *startarg++;
			*s = NUL;
			did_show = TRUE;	/* error message counts as show */
			++no_wait_return;	/* wait_return done below */
			emsg(IObuff);
			--no_wait_return;
			arg = startarg;		/* skip to next argument */
			++errcnt;			/* count number of errors */
		}
		skiptospace(&arg);				/* skip to next white space */
		skipspace(&arg);				/* skip spaces */
	}

	/*
	 * when 'updatecount' changes from zero to non-zero, open swap files
	 */
	if (p_uc && !olduc)
		ml_open_files();

	if (p_ch != oldch)					/* p_ch changed value */
		command_height();
	comp_col();

	/*
	 * Update the screen in case we changed something like "tabstop" or
	 * "lines" or "list" that will change its appearance.
	 * If we messed up the screen by showing more than one line of param
	 * values or an error message, call wait_return(), which will also
	 * update the screen.
	 */
	for (wp = firstwin; wp; wp = wp->w_next)
		wp->w_redr_status = TRUE;		/* mark all status lines dirty */
	if (p_ea && !oldea)
		win_equal(curwin, FALSE);
	if (did_show && msg_check())
	{
		msg_outchar('\n');
		wait_return(TRUE);
	}
	else
		updateScreen(NOT_VALID);
	return (errcnt == 0 ? OK : FAIL);
}

/*
 * expand environment variable at the start of some string options
 */
	static void
param_expand(i, dofree)
	int		i;
	int		dofree;
{
	char_u *p;

	if ((params[i].flags & P_EXPAND) &&
				(p = *(char_u **)(params[i].var)) != NULL &&
				(*p == '$' || *p == '~'))
	{
		expand_env(*(char_u **)(params[i].var), IObuff, IOSIZE);
		p = strsave(IObuff);
		if (p)
		{
			if (dofree)
				free(*(char_u **)(params[i].var));
			*(char_u **)(params[i].var) = p;
		}
	}
}

/*
 * find index for option 'arg'
 * return -1 if not found
 */
	static int
findparam(arg)
	char_u *arg;
{
	int		i;
	char	*s;

	for (i = 0; (s = params[i].fullname) != NULL; i++)
	{
		if (STRCMP(arg, s) == 0) /* match full name */
			break;
	}
	if (s == NULL)
	{
		for (i = 0; params[i].fullname != NULL; i++)
		{
			s = params[i].shortname;
			if (s != NULL && STRCMP(arg, s) == 0) /* match short name */
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
	char_u *arg;
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
	int				col;
	int				isterm;
	char_u			*varp;
	struct param	*(items[PARAM_COUNT]);
	int				item_count;
	int				run;
	int				row, rows;
	int				cols;
	int				i;
	int				len;

#define INC	19

	gotocmdline(TRUE, NUL);
	msg_outstr((char_u *)"--- Parameters ---\n");

	/*
	 * do the loop two times:
	 * 1. display the short items (non-strings and short strings)
	 * 2. display the long items (strings)
	 */
	for (run = 1; run <= 2 && !got_int; ++run)
	{
		/*
		 * collect the items in items[]
		 */
		item_count = 0;
		for (p = &params[0]; p->fullname != NULL && !got_int; p++)
		{
			isterm = istermparam(p);
			varp = get_varp(p);
			if (varp && (
				(all == 2 && isterm) ||
				(all == 1 && !isterm) ||
				(all == 0 && (p->flags & P_CHANGED))))
			{
				if ((p->flags & P_STRING) && *(char_u **)(varp) != NULL)
					len = STRLEN(p->fullname) + strsize(*(char_u **)(varp));
				else
					len = 1;		/* a non-string is assumed to fit always */
				if ((len <= INC - 4 && run == 1) || (len > INC - 4 && run == 2))
					items[item_count++] = p;
			}
			breakcheck();
		}
		/*
		 * display the items
		 */
		if (run == 1)
		{
			cols = Columns / INC;
			if (cols == 0)
				cols = 1;
			rows = (item_count + cols - 1) / cols;
		}
		else	/* run == 2 */
			rows = item_count;
		for (row = 0; row < rows && !got_int; ++row)
		{
			col = 0;
			for (i = row; i < item_count; i += rows)
			{
				msg_pos(-1, col);					/* make columns */
				showonep(items[i]);
				col += INC;
			}
			msg_outchar('\n');				/* scroll screen one line up */
			flushbuf();
			breakcheck();
		}
	}

	wait_return(FALSE);
}

/*
 * showonep: show the value of one option
 * must not be called with a hidden option!
 */
	static void
showonep(p)
		struct param *p;
{
	char_u			buf[64];
	char_u			*varp;

	varp = get_varp(p);

	if ((p->flags & P_BOOL) && !*(int *)(varp))
		msg_outstr((char_u *)"no");
	else
		msg_outstr((char_u *)"  ");
	msg_outstr((char_u *)p->fullname);
	if (!(p->flags & P_BOOL))
	{
		msg_outchar('=');
		if (p->flags & P_NUM)
		{
			sprintf((char *)buf, "%ld", *(long *)(varp));
			msg_outstr(buf);
		}
		else if (*(char_u **)(varp) != NULL)
		{
			if (p->flags & P_EXPAND)
			{
				home_replace(*(char_u **)(varp), NameBuff, MAXPATHL);
				msg_outtrans(NameBuff, -1);
			}
			else
				msg_outtrans(*(char_u **)(varp), -1);
		}
	}
}

/*
 * Write modified parameters as set command to a file.
 * Return FAIL on error, OK otherwise.
 */
	int
makeset(fd)
	FILE *fd;
{
	struct param	*p;
	char_u			*s;
	int				e;
	char_u			*varp;

	for (p = &params[0]; p->fullname != NULL; p++)
		if ((p->flags & P_CHANGED) && p->var)
		{
			varp = get_varp(p);
			if (p->flags & P_BOOL)
				fprintf(fd, "set %s%s", *(int *)(varp) ? "" : "no", p->fullname);
			else if (p->flags & P_NUM)
				fprintf(fd, "set %s=%ld", p->fullname, *(long *)(varp));
			else
			{
				fprintf(fd, "set %s=", p->fullname);
				s = *(char_u **)(varp);
					/* some characters hav to be escaped with CTRL-V or backslash */
				if (s != NULL && putescstr(fd, s, TRUE) == FAIL)
					return FAIL;
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
				return FAIL;
		}
	return OK;
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
				free(*(char_u **)(p->var));
			*(char_u **)(p->var) = NULL;
			p->flags &= ~P_CHANGED;
		}
}

/*
 * return TRUE if 'p' starts with 't_'
 */
	static int
istermparam(p)
	struct param *p;
{
	return (p->fullname[0] == 't' && p->fullname[1] == '_');
}

/*
 * Compute columns for ruler and shown command. 'sc_col' is also used to
 * decide what the maximum length of a message on the status line can be.
 * If there is a status line for the last window, 'sc_col' is independent
 * of 'ru_col'.
 */

#define COL_SHOWCMD 10		/* columns needed by shown command */
#define COL_RULER 17		/* columns needed by ruler */

	void
comp_col()
{
	int last_status = (p_ls == 2 || (p_ls == 1 && firstwin != lastwin));

	sc_col = 0;
	ru_col = 0;
	if (p_ru)
	{
		ru_col = COL_RULER + 1;
							/* no last status line, adjust sc_col */
		if (!last_status)
			sc_col = ru_col;
	}
	if (p_sc)
	{
		sc_col += COL_SHOWCMD;
		if (!p_ru || last_status)		/* no need for separating space */
			++sc_col;
	}
	sc_col = Columns - sc_col;
	ru_col = Columns - ru_col;
	if (sc_col <= 0)			/* screen too narrow, will become a mess */
		sc_col = 1;
	if (ru_col <= 0)
		ru_col = 1;
}

	static char_u *
get_varp(p)
	struct param	*p;
{
	if (!(p->flags & P_IND))
		return p->var;

	switch ((long)(p->var))
	{
		case PV_LIST:	return (char_u *)&(curwin->w_p_list);
		case PV_NU:		return (char_u *)&(curwin->w_p_nu);
		case PV_SCROLL:	return (char_u *)&(curwin->w_p_scroll);
		case PV_WRAP:	return (char_u *)&(curwin->w_p_wrap);

		case PV_AI:		return (char_u *)&(curbuf->b_p_ai);
		case PV_BIN:	return (char_u *)&(curbuf->b_p_bin);
		case PV_EOL:	return (char_u *)&(curbuf->b_p_eol);
		case PV_ET:		return (char_u *)&(curbuf->b_p_et);
		case PV_ML:		return (char_u *)&(curbuf->b_p_ml);
		case PV_RO:		return (char_u *)&(curbuf->b_p_ro);
		case PV_SI:		return (char_u *)&(curbuf->b_p_si);
		case PV_SN:		return (char_u *)&(curbuf->b_p_sn);
		case PV_SW:		return (char_u *)&(curbuf->b_p_sw);
		case PV_TS:		return (char_u *)&(curbuf->b_p_ts);
		case PV_TW:		return (char_u *)&(curbuf->b_p_tw);
		case PV_TX:		return (char_u *)&(curbuf->b_p_tx);
		case PV_WM:		return (char_u *)&(curbuf->b_p_wm);
		default:		EMSG("get_varp ERROR");
	}
	/* always return a valid pointer to avoid a crash! */
	return (char_u *)&(curbuf->b_p_wm);
}

/*
 * Copy options from one window to another.
 * Used when creating a new window.
 */
	void
win_copy_options(wp_from, wp_to)
	WIN		*wp_from;
	WIN		*wp_to;
{
	wp_to->w_p_list = wp_from->w_p_list;
	wp_to->w_p_nu = wp_from->w_p_nu;
	wp_to->w_p_scroll = wp_from->w_p_scroll;
	wp_to->w_p_wrap = wp_from->w_p_wrap;
}

/*
 * Copy options from one buffer to another.
 * Used when creating a new buffer.
 */
	void
buf_copy_options(bp_from, bp_to)
	BUF		*bp_from;
	BUF		*bp_to;
{
	bp_to->b_p_ai = bp_from->b_p_ai;
	bp_to->b_p_si = bp_from->b_p_si;
	bp_to->b_p_ro = bp_from->b_p_ro;
	bp_to->b_p_sw = bp_from->b_p_sw;
	bp_to->b_p_ts = bp_from->b_p_ts;
	bp_to->b_p_tw = bp_from->b_p_tw;
	bp_to->b_p_wm = bp_from->b_p_wm;
	bp_to->b_p_bin = bp_from->b_p_bin;
	bp_to->b_p_et = bp_from->b_p_et;
	bp_to->b_p_ml = bp_from->b_p_ml;
	bp_to->b_p_sn = bp_from->b_p_sn;
	bp_to->b_p_tx = bp_from->b_p_tx;
}

#ifdef WEBB_COMPLETE
	void
set_context_in_set_cmd(arg)
	char_u *arg;
{
	int 		nextchar;
	int 		flags;
	int			i;
	char_u		*p;
	char_u		*after_blank = NULL;

	expand_context = EXPAND_SETTINGS;
	if (*arg == NUL)
	{
		expand_pattern = arg;
		return;
	}
	p = arg + STRLEN(arg) - 1;
	if (*p == ' ' && *(p - 1) != '\\')
	{
		expand_pattern = p + 1;
		return;
	}
	while (p != arg && (*p != ' ' || *(p - 1) == '\\'))
	{
		if (*p == ' ' && after_blank == NULL)
			after_blank = p + 1;
		p--;
	}
	if (p != arg)
		p++;
	if (STRNCMP(p, "no", (size_t) 2) == 0)
	{
		expand_context = EXPAND_BOOL_SETTINGS;
		p += 2;
	}
	if (STRNCMP(p, "inv", (size_t) 3) == 0)
	{
		expand_context = EXPAND_BOOL_SETTINGS;
		p += 3;
	}
	expand_pattern = arg = p;
	while (isalnum(*p) || *p == '_' || *p == '*')	/* Allow * as wildcard */
		p++;
	if (*p == NUL)
		return;
	nextchar = *p;
	*p = NUL;
	i = findparam(arg);
	*p = nextchar;
	if (i == -1 || params[i].var == NULL)
	{
		expand_context = EXPAND_NOTHING;
		return;
	}
	flags = params[i].flags;
	if (flags & P_BOOL)
	{
		expand_context = EXPAND_NOTHING;
		return;
	}
	if ((nextchar != '=' && nextchar != ':')
	  || expand_context == EXPAND_BOOL_SETTINGS)
	{
		expand_context = EXPAND_UNSUCCESSFUL;
		return;
	}
	expand_context = EXPAND_NOTHING;
	if (flags & P_NUM)
		return;
	if (after_blank != NULL)
		expand_pattern = after_blank;
	else
		expand_pattern = p + 1;
	if (flags & P_EXPAND)
	{
		p = params[i].var;
		if (
#ifdef UNIX
			p == (char_u *)&p_bdir ||
#endif
			p == (char_u *)&p_dir || p == (char_u *)&p_path)
			expand_context = EXPAND_DIRECTORIES;
		else
			expand_context = EXPAND_FILES;
	}
	return;
}

	int
ExpandSettings(prog, num_file, file)
	regexp *prog;
	int *num_file;
	char_u ***file;
{
	int num_normal = 0;		/* Number of matching non-term-code settings */
	int num_term = 0;		/* Number of matching terminal code settings */
	int i;
	int match;
	int count;
	char_u *str;

	if (expand_context != EXPAND_BOOL_SETTINGS)
	{
		if (regexec(prog, (char_u *)"all", TRUE))
			num_normal++;
		if (regexec(prog, (char_u *)"termcap", TRUE))
			num_normal++;
	}
	for (i = 0; (str = (char_u *)params[i].fullname) != NULL; i++)
	{
		if (params[i].var == NULL)
			continue;
		if (expand_context == EXPAND_BOOL_SETTINGS
		  && !(params[i].flags & P_BOOL))
			continue;
		if (istermparam(&params[i]) && num_normal > 0)
			continue;
		match = FALSE;
		if (regexec(prog, str, TRUE))
			match = TRUE;
		else if (params[i].shortname != NULL
		  && regexec(prog, (char_u *)params[i].shortname, TRUE))
			match = TRUE;
		if (match)
		{
			if (istermparam(&params[i]))
				num_term++;
			else
				num_normal++;
		}
	}
	if (num_normal > 0)
		*num_file = num_normal;
	else if (num_term > 0)
		*num_file = num_term;
	else
		return OK;
	*file = (char_u **) alloc((unsigned)(*num_file * sizeof(char_u *)));
	if (*file == NULL)
	{
		*file = (char_u **)"";
		return FAIL;
	}
	count = 0;
	if (expand_context != EXPAND_BOOL_SETTINGS)
	{
		if (regexec(prog, (char_u *)"all", TRUE))
			(*file)[count++] = strsave((char_u *)"all");
		if (regexec(prog, (char_u *)"termcap", TRUE))
			(*file)[count++] = strsave((char_u *)"termcap");
	}
	for (i = 0; (str = (char_u *)params[i].fullname) != NULL; i++)
	{
		if (params[i].var == NULL)
			continue;
		if (expand_context == EXPAND_BOOL_SETTINGS
		  && !(params[i].flags & P_BOOL))
			continue;
		if (istermparam(&params[i]) && num_normal > 0)
			continue;
		match = FALSE;
		if (regexec(prog, str, TRUE))
			match = TRUE;
		else if (params[i].shortname != NULL
		  && regexec(prog, (char_u *)params[i].shortname, TRUE))
			match = TRUE;
		if (match)
			(*file)[count++] = strsave(str);
	}
	return OK;
}
#endif /* WEBB_COMPLETE */
