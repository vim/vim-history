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
 * George V. Reilly <georgere@microsoft.com> wrote most of this.
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

#ifndef FEAT_GUI_W32
/* Win32 Console handles for input and output */
# ifndef FEAT_SNIFF	/* used in if_sniff.c */
static
# endif
       HANDLE g_hConIn	= INVALID_HANDLE_VALUE;
static HANDLE g_hSavOut = INVALID_HANDLE_VALUE;
static HANDLE g_hCurOut = INVALID_HANDLE_VALUE;
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

/*
 * The old solution for opening a console is still there.
 */
//#define OLD_CONSOLE_STUFF

#ifdef OLD_CONSOLE_STUFF
static void mch_open_console(void);
static void mch_close_console(int wait_key, DWORD ret);
#endif
garray_t error_ga = {0, 0, 0, 0, NULL};

#ifndef FEAT_GUI_W32
static int suppress_winsize = 1;	/* don't fiddle with console */
#endif

/* This symbol is not defined in older versions of the SDK or Visual C++ */

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

static void PlatformId(void);
static DWORD g_PlatformId;

/*
 * Set g_PlatformId to VER_PLATFORM_WIN32_NT (NT) or
 * VER_PLATFORM_WIN32_WINDOWS (Win95).
 */
    static void
PlatformId(void)
{
    static int done = FALSE;

    if (!done)
    {
	OSVERSIONINFO ovi;

	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	g_PlatformId = ovi.dwPlatformId;
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

    SetConsoleCursorInfo(g_hCurOut, &ConsoleCursorInfo);
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

#ifdef FEAT_GUI_W32

#ifndef __MINGW32__
# include <shellapi.h>	/* required for FindExecutable() */
#endif

    static int
executable_exists(char *name)
{
    char vimrun_location[2 * _MAX_PATH + 2];
    char widename[2 * _MAX_PATH];

    /* There appears to be a bug in FindExecutableA() on Windows NT.
     * Use FindExecutableW() instead... */
    if (g_PlatformId == VER_PLATFORM_WIN32_NT)
    {
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
						 (LPWSTR)widename, _MAX_PATH);
	if (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
				     (LPWSTR)vimrun_location) > (HINSTANCE)32)
	    return TRUE;
    }
    else
    {
	if (FindExecutableA((LPCTSTR)name, (LPCTSTR)"",
				     (LPTSTR)vimrun_location) > (HINSTANCE)32)
	    return TRUE;
    }
    return FALSE;
}

/*
 * GUI version of mch_shellinit().
 */
    void
