/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#include "env.h"

#include <stdio.h>

#ifndef UNIX
# include <stdlib.h>
#endif

#include <ctype.h>

#include <string.h>

#include "ascii.h"
#include "keymap.h"
#include "param.h"
#include "term.h"
#include "macros.h"
#ifdef LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#else
# ifdef _DCC
#  include <sys/stat.h>
# else
#  include <stat.h>
# endif
#endif

#ifdef AMIGA
/*
 * arpbase.h must be included before functions.h
 */
# include <libraries/arpbase.h>

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#include <signal.h>
#endif

#ifdef AZTEC_C
#include <functions.h>
#define __ARGS(x) x
#endif

#ifdef _DCC
#include <functions.h>
#define __ARGS(x) x
#endif

#ifndef __ARGS
#define __ARGS(x) ()
#endif

#define NORMAL					 0
#define CMDLINE 				 1
#define INSERT					 2
#define APPEND					 3
#define UNDO					 4
#define REDO					 5
#define PUT 					 6
#define VALID					 9
#define NOT_VALID				10
#define VALID_TO_CURSCHAR		11
#define REPLACE 				12	/* replace mode */
#define HELP					13
#define NOMAPPING 				14	/* no :mapping mode for vgetc() */
#define HITRETURN				15
#define SETWINSIZE				16
#define NORMAL_BUSY				17	/* busy interpreting a command */

#define FORWARD 				 1
#define BACKWARD				 -1

/*
 * Boolean type definition and constants
 */
typedef int 	bool_t;

#ifndef TRUE
#define FALSE	(0)			/* note: this is an int, not a long */
#define TRUE	(1)
#endif

#define YES 	 (1)
#define NO		 (0)

/*
 * Maximum screen dimensions
 */
#define MAX_COLUMNS 140L

/*
 * Buffer sizes
 */
#define CMDBUFFSIZE	256			/* size of the command processing buffer */

#define LSIZE		512			/* max. size of a line in the tags file */

#define IOSIZE	   (1024+1) 	/* file i/o and sprintf buffer size */

#define CHANGED   Changed = 1
#define UNCHANGED Changed = 0

typedef unsigned char	u_char;		/* shorthand */
typedef unsigned short	u_short;	/* shorthand */
typedef unsigned int	u_int;		/* shorthand */
typedef unsigned long	u_long;		/* shorthand */
typedef long			linenr_t;	/* line number type */
typedef unsigned		colnr_t;	/* column number type */
typedef struct fpos		FPOS;		/* file position type */

#define INVLNUM (0x7fffffff)		/* invalid line number */

struct fpos
{
		linenr_t		lnum;	/* line number */
		colnr_t 		col;	/* column number */
};

#include "globals.h"

/* alloc.c */
char	*alloc __ARGS((unsigned));
char	*lalloc __ARGS((u_long, bool_t));
char	*strsave __ARGS((char *));
char	*strnsave __ARGS((char *, int));
char	*mkstr __ARGS((unsigned));

/* buffers.c */
u_char	*get_bufcont __ARGS((struct buffheader *));
u_char	*get_recorded __ARGS((void));
u_char	*get_inserted __ARGS((void));
void	flush_buffers __ARGS((void));
void	ResetBuffers __ARGS((void));
void	AppendToRedobuff __ARGS((char *));
void	AppendNumberToRedobuff __ARGS((long));
void	stuffReadbuff __ARGS((char *));
void	stuffnumReadbuff __ARGS((long));
void	copy_redo __ARGS((void));
bool_t	start_redo __ARGS((long));
bool_t	start_redo_ins __ARGS((void));
void	stop_redo_ins __ARGS((void));
int		ins_mapbuf __ARGS((char *));
u_char	vgetc __ARGS((void));
u_char	vpeekc __ARGS((void));
int		domap __ARGS((int, char *, int));
int		makemap __ARGS((FILE *));

/* charset.c */
char	*transchar __ARGS((unsigned));
void	outtrans __ARGS((char *, int));
int		charsize __ARGS((int));
int		chartabsize __ARGS((int, int));
int		isidchar __ARGS((int));

/* cmdline.c */
bool_t	getcmdline __ARGS((int, u_char *));
void	redrawcmdline __ARGS((void));
void	redrawcmd __ARGS((void));
void	docmdline __ARGS((u_char *));
int		doarglist __ARGS((char *));
void	gotocmdline __ARGS((bool_t, int));
int		getfile __ARGS((char *, bool_t));
bool_t	samealtfile __ARGS((int));
int		getaltfile __ARGS((int, linenr_t, bool_t));
char	*getaltfname __ARGS((int));
int		dosource __ARGS((char *));

/* cmdsearch.c */
void	dosub __ARGS((linenr_t, linenr_t, char *, u_char **));
void	doglob __ARGS((int, linenr_t, linenr_t, char *));

/* edit.c */
void	edit __ARGS((long));
bool_t	oneright __ARGS((void));
bool_t	oneleft __ARGS((void));
void	beginline __ARGS((bool_t));
bool_t	oneup __ARGS((long));
bool_t	onedown __ARGS((long));

/* fileio.c */
void	filemess __ARGS((char *, char *));
bool_t	readfile __ARGS((char *, linenr_t));
bool_t	writeit __ARGS((char *, linenr_t, linenr_t, bool_t));

/* help.c */
void	help __ARGS((void));
int		redrawhelp __ARGS((void));

/* linefunc.c */
void	coladvance __ARGS((colnr_t));
int		inc __ARGS((FPOS *));
int		incCurpos __ARGS((void));
int		incl __ARGS((FPOS *));
int		dec __ARGS((FPOS *));
int		decCurpos __ARGS((void));
int		decl __ARGS((FPOS *));
void	adjustCurpos __ARGS((void));

