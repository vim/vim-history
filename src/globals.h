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
 * definition of global variables
 *
 * EXTERN is only defined in main.c
 */

#ifndef EXTERN
# define EXTERN extern
# define INIT(x)
#else
# define INIT(x) x
#endif

EXTERN int		Rows;			/* Number of Rows and Columns */
EXTERN int		Columns;		/* in the current window. */

EXTERN char 	*Filename INIT(= NULL);    /* Current file name */


EXTERN linenr_t Topline;		/* number of the line at the top of the screen */
EXTERN linenr_t Botline;		/* number of the line below the bottom of the
								 * screen */
EXTERN linenr_t line_count; 	/* current number of lines in the file */

EXTERN FPOS 	Curpos; 		/* current position of the cursor */

EXTERN int		Curscol;		/* Current position of cursor (column) */
EXTERN int		Cursrow;		/* Current position of cursor (row) */

EXTERN int		Cursvcol;		/* Current virtual column, the column number
								 * of the file's actual line, as opposed to
								 * the column number we're at on the screen.
								 * This makes a difference on lines that span
								 * more than one screen line. */

EXTERN colnr_t	Curswant INIT(= 0); 	/* The column we'd like to be at. This is
								 * used to try to stay in the same column
								 * through up/down cursor motions. */

EXTERN bool_t	set_want_col;	/* If set, then update Curswant the next time
								 * through cursupdate() to the current
								 * virtual column. */

EXTERN FPOS 	Insstart;		/* This is where the latest insert/append
								 * mode started. */

/*
 * This flag is used to make auto-indent work right on lines where only a
 * <RETURN> or <ESC> is typed. It is set when an auto-indent is done, and
 * reset when any other editting is done on the line. If an <ESC> or <RETURN>
 * is received, and did_ai is TRUE, the line is truncated.
 */
EXTERN bool_t		   did_ai INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN bool_t			did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent character will be removed.
 */
EXTERN bool_t			can_si INIT(= FALSE);

EXTERN int		State INIT(= NORMAL);	/* This is the current state of the command
										 * interpreter. */

EXTERN bool_t	Recording INIT(= FALSE); /* TRUE when recording into a buffer */

EXTERN bool_t	Changed INIT(= FALSE);/* Set to TRUE if something in the file has
								 * been changed and not written out. */

EXTERN char 	*IObuff;		/* sprintf's are done into this buffer */

EXTERN bool_t	RedrawingDisabled INIT(= FALSE);/* Set to TRUE if doing :g */

EXTERN bool_t	readonlymode INIT(= FALSE); /* Set to TRUE for "view" */
EXTERN bool_t	recoverymode INIT(= FALSE); /* Set to TRUE for "-r" option */

EXTERN bool_t	KeyTyped;				/* TRUE if user typed the character */

EXTERN char 	**files INIT(= NULL);	/* list of input files */
EXTERN int		numfiles INIT(= 0); 	/* number of input files */
EXTERN int		curfile INIT(= 0);		/* number of the current file */
#define NSCRIPT 15
EXTERN FILE 	*scriptin[NSCRIPT];		/* streams to read script from */
EXTERN int		curscript INIT(= 0);	/* index in scriptin[] */
EXTERN FILE 	*scriptout	INIT(= NULL); /* stream to write script to */

EXTERN bool_t	got_int INIT(= FALSE);	/* set to TRUE when interrupt
										   signal occurred */
#ifdef AUX
EXTERN bool_t	Aux_Device INIT(= FALSE);		/* set to TRUE when AUX: used */
#endif

extern char *Version;			/* this is in version.c */
extern char *longVersion;		/* this is in version.c */
