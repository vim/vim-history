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

/*
 * The param structure is initialized here.
 * The order of the parameters should be alfabetic
 */
struct param params[] =
{
		{"autoindent",	"ai",	P_BOOL,		(char *)&p_ai},
/*		{"autoprint",	"ap",	P_BOOL,		(char *)&p_ap}, */
		{"autowrite",	"aw",	P_BOOL,		(char *)&p_aw},
		{"backspace",	"bs",	P_NUM,		(char *)&p_bs},
		{"backup",		"bk",	P_BOOL,		(char *)&p_bk},
#ifdef UNIX
 		{"backupdir",	"bdir",	P_STRING,	(char *)&p_bdir},
#endif
/*		{"beautify",	"bf",	P_BOOL,		(char *)&p_bf}, */
		{"columns",		"co",	P_NUM,		(char *)&Columns},
		{"compatible",	"cp",	P_BOOL,		(char *)&p_cp},
#ifdef DIGRAPHS
		{"digraph",		"dg",	P_BOOL,		(char *)&p_dg},
#endif /* DIGRAPHS */
 		{"directory",	"dir",	P_STRING,	(char *)&p_dir},
		{"equalprg",	"ep",  	P_STRING,	(char *)&p_ep},
		{"errorbells",	"eb",	P_BOOL,		(char *)&p_eb},
		{"errorfile",	"ef",  	P_STRING,	(char *)&p_ef},
		{"expandtab",	"et",	P_BOOL,		(char *)&p_et},
		{"graphic",		"gr",	P_BOOL,		(char *)&p_gr},
/*		{"hardtabs",	"ht",	P_NUM,		(char *)&p_ht}, */
		{"helpfile",	"hf",  	P_STRING,	(char *)&p_hf},
		{"history", 	"hi", 	P_NUM,		(char *)&p_hi},
		{"ignorecase",	"ic",	P_BOOL,		(char *)&p_ic},
		{"insertmode",	"im",	P_BOOL,		(char *)&p_im},
		{"joinspaces", 	"js",	P_BOOL,		(char *)&p_js},
		{"keywordprg",	"kp",  	P_STRING,	(char *)&p_kp},
		{"lines",		NULL, 	P_NUM,		(char *)&Rows},
/*		{"lisp",		NULL,	P_BOOL		(char *)&p_lisp}, */
		{"list",		NULL,	P_BOOL,		(char *)&p_list},
		{"magic",		NULL,	P_BOOL,		(char *)&p_magic},
		{"modelines",	"ml",	P_NUM,		(char *)&p_ml},
		{"number",		"nu",	P_BOOL,		(char *)&p_nu},
/*		{"open",		NULL,	P_BOOL,		(char *)&p_open}, */
/*		{"optimize",	"opt",	P_BOOL,		(char *)&p_opt}, */
		{"paragraphs",	"para",	P_STRING,	(char *)&p_para},
/*		{"prompt",		NULL,	P_BOOL,		(char *)&p_prompt}, */
		{"readonly",	"ro",	P_BOOL,		(char *)&p_ro},
/*		{"redraw",		NULL,	P_BOOL,		(char *)&p_redraw}, */
		{"remap",		NULL,	P_BOOL,		(char *)&p_remap},
		{"repdel",		"rd",	P_BOOL,		(char *)&p_rd},
		{"report",		NULL,	P_NUM,		(char *)&p_report},
		{"ruler",		"ru",	P_BOOL,		(char *)&p_ru},
		{"scroll",		NULL, 	P_NUM,		(char *)&p_scroll},
		{"scrolljump",	"sj", 	P_NUM,		(char *)&p_sj},
		{"sections",	NULL,	P_STRING,	(char *)&p_sections},
		{"shell",		"sh",	P_STRING,	(char *)&p_sh},
		{"shelltype",	"st",	P_NUM,		(char *)&p_st},
		{"shiftround",	"sr",	P_BOOL,		(char *)&p_sr},
		{"shiftwidth",	"sw",	P_NUM,		(char *)&p_sw},
#ifndef MSDOS
		{"shortname",	"sn",	P_BOOL,		(char *)&p_sn},
#endif
		{"showcmd",		"sc",	P_BOOL,		(char *)&p_sc},
		{"showmatch",	"sm",	P_BOOL,		(char *)&p_sm},
		{"showmode",	"mo",	P_BOOL,		(char *)&p_mo},
/*		{"slowopen",	"slow",	P_BOOL,		(char *)&p_slow}, */
		{"smartindent", "si",	P_BOOL,		(char *)&p_si},
		{"suffixes",	"su",	P_STRING,	(char *)&p_su},
		{"tabstop", 	"ts",	P_NUM,		(char *)&p_ts},
		{"taglength",	"tl",	P_NUM,		(char *)&p_tl},
		{"tags",		NULL,	P_STRING,	(char *)&p_tags},
		{"term",		NULL,	P_STRING,	(char *)&term_strings.t_name},
		{"terse",		NULL,	P_BOOL,		(char *)&p_terse},
#ifdef MSDOS
		{"textmode",	"tx",	P_BOOL,		(char *)&p_tx},
#endif
		{"textwidth",	"tw",	P_NUM,		(char *)&p_tw},
		{"tildeop", 	"to",	P_BOOL,		(char *)&p_to},
		{"timeout", 	NULL,	P_BOOL,		(char *)&p_timeout},
		{"ttimeout", 	NULL,	P_BOOL,		(char *)&p_ttimeout},
		{"undolevels",	"ul",	P_NUM,		(char *)&p_ul},
		{"updatecount",	"uc",	P_NUM,		(char *)&p_uc},
		{"updatetime",	"ut",	P_NUM,		(char *)&p_ut},
		{"visualbell",	"vb",	P_BOOL,		(char *)&p_vb},
		{"warn",		NULL,	P_BOOL,		(char *)&p_warn},
/*		{"window",		NULL, 	P_NUM,		(char *)&p_window}, */
/*		{"w300",		NULL, 	P_NUM,		(char *)&p_w300}, */
/*		{"w1200",		NULL, 	P_NUM,		(char *)&p_w1200}, */
/*		{"w9600",		NULL, 	P_NUM,		(char *)&p_w9600}, */
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

static void	showparams __ARGS((int));
static void showonep __ARGS((struct param *));
static int  istermparam __ARGS((struct param *));

/*
 * Initialize the shell parameter and scroll size.
 */
	void
set_init()
{
		char *p;

		if ((p = (char *)vimgetenv("SHELL")) != NULL)
			p_sh = strsave(p);
		p_scroll = (Rows >> 1);
}

