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

#define DEADKEYSPATCH	/* include patch for typing ' ' after a dead key */

#include "vim.h"

#include <windows.h>
#include <shellapi.h>

#ifdef WIN32
# include <windowsx.h>
#else /* WIN16 */
# include <windowsx.h16>
#endif

#ifdef PROTO
/*
 * Define a few things for generating prototypes.  This is just to avoid
 * syntax errors, the defines do not need to be correct.
 */
# define HINSTANCE  void *
# define HWND	    void *
# define HMENU	    void *
# define UINT	    int
# define WPARAM	    int
# define LPARAM	    int
typedef int LOGFONT[];
# define ENUMLOGFONT int
# define NEWTEXTMETRIC int
# define VOID	    void
# define CALLBACK
# define DWORD	    int
# define HDROP	    int
# define BOOL	    int
# define LPRECT	    int
# define LRESULT    int
# define WINAPI
# define LPSTR	    int
# define LPWINDOWPOS int
# define RECT	    int
# define LPCREATESTRUCT int
# define _cdecl
#endif

/* For the Intellimouse: */
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL	0x20a
#endif

/* Local variables: */
static HINSTANCE    s_hinst = NULL;
static HWND	    s_hwnd = NULL;
static HWND	    s_textArea = NULL;
static HMENU	    s_menuBar = NULL;
static UINT	    s_menu_id = 0;
static UINT	    s_wait_timer = 0;	/* Timer for get char from user */
static int	    destroying = FALSE;	/* calling DestroyWindow() ourselves */

static UINT	    s_uMsg = 0;
static WPARAM	    s_wParam = 0;
static LPARAM	    s_lParam = 0;

static int	    s_timed_out = FALSE;

static const LOGFONT s_lfDefault =
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

#ifdef DEADKEYSPATCH
static int dead_key = 0;	/* 0 - no dead key, 1 - dead key pressed */
#endif

/*
 * The scrollbar stuff can handle only up to 32767 lines.  When the file is
 * longer, scroll_shift is set to the number of shifts to reduce the count.
 */
static int scroll_shift = 0;

static int mouse_scroll_lines = 0;

#ifdef DEBUG
/* Print out the last Windows error message */
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
    static OSVERSIONINFO ovi;

    if (ovi.dwOSVersionInfoSize != sizeof(ovi))
    {
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);
    }
    return ((ovi.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& ovi.dwMajorVersion == 3)
	    || (ovi.dwPlatformId == VER_PLATFORM_WIN32s));
}

/*
 * Return TRUE when running under Win32s.
 */
    int
gui_is_win32s(void)
{
    static OSVERSIONINFO ovi;

    if (ovi.dwOSVersionInfoSize != sizeof(ovi))
    {
	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);
    }

    return (ovi.dwPlatformId == VER_PLATFORM_WIN32s);
}

/*
 * Figure out how high the menu bar is at the moment.
 */
    static int
gui_w32_get_menu_height(
    int	    fix_window)	    /* If TRUE, resize window if menu height changed */
{
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
	    RECT r1, r2;
	    int frameht = GetSystemMetrics(SM_CYFRAME);
	    int capht = GetSystemMetrics(SM_CYCAPTION);

	    /*	get window rect of s_hwnd
		get client rect of s_hwnd
		get cap height
		subtract from window rect, the sum of client height,
		(if not maximized)frame thickness, and caption height.
	     */
	    GetWindowRect(s_hwnd, &r1);
	    GetClientRect(s_hwnd, &r2);
	    menu_height = r1.bottom - r1.top - (r2.bottom-r2.top +
				   2 * frameht * (!IsZoomed(s_hwnd)) + capht);
	}
	else			/* win95 and variants (NT 4.0, I guess) */
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	    GetMenuItemRect(s_hwnd, s_menuBar, num - 1, &rc2);
	    menu_height = rc2.bottom - rc1.top + 1;
	}
    }

    if (fix_window && menu_height != old_menu_height)
	gui_set_winsize(FALSE);

    old_menu_height = menu_height;
    return menu_height;
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
    /* Only exit when there are no changed files */
    exiting = TRUE;
    if (!check_changed_any())	    /* will give warning for changed buffer */
	getout(0);

    exiting = FALSE;
    setcursor();		    /* position cursor */
    out_flush();
}

