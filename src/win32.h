/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Win32 (Windows NT and Windows 95) machine-dependent things.
 */

#define CASE_INSENSITIVE_FILENAME	/* ignore case when comparing file names */
#define SPACE_IN_FILENAME
#define BACKSLASH_IN_FILENAME
#define USE_FNAME_CASE			/* adjust case of file names */
#define BINARY_FILE_IO
#define USE_CRNL				/* lines end in CR-NL instead of NL */
#define USE_VIM_REMOVE
#define USE_VIM_CHDIR
#define USE_MOUSE				/* include mouse support */
#define SYNC_DUP_CLOSE			/* sync() a file with dup() and close() */
#define HAVE_STRING_H
#define HAVE_STRCSPN
#define HAVE_MEMSET
#define USE_TMPNAM				/* use tmpnam() instead of mktemp() */
#define HAVE_LOCALE_H
#define HAVE_FCNTL_H
#define HAVE_QSORT
#if defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_TIME
#endif
#define BREAKCHECK_SKIP		1		/* call mch_breakcheck() each time, it's
									   quite fast */

#define SIZEOF_INT 4

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
#define TMPNAMELEN		15

#include <stdlib.h>

/*
 * Win32 has plenty of memory, use large buffers
 */
#define CMDBUFFSIZE	1024		/* size of the command processing buffer */
#define MAXPATHL	_MAX_PATH	/* Win32 has long paths and plenty of memory */

#ifndef MAXMEM
# define MAXMEM			512			/* use up to  512 Kbyte for buffer */
#endif

#ifndef MAXMEMTOT
# define MAXMEMTOT		2048		/* use up to 2048 Kbyte for Vim */
#endif

#define BASENAMELEN		(MAXPATHL-5)	/* length of base of filename */

/* codes for Win32 mouse event */
#define MOUSE_LEFT		0x00
#define MOUSE_MIDDLE	0x01
#define MOUSE_RIGHT		0x02
#define MOUSE_RELEASE	0x03

#define MOUSE_CLICK_MASK	0x03

#define MOUSE_SHIFT		0x04
#define MOUSE_ALT		0x08
#define MOUSE_CTRL		0x10

/* 0x20 is reserved */

#define MOUSE_DRAG		(0x40 | MOUSE_RELEASE)

#define NUM_MOUSE_CLICKS(code) ((((code) & 0xC0) >> 6) + 1)

#define SET_NUM_MOUSE_CLICKS(code, num) \
	(code) = ((code) & 0x3f) | (((num) - 1) << 6)

/* Work around a bug in Windows 95's rename() */
int win95rename(const char* pszOldFile, const char* pszNewFile);
#define rename(old, new)  win95rename(old, new)
