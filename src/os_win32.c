/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_win32.c
 *
 * Used for both the console version and the Win32 GUI.  A lot of code is for
 * the console version only, so there is a lot of "#ifndef FEAT_GUI_W32".
 *
 * Win32 (Windows NT and Windows 95) system-dependent routines.
 * Portions lifted from the Win32 SDK samples, the MSDOS-dependent code,
 * NetHack 3.1.3, GNU Emacs 19.30, and Vile 5.5.
 *
 * George V. Reilly <george@reilly.org> wrote most of this.
 * Roger Knobbe <rogerk@wonderware.com> did the initial port of Vim 3.0.
 */

#include <io.h>
#include "vim.h"

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
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

#ifndef FEAT_GUI_W32
/* Undocumented API in kernel32.dll needed to work around dead key bug in
 * console-mode applications in NT 4.0.  If you switch keyboard layouts
 * in a console app to a layout that includes dead keys and then hit a
 * dead key, a call to ToAscii will trash the stack.  My thanks to Ian James
 * and Michael Dietrich for helping me figure out this workaround.
 */

/* WINBASEAPI BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR); */
#ifndef WINBASEAPI
# define WINBASEAPI __stdcall
#endif
#if defined(__BORLANDC__)
typedef BOOL (__stdcall *PFNGCKLN)(LPSTR);
#else
typedef WINBASEAPI BOOL (WINAPI *PFNGCKLN)(LPSTR);
#endif
PFNGCKLN    s_pfnGetConsoleKeyboardLayoutName = NULL;
#endif


#ifndef FEAT_GUI_W32
/* Win32 Console handles for input and output */
# ifndef FEAT_SNIFF	/* used in if_sniff.c */
static
# endif
       HANDLE g_hConIn	= INVALID_HANDLE_VALUE;
static HANDLE g_hConOut = INVALID_HANDLE_VALUE;

/* Win32 Screen buffer,coordinate,console I/O information */
static SMALL_RECT g_srScrollRegion;
static COORD	  g_coord;  /* 0-based, but external coords are 1-based */

/* The attribute of the screen when the editor was started */
static WORD  g_attrDefault = 7;  /* lightgray text on black background */
static WORD  g_attrCurrent;

static int g_fCBrkPressed = FALSE;  /* set by ctrl-break interrupt */
static int g_fCtrlCPressed = FALSE; /* set when ctrl-C or ctrl-break detected */
static char_u g_chPending = NUL;

static void termcap_mode_start(void);
static void termcap_mode_end(void);
static void clear_chars(COORD coord, DWORD n);
static void clear_screen(void);
static void clear_to_end_of_display(void);
static void clear_to_end_of_line(void);
static void scroll(unsigned cLines);
static void set_scroll_region(unsigned left, unsigned top,
			      unsigned right, unsigned bottom);
static void insert_lines(unsigned cLines);
static void delete_lines(unsigned cLines);
static void gotoxy(unsigned x, unsigned y);
static void normvideo(void);
static void textattr(WORD wAttr);
static void textcolor(WORD wAttr);
static void textbackground(WORD wAttr);
static void standout(void);
static void standend(void);
static void visual_bell(void);
static void cursor_visible(BOOL fVisible);
static BOOL write_chars(LPCSTR pchBuf, DWORD cchToWrite, DWORD* pcchWritten);
static char_u tgetch(void);
static void create_conin(void);
static int s_cursor_visible = TRUE;
static int did_create_conin = FALSE;
#else
static int s_dont_use_vimrun = TRUE;
static char *vimrun_path = "vimrun ";
#endif

#ifndef FEAT_GUI_W32
static int suppress_winsize = 1;	/* don't fiddle with console */
#endif

/* This symbol is not defined in older versions of the SDK or Visual C++ */

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

DWORD g_PlatformId;

/*
 * These are needed to dynamically load the ADVAPI DLL, which is not
 * implemented under Windows 95 (and causes VIM to crash)
 */
typedef DWORD (WINAPI *PSNSECINFO) (LPTSTR, enum SE_OBJECT_TYPE,
	SECURITY_INFORMATION, PSID, PSID, PACL, PACL);
typedef DWORD (WINAPI *PGNSECINFO) (LPSTR, enum SE_OBJECT_TYPE,
	SECURITY_INFORMATION, PSID *, PSID *, PACL *, PACL *,
	PSECURITY_DESCRIPTOR *);
static HANDLE advapi_lib = NULL;	/* Handle for ADVAPI library */
static PSNSECINFO pSetNamedSecurityInfo;
static PGNSECINFO pGetNamedSecurityInfo;

/*
 * Set g_PlatformId to VER_PLATFORM_WIN32_NT (NT) or
 * VER_PLATFORM_WIN32_WINDOWS (Win95).
 */
    void
PlatformId(void)
{
    static int done = FALSE;

    if (!done)
    {
	OSVERSIONINFO ovi;

	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	g_PlatformId = ovi.dwPlatformId;

	/*
	 * Load the ADVAPI runtime if we are on anything
	 * other than Windows 95
	 */
	if (g_PlatformId == VER_PLATFORM_WIN32_NT)
	{
	    /*
	     * do this load.  Problems: Doesn't unload at end of run (this is
	     * theoretically okay, since Windows should unload it when VIM
	     * terminates).  Should we be using the 'mch_libcall' routines?
	     * Seems like a lot of overhead to load/unload ADVAPI32.DLL each
	     * time we verify security...
	     */
	    advapi_lib = LoadLibrary("ADVAPI32.DLL");
	    if (advapi_lib != NULL)
	    {
		pSetNamedSecurityInfo = (PSNSECINFO)GetProcAddress(advapi_lib,
						      "SetNamedSecurityInfoA");
		pGetNamedSecurityInfo = (PGNSECINFO)GetProcAddress(advapi_lib,
						      "GetNamedSecurityInfoA");
		if (pSetNamedSecurityInfo == NULL
			|| pGetNamedSecurityInfo == NULL)
		{
		    /* If we can't get the function addresses, set advapi_lib
		     * to NULL so that we don't use them. */
		    FreeLibrary(advapi_lib);
		    advapi_lib = NULL;
		}
	    }
	}
	done = TRUE;
    }
}

/*
 * Return TRUE when running on Windows 95.  Only to be used after
 * mch_shellinit().
 */
    int
mch_windows95(void)
{
    return g_PlatformId == VER_PLATFORM_WIN32_WINDOWS;
}

#ifdef FEAT_GUI_W32
/*
 * Used to work around the "can't do synchronous spawn"
 * problem on Win32s, without resorting to Universal Thunk.
 */
static int old_num_windows;
static int num_windows;

    static BOOL CALLBACK
win32ssynch_cb(HWND hwnd, LPARAM lparam)
{
    num_windows++;
    return TRUE;
}
#endif

#ifndef FEAT_GUI_W32

#define SHIFT  (SHIFT_PRESSED)
#define CTRL   (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)
#define ALT    (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED)
#define ALT_GR (RIGHT_ALT_PRESSED  | LEFT_CTRL_PRESSED)


/* When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
 * We map function keys to their ANSI terminal equivalents, as produced
 * by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
 * ANSI key with a value >= '\300' is nonstandard, but provided anyway
 * so that the user can have access to all SHIFT-, CTRL-, and ALT-
 * combinations of function/arrow/etc keys.
 */

const static struct
{
    WORD    wVirtKey;
    BOOL    fAnsiKey;
    int	    chAlone;
    int	    chShift;
    int	    chCtrl;
    int	    chAlt;
} VirtKeyMap[] =
{

/*    Key	ANSI	alone	shift	ctrl	    alt */
    { VK_ESCAPE,FALSE,	ESC,	ESC,	ESC,	    ESC,    },

    { VK_F1,	TRUE,	';',	'T',	'^',	    'h', },
    { VK_F2,	TRUE,	'<',	'U',	'_',	    'i', },
    { VK_F3,	TRUE,	'=',	'V',	'`',	    'j', },
    { VK_F4,	TRUE,	'>',	'W',	'a',	    'k', },
    { VK_F5,	TRUE,	'?',	'X',	'b',	    'l', },
    { VK_F6,	TRUE,	'@',	'Y',	'c',	    'm', },
    { VK_F7,	TRUE,	'A',	'Z',	'd',	    'n', },
    { VK_F8,	TRUE,	'B',	'[',	'e',	    'o', },
    { VK_F9,	TRUE,	'C',	'\\',	'f',	    'p', },
    { VK_F10,	TRUE,	'D',	']',	'g',	    'q', },
    { VK_F11,	TRUE,	'\205',	'\207',	'\211',	    '\213', },
    { VK_F12,	TRUE,	'\206',	'\210',	'\212',	    '\214', },

    { VK_HOME,	TRUE,	'G',	'\302',	'w',	    '\303', },
    { VK_UP,	TRUE,	'H',	'\304',	'\305',	    '\306', },
    { VK_PRIOR,	TRUE,	'I',	'\307',	'\204',	    '\310', }, /*PgUp*/
    { VK_LEFT,	TRUE,	'K',	'\311',	's',	    '\312', },
    { VK_RIGHT,	TRUE,	'M',	'\313',	't',	    '\314', },
    { VK_END,	TRUE,	'O',	'\315',	'u',	    '\316', },
    { VK_DOWN,	TRUE,	'P',	'\317',	'\320',	    '\321', },
    { VK_NEXT,	TRUE,	'Q',	'\322',	'v',	    '\323', }, /*PgDn*/
    { VK_INSERT,TRUE,	'R',	'\324',	'\325',	    '\326', },
    { VK_DELETE,TRUE,	'S',	'\327',	'\330',	    '\331', },

    { VK_SNAPSHOT,TRUE,	0,	0,	0,	    'r', }, /*PrtScrn*/

#if 0
    /* Most people don't have F13-F20, but what the hell... */
    { VK_F13,	TRUE,	'\332',	'\333',	'\334',	    '\335', },
    { VK_F14,	TRUE,	'\336',	'\337',	'\340',	    '\341', },
    { VK_F15,	TRUE,	'\342',	'\343',	'\344',	    '\345', },
    { VK_F16,	TRUE,	'\346',	'\347',	'\350',	    '\351', },
    { VK_F17,	TRUE,	'\352',	'\353',	'\354',	    '\355', },
    { VK_F18,	TRUE,	'\356',	'\357',	'\360',	    '\361', },
    { VK_F19,	TRUE,	'\362',	'\363',	'\364',	    '\365', },
    { VK_F20,	TRUE,	'\366',	'\367',	'\370',	    '\371', },
#endif
    { VK_ADD,	TRUE,   'N',    'N',    'N',	'N',	}, /* keyp '+' */
    { VK_SUBTRACT, TRUE,'J',	'J',    'J',	'J',	}, /* keyp '-' */
 /* { VK_DIVIDE,   TRUE,'N',	'N',    'N',	'N',	},    keyp '/' */
    { VK_MULTIPLY, TRUE,'7',	'7',    '7',	'7',	}, /* keyp '*' */

    { VK_NUMPAD0,TRUE,  '\332',	'\333',	'\334',	    '\335', },
    { VK_NUMPAD1,TRUE,  '\336',	'\337',	'\340',	    '\341', },
    { VK_NUMPAD2,TRUE,  '\342',	'\343',	'\344',	    '\345', },
    { VK_NUMPAD3,TRUE,  '\346',	'\347',	'\350',	    '\351', },
    { VK_NUMPAD4,TRUE,  '\352',	'\353',	'\354',	    '\355', },
    { VK_NUMPAD5,TRUE,  '\356',	'\357',	'\360',	    '\361', },
    { VK_NUMPAD6,TRUE,  '\362',	'\363',	'\364',	    '\365', },
    { VK_NUMPAD7,TRUE,  '\366',	'\367',	'\370',	    '\371', },
    { VK_NUMPAD8,TRUE,  '\372',	'\373',	'\374',	    '\375', },
    /* Sorry, out of number space! <negri>*/
    { VK_NUMPAD9,TRUE,  '\376',	'\377',	'\377',	    '\367', },

};


// The ToAscii bug destroys several registers.	Need to turn off optimization
// or the GetConsoleKeyboardLayoutName hack will fail in non-debug versions

#pragma optimize("", off)

#if defined(__GNUC__) && !defined(__MINGW32__)
#define AChar AsciiChar
#else
#define AChar uChar.AsciiChar
#endif

/* The return code indicates key code size. */
    static int
#ifdef __BORLANDC__
    __stdcall
#endif

win32_kbd_patch_key(
    KEY_EVENT_RECORD* pker)
{
    UINT uMods = pker->dwControlKeyState;
    static int s_iIsDead = 0;
    static WORD awAnsiCode[2];
    static BYTE abKeystate[256];


    if (s_iIsDead == 2)
    {
	pker->AChar = (CHAR) awAnsiCode[1];
	s_iIsDead = 0;
	return 1;
    }

    if (pker->AChar != 0)
	return 1;

    memset(abKeystate, 0, sizeof (abKeystate));

    // Should only be non-NULL on NT 4.0
    if (s_pfnGetConsoleKeyboardLayoutName != NULL)
    {
	CHAR szKLID[KL_NAMELENGTH];

	if ((*s_pfnGetConsoleKeyboardLayoutName)(szKLID))
	    (void)LoadKeyboardLayout(szKLID, KLF_ACTIVATE);
    }

    /* Clear any pending dead keys */
    ToAscii(VK_SPACE, MapVirtualKey(VK_SPACE, 0), abKeystate, awAnsiCode, 0);

    if (uMods & SHIFT_PRESSED)
	abKeystate[VK_SHIFT] = 0x80;
    if (uMods & CAPSLOCK_ON)
	abKeystate[VK_CAPITAL] = 1;

    if ((uMods & ALT_GR) == ALT_GR)
    {
	abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
	    abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
    }

    s_iIsDead = ToAscii(pker->wVirtualKeyCode, pker->wVirtualScanCode,
			abKeystate, awAnsiCode, 0);

    if (s_iIsDead > 0)
	pker->AChar = (CHAR) awAnsiCode[0];

    return s_iIsDead;
}

