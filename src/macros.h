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
 * macros.h: macro definitions for often used code
 */

/*
 * pchar(lp, c) - put character 'c' at position 'lp'
 */
#define pchar(lp, c) (*(nr2ptr((lp).lnum) + (lp).col) = (c))

/*
 * Position comparisons
 */
#define lt(a, b) (((a).lnum != (b).lnum) \
				   ? ((a).lnum < (b).lnum) : ((a).col < (b).col))

#define ltoreq(a, b) (((a).lnum != (b).lnum) \
				   ? ((a).lnum < (b).lnum) : ((a).col <= (b).col))

#define equal(a, b) (((a).lnum == (b).lnum) && ((a).col == (b).col))

/*
 * buf1line() - return TRUE if there is only one line in file buffer
 */
#define buf1line() (line_count == 1)

/*
 * lineempty() - return TRUE if the line is empty
 */
#define lineempty(p) (*nr2ptr(p) == NUL)

/*
 * bufempty() - return TRUE if the file buffer is empty
 */
#define bufempty() (buf1line() && lineempty((linenr_t)1))
