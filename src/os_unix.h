/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

/* On AIX 4.2 there is a conflicting prototype for ioctl() in stropts.h and
 * unistd.h.  This hack should fix that (suggested by Jeff George) */
#if defined(USE_GUI) && defined(_AIX) && !defined(_NO_PROTO)
# define _NO_PROTO
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_LIBC_H
# include <libc.h>		    /* for NeXT */
#endif

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>	    /* defines BSD, if it's a BSD system */
#endif

/*
 * SVR4 will be defined for linux and FreeBSD if elf.h exists, but those aren't
 * SVR4
 */
#if defined(SVR4) && (defined(__linux__) || defined(BSD))
# undef SVR4
#endif

/*
 * Sun defines FILE on SunOS 4.x.x, Solaris has a typedef for FILE
 */
#if defined(sun) && !defined(FILE)
# define SOLARIS
#endif

/*
 * Using getcwd() is preferred, because it checks for a buffer overflow.
 * Don't use getcwd() on systems do use system("sh -c pwd").  There is an
 * autoconf check for this.
 * Use getcwd() anyway if getwd() isn't present.
 */
#if defined(HAVE_GETCWD) && !(defined(BAD_GETCWD) && defined(HAVE_GETWD))
# define USE_GETCWD
#endif

#ifndef __ARGS
# if defined(__STDC__) || defined(__GNUC__)
#  define __ARGS(x) x
# else
#  define __ARGS(x) ()
# endif
#endif

/* always use unlink() to remove files */
#define vim_remove(x) unlink((char *)(x))

/* The number of arguments to a signal handler is configured here. */
/* It used to be a long list of almost all systems. Any system that doesn't
 * have an argument??? */
/* #if defined(SVR4) || (defined(SYSV) && defined(ISC)) || defined(_AIX) || defined(__linux__) || defined(ultrix) || defined(__386BSD__) || defined(__FreeBSD__) || defined(__bsdi__) || defined(POSIX) || defined(NeXT)  || defined(__alpha) || defined(apollo) */
#if !defined(SOME_SYSTEM)
# define SIGHASARG
#endif

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
# define SIGPROTOARG   (void)
# define SIGDEFARG(s)  ()
# define SIGDUMMYARG
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# ifndef NAMLEN
#  define NAMLEN(dirent) strlen((dirent)->d_name)
# endif
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
# include <time.h>	    /* on some systems time.h should not be
			       included together with sys/time.h */
#endif
#ifdef HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#include <signal.h>

#if defined(DIRSIZ) && !defined(MAXNAMLEN)
# define MAXNAMLEN DIRSIZ
#endif

#if defined(UFS_MAXNAMLEN) && !defined(MAXNAMLEN)
# define MAXNAMLEN UFS_MAXNAMLEN    /* for dynix/ptx */
#endif

#if defined(NAME_MAX) && !defined(MAXNAMLEN)
# define MAXNAMLEN NAME_MAX	    /* for Linux before .99p3 */
#endif

/*
 * Note: if MAXNAMLEN has the wrong value, you will get error messages
 *	 for not being able to open the swap file.
 */
#if !defined(MAXNAMLEN)
# define MAXNAMLEN 512		    /* for all other Unix */
#endif

#define BASENAMELEN	(MAXNAMLEN - 5)

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#ifdef __COHERENT__
# undef __ARGS
#endif

#ifndef W_OK
# define W_OK 2		/* for systems that don't have W_OK in unistd.h */
#endif

/*
 * Unix system-dependent file names
 */
#ifndef SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#endif
#ifndef SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#endif
#ifndef SYS_MENU_FILE
# define SYS_MENU_FILE "$VIM/menu.vim"
#endif

#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE	"$HOME/.exrc"
#endif
#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"$HOME/.vimrc"
#endif
#ifndef USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "$HOME/.gvimrc"
#endif

#ifdef VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"$HOME/.viminfo"
# endif
#endif

#ifndef EXRC_FILE
# define EXRC_FILE	".exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#endif

#ifdef USE_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	".gvimrc"
# endif
#endif

#ifndef VIM_HLP
# define VIM_HLP "$VIM/doc/help.txt"
#endif

#ifndef DEF_BDIR
# ifdef OS2
#  define DEF_BDIR	".,c:/tmp,~/tmp,~/"
# else
#  define DEF_BDIR	".,~/tmp,~/"	/* default for 'backupdir' */
# endif
#endif

#ifndef DEF_DIR
# ifdef OS2
#  define DEF_DIR	".,~/tmp,c:/tmp,/tmp"
# else
#  define DEF_DIR	".,~/tmp,/tmp"	/* default for 'directory' */
# endif
#endif

#define ERRORFILE	"errors.err"
#ifdef OS2
# define MAKEEF		"vim##.err"
#else
# define MAKEEF		"/tmp/vim##.err"
#endif

#ifdef OS2
/*
 * Try several directories to put the temp files.
 */
#define TEMPDIRNAMES	"$TMP", "$TEMP", "c:\\TMP", "c:\\TEMP", ""
#define TEMPNAME	"v?XXXXXX"
#define TEMPNAMELEN	128
#else
#define TEMPNAME	"/tmp/v?XXXXXX"
#define TEMPNAMELEN	15
#endif

/* Special wildcards that need to be handled by the shell */
#define SPECIAL_WILDCHAR    "`"

#if !defined(HAVE_OPENDIR) || !defined(HAVE_QSORT)
# define NO_EXPANDPATH
#endif

/*
 * Unix has plenty of memory, use large buffers
 */
#define CMDBUFFSIZE 1024	/* size of the command processing buffer */
#define MAXPATHL    1024	/* Unix has long paths and plenty of memory */

#define CHECK_INODE		/* used when checking if a swap file already
				    exists for a file */
#ifndef USE_MOUSE
# define USE_MOUSE		/* include mouse support */
#endif

#ifndef MAXMEM
# define MAXMEM		(5*1024)    /* use up to 5 Mbyte for a buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT	(10*1024)    /* use up to 10 Mbyte for Vim */
#endif

/* memmove is not present on all systems, use memmove, bcopy, memcpy or our
 * own version */
/* Some systems have (void *) arguments, some (char *). If we use (char *) it
 * works for all */
#ifdef USEMEMMOVE
# define vim_memmove(to, from, len) memmove((char *)(to), (char *)(from), len)
#else
# ifdef USEBCOPY
#  define vim_memmove(to, from, len) bcopy((char *)(from), (char *)(to), len)
# else
#  ifdef USEMEMCPY
#   define vim_memmove(to, from, len) memcpy((char *)(to), (char *)(from), len)
#  else
#   define VIM_MEMMOVE	    /* found in alloc.c */
#  endif
# endif
#endif
