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

#define NSUBEXP  10
typedef struct regexp {
	char		   *startp[NSUBEXP];
	char		   *endp[NSUBEXP];
	char			regstart;	/* Internal use only. */
	char			reganch;	/* Internal use only. */
	char		   *regmust;	/* Internal use only. */
	int 			regmlen;	/* Internal use only. */
	char			program[1]; /* Unwarranted chumminess with compiler. */
}				regexp;

/* regexp.c */
regexp *regcomp __ARGS((char *));
int regexec __ARGS((regexp *, char *, int));
int cstrncmp __ARGS((char *, char *, int));
char *cstrchr __ARGS((char *, int));

/* regsub.c */
int regsub __ARGS((regexp *, char *, char *, int, int));

/* search.c */
extern void 	regerror __ARGS((char *));

#ifndef ORIGINAL
extern int		reg_ic; 		/* set non-zero to ignore case in searches */
extern int		reg_magic;		/* set zero to disable magicness of .*[~& */
#endif
