/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 * Multibyte extensions by Sung-Hoon Baek
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 * file : multibyte.c
 */

#include "vim.h"
#include "globals.h"
#include "option.h"
#ifdef WIN32
# include <windows.h>
#ifndef __MINGW32__
# include <winnls.h>
#endif
#endif
#ifdef FEAT_GUI_X11
# include <X11/Intrinsic.h>
#endif
#ifdef X_LOCALE
#include <X11/Xlocale.h>
#endif

#if defined(FEAT_MBYTE) || defined(PROTO)

/*
 * Code specifically for handling multi-byte characters.
 *
 * Currently these types of character encodings are supported:
 *
 * "cc_dbcs"	    When non-zero it tells the type of double byte character
 *		    encoding (Chinese, Korean, Japanese, etc.).
 *		    The cell width on the display is always equal to the
 *		    number of bytes.
 *		    Recognizing the first or second byte is difficult, it
 *		    requires checking a byte sequence from the start.
 * "cc_utf8"	    When TRUE use Unicode characters in UTF-8 encoding.
 *		    The cell width on the display needs to be determined from
 *		    the character value.
 *		    Recognizing bytes is easy: 0xxx.xxxx is a single-byte
 *		    char, 10xx.xxxx is a trailing byte, 11xx.xxxx is a leading
 *		    byte of a multi-byte character.
 *		    To make things complicated, up to two composing characters
 *		    are allowed.  These are drawn on top of the first char.
 *		    For most editing the sequence of bytes with composing
 *		    characters included is considered to be one character.
 * "cc_unicode"	    When 2 use 16-bit Unicode characters.
 *		    When 4 use 32-but Unicode characters.
 *		    Internally characters are stored in UTF-8 encoding to
 *		    avoid NUL bytes.  Conversion happens when doing I/O.
 *		    "cc_utf8" will also be TRUE.
 *
 * "has_mbyte" is set when "cc_dbcs" or "cc_utf8" is non-zero.
 *
 * If none of these is TRUE 8-bit bytes are used for a character.
 */

/* Lookup table to quickly get the length in bytes of a UTF-8 character from
 * the first byte of an UTF-8 string.  Byte which are illegal when used as the
 * first byte have a one, because these will be used separately. */
static char utf8len_tab[256] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /*bogus*/
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /*bogus*/
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1,
};

/*
 * Set up for using multi-byte characters.
 * Called when 'charcode' has been changed.
 * Sets cc_unicode, cc_utf8 and cc_dbcs flags for new value of 'charcode'.
 * Fills mb_bytelen_tab[].
 * When there is something wrong: Returns an error message and doesn't change
 * anything.  Returns NULL when there are no problems.
 */
    char_u *
mb_init()
{
    int		i;
    int		n;
    int		cc_dbcs_new = 0;

    if (p_cc == NULL)
    {
	/* Just starting up: set the whole table to one's. */
	for (i = 0; i < 256; ++i)
	    mb_bytelen_tab[i] = 1;
	return NULL;
    }

    cc_unicode = 0;
    cc_utf8 = FALSE;
    if (STRCMP(p_cc, CC_DBJPN) == 0)
	cc_dbcs_new = DBCS_JPN;
    else if (STRCMP(p_cc, CC_DBKOR) == 0)
	cc_dbcs_new = DBCS_KOR;
    else if (STRCMP(p_cc, CC_DBCHT) == 0)
	cc_dbcs_new = DBCS_CHT;
    else if (STRCMP(p_cc, CC_DBCHS) == 0)
	cc_dbcs_new = DBCS_CHS;
    else if (STRCMP(p_cc, CC_UNICODE) == 0
	    || STRCMP(p_cc, CC_UCS2) == 0
	    || STRCMP(p_cc, CC_UCS2BE) == 0
	    || STRCMP(p_cc, CC_UCS2LE) == 0)
    {
	/* 16 bit Unicode is stored internally as UTF-8 */
	cc_unicode = 2;
	cc_utf8 = TRUE;
    }
    else if (STRCMP(p_cc, CC_UCS4) == 0
	    || STRCMP(p_cc, CC_UCS4BE) == 0
	    || STRCMP(p_cc, CC_UCS4LE) == 0
	    || STRCMP(p_cc, CC_UCS4BL) == 0
	    || STRCMP(p_cc, CC_UCS4LB) == 0)
    {
	/* 32 bit Unicode is stored internally as UTF-8 */
	cc_unicode = 4;
	cc_utf8 = TRUE;
    }
    else if (STRCMP(p_cc, CC_UTF8) == 0)
	cc_utf8 = TRUE;
#ifdef MB_DEBUG
    else if (STRCMP(p_cc, CC_DEBUG) == 0)
	cc_dbcs_new = -1;
#endif
    /* else: must be "ansi" or "latin-1". */

#ifdef FEAT_GUI_W32
    if (cc_dbcs_new != 0)
    {
	/* Check if the DBCS code page is OK. */
	if (!IsValidCodePage(cc_dbcs_new))
	    return (char_u *)N_("Not a valid codepage");
    }
#endif
    cc_dbcs = cc_dbcs_new;
#ifdef FEAT_GUI_W32
    is_funky_dbcs = cc_dbcs && ((int)GetACP() != cc_dbcs);
#endif

    has_mbyte = (cc_dbcs || cc_utf8);

    /*
     * Fill the mb_bytelen_tab[] for MB_BYTE2LEN().
     */
    for (i = 0; i < 256; ++i)
    {
	/* Our own function to reliably check the lenght of UTF-8 characters,
	 * independent of mblen(). */
	if (cc_utf8)
	    n = utf8len_tab[i];
	else if (cc_dbcs == 0)
	    n = 1;
#ifdef MB_DEBUG
	else if (cc_dbcs == -1)
	{
	    /* Debugging DBCS: make 0xc0-0xff chars double-width. */
	    if (i >= 0xc0)
		n = 2;
	    else
		n = 1;
	}
#endif
	else
	{
#ifdef WIN32
	    /* cc_dbcs is set by setting 'filecharcode'.  It becomes a Windows
	     * CodePage identifier, which we can pass directly in to Windows
	     * API */
	    n = IsDBCSLeadByteEx(cc_dbcs, (BYTE)i) ? 2 : 1;
#else
# ifdef macintosh
	    /*
	     * if mblen() is not available, character which MSB is turned on
	     * are treated as leading byte character. (note : This assumption
	     * is not always true.)
	     */
	    n = (i & 0x80) ? 2 : 1;
# else
	    char buf[MB_MAXBYTES];
# ifdef X_LOCALE
	    extern int _Xmblen __ARGS((char *, size_t));
#  ifndef mblen
#   define mblen _Xmblen
#  endif
# endif
	    if (i == NUL)	/* just in case mblen() can't handle "" */
		n = 1;
	    else
	    {
		/*
		 * mblen() should return -1 for invalid (means the leading
		 * multibyte) character.  However there are some platform
		 * where mblen() returns 0 for invalid character.  Therefore,
		 * following condition includes 0.
		 */
		buf[0] = i;
		buf[1] = 0;
		if (mblen(buf, (size_t)1) <= 0)
		    n = 2;
		else
		    n = 1;
	    }
# endif
#endif
	}

	mb_bytelen_tab[i] = n;
    }

    /* The cell width depends on the type of multi-byte characters. */
    (void)init_chartab();

    /* When cc_utf8 is set or reset, (de)allocate ScreenLinesUC[] */
    screenalloc(FALSE);

    /* When using Unicode, set default for 'filecharcodes'. */
    if (cc_utf8 && !option_was_set((char_u *)"fccs"))
	set_string_option_direct((char_u *)"fccs", -1,
				 (char_u *)"ucs-bom,utf-8,latin-1", OPT_FREE);

#ifdef FEAT_AUTOCMD
    /* Fire an autocommand to let people do custom font setup. This must be
     * after Vim has been setup for the new charcode. */
    apply_autocmds(EVENT_CHARCODE, NULL, (char_u *)"", FALSE, curbuf);
#endif

    return NULL;
}

/*
 * Return the size of the BOM for the current buffer:
 * 0 - no BOM
 * 2 - UCS-2 BOM
 * 4 - UCS-4 BOM
 * 3 - UTF-8 BOM
 */
    int
