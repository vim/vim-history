/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Archimedes Machine-dependent things
 */

#if defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_TIME
#endif
#define HAVE_QSORT
#define HAVE_FCNTL_H
#define HAVE_STRFTIME	    /* guessed */
#define NO_EXPANDPATH		    /* always call mch_expand_wildcards */

#define vim_remove(x) remove((char *)(x))

/*
 * sorry, this file is missing
 */
