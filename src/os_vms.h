/* vi:ts=8:sts=4:sw=4
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include <stdio.h>
#include <ctype.h>

#ifdef VAXC
# include <types.h>
# include <stat.h>
#else
# include <sys/types.h>
# include <sys/stat.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_LIBC_H
# include <libc.h>		/* for NeXT */
#endif

#include <socket.h>

#ifdef FEAT_NORMAL
# define VIM_BACKTICK		/* internal backtick expansion */
#endif

#ifndef __ARGS
# if defined(__STDC__) || defined(__GNUC__)
#  define __ARGS(x) x
# else
#  define __ARGS(x) ()
# endif
#endif

/* use delete() to remove files */
#define mch_remove(x) delete((char *)(x))

/* The number of arguments to a signal handler is configured here. */
/* It used to be a long list of almost all systems. Any system that doesn't
 * have an argument??? */
#define SIGHASARG

/* List 3 arg systems here. I guess __sgi, please test and correct me. jw. */
#if defined(__sgi)
# define SIGHAS3ARGS
#endif

#ifdef SIGHASARG
# ifdef SIGHAS3ARGS
#  define SIGPROTOARG	(int, int, struct sigcontext *)
#  define SIGDEFARG(s)	(s, sig2, scont) int s, sig2; struct sigcontext *scont;
#  define SIGDUMMYARG	0, 0, (struct sigcontext *)0
# else
#  define SIGPROTOARG	(int)
#  define SIGDEFARG(s)	(s) int s;
#  define SIGDUMMYARG	0
# endif
#else
# define SIGPROTOARG	(void)
# define SIGDEFARG(s)	()
# define SIGDUMMYARG
#endif

#ifdef HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#if !defined(HAVE_SYS_TIME_H) || defined(TIME_WITH_SYS_TIME)
# include <time.h>	/* on some systems time.h should not be
			   included together with sys/time.h */
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#if defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#endif

#if defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN	/* for dynix/ptx */
#endif

#if defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX		/* for Linux before .99p3 */
#endif

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#if !defined(MAXNAMLEN)
# define MAXNAMLEN 512			/* for all other Unix */
#endif

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#include <unixio.h>
#include <unixlib.h>
#include <signal.h>
#include <file.h>
#include <ssdef.h>
#include <descrip.h>
#include <libclidef.h>
#include <lnmdef.h>
#include <psldef.h>
#include <prvdef.h>
#include <dvidef.h>
#include <dcdef.h>
#include <stsdef.h>
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <jpidef.h>
#include <rms.h>
#include <string.h>
#include <starlet.h>
#include <lib$routines.h>

#ifdef __COHERENT__
# undef __ARGS
#endif /* __COHERENT__ */

#ifndef W_OK
# define W_OK 2		/* for systems that don't have W_OK in unistd.h */
#endif

#ifndef R_OK
# define R_OK 4		/* for systems that don't have R_OK in unistd.h */
#endif


/*
 * System definitions
 */

#define CASE_INSENSITIVE_FILENAME   /* Open VMS is case insensitive */
#define SPACE_IN_FILENAME	    /* There is space between user and passwd */
#define FNAME_ILLEGAL "|*#?%"	    /* Illegal characters in a file name */
#define BINARY_FILE_IO		    /* Use binary fileio */

/* #define USE_TMPNAM */	    /* use tmpnam() instead of mktemp() */

/*
 * system-dependent filenames
 */

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE		"sys$login:.exrc"
#endif

#ifndef USR_EXRC_FILE2
# define USR_EXRC_FILE2		"sys$login:_exrc"
#endif

#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE		"sys$login:.vimrc"
#endif

#ifndef USR_VIMRC_FILE2
# define USR_VIMRC_FILE2	"sys$login:_vimrc"
#endif

#ifdef FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE	"sys$login:.gvimrc"
# endif
# ifndef USR_GVIMRC_FILE2
#  define USR_GVIMRC_FILE2	"sys$login:_gvimrc"
# endif
#endif

#ifndef SYS_VIMRC_FILE
# define SYS_VIMRC_FILE		"$VIM/vimrc"
#endif

#ifdef FEAT_GUI
# ifndef SYS_GVIMRC_FILE
#  define SYS_GVIMRC_FILE	"$VIM/gvimrc"
# endif
#endif

#ifndef SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#endif

#ifndef FILETYPE_FILE
# define FILETYPE_FILE	"filetype.vim"
#endif
#ifndef SETTINGS_FILE
# define SETTINGS_FILE	"settings.vim"
#endif
#ifndef FTOFF_FILE
# define FTOFF_FILE	"ftoff.vim"
#endif
#ifndef SETSOFF_FILE
# define SETSOFF_FILE	"setsoff.vim"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE	".exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#endif

#ifdef FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#endif

#ifndef DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#endif

#ifndef SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#endif

#ifdef FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"sys$login:.viminfo"
# endif
# ifndef VIMINFO_FILE2
#  define VIMINFO_FILE2	"sys$login:_viminfo"
# endif
#endif

/*
* This should contain a comma-separated list of all the user account base
* directories on your system.  For instance, if accounts are found in
* USER:[000000] and USER2:[000000], define USER_HOME as follows:
*/
#ifndef USER_HOME
# define USER_HOME    "/user,/user2"
#endif

#ifndef DFLT_BDIR
# define DFLT_BDIR	"./,sys$login:,tmp:"	/* default for 'backupdir' */
#endif

#ifndef DFLT_DIR
# define DFLT_DIR	"./,sys$login:,tmp:"	/* default for 'directory' */
#endif

#define TEMPNAME	"tmp:v?XXXXXX.txt"
#define TEMPNAMELEN	28

#define CMDBUFFSIZE 1024    /* size of the command processing buffer */
#define MAXPATHL    1024    /* VMS has long paths and plenty of memory */

#define CHECK_INODE	    /* used when checking if a swap file already
			       exists for a file */

#ifndef DFLT_MAXMEM
# define DFLT_MAXMEM	512	    /* use up to 512Kbyte for buffer */
#endif
#ifndef DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	    /* use up to 2048Kbyte for Vim */
#endif

#define BASENAMELEN	(MAXNAMLEN - 5)

/* memmove is not present on all systems, use memmove, bcopy, memcpy or our
 * own version */
/* Some systems have (void *) arguments, some (char *). If we use (char *) it
 * works for all */
#ifdef USEMEMMOVE
# define mch_memmove(to, from, len) memmove((char *)(to), (char *)(from), len)
#else
# ifdef USEBCOPY
#  define mch_memmove(to, from, len) bcopy((char *)(from), (char *)(to), len)
# else
#  ifdef USEMEMCPY
#   define mch_memmove(to, from, len) memcpy((char *)(to), (char *)(from), len)
#  else
#   define VIM_MEMMOVE	    /* found in misc2.c */
#  endif
# endif
#endif

#define mch_rename(src, dst) rename(src, dst)
#define mch_chdir(s) chdir(vms_fixfilename(s))

typedef struct dsc$descriptor   DESC;
#define DFLT_ERRORFILE		"errors.err"
#define DFLT_MAKEEF		"tmp:vim##.err"	 /* user must assign tmp: */
#define DFLT_RUNTIMEPATH	"sys$login:vimfiles,$VIM/vimfiles,$VIMRUNTIME"