/* MUST switch optimization on again here, otherwise a call to
 * decode_key_event() may crash (e.g. when hitting caps-lock) */
#pragma optimize("", on)

#if (_MSC_VER < 1100)
/* MUST turn off global optimisation for this next function, or
 * pressing ctrl-minus in insert mode crashes Vim when built with
 * VC4.1. -- negri. */
#pragma optimize("g", off)
#endif

static BOOL g_fJustGotFocus = FALSE;

/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
    static BOOL
decode_key_event(
    KEY_EVENT_RECORD	*pker,
    char_u		*pch,
    char_u		*pchPending,
    BOOL		fDoPost)
{
    int i;
    const int nModifs = pker->dwControlKeyState & (SHIFT | ALT | CTRL);

    *pch = *pchPending = NUL;
    g_fJustGotFocus = FALSE;

    /* ignore key up events */
    if (!pker->bKeyDown)
	return FALSE;

    /* ignore some keystrokes */
    switch (pker->wVirtualKeyCode)
    {
    /* modifiers */
    case VK_SHIFT:
    case VK_CONTROL:
    case VK_MENU:   /* Alt key */
	return FALSE;

    default:
	break;
    }

    /* special cases */
    if ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0 && pker->AChar == NUL)
    {
	/* Ctrl-6 is Ctrl-^ */
	if (pker->wVirtualKeyCode == '6')
	{
	    *pch = Ctrl_HAT;
	    return TRUE;
	}
	/* Ctrl-2 is Ctrl-@ */
	else if (pker->wVirtualKeyCode == '2')
	{
	    *pch = NUL;
	    return TRUE;
	}
	/* Ctrl-- is Ctrl-_ */
	else if (pker->wVirtualKeyCode == 0xBD)
	{
	    *pch = Ctrl__;
	    return TRUE;
	}
    }

    /* Shift-TAB */
    if (pker->wVirtualKeyCode == VK_TAB && (nModifs & SHIFT_PRESSED))
    {
	*pch = K_NUL;
	*pchPending = '\017';
	return TRUE;
    }

    for (i = sizeof(VirtKeyMap) / sizeof(VirtKeyMap[0]);  --i >= 0;  )
    {
	if (VirtKeyMap[i].wVirtKey == pker->wVirtualKeyCode)
	{
	    if (nModifs == 0)
		*pch = VirtKeyMap[i].chAlone;
	    else if ((nModifs & SHIFT) != 0 && (nModifs & ~SHIFT) == 0)
		*pch = VirtKeyMap[i].chShift;
	    else if ((nModifs & CTRL) != 0 && (nModifs & ~CTRL) == 0)
		*pch = VirtKeyMap[i].chCtrl;
	    else if ((nModifs & ALT) != 0 && (nModifs & ~ALT) == 0)
		*pch = VirtKeyMap[i].chAlt;

	    if (*pch != 0)
	    {
		if (VirtKeyMap[i].fAnsiKey)
		{
		    *pchPending = *pch;
		    *pch = K_NUL;
		}

		return TRUE;
	    }
	}
    }

    i = win32_kbd_patch_key(pker);

    if (i < 0)
	*pch = NUL;
    else
    {
	*pch = (i > 0)	? pker->AChar  :  NUL;
	/* Interpret the ALT key as making the key META, but only when not
	 * combined with CTRL (which is ALTGR). */
	if ((nModifs & ALT) != 0 && (nModifs & CTRL) == 0)
	    *pch |= 0x80;
    }

    return (*pch != NUL);
}
#pragma optimize("", on)
#endif /* FEAT_GUI_W32 */


#ifdef FEAT_MOUSE

/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
# ifdef FEAT_GUI_W32
    void
mch_setmouse(
    int on)
{
}
# else
static int g_fMouseAvail = FALSE;   /* mouse present */
static int g_fMouseActive = FALSE;  /* mouse enabled */
static int g_nMouseClick = -1;	    /* mouse status */
static int g_xMouse;		    /* mouse x coordinate */
static int g_yMouse;		    /* mouse y coordinate */

/*
 * Enable or disable mouse input
 */
    void
mch_setmouse(
    int on)
{
    DWORD cmodein;

    if (! g_fMouseAvail)
	return;

    g_fMouseActive = on;
    GetConsoleMode(g_hConIn, &cmodein);

    if (g_fMouseActive)
	cmodein |= ENABLE_MOUSE_INPUT;
    else
	cmodein &= ~ENABLE_MOUSE_INPUT;

    SetConsoleMode(g_hConIn, cmodein);
}


/*
 * Decode a MOUSE_EVENT.  If it's a valid event, return MOUSE_LEFT,
 * MOUSE_MIDDLE, or MOUSE_RIGHT for a click; MOUSE_DRAG for a mouse
 * move with a button held down; and MOUSE_RELEASE after a MOUSE_DRAG
 * or a MOUSE_LEFT, _MIDDLE, or _RIGHT.  We encode the button type,
 * the number of clicks, and the Shift/Ctrl/Alt modifiers in g_nMouseClick,
 * and we return the mouse position in g_xMouse and g_yMouse.
 *
 * Every MOUSE_LEFT, _MIDDLE, or _RIGHT will be followed by zero or more
 * MOUSE_DRAGs and one MOUSE_RELEASE.  MOUSE_RELEASE will be followed only
 * by MOUSE_LEFT, _MIDDLE, or _RIGHT.
 *
 * For multiple clicks, we send, say, MOUSE_LEFT/1 click, MOUSE_RELEASE,
 * MOUSE_LEFT/2 clicks, MOUSE_RELEASE, MOUSE_LEFT/3 clicks, MOUSE_RELEASE, ....
 *
 * Windows will send us MOUSE_MOVED notifications whenever the mouse
 * moves, even if it stays within the same character cell.  We ignore
 * all MOUSE_MOVED messages if the position hasn't really changed, and
 * we ignore all MOUSE_MOVED messages where no button is held down (i.e.,
 * we're only interested in MOUSE_DRAG).
 *
 * All of this is complicated by the code that fakes MOUSE_MIDDLE on
 * 2-button mouses by pressing the left & right buttons simultaneously.
 * In practice, it's almost impossible to click both at the same time,
 * so we need to delay a little.  Also, we tend not to get MOUSE_RELEASE
 * in such cases, if the user is clicking quickly.
 */
    static BOOL
decode_mouse_event(
    MOUSE_EVENT_RECORD* pmer)
{
    static int s_nOldButton = -1;
    static int s_nOldMouseClick = -1;
    static int s_xOldMouse = -1;
    static int s_yOldMouse = -1;
    static linenr_t s_old_topline = 0;
    static int s_cClicks = 1;
    static BOOL s_fReleased = TRUE;
    static s_dwLastClickTime = 0;
    static BOOL s_fNextIsMiddle = FALSE;

    const DWORD LEFT = FROM_LEFT_1ST_BUTTON_PRESSED;
    const DWORD MIDDLE = FROM_LEFT_2ND_BUTTON_PRESSED;
    const DWORD RIGHT = RIGHTMOST_BUTTON_PRESSED;
    const DWORD LEFT_RIGHT = LEFT | RIGHT;

    int nButton;

    if (! g_fMouseAvail || !g_fMouseActive)
    {
	g_nMouseClick = -1;
	return FALSE;
    }

    /* get a spurious MOUSE_EVENT immediately after receiving focus; ignore */
    if (g_fJustGotFocus)
    {
	g_fJustGotFocus = FALSE;
	return FALSE;
    }

    /* unprocessed mouse click? */
    if (g_nMouseClick != -1)
	return TRUE;

    nButton = -1;
    g_xMouse = pmer->dwMousePosition.X;
    g_yMouse = pmer->dwMousePosition.Y;

    if (pmer->dwEventFlags == MOUSE_MOVED)
    {
	/* ignore MOUSE_MOVED events if (x, y) hasn't changed.	(We get these
	 * events even when the mouse moves only within a char cell.) */
	if (s_xOldMouse == g_xMouse && s_yOldMouse == g_yMouse)
	    return FALSE;
    }

    /* If no buttons are pressed... */
    if (pmer->dwButtonState == 0)
    {
	/* If the last thing returned was MOUSE_RELEASE, ignore this */
	if (s_fReleased)
	    return FALSE;

	nButton = MOUSE_RELEASE;
	s_fReleased = TRUE;
    }
    else    /* one or more buttons pressed */
    {
	const int cButtons = GetSystemMetrics(SM_CMOUSEBUTTONS);

	/* on a 2-button mouse, hold down left and right buttons
	 * simultaneously to get MIDDLE. */

	if (cButtons == 2 && s_nOldButton != MOUSE_DRAG)
	{
	    DWORD dwLR = (pmer->dwButtonState & LEFT_RIGHT);

	    /* if either left or right button only is pressed, see if the
	     * the next mouse event has both of them pressed */
	    if (dwLR == LEFT || dwLR == RIGHT)
	    {
		for (;;)
		{
		    /* wait a short time for next input event */
		    if (WaitForSingleObject(g_hConIn, p_mouset / 3)
			!= WAIT_OBJECT_0)
			break;
		    else
		    {
			DWORD cRecords = 0;
			INPUT_RECORD ir;
			MOUSE_EVENT_RECORD* pmer2 = &ir.Event.MouseEvent;

			PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);

			if (cRecords == 0 || ir.EventType != MOUSE_EVENT
				|| !(pmer2->dwButtonState & LEFT_RIGHT))
			    break;
			else
			{
			    if (pmer2->dwEventFlags != MOUSE_MOVED)
			    {
				ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);

				return decode_mouse_event(pmer2);
			    }
			    else if (s_xOldMouse == pmer2->dwMousePosition.X &&
				     s_yOldMouse == pmer2->dwMousePosition.Y)
			    {
				/* throw away spurious mouse move */
				ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);

				/* are there any more mouse events in queue? */
				PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);

				if (cRecords==0 || ir.EventType != MOUSE_EVENT)
				    break;
			    }
			    else
				break;
			}
		    }
		}
	    }
	}

	if (s_fNextIsMiddle)
	{
	    nButton = (pmer->dwEventFlags == MOUSE_MOVED)
		? MOUSE_DRAG : MOUSE_MIDDLE;
	    s_fNextIsMiddle = FALSE;
	}
	else if (cButtons == 2	&&
	    ((pmer->dwButtonState & LEFT_RIGHT) == LEFT_RIGHT))
	{
	    nButton = MOUSE_MIDDLE;

	    if (! s_fReleased && pmer->dwEventFlags != MOUSE_MOVED)
	    {
		s_fNextIsMiddle = TRUE;
		nButton = MOUSE_RELEASE;
	    }
	}
	else if ((pmer->dwButtonState & LEFT) == LEFT)
	    nButton = MOUSE_LEFT;
	else if ((pmer->dwButtonState & MIDDLE) == MIDDLE)
	    nButton = MOUSE_MIDDLE;
	else if ((pmer->dwButtonState & RIGHT) == RIGHT)
	    nButton = MOUSE_RIGHT;

	if (! s_fReleased && ! s_fNextIsMiddle
		&& nButton != s_nOldButton && s_nOldButton != MOUSE_DRAG)
	    return FALSE;

	s_fReleased = s_fNextIsMiddle;
    }

    if (pmer->dwEventFlags == 0 || pmer->dwEventFlags == DOUBLE_CLICK)
    {
	/* button pressed or released, without mouse moving */
	if (nButton != -1 && nButton != MOUSE_RELEASE)
	{
	    DWORD dwCurrentTime = GetTickCount();

	    if (s_xOldMouse != g_xMouse
		    || s_yOldMouse != g_yMouse
		    || s_nOldButton != nButton
		    || s_old_topline != curwin->w_topline
		    || (int)(dwCurrentTime - s_dwLastClickTime) > p_mouset)
	    {
		s_cClicks = 1;
	    }
	    else if (++s_cClicks > 4)
	    {
		s_cClicks = 1;
	    }

	    s_dwLastClickTime = dwCurrentTime;
	}
    }
    else if (pmer->dwEventFlags == MOUSE_MOVED)
    {
	if (nButton != -1 && nButton != MOUSE_RELEASE)
	    nButton = MOUSE_DRAG;

	s_cClicks = 1;
    }

    if (nButton == -1)
	return FALSE;

    if (nButton != MOUSE_RELEASE)
	s_nOldButton = nButton;

    g_nMouseClick = nButton;

    if (pmer->dwControlKeyState & SHIFT_PRESSED)
	g_nMouseClick |= MOUSE_SHIFT;
    if (pmer->dwControlKeyState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED))
	g_nMouseClick |= MOUSE_CTRL;
    if (pmer->dwControlKeyState & (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED))
	g_nMouseClick |= MOUSE_ALT;

    if (nButton != MOUSE_DRAG && nButton != MOUSE_RELEASE)
	SET_NUM_MOUSE_CLICKS(g_nMouseClick, s_cClicks);

    /* only pass on interesting (i.e., different) mouse events */
    if (s_xOldMouse == g_xMouse
	    && s_yOldMouse == g_yMouse
	    && s_nOldMouseClick == g_nMouseClick)
    {
	g_nMouseClick = -1;
	return FALSE;
    }

    s_xOldMouse = g_xMouse;
    s_yOldMouse = g_yMouse;
    s_old_topline = curwin->w_topline;
    s_nOldMouseClick = g_nMouseClick;

    return TRUE;
}

