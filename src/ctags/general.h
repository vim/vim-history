/*****************************************************************************
*   $Id$
*
*   Copyright (c) 1998-2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   Provides the general (non-ctags-specific) environment assumed by all.
*****************************************************************************/
#ifndef _GENERAL_H
#define _GENERAL_H

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/*============================================================================
=   Macros
============================================================================*/

#if defined(__STDC__) || defined(MSDOS) || defined(WIN32) || defined(OS2)
# define ENABLE_PROTOTYPES 1
#endif

/*  Determine whether to use prototypes or simple declarations.
 */
#ifndef __ARGS
# ifdef ENABLE_PROTOTYPES
#  define __ARGS(x) x
# else
#  define __ARGS(x) ()
# endif
#endif

/*  This is a helpful internal feature of later versions (> 2.7) of GCC
 *  to prevent warnings about unused variables.
 */
#if __GNUC__ > 2  ||  (__GNUC__ == 2  &&  __GNUC_MINOR__ >= 7)
# define __unused__	__attribute__((unused))
#else
# define __unused__
#endif

/*  MS-DOS doesn't allow manipulation of standard error, so we send it to
 *  stdout instead.
 */
#if defined(MSDOS) || defined(WIN32)
# define errout	stdout
#else
# define errout	stderr
#endif

#if defined(__STDC__) || defined(MSDOS) || defined(WIN32) || defined(OS2)
# define ENABLE_STDARG 1
#endif

#if defined(MSDOS) || defined(WIN32)
# define HAVE_DOS_H 1
# define HAVE_FCNTL_H 1
# define HAVE_IO_H 1
# define HAVE_STDLIB_H 1
# define HAVE_SYS_STAT_H 1
# define HAVE_SYS_TYPES_H 1
# define HAVE_TIME_H 1
# define HAVE_CLOCK 1
# define HAVE_CHSIZE 1
# define HAVE_FGETPOS 1
# define HAVE_STRERROR 1
# define HAVE_FINDNEXT 1
# ifdef __BORLANDC__
#  define HAVE_DIR_H 1
#  define HAVE_DIRENT_H 1
#  define HAVE_FINDFIRST 1
# else
#  ifdef _MSC_VER
#   define HAVE__FINDFIRST 1
#  else
#   ifdef __MINGW32__
#    define HAVE_DIR_H 1
#    define HAVE_DIRENT_H 1
#    define HAVE__FINDFIRST 1
#    define NEED_PROTO_FGETPOS 1
#    define ffblk _finddata_t
#    define FA_DIREC _A_SUBDIR
#    define ff_name name
#   endif
#  endif
# endif
#endif

#ifdef DJGPP
# define HAVE_DIR_H 1
# define HAVE_SYS_STAT_H 1
# define HAVE_SYS_TYPES_H 1
# define HAVE_UNISTD_H 1
# define HAVE_FGETPOS 1
# define HAVE_FINDFIRST 1
# define HAVE_TRUNCATE 1
#endif

#if defined(OS2)
# define HAVE_DIRENT_H 1
# define HAVE_IO_H 1
# define HAVE_TIME_H 1
# define HAVE_STDLIB_H 1
# define HAVE_SYS_STAT_H 1
# define HAVE_SYS_TYPES_H 1
# define HAVE_UNISTD_H 1
# define HAVE_CLOCK 1
# define HAVE_CHSIZE 1
# define HAVE_FGETPOS 1
# define HAVE_OPENDIR 1
# define HAVE_STRERROR 1
# define HAVE_TRUNCATE 1
#endif

#ifdef AMIGA
# define HAVE_STDLIB_H 1
# define HAVE_SYS_STAT_H 1
# define HAVE_SYS_TYPES_H 1
# define HAVE_TIME_H 1
# define HAVE_CLOCK 1
# define HAVE_FGETPOS 1
# define HAVE_STRERROR 1
#endif

#if defined(__MWERKS__) && defined(__MACINTOSH__)
# define HAVE_STAT_H 1
#endif

#ifdef QDOS
# define HAVE_DIRENT_H 1
# define HAVE_STDLIB_H 1
# define HAVE_SYS_STAT_H 1
# define HAVE_SYS_TIMES_H 1
# define HAVE_SYS_TYPES_H 1
# define HAVE_TIME_H 1
# define HAVE_UNISTD_H 1
# define STDC_HEADERS 1
# define HAVE_CLOCK 1
# define HAVE_FGETPOS 1
# define HAVE_FTRUNCATE 1
# define HAVE_OPENDIR 1
# define HAVE_PUTENV 1
# define HAVE_REMOVE 1
# define HAVE_STRERROR 1
# define HAVE_STRSTR 1
# define HAVE_TIMES 1
# define HAVE_TRUNCATE 1
# define NON_CONST_PUTENV_PROTOTYPE 1
#endif

#if defined(__vms) && !defined(VMS)
# define VMS
#endif
#ifdef VMS
# define HAVE_STDLIB_H 1
# define HAVE_TIME_H 1
# ifdef VAXC
#  define HAVE_STAT_H 1
#  define HAVE_TYPES_H 1
# else
#  define HAVE_SYS_STAT_H 1
#  define HAVE_SYS_TYPES_H 1
# endif
# define HAVE_CLOCK 1
# define HAVE_FGETPOS 1
# define HAVE_STRERROR 1
#endif

/*============================================================================
=   Data declarations
============================================================================*/

#undef FALSE
#undef TRUE
#ifdef VAXC
typedef enum { FALSE, TRUE } booleanType;
typedef int boolean;
#else
typedef enum { FALSE, TRUE } boolean;
#endif

#if !defined(HAVE_FGETPOS) && !defined(fpos_t)
# define fpos_t long
#endif

/*----------------------------------------------------------------------------
-	Possibly missing system prototypes.
----------------------------------------------------------------------------*/

#if defined(NEED_PROTO_REMOVE) && defined(HAVE_REMOVE)
extern int remove __ARGS((const char *));
#endif

#if defined(NEED_PROTO_UNLINK) && !defined(HAVE_REMOVE)
extern void *unlink __ARGS((const char *));
#endif

#ifdef NEED_PROTO_GETENV
extern char *getenv __ARGS((const char *));
#endif

#ifdef NEED_PROTO_STRSTR
extern char *strstr __ARGS((const char *str, const char *substr));
#endif

#endif	/* _GENERAL_H */

/* vi:set tabstop=8 shiftwidth=4: */
