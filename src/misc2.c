/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * misc2.c: Various functions.
 */

#include "vim.h"

/*
 * coladvance(col)
 *
 * Try to advance the Cursor to the specified column.
 *
 * return OK if desired column is reached, FAIL if not
 */
    int
coladvance(wcol)
    colnr_t	    wcol;
{
    int		idx;
    char_u	*ptr;
    colnr_t	col;

    ptr = ml_get_curline();

    /* try to advance to the specified column */
    idx = -1;
    col = 0;
    while (col <= wcol && *ptr)
    {
	++idx;
	/* Count a tab for what it's worth (if list mode not on) */
	col += lbr_chartabsize(ptr, col);
	++ptr;
    }
    /*
     * in insert mode it is allowed to be one char beyond the end of the line
     */
    if ((State & INSERT) && col <= wcol)
	++idx;
    if (idx < 0)
	curwin->w_cursor.col = 0;
    else
	curwin->w_cursor.col = idx;
    if (col <= wcol)
	return FAIL;	    /* Couldn't reach column */
    else
	return OK;	    /* Reached column */
}

/*
 * inc(p)
 *
 * Increment the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at end of file, 0 otherwise.
 */
    int
inc_cursor()
{
    return inc(&curwin->w_cursor);
}

    int
inc(lp)
    FPOS  *lp;
{
    char_u  *p = ml_get_pos(lp);

    if (*p != NUL)	/* still within line, move to next char (may be NUL) */
    {
	lp->col++;
	return ((p[1] != NUL) ? 0 : 1);
    }
    if (lp->lnum != curbuf->b_ml.ml_line_count)	    /* there is a next line */
    {
	lp->col = 0;
	lp->lnum++;
	return 1;
    }
    return -1;
}

/*
 * incl(lp): same as inc(), but skip the NUL at the end of non-empty lines
 */
    int
incl(lp)
    FPOS    *lp;
{
    int	    r;

    if ((r = inc(lp)) == 1 && lp->col)
	r = inc(lp);
    return r;
}

/*
 * dec(p)
 *
 * Decrement the line pointer 'p' crossing line boundaries as necessary.
 * Return 1 when crossing a line, -1 when at start of file, 0 otherwise.
 */
    int
dec_cursor()
{
    return dec(&curwin->w_cursor);
}

    int
dec(lp)
    FPOS  *lp;
{
    if (lp->col > 0)
    {		/* still within line */
	lp->col--;
	return 0;
    }
    if (lp->lnum > 1)
    {		/* there is a prior line */
	lp->lnum--;
	lp->col = STRLEN(ml_get(lp->lnum));
	return 1;
    }
    return -1;			/* at start of file */
}

/*
 * decl(lp): same as dec(), but skip the NUL at the end of non-empty lines
 */
    int
decl(lp)
    FPOS    *lp;
{
    int	    r;

    if ((r = dec(lp)) == 1 && lp->col)
	r = dec(lp);
    return r;
}

/*
 * Make sure curwin->w_cursor.lnum is valid.
 */
    void
check_cursor_lnum()
{
    if (curwin->w_cursor.lnum > curbuf->b_ml.ml_line_count)
	curwin->w_cursor.lnum = curbuf->b_ml.ml_line_count;
    if (curwin->w_cursor.lnum <= 0)
	curwin->w_cursor.lnum = 1;
}

/*
 * make sure curwin->w_cursor in on a valid character
 */
    void
adjust_cursor()
{
    colnr_t len;

    check_cursor_lnum();

    len = STRLEN(ml_get_curline());
    if (len == 0)
	curwin->w_cursor.col = 0;
    else if (curwin->w_cursor.col >= len)
	curwin->w_cursor.col = len - 1;
}

/*
 * Make sure curwin->w_cursor is not on the NUL at the end of the line.
 */
    void
adjust_cursor_col()
{
    if (curwin->w_cursor.col && gchar_cursor() == NUL)
	--curwin->w_cursor.col;
}

/*
 * When curwin->w_leftcol has changed, adjust the cursor position.
 * Return TRUE if the cursor was moved.
 */
    int
leftcol_changed()
{
    long	lastcol;
    colnr_t	s, e;
    int		retval = FALSE;

    changed_cline_bef_curs();
    lastcol = curwin->w_leftcol + Columns - (curwin->w_p_nu ? 8 : 0) - 1;
    validate_virtcol();

    /*
     * If the cursor is right or left of the screen, move it to last or first
     * character.
     */
    if (curwin->w_virtcol > (colnr_t)lastcol)
    {
	retval = TRUE;
	coladvance((colnr_t)lastcol);
    }
    else if (curwin->w_virtcol < curwin->w_leftcol)
    {
	retval = TRUE;
	(void)coladvance(curwin->w_leftcol);
    }

    /*
     * If the start of the character under the cursor is not on the screen,
     * advance the cursor one more char.  If this fails (last char of the
     * line) adjust the scrolling.
     */
    getvcol(curwin, &curwin->w_cursor, &s, NULL, &e);
    if (e > (colnr_t)lastcol)
    {
	retval = TRUE;
	coladvance(s - 1);
    }
    else if (s < curwin->w_leftcol)
    {
	retval = TRUE;
	if (coladvance(e + 1) == FAIL)	/* there isn't another character */
	{
	    curwin->w_leftcol = s;	/* adjust w_leftcol instead */
	    changed_cline_bef_curs();
	}
    }

    redraw_later(NOT_VALID);
    return retval;
}

/**********************************************************************
 * Various routines dealing with allocation and deallocation of memory.
 */

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#define KEEP_ROOM 8192L

static void vim_strup __ARGS((char_u *p));

