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
 * help.c: display help from the vim.hlp file
 */

#include "vim.h"

static long helpfilepos;		/* position in help file */
static FILE *helpfd;			/* file descriptor of help file */

	void
help()
{
	int c;
	int eof;
	int i;
	long filepos[26];	/* seek position for each screen */
	int	screennr;		/* screen number; 'c' == 1 */

	if ((helpfd = fopen("vim.hlp", "r")) == NULL &&
						(helpfd = fopen("vim:vim.hlp", "r")) == NULL)
	{
		msg("Sorry, help file not found");
		return;
	}
	helpfilepos = 0;
	screennr = 0;
	for (i = 0; i < 26; ++i)
		filepos[i] = 0;
	State = HELP;
	for (;;)
	{
		eof = redrawhelp();
		if (!eof)
			filepos[screennr + 1] = ftell(helpfd);

		if ((c = vgetc()) == '\n' || c == '\r')
			break;

		if (c == ' ')						/* one screen forwards */
		{
			if (screennr < 25 && !eof)
				++screennr;
		}
		else if (c == 'a')					/* go to first screen */
			screennr = 0;
		else if (c == 'b')					/* go one screen backwards */
		{
			if (screennr > 0)
				--screennr;
		}
		else if (c >= 'c' && c <= 'z')		/* go to specified screen */
		{
			if (c - 'b' < screennr)			/* backwards */
				screennr = c - 'b';
			else							/* forwards */
			{
				while (screennr < c - 'b' && filepos[screennr + 1])
					++screennr;
				fseek(helpfd, filepos[screennr], 0);
				while (screennr < c - 'b')
				{
					while ((i = getc(helpfd)) != '\f' && i != -1)
						;
					if (i == -1)
						break;
					filepos[++screennr] = ftell(helpfd);
				}
			}
		}
		helpfilepos = filepos[screennr];
	}
	State = NORMAL;
	script_winsize_pp();
	fclose(helpfd);
	screenclear();
	updateScreen(NOT_VALID);
}

redrawhelp()
{
		int nextc;
		int col;

		fseek(helpfd, helpfilepos, 0);
		col = Columns - 52;
		if (col < 0)
				col = 0;
		outstr(T_ED);
		while ((nextc = getc(helpfd)) != -1 && nextc != '\f')
				outchar((char)nextc);
		windgoto(0, (int)(Columns - strlen(Version) - 1));
		outstr(Version);
		windgoto(Rows - 1, col);
		outstr("<space = next; return = quit; a = index; b = back>");
		return (nextc == -1);
}