# endif /* FEAT_GUI_W32 */
#endif /* FEAT_MOUSE */


#ifdef MCH_CURSOR_SHAPE
/*
 * Set the shape of the cursor.
 * 'thickness' can be from 1 (thin) to 99 (block)
 */
    static void
mch_set_cursor_shape(int thickness)
{
    CONSOLE_CURSOR_INFO ConsoleCursorInfo;
    ConsoleCursorInfo.dwSize = thickness;
    ConsoleCursorInfo.bVisible = s_cursor_visible;

    SetConsoleCursorInfo(g_hConOut, &ConsoleCursorInfo);
}

    void
mch_update_cursor(void)
{
    int		idx;
    int		thickness;

    /*
     * How the cursor is drawn depends on the current mode.
     */
    idx = get_shape_idx(FALSE);

    if (shape_table[idx].shape == SHAPE_BLOCK)
	thickness = 99;	/* 100 doesn't work on W95 */
    else
	thickness = shape_table[idx].percentage;
    mch_set_cursor_shape(thickness);
}
#endif

#ifndef FEAT_GUI_W32	    /* this isn't used for the GUI */
/*
 * Handle FOCUS_EVENT.
 */
    static void
handle_focus_event(INPUT_RECORD ir)
{
    g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
    if (g_fJustGotFocus)
	check_timestamps(FALSE);
#ifdef FEAT_AUTOCMD
    apply_autocmds(g_fJustGotFocus ? EVENT_FOCUSGAINED
				: EVENT_FOCUSLOST, NULL, NULL, FALSE, curbuf);
#endif
}

/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up.
 * Return TRUE if something is available FALSE if not.
 */
    static int
WaitForChar(long msec)
{
    DWORD	    dwNow, dwEndTime;
    INPUT_RECORD    ir;
    DWORD	    cRecords;
    char_u	    ch, ch2;


    if (msec > 0)
	dwEndTime = GetTickCount() + msec;

    /* We need to loop until the end of the time period, because
     * we might get multiple unusable mouse events in that time.
     */
    for (;;)
    {
	if (g_chPending != NUL
#ifdef FEAT_MOUSE
				|| g_nMouseClick != -1
#endif
							    )
	    return TRUE;

	if (msec > 0)
	{
	    dwNow = GetTickCount();
	    if (dwNow >= dwEndTime)
		break;
	    if (WaitForSingleObject(g_hConIn, dwEndTime - dwNow)
							     != WAIT_OBJECT_0)
		continue;
	}

	cRecords = 0;
	PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);

	if (cRecords > 0)
	{
#ifdef FEAT_MBYTE_IME
	    /* Windows IME sends two '\n's with only one 'ENTER'.
	       first, wVirtualKeyCode == 13. second, wVirtualKeyCode == 0 */
	    if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown)
	    {
		if (ir.Event.KeyEvent.uChar.UnicodeChar == 0
			&& ir.Event.KeyEvent.wVirtualKeyCode == 13)
		{
		    ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
		    continue;
		}
		return decode_key_event(&ir.Event.KeyEvent, &ch, &ch2, FALSE);
	    }
#else
	    if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown
		    && decode_key_event(&ir.Event.KeyEvent, &ch, &ch2, FALSE))
		return TRUE;
#endif

	    ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);

	    if (ir.EventType == FOCUS_EVENT)
		handle_focus_event(ir);
	    else if (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
		shell_resized();
#ifdef FEAT_MOUSE
	    else if (ir.EventType == MOUSE_EVENT
		    && decode_mouse_event(&ir.Event.MouseEvent))
		return TRUE;
#endif
	}
	else if (msec == 0)
	    break;
    }

    return FALSE;
}

#ifndef FEAT_GUI_MSWIN
/*
 * return non-zero if a character is available
 */
    int
mch_char_avail()
{
    return WaitForChar(0L);
}
#endif

/*
 * Create the console input.  Used when reading stdin doesn't work.
 */
    static void
create_conin(void)
{
    g_hConIn =	CreateFile("CONIN$", GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES) NULL,
			OPEN_EXISTING, (DWORD)NULL, (HANDLE)NULL);
    did_create_conin = TRUE;
}

/*
 * Get a keystroke or a mouse event
 */
    static char_u
tgetch(void)
{
    char_u ch;

    if (g_chPending != NUL)
    {
	ch = g_chPending;
	g_chPending = NUL;
	return ch;
    }

    for ( ; ; )
    {
	INPUT_RECORD ir;
	DWORD cRecords = 0;

	if (ReadConsoleInput(g_hConIn, &ir, 1, &cRecords) == 0)
	{
	    if (did_create_conin)
		read_error_exit();
	    create_conin();
	    continue;
	}

	if (ir.EventType == KEY_EVENT)
	{
	    if (decode_key_event(&ir.Event.KeyEvent, &ch, &g_chPending, TRUE))
		return ch;
	}
	else if (ir.EventType == FOCUS_EVENT)
	    handle_focus_event(ir);
	else if (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
	{
	    shell_resized();
	}
#ifdef FEAT_MOUSE
	else if (ir.EventType == MOUSE_EVENT)
	{
	    if (decode_mouse_event(&ir.Event.MouseEvent))
		return 0;
	}
#endif
    }
}
#endif /* !FEAT_GUI_W32 */


/*
 * mch_inchar(): low-level input funcion.
 * Get one or more characters from the keyboard or the mouse.
 * If time == 0, do not wait for characters.
 * If time == n, wait a short time for characters.
 * If time == -1, wait forever for characters.
 * Returns the number of characters read into buf.
 */
    int
mch_inchar(
    char_u	*buf,
    int		maxlen,
    long	time)
{
#ifndef FEAT_GUI_W32	    /* this isn't used for the GUI */

    int		len = 0;
    int		c;
#ifdef FEAT_AUTOCMD
    static int	once_already = 0;
#endif


#ifdef FEAT_SNIFF
    if (sniff_request_waiting)
    {
	/* return K_SNIFF */
	*buf++ = '\233';
	*buf++ = 's';
	*buf++ = 'n';
	*buf++ = 'i';
	*buf++ = 'f';
	*buf++ = 'f';
	len += 6;
	sniff_request_waiting = 0;
	want_sniff_request = 0;
	return len;
    }
#endif

    if (time >= 0)
    {
	if (!WaitForChar(time))     /* no character available */
	{
#ifdef FEAT_AUTOCMD
	    once_already = 0;
#endif
	    return 0;
	}
    }
    else    /* time == -1, wait forever */
    {
#ifdef FEAT_AUTOCMD
	/* If there is no character available within 2 seconds (default),
	 * write the autoscript file to disk */
	if (once_already == 2)
	    updatescript(0);
	else if (once_already == 1)
	{
	    setcursor();
	    once_already = 2;
	    return 0;
	}
	else
#endif
	    if (!WaitForChar(p_ut))
	{
#ifdef FEAT_AUTOCMD
	    if (has_cursorhold() && get_real_state() == NORMAL_BUSY)
	    {
		apply_autocmds(EVENT_CURSORHOLD, NULL, NULL, FALSE, curbuf);
		update_screen(VALID);
		once_already = 1;
		return 0;
	    }
	    else
#endif
		updatescript(0);
	}
    }

    /*
     * Try to read as many characters as there are, until the buffer is full.
     */

    /* we will get at least one key. Get more if they are available. */
    g_fCBrkPressed = FALSE;

#ifdef MCH_WRITE_DUMP
    if (fdDump)
	fputc('[', fdDump);
#endif

    while ((len == 0 || WaitForChar(0)) && len < maxlen)
    {
#ifdef FEAT_MOUSE
	if (g_nMouseClick != -1 && maxlen - len >= 5)
	{
# ifdef MCH_WRITE_DUMP
	    if (fdDump)
		fprintf(fdDump, "{%02x @ %d, %d}",
			g_nMouseClick, g_xMouse, g_yMouse);
# endif

	    len += 5;
	    *buf++ = ESC + 128;
	    *buf++ = 'M';
	    *buf++ = g_nMouseClick;
	    *buf++ = g_xMouse + '!';
	    *buf++ = g_yMouse + '!';
	    g_nMouseClick = -1;

	}
	else
#endif /* FEAT_MOUSE */
	{
	    if ((c = tgetch()) == Ctrl_C)
		g_fCBrkPressed = TRUE;

#ifdef FEAT_MOUSE
	    if (g_nMouseClick == -1)
#endif
	    {
		*buf++ = c;
		len++;

#ifdef MCH_WRITE_DUMP
		if (fdDump)
		    fputc(c, fdDump);
#endif
	    }
	}
    }

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fputs("]\n", fdDump);
	fflush(fdDump);
    }
#endif

#ifdef FEAT_AUTOCMD
    once_already = 0;
#endif
    return len;
#else /* FEAT_GUI_W32 */
    return 0;
#endif /* FEAT_GUI_W32 */
}

#ifndef __MINGW32__
# include <shellapi.h>	/* required for FindExecutable() */
#endif

    static int
executable_exists(char *name)
{
    char location[2 * _MAX_PATH + 2];
    char widename[2 * _MAX_PATH];

    /* There appears to be a bug in FindExecutableA() on Windows NT.
     * Use FindExecutableW() instead... */
    if (g_PlatformId == VER_PLATFORM_WIN32_NT)
    {
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
						 (LPWSTR)widename, _MAX_PATH);
	if (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
					    (LPWSTR)location) > (HINSTANCE)32)
	    return TRUE;
    }
    else
    {
	if (FindExecutableA((LPCTSTR)name, (LPCTSTR)"",
					    (LPTSTR)location) > (HINSTANCE)32)
	    return TRUE;
    }
    return FALSE;
}

#ifdef FEAT_GUI_W32

/*
 * GUI version of mch_shellinit().
 */
    void
mch_shellinit()
{
#ifndef __MINGW32__
    extern int _fmode;
#endif

    /* Let critical errors result in a failure, not in a dialog box.  Required
     * for the timestamp test to work on removed floppies. */
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		/* we do our own CR-LF translation */

    /* Specify window size.  Is there a place to get the default from? */
    Rows = 25;
    Columns = 80;

    /* Look for 'vimrun' */
    if (!gui_is_win32s())
    {
	char_u vimrun_location[_MAX_PATH + 2];

	/* First try in same directory as gvim.exe */
	STRCPY(vimrun_location, exe_name);
	STRCPY(gettail(vimrun_location), "vimrun.exe");
	if (mch_getperm(vimrun_location) >= 0)
	{
	    STRCPY(gettail(vimrun_location), "vimrun ");
	    vimrun_path = (char *)vim_strsave(vimrun_location);
	    s_dont_use_vimrun = FALSE;
	}
	else if (executable_exists("vimrun.exe"))
	    s_dont_use_vimrun = FALSE;

	if (s_dont_use_vimrun)
	    MessageBox(NULL,
			_("VIMRUN.EXE not found in your $PATH.\n"
			"External commands will not pause after completion.\n"
			"See  :help win32-vimrun  for more information."),
			_("Vim Warning"),
			MB_ICONWARNING);
    }

    /*
     * If "finstr.exe" doesn't exist, use "grep -n" for 'grepprg'.
     * Otherwise the default "findstr /n" is used.
     */
    if (!executable_exists("findstr.exe"))
	set_option_value((char_u *)"grepprg", 0, (char_u *)"grep -n", FALSE);

#ifdef FEAT_CLIPBOARD
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line, or
     * rectangular block.  Only useful for copying between two Vims.
     */
    clipboard.format = RegisterClipboardFormat("VimClipboard");
#endif
}


#else /* FEAT_GUI_W32 */

#define SRWIDTH(sr) ((sr).Right - (sr).Left + 1)
#define SRHEIGHT(sr) ((sr).Bottom - (sr).Top + 1)

/*
 * ClearConsoleBuffer()
 * Description:
 *  Clears the entire contents of the console screen buffer, using the
 *  specified attribute.
 * Returns:
 *  TRUE on success
 */
    static BOOL
ClearConsoleBuffer(WORD wAttribute)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord;
    DWORD NumCells, dummy;

    if (!GetConsoleScreenBufferInfo(g_hConOut, &csbi))
	return FALSE;

    NumCells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    if (!FillConsoleOutputCharacter(g_hConOut, ' ', NumCells,
	    coord, &dummy))
    {
	return FALSE;
    }
    if (!FillConsoleOutputAttribute(g_hConOut, wAttribute, NumCells,
	    coord, &dummy))
    {
	return FALSE;
    }

    return TRUE;
}

/*
 * FitConsoleWindow()
 * Description:
 *  Checks if the console window will fit within given buffer dimensions.
 *  Also, if requested, will shrink the window to fit.
 * Returns:
 *  TRUE on success
 */
    static BOOL