/*
 * Note: if unsinged is 16 bits we can only allocate up to 64K with alloc().
 * Use lalloc for larger blocks.
 */
    char_u *
alloc(size)
    unsigned	    size;
{
    return (lalloc((long_u)size, TRUE));
}

/*
 * Allocate memory and set all bytes to zero.
 */
    char_u *
alloc_clear(size)
    unsigned	    size;
{
    char_u *p;

    p = (lalloc((long_u)size, TRUE));
    if (p != NULL)
	(void)vim_memset(p, 0, (size_t)size);
    return p;
}

/*
 * alloc() with check for maximum line length
 */
    char_u *
alloc_check(size)
    unsigned	    size;
{
#if !defined(UNIX) && !defined(__EMX__)
    if (sizeof(int) == 2 && size > 0x7fff)
    {
	EMSG("Line is becoming too long");
	return NULL;
    }
#endif
    return (lalloc((long_u)size, TRUE));
}

/*
 * Allocate memory like lalloc() and set all bytes to zero.
 */
    char_u *
lalloc_clear(size, message)
    long_u	size;
    int		message;
{
    char_u *p;

    p = (lalloc(size, message));
    if (p != NULL)
	(void)vim_memset(p, 0, (size_t)size);
    return p;
}

    char_u *
lalloc(size, message)
    long_u	    size;
    int		    message;
{
    char_u	*p;		    /* pointer to new storage space */
    static int	releasing = FALSE;  /* don't do mf_release_all() recursive */
    int		try_again;

    if (size <= 0)
    {
	EMSGN("Internal error: lalloc(%ld, )", size);
	return NULL;
    }
#if defined(MSDOS) && !defined(DJGPP)
    if (size >= 0xfff0)		/* in MSDOS we can't deal with >64K blocks */
	p = NULL;
    else
#endif

    /*
     * If out of memory, try to release some memfile blocks.
     * If some blocks are released call malloc again.
     */
    for (;;)
    {
	if ((p = (char_u *)malloc((size_t)size)) != NULL)
	{
	    if (mch_avail_mem(TRUE) < KEEP_ROOM && !releasing)
	    {				    /* System is low... no go! */
		    vim_free((char *)p);
		    p = NULL;
	    }
	}
    /*
     * Remember that mf_release_all() is being called to avoid an endless loop,
     * because mf_release_all() may call alloc() recursively.
     */
	if (p != NULL || releasing)
	    break;
	releasing = TRUE;
	try_again = mf_release_all();
	releasing = FALSE;
	if (!try_again)
	    break;
    }

    /*
     * Avoid repeating the error message many times (they take 1 second each).
     * Did_outofmem_msg is reset when a character is read.
     */
    if (message && p == NULL)
	do_outofmem_msg();
    return (p);
}

    void
do_outofmem_msg()
{
    if (!did_outofmem_msg)
    {
	emsg(e_outofmem);
	did_outofmem_msg = TRUE;
    }
}

/*
 * copy a string into newly allocated memory
 */
    char_u *
vim_strsave(string)
    char_u	*string;
{
    char_u	*p;
    unsigned	len;

    len = STRLEN(string) + 1;
    p = alloc(len);
    if (p != NULL)
	vim_memmove(p, string, (size_t)len);
    return p;
}

    char_u *
vim_strnsave(string, len)
    char_u	*string;
    int		len;
{
    char_u	*p;

    p = alloc((unsigned)(len + 1));
    if (p != NULL)
    {
	STRNCPY(p, string, len);
	p[len] = NUL;
    }
    return p;
}

/*
 * like vim_strnsave(), but remove backslashes from the string.
 */
    char_u *
vim_strnsave_esc(string, len)
    char_u	*string;
    int		len;
{
    char_u *p1, *p2;

    p1 = alloc((unsigned) (len + 1));
    if (p1 != NULL)
    {
	STRNCPY(p1, string, len);
	p1[len] = NUL;
	for (p2 = p1; *p2; ++p2)
	    if (*p2 == '\\' && *(p2 + 1) != NUL)
		STRCPY(p2, p2 + 1);
    }
    return p1;
}

/*
 * Same as vim_strsave(), but any characters found in esc_chars are preceded
 * by a backslash.
 */
    char_u *
vim_strsave_escaped(string, esc_chars)
    char_u	*string;
    char_u	*esc_chars;
{
    char_u	*p;
    char_u	*p2;
    char_u	*escaped_string;
    unsigned	length;

    /*
     * First count the number of backslashes required.
     * Then allocate the memory and insert them.
     */
    length = 1;				/* count the trailing '/' and NUL */
    for (p = string; *p; p++)
    {
	if (vim_strchr(esc_chars, *p) != NULL)
	    ++length;			/* count a backslash */
	++length;			/* count an ordinary char */
    }
    escaped_string = alloc(length);
    if (escaped_string != NULL)
    {
	p2 = escaped_string;
	for (p = string; *p; p++)
	{
	    if (vim_strchr(esc_chars, *p) != NULL)
		*p2++ = '\\';
	    *p2++ = *p;
	}
	*p2 = NUL;
    }
    return escaped_string;
}

/*
 * like vim_strsave(), but make all characters uppercase.
 */
    char_u *
vim_strsave_up(string)
    char_u	*string;
{
    char_u *p1;

    p1 = vim_strsave(string);
    vim_strup(p1);
    return p1;
}

/*
 * like vim_strnsave(), but make all characters uppercase.
 */
    char_u *
vim_strnsave_up(string, len)
    char_u	*string;
    int		len;
{
    char_u *p1;

    p1 = vim_strnsave(string, len);
    vim_strup(p1);
    return p1;
}

    static void
