/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 *
 * gui_w16.c
 *
 * GUI support for Microsoft Windows 3.1x
 *
 * George V. Reilly <george@reilly.org> wrote the original Win32 GUI.
 * Robert Webb reworked it to use the existing GUI stuff and added menu,
 * scrollbars, etc.
 *
 * Vince Negri then butchered the code to get it compiling for
 * 16-bit windows.
 *
 */

#ifndef FEAT_TINY
# define WIN16_FIND_REPLACE	/* include code for find/replace dialog */
#endif


#include "vim.h"
#ifdef FEAT_MENU
# define MENUHINTS		/* show menu hints in command line */
#endif
#include "version.h"	/* used by dialog box routine for default title */
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <windowsx.h>
#ifdef WIN16_3DLOOK
# include <ctl3d.h>
#endif
#include "guiw16rc.h"

/* Undocumented Windows Message - not even defined in some SDK headers */
#define WM_EXITSIZEMOVE			0x0232

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
# define NEAR
# define FAR
# define BYTE		int
# define LPCSTR		int
#endif


#ifdef FEAT_TOOLBAR
# define CMD_TB_BASE (99)
# include <vimtbar.h>
#endif

#define HANDLE_WM_DROPFILES(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HDROP)(wParam)), 0L)

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


/* Local variables: */
static int	    s_need_activate = FALSE;
static int	    s_button_pending = -1;
static int	    s_x_pending;
static int	    s_y_pending;
static UINT	    s_kFlags_pending;

static HINSTANCE    s_hinst = NULL;
static HWND	    s_hwnd = NULL;
static HDC	    s_hdc = NULL;
static HWND	    s_textArea = NULL;
static HBRUSH	    s_brush = NULL;
#ifdef FEAT_MENU
static HMENU	    s_menuBar = NULL;
static UINT	    s_menu_id = 100;
#endif
static UINT	    s_wait_timer = 0;	/* Timer for get char from user */
static int	    destroying = FALSE;	/* calling DestroyWindow() ourselves */

#ifdef FEAT_TOOLBAR
static HWND	    s_toolbarhwnd = NULL;
#endif

#ifdef WIN16_FIND_REPLACE
static HWND	    s_findrep_hwnd = NULL;
static UINT	    s_findrep_msg = 0;
static FINDREPLACE  s_findrep_struct;
static int	    s_findrep_is_find;
#endif
static UINT	    s_uMsg = 0;
static WPARAM	    s_wParam = 0;
static LPARAM	    s_lParam = 0;

static int	    s_timed_out = FALSE;

/* Flag that is set while processing a message that must not be interupted by
 * processing another message. */
static int		s_busy_processing = FALSE;

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
    {VK_ADD,	    'K', '6'},
    {VK_SUBTRACT,   'K', '7'},
    {VK_DIVIDE,	    'K', '8'},
    {VK_MULTIPLY,   'K', '9'},
    {VK_SEPARATOR,  'K', 'A'},	    /* Keypad Enter */
    {VK_DECIMAL,    'K', 'B'},

    {VK_NUMPAD0,    'K', 'C'},
    {VK_NUMPAD1,    'K', 'D'},
    {VK_NUMPAD2,    'K', 'E'},
    {VK_NUMPAD3,    'K', 'F'},
    {VK_NUMPAD4,    'K', 'G'},
    {VK_NUMPAD5,    'K', 'H'},
    {VK_NUMPAD6,    'K', 'I'},
    {VK_NUMPAD7,    'K', 'J'},
    {VK_NUMPAD8,    'K', 'K'},
    {VK_NUMPAD0,    'K', 'L'},


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

#define DLG_ALLOC_SIZE 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
#if defined(FEAT_GUI_DIALOG) || defined(PROTO)
static BOOL CALLBACK dialog_callback(HWND, UINT, WPARAM, LPARAM);

static BOOL CenterWindow(HWND hwndChild, HWND hwndParent);
static LPWORD
add_dialog_element(
	LPWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	BYTE clss,
	const char *caption);

static int dialog_default_button = -1;
#endif

static void get_dialog_font_metrics(void);

static WORD	s_dlgfntheight;	    /* height of the dialog font */
static WORD	s_dlgfntwidth;	    /* width of the dialog font	*/


#ifdef FEAT_TOOLBAR
static void initialise_toolbar(void);
static int get_toolbar_bitmap(char_u *name);
#endif
#ifdef WIN16_FIND_REPLACE
static void initialise_findrep(char_u *initial_string);
#endif

#ifdef WIN16_FIND_REPLACE
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
	cmd[0] = Ctrl_O;
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

#ifdef FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    static int
gui_w16_get_menu_height(
    int	    fix_window)	    /* If TRUE, resize window if menu height changed */
{
    static int	old_menu_height = -1;

    int	    num;
    int	    menu_height;

    if (gui.menu_is_active)
	num = GetMenuItemCount(s_menuBar);
    else
	num = 0;

    if (num == 0)
	menu_height = 0;
    else if (gui.starting)
	menu_height = GetSystemMetrics(SM_CYMENU);
    else
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

    if (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE);

    old_menu_height = menu_height;
    return menu_height;
}
#endif /*FEAT_MENU*/

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
gui_w16_rm_blink_timer(void)
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
    gui_w16_rm_blink_timer();
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
    gui_w16_rm_blink_timer();

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
 * Get this message when the user double-clicks the control box.
 */
    static void
_OnClose(
    HWND hwnd)
{
#ifdef FEAT_BROWSE
    int save_browse = cmdmod.browse;
#endif
#if defined(FEAT_GUI_DIALOG) || defined(CON_DIALOG)
    int save_confirm = cmdmod.confirm;
#endif

    /* Only exit when there are no changed files */
    exiting = TRUE;
#ifdef FEAT_BROWSE
    cmdmod.browse = TRUE;
#endif
#if defined(FEAT_GUI_DIALOG) || defined(CON_DIALOG)
    cmdmod.confirm = TRUE;
#endif
    if (!check_changed_any(FALSE))    /* will give warning for changed buffer */
	getout(0);

    exiting = FALSE;
#ifdef FEAT_BROWSE
    cmdmod.browse = save_browse;
#endif
#if defined(FEAT_GUI_DIALOG) || defined(CON_DIALOG)
    cmdmod.confirm = save_confirm;
#endif
    setcursor();		    /* position cursor */
    out_flush();
}