/*
 * Get a message when the the window is being destroyed.
 */
    static void
_OnDestroy(
    HWND hwnd)
{
    if (!destroying)
	_OnClose(hwnd);
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

    /* TRACE("_OnDropFiles: %d files dropped\n", cFiles); */

    /* reset_VIsual(); */
    if (VIsual_active)
    {
	end_visual_mode();
	update_curbuf(NOT_VALID);	/* delete the inversion */
    }

    fnames = (char_u **) alloc(cFiles * sizeof(char_u *));

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

    /* Handle the drop, by resetting the :args list */
    handle_drop(cFiles, fnames);

    /* Update the screen display */
    update_screen(NOT_VALID);
    setcursor();
    out_flush();
}

#ifdef DEADKEYSPATCH
    static void
_OnDeadChar(
    HWND hwnd,
    UINT ch,
    int cRepeat)
{
    dead_key = 1;
}
#endif

    static void
_OnChar(
    HWND hwnd,
    UINT ch,
    int cRepeat)
{
    char_u	string[1];

    /* TRACE("OnChar(%d, %c)\n", ch, ch); */

    string[0] = ch;
    if (string[0] == Ctrl('C'))
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

    /*
     * While button is down, keep grabbing mouse move events when the mouse
     * goes outside the window
     */
    SetCapture(s_textArea);

    if (s_uMsg == WM_LBUTTONDOWN || s_uMsg == WM_LBUTTONDBLCLK)
	button = MOUSE_LEFT;
    else if (s_uMsg == WM_MBUTTONDOWN || s_uMsg == WM_MBUTTONDBLCLK)
	button = MOUSE_MIDDLE;
    else if (s_uMsg == WM_RBUTTONDOWN || s_uMsg == WM_RBUTTONDBLCLK)
	button = MOUSE_RIGHT;

    if (button >= 0)
    {
	repeated_click = ((int)(currentTime - s_prevTime) < p_mouset);

	/*
	 * TODO: Fix this, it doesn't work:
	 */
	/*
	 * Holding down the left and right buttons simulates pushing the middle
	 * button.  Hmm, this doesn't really work because we have already
	 * passed through the first click as a left or right button click.
	 */
	if (repeated_click &&
	    ((button == MOUSE_LEFT && (keyFlags & MK_RBUTTON)) ||
	     (button == MOUSE_RIGHT && (keyFlags & MK_LBUTTON))))
	{
	    /*
	     * Hmm, gui.c will ignore more than one button down at a time, so
	     * pretend we let go of it first.
	     */
	    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	    button = MOUSE_MIDDLE;
	    repeated_click = FALSE;
	}

	s_prevTime = currentTime;
	/* TRACE("Button down at x %d, y %d\n", x, y); */
	_OnMouseEvent(button, x, y, repeated_click, keyFlags);
    }
}

    static void
