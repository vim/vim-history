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
 * The defines in this file establish the environment we're compiling
 * in. Set these appropriately before compiling the editor.
 */

/*
 * One (and only 1) of the following defines should be uncommented. Most of
 * the code is pretty machine-independent. Machine dependent code goes in a
 * file like tos.c or unix.c. The only other place where machine dependent
 * code goes is term.h for escape sequences.
 */

/* #define		ATARI			Atari ST */
/* #define		UNIX			System V */
/* #define		BSD 			BSD 4.3 */
/* #define		OS2 			Microsoft OS/2 */
/* #define		DOS 			MS DOS 3.3 */
/* #define		AMIGA			Amiga (this is given as an argument to cc) */

/*
 * If ATARI is defined, one of the following compilers must be selected.
 */
#ifdef	ATARI
/* #define MWC					Mark William's C 3.0.9 */
/* #define		MEGAMAX 		Megamax Compiler */
/* #define		ALCYON			Alcyon C compiler */

# ifdef MWC
#  define AppendNumberToUndoUndobuff	XX1
#  define AppendPositionToUndoUndobuff	XX2
#  define FOPENB
# endif

# ifdef MEGAMAX
#  define FOPENB
# endif
#endif

/*
 * STRCSPN should be defined if the target system doesn't have the
 * routine strcspn() available. See regexp.c for details.
 */

/* #ifdef		ATARI */
#define STRCSPN
/* #endif */

/*
 * The following defines control the inclusion of "optional" features. As
 * the code size of the editor grows, it will probably be useful to be able
 * to tailor the editor to get the features you most want in environments
 * with code size limits.
 *
 * TILDEOP
 *		Normally the '~' command works on a single character. This define
 *		turns on code that allows it to work like an operator. This is
 *		then enabled at runtime with the "tildeop" parameter.
 *
 * TERMCAP
 *		Where termcap support is provided, it is generally optional. If
 *		not enabled, you generally get hard-coded escape sequences for
 *		some "reasonable" terminal. In Minix, this means the console. For
 *		UNIX, this means an ANSI standard terminal. See the file "term.h"
 *		for details about specific environments.
 *
 */
#define TILDEOP 		/* enable tilde to be an operator */
#define TERMCAP 		/* enable termcap support */
