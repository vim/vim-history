/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include "vim.h"

#ifdef FEAT_LINEBREAK
static int win_chartabsize __ARGS((win_t *wp, char_u *p, colnr_t col));
#endif

#if defined(FEAT_MBYTE)
static int nr2hex __ARGS((int c));
#endif

/*
 * chartab[] is used
 * - to quickly recognize ID characters
 * - to quickly recognize file name characters
 * - to quickly recognize printable characters
 * - to store the size of a character on the screen: Printable is 1 position,
 *   2 otherwise
 */
static int    chartab_initialized = FALSE;

/*
 * init_chartab(): Fill chartab[] with flags for ID and file name characters
 * and the size of characters on the screen (1 or 2 positions).
 * Also fills curbuf->b_chartab[] with flags for keyword characters for
 * current buffer.
 *
 * Return FAIL if 'iskeyword', 'isident', 'isfname' or 'isprint' option has an
 * error, OK otherwise.
 */
    int
init_chartab()
{
    return buf_init_chartab(curbuf, TRUE);
}

    int
buf_init_chartab(buf, global)
    buf_t	*buf;
    int		global;		/* FALSE: only set buf->b_chartab[] */
{
    int		c;
    int		c2;
    char_u	*p;
    int		i;
    int		tilde;
    int		do_isalpha;

    if (global)
    {
	/*
	 * Set the default size for printable characters:
	 * From <Space> to '~' is 1 (printable), others are 2 (not printable).
	 * This also inits all 'isident' and 'isfname' flags to FALSE.
	 *
	 * EBCDIC: all chars below ' ' are not printable, all others are
	 * printable.
	 */
	c = 0;
	while (c < ' ')
	    chartab[c++] = 2;
#ifdef EBCDIC
	while (c < 255)
#else
	while (c <= '~')
#endif
	    chartab[c++] = 1 + CT_PRINT_CHAR;
#ifdef FEAT_FKMAP
	if (p_altkeymap)
	{
	    while (c < YE)
		chartab[c++] = 1 + CT_PRINT_CHAR;
	}
#endif
	while (c < 256)
	{
#ifdef FEAT_MBYTE
	    /* UTF-8: can't tell width from first byte unless it's ASCII */
	    if (cc_utf8 && c >= 0xa0)
		chartab[c++] = CT_PRINT_CHAR + 1;
	    /* double-byte chars are either unprintable or double-width */
	    else if (cc_dbcs && MB_BYTE2LEN(c) == 2)
		chartab[c++] = 2;
	    else
#endif
		/* the rest is unprintable by default */
		chartab[c++] = 2;
	}

#ifdef FEAT_MBYTE
	/* Assume that every multi-byte char is a filename character. */
	for (c = 1; c < 256; ++c)
	    if ((cc_dbcs && MB_BYTE2LEN(c) > 1) || (cc_utf8 && c >= 0xa0))
		chartab[c] |= CT_FNAME_CHAR;
#endif
    }

    /*
     * Init word char flags all to FALSE
     */
    for (c = 0; c < 256; ++c)
    {
#ifdef FEAT_MBYTE
	/* double-byte characters are probably word characters */
	if (cc_dbcs && MB_BYTE2LEN(c) == 2)
	    buf->b_chartab[c] = TRUE;
	else
#endif
	    buf->b_chartab[c] = FALSE;
    }

#ifdef FEAT_LISP
    /*
     * In lisp mode the '-' character is included in keywords.
     */
    if (buf->b_p_lisp)
	buf->b_chartab['-'] = TRUE;
#endif

    /* Walk through the 'isident', 'iskeyword', 'isfname' and 'isprint'
     * options Each option is a list of characters, character numbers or
     * ranges, separated by commas, e.g.: "200-210,x,#-178,-"
     */
    for (i = global ? 0 : 3; i <= 3; ++i)
    {
	if (i == 0)
	    p = p_isi;		/* first round: 'isident' */
	else if (i == 1)
	    p = p_isp;		/* second round: 'isprint' */
	else if (i == 2)
	    p = p_isf;		/* third round: 'isfname' */
	else	/* i == 3 */
	    p = buf->b_p_isk;	/* fourth round: 'iskeyword' */

	while (*p)
	{
	    tilde = FALSE;
	    do_isalpha = FALSE;
	    if (*p == '^' && p[1] != NUL)
	    {
		tilde = TRUE;
		++p;
	    }
	    if (isdigit(*p))
		c = getdigits(&p);
	    else
		c = *p++;
	    c2 = -1;
	    if (*p == '-' && p[1] != NUL)
	    {
		++p;
		if (isdigit(*p))
		    c2 = getdigits(&p);
		else
		    c2 = *p++;
	    }
	    if (c <= 0 || (c2 < c && c2 != -1) || c2 >= 256
						 || !(*p == NUL || *p == ','))
		return FAIL;

	    if (c2 == -1)	/* not a range */
	    {
		/*
		 * A single '@' (not "@-@"):
		 * Decide on letters being ID/printable/keyword chars with
		 * standard function isalpha(). This takes care of locale.
		 */
		if (c == '@')
		{
		    do_isalpha = TRUE;
		    c = 1;
		    c2 = 255;
		}
		else
		    c2 = c;
	    }
	    while (c <= c2)
	    {
		if (!do_isalpha || isalpha(c)
#ifdef FEAT_FKMAP
			|| (p_altkeymap && (F_isalpha(c) || F_isdigit(c)))
#endif
			    )
		{
		    if (i == 0)			/* (re)set ID flag */
		    {
			if (tilde)
			    chartab[c] &= ~CT_ID_CHAR;
			else
			    chartab[c] |= CT_ID_CHAR;
		    }
		    else if (i == 1)		/* (re)set printable */
		    {
			if (c < ' '
#ifndef EBCDIC
				|| c > '~'
#endif
#ifdef FEAT_FKMAP
				|| (p_altkeymap
				    && (F_isalpha(c) || F_isdigit(c)))
#endif
			    )
			{
			    if (tilde)
			    {
				chartab[c] = (chartab[c] & ~CT_CELL_MASK) + 2;
				chartab[c] &= ~CT_PRINT_CHAR;
			    }
			    else
			    {
				chartab[c] = (chartab[c] & ~CT_CELL_MASK) + 1;
				chartab[c] |= CT_PRINT_CHAR;
			    }
			}
		    }
		    else if (i == 2)		/* (re)set fname flag */
		    {
			if (tilde)
			    chartab[c] &= ~CT_FNAME_CHAR;
			else
			    chartab[c] |= CT_FNAME_CHAR;
		    }
		    else /* i == 3 */		/* (re)set keyword flag */
			buf->b_chartab[c] = !tilde;
		}
		++c;
	    }
	    p = skip_to_option_part(p);
	}
    }
    chartab_initialized = TRUE;
    return OK;
}