vim_strup(p)
    char_u	*p;
{
    char_u  *p2;
    int	    c;

    if (p != NULL)
    {
	p2 = p;
	while ((c = *p2) != NUL)
	    *p2++ = TO_UPPER(c);
    }
}

/*
 * copy a number of spaces
 */
    void
copy_spaces(ptr, count)
    char_u  *ptr;
    size_t  count;
{
    size_t  i = count;
    char_u  *p = ptr;

    while (i--)
	*p++ = ' ';
}

/*
 * delete spaces at the end of a string
 */
    void
del_trailing_spaces(ptr)
    char_u *ptr;
{
    char_u  *q;

    q = ptr + STRLEN(ptr);
    while (--q > ptr && vim_iswhite(q[0]) && q[-1] != '\\' &&
							   q[-1] != Ctrl('V'))
	*q = NUL;
}

/*
 * vim_strncpy()
 *
 * This is here because strncpy() does not guarantee successful results when
 * the to and from strings overlap.  It is only currently called from nextwild()
 * which copies part of the command line to another part of the command line.
 * This produced garbage when expanding files etc in the middle of the command
 * line (on my terminal, anyway) -- webb.
 */
    void
vim_strncpy(to, from, len)
    char_u *to;
    char_u *from;
    int len;
{
    int i;

    if (to <= from)
    {
	while (len-- && *from)
	    *to++ = *from++;
	if (len >= 0)
	    *to = *from;    /* Copy NUL */
    }
    else
    {
	for (i = 0; i < len; i++)
	{
	    to++;
	    if (*from++ == NUL)
	    {
		i++;
		break;
	    }
	}
	for (; i > 0; i--)
	    *--to = *--from;
    }
}

/*
 * Isolate one part of a string option where parts are separated with commas.
 * The part is copied into buf[maxlen].
 * "*option" is advanced to the next part.
 * The length is returned.
 */
    int
copy_option_part(option, buf, maxlen, sep_chars)
    char_u	**option;
    char_u	*buf;
    int		maxlen;
    char	*sep_chars;
{
    int	    len = 0;
    char_u  *p = *option;

    /* skip '.' at start of option part, for 'suffixes' */
    if (*p == '.')
	buf[len++] = *p++;
    while (*p && vim_strchr((char_u *)sep_chars, *p) == NULL)
    {
	/*
	 * Skip backslash before a separator character and space.
	 */
	if (p[0] == '\\' && vim_strchr((char_u *)sep_chars, p[1]) != NULL)
	    ++p;
	if (len < maxlen - 1)
	    buf[len++] = *p;
	++p;
    }
    buf[len] = NUL;

    p = skip_to_option_part(p);	/* p points to next file name */

    *option = p;
    return len;
}

/*
 * replacement for free() that ignores NULL pointers
 */
    void
vim_free(x)
    void *x;
{
    if (x != NULL)
	free(x);
}

#ifndef HAVE_MEMSET
    void *
vim_memset(ptr, c, size)
    void    *ptr;
    int	    c;
    size_t  size;
{
    char *p = ptr;

    while (size-- > 0)
	*p++ = c;
    return ptr;
}
#endif

#ifdef VIM_MEMCMP
/*
 * Return zero when "b1" and "b2" are the same for "len" bytes.
 * Return non-zero otherwise.
 */
    int
vim_memcmp(b1, b2, len)
    void    *b1;
    void    *b2;
    size_t  len;
{
    char_u  *p1 = (char_u *)b1, *p2 = (char_u *)b2;

    for ( ; len > 0; --len)
    {
	if (*p1 != *p2)
	    return 1;
	++p1;
	++p2;
    }
    return 0;
}
#endif

#ifdef VIM_MEMMOVE
/*
 * Version of memmove that handles overlapping source and destination.
 * For systems that don't have a function that is guaranteed to do that (SYSV).
 */
    void
vim_memmove(dst_arg, src_arg, len)
    void    *src_arg, *dst_arg;
    size_t  len;
{
    /*
     * A void doesn't have a size, we use char pointers.
     */
    char *dst = dst_arg, *src = src_arg;

					/* overlap, copy backwards */
    if (dst > src && dst < src + len)
    {
	src +=len;
	dst +=len;
	while (len-- > 0)
	    *--dst = *--src;
    }
    else				/* copy forwards */
	while (len-- > 0)
	    *dst++ = *src++;
}
#endif

#if (!defined(HAVE_STRCASECMP) && !defined(HAVE_STRICMP)) || defined(PROTO)
/*
 * Compare two strings, ignoring case.
 * return 0 for match, 1 for difference
 */
    int
vim_stricmp(s1, s2)
    char    *s1;
    char    *s2;
{
    for (;;)
    {
	if (TO_LOWER(*s1) != TO_LOWER(*s2))
	    return 1;			    /* this character different */
	if (*s1 == NUL)
	    break;			    /* strings match until NUL */
	++s1;
	++s2;
    }
    return 0;				    /* strings match */
}
#endif

#if (!defined(HAVE_STRNCASECMP) && !defined(HAVE_STRNICMP)) || defined(PROTO)
/*
 * Compare two strings, for length "len", ignoring case.
 * return 0 for match, 1 for difference
 */
    int
vim_strnicmp(s1, s2, len)
    char    *s1;
    char    *s2;
    size_t  len;
{
    while (len)
    {
	if (TO_LOWER(*s1) != TO_LOWER(*s2))
	    return 1;			    /* this character different */
	if (*s1 == NUL)
	    break;			    /* strings match until NUL */
	++s1;
	++s2;
	--len;
    }
    return 0;				    /* strings match */
}
#endif

