/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_mswin.c
 *
 * Routines common to both Win16 and Win32.
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

#ifdef FEAT_PRINTER
# include <dlgs.h>
# ifdef WIN3264
#  include <winspool.h>
# endif
# include <commdlg.h>
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

#ifdef WIN3264
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
# define HFONT int
# define PRINTDLG int
# define TEXTMETRIC int
# define COLORREF int
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


#if defined(FEAT_GUI_MSWIN) || defined(PROTO)
/*
 * GUI version of mch_exit().
 * Shut down and exit with status `r'
 * Careful: mch_exit() may be called before mch_init()!
 */
    void
mch_exit(int r)
{
    display_errors();

    ml_close_all(TRUE);		/* remove all memfiles */

# ifdef FEAT_OLE
    UninitOLE();
# endif
#ifdef DYNAMIC_GETTEXT
    dyn_libintl_end();
#endif

    if (gui.in_use)
	gui_exit(r);
    exit(r);
}

#endif /* FEAT_GUI_MSWIN */


/*
 * Init the tables for toupper() and tolower().
 */
    void
mch_early_init(void)
{
    int		i;

#ifdef WIN3264
    PlatformId();
#endif

    /* Init the tables for toupper() and tolower() */
    for (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
#ifdef WIN3264
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
# ifdef FEAT_GUI_MSWIN
    gui_mch_settitle(title, icon);
# else
    if (title != NULL)
	SetConsoleTitle(title);
# endif
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
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		p += (*mb_ptr2len_check)(p);
	    else
#endif
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

#if defined(FEAT_GUI_MSWIN) || defined(PROTO)
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

#if defined(USE_MCH_ERRMSG) || defined(PROTO)

#ifdef display_errors
# undef display_errors
#endif

/*
 * Display the saved error message(s).
 */
    void
display_errors()
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
#ifdef WIN3264
		MessageBox(0, p, "Vim", MB_TASKMODAL|MB_SETFOREGROUND);
#else
		MessageBox(0, p, "Vim", MB_TASKMODAL);
#endif
		break;
	    }
	ga_clear(&error_ga);
    }
}
#endif


/*
 * Does `s' contain a wildcard?
 */
    int
mch_has_wildcard(char_u *p)
{
    for ( ; *p; ++p)
    {
	if (vim_strchr((char_u *)
#  ifdef VIM_BACKTICK
				    "?*$[`"
#  else
				    "?*$["
#  endif
                                                , *p) != NULL
		|| (*p == '~' && p[1] != NUL))
	    return TRUE;
    }
    return FALSE;
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
    EMSG(_("E359: Screen mode setting not supported"));
    return FAIL;
}


#ifdef FEAT_EVAL
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
#ifdef WIN3264
typedef LPTSTR (*MYSTRPROCSTR)(LPTSTR);
typedef LPTSTR (*MYINTPROCSTR)(int);
typedef int (*MYSTRPROCINT)(LPTSTR);
typedef int (*MYINTPROCINT)(int);
#else
typedef LPSTR (*MYSTRPROCSTR)(LPSTR);
typedef LPSTR (*MYINTPROCSTR)(int);
typedef int (*MYSTRPROCINT)(LPSTR);
typedef int (*MYINTPROCINT)(int);
#endif

    int
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	/* NULL when using a argint */
    int		argint,
    char_u	**string_result,/* NULL when using number_result */
    int		*number_result)
{
    HINSTANCE		hinstLib;
    MYSTRPROCSTR	ProcAdd;
    MYINTPROCSTR	ProcAddI;
    char_u		*retval_str = NULL;
    int			retval_int = 0;

    BOOL fRunTimeLinkSuccess = FALSE;

    // Get a handle to the DLL module.
    hinstLib = LoadLibrary(libname);

    // If the handle is valid, try to get the function address.
    if (hinstLib != NULL)
    {
	if (argstring != NULL)
	{
	    /* Call with string argument */
	    ProcAdd = (MYSTRPROCSTR) GetProcAddress(hinstLib, funcname);
	    if ((fRunTimeLinkSuccess = (ProcAdd != NULL)) != 0)
	    {
		if (string_result == NULL)
		    retval_int = ((MYSTRPROCINT)ProcAdd)(argstring);
		else
		    retval_str = (ProcAdd)(argstring);
	    }
	}
	else
	{
	    /* Call with number argument */
	    ProcAddI = (MYINTPROCSTR) GetProcAddress(hinstLib, funcname);
	    if ((fRunTimeLinkSuccess = (ProcAddI != NULL)) != 0)
	    {
		if (string_result == NULL)
		    retval_int = ((MYINTPROCINT)ProcAddI)(argint);
		else
		    retval_str = (ProcAddI)(argint);
	    }
	}

	// Save the string before we free the library.
	// Assume that a "1" result is an illegal pointer.
	if (string_result == NULL)
	    *number_result = retval_int;
	else if (retval_str != NULL
		&& retval_str != (char_u *)1
		&& retval_str != (char_u *)-1
		&& !IsBadStringPtr(retval_str, INT_MAX))
	    *string_result = vim_strsave(retval_str);

	// Free the DLL module.
	(void)FreeLibrary(hinstLib);
    }

    if (!fRunTimeLinkSuccess)
    {
	EMSG2(_("E364: Library call failed for \"%s()\""), funcname);
	return FAIL;
    }

    return OK;
}
#endif

