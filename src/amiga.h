/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Amiga Machine-dependent things
 */

#define CASE_INSENSITIVE_FILENAME	/* ignore case when comparing file names */
#define SPACE_IN_FILENAME
#define USE_FNAME_CASE				/* adjust case of file names */
#ifndef _DCC
#define HAVE_STAT_H
#endif
#define HAVE_STDLIB_H
#define HAVE_STRING_H
#define HAVE_FCNTL_H
#define HAVE_STRCSPN
#define HAVE_MEMSET
#define HAVE_QSORT
#if defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_TIME
#endif

/*
 * Be conservative about sizeof(int). It could be 4 too.
 */
#define SIZEOF_INT	2

#ifdef LATTICE
# define USE_TMPNAM		/* use tmpnam() instead of mktemp() */
#endif

/* always use remove() to remove a file */
#define vim_remove(x) remove((char *)(x))

/*
 * arpbase.h must be included before functions.h
 */
#ifndef NO_ARP
# include <libraries/arpbase.h>
#endif

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#include <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"s:.vimrc"
#endif

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE	"s:.exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE		".vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE		".exrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP		"vim:vim_help.txt"
#endif

#ifdef VIMINFO
#ifndef VIMINFO_FILE
# define VIMINFO_FILE	"s:.viminfo"
#endif
#endif /* VIMINFO */

#ifndef DEF_BDIR
# define DEF_BDIR		".,t:"		/* default for 'backupdir' */
#endif

#ifndef DEF_DIR
# define DEF_DIR		".,t:"		/* default for 'directory' */
#endif

#define TMPNAME1		"t:viXXXXXX"
#define TMPNAME2		"t:voXXXXXX"
#define TMPNAMELEN		12

#ifndef MAXMEM
# define MAXMEM			256		/* use up to 256Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT		0		/* decide in set_init */
#endif

#define BASENAMELEN		26		/* Amiga */
