/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"


	char_u *
transchar(c)
	int	 c;
{
	static char_u buf[3];

	if (c < ' ' || c == DEL)
	{
		if (c == NL)
			c = NUL;			/* we use newline in place of a NUL */
		buf[0] = '^';
		buf[1] = c ^ 0x40;		/* DEL displayed as ^? */
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
	char_u *s;
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
	register int	c;
	long			col;
{
	if ((c >= ' ' && (c <= '~' || p_gr)) || c > 0xa0)
   		return 1;
   	else if (c == TAB && !curwin->w_p_list)
   		return (int)(curbuf->b_p_ts - (col % curbuf->b_p_ts));
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
		return (
#ifdef __STDC__
				isalnum(c)
#else
				isalpha(c) || isdigit(c)
#endif
				|| c == '_'
	/*
	 * we also accept alhpa's with accents
	 */
#ifdef MSDOS
				|| (c >= 0x80 && c <= 0xa7) || (c >= 0xe0 && c <= 0xeb)
#else
				|| (c >= 0xc0 && c <= 0xff)
#endif
				);
}