bomb_size()
{
    int n = 0;

    if (curbuf->b_p_bomb && !curbuf->b_p_bin)
    {
	if (*curbuf->b_p_fcc == NUL)
	{
	    if (cc_utf8)
	    {
		if (cc_unicode != 0)
		    n = cc_unicode;
		else
		    n = 3;
	    }
	}
	else if (STRCMP(curbuf->b_p_fcc, "utf-8") == 0)
	    n = 3;
	else if (STRNCMP(curbuf->b_p_fcc, "ucs-2", 5) == 0)
	    n = 2;
	else if (STRNCMP(curbuf->b_p_fcc, "ucs-4", 5) == 0)
	    n = 4;
    }
    return n;
}

/*
 * Get class of pointer.
 */
    int
mb_get_class(p)
    char_u	*p;
{
    int this_class = 0;

    if (cc_dbcs && p[0] != NUL && p[1] != NUL && MB_BYTE2LEN(p[0]) > 1)
	this_class = mb_class(p[0], p[1]);
    return this_class;
}

/*
 * Get class of a multibyte character.
 */
    int
mb_class(lead, trail)
    unsigned	lead;
    unsigned	trail;
{
    switch (cc_dbcs)
    {
	/* please add classfy routine for your language in here */

	case DBCS_JPN:
	    {
		/* JIS code classification */
		unsigned char lb = lead;
		unsigned char tb = trail;

		/* convert process code to JIS */
# if defined(WIN32) || defined(macintosh)
		/* process code is SJIS */
		if (lb <= 0x9f)
		    lb = (lb - 0x81) * 2 + 0x21;
		else
		    lb = (lb - 0xc1) * 2 + 0x21;
		if (tb <= 0x7e)
		    tb -= 0x1f;
		else if (tb <= 0x9e)
		    tb -= 0x20;
		else
		{
		    tb -= 0x7e;
		    lb += 1;
		}
# else
		/*
		 * XXX: Code page identification can not use with all
		 *	    system! So, some other encoding information
		 *	    will be needed.
		 *	    In japanese: SJIS,EUC,UNICODE,(JIS)
		 *	    Note that JIS-code system don't use as
		 *	    process code in most system because it uses
		 *	    escape sequences(JIS is context depend encoding).
		 */
		/* assume process code is JAPANESE-EUC */
		lb &= 0x7f;
		tb &= 0x7f;
# endif
		/* exceptions */
		switch (lb << 8 | tb)
		{
		    case 0x213c: /* prolongedsound handled as KATAKANA */
			return 13;
		}
		/* sieved by KU code */
		switch (lb)
		{
		    case 0x21:
		    case 0x22:
			/* special symbols */
			return 10;
		    case 0x23:
			/* alpha-numeric */
			return 11;
		    case 0x24:
			/* hiragana */
			return 12;
		    case 0x25:
			/* katakana */
			return 13;
		    case 0x26:
			/* greek */
			return 14;
		    case 0x27:
			/* russian */
			return 15;
		    case 0x28:
			/* lines */
			return 16;
		    default:
			/* kanji */
			return 17;
		}
	    }
	case DBCS_KOR:
	    {
		/* KS code classification */
		unsigned char c1 = lead;
		unsigned char c2 = trail;

		/*
		 * 20 : Hangul
		 * 21 : Hanja
		 * 22 : Symbols
		 * 23 : Alpha-numeric/Roman Letter (Full width)
		 * 24 : Hangul Letter(Alphabet)
		 * 25 : Roman Numeral/Greek Letter
		 * 26 : Box Drawings
		 * 27 : Unit Symbols
		 * 28 : Circled/Parenthesized Letter
		 * 29 : Hirigana/Katakana
		 * 30 : Cyrillic Letter
		 */

		if (c1 >= 0xB0 && c1 <= 0xC8)
		    /* Hangul */
		    return 20;
#ifdef WIN32
		else if (c1 <= 0xA0 || c2 <= 0xA0)
		    /* Extended Hangul Region : MS UHC(Unified Hangul Code) */
		    /* c1: 0x81-0xA0 with c2: 0x41-0x5A, 0x61-0x7A, 0x81-0xFE
		     * c1: 0xA1-0xC6 with c2: 0x41-0x5A, 0x61-0x7A, 0x81-0xA0
		     */
		    return 20;
#endif

		else if (c1 >= 0xCA && c1 <= 0xFD)
		    /* Hanja */
		    return 21;
		else switch (c1)
		{
		    case 0xA1:
		    case 0xA2:
			/* Symbols */
			return 22;
		    case 0xA3:
			/* Alpha-numeric */
			return 23;
		    case 0xA4:
			/* Hangul Letter(Alphabet) */
			return 24;
		    case 0xA5:
			/* Roman Numeral/Greek Letter */
			return 25;
		    case 0xA6:
			/* Box Drawings */
			return 26;
		    case 0xA7:
			/* Unit Symbols */
			return 27;
		    case 0xA8:
		    case 0xA9:
			if (c2 <= 0xAF)
			    return 25;  /* Roman Letter */
			else if (c2 >= 0xF6)
			    return 22;  /* Symbols */
			else
			    /* Circled/Parenthesized Letter */
			    return 28;
		    case 0xAA:
		    case 0xAB:
			/* Hirigana/Katakana */
			return 29;
		    case 0xAC:
			/* Cyrillic Letter */
			return 30;
		}
	    }
	default:
	    break;
    }
    return 3;
}

/*
 * Return length in bytes of character "c".
 * Returns 1 for a single-byte character.
 */
    int
mb_char2len(c)
    int		c;
{
    if (c < 0x80 || !has_mbyte)	/* be quick for ASCII */
	return 1;

    if (cc_utf8)
	return utf_char2len(c);

    /* cc_dbcs */
    if (c >= 0x100)
	return 2;
    return 1;
}

/*
 * Convert a character to its bytes.
 * Returns the length in bytes.
 */
    int
mb_char2bytes(c, buf)
    int		c;
    char_u	*buf;
{
    if (c < 0x80 || !has_mbyte)	/* be quick for ASCII */
    {
	buf[0] = c;
	return 1;
    }

    if (cc_utf8)
	return utf_char2bytes(c, buf);

    /* cc_dbcs */
    if (c >= 0x100)
    {
	buf[0] = (unsigned)c >> 8;
	buf[1] = c;
	return 2;
    }
    buf[0] = c;
    return 1;
}

/*
 * Get byte length of character at "*p" but stop at a NUL.
 * For UTF-8 this includes following composing characters.
 */
    int
mb_ptr2len_check(p)
    char_u	*p;
{
    int		len;

    if (cc_utf8)
	return utfc_ptr2len_check(p);

    /* Latin-1 or DBCS: up to two bytes */
    len = MB_BYTE2LEN(*p);
    if (len == 1 || p[1] == NUL)
	return 1;

    return len;
}

/*
 * For UTF-8 character "c" return 2 for a double-width character, 1 for others.
 */
    int
utf_char2cells(c)
    int		c;
{
    if (c <= 0x9f && c >= 0x80)	    /* unprintable, displays <xx> */
	return 4;
    if (c >= 0x1100
	    && (c <= 0x115f			/* Hangul Jamo */
		|| (c >= 0x2e80 && c <= 0xa4cf && (c & ~0x0011) != 0x300a
		    && c != 0x303f)		/* CJK ... Yi */
		|| (c >= 0xac00 && c <= 0xd7a3)	/* Hangul Syllables */
		|| (c >= 0xf900 && c <= 0xfaff)	/* CJK Compatibility
						   Ideographs */
		|| (c >= 0xfe30 && c <= 0xfe6f)	/* CJK Compatibility Forms */
		|| (c >= 0xff00 && c <= 0xff5f)	/* Fullwidth Forms */
		|| (c >= 0xffe0 && c <= 0xffe6)))
	return 2;
    return 1;
}

/*
 * Return the number of display cells character at "*p" occupies.
 * This doesn't take care of ASCII characters, use ptr2cells() for that.
 */
    int
