/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

#if defined(SYSV_UNIX) || defined(BSD_UNIX)
# ifndef UNIX
#  define UNIX
# endif
#endif

#include "debug.h"

#include <stdio.h>

#include <ctype.h>
#ifndef DOMAIN
#include <limits.h>		/* For MAX_INT, remove this if it does not exist */
#endif

#ifdef BSD_UNIX
# include <strings.h>
# ifdef __STDC__
#  include <string.h>
# endif
#else
# include <string.h>
#endif

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
#	 ifndef linux
#	  define volatile		/* needed for gcc */
#	  define signed			/* needed for gcc */
#    endif
#    include <sys/types.h>
#    include <sys/stat.h>
#   else
#     include <stat.h>
#   endif
#  endif
# endif
#endif

#ifndef DOMAIN
# include <stdlib.h>
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

#ifndef AMIGA
/*
 * For the Amiga we use a version of getenv that does local variables under 2.0
 */
#define vimgetenv(x) getenv(x)
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

#ifdef SOLARIS
# include <stdlib.h>
# include <unistd.h>
#endif

#ifdef UNIX
# ifdef SCO
#  undef M_XENIX
#  include <sys/ndir.h>		/* for MAXNAMLEN */
# else
#  if defined(SOLARIS) || defined(AIX)
#   include <dirent.h>		/* for MAXNAMLEN */
#  else
#   include <sys/dir.h>		/* for MAXNAMLEN */
#  endif
# endif
# ifdef USL
#  define MAXNAMLEN DIRSIZ
# endif
# if defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
#  define MAXNAMLEN UFS_MAXNAMLEN		/* for dynix/ptx */
# endif
# if defined(NAME_MAX) && !defined(MAXNAMLEN)
#  define MAXNAMLEN NAME_MAX			/* for Linux before .99p3 */
# endif
# if !defined(MAXNAMLEN)
#  define MAXNAMLEN 512                 /* for all other Unix */
# endif
#endif

#ifdef UNICOS		/* would make sense for other systems too */
# include <errno.h>
#endif

#if defined(__STDC__) || defined(__GNUC__)
# ifndef __ARGS
#  define __ARGS(x) x
# endif /* __ARGS */
# if defined(_SEQUENT_)
#  include "ptx_stdlib.h"
# endif
# if defined(sun) && !defined(SOLARIS)
#  include "sun_stdlib.h"
# endif
# if defined(linux) || defined(SCO) || defined(M_UNIX)
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
# define free(x)	nofreeNULL(x)
  extern void nofreeNULL __ARGS((void *));
#endif

/*
 * fnamecmp() is used to compare filenames.
 * On some systems case in a filename does not matter, on others it does.
 * (this does not account for maximum name lengths, thus it is not 100% accurate!)
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
/*
 * The lowest three bits are used to distinguish normal/cmdline/insert+replace
 * mode. This is used for mapping.
 */
#define NORMAL					0x01
#define NORMAL_BUSY				0x11	/* busy interpreting a command */
#define CMDLINE 				0x02
#define INSERT					0x04
#define REPLACE 				0x24	/* replace mode */
#define HELP					0x30	/* displaying help */
#define NOMAPPING 				0x40	/* no :mapping mode for vgetc() */
#define HITRETURN				0x51	/* waiting for a return */
#define SETWSIZE				0x60	/* window size has changed */
#define ABBREV					0x80	/* abbreviation instead of mapping */

/* directions */
#define FORWARD 				 1
#define BACKWARD				 -1

/* for GetChars */
#define T_PEEK					1	/* do not wait at all */
#define T_WAIT					2	/* wait for a short time */
#define T_BLOCK					3	/* wait forever */

#define VISUALLINE			MAXCOL	/* Visual is linewise */

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#ifndef SYSVIMRC_FILE
# define SYSVIMRC_FILE	"s:.vimrc"
#endif
#ifndef SYSEXRC_FILE
# define SYSEXRC_FILE	"s:.exrc"
#endif
#ifndef VIMRC_FILE
# define VIMRC_FILE		".vimrc"
#endif
#ifndef EXRC_FILE
# define EXRC_FILE		".exrc"
#endif
#ifndef VIM_HLP
# define VIM_HLP		"vim:vim.hlp"
#endif
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
 * Maximum screen width
 */
#define MAX_COLUMNS 255L

/*
 * Buffer sizes
 */
#ifdef UNIX		/* Unix has plenty of memory */
# define CMDBUFFSIZE	1024	/* size of the command processing buffer */
#else
# define CMDBUFFSIZE	256		/* size of the command processing buffer */
#endif

#define LSIZE		512			/* max. size of a line in the tags file */

#define IOSIZE	   (1024+1) 	/* file i/o and sprintf buffer size */

#define	TERMBUFSIZE	1024

#ifdef linux
# define TBUFSZ 2048			/* buffer size for termcap entry */
#else
# define TBUFSZ 1024			/* buffer size for termcap entry */
#endif

/*
 * maximum length of a file name path
 */
#ifdef UNIX
# define MAXPATHL	1024		/* Unix has long paths and plenty of memory */
#else
# define MAXPATHL	128			/* not too long to put name on stack */
#endif

#ifdef MSDOS
# define BASENAMELEN	8		/* length of base of file name */
#else
# ifdef UNIX
#  define BASENAMELEN	(MAXNAMLEN - 5)
# else
#  define BASENAMELEN	26		/* Amiga */
# endif
#endif

#ifdef MSDOS
# define WRITEBIN	"wb"		/* no CR-LF translation */
# define READBIN	"rb"
#else
# define WRITEBIN	"w"
# define READBIN	"r"
#endif

#define CHANGED   set_Changed()
#define UNCHANGED Changed = 0

#if !defined(BSD_UNIX) && !defined(linux) && !defined(SASC) && !defined(__sgi) && !defined(SCO) && !defined(hpux) && !defined(SOLARIS) && !defined(M_UNIX) && !defined(AIX) && !defined(_UTS) && !defined(USL)
typedef unsigned char	u_char;		/* shorthand */
typedef unsigned short	u_short;	/* shorthand */
typedef unsigned int	u_int;		/* shorthand */
typedef unsigned long	u_long;		/* shorthand */
#endif

#if defined(BSD_UNIX) && !defined(__STDC__)
# define strchr(ptr, c)			index((ptr), (c))
# define strrchr(ptr, c)		rindex((ptr), (c))
#endif

#ifdef BSD_UNIX
# define memset(ptr, c, size)	bsdmemset((ptr), (c), (size))
char *bsdmemset __ARGS((char *, int, long));
#endif

typedef long			linenr_t;	/* line number type */
typedef unsigned		colnr_t;	/* column number type */
typedef struct fpos		FPOS;		/* file position type */

#define INVLNUM (0x7fffffff)		/* invalid line number */
#ifdef MAX_INT
# define MAXCOL	MAX_INT				/* maximum column number */
#else
# define MAXCOL	32767				/* maximum column number */
#endif

struct fpos
{
		linenr_t		lnum;	/* line number */
		colnr_t 		col;	/* column number */
};

/*
 * Some versions of isspace() handle Meta character like a space!
 * This define fixes that.
 */
#ifdef VIM_ISSPACE
# ifdef isspace
#  undef isspace
# endif /* isspace */
# define isspace(x)  (((x) >= 9 && (x) <= 13) || ((x) == 32))
#endif /* VIM_ISSPACE */