	void
doset(arg)
	char		*arg;	/* parameter string */
{
	register int i;
	char		*s;
	char		*errmsg;
	char		*startarg;
	int			prefix;	/* 0: nothing, 1: "no", 2: "inv" in front of name */
	int 		nextchar;
	int 		len = 0;
	int 		flags;
	int			olduc = p_uc;	/* remember old update count */

	if (*arg == NUL)
	{
		showparams(0);
		return;
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
			for (i = 0; (s = params[i].fullname) != NULL; i++)
			{
				if (strncmp(arg, s, (size_t)(len = strlen(s))) == 0) /* match full name */
					break;
			}
			if (s == NULL)
			{
				for (i = 0; params[i].fullname != NULL; i++)
				{
						s = params[i].shortname;
						if (s != NULL && strncmp(arg, s, (size_t)(len = strlen(s))) == 0) /* match short name */
							break;
						s = NULL;
				}
			}

			if (s == NULL)		/* found a mismatch: skip the rest */
			{
				errmsg = "Unknown option:   ";	/* must be 18 chars */
				goto skip;
			}

			flags = params[i].flags;
			nextchar = arg[len];
			/*
			 * allow '=' and ':' as MSDOS command.com allows only one
			 * '=' character per "set" command line. grrr. (jw)
			 */
			if (nextchar == '?' || 
			    (prefix == 1 && nextchar != '=' &&
				 nextchar != ':' && !(flags & P_BOOL)))
			{										/* print value */
				gotocmdline(TRUE, NUL);
				showonep(&params[i]);
			}
			else if (nextchar != NUL && strchr("=: \t", nextchar) == NULL)
			{
				errmsg = e_setarg;
				goto skip;
			}
			else if (flags & P_BOOL)					/* boolean */
			{
					if (nextchar == '=' || nextchar == ':')
					{
						errmsg = e_setarg;
						goto skip;
					}
					if (prefix == 2)
						*(int *)(params[i].var) ^= 1;	/* invert it */
					else
						*(int *)(params[i].var) = prefix;
					if ((int *)params[i].var == &p_cp && p_cp)	/* handle cp here */
					{
						p_bs = 0;		/* normal backspace */
						p_bk = 0;		/* no backup file */
#ifdef DIGRAPHS
						p_dg = 0;		/* no digraphs */
#endif /* DIGRAPHS */
						p_et = 0;		/* no expansion of tabs */
						p_hi = 0;		/* no history */
						p_im = 0;		/* do not start in insert mode */
						p_js = 1;		/* insert 2 spaces after period */
						p_ml = 0;		/* no modelines */
						p_rd = 1;		/* del replaces char */
						p_ru = 0;		/* no ruler */
						p_sj = 1;		/* no scrolljump */
						p_sr = 0;		/* do not round indent to shiftwidth */
						p_sc = 0;		/* no showcommand */
						p_mo = 0;		/* no showmode */
						p_si = 0;		/* no smartindent */
						p_tw = 9999;	/* maximum textwidth */
						p_to = 0;		/* no tilde operator */
						p_ttimeout = 0;	/* no terminal timeout */
						p_ul = 0;		/* no multilevel undo */
						p_uc = 0;		/* no autoscript file */
						p_wb = 0;		/* no backup file */
						p_ye = 0;		/* no yank to end of line */
					}
			}
			else								/* numeric or string */
			{
				if ((nextchar != '=' && nextchar != ':') || prefix != 1)
				{
					errmsg = e_setarg;
					goto skip;
				}
				if (flags & P_NUM)				/* numeric */
				{
					len = atoi(arg + len + 1);
					if ((long *)params[i].var == &p_wm)	/* wrapmargin is translated into textlength */
					{
						if (len >= Columns)
							len = Columns - 1;
						p_tw = Columns - len;
					}
					*(long *)(params[i].var) = len;
				}
				else							/* string */
				{
					arg += len + 1;
					s = alloc((unsigned)(strlen(arg) + 1)); /* get a bit too much */
					if (s == NULL)
						break;
					if (flags & P_CHANGED)
						free(*(char **)(params[i].var));
					*(char **)(params[i].var) = s;
								/* copy the string */
					while (*arg && *arg != ' ' && *arg != '\t')
					{
						if (*arg == '\\' && *(arg + 1)) /* skip over escaped chars */
								++arg;
						*s++ = *arg++;
					}
					*s = NUL;
					/*
					 * options that need some action
					 * to perform when changed (jw)
					 */
					if (params[i].var == (char *)&term_strings.t_name)
					    set_term(term_strings.t_name);
					else if (istermparam(&params[i]))
						ttest(FALSE);
				}
			}
			params[i].flags |= P_CHANGED;
		}

skip:
		if (errmsg)
		{
			strcpy(IObuff, errmsg);
			s = IObuff + 18;
			while (*startarg && !isspace(*startarg))
				*s++ = *startarg++;
			*s = NUL;
			emsg(IObuff);
			arg = startarg;		/* skip to next argument */
		}

		skiptospace(&arg);				/* skip to next white space */
		skipspace(&arg);				/* skip spaces */
	}