mb_ptr2cells(p)
    char_u	*p;
{
    int		c;

    /* For DBCS we only need to check the first byte. */
    if (cc_dbcs != 0)
	return MB_BYTE2LEN(*p);

    /* For UTF-8 we need to convert to a wide character. */
    if (cc_utf8 && *p >= 0x80)
    {
	c = utf_ptr2char(p);
	/* An illegal byte is displayed as <xx>. */
	if (utf_ptr2len_check(p) == 1 || c == NUL)
	    return 4;
	/* If the char is ASCII it must be an overlong sequence. */
	if (c < 0x80)
	    return char2cells(c);
	return utf_char2cells(c);
    }

    return 1;
}

/*
 * Return the number of display cells character "c" occupies.
 * Only takes care of multi-byte chars, not "^C" and such.
 */
    int
mb_char2cells(c)
    int		c;
{
    /* For DBCS we only need to check the first byte. */
    if (cc_dbcs != 0)
	return MB_BYTE2LEN((unsigned)c >> 8);

    /* For UTF-8 we need to check the value. */
    if (cc_utf8)
	return utf_char2cells(c);

    return 1;
}

/*
 * Return number of display cells for char at ScreenLines[off].
 * Caller must make sure "off" and "off + 1" are valid!
 */
    int
mb_off2cells(off)
    unsigned	off;
{
    if (cc_dbcs != 0)
	return MB_BYTE2LEN(ScreenLines[off]);
    if (cc_utf8)
	return ScreenLines[off + 1] == 0 ? 2 : 1;
    return 1;
}

/*
 * Convert a byte sequence into a character.
 */
    int
mb_ptr2char(p)
    char_u	*p;
{
    if (cc_utf8)
	return utf_ptr2char(p);
    if (cc_dbcs && MB_BYTE2LEN(*p) > 1)
	return (p[0] << 8) + p[1];
    return *p;
}

/*
 * Convert a UTF-8 byte sequence to a wide character.
 * If the sequence is illegal or truncated by a NUL the first byte is
 * returned.
 */
    int
utf_ptr2char(p)
    char_u	*p;
{
    int		len;

    if (p[0] < 0x80)	/* be quick for ASCII */
	return p[0];

    len = utf8len_tab[p[0]];
    if ((p[1] & 0xc0) == 0x80)
    {
	if (len == 2)
	    return ((p[0] & 0x1f) << 6) + (p[1] & 0x3f);
	if ((p[2] & 0xc0) == 0x80)
	{
	    if (len == 3)
		return ((p[0] & 0x0f) << 12) + ((p[1] & 0x3f) << 6)
		    + (p[2] & 0x3f);
	    if ((p[3] & 0xc0) == 0x80)
	    {
		if (len == 4)
		    return ((p[0] & 0x07) << 18) + ((p[1] & 0x3f) << 12)
			+ ((p[2] & 0x3f) << 6) + (p[3] & 0x3f);
		if ((p[4] & 0xc0) == 0x80)
		{
		    if (len == 5)
			return ((p[0] & 0x03) << 24) + ((p[1] & 0x3f) << 18)
			    + ((p[2] & 0x3f) << 12) + ((p[3] & 0x3f) << 6)
			    + (p[4] & 0x3f);
		    if ((p[5] & 0xc0) == 0x80 && len == 6)
			return ((p[0] & 0x01) << 30) + ((p[1] & 0x3f) << 24)
			    + ((p[2] & 0x3f) << 18) + ((p[3] & 0x3f) << 12)
			    + ((p[4] & 0x3f) << 6) + (p[5] & 0x3f);
		}
	    }
	}
    }
    /* Illegal value, just return the first byte */
    return p[0];
}

/*
 * Convert a UTF-8 byte string to a wide chararacter.  Also get up to two
 * composing characters.
 */
    int
utfc_ptr2char(p, p1, p2)
    char_u	*p;
    int		*p1;	/* return: first composing char or 0 */
    int		*p2;	/* return: second composing char or 0 */
{
    int		len;
    int		c;
    int		cc;

    c = utf_ptr2char(p);
    len = utf_ptr2len_check(p);
    /* Only accept a composing char when the first char isn't illegal. */
    if ((len > 1 || *p < 0x80)
	    && p[len] >= 0x80 && utf_iscomposing(cc = utf_ptr2char(p + len)))
    {
	*p1 = cc;
	len += utf_ptr2len_check(p + len);
	if (p[len] >= 0x80 && utf_iscomposing(cc = utf_ptr2char(p + len)))
	    *p2 = cc;
	else
	    *p2 = 0;
    }
    else
    {
	*p1 = 0;
	*p2 = 0;
    }
    return c;
}

/*
 * Convert the character at screen position "off" to a sequence of bytes.
 * Includes the composing characters.
 * "buf" must at least have the length MB_MAXBYTES.
 * Returns the produced number of bytes.
 */
    int
utfc_char2bytes(off, buf)
    int		off;
    char_u	*buf;
{
    int		len;

    len = utf_char2bytes(ScreenLinesUC[off], buf);
    if (ScreenLinesC1[off] != 0)
    {
	len += utf_char2bytes(ScreenLinesC1[off], buf + len);
	if (ScreenLinesC2[off] != 0)
	    len += utf_char2bytes(ScreenLinesC2[off], buf + len);
    }
    return len;
}

/*
 * Get the length of a UTF-8 byte sequence.  Ignores any following composing
 * characters.
 * Returns 0 for "".
 * Returns 1 for an illegal byte sequence.
 */
    int
utf_ptr2len_check(p)
    char_u	*p;
{
    int		len;
    int		i;

    if (*p == NUL)
	return 0;
    len = utf8len_tab[*p];
    for (i = 1; i < len; ++i)
	if ((p[i] & 0xc0) != 0x80)
	    return 1;
    return len;
}

/*
 * Return length of UTF-8 character, obtained from the first byte.
 * "b" must be between 0 and 255!
 */
    int
utf_byte2len(b)
    int		b;
{
    return utf8len_tab[b];
}

/*
 * Get the length of UTF-8 byte sequence "p[size]".  Ignores any following
 * composing characters.
 * Returns 1 for "".
 * Returns 1 for an illegal byte sequence.
 * Returns number > "size" for an incomplete byte sequence.
 */
    int
utf_ptr2len_check_len(p, size)
    char_u	*p;
    int		size;
{
    int		len;
    int		i;

    if (*p == NUL)
	return 1;
    len = utf8len_tab[*p];
    for (i = 1; i < len && i < size; ++i)
	if ((p[i] & 0xc0) != 0x80)
	    return 1;
    return len;
}

/*
 * Return the number of bytes the UTF-8 encoding of the character at "p" takes.
 * This includes following composing characters.
 */
    int
utfc_ptr2len_check(p)
    char_u	*p;
{
    int		len;

    if (*p == NUL)
	return 0;
    if (p[0] < 0x80 && p[1] < 0x80)	/* be quick for ASCII */
	return 1;

    /* Skip over first UTF-8 char, stopping at a NUL byte. */
    len = utf_ptr2len_check(p);

    /* Check for illegal byte. */
    if (len == 1 && p[0] >= 0x80)
	return 1;

    /*
     * Check for composing characters.  We can handle only the first two, but
     * skip all of them (otherwise the cursor would get stuck).
     */
    for (;;)
    {
	if (p[len] < 0x80 || !utf_iscomposing(utf_ptr2char(p + len)))
	    return len;

	/* Skip over composing char */
	len += utf_ptr2len_check(p + len);
    }
}

/*
 * Return the number of bytes the UTF-8 encoding of character "c" takes.
 * This does not include composing characters.
 */
    int
utf_char2len(c)
    int		c;
{
    if (c < 0x80)
	return 1;
    if (c < 0x800)
	return 2;
    if (c < 0x10000)
	return 3;
    if (c < 0x200000)
	return 4;
    if (c < 0x4000000)
	return 5;
    return 6;
}

/*
 * Convert UTF-8 character "c" to string of bytes in "buf[]".
 * Returns the number of bytes.
 * This does not include composing characters.
 */
    int