#if defined(FEAT_STL_OPT) || defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * Translate any special characters in buf[bufsize].
 * If there is not enough room, not all characters will be translated.
 */
    void
trans_characters(buf, bufsize)
    char_u  *buf;
    int	    bufsize;
{
    int	    len;	    /* length of string needing translation */
    int	    room;	    /* room in buffer after string */
    char_u  *new;	    /* translated character */
    int	    new_len;	    /* length of new[] */

    len = STRLEN(buf);
    room = bufsize - len;
    while (*buf != 0)
    {
#ifdef FEAT_MBYTE
	/* Assume a multi-byte character doesn't need translation. */
	if (has_mbyte && (new_len = mb_ptr2len_check(buf)) > 1)
	    len -= new_len;
	else
#endif
	{
	    new = transchar(*buf);
	    new_len = STRLEN(new);
	    if (new_len > 1)
	    {
		room -= new_len - 1;
		if (room <= 0)
		    return;
		mch_memmove(buf + new_len, buf + 1, (size_t)len);
	    }
	    mch_memmove(buf, new, (size_t)new_len);
	    --len;
	}
	buf += new_len;
    }
}
#endif

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Translate a string into allocated memory, replacing special chars with
 * printable chars.  Returns NULL when out of memory.
 */
    char_u *
