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
#include "globals.h"
#include "proto.h"
#include "param.h"


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
		else if (c <= '~' || c > 0xa0 || p_gr)
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
 * output 'len' characters in 'str' (including NULs) with translation
 * if 'len' is -1, output upto a NUL character
 * return the number of characters it takes on the screen
 */
	int
outtrans(str, len)
	register char *str;
	register int   len;
{
	int retval = 0;

	if (len == -1)
		len = strlen(str);
	while (--len >= 0)
	{
		outstrn(transchar(*(u_char *)str));
		retval += charsize(*(u_char *)str);
		++str;
	}
	return retval;
}

/*
 * return the number of characters 'c' will take on the screen
 */
	int
charsize(c)
	int c;
{
	return ((c >= ' ' && (p_gr || c <= '~')) || c > 0xa0 ? 1 : 2);
}

/*
 * return the number of characters string 's' will take on the screen
 */
	int
strsize(s)
	char *s;
{
	int	len = 0;

	while (*s)
		len += charsize(*s++);
	return len;
}

/*
 * return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab
 */
	int
chartabsize(c, col)
	int c, col;
{
	if ((c >= ' ' && (c <= '~' || p_gr)) || c > 0xa0)
   		return 1;
   	else if (c == TAB && !p_list)
   		return (int)(p_ts - (col % p_ts));
   	else
		return 2;
}

/*
 * return TRUE if 'c' is an identifier character
 */
	int
isidchar(c)
	int c;
{
#ifdef __STDC__
		return (isalnum(c) || c == '_');
#else
		return (isalpha(c) || isdigit(c) || c == '_');
#endif
}
