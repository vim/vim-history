/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#define EXTERN
#include "vim.h"

	static void
usage()
{
	register int i, j;
	static char *(use[]) = {"[file] ..\n",
							"-t tag\n",
							"+[num] file ..\n",
							"+/pat file ..\n",
							"-e\n"};

	fprintf(stderr, "usage: vim [-v] [-n] [-r] [-s scriptin] [-w scriptout] ");
	for (i = 0; ; ++i)
	{
		fprintf(stderr, use[i]);
		if (i == (sizeof(use) / sizeof(char *)) - 1)
			break;
		for (j = 55; --j >= 0; )
			putc(' ', stderr);
	}
	windexit(1);
}

#ifdef AMIGA
	void
#else
	int
#endif
main(argc, argv)
	int 			argc;
	char		  **argv;
{
	char		   *initstr;		/* init string from the environment */
	char		   *fname = NULL;	/* file name from command line */
	int 			c;
	int				doqf = 0;
	char		   *command = NULL;	/* command from + option */

#ifdef AMIGA
	/*
	 * Check if we have an interactive window.
	 * If not, open one with a newcli command (needed for :! to work).
	 */
	check_win(argc, argv);
#endif

	windinit();
	set_init();

	++argv;
	/*
	 * Process the command line arguments
	 * 		'-s scriptin'
	 *		'-w scriptout'
	 *		'-v'
	 *		'-n'
	 *		'-r'
	 */
	while (argc > 1 && argv[0][0] == '-' && strchr("swvnr", c = argv[0][1]) != NULL && c)
	{
		--argc;
		if (c == 'v')
		{
			readonlymode = TRUE;
			P(P_RO) = TRUE;
		}
		else if (c == 'n')
		{
			P(P_UC) = 0;
		}
		else if (c == 'r')
		{
			recoverymode = 1;
		}
		else
		{
			++argv;
			--argc;
			if (argc < 1)
				usage();
			if (c == 's')
			{
				if ((scriptin[0] = fopen(argv[0], "r")) == NULL)
				{
						fprintf(stderr, "cannot open %s for reading\n", argv[0]);
						windexit(2);
				}
			}
			else	/* c == 'w' */
			{
				if ((scriptout = fopen(argv[0], "a")) == NULL)
				{
						fprintf(stderr, "cannot open %s for output\n", argv[0]);
						windexit(2);
				}
			}
		}
		++argv;
	}

	/*
	 * Allocate space for the generic buffer
	 */
	if ((IObuff = alloc(IOSIZE)) == NULL)
		windexit(0);

	/*
	 * Process the other command line arguments.
	 */
	if (argc > 1)
	{
		c = argv[0][1];
		switch (argv[0][0])
		{
		  case '-':
		  	if (c == 'e')		/* -e QuickFix mode */
			{
				if (argc != 2)
					usage();
				if (qf_init(PS(P_EF)))
					windexit(3);
				doqf = 1;
				break;
			}
			if (c != 't' || argc != 3)
				usage();
								/* -t tag */
			++argv;
			stuffReadbuff(":ta ");
			stuffReadbuff(argv[0]);
			stuffReadbuff("\n");
			break;

		  case '+': 			/* + or +n or +/pat or +command */
			if (argc < 3)		/* no filename */
					usage();
			if (c == NUL)
				command = "$";
			else
				command = &(argv[0][1]);

			++argv;
			--argc;
			/*FALLTHROUGH*/

		  default:				/* must be a file name */
#ifdef WILD_CARDS
			ExpandWildCards(argc - 1, argv, &numfiles, &files, (bool_t)TRUE, (bool_t)TRUE);
			if (numfiles != 0)
				fname = files[0];
#else
			files = argv;
			numfiles = argc - 1;
			fname = argv[0];
#endif
			if (numfiles > 1)
				printf("%d files to edit\n", numfiles);
			break;
		}
	}

	if (numfiles == 0)
		numfiles = 1;

	filealloc();				/* Initialize storage structure */
	screenalloc();				/* allocate screen buffers */
	init_yank();				/* init yank buffers */

/*
 * Read the EXINIT environment variable (commands are to be separated with '|').
 * If there is none, read initialization commands from "s:.exrc".
 */
	if ((initstr = getenv("EXINIT")) != NULL)
		docmdline((u_char *)initstr);
	else
		dosource("s:.exrc");

/*
 * read initialization commands from ".exrc"
 */
	dosource(".exrc");

#ifdef AMIGA
	fname_case(fname);		/* set correct case for file name */
#endif
	setfname(fname);
	maketitle();
	if (Filename != NULL)
		readfile(Filename, (linenr_t)0);
	else
		msg("Empty Buffer");

	setpcmark();
	startscript();				/* start writing to auto script file */

	if (recoverymode && !scriptin[curscript])	/* first do script file, then recover */
		openrecover();

	updateScreen(NOT_VALID);

	/* position the display and the cursor at the top of the file. */
	Topline = 1;
	Curpos.lnum = 1;
	Curpos.col = 0;
	Cursrow = Curscol = 0;

	if (doqf)
		qf_jump(0);

	if (command)
		docmdline(command);

/*
 * main command loop
 */
	for (;;)
	{
		adjustCurpos();
		cursupdate();	/* Figure out where the cursor is based on Curpos. */

		windgoto(Cursrow, Curscol);

		normal();		/* execute a command */
	}
	/*NOTREACHED*/
}

	void
getout(r)
	int 			r;
{
	windgoto(Rows - 1, 0);
	outchar('\r');
	outchar('\n');
	windexit(r);
}