FitConsoleWindow(
    COORD dwBufferSize,
    BOOL WantAdjust)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD dwWindowSize;
    BOOL NeedAdjust = FALSE;

    if (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	/*
	 * A buffer resize will fail if the current console window does
	 * not lie completely within that buffer.  To avoid this, we might
	 * have to move and possibly shrink the window.
	 */
	if (csbi.srWindow.Right >= dwBufferSize.X)
	{
	    dwWindowSize.X = SRWIDTH(csbi.srWindow);
	    if (dwWindowSize.X > dwBufferSize.X)
		dwWindowSize.X = dwBufferSize.X;
	    csbi.srWindow.Right = dwBufferSize.X - 1;
	    csbi.srWindow.Left = dwBufferSize.X - dwWindowSize.X;
	    NeedAdjust = TRUE;
	}
	if (csbi.srWindow.Bottom >= dwBufferSize.Y)
	{
	    dwWindowSize.Y = SRHEIGHT(csbi.srWindow);
	    if (dwWindowSize.Y > dwBufferSize.Y)
		dwWindowSize.Y = dwBufferSize.Y;
	    csbi.srWindow.Bottom = dwBufferSize.Y - 1;
	    csbi.srWindow.Top = dwBufferSize.Y - dwWindowSize.Y;
	    NeedAdjust = TRUE;
	}
	if (NeedAdjust && WantAdjust)
	{
	    if (!SetConsoleWindowInfo(g_hConOut, TRUE, &csbi.srWindow))
		return FALSE;
	}
	return TRUE;
    }

    return FALSE;
}

typedef struct ConsoleBufferStruct
{
    BOOL IsValid;
    CONSOLE_SCREEN_BUFFER_INFO Info;
    PCHAR_INFO Buffer;
    COORD BufferSize;
} ConsoleBuffer;

/*
 * SaveConsoleBuffer()
 * Description:
 *  Saves important information about the console buffer, including the
 *  actual buffer contents.  The saved information is suitable for later
 *  restoration by RestoreConsoleBuffer().
 * Returns:
 *  TRUE if all information was saved; FALSE otherwise
 *  If FALSE, still sets cb->IsValid if buffer characteristics were saved.
 */
    static BOOL
SaveConsoleBuffer(
    ConsoleBuffer *cb)
{
    DWORD NumCells;
    COORD BufferCoord;
    SMALL_RECT ReadRegion;
    WORD Y, Y_incr;

    if (cb == NULL)
	return FALSE;

    if (!GetConsoleScreenBufferInfo(g_hConOut, &cb->Info))
    {
	cb->IsValid = FALSE;
	return FALSE;
    }
    cb->IsValid = TRUE;

    /*
     * Allocate a buffer large enough to hold the entire console screen
     * buffer.  If this ConsoleBuffer structure has already been initialized
     * with a buffer of the correct size, then just use that one.
     */
    if (!cb->IsValid || cb->Buffer == NULL ||
	    cb->BufferSize.X != cb->Info.dwSize.X ||
	    cb->BufferSize.Y != cb->Info.dwSize.Y)
    {
	cb->BufferSize.X = cb->Info.dwSize.X;
	cb->BufferSize.Y = cb->Info.dwSize.Y;
	NumCells = cb->BufferSize.X * cb->BufferSize.Y;
	if (cb->Buffer != NULL)
	    vim_free(cb->Buffer);
	cb->Buffer = (PCHAR_INFO) alloc(NumCells * sizeof(CHAR_INFO));
	if (cb->Buffer == NULL)
	    return FALSE;
    }

    /*
     * We will now copy the console screen buffer into our buffer.
     * ReadConsoleOutput() seems to be limited as far as how much you
     * can read at a time.  Empirically, this number seems to be about
     * 15000 cells (rows * columns).  Start at position (0, 0) and copy
     * in chunks until it is all copied.  The chunks will all have the
     * same horizontal characteristics, so initialize them now.  The
     * height of each chunk will be (15000 / width).
     */
    BufferCoord.X = 0;
    ReadRegion.Left = 0;
    ReadRegion.Right = cb->Info.dwSize.X - 1;
    Y_incr = 15000 / cb->Info.dwSize.X;
    for (Y = 0; Y < cb->BufferSize.Y; Y += Y_incr)
    {
	/*
	 * Read into position (0, Y) in our buffer.
	 */
	BufferCoord.Y = Y;
	/*
	 * Read the region whose top left corner is (0, Y) and whose bottom
	 * right corner is (width - 1, Y + Y_incr - 1).  This should define
	 * a region of size width by Y_incr.  Don't worry if this region is
	 * too large for the remaining buffer; it will be cropped.
	 */
	ReadRegion.Top = Y;
	ReadRegion.Bottom = Y + Y_incr - 1;
	if (!ReadConsoleOutput(g_hConOut,	/* output handle */
		cb->Buffer,			/* our buffer */
		cb->BufferSize,			/* dimensions of our buffer */
		BufferCoord,			/* offset in our buffer */
		&ReadRegion))			/* region to save */
	{
	    vim_free(cb->Buffer);
	    cb->Buffer = NULL;
	    return FALSE;
	}
    }

    return TRUE;
}

/*
 * RestoreConsoleBuffer()
 * Description:
 *  Restores important information about the console buffer, including the
 *  actual buffer contents, if desired.  The information to restore is in
 *  the same format used by SaveConsoleBuffer().
 * Returns:
 *  TRUE on success
 */
    static BOOL
RestoreConsoleBuffer(
    ConsoleBuffer *cb,
    BOOL RestoreScreen)
{
    COORD BufferCoord;
    SMALL_RECT WriteRegion;

    if (cb == NULL || !cb->IsValid)
	return FALSE;

    FitConsoleWindow(cb->Info.dwSize, TRUE);
    if (!SetConsoleScreenBufferSize(g_hConOut, cb->Info.dwSize))
	return FALSE;
    if (!SetConsoleTextAttribute(g_hConOut, cb->Info.wAttributes))
	return FALSE;

    if (!RestoreScreen)
    {
	/*
	 * No need to restore the screen buffer contents, so we're done.
	 */
	return TRUE;
    }

    /*
     * Before restoring the buffer contents, clear the current buffer, and
     * restore the cursor position and window information.  Doing this now
     * prevents old buffer contents from "flashing" onto the screen.
     */
    ClearConsoleBuffer(cb->Info.wAttributes);
    if (!SetConsoleCursorPosition(g_hConOut, cb->Info.dwCursorPosition))
	return FALSE;
    if (!SetConsoleWindowInfo(g_hConOut, TRUE, &cb->Info.srWindow))
	return FALSE;

    /*
     * Restore the screen buffer contents.
     */
    if (cb->Buffer != NULL)
    {
	BufferCoord.X = 0;
	BufferCoord.Y = 0;
	WriteRegion.Left = 0;
	WriteRegion.Top = 0;
	WriteRegion.Right = cb->Info.dwSize.X - 1;
	WriteRegion.Bottom = cb->Info.dwSize.Y - 1;
	if (!WriteConsoleOutput(g_hConOut,	/* output handle */
		cb->Buffer,			/* our buffer */
		cb->BufferSize,			/* dimensions of our buffer */
		BufferCoord,			/* offset in our buffer */
		&WriteRegion))			/* region to restore */
	{
	    return FALSE;
	}
    }

    return TRUE;
}

#ifdef FEAT_RESTORE_ORIG_SCREEN
static ConsoleBuffer g_cbOrig = { 0 };
#endif
static ConsoleBuffer g_cbNonTermcap = { 0 };
static ConsoleBuffer g_cbTermcap = { 0 };

#ifdef FEAT_TITLE
#ifdef __BORLANDC__
typedef HWND (__stdcall *GETCONSOLEWINDOWPROC)(VOID);
#else
typedef WINBASEAPI HWND (WINAPI *GETCONSOLEWINDOWPROC)(VOID);
#endif
char g_szOrigTitle[256] = { 0 };
static HWND g_hWnd = NULL;
static HICON g_hOrigIconSmall = NULL;
static HICON g_hOrigIcon = NULL;
static HICON g_hVimIcon = NULL;
static BOOL g_fCanChangeIcon = FALSE;

/*
 * GetConsoleIcon()
 * Description:
 *  Attempts to retrieve the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    static BOOL
GetConsoleIcon(
    HWND hWnd,
    HICON *phIconSmall,
    HICON *phIcon)
{
    if (hWnd == NULL)
	return FALSE;

    if (phIconSmall != NULL)
    {
	*phIconSmall = (HICON) SendMessage(hWnd, WM_GETICON,
			    (WPARAM) ICON_SMALL, (LPARAM) 0);
    }
    if (phIcon != NULL)
    {
	*phIcon = (HICON) SendMessage(hWnd, WM_GETICON,
			    (WPARAM) ICON_BIG, (LPARAM) 0);
    }
    return TRUE;
}

/*
 * SetConsoleIcon()
 * Description:
 *  Attempts to change the small icon and/or the big icon currently in
 *  use by a given window.
 * Returns:
 *  TRUE on success
 */
    static BOOL
SetConsoleIcon(
    HWND hWnd,
    HICON hIconSmall,
    HICON hIcon)
{
    HICON hPrevIconSmall;
    HICON hPrevIcon;

    if (hWnd == NULL)
	return FALSE;

    if (hIconSmall != NULL)
    {
	hPrevIconSmall = (HICON) SendMessage(hWnd, WM_SETICON,
			    (WPARAM) ICON_SMALL, (LPARAM) hIconSmall);
    }
    if (hIcon != NULL)
    {
	hPrevIcon = (HICON) SendMessage(hWnd, WM_SETICON,
			    (WPARAM) ICON_BIG, (LPARAM) hIcon);
    }
    return TRUE;
}

/*
 * SaveConsoleTitleAndIcon()
 * Description:
 *  Saves the current console window title in g_szOrigTitle, for later
 *  restoration.  Also, attempts to obtain a handle to the console window,
 *  and use it to save the small and big icons currently in use by the
 *  console window.  This is not always possible on some versions of Windows;
 *  nor is it possible when running Vim remotely using Telnet (since the
 *  console window the user sees is owned by a remote process).
 */
    static void
SaveConsoleTitleAndIcon(void)
{
    GETCONSOLEWINDOWPROC GetConsoleWindowProc;

    /* Save the original title. */
    if (!GetConsoleTitle(g_szOrigTitle, sizeof(g_szOrigTitle)))
	return;

    /*
     * Obtain a handle to the console window using GetConsoleWindow() from
     * KERNEL32.DLL; we need to handle in order to change the window icon.
     * This function only exists on NT-based Windows, starting with Windows
     * 2000.  On older operating systems, we can't change the window icon
     * anyway.
     */
    if ((GetConsoleWindowProc = (GETCONSOLEWINDOWPROC)
	    GetProcAddress(GetModuleHandle("KERNEL32.DLL"),
		    "GetConsoleWindow")) != NULL)
    {
	g_hWnd = (*GetConsoleWindowProc)();
    }
    if (g_hWnd == NULL)
	return;

    /* Save the original console window icon. */
    GetConsoleIcon(g_hWnd, &g_hOrigIconSmall, &g_hOrigIcon);
    if (g_hOrigIconSmall == NULL || g_hOrigIcon == NULL)
	return;

    /* Extract the first icon contained in the Vim executable. */
    g_hVimIcon = ExtractIcon(NULL, exe_name, 0);
    if (g_hVimIcon != NULL)
	g_fCanChangeIcon = TRUE;
}
#endif

static int g_fWindInitCalled = FALSE;
static int g_fTermcapMode = FALSE;
static CONSOLE_CURSOR_INFO g_cci;
static DWORD g_cmodein = 0;
static DWORD g_cmodeout = 0;

/*
 * non-GUI version of mch_shellinit().
 */
    void
