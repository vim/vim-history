/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#define EXTERN
#include "vim.h"

#ifdef SPAWNO
# include <spawno.h>		/* special MSDOS swapping library */
#endif

static void mainerr __ARGS((int, char_u *));
static void usage __ARGS((void));
static int get_number_arg __ARGS((char_u *p, int *idx, int def));

/*
 * Type of error message.  These must match with errors[] in mainerr().
 */
#define ME_UNKNOWN_OPTION	0
#define ME_TOO_MANY_ARGS	1
#define ME_ARG_MISSING		2
#define ME_GARBAGE		3
#define ME_EXTRA_CMD		4

    static void
mainerr(n, str)
    int	    n;
    char_u  *str;
{
    static char	*(errors[]) =
    {
	"Unknown option",
	"Too many edit arguments",
	"Argument missing after",
	"Garbage after option",
	"Too many \"+command\" or \"-c command\" arguments",
    };

#if defined(UNIX) || defined(__EMX__)
    reset_signals();		/* kill us with CTRL-C here, if you like */
#endif

    mch_errmsg(longVersion);
    mch_errmsg("\n");
    mch_errmsg(errors[n]);
    if (str != NULL)
    {
	mch_errmsg(": \"");
	mch_errmsg((char *)str);
	mch_errmsg("\"");
    }
    mch_errmsg("\nMore info with: \"vim -h\"\n");

    mch_windexit(1);
}

    static void
usage()
{
    int		    i;
    static char_u *(use[]) =
    {
	(char_u *)"[file ..]       edit specified file(s)",
	(char_u *)"-               read from stdin",
	(char_u *)"-t tag          edit file where tag is defined",
#ifdef QUICKFIX
	(char_u *)"-q [errorfile]  edit file with first error"
#endif
    };

#if defined(UNIX) || defined(__EMX__)
    reset_signals();		/* kill us with CTRL-C here, if you like */
#endif

    mch_errmsg(longVersion);
    mch_errmsg("\nusage:");
    for (i = 0; ; ++i)
    {
	mch_errmsg(" vim [options] ");
	mch_errmsg((char *)use[i]);
	if (i == (sizeof(use) / sizeof(char_u *)) - 1)
	    break;
	mch_errmsg("\n   or:");
    }

    mch_errmsg("\n\nOptions:\n");
    mch_errmsg("   --\t\t\tEnd of options\n");
#ifdef USE_GUI
    mch_errmsg("   -g\t\t\tRun using GUI (like \"gvim\")\n");
    mch_errmsg("   -f\t\t\tForeground: Don't fork when starting GUI\n");
#endif
    mch_errmsg("   -v\t\t\tVi mode (like \"vi\")\n");
    mch_errmsg("   -e\t\t\tEx mode (like \"ex\")\n");
    mch_errmsg("   -s\t\t\tSilent (batch) mode (only for \"ex\")\n");
    mch_errmsg("   -R\t\t\tReadonly mode (like \"view\")\n");
    mch_errmsg("   -Z\t\t\tRestricted mode (like \"rvim\")\n");
    mch_errmsg("   -b\t\t\tBinary mode\n");
#ifdef LISPINDENT
    mch_errmsg("   -l\t\t\tLisp mode\n");
#endif
    mch_errmsg("   -C\t\t\tCompatible with Vi: 'compatible'\n");
    mch_errmsg("   -N\t\t\tNot fully Vi compatible: 'nocompatible'\n");
    mch_errmsg("   -V[N]\t\tVerbose level\n");
    mch_errmsg("   -n\t\t\tNo swap file, use memory only\n");
    mch_errmsg("   -r\t\t\tList swap files and exit\n");
    mch_errmsg("   -r (with file name)\tRecover crashed session\n");
    mch_errmsg("   -L\t\t\tSame as -r\n");
#ifdef AMIGA
    mch_errmsg("   -f\t\t\tDon't use newcli to open window\n");
    mch_errmsg("   -d <device>\t\tUse <device> for I/O\n");
#endif
#ifdef RIGHTLEFT
    mch_errmsg("   -H\t\t\tstart in Hebrew mode\n");
#endif
#ifdef FKMAP
    mch_errmsg("   -F\t\t\tstart in Farsi mode\n");
#endif
    mch_errmsg("   -T <terminal>\tSet terminal type to <terminal>\n");
    mch_errmsg("   -o[N]\t\tOpen N windows (default: one for each file)\n");
    mch_errmsg("   +\t\t\tStart at end of file\n");
    mch_errmsg("   +<lnum>\t\tStart at line <lnum>\n");
    mch_errmsg("   -c <command>\t\tExecute <command> first\n");
    mch_errmsg("   -s <scriptin>\tRead commands from script file <scriptin>\n");
    mch_errmsg("   -w <scriptout>\tAppend commands to script file <scriptout>\n");
    mch_errmsg("   -W <scriptout>\tWrite commands to script file <scriptout>\n");
    mch_errmsg("   -u <vimrc>\t\tUse <vimrc> instead of any .vimrc\n");
#ifdef USE_GUI
    mch_errmsg("   -U <gvimrc>\t\tUse <gvimrc> instead of any .gvimrc\n");
#endif
#ifdef VIMINFO
    mch_errmsg("   -i <viminfo>\t\tUse <viminfo> instead of .viminfo\n");
#endif
    mch_errmsg("   -h\t\t\tprint Help and exit (this message)\n");

#ifdef USE_GUI_X11
# ifdef USE_GUI_MOTIF
    mch_errmsg("\nOptions recognised by gvim (Motif version):\n");
# else
#  ifdef USE_GUI_ATHENA
    mch_errmsg("\nOptions recognised by gvim (Athena version):\n");
#  endif /* USE_GUI_ATHENA */
# endif /* USE_GUI_MOTIF */
    mch_errmsg("   -display <display>\tRun vim on <display>\n");
    mch_errmsg("   -iconic\t\tStart vim iconified\n");
# if 0
    mch_errmsg("   -name <name>\t\tUse resource as if vim was <name>\n");
    mch_errmsg("\t\t\t  (Unimplemented)\n");
# endif
    mch_errmsg("   -background <color>\tUse <color> for the background (also: -bg)\n");
    mch_errmsg("   -foreground <color>\tUse <color> for normal text (also: -fg)\n");
    mch_errmsg("   -font <font>\t\tUse <font> for normal text (also: -fn)\n");
    mch_errmsg("   -boldfont <font>\tUse <font> for bold text\n");
    mch_errmsg("   -italicfont <font>\tUse <font> for italic text\n");
    mch_errmsg("   -geometry <geom>\tUse <geom> for initial geometry (also: -geom)\n");
    mch_errmsg("   -borderwidth <width>\tUse a border width of <width> (also: -bw)\n");
    mch_errmsg("   -scrollbarwidth <width>\tUse a scrollbar width of <width> (also: -sw)\n");
    mch_errmsg("   -menuheight <height>\tUse a menu bar height of <height> (also: -mh)\n");
    mch_errmsg("   -reverse\t\tUse reverse video (also: -rv)\n");
    mch_errmsg("   +reverse\t\tDon't use reverse video (also: +rv)\n");
    mch_errmsg("   -xrm <resource>\tSet the specified resource\n");
#endif /* USE_GUI_X11 */

    mch_windexit(1);
}

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

