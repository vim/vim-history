/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
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
#define lt(a, b) (((a).lnum != (b).lnum) \
				   ? ((a).lnum < (b).lnum) : ((a).col < (b).col))

#define ltoreq(a, b) (((a).lnum != (b).lnum) \
				   ? ((a).lnum < (b).lnum) : ((a).col <= (b).col))

#define equal(a, b) (((a).lnum == (b).lnum) && ((a).col == (b).col))

/*
 * lineempty() - return TRUE if the line is empty
 */
#define lineempty(p) (*ml_get(p) == NUL)

/*
 * bufempty() - return TRUE if the file buffer is empty
 */
#define bufempty() (curbuf->b_ml.ml_flags & ML_EMPTY)

/*
 * On some systems toupper()/tolower() only work on lower/uppercase characters
 */
#if defined(sequent) || defined(DOMAIN) || !defined(__STDC__)
# define TO_UPPER(c)	(islower(c) ? toupper(c) : (c))
# define TO_LOWER(c)	(isupper(c) ? tolower(c) : (c))
#else
# define TO_UPPER		toupper
# define TO_LOWER		tolower
#endif