#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

/*
 * Make vim the owner of the current selection.  Return OK upon success.
 */
    int
clip_mch_own_selection(VimClipboard *cbd)
{
    /*
     * Never actually own the clipboard.  If another application sets the
     * clipboard, we don't want to think that we still own it.
     */
    return FAIL;
}

/*
 * Make vim NOT the owner of the current selection.
 */
    void
clip_mch_lose_selection(VimClipboard *cbd)
{
    /* Nothing needs to be done here */
}

/*
 * Get the current selection and put it in the clipboard register.
 *
 * NOTE: Must use GlobalLock/Unlock here to ensure Win32s compatibility.
 * On NT/W95 the clipboard data is a fixed global memory object and
 * so its handle = its pointer.
 * On Win32s, however, co-operation with the Win16 system means that
 * the clipboard data is moveable and its handle is not a pointer at all,
 * so we can't just cast the return value of GetClipboardData to (char_u*).
 * <VN>
 */
    void
clip_mch_request_selection(VimClipboard *cbd)
{
    int		type = MCHAR;
    HGLOBAL	hMem;
    char_u	*str = NULL;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    if (OpenClipboard(NULL))
    {
	/* Check for vim's own clipboard format first */
	if ((hMem = GetClipboardData(cbd->format)) != NULL)
	{
	    str = (char_u *)GlobalLock(hMem);
	    if (str != NULL)
		switch (*str++)
		{
		    default:
		    case 'L':	type = MLINE;	break;
		    case 'C':	type = MCHAR;	break;
		    case 'B':	type = MBLOCK;	break;
		}
	}
	/* Otherwise, check for the normal text format */
	else if ((hMem = GetClipboardData(CF_TEXT)) != NULL)
	{
	    str = (char_u *)GlobalLock(hMem);
	    if (str != NULL)
		type = (vim_strchr((char*) str, '\r') != NULL) ? MLINE : MCHAR;
	}

	if (hMem != NULL && str != NULL)
	{
	    /* successful lock - must unlock when finished */
	    if (*str != NUL)
	    {
		LPCSTR		psz = (LPCSTR)str;
		char_u		*temp_clipboard;
		char_u		*pszTemp;
		const char	*pszNL;
		int		len;

		temp_clipboard = (char_u *)lalloc((long_u)STRLEN(psz) + 1,
									TRUE);
		if (temp_clipboard != NULL)
		{
		    /* Translate <CR><NL> into <NL>. */
		    pszTemp = temp_clipboard;
		    while (*psz != NUL)
		    {
			pszNL = psz;
			for (;;)
			{
			    pszNL = strchr(pszNL, '\r');
			    if (pszNL == NULL || pszNL[1] == '\n')
				break;
			    ++pszNL;
			}
			len = (int)((pszNL != NULL) ? pszNL - psz : STRLEN(psz));
			STRNCPY(pszTemp, psz, len);
			pszTemp += len;
			if (pszNL != NULL)
			    *pszTemp++ = '\n';
			psz += len + ((pszNL != NULL) ? 2 : 0);
		    }
		    *pszTemp = NUL;
		    clip_yank_selection(type, temp_clipboard,
				(long)(pszTemp - temp_clipboard), cbd);
		    vim_free(temp_clipboard);
		}
	    }
	    /* unlock the global object */
	    (void)GlobalUnlock(hMem);
	}
	CloseClipboard();
    }
}