utf_char2bytes(c, buf)
    int		c;
    char_u	*buf;
{
    if (c < 0x80)		/* 7 bits */
    {
	buf[0] = c;
	return 1;
    }
    if (c < 0x800)		/* 11 bits */
    {
	buf[0] = 0xc0 + ((unsigned)c >> 6);
	buf[1] = 0x80 + (c & 0x3f);
	return 2;
    }
    if (c < 0x10000)		/* 16 bits */
    {
	buf[0] = 0xe0 + ((unsigned)c >> 12);
	buf[1] = 0x80 + (((unsigned)c >> 6) & 0x3f);
	buf[2] = 0x80 + (c & 0x3f);
	return 3;
    }
    if (c < 0x200000)		/* 21 bits */
    {
	buf[0] = 0xf0 + ((unsigned)c >> 18);
	buf[1] = 0x80 + (((unsigned)c >> 12) & 0x3f);
	buf[2] = 0x80 + (((unsigned)c >> 6) & 0x3f);
	buf[3] = 0x80 + (c & 0x3f);
	return 4;
    }
    if (c < 0x4000000)		/* 26 bits */
    {
	buf[0] = 0xf8 + ((unsigned)c >> 24);
	buf[1] = 0x80 + (((unsigned)c >> 18) & 0x3f);
	buf[2] = 0x80 + (((unsigned)c >> 12) & 0x3f);
	buf[3] = 0x80 + (((unsigned)c >> 6) & 0x3f);
	buf[4] = 0x80 + (c & 0x3f);
	return 5;
    }
				/* 31 bits */
    buf[0] = 0xfc + ((unsigned)c >> 30);
    buf[1] = 0x80 + (((unsigned)c >> 24) & 0x3f);
    buf[2] = 0x80 + (((unsigned)c >> 18) & 0x3f);
    buf[3] = 0x80 + (((unsigned)c >> 12) & 0x3f);
    buf[4] = 0x80 + (((unsigned)c >> 6) & 0x3f);
    buf[5] = 0x80 + (c & 0x3f);
    return 6;
}

/*
 * Return TRUE if "c" is a composing UTF-8 character.  This means it will be
 * drawn on top of the preceding character.
 * Based on code from Markus Kuhn.
 */
    int
utf_iscomposing(c)
    int		c;
{
    /* sorted list of non-overlapping intervals */
    static struct interval
    {
	unsigned short first;
	unsigned short last;
    }
    combining[] =
    {
	{0x0300, 0x034E}, {0x0360, 0x0362}, {0x0483, 0x0486}, {0x0488, 0x0489},
	{0x0591, 0x05A1}, {0x05A3, 0x05B9}, {0x05BB, 0x05BD}, {0x05BF, 0x05BF},
	{0x05C1, 0x05C2}, {0x05C4, 0x05C4}, {0x064B, 0x0655}, {0x0670, 0x0670},
	{0x06D6, 0x06E4}, {0x06E7, 0x06E8}, {0x06EA, 0x06ED}, {0x0711, 0x0711},
	{0x0730, 0x074A}, {0x07A6, 0x07B0}, {0x0901, 0x0902}, {0x093C, 0x093C},
	{0x0941, 0x0948}, {0x094D, 0x094D}, {0x0951, 0x0954}, {0x0962, 0x0963},
	{0x0981, 0x0981}, {0x09BC, 0x09BC}, {0x09C1, 0x09C4}, {0x09CD, 0x09CD},
	{0x09E2, 0x09E3}, {0x0A02, 0x0A02}, {0x0A3C, 0x0A3C}, {0x0A41, 0x0A42},
	{0x0A47, 0x0A48}, {0x0A4B, 0x0A4D}, {0x0A70, 0x0A71}, {0x0A81, 0x0A82},
	{0x0ABC, 0x0ABC}, {0x0AC1, 0x0AC5}, {0x0AC7, 0x0AC8}, {0x0ACD, 0x0ACD},
	{0x0B01, 0x0B01}, {0x0B3C, 0x0B3C}, {0x0B3F, 0x0B3F}, {0x0B41, 0x0B43},
	{0x0B4D, 0x0B4D}, {0x0B56, 0x0B56}, {0x0B82, 0x0B82}, {0x0BC0, 0x0BC0},
	{0x0BCD, 0x0BCD}, {0x0C3E, 0x0C40}, {0x0C46, 0x0C48}, {0x0C4A, 0x0C4D},
	{0x0C55, 0x0C56}, {0x0CBF, 0x0CBF}, {0x0CC6, 0x0CC6}, {0x0CCC, 0x0CCD},
	{0x0D41, 0x0D43}, {0x0D4D, 0x0D4D}, {0x0DCA, 0x0DCA}, {0x0DD2, 0x0DD4},
	{0x0DD6, 0x0DD6}, {0x0E31, 0x0E31}, {0x0E34, 0x0E3A}, {0x0E47, 0x0E4E},
	{0x0EB1, 0x0EB1}, {0x0EB4, 0x0EB9}, {0x0EBB, 0x0EBC}, {0x0EC8, 0x0ECD},
	{0x0F18, 0x0F19}, {0x0F35, 0x0F35}, {0x0F37, 0x0F37}, {0x0F39, 0x0F39},
	{0x0F71, 0x0F7E}, {0x0F80, 0x0F84}, {0x0F86, 0x0F87}, {0x0F90, 0x0F97},
	{0x0F99, 0x0FBC}, {0x0FC6, 0x0FC6}, {0x102D, 0x1030}, {0x1032, 0x1032},
	{0x1036, 0x1037}, {0x1039, 0x1039}, {0x1058, 0x1059}, {0x17B7, 0x17BD},
	{0x17C6, 0x17C6}, {0x17C9, 0x17D3}, {0x18A9, 0x18A9}, {0x20D0, 0x20E3},
	{0x302A, 0x302F}, {0x3099, 0x309A}, {0xFB1E, 0xFB1E}, {0xFE20, 0xFE23}
    };
    int min = 0;
    int max = sizeof(combining) / sizeof(struct interval) - 1;
    int mid;

    /* first quick check for Latin-1 etc. characters */
    if (c < combining[0].first)
	return 0;

    /* binary search in table */
    while (max >= min)
    {
	mid = (min + max) / 2;
	if (combining[mid].last < c)
	    min = mid + 1;
	else if (combining[mid].first > c)
	    max = mid - 1;
	else if (combining[mid].first <= c && combining[mid].last >= c)
	    return TRUE;
    }

    return FALSE;
}

/*
 * "g8": show bytes of the UTF-8 char under the cursor.  Doesn't matter what
 * 'charcode' has been set to.
 */
    void
show_utf8()
{
    int		len;
    char_u	*line;
    int		clen;
    int		i;

    /* Get the byte length of the char under the cursor, including composing
     * characters. */
    line = ml_get_cursor();
    len = utfc_ptr2len_check(line);
    if (len == 0)
    {
	MSG("NUL");
	return;
    }

    IObuff[0] = NUL;
    clen = 0;
    for (i = 0; i < len; ++i)
    {
	if (clen == 0)
	{
	    /* start of (composing) character, get its length */
	    if (i > 0)
		STRCAT(IObuff, "+ ");
	    clen = utf_ptr2len_check(line + i);
	}
	sprintf((char *)IObuff + STRLEN(IObuff), "%02x ", line[i]);
	--clen;
    }

    msg(IObuff);
}

/*
 * Return offset from "p" to the first byte of the character it points into.
 * Returns 0 when already at the first byte of a character.
 */
    int
mb_head_off(base, p)
    char_u	*base;
    char_u	*p;
{
    char_u	*q;

    if (cc_utf8)
	return utf_head_off(base, p);

    /* It can't be a trailing byte when not using DBCS, at the start of the
     * string or the previous byte can't start a double-byte. */
    if (cc_dbcs == 0 || p <= base || MB_BYTE2LEN(p[-1]) == 1)
	return 0;

    /* This is slow: need to start at the base and go forward until the
     * byte we are looking for.  Return 1 when we went past it, 0 otherwise. */
    q = base;
    while (q < p)
	q += mb_ptr2len_check(q);
    return (q == p) ? 0 : 1;
}

    int
utf_head_off(base, p)
    char_u	*base;
    char_u	*p;
{
    char_u	*q;
    char_u	*s;

    if (*p < 0x80)		/* be quick for ASCII */
	return 0;

    /* Skip backwards over trailing bytes: 10xx.xxxx
     * Skip backwards again if on a composing char. */
    for (q = p; ; --q)
    {
	/* Move s to the last byte of this char. */
	for (s = q; (s[1] & 0xc0) == 0x80; ++s)
	    ;
	/* Move q to the first byte of this char. */
	while (q > base && (*q & 0xc0) == 0x80)
	    --q;
	/* Check for illegal sequence. */
	if (utf8len_tab[*q] != (int)(s - q + 1))
	    return 0;
	if (q <= base || !utf_iscomposing(utf_ptr2char(q)))
	    break;
    }

    return (int)(p - q);
}