_OnMouseMoveOrRelease(
    HWND hwnd,
    int x,
    int y,
    UINT keyFlags)
{
    int button;

    if (s_uMsg == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	if (!(keyFlags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON)))
	    return;

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

/*
 * Find the GuiMenu with the given id
 */
	static GuiMenu *
gui_w32_find_menu(
	GuiMenu * pMenu,
	int id)
{
	GuiMenu * pChildMenu;
	while(pMenu) {
		if(pMenu->id == (UINT)id) /* && pMenu->submenu_id == NULL) */
			break;
		if(pMenu->children != NULL) {
			pChildMenu = gui_w32_find_menu(pMenu->children, id);
			if(pChildMenu) {
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
    if (is_winnt_3())		/* for NT 3.xx */
    {
	GuiMenu *pMenu = gui_w32_find_menu(gui.root_menu, id);
	if (pMenu)
	    gui_menu_cb(pMenu);
    }
    else			/* win95 and variants (NT 4.0, I guess) */
    {
	MENUITEMINFO	info;

	info.cbSize = sizeof(info);
	info.fMask = MIIM_DATA;

	/*
	 * The menu id is only 16-bit, so we store the menu pointer in the data
	 * field instead.
	 */
	GetMenuItemInfo(s_menuBar, id, FALSE, &info);
	gui_menu_cb((GuiMenu *)info.dwItemData);
    }
}

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
    WIN		*wp;

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
	    if (scroll_shift > 0)
		val <<= scroll_shift;
	    break;
	case SB_LINEDOWN:
	    /* TRACE("SB_LINEDOWN\n"); */
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
	    if (scroll_shift > 0)
		val <<= scroll_shift;
	    break;

	default:
	    /* TRACE("Unknown scrollbar event %d\n", code); */
	    return 0;
    }

    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    if (scroll_shift > 0)
	si.nPos = val >> scroll_shift;
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

    /*
     * Scrollbars seem to grab focus and vim doesn't read the input queue until
     * you stop dragging the scrollbar.  We get here each time the scrollbar is
     * dragged another pixel, but as far as the rest of vim goes, it thinks
     * we're just hanging in the call to DispatchMessage() in
     * process_message().  The DispatchMessage() call that hangs was passed a
     * mouse button click event in the scrollbar window. -- webb.
     *
     * Solution: Do the scrolling right here.  But only when allowed.
     * Ignore the scrollbars while executing an external command or when there
     * are still characters to be processed.
     */
    if (dont_scroll || !termcap_active || !vim_is_input_buf_empty())
	return 0;

    if (dragging)
    {
	if (sb->wp == NULL)
	    gui.dragged_sb = SBAR_BOTTOM;
	else if (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    gui.dragged_sb = SBAR_LEFT;
	else
	    gui.dragged_sb = SBAR_RIGHT;
	gui.dragged_wp = sb->wp;
    }
    else
	gui.dragged_sb = SBAR_NONE;

    /* Vertical sbar info is kept in the first sbar (the left one) */
    if (sb->wp != NULL)
	sb = &sb->wp->w_scrollbars[0];

    /*
     * Check validity of value
     */
    if (val < 0)
	val = 0;
    if (val > sb->max - sb->size + 1)
	val = sb->max - sb->size + 1;

    sb->value = val;

#ifdef RIGHTLEFT
    if (sb->wp == NULL && curwin->w_p_rl)
    {
	val = sb->max + 1 - sb->size - val;
	if (val < 0)
	    val = 0;
    }
#endif

    scrollbar_value = val;

    if (sb->wp != NULL)		/* vertical scrollbar */
    {
	current_scrollbar = 0;
	for (wp = firstwin; wp != sb->wp && wp != NULL; wp = wp->w_next)
	    current_scrollbar++;
	if (wp == NULL)
	    return 0;

	if (State & NORMAL)
	{
	    gui_do_scroll();
	    setcursor();
	}
	else if (State & INSERT)
	{
	    ins_scroll();
	    setcursor();
	}
	else if (State & CMDLINE)
	{
	    if (!msg_scrolled)
	    {
		gui_do_scroll();
		redrawcmdline();
	    }
	}
    }
    else
    {
	colnr_t	    old_leftcol = curwin->w_leftcol;

	if (State & NORMAL)
	    gui_do_horiz_scroll();
	else if (State & INSERT)
	    ins_horscroll();
	else if (State & CMDLINE)
	{
	    if (!msg_scrolled)
	    {
		gui_do_horiz_scroll();
		redrawcmdline();
	    }
	}
	if (old_leftcol != curwin->w_leftcol)
	{
	    updateWindow(curwin);   /* update window, status line and cmdline */
	    setcursor();
	}
    }
    out_flush();

    return 0;
}

/* Find the number of lines to scroll if the mouse wheel is used
 * (this was stolen from the MFC source with only a few modifications)
 */
    static void
get_mouse_scroll_lines(void)
{
    
#ifndef SPI_GETWHEELSCROLLLINES
# define SPI_GETWHEELSCROLLLINES    104
#endif
    
    UINT msgGetScrollLines = 0;
    WORD nRegisteredMessage = 0;
    OSVERSIONINFO ver;
    HKEY hKey;
    char szData[128];
    DWORD dwKeyDataType;
    DWORD dwDataBufSize = sizeof(szData);

    /* try system settings */

    memset(&ver, 0, sizeof(ver));
    ver.dwOSVersionInfoSize = sizeof(ver);

    mouse_scroll_lines = 3; /* reasonable default */
    if (!GetVersionEx(&ver))
	return;

    if ((ver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ||
	  ver.dwPlatformId == VER_PLATFORM_WIN32_NT)
	&& ver.dwMajorVersion < 4)
    {
	if (RegOpenKeyEx(HKEY_CURRENT_USER,  "Control Panel\\Desktop",
			 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
	    if (RegQueryValueEx(hKey, "WheelScrollLines", NULL, &dwKeyDataType,
			    (LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
	    {
		mouse_scroll_lines = strtoul(szData, NULL, 10);
	    }
	    RegCloseKey(hKey);
	}
    }
    else if (ver.dwPlatformId == VER_PLATFORM_WIN32_NT && ver.dwMajorVersion >= 4)
    {
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &mouse_scroll_lines, 0);
    }
    return;
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

    if (mouse_scroll_lines == 0)
	get_mouse_scroll_lines();

    if (mouse_scroll_lines > 0
	    && mouse_scroll_lines < (size > 2 ? size - 2 : 1))
    {
	for (i = mouse_scroll_lines; i > 0; --i)
	    _OnScroll(hwnd, hwndCtl, zDelta >= 0 ? SB_LINEUP : SB_LINEDOWN, 0);
    }
    else
	_OnScroll(hwnd, hwndCtl, zDelta >= 0 ? SB_PAGEUP : SB_PAGEDOWN, 0);
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
    if (!(lpwpos->flags & SWP_NOSIZE))
    {
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

/*
 * Draw the rectangle just under the left and/or right scrollbars.
 */
    static void
_OnEraseBG(HWND hwnd)
{
    HDC		hdc =  GetDC(hwnd);
    RECT	rect, drect;

    GetClientRect(hwnd, &rect);
    /* top of the rectangle is just below the last window */
    drect.top = rect.bottom - gui.char_height
			   * (Rows - (lastwin->w_winpos + lastwin->w_height));
    drect.top += lastwin->w_status_height * gui.char_height;

    if (gui.which_scrollbars[SBAR_BOTTOM])
	drect.top -= gui.scrollbar_height;
    drect.bottom = rect.bottom;

    if (gui.which_scrollbars[SBAR_LEFT])
    {
	drect.left = 0;
	drect.right = gui.scrollbar_width;
	FillRect(hdc, &drect, GetSysColorBrush(COLOR_BTNFACE));
    }
    if (gui.which_scrollbars[SBAR_RIGHT])
    {
	drect.left = rect.right - gui.scrollbar_width;
	drect.right = rect.right;
	FillRect(hdc, &drect, GetSysColorBrush(COLOR_BTNFACE));
    }
    ReleaseDC(hwnd, hdc);
}

    static BOOL
_OnCreate (HWND hwnd, LPCREATESTRUCT lpcs)
{
    /* nothing to do */
    return 0;
}

    static void
HandleMouseHide (UINT uMsg)
{
    /* Handle specially, to centralise coding. We need to be sure we catch all
     * possible events which should cause us to restore the cursor (as it is a
     * shared resource, we take full responsibility for it).
     */
    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    case WM_CHAR:
    case WM_SYSCHAR:
	/*
	 * blank out the pointer if necessary
	 */
	if (p_mh && !gui.pointer_hidden)
	{
	    ShowCursor(FALSE);
	    gui.pointer_hidden = TRUE;
	}
	break;

    case WM_MOUSEMOVE:
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
	if (gui.pointer_hidden)
	{
	    ShowCursor(TRUE);
	    gui.pointer_hidden = FALSE;
	}
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

    HandleMouseHide(uMsg);

    s_uMsg = uMsg;
    s_wParam = wParam;
    s_lParam = lParam;

    switch (uMsg)
    {
#ifdef DEADKEYSPATCH
	HANDLE_MSG(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_MSG(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
#endif
	/* HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate); */
	HANDLE_MSG(hwnd, WM_CHAR,	_OnChar);
	HANDLE_MSG(hwnd, WM_SYSCHAR,	_OnSysChar);
	HANDLE_MSG(hwnd, WM_CLOSE,	_OnClose);
	/* HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand); */
	HANDLE_MSG(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_MSG(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_MSG(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_MSG(hwnd, WM_KILLFOCUS,	_OnKillFocus);
	HANDLE_MSG(hwnd, WM_COMMAND,	_OnMenu);
	/* HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove); */
	/* HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate); */
	HANDLE_MSG(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_MSG(hwnd, WM_SIZE,	_OnSize);
	/* HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand); */
	/* HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey); */
	/* HANDLE_MSG(hwnd, WM_SYSKEYUP,    _OnAltKey); */
	HANDLE_MSG(hwnd, WM_VSCROLL,	_OnScroll);
	HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);

    case WM_ERASEBKGND:
	_OnEraseBG(hwnd);
	break;

    case WM_CREATE:	/* HANDLE_MSG doesn't seem to handle this one */
	return _OnCreate (hwnd, (LPCREATESTRUCT)lParam);

    case WM_SIZING:	/* HANDLE_MSG doesn't seem to handle this one */
	return _DuringSizing(hwnd, wParam, (LPRECT)lParam);

    case WM_MOUSEWHEEL:
	_OnMouseWheel(hwnd, HIWORD(wParam));
	break;

    default:
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

    HandleMouseHide(uMsg);

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

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(int *argc, char **argv)
{
    /* We don't have any command line arguments for the Windows GUI yet */
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

    gui.scrollbar_width = GetSystemMetrics(SM_CXVSCROLL);
    gui.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL);
    gui.menu_height = 0;	/* Windows takes care of this */
    gui.border_width = 0;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = _WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = s_hinst;
    wndclass.hIcon = LoadIcon(wndclass.hInstance, "IDR_VIM");
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockBrush(LTGRAY_BRUSH);
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
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
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

    s_textArea = CreateWindow(
	szTextAreaClass, "Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				/* Any value will do for now */
	100,				/* Any value will do for now */
	s_hwnd, NULL,
	s_hinst, NULL);

    if (s_textArea == NULL)
	return FAIL;

    s_menuBar = CreateMenu();

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
    gui.border_offset = gui.border_width;

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
    /* Destroy our window (if we have one). */
    if (s_hwnd != NULL)
    {
	destroying = TRUE;	/* ignore WM_DESTROY message now */
	DestroyWindow(s_hwnd);
    }
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
    gui_w32_get_menu_height(TRUE);
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

    scroll_shift = 0;
    while (max > 32767)
    {
	max = max + 1 >> 1;
	val  >>= 1;
	size >>= 1;
	++scroll_shift;
    }

    if (scroll_shift > 0)
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
    SetWindowPos(sb->id, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
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
    static const char ach[] = {'W', 'f', 'g', 'M'};

    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    siz;

    GetTextExtentPoint(hdc, ach, sizeof(ach), &siz);

    gui.char_width = siz.cx / sizeof(ach);
    /*
     * Make characters one pixel higher, so that italic and bold fonts don't
     * draw off the bottom of their character space.  Also means that we can
     * underline an underscore for normal text.
     */
    gui.char_height = siz.cy + 1;

    /* TRACE("GetFontSize: h %d, w %d\n", gui.char_height, gui.char_width); */

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

    static GuiFont
get_font_handle(LOGFONT *lf)
{
    HFONT   *font = NULL;

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
    if (lf->lfCharSet == ANSI_CHARSET)
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
	cf.hwndOwner = NULL;
	cf.Flags = CF_SCREENFONTS | CF_FIXEDPITCHONLY | CF_INITTOLOGFONTSTRUCT;
	if (lastlf != NULL)
	    *lf = *lastlf;
	cf.lpLogFont = lf;
	cf.nFontType = REGULAR_FONTTYPE;
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
	vim_memmove(lastlf, lf, sizeof(LOGFONT));
    
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
    if (gui.norm_font != (GuiFont)0)
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

    void
gui_mch_free_font(
    GuiFont font)
{
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
	"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT,
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
     * Last attempt. Look in the file "$VIM/rgb.txt".
     */
    {
#define LINE_LEN 100
	FILE	*fd;
	char	line[LINE_LEN];
	char_u	*fname;

	fname = expand_env_save((char_u *)"$VIM/rgb.txt");
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

    void
gui_mch_draw_string(
    int	    row,
    int	    col,
    char_u  *s,
    int	    len,
    int	    flags)
{
    static int *padding = NULL;
    static int pad_size = 0;

    HDC	    hdc = GetDC(s_textArea);
    HPEN    hpen, old_pen;
    int	    y;
    int	    i;

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
    HBRUSH  hbr;
    RECT    rc;

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
	FillRect(hdc, &rc, hbr);
	DeleteBrush(hbr);

	SetBkMode(hdc, TRANSPARENT);
    }
#else
    /*
     * The alternative would be to write the characters in opaque mode, but
     * when the text is not exactly the same proportions as normal text, too
     * big or too little a rectangle gets drawn for the background.
     */
    SetBkMode(hdc, OPAQUE);
    SetBkColor(hdc, gui.currBgColor);
#endif
    SetTextColor(hdc, gui.currFgColor);
    SelectFont(hdc, gui.currFont);

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
     */
    ExtTextOut(hdc, TEXT_X(col), TEXT_Y(row), 0, NULL, (char *)s, len, padding);

    if (flags & DRAW_BOLD)
	ExtTextOut(hdc, TEXT_X(col), TEXT_Y(row), 0, NULL, (char *)s,
								len, padding);

    if (flags & DRAW_UNDERL)
    {
	/*
	 * Note: LineTo() excludes the last pixel in the line.
	 */
	hpen = CreatePen(PS_SOLID, 1, gui.currFgColor);
	old_pen = SelectObject(hdc, hpen);
	y = FILL_Y(row + 1) - 1;
	MoveToEx(hdc, FILL_X(col), y, NULL);
	LineTo(hdc, FILL_X(col + len), y);
	DeleteObject(SelectObject(hdc, old_pen));
    }
    /* SelectFont(hdc, old_font); */
    ReleaseDC(s_textArea, hdc);
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
    HDC	    hdc = GetWindowDC(s_textArea);
    RECT    rc;

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = 0;
    rc.top = 0;
    rc.right = gui.num_cols * gui.char_width;
    rc.bottom = gui.num_rows * gui.char_height;
    InvertRect(hdc, &rc);

    ui_delay(20L, TRUE);	/* wait 1/50 of a second */

    InvertRect(hdc, &rc);
    ReleaseDC(s_textArea, hdc);
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
    HDC	    hdc = GetWindowDC(s_textArea);
    RECT    rc;

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(c);
    rc.top = FILL_Y(r);
    rc.right = rc.left + nc * gui.char_width;
    rc.bottom = rc.top + nr * gui.char_height;
    InvertRect(hdc, &rc);
    ReleaseDC(s_textArea, hdc);
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
    if (title != NULL)
	SetWindowText(s_hwnd, (LPCSTR)title);
}

/*
 * Draw a cursor without focus.
 */
    void
gui_mch_draw_hollow_cursor(GuiColor color)
{
    HDC	    hdc = GetWindowDC(s_textArea);
    HBRUSH  hbr;
    RECT    rc;

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(gui.col);
    rc.top = FILL_Y(gui.row);
    rc.right = rc.left + gui.char_width;
    rc.bottom = rc.top + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(hdc, &rc, hbr);
    DeleteBrush(hbr);
    ReleaseDC(s_textArea, hdc);
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
    HDC		hdc = GetWindowDC(s_textArea);
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
    FillRect(hdc, &rc, hbr);
    DeleteBrush(hbr);
    ReleaseDC(s_textArea, hdc);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    static void
process_message(void)
{
    MSG	    msg;
    UINT    vk;		/* Virtual key */
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

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
    {
	vk = (int) msg.wParam;
#ifdef DEADKEYSPATCH
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
#endif
	for (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    if (special_keys[i].key_sym == vk)
	    {
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
	s_wait_timer = SetTimer(NULL, 0, (UINT)wtime, (TIMERPROC)_OnTimer);

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
    HDC	    hdc = GetWindowDC(s_textArea);
    HBRUSH  hbr;

    hbr = CreateSolidBrush(gui.back_pixel);
    FillRect(hdc, rcp, hbr);
    DeleteBrush(hbr);
    ReleaseDC(s_textArea, hdc);
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
	UpdateWindow(s_textArea);

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
    SetMenu(s_hwnd, flag ? s_menuBar : NULL);
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

/*
 * Add a sub menu to the menu bar.
 */
    void
gui_mch_add_menu(
    GuiMenu *menu,
    GuiMenu *parent)
{
    UINT	pos = 99999;
    GuiMenu	*mp;

    if (parent == NULL)	    /* top menu item */
	for (pos = 0, mp = gui.root_menu; mp != NULL; mp = mp->next, ++pos)
	    if (mp == menu)
		break;

    if (is_winnt_3())		/* for NT 3.xx */
    {
	menu->submenu_id = CreatePopupMenu();
	menu->id = s_menu_id++;
	InsertMenu((parent == NULL) ? s_menuBar : parent->submenu_id,
		(UINT)pos, MF_POPUP | MF_STRING | MF_BYPOSITION,
		(UINT)menu->submenu_id, (LPCTSTR) menu->name);
    }
    else			/* win95 and variants (NT 4.0, I guess) */
    {
	MENUITEMINFO	info;

	menu->submenu_id = CreatePopupMenu();

	info.cbSize = sizeof(info);
	info.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID | MIIM_SUBMENU;
	info.dwItemData = (DWORD)menu;
	info.wID = menu->id = s_menu_id++;
	info.fType = MFT_STRING;
	info.dwTypeData = (LPTSTR)menu->name;
	info.cch = STRLEN(menu->name);
	info.hSubMenu = menu->submenu_id;
	InsertMenuItem((parent == NULL) ? s_menuBar : parent->submenu_id,
						    (UINT)pos, TRUE, &info);
    }

    /* Fix window size if menu may have wrapped */
    if (parent == NULL)
	gui_w32_get_menu_height(TRUE);
}

/*
 * Add a menu item to a menu
 */
    void
gui_mch_add_menu_item(
    GuiMenu *menu,
    GuiMenu *parent)
{
    if (is_winnt_3())		/* for NT 3.xx */
    {
	menu->id = s_menu_id++;
	menu->submenu_id = NULL;
	InsertMenu(parent->submenu_id,(UINT)99999, MF_STRING|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) menu->name);
    }
    else			/* win95 and variants (NT 4.0, I guess) */
    {
	MENUITEMINFO	info;

	info.cbSize = sizeof(info);
	info.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID;
	info.dwItemData = (DWORD)menu;
	info.wID = menu->id = s_menu_id++;
	info.fType = MFT_STRING;
	info.dwTypeData = (LPTSTR)menu->name;
	info.cch = STRLEN(menu->name);
	InsertMenuItem(parent->submenu_id, (UINT)99999, TRUE, &info);

	menu->submenu_id = NULL;
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(GuiMenu *menu)
{
    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
    if (menu->submenu_id != NULL)
	DestroyMenu(menu->submenu_id);
}

/*
 * Make a menu either grey or not grey.
 */
    void
gui_mch_menu_grey(
    GuiMenu *menu,
    int	    grey)
{
    if (grey)
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_GRAYED);
    else
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_ENABLED);
}

/*
 * Make menu item hidden or not hidden
 */
    void
gui_mch_menu_hidden(
    GuiMenu *menu,
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

/*
 * Return the RGB value of a pixel as "#RRGGBB".
 */
    char_u *
gui_mch_get_rgb(pixel)
    GuiColor	pixel;
{
    static char_u retval[10];

    sprintf((char *)retval, "#%02x%02x%02x",
	    GetRValue(pixel), GetGValue(pixel), GetBValue(pixel));
    return retval;
}