/*
 * Get a message when the user switches back to vim
 */
    static LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    /* When activated: Check if any file was modified outside of Vim. */
    if (fActivate)
	check_timestamps(TRUE);
#ifdef FEAT_AUTOCMD
    /* In any case, fire the appropriate autocommand */
    apply_autocmds(fActivate ? EVENT_FOCUSGAINED : EVENT_FOCUSLOST,
						   NULL, NULL, FALSE, curbuf);
#endif
     return DefWindowProc(hwnd, WM_ACTIVATEAPP, fActivate, dwThreadId);
}

/*
 * Get a message when the the window is being destroyed.
 */
    static void
_OnDestroy(
    HWND hwnd)
{
#ifdef WIN16_3DLOOK
    Ctl3dUnregister(s_hinst);
#endif
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
#ifdef FEAT_WINDOWS
    char    szFile[MAXPATHL];
    UINT    cFiles = DragQueryFile(hDrop, 0xFFFF, szFile, MAXPATHL);
    UINT    i;
    char_u  *fname;
    char_u  **fnames;
    char_u  redo_dirs = FALSE;

    /* TRACE("_OnDropFiles: %d files dropped\n", cFiles); */

#ifdef FEAT_VISUAL
    reset_VIsual();
#endif

    fnames = (char_u **) alloc(cFiles * sizeof(char_u *));

    for (i = 0; i < cFiles; ++i)
    {
	DragQueryFile(hDrop, i, szFile, MAXPATHL);

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
	 */
	if (cFiles == 1)
	{
	    if (mch_isdir(fnames[0]))
	    {
		if (mch_chdir(fnames[0]) == 0)
		{
		    vim_free(fnames[0]);
		    fnames[0] = NULL;
		    redo_dirs = TRUE;
		}
	    }
	}


	if (GetKeyState(VK_SHIFT) & 0x8000)
	{
	    /* Shift held down, change to first file's directory */
	    if (vim_chdirfile(fnames[0]) == 0)
	    {
		redo_dirs = TRUE;
	    }
	}

	    /* Handle the drop, :edit or :split to get to the file */
	     handle_drop(cFiles, fnames, ((GetKeyState(VK_CONTROL) & 0x8000) != 0));

	if (redo_dirs)
	    shorten_fnames(TRUE);

	/* Update the screen display */
	update_screen(NOT_VALID);
	setcursor();
	out_flush();
    }
    s_need_activate = TRUE;
#endif
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
    UINT ch,	/* Careful: CSI arrives as 0xffffff9b */
    int cRepeat)
{
    char_u	string[3];

    /* TRACE("OnChar(%d, %c)\n", ch, ch); */

    string[0] = ch;
    if (string[0] == Ctrl_C && !mapped_ctrl_c)
    {
	trash_input_buf();
	got_int = TRUE;
    }

    if (string[0] == CSI)
    {
	/* Insert CSI as K_CSI. */
	string[1] = KS_EXTRA;
	string[2] = KE_CSI;
	add_to_input_buf(string, 3);
    }
    else
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

    if (IS_SPECIAL((int)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((int)ch);
	string[len++] = K_THIRD((int)ch);
    }
    else if (ch == CSI)
    {
	string[len++] = CSI;
	string[len++] = KS_EXTRA;
	string[len++] = KE_CSI;
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
#if 0 //<VN>
    else if (s_uMsg == WM_CAPTURECHANGED)
    {
	/* on W95/NT4, somehow you get in here with an odd Msg
	 * if you press one button while holding down the other..*/
	if (s_button_pending == MOUSE_LEFT)
	    button = MOUSE_RIGHT;
	else
	    button = MOUSE_LEFT;
    }
#endif
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
    HWND hwnd,
    int x,
    int y,
    UINT keyFlags)
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
	    gui_mouse_moved(x, y);
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

	out_flush();	    /* make sure all output has been processed */
	(void)BeginPaint(hwnd, &ps);


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
	gui_resize_shell(cx, cy);

#ifdef FEAT_MENU
	/* Menu bar may wrap differently now */
	gui_w16_get_menu_height(TRUE);
#endif
    }
}

    static void
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
     (void) DefWindowProc(WM_SETFOCUS, hwnd, hwndOldFocus,0)  ;
}

    static void
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    gui_focus_change(FALSE);
    (void) DefWindowProc(WM_KILLFOCUS, hwnd, hwndNewFocus,0) ;
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_t with the given id
 */
     static vimmenu_t *
