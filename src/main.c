/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

#define EXTERN
#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

#ifdef SPAWNO
# include <spawno.h>			/* special MSDOS swapping library */
#endif

static void usage __PARMS((int));

	static void
usage(n)
	int n;
{
	register int i;
	static char_u *(use[]) = {(char_u *)"[file ..]\n",
							(char_u *)"-t tag\n",
							(char_u *)"-e [errorfile]\n"};
	static char_u *(errors[]) =  {(char_u *)"Unknown option\n",		/* 0 */
								(char_u *)"Too many arguments\n",	/* 1 */
								(char_u *)"Argument missing\n",		/* 2 */
								};

	fprintf(stderr, (char *)errors[n]);
	fprintf(stderr, "usage:");
	for (i = 0; ; ++i)
	{
		fprintf(stderr, " vim [options] ");
		fprintf(stderr, (char *)use[i]);
		if (i == (sizeof(use) / sizeof(char_u *)) - 1)
			break;
		fprintf(stderr, "   or:");
	}
	fprintf(stderr, "\noptions:\t-v\t\treadonly mode (view)\n");
	fprintf(stderr, "\t\t-n\t\tno swap file, use memory only\n");
	fprintf(stderr, "\t\t-b\t\tbinary mode\n");
	fprintf(stderr, "\t\t-r\t\trecovery mode\n");
#ifdef AMIGA
	fprintf(stderr, "\t\t-x\t\tdon't use newcli to open window\n");
	fprintf(stderr, "\t\t-d device\tuse device for I/O\n");
#endif
	fprintf(stderr, "\t\t-T terminal\tset terminal type\n");
	fprintf(stderr, "\t\t-o[N]\t\topen N windows (def: one for each file)\n");
	fprintf(stderr, "\t\t+\t\tstart at end of file\n");
	fprintf(stderr, "\t\t+lnum\t\tstart at line lnum\n");
	fprintf(stderr, "\t\t-c command\texecute command first\n");
	fprintf(stderr, "\t\t-s scriptin\tread commands from script file\n");
	fprintf(stderr, "\t\t-w scriptout\twrite commands in script file\n");
	mch_windexit(1);
}

#ifdef USE_LOCALE
# include <locale.h>
#endif

	void