/*
 * Version of strchr() and strrchr() that handle unsigned char strings
 * with characters above 128 correctly. Also it doesn't return a pointer to
 * the NUL at the end of the string.
 */
    char_u  *
vim_strchr(string, n)
    char_u  *string;
    int	    n;
{
    char_u	*p;
    int		c;

    p = string;
    while ((c = *p) != NUL)
    {
	if (c == n)
	    return p;
	++p;
    }
    return NULL;
}

    char_u  *
vim_strrchr(string, n)
    char_u  *string;
    int	    n;
{
    char_u  *retval = NULL;

    while (*string)
    {
	if (*string == n)
	    retval = string;
	++string;
    }
    return retval;
}

/*
 * Vim's version of strpbrk(), in case it's missing.
 * Don't generate a prototype for this, causes problems when it's not used.
 */
#ifndef PROTO
# ifndef HAVE_STRPBRK
#  ifdef vim_strpbrk
#   undef vim_strpbrk
#  endif
    char_u *
vim_strpbrk(s, charset)
    char_u  *s;
    char_u  *charset;
{
    while (*s)
    {
	if (vim_strchr(charset, *s) != NULL)
	    return s;
	++s;
    }
    return NULL;
}
# endif
#endif

/*
 * Vim has its own isspace() function, because on some machines isspace()
 * can't handle characters above 128.
 */
    int
vim_isspace(x)
    int	    x;
{
    return ((x >= 9 && x <= 13) || x == ' ');
}

/************************************************************************
 * Functions for hanlding growing arrays.
 */

/*
 * Clear an allocated growing array.
 */
    void
ga_clear(ga)
    struct growarray *ga;
{
    vim_free(ga->ga_data);
    ga_init(ga);
}

/*
 * Initialize a growing array.	Don't forget to set ga_itemsize and ga_growsize!
 */
    void
ga_init(ga)
    struct growarray *ga;
{
    ga->ga_data = NULL;
    ga->ga_room = 0;
    ga->ga_len = 0;
}

/*
 * Make room in growing array "ga" for at least "n" items.
 * Return FAIL for failure, OK otherwise.
 */
    int
ga_grow(ga, n)
    struct growarray	*ga;
    int			n;
{
    size_t	    len;
    char_u	    *pp;

    if (ga->ga_room < n)
    {
	if (n < ga->ga_growsize)
	    n = ga->ga_growsize;
	len = ga->ga_itemsize * (ga->ga_len + n);
	pp = alloc_clear((unsigned)len);
	if (pp == NULL)
	    return FAIL;
	ga->ga_room = n;
	if (ga->ga_data != NULL)
	{
	    vim_memmove(pp, ga->ga_data,
				      (size_t)(ga->ga_itemsize * ga->ga_len));
	    vim_free(ga->ga_data);
	}
	ga->ga_data = pp;
    }
    return OK;
}

/************************************************************************
 * functions that use lookup tables for various things, generally to do with
 * special key codes.
 */

/*
 * Some useful tables.
 */

static struct modmasktable
{
    int	    mod_mask;	    /* Bit-mask for particular key modifier */
    char_u  name;	    /* Single letter name of modifier */
} mod_mask_table[] =
{
    {MOD_MASK_ALT,	(char_u)'M'},
    {MOD_MASK_CTRL,	(char_u)'C'},
    {MOD_MASK_SHIFT,	(char_u)'S'},
    {MOD_MASK_2CLICK,	(char_u)'2'},
    {MOD_MASK_3CLICK,	(char_u)'3'},
    {MOD_MASK_4CLICK,	(char_u)'4'},
    {0x0,		NUL}
};

/*
 * Shifted key terminal codes and their unshifted equivalent.
 * Don't add mouse codes here, they are handled seperately!
 */