/*
 * Return the offset from "p" to the first byte of a character.  When "p" is
 * at the start of a character 0 is returned, otherwise the offset to the next
 * character.  Can start anywhere in a stream of bytes.
 */
    int
mb_off_next(base, p)
    char_u	*base;
    char_u	*p;
{
    int		i;
    int		j;

    if (cc_utf8)
    {
	if (*p < 0x80)		/* be quick for ASCII */
	    return 0;

	/* Find the next character that isn't 10xx.xxxx */
	for (i = 0; (p[i] & 0xc0) == 0x80; ++i)
	    ;
	if (i > 0)
	{
	    /* Check for illegal sequence. */
	    for (j = 0; p - j > base; ++j)
		if ((p[-j] & 0xc0) != 0x80)
		    break;
	    if (utf8len_tab[p[-j]] != i + j)
		return 0;
	}
	return i;
    }

    /* Only need to check if we're on a trail byte, it doesn't matter if we
     * want the offset to the next or current character. */
    return mb_head_off(base, p);
}

/*
 * Return the offset from "p" to the last byte of the character it points
 * into.  Can start anywhere in a stream of bytes.
 */
    int
mb_tail_off(base, p)
    char_u	*base;
    char_u	*p;
{
    int		i;
    int		j;

    if (*p == NUL)
	return 0;

    if (cc_utf8)
    {
	/* Find the last character that is 10xx.xxxx */
	for (i = 0; (p[i + 1] & 0xc0) == 0x80; ++i)
	    ;
	/* Check for illegal sequence. */
	for (j = 0; p - j > base; ++j)
	    if ((p[-j] & 0xc0) != 0x80)
		break;
	if (utf8len_tab[p[-j]] != i + j + 1)
	    return 0;
	return i;
    }

    /* It can't be the first byte if a double-byte when not using DBCS, at the
     * end of the string or the byte can't start a double-byte. */
    if (cc_dbcs == 0 || p[1] == NUL || MB_BYTE2LEN(*p) == 1)
	return 0;

    /* Return 1 when on the lead byte, 0 when on the tail byte. */
    return 1 - mb_head_off(base, p);
}

/*
 * If the cursor moves on an trail byte, set the cursor on the lead byte.
 * Thus it moves left if necessary.
 * Return TRUE when the cursor was adjusted.
 */
    void
mb_adjust_cursor()
{
    mb_adjustpos(&curwin->w_cursor);
}

/*
 * Adjust position "*lp" to point to the first byte of a multi-byte character.
 * If it points to a tail byte it's moved backwards to the head byte.
 */
    void
mb_adjustpos(lp)
    pos_t	*lp;
{
    char_u	*p;
    int		o;

    if (lp->col > 0)
    {
	p = ml_get(lp->lnum);
	o = mb_head_off(p, p + lp->col);
	if (o > 0)
	    lp->col -= o;
    }
}

/*
 * Return the character length of "str".  multi-byte characters counts as one.
 */
    int
mb_charlen(str)
    char_u	*str;
{
    int count;

    if (str == NULL)
	return 0;

    for (count = 0; *str != NUL; count++)
	str += mb_ptr2len_check(str);

    return count;
}

/*
 * Decrement position "lp" by one character, taking care of multi-byte chars.
 */
    int
mb_dec(lp)
    pos_t	*lp;
{
    if (lp->col > 0)		/* still within line */
    {
	--lp->col;
	mb_adjustpos(lp);
	return 0;
    }
    if (lp->lnum > 1)		/* there is a prior line */
    {
	lp->lnum--;
	lp->col = STRLEN(ml_get(lp->lnum));
	mb_adjustpos(lp);
	return 1;
    }
    return -1;			/* at start of file */
}

/*
 * Check if buf[x] is the lead byte of a multi-byte.  Can be used anywhere in
 * a string.
 */
    int
mb_isbyte1(buf, x)
    char_u	*buf;
    int		x;
{
    if (cc_utf8)
	return (buf[x] & 0xc0) == 0xc0;

    /* Check with MB_BYTE2LEN() first, mb_head_off() is slow! */
    return (cc_dbcs != 0
	    && MB_BYTE2LEN(buf[x]) > 1
	    && mb_head_off(buf, buf + x) == 0);
}

#if defined(FEAT_GUI) || defined(PROTO)
/*
 * Return TRUE if the character at "row"/"col" on the screen if it is the left
 * side of a double-width character.
 * Caller must make sure "row" and "col" are not invalid!
 */
    int
mb_lefthalve(row, col)
    int	    row;
    int	    col;
{
#ifdef FEAT_HANGULIN
    if (composing_hangul)
	return TRUE;
#endif
    if (cc_dbcs)
	return MB_BYTE2LEN(ScreenLines[LineOffset[row] + col]) > 1;
    if (cc_utf8)
	return (col + 1 < Columns
		&& ScreenLines[LineOffset[row] + col + 1] == 0);
    return FALSE;
}
#endif

#endif /* FEAT_MBYTE */

#if defined(FEAT_XIM) || defined(PROTO)

static int	xim_has_focus = 0;
#ifdef FEAT_GUI_X11
static XIMStyle	input_style;
static int	status_area_enabled = TRUE;
#endif

#ifdef FEAT_GUI_GTK
static int xim_input_style;
static gboolean	use_status_area = 0;
#endif

    void
xim_set_focus(int focus)
{
    if (!xic)
	return;

    if (focus)
    {
	/* In Normal mode, only connect to IM if user uses over-the-spot. */
	if (!xim_has_focus
		&& (!(State & NORMAL)
#ifdef FEAT_GUI_GTK
		    || (xim_input_style & GDK_IM_PREEDIT_POSITION)
#else
		    || (input_style & XIMPreeditPosition)
#endif
		   ))
	{
	    xim_has_focus = 1;
#ifdef FEAT_GUI_GTK
	    gdk_im_begin(xic, gui.drawarea->window);
#else
	    XSetICFocus(xic);
#endif
	}
    }
    else
    {
	if (xim_has_focus)
	{
	    xim_has_focus = 0;
#ifdef FEAT_GUI_GTK
	    gdk_im_end();
#else
	    XUnsetICFocus(xic);
#endif
	}
    }
}

    void
