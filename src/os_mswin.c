/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_mswin.c
 *
 * Routines common to both Win16 and Win32.
 *
 */

#ifdef WIN16
# ifdef __BORLANDC__
#  pragma warn -par
#  pragma warn -ucp
#  pragma warn -use
#  pragma warn -aus
# endif
#endif

#include <io.h>
#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#ifdef WIN16
# define SHORT_FNAME		/* always 8.3 file name */
# include <dos.h>
# include <string.h>
#endif
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <process.h>

#ifndef STRICT
# define STRICT
#endif
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#undef chdir
#ifdef __GNUC__
# ifndef __MINGW32__
#  include <dirent.h>
# endif
#else
# include <direct.h>
#endif

#if defined(FEAT_TITLE) && !defined(FEAT_GUI_W32)
# include <shellapi.h>
#endif

#ifdef __MINGW32__
# ifndef FROM_LEFT_1ST_BUTTON_PRESSED
#  define FROM_LEFT_1ST_BUTTON_PRESSED    0x0001
# endif
# ifndef RIGHTMOST_BUTTON_PRESSED
#  define RIGHTMOST_BUTTON_PRESSED        0x0002
# endif
# ifndef FROM_LEFT_2ND_BUTTON_PRESSED
#  define FROM_LEFT_2ND_BUTTON_PRESSED    0x0004
# endif
# ifndef FROM_LEFT_3RD_BUTTON_PRESSED
#  define FROM_LEFT_3RD_BUTTON_PRESSED    0x0008
# endif
# ifndef FROM_LEFT_4TH_BUTTON_PRESSED
#  define FROM_LEFT_4TH_BUTTON_PRESSED    0x0010
# endif

/*
 * EventFlags
 */
# ifndef MOUSE_MOVED
#  define MOUSE_MOVED   0x0001
# endif
# ifndef DOUBLE_CLICK
#  define DOUBLE_CLICK  0x0002
# endif
#endif

/* Record all output and all keyboard & mouse input */
/* #define MCH_WRITE_DUMP */

#ifdef MCH_WRITE_DUMP
FILE* fdDump = NULL;
#endif

#ifdef WIN32
extern DWORD g_PlatformId;
#endif
#ifndef FEAT_GUI_MSWIN
extern char g_szOrigTitle[];
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
# define LPTSTR int
# define KEY_EVENT_RECORD int
# define MOUSE_EVENT_RECORD int
# define WINAPI
# define CONSOLE_CURSOR_INFO int
# define LPCSTR char_u *
# define WINBASEAPI
# define INPUT_RECORD int
# define SECURITY_INFORMATION int
# define PSECURITY_DESCRIPTOR int
# define VOID void
# define HWND int
# define PSID int
# define PACL int
# define HICON int
#endif


#if defined(__GNUC__) && !defined(PROTO)
# ifndef __MINGW32__
int _stricoll(char *a, char *b)
{
    // the ANSI-ish correct way is to use strxfrm():
    char a_buff[512], b_buff[512];  // file names, so this is enough on Win32
    strxfrm(a_buff, a, 512);
    strxfrm(b_buff, b, 512);
    return strcoll(a_buff, b_buff);
}

char * _fullpath(char *buf, char *fname, int len)
{
    LPTSTR toss;

    return (char *)GetFullPathName(fname, len, buf, &toss);
}
# endif

int _chdrive(int drive)
{
    char temp [3] = "-:";
    temp[0] = drive + 'A' - 1;
    return !SetCurrentDirectory(temp);
}
#else
#ifdef __BORLANDC__
/* being a more ANSI compliant compiler, BorlandC doesn't define _stricoll:
 * but it does in BC 5.02! */
#if __BORLANDC__ < 0x502
int _stricoll(char *a, char *b)
{
#if 1
    // this is fast but not correct:
    return stricmp(a,b);
#else
    // the ANSI-ish correct way is to use strxfrm():
    char a_buff[512], b_buff[512];  // file names, so this is enough on Win32
    strxfrm(a_buff, a, 512);
    strxfrm(b_buff, b, 512);
    return strcoll(a_buff, b_buff);
#endif
}
#endif
#endif
#endif


garray_t error_ga = {0, 0, 0, 0, NULL};



#ifdef FEAT_GUI_MSWIN
/*
 * GUI version of mch_windexit().
 * Shut down and exit with status `r'
 * Careful: mch_windexit() may be called before mch_shellinit()!
 */
    void
mch_windexit(
    int r)
{
    mch_display_error();

    ml_close_all(TRUE);		/* remove all memfiles */

# ifdef FEAT_OLE
    UninitOLE();
# endif

    if (gui.in_use)
	gui_exit(r);
    exit(r);
}