static char_u shifted_keys_table[] =
{
/*  shifted			unshifted  */
    '&', '9',			'@', '1',		/* begin */
    '&', '0',			'@', '2',		/* cancel */
    '*', '1',			'@', '4',		/* command */
    '*', '2',			'@', '5',		/* copy */
    '*', '3',			'@', '6',		/* create */
    '*', '4',			'k', 'D',		/* delete char */
    '*', '5',			'k', 'L',		/* delete line */
    '*', '7',			'@', '7',		/* end */
    '*', '9',			'@', '9',		/* exit */
    '*', '0',			'@', '0',		/* find */
    '#', '1',			'%', '1',		/* help */
    '#', '2',			'k', 'h',		/* home */
    '#', '3',			'k', 'I',		/* insert */
    '#', '4',			'k', 'l',		/* left arrow */
    '%', 'a',			'%', '3',		/* message */
    '%', 'b',			'%', '4',		/* move */
    '%', 'c',			'%', '5',		/* next */
    '%', 'd',			'%', '7',		/* options */
    '%', 'e',			'%', '8',		/* previous */
    '%', 'f',			'%', '9',		/* print */
    '%', 'g',			'%', '0',		/* redo */
    '%', 'h',			'&', '3',		/* replace */
    '%', 'i',			'k', 'r',		/* right arrow */
    '%', 'j',			'&', '5',		/* resume */
    '!', '1',			'&', '6',		/* save */
    '!', '2',			'&', '7',		/* suspend */
    '!', '3',			'&', '8',		/* undo */
    KS_EXTRA, (int)KE_S_UP,	'k', 'u',		/* up arrow */
    KS_EXTRA, (int)KE_S_DOWN,	'k', 'd',		/* down arrow */

    KS_EXTRA, (int)KE_S_F1,	'k', '1',		/* F1 */
    KS_EXTRA, (int)KE_S_F2,	'k', '2',
    KS_EXTRA, (int)KE_S_F3,	'k', '3',
    KS_EXTRA, (int)KE_S_F4,	'k', '4',
    KS_EXTRA, (int)KE_S_F5,	'k', '5',
    KS_EXTRA, (int)KE_S_F6,	'k', '6',
    KS_EXTRA, (int)KE_S_F7,	'k', '7',
    KS_EXTRA, (int)KE_S_F8,	'k', '8',
    KS_EXTRA, (int)KE_S_F9,	'k', '9',
    KS_EXTRA, (int)KE_S_F10,	'k', ';',		/* F10 */

    KS_EXTRA, (int)KE_S_F11,	'F', '1',
    KS_EXTRA, (int)KE_S_F12,	'F', '2',
    KS_EXTRA, (int)KE_S_F13,	'F', '3',
    KS_EXTRA, (int)KE_S_F14,	'F', '4',
    KS_EXTRA, (int)KE_S_F15,	'F', '5',
    KS_EXTRA, (int)KE_S_F16,	'F', '6',
    KS_EXTRA, (int)KE_S_F17,	'F', '7',
    KS_EXTRA, (int)KE_S_F18,	'F', '8',
    KS_EXTRA, (int)KE_S_F19,	'F', '9',
    KS_EXTRA, (int)KE_S_F20,	'F', 'A',

    KS_EXTRA, (int)KE_S_F21,	'F', 'B',
    KS_EXTRA, (int)KE_S_F22,	'F', 'C',
    KS_EXTRA, (int)KE_S_F23,	'F', 'D',
    KS_EXTRA, (int)KE_S_F24,	'F', 'E',
    KS_EXTRA, (int)KE_S_F25,	'F', 'F',
    KS_EXTRA, (int)KE_S_F26,	'F', 'G',
    KS_EXTRA, (int)KE_S_F27,	'F', 'H',
    KS_EXTRA, (int)KE_S_F28,	'F', 'I',
    KS_EXTRA, (int)KE_S_F29,	'F', 'J',
    KS_EXTRA, (int)KE_S_F30,	'F', 'K',

    KS_EXTRA, (int)KE_S_F31,	'F', 'L',
    KS_EXTRA, (int)KE_S_F32,	'F', 'M',
    KS_EXTRA, (int)KE_S_F33,	'F', 'N',
    KS_EXTRA, (int)KE_S_F34,	'F', 'O',
    KS_EXTRA, (int)KE_S_F35,	'F', 'P',

    KS_EXTRA, (int)KE_S_TAB,	KS_EXTRA, (int)KE_TAB,	/* TAB pseudo code*/

    NUL
};

static struct key_name_entry
{
    int	    key;	/* Special key code or ascii value */
    char_u  *name;	/* Name of key */
} key_names_table[] =
{
    {' ',		(char_u *)"Space"},
    {TAB,		(char_u *)"Tab"},
    {K_TAB,		(char_u *)"Tab"},
    {NL,		(char_u *)"NL"},
    {NL,		(char_u *)"NewLine"},	/* Alternative name */
    {NL,		(char_u *)"LineFeed"},	/* Alternative name */
    {NL,		(char_u *)"LF"},	/* Alternative name */
    {CR,		(char_u *)"CR"},
    {CR,		(char_u *)"Return"},	/* Alternative name */
    {ESC,		(char_u *)"Esc"},
    {'|',		(char_u *)"Bar"},
    {'\\',		(char_u *)"Bslash"},
    {K_UP,		(char_u *)"Up"},
    {K_DOWN,		(char_u *)"Down"},
    {K_LEFT,		(char_u *)"Left"},
    {K_RIGHT,		(char_u *)"Right"},

    {K_F1,		(char_u *)"F1"},
    {K_F2,		(char_u *)"F2"},
    {K_F3,		(char_u *)"F3"},
    {K_F4,		(char_u *)"F4"},
    {K_F5,		(char_u *)"F5"},
    {K_F6,		(char_u *)"F6"},
    {K_F7,		(char_u *)"F7"},
    {K_F8,		(char_u *)"F8"},
    {K_F9,		(char_u *)"F9"},
    {K_F10,		(char_u *)"F10"},

    {K_F11,		(char_u *)"F11"},
    {K_F12,		(char_u *)"F12"},
    {K_F13,		(char_u *)"F13"},
    {K_F14,		(char_u *)"F14"},
    {K_F15,		(char_u *)"F15"},
    {K_F16,		(char_u *)"F16"},
    {K_F17,		(char_u *)"F17"},
    {K_F18,		(char_u *)"F18"},
    {K_F19,		(char_u *)"F19"},
    {K_F20,		(char_u *)"F20"},

    {K_F21,		(char_u *)"F21"},
    {K_F22,		(char_u *)"F22"},
    {K_F23,		(char_u *)"F23"},
    {K_F24,		(char_u *)"F24"},
    {K_F25,		(char_u *)"F25"},
    {K_F26,		(char_u *)"F26"},
    {K_F27,		(char_u *)"F27"},
    {K_F28,		(char_u *)"F28"},
    {K_F29,		(char_u *)"F29"},
    {K_F30,		(char_u *)"F30"},

    {K_F31,		(char_u *)"F31"},
    {K_F32,		(char_u *)"F32"},
    {K_F33,		(char_u *)"F33"},
    {K_F34,		(char_u *)"F34"},
    {K_F35,		(char_u *)"F35"},

    {K_HELP,		(char_u *)"Help"},
    {K_UNDO,		(char_u *)"Undo"},
    {K_BS,		(char_u *)"BS"},
    {K_BS,		(char_u *)"BackSpace"},	/* Alternative name */
    {K_INS,		(char_u *)"Insert"},
    {K_INS,		(char_u *)"Ins"},	/* Alternative name */
    {K_DEL,		(char_u *)"Del"},
    {K_DEL,		(char_u *)"Delete"},	/* Alternative name */
    {K_HOME,		(char_u *)"Home"},
    {K_END,		(char_u *)"End"},
    {K_PAGEUP,		(char_u *)"PageUp"},
    {K_PAGEDOWN,	(char_u *)"PageDown"},
    {K_KHOME,		(char_u *)"kHome"},
    {K_KEND,		(char_u *)"kEnd"},
    {K_KPAGEUP,		(char_u *)"kPageUp"},
    {K_KPAGEDOWN,	(char_u *)"kPageDown"},

    {K_MOUSE,		(char_u *)"Mouse"},
    {K_LEFTMOUSE,	(char_u *)"LeftMouse"},
    {K_LEFTDRAG,	(char_u *)"LeftDrag"},
    {K_LEFTRELEASE,	(char_u *)"LeftRelease"},
    {K_MIDDLEMOUSE,	(char_u *)"MiddleMouse"},
    {K_MIDDLEDRAG,	(char_u *)"MiddleDrag"},
    {K_MIDDLERELEASE,	(char_u *)"MiddleRelease"},
    {K_RIGHTMOUSE,	(char_u *)"RightMouse"},
    {K_RIGHTDRAG,	(char_u *)"RightDrag"},
    {K_RIGHTRELEASE,	(char_u *)"RightRelease"},
    {K_ZERO,		(char_u *)"Nul"},
    {0,			NULL}
};