mch_shellinit()
{
#ifndef __MINGW32__
    extern int _fmode;
#endif

    PlatformId();

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

#else /* FEAT_GUI_W32 */

static char g_szOrigTitle[256];
static int  g_fWindInitCalled = FALSE;
static CONSOLE_CURSOR_INFO g_cci;
static DWORD g_cmodein = 0;
static DWORD g_cmodeout = 0;

/*
 * Because of a bug in the Windows 95 Console, we need to set the screen size
 * back when switching screens.
 */
static int g_nOldRows = 0;
static int g_nOldColumns = 0;

/*
 * non-GUI version of mch_shellinit().
 */
    void
mch_shellinit()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
#ifndef __MINGW32__
    extern int _fmode;
#endif

    PlatformId();

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
    g_hSavOut = GetStdHandle(STD_OUTPUT_HANDLE);
    g_hCurOut = g_hSavOut;

    /* Get current text attributes */
    GetConsoleScreenBufferInfo(g_hSavOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
    if (cterm_normal_fg_color == 0)
	cterm_normal_fg_color = (g_attrCurrent & 0xf) + 1;
    if (cterm_normal_bg_color == 0)
	cterm_normal_bg_color = ((g_attrCurrent >> 4) & 0xf) + 1;
    GetConsoleCursorInfo(g_hSavOut, &g_cci);

    /* set termcap codes to current text attributes */
    update_tcap(csbi.wAttributes);

    GetConsoleMode(g_hConIn,  &g_cmodein);
    GetConsoleMode(g_hSavOut, &g_cmodeout);

    GetConsoleTitle(g_szOrigTitle, sizeof(g_szOrigTitle));
    ui_get_shellsize();

    g_nOldRows = Rows;
    g_nOldColumns = Columns;

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
    /*
     * We don't really want to jump to our own screen yet; do that after
     * starttermcap().	This flashes the window, sorry about that, but
     * otherwise "vim -r" doesn't work.
     */
    g_hCurOut = g_hSavOut;
    SetConsoleActiveScreenBuffer(g_hCurOut);
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
    if (full_screen)
    {
	out_char('\r');
	out_char('\n');
	out_flush();
    }

    if (g_fWindInitCalled)
	settmode(TMODE_COOK);

    if (g_hConOut != INVALID_HANDLE_VALUE)
    {
	(void)CloseHandle(g_hConOut);

	if (g_hSavOut != INVALID_HANDLE_VALUE)
	{
	    SetConsoleTextAttribute(g_hSavOut, g_attrDefault);
	    SetConsoleCursorInfo(g_hSavOut, &g_cci);
	}
    }

    ml_close_all(TRUE);		/* remove all memfiles */

    if (g_fWindInitCalled)
    {
#ifdef FEAT_TITLE
	mch_restore_title(3);
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

    exit(r);
}
#endif /* FEAT_GUI_W32 */


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
 * Init the tables for toupper() and tolower().
 */
    void
win32_init(void)
{
    int		i;

    /* Init the tables for toupper() and tolower() */
    for (i = 0; i < 256; ++i)
	toupper_tab[i] = tolower_tab[i] = i;
    CharUpperBuff(toupper_tab, 256);
    CharLowerBuff(tolower_tab, 256);
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    int
mch_input_isatty()
{
#ifdef FEAT_GUI_W32
    return OK;	    /* GUI always has a tty */
#else
    if (isatty(read_cmd_fd))
	return TRUE;
    return FALSE;
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


#ifdef FEAT_TITLE
/*
 * mch_settitle(): set titlebar of our window
 * Can the icon also be set?
 */
    void
mch_settitle(
    char_u *title,
    char_u *icon)
{
#ifdef FEAT_GUI_W32
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
#ifndef FEAT_GUI_W32
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
 * Return TRUE if we can restore the icon (we can't)
 */
    int
mch_can_restore_icon()
{
    return FALSE;
}
#endif /* FEAT_TITLE */


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

    if (fname == NULL)		/* always fail */
	return FAIL;

#ifdef __BORLANDC__
    if (*buf == NUL) /* Borland behaves badly here - make it consistent */
	nResult = mch_dirname(buf, len);
    else
#endif
	if (_fullpath(buf, fname, len - 1) == NULL)
	STRNCPY(buf, fname, len);   /* failed, use the relative path name */
    else
	nResult = OK;

    fname_case(buf);

    /* Append backslash to directory names; after fname_case() because it
     * removes the backslash again. */
    if (nResult == OK && mch_isdir(buf) && buf[STRLEN(buf) - 1] != psepc)
	STRCAT(buf, pseps);

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
	    || (fname[0] == fname[1] && fname[0] == '/' || fname[0] == '\\'))
	return TRUE;

    /* A name that can't be made absolute probably isn't absolute. */
    if (mch_FullName(fname, szName, _MAX_PATH, FALSE) == FAIL)
	return FALSE;

    return strcoll(fname, szName) == 0;
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

#include <aclapi.h>

struct my_acl
{
    PSECURITY_DESCRIPTOR    pSecurityDescriptor;
    PSID		    pSidOwner;
    PSID		    pSidGroup;
    PACL		    pDacl;
    PACL		    pSacl;
};

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_t
mch_get_acl(fname)
    char_u	*fname;
{
    struct my_acl   *p;

    p = (struct my_acl *)alloc_clear((unsigned)sizeof(struct my_acl));
    if (p != NULL)
    {
	if (GetNamedSecurityInfo(
		    (LPTSTR)fname,		// Abstract filename
		    SE_FILE_OBJECT,		// File Object
		    // Retrieve the entire security descriptor.
		    OWNER_SECURITY_INFORMATION |
			GROUP_SECURITY_INFORMATION |
			DACL_SECURITY_INFORMATION |
			SACL_SECURITY_INFORMATION,
		    &p->pSidOwner,		// Ownership information.
		    &p->pSidGroup,		// Group membership.
		    &p->pDacl,			// Discretionary information.
		    &p->pSacl,			// For auditing purposes.
		    &p->pSecurityDescriptor
				) != ERROR_SUCCESS)
	{
	    mch_free_acl((vim_acl_t)p);
	    p = NULL;
	}
    }
    return (vim_acl_t)p;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 * Errors are ignored.
 */
    void
mch_set_acl(fname, acl)
    char_u	*fname;
    vim_acl_t	acl;
{
    struct my_acl   *p = (struct my_acl *)acl;

    if (p != NULL)
	(void)SetNamedSecurityInfo(
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
}

    void
mch_free_acl(acl)
    vim_acl_t	acl;
{
    struct my_acl   *p = (struct my_acl *)acl;

    if (p != NULL)
    {
	LocalFree(p->pSecurityDescriptor);	// Free the memory just in case
	vim_free(p);
    }
}

#ifdef FEAT_GUI_W32
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
#else

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

    GetConsoleMode(g_hConIn,  &cmodein);
    GetConsoleMode(g_hCurOut, &cmodeout);

    if (tmode == TMODE_RAW)
    {
	cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
		     ENABLE_ECHO_INPUT);
	cmodein |= (
#ifdef FEAT_MOUSE
	    (g_fMouseActive ? ENABLE_MOUSE_INPUT : 0) |
#endif
	    ENABLE_WINDOW_INPUT);

	SetConsoleMode(g_hConIn, cmodein);

	cmodeout &= ~(ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
	SetConsoleMode(g_hCurOut, cmodeout);
	SetConsoleCtrlHandler(handler_routine, TRUE);
    }
    else /* cooked */
    {
	cmodein =  g_cmodein;
	cmodeout = g_cmodeout;
	SetConsoleMode(g_hConIn,  g_cmodein);
	SetConsoleMode(g_hCurOut, g_cmodeout);

	SetConsoleCtrlHandler(handler_routine, FALSE);
    }

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fprintf(fdDump, "mch_settmode(%s, CurOut = %s, in = %x, out = %x)\n",
		tmode == TMODE_RAW ? "raw" :
				    tmode == TMODE_COOK ? "cooked" : "normal",
		(g_hCurOut == g_hSavOut ? "Sav" : "Con"),
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

    if (GetConsoleScreenBufferInfo(g_hCurOut, &csbi))
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
    int		    did_start_termcap = FALSE;

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
	fflush(fdDump);
    }
#endif

    /*
     * The resizing MUST be done while in our own console buffer, otherwise it
     * will never be possible to restore the old one, and we will crash on
     * exit in Windows 95.
     */
    if (g_hCurOut != g_hConOut)
    {
	termcap_mode_start();
	did_start_termcap = TRUE;
    }

    /* get the largest size we can size the console window to */
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    /* define the new console window size and scroll position */
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
    srWindowRect.Right =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    if (GetConsoleScreenBufferInfo(g_hCurOut, &csbi))
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

    if (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
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

    if (did_start_termcap)
	termcap_mode_end();
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

    coordScreen = GetLargestConsoleWindowSize(g_hCurOut);

    /* Clamp Rows and Columns to reasonable values */
    if (Rows > coordScreen.Y)
	Rows = coordScreen.Y;
    if (Columns > coordScreen.X)
	Columns = coordScreen.X;

    ResizeConBufAndWindow(g_hCurOut, Columns, Rows);
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


/*
 * We have no job control, so fake it by starting a new shell.
 */
    void
mch_suspend()
{
    suspend_shell();
}

#if defined(FEAT_GUI_W32) || defined(PROTO)

#ifdef OLD_CONSOLE_STUFF
/*
 * Functions to open and close a console window.
 * The open function sets the size of the window to 25x80, to avoid problems
 * with Windows 95.  In the long term should make sure that the "close" icon
 * cannot crash vim (it doesn't do so at the moment!).
 * The close function waits for the user to press a key before closing the
 * window.
 */

    static BOOL
ConsoleCtrlHandler(DWORD what)
{
    return TRUE;
}

    static void
mch_open_console(void)
{
    COORD	size;
    SMALL_RECT	window_size;
    HANDLE	console_handle;

    AllocConsole();

    /* On windows 95, we must use a 25x80 console size, to avoid trouble with
     * some DOS commands */
    if (g_PlatformId != VER_PLATFORM_WIN32_NT)
    {
	console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	size.X = 80;
	size.Y = 25;
	window_size.Left = 0;
	window_size.Top = 0;
	window_size.Right = 79;
	window_size.Bottom = 24;

	/* First set the window size, then also resize the screen buffer, to
	 * avoid a scrollbar */
	SetConsoleWindowInfo(console_handle, TRUE, &window_size);
	SetConsoleScreenBufferSize(console_handle, size);
    }

    SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleCtrlHandler, TRUE);
}

    static void
mch_close_console(int wait_key, DWORD ret)
{
    if (wait_key)
    {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
	DWORD number;
	static char message[] = N_("\nPress any key to close this window...");
	static char err_buf[80];
	char buffer[1];

	if (ret)
	{
	    sprintf(err_buf, _("\n%ld returned."), ret);
	    WriteConsole(hStderr, err_buf, strlen(err_buf), &number, NULL);
	}
	/* Write a message to the user */
	WriteConsole(hStderr, _(message), sizeof(_(message))-1, &number, NULL);

	/* Clear the console input buffer, and set the input to "raw" mode */
	FlushConsoleInputBuffer(hStdin);
	SetConsoleMode(hStdin, 0);

	/* Wait for a keypress */
	ReadConsole(hStdin, buffer, 1, &number, NULL);
    }

    /* Close the console window */
    FreeConsole();
}
#endif

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
		MessageBox(0, p, "Vim", MB_TASKMODAL|MB_SETFOREGROUND);
		break;
	    }
	ga_clear(&error_ga);
    }
}

/*
 * Specialised version of system() for Win32 GUI mode.
 * This version proceeds as follows:
 *    1. Create a console window for use by the subprocess
 *    2. Run the subprocess (it gets the allocated console by default)
 *    3. Wait for the subprocess to terminate and get its exit code
 *    4. Prompt the user to press a key to close the console window
 */
static BOOL fUseConsole = TRUE;

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

#ifdef OLD_CONSOLE_STUFF
    /* Create a console for the process. This lets us write a termination
     * message at the end, and wait for the user to close the console
     * window manually...
     */
    if (fUseConsole)
    {
	mch_open_console();

	/*
	 * Write the command to the console, so the user can see what is going
	 * on.
	 */
	{
	    HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
	    DWORD number;

	    WriteConsole(hStderr, cmd, STRLEN(cmd), &number, NULL);
	    WriteConsole(hStderr, "\n", 1, &number, NULL);
	}
    }
#else
    /* There is a strange error on Windows 95 when using "c:\\command.com".
     * When the "c:\\" is left out it works OK...? */
    if (mch_windows95()
	    && (STRNICMP(cmd, "c:/command.com", 14) == 0
		|| STRNICMP(cmd, "c:\\command.com", 14) == 0))
	cmd += 3;
#endif

    /* Now, run the command */
    CreateProcess(NULL,			/* Executable name */
		  cmd,			/* Command to execute */
		  NULL,			/* Process security attributes */
		  NULL,			/* Thread security attributes */
		  FALSE,		/* Inherit handles */
		  CREATE_DEFAULT_ERROR_MODE |	/* Creation flags */
#ifdef OLD_CONSOLE_STUFF
		  0,
#else
		  CREATE_NEW_CONSOLE,
#endif
		  NULL,			/* Environment */
		  NULL,			/* Current directory */
		  &si,			/* Startup information */
		  &pi);			/* Process information */


    /* Wait for the command to terminate before continuing */
    if (fUseConsole)
    {
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
    }

    /* Close the handles to the subprocess, so that it goes away */
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    /* Close the console window. If we are not redirecting output, wait for
     * the user to press a key.
     */
#ifdef OLD_CONSOLE_STUFF
    if (fUseConsole)
	mch_close_console(!(options & SHELL_DOOUT), ret);
#endif

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
#ifndef FEAT_GUI_W32
    int	    stopped_termcap_mode = FALSE;
#endif

    out_flush();

#ifndef FEAT_GUI_W32
    /*
     * Switch to non-termcap mode, otherwise ":r !ls" may crash.
     * The only exception is "!start".
     */
    if (g_hCurOut == g_hConOut
	    && (cmd == NULL || STRNICMP(cmd, "start ", 6) != 0))
    {
	termcap_mode_end();
	stopped_termcap_mode = TRUE;
    }
#endif

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

		if (*cmd == '"')
		    *newcmd = '"';
		STRCPY(newcmd + ((*cmd == '"') ? 1 : 0), cmdbase);

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
		sprintf((char *)newcmd, "%s%s %s %s",
#if defined(OLD_CONSOLE_STUFF) || !defined(FEAT_GUI_W32)
			"",
#else
			/*
			 * Do we need to use "vimrun"?
			 */
			((options & SHELL_DOOUT) || s_dont_use_vimrun) ?
			    "" : vimrun_path,
#endif
			p_sh,
			p_shcf,
			cmd);
		x = mch_system((char *)newcmd, options);
	    }
	    vim_free(newcmd);
	}
    }

    settmode(TMODE_RAW);	    /* set to raw mode */

    if (x && !(options & SHELL_SILENT)
#ifdef FEAT_GUI_W32
	    && !fUseConsole
#endif
       )
    {
	smsg(_("%d returned"), x);
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

#ifndef FEAT_GUI_W32
    if (stopped_termcap_mode)
	termcap_mode_start();
#endif

    return x;
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

    start_dot_ok = (s[0] == '.' || s[0] == '*');

    /* If we are expanding wildcards, we try both files and directories */
    if ((hFind = FindFirstFile(buf, &fb)) != INVALID_HANDLE_VALUE)
	while (c)
	{
	    STRCPY(s, fb.cFileName);

	    /*
	     * Ignore "." and "..".  When more follows, this must be a
	     * directory.
	     * Find*File() returns matches that start with a '.', even though
	     * the pattern doesn't start with '.'.  Filter them out manually.
	     */
	    if ((s[0] != '.'
			|| (start_dot_ok
			    && s[1] != NUL
			    && (s[1] != '.' || s[2] != NUL)))
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


#ifndef FEAT_GUI_W32
/*
 * Copy the contents of screen buffer hSrc to the bottom-left corner
 * of screen buffer hDst.
 */
    static void
copy_screen_buffer_text(
    HANDLE hSrc,
    HANDLE hDst)
{
    int	    i, j, nSrcWidth, nSrcHeight, nDstWidth, nDstHeight;
    COORD   coordOrigin;
    DWORD   dwDummy;
    LPSTR   pszOldText;
    CONSOLE_SCREEN_BUFFER_INFO csbiSrc, csbiDst;

#ifdef MCH_WRITE_DUMP
    if (fdDump)
    {
	fprintf(fdDump, "copy_screen_buffer_text(%s, %s)\n",
		(hSrc == g_hSavOut ? "Sav" : "Con"),
		(hDst == g_hSavOut ? "Sav" : "Con"));
	fflush(fdDump);
    }
#endif

    GetConsoleScreenBufferInfo(hSrc, &csbiSrc);
    nSrcWidth =  csbiSrc.srWindow.Right  - csbiSrc.srWindow.Left + 1;
    nSrcHeight = csbiSrc.srWindow.Bottom - csbiSrc.srWindow.Top  + 1;

    GetConsoleScreenBufferInfo(hDst, &csbiDst);
    nDstWidth =  csbiDst.srWindow.Right  - csbiDst.srWindow.Left + 1;
    nDstHeight = csbiDst.srWindow.Bottom - csbiDst.srWindow.Top  + 1;

    pszOldText = (LPSTR) alloc(nDstHeight * nDstWidth);

    /* clear the top few lines if Src shorter than Dst */
    for (i = 0;  i < nDstHeight - nSrcHeight;  i++)
    {
	for (j = 0;  j < nDstWidth;  j++)
	    pszOldText[i * nDstWidth + j] = ' ';
    }

    /* Grab text off source screen. */
    coordOrigin.X = 0;
    coordOrigin.Y = (SHORT) max(0, csbiSrc.srWindow.Bottom + 1 - nDstHeight);

    for (i = 0;  i < min(nSrcHeight, nDstHeight);  i++)
    {
	LPSTR psz = pszOldText
		     + (i + max(0, nDstHeight - nSrcHeight)) * nDstWidth;

	ReadConsoleOutputCharacter(hSrc, psz, min(nDstWidth, nSrcWidth),
				   coordOrigin, &dwDummy);
	coordOrigin.Y++;

	/* clear the last few columns if Src narrower than Dst */
	for (j = nSrcWidth;  j < nDstWidth;  j++)
	    psz[j] = ' ';
    }

    /* paste Src's text onto Dst */
    coordOrigin.Y = csbiDst.srWindow.Top;
    WriteConsoleOutputCharacter(hDst, pszOldText, nDstHeight * nDstWidth,
				coordOrigin, &dwDummy);
    vim_free(pszOldText);
}


/* keep track of state of original console window */
static SMALL_RECT   g_srOrigWindowRect;
static COORD	    g_coordOrig;
static WORD	    g_attrSave = 0;


/*
 * Start termcap mode by switching to our allocated screen buffer
 */
    static void
termcap_mode_start(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwDummy;
    COORD coord;
    WORD wAttr = (WORD) (g_attrSave ? g_attrSave : g_attrCurrent);

    GetConsoleScreenBufferInfo(g_hSavOut, &csbi);
    g_srOrigWindowRect = csbi.srWindow;

    g_coordOrig.X = 0;
    g_coordOrig.Y = csbi.dwCursorPosition.Y;

    if (g_hConOut == INVALID_HANDLE_VALUE)
    {
	/* Create a new screen buffer in which we do all of our editing.
	 * This means we can restore the original screen when we finish. */
	g_hConOut = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ,
					      0, (LPSECURITY_ATTRIBUTES) NULL,
					      CONSOLE_TEXTMODE_BUFFER,
					      (LPVOID) NULL);

	/* initialize the new screenbuffer with what is in the current one */
	coord.X = coord.Y = 0;
	FillConsoleOutputCharacter(g_hConOut, ' ',
					     Rows * Columns, coord, &dwDummy);
	FillConsoleOutputAttribute(g_hConOut, wAttr,
					     Rows * Columns, coord, &dwDummy);
	copy_screen_buffer_text(g_hSavOut, g_hConOut);
    }

    g_hCurOut = g_hConOut;
    SetConsoleActiveScreenBuffer(g_hCurOut);

    check_shellsize();
    ResizeConBufAndWindow(g_hCurOut, Columns, Rows);
    set_scroll_region(0, 0, Columns - 1, Rows - 1);

#ifdef FEAT_TITLE
    resettitle();
#endif

    textattr(wAttr);
}


/*
 * Turn off termcap mode by restoring the screen buffer we had upon startup
 */
    static void
termcap_mode_end(void)
{
    g_attrSave = g_attrCurrent;

    ResizeConBufAndWindow(g_hCurOut, g_nOldColumns, g_nOldRows);

    /* This weird Sleep(0) is required to allow Windows to really resize the
     * console window.  Apparently it's done asynchronously, which may cause
     * the following screenbuffer switch to go wrong */
    Sleep(0);

    check_shellsize();

    g_hCurOut = g_hSavOut;
    SetConsoleActiveScreenBuffer(g_hCurOut);
    SetConsoleCursorInfo(g_hCurOut, &g_cci);

    normvideo();

    if (!p_rs)
	g_coordOrig.Y = g_srOrigWindowRect.Bottom;

    SetConsoleCursorPosition(g_hCurOut, g_coordOrig);

    if (!p_rs)
	copy_screen_buffer_text(g_hConOut, g_hSavOut);

    clear_chars(g_coordOrig,
		g_srOrigWindowRect.Right - g_srOrigWindowRect.Left + 1);

#ifdef FEAT_TITLE
    mch_restore_title(3);
#endif

    SetConsoleMode(g_hConIn,  g_cmodein);
    SetConsoleMode(g_hSavOut, g_cmodeout);
}
#endif /* FEAT_GUI_W32 */

/*
 * Switching off termcap mode is only allowed when Columns is 80, otherwise a
 * crash may result.  It's always allowed on NT or when running the GUI.
 */
    int
can_end_termcap_mode(
    int give_msg)
{
#ifdef FEAT_GUI_W32
    return TRUE;	/* GUI starts a new console anyway */
#else
    if (g_PlatformId == VER_PLATFORM_WIN32_NT || Columns == 80)
	return TRUE;
    if (give_msg)
	msg(_("'columns' is not 80, cannot execute external commands"));
    return FALSE;
#endif
}

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

    FillConsoleOutputCharacter(g_hCurOut, ' ', n, coord, &dwDummy);
    FillConsoleOutputAttribute(g_hCurOut, g_attrCurrent, n, coord, &dwDummy);
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

    ScrollConsoleScreenBuffer(g_hCurOut, &source, NULL, dest, &fill);

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

    ScrollConsoleScreenBuffer(g_hCurOut, &source, NULL, dest, &fill);

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
    COORD coord2;

    if (x < 1 || x > (unsigned)Columns || y < 1 || y > (unsigned)Rows)
	return;

    /* Should we check against g_srScrollRegion? */

    /* external cursor coords are 1-based; internal are 0-based */
    g_coord.X = coord2.X = x - 1;
    g_coord.Y = coord2.Y = y - 1;

    /* If we are using the window buffer that we had upon startup, make
     * sure to position cursor relative to the window upon that buffer */
    if (g_hCurOut == g_hSavOut)
    {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(g_hCurOut, &csbi);
	g_srOrigWindowRect = csbi.srWindow;

	coord2.X += (SHORT) (g_srOrigWindowRect.Left);
	coord2.Y += (SHORT) (g_srOrigWindowRect.Bottom + 1 - Rows);
    }

    SetConsoleCursorPosition(g_hCurOut, coord2);
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

    SetConsoleTextAttribute(g_hCurOut, wAttr);
}


    static void
textcolor(
    WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0xf0) + wAttr;

    SetConsoleTextAttribute(g_hCurOut, g_attrCurrent);
}


    static void
textbackground(
    WORD wAttr)
{
    g_attrCurrent = (g_attrCurrent & 0x0f) + (wAttr << 4);

    SetConsoleTextAttribute(g_hCurOut, g_attrCurrent);
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

    ReadConsoleOutputAttribute(g_hCurOut, oldattrs, Rows * Columns,
			       coordOrigin, &dwDummy);
    FillConsoleOutputAttribute(g_hCurOut, attrFlash, Rows * Columns,
			       coordOrigin, &dwDummy);

    Sleep(15);	    /* wait for 15 msec */
    WriteConsoleOutputAttribute(g_hCurOut, oldattrs, Rows * Columns,
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
    COORD coord2 = g_coord;

    if (g_hCurOut == g_hSavOut)
    {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(g_hCurOut, &csbi);

	coord2.X += (SHORT) (csbi.srWindow.Left);
	coord2.Y += (SHORT) (csbi.srWindow.Bottom + 1 - Rows);
    }

    FillConsoleOutputAttribute(g_hCurOut, g_attrCurrent, cchToWrite,
			       coord2, pcchWritten);
    f = WriteConsoleOutputCharacter(g_hCurOut, pchBuf, cchToWrite,
				    coord2, pcchWritten);

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


/*
 * return non-zero if a character is available
 */
    int
mch_char_avail()
{
#ifdef FEAT_GUI_W32
    /* never used */
    return TRUE;
#else
    return WaitForChar(0L);
#endif
}


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

    /* get base path of new file name */
    if (GetFullPathName(pszNewFile, _MAX_PATH, szNewPath, &pszFilePart) == 0)
	return -1;
    else
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
