/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Win16 (Windows 3.1x) machine-dependent things.
 */

#include "os_dos.h"		/* common MS-DOS and Windows stuff */

#define BINARY_FILE_IO
#define USE_EXE_NAME		/* use argv[0] for $VIM */
#define NO_COOKED_INPUT		/* mch_inchar() doesn't return whole lines */
#define SYNC_DUP_CLOSE		/* sync() a file with dup() and close() */
#define USE_TERM_CONSOLE
#define HAVE_STRING_H
#define HAVE_STRCSPN
#define HAVE_STRICMP
#define HAVE_STRNICMP
#define HAVE_STRFTIME		/* guessed */
#define HAVE_MEMSET
#define USE_TMPNAM		/* use tmpnam() instead of mktemp() */
#define HAVE_LOCALE_H
#define HAVE_FCNTL_H
#define HAVE_QSORT
//#define USE_FNAME_CASE		/* adjust case of file names */
#ifndef FEAT_CLIPBOARD
# define FEAT_CLIPBOARD		/* include clipboard support */
#endif
#if defined(__DATE__) && defined(__TIME__)
# define HAVE_DATE_TIME
#endif
#define HAVE_AVAIL_MEM

#define SHORT_FNAME		/* always 8.3 file name */

#define SMALL_MALLOC		/* 16 bit storage allocation */

#ifdef __BORLANDC__
# define HAVE_PUTENV		/* at least Bcc 5.2 has it */
#endif

#ifdef FEAT_NORMAL
# define VIM_BACKTICK		/* internal backtick expansion */
#endif

#ifdef FEAT_GUI_W16
# define NO_CONSOLE		/* don't included console-only code */
#endif

/* toupper() is not really broken, but it's very slow.	Probably because of
 * using unicde characters on Windows NT */
#define BROKEN_TOUPPER

#define FNAME_ILLEGAL "\"*?><|" /* illegal characters in a file name */

#define SIZEOF_INT 2

typedef long off_t;

#include <stdlib.h>
#include <time.h>

/*
 *  plenty of memory, use large buffers
 */
#define CMDBUFFSIZE 1024	/* size of the command processing buffer */


#define BASENAMELEN	(MAXPATHL-5)	/* length of base of file name */

#ifndef DFLT_MAXMEM
# define DFLT_MAXMEM	(256)    /* use up to 256K for a buffer*/
#endif

#ifndef DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	(5*1024)    /* use up to 5 Mbyte for Vim */
#endif

/*
 * When generating prototypes for Win32 on Unix, these lines make the syntax
 * errors disappear.  They do not need to be correct.
 */
#ifdef PROTO
# define HANDLE int
# define SMALL_RECT int
# define COORD int
# define SHORT int
# define WORD int
# define DWORD int
# define BOOL int
# define LPSTR int
# define KEY_EVENT_RECORD int
# define MOUSE_EVENT_RECORD int
# define WINAPI
# define CONSOLE_CURSOR_INFO int
# define LPCSTR char_u *
#endif


/*
 * Some simple debugging macros that look and behave a lot like their
 * namesakes in MFC.
 */

#ifdef _DEBUG

# if defined(_MSC_VER)	&&  (_MSC_VER >= 1000)
   /* Use the new debugging tools in Visual C++ 4.x */
#  include <crtdbg.h>
#  define ASSERT(f) _ASSERT(f)
# else
#  include <assert.h>
#  define ASSERT(f) assert(f)
# endif

# define VERIFY(f)		ASSERT(f)
# define DEBUG_ONLY(f)		(f)
# define TRACE			Trace
# define TRACE0(sz)		Trace(_T("%s"), _T(sz))
# define TRACE1(sz, p1)		Trace(_T(sz), p1)
# define TRACE2(sz, p1, p2)	Trace(_T(sz), p1, p2)
# define TRACE3(sz, p1, p2, p3) Trace(_T(sz), p1, p2, p3)

/* In debug version, writes trace messages to debug stream */
void __cdecl
Trace(char *pszFormat, ...);

#else /* !_DEBUG */

  /* These macros should all compile away to nothing */
# define ASSERT(f)		((void)0)
# define VERIFY(f)		((void)f)
# define DEBUG_ONLY(f)		((void)0)
# define TRACE			1 ? (void)0 : printf
# define TRACE0(sz)
# define TRACE1(sz, p1)
# define TRACE2(sz, p1, p2)
# define TRACE3(sz, p1, p2, p3)

#endif /* !_DEBUG */


#define ASSERT_POINTER(p, type) \
    ASSERT(((p) != NULL)  &&  IsValidAddress((p), sizeof(type), FALSE))

#define ASSERT_NULL_OR_POINTER(p, type) \
    ASSERT(((p) == NULL)  ||  IsValidAddress((p), sizeof(type), FALSE))

#define mch_setenv(name, val, x) setenv(name, val, x)