main(argc, argv)
	int				argc;
	char		  **argv;
{
	char_u		   *initstr;		/* init string from the environment */
	char_u		   *term = NULL;	/* specified terminal name */
	char_u		   *fname = NULL;	/* file name from command line */
	char_u		   *command = NULL;	/* command from + or -c option */
	char_u		   *tagname = NULL;	/* tag from -t option */
	int 			c;
	int				doqf = 0;
	int				i;
	int				bin_mode = FALSE;	/* -b option used */
	int				win_count = 1;		/* number of windows to use */

#ifdef USE_LOCALE
	setlocale(LC_ALL, "");		/* for ctype() and the like */
#endif

/*
 * Check if we have an interactive window.
 * If not, open one with a newcli command (needed for :! to work).
 * check_win will also handle the -d argument (for the Amiga).
 */
	check_win(argc, argv);

/*
 * allocate the first window and buffer. Can't to anything without it
 */
	if ((curwin = win_alloc(NULL)) == NULL ||
			(curbuf = buflist_new(NULL, NULL, 1L, FALSE)) == NULL)
		mch_windexit(0);
	curwin->w_buffer = curbuf;

/*
 * If the executable is called "view" we start in readonly mode.
 */
	if (STRCMP(gettail((char_u *)argv[0]), (char_u *)"view") == 0)
	{
		readonlymode = TRUE;
		curbuf->b_p_ro = TRUE;
		p_uc = 0;
	}

	++argv;
	/*
	 * Process the command line arguments
	 *		'-c {command}'	execute command
	 *		'+{command}'	execute command
	 * 		'-s scriptin'	read from script file
	 *		'-w scriptout'	write to script file
	 *		'-v'			view
	 *		'-b'			binary
	 *		'-n'			no .vim file
	 *		'-r'			recovery mode
	 *		'-x'			open window directly, not with newcli
	 *		'-o[N]'			open N windows (default: number of files)
	 *		'-T terminal'	terminal name
	 */
	while (argc > 1 && ((c = argv[0][0]) == '+' || (c == '-' &&
			strchr("vnbrxocswTd", c = argv[0][1]) != NULL && c != NUL)))
	{
		--argc;
		switch (c)
		{
		case '+': 			/* + or +{number} or +/{pat} or +{command} */
			c = argv[0][1];
			if (c == NUL)
				command = (char_u *)"$";
			else
				command = (char_u *)&(argv[0][1]);
			break;

		case 'v':
			readonlymode = TRUE;
			curbuf->b_p_ro = TRUE;
			/*FALLTHROUGH*/

		case 'n':
			p_uc = 0;
			break;

		case 'b':
			bin_mode = TRUE;		/* postpone to after reading .exrc files */
			break;

		case 'r':
			recoverymode = 1;
			break;
		
		case 'x':
			break;	/* This is ignored as it is handled in check_win() */

		case 'o':
			c = argv[0][2];
			if (c != NUL && !isdigit(c))
			{
				fprintf(stderr, "-o option needs numeric argument (or none)\n");
				mch_windexit(2);
			}
			win_count = atoi(&(argv[0][2]));		/* 0 means: number of files */
			break;

		default:	/* options with argument */
			++argv;
			--argc;
			if (argc < 1)
				usage(2);

			switch (c)
			{
			case 'c':			/* -c {command} */
				command = (char_u *)&(argv[0][0]);
				break;

			case 's':
				if ((scriptin[0] = fopen(argv[0], READBIN)) == NULL)
				{
					fprintf(stderr, "cannot open %s for reading\n", argv[0]);
					mch_windexit(2);
				}
				break;
			
			case 'w':
				if ((scriptout = fopen(argv[0], APPENDBIN)) == NULL)
				{
					fprintf(stderr, "cannot open %s for output\n", argv[0]);
					mch_windexit(2);
				}
				break;

/*
 * The -T term option is always available and when TERMCAP is supported it
 * overrides the environment variable TERM.
 */
			case 'T':
				term = (char_u *)*argv;
				break;
			
		/*	case 'd':		This is ignored as it is handled in check_win() */
			}
		}
		++argv;
	}

	/*
	 * Allocate space for the generic buffers
	 */
	if ((IObuff = alloc(IOSIZE)) == NULL || (NameBuff = alloc(MAXPATHL)) == NULL)
		mch_windexit(0);

	/* note that we may use mch_windexit() before mch_windinit()! */
	mch_windinit();
	set_init();					/* after mch_windinit because Rows is used */
	firstwin->w_height = Rows - 1;
	cmdline_row = Rows - 1;

	/*
	 * Process the other command line arguments.
	 */
	if (argc > 1)
	{
		c = argv[0][1];
		switch (argv[0][0])
		{
		  case '-':
		    switch (c)
			{
		  	case 'e':			/* -e QuickFix mode */
				switch (argc)
				{
					case 2:
							if (argv[0][2])		/* -eerrorfile */
								p_ef = (char_u *)argv[0] + 2;
							break;				/* -e */

					case 3:						/* -e errorfile */
							++argv;
							p_ef = (char_u *)argv[0];
							break;

					default:					/* argc > 3: too many arguments */
							usage(1);
				}
				doqf = 1;
				break;

			case 't':			/* -t tag  or -ttag */
				switch (argc)
				{
					case 2:
							if (argv[0][2])		/* -ttag */
							{
								tagname = (char_u *)argv[0] + 2;
								break;
							}
							usage(2);			/* argument missing */
							break;

					case 3:						/* -t tag */
							++argv;
							tagname = (char_u *)argv[0];
							break;

					default:					/* argc > 3: too many arguments */
							usage(1);
				}
				break;

			default:
				usage(0);
			}
			break;

		  default:				/* must be a file name */
#if !defined(UNIX) || defined(ARCHIE)
			if (ExpandWildCards(argc - 1, (char_u **)argv, &arg_count,
					&arg_files, TRUE, TRUE) == OK && arg_count != 0)
			{
				fname = arg_files[0];
				arg_exp = TRUE;
			}
#else
			arg_files = (char_u **)argv;
			arg_count = argc - 1;
			fname = (char_u *)argv[0];
#endif
			if (arg_count > 1)
				printf("%d files to edit\n", arg_count);
			break;
		}
	}

	RedrawingDisabled = TRUE;

	curbuf->b_nwindows = 1;		/* there is one window */
	win_init(curwin);			/* init cursor position */
	init_yank();				/* init yank buffers */
	termcapinit(term);			/* get terminal capabilities */
	screenclear();				/* clear screen (just inits screen structures,
									because starting is TRUE) */

#ifdef MSDOS /* default mapping for some often used keys */
	domap(0, "#1 :help\r", NORMAL);			/* F1 is help key */
	domap(0, "\316R i", NORMAL);			/* INSERT is 'i' */
	domap(0, "\316S \177", NORMAL);			/* DELETE is 0x7f */
	domap(0, "\316G 0", NORMAL);			/* HOME is '0' */
	domap(0, "\316w H", NORMAL);			/* CTRL-HOME is 'H' */
	domap(0, "\316O $", NORMAL);			/* END is '$' */
	domap(0, "\316u L", NORMAL);			/* CTRL-END is 'L' */
	domap(0, "\316I \002", NORMAL);			/* PageUp is '^B' */
	domap(0, "\316\204 1G", NORMAL);		/* CTRL-PageUp is '1G' */
	domap(0, "\316Q \006", NORMAL);			/* PageDown is '^F' */
	domap(0, "\316v G", NORMAL);			/* CTRL-PageDown is 'G' */
			/* insert mode */
	domap(0, "#1 \017:help\r", INSERT);		/* F1 is help key */
	domap(0, "\316R \033", INSERT);			/* INSERT is ESC */
			/* note: extra space needed to avoid the same memory used for this
			   and the one above, domap() will add a NUL to it */
	domap(0, "\316S  \177", INSERT+CMDLINE);	/* DELETE is 0x7f */
	domap(0, "\316G \017""0", INSERT);		/* HOME is '^O0' */
	domap(0, "\316w \017H", INSERT);		/* CTRL-HOME is '^OH' */
	domap(0, "\316O \017$", INSERT);		/* END is '^O$' */
	domap(0, "\316u \017L", INSERT);		/* CTRL-END is '^OL' */
	domap(0, "\316I \017\002", INSERT);		/* PageUp is '^O^B' */
	domap(0, "\316\204 \017\061G", INSERT);	/* CTRL-PageUp is '^O1G' */
	domap(0, "\316Q \017\006", INSERT);		/* PageDown is '^O^F' */
	domap(0, "\316v \017G", INSERT);		/* CTRL-PageDown is '^OG' */
#endif

	msg_start();		/* in case a mapping is printed */
	no_wait_return = TRUE;

/*
 * get system wide defaults (for unix)
 */
#ifdef DEFVIMRC_FILE
	(void)dosource(DEFVIMRC_FILE);
#endif

/*
 * Try to read initialization commands from the following places:
 * - environment variable VIMINIT
 * - file s:.vimrc ($HOME/.vimrc for Unix)
 * - environment variable EXINIT
 * - file s:.exrc ($HOME/.exrc for Unix)
 * The first that exists is used, the rest is ignored.
 */
	if ((initstr = vimgetenv((char_u *)"VIMINIT")) != NULL)
		docmdline(initstr);
	else if (dosource((char_u *)SYSVIMRC_FILE) == FAIL)
	{
		if ((initstr = vimgetenv((char_u *)"EXINIT")) != NULL)
			docmdline(initstr);
		else
			(void)dosource((char_u *)SYSEXRC_FILE);
	}

/*
 * Read initialization commands from ".vimrc" or ".exrc" in current directory.
 * This is only done if the 'exrc' option is set.
 * Because of security reasons we disallow shell and write commands now,
 * except for unix if the file is owned by the user or 'secure' option has been
 * reset in environmet of global ".exrc" or ".vimrc".
 * Only do this if VIMRC_FILE is not the same as SYSVIMRC_FILE or DEFVIMRC_FILE.
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
		if (fullpathcmp((char_u *)SYSVIMRC_FILE, (char_u *)VIMRC_FILE)
#ifdef DEFVIMRC_FILE
				&& fullpathcmp((char_u *)DEFVIMRC_FILE, (char_u *)VIMRC_FILE)
#endif
				)
			i = dosource((char_u *)VIMRC_FILE);
#ifdef UNIX
		if (i == FAIL)
		{
			struct stat s;

				/* if ".exrc" file is not owned by user set 'secure' mode */
			if (stat(EXRC_FILE, &s) || s.st_uid != getuid())
				secure = p_secure;
			else
				secure = 0;
		}
