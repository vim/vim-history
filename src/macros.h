/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * macros.h: macro definitions for often used code
 */

/*
 * pchar(lp, c) - put character 'c' at position 'lp'
 */
#define pchar(lp, c) (*(ml_get_buf(curbuf, (lp).lnum, TRUE) + (lp).col) = (c))

/*
 * Position comparisons
 */
#ifdef FEAT_VIRTUALEDIT
# define lt(a, b) (((a).lnum != (b).lnum) \
		   ? (a).lnum < (b).lnum \
		   : (a).col != (b).col \
		       ? (a).col < (b).col \
		       : (a).coladd < (b).coladd)
# define ltp(a, b) (((a)->lnum != (b)->lnum) \
		   ? (a)->lnum < (b)->lnum \
		   : (a)->col != (b)->col \
		       ? (a)->col < (b)->col \
		       : (a)->coladd < (b)->coladd)
# define equal(a, b) (((a).lnum == (b).lnum) && ((a).col == (b).col) && ((a).coladd == (b).coladd))
#else
# define lt(a, b) (((a).lnum != (b).lnum) \
		   ? ((a).lnum < (b).lnum) : ((a).col < (b).col))
# define ltp(a, b) (((a)->lnum != (b)->lnum) \
		   ? ((a)->lnum < (b)->lnum) : ((a)->col < (b)->col))
# define equal(a, b) (((a).lnum == (b).lnum) && ((a).col == (b).col))
#endif

#define ltoreq(a, b) (lt(a, b) || equal(a, b))

/*
 * lineempty() - return TRUE if the line is empty
 */
#define lineempty(p) (*ml_get(p) == NUL)

/*
 * bufempty() - return TRUE if the current buffer is empty
 */
#define bufempty() (curbuf->b_ml.ml_line_count == 1 && *ml_get((linenr_T)1) == NUL)

/*
 * On some systems toupper()/tolower() only work on lower/uppercase characters
 * Careful: Only call TO_UPPER() and TO_LOWER() with a character in the range
 * 0 - 255.  toupper()/tolower() on some systems can't handle others.
 * Note: for UTF-8 use utf_toupper() and utf_tolower().
 */
#ifdef MSWIN
#  define TO_UPPER(c)	toupper_tab[(c) & 255]
#  define TO_LOWER(c)	tolower_tab[(c) & 255]
#else
# ifdef BROKEN_TOUPPER
#  define TO_UPPER(c)	(islower(c) ? toupper(c) : (c))
#  define TO_LOWER(c)	(isupper(c) ? tolower(c) : (c))
# else
#  define TO_UPPER	toupper
#  define TO_LOWER	tolower
# endif
#endif

/*
 * MB_ISLOWER() and MB_ISUPPER() are to be used on multi-byte characters.  But
 * don't use them for negative values or values above 0x100 for DBCS.
 */
#ifdef FEAT_MBYTE
# define MB_ISLOWER(c)	(enc_utf8 && (c) > 0x80 ? utf_islower(c) : islower(c))
# define MB_ISUPPER(c)	(enc_utf8 && (c) > 0x80 ? utf_isupper(c) : isupper(c))
# define MB_TOLOWER(c)	(enc_utf8 && (c) > 0x80 ? utf_tolower(c) : TO_LOWER(c))
# define MB_TOUPPER(c)	(enc_utf8 && (c) > 0x80 ? utf_toupper(c) : TO_UPPER(c))
#else
# define MB_ISLOWER(c)	islower(c)
# define MB_ISUPPER(c)	isupper(c)
# define MB_TOLOWER(c)	TO_LOWER(c)
# define MB_TOUPPER(c)	TO_UPPER(c)
#endif

/* Like isalpha() but reject non-ASCII characters.  Can't be used with a
 * special key (negative value). */