mch_shellinit()
{
#ifndef FEAT_RESTORE_ORIG_SCREEN
    CONSOLE_SCREEN_BUFFER_INFO csbi;
#endif
#ifndef __MINGW32__
    extern int _fmode;
#endif

    /* Let critical errors result in a failure, not in a dialog box.  Required
     * for the timestamp test to work on removed floppies. */
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		/* we do our own CR-LF translation */
    out_flush();

    /* Obtain handles for the standard Console I/O devices */
    if (read_cmd_fd == 0)
	g_hConIn =  GetStdHandle(STD_INPUT_HANDLE);
    else
	create_conin();
    g_hConOut = GetStdHandle(STD_OUTPUT_HANDLE);

#ifdef FEAT_RESTORE_ORIG_SCREEN
    /* Save the initial console buffer for later restoration */
    SaveConsoleBuffer(&g_cbOrig);
    g_attrCurrent = g_attrDefault = g_cbOrig.Info.wAttributes;
#else
    /* Get current text attributes */
    GetConsoleScreenBufferInfo(g_hConOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
#endif
    if (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    if (cterm_normal_bg_color == 0)
	cterm_normal_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;

    /* set termcap codes to current text attributes */
    update_tcap(g_attrCurrent);

    GetConsoleCursorInfo(g_hConOut, &g_cci);
    GetConsoleMode(g_hConIn,  &g_cmodein);
    GetConsoleMode(g_hConOut, &g_cmodeout);

#ifdef FEAT_TITLE
    SaveConsoleTitleAndIcon();
    /*
     * Set both the small and big icons of the console window to Vim's icon.
     * Note that Vim presently only has one size of icon (32x32), but it
     * automatically gets scaled down to 16x16 when setting the small icon.
     */
    if (g_fCanChangeIcon)
	SetConsoleIcon(g_hWnd, g_hVimIcon, g_hVimIcon);
#endif

    ui_get_shellsize();

#ifdef MCH_WRITE_DUMP
    fdDump = fopen("dump", "wt");

    if (fdDump)
    {
	time_t t;

	time(&t);
	fputs(ctime(&t), fdDump);
	fflush(fdDump);
    }
#endif

    g_fWindInitCalled = TRUE;

#ifdef FEAT_MOUSE
    g_fMouseAvail = GetSystemMetrics(SM_MOUSEPRESENT);
#endif

#ifdef FEAT_CLIPBOARD
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line, or
     * rectangular block.  Only useful for copying between two Vims.
     */
    clipboard.format = RegisterClipboardFormat("VimClipboard");
#endif

    /* This will be NULL on anything but NT 4.0 */
    s_pfnGetConsoleKeyboardLayoutName =
	(PFNGCKLN) GetProcAddress(GetModuleHandle("kernel32.dll"),
				  "GetConsoleKeyboardLayoutNameA");
}

/*
 * non-GUI version of mch_windexit().
 * Shut down and exit with status `r'
 * Careful: mch_windexit() may be called before mch_shellinit()!
 */
    void
mch_windexit(
    int r)
{
    stoptermcap();

    if (g_fWindInitCalled)
	settmode(TMODE_COOK);

    ml_close_all(TRUE);		/* remove all memfiles */

    if (g_fWindInitCalled)
    {
#ifdef FEAT_TITLE
	mch_restore_title(3);
	/*
	 * Restore both the small and big icons of the console window to
	 * what they were at startup.
	 */
	if (g_fCanChangeIcon)
	    SetConsoleIcon(g_hWnd, g_hOrigIconSmall, g_hOrigIcon);
#endif

#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
	    time_t t;

	    time(&t);
	    fputs(ctime(&t), fdDump);
	    fclose(fdDump);
	}
	fdDump = NULL;
#endif
    }

    SetConsoleCursorInfo(g_hConOut, &g_cci);
    SetConsoleMode(g_hConIn,  g_cmodein);
    SetConsoleMode(g_hConOut, g_cmodeout);

    exit(r);
}
#endif /* !FEAT_GUI_W32 */


/*
 * Do we have an interactive window?
 */
    int
mch_check_win(
    int argc,
    char **argv)
{
    char	temp[256];

    /* store the name of the executable, may be used for $VIM */
    GetModuleFileName(NULL, temp, 255);
    if (*temp != NUL)
	exe_name = FullName_save((char_u *)temp, FALSE);

#ifdef FEAT_GUI_W32
    return OK;	    /* GUI always has a tty */
#else
    if (isatty(1))
	return OK;
    return FAIL;
#endif
}


/*
 * fname_case(): Set the case of the file name, if it already exists.
 */
    void
fname_case(
    char_u *name)
{
    char szTrueName[_MAX_PATH + 2];
    char szOrigElem[_MAX_PATH + 2];
    char *psz, *pszPrev;
    const int len = (name != NULL)  ?  STRLEN(name)  :	0;

    if (len == 0)
	return;

    STRNCPY(szTrueName, name, _MAX_PATH);
    szTrueName[_MAX_PATH] = '\0';   /* ensure it's sealed off! */
    STRCAT(szTrueName, pseps);	/* sentinel */

    slash_adjust(szTrueName);

    psz = pszPrev = szTrueName;

    /* Skip leading \\ in UNC name or drive letter */
    if (len > 2 && ((psz[0] == psepc && psz[1] == psepc)
				       || (isalpha(psz[0]) && psz[1] == ':')))
    {
	psz = pszPrev = szTrueName + 2;
    }

    while (*psz != NUL)
    {
	WIN32_FIND_DATA	fb;
	HANDLE		hFind;

	while (*psz != psepc)
	    psz++;
	*psz = NUL;

	if ((hFind = FindFirstFile(szTrueName, &fb)) != INVALID_HANDLE_VALUE)
	{
	    /* avoid ".." and ".", etc */
	    if (_stricoll(pszPrev, fb.cFileName) == 0)
	    {
		STRCPY(szOrigElem, pszPrev);
		STRCPY(pszPrev, fb.cFileName);
		/* Look for exact match and prefer it if found */
		while (FindNextFile(hFind, &fb))
		{
		    if (strcoll(szOrigElem, fb.cFileName) == 0)
		    {
			STRCPY(pszPrev, fb.cFileName);
			break;
		    }
		}
	    }
	    FindClose(hFind);
	}

	*psz++ = psepc;
	pszPrev = psz;
    }

    *--psz = NUL;   /* remove sentinel */

    STRCPY(name, szTrueName);
}



/*
 * Insert user name in s[len].
 */
    int
mch_get_user_name(
    char_u *s,
    int len)
{
    char szUserName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD cch = sizeof szUserName;

    if (GetUserName(szUserName, &cch))
    {
	STRNCPY(s, szUserName, len);
	return OK;
    }
    s[0] = NUL;
    return FAIL;
}


/*
 * Insert host name in s[len].
 */
    void
mch_get_host_name(
    char_u	*s,
    int		len)
{
    char szHostName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD cch = sizeof szHostName;

    if (GetComputerName(szHostName, &cch))
    {
	STRCPY(s, "PC ");
	STRNCPY(s + 3, szHostName, len - 3);
    }
    else
	STRNCPY(s, "PC (Win32 Vim)", len);
}


/*
 * return process ID
 */
    long
mch_get_pid()
{
    return (long)GetCurrentProcessId();
}


/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    int
mch_dirname(
    char_u	*buf,
    int		len)
{
    /*
     * Originally this was:
     *    return (getcwd(buf, len) != NULL ? OK : FAIL);
     * But the Win32s known bug list says that getcwd() doesn't work
     * so use the Win32 system call instead. <Negri>
     */
    return (GetCurrentDirectory(len, buf) != 0 ? OK : FAIL);
}

/*
 * get file permissions for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
    long
mch_getperm(
    char_u *name)
{
    return (long)GetFileAttributes((char *)name);
}


/*
 * set file permission for `name' to `perm'
 */
    int
mch_setperm(
    char_u *name,
    long perm)
{
    perm |= FILE_ATTRIBUTE_ARCHIVE;	/* file has changed, set archive bit */
    return SetFileAttributes((char *)name, perm) ? OK : FAIL;
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name)
{
    int	perm;

    perm = GetFileAttributes((char *)name);
    if (perm >= 0)
    {
	perm |= FILE_ATTRIBUTE_HIDDEN;
	SetFileAttributes((char *)name, perm);
    }
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
    int
mch_isdir(char_u *name)
{
    int f = mch_getperm(name);

    if (f == -1)
	return FALSE;		    /* file does not exist at all */

    return (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

/*
 * Return TRUE if file or directory "name" is writable (not readonly).
 */
    int
mch_writable(char_u *name)
{
    int perm = mch_getperm(name);

    return (perm != -1 && !(perm & FILE_ATTRIBUTE_READONLY));
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Return 1 if "name" can be executed, 0 if not.
 * Return -1 if unknown.
 */
    int
mch_can_exe(char_u *name)
{
    return executable_exists((char *)name);
}
#endif

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    int
mch_nodetype(char_u *name)
{
    HANDLE	hFile;
    int		type;

    hFile = CreateFile(name,		/* file name */
		GENERIC_WRITE,		/* access mode */
		0,			/* share mode */
		NULL,			/* security descriptor */
		OPEN_EXISTING,		/* creation disposition */
		0,			/* file attributes */
		NULL);			/* handle to template file */

    if (hFile == INVALID_HANDLE_VALUE)
	return NODE_NORMAL;

    type = GetFileType(hFile);
    CloseHandle(hFile);
    if (type == FILE_TYPE_CHAR)
	return NODE_WRITABLE;
    if (type == FILE_TYPE_DISK)
	return NODE_NORMAL;
    return NODE_OTHER;
}

#ifdef HAVE_ACL
# include <aclapi.h>

struct my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    pSidGroup;
    PACL		    pDacl;
    PACL		    pSacl;
};
#endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_t
mch_get_acl(fname)
    char_u	*fname;
{
#ifndef HAVE_ACL
    return (vim_acl_t)NULL;
#else
    struct my_acl   *p = NULL;

    /* This only works on Windows NT and 2000. */
    if (g_PlatformId == VER_PLATFORM_WIN32_NT && advapi_lib != NULL)
    {
	p = (struct my_acl *)alloc_clear((unsigned)sizeof(struct my_acl));
	if (p != NULL)
	{
	    if (pGetNamedSecurityInfo(
			(LPTSTR)fname,		// Abstract filename
			SE_FILE_OBJECT,		// File Object
			// Retrieve the entire security descriptor.
			OWNER_SECURITY_INFORMATION |
			GROUP_SECURITY_INFORMATION |
			DACL_SECURITY_INFORMATION |
			SACL_SECURITY_INFORMATION,
			&p->pSidOwner,		// Ownership information.
			&p->pSidGroup,		// Group membership.
			&p->pDacl,		// Discretionary information.
			&p->pSacl,		// For auditing purposes.
			&p->pSecurityDescriptor
				    ) != ERROR_SUCCESS)
	    {
		mch_free_acl((vim_acl_t)p);
		p = NULL;
	    }
	}
    }

    return (vim_acl_t)p;
#endif
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 * This must only be called with "acl" equal to what mch_get_acl() returned.
 */
    void
mch_set_acl(fname, acl)
    char_u	*fname;
    vim_acl_t	acl;
{
#ifdef HAVE_ACL
    struct my_acl   *p = (struct my_acl *)acl;

    if (p != NULL && advapi_lib != NULL)
	(void)pSetNamedSecurityInfo(
		    (LPTSTR)fname,		// Abstract filename
		    SE_FILE_OBJECT,		// File Object
		    // Retrieve the entire security descriptor.
		    OWNER_SECURITY_INFORMATION |
			GROUP_SECURITY_INFORMATION |
			DACL_SECURITY_INFORMATION |
			SACL_SECURITY_INFORMATION,
		    p->pSidOwner,		// Ownership information.
		    p->pSidGroup,		// Group membership.
		    p->pDacl,			// Discretionary information.
		    p->pSacl			// For auditing purposes.
		    );
#endif
}

    void
mch_free_acl(acl)
    vim_acl_t	acl;
{
#ifdef HAVE_ACL
    struct my_acl   *p = (struct my_acl *)acl;

    if (p != NULL)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
#endif
}

#ifndef FEAT_GUI_W32

/*
 * handler for ctrl-break, ctrl-c interrupts, and fatal events.
 */
    static BOOL WINAPI
handler_routine(
    DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_C_EVENT:
	g_fCtrlCPressed = TRUE;
	return TRUE;

    case CTRL_BREAK_EVENT:
	g_fCBrkPressed	= TRUE;
	return TRUE;

    /* fatal events: shut down gracefully */
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
	windgoto((int)Rows - 1, 0);
	sprintf((char *)IObuff, _("Vim: Caught %s event\n"),
		(dwCtrlType == CTRL_CLOSE_EVENT
		     ? _("close")
		     : dwCtrlType == CTRL_LOGOFF_EVENT
			 ? _("logoff")
			 : _("shutdown")));

#ifdef DEBUG
	OutputDebugString(IObuff);
#endif

	preserve_exit();	/* output IObuff, preserve files and exit */

	return TRUE;		/* not reached */

    default:
	return FALSE;
    }
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
    void
mch_settmode(
    int tmode)
{
    DWORD cmodein;
    DWORD cmodeout;
    BOOL bEnableHandler;

    GetConsoleMode(g_hConIn, &cmodein);
    GetConsoleMode(g_hConOut, &cmodeout);
    if (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
		     ENABLE_ECHO_INPUT);
#ifdef FEAT_MOUSE
	if (g_fMouseActive)
	    cmodein |= ENABLE_MOUSE_INPUT;
#endif
	cmodeout &= ~(ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = TRUE;
    }
    else /* cooked */
    {
	cmodein |= (ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
		    ENABLE_ECHO_INPUT);
	cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	bEnableHandler = FALSE;
    }
    SetConsoleMode(g_hConIn, cmodein);
    SetConsoleMode(g_hConOut, cmodeout);
    SetConsoleCtrlHandler(handler_routine, bEnableHandler);

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fprintf(fdDump, "mch_settmode(%s, in = %x, out = %x)\n",
		tmode == TMODE_RAW ? "raw" :
				    tmode == TMODE_COOK ? "cooked" : "normal",
		cmodein, cmodeout);
	fflush(fdDump);
    }
#endif
}


/*
 * Get the size of the current window in `Rows' and `Columns'
 * Return OK when size could be determined, FAIL otherwise.
 */
    int
mch_get_shellsize()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!g_fTermcapMode && g_cbTermcap.IsValid)
    {
	/*
	 * For some reason, we are trying to get the screen dimensions
	 * even though we are not in termcap mode.  The 'Rows' and 'Columns'
	 * variables are really intended to mean the size of Vim screen
	 * while in termcap mode.
	 */
	Rows = g_cbTermcap.Info.dwSize.Y;
	Columns = g_cbTermcap.Info.dwSize.X;
    }
    else if (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	Columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    else
    {
	Rows = 25;
	Columns = 80;
    }
    return OK;
}

/*
 * Set a console window to `xSize' * `ySize'
 */
    static void
ResizeConBufAndWindow(
    HANDLE hConsole,
    int xSize,
    int ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;	/* hold current console buffer info */
    SMALL_RECT	    srWindowRect;	/* hold the new console size */
    COORD	    coordScreen;

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
#endif

    /* get the largest size we can size the console window to */
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    /* define the new console window size and scroll position */
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
    srWindowRect.Right =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    if (GetConsoleScreenBufferInfo(g_hConOut, &csbi))
    {
	int sx, sy;

	sx = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	sy = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	if (sy < ySize || sx < xSize)
	{
	    /*
	     * Increasing number of lines/columns, do buffer first.
	     * Use the maximal size in x and y direction.
	     */
	    if (sy < ySize)
		coordScreen.Y = ySize;
	    else
		coordScreen.Y = sy;
	    if (sx < xSize)
		coordScreen.X = xSize;
	    else
		coordScreen.X = sx;
	    SetConsoleScreenBufferSize(hConsole, coordScreen);
	}
    }

    if (!SetConsoleWindowInfo(g_hConOut, TRUE, &srWindowRect))
    {
#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
	    fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
		    GetLastError());
	    fflush(fdDump);
	}
#endif
    }

    /* define the new console buffer size */
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

    if (!SetConsoleScreenBufferSize(hConsole, coordScreen))
    {
#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
	    fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
		    GetLastError());
	    fflush(fdDump);
	}
#endif
    }
}