/*
 * Send the current selection to the clipboard.
 */
    void
clip_mch_set_selection(VimClipboard *cbd)
{
    char_u	*str = NULL;
    long_u	cch;
    int		type;
    HGLOBAL	hMem = NULL;
    HGLOBAL	hMemVim = NULL;
    LPSTR	lpszMem = NULL;
    LPSTR	lpszMemVim = NULL;

    /* If the '*' register isn't already filled in, fill it in now */
    cbd->owned = TRUE;
    clip_get_selection(cbd);
    cbd->owned = FALSE;

    type = clip_convert_selection(&str, &cch, cbd);

    if (type < 0)
	return;

    if ((hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, cch+1)) != NULL
	&& (lpszMem = (LPSTR)GlobalLock(hMem)) != NULL
	&& (hMemVim = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE, cch+2)) != NULL
	&& (lpszMemVim = (LPSTR)GlobalLock(hMemVim)) != NULL)
    {
	switch (type)
	{
	    default:
	    case MLINE:	    *lpszMemVim++ = 'L';    break;
	    case MCHAR:	    *lpszMemVim++ = 'C';    break;
	    case MBLOCK:    *lpszMemVim++ = 'B';    break;
	}

	STRNCPY(lpszMem, str, cch);
	lpszMem[cch] = NUL;

	STRNCPY(lpszMemVim, str, cch);
	lpszMemVim[cch] = NUL;

	/*
	 * Don't pass GetActiveWindow() as an argument to OpenClipboard()
	 * because then we can't paste back into the same window for some
	 * reason - webb.
	 */
	if (OpenClipboard(NULL))
	{
	    if (EmptyClipboard())
	    {
		SetClipboardData(cbd->format, hMemVim);
		SetClipboardData(CF_TEXT, hMem);
	    }
	    CloseClipboard();
	}
    }
    if (lpszMem != NULL)
	GlobalUnlock(hMem);
    if (lpszMemVim != NULL)
	GlobalUnlock(hMemVim);

    vim_free(str);
}

#endif /* FEAT_CLIPBOARD */


/*
 * Debugging helper: expose the MCH_WRITE_DUMP stuff to other modules
 */
    void
DumpPutS(
    const char *psz)
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

#ifdef FEAT_PRINTER
/*=================================================================
 * Win32 printer stuff
 */

static HFONT	    prt_font_handles[2][2][2];
static PRINTDLG	    s_pd;
static const int    boldface[2] = {  FW_REGULAR , FW_BOLD  };
static TEXTMETRIC   s_tm;
static int s_left_margin;
static int s_right_margin;
static int s_top_margin;

/*
 * Convert BGR to RGB for Windows GDI calls
 */
static COLORREF swap_me(COLORREF colorref)
{
    int temp;
    char *ptr = (char *)&colorref;

    temp = *(ptr);
    *(ptr ) = *(ptr + 2);
    *(ptr + 2) = temp;
    return colorref;
}

#ifndef FEAT_GUI
/*
 * Showing the printer dialog is tricky since we have no GUI
 * window to parent it. The following routines are needed to
 * get the window parenting and Z-order to work properly.
 */

HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.

    char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated WindowTitle.
    char pszOldWindowTitle[MY_BUFSIZE]; // Contains original WindowTitle.
    HANDLE hwndFound ;

    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

    wsprintf(pszNewWindowTitle, "%s/%d/%d",
	    pszOldWindowTitle,
	    GetTickCount(),
	    GetCurrentProcessId());

    SetConsoleTitle(pszNewWindowTitle);

    Sleep(40);

    hwndFound = FindWindow(NULL, pszNewWindowTitle);

    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
}


