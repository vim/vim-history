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
 * ops.h: things shared between normal.c, cmdline.c and ops.c
 */

void	doshift __ARGS((int));
bool_t	dorecord __ARGS((int));
int		doexecbuf __ARGS((int));
void	dodelete __ARGS((void));
void	dotilde __ARGS((void));
void	dochange __ARGS((void));
bool_t	doyank __ARGS((bool_t));
void	doput __ARGS((int, long));
void	dodis __ARGS((void));
void	dodojoin __ARGS((long, bool_t));
bool_t	dojoin __ARGS((bool_t));
void	startinsert __ARGS((int, int, long));

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

/*
 * operator characters; the order must correspond to the defines above
 */
EXTERN char *opchars INIT(= "dyc<>!~=");

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

EXTERN int		operator INIT(= NOP);	/* current pending operator */
EXTERN int		mtype;					/* type of the current cursor motion */
EXTERN bool_t	mincl;					/* true if char motion is inclusive */
EXTERN int		oneless;				/* 1 if !mincl and startop != endop */
EXTERN FPOS 	startop;				/* cursor pos. at start of operator */
EXTERN FPOS		endop;					/* cursor pos. at end of operator */
EXTERN long		nlines;					/* lines between startop and endop + 1 */
EXTERN int		yankbuffer INIT(= 0);	/* current yank buffer */