/*
 * Set the console window to `Rows' * `Columns'
 */
    void
mch_set_shellsize()
{
    COORD coordScreen;

    /* Don't change window size while still starting up */
    if (suppress_winsize)
    {
	suppress_winsize = 2;
	return;
    }

    coordScreen = GetLargestConsoleWindowSize(g_hConOut);

    /* Clamp Rows and Columns to reasonable values */
    if (Rows > coordScreen.Y)
	Rows = coordScreen.Y;
    if (Columns > coordScreen.X)
	Columns = coordScreen.X;

    ResizeConBufAndWindow(g_hConOut, Columns, Rows);
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize()
{
    set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * Called when started up, to set the winsize that was delayed.
 */
    void
mch_set_winsize_now()
{
    if (suppress_winsize == 2)
    {
	suppress_winsize = 0;
	mch_set_shellsize();
	shell_resized();
    }
    suppress_winsize = 0;
}
#endif /* FEAT_GUI_W32 */



#if defined(FEAT_GUI_W32) || defined(PROTO)

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
    static int
mch_system(char *cmd, int options)
{
    STARTUPINFO		si;
    PROCESS_INFORMATION pi;
    DWORD		ret = 0;
    HWND		hwnd = GetFocus();

    si.cb = sizeof(si);
    si.lpReserved = NULL;
    si.lpDesktop = NULL;
    si.lpTitle = NULL;
    si.dwFlags = STARTF_USESHOWWINDOW;
    /*
     * It's nicer to run a filter command in a minimized window, but in
     * Windows 95 this makes the command MUCH slower.  We can't do it under
     * Win32s either as it stops the synchronous spawn workaround working.
     */
    if ((options & SHELL_DOOUT) && !mch_windows95() && !gui_is_win32s())
	si.wShowWindow = SW_SHOWMINIMIZED;
    else
	si.wShowWindow = SW_SHOWNORMAL;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;

    /* There is a strange error on Windows 95 when using "c:\\command.com".
     * When the "c:\\" is left out it works OK...? */
    if (mch_windows95()
	    && (STRNICMP(cmd, "c:/command.com", 14) == 0
		|| STRNICMP(cmd, "c:\\command.com", 14) == 0))
	cmd += 3;

    /* Now, run the command */
    CreateProcess(NULL,			/* Executable name */
		  cmd,			/* Command to execute */
		  NULL,			/* Process security attributes */
		  NULL,			/* Thread security attributes */
		  FALSE,		/* Inherit handles */
		  CREATE_DEFAULT_ERROR_MODE |	/* Creation flags */
			CREATE_NEW_CONSOLE,
		  NULL,			/* Environment */
		  NULL,			/* Current directory */
		  &si,			/* Startup information */
		  &pi);			/* Process information */


    /* Wait for the command to terminate before continuing */
    if (g_PlatformId != VER_PLATFORM_WIN32s)
    {
	WaitForSingleObject(pi.hProcess, INFINITE);

	/* Get the command exit code */
	GetExitCodeProcess(pi.hProcess, &ret);
    }
    else
    {
	/*
	 * This ugly code is the only quick way of performing
	 * a synchronous spawn under Win32s. Yuk.
	 */
	num_windows = 0;
	EnumWindows(win32ssynch_cb, 0);
	old_num_windows = num_windows;
	do
	{
	    Sleep(1000);
	    num_windows = 0;
	    EnumWindows(win32ssynch_cb, 0);
	} while (num_windows == old_num_windows);
	ret = 0;
    }

    /* Close the handles to the subprocess, so that it goes away */
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    /* Try to get input focus back.  Doesn't always work though. */
    SetFocus(hwnd);

    return ret;
}
#else

# define mch_system(c, o) system(c)

#endif

/*
 * Either execute a command by calling the shell or start a new shell
 */
    int
mch_call_shell(
    char_u *cmd,
    int options)	/* SHELL_*, see vim.h */
{
    int	    x = 0;
#ifdef FEAT_TITLE
    char szShellTitle[512];

    /* Change the title to reflect that we are in a subshell. */
    if (GetConsoleTitle(szShellTitle, sizeof(szShellTitle) - 4) > 0)
    {
	if (cmd == NULL)
	    strcat(szShellTitle, " :sh");
	else
	{
	    strcat(szShellTitle, " - !");
	    if ((strlen(szShellTitle) + strlen(cmd) < sizeof(szShellTitle)))
		strcat(szShellTitle, cmd);
	}
	mch_settitle(szShellTitle, NULL);
    }
#endif

    out_flush();

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fprintf(fdDump, "mch_call_shell(\"%s\", %d)\n", cmd, options);
	fflush(fdDump);
    }
#endif

    /*
     * Catch all deadly signals while running the external command, because a
     * CTRL-C, Ctrl-Break or illegal instruction  might otherwise kill us.
     */
    signal(SIGINT, SIG_IGN);
#if defined(__GNUC__) && !defined(__MINGW32__)
    signal(SIGKILL, SIG_IGN);
#else
    signal(SIGBREAK, SIG_IGN);
#endif
    signal(SIGILL, SIG_IGN);
    signal(SIGFPE, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGABRT, SIG_IGN);

    if (options & SHELL_COOKED)
	settmode(TMODE_COOK);	/* set to normal mode */

    if (cmd == NULL)
    {
	x = mch_system(p_sh, options);
    }
    else
    {
	/* we use "command" or "cmd" to start the shell; slow but easy */
	char_u *newcmd;

	newcmd = lalloc(
#ifdef FEAT_GUI_W32
		STRLEN(vimrun_path) +
#endif
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10, TRUE);
	if (newcmd != NULL)
	{
	    char_u *cmdbase = (*cmd == '"' ? cmd + 1 : cmd);

	    if ((STRNICMP(cmdbase, "start", 5) == 0) && vim_iswhite(cmdbase[5]))
	    {
		STARTUPINFO		si;
		PROCESS_INFORMATION	pi;

		si.cb = sizeof(si);
		si.lpReserved = NULL;
		si.lpDesktop = NULL;
		si.lpTitle = NULL;
		si.dwFlags = 0;
		si.cbReserved2 = 0;
		si.lpReserved2 = NULL;

		cmdbase = skipwhite(cmdbase + 5);
		if ((STRNICMP(cmdbase, "/min", 4) == 0)
			&& vim_iswhite(cmdbase[4]))
		{
		    cmdbase = skipwhite(cmdbase + 4);
		    si.dwFlags = STARTF_USESHOWWINDOW;
		    si.wShowWindow = SW_SHOWMINNOACTIVE;
		}

		/* When the command is in double quotes, but 'shellxquote' is
		 * empty, keep the double quotes around the command.
		 * Otherwise remove the double quotes, they aren't needed
		 * here, because we don't use a shell to run the command. */
		if (*cmd == '"' && *p_sxq == NUL)
		{
		    newcmd[0] = '"';
		    STRCPY(newcmd + 1, cmdbase);
		}
		else
		{
		    STRCPY(newcmd, cmdbase);
		    if (*cmd == '"' && *newcmd != NUL)
			newcmd[STRLEN(newcmd) - 1] = NUL;
		}

		/*
		 * Now, start the command as a process, so that it doesn't
		 * inherit our handles which causes unpleasant dangling swap
		 * files if we exit before the spawned process
		 */
		if (CreateProcess (NULL,	// Executable name
			newcmd,			// Command to execute
			NULL,			// Process security attributes
			NULL,			// Thread security attributes
			FALSE,			// Inherit handles
			CREATE_NEW_CONSOLE,	// Creation flags
			NULL,			// Environment
			NULL,			// Current directory
			&si,			// Startup information
			&pi))			// Process information
		    x = 0;
		else
		{
		    x = -1;
#ifdef FEAT_GUI_W32
		    EMSG(_("Command not found"));
#endif
		}
		/* Close the handles to the subprocess, so that it goes away */
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	    }
	    else
	    {
#if defined(FEAT_GUI_W32)
		if (!(options & SHELL_DOOUT) && !s_dont_use_vimrun)
		    /* Use vimrun to execute the command.  It opens a console
		     * window, which can be closed without killing Vim. */
		    sprintf((char *)newcmd, "%s%s%s %s %s",
			    vimrun_path, msg_silent ? "-s " : "",
			    p_sh, p_shcf, cmd);
		else
#endif
		    sprintf((char *)newcmd, "%s %s %s", p_sh, p_shcf, cmd);
		x = mch_system((char *)newcmd, options);
	    }
	    vim_free(newcmd);
	}
    }

    settmode(TMODE_RAW);	    /* set to raw mode */

    if (x && !(options & SHELL_SILENT))
    {
	smsg(_("shell returned %d"), x);
	msg_putchar('\n');
    }
#ifdef FEAT_TITLE
    resettitle();
#endif

    signal(SIGINT, SIG_DFL);
#if defined(__GNUC__) && !defined(__MINGW32__)
    signal(SIGKILL, SIG_DFL);
#else
    signal(SIGBREAK, SIG_DFL);
#endif
    signal(SIGILL, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGABRT, SIG_DFL);

    return x;
}


/*
 * comparison function for qsort in expandpath
 */
    static int
#ifdef __BORLANDC__
_RTLENTRYF
#endif
pstrcmp(
    const void *a,
    const void *b)
{
    return (_stricoll(* (char **)a, * (char **)b));
}


/*
 * Recursively build up a list of files in "gap" matching the first wildcard
 * in `path'.  Called by expand_wildcards().
 */
    static int
win32_expandpath(
    garray_t	*gap,
    char_u	*path,
    char_u	*wildc,
    int		flags)		/* EW_* flags */
{
    char		*buf;
    char		*p, *s, *e;
    int			start_len;
    int			c = 1;
    WIN32_FIND_DATA	fb;
    HANDLE		hFind;
    int			matches;
    int			start_dot_ok;
    int			len;

    start_len = gap->ga_len;

    /* allocate buffer with room for file name */
    buf = (char *)alloc(STRLEN(path) + BASENAMELEN + 5);
    if (buf == NULL)
	return 0;

    /*
     * Find the first part in the path name that contains a wildcard.
     * Copy it into `buf', including the preceding characters.
     */
    p = buf;
    s = buf;
    e = NULL;
    while (*path)
    {
	if (path >= wildc && rem_backslash(path))	/* remove a backslash */
	    ++path;
	else if (*path == '\\' || *path == ':' || *path == '/')
	{
	    if (e)
		break;
	    else
		s = p + 1;
	}
	else if (*path == '*' || *path == '?')
	    e = p;
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    len = mb_ptr2len_check(path);
	    STRNCPY(p, path, len);
	    p += len;
	    path += len;
	}
	else
#endif
	    *p++ = *path++;
    }
    e = p;
    *e = NUL;
    /* now we have one wildcard component between `s' and `e' */

#ifdef FEAT_GUI_W32
    if (gui_is_win32s())
    {
	/* It appears the Win32s FindFirstFile() call doesn't like a pattern
	 * such as \sw\rel1.0\cod* because of the dot in the directory name.
	 * It doesn't match files with extensions.
	 * if the file name ends in "*" and does not contain a "." after the
	 * last \ , add ".*"
	 */
	if (e[-1] == '*' && vim_strchr(s, '.') == NULL)
	{
	    *e++ = '.';
	    *e++ = '*';
	    *e = NUL;
	}
    }
#endif

    start_dot_ok = (*s == '.');

    /* If we are expanding wildcards, we try both files and directories */
    if ((hFind = FindFirstFile(buf, &fb)) != INVALID_HANDLE_VALUE)
	while (c)
	{
	    STRCPY(s, fb.cFileName);

	    /*
	     * Ignore entries starting with a dot, unless when asked for.
	     */
	    if ((s[0] != '.' || start_dot_ok)
		    && (*path == NUL || mch_isdir(buf)))
	    {
		len = STRLEN(buf);
		STRCPY(buf + len, path);
		if (mch_has_wildcard(path))	    /* handle more wildcards */
		{
		    /* need to expand another component of the path */
		    /* remove backslashes for the remaining components only */
		    (void)win32_expandpath(gap, buf, buf + len + 1, flags);
		}
		else if (mch_getperm(buf) >= 0)	    /* add existing file */
		{
		    /* no more wildcards, check if there is a match */
		    /* remove backslashes for the remaining components only */
		    if (*path)
			backslash_halve(buf + len + 1);
		    if (mch_getperm(buf) >= 0)	/* add existing file */
			addfile(gap, buf, flags);
		}
	    }

	    c = FindNextFile(hFind, &fb);
	}
    FindClose(hFind);
    vim_free(buf);

    matches = gap->ga_len - start_len;
    if (matches)
	qsort(((char_u **)gap->ga_data) + start_len, matches,
						     sizeof(char *), pstrcmp);
    return matches;
}

    int