#define KEY_NAMES_TABLE_LEN (sizeof(key_names_table) / sizeof(struct key_name_entry))

#ifdef USE_MOUSE
static struct mousetable
{
    int	    pseudo_code;	/* Code for pseudo mouse event */
    int	    button;		/* Which mouse button is it? */
    int	    is_click;		/* Is it a mouse button click event? */
    int	    is_drag;		/* Is it a mouse drag event? */
} mouse_table[] =
{
    {(int)KE_LEFTMOUSE,		MOUSE_LEFT,	TRUE,	FALSE},
    {(int)KE_LEFTDRAG,		MOUSE_LEFT,	FALSE,	TRUE},
    {(int)KE_LEFTRELEASE,	MOUSE_LEFT,	FALSE,	FALSE},
    {(int)KE_MIDDLEMOUSE,	MOUSE_MIDDLE,	TRUE,	FALSE},
    {(int)KE_MIDDLEDRAG,	MOUSE_MIDDLE,	FALSE,	TRUE},
    {(int)KE_MIDDLERELEASE,	MOUSE_MIDDLE,	FALSE,	FALSE},
    {(int)KE_RIGHTMOUSE,	MOUSE_RIGHT,	TRUE,	FALSE},
    {(int)KE_RIGHTDRAG,		MOUSE_RIGHT,	FALSE,	TRUE},
    {(int)KE_RIGHTRELEASE,	MOUSE_RIGHT,	FALSE,	FALSE},
    /* DRAG without CLICK */
    {(int)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	TRUE},
    /* RELEASE without CLICK */
    {(int)KE_IGNORE,		MOUSE_RELEASE,	FALSE,	FALSE},
    {0,				0,		0,	0},
};
#endif /* USE_MOUSE */

/*
 * Return the modifier mask bit (MOD_MASK_*) which corresponds to the given
 * modifier name ('S' for Shift, 'C' for Ctrl etc).
 */
    int
name_to_mod_mask(c)
    int	    c;
{
    int	    i;

    for (i = 0; mod_mask_table[i].mod_mask; i++)
	if (TO_LOWER(c) == TO_LOWER(mod_mask_table[i].name))
	    return mod_mask_table[i].mod_mask;
    return 0x0;
}

/*
 * Decide whether the given key code (K_*) is a shifted special
 * key (by looking at mod_mask).  If it is, then return the appropriate shifted
 * key code, otherwise just return the character as is.
 */
    int
check_shifted_spec_key(c)
    int	    c;
{
    return simplify_key(c, &mod_mask);
}

/*
 * Check if if there is a special key code for "key" that include the
 * modifiers specified.
 */
    int
simplify_key(key, modifiers)
    int	    key;
    int	    *modifiers;
{
    int	    i;
    int	    key0;
    int	    key1;

    if (*modifiers & MOD_MASK_SHIFT)
    {
	if (key == TAB)		/* TAB is a special case */
	{
	    *modifiers &= ~MOD_MASK_SHIFT;
	    return K_S_TAB;
	}
	key0 = KEY2TERMCAP0(key);
	key1 = KEY2TERMCAP1(key);
	for (i = 0; shifted_keys_table[i] != NUL; i += 4)
	    if (key0 == shifted_keys_table[i + 2] &&
					    key1 == shifted_keys_table[i + 3])
	    {
		*modifiers &= ~MOD_MASK_SHIFT;
		return TERMCAP2KEY(shifted_keys_table[i],
						   shifted_keys_table[i + 1]);
	    }
    }
    return key;
}

/*
 * Return a string which contains the name of the given key when the given
 * modifiers are down.
 */
    char_u *