static UINT CALLBACK PrintHookProc(
	HWND hDlg,	// handle to dialog box
	UINT uiMsg,	// message identifier
	WPARAM wParam,	// message parameter
	LPARAM lParam	// message parameter
	)
{
    HWND hwndOwner;
    RECT rc, rcDlg, rcOwner;
    PRINTDLG  *pPD;

    if (uiMsg == WM_INITDIALOG)
    {
	// Get the owner window and dialog box rectangles.
	if ((hwndOwner = GetParent(hDlg)) == NULL)
	    hwndOwner = GetDesktopWindow();

	GetWindowRect(hwndOwner, &rcOwner);
	GetWindowRect(hDlg, &rcDlg);
	CopyRect(&rc, &rcOwner);

	// Offset the owner and dialog box rectangles so that
	// right and bottom values represent the width and
	// height, and then offset the owner again to discard
	// space taken up by the dialog box.

	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

	// The new position is the sum of half the remaining
	// space and the owner's original position.

	SetWindowPos(hDlg,
		HWND_TOP,
		rcOwner.left + (rc.right / 2),
		rcOwner.top + (rc.bottom / 2),
		0, 0,		// ignores size arguments
		SWP_NOSIZE);

	/*  tackle the printdlg copiesctrl problem */
	pPD = (PRINTDLG *)lParam;
	pPD->nCopies = (WORD)pPD->lCustData;
	SetDlgItemInt( hDlg, edt3, pPD->nCopies, FALSE );
	/*  Bring the window to top */
	BringWindowToTop(GetParent(hDlg));
	SetForegroundWindow(hDlg);
    }

    return FALSE;
}
#endif

    void