/* Maximum number of commands from + or -c options */
#define MAX_ARG_CMDS 10

#ifndef PROTO	    /* don't want a prototype for main() */
    void
#ifdef VIMDLL
_export
#endif
main(argc, argv)
    int		    argc;
    char	  **argv;
{
    char_u	   *initstr;		    /* init string from environment */
    char_u	   *term = NULL;	    /* specified terminal name */
    char_u	   *fname = NULL;	    /* file name from command line */
    char_u	   *tagname = NULL;	    /* tag from -t option */
    char_u	   *use_vimrc = NULL;	    /* vimrc from -u option */
#ifdef QUICKFIX
    char_u	   *use_ef = NULL;	    /* 'errorfile' from -q option */
#endif
    int		    n_commands = 0;	    /* no. of commands from + or -c */
    char_u	   *commands[MAX_ARG_CMDS]; /* commands from + or -c option */
    int		    no_swap_file = FALSE;   /* "-n" option used */
    int		    c;
    int		    i;
    int		    bin_mode = FALSE;	    /* -b option used */
    int		    window_count = 1;	    /* number of windows to use */
    int		    arg_idx = 0;	    /* index for arg_files[] */
    int		    had_minmin = FALSE;	    /* found "--" option */
    int		    argv_idx;		    /* index in argv[n][] */
    int		    want_full_screen = TRUE;
    int		    want_argument;	    /* option with argument */
#define EDIT_NONE   0	    /* no edit type yet */
#define EDIT_FILE   1	    /* file name argument[s] given, use arg_files[] */
#define EDIT_STDIN  2	    /* read file from stdin */
#define EDIT_TAG    3	    /* tag name argument given, use tagname */
#define EDIT_QF	    4	    /* start in quickfix mode */
    int		    edit_type = EDIT_NONE;  /* type of editing to do */
    int		    stdout_isatty;	    /* is stdout a terminal? */
    int		    input_isatty;	    /* is active input a terminal? */
    OPARG	    oa;			    /* operator arguments */

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    /*
     * Default mapping for some often used keys.
     * Use the Windows (CUA) keybindings.
     */
    static struct initmap
    {
	char_u	    *arg;
	int	    mode;
    } initmappings[] =
    {
# ifdef USE_GUI
	/* Normal and Visual mode */
	{(char_u *)"<C-PageUp> H", NORMAL+VISUAL},
	{(char_u *)"<C-PageDown> L$", NORMAL+VISUAL},

	/* Insert mode */
	{(char_u *)"<C-PageUp> <C-O>H",INSERT},
	{(char_u *)"<C-PageDown> <C-O>L<C-O>$", INSERT},

	/* paste, copy and cut */
	{(char_u *)"<S-Insert> \"*P", NORMAL},
	{(char_u *)"<S-Insert> \"\"d\"*P", VISUAL},
	{(char_u *)"<S-Insert> <C-R>*", INSERT+CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VISUAL},
	{(char_u *)"<S-Del> \"*d", VISUAL},
	{(char_u *)"<C-Del> \"*d", VISUAL},
	{(char_u *)"<C-X> \"\"d", VISUAL},
	/* Missing: CTRL-C (can't be mapped) and CTRL-V (means something) */
# else
	/* Normal and Visual mode */
	{(char_u *)"\316\204 H", NORMAL+VISUAL},    /* CTRL-PageUp is "H" */
	{(char_u *)"\316v L$", NORMAL+VISUAL},	    /* CTRL-PageDown is "L$" */

	/* Insert mode */
	{(char_u *)"\316\204 \017H",INSERT},	    /* CTRL-PageUp is "^OH"*/
	{(char_u *)"\316v \017L\017$", INSERT},	    /* CTRL-PageDown ="^OL^O$"*/

	/* paste, copy and cut */
#  ifdef USE_CLIPBOARD
	{(char_u *)"\316\324 \"*P", NORMAL},	    /* SHIFT-Insert is "*P */
	{(char_u *)"\316\324 \"\"d\"*P", VISUAL},   /* SHIFT-Insert is ""d"*P */
	{(char_u *)"\316\324 \017\"*P", INSERT},    /* SHIFT-Insert is ^O"*P */
	{(char_u *)"\316\325 \"*y", VISUAL},	    /* CTRL-Insert is "*y */
	{(char_u *)"\316\327 \"*d", VISUAL},	    /* SHIFT-Del is "*d */
	{(char_u *)"\316\330 \"*d", VISUAL},	    /* CTRL-Del is "*d */
	{(char_u *)"\030 \"\"d", VISUAL},	    /* CTRL-X is ""d */
#  else
	{(char_u *)"\316\324 P", NORMAL},	    /* SHIFT-Insert is P */
	{(char_u *)"\316\324 d\"0P", VISUAL},	    /* SHIFT-Insert is d"0P */
	{(char_u *)"\316\324 \017P", INSERT},	    /* SHIFT-Insert is ^OP */
	{(char_u *)"\316\325 y", VISUAL},	    /* CTRL-Insert is y */
	{(char_u *)"\316\327 d", VISUAL},	    /* SHIFT-Del is d */
	{(char_u *)"\316\330 d", VISUAL},	    /* CTRL-Del is d */
#  endif
# endif
    };
#endif

#ifdef __EMX__
    _wildcard(&argc, &argv);
#endif

#ifdef HAVE_LOCALE_H
    setlocale(LC_ALL, "");	/* for ctype() and the like */
#endif

#ifdef USE_GUI_WIN32
# ifdef HAVE_OLE
    /* Check for special OLE command line parameters */
    if (argc == 2 && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	/* Register Vim as an OLE Automation server */
	if (STRICMP(argv[1] + 1, "register") == 0)
	{
	    RegisterMe();
	    mch_windexit(0);
	}

	/* Unregister Vim as an OLE Automation server */
	if (STRICMP(argv[1] + 1, "unregister") == 0)
	{
	    UnregisterMe();
	    mch_windexit(0);
	}

	/* Ignore an -embedding argument. It is only relevant if the
	 * application wants to treat the case when it is started manually
	 * differently from the case where it is started via automation (and
	 * we don't).
	 */
	if (STRICMP(argv[1] + 1, "embedding") == 0)
	    argc = 1;
    }

    InitOLE();
# endif
#endif

#ifdef USE_GUI
    gui_prepare(&argc, argv);	/* Prepare for possibly starting GUI sometime */
#endif

#ifdef USE_CLIPBOARD
    clip_init(FALSE);		/* Initialise clipboard stuff */
#endif

    /*
     * Check if we have an interactive window.
     * On the Amiga: If there is no window, we open one with a newcli command
     * (needed for :! to * work). mch_check_win() will also handle the -d
     * argument.
     */
    stdout_isatty = (mch_check_win(argc, argv) != FAIL);

    /*
     * allocate the first window and buffer. Can't do anything without it
     */
    if ((curwin = win_alloc(NULL)) == NULL ||
			(curbuf = buflist_new(NULL, NULL, 1L, FALSE)) == NULL)
	mch_windexit(0);
    curwin->w_buffer = curbuf;
    curbuf->b_nwindows = 1;	/* there is one window */
    win_init(curwin);		/* init current window */
    init_yank();		/* init yank buffers */

    /*
     * Allocate space for the generic buffers (needed for set_init_1()).
     */
    if ((IObuff = alloc(IOSIZE)) == NULL ||
			    (NameBuff = alloc(MAXPATHL)) == NULL)
	mch_windexit(0);

    /*
     * Set the default values for the options.
     * First find out the home directory, needed to expand "~" in options.
     */
    init_homedir();		/* find real value of $HOME */
    set_init_1();

    /*
     * If the executable name starts with "r" we disable shell commands.
     * If the next character is "g" we run the GUI version.
     * If the next characters are "view" we start in readonly mode.
     * If the next characters are "ex" we start in ex mode.
     */
    initstr = gettail((char_u *)argv[0]);

    if (initstr[0] == 'r')
    {
	restricted = TRUE;
	++initstr;
    }

    if (initstr[0] == 'g')
    {
#ifdef USE_GUI
	gui.starting = TRUE;
	++initstr;
#else
	mch_errmsg((char *)e_nogvim);
	mch_windexit(2);
#endif
    }

    if (STRNCMP(initstr, "view", 4) == 0)
    {
	readonlymode = TRUE;
	curbuf->b_p_ro = TRUE;
	p_uc = 10000;			/* don't update very often */
    }

    if (STRNCMP(initstr, "ex", 2) == 0)
    {
	exmode_active = TRUE;
	change_compatible(TRUE);	/* set 'compatible' */
    }

    /*
     * On some systems, when we compile with the GUI, we always use it.  On Mac
     * there is no terminal version, and on Windows we can't figure out how to
     * fork one off with :gui.
     */
#ifdef ALWAYS_USE_GUI
    gui.starting = TRUE;
#endif

    ++argv;
    --argc;

#ifndef macintosh
    /*
     * Allocate arg_files[], big enough to hold all potential file name
     * arguments.
     */
    arg_files = (char_u **)alloc((unsigned)(sizeof(char_u *) * (argc + 1)));
    if (arg_files == NULL)
	mch_windexit(2);
#else
    arg_files = NULL;
#endif
    arg_file_count = 0;

    /*
     * Process the command line arguments.
     */
    argv_idx = 1;	    /* active option letter is argv[0][argv_idx] */
    while (argc > 0)
    {
	/*
	 * "+" or "+{number}" or "+/{pat}" or "+{command}" argument.
	 */
	if (argv[0][0] == '+' && !had_minmin)
	{
	    if (n_commands >= MAX_ARG_CMDS)
		mainerr(ME_EXTRA_CMD, NULL);
	    argv_idx = -1;	    /* skip to next argument */
	    if (argv[0][1] == NUL)
		commands[n_commands++] = (char_u *)"$";
	    else
		commands[n_commands++] = (char_u *)&(argv[0][1]);
	}

	/*
	 * Option argument.
	 */
	else if (argv[0][0] == '-' && !had_minmin)
	{
	    want_argument = FALSE;
	    c = argv[0][argv_idx++];
	    switch (c)
	    {
	    case NUL:	    /* "-"  read from stdin */
		if (edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		edit_type = EDIT_STDIN;
		read_cmd_fd = 2;	/* read from stderr instead of stdin */
		argv_idx = -1;		/* skip to next argument */
		break;

	    case '-':	    /* "--" don't take any more options */
		had_minmin = TRUE;
		argv_idx = -1;		/* skip to next argument */
		break;

	    case 'b':	    /* "-b" binary mode */
		bin_mode = TRUE;    /* postpone to after reading .exrc files */
		break;

	    case 'C':	    /* "-C"  Compatible */
		change_compatible(TRUE);
		break;

	    case 'e':	    /* "-e" Ex mode */
		exmode_active = TRUE;
		break;

	    case 'f':	    /* "-f"  GUI: run in foreground.  Amiga: open
				window directly, not with newcli */
#ifdef USE_GUI
		gui.dofork = FALSE;	/* don't fork() when starting GUI */
#endif
		break;

	    case 'g':	    /* "-g" start GUI */
#ifdef USE_GUI
		gui.starting = TRUE;	/* start GUI a bit later */
#else
		mch_errmsg((char *)e_nogvim);
		mch_windexit(2);
#endif
		break;

	    case 'F':	    /* "-F" start in Farsi mode: rl + fkmap set */
#ifdef FKMAP
		curwin->w_p_rl = p_fkmap = TRUE;
#else
		mch_errmsg((char *)e_nofarsi);
		mch_windexit(2);
#endif
		break;

	    case 'h':	    /* "-h" give help message */
		usage();
		break;

	    case 'H':	    /* "-H" start in Hebrew mode: rl + hkmap set */
#ifdef RIGHTLEFT
		curwin->w_p_rl = p_hkmap = TRUE;
#else
		mch_errmsg((char *)e_nohebrew);
		mch_windexit(2);
#endif
		break;

	    case 'l':	    /* "-l" lisp mode, 'lisp' and 'showmatch' on */
#ifdef LISPINDENT
		curbuf->b_p_lisp = TRUE;
		p_sm = TRUE;
#endif
		break;

	    case 'N':	    /* "-N"  Nocompatible */
		change_compatible(FALSE);
		break;

	    case 'n':	    /* "-n" no swap file */
		no_swap_file = TRUE;
		break;

	    case 'o':	    /* "-o[N]" open N windows */
		/* default is 0: open window for each file */
		window_count = get_number_arg((char_u *)argv[0], &argv_idx, 0);
		break;

#ifdef QUICKFIX
	    case 'q':	    /* "-q" QuickFix mode */
		if (edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		edit_type = EDIT_QF;
		if (argv[0][argv_idx])		    /* "-q{errorfile}" */
		{
		    use_ef = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		else if (argc > 1)		    /* "-q {errorfile}" */
		    want_argument = TRUE;
		break;
#endif

	    case 'R':	    /* "-R" readonly mode */
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 10000;		    /* don't update very often */
		break;

	    case 'r':	    /* "-r" recovery mode */
	    case 'L':	    /* "-L" recovery mode */
		recoverymode = 1;
		break;

	    case 's':
		if (exmode_active)	/* "-s" silent (batch) mode */
		    silent_mode = TRUE;
		else		/* "-s {scriptin}" read from script file */
		    want_argument = TRUE;
		break;

	    case 't':	    /* "-t {tag}" or "-t{tag}" jump to tag */
		if (edit_type != EDIT_NONE)
		    mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
		edit_type = EDIT_TAG;
		if (argv[0][argv_idx])	    /* "-t{tag}" */
		{
		    tagname = (char_u *)argv[0] + argv_idx;
		    argv_idx = -1;
		}
		else			    /* "-t {tag}" */
		    want_argument = TRUE;
		break;

	    case 'V':	    /* "-V{N}"	Verbose level */
		/* default is 10: a little bit verbose */
		p_verbose = get_number_arg((char_u *)argv[0], &argv_idx, 10);
		break;

	    case 'v':	    /* "-v"  Vi-mode (as if called "vi") */
		exmode_active = FALSE;
		break;

	    case 'w':	    /* "-w{number}"	set window height */
			    /* "-w {scriptout}"	write to script */
		if (isdigit(argv[0][argv_idx]))
		{
		    argv_idx = -1;
		    break;			/* not implemented, ignored */
		}
		want_argument = TRUE;
		break;

	    case 'x':	    /* "-x"  use "crypt" for reading/writing files. */
		/* TODO */
		break;

	    case 'Z':	    /* "-Z"  restricted mode */
		restricted = TRUE;
		break;

	    case 'c':	    /* "-c {command}" execute command */
	    case 'd':	    /* "-d {device}" device (for Amiga) */
	    case 'i':	    /* "-i {viminfo}" use for viminfo */
	    case 'T':	    /* "-T {terminal}" terminal name */
	    case 'u':	    /* "-u {vimrc}" vim inits file */
	    case 'U':	    /* "-U {gvimrc}" gvim inits file */
	    case 'W':	    /* "-W {scriptout}" overwrite */
		want_argument = TRUE;
		break;

	    default:
		mainerr(ME_UNKNOWN_OPTION, (char_u *)argv[0]);
	    }

	    /*
	     * Handle options with argument.
	     */
	    if (want_argument)
	    {
		/*
		 * Check for garbage immediately after the option letter.
		 */
		if (argv[0][argv_idx] != NUL)
		    mainerr(ME_GARBAGE, (char_u *)argv[0]);

		--argc;
		if (argc < 1)
		    mainerr(ME_ARG_MISSING, (char_u *)argv[0]);
		++argv;
		argv_idx = -1;

		switch (c)
		{
		case 'c':	    /* "-c {command}" execute command */
		    if (n_commands >= MAX_ARG_CMDS)
			mainerr(ME_EXTRA_CMD, NULL);
		    commands[n_commands++] = (char_u *)argv[0];
		    break;

	    /*	case 'd':   This is handled in mch_check_win() */

#ifdef QUICKFIX
		case 'q':	    /* "-q {errorfile}" QuickFix mode */
		    use_ef = (char_u *)argv[0];
		    break;
#endif

		case 'i':	    /* "-i {viminfo}" use for viminfo */
		    use_viminfo = (char_u *)argv[0];
		    break;

		case 's':	    /* "-s {scriptin}" read from script file */
		    if (scriptin[0] != NULL)
		    {
			mch_errmsg("Attempt to open script file again: \"");
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_windexit(2);
		    }
		    if ((scriptin[0] = fopen(argv[0], READBIN)) == NULL)
		    {
			mch_errmsg("Cannot open \"");
			mch_errmsg(argv[0]);
			mch_errmsg("\" for reading\n");
			mch_windexit(2);
		    }
		    if (save_typebuf() == FAIL)
			mch_windexit(2);	/* out of memory */
		    break;

		case 't':	    /* "-t {tag}" */
		    tagname = (char_u *)argv[0];
		    break;

		case 'T':	    /* "-T {terminal}" terminal name */
		    /*
		     * The -T term option is always available and when
		     * HAVE_TERMLIB is supported it overrides the environment
		     * variable TERM.
		     */
#ifdef USE_GUI
		    if (term_is_gui((char_u *)argv[0]))
			gui.starting = TRUE;	/* start GUI a bit later */
		    else
#endif
			term = (char_u *)argv[0];
		    break;

		case 'u':	    /* "-u {vimrc}" vim inits file */
		    use_vimrc = (char_u *)argv[0];
		    break;

		case 'U':	    /* "-U {gvimrc}" gvim inits file */
		    use_gvimrc = (char_u *)argv[0];
		    break;

		case 'w':	    /* "-w {scriptout}" append to script file */
		case 'W':	    /* "-W {scriptout}" overwrite script file */
		    if (scriptout != NULL)
		    {
			mch_errmsg("Attempt to open script file again: \"");
			mch_errmsg(argv[-1]);
			mch_errmsg(" ");
			mch_errmsg(argv[0]);
			mch_errmsg("\"\n");
			mch_windexit(2);
		    }
		    if ((scriptout = fopen(argv[0],
				    c == 'w' ? APPENDBIN : WRITEBIN)) == NULL)
		    {
			mch_errmsg("cannot open \"");
			mch_errmsg(argv[0]);
			mch_errmsg("\" for output\n");
			mch_windexit(2);
		    }
		    break;
		}
	    }
	}

	/*
	 * File name argument.
	 */
	else
	{
	    argv_idx = -1;	    /* skip to next argument */
	    if (edit_type != EDIT_NONE && edit_type != EDIT_FILE)
		mainerr(ME_TOO_MANY_ARGS, (char_u *)argv[0]);
	    edit_type = EDIT_FILE;
	    arg_files[arg_file_count] = vim_strsave((char_u *)argv[0]);
	    if (arg_files[arg_file_count] != NULL)
		++arg_file_count;
	}

	/*
	 * If there are no more letters after the current "-", go to next
	 * argument.  argv_idx is set to -1 when the current argument is to be
	 * skipped.
	 */
	if (argv_idx <= 0 || argv[0][argv_idx] == NUL)
	{
	    --argc;
	    ++argv;
	    argv_idx = 1;
	}
    }

    /*
     * May expand wildcards in file names.
     */
    if (arg_file_count > 0)
    {
#if (!defined(UNIX) && !defined(__EMX__)) || defined(ARCHIE)
	char_u	    **new_arg_files;
	int	    new_arg_file_count;

	if (expand_wildcards(arg_file_count, arg_files, &new_arg_file_count,
				    &new_arg_files, EW_FILE|EW_NOTFOUND) == OK
		&& new_arg_file_count != 0)
	{
	    FreeWild(arg_file_count, arg_files);
	    arg_file_count = new_arg_file_count;
	    arg_files = new_arg_files;
	}
#endif
	fname = arg_files[0];
    }
    if (arg_file_count > 1)
	printf("%d files to edit\n", arg_file_count);

    RedrawingDisabled = TRUE;

    /*
     * When listing swap file names, don't do cursor positioning et. al.
     */
    if (recoverymode && fname == NULL)
	want_full_screen = FALSE;

    /*
     * When starting the GUI, don't need to check capabilities of terminal.
     */
#ifdef USE_GUI
    if (gui.starting)
	want_full_screen = FALSE;
#endif

    /*
     * mch_windinit() sets up the terminal (window) for use.  This must be
     * done after resetting full_screen, otherwise it may move the cursor
     * (MSDOS).
     * Note that we may use mch_windexit() before mch_windinit()!
     */
    mch_windinit();

    /*
     * Print a warning if stdout is not a terminal.
     * When starting in Ex mode and commands come from a file, set Silent mode.
     */
    input_isatty = mch_input_isatty();
    if (exmode_active)
    {
	if (!input_isatty)
	    silent_mode = TRUE;
    }
    else if (want_full_screen && (!stdout_isatty || !input_isatty))
    {
	if (!stdout_isatty)
	    mch_errmsg("Vim: Warning: Output is not to a terminal\n");
	if (!input_isatty)
	    mch_errmsg("Vim: Warning: Input is not from a terminal\n");
	ui_delay(2000L, TRUE);
    }

    if (want_full_screen)
    {
	termcapinit(term);	/* set terminal name and get terminal
				   capabilities (will set full_screen) */
	screen_start();		/* don't know where cursor is now */
    }
    screenclear();		/* clear screen (just inits screen structures,
				    because starting is TRUE) */

    /*
     * Set the default values for the options that use Rows and Columns.
     */
    ui_get_winsize();		/* inits Rows and Columns */
    set_init_2();

    firstwin->w_height = Rows - 1;
    cmdline_row = Rows - 1;

    if (full_screen)
	msg_start();	    /* in case a mapping or error message is printed */
    msg_scroll = TRUE;
    no_wait_return = TRUE;

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    /*
     * Default mappings for some often used keys.
     * Need to put string in allocated memory, because do_map() will modify it.
     */
    {
	char_u *cpo_save = p_cpo;

	p_cpo = "";	/* Allow <> notation */
	for (i = 0; i < sizeof(initmappings) / sizeof(struct initmap); ++i)
	{
	    initstr = vim_strsave(initmappings[i].arg);
	    if (initstr != NULL)
	    {
		do_map(0, initstr, initmappings[i].mode, FALSE);
		vim_free(initstr);
	    }
	}
	p_cpo = cpo_save;
    }
#endif

    init_highlight(TRUE);	/* set the default highlight groups */
#ifdef USE_GUI
    parse_guicursor();		/* set cursor shapes from 'guicursor' */
#endif

    /*
     * If -u option given, use only the initializations from that file and
     * nothing else.
     */
    if (use_vimrc != NULL)
    {
	if (STRCMP(use_vimrc, "NONE") == 0)
	{
	    if (use_gvimrc == NULL)	    /* don't load gvimrc either */
		use_gvimrc = use_vimrc;
	}
	else
	{
	    if (do_source(use_vimrc, FALSE, FALSE) != OK)
		EMSG2("Cannot read from \"%s\"", use_vimrc);
	}
    }
    else if (!silent_mode)
    {
	/*
	 * Get system wide defaults, if the file name is defined.
	 */
#ifdef SYS_VIMRC_FILE
	(void)do_source((char_u *)SYS_VIMRC_FILE, TRUE, FALSE);
#endif

	/*
	 * Try to read initialization commands from the following places:
	 * - environment variable VIMINIT
	 * - user vimrc file (s:.vimrc for Amiga, ~/.vimrc otherwise)
	 * - second user vimrc file ($VIM/.vimrc for Dos)
	 * - environment variable EXINIT
	 * - user exrc file (s:.exrc for Amiga, ~/.exrc otherwise)
	 * - second user exrc file ($VIM/.exrc for Dos)
	 * The first that exists is used, the rest is ignored.
	 */
	if (process_env((char_u *)"VIMINIT") == OK)
	    vimrc_found();
	else
	{
	    if (do_source((char_u *)USR_VIMRC_FILE, TRUE, TRUE) == FAIL
#ifdef USR_VIMRC_FILE2
		&& do_source((char_u *)USR_VIMRC_FILE2, TRUE, TRUE) == FAIL
#endif
		&& process_env((char_u *)"EXINIT") == FAIL
		&& do_source((char_u *)USR_EXRC_FILE, FALSE, FALSE) == FAIL)
	    {
#ifdef USR_EXRC_FILE2
		(void)do_source((char_u *)USR_EXRC_FILE2, FALSE, FALSE);
#endif
	    }
	}

	/*
	 * Read initialization commands from ".vimrc" or ".exrc" in current
	 * directory.  This is only done if the 'exrc' option is set.
	 * Because of security reasons we disallow shell and write commands
	 * now, except for unix if the file is owned by the user or 'secure'
	 * option has been reset in environmet of global ".exrc" or ".vimrc".
	 * Only do this if VIMRC_FILE is not the same as USR_VIMRC_FILE or
	 * SYS_VIMRC_FILE.
	 */
	if (p_exrc)
	{
#ifdef UNIX
	    {
		struct stat s;

		/* if ".vimrc" file is not owned by user, set 'secure' mode */
		if (stat(VIMRC_FILE, &s) || s.st_uid != getuid())
		    secure = p_secure;
	    }
#else
	    secure = p_secure;
#endif

	    i = FAIL;
	    if (fullpathcmp((char_u *)USR_VIMRC_FILE,
				      (char_u *)VIMRC_FILE, FALSE) != FPC_SAME
#ifdef USR_VIMRC_FILE2
		    && fullpathcmp((char_u *)USR_VIMRC_FILE2,
				      (char_u *)VIMRC_FILE, FALSE) != FPC_SAME
#endif
#ifdef SYS_VIMRC_FILE
		    && fullpathcmp((char_u *)SYS_VIMRC_FILE,
				      (char_u *)VIMRC_FILE, FALSE) != FPC_SAME
#endif
				)
		i = do_source((char_u *)VIMRC_FILE, TRUE, TRUE);

	    if (i == FAIL)
	    {
#ifdef UNIX
		struct stat s;

		/* if ".exrc" is not owned by user set 'secure' mode */
		if (stat(EXRC_FILE, &s) || s.st_uid != getuid())
		    secure = p_secure;
		else
		    secure = 0;
#endif
		if (	   fullpathcmp((char_u *)USR_EXRC_FILE,
				      (char_u *)EXRC_FILE, FALSE) != FPC_SAME
#ifdef USR_EXRC_FILE2
			&& fullpathcmp((char_u *)USR_EXRC_FILE2,
				      (char_u *)EXRC_FILE, FALSE) != FPC_SAME
#endif
				)
		    (void)do_source((char_u *)EXRC_FILE, FALSE, FALSE);
	    }
	}
	if (secure == 2)
	    need_wait_return = TRUE;
	secure = 0;
    }

    /*
     * Recovery mode without a file name: List swap files.
     * This uses the 'dir' option, therefore it must be after the
     * initializations.
     */
    if (recoverymode && fname == NULL)
    {
	recover_names(NULL, TRUE, 0);
	mch_windexit(0);
    }

    /*
     * Set a few option defaults after reading .vimrc files:
     * 'title' and 'icon', Unix: 'shellpipe' and 'shellredir'.
     */
    set_init_3();

    /*
     * "-n" argument: Disable swap file by setting 'updatecount' to 0.
     * Note that this overrides anything from a vimrc file.
     */
    if (no_swap_file)
	p_uc = 0;

#ifdef FKMAP
    if (curwin->w_p_rl && p_altkeymap)
    {
	p_hkmap = FALSE;	/* Reset the Hebrew keymap mode */
	p_fkmap = TRUE;		/* Set the Farsi keymap mode */
    }
#endif

    if (bin_mode)		    /* "-b" argument used */
    {
	set_options_bin(curbuf->b_p_bin, 1);
	curbuf->b_p_bin = 1;	    /* binary file I/O */
    }

#ifdef USE_GUI
    if (gui.starting)
	gui_start();		/* will set full_screen to TRUE */
#endif

#ifdef VIMINFO
    /*
     * Read in registers, history etc, but not marks, from the viminfo file
     */
    if (*p_viminfo != NUL)
	read_viminfo(NULL, TRUE, FALSE, FALSE);
#endif /* VIMINFO */

#ifdef SPAWNO		/* special MSDOS swapping library */
    init_SPAWNO("", SWAP_ANY);
#endif

#ifdef QUICKFIX
    /*
     * "-q errorfile": Load the error file now.
     * If the error file can't be read, exit before doing anything else.
     */
    if (edit_type == EDIT_QF)
    {
	if (use_ef != NULL)
	    set_string_option_direct((char_u *)"ef", -1, use_ef, TRUE);
	if (qf_init(p_ef) < 0)
	{
	    out_char('\n');
	    mch_windexit(3);
	}
    }
#endif

    /*
     * Don't set the file name if there was a command in .vimrc that already
     * loaded the file
     */
    if (curbuf->b_ffname == NULL)
    {
	(void)setfname(fname, NULL, TRUE);  /* includes maketitle() */
	++arg_idx;			    /* used first argument name */
    }

    if (window_count == 0)
	window_count = arg_file_count;
    if (window_count > 1)
    {
	/* Don't change the windows if there was a command in .vimrc that
	 * already split some windows */
	if (firstwin->w_next == NULL)
	    window_count = make_windows(window_count);
	else
	    window_count = win_count();
    }
    else
	window_count = 1;

    /*
     * Start putting things on the screen.
     * Scroll screen down before drawing over it
     * Clear screen now, so file message will not be cleared.
     */
    starting = FALSE;
    no_wait_return = FALSE;
    msg_scroll = FALSE;

#ifdef USE_GUI
    /*
     * This seems to be required to make callbacks to be called now, instead
     * of after things have been put on the screen, which then may be deleted
     * when getting a resize callback.
     */
    if (gui.in_use)
	gui_wait_for_chars(50L);
#endif

    /*
     * When done something that is not allowed or error message call
     * wait_return.  This must be done before starttermcap(), because it may
     * switch to another screen. It must be done after settmode(TMODE_RAW),
     * because we want to react on a single key stroke.
     * Call settmode and starttermcap here, so the T_KS and T_TI may be
     * defined by termcapinit and redifined in .exrc.
     */
    settmode(TMODE_RAW);
    if (need_wait_return || msg_didany)
	wait_return(TRUE);

    starttermcap();	    /* start termcap if not done by wait_return() */
#ifdef USE_MOUSE
    setmouse();				/* may start using the mouse */
#endif
    if (scroll_region)
	scroll_region_reset();		/* In case Rows changed */

    scroll_start();
    /*
     * Don't clear the screen when starting in Ex mode, unless using the GUI.
     */
    if (exmode_active
#ifdef USE_GUI
			&& !gui.in_use
#endif
					)
	must_redraw = CLEAR;
    else
	screenclear();			/* clear screen */

    no_wait_return = TRUE;

    if (recoverymode)			/* do recover */
    {
	msg_scroll = TRUE;		/* scroll message up */
	ml_recover();
	msg_scroll = FALSE;
	if (curbuf->b_ml.ml_mfp == NULL) /* failed */
	    getout(1);
	do_modelines();			/* do modelines */
    }
    else
    {
	/*
	 * If "-" argument given: read file from stdin.
	 * Need to stop Raw mode for terminal in case stdin and stderr are the
	 * same terminal: "cat | vim -".
	 */
	if (edit_type == EDIT_STDIN)
	{
	    stoptermcap();
	    settmode(TMODE_COOK);	/* set to normal mode */
	    (void)open_buffer(TRUE);	/* create memfile and read file */
	    if (!termcap_active)	/* if readfile() didn't do it already */
	    {
		settmode(TMODE_RAW);	/* set to raw mode */
		starttermcap();
	    }
	}

	/*
	 * Open a buffer for windows that don't have one yet.
	 * Commands in the .vimrc might have loaded a file or split the window.
	 * Watch out for autocommands that delete a window.
	 */
#ifdef AUTOCMD
	/*
	 * Don't execute Win/Buf Enter/Leave autocommands here
	 */
	++autocmd_no_enter;
	++autocmd_no_leave;
#endif
	for (curwin = firstwin; curwin != NULL; curwin = curwin->w_next)
	{
	    curbuf = curwin->w_buffer;
	    if (curbuf->b_ml.ml_mfp == NULL)
	    {
		(void)open_buffer(FALSE);   /* create memfile and read file */
#ifdef AUTOCMD
		curwin = firstwin;	    /* start again */
#endif
	    }
	    ui_breakcheck();
	    if (got_int)
	    {
		(void)vgetc();	/* only break the file loading, not the rest */
		break;
	    }
	}
#ifdef AUTOCMD
	--autocmd_no_enter;
	--autocmd_no_leave;
#endif
	curwin = firstwin;
	curbuf = curwin->w_buffer;
    }

    /* Ex starts at last line of the file */
    if (exmode_active)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;

#ifdef AUTOCMD
    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE);
#endif
    setpcmark();

#ifdef QUICKFIX
    /*
     * When started with "-q errorfile" jump to first error now.
     */
    if (edit_type == EDIT_QF)
	qf_jump(0, 0, FALSE);
#endif

    /*
     * If opened more than one window, start editing files in the other
     * windows.  Make_windows() has already opened the windows.
     */
#ifdef AUTOCMD
    /*
     * Don't execute Win/Buf Enter/Leave autocommands here
     */
    ++autocmd_no_enter;
    ++autocmd_no_leave;
#endif
    for (i = 1; i < window_count; ++i)
    {
	if (curwin->w_next == NULL)	    /* just checking */
	    break;
	win_enter(curwin->w_next, FALSE);

	/* Only open the file if there is no file in this window yet (that can
	 * happen when .vimrc contains ":sall") */
	if (curbuf == firstwin->w_buffer || curbuf->b_ffname == NULL)
	{
	    curwin->w_arg_idx = arg_idx;
	    /* edit file from arg list, if there is one */
	    (void)do_ecmd(0,
			 arg_idx < arg_file_count ? arg_files[arg_idx] : NULL,
					  NULL, NULL, (linenr_t)0, ECMD_HIDE);
	    if (arg_idx == arg_file_count - 1)
		arg_had_last = TRUE;
	    ++arg_idx;
	}
	ui_breakcheck();
	if (got_int)
	{
	    (void)vgetc();	/* only break the file loading, not the rest */
	    break;
	}
    }
#ifdef AUTOCMD
    --autocmd_no_enter;
#endif
    win_enter(firstwin, FALSE);		    /* back to first window */
#ifdef AUTOCMD
    --autocmd_no_leave;
#endif
    if (window_count > 1)
	win_equal(curwin, FALSE);	    /* adjust heights */

    /*
     * If there are more file names in the argument list than windows,
     * put the rest of the names in the buffer list.
     */
    while (arg_idx < arg_file_count)
	(void)buflist_add(arg_files[arg_idx++]);

    /*
     * Need to jump to the tag before executing the '-c command'.
     * Makes "vim -c '/return' -t main" work.
     */
    if (tagname)
    {
	STRCPY(IObuff, "ta ");
	STRCAT(IObuff, tagname);
	do_cmdline(IObuff, NULL, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);
    }

    if (n_commands > 0)
    {
	/*
	 * We start commands on line 0, make "vim +/pat file" match a
	 * pattern on line 1.
	 */
	curwin->w_cursor.lnum = 0;
	sourcing_name = (char_u *)"command line";
	for (i = 0; i < n_commands; ++i)
	    do_cmdline(commands[i], NULL, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);
	sourcing_name = NULL;
	if (curwin->w_cursor.lnum == 0)
	    curwin->w_cursor.lnum = 1;

#ifdef QUICKFIX
	/* When started with "-q errorfile" jump to first again. */
	if (edit_type == EDIT_QF)
	    qf_jump(0, 0, FALSE);
#endif
    }

    RedrawingDisabled = FALSE;
    redraw_later(NOT_VALID);
    no_wait_return = FALSE;

    /* start in insert mode */
    if (p_im)
	need_start_insertmode = TRUE;

    /*
     * main command loop
     */
    clear_oparg(&oa);
    for (;;)
    {
	if (stuff_empty())
	{
	    if (need_check_timestamps)
		check_timestamps();
	    if (need_wait_return)	/* if wait_return still needed ... */
		wait_return(FALSE);	/* ... call it now */
	    if (need_start_insertmode)
	    {
		need_start_insertmode = FALSE;
		stuffReadbuff((char_u *)"i");	/* start insert mode next */
		/* skip the fileinfo message now, because it would be shown
		 * after insert mode finishes! */
		need_fileinfo = FALSE;
	    }
	}
	dont_wait_return = FALSE;
	if (got_int && !global_busy)
	{
	    (void)vgetc();		/* flush all buffers */
	    got_int = FALSE;
	}
	msg_scroll = FALSE;
	quit_more = FALSE;

	/*
	 * If skip redraw is set (for ":" in wait_return()), don't redraw now.
	 * If there is nothing in the stuff_buffer or do_redraw is TRUE,
	 * update cursor and redraw.
	 */
	if (skip_redraw || exmode_active)
	    skip_redraw = FALSE;
	else if (do_redraw || stuff_empty())
	{
	    /*
	     * Before redrawing, make sure w_topline is correct, and w_leftcol
	     * if lines don't wrap.
	     */
	    update_topline();
	    if (!curwin->w_p_wrap)
		validate_cursor();

	    if (VIsual_active)
		update_curbuf(INVERTED);/* update inverted part */
	    else if (must_redraw)
		update_screen(must_redraw);
	    else if (redraw_cmdline || clear_cmdline)
		showmode();
	    /* display message after redraw */
	    if (keep_msg != NULL)
		msg_attr(keep_msg, keep_msg_attr);
	    if (need_fileinfo)		/* show file info after redraw */
	    {
		fileinfo(FALSE, TRUE, FALSE);
		need_fileinfo = FALSE;
	    }

	    emsg_on_display = FALSE;	/* can delete error message now */
	    msg_didany = FALSE;		/* reset lines_left in msg_start() */
	    do_redraw = FALSE;
	    showruler(FALSE);

	    setcursor();
	    cursor_on();
	}

	/*
	 * Update w_curswant if w_set_curswant has been set.
	 * Postponed until here to avoid computing w_virtcol too often.
	 */
	update_curswant();

	/*
	 * If we're invoked as ex, do a round of ex commands.
	 * Otherwise, get and execute a normal mode command.
	 */
	if (exmode_active)
	    do_exmode();
	else
	    normal_cmd(&oa, TRUE);
    }
    /*NOTREACHED*/
}
#endif /* PROTO */

/*
 * Get a (optional) count for a Vim argument.
 */
    static int
get_number_arg(p, idx, def)
    char_u	*p;	    /* pointer to argument */
    int		*idx;	    /* index in argument, is incremented */
    int		def;	    /* default value */
{
    if (isdigit(p[*idx]))
    {
	def = atoi((char *)&(p[*idx]));
	while (isdigit(p[*idx]))
	    *idx = *idx + 1;
    }
    return def;
}

/*
 * Get an evironment variable, and execute it as Ex commands.
 * Returns FAIL if the environment variable was not executed, OK otherwise.
 */
    int
process_env(env)
    char_u	*env;
{
    char_u	*initstr;

    if ((initstr = vim_getenv(env)) != NULL && *initstr != NUL)
    {
	sourcing_name = env;
	do_cmdline(initstr, NULL, NULL, DOCMD_NOWAIT|DOCMD_VERBOSE);
	sourcing_name = NULL;
	return OK;
    }
    return FAIL;
}

    void
getout(r)
    int		    r;
{
    exiting = TRUE;

    /* Position the cursor on the last screen line, below all the text */
#ifdef USE_GUI
    if (!gui.in_use)
#endif
	windgoto((int)Rows - 1, 0);

#ifdef HAVE_PERL_INTERP
    perl_end();
#endif

#ifdef VIMINFO
    if (*p_viminfo != NUL)
    {
	/* Write out the registers, history, marks etc, to the viminfo file */
	msg_didany = FALSE;
	write_viminfo(NULL, FALSE);
	if (msg_didany)		/* make the user read the error message */
	{
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	}
    }
#endif /* VIMINFO */

#ifdef AUTOCMD
    apply_autocmds(EVENT_VIMLEAVE, NULL, NULL, FALSE);

    /* Position the cursor again, the autocommands may have moved it */
# ifdef USE_GUI
    if (!gui.in_use)
# endif
	windgoto((int)Rows - 1, 0);
#endif

    mch_windexit(r);
}

/*
 * When FKMAP is defined, also compile the Farsi source code.
 */
#ifdef FKMAP
# include "farsi.c"
#endif
