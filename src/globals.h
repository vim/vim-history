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
 * EXTERN is only defined in main.c (and in param.h)
 */

#ifndef EXTERN
# define EXTERN extern
# define INIT(x)
#else
# ifndef INIT
#  define INIT(x) x
# endif
#endif

EXTERN long		Rows;			/* Number of Rows and Columns */
EXTERN long		Columns;		/*     in the current window. */

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

EXTERN int		set_want_col;	/* If set, then update Curswant the next time
								 * through cursupdate() to the current
								 * virtual column. */

EXTERN int		exiting INIT(= FALSE);
								/* set to TRUE when abandoning the file */

EXTERN FPOS 	Quote; 			/* start position of quotation
								 * (Quote.lnum == 0 when not active) */
EXTERN int		Quote_block INIT(= FALSE);
								/* Quoting is blockwise */

EXTERN FPOS 	Insstart;		/* This is where the latest insert/append
								 * mode started. */

/*
 * This flag is used to make auto-indent work right on lines where only a
 * <RETURN> or <ESC> is typed. It is set when an auto-indent is done, and
 * reset when any other editting is done on the line. If an <ESC> or <RETURN>
 * is received, and did_ai is TRUE, the line is truncated.
 */
EXTERN int			   did_ai INIT(= FALSE);

/*
 * This flag is set when a smart indent has been performed. When the next typed
 * character is a '{' the inserted tab will be deleted again.
 */
EXTERN int				did_si INIT(= FALSE);

/*
 * This flag is set after an auto indent. If the next typed character is a '}'
 * one indent character will be removed.
 */
EXTERN int				can_si INIT(= FALSE);

EXTERN int		State INIT(= NORMAL);	/* This is the current state of the command
										 * interpreter. */

EXTERN int		Recording INIT(= FALSE);/* TRUE when recording into a buffer */

EXTERN int		Changed INIT(= FALSE);	/* Set to TRUE if something in the file has
								 		 * been changed and not written out. */

EXTERN int		Updated INIT(= FALSE);	/* Set to TRUE if something in the file has
								 		 * been changed and .vim not flushed yet */

EXTERN char 	*IObuff;				/* sprintf's are done into this buffer */

EXTERN int		RedrawingDisabled INIT(= FALSE);
										/* Set to TRUE if doing :g */

EXTERN int		readonlymode INIT(= FALSE); /* Set to TRUE for "view" */
EXTERN int		recoverymode INIT(= FALSE); /* Set to TRUE for "-r" option */

EXTERN int		KeyTyped;				/* TRUE if user typed the character */

EXTERN char 	**files INIT(= NULL);	/* list of input files */
EXTERN int		numfiles INIT(= 0); 	/* number of input files */
EXTERN int		curfile INIT(= 0);		/* number of the current file */
#define NSCRIPT 15
EXTERN FILE 	*scriptin[NSCRIPT];		/* streams to read script from */
EXTERN int		curscript INIT(= 0);	/* index in scriptin[] */
EXTERN FILE 	*scriptout	INIT(= NULL); /* stream to write script to */

EXTERN int		got_int INIT(= FALSE);	/* set to TRUE when interrupt
										   signal occurred */
#ifdef AMIGA
EXTERN int		term_console INIT(= TRUE);	/* set to TRUE when amiga window used */
#else
EXTERN int		term_console INIT(= FALSE);
#endif

#ifdef DEBUG
EXTERN FILE *debugfp INIT(=NULL);
#endif

extern char *Version;			/* this is in version.c */
extern char *longVersion;		/* this is in version.c */

/* just a string of 15 spaces */
EXTERN char spaces[]		INIT(= "               ");

/*
 * The error messages that can be shared are included here.
 * Excluded are very specific errors and debugging messages.
 */
EXTERN char e_abort[]		INIT(="Command aborted");
EXTERN char e_ambmap[]		INIT(="Ambiguous mapping");
EXTERN char e_argreq[]		INIT(="Argument required");
EXTERN char e_errorf[]		INIT(="No errorfile name");
EXTERN char e_exists[]		INIT(="File exists (use ! to override)");
EXTERN char e_failed[] 		INIT(="Command failed");
EXTERN char e_internal[]	INIT(="Internal error");
EXTERN char e_interr[]		INIT(="Interrupted");
EXTERN char e_invaddr[]		INIT(="Invalid address");
EXTERN char e_invarg[]		INIT(="Invalid argument");
EXTERN char e_invrange[]	INIT(="Invalid range");
EXTERN char e_invcmd[]		INIT(="Invalid command");
EXTERN char e_invstring[]	INIT(="Invalid search string");
EXTERN char e_more[]		INIT(="Still more files to edit");
EXTERN char e_nesting[]		INIT(="Scripts nested too deep");
EXTERN char e_noalt[]		INIT(="No alternate file");
EXTERN char e_nomap[]		INIT(="No such mapping");
EXTERN char e_nomatch[]		INIT(="No match");
EXTERN char e_nomore[]		INIT(="No more files to edit");
EXTERN char e_noname[]		INIT(="No file name");
EXTERN char e_nopresub[]	INIT(="No previous substitute");
EXTERN char e_noprev[]		INIT(="No previous command");
EXTERN char e_noprevre[]	INIT(="No previous regexp");
EXTERN char e_norange[] 	INIT(="No range allowed");
EXTERN char e_notcreate[] 	INIT(="Can't create file");
EXTERN char e_notmp[]		INIT(="Can't get temp file name");
EXTERN char e_notopen[]		INIT(="Can't open file");
EXTERN char e_notread[]		INIT(="Can't read file");
EXTERN char e_nowrtmsg[]	INIT(="No write since last change (use ! to override)");
EXTERN char e_null[]		INIT(="Null argument");
EXTERN char e_number[]		INIT(="Number expected");
EXTERN char e_openerrf[]	INIT(="Can't open errorfile");
EXTERN char e_outofmem[]	INIT(="Out of memory!");
EXTERN char e_patnotf[]		INIT(="Pattern not found");
EXTERN char e_positive[]	INIT(="Argument must be positive");
EXTERN char e_quickfix[]	INIT(="No errorfile; use :cf");
EXTERN char e_re_damg[]		INIT(="Damaged match string");
EXTERN char e_re_corr[]		INIT(="Corrupted regexp program");
EXTERN char e_readonly[]	INIT(="File is readonly");
EXTERN char e_readerrf[]	INIT(="Error while reading errorfile");
EXTERN char e_setparam[]	INIT(="Invalid 'set' parameter");
EXTERN char e_scroll[]		INIT(="Invalid scroll size");
EXTERN char e_tabsize[]		INIT(="Invalid tab size");
EXTERN char e_toocompl[]	INIT(="Command too complex");
EXTERN char e_toombra[]		INIT(="Too many (");
EXTERN char e_toomket[]		INIT(="Too many )");
EXTERN char e_toomsbra[]	INIT(="Too many [");
EXTERN char e_toolong[]		INIT(="Command too long");
EXTERN char e_toomany[]		INIT(="Too many file names");
EXTERN char e_trailing[]	INIT(="Trailing characters");
EXTERN char e_umark[]		INIT(="Unknown mark");
EXTERN char e_unknown[]		INIT(="Unknown");
EXTERN char e_unrset[]		INIT(="Unrecognized 'set' option");
EXTERN char e_write[]		INIT(="Error while writing");
EXTERN char e_zerocount[] 	INIT(="Zero count");