xim_set_preedit()
{
    if (!xic)
	return;

    xim_set_focus(TRUE);

#ifdef FEAT_GUI_GTK
    if (gdk_im_ready())
    {
	GdkICAttributesType attrmask;
	GdkICAttr	    *attr;

	if (!xic_attr)
	    return;

	attr = xic_attr;
	attrmask = 0;

# ifdef FEAT_XFONTSET
	if ((xim_input_style & GDK_IM_PREEDIT_POSITION)
		&& gui.fontset != NOFONTSET
		&& gui.fontset->type == GDK_FONT_FONTSET)
	{
	    if (!xim_has_focus)
	    {
		if (attr->spot_location.y >= 0)
		{
		    attr->spot_location.x = 0;
		    attr->spot_location.y = -100;
		    attrmask |= GDK_IC_SPOT_LOCATION;
		}
	    }
	    else
	    {
		gint	width, height;

		if (attr->spot_location.x != TEXT_X(gui.col)
		    || attr->spot_location.y != TEXT_Y(gui.row))
		{
		    attr->spot_location.x = TEXT_X(gui.col);
		    attr->spot_location.y = TEXT_Y(gui.row);
		    attrmask |= GDK_IC_SPOT_LOCATION;
		}

		gdk_window_get_size(gui.drawarea->window, &width, &height);
		width -= 2 * gui.border_offset;
		height -= 2 * gui.border_offset;
		if (xim_input_style & GDK_IM_STATUS_AREA)
		    height -= gui.char_height;
		if (attr->preedit_area.width != width
		    || attr->preedit_area.height != height)
		{
		    attr->preedit_area.x = gui.border_offset;
		    attr->preedit_area.y = gui.border_offset;
		    attr->preedit_area.width = width;
		    attr->preedit_area.height = height;
		    attrmask |= GDK_IC_PREEDIT_AREA;
		}

		if (attr->preedit_fontset != gui.current_font)
		{
		    attr->preedit_fontset = gui.current_font;
		    attrmask |= GDK_IC_PREEDIT_FONTSET;
		}
	    }
	}
# endif /* FEAT_XFONTSET */

	if (xim_fg_color < 0)
	{
	    xim_fg_color = gui.def_norm_pixel;
	    xim_bg_color = gui.def_back_pixel;
	}
	if (attr->preedit_foreground.pixel != xim_fg_color)
	{
	    attr->preedit_foreground.pixel = xim_fg_color;
	    attrmask |= GDK_IC_PREEDIT_FOREGROUND;
	}
	if (attr->preedit_background.pixel != xim_bg_color)
	{
	    attr->preedit_background.pixel = xim_bg_color;
	    attrmask |= GDK_IC_PREEDIT_BACKGROUND;
	}

	if (attrmask)
	    gdk_ic_set_attr(xic, attr, attrmask);
    }
#else /* FEAT_GUI_GTK */
    {
	XVaNestedList attr_list;
	XRectangle spot_area;
	XPoint over_spot;
	int line_space;

	if (!xim_has_focus)
	{
	    /* hide XIM cursor */
	    over_spot.x = 0;
	    over_spot.y = -100; /* arbitrary invisible position */
	    attr_list = (XVaNestedList) XVaCreateNestedList(0,
							    XNSpotLocation,
							    &over_spot,
							    NULL);
	    XSetICValues(xic, XNPreeditAttributes, attr_list, NULL);
	    XFree(attr_list);
	    return;
	}

	if (input_style & XIMPreeditPosition)
	{
	    if (xim_fg_color < 0)
	    {
		xim_fg_color = gui.def_norm_pixel;
		xim_bg_color = gui.def_back_pixel;
	    }
	    over_spot.x = TEXT_X(gui.col);
	    over_spot.y = TEXT_Y(gui.row);
	    spot_area.x = 0;
	    spot_area.y = 0;
	    spot_area.height = gui.char_height * Rows;
	    spot_area.width  = gui.char_width * Columns;
	    line_space = gui.char_height;
	    attr_list = (XVaNestedList) XVaCreateNestedList(0,
					    XNSpotLocation, &over_spot,
					    XNForeground, (Pixel) xim_fg_color,
					    XNBackground, (Pixel) xim_bg_color,
					    XNArea, &spot_area,
					    XNLineSpace, line_space,
					    NULL);
	    if (XSetICValues(xic, XNPreeditAttributes, attr_list, NULL))
		EMSG(_("Cannot set IC values"));
	    XFree(attr_list);
	}
    }
#endif /* FEAT_GUI_GTK */
}

/*
 * Set up the status area.
 *
 * This should use a separate Widget, but that seems not possible, because
 * preedit_area and status_area should be set to the same window as for the
 * text input.  Unfortunately this means the status area pollutes the text
 * window...
 */
    void
xim_set_status_area()
{
    if (!xic)
	return;

#ifdef FEAT_GUI_GTK
# if defined(FEAT_XFONTSET)
    if (use_status_area)
    {
	GdkICAttr   *attr;
	GdkIMStyle  style;
	gint	    width, height;
	GtkWidget   *widget;
	GdkICAttributesType attrmask;

	if (!xic_attr)
	    return;

	attr = xic_attr;
	attrmask = 0;
	style = gdk_ic_get_style(xic);
	if ((style & GDK_IM_STATUS_MASK) == GDK_IM_STATUS_AREA)
	{
	    if (gui.fontset != NOFONTSET
		    && gui.fontset->type == GDK_FONT_FONTSET)
	    {
		widget = gui.mainwin;
		gdk_window_get_size(widget->window, &width, &height);

		attrmask |= GDK_IC_STATUS_AREA;
		attr->status_area.x = 0;
		attr->status_area.y = height - gui.char_height - 1;
		attr->status_area.width = width;
		attr->status_area.height = gui.char_height;
	    }
	}
	if (attrmask)
	    gdk_ic_set_attr(xic, attr, attrmask);
    }
# endif
#else
    {
	XVaNestedList preedit_list = 0, status_list = 0, list = 0;
	XRectangle pre_area, status_area;

	if (input_style & XIMStatusArea)
	{
	    if (input_style & XIMPreeditArea)
	    {
		XRectangle *needed_rect;

		/* to get status_area width */
		status_list = XVaCreateNestedList(0, XNAreaNeeded,
						  &needed_rect, NULL);
		XGetICValues(xic, XNStatusAttributes, status_list, NULL);
		XFree(status_list);

		status_area.width = needed_rect->width;
	    }
	    else
		status_area.width = gui.char_width * Columns;

	    status_area.x = 0;
	    status_area.y = gui.char_height * Rows + gui.border_offset;
	    if (gui.which_scrollbars[SBAR_BOTTOM])
		status_area.y += gui.scrollbar_height;
#if defined(FEAT_MENU) && !defined(FEAT_GUI_GTK)
	    if (gui.menu_is_active)
		status_area.y += gui.menu_height;
#endif
	    status_area.height = gui.char_height;
	    status_list = XVaCreateNestedList(0, XNArea, &status_area, NULL);
	}
	else
	{
	    status_area.x = 0;
	    status_area.y = gui.char_height * Rows + gui.border_offset;
	    if (gui.which_scrollbars[SBAR_BOTTOM])
		status_area.y += gui.scrollbar_height;
#ifdef FEAT_MENU
	    if (gui.menu_is_active)
		status_area.y += gui.menu_height;
#endif
	    status_area.width = 0;
	    status_area.height = gui.char_height;
	}

	if (input_style & XIMPreeditArea)   /* off-the-spot */
	{
	    pre_area.x = status_area.x + status_area.width;
	    pre_area.y = gui.char_height * Rows + gui.border_offset;
	    pre_area.width = gui.char_width * Columns - pre_area.x;
	    if (gui.which_scrollbars[SBAR_BOTTOM])
		pre_area.y += gui.scrollbar_height;
#ifdef FEAT_MENU
	    if (gui.menu_is_active)
		pre_area.y += gui.menu_height;
#endif
	    pre_area.height = gui.char_height;
	    preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, NULL);
	}
	else if (input_style & XIMPreeditPosition)   /* over-the-spot */
	{
	    pre_area.x = 0;
	    pre_area.y = 0;
	    pre_area.height = gui.char_height * Rows;
	    pre_area.width = gui.char_width * Columns;
	    preedit_list = XVaCreateNestedList(0, XNArea, &pre_area, NULL);
	}

	if (preedit_list && status_list)
	    list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				       XNStatusAttributes, status_list, NULL);
	else if (preedit_list)
	    list = XVaCreateNestedList(0, XNPreeditAttributes, preedit_list,
				       NULL);
	else if (status_list)
	    list = XVaCreateNestedList(0, XNStatusAttributes, status_list,
				       NULL);
	else
	    list = NULL;

	if (list)
	{
	    XSetICValues(xic, XNVaNestedList, list, NULL);
	    XFree(list);
	}
	if (status_list)
	    XFree(status_list);
	if (preedit_list)
	    XFree(preedit_list);
    }
#endif
}

#if defined(FEAT_GUI_X11) || defined(PROTO)
# if defined(XtSpecificationRelease) && XtSpecificationRelease >= 6 && !defined(sun)
#  define USE_X11R6_XIM
# endif

static int xim_real_init __ARGS((Window x11_window, Display *x11_display));

#ifdef USE_X11R6_XIM
static void xim_instantiate_cb __ARGS((Display *display, XPointer client_data, XPointer	call_data));
static void xim_destroy_cb __ARGS((XIM im, XPointer client_data, XPointer call_data));

/*ARGSUSED*/
    static void
xim_instantiate_cb(display, client_data, call_data)
    Display	*display;
    XPointer	client_data;
    XPointer	call_data;
{
    Window	x11_window;
    Display	*x11_display;

    gui_get_x11_windis(&x11_window, &x11_display);
    if (display != x11_display)
	return;

    xim_real_init(x11_window, x11_display);
    gui_set_shellsize(FALSE);
    if (xic != NULL)
	XUnregisterIMInstantiateCallback(x11_display, NULL, NULL, NULL,
					 xim_instantiate_cb, NULL);
}

