/* vi:ts=4:sw=4
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
 *
 * This is NOT the original regular expression code as written by
 * Henry Spencer. This code has been modified specifically for use
 * with the VIM editor, and should not be used apart from compiling
 * VIM. If you want a good regular expression library, get the
 * original code. The copyright notice that follows is from the
 * original.
 *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
 *
 * Definitions etc. for regexp(3) routines.
 *
 * Caveat:	this is V8 regexp(3) [actually, a reimplementation thereof],
 * not the System V one.
 */

#ifndef _REGEXP_H
#define _REGEXP_H

#define NSUBEXP  10
typedef struct regexp {
	char_u		   *startp[NSUBEXP];
	char_u		   *endp[NSUBEXP];
	char_u			regstart;	/* Internal use only. */
	char_u			reganch;	/* Internal use only. */
	char_u		   *regmust;	/* Internal use only. */
	int 			regmlen;	/* Internal use only. */
	char_u			program[1]; /* Unwarranted chumminess with compiler. */
}				regexp;

/* regexp.c */
regexp *regcomp __ARGS((char_u *));
int regexec __ARGS((regexp *, char_u *, int));
/* int cstrncmp __ARGS((char_u *, char_u *, int)); */
char_u *cstrchr __ARGS((char_u *, int));

/* regsub.c */
int regsub __ARGS((regexp *, char_u *, char_u *, int, int));

/* search.c */
extern void 	regerror __ARGS((char_u *));

#ifndef ORIGINAL
extern int		reg_ic; 		/* set non-zero to ignore case in searches */
extern int		reg_magic;		/* set zero to disable magicness of .*[~& */
#endif
#endif	/* _REGEXP_H */