mch_print_cleanup(void)
{
    int pifItalic;
    int pifBold;
    int pifUnderline;
    for(pifBold = 0 ; pifBold <= 1 ; pifBold++)
	for(pifItalic = 0 ; pifItalic <= 1; pifItalic++)
	    for(pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
		DeleteObject(prt_font_handles[pifBold][pifItalic][pifUnderline]);

    if (s_pd.hDC != NULL)
	DeleteDC (s_pd.hDC);
}

#ifdef FEAT_GUI
extern HWND s_hwnd;
#endif

    static int
to_device_units(int idx, int dpi, int physsize, int offset)
{
    int ret;

    if (printer_opts[idx].string[0] == 'i')
	ret = (printer_opts[idx].number * dpi);
    else if (printer_opts[idx].string[0] == 'm')
	ret = (printer_opts[idx].number * 10 * dpi) / 254;
    else
	ret = (physsize * printer_opts[idx].number) / 100;

    if (ret < offset)
	return 0;
    else
	return ret - offset;
}

    static int
mch_print_get_cpl(int *yChar_out, int *number_width_out)
{
    int cpl;
    int hr;
    int phyw;
    int dvoff;
    int rev_offset;
    int dpi;

#ifdef WIN16
    POINT pagesize;
#endif
    GetTextMetrics (s_pd.hDC, &s_tm) ;
    *yChar_out = s_tm.tmHeight + s_tm.tmExternalLeading ;

    hr	    = GetDeviceCaps(s_pd.hDC, HORZRES);
#ifdef WIN16
    Escape(s_pd.hDC, GETPHYSPAGESIZE, NULL, NULL, &pagesize);
    phyw    = pagesize.x;
    Escape(s_pd.hDC, GETPRINTINGOFFSET, NULL, NULL, &pagesize);
    dvoff   = pagesize.x;
#else
    phyw    = GetDeviceCaps(s_pd.hDC, PHYSICALWIDTH);
    dvoff   = GetDeviceCaps(s_pd.hDC, PHYSICALOFFSETX);
#endif
    dpi	    = GetDeviceCaps(s_pd.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + hr);

    s_left_margin = to_device_units(OPT_PRINT_LEFT, dpi, phyw, dvoff);
    if (printer_opts[OPT_PRINT_NUMBER].present)
    {
	*number_width_out = 8 * s_tm.tmAveCharWidth;
	s_left_margin += *number_width_out;
    }

    s_right_margin = hr - to_device_units(OPT_PRINT_RIGHT, dpi, phyw, rev_offset);

    cpl	= (s_right_margin - s_left_margin) / s_tm.tmAveCharWidth ;

    return cpl;
}

    static int
mch_print_get_lpp(int yCharsize)
{
    int vr;
    int phyw;
    int dvoff;
    int rev_offset;
    int	bottom_margin;
    int	dpi;

#ifdef WIN16
    POINT pagesize;
#endif
    vr	    = GetDeviceCaps(s_pd.hDC, VERTRES);
#ifdef WIN16
    Escape(s_pd.hDC, GETPHYSPAGESIZE, NULL, NULL, &pagesize);
    phyw    = pagesize.y;
    Escape(s_pd.hDC, GETPRINTINGOFFSET, NULL, NULL, &pagesize);
    dvoff   = pagesize.y;
#else
    phyw    = GetDeviceCaps(s_pd.hDC, PHYSICALHEIGHT);
    dvoff   = GetDeviceCaps(s_pd.hDC, PHYSICALOFFSETY);
#endif
    dpi	    = GetDeviceCaps(s_pd.hDC, LOGPIXELSY);

    rev_offset = phyw - (dvoff + vr);

    s_top_margin = to_device_units(OPT_PRINT_TOP, dpi, phyw, dvoff);


    /* adjust top margin if there is a header */
    if (printer_opts[OPT_PRINT_HEADERHEIGHT].present)
	s_top_margin += (yCharsize * printer_opts[OPT_PRINT_HEADERHEIGHT].number);

    bottom_margin = vr - to_device_units(OPT_PRINT_BOT, dpi, phyw, rev_offset);


    return (bottom_margin - s_top_margin) / yCharsize ;
}

    int
mch_print_init(prt_settings_T *psettings, char_u *jobname, int forceit)
{
    static HGLOBAL stored_dm	= NULL;
    static HGLOBAL stored_devn	= NULL;
    static int  stored_nCopies	= 1;
    static int  stored_nFlags	= 0;

    LOGFONT  fLogFont;
    int pifItalic;
    int pifBold;
    int pifUnderline;

    LPVOID mem = NULL;

    memset( &s_pd, 0, sizeof(PRINTDLG) );
    s_pd.lStructSize	= sizeof (PRINTDLG) ;
#ifdef FEAT_GUI
    s_pd.hwndOwner	= s_hwnd;
#else
    s_pd.hwndOwner = GetConsoleHwnd();
#endif
    s_pd.Flags		=  PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC;
    s_pd.hDevMode = stored_dm;
    s_pd.hDevNames = stored_devn;
    s_pd.lCustData = stored_nCopies; /* work around bug in print dialogue */
#ifndef FEAT_GUI
    /*
     * Use hook to prevent console window being sent to back
     */
    s_pd.lpfnPrintHook	= PrintHookProc ;
    s_pd.Flags		|= PD_ENABLEPRINTHOOK;
#endif
    s_pd.Flags |= stored_nFlags;
    /*
     * If bang present, return default printer setup with no dialogue
     * never show dialogue if we are running over telnet
     */
    if (forceit
#ifndef FEAT_GUI
	    || !term_console
#endif
	    )
    {
	s_pd.Flags |= PD_RETURNDEFAULT;
#ifdef WIN3264
	/*
	 * MSDN suggests setting the first parameter to WINSPOOL for
	 * NT, but NULL appears to work just as well.
	 */
	if (STRLEN(p_prtname))
	    s_pd.hDC = CreateDC(NULL, p_prtname, NULL, NULL);
	else
#endif
	{
	    s_pd.Flags |= PD_RETURNDEFAULT;
	    if (PrintDlg(&s_pd) == 0)
		goto init_fail_dlg;
	}
    }
    else if (PrintDlg(&s_pd) == 0)
	goto init_fail_dlg;

    if (s_pd.hDC == NULL)
    {
	EMSG(_("E237: Printer selection failed"));
	mch_print_cleanup();
	return FALSE;
    }

    /*
     * keep the previous driver context
     */
    stored_dm = s_pd.hDevMode;
    stored_devn = s_pd.hDevNames;
    stored_nFlags = s_pd.Flags;
    stored_nCopies = s_pd.nCopies;

#ifdef WIN3264
    /*
     * On some windows systems the nCopies parameter is not
     * passed back correctly. It must be retrieved from the
     * hDevMode struct.
     */
    mem = GlobalLock(s_pd.hDevMode);
    if(mem != NULL && (((DEVMODE *)mem)->dmCopies != 1))
	stored_nCopies = ((DEVMODE *)mem)->dmCopies;
    GlobalUnlock(s_pd.hDevMode);
#endif

    memset(&fLogFont, 0, sizeof(fLogFont));
    /*
     * Initialise the font according to 'printerfont'
     */
#ifdef FEAT_GUI
    if (!get_logfont(&fLogFont, p_prtfont, s_pd.hDC))
	return FALSE;
#else
    /*<VN> need to rearrange win32 code so we can call get_logfont*/
    /* Should use p_prtfont here, but only the font name. */
    STRCPY(fLogFont.lfFaceName, "Courier New");
#endif

    for (pifBold = 0 ; pifBold <= 1 ; pifBold++)
	for (pifItalic = 0 ; pifItalic <= 1; pifItalic++)
	    for (pifUnderline = 0; pifUnderline <= 1; pifUnderline++)
	    {
		fLogFont.lfWeight =  boldface[pifBold];
		fLogFont.lfCharSet = DEFAULT_CHARSET;
		fLogFont.lfItalic = pifItalic;
		fLogFont.lfUnderline = pifUnderline;
		prt_font_handles[pifBold][pifItalic][pifUnderline] =  CreateFontIndirect(&fLogFont);
	    }

    SetBkMode(s_pd.hDC , OPAQUE);
    SelectObject(s_pd.hDC , prt_font_handles[0][0][0]);

    /*
     * Fill in the settings struct
     */
    psettings->chars_per_line = mch_print_get_cpl(&psettings->line_height, &psettings->number_width);
    psettings->lines_per_page = mch_print_get_lpp(psettings->line_height);
    psettings->n_collated_copies =  (s_pd.Flags & PD_COLLATE) ? s_pd.nCopies : 1;
    psettings->n_uncollated_copies =  (s_pd.Flags & PD_COLLATE) ? 1 : s_pd.nCopies;

    if (psettings->n_collated_copies == 0)
	    psettings->n_collated_copies = 1;

    if (psettings->n_uncollated_copies == 0)
	    psettings->n_uncollated_copies = 1;

    psettings->jobname = jobname;

    return TRUE;

init_fail_dlg:
    {
	DWORD err = CommDlgExtendedError();

	if (err)
	{
#ifdef WIN16
	    char buf[8];
	    sprintf(buf, "%ld", err);
	    EMSG2(_("E238: Print error: "), buf);
#else
	    char_u *buf;
	    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			  FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS,
			  NULL, err, 0, (LPTSTR)(&buf), 0, NULL);
	    EMSG2(_("E238: Print error: "), buf);
	    LocalFree((LPVOID)(buf));
#endif
	}
	else
	    msg_clr_eos(); /* Maybe canceled */

	mch_print_cleanup();
	return FALSE;
    }
}


    int