/*ARGSUSED*/
    static void
xim_destroy_cb(im, client_data, call_data)
    XIM		im;
    XPointer	client_data;
    XPointer	call_data;
{
    Window	x11_window;
    Display	*x11_display;

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = NULL;
    status_area_enabled = FALSE;

    gui_set_shellsize(FALSE);

    XRegisterIMInstantiateCallback(x11_display, NULL, NULL, NULL,
				   xim_instantiate_cb, NULL);
}
#endif

    void
xim_init()
{
    Window	x11_window;
    Display	*x11_display;

    gui_get_x11_windis(&x11_window, &x11_display);

    xic = NULL;

    if (xim_real_init(x11_window, x11_display))
	return;

    gui_set_shellsize(FALSE);

#ifdef USE_X11R6_XIM
    XRegisterIMInstantiateCallback(x11_display, NULL, NULL, NULL,
				   xim_instantiate_cb, NULL);
#endif
}

    static int
xim_real_init(x11_window, x11_display)
    Window  x11_window;
    Display *x11_display;
{
    int		i;
    char	*p,
		*s,
		*ns,
		*end,
		tmp[1024],
		buf[32];
    XIM		xim = NULL;
    XIMStyles	*xim_styles;
    XIMStyle	this_input_style = 0;
    Boolean	found;
    XPoint	over_spot;
    XVaNestedList preedit_list, status_list;

    input_style = 0;
    status_area_enabled = FALSE;

#ifdef FEAT_XFONTSET
    if (*p_guifontset == NUL)
    {
	/* This message is annoying when Vim was compiled with XIM support but
	 * it's not being used.
	 * EMSG(_("XIM requires guifontset setting"));
	 */
	return FALSE;
    }
#else
    EMSG(_("XIM requires VIM compiled with +xfontset feature."));
    return FALSE;
#endif

    if (xic != NULL)
	return FALSE;

    if (!gui.input_method || !*gui.input_method)
    {
	if ((p = XSetLocaleModifiers("")) != NULL && *p)
	    xim = XOpenIM(x11_display, NULL, NULL, NULL);
    }
    else
    {
	strcpy(tmp, gui.input_method);
	for (ns = s = tmp; ns && *s;)
	{
	    while (*s && isspace((unsigned char)*s))
		s++;
	    if (!*s)
		break;
	    if ((ns = end = strchr(s, ',')) == 0)
		end = s + strlen(s);
	    while (isspace((unsigned char)*end))
		end--;
	    *end = '\0';

	    strcpy(buf, "@im=");
	    strcat(buf, s);
	    if ((p = XSetLocaleModifiers(buf)) != NULL
		&& *p
		&& (xim = XOpenIM(x11_display, NULL, NULL, NULL)) != NULL)
		break;

	    s = ns + 1;
	}
    }

    if (xim == NULL && (p = XSetLocaleModifiers("")) != NULL && *p)
	xim = XOpenIM(x11_display, NULL, NULL, NULL);

    if (!xim)
	xim = XOpenIM(x11_display, NULL, NULL, NULL);

    if (!xim)
    {
	EMSG(_("Failed to open input method"));
	return FALSE;
    }

#ifdef USE_X11R6_XIM
    {
	XIMCallback destroy_cb;

	destroy_cb.callback = xim_destroy_cb;
	destroy_cb.client_data = NULL;
	if (XSetIMValues(xim, XNDestroyCallback, &destroy_cb, NULL))
	    EMSG(_("Warning: Could not set destroy callback to IM"));
    }
#endif

    if (XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL) || !xim_styles)
    {
	EMSG(_("input method doesn't support any style"));
	XCloseIM(xim);
	return FALSE;
    }

    found = False;
    strcpy(tmp, gui.preedit_type);
    for (s = tmp; s && !found; )
    {
	while (*s && isspace((unsigned char)*s))
	    s++;
	if (!*s)
	    break;
	if ((ns = end = strchr(s, ',')) != 0)
	    ns++;
	else
	    end = s + strlen(s);
	while (isspace((unsigned char)*end))
	    end--;
	*end = '\0';

	if (!strcmp(s, "OverTheSpot"))
	    this_input_style = (XIMPreeditPosition | XIMStatusArea);
	else if (!strcmp(s, "OffTheSpot"))
	    this_input_style = (XIMPreeditArea | XIMStatusArea);
	else if (!strcmp(s, "Root"))
	    this_input_style = (XIMPreeditNothing | XIMStatusNothing);

	for (i = 0; (unsigned short)i < xim_styles->count_styles; i++)
	{
	    if (this_input_style == xim_styles->supported_styles[i])
	    {
		found = True;
		break;
	    }
	}
	if (!found)
	    for (i = 0; (unsigned short)i < xim_styles->count_styles; i++)
	    {
		if ((xim_styles->supported_styles[i] & this_input_style)
			== (this_input_style & ~XIMStatusArea))
		{
		    this_input_style &= ~XIMStatusArea;
		    found = True;
		    break;
		}
	    }

	s = ns;
    }
    XFree(xim_styles);

    if (!found)
    {
	EMSG(_("input method doesn't support my preedit type"));
	XCloseIM(xim);
	return FALSE;
    }

    over_spot.x = TEXT_X(gui.col);
    over_spot.y = TEXT_Y(gui.row);
    input_style = this_input_style;
    preedit_list = XVaCreateNestedList(0,
			    XNSpotLocation, &over_spot,
			    XNForeground, (Pixel)gui.def_norm_pixel,
			    XNBackground, (Pixel)gui.def_back_pixel,
			    XNFontSet, (XFontSet)gui.fontset,
			    NULL);
    status_list = XVaCreateNestedList(0,
			    XNForeground, (Pixel)gui.def_norm_pixel,
			    XNBackground, (Pixel)gui.def_back_pixel,
			    XNFontSet, (XFontSet)gui.fontset,
			    NULL);
    xic = XCreateIC(xim,
		    XNInputStyle, input_style,
		    XNClientWindow, x11_window,
		    XNFocusWindow, gui.wid,
		    XNPreeditAttributes, preedit_list,
		    XNStatusAttributes, status_list,
		    NULL);
    XFree(status_list);
    XFree(preedit_list);
    if (xic)
    {
	if (input_style & XIMStatusArea)
	{
	    xim_set_status_area();
	    status_area_enabled = TRUE;
	}
	else
	    gui_set_shellsize(FALSE);
    }
    else
    {
	EMSG(_("Failed to create input context"));
	XCloseIM(xim);
	return FALSE;
    }

    return TRUE;
}

#endif /* FEAT_GUI_X11 */

#if defined(FEAT_GUI_GTK) || defined(PROTO)

void
xim_decide_input_style()
{
    GdkIMStyle supported_style = GDK_IM_PREEDIT_NONE |
				 GDK_IM_PREEDIT_NOTHING |
				 GDK_IM_PREEDIT_POSITION |
				 GDK_IM_PREEDIT_CALLBACKS |
				 /* GDK_IM_STATUS_CALLBACKS | */
				 GDK_IM_STATUS_AREA |
				 GDK_IM_STATUS_NONE |
				 GDK_IM_STATUS_NOTHING;

    if (!gdk_im_ready()) {
	xim_input_style = 0;
    } else {
	if (gtk_major_version > 1
	    || (gtk_major_version == 1
		&& (gtk_minor_version > 2
		    || (gtk_minor_version == 2 && gtk_micro_version >= 3))))
	    use_status_area = TRUE;
	else
	{
	    EMSG(_("Your GTK+ is older than 1.2.3. Status area disabled"));
	    use_status_area = FALSE;
	}
#ifdef FEAT_XFONTSET
	if (gui.fontset == NOFONTSET || gui.fontset->type != GDK_FONT_FONTSET)
#endif
	    supported_style &= ~(GDK_IM_PREEDIT_POSITION | GDK_IM_STATUS_AREA);
	if (!use_status_area)
	    supported_style &= ~GDK_IM_STATUS_AREA;
	xim_input_style = gdk_im_decide_style(supported_style);
    }
}

#ifdef FEAT_GUI_GTK
# include <gdk/gdkx.h>
#else
/* Define a few things to be able to generate prototypes while not configured
 * for GTK. */
# define GSList int
# define gboolean int
  typedef int GdkEvent;