#endif
		if (i == FAIL && fullpathcmp((char_u *)SYSEXRC_FILE, (char_u *)EXRC_FILE))
			(void)dosource((char_u *)EXRC_FILE);
	}

#ifdef SPAWNO			/* special MSDOS swapping library */
	init_SPAWNO("", SWAP_ANY);
#endif
/*
 * Call settmode and starttermcap here, so the T_KS and T_TS may be defined
 * by termcapinit and redifined in .exrc.
 */
	settmode(1);
	starttermcap();

	no_wait_return = FALSE;
		/* done something that is not allowed or error message */
	if (secure == 2 || need_wait_return)
		wait_return(TRUE);		/* must be called after settmode(1) */
	secure = 0;

	if (bin_mode)					/* -b option used */
	{
		curbuf->b_p_bin = 1;		/* binary file I/O */
		curbuf->b_p_tw = 0;			/* no automatic line wrap */
		curbuf->b_p_wm = 0;			/* no automatic line wrap */
		curbuf->b_p_tx = 0;			/* no text mode */
		p_ta = 0;					/* no text auto */
		curbuf->b_p_ml = 0;			/* no modelines */
		curbuf->b_p_et = 0;			/* no expand tab */
	}

	(void)setfname(fname, NULL, TRUE);
	maketitle();

	if (win_count == 0)
		win_count = arg_count;
	if (win_count > 1)
		win_count = make_windows(win_count);
	else
		win_count = 1;

