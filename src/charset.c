/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#include "vim.h"

	char *
transchar(c)
	unsigned c;
{
		static char buf[3];

		if (c < ' ')
		{
				if (c == NL)
						c = NUL;		/* we use newline in place of a NUL */
				buf[0] = '^';
				buf[1] = '@' + c;
				buf[2] = NUL;
		}
		else if (c <= '~' || c > 0xa0)
		{
				buf[0] = c;
				buf[1] = NUL;
		}
		else
		{
				buf[0] = '~';
				buf[1] = c - 0x80 + '@';
				buf[2] = NUL;
		}
		return buf;
}

/*
 * output 'len' characters in 'str' with translation
 * if 'len' is -1, output upto a NUL character
 */
	void
outtrans(str, len)
	register char *str;
	register int   len;
{
	if (len == -1)
		len = strlen(str);
	while (--len >= 0)
	{
		outstr(transchar(*(u_char *)str));
		++str;
	}
}

/*
 * return the number of characters 'c' will take on the screen
 */
charsize(c)
{
	return (c >= ' ' && c <= '~' || c > 0xa0 ? 1 : 2);
}

/*
 * return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab
 */
chartabsize(c, col)
	int c, col;
{
	if (c >= ' ' && c <= '~' || c > 0xa0)
   		return 1;
   	else if (c == TAB && !P(P_LS))
   		return (P(P_TS) - (col % P(P_TS)));
   	else
		return 2;
}

/*
 * return TRUE if 'c' is an identifier character
 */
isidchar(c)
{
#ifdef __STDC__
		return (isalnum(c) || c == '_');
#else
		return (isalpha(c) || isdigit(c) || c == '_');
#endif
}