	/*
	 * Check the bounds for numeric parameters here
	 */
	if (Rows < 2)
	{
		Rows = 2;
		emsg("Need at least 2 lines");
	}
	if (p_ts <= 0 || p_ts > 16)
	{
		emsg(e_tabsize);
		p_ts = 8;
	}
	if (p_scroll <= 0 || p_scroll > Rows)
	{
		emsg(e_scroll);
		p_scroll = Rows >> 1;
	}
	if (p_report < 0)
	{
		emsg(e_positive);
		p_report = 1;
	}
	if (p_sj < 0 || p_sj >= Rows)
	{
		emsg(e_scroll);
		p_sj = 1;
	}
	if (p_ul < 0)
	{
		emsg(e_positive);
		p_ul = 100;
	}
	if (p_uc < 0)
	{
		emsg(e_positive);
		p_uc = 100;
	}
	if (p_uc == 0 && olduc != 0)		/* p_uc changed from on to off */
		stopscript();
	if (p_uc > 0 && olduc == 0)		/* p_uc changed from off to on */
		startscript();
#ifdef MSDOS
	textfile(p_tx);
#endif
	if (p_ut < 0)
	{
		emsg(e_positive);
		p_ut = 2000;
	}

	/*
	 * Update the screen in case we changed something like "tabstop" or
	 * "lines" or "list" that will change its appearance.
	 */
	updateScreen(NOT_VALID);
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
		if ((all == 2 && isterm) ||
			(all == 1 && !isterm) ||
			(all == 0 && (p->flags & P_CHANGED)))
		{
			if ((p->flags & P_STRING) && *(char **)(p->var) != NULL)
				inc = strlen(p->fullname) + strsize(*(char **)(p->var)) + 1;
			else
				inc = 1;
			if (col + inc >= Columns)
			{
				outchar('\n');
				col = 0;
			}

			showonep(p);
			col += inc;
			col += 19 - col % 19;
			if (col < Columns - 19)
				windgoto((int)Rows - 1, col); /* make columns */
			else
			{
				col = 0;
				outchar('\n');
			}
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
		if (p->flags & P_CHANGED)
		{
			if (p->flags & P_BOOL)
				e = fprintf(fd, "set %s%s\n", *(int *)(p->var) ? "" : "no", p->fullname);
			else if (p->flags & P_NUM)
				e = fprintf(fd, "set %s=%ld\n", p->fullname, *(long *)(p->var));
			else
			{
				fprintf(fd, "set %s=", p->fullname);
				s = *(char **)(p->var);
				if (s != NULL)
					for ( ; *s; ++s)
					{
						if (*s < ' ' || *s > '~')
							putc(Ctrl('V'), fd);
						putc(*s, fd);
					}
				e = putc('\n', fd);
			}
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
		if (istermparam(p))
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
