/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 * Multibyte extensions by Sung-Hoon Baek
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 *	file : multbyte.c
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "option.h"
#ifdef WIN32
# include <windows.h>
# include <winnls.h>
#endif

#if defined(MULTI_BYTE) || defined(PROTO)

/*
 * Is 'c' a lead byte of multi-byte character?
 */
    int
IsLeadByte(c)
    int		c;
{
#ifdef WIN32
    /* is_dbcs is set by setting 'fileencoding'.  It becomes a Windows
     * CodePage identifier, which we can pass directly in to Windows API*/
    return IsDBCSLeadByteEx(is_dbcs, (BYTE)c);
#else
    return (c & 0x80);
#endif
}

/*
 * Is *p a trail byte of multi-byte character?  base : string pointer to line
 */
    int
IsTrailByte(base, p)
    char_u *base;
    char_u *p;
{
    int lbc = 0;    /* lead byte count*/

    if (base >= p)
	return 0;

    while (p > base)
    {
	if (!IsLeadByte(*(--p)))
	    break;
	lbc++;
    }

    return (lbc & 1);
}

/*
 * if the cursor moves on an trail byte, set the cursor on the lead byte.
 */
    int
AdjustCursorForMultiByteCharacter()
{
    char_u *p;

    if (curwin->w_cursor.col > 0 )
    {
	p = ml_get(curwin->w_cursor.lnum);
	if (IsTrailByte(p, p + curwin->w_cursor.col))
	{
	    --curwin->w_cursor.col;
	    return 1;
	}
    }
    return 0;
}

/*
 * count the length of the str which has multi-byte characters.  two-byte
 * character counts as one character.
 */
    int
MultiStrLen(str)
    char_u	*str;
{
    int count;

    if (str == NULL)
	return 0;
    for (count = 0; *str != NUL; count++)
    {
	if (IsLeadByte(*str))
	{
	    str++;
	    if (*str != NUL)
		str++;
	}
	else
	    str++;
    }
    return count;
}

    int
han_dec(lp)
    FPOS  *lp;
{
    char_u *p = ml_get(lp->lnum);

    if (lp->col > 0)
    {		/* still within line */
	lp->col--;
	if ( lp->col > 0 && IsTrailByte(p, p + lp->col))
	    lp->col--;
	return 0;
    }
    if (lp->lnum > 1)
    {		/* there is a prior line */
	lp->lnum--;
	lp->col = STRLEN(ml_get(lp->lnum));
	if ( lp->col > 0 && IsTrailByte(p, p + lp->col))
	    lp->col--;
	return 1;
    }
    return -1;			/* at start of file */
}

#endif /* MULTI_BYTE */
