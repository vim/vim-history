/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * MSDOS Machine-dependent things.
 */

#define CASE_INSENSITIVE_FILENAME	/* ignore case when comparing file names */
#define SPACE_IN_FILENAME
#define BACKSLASH_IN_FILENAME
#define BINARY_FILE_IO
#define USE_CRNL				/* lines end in CR-NL instead of NL */
#define USE_VIM_REMOVE
#define USE_VIM_CHDIR
#define USE_MOUSE				/* include mouse support */
#define SYNC_DUP_CLOSE			/* sync() a file with dup() and close() */
#ifdef DJGPP
# define USE_LONG_FNAME _use_lfn()	/* decide at run time */
# define USE_FNAME_CASE
#else
# define SHORT_FNAME				/* always 8.3 filename */
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
#define BREAKCHECK_SKIP		1		/* call mch_breakcheck() each time, it's
									   quite fast */

#ifdef DJGPP
# define SIZEOF_INT	4				/* 32 bit ints */
#else
# define SIZEOF_INT 2				/* 16 bit ints */
#endif

#include <dos.h>
#include <dir.h>
#include <time.h>

#ifdef DJGPP
# include <unistd.h>
#endif

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"$VIM\\_vimrc"
#endif

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE	"$VIM\\_exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE		"_vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE		"_exrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP		"$VIM\\vim_help.txt"
#endif

#ifdef VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"$VIM\\viminfo"
# endif
#endif /* VIMINFO */

#ifndef DEF_BDIR
# define DEF_BDIR		".,c:\\tmp,c:\\temp"	/* default for 'backupdir' */
#endif

#ifndef DEF_DIR
# define DEF_DIR		".,c:\\tmp,c:\\temp"	/* default for 'directory' */
#endif

#define TMPNAME1		"viXXXXXX"		/* put it in current dir */
#define TMPNAME2		"voXXXXXX"		/*  is there a better place? */
#define TMPNAMELEN		10

#ifndef MAXMEM
# define MAXMEM			256				/* use up to 256Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT		0				/* decide in set_init */
#endif

#ifdef DJGPP
# define BASENAMELEN  (_use_lfn()?250:8)	/* length of base of file name */
#else
# define BASENAMELEN		8				/* length of base of file name */
#endif

/* codes for msdos mouse event */
#define MOUSE_LEFT		0x01
#define MOUSE_RIGHT		0x02
#define MOUSE_MIDDLE	0x04
#define MOUSE_RELEASE	0x07

#define MOUSE_SHIFT		0x08
#define MOUSE_ALT		0x10
#define MOUSE_CTRL		0x20

#define MOUSE_DRAG		(0x40 | MOUSE_RELEASE)

#define MOUSE_CLICK_MASK	0x07

#define NUM_MOUSE_CLICKS(code) \
	((((code) & 0xff) >> 6) + 1)

#define SET_NUM_MOUSE_CLICKS(code, num) \
	(code) = ((code) & 0x3f) + (((num) - 1) << 6)

#ifdef DJGPP
/* Work around a bug in Windows 95's rename() */
#define rename(old, new)  djgpp_rename(old, new)
#endif