transstr(s)
    char_u	*s;
{
    char_u	*res;
#ifdef FEAT_MBYTE
    int		l;
#endif

    res = alloc((unsigned)(vim_strsize(s) + 1));
    if (res != NULL)
    {
	*res = NUL;
	while (*s != NUL)
	{
#ifdef FEAT_MBYTE
	    if (has_mbyte && (l = mb_ptr2len_check(s)) > 1)
	    {
		STRNCAT(res, s, l);
		s += l;
	    }
	    else
#endif
		STRCAT(res, transchar(*s++));
	}
    }
    return res;
}
#endif

/*
 * Catch 22: chartab[] can't be initialized before the options are
 * initialized, and initializing options may cause transchar() to be called!
 * When chartab_initialized == FALSE don't use chartab[].
 */
    char_u *
transchar(c)
    int		c;
{
    static char_u	buf[5];
    int			i;

    i = 0;
    if (IS_SPECIAL(c))	    /* special key code, display as ~@ char */
    {
	buf[0] = '~';
	buf[1] = '@';
	i = 2;
	c = K_SECOND(c);
    }

    if ((!chartab_initialized && (
#ifdef EBCDIC
		    (c >= 64 && c < 255)
#else
		    (c >= ' ' && c <= '~')
#endif
#ifdef FEAT_FKMAP
			|| F_ischar(c)
#endif
		)) || (c < 256 && (chartab[c] & CT_PRINT_CHAR)))
    {
	/* printable character */
	buf[i] = c;
	buf[i + 1] = NUL;
    }
    else
	transchar_nonprint(buf + i, c);
    return buf;
}

/*
 * Convert non-printable character to two or more printable characters in
 * "buf[]".  "buf" needs to be able to hold five bytes.
 */
    void
transchar_nonprint(buf, c)
    char_u	*buf;
    int		c;
{
#ifdef EBCDIC
    /* For EBCDIC only the characters 0-63 and 255 are not printable */
    if (CtrlChar(c) != 0 || c == DEL)
#else
    if (c <= 0x7f)				    /* 0x00 - 0x1f and 0x7f */
#endif
    {
	if (c == NL)
	    c = NUL;			/* we use newline in place of a NUL */
	else if (c == CR && get_fileformat(curbuf) == EOL_MAC)
	    c = NL;		/* we use CR in place of  NL in this case */
	buf[0] = '^';
#ifdef EBCDIC
	if (c == DEL)
	    buf[1] = '?';		/* DEL displayed as ^? */
	else
	    buf[1] = CtrlChar(c);
#else
	buf[1] = c ^ 0x40;		/* DEL displayed as ^? */
#endif

	buf[2] = NUL;
    }
#ifdef FEAT_MBYTE
    else if (cc_utf8 && c >= 0x80)
    {
	transchar_hex(buf, c);
    }
#endif
#ifndef EBCDIC
    else if (c >= ' ' + 0x80 && c <= '~' + 0x80)    /* 0xa0 - 0xfe */
    {
	buf[0] = '|';
	buf[1] = c - 0x80;
	buf[2] = NUL;
    }
#else
    else if (c < 64)
    {
	buf[0] = '~';
	buf[1] = MetaChar(c);
	buf[2] = NUL;
    }
#endif
    else					    /* 0x80 - 0x9f and 0xff */
    {
	/*
	 * TODO: EBCDIC I don't know what to do with this chars, so I display
	 * them as '~?' for now
	 */
	buf[0] = '~';
#ifdef EBCDIC
	buf[1] = '?';			/* 0xff displayed as ~? */
#else
	buf[1] = (c - 0x80) ^ 0x40;	/* 0xff displayed as ~? */
#endif
	buf[2] = NUL;
    }
}

#if defined(FEAT_MBYTE) || defined(PROTO)
    void
transchar_hex(buf, c)
    char_u	*buf;
    int		c;
{
    buf[0] = '<';
    buf[1] = nr2hex((unsigned)c >> 4);
    buf[2] = nr2hex(c);
    buf[3] = '>';
    buf[4] = NUL;
}
#endif

/*
 * Return number of display cells occupied by byte "b".
 * Caller must make sure 0 <= b <= 255.
 * For multi-byte mode "b" must be the first byte of a character.
 * A TAB is counted as two cells: "^I".
 * For UTF-8 mode this will return 0 for bytes >= 0x80, because the number of
 * cells depends on further bytes.
 */
    int