mch_expandpath(
    garray_t	*gap,
    char_u	*path,
    int		flags)		/* EW_* flags */
{
    return win32_expandpath(gap, path, path, flags);
}



#ifndef FEAT_GUI_W32

/*
 * Start termcap mode
 */
    static void
termcap_mode_start(void)
{
    DWORD cmodein;

    if (g_fTermcapMode)
	return;

    SaveConsoleBuffer(&g_cbNonTermcap);

    if (g_cbTermcap.IsValid)
    {
	/*
	 * We've been in termcap mode before.  Restore certain screen
	 * characteristics, including the buffer size and the window
	 * size.  Since we will be redrawing the screen, we don't need
	 * to restore the actual contents of the buffer.
	 */
	RestoreConsoleBuffer(&g_cbTermcap, FALSE);
	SetConsoleWindowInfo(g_hConOut, TRUE, &g_cbTermcap.Info.srWindow);
	Rows = g_cbTermcap.Info.dwSize.Y;
	Columns = g_cbTermcap.Info.dwSize.X;
    }
    else
    {
	/*
	 * This is our first time entering termcap mode.  Clear the console
	 * screen buffer, and resize the buffer to match the current window
	 * size.  We will use this as the size of our editing environment.
	 */
	ClearConsoleBuffer(g_attrCurrent);
	ResizeConBufAndWindow(g_hConOut, Columns, Rows);
    }

#ifdef FEAT_TITLE
    resettitle();
#endif

    GetConsoleMode(g_hConIn, &cmodein);
#ifdef FEAT_MOUSE
    if (g_fMouseActive)
	cmodein |= ENABLE_MOUSE_INPUT;
    else
	cmodein &= ~ENABLE_MOUSE_INPUT;
#endif
    cmodein |= ENABLE_WINDOW_INPUT;
    SetConsoleMode(g_hConIn, cmodein);

    redraw_later_clear();
    g_fTermcapMode = TRUE;
}


/*
 * End termcap mode
 */
    static void
termcap_mode_end(void)
{
    DWORD cmodein;
    ConsoleBuffer *cb;
    COORD coord;
    DWORD dwDummy;

    if (!g_fTermcapMode)
	return;

    SaveConsoleBuffer(&g_cbTermcap);

    GetConsoleMode(g_hConIn, &cmodein);
    cmodein &= ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    SetConsoleMode(g_hConIn, cmodein);

#ifdef FEAT_RESTORE_ORIG_SCREEN
    cb = exiting ? &g_cbOrig : &g_cbNonTermcap;
#else
    cb = &g_cbNonTermcap;
#endif
    RestoreConsoleBuffer(cb, p_rs);
    SetConsoleCursorInfo(g_hConOut, &g_cci);

    if (p_rs || exiting)
    {
	/*
	 * Clear anything that happens to be on the current line.
	 */
	coord.X = 0;
	coord.Y = p_rs ? cb->Info.dwCursorPosition.Y : (Rows - 1);
	FillConsoleOutputCharacter(g_hConOut, ' ',
		cb->Info.dwSize.X, coord, &dwDummy);
	/*
	 * The following is just for aesthetics.  If we are exiting without
	 * restoring the screen, then we want to have a prompt string
	 * appear at the bottom line.  However, the command interpreter
	 * seems to always advance the cursor one line before displaying
	 * the prompt string, which causes the screen to scroll.  To
	 * counter this, move the cursor up one line before exiting.
	 */
	if (exiting && !p_rs)
	    coord.Y--;
	/*
	 * Position the cursor at the leftmost column of the desired row.
	 */
	SetConsoleCursorPosition(g_hConOut, coord);
    }

    g_fTermcapMode = FALSE;
}
#endif /* FEAT_GUI_W32 */


#ifdef FEAT_GUI_W32
    void
mch_write(
    char_u  *s,
    int	    len)
{
    /* never used */
}

#else

/*
 * clear `n' chars, starting from `coord'
 */
    static void
clear_chars(
    COORD coord,
    DWORD n)
{
    DWORD dwDummy;

    FillConsoleOutputCharacter(g_hConOut, ' ', n, coord, &dwDummy);
    FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, n, coord, &dwDummy);
}


/*
 * Clear the screen
 */
    static void
clear_screen(void)
{
    g_coord.X = g_coord.Y = 0;
    clear_chars(g_coord, Rows * Columns);
}


/*
 * Clear to end of display
 */
    static void
clear_to_end_of_display(void)
{
    clear_chars(g_coord, (Rows - g_coord.Y - 1)
					   * Columns + (Columns - g_coord.X));
}


/*
 * Clear to end of line
 */
    static void
clear_to_end_of_line(void)
{
    clear_chars(g_coord, Columns - g_coord.X);
}


/*
 * Scroll the scroll region up by `cLines' lines
 */
    static void
scroll(
    unsigned cLines)
{
    COORD oldcoord = g_coord;

    gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Top + 1);
    delete_lines(cLines);

    g_coord = oldcoord;
}


/*
 * Set the scroll region
 */
    static void
set_scroll_region(
    unsigned left,
    unsigned top,
    unsigned right,
    unsigned bottom)
{
    if (left >= right
	    || top >= bottom
	    || right > (unsigned) Columns - 1
	    || bottom > (unsigned) Rows - 1)
	return;

    g_srScrollRegion.Left =   left;
    g_srScrollRegion.Top =    top;
    g_srScrollRegion.Right =  right;
    g_srScrollRegion.Bottom = bottom;
}


/*
 * Insert `cLines' lines at the current cursor position
 */
    static void
insert_lines(
    unsigned cLines)
{
    SMALL_RECT	    source;
    COORD	    dest;
    CHAR_INFO	    fill;

    dest.X = 0;
    dest.Y = g_coord.Y + cLines;

    source.Left   = 0;
    source.Top	  = g_coord.Y;
    source.Right  = g_srScrollRegion.Right;
    source.Bottom = g_srScrollRegion.Bottom - cLines;

    fill.Char.AsciiChar = ' ';
    fill.Attributes = g_attrCurrent;

    ScrollConsoleScreenBuffer(g_hConOut, &source, NULL, dest, &fill);

    /* Here we have to deal with a win32 console flake: If the scroll
     * region looks like abc and we scroll c to a and fill with d we get
     * cbd... if we scroll block c one line at a time to a, we get cdd...
     * vim expects cdd consistently... So we have to deal with that
     * here... (this also occurs scrolling the same way in the other
     * direction).  */

    if (source.Bottom < dest.Y)
    {
	COORD coord;

	coord.X = 0;
	coord.Y = source.Bottom;
	clear_chars(coord, Columns * (dest.Y - source.Bottom));
    }
}


/*
 * Delete `cLines' lines at the current cursor position
 */
    static void
delete_lines(
    unsigned cLines)
{
    SMALL_RECT	    source;
    COORD	    dest;
    CHAR_INFO	    fill;
    int		    nb;

    dest.X = 0;
    dest.Y = g_coord.Y;

    source.Left   = 0;
    source.Top	  = g_coord.Y + cLines;
    source.Right  = g_srScrollRegion.Right;
    source.Bottom = g_srScrollRegion.Bottom;

    fill.Char.AsciiChar = ' ';
    fill.Attributes = g_attrCurrent;

    ScrollConsoleScreenBuffer(g_hConOut, &source, NULL, dest, &fill);

    /* Here we have to deal with a win32 console flake: If the scroll
     * region looks like abc and we scroll c to a and fill with d we get
     * cbd... if we scroll block c one line at a time to a, we get cdd...
     * vim expects cdd consistently... So we have to deal with that
     * here... (this also occurs scrolling the same way in the other
     * direction).  */

    nb = dest.Y + (source.Bottom - source.Top) + 1;

    if (nb < source.Top)
    {
	COORD coord;

	coord.X = 0;
	coord.Y = nb;
	clear_chars(coord, Columns * (source.Top - nb));
    }
}


/*
 * Set the cursor position
 */
    static void