gui_w16_find_menu(
     vimmenu_t *pMenu,
     int id)
{
     vimmenu_t *pChildMenu;

     while (pMenu)
     {
	if (pMenu->id == (UINT)id)
	     break;
	if (pMenu->children != NULL)
	{
	     pChildMenu = gui_w16_find_menu(pMenu->children, id);
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
     vimmenu_t *pMenu;

     pMenu = gui_w16_find_menu(root_menu, id);
     if (pMenu)
	gui_menu_cb(pMenu);
}
#endif
/*
 * Find the scrollbar with the given hwnd.
 */
	 static scrollbar_t *
gui_w16_find_scrollbar(HWND hwnd)
{
    win_t		*wp;

    if (gui.bottom_sbar.id == hwnd)
	return &gui.bottom_sbar;
#ifndef FEAT_WINDOWS
    wp = curwin;
#else
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
#endif
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
    scrollbar_t *sb, *sb_info;
    long	val;
    int		dragging = FALSE;
    int		nPos;

    sb = gui_w16_find_scrollbar(hwndCtl);
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
	    val = pos;
	    dragging = TRUE;
	    if (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    break;
	case SB_LINEDOWN:
	    /* Because of round-off errors we can't move one line when
	     * scroll_shift is non-zero.  Scroll some extra. */
	    if (sb->scroll_shift > 0)
		val += (1 << sb->scroll_shift);
	    else
		val++;
	    break;
	case SB_LINEUP:
	    val--;
	    break;
	case SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    break;
	case SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    break;
	case SB_TOP:
	    val = 0;
	    break;
	case SB_BOTTOM:
	    val = sb_info->max;
	    break;
	case SB_ENDSCROLL:
	    /*
	     * "pos" only gives us 16-bit data.  In case of large file, use
	     * GetScrollPos() which returns 32-bit.  Unfortunately it is not
	     * valid while the scrollbar is being dragged.
	     */
	    val = GetScrollPos(hwndCtl, SB_CTL);
	    if (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    break;

	default:
	    /* TRACE("Unknown scrollbar event %d\n", code); */
	    return 0;
    }

    if (sb->scroll_shift > 0)
	nPos = val >> sb->scroll_shift;
    else
	nPos = val;
    SetScrollPos(hwndCtl, SB_CTL, nPos, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    if (sb->wp != NULL)
    {
	if (sb == &sb->wp->w_scrollbars[SBAR_LEFT])
	    SetScrollPos(sb->wp->w_scrollbars[SBAR_RIGHT].id,
						   SB_CTL, nPos, TRUE);
	else
	    SetScrollPos(sb->wp->w_scrollbars[SBAR_LEFT].id,
						  SB_CTL, nPos, TRUE);
    }

    /* Don't let us be interrupted here by another message. */
    s_busy_processing = TRUE;
    gui_drag_scrollbar(sb, val, dragging);
    s_busy_processing = FALSE;

    return 0;
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

    GetWindowRect(s_textArea, &rct);

    GetCursorPos((LPPOINT)&mp);
    return (int)(mp.x - rct.left);

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

    GetWindowRect(s_textArea, &rct);

	GetCursorPos((LPPOINT)&mp);
	    return (int)(mp.y - rct.top);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    void
gui_mch_setmouse(int x, int y)
{
    RECT rct;

    GetWindowRect(s_textArea, &rct);
	(void)SetCursorPos(x + gui.border_offset + rct.left,
			   y + gui.border_offset + rct.top);
}

    static void
gui_w16_get_valid_dimensions(
    int w,
    int h,
    int *valid_w,
    int *valid_h)
{
    int	    base_width, base_height;

    base_width = gui_get_base_width()
	+ GetSystemMetrics(SM_CXFRAME) * 2;
    base_height = gui_get_base_height()
	+ GetSystemMetrics(SM_CYFRAME) * 2
	+ GetSystemMetrics(SM_CYCAPTION)
#ifdef FEAT_MENU
	+ gui_w16_get_menu_height(FALSE)
#endif
	;
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

    if (!IsIconic(hwnd) && !(lpwpos->flags & SWP_NOSIZE))
    {
	gui_w16_get_valid_dimensions(lpwpos->cx, lpwpos->cy,
				     &lpwpos->cx, &lpwpos->cy);
    }
    return 0;
}



    static BOOL
_OnCreate (HWND hwnd, LPCREATESTRUCT lpcs)
{
    return 0;
}


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

    case WM_SYSKEYUP:	 /* show the pointer when a system-key is pressed */
    case WM_SYSCHAR:
    case WM_MOUSEMOVE:	 /* show the pointer on any mouse action */
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
#ifdef FEAT_MENU
	HANDLE_MSG(hwnd, WM_COMMAND,	_OnMenu);
#endif
	/* HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove); */
	/* HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate); */
	HANDLE_MSG(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_MSG(hwnd, WM_SIZE,	_OnSize);
	/* HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand); */
	/* HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey); */
	/* HANDLE_MSG(hwnd, WM_SYSKEYUP,    _OnAltKey); */
	HANDLE_MSG(hwnd, WM_VSCROLL,	_OnScroll);
	HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_MSG(hwnd, WM_ACTIVATEAPP, _OnActivateApp);


    case WM_QUERYENDSESSION:	/* System wants to go down. */
	gui_shell_closed();    /* Will exit when no changed buffers. */
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
#ifdef FEAT_MENU
	if (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((int)wParam))
		)
#endif
	    return HANDLE_WM_SYSCHAR((hwnd), (wParam), (lParam), (_OnSysChar));
#ifdef FEAT_MENU
	else
	    return DefWindowProc(hwnd, uMsg, wParam, lParam);
#endif

    case WM_SYSKEYUP:
#ifdef FEAT_MENU
	if (gui.menu_is_active)
	    return DefWindowProc(hwnd, uMsg, wParam, lParam);
	else
#endif
	    return 0;

    case WM_CREATE:	/* HANDLE_MSG doesn't seem to handle this one */
	return _OnCreate (hwnd, (LPCREATESTRUCT)lParam);


#ifdef MENUHINTS
    case WM_MENUSELECT:
	if (((UINT) LOWORD(lParam)
		    & (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
		== MF_HILITE
		&& (State & CMDLINE) == 0)
	{
	    UINT idButton;
	    int	idx;
	    vimmenu_t *pMenu;

	    idButton = (UINT)(wParam);
	    pMenu = gui_w16_find_menu(root_menu, idButton);
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
    case WM_NCHITTEST:
	{
	    LRESULT	result;
	    int x, y;
	    int xPos = LOWORD(lParam);

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
	/* break; */
    default:
#ifdef WIN16_FIND_REPLACE
	if (uMsg == s_findrep_msg && s_findrep_msg != 0)
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


/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(int *argc, char **argv)
{
    /* No special args for win16 GUI at the moment. */

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

#ifdef WIN16_3DLOOK
    Ctl3dRegister(s_hinst);
    Ctl3dAutoSubclass(s_hinst);
#endif

    /* Display any pending error messages */
    mch_display_error();

    gui.scrollbar_width = GetSystemMetrics(SM_CXVSCROLL);
    gui.scrollbar_height = GetSystemMetrics(SM_CYHSCROLL);
#ifdef FEAT_MENU
    gui.menu_height = 0;	/* Windows takes care of this */
#endif
    gui.border_width = 0;

    gui.currBgColor = (long) -1;

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    if (GetClassInfo(s_hinst, szVimWndClass, &wndclass) == 0) {
	wndclass.style = 0;
	wndclass.lpfnWndProc = _WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = s_hinst;
	wndclass.hIcon = LoadIcon(wndclass.hInstance, MAKEINTRESOURCE(IDR_VIM));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = s_brush;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szVimWndClass;

	if (RegisterClass(&wndclass) == 0)
	    return FAIL;
    }

    s_hwnd = CreateWindow(
	szVimWndClass, "Vim W16 GUI",
	WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT, CW_USEDEFAULT,
	100,				/* Any value will do */
	100,				/* Any value will do */
	NULL, NULL,
	s_hinst, NULL);

    if (s_hwnd == NULL)
	return FAIL;

    /* Create the text area window */
    if (GetClassInfo(s_hinst, szTextAreaClass, &wndclass) == 0) {
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
    }
    s_textArea = CreateWindow(
	szTextAreaClass, "Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				/* Any value will do for now */
	100,				/* Any value will do for now */
	s_hwnd, NULL,
	s_hinst, NULL);

    if (s_textArea == NULL)
	return FAIL;


#ifdef FEAT_MENU
    s_menuBar = CreateMenu();
#endif
    s_hdc = GetDC(s_textArea);

#ifdef MSWIN16_FASTTEXT
    SetBkMode(s_hdc, OPAQUE);
#endif

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


    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();

#ifdef FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#endif
#ifdef WIN16_FIND_REPLACE
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
gui_mch_set_shellsize(int width, int height, int min_width, int min_height,
		    int base_width, int base_height)
{
    RECT    workarea_rect;
    int     win_width, win_height;
    int	    win_xpos, win_ypos;
    WINDOWPLACEMENT wndpl;

    /* try to keep window completely on screen */
    /* get size of the screen work area - use SM_CYFULLSCREEN
     * instead of SM_CYSCREEN so that we don't overlap the
     * taskbar if someone fires us up on Win95/NT */
    workarea_rect.left = 0;
    workarea_rect.top = 0;
    workarea_rect.right = GetSystemMetrics(SM_CXSCREEN);
    workarea_rect.bottom = GetSystemMetrics(SM_CYFULLSCREEN);

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
    win_width = width + GetSystemMetrics(SM_CXFRAME) * 2;
    win_height = height + GetSystemMetrics(SM_CYFRAME) * 2
			+ GetSystemMetrics(SM_CYCAPTION)
#ifdef FEAT_MENU
			+ gui_w16_get_menu_height(FALSE)
#endif
			;

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

#ifdef FEAT_MENU
    /* Menu may wrap differently now */
    gui_w16_get_menu_height(!gui.starting);
#endif
}

    void
gui_mch_get_screen_dimensions(int *screen_w, int *screen_h)
{

    *screen_w = GetSystemMetrics(SM_CXSCREEN)
	      - GetSystemMetrics(SM_CXFRAME) * 2;
    *screen_h = GetSystemMetrics(SM_CYSCREEN)
	      - GetSystemMetrics(SM_CYFRAME) * 2
	      - GetSystemMetrics(SM_CYCAPTION)
#ifdef FEAT_MENU
	      - gui_w16_get_menu_height(FALSE)
#endif
	      ;
}

    void
gui_mch_set_text_area_pos(int x, int y, int w, int h)
{
    SetWindowPos(s_textArea, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
#ifdef FEAT_TOOLBAR
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
    scrollbar_t     *sb,
    int		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);
}

    void
gui_mch_set_scrollbar_thumb(
    scrollbar_t     *sb,
    long	    val,
    long	    size,
    long	    max)
{
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

    SetScrollRange(sb->id, SB_CTL, 0, (int) max, FALSE);
    SetScrollPos(sb->id, SB_CTL, (int) val, TRUE);
}

    void
gui_mch_set_scrollbar_pos(
    scrollbar_t     *sb,
    int		    x,
    int		    y,
    int		    w,
    int		    h)
{
    SetWindowPos(sb->id, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    void
gui_mch_create_scrollbar(
    scrollbar_t	    *sb,
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
gui_mch_destroy_scrollbar(scrollbar_t *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get the character size of a font
 */
    static void
GetFontSize(GuiFont font)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    gui.char_width = tm.tmAveCharWidth + tm.tmOverhang;

    gui.char_height = tm.tmHeight
#ifndef MSWIN16_FASTTEXT
	+ p_linespace
#endif
	;


    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

    int
gui_mch_adjust_charsize(void)
{
    GetFontSize(gui.norm_font);
    return OK;
}

    static GuiFont
get_font_handle(LOGFONT *lf)
{
    HFONT   font = NULL;

    /* Load the font */
    font = CreateFontIndirect(lf);

    if (font == NULL)
	return NOFONT;

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
			 (LPCSTR)lf->lfFaceName,
			 (FONTENUMPROC)font_enumproc,
			 (LPARAM)lf);

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
	/* if name is "*", bring up std font dialog: */
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
#ifndef MSWIN16_FASTTEXT
		lf->lfWeight = FW_BOLD;
#endif
		break;
	    case 'i':
#ifndef MSWIN16_FASTTEXT
		lf->lfItalic = TRUE;
#endif
		break;
	    case 'u':
		lf->lfUnderline = TRUE;
		break;
	    case 's':
		lf->lfStrikeOut = TRUE;
		break;
	    case 'c':
		{
		    static struct charset_pair
		    {
			char	*name;
			BYTE	charset;
		    } charset_pairs[] =
		    {
			{"ANSI",	ANSI_CHARSET},
			{"CHINESEBIG5",	CHINESEBIG5_CHARSET},
			{"DEFAULT",	DEFAULT_CHARSET},
			{"HANGEUL",	HANGEUL_CHARSET},
			{"OEM",		OEM_CHARSET},
			{"SHIFTJIS",	SHIFTJIS_CHARSET},
			{"SYMBOL",	SYMBOL_CHARSET},
			{NULL,		0}
		    };
		    struct charset_pair *cp;

		    for (cp = charset_pairs; cp->name != NULL; ++cp)
			if (STRNCMP(p, cp->name, strlen(cp->name)) == 0)
			{
			    lf->lfCharSet = cp->charset;
			    p += strlen(cp->name);
			    break;
			}
		    if (cp->name == NULL)
		    {
			sprintf((char *)IObuff, _("Illegal charset name \"%s\" in font name \"%s\""), p, name);
			EMSG(IObuff);
			break;
		    }
		    break;
		}
	    default:
		sprintf((char *)IObuff,
			_("Illegal char '%c' in font name \"%s\""),
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
gui_mch_init_font(char_u *font_name, int fontset)
{
    LOGFONT	lf;
    GuiFont	font = NOFONT;
    char	*p;


    /* Load the font */
    if (get_logfont(&lf, font_name))
	font = get_font_handle(&lf);
    if (font == NOFONT)
	return FAIL;
    if (font_name == NULL)
	font_name = lf.lfFaceName;
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
#ifndef MSWIN16_FASTTEXT
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
#endif

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
    if (font == NOFONT && giveErrorIfMissing)
	EMSG2(_("Unknown font: %s"), name);
    return font;
}

/*
 * Set the current text font.
 */
    void
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
    SelectFont(s_hdc, gui.currFont);
}


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
    guicolor_t
gui_mch_get_color(char_u *name)
{
    typedef struct guicolor_tTable
    {
	char	    *name;
	COLORREF    color;
    } guicolor_tTable;

    static guicolor_tTable table[] =
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
	{"SYS_BTNFACE", COLOR_BTNFACE},
	{"SYS_BTNSHADOW", COLOR_BTNSHADOW},
	{"SYS_ACTIVEBORDER", COLOR_ACTIVEBORDER},
	{"SYS_ACTIVECAPTION", COLOR_ACTIVECAPTION},
	{"SYS_APPWORKSPACE", COLOR_APPWORKSPACE},
	{"SYS_BACKGROUND", COLOR_BACKGROUND},
	{"SYS_BTNTEXT", COLOR_BTNTEXT},
	{"SYS_CAPTIONTEXT", COLOR_CAPTIONTEXT},
	{"SYS_GRAYTEXT", COLOR_GRAYTEXT},
	{"SYS_HIGHLIGHT", COLOR_HIGHLIGHT},
	{"SYS_HIGHLIGHTTEXT", COLOR_HIGHLIGHTTEXT},
	{"SYS_INACTIVEBORDER", COLOR_INACTIVEBORDER},
	{"SYS_INACTIVECAPTION", COLOR_INACTIVECAPTION},
	{"SYS_INACTIVECAPTIONTEXT", COLOR_INACTIVECAPTIONTEXT},
	{"SYS_MENU", COLOR_MENU},
	{"SYS_MENUTEXT", COLOR_MENUTEXT},
	{"SYS_SCROLLBAR", COLOR_SCROLLBAR},
	{"SYS_WINDOW", COLOR_WINDOW},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT},
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
	    return (guicolor_t)-1;
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
	    return (guicolor_t)-1;

	fd = fopen((char *)fname, "rt");
	vim_free(fname);
	if (fd == NULL)
	    return (guicolor_t)-1;

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
		return (guicolor_t) RGB(r, g, b);
	    }
	}

	fclose(fd);
    }

    return (guicolor_t)-1;
}

/*
 * Set the current text foreground color.
 */
    void
gui_mch_set_fg_color(guicolor_t color)
{
    gui.currFgColor = color;
    SetTextColor(s_hdc, gui.currFgColor);
}

/*
 * Set the current text background color.
 */
    void
gui_mch_set_bg_color(guicolor_t color)
{
    if (gui.currBgColor == color)
	return;

    gui.currBgColor = color;
    SetBkColor(s_hdc, gui.currBgColor);
}



    void
gui_mch_draw_string(
    int		row,
    int		col,
    char_u	*s,
    int		len,
    int		flags)
{
#ifndef MSWIN16_FASTTEXT
    static int	*padding = NULL;
    static int	pad_size = 0;
    int		i;
#endif
    HPEN	hpen, old_pen;
    int		y;

#ifndef MSWIN16_FASTTEXT
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
     * Alternative: write the characters in opaque mode, since we have blocked
     * bold or italic fonts.
     */
    /* The OPAQUE mode and backcolour have already been set */
#endif
    /* The forecolor and font have already been set */

#ifndef MSWIN16_FASTTEXT

    if (pad_size != Columns || padding == NULL || padding[0] != gui.char_width)
    {
	vim_free(padding);
	pad_size = Columns;

	padding = (int *)alloc(pad_size * sizeof(int));
	if (padding != NULL)
	    for (i = 0; i < pad_size; i++)
		padding[i] = gui.char_width;
    }
#endif

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */
#ifndef MSWIN16_FASTTEXT
    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row), 0, NULL,
						     (char *)s, len, padding);
#else
    TextOut(s_hdc, TEXT_X(col), TEXT_Y(row), (char *)s, len);
#endif

    if (flags & DRAW_UNDERL)
    {
	hpen = CreatePen(PS_SOLID, 1, gui.currFgColor);
	old_pen = SelectObject(s_hdc, hpen);
	/* When p_linespace is 0, overwrite the bottom row of pixels.
	 * Otherwise put the line just below the character. */
	y = FILL_Y(row + 1) - 1;
#ifndef MSWIN16_FASTTEXT
	if (p_linespace > 1)
	    y -= p_linespace - 1;
#endif
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
gui_mch_flash(int msec)
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

    ui_delay((long) msec, TRUE);	/* wait */

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
gui_mch_draw_hollow_cursor(guicolor_t color)
{
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
    guicolor_t	color)
{
    HBRUSH	hbr;
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.left =
#ifdef FEAT_RIGHTLEFT
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
    UINT    vk;		/* Virtual key */
    char_u  string[3];
    int	    i;
    int	    modifiers = 0;
    int	    key;

    GetMessage(&msg, NULL, 0, 0);

#ifdef WIN16_FIND_REPLACE
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
	    string[0] = Ctrl_C;
	    add_to_input_buf(string, 1);
	}

	for (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    /* ignore VK_SPACE when ALT key pressed: system menu */
	    if (special_keys[i].key_sym == vk
		    && (vk != VK_SPACE || !(GetKeyState(VK_MENU) & 0x8000)))
	    {
#ifdef FEAT_MENU
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
		    string[0] = Ctrl_HAT;
		    add_to_input_buf(string, 1);
		}
		/* vk == 0xDB AZERTY for CTRL-'-', but CTRL-[ for * QWERTY! */
		else if (vk == 0xBD)	/* QWERTY for CTRL-'-' */
		{
		    string[0] = Ctrl__;
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

	if (s_need_activate) {
	    (void) SetActiveWindow(s_hwnd);
	    s_need_activate = FALSE;
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
    /* Use trick for fast rect clear */
    gui_mch_set_bg_color(gui.back_pixel);
    ExtTextOut(s_hdc, 0, 0, ETO_CLIPPED | ETO_OPAQUE, rcp, NULL, 0, NULL);
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
	#pragma warn -eff
	rc.left = FILL_X(0);
	#pragma warn +eff
	rc.right = FILL_X(Columns);
	rc.top = FILL_Y(row);
	rc.bottom = FILL_Y(gui.scroll_region_bot + 1);
	/* The SW_INVALIDATE is required when part of the window is covered or
	 * off-screen.	How do we avoid it when it's not needed? */
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
	#pragma warn -eff
	rc.left = FILL_X(0);
	#pragma warn +eff
	rc.right = FILL_X(Columns);
	rc.top = FILL_Y(row);
	rc.bottom = FILL_Y(gui.scroll_region_bot + 1);
	/* The SW_INVALIDATE is required when part of the window is covered or
	 * off-screen.	How do we avoid it when it's not needed? */
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
#ifdef FEAT_MENU
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

#ifdef FEAT_MENU
/*
 * Add a sub menu to the menu bar.
 */
    void
gui_mch_add_menu(
    vimmenu_t	*menu,
    int		pos)
{
    vimmenu_t	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    if (menu_is_menubar(menu->name))
    {
	InsertMenu((parent == NULL) ? s_menuBar : parent->submenu_id,
		(UINT)pos, MF_POPUP | MF_STRING | MF_BYPOSITION,
		(UINT)menu->submenu_id,  menu->name);
    }

    /* Fix window size if menu may have wrapped */
    if (parent == NULL)
	gui_w16_get_menu_height(!gui.starting);
}

    void
gui_mch_show_popupmenu_at(vimmenu_t *menu, int x, int y)
{
    (void)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(int)0,	    /*reserved param*/
	s_hwnd,
	NULL);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}

    void
gui_mch_show_popupmenu(vimmenu_t *menu)
{
    POINT mp;

    GetCursorPos((LPPOINT)&mp);
    gui_mch_show_popupmenu_at(menu, (int)mp.x, (int)mp.y);
}


    void
gui_make_popup(char_u *path_name)
{
    vimmenu_t	*menu = gui_find_menu(path_name);

    if (menu!=NULL)
    {
	/* Find the position of the current cursor */
	DWORD	temp_p;
	POINT	p;
	temp_p = GetDCOrg(s_hdc);
	p.x = LOWORD(temp_p);
	p.y = HIWORD(temp_p);
	if (curwin!=NULL)
	{
	    p.x+= TEXT_X(W_WINCOL(curwin) + curwin->w_wcol +1);
	    p.y+= TEXT_Y(W_WINROW(curwin) + curwin->w_wrow +1);
	}
	msg_scroll = FALSE;
	gui_mch_show_popupmenu_at(menu, (int)p.x, (int)p.y);
    }
}

/*
 * Add a menu item to a menu
 */
    void
gui_mch_add_menu_item(
    vimmenu_t	*menu,
    int		idx)
{
    vimmenu_t	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = NULL;

#ifdef FEAT_TOOLBAR
    if (STRCMP(parent->name, "ToolBar") == 0)
    {
	TBBUTTON newtb;

	vim_memset(&newtb, 0, sizeof(newtb));
	if (menu_is_separator(menu->name))
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
	SendMessage(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    else
#endif
    InsertMenu(parent->submenu_id, (UINT)idx,
		(menu_is_separator(menu->name) ? MF_SEPARATOR : MF_STRING)
							      | MF_BYPOSITION,
		(UINT)menu->id, menu->name);
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(vimmenu_t *menu)
{
    UINT i, j;
    char pants[80]; /*<VN> hack*/
#ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    if (menu->submenu_id == (HMENU)-1)
    {
	int iButton;

	iButton = SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX, (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    else
#endif
    {
	/*
	 * negri: horrible API bug when running 16-bit programs under Win9x or
	 * NT means that we can't use MF_BYCOMMAND for menu items which have
	 * submenus, including the top-level headings. We have to find the menu
	 * item and use MF_BYPOSITION instead. :-p
	 */
    if (menu->parent != NULL
	    && menu_is_popup(menu->parent->dname)
	    && menu->parent->submenu_id != NULL)
	RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
    else if (menu->submenu_id == NULL)
	RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
    else if (menu->parent != NULL)
    {
	i = GetMenuItemCount(menu->parent->submenu_id);
	for (j = 0; j < i; ++j)
	{
	    GetMenuString(menu->parent->submenu_id, j,
		    pants, 80, MF_BYPOSITION);
	    if (strcmp(pants, menu->name) == 0)
	    {
		RemoveMenu(menu->parent->submenu_id, j, MF_BYPOSITION);
		break;
	    }
	}
    }
    else
    {
	i = GetMenuItemCount(s_menuBar);
	for (j = 0; j < i; ++j)
	{
	    GetMenuString(s_menuBar, j, pants, 80, MF_BYPOSITION);
	    if (strcmp(pants, menu->name) == 0)
	    {
		RemoveMenu(s_menuBar, j, MF_BYPOSITION);
		break;
	    }
	}
    }

    if (menu->submenu_id != NULL)
	DestroyMenu(menu->submenu_id);
    }
    DrawMenuBar(s_hwnd);
}


/*
 * Make a menu either grey or not grey.
 */
    void
gui_mch_menu_grey(
    vimmenu_t *menu,
    int	    grey)
{
#ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    if (menu->submenu_id == (HMENU)-1)
    {
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
	    (WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0) );
    }
    else
#endif
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
    vimmenu_t *menu,
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

#endif /*FEAT_MENU*/

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
    guicolor_t	pixel;
{
    return (GetRValue(pixel)*3 + GetGValue(pixel)*6 + GetBValue(pixel)) / 10;
}

#if (defined(FEAT_SYN_HL) && defined(FEAT_EVAL)) || defined(PROTO)
/*
 * Return the RGB value of a pixel as "#RRGGBB".
 */
    char_u *
gui_mch_get_rgb(
    guicolor_t	pixel)
{
    static char_u retval[10];

    sprintf((char *)retval, "#%02x%02x%02x",
	    GetRValue(pixel), GetGValue(pixel), GetBValue(pixel));
    return retval;
}
#endif

#ifdef FEAT_BROWSE

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
    char_u	    fileBuf[MAXPATHL], *p;

    if (dflt == NULL)
	fileBuf[0] = '\0';
    else
    {
	STRNCPY(fileBuf, dflt, MAXPATHL - 1);
	fileBuf[MAXPATHL - 1] = NUL;
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
    fileStruct.nMaxFile = MAXPATHL;
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
     */
    fileStruct.Flags = (OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT
			| OFN_PATHMUSTEXIST | OFN_HIDEREADONLY);
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

#endif /* FEAT_BROWSE */

#if defined(FEAT_GUI_DIALOG) || defined(PROTO)
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
#endif


/* define some macros used to make the dialogue creation more readable */

#define add_string(s) strcpy((LPSTR)p, s); (LPSTR)p += (strlen((LPSTR)p) + 1)
#define add_word(x)		*p++ = (x)
#define add_byte(x)		*((LPSTR)p)++ = (x)
#define add_long(x)		*((LPDWORD)p)++ = (x)

#if defined(FEAT_GUI_DIALOG) || defined(PROTO)
/*
 * stuff for dialogs
 */

/*
 * The callback routine used by all the dialogs.  Very simple.	First,
 * acknowledges the INITDIALOG message so that Windows knows to do standard
 * dialog stuff (Return = default, Esc = cancel....) Second, if a button is
 * pressed, return that button's ID - IDCANCEL (2), which is the button's
 * number.
 */
	 static BOOL CALLBACK
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
	    SetFocus(GetDlgItem(hwnd, dialog_default_button + IDCANCEL));
	return (FALSE);
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

static const char_u dlg_icons[] = /* must match names in resource file */
{
    IDR_VIM,
    IDR_VIM_ERROR,
    IDR_VIM_ALERT,
    IDR_VIM_INFO,
    IDR_VIM_QUESTION
};

    int
gui_mch_dialog(
    int		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    int		 dfltbutton)
{
    FARPROC	dp;
    LPWORD	p, pnumitems;
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
    HFONT	oldFont;
    TEXTMETRIC	fontInfo;
    int		fontHeight;
    int		textWidth, minButtonWidth, messageWidth;
    int		maxDialogWidth;
    int		vertical;
    int		dlgPaddingX;
    int		dlgPaddingY;
    HGLOBAL	hglbDlgTemp;

#ifndef NO_CONSOLE
    /* Don't output anything in silent mode ("ex -s") */
    if (silent_mode)
	return dfltbutton;   /* return default option */
#endif

    if ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    /* allocate some memory for dialog template */
    /* TODO should compute this really*/

    hglbDlgTemp = GlobalAlloc(GHND,  DLG_ALLOC_SIZE);
    if (hglbDlgTemp == NULL)
	return -1;

    p = (LPWORD) GlobalLock(hglbDlgTemp);

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
    oldFont = SelectFont(hdc, GetStockObject(SYSTEM_FONT));
    dlgPaddingX = DLG_OLD_STYLE_PADDING_X;
    dlgPaddingY = DLG_OLD_STYLE_PADDING_Y;

    GetTextMetrics(hdc, &fontInfo);
    fontHeight = fontInfo.tmHeight;

    /* Minimum width for horizontal button */
    minButtonWidth = GetTextWidth(hdc, "Cancel", 6);

    /* Maximum width of a dialog, if possible */
    GetWindowRect(s_hwnd, &rect);
    maxDialogWidth = rect.right - rect.left
		     - GetSystemMetrics(SM_CXFRAME) * 2;
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
    lStyle = DS_MODALFRAME | WS_CAPTION | WS_VISIBLE ;

    add_long(lStyle);
    pnumitems = p;	/*save where the number of items must be stored*/
    add_byte(0);// NumberOfItems(will change later)
    add_word(10);
    add_word(10);
    add_word(PixelToDialogX(dlgwidth));

    // Dialog height.
    if (vertical)
	add_word(PixelToDialogY(msgheight + 2 * dlgPaddingY +
			      DLG_VERT_PADDING_Y + 2 * fontHeight * numButtons));
    else
	add_word(PixelToDialogY(msgheight + 3 * dlgPaddingY + 2 * fontHeight));

    add_byte(0);//menu
    add_byte(0);//class

    /* copy the title of the dialog */
    add_string(title ? title : ("Vim"VIM_VERSION_MEDIUM));


    pstart = tbuffer; //dflt_text
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
	 * the default!! Grrr.	Workaround: Make the default button the only
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
		    (WORD)(IDCANCEL + 1 + i), (BYTE)0x80, pstart);
	}
	else
	{
	    p = add_dialog_element(p,
		    BS_PUSHBUTTON | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(msgheight + 2 * dlgPaddingY),
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (BYTE)0x80, pstart);
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
	    0, (BYTE)0x82,
	    &dlg_icons[type]);


    /* Dialog message */
    p = add_dialog_element(p, SS_LEFT,
	    PixelToDialogX(2 * dlgPaddingX + DLG_ICON_WIDTH),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    1, (BYTE)0x82, message);

    *pnumitems += 2;

    SelectFont(hdc, oldFont);
    ReleaseDC(hwnd, hdc);
    dp = MakeProcInstance(dialog_callback, s_hinst);


    /* Let the dialog_callback() function know which button to make default */
    dialog_default_button = dfltbutton + 1;	/* Back to 1-based for this */

    /*show the dialog box modally and get a return value*/
    nchar = DialogBoxIndirect(
	    s_hinst,
	    (HGLOBAL) hglbDlgTemp,
	    s_hwnd,
	    (DLGPROC)dp);

    FreeProcInstance( dp );
    GlobalUnlock(hglbDlgTemp);
    GlobalFree(hglbDlgTemp);
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);


    return nchar;
}

/*
 * Put a simple element (basic class) onto a dialog template in memory.
 * return a pointer to where the next item should be added.
 *
 * parameters:
 *  lStyle = additional style flags
 *  x,y = x & y positions IN DIALOG UNITS
 *  w,h = width and height IN DIALOG UNITS
 *  Id	= ID used in messages
 *  clss  = class ID, e.g 0x80 for a button, 0x82 for a static
 *  caption = usually text or resource name
 *
 *  TODO: use the length information noted here to enable the dialog creation
 *  routines to work out more exactly how much memory they need to alloc.
 */
    static LPWORD
add_dialog_element(
    LPWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    BYTE clss,
    const char *caption)
{

    lStyle = lStyle | WS_VISIBLE | WS_CHILD;

    add_word(x);
    add_word(y);
    add_word(w);
    add_word(h);
    add_word(Id);
    add_long(lStyle);
    add_byte(clss);
    if (((lStyle & SS_ICON) != 0) && (clss == 0x82))
    {
	/* Use resource ID */
	add_byte(0xff);
	add_byte(*caption);
    }
    else
	add_string(caption);

    add_byte(0);    //# of extra bytes following


    return p;
}



#undef add_byte
#undef add_string
#undef add_long
#undef add_word

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

    yNew = rParent.top	+ ((hParent - hChild) /2);
    if (yNew < 0)
	yNew = 0;
    else if ((yNew+hChild) > hScreen)
	yNew = hScreen - hChild;

    return SetWindowPos(hwndChild, NULL, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif /* FEAT_GUI_DIALOG */


void
gui_mch_activate_window(void)
{
    (void)SetActiveWindow(s_hwnd);
}


    static void
get_dialog_font_metrics(void)
{
    DWORD	    dlgFontSize;
	dlgFontSize = GetDialogBaseUnits();	/* fall back to big old system*/
	s_dlgfntwidth = LOWORD(dlgFontSize);
	s_dlgfntheight = HIWORD(dlgFontSize);
}


#if defined(FEAT_TOOLBAR) || defined(PROTO)
#include "gui_w3~1.h"
/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    static void
initialise_toolbar(void)
{
    s_toolbarhwnd = CreateToolbar(
		    s_hwnd,
		    WS_CHILD | WS_VISIBLE,
		    CMD_TB_BASE, /*<vn>*/
		    31,			//number of images in inital bitmap
		    s_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    NULL,
		    0			// initial number of buttons
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
    "WinVSplit",	//28
    "WinMaxWidth",	//29
    "WinMinWidth",	//30
    NULL
};
    static int
get_toolbar_bitmap(char_u *name)
{
    int i = -1;

    if (STRNCMP(name, "BuiltIn", 7) == 0)
    {
	char *dummy;
	/*
	 * reference by index
	 */
	i = strtol( name + 7, &dummy, 0);
	return i;
    }

    for (i = 0; BuiltInBitmaps[i]; i++)
    {
	if (STRCMP(name, BuiltInBitmaps[i]) == 0)
	    return i;
    }
    return i;
}
#endif

    void
gui_simulate_alt_key(exarg_t *eap)
{
    char_u *keys = eap->arg;

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    while (*keys)
    {
	if (*keys == '~')
	    *keys = ' ';	    /* for showing system menu */
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*keys, (LPARAM)0);
	keys++;
    }
}


    void
gui_mch_find_dialog(exarg_t *eap)
{
#ifdef WIN16_FIND_REPLACE
    if (s_findrep_msg != 0)
    {
	if (IsWindow(s_findrep_hwnd) && (s_findrep_is_find == FALSE))
	    DestroyWindow(s_findrep_hwnd);

	if (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
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
gui_mch_replace_dialog(exarg_t *eap)
{
#ifdef WIN16_FIND_REPLACE
    if (s_findrep_msg != 0)
    {
	if (IsWindow(s_findrep_hwnd) && (s_findrep_is_find == TRUE))
	    DestroyWindow(s_findrep_hwnd);

	if (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceText((LPFINDREPLACE) &s_findrep_struct);
	}

	(void)SetWindowText(s_findrep_hwnd,
			    (LPCSTR) "Find & Replace (use '\\\\' to find  a '\\')");
	(void)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}

#ifdef WIN16_FIND_REPLACE
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

#ifdef FEAT_MOUSESHAPE
/* Table for shape IDCs.  Keep in sync with the mshape_names[] table in
 * misc2.c! */
static LPCSTR mshape_idcs[] =
{
    MAKEINTRESOURCE(IDC_ARROW),		/* arrow */
    MAKEINTRESOURCE(0),			/* blank */
    MAKEINTRESOURCE(IDC_IBEAM),		/* beam */
    MAKEINTRESOURCE(IDC_SIZENS),	/* updown */
    MAKEINTRESOURCE(IDC_SIZENS),	/* udsizing */
    MAKEINTRESOURCE(IDC_SIZEWE),	/* leftright */
    MAKEINTRESOURCE(IDC_SIZEWE),	/* lrsizing */
    MAKEINTRESOURCE(IDC_WAIT),		/* busy */
    MAKEINTRESOURCE(IDC_ICON),		/* no */
    MAKEINTRESOURCE(IDC_ARROW),		/* crosshair */
    MAKEINTRESOURCE(IDC_ARROW),		/* hand1 */
    MAKEINTRESOURCE(IDC_ARROW),		/* hand2 */
    MAKEINTRESOURCE(IDC_ARROW),		/* pencil */
    MAKEINTRESOURCE(IDC_ARROW),		/* question */
    MAKEINTRESOURCE(IDC_ARROW),		/* right-arrow */
    MAKEINTRESOURCE(IDC_UPARROW),	/* up-arrow */
    MAKEINTRESOURCE(IDC_ARROW)		/* last one */
};

    void
mch_set_mouse_shape(int shape)
{
    LPCSTR idc;

    if (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    else
    {
	if (!p_mh)
	    ShowCursor(TRUE);
	if (shape >= MSHAPE_NUMBERED)
	    idc = MAKEINTRESOURCE(IDC_ARROW);
	else
	    idc = mshape_idcs[shape];
	SetClassWord(s_textArea, GCW_HCURSOR, LoadCursor(NULL, idc));
    }
}
#endif