byte2cells(b)
    int		b;
{
#ifdef FEAT_MBYTE
    if (cc_utf8 && b >= 0x80)
	return 0;
#endif
    return (chartab[b] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character "c".
 * "c" can be a special key (negative number) in which case 3 or 4 is returned.
 * A TAB is counted as two cells: "^I".
 */
    int
char2cells(c)
    int		c;
{
    if (IS_SPECIAL(c))
	return char2cells(K_SECOND(c)) + 2;
#ifdef FEAT_MBYTE
    if (c >= 0x80)
    {
	/* UTF-8: above 0x80 need to check the value */
	if (cc_utf8)
	    return utf_char2cells(c);
	/* DBCS: double-byte means double-width */
	if (cc_dbcs && c >= 0x100)
	    return 2;
    }
#endif
    return (chartab[c & 0xff] & CT_CELL_MASK);
}

/*
 * Return number of display cells occupied by character at "*p".
 * A TAB is counted as two cells: "^I".
 */
    int
ptr2cells(p)
    char_u	*p;
{
#ifdef FEAT_MBYTE
    /* For UTF-8 we need to look at more bytes if the first byte is >= 0x80. */
    if (cc_utf8 && *p >= 0x80)
	return mb_ptr2cells(p);
    /* For DBCS we can tell the cell count from the first byte. */
#endif
    return (chartab[*p] & CT_CELL_MASK);
}

/*
 * Return the number of characters string 's' will take on the screen,
 * counting TABs as two characters: "^I".
 */
    int
vim_strsize(s)
    char_u	*s;
{
    int		len = 0;

    while (*s)
    {
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    len += ptr2cells(s);
	    s += mb_ptr2len_check(s);
	}
	else
#endif
	    len += byte2cells(*s++);
    }
    return len;
}

/*
 * Return the number of characters 'c' will take on the screen, taking
 * into account the size of a tab.
 * Use a define to make it fast, this is used very often!!!
 * Also see getvcol() below.
 */

#define RET_WIN_BUF_CHARTABSIZE(wp, buf, p, col) \
    if (*(p) == TAB && (!(wp)->w_p_list || lcs_tab1)) \
    { \
	int ts; \
	ts = (buf)->b_p_ts; \
	return (int)(ts - (col % ts)); \
    } \
    else \
	return ptr2cells(p);

    int
chartabsize(p, col)
    char_u	*p;
    colnr_t	col;
{
    RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, p, col)
}

#ifdef FEAT_LINEBREAK
    static int
win_chartabsize(wp, p, col)
    win_t	*wp;
    char_u	*p;
    colnr_t	col;
{
    RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, p, col)
}
#endif

/*
 * return the number of characters the string 's' will take on the screen,
 * taking into account the size of a tab
 */
    int
linetabsize(s)
    char_u	*s;
{
    colnr_t	col = 0;

    while (*s != NUL)
	col += lbr_chartabsize_adv(&s, col);
    return (int)col;
}

/*
 * Like linetabsize(), but for a given window instead of the current one.
 */
    int
win_linetabsize(wp, s)
    win_t	*wp;
    char_u	*s;
{
    colnr_t	col = 0;

    while (*s != NUL)
    {
	col += win_lbr_chartabsize(wp, s, col, NULL);
#ifdef FEAT_MBYTE
	if (has_mbyte)
	    s += mb_ptr2len_check(s);
	else
#endif
	    ++s;
    }
    return (int)col;
}

/*
 * return TRUE if 'c' is a normal identifier character
 * letters and characters from 'isident' option.
 */
    int
vim_isIDc(c)
    int c;
{
    return (c > 0 && c < 0x100 && (chartab[c] & CT_ID_CHAR));
}

/*
 * return TRUE if 'c' is a keyword character: Letters and characters from
 * 'iskeyword' option for current buffer.
 */
    int
vim_iswordc(c)
    int c;
{
    return (c > 0 && c < 0x100 && curbuf->b_chartab[c]);
}

#if defined(FEAT_SYN_HL) || defined(PROTO)
    int
vim_iswordc_buf(c, buf)
    int		c;
    buf_t	*buf;
{
    return (c > 0 && c < 0x100 && buf->b_chartab[c]);
}
#endif