#ifdef EBCDIC
# define ASCII_ISALPHA(c) isalpha(c)
# define ASCII_ISALNUM(c) isalnum(c)
# define ASCII_ISLOWER(c) islower(c)
# define ASCII_ISUPPER(c) isupper(c)
#else
# define ASCII_ISALPHA(c) ((c) < 0x7f && isalpha(c))
# define ASCII_ISALNUM(c) ((c) < 0x7f && isalnum(c))
# define ASCII_ISLOWER(c) ((c) < 0x7f && islower(c))
# define ASCII_ISUPPER(c) ((c) < 0x7f && isupper(c))
#endif

/* macro version of chartab().
 * Only works with values 0-255!
 * Doesn't work for UTF-8 mode with chars >= 0x80. */
#define CHARSIZE(c)	(chartab[c] & CT_CELL_MASK)

#ifdef FEAT_LANGMAP
/*
 * Adjust chars in a language according to 'langmap' option.
 * NOTE that there is NO overhead if 'langmap' is not set; but even
 * when set we only have to do 2 ifs and an array lookup.
 * Don't apply 'langmap' if the character comes from the Stuff buffer.
 * The do-while is just to ignore a ';' after the macro.
 */
# define LANGMAP_ADJUST(c, condition) do { \
	if (*p_langmap && (condition) && !KeyStuffed && (c) >= 0 && (c) < 256) \
	    c = langmap_mapchar[c]; \
    } while (0)
#endif

/*
 * vim_isbreak() is used very often if 'linebreak' is set, use a macro to make
 * it work fast.
 */
#define vim_isbreak(c) (breakat_flags[(char_u)(c)])

/*
 * On VMS file names are different and require a translation.
 * On the Mac open() has only two arguments.
 */
#ifdef VMS
# define mch_access(n, p)	access(vms_fixfilename(n), (p))
				/* see mch_open() comment */
# define mch_fopen(n, p)	fopen(vms_fixfilename(n), (p))
# define mch_fstat(n, p)	fstat(vms_fixfilename(n), (p))
	/* VMS does not have lstat() */
# define mch_stat(n, p)		stat(vms_fixfilename(n), (p))
#else
# ifndef WIN32
#   define mch_access(n, p)	access((n), (p))
# endif
# define mch_fopen(n, p)	fopen((n), (p))
# define mch_fstat(n, p)	fstat((n), (p))
# define mch_lstat(n, p)	lstat((n), (p))
# ifdef MSWIN	/* has it's own mch_stat() function */
#  define mch_stat(n, p)	vim_stat((n), (p))
# else
#  define mch_stat(n, p)	stat((n), (p))
# endif
#endif

#ifdef MACOS_CLASSIC
/* MacOS classic doesn't support perm but MacOS X does. */
# define mch_open(n, m, p)	open((n), (m))
#else
# ifdef VMS
/*
 * It is possible to force some record format with:
 * #  define mch_open(n, m, p) open(vms_fixfilename(n), (m), (p)), "rat=cr", "rfm=stmlf", "mrs=0")
 * but it is not recomended, because it can destroy indexes etc.
 */
#  define mch_open(n, m, p)	open(vms_fixfilename(n), (m), (p))
# else
#  define mch_open(n, m, p)	open((n), (m), (p))
# endif
#endif

/*
 * Encryption macros.  Mohsin Ahmed, mosh@sasi.com 98-09-24
 * Based on zip/crypt sources.
 */

#ifdef FEAT_CRYPT

#ifndef __MINGW32__
# define PWLEN 80
#endif

/* encode byte c, using temp t.  Warning: c must not have side effects. */
# define ZENCODE(c, t)  (t = decrypt_byte(), update_keys(c), t^(c))

/* decode byte c in place */
# define ZDECODE(c)   update_keys(c ^= decrypt_byte())

#endif

#ifdef STARTUPTIME
# define TIME_MSG(s) time_msg(s, NULL)
#else
# define TIME_MSG(s)
#endif

#ifdef FEAT_VREPLACE
# define REPLACE_NORMAL(s) (((s) & REPLACE_FLAG) && !((s) & VREPLACE_FLAG))
#else
# define REPLACE_NORMAL(s) ((s) & REPLACE_FLAG)
#endif
