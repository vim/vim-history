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

#ifndef ATARI
# ifndef UNIX
#	include <stdlib.h>
# endif
#endif

#include <ctype.h>

#ifndef MWC
#  include <string.h>
#else
extern char    *index();
extern char    *rindex();
#endif

#include "ascii.h"
#include "keymap.h"
#include "param.h"
#include "term.h"
#include "macros.h"
#include "stat.h"

#ifdef AMIGA
/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#include <signal.h>
#endif

#ifdef MCH_AMIGA
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

#define FORWARD 				 1
#define BACKWARD				 -1

/*
 * Boolean type definition and constants
 */
typedef int 	bool_t;

#ifndef TRUE
#define FALSE	(0)
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
typedef unsigned		linenr_t;	/* line number type */
typedef unsigned		colnr_t;	/* column number type */
typedef struct fpos		FPOS;		/* file position type */

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
u_char	*get_recorded();
u_char	*get_inserted();
int		flush_buffers();
void	ResetBuffers();
void	AppendToRedobuff __ARGS((char *));
void	AppendNumberToRedobuff __ARGS((int));
void	stuffReadbuff __ARGS((char *));
void	stuffnumReadbuff __ARGS((int));
int		read_redo __ARGS((bool_t));
int		copy_redo();
bool_t	start_redo __ARGS((int));
bool_t	start_redo_ins();
int		stop_redo_ins();
u_char	vgetc();
u_char	vpeekc();
int		domap __ARGS((int, char *, int));

/* charset.c */
char	*transchar __ARGS((unsigned));
int		outtrans __ARGS((char *, int));
int		charsize __ARGS((int));
int		chartabsize __ARGS((int, int));
int		isidchar __ARGS((int));

/* cmdline.c */
bool_t	getcmdline __ARGS((int, u_char *));
void	redrawcmdline();
void	redrawcmd();
void	docmdline __ARGS((u_char *));
void	dotag __ARGS((char *));
void	doshell __ARGS((char *, bool_t));
int		doarglist __ARGS((char *));
void	gotocmdline __ARGS((bool_t, int));
int		getfile __ARGS((char *));
void	msg __ARGS((char *));
void	emsg __ARGS((char *));
void	smsg(); /*  __ARGS((char *, ...)); does not work */
void	msgmore __ARGS((int));
void	wait_return __ARGS((int));
int		ask_yesno __ARGS((char *));
int		dosource __ARGS((char *));

/* cmdsearch.c */
void	dosub __ARGS((linenr_t, linenr_t, char *, u_char **));
void	doglob __ARGS((int, linenr_t, linenr_t, char *));

/* edit.c */
void	edit __ARGS((int));
void	getout __ARGS((int));
bool_t	oneright();
bool_t	oneleft();
void	beginline __ARGS((bool_t));
bool_t	oneup __ARGS((int));
bool_t	onedown __ARGS((int));

/* fileio.c */
void	filemess __ARGS((char *, char *));
bool_t	readfile __ARGS((char *, linenr_t));
bool_t	writeit __ARGS((char *, linenr_t, linenr_t, bool_t));

/* help.c */
bool_t	help();
int		redrawhelp();

/* linefunc.c */
void	coladvance __ARGS((colnr_t));
int		inc __ARGS((FPOS *));
int		incCurpos();
int		incl __ARGS((FPOS *));
int		dec __ARGS((FPOS *));
int		decCurpos();
int		decl __ARGS((FPOS *));
void	adjustCurpos();

/* main.c */
void	getout __ARGS((int));

/* mark.c */
bool_t	setmark __ARGS((int));
void	setpcmark();
FPOS	*getmark __ARGS((int));
void	clrallmarks();
void	adjustmark __ARGS((char *, char *));

/* misccmds.c */
int		get_indent();
int		set_indent __ARGS((int, int));
bool_t	Opencmd __ARGS((int));
int		plines __ARGS((linenr_t));
int		plines_m __ARGS((linenr_t, linenr_t));
void	fileinfo();
void	setfname __ARGS((char *));
void	inschar __ARGS((int));
void	insstr __ARGS((char *));
bool_t	delchar __ARGS((bool_t));
void	delline __ARGS((int, bool_t));
int		gchar __ARGS((FPOS *));
int		gcharCurpos();
int		inindent();
void	skipspace __ARGS((char **));
long	getdigits __ARGS((char **));
char	*plural __ARGS((int));

/* normal.c */
void	normal __ARGS((void));

/* ops.c */
int		shift_line __ARGS((int));
int		init_yank();

/* param.c */
int		set_init();
void	doset __ARGS((char *));

/* quickfix.c */
int		qf_init __ARGS((char *));
int		qf_jump __ARGS((int));
int		qf_list();
void	qf_clrallmarks();
void	qf_adjustmark __ARGS((char *, char *));

/* screen.c */
void	updateline();
void	updateScreen __ARGS((int));
void	prt_line __ARGS((char *));
void	screenalloc();
void	screenclear();
void	cursupdate();
void	scrolldown __ARGS((int));
void	scrollup __ARGS((int));
void	s_ins __ARGS((int, int, bool_t));
void	s_del __ARGS((int, int, bool_t));
void	showmode();

/* script.c */
char	*modname __ARGS((char *, char *));

/* search.c */
void	mapstring __ARGS((char *, char *));
int		searchit __ARGS((FPOS *, int, char *, int, bool_t));
bool_t	dosearch __ARGS((int, char *, bool_t, int));
bool_t	searchc __ARGS((int, int, int, int));
FPOS	*showmatch();
bool_t 	findfunc __ARGS((int, bool_t, int));
int		findsent __ARGS((int, int));
bool_t	findpar __ARGS((int, int, int));
int		startPS __ARGS((linenr_t, int));
int		fwd_word __ARGS((int, int));
int		bck_word __ARGS((int, int));
int		end_word __ARGS((int, int, bool_t));
int		skip_chars __ARGS((int, int));

/* storage.c */
char	*m_blockalloc __ARGS((u_long, bool_t));
void	m_blockfree();
void	free_line __ARGS((char *));
char	*alloc_line __ARGS((unsigned));
char	*save_line __ARGS((char *));
void	filealloc();
void	freeall();
char	*nr2ptr __ARGS((linenr_t));
int		setmarked __ARGS((linenr_t));
linenr_t firstmarked();
int		clearmarked();
linenr_t ptr2nr __ARGS((char *, linenr_t));
bool_t	appendline __ARGS((linenr_t, char *));
char	*delsline __ARGS((linenr_t));
char	*replaceline __ARGS((linenr_t, char *));
bool_t	canincrease __ARGS((int));

/* undo.c */
bool_t	u_saveCurpos();
bool_t	u_save __ARGS((linenr_t, linenr_t));
bool_t	u_savesub __ARGS((linenr_t, char *));
bool_t	u_savedel __ARGS((linenr_t, char *));
void	u_undo();
void	u_redo();
void	u_sync();
void	u_clearall();
void	u_saveline __ARGS((linenr_t));
void	u_clearline();
void	u_undoline();

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