#endif /* FEAT_GUI_MSWIN */


/*
 * Init the tables for toupper() and tolower().
 */
    void
mch_init(void)
{
    int		i;

#ifdef WIN32
    PlatformId();
#endif

    /* Init the tables for toupper() and tolower() */
    for (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
#ifdef WIN32
    CharUpperBuff(toupper_tab, 256);
    CharLowerBuff(tolower_tab, 256);
#else
    AnsiUpperBuff(toupper_tab, 256);
    AnsiLowerBuff(tolower_tab, 256);
#endif
}


/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    int
mch_input_isatty()
{
#ifdef FEAT_GUI_MSWIN
    return OK;	    /* GUI always has a tty */
#else
    if (isatty(read_cmd_fd))
	return TRUE;
    return FALSE;
#endif
}



#ifdef FEAT_TITLE
/*
 * mch_settitle(): set titlebar of our window
 */
    void
mch_settitle(
    char_u *title,
    char_u *icon)
{
#ifdef FEAT_GUI_MSWIN
    gui_mch_settitle(title, icon);
#else
    if (title != NULL)
	SetConsoleTitle(title);
#endif
}


/*
 * Restore the window/icon title.
 * which is one of:
 *  1: Just restore title
 *  2: Just restore icon (which we don't have)
 *  3: Restore title and icon (which we don't have)
 */
    void
mch_restore_title(
    int which)
{
#ifndef FEAT_GUI_MSWIN
    mch_settitle((which & 1) ? g_szOrigTitle : NULL, NULL);
#endif
}


/*
 * Return TRUE if we can restore the title (we can)
 */
    int
mch_can_restore_title()
{
    return TRUE;
}


/*
 * Return TRUE if we can restore the icon title (we can't)
 */
    int
mch_can_restore_icon()
{
    return FALSE;
}
#endif /* FEAT_TITLE */


/*
 * Get absolute file name into buffer 'buf' of length 'len' bytes,
 * turning all '/'s into '\\'s and getting the correct case of each
 * component of the file name.  Append a backslash to a directory name.
 * When 'shellslash' set do it the other way around.
 * Return OK or FAIL.
 */
    int
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    int		len,
    int		force)
{
    int		nResult = FAIL;

#ifdef __BORLANDC__
    if (*fname == NUL) /* Borland behaves badly here - make it consistent */
	nResult = mch_dirname(buf, len);
    else
#endif
	if (_fullpath(buf, fname, len - 1) == NULL)
    {
	STRNCPY(buf, fname, len);   /* failed, use the relative path name */
	buf[len - 1] = NUL;
#ifndef USE_FNAME_CASE
	slash_adjust(buf);
#endif
    }
    else
	nResult = OK;

#ifdef USE_FNAME_CASE
    fname_case(buf);
#endif

    return nResult;
}


/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    int
mch_isFullName(char_u *fname)
{
    char szName[_MAX_PATH + 1];

    /* A name like "d:/foo" and "//server/share" is absolute */
    if ((fname[0] && fname[1] == ':' && (fname[2] == '/' || fname[2] == '\\'))
	    || (fname[0] == fname[1] && (fname[0] == '/' || fname[0] == '\\')))
	return TRUE;

    /* A name that can't be made absolute probably isn't absolute. */
    if (mch_FullName(fname, szName, _MAX_PATH, FALSE) == FAIL)
	return FALSE;

    return pathcmp(fname, szName) == 0;
}

/*
 * Replace all slashes by backslashes.
 * This used to be the other way around, but MS-DOS sometimes has problems
 * with slashes (e.g. in a command name).  We can't have mixed slashes and
 * backslashes, because comparing file names will not work correctly.  The
 * commands that use a file name should try to avoid the need to type a
 * backslash twice.
 * When 'shellslash' set do it the other way around.
 */
    void
slash_adjust(p)
    char_u  *p;
{
    if (p != NULL)
	while (*p)
	{
	    if (*p == psepcN)
		*p = psepc;
	    ++p;
	}
}


/*
 * stat() can't handle a trailing '/' or '\', remove it first.
 */
    int
vim_stat(const char *name, struct stat *stp)
{
    char	buf[_MAX_PATH + 1];
    char	*p;

    STRNCPY(buf, name, _MAX_PATH);
    buf[_MAX_PATH] = NUL;
    p = buf + strlen(buf);
    if (p > buf)
	--p;
    if (p > buf && (*p == '\\' || *p == '/') && p[-1] != ':')
	*p = NUL;
    return stat(buf, stp);
}