/*
 * return TRUE if 'c' is a valid file-name character
 * Assume characters above 0x100 are valid (multi-byte).
 */
    int
vim_isfilec(c)
    int	c;
{
    return (c >= 0x100 || (c > 0 && (chartab[c] & CT_FNAME_CHAR)));
}

/*
 * return TRUE if 'c' is a printable character
 * Assume characters above 0x100 are printable (multi-byte).
 */
    int
vim_isprintc(c)
    int c;
{
    return (c >= 0x100 || (c > 0 && (chartab[c] & CT_PRINT_CHAR)));
}

/*
 * return TRUE if 'c' is a printable character
 *
 * No check for (c < 0x100) to make it a bit faster.
 * This is the most often used function, keep it fast!
 */
    int
safe_vim_isprintc(c)
    int c;
{
    return (chartab[c] & CT_PRINT_CHAR);
}

/*
 * like chartabsize(), but also check for line breaks on the screen
 */
    int
lbr_chartabsize(s, col)
    unsigned char   *s;
    colnr_t	    col;
{
#ifdef FEAT_LINEBREAK
    if (!curwin->w_p_lbr && *p_sbr == NUL)
    {
#endif
	RET_WIN_BUF_CHARTABSIZE(curwin, curbuf, s, col)
#ifdef FEAT_LINEBREAK
    }
    return win_lbr_chartabsize(curwin, s, col, NULL);
#endif
}

/*
 * Call lbr_chartabsize() and advance the pointer.
 */
    int
lbr_chartabsize_adv(s, col)
    unsigned char   **s;
    colnr_t	    col;
{
    int		retval;

    retval = lbr_chartabsize(*s, col);
#ifdef FEAT_MBYTE
    if (has_mbyte)
	*s += mb_ptr2len_check(*s);
    else
#endif
	++*s;
    return retval;
}

/*
 * This function is used very often, keep it fast!!!!
 *
 * If "head" not NULL, set *head to the size of what we for 'showbreak' string
 * at start of line.  Warning: *head is only set if it's a non-zero value,
 * init to 0 before calling.
 */
/*ARGSUSED*/
    int
win_lbr_chartabsize(wp, s, col, head)
    win_t		*wp;
    unsigned char	*s;
    colnr_t		col;
    int			*head;
{
#ifdef FEAT_LINEBREAK
    int		c = *s;
    int		size;
    colnr_t	col2;
    colnr_t	colmax;
    int		added;
    int		numberextra;
    char_u	*ps;

    /*
     * No 'linebreak' and 'showbreak': return quickly.
     */
    if (!wp->w_p_lbr && *p_sbr == NUL)
    {
#endif
	RET_WIN_BUF_CHARTABSIZE(wp, wp->w_buffer, s, col)
#ifdef FEAT_LINEBREAK
    }

    /*
     * First get normal size, without 'linebreak'
     */
    size = win_chartabsize(wp, s, col);

    /*
     * If 'linebreak' set check at a blank before a non-blank if the line
     * needs a break here
     */
    if (wp->w_p_lbr && vim_isbreak(c) && !vim_isbreak(s[1])
	    && !wp->w_p_list && wp->w_p_wrap
# ifdef FEAT_VERTSPLIT
	    && wp->w_width != 0
# endif
       )
    {
	/*
	 * Count all characters from first non-blank after a blank up to next
	 * non-blank after a blank.
	 */
	numberextra = win_col_off(wp);
	col2 = col;
	colmax = W_WIDTH(wp) - numberextra;
	if (col >= colmax)
	    colmax += (((col - colmax)
			/ (colmax + win_col_off2(wp))) + 1)
			* (colmax + win_col_off2(wp));
	for (;;)
	{
	    ps = s;
# ifdef FEAT_MBYTE
	    if (has_mbyte)
		s += mb_ptr2len_check(s);
	    else
# endif
		++s;
	    c = *s;
	    if (!(c != NUL
		    && (vim_isbreak(c)
			|| (!vim_isbreak(c)
			    && (col2 == col || !vim_isbreak(*ps))))))
		break;

	    col2 += win_chartabsize(wp, s, col2);
	    if (col2 >= colmax)		/* doesn't fit */
	    {
		size = colmax - col;
		break;
	    }
	}
    }

    /*
     * May have to add something for 'showbreak' string at start of line
     * Set *head to the size of what we add.
     */
    added = 0;
    if (*p_sbr != NUL && wp->w_p_wrap && col != 0
# ifdef FEAT_VERTSPLIT
					     && wp->w_width != 0
# endif
	    )
    {
	numberextra = win_col_off(wp);
	col += numberextra;
	if (col >= (colnr_t)W_WIDTH(wp))
	{
	    col -= W_WIDTH(wp);
	    numberextra -= win_col_off2(wp);
	    col = col % (W_WIDTH(wp) - numberextra);
	}
	if (col == 0 || col + size > (colnr_t)W_WIDTH(wp))
	{
	    added = STRLEN(p_sbr);
	    size += added;
	    if (col != 0)
		added = 0;
	}
    }
    if (head != NULL)
	*head = added;
    return size;
#endif
}

