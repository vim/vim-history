/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#if defined(SYSV) || defined(BSD)
# ifndef UNIX
#  define UNIX
# endif
#endif

#include "debug.h"

#include <stdio.h>

#ifndef SYSV
# include <stdlib.h>
#endif

#include <ctype.h>

#include <string.h>

#include "ascii.h"
#include "keymap.h"
#include "term.h"
#include "macros.h"
#ifdef LATTICE
# include <sys/types.h>
# include <sys/stat.h>
#else
# ifdef _DCC
#  include <sys/stat.h>
# else
#  ifdef MSDOS 
#   include <sys\stat.h>
#  else
#   ifdef UNIX
#	  define volatile		/* needed for gcc */
#	  define signed			/* needed for gcc */
#     include <sys/types.h>
#     include <sys/stat.h>
#   else
#     include <stat.h>
#   endif
#  endif
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
# include <functions.h>
# define __ARGS(x)	x
# define __PARMS(x)	x
#endif

#ifdef SASC
# include <clib/exec_protos.h>
# define __ARGS(x)	x
# define __PARMS(x)	x
#endif

#ifdef _DCC
# include <functions.h>
# define __ARGS(x)	x
# define __PARMS(x)	x
#endif

#ifdef __TURBOC__
# define __ARGS(x) x
#endif

#ifdef MSDOS
# include <dos.h>
# include <dir.h>
#endif

#if defined(__STDC__) || defined(__GNUC__)
# ifndef __ARGS
#  define __ARGS(x) x
# endif /* __ARGS */
# if defined(_SEQUENT_)
#  include "ptx_stdlib.h"
# endif
# if defined(sun)
#  include "sun_stdlib.h"
# endif
# if defined(linux)
#  include <unistd.h>  /* may make sense for others too. jw. */
# endif
#else /*__STDC__*/
# if defined(_SEQUENT_) && !defined(_STDLIB_H_)
  extern char *getenv();
  extern void *malloc();
# endif
#endif /* __STDC__ */

#ifndef __ARGS
#define __ARGS(x)	()
#endif
#ifndef __PARMS
#define __PARMS(x)	()
#endif

/*
 * for systems that do not allow free(NULL)
 */
#ifdef NO_FREE_NULL
# define free(x)	{if ((x) != NULL) free(x);}
#endif

/*
 * fnamecmp() is used to compare filenames.
 * On some systems case in a filename does not matter, on others it does.
 */
#if defined(AMIGA) || defined(MSDOS)
# define fnamecmp(x, y) stricmp((x), (y))
#else
# define fnamecmp(x, y) strcmp((x), (y))
#endif

/* flags for updateScreen() */
#define VALID					90	/* buffer not changed */
#define NOT_VALID				91	/* buffer changed */
#define VALID_TO_CURSCHAR		92	/* buffer before cursor not changed */
#define INVERTED				93	/* redisplay inverted part */
#define CLEAR					94	/* first clear screen */
#define CURSUPD					95	/* update cursor first */

/* values for State */
#define NORMAL					 0
#define CMDLINE 				 1
#define INSERT					 2
#define APPEND					 3
#define REPLACE 				 4	/* replace mode */
#define HELP					 5
#define NOMAPPING 				 6	/* no :mapping mode for vgetc() */
#define HITRETURN				 7
#define SETWINSIZE				 8
#define NORMAL_BUSY				 9	/* busy interpreting a command */

/* directions */
#define FORWARD 				 1
#define BACKWARD				 -1

/* for GetChars */
#define T_PEEK					1	/* do not wait at all */
#define T_WAIT					2	/* wait for a short time */
#define T_BLOCK					3	/* wait forever */

#define QUOTELINE				29999	/* Quoting is linewise */

/*
 * Names for the EXRC, HELP and temporary files.
 * This may be redefined by machine dependent header files
 * that are included below.
 */
#define SYSVIMRC_FILE	"s:.vimrc"
#define SYSEXRC_FILE	"s:.exrc"
#define VIMRC_FILE		".vimrc"
#define EXRC_FILE		".exrc"
#define VIM_HLP			"vim:vim.hlp"
#define TMPNAME1		"t:viXXXXXX"
#define TMPNAME2		"t:voXXXXXX"
#define TMPNAMELEN		12

/*
 * Boolean constants
 */
#ifndef TRUE
#define FALSE	(0)			/* note: this is an int, not a long! */
#define TRUE	(1)
#endif

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

/*
 * maximum length of a file name path
 */
#ifdef UNIX
# define MAXPATHL	1024		/* Unix has long paths and plenty of memory */
#else
# define MAXPATHL	128			/* not too long to put name on stack */
#endif

#define CHANGED   set_Changed()
#define UNCHANGED Changed = 0

#if !defined(BSD) && !defined(linux) && !defined(SASC)
typedef unsigned char	u_char;		/* shorthand */
typedef unsigned short	u_short;	/* shorthand */
typedef unsigned int	u_int;		/* shorthand */
typedef unsigned long	u_long;		/* shorthand */
#endif

typedef long			linenr_t;	/* line number type */
typedef unsigned		colnr_t;	/* column number type */
typedef struct fpos		FPOS;		/* file position type */

#define INVLNUM (0x7fffffff)		/* invalid line number */

struct fpos
{
		linenr_t		lnum;	/* line number */
		colnr_t 		col;	/* column number */
};