mch_print_begin(prt_settings_T *psettings)
{
    int ret;
    static DOCINFO  di;
    memset(&di, 0, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = psettings->jobname;
    ret = StartDoc (s_pd.hDC, &di);

    return (ret > 0);
}

    void
mch_print_end(void)
{
    EndDoc (s_pd.hDC) ;
}

    int
mch_print_end_page(void)
{
    return (EndPage(s_pd.hDC) > 0);
}

    int
mch_print_begin_page(void)
{
    return (StartPage(s_pd.hDC) > 0);
}

    int
mch_print_text_out(int x, int y, char_u *p, int len, int *must_break)
{
    SIZE	    sz;
    int		    step;
    TextOut (s_pd.hDC, x + s_left_margin, y + s_top_margin, p, len);
#ifdef WIN16
    GetTextExtentPoint(s_pd.hDC, p, len , &sz);
#else
    GetTextExtentPoint32(s_pd.hDC, p, len , &sz);
#endif
    step = (sz.cx - s_tm.tmOverhang);
#ifndef FEAT_PROPORTIONAL_FONTS
    *must_break = ((x + s_left_margin + step + s_tm.tmAveCharWidth
					     + s_tm.tmOverhang)
			> s_right_margin);
#else
    if (*(p+len) == NUL)
	*must_break = FALSE;
    else
    {
#ifdef WIN16
	GetTextExtentPoint(s_pd.hDC, p + len, 1 , &sz);
#else
	GetTextExtentPoint32(s_pd.hDC, p + len, 1 , &sz);
#endif
	*must_break = ((x + s_left_margin + step + sz.cx)
			> s_right_margin);
    }
#endif
    return step;
}

    void
mch_print_setfont(int iBold, int iItalic, int iUnderline)
{
    SelectObject(s_pd.hDC , prt_font_handles[iBold][iItalic][iUnderline]);
}

    void
mch_print_set_bg(unsigned long bgcol)
{
    SetBkColor(s_pd.hDC, swap_me(bgcol));
}
    void
mch_print_set_fg(unsigned long fgcol)
{
    SetTextColor(s_pd.hDC, GetNearestColor(s_pd.hDC, swap_me(fgcol)));
}

#endif /*FEAT_PRINTER*/

#if defined(FEAT_SHORTCUT) || defined(PROTO)
# include <shlobj.h>

/*
 * When "fname" is the name of a shortcut (*.lnk) resolve the file it points
 * to and return that name in allocated memory.
 * Otherwise NULL is returned.
 */
    char_u *
mch_resolve_shortcut(char_u *fname)
{
    HRESULT		hr;
    IShellLink		*psl = NULL;
    IPersistFile	*ppf = NULL;
    OLECHAR		wsz[MAX_PATH];
    WIN32_FIND_DATA	ffd; // we get those free of charge
    TCHAR		buf[MAX_PATH]; // could have simply reused 'wsz'...
    char_u		*rfname = NULL;

    if (!fname || STRLEN(fname) == 0)
	return rfname;

    CoInitialize(NULL);

    // create a link manager object and request its interface
    hr = CoCreateInstance(
	    &CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
	    &IID_IShellLink, (void**)&psl);
    if (hr != S_OK)
	goto shortcut_error;

    // Get a pointer to the IPersistFile interface.
    hr = psl->lpVtbl->QueryInterface(
	    psl, &IID_IPersistFile, (void**)&ppf);
    if (hr != S_OK)
	goto shortcut_error;

    // full path string must be in Unicode.
    MultiByteToWideChar(CP_ACP, 0, fname, -1, wsz, MAX_PATH);

    // "load" the name and resove the link
    hr = ppf->lpVtbl->Load(ppf, wsz, STGM_READ);
    if (hr != S_OK)
	goto shortcut_error;
#if 0  // This makes Vim wait a long time if the target doesn't exist.
    hr = psl->lpVtbl->Resolve(psl, NULL, SLR_NO_UI);
    if (hr != S_OK)
	goto shortcut_error;
#endif

    // Get the path to the link target.
    ZeroMemory(buf, MAX_PATH);
    hr = psl->lpVtbl->GetPath(psl, buf, MAX_PATH, &ffd, 0);
    if (hr == S_OK && buf[0] != NUL)
	rfname = vim_strsave(buf);

shortcut_error:
    // Release all interface pointers (both belong to the same object)
    if (ppf != NULL)
	ppf->lpVtbl->Release(ppf);
    if (psl != NULL)
	psl->lpVtbl->Release(psl);

    CoUninitialize();
    return rfname;
}
#endif