/*
 * Get virtual column number of pos.
 *  start: on the first position of this character (TAB, ctrl)
 * cursor: where the cursor is on this character (first char, except for TAB)
 *    end: on the last position of this character (TAB, ctrl)
 *
 * This is used very often, keep it fast!
 */
    void
getvcol(wp, pos, start, cursor, end)
    win_t	*wp;
    pos_t	*pos;
    colnr_t	*start;
    colnr_t	*cursor;
    colnr_t	*end;
{
    colnr_t	vcol;
    char_u	*ptr;		/* points to current char */
    char_u	*posptr;	/* points to char at pos->col */
    int		incr;
    int		head;
    int		ts = wp->w_buffer->b_p_ts;
    int		c;

    vcol = 0;
    ptr = ml_get_buf(wp->w_buffer, pos->lnum, FALSE);
    posptr = ptr + pos->col;

    /*
     * This function is used very often, do some speed optimizations.
     * When 'list', 'linebreak' and 'showbreak' are not set use a simple loop.
     * Also use this when 'list' is set but tabs take their normal size.
     */
    if ((!wp->w_p_list || lcs_tab1 != NUL)
#ifdef FEAT_LINEBREAK
	    && !wp->w_p_lbr && *p_sbr == NUL
#endif
       )
    {
	head = 0;
	for (;;)
	{
	    c = *ptr;
	    /* make sure we don't go past the end of the line */
	    if (c == NUL)
	    {
		incr = 1;	/* NUL at end of line only takes one column */
		break;
	    }
	    /* A tab gets expanded, depending on the current column */
	    if (c == TAB)
		incr = ts - (vcol % ts);
	    else
	    {
#ifdef FEAT_MBYTE
		if (has_mbyte)
		{
		    /* For utf-8, if the byte is >= 0x80, need to look at
		     * further bytes to find the cell width. */
		    if (cc_utf8 && c >= 0x80)
			incr = mb_ptr2cells(ptr);
		    else
			incr = CHARSIZE(c);

		    /* If a double-cell char doesn't fit at the end of a line
		     * it wraps to the next line, it's like this char is three
		     * cells wide. */
		    if (incr == 2
			    && wp->w_p_wrap
#ifdef FEAT_VERTSPLIT
			    && wp->w_width != 0
#endif
			    && (int)(vcol % W_WIDTH(wp))
						      == (int)W_WIDTH(wp) - 1)
			++incr;
		}
		else
#endif
		    incr = CHARSIZE(c);
	    }

	    if (ptr >= posptr)	/* character at pos->col */
		break;

	    vcol += incr;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		ptr += mb_ptr2len_check(ptr);
	    else
#endif
		++ptr;
	}
    }
    else
    {
	for (;;)
	{
	    /* A tab gets expanded, depending on the current column */
	    head = 0;
	    incr = win_lbr_chartabsize(wp, ptr, vcol, &head);
	    /* make sure we don't go past the end of the line */
	    if (*ptr == NUL)
	    {
		incr = 1;	/* NUL at end of line only takes one column */
		break;
	    }

#ifdef FEAT_MBYTE
	    /* If a double-cell char doesn't fit at the end of a line it wraps
	     * to the next line, it's like this char is three cells wide. */
	    if (has_mbyte
		    && mb_ptr2cells(ptr) == 2
		    && wp->w_p_wrap
#ifdef FEAT_VERTSPLIT
		    && wp->w_width != 0
#endif
		    && (int)(vcol % W_WIDTH(wp)) == (int)W_WIDTH(wp) - 1)
		++incr;
#endif
	    if (ptr >= posptr)	/* character at pos->col */
		break;

	    vcol += incr;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		ptr += mb_ptr2len_check(ptr);
	    else
#endif
		++ptr;
	}
    }
    if (start != NULL)
	*start = vcol + head;
    if (end != NULL)
	*end = vcol + incr - 1;
    if (cursor != NULL)
    {
	if (*ptr == TAB
		&& (State & NORMAL)
		&& !wp->w_p_list
#ifdef FEAT_VIRTUALEDIT
		&& !virtual_active()
#endif
#ifdef FEAT_VISUAL
		&& !(VIsual_active && *p_sel == 'e')
#endif
		)
	    *cursor = vcol + incr - 1;	    /* cursor at end */
	else
	    *cursor = vcol + head;	    /* cursor at start */
    }
}

