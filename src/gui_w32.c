/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * Windows GUI.
 *
 * GUI support for Microsoft Windows.  Win32 initially; maybe Win16 later
 *
 * George V. Reilly <gvr@halcyon.com> wrote the original Win32 GUI.
 * Robert Webb reworked it to use the existing GUI stuff and added menu,
 * scrollbars, etc.
 *
 * Note: Clipboard stuff, for cutting and pasting text to other windows, is in
 * os_win32.c.	(It can also be done from the terminal version).
 */

#define WIN32_FIND_REPLACE	/* include code for find/replace dialog */
#define MENUHINTS		/* show menu hints in command line */

#include "vim.h"
#include "version.h"	/* used by dialog box routine for default title */
#include <windows.h>
#ifndef __MINGW32__
#include <shellapi.h>
#endif
#ifdef USE_TOOLBAR
# include <commctrl.h>
#endif
#include <windowsx.h>

#ifdef __MINGW32__

#define _cdecl

#define     IsMinimized(hwnd)        IsIconic(hwnd)
#define     IsMaximized(hwnd)        IsZoomed(hwnd)
#define     SelectFont(hdc, hfont)  ((HFONT)SelectObject((hdc), (HGDIOBJ)(HFONT)(hfont)))
#define     GetStockBrush(i)     ((HBRUSH)GetStockObject(i))
#define     DeleteBrush(hbr)     DeleteObject((HGDIOBJ)(HBRUSH)(hbr))

#define HANDLE_WM_RBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_MBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_MBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_LBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), TRUE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_RBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_MOUSEMOVE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_RBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_MBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_LBUTTONUP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_LBUTTONDOWN(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), FALSE, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#define HANDLE_WM_SYSCHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define HANDLE_WM_ACTIVATEAPP(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (BOOL)(wParam), (DWORD)(lParam)), 0L)
#define HANDLE_WM_WINDOWPOSCHANGING(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(BOOL)(fn)((hwnd), (LPWINDOWPOS)(lParam))
#define HANDLE_WM_VSCROLL(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(lParam), (UINT)(LOWORD(wParam)),  (int)(short)HIWORD(wParam)), 0L)
#define HANDLE_WM_SETFOCUS(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define HANDLE_WM_KILLFOCUS(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(wParam)), 0L)
#define HANDLE_WM_HSCROLL(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HWND)(lParam), (UINT)(LOWORD(wParam)), (int)(short)HIWORD(wParam)), 0L)
#define HANDLE_WM_DROPFILES(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HDROP)(wParam)), 0L)
#define HANDLE_WM_CHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define HANDLE_WM_SYSDEADCHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#define HANDLE_WM_DEADCHAR(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam)), 0L)
#endif /* __MINGW32__ */

#ifndef GET_X_LPARAM
# define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif

/* Some parameters for dialog boxes.  All in pixels. */
#define DLG_PADDING_X		10
#define DLG_PADDING_Y		10
#define DLG_OLD_STYLE_PADDING_X	5
#define DLG_OLD_STYLE_PADDING_Y	5
#define DLG_VERT_PADDING_X	4   /* For vertical buttons */
#define DLG_VERT_PADDING_Y	4
#define DLG_ICON_WIDTH		34
#define DLG_ICON_HEIGHT		34
#define DLG_MIN_WIDTH		150
#define DLG_FONT_NAME		"MS Sans Serif"
#define DLG_FONT_POINT_SIZE	8
#define DLG_MIN_MAX_WIDTH	400

/* Some parameters for tearoff menus.  All in pixels. */
#define TEAROFF_PADDING_X	2
#define TEAROFF_BUTTON_PAD_X	8
#define TEAROFF_MIN_WIDTH	200
#define TEAROFF_SUBMENU_LABEL	">>"
#define TEAROFF_COLUMN_PADDING	3	// # spaces to pad column with.

#ifdef PROTO
/*
 * Define a few things for generating prototypes.  This is just to avoid
 * syntax errors, the defines do not need to be correct.
 */
# define HINSTANCE	void *
# define HWND		void *
# define HDC		void *
# define HMENU		void *
# define UINT		int
# define WPARAM		int
# define LPARAM		int
typedef int LOGFONT[];
# define ENUMLOGFONT	int
# define NEWTEXTMETRIC	int
# define VOID		void
# define CALLBACK
# define WORD		int
# define DWORD		int
# define HBITMAP	int
# define HDROP		int
# define BOOL		int
# define PWORD		int
# define LPWORD		int
# define LPRECT		int
# define LRESULT	int
# define WINAPI
# define APIENTRY
# define LPSTR		int
# define LPWINDOWPOS	int
# define RECT		int
# define LPCREATESTRUCT int
# define _cdecl
# define FINDREPLACE	int
# define LPCTSTR	int
# define OSVERSIONINFO	int
# define HBRUSH		int
#endif

/* For the Intellimouse: */
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL	0x20a
#endif

#ifdef MULTI_BYTE
static int sysfixed_width = 0;
static int sysfixed_height = 0;
#ifdef MULTI_BYTE_IME
static LOGFONT norm_logfont;
#endif
#endif

/* Local variables: */
static int		s_button_pending = -1;
static int		s_x_pending;
static int		s_y_pending;
static UINT		s_kFlags_pending;

static HINSTANCE	s_hinst = NULL;
#ifndef USE_SNIFF   /* if_sniff.c uses s_hwnd */
static
#endif
       HWND		s_hwnd = NULL;
static HDC		s_hdc = NULL;
static HBRUSH		s_brush = NULL;
#ifdef USE_TOOLBAR
static HWND		s_toolbarhwnd = NULL;
#endif
#ifdef WIN32_FIND_REPLACE
static HWND		s_findrep_hwnd = NULL;
static UINT		s_findrep_msg = 0;
static FINDREPLACE	s_findrep_struct;
static int		s_findrep_is_find;
#endif
static HWND		s_textArea = NULL;
static HMENU		s_menuBar = NULL;
static UINT		s_menu_id = 100;
static UINT		s_wait_timer = 0;   /* Timer for get char from user */
static int		destroying = FALSE; /* call DestroyWindow() ourselves */

static UINT		s_uMsg = 0;
static WPARAM		s_wParam = 0;
static LPARAM		s_lParam = 0;

static int		s_timed_out = FALSE;

/* Flag that is set while processing a message that must not be interupted by
 * processing another message. */
static int		s_busy_processing = FALSE;

static const LOGFONT	s_lfDefault =
{
    -12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY, FIXED_PITCH | FF_DONTCARE,
    "Fixedsys"	/* see _ReadVimIni */
};

/* Initialise the "current height" to -12 (same as s_lfDefault) just
 * in case the user specifies a font in "guifont" with no size before a font
 * with an explicit size has been set. This defaults the size to this value
 * (-12 equates to roughly 9pt).
 */
static int current_font_height = -12;

static struct
{
    UINT    key_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_keys[] =
{
    {VK_UP,	    'k', 'u'},
    {VK_DOWN,	    'k', 'd'},
    {VK_LEFT,	    'k', 'l'},
    {VK_RIGHT,	    'k', 'r'},

    {VK_F1,	    'k', '1'},
    {VK_F2,	    'k', '2'},
    {VK_F3,	    'k', '3'},
    {VK_F4,	    'k', '4'},
    {VK_F5,	    'k', '5'},
    {VK_F6,	    'k', '6'},
    {VK_F7,	    'k', '7'},
    {VK_F8,	    'k', '8'},
    {VK_F9,	    'k', '9'},
    {VK_F10,	    'k', ';'},

    {VK_F11,	    'F', '1'},
    {VK_F12,	    'F', '2'},
    {VK_F13,	    'F', '3'},
    {VK_F14,	    'F', '4'},
    {VK_F15,	    'F', '5'},
    {VK_F16,	    'F', '6'},
    {VK_F17,	    'F', '7'},
    {VK_F18,	    'F', '8'},
    {VK_F19,	    'F', '9'},
    {VK_F20,	    'F', 'A'},

    {VK_F21,	    'F', 'B'},
    {VK_F22,	    'F', 'C'},
    {VK_F23,	    'F', 'D'},
    {VK_F24,	    'F', 'E'},	    /* winuser.h defines up to F24 */

    {VK_HELP,	    '%', '1'},
    {VK_BACK,	    'k', 'b'},
    {VK_INSERT,	    'k', 'I'},
    {VK_DELETE,	    'k', 'D'},
    {VK_HOME,	    'k', 'h'},
    {VK_END,	    '@', '7'},
    {VK_PRIOR,	    'k', 'P'},
    {VK_NEXT,	    'k', 'N'},
    {VK_PRINT,	    '%', '9'},
    {VK_ADD,	    'K', '6'},
    {VK_SUBTRACT,   'K', '7'},
    {VK_DIVIDE,	    'K', '8'},
    {VK_MULTIPLY,   'K', '9'},
    {VK_SEPARATOR,  'K', 'A'},	    /* Keypad Enter */

    /* Keys that we want to be able to use any modifier with: */
    {VK_SPACE,	    ' ', NUL},
    {VK_TAB,	    TAB, NUL},
    {VK_ESCAPE,	    ESC, NUL},
    {NL,	    NL, NUL},
    {CR,	    CR, NUL},

    /* End of list marker: */
    {0,		    0, 0}
};


#define VIM_NAME	"vim"
#define VIM_CLASS	"Vim"

static int dead_key = 0;	/* 0 - no dead key, 1 - dead key pressed */
static OSVERSIONINFO os_version;    /* like it says.  Init in gui_mch_init() */

/* ron: No need to be stingy on Win32. Make it 16K - s/b big enough for
 * everyone!
 * I have had problems with the original 1000 byte, and with 2 or 5 K.  But
 * 16K should be good for all but the biggest.  Anyway, we free the memory
 * right away.
 */
#define DLG_ALLOC_SIZE 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
static LRESULT APIENTRY dialog_callback(HWND, UINT, WPARAM, LPARAM);
static LRESULT APIENTRY tearoff_callback(HWND, UINT, WPARAM, LPARAM);
static BOOL CenterWindow(HWND hwndChild, HWND hwndParent);
static PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	const char *caption);
static LPWORD lpwAlign(LPWORD);
static int nCopyAnsiToWideChar(LPWORD, LPSTR);
static void gui_mch_tearoff(char_u *title, VimMenu *menu, int initX, int initY);
static void rebuild_tearoff(VimMenu *menu);
static void get_dialog_font_metrics(void);

static int dialog_default_button = -1;

/* Intellimouse support */
static int mouse_scroll_lines = 0;
static UINT msh_msgmousewheel = 0;

static int	s_usenewlook;	    /* emulate W95/NT4 non-bold dialogs */
static WORD	s_dlgfntheight;	    /* height of the dialog font */
static WORD	s_dlgfntwidth;	    /* width of the dialog font	*/
static HBITMAP	s_htearbitmap;	    /* bitmap used to indicate tearoff */
#ifdef USE_TOOLBAR
static void initialise_toolbar(void);
static int get_toolbar_bitmap(char_u *name);
#endif

#ifdef WIN32_FIND_REPLACE
static void initialise_findrep(char_u *initial_string);
static void find_rep_mode_adjust(char_u * buf);
#endif

#ifdef DEBUG
/*
 * Print out the last Windows error message
 */
    static void
print_windows_error(void)
{
    LPVOID  lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  NULL, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lpMsgBuf, 0, NULL);
    TRACE1("Error: %s\n", lpMsgBuf);
    LocalFree(lpMsgBuf);
}
#endif /* DEBUG */

/*
 * Return TRUE when running under Windows NT 3.x or Win32s, both of which have
 * less fancy GUI APIs.
 */
    static int
is_winnt_3(void)
{
    return ((os_version.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& os_version.dwMajorVersion == 3)
	    || (os_version.dwPlatformId == VER_PLATFORM_WIN32s));
}

/*
 * Return TRUE when running under Win32s.
 */
    int
gui_is_win32s(void)
{
    return (os_version.dwPlatformId == VER_PLATFORM_WIN32s);
}

/*
 * Figure out how high the menu bar is at the moment.
 */
    static int
gui_w32_get_menu_height(
    int	    fix_window)	    /* If TRUE, resize window if menu height changed */
{
#ifdef WANT_MENU
    static int	old_menu_height = -1;

    RECT    rc1, rc2;
    int	    num;
    int	    menu_height;

    if (gui.menu_is_active)
	num = GetMenuItemCount(s_menuBar);
    else
	num = 0;

    if (num == 0)
	menu_height = 0;
    else
    {
	if (is_winnt_3())	/* for NT 3.xx */
	{
	    if (gui.starting)
		menu_height = GetSystemMetrics(SM_CYMENU);
	    else
	    {
		RECT r1, r2;
		int frameht = GetSystemMetrics(SM_CYFRAME);
		int capht = GetSystemMetrics(SM_CYCAPTION);

		/* get window rect of s_hwnd
		 * get client rect of s_hwnd
		 * get cap height
		 * subtract from window rect, the sum of client height,
		 * (if not maximized)frame thickness, and caption height.
		 */
		GetWindowRect(s_hwnd, &r1);
		GetClientRect(s_hwnd, &r2);
		menu_height = r1.bottom - r1.top - (r2.bottom - r2.top
				 + 2 * frameht * (!IsZoomed(s_hwnd)) + capht);
	    }
	}
	else			/* win95 and variants (NT 4.0, I guess) */
	{
	    /*
	     * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	     * seem to have been set yet, so menu wraps in default window
	     * width which is very narrow.  Instead just return height of a
	     * single menu item.  Will still be wrong when the menu really
	     * should wrap over more than one line.
	     */
	    GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	    if (gui.starting)
		menu_height = rc1.bottom - rc1.top + 1;
	    else
	    {
		GetMenuItemRect(s_hwnd, s_menuBar, num - 1, &rc2);
		menu_height = rc2.bottom - rc1.top + 1;
	    }
	}
    }

    if (fix_window && menu_height != old_menu_height)
	gui_set_winsize(FALSE);

    old_menu_height = menu_height;
    return menu_height;
#else
    return 0;
#endif
}

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#define BLINK_NONE  0
#define BLINK_OFF   1
#define BLINK_ON    2

static int		blink_state = BLINK_NONE;
static long_u		blink_waittime = 700;
static long_u		blink_ontime = 400;
static long_u		blink_offtime = 250;
static UINT		blink_timer = 0;

    void
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