get_special_key_name(c, modifiers)
    int	    c;
    int	    modifiers;
{
    static char_u string[MAX_KEY_NAME_LEN + 1];

    int	    i, idx;
    int	    table_idx;
    char_u  *s;

    string[0] = '<';
    idx = 1;

    /*
     * Translate shifted special keys into unshifted keys and set modifier.
     */
    if (IS_SPECIAL(c))
    {
	for (i = 0; shifted_keys_table[i]; i += 4)
	    if (       KEY2TERMCAP0(c) == shifted_keys_table[i]
		    && KEY2TERMCAP1(c) == shifted_keys_table[i + 1])
	    {
		modifiers |= MOD_MASK_SHIFT;
		c = TERMCAP2KEY(shifted_keys_table[i + 2],
						   shifted_keys_table[i + 3]);
		break;
	    }
    }

    /* try to find the key in the special key table */
    table_idx = find_special_key_in_table(c);

    /*
     * When not a known special key, and not a printable character, try to
     * extract modifiers.
     */
    if (table_idx < 0 && !vim_isprintc(c) && (c & 0x80))
    {
	c &= 0x7f;
	modifiers |= MOD_MASK_ALT;
	/* try again, to find the un-alted key in the special key table */
	table_idx = find_special_key_in_table(c);
    }
    if (table_idx < 0 && !vim_isprintc(c) && c < ' ')
    {
	c += '@';
	modifiers |= MOD_MASK_CTRL;
    }

    /* translate the modifier into a string */
    for (i = 0; mod_mask_table[i].mod_mask; i++)
	if (modifiers & mod_mask_table[i].mod_mask)
	{
	    string[idx++] = mod_mask_table[i].name;
	    string[idx++] = (char_u)'-';
	}

    if (table_idx < 0)		/* unknown special key, output t_xx */
    {
	if (IS_SPECIAL(c))
	{
	    string[idx++] = 't';
	    string[idx++] = '_';
	    string[idx++] = KEY2TERMCAP0(c);
	    string[idx++] = KEY2TERMCAP1(c);
	}
	/* Not a special key, only modifiers, output directly */
	else
	{
	    if (vim_isprintc(c))
		string[idx++] = c;
	    else
	    {
		s = transchar(c);
		while (*s)
		    string[idx++] = *s++;
	    }
	}
    }
    else		/* use name of special key */
    {
	STRCPY(string + idx, key_names_table[table_idx].name);
	idx = STRLEN(string);
    }
    string[idx++] = '>';
    string[idx] = NUL;
    return string;
}

/*
 * Try translating a <> name at (*srcp)[] to dst[].
 * Return the number of characters added to dst[], zero for no match.
 * If there is a match, srcp is advanced to after the <> name.
 * dst[] must be big enough to hold the result!
 */
    int
trans_special(srcp, dst)
    char_u  **srcp;
    char_u  *dst;
{
    int	    modifiers;
    int	    key;
    int	    dlen = 0;

    key = find_special_key(srcp, &modifiers);
    if (key == 0)
	return 0;

    /* Put the appropriate modifier in a string */
    if (modifiers != 0)
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KS_MODIFIER;
	dst[dlen++] = modifiers;
    }

    if (IS_SPECIAL(key))
    {
	dst[dlen++] = K_SPECIAL;
	dst[dlen++] = KEY2TERMCAP0(key);
	dst[dlen++] = KEY2TERMCAP1(key);
    }
    else
	dst[dlen++] = key;

    return dlen;
}

/*
 * Try translating a <> name at (*srcp)[], return the key and modifiers.
 * srcp is advanced to after the <> name.
 * returns 0 if there is no match.
 */
    int
find_special_key(srcp, modp)
    char_u	**srcp;
    int		*modp;
{
    char_u  *last_dash;
    char_u  *end_of_name;
    char_u  *src;
    char_u  *bp;
    int	    modifiers;
    int	    bit;
    int	    key;

    src = *srcp;
    if (src[0] != '<')
	return 0;

    /* Find end of modifier list */
    last_dash = src;
    for (bp = src + 1; *bp == '-' || vim_isIDc(*bp); bp++)
    {
	if (*bp == '-')
	{
	    last_dash = bp;
	    if (bp[1] != NUL && bp[2] == '>')
		++bp;	/* anything accepted, like <C-?> */
	}
	if (bp[0] == 't' && bp[1] == '_' && bp[2] && bp[3])
	    bp += 3;	/* skip t_xx, xx may be '-' or '>' */
    }

    if (*bp == '>')	/* found matching '>' */
    {
	end_of_name = bp + 1;

	/* Which modifiers are given? */
	modifiers = 0x0;
	for (bp = src + 1; bp < last_dash; bp++)
	{
	    if (*bp != '-')
	    {
		bit = name_to_mod_mask(*bp);
		if (bit == 0x0)
		    break;	/* Illegal modifier name */
		modifiers |= bit;
	    }
	}

	/*
	 * Legal modifier name.
	 */
	if (bp >= last_dash)
	{
	    /*
	     * Modifier with single letter, or special key name.
	     */
	    if (modifiers != 0 && last_dash[2] == '>')
		key = last_dash[1];
	    else
		key = get_special_key_code(last_dash + 1);

	    /*
	     * get_special_key_code() may return NUL for invalid
	     * special key name.
	     */
	    if (key != NUL)
	    {
		/*
		 * Only use a modifier when there is no special key code that
		 * includes the modifier.
		 */
		key = simplify_key(key, &modifiers);

		/*
		 * Normal Key with modifier: Try to make a single byte code.
		 */
		if (!IS_SPECIAL(key))
		{
		    if ((modifiers & MOD_MASK_SHIFT) && isalpha(key))
		    {
			key = TO_UPPER(key);
			modifiers &= ~MOD_MASK_SHIFT;
		    }
		    if ((modifiers & MOD_MASK_CTRL)
			    && ((key >= '?' && key <= '_') || isalpha(key)))
		    {
			if (key == '?')
			    key = DEL;
			else
			    key &= 0x1f;
			modifiers &= ~MOD_MASK_CTRL;
		    }
		    if ((modifiers & MOD_MASK_ALT) && key < 0x80)
		    {
			key |= 0x80;
			modifiers &= ~MOD_MASK_ALT;
		    }
		}

		*modp = modifiers;
		*srcp = end_of_name;
		return key;
	    }
	}
    }
    return 0;
}