# define GdkIC int
#endif

/*ARGSUSED*/
    static void
preedit_start_cbproc(XIC xic, XPointer client_data, XPointer call_data)
{
}

    static void
xim_back_delete(int n)
{
    char_u str[3];

    str[0] = CSI;
    str[1] = 'k';
    str[2] = 'b';
    while (n-- > 0)
	add_to_input_buf(str, 3);
}

static GSList *key_press_event_queue = NULL;
static int preedit_buf_len = 0;
static gboolean processing_queued_event = FALSE;

/*ARGSUSED*/
    static void
preedit_draw_cbproc(XIC xic, XPointer client_data, XPointer call_data)
{
    XIMPreeditDrawCallbackStruct *draw_data;
    XIMText *text;
    GdkWChar *wstr = NULL;
    char *src;
    GSList *event_queue;

    draw_data = (XIMPreeditDrawCallbackStruct *) call_data;
    text = (XIMText *) draw_data->text;

    if (draw_data->chg_length > 0)
    {
	xim_back_delete(draw_data->chg_length);
	preedit_buf_len -= draw_data->chg_length;
    }
    if (text && (src = text->string.multi_byte))
    {
	int len = strlen(src);

	wstr = g_new(GdkWChar, len);
	preedit_buf_len += gdk_mbstowcs(wstr, src, len);
	g_free(wstr);
	add_to_input_buf((char_u *)src, len);
    }
    event_queue = key_press_event_queue;
    processing_queued_event = TRUE;
    while (event_queue)
    {
	GdkEvent *ev = event_queue->data;
	gboolean *ret;
	gtk_signal_emit_by_name((GtkObject*)gui.mainwin, "key_press_event",
				ev, &ret);
	gdk_event_free(ev);
	event_queue = event_queue->next;
    }
    processing_queued_event = FALSE;
    if (key_press_event_queue)
    {
	g_slist_free(key_press_event_queue);
	key_press_event_queue = NULL;
    }
    if (gtk_main_level() > 0)
	gtk_main_quit();
}

/*ARGSUSED*/
    static void
preedit_caret_cbproc(XIC xic, XPointer client_data, XPointer call_data)
{
}

/*ARGSUSED*/
    static void
preedit_done_cbproc(XIC xic, XPointer client_data, XPointer call_data)
{
}

    void
xim_reset(void)
{
    char *text;

    if (xic)
    {
	text = XmbResetIC(((GdkICPrivate *)xic)->xic);
	if (text != NULL && !(xim_input_style & GDK_IM_PREEDIT_CALLBACKS))
	    add_to_input_buf((char_u *)text, strlen(text));
	else
	    preedit_buf_len = 0;
	if (text != NULL)
	    XFree(text);
    }
}

    int
xim_queue_key_press_event(GdkEvent *ev)
{
    if (preedit_buf_len <= 0 || processing_queued_event)
	return FALSE;

    key_press_event_queue = g_slist_append(key_press_event_queue,
                                           gdk_event_copy(ev));
    return TRUE;
}

/*ARGSUSED*/
    static void
preedit_callback_setup(GdkIC *ic)
{
    XIC xxic;
    XVaNestedList preedit_attr;
    XIMCallback preedit_start_cb;
    XIMCallback preedit_draw_cb;
    XIMCallback preedit_caret_cb;
    XIMCallback preedit_done_cb;

    xxic = ((GdkICPrivate*)xic)->xic;
    preedit_start_cb.callback = (XIMProc)preedit_start_cbproc;
    preedit_draw_cb.callback = (XIMProc)preedit_draw_cbproc;
    preedit_caret_cb.callback = (XIMProc)preedit_caret_cbproc;
    preedit_done_cb.callback = (XIMProc)preedit_done_cbproc;
    preedit_attr
	= XVaCreateNestedList (0,
			       XNPreeditStartCallback, &preedit_start_cb,
			       XNPreeditDrawCallback, &preedit_draw_cb,
			       XNPreeditCaretCallback, &preedit_caret_cb,
			       XNPreeditDoneCallback, &preedit_done_cb,
			       0);
    XSetICValues (xxic, XNPreeditAttributes, preedit_attr, 0);
    XFree(preedit_attr);
}

    static void
reset_state_setup(GdkIC *ic)
{
    /* don't change the input context when we call reset */
    XSetICValues(((GdkICPrivate*)ic)->xic, XNResetState, XIMPreserveState, 0);
}

    void
xim_init(void)
{
    xic = NULL;
    xic_attr = NULL;

    if (!gdk_im_ready())
    {
	EMSG(_("Input Method Server is not running"));
	return;
    }
    if ((xic_attr = gdk_ic_attr_new()) != NULL)
    {
	gint width, height;
	GdkEventMask mask;
	GdkColormap *colormap;
	GdkICAttr *attr = xic_attr;
	GdkICAttributesType attrmask = GDK_IC_ALL_REQ;
	GtkWidget *widget = gui.drawarea;

	attr->style = xim_input_style;
	attr->client_window = gui.mainwin->window;

	if ((colormap = gtk_widget_get_colormap(widget)) !=
	    gtk_widget_get_default_colormap())
	{
	    attrmask |= GDK_IC_PREEDIT_COLORMAP;
	    attr->preedit_colormap = colormap;
	}
	attrmask |= GDK_IC_PREEDIT_FOREGROUND;
	attrmask |= GDK_IC_PREEDIT_BACKGROUND;
	attr->preedit_foreground = widget->style->fg[GTK_STATE_NORMAL];
	attr->preedit_background = widget->style->base[GTK_STATE_NORMAL];

#ifdef FEAT_XFONTSET
	if ((xim_input_style & GDK_IM_PREEDIT_MASK) == GDK_IM_PREEDIT_POSITION)
	{
	    if (gui.fontset == NOFONTSET
		    || gui.fontset->type != GDK_FONT_FONTSET)
	    {
		EMSG(_("over-the-spot style requires fontset"));
	    }
	    else
	    {
		gdk_window_get_size(widget->window, &width, &height);

		attrmask |= GDK_IC_PREEDIT_POSITION_REQ;
		attr->spot_location.x = TEXT_X(0);
		attr->spot_location.y = TEXT_Y(0);
		attr->preedit_area.x = gui.border_offset;
		attr->preedit_area.y = gui.border_offset;
		attr->preedit_area.width = width - 2*gui.border_offset;
		attr->preedit_area.height = height - 2*gui.border_offset;
		attr->preedit_fontset = gui.fontset;
	    }
	}

	if ((xim_input_style & GDK_IM_STATUS_MASK) == GDK_IM_STATUS_AREA)
	{
	    if (gui.fontset == NOFONTSET
		    || gui.fontset->type != GDK_FONT_FONTSET)
	    {
		EMSG(_("over-the-spot style requires fontset"));
	    }
	    else
	    {
		gdk_window_get_size(gui.mainwin->window, &width, &height);
		attrmask |= GDK_IC_STATUS_AREA_REQ;
		attr->status_area.x = 0;
		attr->status_area.y = height - gui.char_height - 1;
		attr->status_area.width = width;
		attr->status_area.height = gui.char_height;
		attr->status_fontset = gui.fontset;
	    }
	}
	else if ((xim_input_style & GDK_IM_STATUS_MASK)
		 == GDK_IM_STATUS_CALLBACKS)
	{
	    /* FIXME */
	}
#endif

	xic = gdk_ic_new(attr, attrmask);

	if (xic == NULL)
	    EMSG(_("Can't create input context."));
	else
	{
	    mask = gdk_window_get_events(widget->window);
	    mask |= gdk_ic_get_events(xic);
	    gdk_window_set_events(widget->window, mask);
	    if (xim_input_style & GDK_IM_PREEDIT_CALLBACKS)
		preedit_callback_setup(xic);
	    reset_state_setup(xic);
	}
    }
}
#endif /* FEAT_GUI_GTK */

    int
xim_get_status_area_height(void)
{
#if defined(FEAT_XFONTSET)
# ifdef FEAT_GUI_GTK
    if (xim_input_style & GDK_IM_STATUS_AREA)
	return gui.char_height;
# else
    if (status_area_enabled)
	return gui.char_height;
# endif
#endif
    return 0;
}

#endif /* FEAT_XIM */
