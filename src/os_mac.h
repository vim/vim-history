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
#include <QuickDraw.h>
#include <ToolUtils.h>
#include <LowMem.h>
#include <Scrap.h>
#include <Sound.h>
#include <TextUtils.h>
#include <Memory.h>
#include <OSUtils.h>
#include <Files.h>

#include <unistd.h>
#include <utsname.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stat.h>
#include <unix.h>

/*
 * Use Macintosh subroutine to alloc the memory.
 * (malloc generate Ptr format hard to debug with ZoneRanger)
 */
#define malloc(x) NewPtr(x)
#define free(x)   DisposePtr((char *) x)

/* This will go away when CMD_KEY fully tested */
#define USE_CMD_KEY

#define DONT_ADD_PATHSEP_TO_DIR
#define USE_EXE_NAME		    /* to find  $VIM */
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

#ifndef __POWERPC__
# if !defined(__fourbyteints__) || !__option(enumsalwaysint)
   ERROR: you must compile the projecct with 4-byte ints and enums always int
# endif
#endif

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ifndef SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM:vimrc"
#endif
#ifndef SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM:gvimrc"
#endif
#ifndef SYS_MENU_FILE
# define SYS_MENU_FILE "$VIM:menu.vim"
#endif

#ifdef USE_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "$VIM:.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#endif
#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"$VIM:.vimrc"
#endif

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE	"$VIM:.exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE	"_exrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP	"$VIM:doc:help.txt"
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

/*
 * Macintosh has plenty of memory, use large buffers
 */
#define CMDBUFFSIZE 1024	/* size of the command processing buffer */

#define MAXPATHL    256 	/* Limited by the Pascal Strings */

#define BASENAMELEN	(MAXPATHL-5)	/* length of base of filename */

#ifndef MAXMEM
# define MAXMEM		512	/* use up to  512 Kbyte for buffer */
#endif

#ifndef MAXMEMTOT
# define MAXMEMTOT	2048	/* use up to 2048 Kbyte for Vim */
#endif

#define WILDCHAR_LIST "*?[{`$"
