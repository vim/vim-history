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
 * macro for it in param.h. If it's a numeric parameter, add any necessary
 * bounds checks to doset().
 */

#include "vim.h"

/*
 * The param structure is initialized here.
 * The order of the parameters MUST be the same as defined in param.h
 */
struct param params[] =
{
		{"autoindent",	"ai",	FALSE,	P_BOOL},
/*		{"autoprint",	"ap",	TRUE,	P_BOOL}, */
		{"autowrite",	"aw",	FALSE,	P_BOOL},
		{"backspace",	"bs",	FALSE,	P_BOOL},
		{"backup",		"bk",	TRUE,	P_BOOL},
/*		{"beautify",	"bf",	FALSE,	P_BOOL}, */
 		{"directory",	"dir",	0,		P_STRING},
		{"errorbells",	"eb",	FALSE,	P_BOOL},
		{"errorfile",	"ef",   0,		P_STRING},
		{"expandtab",	"et",	FALSE,	P_BOOL},
/*		{"hardtabs",	"ht",	8,		P_NUM}, */
		{"history", 	"hi",	20, 	P_NUM},
		{"ignorecase",	"ic",	FALSE,	P_BOOL},
		{"joinspaces", 	"js",	TRUE,	P_BOOL},
		{"lines",		NULL,	25, 	P_NUM},
/*		{"lisp",		NULL,	FALSE,	P_BOOL}, */
		{"list",		NULL,	FALSE,	P_BOOL},
		{"magic",		NULL,	TRUE,	P_BOOL},
		{"modelines",	"ml",	5,		P_NUM},
		{"number",		"nu",	FALSE,	P_BOOL},
/*		{"open",		NULL,	TRUE,	P_BOOL}, */
/*		{"optimize",	"opt",	TRUE,	P_BOOL}, */
		{"paragraphs",	"para", 0,		P_STRING},
/*		{"prompt",		NULL,	TRUE,	P_BOOL}, */
		{"readonly",	"ro",	FALSE,	P_BOOL},
/*		{"redraw",		NULL,	FALSE,	P_BOOL}, */
		{"remap",		NULL,	TRUE,	P_BOOL},
		{"repdel",		"rd",	TRUE,	P_BOOL},
		{"report",		NULL,	5,		P_NUM},
		{"scroll",		NULL,	12, 	P_NUM},
		{"sections",	NULL,	0,		P_STRING},
		{"shell",		"sh",	0,		P_STRING},
		{"shelltype",	"st",	0,		P_NUM},
		{"shiftround",	"sr",	FALSE,	P_BOOL},
		{"shiftwidth",	"sw",	8,		P_NUM},
		{"showcmd",		"sc",	TRUE,	P_BOOL},
		{"showmatch",	"sm",	FALSE,	P_BOOL},
		{"showmode",	"mo",	TRUE,	P_BOOL},
/*		{"slowopen",	"slow", FALSE,	P_BOOL}, */
		{"smartindent", "si",	FALSE,	P_BOOL},
		{"suffixes",	"su",	0,		P_STRING},
		{"tabstop", 	"ts",	8,		P_NUM},
		{"taglength",	"tl",	0,		P_NUM},
		{"tags",		NULL,	0,		P_STRING},
/*		{"term",		NULL,	0,		P_STRING}, */
		{"terse",		NULL,	TRUE,	P_BOOL},
		{"textwidth",	"tw",	9999,	P_NUM},
		{"tildeop", 	"to",	FALSE,	P_BOOL},
		{"undolevels",	"ul",	100,	P_NUM},
		{"updatecount",	"uc",	100,	P_NUM},
		{"updatetime",	"ut",	2000,	P_NUM},
		{"visualbell",	"vb",	FALSE,	P_BOOL},
		{"warn",		NULL,	TRUE,	P_BOOL},
/*		{"window",		NULL,	24, 	P_NUM}, */
/*		{"w300",		NULL,	24, 	P_NUM}, */
/*		{"w1200",		NULL,	24, 	P_NUM}, */
/*		{"w9600",		NULL,	24, 	P_NUM}, */
		{"wrapscan",	"ws",	TRUE,	P_BOOL},
		{"wrapmargin",	"wm",	0,		P_NUM},
		{"writeany",	"wa",	FALSE,	P_BOOL},
		{"yankendofline", "ye", FALSE,	P_BOOL},
		{NULL, NULL, 0, 0}								/* end marker */
};

/*
 * Initialize some of the parameters.
 * We need this because string parameters are unions which cannot be initialized
 * by the compiler.
 */
	void
set_init()
{
		char *p;

		PS(P_EF) = "AztecC.Err";
		PS(P_PARA) = "IPLPPPQPP LIpplpipbp";
		PS(P_DIR) = "";
		PS(P_SECTIONS) = "SHNHH HUnhsh";
		if ((p = getenv("SHELL")) == NULL)
			PS(P_SHELL) = "sh";
		else
			PS(P_SHELL) = strsave(p);
		PS(P_SU) = ".bak.o.h.info.vim";
		PS(P_TAGS) = "tags";
}

static void	showparams __ARGS((bool_t));
static void showonep __ARGS((struct param *));
static char paramerr[] = "invalid 'set' parameter";

	void