#if defined(FEAT_VIRTUALEDIT) || defined(PROTO)
/*
 * Get virtual column in virtual mode.
 */
    void
getvvcol(wp, pos, start, cursor, end)
    win_t	*wp;
    pos_t	*pos;
    colnr_t	*start;
    colnr_t	*cursor;
    colnr_t	*end;
{
    int		add = 0;

    getvcol(wp, pos, start, cursor, end);

    if (virtual_active() && start != NULL)
    {
	/* We should pass the "coladd" value instead of guessing it... */
	if (pos->col == wp->w_cursor.col && pos->lnum == wp->w_cursor.lnum)
	    add = wp->w_coladd;
	else if (pos->col == curwin->w_cursor.col
					&& pos->lnum == curwin->w_cursor.lnum)
	    add = wp->w_coladd;
#ifdef FEAT_VISUAL
	else if (pos->col == VIsual.col && pos->lnum == VIsual.lnum)
	    add = VIsual_coladd;
#endif
	*start += add;
	if (cursor != NULL)
	    *cursor = *start;
	if (end != NULL)
	    *end = *start;
    }
}
#endif

/*
 * Get the most left and most right virtual column of pos1 and pos2.
 * Used for Visual block mode.
 */
    void
getvcols(pos1, pos2, left, right)
    pos_t    *pos1, *pos2;
    colnr_t *left, *right;
{
    colnr_t	l1, l2, r1, r2;

    getvvcol(curwin, pos1, &l1, NULL, &r1);
    getvvcol(curwin, pos2, &l2, NULL, &r2);
    if (l1 < l2)
	*left = l1;
    else
	*left = l2;
    if (r1 > r2)
	*right = r1;
    else
	*right = r2;
}

/*
 * skipwhite: skip over ' ' and '\t'.
 */
    char_u *
skipwhite(p)
    char_u	*p;
{
    while (vim_iswhite(*p)) /* skip to next non-white */
	++p;
    return p;
}

/*
 * skipdigits: skip over digits;
 */
    char_u *
skipdigits(p)
    char_u	*p;
{
    while (isdigit(*p))	/* skip to next non-digit */
	++p;
    return p;
}

/*
 * vim_isdigit: version of isdigit() that can handle characters > 0x100.
 */
    int
vim_isdigit(c)
    int	    c;
{
    return (c > 0 && c < 0x100 && isdigit(c));
}

/*
 * skiptowhite: skip over text until ' ' or '\t' or NUL.
 */
    char_u *
skiptowhite(p)
    char_u	*p;
{
    while (*p != ' ' && *p != '\t' && *p != NUL)
	++p;
    return p;
}

/*
 * skiptowhite_esc: Like skiptowhite(), but also skip escaped chars
 */
    char_u *
skiptowhite_esc(p)
    char_u	*p;
{
    while (*p != ' ' && *p != '\t' && *p != NUL)
    {
	if ((*p == '\\' || *p == Ctrl_V) && *(p + 1) != NUL)
	    ++p;
	++p;
    }
    return p;
}