/*
 * Start putting things on the screen.
 * Scroll screen down before drawing over it
 * Clear screen now, so file message will not be cleared.
 */
	starting = FALSE;
	if (T_CVV != NULL && *T_CVV)
	{
		outstr(T_CVV);
		outstr(T_CV);
	}
	screenclear();						/* clear screen */

	if (recoverymode)					/* do recover */
	{
		if (ml_open() == FAIL)			/* Initialize storage structure */
			getout(1);
		ml_recover();
	}
	else
		(void)open_buffer();			/* create memfile and read file */

	setpcmark();

	if (doqf && qf_init() == FAIL)		/* if reading error file fails: exit */
		mch_windexit(3);

	/*
	 * If opened more than one window, start editing files in the other windows.
	 * Make_windows() has already opened the windows.
	 * This is all done by putting commands in the stuff buffer.
	 */
	for (i = 1; i < win_count; ++i)
	{
		if (curwin->w_next == NULL)			/* just checking */
			break;
		win_enter(curwin->w_next, FALSE);
											/* edit file i, if there is one */
		(void)doecmd(i < arg_count ? arg_files[i] : NULL,
											NULL, NULL, TRUE, (linenr_t)1);
		curwin->w_arg_idx = i;
	}
	win_enter(firstwin, FALSE);				/* back to first window */

	/*
	 * If there are more file names in the argument list than windows,
	 * put the rest of the names in the buffer list.
	 */
	for (i = win_count; i < arg_count; ++i)
		(void)buflist_add(arg_files[i]);

	if (command)
		docmdline(command);
	/*
	 * put the :ta command in the stuff buffer here, so that it will not
	 * be erased by an emsg().
	 */
	if (tagname)
	{
		stuffReadbuff((char_u *)":ta ");
		stuffReadbuff(tagname);
		stuffReadbuff((char_u *)"\n");
	}

	RedrawingDisabled = FALSE;
	updateScreen(NOT_VALID);

		/* start in insert mode (already taken care of for :ta command) */
	if (p_im && stuff_empty())
		stuffReadbuff((char_u *)"i");
/*
 * main command loop
 */
	for (;;)
	{
		if (got_int)
		{
			(void)vgetc();				/* flush all buffers */
			got_int = FALSE;
		}
		adjust_cursor();				/* put cursor on an existing line */
		if (skip_redraw)				/* skip redraw (for ":" in wait_return()) */
			skip_redraw = FALSE;
		else if (stuff_empty())			/* only when no command pending */
		{
			cursupdate();				/* Figure out where the cursor is based
											on curwin->w_cursor. */
			if (VIsual.lnum)
				updateScreen(INVERTED);	/* update inverted part */
			if (must_redraw)
				updateScreen(must_redraw);
			if (keep_msg)
				msg(keep_msg);			/* display message after redraw */

			showruler(FALSE);

			setcursor();
			cursor_on();
		}

		normal();						/* get and execute a command */
	}
	/*NOTREACHED*/
}

	void
getout(r)
	int 			r;
{
	windgoto((int)Rows - 1, 0);
	outchar('\r');
	outchar('\n');
	mch_windexit(r);
}
