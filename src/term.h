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
 * This file contains the machine dependent escape sequences that the editor
 * needs to perform various operations. Some of the sequences here are
 * optional. Anything not available should be indicated by a null string. In
 * the case of insert/delete line sequences, the editor checks the capability
 * and works around the deficiency, if necessary.
 *
 * Currently, insert/delete line sequences are used for screen scrolling. There
 * are lots of terminals that have 'index' and 'reverse index' capabilities,
 * but no line insert/delete. For this reason, the editor routines s_ins()
 * and s_del() should be modified to use 'index' sequences when the line to
 * be inserted or deleted at line zero.
 */

/*
 * The macro names here correspond (more or less) to the actual ANSI names
 */

#ifdef	ATARI
#define T_EL	"\033l" 		/* erase the entire current line */
#define T_IL	"\033L" 		/* insert one line */
#define T_IL_B	""
#define T_DL	"\033M" 		/* delete one line */
#define T_DL_B	""
#define T_SC	"\033j" 		/* save the cursor position */
#define T_ED	"\033E" 		/* erase display (may optionally home cursor) */
#define T_RC	"\033k" 		/* restore the cursor position */
#define T_CI	"\033f" 		/* invisible cursor (very optional) */
#define T_CV	"\033e" 		/* visible cursor (very optional) */
#define T_TP	""				/* plain text */
#define T_TI	""				/* inverse-video text */
#endif

#ifdef	UNIX
/*
 * The UNIX sequences are hard-wired for ansi-like terminals. I should really
 * use termcap/terminfo, but the UNIX port was done for profiling, not for
 * actual use, so it wasn't worth the effort.
 */
#define T_EL	"\033[2K"		/* erase the entire current line */
#define T_IL	"\033[L"		/* insert one line */
#define T_IL_B	""
#define T_DL	"\033[M"		/* delete one line */
#define T_DL_B	""
#define T_ED	"\033[2J"		/* erase display (may optionally home cursor) */
#define T_SC	"\0337" 		/* save the cursor position */
#define T_RC	"\0338" 		/* restore the cursor position */
#define T_CI	""				/* invisible cursor (very optional) */
#define T_CV	""				/* visible cursor (very optional) */
#define T_TP	""				/* plain text */
#define T_TI	""				/* inverse-video text */
#endif

#ifdef	BSD
/* The BSD 4.3 sequences are hard-wired for ansi-like terminals. */
#define T_EL	"\033[2K"		/* erase the entire current line */
#define T_IL	"\033[L"		/* insert line */
#define T_IL_B	""
#define T_DL	"\033[M"		/* delete line */
#define T_DL_B	""
#define T_ED	"\033[2J"		/* erase display (may optionally home cursor) */
#define T_SC	""				/* save the cursor position */
#define T_RC	""				/* restore the cursor position */
#define T_CI	""				/* invisible cursor (very optional) */
#define T_CV	""				/* visible cursor (very optional) */
#define T_TP	""				/* plain text */
#define T_TI	""				/* inverse-video text */
#endif

#ifdef	OS2
/*
 * The OS/2 ansi console driver is pretty deficient. No insert or delete line
 * sequences. The erase line sequence only erases from the cursor to the end
 * of the line. For our purposes that works out okay, since the only time
 * T_EL is used is when the cursor is in column 0.
 *
 * The insert/delete line sequences marked here are actually implemented in
 * the file os2.c using direct OS/2 system calls. This makes the capability
 * available for the rest of the editor via appropriate escape sequences
 * passed to outstr().
 */
#define T_EL	"\033[K"		/* erase the entire current line */
#define T_IL	"\033[L"		/* insert one line - fake (see os2.c) */
#define T_IL_B	""
#define T_DL	"\033[M"		/* delete one line - fake (see os2.c) */
#define T_DL_B	""
#define T_ED	"\033[2J"		/* erase display (may optionally home cursor) */
#define T_SC	"\033[s"		/* save the cursor position */
#define T_RC	"\033[u"		/* restore the cursor position */
#define T_CI	""				/* invisible cursor (very optional) */
#define T_CV	""				/* visible cursor (very optional) */
#define T_TP	""				/* plain text */
#define T_TI	""				/* inverse-video text */
#endif

#ifdef AMIGA
/*
 * The erase line sequence only erases from the cursor to the end of the
 * line. For our purposes that works out okay, since the only time T_EL is
 * used is when the cursor is in column 0.
 */
#define T_EL	"\033[K" 		/* erase the entire current line */
#define T_IL	"\033["			/* insert line */
#define T_IL_B	"L"
#define T_DL	"\033["			/* delete line */
#define T_DL_B	"M"
#ifdef AUX
# define T_ED	"\033[2J\014"	/* erase display (may optionally home cursor) */
#else
# define T_ED	"\014"			/* erase display (may optionally home cursor) */
#endif
#define T_RC	""				/* restore the cursor position */
#define T_SC	""				/* save the cursor position */
#define T_CI	"\033[0 p"		/* invisible cursor (very optional) */
#define T_CV	"\033[1 p"		/* visible cursor (very optional) */
#define T_TP	"\033[0m"		/* plain text */
#define T_TI	"\033[7m"		/* inverse-video text */
#endif

#ifdef	DOS
/*
 * DOS sequences
 *
 * Some of the following sequences require the use of the "nansi.sys"
 * console driver. The standard "ansi.sys" driver doesn't support
 * sequences for insert/delete line.
 */
#define T_EL	"\033[K"		/* erase the entire current line */
#define T_IL	"\033[L"		/* insert line (requires nansi.sys driver) */
#define T_IL_B	""
#define T_DL	"\033[M"		/* delete line (requires nansi.sys driver) */
#define T_DL_B	""
#define T_ED	"\033[2J"		/* erase display (may optionally home cursor) */
#define T_SC	"\033[s"		/* save the cursor position */
#define T_RC	"\033[u"		/* restore the cursor position */
#define T_CI	""				/* invisible cursor (very optional) */
#define T_CV	""				/* visible cursor (very optional) */
#define T_TP	""				/* plain text */
#define T_TI	""				/* inverse-video text */
#endif