gotoxy(
    unsigned x,
    unsigned y)
{
    if (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	return;

    /* external cursor coords are 1-based; internal are 0-based */
    g_coord.X = x - 1;
    g_coord.Y = y - 1;
    SetConsoleCursorPosition(g_hConOut, g_coord);
}


/*
 * Set the current text attribute = (foreground | background)
 * See ../doc/os_win32.txt for the numbers.
 */
    static void
textattr(
    WORD wAttr)
{
    g_attrCurrent = wAttr;

    SetConsoleTextAttribute(g_hConOut, wAttr);
}


    static void
textcolor(
    WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + wAttr;

    SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
}


    static void
textbackground(
    WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + (wAttr << 4);

    SetConsoleTextAttribute(g_hConOut, g_attrCurrent);
}


/*
 * restore the default text attribute (whatever we started with)
 */
    static void
normvideo()
{
    textattr(g_attrDefault);
}


static WORD g_attrPreStandout = 0;

/*
 * Make the text standout, by brightening it
 */
    static void
standout(void)
{
    g_attrPreStandout = g_attrCurrent;
    textattr((WORD) (g_attrCurrent|FOREGROUND_INTENSITY|BACKGROUND_INTENSITY));
}


/*
 * Turn off standout mode
 */
    static void
standend()
{
    if (g_attrPreStandout)
    {
	textattr(g_attrPreStandout);
	g_attrPreStandout = 0;
    }
}


/*
 * Set normal fg/bg color, based on T_ME.  Called whem t_me has been set.
 */
    void
mch_set_normal_colors()
{
    char_u	*p;
    int		n;

    cterm_normal_fg_color = (g_attrDefault & 0xf) + 1;
    cterm_normal_bg_color = ((g_attrDefault >> 4) & 0xf) + 1;
    if (T_ME[0] == ESC && T_ME[1] == '|')
    {
	p = T_ME + 2;
	n = getdigits(&p);
	if (*p == 'm' && n > 0)
	{
	    cterm_normal_fg_color = (n & 0xf) + 1;
	    cterm_normal_bg_color = ((n >> 4) & 0xf) + 1;
	}
    }
}


/*
 * visual bell: flash the screen
 */
    static void
visual_bell()
{
    COORD   coordOrigin = {0, 0};
    WORD    attrFlash = ~g_attrCurrent & 0xff;

    DWORD   dwDummy;
    LPWORD  oldattrs = (LPWORD) alloc(Rows * Columns * sizeof(WORD));

    ReadConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    FillConsoleOutputAttribute(g_hConOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    /* wait for 15 msec */
    WriteConsoleOutputAttribute(g_hConOut, oldattrs, Rows * Columns,
				coordOrigin, &dwDummy);
    vim_free(oldattrs);
}


/*
 * Make the cursor visible or invisible
 */
    static void
cursor_visible(
    BOOL fVisible)
{
    s_cursor_visible = fVisible;
#ifdef MCH_CURSOR_SHAPE
    mch_update_cursor();
#endif
}


/*
 * write `cchToWrite' characters in `pchBuf' to the screen
 */
    static BOOL
write_chars(
    LPCSTR pchBuf,
    DWORD  cchToWrite,
    DWORD* pcchWritten)
{
    BOOL f;
    COORD coord = g_coord;

    FillConsoleOutputAttribute(g_hConOut, g_attrCurrent, cchToWrite,
			       coord, pcchWritten);
    f = WriteConsoleOutputCharacter(g_hConOut, pchBuf, cchToWrite,
				    coord, pcchWritten);

    g_coord.X += (SHORT) *pcchWritten;

    while (g_coord.X > g_srScrollRegion.Right)
    {
	g_coord.X -= (SHORT) Columns;
	if (g_coord.Y < g_srScrollRegion.Bottom)
	    g_coord.Y++;
    }

    gotoxy(g_coord.X + 1, g_coord.Y + 1);

    return f;
}


/*
 * mch_write(): write the output buffer to the screen, translating ESC
 * sequences into calls to console output routines.
 */
    void
mch_write(
    char_u  *s,
    int	    len)
{
    s[len] = NUL;

    if (!term_console)
    {
	write(1, s, (unsigned)len);
	return;
    }

    /* translate ESC | sequences into faked bios calls */
    while (len--)
    {
	/* optimization: use one single write_chars for runs of text,
	 * rather than once per character  It ain't curses, but it helps. */

	DWORD  prefix = strcspn(s, "\n\r\b\a\033");

	if (p_wd)
	{
	    WaitForChar(p_wd);
	    if (prefix)
		prefix = 1;
	}

	if (prefix)
	{
	    DWORD nWritten;

	    if (write_chars(s, prefix, &nWritten))
	    {
#ifdef MCH_WRITE_DUMP
		if (fdDump)
		{
		    fputc('>', fdDump);
		    fwrite(s, sizeof(char_u), nWritten, fdDump);
		    fputs("<\n", fdDump);
		}
#endif
		len -= (nWritten - 1);
		s += nWritten;
	    }
	}
	else if (s[0] == '\n')
	{
	    /* \n, newline: go to the beginning of the next line or scroll */
	    if (g_coord.Y == g_srScrollRegion.Bottom)
	    {
		scroll(1);
		gotoxy(g_srScrollRegion.Left + 1, g_srScrollRegion.Bottom + 1);
	    }
	    else
	    {
		gotoxy(g_srScrollRegion.Left + 1, g_coord.Y + 2);
	    }
#ifdef MCH_WRITE_DUMP
	    if (fdDump)
		fputs("\\n\n", fdDump);
#endif
	    s++;
	}
	else if (s[0] == '\r')
	{
	    /* \r, carriage return: go to beginning of line */
	    gotoxy(g_srScrollRegion.Left+1, g_coord.Y + 1);
#ifdef MCH_WRITE_DUMP
	    if (fdDump)
		fputs("\\r\n", fdDump);
#endif
	    s++;
	}
	else if (s[0] == '\b')
	{
	    /* \b, backspace: move cursor one position left */
	    if (g_coord.X > g_srScrollRegion.Left)
		g_coord.X--;
	    else if (g_coord.Y > g_srScrollRegion.Top)
	    {
		g_coord.X = g_srScrollRegion.Right;
		g_coord.Y--;
	    }
	    gotoxy(g_coord.X + 1, g_coord.Y + 1);
#ifdef MCH_WRITE_DUMP
	    if (fdDump)
		fputs("\\b\n", fdDump);
#endif
	    s++;
	}
	else if (s[0] == '\a')
	{
	    /* \a, bell */
	    MessageBeep(0xFFFFFFFF);
#ifdef MCH_WRITE_DUMP
	    if (fdDump)
		fputs("\\a\n", fdDump);
#endif
	    s++;
	}
	else if (s[0] == ESC && len >= 3-1 && s[1] == '|')
	{
#ifdef MCH_WRITE_DUMP
	    char_u* old_s = s;
#endif
	    char_u* p;
	    int arg1 = 0, arg2 = 0;

	    switch (s[2])
	    {
	    /* one or two numeric arguments, separated by ';' */

	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		p = s + 2;
		arg1 = getdigits(&p);	    /* no check for length! */
		if (p > s + len)
		    break;

		if (*p == ';')
		{
		    ++p;
		    arg2 = getdigits(&p);   /* no check for length! */
		    if (p > s + len)
			break;

		    if (*p == 'H')
			gotoxy(arg2, arg1);
		    else if (*p == 'r')
			set_scroll_region(0, arg1 - 1, Columns - 1, arg2 - 1);
		}
		else if (*p == 'A')
		{
		    /* move cursor up arg1 lines in same column */
		    gotoxy(g_coord.X + 1,
			   max(g_srScrollRegion.Top, g_coord.Y - arg1) + 1);
		}
		else if (*p == 'C')
		{
		    /* move cursor right arg1 columns in same line */
		    gotoxy(min(g_srScrollRegion.Right, g_coord.X + arg1) + 1,
			   g_coord.Y + 1);
		}
		else if (*p == 'H')
		{
		    gotoxy(1, arg1);
		}
		else if (*p == 'L')
		{
		    insert_lines(arg1);
		}
		else if (*p == 'm')
		{
		    if (arg1 == 0)
			normvideo();
		    else
			textattr((WORD) arg1);
		}
		else if (*p == 'f')
		{
		    textcolor((WORD) arg1);
		}
		else if (*p == 'b')
		{
		    textbackground((WORD) arg1);
		}
		else if (*p == 'M')
		{
		    delete_lines(arg1);
		}

		len -= p - s;
		s = p + 1;
		break;


	    /* Three-character escape sequences */

	    case 'A':
		/* move cursor up one line in same column */
		gotoxy(g_coord.X + 1,
		       max(g_srScrollRegion.Top, g_coord.Y - 1) + 1);
		goto got3;

	    case 'B':
		visual_bell();
		goto got3;

	    case 'C':
		/* move cursor right one column in same line */
		gotoxy(min(g_srScrollRegion.Right, g_coord.X + 1) + 1,
		       g_coord.Y + 1);
		goto got3;

	    case 'E':
		termcap_mode_end();
		goto got3;

	    case 'F':
		standout();
		goto got3;

	    case 'f':
		standend();
		goto got3;

	    case 'H':
		gotoxy(1, 1);
		goto got3;

	    case 'j':
		clear_to_end_of_display();
		goto got3;

	    case 'J':
		clear_screen();
		goto got3;

	    case 'K':
		clear_to_end_of_line();
		goto got3;

	    case 'L':
		insert_lines(1);
		goto got3;

	    case 'M':
		delete_lines(1);
		goto got3;

	    case 'S':
		termcap_mode_start();
		goto got3;

	    case 'V':
		cursor_visible(TRUE);
		goto got3;

	    case 'v':
		cursor_visible(FALSE);
		goto got3;

	    got3:
		s += 3;
		len -= 2;
	    }

#ifdef MCH_WRITE_DUMP
	    if (fdDump)
	    {
		fputs("ESC | ", fdDump);
		fwrite(old_s + 2, sizeof(char_u), s - old_s - 2, fdDump);
		fputc('\n', fdDump);
	    }
#endif
	}
	else
	{
	    /* Write a single character */
	    DWORD nWritten;

	    if (write_chars(s, 1, &nWritten))
	    {
#ifdef MCH_WRITE_DUMP
		if (fdDump)
		{
		    fputc('>', fdDump);
		    fwrite(s, sizeof(char_u), nWritten, fdDump);
		    fputs("<\n", fdDump);
		}
#endif

		len -= (nWritten - 1);
		s += nWritten;
	    }
	}
    }

#ifdef MCH_WRITE_DUMP
    if (fdDump)
	fflush(fdDump);
#endif
}

#endif /* FEAT_GUI_W32 */


/*
 * Delay for half a second.
 */
    void
mch_delay(
    long    msec,
    int	    ignoreinput)
{
#ifdef FEAT_GUI_W32
    Sleep((int)msec);	    /* never wait for input */
#else
    if (ignoreinput)
	Sleep((int)msec);
    else
	WaitForChar(msec);
#endif
}


/*
 * this version of remove is not scared by a readonly (backup) file
 */
    int
mch_remove(char_u *name)
{
    SetFileAttributes(name, FILE_ATTRIBUTE_NORMAL);
    return DeleteFile(name) ? 0 : -1;
}


/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
    void
mch_breakcheck()
{
#ifndef FEAT_GUI_W32	    /* never used */
    if (g_fCtrlCPressed || g_fCBrkPressed)
    {
	g_fCtrlCPressed = g_fCBrkPressed = FALSE;
	got_int = TRUE;
    }
#endif
}


/*
 * How much memory is available?
 * Return sum of available physical and page file memory.
 */
    long_u
mch_avail_mem(
    int special)
{
    MEMORYSTATUS	ms;

    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return(ms.dwAvailPhys + ms.dwAvailPageFile);
}


#ifdef FEAT_EVAL
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
typedef LPTSTR (*MYSTRPROCSTR)(LPTSTR);
typedef LPTSTR (*MYINTPROCSTR)(int);
typedef int (*MYSTRPROCINT)(LPTSTR);
typedef int (*MYINTPROCINT)(int);

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
	EMSG2(_("Library call failed for \"%s\"()"), funcname);
	return FAIL;
    }

    return OK;
}
#endif

/*
 * mch_rename() works around a bug in rename (aka MoveFile) in
 * Windows 95: rename("foo.bar", "foo.bar~") will generate a
 * file whose short file name is "FOO.BAR" (its long file name will
 * be correct: "foo.bar~").  Because a file can be accessed by
 * either its SFN or its LFN, "foo.bar" has effectively been
 * renamed to "foo.bar", which is not at all what was wanted.  This
 * seems to happen only when renaming files with three-character
 * extensions by appending a suffix that does not include ".".
 * Windows NT gets it right, however, with an SFN of "FOO~1.BAR".
 *
 * Like rename(), returns 0 upon success, non-zero upon failure.
 * Should probably set errno appropriately when errors occur.
 */
    int
mch_rename(
    const char	*pszOldFile,
    const char	*pszNewFile)
{
    char	szTempFile[_MAX_PATH+1];
    char	szNewPath[_MAX_PATH+1];
    char	*pszFilePart;
    HANDLE	hf;

    /*
     * No need to play tricks if not running Windows 95
     */
    if (!mch_windows95())
	return rename(pszOldFile, pszNewFile);

    /* Get base path of new file name.  Undocumented feature: If pszNewFile is
     * a directory, no error is returned and pszFilePart will be NULL. */
    if (GetFullPathName(pszNewFile, _MAX_PATH, szNewPath, &pszFilePart) == 0
	    || pszFilePart == NULL)
	return -1;
    *pszFilePart = NUL;

    /* Get (and create) a unique temporary file name in directory of new file */
    if (GetTempFileName(szNewPath, "VIM", 0, szTempFile) == 0)
	return -2;

    /* blow the temp file away */
    if (!DeleteFile(szTempFile))
	return -3;

    /* rename old file to the temp file */
    if (!MoveFile(pszOldFile, szTempFile))
	return -4;

    /* now create an empty file called pszOldFile; this prevents the operating
     * system using pszOldFile as an alias (SFN) if we're renaming within the
     * same directory.  For example, we're editing a file called
     * filename.asc.txt by its SFN, filena~1.txt.  If we rename filena~1.txt
     * to filena~1.txt~ (i.e., we're making a backup while writing it), the
     * SFN for filena~1.txt~ will be filena~1.txt, by default, which will
     * cause all sorts of problems later in buf_write.  So, we create an empty
     * file called filena~1.txt and the system will have to find some other
     * SFN for filena~1.txt~, such as filena~2.txt
     */
    if ((hf = CreateFile(pszOldFile, GENERIC_WRITE, 0, NULL, CREATE_NEW,
		    FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
	return -5;
    if (!CloseHandle(hf))
	return -6;

    /* rename the temp file to the new file */
    if (!MoveFile(szTempFile, pszNewFile))
    {
	/* Renaming failed.  Rename the file back to its old name, so that it
	 * looks like nothing happened. */
	(void)MoveFile(szTempFile, pszOldFile);

	return -7;
    }

    /* Seems to be left around on Novell filesystems */
    DeleteFile(szTempFile);

    /* finally, remove the empty old file */
    if (!DeleteFile(pszOldFile))
	return -8;

    return 0;	/* success */
}

/*
 * Get the default shell for the current hardware platform
 */
    char*
default_shell()
{
    char* psz = NULL;

    PlatformId();

    if (g_PlatformId == VER_PLATFORM_WIN32_NT)		/* Windows NT */
	psz = "cmd.exe";
    else if (g_PlatformId == VER_PLATFORM_WIN32_WINDOWS) /* Windows 95 */
	psz = "command.com";

    return psz;
}


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

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
clip_mch_request_selection()
{
    int	    type = 0;
    HGLOBAL hMem;
    char_u  *str = NULL;

    /*
     * Don't pass GetActiveWindow() as an argument to OpenClipboard() because
     * then we can't paste back into the same window for some reason - webb.
     */
    if (OpenClipboard(NULL))
    {
	/* Check for vim's own clipboard format first */
	if ((hMem = GetClipboardData(clipboard.format)) != NULL)
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
		/* TRACE("Got '%c' type\n", str[-1]); */
	}
	/* Otherwise, check for the normal text format */
	else if ((hMem = GetClipboardData(CF_TEXT)) != NULL)
	{
	    str = (char_u *)GlobalLock(hMem);
	    if (str != NULL)
		type = (vim_strchr((char*) str, '\r') != NULL) ? MLINE : MCHAR;
	    /* TRACE("TEXT\n"); */
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

		temp_clipboard = (char_u *)lalloc(STRLEN(psz) + 1, TRUE);
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
			len = (pszNL != NULL) ?  pszNL - psz : STRLEN(psz);
			STRNCPY(pszTemp, psz, len);
			pszTemp += len;
			if (pszNL != NULL)
			    *pszTemp++ = '\n';
			psz += len + ((pszNL != NULL) ? 2 : 0);
		    }
		    *pszTemp = NUL;
		    clip_yank_selection(type, temp_clipboard,
					    (long)(pszTemp - temp_clipboard));
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
clip_mch_set_selection()
{
    char_u  *str = NULL;
    long_u  cch;
    int	    type;
    HGLOBAL hMem = NULL;
    HGLOBAL hMemVim = NULL;
    LPSTR   lpszMem = NULL;
    LPSTR   lpszMemVim = NULL;

    /* If the '*' register isn't already filled in, fill it in now */
    clipboard.owned = TRUE;
    clip_get_selection();
    clipboard.owned = FALSE;

    type = clip_convert_selection(&str, &cch);

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
		SetClipboardData(clipboard.format, hMemVim);
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