/* main.c */
void	getout __ARGS((int));

/* mark.c */
bool_t	setmark __ARGS((int));
void	setpcmark __ARGS((void));
FPOS	*movemark __ARGS((int));
FPOS	*getmark __ARGS((int, bool_t));
void	clrallmarks __ARGS((void));
void	incrmarks __ARGS((void));
void	decrmarks __ARGS((void));
void	adjustmark __ARGS((char *, char *));
void	domarks __ARGS((void));
void	dojumps __ARGS((void));

/* message.c */
void	msg __ARGS((char *));
void	smsg(); /*  __ARGS((char *, ...)); does not work */
void	emsg __ARGS((char *));
void	msgmore __ARGS((long));
void	wait_return __ARGS((bool_t));
int		ask_yesno __ARGS((char *));

/* misccmds.c */
int		get_indent __ARGS((void));
void	set_indent __ARGS((int, int));
bool_t	Opencmd __ARGS((int));
int		plines __ARGS((linenr_t));
int		plines_m __ARGS((linenr_t, linenr_t));
void	fileinfo __ARGS((void));
void	setfname __ARGS((char *));
void	maketitle __ARGS((void));
void	inschar __ARGS((int));
void	insstr __ARGS((char *));
bool_t	delchar __ARGS((bool_t));
void	delline __ARGS((long, bool_t));
int		gchar __ARGS((FPOS *));
int		gcharCurpos __ARGS((void));
int		inindent __ARGS((void));
void	skipspace __ARGS((char **));
long	getdigits __ARGS((char **));
char	*plural __ARGS((long));

/* normal.c */
void	normal __ARGS((void));

/* ops.c */
void	shift_line __ARGS((int));
void	init_yank __ARGS((void));

/* param.c */
void	set_init __ARGS((void));
void	doset __ARGS((char *));
int		makeset __ARGS((FILE *));

/* quickfix.c */
int		qf_init __ARGS((char *));
void	qf_jump __ARGS((int));
void	qf_list __ARGS((void));
void	qf_clrallmarks __ARGS((void));
void	qf_adjustmark __ARGS((char *, char *));

/* screen.c */
void	updateline __ARGS((void));
void	updateScreen __ARGS((int));
void	prt_line __ARGS((char *));
void	screenalloc __ARGS((void));
void	screenclear __ARGS((void));
void	cursupdate __ARGS((void));
void	scrolldown __ARGS((long));
void	scrollup __ARGS((long));
void	s_ins __ARGS((int, int, bool_t));
void	s_del __ARGS((int, int, bool_t));
void	showmode __ARGS((void));

/* script.c */
void	startscript __ARGS((void));
void	stopscript __ARGS((void));
int		openscript __ARGS((char *));
void	updatescript __ARGS((int));
void	openrecover __ARGS((void));
char	*modname __ARGS((char *, char *));
void	script_winsize __ARGS((void));
void	script_winsize_pp __ARGS((void));

/* search.c */
int		searchit __ARGS((FPOS *, int, char *, long, bool_t));
bool_t	dosearch __ARGS((int, char *, bool_t, long));
bool_t	searchc __ARGS((int, int, int, long));
FPOS	*showmatch __ARGS((void));
bool_t 	findfunc __ARGS((int, bool_t, long));
int		findsent __ARGS((int, long));
bool_t	findpar __ARGS((int, long, int));
int		startPS __ARGS((linenr_t, int));
int		fwd_word __ARGS((long, int));
int		bck_word __ARGS((long, int));
int		end_word __ARGS((long, int, bool_t));
int		skip_chars __ARGS((int, int));

/* storage.c */
char	*m_blockalloc __ARGS((u_long, bool_t));
void	m_blockfree __ARGS((void));
void	free_line __ARGS((char *));
char	*alloc_line __ARGS((unsigned));
char	*save_line __ARGS((char *));
void	filealloc __ARGS((void));
void	freeall __ARGS((void));
char	*nr2ptr __ARGS((linenr_t));
void	setmarked __ARGS((linenr_t));
linenr_t firstmarked __ARGS((void));
void	clearmarked __ARGS((void));
linenr_t ptr2nr __ARGS((char *, linenr_t));
bool_t	appendline __ARGS((linenr_t, char *));
char	*delsline __ARGS((linenr_t));
char	*replaceline __ARGS((linenr_t, char *));
bool_t	canincrease __ARGS((int));

/* tag.c */
void	dotag __ARGS((char *, int, int));
void	clrtags __ARGS((void));
void	incrtags __ARGS((void));
void	decrtags __ARGS((void));
void	dotags __ARGS((void));

/* undo.c */
bool_t	u_saveCurpos __ARGS((void));
bool_t	u_save __ARGS((linenr_t, linenr_t));
bool_t	u_savesub __ARGS((linenr_t, char *));
bool_t	u_savedel __ARGS((linenr_t, char *));
void	u_undo __ARGS((int));
void	u_redo __ARGS((int));
void	u_sync __ARGS((void));
void	u_clearall __ARGS((void));
void	u_saveline __ARGS((linenr_t));
void	u_clearline __ARGS((void));
void	u_undoline __ARGS((void));

/*
 * Machine-dependent routines.
 */
#ifdef AMIGA
# include "amiga.h"
#endif
#ifdef BSD
# include "bsd.h"
#endif
#ifdef UNIX
# include "unix.h"
#endif
#ifdef TOS
# include "tos.h"
#endif
#ifdef OS2
# include "os2.h"
#endif
#ifdef DOS
# include "dos.h"
#endif