/*
 * Getdigits: Get a number from a string and skip over it.
 * Note: the argument is a pointer to a char_u pointer!
 */
    long
getdigits(pp)
    char_u **pp;
{
    char_u	*p;
    long	retval;

    p = *pp;
    retval = atol((char *)p);
    if (*p == '-')		/* skip negative sign */
	++p;
    p = skipdigits(p);		/* skip to next non-digit */
    *pp = p;
    return retval;
}

/*
 * Return TRUE if "lbuf" is empty or only contains blanks.
 */
    int
vim_isblankline(lbuf)
    char_u	*lbuf;
{
    char_u	*p;

    p = skipwhite(lbuf);
    return (*p == NUL || *p == '\r' || *p == '\n');
}

/*
 * Convert a string into a long and/or unsigned long, taking care of
 * hexadecimal and octal numbers.
 * If "hexp" is not NULL, returns a flag to indicate the type of the number:
 *  0	    decimal
 *  '0'	    octal
 *  'X'	    hex
 *  'x'	    hex
 * If "len" is not NULL, the length of the number in characters is returned.
 * If "nptr" is not NULL, the signed result is returned in it.
 * If "unptr" is not NULL, the unsigned result is returned in it.
 */
    void
vim_str2nr(start, hexp, len, dooct, dohex, nptr, unptr)
    char_u		*start;
    int			*hexp;	    /* return: type of number 0 = decimal, 'x'
				       or 'X' is hex, '0' = octal */
    int			*len;	    /* return: detected length of number */
    int			dooct;	    /* recognize octal number */
    int			dohex;	    /* recognize hex number */
    long		*nptr;	    /* return: signed result */
    unsigned long	*unptr;	    /* return: unsigned result */
{
    char_u	    *ptr = start;
    int		    hex = 0;		/* default is decimal */
    int		    negative = FALSE;
    long	    n = 0;
    unsigned long   un = 0;

    if (ptr[0] == '-')
    {
	negative = TRUE;
	++ptr;
    }

    if (ptr[0] == '0')			/* could be hex or octal */
    {
	hex = ptr[1];
	if (dohex && (hex == 'X' || hex == 'x') && isxdigit(ptr[2]))
	    ptr += 2;			/* hexadecimal */
	else
	{
	    if (dooct && isdigit(hex))
		hex = '0';		/* octal */
	    else
		hex = 0;		/* 0 by itself is decimal */
	}
    }

    /*
     * Do the string-to-numeric conversion "manually" to avoid sscanf quirks.
     */
    if (hex)
    {
	if (hex == '0')
	{
	    /* octal */
	    while ('0' <= *ptr && *ptr <= '7')
	    {
		n = 8 * n + (long)(*ptr - '0');
		un = 8 * un + (unsigned long)(*ptr - '0');
		++ptr;
	    }
	}
	else
	{
	    /* hex */
	    while (isxdigit(*ptr))
	    {
		n = 16 * n + (long)hex2nr(*ptr);
		un = 16 * un + (unsigned long)hex2nr(*ptr);
		++ptr;
	    }
	}
    }
    else
    {
	/* decimal */
	while (isdigit(*ptr))
	{
	    n = 10 * n + (long)(*ptr - '0');
	    un = 10 * un + (unsigned long)(*ptr - '0');
	    ++ptr;
	}
    }

    if (!hex && negative)   /* account for leading '-' for decimal numbers */
	n = -n;

    if (hexp != NULL)
	*hexp = hex;
    if (len != NULL)
	*len = ptr - start;
    if (nptr != NULL)
	*nptr = n;
    if (unptr != NULL)
	*unptr = un;
}

/*
 * Return the value of a single hex character.
 * Only valid when the argument is '0' - '9', 'A' - 'F' or 'a' - 'f'.
 */
    int
hex2nr(c)
    int		c;
{
    if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
	return c - 'A' + 10;
    return c - '0';
}

#if defined(FEAT_MBYTE)
/*
 * Convert the lower 4 bits of byte "c" to its hex character.
 */
    static int
nr2hex(c)
    int		c;
{
    if ((c & 0xf) <= 9)
	return (c & 0xf) + '0';
    return (c & 0xf) - 10 + 'A';
}
#endif
