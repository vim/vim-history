/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Macintosh machine-dependent things.
 */

#define CASE_INSENSITIVE_FILENAME   /* ignore case when comparing file names */
#define SPACE_IN_FILENAME
#define COLON_AS_PATHSEP
/* #define USE_FNAME_CASE	    /* adjust case of file names */
#define BINARY_FILE_IO
#define EOL_DEFAULT EOL_MAC
#define USE_CR
#define NO_CONSOLE		    /* don't include console mode */

#define USE_VIM_REMOVE
#define USE_VIM_CHDIR
/* #define SYNC_DUP_CLOSE	    /* sync() a file with dup() and close() */
#define HAVE_STRING_H
#define HAVE_STRCSPN
#define HAVE_MEMSET
#define USE_TMPNAM		    /* use tmpnam() instead of mktemp() */
/* #define HAVE_LOCALE_H */
#define HAVE_FCNTL_H
#define HAVE_QSORT
#if defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_TIME
#endif
#define BREAKCHECK_SKIP	    1	    /* call mch_breakcheck() each time, it's
				       quite fast */

#if defined(__POWERPC__) || defined (__fourbyteints__)
# define SIZEOF_INT 4
#else
# define SIZEOF_INT 2
#endif
/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ifdef USE_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE ".gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#endif
#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"$VIM:.vimrc"
#endif

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE	"$VIM\:.exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE	".exrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP	"$VIM:vim_help.txt"
#endif

#ifdef VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"$VIM:viminfo"
# endif
#endif /* VIMINFO */

#ifndef DEF_BDIR
# define DEF_BDIR	".,c:\\tmp,c:\\temp"	/* default for 'backupdir' */
#endif

#ifndef DEF_DIR
# define DEF_DIR	".,c:\\tmp,c:\\temp"	/* default for 'directory' */
#endif

#define ERRORFILE   "errors.err"
#define MAKEEF	    "vim##.err"

#include <stdlib.h>
#include <unistd.h>
#include <stat.h>
#include <unix.h>

/*
 * Win32 has plenty of memory, use large buffers
 */
#define CMDBUFFSIZE 1024	/* size of the command processing buffer */

#define MAXPATHL    1024	/** Mac has long paths and plenty of memory **/

#define BASENAMELEN	(MAXPATHL-5)	/* length of base of filename */

#ifndef MAXMEM
# define MAXMEM		512	/* use up to  512 Kbyte for buffer */
#endif

#ifndef MAXMEMTOT
# define MAXMEMTOT	2048	/* use up to 2048 Kbyte for Vim */
#endif

#define WILDCARD_LIST "*?[{`~$"
