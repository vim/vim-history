/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * Unix system-dependent filenames
 */

#ifndef SYSEXRC_FILE
# define SYSEXRC_FILE	"$HOME/.exrc"
#endif

#ifndef SYSVIMRC_FILE
# define SYSVIMRC_FILE	"$HOME/.vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE		".exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE		".vimrc"
#endif

#ifndef DEFVIMRC_FILE
# define DEFVIMRC_FILE	"/usr/local/etc/vimrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP		"/usr/local/lib/vim.hlp"
#endif

#ifndef BACKUPDIR
# define BACKUPDIR		"$HOME"
#endif

#ifndef DEF_DIR
# define DEF_DIR		"/tmp"
#endif

#define TMPNAME1		"/tmp/viXXXXXX"
#define TMPNAME2		"/tmp/voXXXXXX"
#define TMPNAMELEN		15

#ifndef MAXMEM
# define MAXMEM			512			/* use up to 512Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT		2048		/* use up to 2048Kbyte for Vim */
#endif

#define BASENAMELEN		(MAXNAMLEN - 5)

#define stricmp vim_stricmp

/*
 * prototypes for functions not in unix.c
 */
#ifdef SCO
int		chmod __ARGS((const char *, mode_t));
#endif
#if !defined(linux) && !defined(__NeXT) && !defined(M_UNIX) && !defined(ISC) && !defined(USL) && !defined(SOLARIS)
int		remove __ARGS((const char *));
/*
 * If you get an error message on "const" in the lines above, try
 * adding "-Dconst=" to the options in the makefile.
 */

# if 0		/* should be in unistd.h */
void	sleep __ARGS((int));
# endif

int		rename __ARGS((const char *, const char *));
#endif

int		stricmp __ARGS((char *, char *));

/* memmove is not present on all systems, use our own version or bcopy */
#if !defined(SCO) && !defined(SOLARIS) && !defined(AIX) && !defined(UTS4) && !defined(USL) && !defined(MIPS) && !defined(__NetBSD__) && !defined(__FreeBSD__) && !defined(linux) && !defined(UNISYS)
# ifdef SYSV_UNIX
#   define MEMMOVE
void *memmove __ARGS((void *, void *, int));
# else
#  define memmove(to, from, len) bcopy(from, to, len)
#  if !(defined(hpux) && defined(__STDC__))
#   ifdef linux
extern void bcopy __ARGS((const void *, void *, int));
#   else
extern void bcopy __ARGS((char *, char *, int));
#   endif
#  endif
# endif
#endif

#if defined(BSD_UNIX) && !defined(__STDC__)
# define strchr(ptr, c)			index((ptr), (c))
# define strrchr(ptr, c)		rindex((ptr), (c))
#endif

#ifdef BSD_UNIX
# define memset(ptr, c, size)	bsdmemset((ptr), (c), (size))
char *bsdmemset __ARGS((char *, int, long));
#endif

/*
 * Most unixes don't have these in include files.
 * If you get a "redefined" error, delete the offending line.
 */
#if !defined(__NetBSD__) && !defined(__FreeBSD__)
  extern int	ioctl __ARGS((int, int, ...));
#endif
extern int	fsync __ARGS((int));
extern char *getwd __ARGS((char *));
#if !defined(__NetBSD__) && !defined(__FreeBSD__)
# ifdef linux
   extern void bzero __ARGS((void *, int));
# else
   extern void bzero __ARGS((char *, int));
# endif
#endif
#if defined(system_that_does_not_have_access_in_an_include_file)
extern int access __ARGS((char *, int));
#endif