doset(arg)
	char		*arg;	/* parameter string */
{
	register int		 i;
	char		*s;
	bool_t		did_lines = FALSE;
	bool_t		state;	/* new state of boolean parms. */
	int 		nextchar;
	int 		len;
	int 		flags;
	int			olduc = P(P_UC);	/* remember old update count */

	if (*arg == NUL)
	{
		showparams((bool_t)FALSE);
		return;
	}

	while (*arg)		/* loop to process all parameters */
	{
		if (strncmp(arg, "all", (size_t)3) == 0)
				showparams((bool_t)TRUE);
		else
		{
			state = TRUE;
			if (strncmp(arg, "no", (size_t)2) == 0)
			{
				state = FALSE;
				arg += 2;
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
				emsg("Unrecognized 'set' option");
				break;
			}

			flags = params[i].flags;
			nextchar = arg[len];
			if (nextchar == '?' || nextchar != '=' && !(flags & P_BOOL))
			{										/* print value */
				gotocmdline((bool_t)TRUE, NUL);
				showonep(&params[i]);
			}
			else if (nextchar != NUL && strchr("= \t", nextchar) == NULL)
			{
				emsg(paramerr);
				break;
			}
			else if (flags & P_BOOL)					/* boolean */
			{
					if (nextchar == '=')
					{
						emsg(paramerr);
						break;
					}
					P(i) = state;
			}
			else								/* numeric or string */
			{
				if (nextchar != '=' || state == FALSE)
				{
					emsg(paramerr);
					break;
				}
				if (flags & P_NUM)				/* numeric */
				{
					did_lines = (i == P_LI);
					len = atoi(arg + len + 1);
					if (i == P_WM)	/* wrapmargin is translated into textlength */
					{
						if (len >= Columns)
							len = Columns - 1;
						P(P_TW) = Columns - len;
					}
					P(i) = len;
				}
				else							/* string */
				{
					arg += len + 1;
					s = alloc((unsigned)(strlen(arg) + 1)); /* get a bit too much */
					if (s == NULL)
						break;
					if (flags & P_CHANGED)
						free(PS(i));
					PS(i) = s;
								/* copy the string */
					while (*arg && *arg != ' ' && *arg != '\t')
					{
						if (*arg == '\\' && *(arg + 1)) /* skip over escaped chars */
								++arg;
						*s++ = *arg++;
					}
					*s = NUL;
				}
			}
			params[i].flags |= P_CHANGED;
		}

										/* skip to next white space */
		while (*arg != ' ' && *arg != '\t' && *arg != NUL)
				arg++;
		skipspace(&arg);				/* skip spaces */
	}

	/*
	 * Check the bounds for numeric parameters here
	 */
	if (P(P_TS) <= 0 || P(P_TS) > 16)
	{
		emsg("Invalid tab size specified");
		P(P_TS) = 8;
	}
	if (P(P_SS) <= 0 || P(P_SS) > Rows)
	{
		emsg("Invalid scroll size specified");
		P(P_SS) = Rows >> 1;
	}
	if (P(P_UL) < 0)
	{
		emsg("undo levels must be positive");
		P(P_UL) = 100;
	}
	if (P(P_UC) < 0)
	{
		emsg("update count must be positive");
		P(P_UC) = 100;
	}
	if (P(P_UC) == 0 && olduc != 0)		/* P_UC changed from on to off */
		stopscript();
	if (P(P_UC) > 0 && olduc == 0)		/* P_UC changed from off to on */
		startscript();
	if (P(P_UT) < 0)
	{
		emsg("update time must be positive");
		P(P_UT) = 2000;
	}

	/*
	 * Update the screen in case we changed something like "tabstop" or
	 * "list" that will change its appearance.
	 */
	if (did_lines)
		Rows = P(P_LI);		/* screen buffers will be allocated by updateScreen() */
	updateScreen(NOT_VALID);
}

/*
 * if 'all' == 1: show all parameters
 * if 'all' == 0: show changed parameters
 */
	static void
showparams(all)
	bool_t			all;
{
	struct param   *p;
	int				col = 0;
	int				inc;

	gotocmdline(YES, NUL);
	outstr("Parameters:\r\n");

	setmode(0);				/* set cooked mode so output can be halted */
	for (p = &params[0]; p->fullname != NULL; p++)
		if (all || (p->flags & P_CHANGED))
		{
			if (p->flags & P_STRING)
				inc = strlen(p->fullname) + strlen(p->val.strval) + 1;
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
				windgoto(Rows - 1, col); /* make columns */
			else
			{
				outchar('\n');
				col = 0;
			}
			flushbuf();
		}

	if (col)
		outchar('\n');
	setmode(1);
	wait_return((bool_t)TRUE);
}

	static void
showonep(p)
		struct param *p;
{
	char			buf[64];

		if ((p->flags & P_BOOL) && !p->val.intval)
			outstr("no");
		outstr(p->fullname);
		if (!(p->flags & P_BOOL))
		{
			outchar('=');
			if (p->flags & P_NUM)
			{
				sprintf(buf, "%ld", p->val.intval);
				outstr(buf);
			}
			else
				outstr(p->val.strval);
		}
}

/*
 * Write modified parameters as set command to a file.
 * Return 1 on error.
 */
makeset(fd)
	FILE *fd;
{
	struct param   *p;
	int e;

	for (p = &params[0]; p->fullname != NULL; p++)
		if (p->flags & P_CHANGED)
		{
			if (p->flags & P_BOOL)
				e = fprintf(fd, "set %s%s\n", p->val.intval ? "" : "no", p->fullname);
			else if (p->flags & P_NUM)
				e = fprintf(fd, "set %s=%ld\n", p->fullname, p->val.intval);
			else
				e = fprintf(fd, "set %s=%s\n", p->fullname, p->val.strval);
			if (e < 0)
				return 1;
		}
	return 0;
}