#ifdef FEAT_GUI_MSWIN
    void
mch_settmode(int tmode)
{
    /* nothing to do */
}

    int
mch_get_shellsize(void)
{
    /* never used */
    return OK;
}

    void
mch_set_shellsize(void)
{
    /* never used */
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    /* never used */
}

#endif

/*
 * We have no job control, so fake it by starting a new shell.
 */
    void
mch_suspend()
{
    suspend_shell();
}


#ifdef mch_errmsg
# undef mch_errmsg
# undef mch_display_error
#endif

/*
 * Record an error message for later display.
 */
    void
mch_errmsg(char *str)
{
    int		len = STRLEN(str) + 1;

    if (error_ga.ga_growsize == 0)
    {
	error_ga.ga_growsize = 80;
	error_ga.ga_itemsize = 1;
    }
    if (ga_grow(&error_ga, len) == OK)
    {
	mch_memmove((char_u *)error_ga.ga_data + error_ga.ga_len,
							  (char_u *)str, len);
	--len;			/* don't count the NUL at the end */
	error_ga.ga_len += len;
	error_ga.ga_room -= len;
    }
}

/*
 * Display the saved error message(s).
 */
    void
mch_display_error()
{
    char *p;

    if (error_ga.ga_data != NULL)
    {
	/* avoid putting up a message box with blanks only */
	for (p = (char *)error_ga.ga_data; *p; ++p)
	    if (!isspace(*p))
	    {
		/* Truncate a very long message, it will go off-screen. */
		if (STRLEN(p) > 2000)
		    STRCPY(p + 2000 - 14, "...(truncated)");
#ifdef WIN32
		MessageBox(0, p, "Vim", MB_TASKMODAL|MB_SETFOREGROUND);
#else
		MessageBox(0, p, "Vim", MB_TASKMODAL);
#endif
		break;
	    }
	ga_clear(&error_ga);
    }
}


/*
 * Does `s' contain a wildcard?
 */
    int
mch_has_wildcard(
    char_u *s)
{
#ifdef VIM_BACKTICK
    return (vim_strpbrk(s, (char_u *)"?*$`~") != NULL);
#else
    return (vim_strpbrk(s, (char_u *)"?*$~") != NULL);
#endif
}


/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
    int
mch_chdir(char *path)
{
    if (path[0] == NUL)		/* just checking... */
	return -1;

    if (isalpha(path[0]) && path[1] == ':')	/* has a drive name */
    {
	if (_chdrive(TO_LOWER(path[0]) - 'a' + 1) != 0)
	    return -1;		/* invalid drive name */
	path += 2;
    }

    if (*path == NUL)		/* drive name only */
	return 0;

    return chdir(path);	       /* let the normal chdir() do the rest */
}


/*
 * Switching off termcap mode is only allowed when Columns is 80, otherwise a
 * crash may result.  It's always allowed on NT or when running the GUI.
 */
    int
can_end_termcap_mode(
    int give_msg)
{
#ifdef FEAT_GUI_MSWIN
    return TRUE;	/* GUI starts a new console anyway */
#else
    if (g_PlatformId == VER_PLATFORM_WIN32_NT || Columns == 80)
	return TRUE;
    if (give_msg)
	msg(_("'columns' is not 80, cannot execute external commands"));
    return FALSE;
#endif
}

#ifdef FEAT_GUI_MSWIN
/*
 * return non-zero if a character is available
 */
    int
mch_char_avail()
{
    /* never used */
    return TRUE;
}
#endif


/*
 * set screen mode, always fails.
 */
    int
mch_screenmode(
    char_u *arg)
{
    EMSG(_("Screen mode setting not supported"));
    return FAIL;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

/*
 * Make vim the owner of the current selection.
 */
    void
clip_mch_lose_selection()
{
    /* Nothing needs to be done here */
}

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    int
clip_mch_own_selection()
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    return FAIL;
}


#endif /* FEAT_CLIPBOARD */


/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    void
DumpPutS(
    const char* psz)
{
# ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fputs(psz, fdDump);
	if (psz[strlen(psz) - 1] != '\n')
	    fputc('\n', fdDump);
	fflush(fdDump);
    }
# endif
}

#ifdef _DEBUG

void __cdecl
Trace(
    char *pszFormat,
    ...)
{
    CHAR szBuff[2048];
    va_list args;

    va_start(args, pszFormat);
    vsprintf(szBuff, pszFormat, args);
    va_end(args);

    OutputDebugString(szBuff);
}

#endif //_DEBUG