/* ARGSUSED */
    static VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT uMsg,
    UINT idEvent,
    DWORD dwTime)
{
    MSG msg;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(NULL, idEvent);

    /* Eat spurious WM_TIMER messages */
    while (PeekMessage(&msg, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    if (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(NULL, 0, (UINT)blink_offtime,
						    (TIMERPROC)_OnBlinkTimer);
    }
    else
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(NULL, 0, (UINT)blink_ontime,
						    (TIMERPROC)_OnBlinkTimer);
    }
}

    static void
gui_w32_rm_blink_timer(void)
{
    MSG msg;

    if (blink_timer != 0)
    {
	KillTimer(NULL, blink_timer);
	/* Eat spurious WM_TIMER messages */
	while (PeekMessage(&msg, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	blink_timer = 0;
    }
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    void
gui_mch_stop_blink(void)
{
    gui_w32_rm_blink_timer();
    if (blink_state == BLINK_OFF)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    void
gui_mch_start_blink(void)
{
    gui_w32_rm_blink_timer();

    /* Only switch blinking on if none of the times is zero */
    if (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(NULL, 0, (UINT)blink_waittime,
						    (TIMERPROC)_OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

/*
 * Call-back routines.
 */

    static VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT uMsg,
    UINT idEvent,
    DWORD dwTime)
{
    MSG msg;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(NULL, idEvent);
    s_timed_out = TRUE;

    /* Eat spurious WM_TIMER messages */
    while (PeekMessage(&msg, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    if (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    static void
_OnClose(
    HWND hwnd)
{
    gui_window_closed();
}

/*
 * Get a message when the user switches back to vim
 */
    static void
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    /* When activated: Check if any file was modified outside of Vim. */
    if (fActivate)
	check_timestamps(TRUE);

#ifdef AUTOCMD
    /* In any case, fire the appropriate autocommand */
    apply_autocmds(fActivate ? EVENT_FOCUSGAINED : EVENT_FOCUSLOST,
						   NULL, NULL, FALSE, curbuf);
#endif
    (void)DefWindowProc(hwnd, WM_ACTIVATEAPP, fActivate, dwThreadId);
}

/*
 * Get a message when the window is being destroyed.
 */
    static void
_OnDestroy(
    HWND hwnd)
{
    if (!destroying)
	_OnClose(hwnd);
}

/*
 * Got a message when the system will go down.
 */
    static void
_OnEndSession(void)
{
    ml_close_notmod();		    /* close all not-modified buffers */
    ml_sync_all(FALSE, FALSE);	    /* preserve all swap files */
    ml_close_all(FALSE);	    /* close all memfiles, without deleting */
    getout(1);			    /* exit Vim properly */
}

    static void
_OnDropFiles(
    HWND hwnd,
    HDROP hDrop)
{
    char    szFile[_MAX_PATH];
    UINT    cFiles = DragQueryFile(hDrop, 0xFFFFFFFF, szFile, _MAX_PATH);
    UINT    i;
    char_u  *fname;
    char_u  **fnames;
    char_u  redo_dirs = FALSE;

    /* TRACE("_OnDropFiles: %d files dropped\n", cFiles); */

    /* reset_VIsual(); */
    if (VIsual_active)
    {
	end_visual_mode();
	VIsual_reselect = FALSE;
	update_curbuf(NOT_VALID);	/* delete the inversion */
    }

    fnames = (char_u **)alloc(cFiles * sizeof(char_u *));

    for (i = 0; i < cFiles; ++i)
    {
	DragQueryFile(hDrop, i, szFile, _MAX_PATH);
	/* TRACE("  dropped %2u: '%s'\n", i, szFile); */

	mch_dirname(IObuff, IOSIZE);
	fname = shorten_fname(szFile, IObuff);
	if (fname == NULL)
	    fname = szFile;
	fnames[i] = vim_strsave(fname);
    }
    DragFinish(hDrop);

    /*
     * When the cursor is at the command line, add the file names to the
     * command line, don't edit the files.
     */
    if (State & CMDLINE)
    {
	for (i = 0; i < cFiles; ++i)
	{
	    if (fnames[i] != NULL)
	    {
		if (i > 0)
		    add_to_input_buf(" ", 1);
		add_to_input_buf(fnames[i], STRLEN(fnames[i]));
	    }
	}
    }
    else
    {
	/*
	 * Handle dropping a directory on Vim.
	 * Change to that directory and don't open any file.
	 */
	if (cFiles == 1 && mch_isdir(fnames[0]))
	{
	    if (mch_chdir(fnames[0]) == 0)
	    {
		smsg((char_u *)":cd %s", fnames[0]);
		vim_free(fnames[0]);
		fnames[0] = NULL;
		redo_dirs = TRUE;
	    }
	}

	if (fnames[0] != NULL)
	{
	    /* Shift held down, change to first file's directory */
	    if (GetKeyState(VK_SHIFT) & 0x8000)
		if (vim_chdirfile(fnames[0]) == 0)
		    redo_dirs = TRUE;

	    /* Handle the drop, :edit or :split to get to the file */
	    handle_drop(cFiles, fnames,
				   ((GetKeyState(VK_CONTROL) & 0x8000) != 0));
	}

	if (redo_dirs)
	    shorten_fnames(TRUE);

	/* Update the screen display */
	update_screen(NOT_VALID);
	setcursor();
	out_flush();
    }

    /* SetActiveWindow() doesn't work here... */
    (void)SetForegroundWindow(s_hwnd);
}

    static void
_OnDeadChar(
    HWND hwnd,
    UINT ch,
    int cRepeat)
{
    dead_key = 1;
}

    static void
_OnChar(
    HWND hwnd,
    UINT ch,
    int cRepeat)
{
    char_u	string[1];

    /* TRACE("OnChar(%d, %c)\n", ch, ch); */

    string[0] = ch;
    if (string[0] == Ctrl('C') && !mapped_ctrl_c)
    {
	trash_input_buf();
	got_int = TRUE;
    }
    add_to_input_buf(string, 1);
}

    static void
_OnSysChar(
    HWND hwnd,
    UINT ch,
    int cRepeat)
{
    char_u	string[6]; /* Enough for maximum key sequence - see below */
    int		len;
    int		modifiers;

    /* TRACE("OnSysChar(%d, %c)\n", ch, ch); */

    /* OK, we have a character key (given by ch) which was entered with the
     * ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
     * that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
     * CAPSLOCK is pressed) at this point.
     */
    modifiers = MOD_MASK_ALT;
    if (GetKeyState(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_key(ch, &modifiers);
    /* remove the SHIFT modifier for keys where it's already included, e.g.,
     * '(' and '*' */
    if (ch < 0x100 && (!isalpha(ch)) && isprint(ch))
	modifiers &= ~MOD_MASK_SHIFT;
    /* Interpret the ALT key as making the key META */
    if (modifiers & MOD_MASK_ALT)
    {
	ch |= 0x80;
	modifiers &= ~MOD_MASK_ALT;
    }

    len = 0;
    if (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }
    if (IS_SPECIAL(ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND(ch);
	string[len++] = K_THIRD(ch);
    }
    else
	string[len++] = ch;
    add_to_input_buf(string, len);
}

    static void
_OnMouseEvent(
    int button,
    int x,
    int y,
    int repeated_click,
    UINT keyFlags)
{
    int vim_modifiers = 0x0;

    if (keyFlags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    if (keyFlags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    if (GetKeyState(VK_MENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    static void
_OnMouseButtonDown(
    HWND hwnd,
    BOOL fDoubleClick,
    int x,
    int y,
    UINT keyFlags)
{
    static LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    int	    button = -1;
    int	    repeated_click;

    if (s_uMsg == WM_LBUTTONDOWN || s_uMsg == WM_LBUTTONDBLCLK)
	button = MOUSE_LEFT;
    else if (s_uMsg == WM_MBUTTONDOWN || s_uMsg == WM_MBUTTONDBLCLK)
	button = MOUSE_MIDDLE;
    else if (s_uMsg == WM_RBUTTONDOWN || s_uMsg == WM_RBUTTONDBLCLK)
	button = MOUSE_RIGHT;
    else if (s_uMsg == WM_CAPTURECHANGED)
    {
	/* on W95/NT4, somehow you get in here with an odd Msg
	 * if you press one button while holding down the other..*/
	if (s_button_pending == MOUSE_LEFT)
	    button = MOUSE_RIGHT;
	else
	    button = MOUSE_LEFT;
    }

    if (button >= 0)
    {
	repeated_click = ((int)(currentTime - s_prevTime) < p_mouset);

	/*
	 * Holding down the left and right buttons simulates pushing the middle
	 * button.
	 */
	if (repeated_click &&
		((button == MOUSE_LEFT && s_button_pending == MOUSE_RIGHT) ||
		 (button == MOUSE_RIGHT && s_button_pending == MOUSE_LEFT)))
	{
	    /*
	     * Hmm, gui.c will ignore more than one button down at a time, so
	     * pretend we let go of it first.
	     */
	    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	    button = MOUSE_MIDDLE;
	    repeated_click = FALSE;
	    s_button_pending = -1;
	    _OnMouseEvent(button, x, y, repeated_click, keyFlags);
	}
	else if ((repeated_click)
		|| (mouse_model_popup() && (button == MOUSE_RIGHT)))
	{
	    if (s_button_pending > -1)
	    {
		    _OnMouseEvent(s_button_pending, x, y, FALSE, keyFlags);
		    s_button_pending = -1;
	    }
	    /* TRACE("Button down at x %d, y %d\n", x, y); */
	    _OnMouseEvent(button, x, y, repeated_click, keyFlags);
	}
	else
	{
	    /*
	     * If this is the first press (i.e. not a multiple click) don't
	     * action immediately, but store and wait for:
	     * i) button-up
	     * ii) mouse move
	     * iii) another button press
	     * before using it.
	     * This enables us to make left+right simulate middle button,
	     * without left or right being actioned first.  The side-effect is
	     * that if you click and hold the mouse without dragging, the
	     * cursor doesn't move until you release the button. In practice
	     * this is hardly a problem.
	     */
	    s_button_pending = button;
	    s_x_pending = x;
	    s_y_pending = y;
	    s_kFlags_pending = keyFlags;
	}

	s_prevTime = currentTime;
    }
}

    static void
_OnMouseMoveOrRelease(
    HWND	hwnd,
    int		x,
    int		y,
    UINT	keyFlags)
{
    int button;

    if (s_button_pending > -1)
    {
	/* Delayed action for mouse down event */
	_OnMouseEvent(s_button_pending, s_x_pending,
			s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    if (s_uMsg == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	if (!(keyFlags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
	{
	    gui_mouse_moved(y);
	    return;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	/* TRACE("  move at x %d, y %d\n", x, y); */
    }
    else
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	/* TRACE("  up at x %d, y %d\n", x, y); */
    }

    _OnMouseEvent(button, x, y, FALSE, keyFlags);
}

    static void
_OnPaint(
    HWND hwnd)
{
    if (!IsMinimized(hwnd))
    {
	PAINTSTRUCT ps;
	HDC hdc;


	out_flush();	    /* make sure all output has been processed */
	hdc = BeginPaint(hwnd, &ps);

#ifdef MULTI_BYTE
	/* prevent multi-byte characters from misprinting on an invalid
	 * rectangle */
	if (is_dbcs)
	{
	    RECT rect;

	    GetClientRect(hwnd, &rect);
	    ps.rcPaint.left = rect.left;
	    ps.rcPaint.right = rect.right;
	}
#endif

	if (!IsRectEmpty(&ps.rcPaint))
	    gui_redraw(ps.rcPaint.left, ps.rcPaint.top,
		    ps.rcPaint.right - ps.rcPaint.left + 1,
		    ps.rcPaint.bottom - ps.rcPaint.top + 1);
	EndPaint(hwnd, &ps);
    }
}

    static void
_OnSize(
    HWND hwnd,
    UINT state,
    int cx,
    int cy)
{
    if (!IsMinimized(hwnd))
    {
	gui_resize_window(cx, cy);

	/* Menu bar may wrap differently now */
	gui_w32_get_menu_height(TRUE);
    }
}

    static void
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
}

    static void
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    gui_focus_change(FALSE);
}

#ifdef WANT_MENU
/*
 * Find the VimMenu with the given id
 */
    static VimMenu *
gui_w32_find_menu(
    VimMenu *pMenu,
    int id)
{
    VimMenu *pChildMenu;

    while (pMenu)
    {
	if (pMenu->id == (UINT)id) /* && pMenu->submenu_id == NULL) */
	    break;
	if (pMenu->children != NULL)
	{
	    pChildMenu = gui_w32_find_menu(pMenu->children, id);
	    if (pChildMenu)
	    {
		pMenu = pChildMenu;
		break;
	    }
	}
	pMenu = pMenu->next;
    }
    return pMenu;
}

    static void
_OnMenu(
    HWND hwnd,
    int id,
    HWND hwndCtl,
    UINT codeNotify)
{
    VimMenu *pMenu;

    pMenu = gui_w32_find_menu(root_menu, id);
    if (pMenu)
	gui_menu_cb(pMenu);
}
#endif
/*
 * Find the scrollbar with the given hwnd.
 */
    static GuiScrollbar *
gui_w32_find_scrollbar(HWND hwnd)
{
    WIN		*wp;

    if (gui.bottom_sbar.id == hwnd)
	return &gui.bottom_sbar;
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
    {
	if (wp->w_scrollbars[SBAR_LEFT].id == hwnd)
	    return &wp->w_scrollbars[SBAR_LEFT];
	if (wp->w_scrollbars[SBAR_RIGHT].id == hwnd)
	    return &wp->w_scrollbars[SBAR_RIGHT];
    }
    return NULL;
}

    static int
_OnScroll(
    HWND hwnd,
    HWND hwndCtl,
    UINT code,
    int pos)
{
    GuiScrollbar *sb, *sb_info;
    int		val;
    int		dragging = FALSE;
    SCROLLINFO	si;

    sb = gui_w32_find_scrollbar(hwndCtl);
    if (sb == NULL)
	return 0;

    if (sb->wp != NULL)	    /* Left or right scrollbar */
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    else	    /* Bottom scrollbar */
	sb_info = sb;
    val = sb_info->value;

    switch (code)
    {
	case SB_THUMBTRACK:
	    /* TRACE("SB_THUMBTRACK, %d\n", pos); */
	    val = pos;
	    dragging = TRUE;
	    if (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    break;
	case SB_LINEDOWN:
	    /* TRACE("SB_LINEDOWN\n"); */
	    /* Because of round-off errors we can't move one line when
	     * scroll_shift is non-zero.  Scroll some extra. */
	    if (sb->scroll_shift > 0)
		val += (1 << sb->scroll_shift);
	    else
		val++;
	    break;
	case SB_LINEUP:
	    /* TRACE("SB_LINEUP\n"); */
	    val--;
	    break;
	case SB_PAGEDOWN:
	    /* TRACE("SB_PAGEDOWN\n"); */
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    break;
	case SB_PAGEUP:
	    /* TRACE("SB_PAGEUP\n"); */
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    break;
	case SB_TOP:
	    /* TRACE("SB_TOP\n"); */
	    val = 0;
	    break;
	case SB_BOTTOM:
	    /* TRACE("SB_BOTTOM\n"); */
	    val = sb_info->max;
	    break;
	case SB_ENDSCROLL:
	    /*
	     * "pos" only gives us 16-bit data.  In case of large file, use
	     * GetScrollPos() which returns 32-bit.  Unfortunately it is not
	     * valid while the scrollbar is being dragged.
	     */
	    /* TRACE("SB_ENDSCROLL\n"); */
	    val = GetScrollPos(hwndCtl, SB_CTL);
	    if (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    break;

	default:
	    /* TRACE("Unknown scrollbar event %d\n", code); */
	    return 0;
    }

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    if (sb->scroll_shift > 0)
	si.nPos = val >> sb->scroll_shift;
    else
	si.nPos = val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    if (sb->wp != NULL)
    {
	if (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    SetScrollInfo(sb->wp->w_scrollbars[SBAR_RIGHT].id,
							   SB_CTL, &si, TRUE);
	else
	    SetScrollInfo(sb->wp->w_scrollbars[SBAR_LEFT].id,
							   SB_CTL, &si, TRUE);
    }

    /* Don't let us be interrupted here by another message. */
    s_busy_processing = TRUE;
    gui_drag_scrollbar(sb, val, dragging);
    s_busy_processing = FALSE;

    return 0;
}

/*
 * Setup for the Intellimouse
 */
    static void
init_mouse_wheel(void)
{

#ifndef SPI_GETWHEELSCROLLLINES
# define SPI_GETWHEELSCROLLLINES    104
#endif

#define VMOUSEZ_CLASSNAME  "MouseZ"		/* hidden wheel window class */
#define VMOUSEZ_TITLE      "Magellan MSWHEEL"	/* hidden wheel window title */
#define VMSH_MOUSEWHEEL    "MSWHEEL_ROLLMSG"
#define VMSH_SCROLL_LINES  "MSH_SCROLL_LINES_MSG"

    HWND hdl_mswheel;
    UINT msh_msgscrolllines;

    msh_msgmousewheel = 0;
    mouse_scroll_lines = 3;	/* reasonable default */

    if ((os_version.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& os_version.dwMajorVersion >= 4)
	    || (os_version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
		&& ((os_version.dwMajorVersion == 4
			&& os_version.dwMinorVersion >= 10)
		    || os_version.dwMajorVersion >= 5)))
    {
	/* if NT 4.0+ (or Win98) get scroll lines directly from system */
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0,
		&mouse_scroll_lines, 0);
    }
    else if (os_version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
	    || (os_version.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& os_version.dwMajorVersion < 4))
    {	/*
	 * If Win95 or NT 3.51,
	 * try to find the hidden point32 window.
	 */
	hdl_mswheel = FindWindow(VMOUSEZ_CLASSNAME, VMOUSEZ_TITLE);
	if (hdl_mswheel)
	{
	    msh_msgscrolllines = RegisterWindowMessage(VMSH_SCROLL_LINES);
	    if (msh_msgscrolllines)
	    {
		mouse_scroll_lines = SendMessage(hdl_mswheel,
			msh_msgscrolllines, 0, 0);
		msh_msgmousewheel  = RegisterWindowMessage(VMSH_MOUSEWHEEL);
	    }
	}
    }
}


/* Intellimouse wheel handler */
    static void
_OnMouseWheel(
    HWND hwnd,
    short zDelta)
{
/* Treat a mouse wheel event as if it were a scroll request */
    int i;
    int size;
    HWND hwndCtl;

    if (curwin->w_scrollbars[SBAR_RIGHT].id != 0)
    {
	hwndCtl = curwin->w_scrollbars[SBAR_RIGHT].id;
	size = curwin->w_scrollbars[SBAR_RIGHT].size;
    }
    else if (curwin->w_scrollbars[SBAR_LEFT].id != 0)
    {
	hwndCtl = curwin->w_scrollbars[SBAR_LEFT].id;
	size = curwin->w_scrollbars[SBAR_LEFT].size;
    }
    else
	return;

    size = curwin->w_height;
    if (mouse_scroll_lines == 0)
	init_mouse_wheel();

    if (mouse_scroll_lines > 0
	    && mouse_scroll_lines < (size > 2 ? size - 2 : 1))
    {
	for (i = mouse_scroll_lines; i > 0; --i)
	    _OnScroll(hwnd, hwndCtl, zDelta >= 0 ? SB_LINEUP : SB_LINEDOWN, 0);
    }
    else
	_OnScroll(hwnd, hwndCtl, zDelta >= 0 ? SB_PAGEUP : SB_PAGEDOWN, 0);
}

/*
 * Get current x mouse coordinate in text window.
 * Return -1 when unknown.
 */
    int
gui_mch_get_mouse_x(void)
{
    RECT rct;
    POINT mp;

    if (GetWindowRect(s_textArea, &rct))
    {
	if (GetCursorPos((LPPOINT)&mp))
	    return (int)(mp.x - rct.left);
    }
    return -1;
}

/*
 * Get current y mouse coordinate in text window.
 * Return -1 when unknown.
 */
    int
gui_mch_get_mouse_y(void)
{
    RECT rct;
    POINT mp;

    if (GetWindowRect(s_textArea, &rct))
    {
	if (GetCursorPos((LPPOINT)&mp))
	    return (int)(mp.y - rct.top);
    }
    return -1;
}

/*
 * Move mouse pointer to character at (x, y).
 */
    void
gui_mch_setmouse(int x, int y)
{
    RECT rct;

    if (GetWindowRect(s_textArea, &rct))
	(void)SetCursorPos(x + gui.border_offset + rct.left,
			   y + gui.border_offset + rct.top);
}

    static void
gui_w32_get_valid_dimensions(
    int w,
    int h,
    int *valid_w,
    int *valid_h)
{
    int	    base_width, base_height;

    base_width = gui_get_base_width()
	+ GetSystemMetrics(SM_CXSIZEFRAME) * 2;
    base_height = gui_get_base_height()
	+ GetSystemMetrics(SM_CYSIZEFRAME) * 2
	+ GetSystemMetrics(SM_CYCAPTION)
	+ gui_w32_get_menu_height(FALSE);
    *valid_w = base_width +
		    ((w - base_width) / gui.char_width) * gui.char_width;
    *valid_h = base_height +
		    ((h - base_height) / gui.char_height) * gui.char_height;
}

/*
 * Even though we have _DuringSizing() which makes the rubber band a valid
 * size, we need this for when the user maximises the window.
 * TODO: Doesn't seem to adjust the width though for some reason.
 */
    static BOOL
_OnWindowPosChanging(
    HWND hwnd,
    LPWINDOWPOS lpwpos)
{
    RECT    workarea_rect;

    if (!(lpwpos->flags & SWP_NOSIZE))
    {
	if (IsMaximized(hwnd)
		&& (os_version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS
		    || (os_version.dwPlatformId == VER_PLATFORM_WIN32_NT
			&& os_version.dwMajorVersion >= 4)))
	{
	    SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea_rect, 0);
	    lpwpos->x = workarea_rect.left;
	    lpwpos->y = workarea_rect.top;
	    lpwpos->cx = workarea_rect.right - workarea_rect.left;
	    lpwpos->cy = workarea_rect.bottom - workarea_rect.top;
	}
	gui_w32_get_valid_dimensions(lpwpos->cx, lpwpos->cy,
				     &lpwpos->cx, &lpwpos->cy);
    }
    return 0;
}

    static int
_DuringSizing(
    HWND hwnd,
    UINT fwSide,
    LPRECT lprc)
{
    int	    w, h;
    int	    valid_w, valid_h;
    int	    w_offset, h_offset;

    w = lprc->right - lprc->left;
    h = lprc->bottom - lprc->top;
    gui_w32_get_valid_dimensions(w, h, &valid_w, &valid_h);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    if (fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->left += w_offset;
    else if (fwSide == WMSZ_RIGHT || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->right -= w_offset;

    if (fwSide == WMSZ_TOP || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->top += h_offset;
    else if (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;
    return TRUE;
}

    static BOOL
_OnCreate (HWND hwnd, LPCREATESTRUCT lpcs)
{
#ifdef MULTI_BYTE
    /* get system fixed font size*/
    static const char ach[] = {'W', 'f', 'g', 'M'};

    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, GetStockObject(SYSTEM_FIXED_FONT));
    SIZE    siz;

    GetTextExtentPoint(hdc, ach, sizeof(ach), &siz);

    sysfixed_width = siz.cx / sizeof(ach);
    /*
     * Make characters one pixel higher, so that italic and bold fonts don't
     * draw off the bottom of their character space.  Also means that we can
     * underline an underscore for normal text.
     */
    sysfixed_height = siz.cy + 1;

    /* TRACE("GetFontSize: h %d, w %d\n", gui.char_height, gui.char_width); */

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);

#endif
    return 0;
}

#ifdef WIN32_FIND_REPLACE
    static void
fr_setwhat(char_u *cmd)
{
    if (s_findrep_struct.Flags & FR_WHOLEWORD)
	STRCAT(cmd, "\\<");
    STRCAT(cmd, s_findrep_struct.lpstrFindWhat);
    if (s_findrep_struct.Flags & FR_WHOLEWORD)
	STRCAT(cmd, "\\>");
}

    static void
fr_setreplcmd(char_u *cmd)
{
    STRCAT(cmd, ":%sno/");
    fr_setwhat(cmd);
    STRCAT(cmd, "/");
    STRCAT(cmd, s_findrep_struct.lpstrReplaceWith);
    if (s_findrep_struct.Flags & FR_REPLACE)
	STRCAT(cmd, "/gc");
    else
	STRCAT(cmd, "/g");
    if (s_findrep_struct.Flags & FR_MATCHCASE)
	STRCAT(cmd, "I");
    else
	STRCAT(cmd, "i");
    STRCAT(cmd, "\r");
}

/*
 * Handle a Find/Replace window message.
 */
    static void
_OnFindRepl(void)
{
    char_u cmd[600]; //XXX kludge

    /* Add a char before the command if needed */
    if (State & INSERT)
	cmd[0] = Ctrl('O');
    else if ((State & NORMAL) == 0 && State != CONFIRM)
	cmd[0] = ESC;
    else
	cmd[0] = NUL;
    cmd[1] = NUL;

    if (s_findrep_struct.Flags & FR_DIALOGTERM)
    {
	if (State == CONFIRM)
	{
	    add_to_input_buf("q", 1);
	}
	return;
    }

    if (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	if (State == CONFIRM)
	{
	    STRCAT(cmd, "n");
	}
	else
	{
	    /* Set 'ignorecase' just for this search command. */
	    if (!(s_findrep_struct.Flags & FR_MATCHCASE) == !p_ic)
	    {
		if (p_ic)
		    STRCAT(cmd, ":set noic\r");
		else
		    STRCAT(cmd, ":set ic\r");
		if (State & INSERT)
		    STRCAT(cmd, "\017");	/* CTRL-O */
	    }
	    if (s_findrep_struct.Flags & FR_DOWN)
		STRCAT(cmd, "/");
	    else
		STRCAT(cmd, "?");
	    fr_setwhat(cmd);
	    STRCAT(cmd, "\r");
	    if (!(s_findrep_struct.Flags & FR_MATCHCASE) == !p_ic)
	    {
		if (State & INSERT)
		    STRCAT(cmd, "\017");	/* CTRL-O */
		if (p_ic)
		    STRCAT(cmd, ":set ic\r");
		else
		    STRCAT(cmd, ":set noic\r");
	    }
	}
	/*
	 * Give main window the focus back: this is so
	 * the cursor isn't hollow.
	 */
	(void)SetFocus(s_hwnd);
    }
    else if (s_findrep_struct.Flags & FR_REPLACE)
    {
	if (State == CONFIRM)
	    STRCAT(cmd, "y");
	else
	    fr_setreplcmd(cmd);
	/*
	 * Give main window the focus back: this is to allow
	 * handling of the confirmation y/n/a/q stuff.
	 */
	(void)SetFocus(s_hwnd);
    }
    else if (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	if (State == CONFIRM)
	    STRCAT(cmd, "a");
	else
	    fr_setreplcmd(cmd);
    }
    if (*cmd)
	add_to_input_buf(cmd, STRLEN(cmd));
}
#endif

    static void
HandleMouseHide(UINT uMsg, LPARAM lParam)
{
    static LPARAM last_lParam = 0L;

    /* We sometimes get a mousemove when the mouse didn't move... */
    if (uMsg == WM_MOUSEMOVE)
    {
	if (lParam == last_lParam)
	    return;
	last_lParam = lParam;
    }

    /* Handle specially, to centralise coding. We need to be sure we catch all
     * possible events which should cause us to restore the cursor (as it is a
     * shared resource, we take full responsibility for it).
     */
    switch (uMsg)
    {
    case WM_KEYUP:
    case WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	if (p_mh)
	    gui_mch_mousehide(TRUE);
	break;

    case WM_SYSKEYUP:    /* show the pointer when a system-key is pressed */
    case WM_SYSCHAR:
    case WM_MOUSEMOVE:   /* show the pointer on any mouse action */
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	break;
    }
}

    static LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(uMsg, lParam);

    s_uMsg = uMsg;
    s_wParam = wParam;
    s_lParam = lParam;

    switch (uMsg)
    {
	HANDLE_MSG(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_MSG(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	/* HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate); */
	HANDLE_MSG(hwnd, WM_CHAR,	_OnChar);
	HANDLE_MSG(hwnd, WM_CLOSE,	_OnClose);
	/* HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand); */
	HANDLE_MSG(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_MSG(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_MSG(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_MSG(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef WANT_MENU
	HANDLE_MSG(hwnd, WM_COMMAND,	_OnMenu);
#endif
	/* HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove); */
	/* HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate); */
	HANDLE_MSG(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_MSG(hwnd, WM_SIZE,	_OnSize);
	/* HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand); */
	/* HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey); */
	HANDLE_MSG(hwnd, WM_VSCROLL,	_OnScroll);
	HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_MSG(hwnd, WM_ACTIVATEAPP, _OnActivateApp);

    case WM_QUERYENDSESSION:	/* System wants to go down. */
	gui_window_closed();    /* Will exit when no changed buffers. */
	return FALSE;		/* Do NOT allow system to go down. */

    case WM_ENDSESSION:
	if (wParam)	/* system only really goes down when wParam is TRUE */
	    _OnEndSession();
	break;

    case WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef WANT_MENU
	if (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((int)wParam))
		)
	    return HANDLE_WM_SYSCHAR((hwnd), (wParam), (lParam), (_OnSysChar));
	else
#endif
	    return DefWindowProc(hwnd, uMsg, wParam, lParam);

    case WM_SYSKEYUP:
#ifdef WANT_MENU
	/* Only when menu is active, ALT key is used for that. */
	if (gui.menu_is_active)
	    return DefWindowProc(hwnd, uMsg, wParam, lParam);
	else
#endif
	    return 0;

    case WM_CREATE:	/* HANDLE_MSG doesn't seem to handle this one */
	return _OnCreate (hwnd, (LPCREATESTRUCT)lParam);

    case WM_SIZING:	/* HANDLE_MSG doesn't seem to handle this one */
	return _DuringSizing(hwnd, wParam, (LPRECT)lParam);

    case WM_MOUSEWHEEL:
	_OnMouseWheel(hwnd, HIWORD(wParam));
	break;

#ifdef USE_TOOLBAR
    case WM_NOTIFY:
	switch (((LPNMHDR) lParam)->code)
	{
	    case TTN_NEEDTEXT:
		{
		    LPTOOLTIPTEXT	lpttt;
		    UINT		idButton;
		    int			idx;
		    VimMenu		*pMenu;

		    lpttt = (LPTOOLTIPTEXT)lParam;
		    idButton = lpttt->hdr.idFrom;
		    pMenu = gui_w32_find_menu(root_menu, idButton);
		    if (pMenu)
		    {
			idx = MENU_INDEX_TIP;
			if (pMenu->strings[idx])
			{
			    lpttt->hinst = NULL;  /* string, not resource*/
			    lpttt->lpszText = pMenu->strings[idx];
			}
		    }
		}
		break;
	    default:
		break;
	}
	break;
#endif
#if defined(MENUHINTS) && defined(WANT_MENU)
    case WM_MENUSELECT:
	if (((UINT) HIWORD(wParam)
		    & (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
		== MF_HILITE
		&& (State & CMDLINE) == 0)
	{
	    UINT idButton;
	    int	idx;
	    VimMenu *pMenu;

	    idButton = (UINT)LOWORD(wParam);
	    pMenu = gui_w32_find_menu(root_menu, idButton);
	    if (pMenu)
	    {
		idx = MENU_INDEX_TIP;
		if (pMenu->strings[idx])
		    msg(pMenu->strings[idx]);
		else
		    msg("");
		setcursor();
		out_flush();
	    }
	}
	break;
#endif
#if 1
    case WM_NCHITTEST:
	{
	    LRESULT	result;
	    int x, y;
	    int xPos = GET_X_LPARAM(lParam);

	    result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	    if (result == HTCLIENT)
	    {
		gui_mch_get_winpos(&x, &y);
		xPos -= x;

		if (xPos < 48) /*<VN> TODO should use system metric?*/
		    return HTBOTTOMLEFT;
		else
		    return HTBOTTOMRIGHT;
		}
	    else
		return result;
	}
	break;
#endif

    default:
	if (uMsg == msh_msgmousewheel && msh_msgmousewheel != 0)
	{   /* handle MSH_MOUSEWHEEL messages for Intellimouse */
	    _OnMouseWheel(hwnd, HIWORD(wParam));
	    break;
	}
#ifdef WIN32_FIND_REPLACE
	else if (uMsg == s_findrep_msg && s_findrep_msg != 0)
	{
	    _OnFindRepl();
	}
#endif
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 1;
}

    static LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(uMsg, lParam);

    s_uMsg = uMsg;
    s_wParam = wParam;
    s_lParam = lParam;

    switch (uMsg)
    {
	HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_MSG(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);

    default:
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


/*
 * End of call-back routines
 */

    static void
gui_w32_oleerr(char *arg)
{
    EMSG2("Argument not supported: \"-%s\"; Use the OLE version.", arg);
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(int *argc, char **argv)
{
    /* Check for special OLE command line parameters */
    if (*argc == 2 && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	/* Register Vim as an OLE Automation server */
	if (STRICMP(argv[1] + 1, "register") == 0)
	{
#ifdef HAVE_OLE
	    RegisterMe();
	    mch_windexit(0);
#else
	    gui_w32_oleerr("register");
	    mch_windexit(2);
#endif
	}

	/* Unregister Vim as an OLE Automation server */
	if (STRICMP(argv[1] + 1, "unregister") == 0)
	{
#ifdef HAVE_OLE
	    UnregisterMe(TRUE);
	    mch_windexit(0);
#else
	    gui_w32_oleerr("unregister");
	    mch_windexit(2);
#endif
	}

	/* Ignore an -embedding argument. It is only relevant if the
	 * application wants to treat the case when it is started manually
	 * differently from the case where it is started via automation (and
	 * we don't).
	 */
	if (STRICMP(argv[1] + 1, "embedding") == 0)
	{
#ifdef HAVE_OLE
	    *argc = 1;
#else
	    gui_w32_oleerr("embedding");
	    mch_windexit(2);
#endif
	}
    }

#ifdef HAVE_OLE
    {
	int	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	/* automatically exit after registering */
	if (bDoRestart)
	    mch_windexit(0);
    }
#endif

    /* get the OS version info */
    os_version.dwOSVersionInfoSize = sizeof(os_version);
    GetVersionEx(&os_version); /* this call works on Win32s, Win95 and WinNT */
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    int
gui_mch_init(void)
{
    const char szVimWndClass[] = VIM_CLASS;
    const char szTextAreaClass[] = "VimTextArea";
    WNDCLASS wndclass;

    /* Display any pending error messages */
    mch_display_error();

    /* Return here if the window was already opened (happens when
     * gui_mch_dialog() is called early). */
    if (s_hwnd != NULL)
	return OK;

    /*
     * Load the tearoff bitmap
     */
    s_htearbitmap = LoadBitmap(s_hinst, "IDB_TEAROFF");

    gui.scrollbar_width = GetSystemMetrics(SM_CXVSCROLL);
    gui.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL);
#ifdef WANT_MENU
    gui.menu_height = 0;	/* Windows takes care of this */
#endif
    gui.border_width = 0;

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    wndclass.style = 0;
    wndclass.lpfnWndProc = _WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = s_hinst;
    wndclass.hIcon = LoadIcon(wndclass.hInstance, "IDR_VIM");
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = s_brush;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szVimWndClass;

    if (RegisterClass(&wndclass) == 0)
	return FAIL;

    s_hwnd = CreateWindow(
	szVimWndClass, "Vim W32 GUI",
	WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, CW_USEDEFAULT,
	100,				/* Any value will do */
	100,				/* Any value will do */
	NULL, NULL,
	s_hinst, NULL);

    if (s_hwnd == NULL)
	return FAIL;

    /* Create the text area window */
    wndclass.style = CS_OWNDC;
    wndclass.lpfnWndProc = _TextAreaWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = s_hinst;
    wndclass.hIcon = NULL;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szTextAreaClass;

    if (RegisterClass(&wndclass) == 0)
	return FAIL;

    s_textArea = CreateWindowEx(
	WS_EX_CLIENTEDGE,
	szTextAreaClass, "Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				/* Any value will do for now */
	100,				/* Any value will do for now */
	s_hwnd, NULL,
	s_hinst, NULL);

    if (s_textArea == NULL)
	return FAIL;

#ifdef WANT_MENU
    s_menuBar = CreateMenu();
#endif
    s_hdc = GetDC(s_textArea);

    DragAcceptFiles(s_hwnd, TRUE);

    /* Do we need to bother with this? */
    /* m_fMouseAvail = GetSystemMetrics(SM_MOUSEPRESENT); */

    /* Get background/foreground colors from system */
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    /* Get the colors from the "Normal" group (set in syntax.c or in a vimrc
     * file) */
    set_normal_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    /* Get the colors for the highlight groups (gui_check_colors() might have
     * changed them) */
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_offset = gui.border_width + 2;	/*CLIENT EDGE*/

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#ifdef USE_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#endif
#ifdef WIN32_FIND_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_msg = RegisterWindowMessage(FINDMSGSTRING);

    /* Initialise the struct */
    s_findrep_struct.lStructSize = sizeof(s_findrep_struct);
    s_findrep_struct.lpstrFindWhat = alloc(256);
    s_findrep_struct.lpstrFindWhat[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = alloc(256);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.wFindWhatLen = 256;
    s_findrep_struct.wReplaceWithLen = 256;
#endif

    return OK;
}

/*
 * Called when the foreground or background color has been changed.
 */
    void
gui_mch_new_colors(void)
{
    /* nothing to do? */
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    int
gui_mch_open(void)
{
    /* Actually open the window */
    ShowWindow(s_hwnd, SW_SHOWNORMAL);

    return OK;
}

    void
gui_mch_exit(int rc)
{
    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

    /* Unload the tearoff bitmap */
    (void)DeleteObject((HGDIOBJ)s_htearbitmap);

    /* Destroy our window (if we have one). */
    if (s_hwnd != NULL)
    {
	destroying = TRUE;	/* ignore WM_DESTROY message now */
	DestroyWindow(s_hwnd);
    }
}

/*
 * Get the position of the top left corner of the window.
 */
    int
gui_mch_get_winpos(int *x, int *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.left;
    *y = rect.top;
    return OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    void
gui_mch_set_winpos(int x, int y)
{
    SetWindowPos(s_hwnd, NULL, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    void
gui_mch_set_winsize(int width, int height, int min_width, int min_height,
		    int base_width, int base_height)
{
    RECT    workarea_rect;
    int     win_width, win_height;
    int	    win_xpos, win_ypos;
    WINDOWPLACEMENT wndpl;

    /* try to keep window completely on screen */
    /* get size of the screen work area (excludes taskbar, appbars) */
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea_rect, 0);

    /* get current posision of our window */
    wndpl.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(s_hwnd, &wndpl);
    if (wndpl.showCmd == SW_SHOWNORMAL)
    {
	win_xpos = wndpl.rcNormalPosition.left;
	win_ypos = wndpl.rcNormalPosition.top;
    }
    else
    {
	win_xpos = workarea_rect.left;
	win_ypos = workarea_rect.top;
    }

    /* compute the size of the outside of the window */
    win_width = width + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
    win_height = height + GetSystemMetrics(SM_CYSIZEFRAME) * 2
			+ GetSystemMetrics(SM_CYCAPTION)
			+ gui_w32_get_menu_height(FALSE);

    /* if the window is going off the screen, move it on to the screen */
    if (win_xpos + win_width > workarea_rect.right)
	win_xpos = workarea_rect.right - win_width;

    if (win_xpos < workarea_rect.left)
	win_xpos = workarea_rect.left;

    if (win_ypos + win_height > workarea_rect.bottom)
	win_ypos = workarea_rect.bottom - win_height;

    if (win_ypos < workarea_rect.top)
	win_ypos = workarea_rect.top;

    /* set window position */
    SetWindowPos(s_hwnd, NULL, win_xpos, win_ypos, win_width, win_height,
		 SWP_NOZORDER | SWP_NOACTIVATE);

    /* Menu may wrap differently now */
    gui_w32_get_menu_height(!gui.starting);
}

    void
gui_mch_get_screen_dimensions(int *screen_w, int *screen_h)
{
    RECT    workarea_rect;

    /* get size of the screen work area (excludes taskbar, appbars) */
    SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea_rect, 0);

    *screen_w = workarea_rect.right - workarea_rect.left
	      - GetSystemMetrics(SM_CXSIZEFRAME) * 2;
    *screen_h = workarea_rect.bottom - workarea_rect.top
	      - GetSystemMetrics(SM_CYSIZEFRAME) * 2
	      - GetSystemMetrics(SM_CYCAPTION)
	      - gui_w32_get_menu_height(FALSE);
}

    void
gui_mch_set_text_area_pos(int x, int y, int w, int h)
{
    SetWindowPos(s_textArea, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef USE_TOOLBAR
    if (vim_strchr(p_go, GO_TOOLBAR) != NULL)
	SendMessage(s_toolbarhwnd, WM_SIZE,
		(WPARAM)0, (LPARAM)(w + ((long)(TOOLBAR_BUTTON_HEIGHT+8)<<16)));
#endif
}


/*
 * Scrollbar stuff:
 */

    void
gui_mch_enable_scrollbar(
    GuiScrollbar    *sb,
    int		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);
}

    void
gui_mch_set_scrollbar_thumb(
    GuiScrollbar    *sb,
    int		    val,
    int		    size,
    int		    max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    while (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    if (sb->scroll_shift > 0)
	++size;

    info.cbSize = sizeof(info);
    info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.nPage = size;
    SetScrollInfo(sb->id, SB_CTL, &info, TRUE);
}

    void
gui_mch_set_scrollbar_pos(
    GuiScrollbar    *sb,
    int		    x,
    int		    y,
    int		    w,
    int		    h)
{
    SetWindowPos(sb->id, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    void
gui_mch_create_scrollbar(
    GuiScrollbar    *sb,
    int		    orient)	/* SBAR_VERT or SBAR_HORIZ */
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				/* Any value will do for now */
	10,				/* Any value will do for now */
	s_hwnd, NULL,
	s_hinst, NULL);
}

    void
gui_mch_destroy_scrollbar(GuiScrollbar *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get the character size of a font, by measuring the size of four characters.
 */
    static void
GetFontSize(GuiFont font)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    TEXTMETRIC tm;
    /*
    static const char ach[] = {'W', 'f', 'g', 'M'};
    SIZE    siz;

    GetTextExtentPoint(hdc, ach, sizeof(ach), &siz);
    */

    GetTextMetrics(hdc, &tm);
    gui.char_width = tm.tmAveCharWidth + tm.tmOverhang;

    /*
     * Make characters one pixel higher, so that italic and bold fonts don't
     * draw off the bottom of their character space.  Also means that we can
     * underline an underscore for normal text.
     */
    gui.char_height = tm.tmHeight+1; /*siz.cy + 1;*/

    /* TRACE("GetFontSize: h %d, w %d\n", gui.char_height, gui.char_width); */

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

    static GuiFont
get_font_handle(LOGFONT *lf)
{
    HFONT   font = NULL;

    /* Load the font */
    font = CreateFontIndirect(lf);

    if (font == NULL)
	return (GuiFont)0;

    return (GuiFont)font;
}

/* Convert a string representing a point size into pixels. The string should
 * be a positive decimal number, with an optional decimal point (eg, "12", or
 * "10.5"). The pixel value is returned, and a pointer to the next unconverted
 * character is stored in *end. The flag "vertical" says whether this
 * calculation is for a vertical (height) size or a horizontal (width) one.
 */
    static int
points_to_pixels(char_u *str, char_u **end, int vertical)
{
    int		pixels;
    int		points = 0;
    int		divisor = 0;
    HWND	hwnd;
    HDC		hdc;

    while (*str)
    {
	if (*str == '.' && divisor == 0)
	{
	    /* Start keeping a divisor, for later */
	    divisor = 1;
	    continue;
	}

	if (!isdigit(*str))
	    break;

	points *= 10;
	points += *str - '0';
	divisor *= 10;

	++str;
    }

    if (divisor == 0)
	divisor = 1;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    pixels = MulDiv(points,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX),
		    72 * divisor);

    ReleaseDC(hwnd, hdc);

    *end = str;
    return pixels;
}

    static int
pixels_to_points(int pixels, int vertical)
{
    int		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    return points;
}

    static int CALLBACK
font_enumproc(
    ENUMLOGFONT	    *elf,
    NEWTEXTMETRIC   *ntm,
    int		    type,
    LPARAM	    lparam)
{
    /* Return value:
     *	  0 = terminate now (monospace & ANSI)
     *	  1 = continue, still no luck...
     *	  2 = continue, but we have an acceptable LOGFONT
     *	      (monospace, not ANSI)
     * We use these values, as EnumFontFamilies returns 1 if the
     * callback function is never called. So, we check the return as
     * 0 = perfect, 2 = OK, 1 = no good...
     * It's not pretty, but it works!
     */

    LOGFONT *lf = (LOGFONT *)(lparam);

    /* Ignore non-monospace fonts without further ado */
    if ((ntm->tmPitchAndFamily & 1) != 0)
	return 1;

    /* Remember this LOGFONT as a "possible" */
    *lf = elf->elfLogFont;

    /* Terminate the scan as soon as we find an ANSI font */
    if (lf->lfCharSet == ANSI_CHARSET
	    || lf->lfCharSet == OEM_CHARSET
	    || lf->lfCharSet == DEFAULT_CHARSET)
	return 0;

    /* Continue the scan - we have a non-ANSI font */
    return 2;
}

    static int
init_logfont(LOGFONT *lf)
{
    int		n;
    HWND	hwnd = GetDesktopWindow();
    HDC		hdc = GetWindowDC(hwnd);

    n = EnumFontFamilies(hdc,
			 lf->lfFaceName,
			 (FONTENUMPROC)font_enumproc,
			 (LPARAM)(lf));

    ReleaseDC(hwnd, hdc);

    /* If we couldn't find a useable font, return failure */
    if (n == 1)
	return FAIL;

    /* Tidy up the rest of the LOGFONT structure. We set to a basic
     * font - get_logfont() sets bold, italic, etc based on the user's
     * input.
     */
    lf->lfHeight = current_font_height;
    lf->lfWidth = 0;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;
    lf->lfWeight = FW_NORMAL;

    /* Return success */
    return OK;
}

    static int
get_logfont(
    LOGFONT *lf,
    char_u  *name)
{
    char_u	*p;
    CHOOSEFONT	cf;
    int		i;
    static LOGFONT *lastlf = NULL;

    *lf = s_lfDefault;
    if (name == NULL)
	return 1;

    if (STRCMP(name, "*") == 0)
    {
	/* ron: if name is "*", bring up std font dialog: */
	memset(&cf, 0, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = s_hwnd;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	if (lastlf != NULL)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = 0 ; //REGULAR_FONTTYPE;
	if (ChooseFont(&cf))
	    goto theend;
    }

    /*
     * Split name up, it could be <name>:h<height>:w<width> etc.
     */
    for (p = name; *p && *p != ':'; p++)
    {
	if (p - name + 1 > LF_FACESIZE)
	    return 0;			/* Name too long */
	lf->lfFaceName[p - name] = *p;
    }
    if (p != name)
	lf->lfFaceName[p - name] = NUL;

    /* First set defaults */
    lf->lfHeight = -12;
    lf->lfWidth = 0;
    lf->lfWeight = FW_NORMAL;
    lf->lfItalic = FALSE;
    lf->lfUnderline = FALSE;
    lf->lfStrikeOut = FALSE;

    /*
     * If the font can't be found, try replacing '_' by ' '.
     */
    if (init_logfont(lf) == FAIL)
    {
	int	did_replace = FALSE;

	for (i = 0; lf->lfFaceName[i]; ++i)
	    if (lf->lfFaceName[i] == '_')
	    {
		lf->lfFaceName[i] = ' ';
		did_replace = TRUE;
	    }
	if (!did_replace || init_logfont(lf) == FAIL)
	    return 0;
    }

    while (*p == ':')
	p++;

    /* Set the values found after ':' */
    while (*p)
    {
	switch (*p++)
	{
	    case 'h':
		lf->lfHeight = - points_to_pixels(p, &p, TRUE);
		break;
	    case 'w':
		lf->lfWidth = points_to_pixels(p, &p, FALSE);
		break;
	    case 'b':
		lf->lfWeight = FW_BOLD;
		break;
	    case 'i':
		lf->lfItalic = TRUE;
		break;
	    case 'u':
		lf->lfUnderline = TRUE;
		break;
	    case 's':
		lf->lfStrikeOut = TRUE;
		break;
	    default:
		sprintf((char *)IObuff,
			"Illegal char '%c' in font name \"%s\"",
			p[-1], name);
		EMSG(IObuff);
		break;
	}
	while (*p == ':')
	    p++;
    }

theend:
    /* ron: init lastlf */
    vim_free(lastlf);
    lastlf = (LOGFONT *)alloc(sizeof(LOGFONT));
    if (lastlf != NULL)
	mch_memmove(lastlf, lf, sizeof(LOGFONT));

    return 1;
}

/*
 * Initialise vim to use the font with the given name.	Return FAIL if the font
 * could not be loaded, OK otherwise.
 */
    int
gui_mch_init_font(char_u *font_name)
{
    LOGFONT	lf;
    GuiFont	font = (GuiFont)0;
    char	*p;

    /* Load the font */
    if (get_logfont(&lf, font_name))
	font = get_font_handle(&lf);
    if (font == (GuiFont)0)
	return FAIL;
    if (font_name == NULL)
	font_name = lf.lfFaceName;
#ifdef MULTI_BYTE_IME
    norm_logfont = lf;
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lf.lfHeight;
    GetFontSize(font);
    hl_set_font_name(lf.lfFaceName);
    if (STRCMP(font_name, "*") == 0)
    {
	p = alloc((unsigned)(strlen(lf.lfFaceName) + 10));
	if (p != NULL)
	{
	    /* make a normal font string out of the lf thing:*/
	    sprintf(p, "%s:h%d", lf.lfFaceName, pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE));
	    vim_free(p_guifont);
	    p_guifont = p;
	    while (*p)
	    {
		if (*p == ' ')
		    *p = '_';
		++p;
	    }
	}
    }

    if (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    if (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	if (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    return OK;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    int		giveErrorIfMissing)
{
    LOGFONT	lf;
    GuiFont	font;

    get_logfont(&lf, name);
    font = get_font_handle(&lf);
    if (font == (GuiFont)0 && giveErrorIfMissing)
	EMSG2("Unknown font: %s", name);
    return font;
}

/*
 * Set the current text font.
 */
    void
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}

#if 0 /* not used */
/*
 * Return TRUE if the two fonts given are equivalent.
 */
    int
gui_mch_same_font(
    GuiFont f1,
    GuiFont f2)
{
    return f1 == f2;
}
#endif

    void
gui_mch_free_font(GuiFont font)
{
    if (font)
	DeleteObject((HFONT)font);
}

    static int
hex_digit(int c)
{
    if (vim_isdigit(c))
	return c - '0';
    c = TO_LOWER(c);
    if (c >= 'a' && c <= 'f')
	return c - 'a' + 10;
    return -1000;
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return -1 for error.
 */
    GuiColor
gui_mch_get_color(char_u *name)
{
    typedef struct GuiColorTable
    {
	char	    *name;
	COLORREF    color;
    } GuiColorTable;

    static GuiColorTable table[] =
    {
	{"Black",		RGB(0x00, 0x00, 0x00)},
	{"DarkGray",		RGB(0x80, 0x80, 0x80)},
	{"DarkGrey",		RGB(0x80, 0x80, 0x80)},
	{"Gray",		RGB(0xC0, 0xC0, 0xC0)},
	{"Grey",		RGB(0xC0, 0xC0, 0xC0)},
	{"LightGray",		RGB(0xE0, 0xE0, 0xE0)},
	{"LightGrey",		RGB(0xE0, 0xE0, 0xE0)},
	{"White",		RGB(0xFF, 0xFF, 0xFF)},
	{"DarkRed",		RGB(0x80, 0x00, 0x00)},
	{"Red",			RGB(0xFF, 0x00, 0x00)},
	{"LightRed",		RGB(0xFF, 0xA0, 0xA0)},
	{"DarkBlue",		RGB(0x00, 0x00, 0x80)},
	{"Blue",		RGB(0x00, 0x00, 0xFF)},
	{"LightBlue",		RGB(0xA0, 0xA0, 0xFF)},
	{"DarkGreen",		RGB(0x00, 0x80, 0x00)},
	{"Green",		RGB(0x00, 0xFF, 0x00)},
	{"LightGreen",		RGB(0xA0, 0xFF, 0xA0)},
	{"DarkCyan",		RGB(0x00, 0x80, 0x80)},
	{"Cyan",		RGB(0x00, 0xFF, 0xFF)},
	{"LightCyan",		RGB(0xA0, 0xFF, 0xFF)},
	{"DarkMagenta",		RGB(0x80, 0x00, 0x80)},
	{"Magenta",		RGB(0xFF, 0x00, 0xFF)},
	{"LightMagenta",	RGB(0xFF, 0xA0, 0xFF)},
	{"Brown",		RGB(0x80, 0x40, 0x40)},
	{"Yellow",		RGB(0xFF, 0xFF, 0x00)},
	{"LightYellow",		RGB(0xFF, 0xFF, 0xA0)},
	{"SeaGreen",		RGB(0x2E, 0x8B, 0x57)},
	{"Orange",		RGB(0xFF, 0xA5, 0x00)},
	{"Purple",		RGB(0xA0, 0x20, 0xF0)},
	{"SlateBlue",		RGB(0x6A, 0x5A, 0xCD)},
	{"Violet",		RGB(0xEE, 0x82, 0xEE)},
    };

    typedef struct SysColorTable
    {
	char	    *name;
	int	    color;
    } SysColorTable;

    static SysColorTable sys_table[] =
    {
	"SYS_3DDKSHADOW", COLOR_3DDKSHADOW,
	"SYS_3DFACE", COLOR_3DFACE,
	"SYS_BTNFACE", COLOR_BTNFACE,
	"SYS_3DHILIGHT", COLOR_3DHILIGHT,
#ifndef __MINGW32__
	"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT,
#endif
	"SYS_BTNHILIGHT", COLOR_BTNHILIGHT,
	"SYS_BTNHIGHLIGHT", COLOR_BTNHIGHLIGHT,
	"SYS_3DLIGHT", COLOR_3DLIGHT,
	"SYS_3DSHADOW", COLOR_3DSHADOW,
	"SYS_BTNSHADOW", COLOR_BTNSHADOW,
	"SYS_ACTIVEBORDER", COLOR_ACTIVEBORDER,
	"SYS_ACTIVECAPTION", COLOR_ACTIVECAPTION,
	"SYS_APPWORKSPACE", COLOR_APPWORKSPACE,
	"SYS_BACKGROUND", COLOR_BACKGROUND,
	"SYS_DESKTOP", COLOR_DESKTOP,
	"SYS_BTNTEXT", COLOR_BTNTEXT,
	"SYS_CAPTIONTEXT", COLOR_CAPTIONTEXT,
	"SYS_GRAYTEXT", COLOR_GRAYTEXT,
	"SYS_HIGHLIGHT", COLOR_HIGHLIGHT,
	"SYS_HIGHLIGHTTEXT", COLOR_HIGHLIGHTTEXT,
	"SYS_INACTIVEBORDER", COLOR_INACTIVEBORDER,
	"SYS_INACTIVECAPTION", COLOR_INACTIVECAPTION,
	"SYS_INACTIVECAPTIONTEXT", COLOR_INACTIVECAPTIONTEXT,
	"SYS_INFOBK", COLOR_INFOBK,
	"SYS_INFOTEXT", COLOR_INFOTEXT,
	"SYS_MENU", COLOR_MENU,
	"SYS_MENUTEXT", COLOR_MENUTEXT,
	"SYS_SCROLLBAR", COLOR_SCROLLBAR,
	"SYS_WINDOW", COLOR_WINDOW,
	"SYS_WINDOWFRAME", COLOR_WINDOWFRAME,
	"SYS_WINDOWTEXT", COLOR_WINDOWTEXT,
    };

    int		    r, g, b;
    int		    i;

    if (name[0] == '#' && strlen(name) == 7)
    {
	/* Name is in "#rrggbb" format */
	r = hex_digit(name[1]) * 16 + hex_digit(name[2]);
	g = hex_digit(name[3]) * 16 + hex_digit(name[4]);
	b = hex_digit(name[5]) * 16 + hex_digit(name[6]);
	if (r < 0 || g < 0 || b < 0)
	    return (GuiColor)-1;
	return RGB(r, g, b);
    }
    else
    {
	/* Check if the name is one of the colors we know */
	for (i = 0; i < sizeof(table) / sizeof(table[0]); i++)
	    if (STRICMP(name, table[i].name) == 0)
		return table[i].color;
    }

    /*
     * Try to look up a system colour.
     */
    for (i = 0; i < sizeof(sys_table) / sizeof(sys_table[0]); i++)
	if (STRICMP(name, sys_table[i].name) == 0)
	    return GetSysColor(sys_table[i].color);

    /*
     * Last attempt. Look in the file "$VIMRUNTIME/rgb.txt".
     */
    {
#define LINE_LEN 100
	FILE	*fd;
	char	line[LINE_LEN];
	char_u	*fname;

	fname = expand_env_save((char_u *)"$VIMRUNTIME/rgb.txt");
	if (fname == NULL)
	    return (GuiColor)-1;

	fd = fopen((char *)fname, "rt");
	vim_free(fname);
	if (fd == NULL)
	    return (GuiColor)-1;

	while (!feof(fd))
	{
	    int	    len;
	    int	    pos;
	    char    *color;

	    fgets(line, LINE_LEN, fd);
	    len = strlen(line);

	    if (len <= 1 || line[len-1] != '\n')
		continue;

	    line[len-1] = '\0';

	    i = sscanf(line, "%d %d %d %n", &r, &g, &b, &pos);
	    if (i != 3)
		continue;

	    color = line + pos;

	    if (STRICMP(color, name) == 0)
	    {
		fclose(fd);
		return (GuiColor) RGB(r,g,b);
	    }
	}

	fclose(fd);
    }

    return (GuiColor)-1;
}

/*
 * Set the current text foreground color.
 */
    void
gui_mch_set_fg_color(GuiColor color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    void
gui_mch_set_bg_color(GuiColor color)
{
    gui.currBgColor = color;
}

#if defined(MULTI_BYTE) || defined(PROTO)

/*
 * Multi-byte handling, by Sung-Hoon Baek
 */
    static void
HanExtTextOut(HDC hdc,int X, int Y, UINT fuOption, const RECT *lprc,
	LPCTSTR lpString, UINT cbCount, const int *lpDx, BOOL bOpaque)
{
    LPCTSTR	pszTemp;
    int		i;
    HPEN	hpen, old_pen;
    POINT	point;

    if (gui.char_width == sysfixed_width && gui.char_height == sysfixed_height)
    {
	hpen = CreatePen(PS_SOLID, 2, gui.currFgColor);
	old_pen = SelectObject(hdc, hpen);

	pszTemp = lpString;
	i = 0;
	while (cbCount > 0)
	{
	    if (cbCount > 1 && IsLeadByte(*pszTemp))
	    {
		cbCount -= 2;
		pszTemp += 2;
		i += 2;
	    }
	    else if (*pszTemp == '\\')
	    {
		if (i > 0)
		    ExtTextOut(hdc, X+((pszTemp-i)-lpString)*gui.char_width, Y,
			    fuOption, lprc, pszTemp-i, i, lpDx);
		MoveToEx(hdc, (int)(X+(pszTemp-lpString)*gui.char_width
			    + gui.char_width*0.2),
			(int)(Y + gui.char_height*0.2), &point);
		LineTo(hdc, (int)(X+(pszTemp-lpString)*gui.char_width
			    + gui.char_width*0.8),
			(int)(Y + gui.char_height*0.75));
		pszTemp++;
		cbCount--;
		i = 0;
	    }
	    else
	    {
		pszTemp++;
		cbCount--;
		i++;
	    }
	}
	if (i > 0)
	{
	    int OldBkMode;

	    if (bOpaque)
	    {
		OldBkMode = GetBkMode(hdc);
		SetBkMode(hdc, OPAQUE);
	    }
	    ExtTextOut(hdc,X+((pszTemp-i)-lpString)*gui.char_width,Y,
		    fuOption,lprc,pszTemp-i,i,lpDx);
	    if (bOpaque)
		SetBkMode(hdc, OldBkMode);
	}

	DeleteObject(SelectObject(hdc, old_pen));
    }
    else
	ExtTextOut(hdc,X,Y,fuOption,lprc,lpString,cbCount,lpDx);
}

# if defined(MULTI_BYTE_IME) || defined(PROTO)

#include <ime.h>
#include <imm.h>

static BOOL bImeOpenStatus = FALSE;
static char lpCompStr[100];		// Pointer to composition str.
static BOOL bInComposition=FALSE;
static BOOL bCommandMode=TRUE;
static BOOL bImeNative = FALSE;

/*
 * display composition string(korean)
 */
    static void
DisplayCompStringOpaque(char_u *s, int len)
{
    int OldBkMode = GetBkMode(s_hdc);

    SetBkMode(s_hdc,OPAQUE);
    gui_outstr_nowrap(s,len,GUI_MON_TRS_CURSOR , (GuiColor)0, (GuiColor)0 ,0);
    SetBkMode(s_hdc,OldBkMode);
}


/*
 * When enter to insert mode, set IME to previous language mode
 */
    void
ImeSetOriginMode(void)
{
    HIMC    hImc;
    DWORD   dwConvMode, dwSentMode;

    if ((hImc = ImmGetContext(s_hwnd)))
    {
	if (!ImmGetOpenStatus(hImc) && bImeOpenStatus == TRUE)
	{
		ImmSetOpenStatus(hImc, TRUE);
	}
	else
	    bImeOpenStatus = FALSE;
    }
    bCommandMode = FALSE;
}


/* When enter to command mode, set IME to english mode */
    void
ImeSetEnglishMode(void)
{
    HIMC    hImc;
    DWORD   dwConvMode, dwSentMode;

    if ((hImc = ImmGetContext(s_hwnd)))
    {
	ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
	if (ImmGetOpenStatus(hImc))
	{
		ImmSetOpenStatus(hImc, FALSE);
	    bImeOpenStatus = TRUE;
	}
	else
	    bImeOpenStatus = FALSE;
    }
    bCommandMode = TRUE;
}

/* get composition string from WIN_IME */
    static void
GetCompositionStr(HWND hwnd, LPARAM CompFlag)
{
    DWORD	dwBufLen;		// Stogare for len. of composition str
    HIMC	hIMC;			// Input context handle.

    // If fail to get input context handle then do nothing.
    // Applications should call ImmGetContext API to get
    // input context handle.

    if (!(hIMC = ImmGetContext(hwnd)))
	return;

    // Determines how much memory space to store the composition string.
    // Applications should call ImmGetCompositionString with
    // GCS_COMPSTR flag on, buffer length zero, to get the bullfer
    // length.

    if ((dwBufLen = ImmGetCompositionString( hIMC, GCS_COMPSTR,
		    (void FAR*)NULL, 0l)) < 0)
	goto exit2;

    if (dwBufLen > 99)
	goto exit2;

    // Reads in the composition string.
    if ( dwBufLen != 0 )
    {
	ImmGetCompositionString(hIMC, GCS_COMPSTR, lpCompStr, dwBufLen);
	lpCompStr[dwBufLen] = 0;
    }
    else
    {
	strcpy(lpCompStr,"  ");
	dwBufLen = 2;
    }

    // Display new composition chars.
    DisplayCompStringOpaque(lpCompStr, dwBufLen);


exit2:
    ImmReleaseContext(hwnd, hIMC);
}


// void GetResultStr()
//
// This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
//
// get complete composition string

    static void
GetResultStr(HWND hwnd)
{
    DWORD	dwBufLen;		// Storage for length of result str.
    HIMC	hIMC;			// Input context handle.

    // If fail to get input context handle then do nothing.
    if (!(hIMC = ImmGetContext(hwnd)))
	return;

    // Determines how much memory space to store the result string.
    // Applications should call ImmGetCompositionString with
    // GCS_RESULTSTR flag on, buffer length zero, to get the bullfer
    // length.
    if ((dwBufLen = ImmGetCompositionString( hIMC, GCS_RESULTSTR,
		    (void FAR *)NULL, (DWORD) 0)) <= 0)
	goto exit2;

    if (dwBufLen > 99)
	goto exit2;

    // Reads in the result string.
    ImmGetCompositionString(hIMC, GCS_RESULTSTR, lpCompStr, dwBufLen);

    // Displays the result string.
    DisplayCompStringOpaque(lpCompStr, dwBufLen);

exit2:
    ImmReleaseContext(hwnd, hIMC);
}

/* this handles WM_IME_STARTCOMPOSITION */
    static void
ImeUIStartComposition(HWND hwnd)
{
     bInComposition = TRUE;
     //GetResultStr( hwnd );
}

/* WM_IME_COMPOSITION */
    static void
ImeUIComposition(HWND hwnd, WPARAM wParam,LPARAM CompFlag)
{

    if (CompFlag & GCS_RESULTSTR)
	GetResultStr( hwnd );
    else if (CompFlag & GCS_COMPSTR)
	GetCompositionStr( hwnd, CompFlag );
}

/* WM_IME_COMPOSITION */
    static void
ImeUIEndComposition(HWND hwnd)
{
    bInComposition = FALSE;
    //GetResultStr( hwnd );
}

    static char *
ImeGetTempComposition(void)
{
    if ( bInComposition == TRUE /* && bCommandMode == FALSE */)
    {
	HIMC    hImc;
	DWORD   dwConvMode, dwSentMode;

	if ((hImc = ImmGetContext(s_hwnd)))
	{
	    ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
	    if ((dwConvMode & IME_CMODE_NATIVE))
		return lpCompStr;
	}
    }
    return NULL;
}

    static void
ImeNotify(WPARAM w, LPARAM l)
{
    HIMC    hImc;
    DWORD   dwConvMode, dwSentMode;
    COMPOSITIONFORM cf;

    if ((hImc = ImmGetContext(s_hwnd)))
    {
	ImmGetConversionStatus(hImc, &dwConvMode, &dwSentMode);
	if (dwConvMode & IME_CMODE_NATIVE)
	{
	    RECT t_rct;
	    RECT w_rct;
	    if (w = IMN_SETOPENSTATUS && GetWindowRect(s_textArea, &t_rct)
		&& GetWindowRect(s_hwnd, &w_rct))
	    {
		ImmSetCompositionFont(hImc, &norm_logfont);
		cf.dwStyle = CFS_POINT;
		cf.ptCurrentPos.x = TEXT_X(gui.col) + gui.border_offset;
		cf.ptCurrentPos.y = TEXT_Y(gui.row) + gui.border_offset;
#ifdef WANT_MENU
		if (gui.menu_is_active)
		    cf.ptCurrentPos.y += gui.menu_height;
#endif
#ifdef USE_TOOLBAR
		if (vim_strchr(p_go, GO_TOOLBAR) != NULL)
		    cf.ptCurrentPos.y +=
			TOOLBAR_BUTTON_HEIGHT + TOOLBAR_BORDER_HEIGHT;
#endif
		ImmSetCompositionWindow(hImc, &cf);
	    }
	    bImeNative = TRUE;
	}
	else
	    bImeNative = FALSE;
    }
}

#  if 0 // This is not used !?
    void
ImeOpenClose(HWND hWnd, BOOL fFlag)
{
    HIMC	hIMC;

    //
    // If fFlag is true then open IME; otherwise close it.
    //

    if (!(hIMC = ImmGetContext(hWnd)))
	return;

    ImmSetOpenStatus(hIMC, fFlag);

    ImmReleaseContext(hWnd, hIMC);
}

/*
*   IsDBCSTrailByte - returns TRUE if the given byte is a DBCS trail byte
*
*		The algorithm searchs backward in the string, to some known
*		character boundary, counting consecutive bytes in the lead
*		byte range. An odd number indicates the current byte is part
*		of a two byte character code.
*
*   INPUT: PCHAR  - pointer to a preceding known character boundary.
*	   PCHAR  - pointer to the character to test.
*
*   OUTPUT:BOOL   - indicating truth of p==trailbyte.
*
*/
    int
IsDBCSTrailByte(char *base, char *p)
{
    int lbc = 0;    // lead byte count

    if (base > p)
	return 0;
    if (strlen(base) <= (size_t)(p-base))
	return 0;

    while (p > base)
    {
	if (!IsLeadByte(*(--p)))
	    break;
	lbc++;
    }

    return (lbc & 1);
}
#  endif /* not used */

# endif /* MULTI_BYTE_IME */

#endif /* MULTI_BYTE */

#define UNIBUFSIZE 2000		/* a big buffer */

    void
gui_mch_draw_string(
    int		row,
    int		col,
    char_u	*s,
    int		len,
    int		flags)
{
    static int	*padding = NULL;
    static int	pad_size = 0;
#ifdef MULTI_BYTE
    static WCHAR *unicodebuf = NULL;
#endif
    HPEN	hpen, old_pen;
    int		y;
    int		i;

#if 1
    /*
     * Italic and bold text seems to have an extra row of pixels at the bottom
     * (below where the bottom of the character should be).  If we draw the
     * characters with a solid background, the top row of pixels in the
     * character below will be overwritten.  We can fix this by filling in the
     * background ourselves, to the correct character proportions, and then
     * writing the character in transparent mode.  Still have a problem when
     * the character is "_", which gets written on to the character below.
     * New fix: set gui.char_ascent to -1.  This shifts all characters up one
     * pixel in their slots, which fixes the problem with the bottom row of
     * pixels.	We still need this code because otherwise the top row of pixels
     * becomes a problem. - webb.
     */
    HBRUSH	hbr;
    RECT	rc;
#ifdef MULTI_BYTE_IME
    char	*szComp;
#endif

#ifdef MULTI_BYTE
    int		OrgLen;

    if (is_dbcs)
    {

	OrgLen = len;
	if (len == 1 && IsLeadByte(s[0]))
	    len++;
    }
#endif

    if (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.left = FILL_X(col);
	rc.top = FILL_Y(row);
	rc.right = FILL_X(col + len);	    /* Add +1 to erase fake bold? */
	rc.bottom = FILL_Y(row + 1);
	hbr = CreateSolidBrush(gui.currBgColor);
	FillRect(s_hdc, &rc, hbr);
	DeleteBrush(hbr);

	SetBkMode(s_hdc, TRANSPARENT);
    }
#else
    /*
     * The alternative would be to write the characters in opaque mode, but
     * when the text is not exactly the same proportions as normal text, too
     * big or too little a rectangle gets drawn for the background.
     */
    SetBkMode(s_hdc, OPAQUE);
    SetBkColor(s_hdc, gui.currBgColor);
#endif
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

    if (pad_size != Columns || padding == NULL || padding[0] != gui.char_width)
    {
	vim_free(padding);
	pad_size = Columns;

	padding = (int *)alloc(pad_size * sizeof(int));
	if (padding != NULL)
	    for (i = 0; i < pad_size; i++)
		padding[i] = gui.char_width;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */
#ifdef MULTI_BYTE_IME
    if (is_dbcs)
    {
	/* draw an incomplete composition character (korean) */
	if (OrgLen == 1 && blink_state == BLINK_ON
		&& (szComp = ImeGetTempComposition()) != NULL) // hangul
	    HanExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row), 0, NULL, szComp,
		    2, padding, TRUE);
	else
	    HanExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row), 0, NULL, (char *)s,
		    len, padding, FALSE);
    }
    else
#endif
    {
#ifdef MULTI_BYTE
	/* if we want to display DBCS, and the current CP is not the DBCS one,
	 * we need to go via Unicode */
	if (is_funky_dbcs)
	{
	    /* check if our output buffer exists, if not create it */
	    if (unicodebuf == NULL)
		unicodebuf = (WCHAR *)alloc(UNIBUFSIZE);
	    if (unicodebuf != NULL)
	    {
		if (len = MultiByteToWideChar(is_dbcs,
			    MB_PRECOMPOSED,
			    (char *)s, len,
			    (LPWSTR)unicodebuf, UNIBUFSIZE))
		    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row), 0, NULL,
						       unicodebuf, len, NULL);
	    }
	}
	else
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row), 0, NULL,
						     (char *)s, len, padding);
    }

    if (flags & DRAW_UNDERL)
    {
	hpen = CreatePen(PS_SOLID, 1, gui.currFgColor);
	old_pen = SelectObject(s_hdc, hpen);
	y = FILL_Y(row + 1) - 1;
	MoveToEx(s_hdc, FILL_X(col), y, NULL);
	/* Note: LineTo() excludes the last pixel in the line. */
	LineTo(s_hdc, FILL_X(col + len), y);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    int
gui_mch_haskey(char_u *name)
{
    int i;

    for (i = 0; special_keys[i].vim_code1 != NUL; i++)
	if (name[0] == special_keys[i].vim_code0 &&
					 name[1] == special_keys[i].vim_code1)
	    return OK;
    return FAIL;
}

    void
gui_mch_beep(void)
{
    MessageBeep(MB_OK);
}

    void
gui_mch_flash(void)
{
    RECT    rc;

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = 0;
    rc.top = 0;
    rc.right = gui.num_cols * gui.char_width;
    rc.bottom = gui.num_rows * gui.char_height;
    InvertRect(s_hdc, &rc);

    ui_delay(20L, TRUE);	/* wait 1/50 of a second */

    InvertRect(s_hdc, &rc);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    void
gui_mch_invert_rectangle(
    int	    r,
    int	    c,
    int	    nr,
    int	    nc)
{
    RECT    rc;

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(c);
    rc.top = FILL_Y(r);
    rc.right = rc.left + nc * gui.char_width;
    rc.bottom = rc.top + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    void
gui_mch_iconify(void)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

#if defined(HAVE_OLE) || defined(PROTO)
/*
 * Make the GUI window come to the foreground.
 */
    void
gui_mch_set_foreground(void)
{
    if (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

/*
 * Set the window title
 */
    void
gui_mch_settitle(
    char_u  *title,
    char_u  *icon)
{
    SetWindowText(s_hwnd, (LPCSTR)(title == NULL ? "VIM" : title));
}

/*
 * Draw a cursor without focus.
 */
    void
gui_mch_draw_hollow_cursor(GuiColor color)
{
    HBRUSH  hbr;
    RECT    rc;

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(gui.col);
    rc.top = FILL_Y(gui.row);
    rc.right = rc.left + gui.char_width;
#ifdef MULTI_BYTE
    if (IsLeadByte((int)*(LinePointers[gui.row] + gui.col)))
	rc.right += gui.char_width;
#endif
    rc.bottom = rc.top + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    void
gui_mch_draw_part_cursor(
    int		w,
    int		h,
    GuiColor	color)
{
    HBRUSH	hbr;
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.left =
#ifdef RIGHTLEFT
		/* vertical line should be on the right of current point */
		State != CMDLINE && curwin->w_p_rl ?  FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.top = FILL_Y(gui.row) + gui.char_height - h;
    rc.right = rc.left + w;
    rc.bottom = rc.top + h;
    hbr = CreateSolidBrush(color);
    FillRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    static void
process_message(void)
{
    MSG	    msg;
    UINT    vk = 0;		/* Virtual key */
    char_u  string[3];
    int	    i;
    int	    modifiers = 0;
    int	    key;

    GetMessage(&msg, NULL, 0, 0);

#ifdef HAVE_OLE
    /* Look after OLE Automation commands */
    if (msg.message == WM_OLE)
    {
	char_u *str = (char_u *)msg.lParam;
	add_to_input_buf(str, strlen(str));
	vim_free(str);
	return;
    }
#endif

#ifdef USE_SNIFF
    if (msg.message == WM_USER && sniff_request_waiting)
    {
	add_to_input_buf((char_u *)"\33", 1); /* just add a single escape */
	/* request is handled in normal.c */
	return;
    }
#endif

#ifdef WIN32_FIND_REPLACE
    /* Don't process messages used by the dialog */
    if ((s_findrep_hwnd) && (IsDialogMessage(s_findrep_hwnd, &msg)))
	return;
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
    {
	vk = (int) msg.wParam;
	/* handle key after dead key, but ignore shift, alt and control */
	if (dead_key && vk != VK_SHIFT && vk != VK_MENU && vk != VK_CONTROL)
	{
	    dead_key = 0;
	    /* handle non-alphabetic keys (ones that hopefully cannot generate
	     * umlaut-characters), unless when control is down */
	    if (vk < 'A' || vk > 'Z' || (GetKeyState(VK_CONTROL) & 0x8000))
	    {
		MSG dm;

		dm.message = msg.message;
		dm.hwnd = msg.hwnd;
		dm.wParam = VK_SPACE;
		TranslateMessage(&dm);	    /* generate dead character */
		if (vk != VK_SPACE) /* and send current character once more */
		    PostMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		return;
	    }
	}

	/* Check for CTRL-BREAK */
	if (vk == VK_CANCEL)
	{
	    trash_input_buf();
	    got_int = TRUE;
	    string[0] = Ctrl('C');
	    add_to_input_buf(string, 1);
	}

	for (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    /* ignore VK_SPACE when ALT key pressed: system menu */
	    if (special_keys[i].key_sym == vk
		    && (vk != VK_SPACE || !(GetKeyState(VK_MENU) & 0x8000)))
	    {
#ifdef WANT_MENU
		/* Check for <F10>: Windows selects the menu.  Ignore it when
		 * 'winaltkeys' is "yes" or "menu" */
		if (vk == VK_F10
			&& gui.menu_is_active
			&& (*p_wak == 'y' || *p_wak == 'm'))
		    break;
#endif

		if (GetKeyState(VK_SHIFT) & 0x8000)
		    modifiers |= MOD_MASK_SHIFT;
		/*
		 * Don't use caps-lock as shift, because these are special keys
		 * being considered here, and we only want letters to get
		 * shifted -- webb
		 */
		/*
		if (GetKeyState(VK_CAPITAL) & 0x0001)
		    modifiers ^= MOD_MASK_SHIFT;
		*/
		if (GetKeyState(VK_CONTROL) & 0x8000)
		    modifiers |= MOD_MASK_CTRL;
		if (GetKeyState(VK_MENU) & 0x8000)
		    modifiers |= MOD_MASK_ALT;

		if (special_keys[i].vim_code1 == NUL)
		    key = special_keys[i].vim_code0;
		else
		    key = TO_SPECIAL(special_keys[i].vim_code0,
						   special_keys[i].vim_code1);
		key = simplify_key(key, &modifiers);

		if (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_input_buf(string, 3);
		}

		if (IS_SPECIAL(key))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(key);
		    string[2] = K_THIRD(key);
		    add_to_input_buf(string, 3);
		}
		else
		{
		    string[0] = key;
		    add_to_input_buf(string, 1);
		}
		break;
	    }
	}
	if (special_keys[i].key_sym == 0)
	{
	    /* Some keys need C-S- where they should only need C- */
	    if ((GetKeyState(VK_CONTROL) & 0x8000)
		    && !(GetKeyState(VK_SHIFT) & 0x8000)
		    && !(GetKeyState(VK_MENU) & 0x8000))
	    {
		if (vk == '6')
		{
		    string[0] = Ctrl('^');
		    add_to_input_buf(string, 1);
		}
		/* vk == 0xDB AZERTY for CTRL-'-', but CTRL-[ for * QWERTY! */
		else if (vk == 0xBD)	/* QWERTY for CTRL-'-' */
		{
		    string[0] = Ctrl('_');
		    add_to_input_buf(string, 1);
		}
		else
		    TranslateMessage(&msg);
	    }
	    else
		TranslateMessage(&msg);
	}
    }
#ifdef MULTI_BYTE_IME
#if 0
    else if (msg.message == WM_IME_STARTCOMPOSITION)
	ImeUIStartComposition(s_hwnd);
    else if (msg.message == WM_IME_COMPOSITION)
	ImeUIComposition(s_hwnd, msg.wParam, msg.lParam);
    else if (msg.message == WM_IME_ENDCOMPOSITION)
	ImeUIEndComposition(s_hwnd);
    else if (msg.message == WM_IME_COMPOSITIONFULL)
	ImeUIEndComposition(s_hwnd);
    else
#endif
    if (msg.message == WM_IME_NOTIFY)
	ImeNotify(msg.wParam, msg.lParam);
#endif
#ifdef WANT_MENU
    /* Check for <F10>: Windows selects the menu.  Don't let Windows handle it
     * when 'winaltkeys' is "no" */
    if (vk != VK_F10 || *p_wak != 'n')
#endif
	DispatchMessage(&msg);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    void
gui_mch_update(void)
{
    MSG	    msg;

    if (!s_busy_processing)
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)
						  && !vim_is_input_buf_full())
	    process_message();
}

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    int
gui_mch_wait_for_chars(int wtime)
{
    MSG		msg;
    int		focus;

    s_timed_out = FALSE;

    if (wtime > 0)
    {
	/* Don't do anything while processing a (scroll) message. */
	if (s_busy_processing)
	    return FAIL;
	s_wait_timer = SetTimer(NULL, 0, (UINT)wtime, (TIMERPROC)_OnTimer);
    }

    focus = gui.in_focus;
    while (!s_timed_out)
    {
	/* Stop or start blinking when focus changes */
	if (gui.in_focus != focus)
	{
	    if (gui.in_focus)
		gui_mch_start_blink();
	    else
		gui_mch_stop_blink();
	    focus = gui.in_focus;
	}

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use GetMessage() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char -- webb
	 */
	process_message();

	if (!vim_is_input_buf_empty())
	{
	    if (s_wait_timer != 0 && !s_timed_out)
	    {
		KillTimer(NULL, s_wait_timer);

		/* Eat spurious WM_TIMER messages */
		while (PeekMessage(&msg, s_hwnd,
					      WM_TIMER, WM_TIMER, PM_REMOVE));
		s_wait_timer = 0;
	    }
	    return OK;
	}
    }
    return FAIL;
}

/*
 * Output routines.
 */

/* Flush any output to the screen */
    void
gui_mch_flush(void)
{
    /* Is anything needed here? */
}

    static void
clear_rect(RECT *rcp)
{
    HBRUSH  hbr;

    hbr = CreateSolidBrush(gui.back_pixel);
    FillRect(s_hdc, rcp, hbr);
    DeleteBrush(hbr);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_mch_clear_block(
    int	    row1,
    int	    col1,
    int	    row2,
    int	    col2)
{
    RECT    rc;

    /*
     * Clear one extra pixel at the right, for when bold characters have
     * spilled over to the next column.  Note: FillRect() excludes right and
     * bottom of rectangle.
     * Can this ever erase part of the next character? - webb
     */
    rc.left = FILL_X(col1);
    rc.top = FILL_Y(row1);
    rc.right = FILL_X(col2 + 1) + 1;
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    void
gui_mch_clear_all(void)
{
    RECT    rc;

    rc.left = 0;
    rc.top = 0;
    rc.right = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    void
gui_mch_delete_lines(
    int	    row,
    int	    num_lines)
{
    if (num_lines <= 0)
	return;

    if (row + num_lines > gui.scroll_region_bot)
    {
	/* Scrolled out of region, just blank the lines out */
	gui_clear_block(row, 0, gui.scroll_region_bot, Columns - 1);
    }
    else
    {
	RECT	rc;

	rc.left = FILL_X(0);
	rc.right = FILL_X(Columns);
	rc.top = FILL_Y(row);
	rc.bottom = FILL_Y(gui.scroll_region_bot + 1);
	/* The SW_INVALIDATE is required when part of the window is covered or
	 * off-screen.  How do we avoid it when it's not needed? */
	ScrollWindowEx(s_textArea, 0, -num_lines * gui.char_height,
		&rc, &rc, NULL, NULL, SW_INVALIDATE);

	/* Update gui.cursor_row if the cursor scrolled or copied over */
	if (gui.cursor_row >= row)
	{
	    if (gui.cursor_row < row + num_lines)
		gui.cursor_is_valid = FALSE;
	    else if (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_row -= num_lines;
	}
	gui_undraw_cursor();
	UpdateWindow(s_textArea);
	/* This seems to be required to avoid the cursor disappearing when
	 * scrolling such that the cursor ends up in the top-left character on
	 * the screen...   But why?  (Webb) */
	gui.cursor_is_valid = FALSE;

	gui_clear_block(gui.scroll_region_bot - num_lines + 1, 0,
	    gui.scroll_region_bot, Columns - 1);
    }
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    void
gui_mch_insert_lines(
    int	    row,
    int	    num_lines)
{
    if (num_lines <= 0)
	return;

    if (row + num_lines > gui.scroll_region_bot)
    {
	/* Scrolled out of region, just blank the lines out */
	gui_clear_block(row, 0, gui.scroll_region_bot, Columns - 1);
    }
    else
    {
	RECT	rc;

	rc.left = FILL_X(0);
	rc.right = FILL_X(Columns);
	rc.top = FILL_Y(row);
	rc.bottom = FILL_Y(gui.scroll_region_bot + 1);
	/* The SW_INVALIDATE is required when part of the window is covered or
	 * off-screen.  How do we avoid it when it's not needed? */
	ScrollWindowEx(s_textArea, 0, num_lines * gui.char_height,
		&rc, &rc, NULL, NULL, SW_INVALIDATE);

	/* Update gui.cursor_row if the cursor scrolled or copied over */
	if (gui.cursor_row >= gui.row)
	{
	    if (gui.cursor_row <= gui.scroll_region_bot - num_lines)
		gui.cursor_row += num_lines;
	    else if (gui.cursor_row <= gui.scroll_region_bot)
		gui.cursor_is_valid = FALSE;
	}

	gui_undraw_cursor();
	UpdateWindow(s_textArea);

	gui_clear_block(row, 0, row + num_lines - 1, Columns - 1);
    }
}


/*
 * Menu stuff.
 */

    void
gui_mch_enable_menu(int flag)
{
#ifdef WANT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : NULL);
#endif
}

    void
gui_mch_set_menu_pos(
    int	    x,
    int	    y,
    int	    w,
    int	    h)
{
    /* It will be in the right place anyway */
}

#ifdef WANT_MENU
/*
 * Add a sub menu to the menu bar.
 */
    void
gui_mch_add_menu(
    VimMenu	*menu,
    VimMenu	*parent,
    int		pos)
{
    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;
    menu->parent = parent;

    if (menubar_menu(menu->name))
    {
	if (is_winnt_3())
	{
	    InsertMenu((parent == NULL) ? s_menuBar : parent->submenu_id,
		    (UINT)pos, MF_POPUP | MF_STRING | MF_BYPOSITION,
		    (UINT)menu->submenu_id, (LPCTSTR) menu->name);
	}
	else
	{
	    MENUITEMINFO	info;

	    info.cbSize = sizeof(info);
	    info.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
	    info.dwItemData = (DWORD)menu;
	    info.wID = menu->id;
	    info.fType = MFT_STRING;
	    info.dwTypeData = (LPTSTR)menu->name;
	    info.cch = STRLEN(menu->name);
	    info.hSubMenu = menu->submenu_id;
	    InsertMenuItem((parent == NULL) ? s_menuBar : parent->submenu_id,
		    (UINT)pos, TRUE, &info);
	}
    }

    /* Fix window size if menu may have wrapped */
    if (parent == NULL)
	gui_w32_get_menu_height(!gui.starting);
    else if (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
}

    void
gui_mch_show_popupmenu(VimMenu *menu)
{
    POINT mp;

    if (GetCursorPos((LPPOINT)&mp))
    {
	(void)TrackPopupMenu(
		 (HMENU)menu->submenu_id,
		 TPM_LEFTALIGN | TPM_LEFTBUTTON,
		 (int)mp.x,
		 (int)mp.y,
		 (int)0,	    /*reserved param*/
		 s_hwnd,
		 NULL);
	/*
	 * NOTE: The pop-up menu can eat the mouse up event.
	 * We deal with this in normal.c.
	 */
    }
}

/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    void
gui_make_tearoff(char_u *path_name)
{
    VimMenu	**menup;
    VimMenu	*menu = NULL;
    VimMenu	*parent = NULL;
    char_u	*name;
    char_u	*saved_name;
    char_u	*p;

    menup = &root_menu;

    saved_name = vim_strsave(path_name);
    if (saved_name == NULL)
	return;

    name = saved_name;
    while (*name)
    {
	/* Find in the menu hierarchy */
	p = menu_name_skip(name);

	menu = *menup;
	while (menu != NULL)
	{
	    if (STRCMP(name, menu->name) == 0 || STRCMP(name, menu->dname) == 0)
	    {
		if (*p == NUL && menu->children == NULL)
		{
		    /* not allowed to tear off one item*/
		    EMSG("Menu path must lead to a sub-menu");
		    vim_free(saved_name);
		    return;
		}
		else if (*p != NUL && menu->children == NULL)
		{
		    EMSG("Part of menu-item path is not sub-menu");
		    vim_free(saved_name);
		    return;
		}
		break;
	    }
	    menup = &menu->next;
	    menu = menu->next;
	}

	menup = &menu->children;
	parent = menu;
	name = p;
    }
    vim_free(saved_name);
    if (menu == NULL)
    {
	EMSG("Menu not found - check menu names");
	return;
    }

    /* Found the menu, so tear it off. */
    gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}

/*
 * Add a menu item to a menu
 */
    void
gui_mch_add_menu_item(
    VimMenu	*menu,
    VimMenu	*parent,
    int		idx)
{
    menu->id = s_menu_id++;
    menu->submenu_id = NULL;
    menu->parent = parent;

    if (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	InsertMenu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
#ifdef USE_TOOLBAR
    else if (STRCMP(parent->name, "ToolBar") == 0)
    {
	TBBUTTON newtb;

	vim_memset(&newtb, 0, sizeof(newtb));
	if (is_menu_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	else
	{
	    newtb.iBitmap = get_toolbar_bitmap(menu->name);
	    newtb.fsStyle = TBSTYLE_BUTTON;
	}
	newtb.idCommand = menu->id;
	newtb.fsState = TBSTATE_ENABLED;
	newtb.iString = 0;
	SendMessage(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
#endif
    else
    {
	InsertMenu(parent->submenu_id, (UINT)idx,
		(is_menu_separator(menu->name) ? MF_SEPARATOR : MF_STRING)
							      | MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR)menu->name);
	if (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(VimMenu *menu)
{
#ifdef USE_TOOLBAR
    /*
     * is this a toolbar button?
     */
    if (menu->submenu_id == (HMENU)-1)
    {
	int iButton;

	iButton = SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    else
#endif
    {
	if (menu->parent != NULL
		&& popup_menu(menu->parent->dname)
		&& menu->parent->submenu_id != NULL)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	else
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	if (menu->submenu_id != NULL)
	    DestroyMenu(menu->submenu_id);
	if (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	if (menu->parent != NULL
		&& menu->parent->children != NULL
		&& IsWindow(menu->parent->tearoff_handle))
	    rebuild_tearoff(menu->parent);
    }
}

    static void
rebuild_tearoff(VimMenu *menu)
{
    /*hackish*/
    char_u  tbuf[128];
    RECT    trect;
    RECT    rct;
    RECT    roct;
    int	    x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, tbuf, 127);
    if (GetWindowRect(thwnd, &trect) &&
	GetWindowRect(s_hwnd, &rct) &&
	GetClientRect(s_hwnd, &roct))
    {
	x = trect.left - rct.left;
	y = (trect.top -  rct.bottom  + roct.bottom);
    }
    else
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    if (menu->children != NULL)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	if (IsWindow(menu->tearoff_handle))
	    (void) SetWindowPos(menu->tearoff_handle,
				NULL,
				(int)trect.left,
				(int)trect.top,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

/*
 * Make a menu either grey or not grey.
 */
    void
gui_mch_menu_grey(
    VimMenu *menu,
    int	    grey)
{
# ifdef USE_TOOLBAR
    /*
     * is this a toolbar button?
     */
    if (menu->submenu_id == (HMENU)-1)
    {
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
	    (WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0) );
    }
    else
# endif
    if (grey)
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_GRAYED);
    else
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_ENABLED);

#if 1
    if ((menu->parent != NULL) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	if (menu->children == NULL)
	    menuID = (WORD)(menu->id);
	else
	    menuID = (WORD)((WORD)(menu->submenu_id) | (WORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	if (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
#endif
}

/*
 * Make menu item hidden or not hidden
 */
    void
gui_mch_menu_hidden(
    VimMenu *menu,
    int	    hidden)
{
    /*
     * This doesn't do what we want.  Hmm, just grey the menu items for now.
     */
    /*
    if (hidden)
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_DISABLED);
    else
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_ENABLED);
    */
    gui_mch_menu_grey(menu, hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    void
gui_mch_draw_menubar(void)
{
    DrawMenuBar(s_hwnd);
}
#endif /* WANT_MENU */

/* cproto doesn't create a prototype for main() */
int main __ARGS((int argc, char **argv));

#ifndef PROTO
void
#ifdef VIMDLL
_export
#endif
_cdecl
SaveInst(HINSTANCE hInst)
{
    s_hinst = hInst;
}
#endif

/*
 * Return the lightness of a pixel.  White is 255.
 */
    int
gui_mch_get_lightness(pixel)
    GuiColor	pixel;
{
    return (GetRValue(pixel)*3 + GetGValue(pixel)*6 + GetBValue(pixel)) / 10;
}

#if (defined(SYNTAX_HL) && defined(WANT_EVAL)) || defined(PROTO)
/*
 * Return the RGB value of a pixel as "#RRGGBB".
 */
    char_u *
gui_mch_get_rgb(
    GuiColor	pixel)
{
    static char_u retval[10];

    sprintf((char *)retval, "#%02x%02x%02x",
	    GetRValue(pixel), GetGValue(pixel), GetBValue(pixel));
    return retval;
}
#endif

#ifdef USE_BROWSE

/*
 * Pop open a file browser and return the file selected, in allocated memory,
 * or NULL if Cancel is hit.
 *  saving  - TRUE if the file will be saved to, FALSE if it will be opened.
 *  title   - Title message for the file browser dialog.
 *  dflt    - Default name of file.
 *  ext     - Default extension to be added to files without extensions.
 *  initdir - directory in which to open the browser (NULL = current dir)
 *  filter  - Filter for matched files to choose from.
 *	Has a format like this:
 *	"C Files (*.c)\0*.c\0"
 *	"All Files\0*.*\0\0"
 *	If these two strings were concatenated, then a choice of two file
 *	filters will be selectable to the user.  Then only matching files will
 *	be shown in the browser.  If NULL, the default allows all files.
 *
 *	*NOTE* - the filter string must be terminated with TWO nulls.
 */
    char_u *
gui_mch_browse(
	int saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    OPENFILENAME    fileStruct;
    char_u	    fileBuf[MAX_PATH], *p;

    if (dflt == NULL)
	fileBuf[0] = '\0';
    else
    {
	STRNCPY(fileBuf, dflt, MAX_PATH - 1);
	fileBuf[MAX_PATH - 1] = NUL;
    }

    /*
     * The default filter. NOTE: should we perhaps put this in
     * feature.h?
     */
    if (filter == NULL)
	filter =
	    "All Files (*.*)\0*.*\0"
	    "C source (*.c, *.h)\0*.c;*.h\0"
	    "C++ source (*.cpp, *.hpp)\0*.cpp;*.hpp\0"
	    "VB code (*.bas, *.frm)\0*.bas;*.frm\0"
	    "Vim files (*.vim, _vimrc, _gvimrc)\0*.vim;_vimrc;_gvimrc\0\0";

    memset(&fileStruct, 0, sizeof(OPENFILENAME));
    fileStruct.lStructSize = sizeof(OPENFILENAME);
    fileStruct.lpstrFilter = filter;
    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAX_PATH;
    fileStruct.lpstrTitle = title;
    fileStruct.lpstrDefExt = ext;
    fileStruct.hwndOwner = s_hwnd;		/* main Vim window is owner*/
    /* has an initial dir been specified? */
    if (initdir != NULL && *initdir != NUL)
	fileStruct.lpstrInitialDir = initdir;

    /*
     * TODO: Allow selection of multiple files.  Needs another arg to this
     * function to ask for it, and need to use OFN_ALLOWMULTISELECT below.
     * Also, should we use OFN_FILEMUSTEXIST when opening?  Vim can edit on
     * files that don't exist yet, so I haven't put it in.  What about
     * OFN_PATHMUSTEXIST?
     * Don't use OFN_OVERWRITEPROMPT, Vim has its own ":confirm" dialog.
     */
    fileStruct.Flags = (OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY);
    if (saving)
    {
	if (!GetSaveFileName(&fileStruct))
	    return NULL;
    }
    else
    {
	if (!GetOpenFileName(&fileStruct))
	    return NULL;
    }

    /* Shorten the file name if possible */
    mch_dirname(IObuff, IOSIZE);
    p = shorten_fname(fileBuf, IObuff);
    if (p == NULL)
	p = fileBuf;
    return vim_strsave(p);
}

#endif /* USE_BROWSE */

/* Convert pixels in X to dialog units */
static WORD
PixelToDialogX(int numPixels)
{
    return (WORD)((numPixels * 4) / s_dlgfntwidth);
}

/* Convert pixels in Y to dialog units */
static WORD
PixelToDialogY(int numPixels)
{
    return (WORD)((numPixels * 8) / s_dlgfntheight);
}

/* Return the width in pixels of the given text in the given DC. */
static int
GetTextWidth(HDC hdc, char_u *str, int len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, str, len, &size);
    return size.cx;
}

#if defined(GUI_DIALOG) || defined(PROTO)
/*
 * stuff for dialogs
 */

/*
 * The callback routine used by all the dialogs.  Very simple.  First,
 * acknowledges the INITDIALOG message so that Windows knows to do standard
 * dialog stuff (Return = default, Esc = cancel....) Second, if a button is
 * pressed, return that button's ID - IDCANCEL (2), which is the button's
 * number.
 */
    static LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    if (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	/* Set focus to the dialog.  Set the default button, if specified. */
	(void)SetFocus(hwnd);
	if (dialog_default_button > 0)
	    (void)SetFocus(GetDlgItem(hwnd, dialog_default_button + IDCANCEL));
	return FALSE;
    }

    if (message == WM_COMMAND)
    {
	int	button = LOWORD(wParam);

	/*
	 * Need to check for IDOK because if the user just hits Return to
	 * accept the default value, some reason this is what we get.
	 */
	if (button == IDOK)
	    EndDialog(hwnd, dialog_default_button);
	else
	    EndDialog(hwnd, button - IDCANCEL);
	return TRUE;
    }
    if ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	return TRUE;
    }
    return FALSE;
}

/*
 * Create a dialog dynamically from the parameter strings.
 * type		= type of dialog (question, alert, etc.)
 * title	= dialog title. may be NULL for default title.
 * message	= text to display. Dialog sizes to accommodate it.
 * buttons	= '\n' separated list of button captions, default first.
 * dfltbutton	= number of default button.
 *
 * This routine returns 1 if the first button is pressed,
 *			2 for the second, etc.
 *
 *			0 indicates Esc was pressed.
 *			-1 for unexpected error
 *
 * If stubbing out this fn, return 1.
 */

static const char_u *dlg_icons[] = /* must match names in resource file */
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    int
gui_mch_dialog(
    int		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    int		 dfltbutton)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    int		numButtons;
    int		*buttonWidths, *buttonPositions;
    int		nchar, i;
    DWORD	lStyle;
    int		dlgwidth = 0;
    int		horizWidth;
    int		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    int		fontHeight;
    int		textWidth, minButtonWidth, messageWidth;
    int		maxDialogWidth;
    int		vertical;
    int		dlgPaddingX;
    int		dlgPaddingY;

#ifndef NO_CONSOLE
    /* Don't output anything in silent mode ("ex -s") */
    if (silent_mode)
	return dfltbutton;   /* return default option */
#endif

    /* If there is no window yet, open it. */
    if (s_hwnd == NULL && gui_mch_init() == FAIL)
	return dfltbutton;

    if ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    /* allocate some memory for dialog template */
    /* TODO should compute this really*/
    pdlgtemplate = p = (PWORD) LocalAlloc(LPTR, DLG_ALLOC_SIZE);

    if (p == NULL)
	return -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  complier grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    if (tbuffer == NULL)
	return -1;

    --dfltbutton;   /* Change from one-based to zero-based */

    /* Count buttons */
    numButtons = 1;
    for (i = 0; tbuffer[i] != '\0'; i++)
    {
	if (tbuffer[i] == DLG_BUTTON_SEP)
	    numButtons++;
    }
    if (dfltbutton >= numButtons)
	dfltbutton = 0;

    /* Allocate array to hold the width of each button */
    buttonWidths = (int *) lalloc(numButtons * sizeof(int), TRUE);
    if (buttonWidths == NULL)
	return -1;

    /* Allocate array to hold the X position of each button */
    buttonPositions = (int *) lalloc(numButtons * sizeof(int), TRUE);
    if (buttonPositions == NULL)
	return -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
    font = CreateFont(-DLG_FONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		      VARIABLE_PITCH , DLG_FONT_NAME);
    if (s_usenewlook)
    {
	oldFont = SelectFont(hdc, font);
	dlgPaddingX = DLG_PADDING_X;
	dlgPaddingY = DLG_PADDING_Y;
    }
    else
    {
	oldFont = SelectFont(hdc, GetStockObject(SYSTEM_FONT));
	dlgPaddingX = DLG_OLD_STYLE_PADDING_X;
	dlgPaddingY = DLG_OLD_STYLE_PADDING_Y;
    }
    GetTextMetrics(hdc, &fontInfo);
    fontHeight = fontInfo.tmHeight;

    /* Minimum width for horizontal button */
    minButtonWidth = GetTextWidth(hdc, "Cancel", 6);

    /* Maximum width of a dialog, if possible */
    GetWindowRect(s_hwnd, &rect);
    maxDialogWidth = rect.right - rect.left
		     - GetSystemMetrics(SM_CXSIZEFRAME) * 2;
    if (maxDialogWidth < DLG_MIN_MAX_WIDTH)
	maxDialogWidth = DLG_MIN_MAX_WIDTH;

    /* Set dlgwidth to width of message */
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    do
    {
	pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	if (pend == NULL)
	    pend = pstart + STRLEN(pstart);	/* Last line of message. */
	msgheight += fontHeight;
	textWidth = GetTextWidth(hdc, pstart, pend - pstart);
	if (textWidth > messageWidth)
	    messageWidth = textWidth;
	pstart = pend + 1;
    } while (*pend != NUL);
    dlgwidth = messageWidth;

    /* Add width of icon to dlgwidth, and some space */
    dlgwidth += DLG_ICON_WIDTH + 3 * dlgPaddingX;

    if (msgheight < DLG_ICON_HEIGHT)
	msgheight = DLG_ICON_HEIGHT;

    /*
     * Check button names.  A long one will make the dialog wider.
     */
    vertical = (vim_strchr(p_go, GO_VERTICAL) != NULL);
    if (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	do
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    if (pend == NULL)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidth(hdc, pstart, pend - pstart);
	    if (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    /* Padding within button */
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; /* Pad between buttons */
	    pstart = pend + 1;
	} while (*pend != NUL);

	if (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	else if (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    if (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	do
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    if (pend == NULL)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidth(hdc, pstart, pend - pstart);
	    textWidth += dlgPaddingX;		/* Padding within button */
	    textWidth += DLG_VERT_PADDING_X * 2; /* Padding around button */
	    if (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} while (*pend != NUL);
    }

    if (dlgwidth < DLG_MIN_WIDTH)
	dlgwidth = DLG_MIN_WIDTH;	/* Don't allow a really thin dialog!*/

    /* start to fill in the dlgtemplate information.  addressing by WORDs */
    if (s_usenewlook)
	lStyle = DS_MODALFRAME | WS_CAPTION |DS_3DLOOK| WS_VISIBLE |DS_SETFONT;
    else
	lStyle = DS_MODALFRAME | WS_CAPTION |DS_3DLOOK| WS_VISIBLE;

    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    pnumitems = p;	/*save where the number of items must be stored*/
    *p++ = 0;		// NumberOfItems(will change later)
    *p++ = 10;		// x
    *p++ = 10;		// y
    *p++ = PixelToDialogX(dlgwidth);	// cx

    // Dialog height.
    if (vertical)
	*p++ = PixelToDialogY(msgheight + 2 * dlgPaddingY +
			      DLG_VERT_PADDING_Y + 2 * fontHeight * numButtons);
    else
	*p++ = PixelToDialogY(msgheight + 3 * dlgPaddingY + 2 * fontHeight);

    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    /* copy the title of the dialog */
    nchar = nCopyAnsiToWideChar(p, (title ?
				    (LPSTR)title :
				    (LPSTR)("Vim "VIM_VERSION_MEDIUM)));
    p += nchar;

    if (s_usenewlook)
    {
	/* do the font, since DS_3DLOOK doesn't work properly */
	*p++ = DLG_FONT_POINT_SIZE;		//point size
	nchar = nCopyAnsiToWideChar (p, TEXT(DLG_FONT_NAME));
	p += nchar;
    }

    pstart = tbuffer;
    horizWidth = (dlgwidth - horizWidth) / 2;	/* Now it's X offset */
    for (i = 0; i < numButtons; i++)
    {
	/* get end of this button. */
	for (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	if (*pend)
	    *pend = '\0';

	/*
	 * NOTE:
	 * setting the BS_DEFPUSHBUTTON style doesn't work because Windows sets
	 * the focus to the first tab-able button and in so doing makes that
	 * the default!! Grrr.  Workaround: Make the default button the only
	 * one with WS_TABSTOP style. Means user can't tab between buttons, but
	 * he/she can use arrow keys.
	 */
	if (vertical)
	{
	    p = add_dialog_element(p,
		    BS_PUSHBUTTON | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(msgheight + 2 * dlgPaddingY
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, pstart);
	}
	else
	{
	    p = add_dialog_element(p,
		    BS_PUSHBUTTON | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(msgheight + 2 * dlgPaddingY),
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, pstart);
	}
	pstart = pend + 1;	/*next button*/
    }
    *pnumitems += numButtons;

    /* Vim icon */
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(DLG_ICON_WIDTH),
	    PixelToDialogY(DLG_ICON_HEIGHT),
	    0, (WORD)0x0082, dlg_icons[type]);

    /* Dialog message */
    p = add_dialog_element(p, SS_LEFT,//SS_CENTER,
	    PixelToDialogX(2 * dlgPaddingX + DLG_ICON_WIDTH),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    1, (WORD)0x0082, message);

    *pnumitems += 2;

    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /* Let the dialog_callback() function know which button to make default */
    dialog_default_button = dfltbutton + 1;	/* Back to 1-based for this */

    /* show the dialog box modally and get a return value */
    nchar = DialogBoxIndirect(
	    s_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);

    return nchar;
}
#endif /* GUI_DIALOG */

/*
 * Put a simple element (basic class) onto a dialog template in memory.
 * return a pointer to where the next item shoudl be added.
 *
 * parameters:
 *  lStyle = additional style flags
 *		(be careful, NT3.51 & Win32s will ignore the new ones)
 *  x,y = x & y positions IN DIALOG UNITS
 *  w,h = width and height IN DIALOG UNITS
 *  Id  = ID used in messages
 *  clss  = class ID, e.g 0x0080 for a button, 0x0082 for a static
 *  caption = usually text or resource name
 *
 *  TODO: use the length information noted here to enable the dialog creation
 *  routines to work out more exactly how much memory they need to alloc.
 */
    static PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    const char *caption)
{
    int nchar;

    p = lpwAlign(p);	/* Align to dword boundary*/
    lStyle = lStyle | WS_VISIBLE | WS_CHILD;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (WORD)0xffff;
    *p++ = clss;			//2 more here

    nchar = nCopyAnsiToWideChar(p, (LPSTR)caption); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    return p;	//total = 15+ (strlen(caption)) words
		//	   = 30 + 2(strlen(caption) bytes reqd
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    static LPWORD
lpwAlign(
    LPWORD lpIn)
{
    ULONG ul;

    ul = (ULONG)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    return (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 */
    static int
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn)
{
    int nChar = 0;

#ifdef MULTI_BYTE
    int len = lstrlen(lpAnsiIn) + 1;	/* include NUL character */
    int i;

    nChar = MultiByteToWideChar(
	    CP_OEMCP,
	    MB_PRECOMPOSED,
	    lpAnsiIn, len,
	    lpWCStr, len);
    for (i = 0; i < nChar; ++i)
	if (lpWCStr[i] == (WORD)'\t')	/* replace tabs with spaces */
	    lpWCStr[i] = (WORD)' ';
#else
    do
    {
	if (*lpAnsiIn == '\t')
	    *lpWCStr++ = (WORD)' ';
	else
	    *lpWCStr++ = (WORD)*lpAnsiIn;
	nChar++;
    } while (*lpAnsiIn++);
#endif

    return nChar;
}

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples.
 */
    static BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    RECT    rChild, rParent;
    int     wChild, hChild, wParent, hParent;
    int     wScreen, hScreen, xNew, yNew;
    HDC     hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.right - rChild.left;
    hChild = rChild.bottom - rChild.top;

    GetWindowRect(hwndParent, &rParent);
    wParent = rParent.right - rParent.left;
    hParent = rParent.bottom - rParent.top;

    hdc = GetDC(hwndChild);
    wScreen = GetDeviceCaps (hdc, HORZRES);
    hScreen = GetDeviceCaps (hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    xNew = rParent.left + ((wParent - wChild) /2);
    if (xNew < 0)
    {
	xNew = 0;
    }
    else if ((xNew+wChild) > wScreen)
    {
	xNew = wScreen - wChild;
    }

    yNew = rParent.top  + ((hParent - hChild) /2);
    if (yNew < 0)
	yNew = 0;
    else if ((yNew+hChild) > hScreen)
	yNew = hScreen - hChild;

    return SetWindowPos(hwndChild, NULL, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    static LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    if (message == WM_INITDIALOG)
	return (TRUE);

    if (message == WM_COMMAND)
    {
	if ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;
	    if (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect));
	    {
		(void)TrackPopupMenu(
			 (HMENU)(LOWORD(wParam) ^ 0x8000),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (int)rect.right - 8,
			 (int)mp.y,
			 (int)0,	    /*reserved param*/
			 s_hwnd,
			 NULL);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	else
	    /* Pass on messages to the main Vim window */
	    PostMessage (s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(void)SetFocus(s_hwnd);
	return TRUE;
    }
    if ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	return TRUE;
    }

    /* When moved around, give main window the focus back. */
    if (message == WM_EXITSIZEMOVE)
	(void)SetActiveWindow(s_hwnd);

    return FALSE;
}

void
gui_mch_activate_window(void)
{
    (void)SetActiveWindow(s_hwnd);
}

#ifdef WANT_MENU
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    static void
gui_mch_tearoff(
    char_u	*title,
    VimMenu	*menu,
    int		initX,
    int		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    int		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    VimMenu	*pmenu;
    VimMenu	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    int		col, spaceWidth, len;
    int		columnWidths[2];
    char_u	*label, *text, *end, *acEnd;
    int		padding0, padding1, padding2;
    int		sepPadding=0;

    /*
     * If this menu is already torn off, then don't
     * tear it off again, but move the existing tearoff
     * to the mouse position.
     */

    if (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	if (GetCursorPos((LPPOINT)&mp))
	{
	    SetWindowPos(menu->tearoff_handle, NULL, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	return;
    }

    /*
     * Otherwise, create a new tearoff
     */

    if (*title == MNU_HIDDEN_CHAR)
	title++;

    /* allocate some memory to play with  */
    /* TODO should compute this really    */
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,  DLG_ALLOC_SIZE);
    if (p == NULL)
	return;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
    font = CreateFont(-DLG_FONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		      VARIABLE_PITCH , DLG_FONT_NAME);
    if (s_usenewlook)
	oldFont = SelectFont(hdc, font);
    else
	oldFont = SelectFont(hdc, GetStockObject(SYSTEM_FONT));
    /*
     * Calculate width of a single space.  Used for padding columns to the
     * right width.
     */
    spaceWidth = GetTextWidth(hdc, " ", 1);

    submenuWidth = 0;
    /* Figure out widths for each column. */
    for (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	for (pmenu = menu->children; pmenu != NULL; pmenu = pmenu->next)
	{
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    if (pmenu->children != NULL)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	    if (text != NULL && *text != NUL)
	    {
		end = text + strlen(text);
		textWidth = GetTextWidth(hdc, text, end - text);
		if (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	}
    }
    if (columnWidths[1] == 0)
    {
	if (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	else
	    columnWidths[0] += spaceWidth;
    }
    else
    {
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }
    /*
     * Now find the width of our 'menu'.
     */
    textWidth = 0;
    for (col = 0; col < 2; col++)
	textWidth += columnWidths[col];
    if (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, TEAROFF_SUBMENU_LABEL,
			      STRLEN(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidth(hdc, title, strlen(title));
    if (textWidth > dlgwidth)
	dlgwidth = textWidth;
    dlgwidth += 2 * TEAROFF_PADDING_X + TEAROFF_BUTTON_PAD_X;

    /* W95 can't do thin dialogs, they look v. weird! */
    if (mch_windows95() && dlgwidth < TEAROFF_MIN_WIDTH)
	dlgwidth = TEAROFF_MIN_WIDTH;

    /* start to fill in the dlgtemplate information.  addressing by WORDs */
    if (s_usenewlook)
	lStyle = DS_MODALFRAME | WS_CAPTION| WS_SYSMENU |DS_SETFONT| WS_VISIBLE;
    else
	lStyle = DS_MODALFRAME | WS_CAPTION| WS_SYSMENU | WS_VISIBLE;

    lExtendedStyle = WS_EX_TOOLWINDOW|WS_EX_STATICEDGE;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = LOWORD (lExtendedStyle);
    *p++ = HIWORD (lExtendedStyle);
    pnumitems = p;	/* save where the number of items must be stored */
    *p++ = 0;		// NumberOfItems(will change later)
    if (initX == 0xffffL)
	*p++ = PixelToDialogX(gui_mch_get_mouse_x()); // x
    else
	*p++ = PixelToDialogX(initX); // x
    if (initY == 0xffffL)
	*p++ = PixelToDialogY(gui_mch_get_mouse_y()); // y
    else
	*p++ = PixelToDialogY(initY); // y
    *p++ = PixelToDialogX(dlgwidth);    // cx
    ptrueheight =p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    /* copy the title of the dialog */
    nchar = nCopyAnsiToWideChar(p, ((*title) ?
				    (LPSTR)title :
				    (LPSTR)("Vim "VIM_VERSION_MEDIUM)));
    p += nchar;

    if (s_usenewlook)
    {
	/* do the font, since DS_3DLOOK doesn't work properly */
	*p++ = DLG_FONT_POINT_SIZE;		//point size
	nchar = nCopyAnsiToWideChar (p, TEXT(DLG_FONT_NAME));
	p += nchar;
    }

    /* Don't include tearbar in tearoff menu */
    if (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    else
	menu = menu->children;

    for ( ; menu != NULL; menu = menu->next)
    {
	if (is_menu_separator(menu->dname))
	{
	    sepPadding += 3;
	    continue;
	}

	/* Figure out length of this menu label */
	len = STRLEN(menu->dname);
	end = menu->dname + STRLEN(menu->dname);
	padding0 = (columnWidths[0] - GetTextWidth(hdc, menu->dname,
		    end - menu->dname)) / spaceWidth;
	len += padding0;
	if (menu->actext != NULL)
	{
	    len += STRLEN(menu->actext);
	    acEnd = menu->actext + STRLEN(menu->actext);
	}
	if (menu->actext != NULL)
	    textWidth = GetTextWidth(hdc, menu->actext,
				     acEnd - menu->actext);
	else
	    textWidth = 0;

	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;
	if (menu->children == NULL)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	else
	{
	    len += STRLEN(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((WORD)(menu->submenu_id) | (WORD)0x8000);
	}

	/* Allocate menu label and fill it in */
	text = label = alloc((unsigned)len + 1);
	if (label == NULL)
	    break;
	STRNCPY(text, menu->dname, end - menu->dname);
	text += end - menu->dname;
	while (padding0-- > 0)
	    *text++ = ' ';
	if (menu->actext != NULL)
	{
	    STRNCPY(text, menu->actext, acEnd - menu->actext);
	    text += acEnd - menu->actext;
	}
	while (padding1-- > 0)
	    *text++ = ' ';
	if (menu->children != NULL)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN(TEAROFF_SUBMENU_LABEL);
	}
	else
	{
	    while (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;
	*end = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	    p = add_dialog_element(p,
		    BS_PUSHBUTTON|BS_LEFT,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		    (WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, label);
	vim_free(label);
	    (*pnumitems)++;
    }

    *ptrueheight = (WORD)(sepPadding + 1 + 13 * (*pnumitems));


    /* show modelessly */
    the_menu->tearoff_handle = CreateDialogIndirect(
	    s_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)tearoff_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing agin!
     */
    (void)SetActiveWindow(s_hwnd);

    /* make sure the right buttons are enabled */
    force_menu_update = TRUE;
}
#endif

/*
 * Decide whether to use the "new look" (small, non-bold font) or the "old
 * look" (big, clanky font) for dialogs, and work out a few values for use
 * later accordingly.
 */
    static void
get_dialog_font_metrics(void)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    DWORD	    dlgFontSize;
    SIZE	    size;

    s_usenewlook = FALSE;

    /*
     * For NT3.51 and Win32s, we stick with the old look
     * because it matches everything else.
     */
    if (!is_winnt_3())
    {
	hfontTools = CreateFont(-DLG_FONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH , DLG_FONT_NAME);

	if (hfontTools)
	{
	    hdc = GetDC (s_hwnd);
	    SelectObject (hdc, hfontTools);
	    /*
	     * GetTextMetrics() doesn't return the right value in
	     * tmAveCharWidth, so we have to figure out the dialog base units
	     * ourselves.
	     */
	    GetTextExtentPoint(hdc,
		    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
		    52, &size);
	    ReleaseDC (s_hwnd, hdc);

	    s_dlgfntwidth = (WORD)((size.cx / 26 + 1) / 2);
	    s_dlgfntheight = (WORD)size.cy;
	    s_usenewlook = TRUE;
	}
    }

    if (!s_usenewlook)
    {
	dlgFontSize = GetDialogBaseUnits();	/* fall back to big old system*/
	s_dlgfntwidth = LOWORD(dlgFontSize);
	s_dlgfntheight = HIWORD(dlgFontSize);
    }
}

#if defined(USE_TOOLBAR) || defined(PROTO)
#include "gui_w32_rc.h"

/* This not defined in older SDKs */
# ifndef TBSTYLE_FLAT
#  define TBSTYLE_FLAT            0x0800
# endif

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    static void
initialise_toolbar(void)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    28,			//number of images in inital bitmap
		    s_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    NULL,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    sizeof(TBBUTTON)
		    );

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != NULL);
}

    void
gui_mch_show_toolbar(int showit)
{
    if (s_toolbarhwnd == NULL)
	return;

    if (showit)
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    else
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}
/*
 * Find a bitmap by name.
 * If the name is one of the built-in bitmaps, return the index to that bitmap.
 * Otherwise, look for $vim/bitmaps/<name>.bmp, load it, add it to the toolbar
 * bitmap list and return the resulting index on failure return -1, which will
 * result in a blank (but still functional) button
 */
static const char_u *BuiltInBitmaps[] =
{
    "New",		//0
    "Open",		//1
    "Save",		//2
    "Undo",		//3
    "Redo",		//4
    "Cut",		//5
    "Copy",		//6
    "Paste",		//7
    "Print",		//8
    "Help",		//9
    "Find",		//10
    "SaveAll",		//11
    "SaveSesn",		//12
    "NewSesn",		//13
    "LoadSesn",		//14
    "RunScript",	//15
    "Replace",		//16
    "WinClose",		//17
    "WinMax",		//18
    "WinMin",		//19
    "WinSplit",		//20
    "Shell",		//21
    "FindPrev",		//22
    "FindNext",		//23
    "FindHelp",		//24
    "Make",		//25
    "TagJump",		//26
    "RunCtags",		//27
    NULL
};
    static int
get_toolbar_bitmap(char_u *name)
{
    int i;

    if (STRNCMP(name, "BuiltIn", 7) == 0)
    {
	char_u *dummy;
	/*
	 * reference by index
	 */
	i = strtol(name + 7, &dummy, 0);
	return i;
    }
    /*
     * Check user bitmaps next
     */
    i = -1;
    if (!is_winnt_3())
    {
	char_u *fname;
	char_u *ffname;
	HANDLE hbitmap;

	fname = alloc(_MAX_PATH);
	STRCPY(fname, "$VIMRUNTIME\\bitmaps\\");
	strcat(fname, name);
	strcat(fname, ".bmp");
	ffname = expand_env_save(fname);

	hbitmap = LoadImage(
		    NULL,
		    ffname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);
	if (hbitmap != NULL)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = NULL;
	    tbAddBitmap.nID = (UINT)hbitmap;

	    i = SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    /* i will be set to -1 if it fails */

	}
	vim_free(fname);
	vim_free(ffname);
    }
    if (i != -1)
	return i;

    for (i = 0; BuiltInBitmaps[i]; i++)
    {
	if (STRCMP(name, BuiltInBitmaps[i]) == 0)
	    return i;
    }
    return i;
}
#endif


    void
gui_simulate_alt_key(char_u *keys)
{
    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    while (*keys)
    {
	if (*keys == '~')
	    *keys = ' ';	    /* for showing system menu */
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*keys, (LPARAM)0);
	keys++;
    }
}

/*
 * Create the find & replace dialogs
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first.
 */

    void
gui_mch_find_dialog(char_u *arg)
{
#ifdef WIN32_FIND_REPLACE
    if (s_findrep_msg != 0)
    {
	if (IsWindow(s_findrep_hwnd) && (s_findrep_is_find == FALSE))
	    DestroyWindow(s_findrep_hwnd);

	if (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(arg);
	    s_findrep_hwnd = FindText((LPFINDREPLACE) &s_findrep_struct);
	}

	(void)SetWindowText(s_findrep_hwnd,
			    (LPCSTR) "Find string (use '\\\\' to find  a '\\')");
	(void)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}

    void
gui_mch_replace_dialog(char_u *arg)
{
#ifdef WIN32_FIND_REPLACE
    if (s_findrep_msg != 0)
    {
	if (IsWindow(s_findrep_hwnd) && (s_findrep_is_find == TRUE))
	    DestroyWindow(s_findrep_hwnd);

	if (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(arg);
	    s_findrep_hwnd = ReplaceText((LPFINDREPLACE) &s_findrep_struct);
	}

	(void)SetWindowText(s_findrep_hwnd,
			    (LPCSTR) "Find & Replace (use '\\\\' to find  a '\\')");
	(void)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}

#ifdef WIN32_FIND_REPLACE
    static void
initialise_findrep(char_u *initial_string)
{
    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    if (p_ic)
	s_findrep_struct.Flags &= ~FR_MATCHCASE;
    else
	s_findrep_struct.Flags |= FR_MATCHCASE;
    if (initial_string != NULL && *initial_string != NUL)
    {
	STRCPY(s_findrep_struct.lpstrFindWhat, initial_string);
	s_findrep_struct.lpstrReplaceWith[0] = NUL;
    }
}
#endif

/*
 * Set visibility of the pointer.
 */
    void
gui_mch_mousehide(int hide)
{
    if (hide != gui.pointer_hidden)
    {
	ShowCursor(!hide);
	gui.pointer_hidden = hide;
    }
}