/*
 * Try to find key "c" in the special key table.
 * Return the index when found, -1 when not found.
 */
    int
find_special_key_in_table(c)
    int	    c;
{
    int	    i;

    for (i = 0; key_names_table[i].name != NULL; i++)
	if (c == key_names_table[i].key)
	    break;
    if (key_names_table[i].name == NULL)
	i = -1;
    return i;
}

/*
 * Find the special key with the given name (the given string does not have to
 * end with NUL, the name is assumed to end before the first non-idchar).
 * If the name starts with "t_" the next two characters are interpreted as a
 * termcap name.
 * Return the key code, or 0 if not found.
 */
    int
get_special_key_code(name)
    char_u  *name;
{
    char_u  *table_name;
    char_u  string[3];
    int	    i, j;

    /*
     * If it's <t_xx> we get the code for xx from the termcap
     */
    if (name[0] == 't' && name[1] == '_' && name[2] != NUL && name[3] != NUL)
    {
	string[0] = name[2];
	string[1] = name[3];
	string[2] = NUL;
	if (add_termcap_entry(string, FALSE) == OK)
	    return TERMCAP2KEY(name[2], name[3]);
    }
    else
	for (i = 0; key_names_table[i].name != NULL; i++)
	{
	    table_name = key_names_table[i].name;
	    for (j = 0; vim_isIDc(name[j]) && table_name[j] != NUL; j++)
		if (TO_LOWER(table_name[j]) != TO_LOWER(name[j]))
		    break;
	    if (!vim_isIDc(name[j]) && table_name[j] == NUL)
		return key_names_table[i].key;
	}
    return 0;
}

    char_u *
get_key_name(i)
    int	    i;
{
    if (i >= KEY_NAMES_TABLE_LEN)
	return NULL;
    return  key_names_table[i].name;
}

#ifdef USE_MOUSE
/*
 * Look up the given mouse code to return the relevant information in the other
 * arguments.  Return which button is down or was released.
 */
    int
get_mouse_button(code, is_click, is_drag)
    int	    code;
    int	    *is_click;
    int	    *is_drag;
{
    int	    i;

    for (i = 0; mouse_table[i].pseudo_code; i++)
	if (code == mouse_table[i].pseudo_code)
	{
	    *is_click = mouse_table[i].is_click;
	    *is_drag = mouse_table[i].is_drag;
	    return mouse_table[i].button;
	}
    return 0;	    /* Shouldn't get here */
}

/*
 * Return the appropriate pseudo mouse event token (KE_LEFTMOUSE etc) based on
 * the given information about which mouse button is down, and whether the
 * mouse was clicked, dragged or released.
 */
    int
get_pseudo_mouse_code(button, is_click, is_drag)
    int	    button;	/* eg MOUSE_LEFT */
    int	    is_click;
    int	    is_drag;
{
    int	    i;

    for (i = 0; mouse_table[i].pseudo_code; i++)
	if (button == mouse_table[i].button
	    && is_click == mouse_table[i].is_click
	    && is_drag == mouse_table[i].is_drag)
	{
	    return mouse_table[i].pseudo_code;
	}
    return (int)KE_IGNORE;	    /* not recongnized, ignore it */
}
#endif /* USE_MOUSE */

/*
 * Return the current end-of-line type: EOL_DOS, EOL_UNIX or EOL_MAC.
 */
    int
get_fileformat(buf)
    BUF	    *buf;
{
    int	    c = *buf->b_p_ff;

    if (buf->b_p_bin || c == 'u')
	return EOL_UNIX;
    if (c == 'm')
	return EOL_MAC;
    return EOL_DOS;
}

/*
 * Set the current end-of-line type to EOL_DOS, EOL_UNIX or EOL_MAC.
 * Sets both 'textmode' and 'fileformat'.
 */
    void
set_fileformat(t)
    int		t;
{
    switch (t)
    {
    case EOL_DOS:
	set_string_option_direct((char_u *)"ff", -1, (char_u *)FF_DOS, TRUE);
	curbuf->b_p_tx = TRUE;
	break;
    case EOL_UNIX:
	set_string_option_direct((char_u *)"ff", -1, (char_u *)FF_UNIX, TRUE);
	curbuf->b_p_tx = FALSE;
	break;
    case EOL_MAC:
	set_string_option_direct((char_u *)"ff", -1, (char_u *)FF_MAC, TRUE);
	curbuf->b_p_tx = FALSE;
	break;
    }
    check_status(curbuf);
}

/*
 * Return the default fileformat from 'fileformats'.
 */
    int
default_fileformat()
{
    switch (*p_ffs)
    {
	case 'm':   return EOL_MAC;
	case 'd':   return EOL_DOS;
    }
    return EOL_UNIX;
}

/*
 * Call shell.	Calls mch_call_shell, with 'shellxquote' added.
 */
    int
call_shell(cmd, opt)
    char_u	*cmd;
    int		opt;
{
    char_u	*ncmd;
    int		retval = FAIL;

    if (cmd == NULL || *p_sxq == NUL)
	return mch_call_shell(cmd, opt);

    ncmd = alloc((unsigned)(STRLEN(cmd) + STRLEN(p_sxq) * 2 + 1));
    if (ncmd != NULL)
    {
	STRCPY(ncmd, p_sxq);
	STRCAT(ncmd, cmd);
	STRCAT(ncmd, p_sxq);
	retval = mch_call_shell(ncmd, opt);
	vim_free(ncmd);
    }
    return retval;
}
