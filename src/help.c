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
 * help.c: display help from the vim.hlp file
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

static long helpfilepos;		/* position in help file */
static FILE *helpfd;			/* file descriptor of help file */

#define MAXSCREENS 52			/* one screen for a-z and A-Z */

	void
help()
{
	int		c;
	int		eof;
	int		screens;
	int		i;
	long	filepos[MAXSCREENS];	/* seek position for each screen */
	int		screennr;			/* screen number; index == 0, 'c' == 1, 'd' == 2, etc */
#ifdef MSDOS
	char	*fnamep;
#endif

/*
 * try to open the file specified by the "helpfile" option
 */
	if ((helpfd = fopen(p_hf, READBIN)) == NULL)
	{
#ifdef MSDOS
	/*
	 * for MSDOS: try the DOS search path
     */
		fnamep = searchpath("vim.hlp");
		if (fnamep == NULL || (helpfd = fopen(fnamep, READBIN)) == NULL)
		{
			smsg("Sorry, help file \"%s\" and \"vim.hlp\" not found", p_hf);
			return;
		}
#else
		smsg("Sorry, help file \"%s\" not found", p_hf);
		return;
#endif
	}
	helpfilepos = 0;
	screennr = 0;
	for (i = 0; i < MAXSCREENS; ++i)
		filepos[i] = 0;
	State = HELP;
	for (;;)
	{
		screens = redrawhelp();				/* show one or more screens */
		eof = (screens < 0);
		if (!eof && screennr + screens < MAXSCREENS)
			filepos[screennr + screens] = ftell(helpfd);

		if ((c = vgetc()) == '\n' || c == '\r' || c == Ctrl('C') || c == ESC)
			break;

		if (c == ' ' ||
#ifdef MSDOS
				(c == K_NUL && vpeekc() == 'Q') ||	/* page down */
#endif
				c == Ctrl('F'))						/* one screen forwards */
		{
			if (screennr < MAXSCREENS && !eof)
				++screennr;
		}
		else if (c == 'a')					/* go to first screen */
			screennr = 0;
		else if (c == 'b' ||
#ifdef MSDOS
				(c == K_NUL && vpeekc() == 'I') ||	/* page up */
#endif
				c == Ctrl('B'))					/* go one screen backwards */
		{
			if (screennr > 0)
				--screennr;
		}
		else if (isalpha(c))				/* go to specified screen */
		{
			if (isupper(c))
				c = c - 'A' + 'z' + 1;		/* 'A' comes after 'z' */
			screennr = c - 'b';
		}
#ifdef MSDOS
		if (c == K_NUL)
			c = vgetc();
#endif
		for (i = screennr; i > 0; --i)
			if (filepos[i])
				break;
		fseek(helpfd, filepos[i], 0);
		while (i < screennr)
		{
			while ((c = getc(helpfd)) != '\f' && c != -1)
				;
			if (c == -1)
				break;
			filepos[++i] = ftell(helpfd);	/* store the position just after the '\f' */
		}
		screennr = i;						/* required when end of file reached */
		helpfilepos = filepos[screennr];
	}
	State = NORMAL;
	script_winsize_pp();
	fclose(helpfd);
	updateScreen(CLEAR);
}

	int
redrawhelp()
{
	int nextc;
	int col;
	int	line = 0;
	int	screens = 1;

	fseek(helpfd, helpfilepos, 0);
	outstr(T_ED);
	windgoto(0,0);
	while ((nextc = getc(helpfd)) != -1 && (nextc != '\f' || line < Rows - 24))
	{
		if (nextc == Ctrl('B'))			/* begin of invert */
			outstr(T_TI);
		else if (nextc == Ctrl('E'))	/* end of invert */
			outstr(T_TP);
		else if (nextc == '\f')			/* start of next screen */
		{
			++screens;
			outchar('\n');
			++line;
		}
		else
		{
			outchar((char)nextc);
			if (nextc == '\n')
				++line;
		}
	}
	windgoto(0, (int)(Columns - strlen(Version) - 1));
	outstrn(Version);
	col = (int)Columns - 52;
	if (col < 0)
		col = 0;
	windgoto((int)Rows - 1, col);
	outstrn("<space = next; return = quit; a = index; b = back>");
	return (nextc == -1 ? -1 : screens);
}
