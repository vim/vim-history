/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * ops.h: things shared between normal.c, cmdline.c and ops.c
 */

/*
 * Operators
 */
#define NOP 	0				/* no pending operation */
#define DELETE	1
#define YANK	2
#define CHANGE	3
#define LSHIFT	4
#define RSHIFT	5
#define FILTER	6
#define TILDE	7
#define INDENT	8
#define FORMAT	9
#define COLON	10
#define UPPER	11
#define LOWER	12

/*
 * operator characters; the order must correspond to the defines above
 */
EXTERN char_u *opchars INIT(= (char_u *)"dyc<>!~=Q:Uu");

/*
 * When a cursor motion command is made, it is marked as being a character or
 * line oriented motion. Then, if an operator is in effect, the operation
 * becomes character or line oriented accordingly.
 *
 * Character motions are marked as being inclusive or not. Most char. motions
 * are inclusive, but some (e.g. 'w') are not.
 *
 * Generally speaking, every command in normal() should either clear any pending
 * operator (with CLEAROP), or set the motion type variable.
 */

/*
 * Motion types
 */
#define MBAD	(-1)			/* 'bad' motion type marks unusable yank buf */
#define MCHAR	0
#define MLINE	1
#define MBLOCK	2

EXTERN int		operator INIT(= NOP);	/* current pending operator */
EXTERN int		mtype;					/* type of the current cursor motion */
EXTERN int		mincl;					/* true if char motion is inclusive */
EXTERN colnr_t	startvcol;				/* start col for block mode operator */
EXTERN colnr_t	endvcol;				/* end col for block mode operator */
EXTERN long		nlines;					/* lines between startop and endop + 1 */
EXTERN int		yankbuffer INIT(= 0);	/* current yank buffer */
EXTERN int		no_op;					/* startop and endop the same */
