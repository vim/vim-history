/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * win32.c
 *
 * Win32 (Windows NT and Windows 95) system-dependent routines.
 * Portions lifted from the Win32 SDK samples, the MSDOS-dependent code,
 * NetHack 3.1.3, GNU Emacs 19.30, and Vile 5.5.
 *
 * George V. Reilly <gvr@halcyon.com> wrote most of this.
 * Roger Knobbe <rogerk@wonderware.com> did the initial port of Vim 3.0.
 */

#include <io.h>
#include "vim.h"
#include "globals.h"
#include "option.h"
#include "proto.h"
#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <process.h>
#include <time.h>

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef chdir
#include <direct.h>


/* Disgusting hack for getting dead keys to work properly on Windows 95.
 * See below for the gory details.  If you're only running on NT or
 * if you're an English-speaking user, you can comment this out. */

/* WARNING: this code is experimental and incomplete and it does not work.
 * Don't use it! */
/* #define WIN95_DEAD_KEYS_HACK */


/* Force all filenames to lowercase */
/* #define DOWNCASE_FILENAMES */


/* Record all output and all keyboard & mouse input */
/* #define MCH_WRITE_DUMP */

#ifdef MCH_WRITE_DUMP
FILE* fdDump = NULL;
#endif /* MCH_WRITE_DUMP */


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

/* Win32 Console handles for input and output */
static HANDLE g_hConIn  = INVALID_HANDLE_VALUE;
static HANDLE g_hSavOut = INVALID_HANDLE_VALUE;
static HANDLE g_hCurOut = INVALID_HANDLE_VALUE;
static HANDLE g_hConOut = INVALID_HANDLE_VALUE;

/* Win32 Screen buffer,coordinate,console I/O information */
static SMALL_RECT g_srScrollRegion;
static COORD      g_coord;	/* 0-based, but external coords are 1-based */

/* The attribute of the screen when the editor was started */
static WORD  g_attrDefault = 7;  /* lightgray text on black background */
static WORD  g_attrCurrent;

static int g_fCBrkPressed = FALSE;  /* set by ctrl-break interrupt */
static int g_fCtrlCPressed = FALSE; /* set when ctrl-C or ctrl-break detected */

static void termcap_mode_start();
static void termcap_mode_end();
static void clear_chars(COORD coord, DWORD n);
static void clear_screen();
static void clear_to_end_of_display();
static void clear_to_end_of_line();
static void scroll(unsigned cLines);
static void set_scroll_region(unsigned left, unsigned top,
							  unsigned right, unsigned bottom);
static void insert_lines(unsigned cLines);
static void delete_lines(unsigned cLines);
static void gotoxy(unsigned x, unsigned y);
static void normvideo();
static void textattr(WORD wAttr);
static void standout();
static void standend();
static void visual_bell();
static void cursor_visible(BOOL fVisible);
static BOOL write_chars(LPCSTR pchBuf, DWORD cchToWrite, DWORD* pcchWritten);


/* This symbol is not defined in older versions of the SDK or Visual C++ */

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

static DWORD g_PlatformId;

/*
 *	Returns VER_PLATFORM_WIN32_NT (NT) or VER_PLATFORM_WIN32_WINDOWS (Win95)
 */
	static DWORD
PlatformId()
{
	OSVERSIONINFO ovi;

	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	g_PlatformId = ovi.dwPlatformId;

	return g_PlatformId;
}



#define SHIFT  (SHIFT_PRESSED)
#define CTRL   (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)
#define ALT    (RIGHT_ALT_PRESSED  | LEFT_ALT_PRESSED)
#define ALT_GR (RIGHT_ALT_PRESSED  | LEFT_CTRL_PRESSED) 



#ifdef WIN95_DEAD_KEYS_HACK

/* The problem that this code attempts to work around is that the
 * console-mode input routines are broken on Windows 95 if you're
 * using dead keys.  A dead key is an accent key, such as acute,
 * grave, or umlaut, that doesn't produce a character by itself,
 * but when followed by another key, produces an accented character,
 * such as a-acute, e-grave, u-umlaut, n-tilde, and so on.  Very
 * useful for most European languages.  English-language keyboard
 * layouts don't use dead keys, as far as I know.
 *
 * Unfortunately, the KEY_EVENT_RECORD contains the wrong data when
 * I get the keystroke that's supposed to have an accented character.
 * The AsciiChar part contains the unaccented character instead.
 *
 * The following code (unsuccessfully) attempts to fake the dead keys.
 * I rely on the fact that the virtual key code and the scan key code
 * are actually correct for both the dead key and the following letter.
 * I set up a second thread that creates a hidden window and then sits
 * in a little message loop.  I send it all the data from the dead key
 * and the following key (each thread has its own keyboard input state),
 * and it sends its own hidden window a suitable sequence of WM_KEYDOWN
 * and WM_KEYUP messages.  TranslateMessage causes these to be turned
 * into WM_DEADCHAR and WM_CHAR messages.  Almost.  It works as long as
 * neither the dead key nor the following key are shifted, but after
 * happens, that it goes to hell in a handbasket.
 *
 * I have spent many, many hours trying all sorts of approaches to get
 * dead keys to work with Windows 95 and I am completely stumped.
 *
 * So why not just have all the input come directly via WM_CHAR?
 * Because this is a console application and I don't know how to do that.
 *
 * So how do you type dead keys?  Answer: you don't, not if you're using
 * Windows 95, not until either Microsoft fixes the bug (and they haven't
 * as of Service Pack 1) or someone (me?) produces a true Windows GUI
 * version of Vim.
 *
 * If someone can come up with some workaround, please, please, please
 * send it to me.
 *
 * You can, however, use digraphs.  They're not as convenient as dead
 * keys, especially if you're used to typing dead keys, but they do
 * work and they're better than nothing.
 *
 * If you're using NT, the dead keys work fine.  At least they do with
 * NT 3.51, Service Pack 3, US Edition.  I haven't tested it with any
 * other version of NT.  I did hear one report that NT3.51, SP3, UK edition
 * had problems with CTRL-vowels, but I don't know about dead keys.
 *
 * /George V. Reilly, 3/15/1996
 *
 * On 24th March, I was told by a senior developer at Microsoft:
 *   Win95 console support has always been and will always be flaky.
 *   
 *   The flakiness is unavoidable because we are stuck between the world of
 *   MS-DOS keyboard TSRs like KEYB (which wants to cook the data; important
 *   for international) and the world of Win32.
 *   
 *   So keys that don't "exist" in MS-DOS land (like dead keys) have a
 *   very tenuous existence in Win32 console land.  Keys that act
 *   differently between MS-DOS land and Win32 console land (like capslock)
 *   will act flaky.
 *   
 *   Don't even *mention* the problems with multiple language keyboard
 *   layouts...
 */

static HWND   g_hwndKbd  = NULL;
static HANDLE g_hthrdKbd = NULL;
static DWORD  g_dwThreadId = 0;

#define WM_SENDDEADKEY (WM_USER+31)


void send_key(UINT uVirtKey, UINT uScanCode, UINT fDown);
void key_down(UINT uVirtKey, UINT uScanCode);
void keystate_down(UINT uCtrlState);
void key_up(UINT uVirtKey, UINT uScanCode);
void keystate_up(UINT uCtrlState);

	static LRESULT CALLBACK
keyboard_wndproc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	static int n, vk, sc; 
	char ch, sz[200];

	sprintf(sz, "kbd: msg = %04x, w = %08x, l = %08x\n", uMsg, wParam, lParam);
	OutputDebugString(sz);

	switch (uMsg)
	{
	case WM_SENDDEADKEY:
		{
			UINT uDeadVirt, uDeadScan, uDeadCtrl, uVirt, uScan, uCtrl;
			BYTE abKeystate[256];
			WORD awAnsiCode[2];
			
			uDeadVirt = LOBYTE(wParam);
			uDeadScan = HIBYTE(wParam);
			uDeadCtrl = HIWORD(wParam);

			uVirt = LOBYTE(lParam);
			uScan = HIBYTE(lParam);
			uCtrl = HIWORD(lParam);

			GetKeyboardState(abKeystate);
			
			memset(abKeystate, 0, sizeof (abKeystate));
			if (!SetKeyboardState(abKeystate))
			{
				n = GetLastError();
				sprintf(sz, "setkeyboardstate failed, error %d\n", n);
				OutputDebugString(sz);
			}

			ToAscii(VK_SPACE, MapVirtualKey(VK_SPACE, 0),
					abKeystate, awAnsiCode, 0);
	
			if (uDeadCtrl & SHIFT_PRESSED) 
				abKeystate[VK_SHIFT] = 0x80;
			if (uDeadCtrl & CAPSLOCK_ON) 
				abKeystate[VK_CAPITAL] = 1;
			
			if ((uDeadCtrl & ALT_GR) == ALT_GR)
			{
				abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
					abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
			}
			
			if (!SetKeyboardState(abKeystate))
			{
				n = GetLastError();
				sprintf(sz, "setkeyboardstate failed, error %d\n", n);
				OutputDebugString(sz);
			}

			//keystate_down(uDeadCtrl);
			key_down(uDeadVirt, uDeadScan);
			key_up(uDeadVirt, uDeadScan);
			//keystate_up(uDeadCtrl);

			memset(abKeystate, 0, sizeof (abKeystate));
	
			if (uCtrl & SHIFT_PRESSED) 
				abKeystate[VK_SHIFT] = 0x80;
			if (uCtrl & CAPSLOCK_ON) 
				abKeystate[VK_CAPITAL] = 1;
			
			if ((uCtrl & ALT_GR) == ALT_GR)
			{
				abKeystate[VK_CONTROL] = abKeystate[VK_LCONTROL] =
					abKeystate[VK_MENU] = abKeystate[VK_RMENU] = 0x80;
			}
			
			if (!SetKeyboardState(abKeystate))
			{
				int n = GetLastError();
			}

			//keystate_down(uCtrl);
			key_down(uVirt, uScan);
			Sleep(30);

			key_up(uVirt, uScan);
			//keystate_up(uCtrl);
		}
		break;
		
	case WM_CHAR:
	case WM_DEADCHAR:
		ch = wParam;
		CharToOemBuff(&ch, &ch, 1);
		sprintf(sz, "WM_%sCHAR: wParam = 0x%2x, Oem = 0x%2x, '%c'\n",
				((uMsg == WM_CHAR) ? "" : "DEAD"),
				wParam, LOBYTE(ch), isprint(ch) ? ch : '?');
		OutputDebugString(sz);

		n = DefWindowProc(hwnd, uMsg, wParam, lParam);
		return n;

	case WM_KEYDOWN:
		vk = wParam;
		sc = HIWORD(lParam);

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}



	static DWORD WINAPI
keyboard_threadproc(
	LPVOID lpvThreadParam)
{
	const char szKbdTrans[] = "VimKbdXlator";
	WNDCLASS wndclass;
	MSG msg;
	int i = 0;
	
	wndclass.style = 0;
	wndclass.lpfnWndProc = keyboard_wndproc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = NULL;
	wndclass.hbrBackground = NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szKbdTrans;

	RegisterClass(&wndclass);
	
	g_hwndKbd = CreateWindow(szKbdTrans, "", 0,
							 CW_USEDEFAULT, CW_USEDEFAULT,
							 CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
							 GetModuleHandle(NULL), NULL);
	
    while (GetMessage(&msg, NULL, 0, 0))
	{
#if 0
		char sz[200];
		sprintf(sz, "gm: %2d: hwnd = %x, msg = %x, w = %x, l = %x\n",
				++i, msg.hwnd, msg.message, msg.wParam, msg.lParam);
		OutputDebugString(sz);
#endif

		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

	return msg.wParam;
}



	static BOOL
keyboard_init()
{
	const char sz[] = "aA\xE0\xE1\xE8\xE9\xF1\xD1";
	const char* psz = sz;

	for ( ; *psz; psz++)
	{
		char sz2[100];
		SHORT w = VkKeyScan(*psz);
		
		sprintf(sz2, "%02x: %04x\n", *psz, w);
		OutputDebugString(sz2);
	}

	g_hthrdKbd = (HANDLE) _beginthreadex(NULL, 0, keyboard_threadproc,
										 (LPVOID) NULL, 0, &g_dwThreadId);

	return (g_hthrdKbd != NULL);
}



	static void
keyboard_exit()
{
	TerminateThread(g_hthrdKbd, 0);
}



	void
send_key(
	UINT uVirtKey,
	UINT uScanCode,
	UINT fDown)
{
	PostMessage(g_hwndKbd,
				fDown ? WM_KEYDOWN : WM_KEYUP,
				uVirtKey,
				MAKELONG(1, uScanCode | (fDown ? 0 : 0x8000)));
	Sleep(20);
}



	void
key_down(
	UINT uVirtKey,
	UINT uScanCode)
{
	send_key(uVirtKey, uScanCode, TRUE);
}



	void
keystate_down(
	UINT uCtrlState)
{
	if (uCtrlState & SHIFT_PRESSED)
		key_down(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0));
	if ((uCtrlState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) != 0)
		key_down(VK_CONTROL, MapVirtualKey(VK_CONTROL, 0));
}



	void
key_up(
	UINT uVirtKey,
	UINT uScanCode)
{
	send_key(uVirtKey, uScanCode, FALSE);
}



	void
keystate_up(
	UINT uCtrlState)
{
	if ((uCtrlState & (RIGHT_CTRL_PRESSED | LEFT_CTRL_PRESSED)) != 0)
		key_up(VK_CONTROL, MapVirtualKey(VK_CONTROL, 0));
	if (uCtrlState & SHIFT_PRESSED)
		key_up(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0));
}

#endif /* WIN95_DEAD_KEYS_HACK */



/* When uChar.AsciiChar is 0, then we need to look at wVirtualKeyCode.
 * We map function keys to their ANSI terminal equivalents, as produced
 * by ANSI.SYS, for compatibility with the MS-DOS version of Vim.  Any
 * ANSI key with a value >= '\300' is nonstandard, but provided anyway
 * so that the user can have access to all SHIFT-, CTRL-, and ALT-
 * combinations of function/arrow/etc keys.
 */

const static struct {
	WORD	wVirtKey;
	BOOL	fAnsiKey;
	int		chAlone;
	int		chShift;
	int		chCtrl;
	int		chAlt;
} VirtKeyMap[] = {

/*	  Key		ANSI	alone	shift	ctrl		alt */
	{ VK_ESCAPE,FALSE,	ESC,	ESC,	ESC,		ESC,	},

	{ VK_F1,	TRUE,	';',	'T',	'^',		'h', },
	{ VK_F2,	TRUE,	'<',	'U',	'_',		'i', },
	{ VK_F3,	TRUE,	'=',	'V',	'`',		'j', },
	{ VK_F4,	TRUE,	'>',	'W',	'a',		'k', },
	{ VK_F5,	TRUE,	'?',	'X',	'b',		'l', },
	{ VK_F6,	TRUE,	'@',	'Y',	'c',		'm', },
	{ VK_F7,	TRUE,	'A',	'Z',	'd',		'n', },
	{ VK_F8,	TRUE,	'B',	'[',	'e',		'o', },
	{ VK_F9,	TRUE,	'C',	'\\',	'f',		'p', },
	{ VK_F10,	TRUE,	'D',	']',	'g',		'q', },
	{ VK_F11,	TRUE,	'\205',	'\207',	'\211',		'\213', },
	{ VK_F12,	TRUE,	'\206',	'\210',	'\212',		'\214', },

	{ VK_HOME,	TRUE,	'G',	'\302',	'w',		'\303', },
	{ VK_UP,	TRUE,	'H',	'\304',	'\305',		'\306', },
	{ VK_PRIOR,	TRUE,	'I',	'\307',	'\204',		'\310', }, /*PgUp*/
	{ VK_LEFT,	TRUE,	'K',	'\311',	's',		'\312', },
	{ VK_RIGHT,	TRUE,	'M',	'\313',	't',		'\314', },
	{ VK_END,	TRUE,	'O',	'\315',	'u',		'\316', },
	{ VK_DOWN,	TRUE,	'P',	'\317',	'\320',		'\321', },
	{ VK_NEXT,	TRUE,	'Q',	'\322',	'v',		'\323', }, /*PgDn*/
	{ VK_INSERT,TRUE,	'R',	'\324',	'\325',		'\326', },
	{ VK_DELETE,TRUE,	'S',	'\327',	'\330',		'\331', },

	{ VK_SNAPSHOT,TRUE,	0,		0,		0,			'r', }, /*PrtScrn*/

	/* Most people don't have F13-F20, but what the hell... */
	{ VK_F13,	TRUE,	'\332',	'\333',	'\334',		'\335', },
	{ VK_F14,	TRUE,	'\336',	'\337',	'\340',		'\341', },
	{ VK_F15,	TRUE,	'\342',	'\343',	'\344',		'\345', },
	{ VK_F16,	TRUE,	'\346',	'\347',	'\350',		'\351', },
	{ VK_F17,	TRUE,	'\352',	'\353',	'\354',		'\355', },
	{ VK_F18,	TRUE,	'\356',	'\357',	'\360',		'\361', },
	{ VK_F19,	TRUE,	'\362',	'\363',	'\364',		'\365', },
	{ VK_F20,	TRUE,	'\366',	'\367',	'\370',		'\371', },
};


static BOOL g_fJustGotFocus = FALSE;

/* The return code indicates key code size. */
	static int
win32_kbd_patch_key(
	KEY_EVENT_RECORD* pker)
{
	static int s_iIsDead = 0;
	static WORD awAnsiCode[2];
	UINT uMods = pker->dwControlKeyState;
	BYTE abKeystate[256];

	if (s_iIsDead == 2)
	{
		pker->uChar.AsciiChar = (CHAR) awAnsiCode[1];
		s_iIsDead = 0;
		return 1;
	}

	if (pker->uChar.AsciiChar != 0) 
		return 1;
	
	memset(abKeystate, 0, sizeof (abKeystate));

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
		pker->uChar.AsciiChar = (CHAR) awAnsiCode[0];

	return s_iIsDead;
}


  
/*
 * Decode a KEY_EVENT into one or two keystrokes
 */
	static BOOL
decode_key_event(
	KEY_EVENT_RECORD* pker,
	char_u* pch,
	char_u* pchPending,
	BOOL fDoPost)
{
	int i;
	const int nModifs = pker->dwControlKeyState & (SHIFT | ALT | CTRL);

#ifdef WIN95_DEAD_KEYS_HACK
	static UINT uDeadVirtKey = 0, uDeadScan = 0, uDeadCtrlState = 0;
	char sz[200];

	sprintf(sz, "Vk = 0x%02x, `%c'; Scan = 0x%02x; "
			"Char = %3d, 0x%02x, `%c'; Ctrl = 0x%02x, Post = %d\n",
			pker->wVirtualKeyCode,
			isprint(pker->wVirtualKeyCode) ? pker->wVirtualKeyCode :'?',
			pker->wVirtualScanCode,
			LOBYTE(pker->uChar.AsciiChar), LOBYTE(pker->uChar.AsciiChar),
			isprint(pker->uChar.AsciiChar) ? pker->uChar.AsciiChar :'?',
			pker->dwControlKeyState, fDoPost);
	OutputDebugString(sz);
#endif /* WIN95_DEAD_KEYS_HACK */

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
	case VK_MENU:	/* Alt key */
		return FALSE;

	default:
		break;
	}

#ifdef WIN95_DEAD_KEYS_HACK
	if (fDoPost  &&  uDeadVirtKey != 0)
	{
		sprintf(sz, "sending vk = 0x%2x, sc = 0x%2x\n", 
				pker->wVirtualKeyCode, pker->wVirtualScanCode);
		OutputDebugString(sz);

		PostMessage(g_hwndKbd, WM_SENDDEADKEY,
					MAKELONG(MAKEWORD(uDeadVirtKey, uDeadScan), uDeadCtrlState),
					MAKELONG(MAKEWORD(pker->wVirtualKeyCode,
									  pker->wVirtualScanCode),
							 pker->dwControlKeyState));
	}
#endif /* WIN95_DEAD_KEYS_HACK */

#if 0
	/* If AltGr has been pressed, remove it.  */
	if ((pker->dwControlKeyState & ALT_GR) == ALT_GR)
		pker->dwControlKeyState &= ~ ALT_GR;
#endif

#if 0
	/* If CapsLock is on on Win95, digits will be shifted by default;
	 * e.g., pressing '1' will give '!'.  Feh. */
	if ((pker->dwControlKeyState & CAPSLOCK_ON) &&
		!(pker->dwControlKeyState & SHIFT_PRESSED) &&
		('0' <= pker->wVirtualKeyCode  &&  pker->wVirtualKeyCode <= '9'))
	{
		pker->uChar.AsciiChar = NUL;
	}
#endif

#if 0
	if ((*pch = pker->uChar.AsciiChar) != NUL)
		return TRUE;
#endif

	/* special cases */
	if ((nModifs & CTRL) != 0  &&  (nModifs & ~CTRL) == 0
		&&  pker->uChar.AsciiChar == NUL)
	{
		/* Ctrl-6 is Ctrl-^ */
		if (pker->wVirtualKeyCode == '6')
		{
			*pch = Ctrl('^');
			return TRUE;
		}
		/* Ctrl-2 is Ctrl-@ */
		else if (pker->wVirtualKeyCode == '2')
		{
			*pch = NUL;
			return TRUE;
		}
	}
	
	/* Shift-TAB */
	if (pker->wVirtualKeyCode == VK_TAB  &&  (nModifs & SHIFT_PRESSED))
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
			else if ((nModifs & CTRL) != 0  &&  (nModifs & ~CTRL) == 0)
				*pch = VirtKeyMap[i].chCtrl;
			else if ((nModifs & ALT) != 0  &&  (nModifs & ~ALT) == 0)
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
	{
		*pch = NUL;

#ifdef WIN95_DEAD_KEYS_HACK
		if (fDoPost)
		{
			uDeadVirtKey = pker->wVirtualKeyCode;
			uDeadScan = pker->wVirtualScanCode;
			uDeadCtrlState = pker->dwControlKeyState;
		}
#endif /* WIN95_DEAD_KEYS_HACK */
	}
	else
	{
#ifdef WIN95_DEAD_KEYS_HACK
		uDeadVirtKey = uDeadScan = uDeadCtrlState = 0;
#endif /* WIN95_DEAD_KEYS_HACK */
		*pch = (i > 0)  ?  pker->uChar.AsciiChar  :  NUL;
	}

	return (*pch != NUL);
}


#ifdef USE_MOUSE

static int g_fMouseAvail = FALSE;	/* mouse present */
static int g_fMouseActive = FALSE;	/* mouse enabled */
static int g_nMouseClick = -1;		/* mouse status */
static int g_xMouse;				/* mouse x coordinate */
static int g_yMouse;				/* mouse y coordinate */

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

	if (! g_fMouseAvail  ||  ! g_fMouseActive)
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
		/* ignore MOUSE_MOVED events if (x, y) hasn't changed.  (We get these
		 * events even when the mouse moves only within a char cell.) */
		if (s_xOldMouse == g_xMouse  &&  s_yOldMouse == g_yMouse)
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
	else	/* one or more buttons pressed */
	{
		const int cButtons = GetSystemMetrics(SM_CMOUSEBUTTONS);

		/* on a 2-button mouse, hold down left and right buttons
		 * simultaneously to get MIDDLE. */

		if (cButtons == 2  &&  s_nOldButton != MOUSE_DRAG)
		{
			DWORD dwLR = (pmer->dwButtonState & LEFT_RIGHT);
			
			/* if either left or right button only is pressed, see if the
			 * the next mouse event has both of them pressed */
			if (dwLR == LEFT  ||  dwLR  == RIGHT)
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
					
						if (cRecords == 0  ||  ir.EventType != MOUSE_EVENT
							||  !(pmer2->dwButtonState & LEFT_RIGHT))
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
		else if (cButtons == 2  &&
			((pmer->dwButtonState & LEFT_RIGHT) == LEFT_RIGHT))
		{
			nButton = MOUSE_MIDDLE;

			if (! s_fReleased  &&  pmer->dwEventFlags != MOUSE_MOVED)
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

		if (! s_fReleased  && ! s_fNextIsMiddle
			&&  nButton != s_nOldButton  &&  s_nOldButton != MOUSE_DRAG)
			return FALSE;
		
		s_fReleased = s_fNextIsMiddle;
	}

	if (pmer->dwEventFlags == 0  ||  pmer->dwEventFlags == DOUBLE_CLICK)
	{
		/* button pressed or released, without mouse moving */
		if (nButton != -1  &&  nButton != MOUSE_RELEASE)
		{
			DWORD dwCurrentTime = GetTickCount();

			if (s_xOldMouse != g_xMouse  ||  s_yOldMouse != g_yMouse
				||  s_nOldButton != nButton
				||  s_old_topline != curwin->w_topline
				||  (int) (dwCurrentTime - s_dwLastClickTime) > p_mouset)
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
		if (nButton != -1  &&  nButton != MOUSE_RELEASE)
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

	/* only pass on interesting (i.e., different) mouse events */
	if (s_xOldMouse == g_xMouse  &&  s_yOldMouse == g_yMouse
		&&  s_nOldMouseClick == g_nMouseClick)
	{
		g_nMouseClick = -1;
		return FALSE;
	}

	g_nMouseClick |= 0x20;

	s_xOldMouse = g_xMouse;
	s_yOldMouse = g_yMouse;
	s_old_topline = curwin->w_topline;
	s_nOldMouseClick = g_nMouseClick;
	
	if (nButton != MOUSE_DRAG  &&  nButton != MOUSE_RELEASE)
		SET_NUM_MOUSE_CLICKS(g_nMouseClick, s_cClicks);
	
	return TRUE;
}


#endif /* USE_MOUSE */


/*
 * Wait until console input from keyboard or mouse is available,
 * or the time is up
 */
	static int
WaitForChar(
	long msec)
{
	DWORD dwNow, dwEndTime;

	if (msec <= 0)
		msec = 2;	/* very short time */

	dwEndTime = GetTickCount() + msec;

	/* We need to loop until the end of the time period, because
	 * we might get multiple unusable mouse events in that time.
	 */
	while ((dwNow = GetTickCount())  <  dwEndTime)
	{
		if (WaitForSingleObject(g_hConIn, dwEndTime - dwNow) == WAIT_OBJECT_0)
		{
			INPUT_RECORD ir;
			DWORD cRecords = 0;
			
			PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);
			
			if (cRecords > 0)
			{
				if (ir.EventType == KEY_EVENT  &&  ir.Event.KeyEvent.bKeyDown)
				{
					char_u ch, ch2;

					return decode_key_event(&ir.Event.KeyEvent, &ch, &ch2,
											FALSE);
				}
				else if (ir.EventType == FOCUS_EVENT)
				{
					ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
					
					g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
				}
				else if (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
				{
					ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
					
					set_winsize(Rows, Columns, FALSE);
				}
#ifdef USE_MOUSE
				else if (ir.EventType == MOUSE_EVENT) 
				{
					ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
					
					if (decode_mouse_event(&ir.Event.MouseEvent))
						return TRUE;
				}
#endif /* USE_MOUSE */
				else
				{
					/* Discard it, it's an insignificant event */
					ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
				}
			}
		}
	}
		
	return FALSE;
}


static char_u g_chPending = NUL;


/*
 * Is there a pending keystroke or mouse event?
 */
	static BOOL
kbhit()
{
    if (g_chPending != NUL
#ifdef USE_MOUSE
		|| g_nMouseClick != -1
#endif /* USE_MOUSE */
		)
        return TRUE;

	for ( ; ; )
	{
		INPUT_RECORD ir;
        DWORD cRecords = 0;

        PeekConsoleInput(g_hConIn, &ir, 1, &cRecords);

        if (cRecords == 0)
			return FALSE;
		else
		{
			if (ir.EventType == KEY_EVENT  &&  ir.Event.KeyEvent.bKeyDown)
			{
				char_u ch, ch2;

                return decode_key_event(&ir.Event.KeyEvent, &ch, &ch2, FALSE);
			}
			else if (ir.EventType == FOCUS_EVENT)
			{
				ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
				
				g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
			}
			else if (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
			{
				ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
				
				set_winsize(Rows, Columns, FALSE);
			}
#ifdef USE_MOUSE
            else if (ir.EventType == MOUSE_EVENT) 
			{
				ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);

				if (decode_mouse_event(&ir.Event.MouseEvent))
					return TRUE;
			}
#endif /* USE_MOUSE */
            else
			{
				/* Discard it, it's an insignificant event */
                ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
			}
        }
    }
}


/*
 * Get a keystroke or a mouse event
 */
	static char_u
tgetch()
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
		
		ReadConsoleInput(g_hConIn, &ir, 1, &cRecords);
		
		if (ir.EventType == KEY_EVENT)
		{
			if (decode_key_event(&ir.Event.KeyEvent, &ch, &g_chPending, TRUE))
				return ch;
        }
		else if (ir.EventType == FOCUS_EVENT)
		{
			g_fJustGotFocus = ir.Event.FocusEvent.bSetFocus;
		}
		else if (ir.EventType == WINDOW_BUFFER_SIZE_EVENT)
		{
			set_winsize(Rows, Columns, FALSE);
		} 
#ifdef USE_MOUSE
		else if (ir.EventType == MOUSE_EVENT) 
		{
			if (decode_mouse_event(&ir.Event.MouseEvent))
				return 0;
		}
#endif /* USE_MOUSE */
    }
}


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
	char_u *buf,
	int maxlen,
	long time)
{
    int len = 0;
    int c;

    if (time >= 0)
	{
        if (! WaitForChar(time))     /* no character available */
            return 0;
    }
	else	/* time == -1, wait forever */
	{
	
        /* If there is no character available within 2 seconds (default),
         * write the autoscript file to disk */
        if (WaitForChar(p_ut) == 0)
            updatescript(0);
    }

	/*
	 * Try to read as many characters as there are.
	 */

    --maxlen;                   /* may get two chars at once */

    /* we will get at least one key. Get more if they are available. */
    g_fCBrkPressed = FALSE;

#ifdef MCH_WRITE_DUMP
	if (fdDump)
		fputc('[', fdDump);
#endif /* MCH_WRITE_DUMP */

	while ((len == 0 || kbhit())  &&  len < maxlen)
	{
#ifdef USE_MOUSE
		if (g_nMouseClick != -1  &&  maxlen >= 5-1)
		{
# ifdef MCH_WRITE_DUMP
			if (fdDump)
				fprintf(fdDump, "{%02x @ %d, %d}",
						g_nMouseClick, g_xMouse, g_yMouse);
# endif /* MCH_WRITE_DUMP */

			len = 5;
			*buf++ = ESC + 128;
			*buf++ = 'M';
			*buf++ = g_nMouseClick;
			*buf++ = g_xMouse + '!';
			*buf++ = g_yMouse + '!';
			g_nMouseClick = -1;
			
		}
		else
#endif /* USE_MOUSE */
		{
			if ((c = tgetch()) == Ctrl('C'))
				g_fCBrkPressed = TRUE;
			
#ifdef USE_MOUSE
			if (g_nMouseClick == -1)
#endif /* USE_MOUSE */
			{
				*buf++ = c;
				len++;
				
#ifdef MCH_WRITE_DUMP
				if (fdDump)
					fputc(c, fdDump);
#endif /* MCH_WRITE_DUMP */
				
			}
		}
	}

#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
		fputs("]\n", fdDump);
		fflush(fdDump);
	}
#endif /* MCH_WRITE_DUMP */

	beep_count = 0;			/* may beep again now that we got some chars */
    return len;
}


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
 * platform-specific initialization
 */
	void
mch_windinit()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	extern int _fmode;

	PlatformId();

    _fmode = O_BINARY;          /* we do our own CR-LF translation */
    flushbuf();

    /* Obtain handles for the standard Console I/O devices */
    g_hConIn =  GetStdHandle(STD_INPUT_HANDLE);
    g_hSavOut = GetStdHandle(STD_OUTPUT_HANDLE);
	g_hCurOut = g_hSavOut;

    /* Get current text attributes */
    GetConsoleScreenBufferInfo(g_hSavOut, &csbi);
    g_attrCurrent = g_attrDefault = csbi.wAttributes;
    GetConsoleCursorInfo(g_hSavOut, &g_cci);

	GetConsoleMode(g_hConIn,  &g_cmodein);
	GetConsoleMode(g_hSavOut, &g_cmodeout);

    GetConsoleTitle(g_szOrigTitle, sizeof(g_szOrigTitle));
    mch_get_winsize();

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
#endif /* MCH_WRITE_DUMP */

	g_fWindInitCalled = TRUE;

#ifdef USE_MOUSE
	g_fMouseAvail = GetSystemMetrics(SM_MOUSEPRESENT);
#endif

#ifdef WIN95_DEAD_KEYS_HACK
	keyboard_init();
#endif

	/*
	 * We don't really want to jump to our own screen yet; do that after
	 * starttermcap().  This flashes the window, sorry about that, but
	 * otherwise "vim -r" doesn't work.
	 */
    g_hCurOut = g_hSavOut;
	SetConsoleActiveScreenBuffer(g_hCurOut);
}


/*
 * Shut down and exit with status `r'
 * Careful: mch_windexit() may be called before mch_windinit()!
 */
	void
mch_windexit(
	int r)
{
    stoptermcap();
    flushbuf();

	if (g_fWindInitCalled)
		settmode(0);

    if (g_hConOut != INVALID_HANDLE_VALUE)
    {
        (void) CloseHandle(g_hConOut);

		if (g_hSavOut != INVALID_HANDLE_VALUE)
		{
			SetConsoleTextAttribute(g_hSavOut, g_attrDefault);
			SetConsoleCursorInfo(g_hSavOut, &g_cci);
		}
    }

	ml_close_all(TRUE); 			/* remove all memfiles */

	if (g_fWindInitCalled)
	{
#ifdef WIN95_DEAD_KEYS_HACK
		keyboard_exit();
#endif

		mch_restore_title(3);

#ifdef MCH_WRITE_DUMP
		if (fdDump)
		{
			time_t t;
			
			time(&t);
			fputs(ctime(&t), fdDump);
			fclose(fdDump);
		}
		fdDump = NULL;
#endif /* MCH_WRITE_DUMP */
	}

    exit(r);
}


/*
 * Do we have an interactive window?
 */
	int
mch_check_win(
	int argc,
	char **argv)
{
    if (isatty(1))
		return OK;
	return FAIL;
}


/*
 * Is input interactive?  (From a keyboard)
 */
	int
mch_check_input()
{
    if (isatty(0))
		return OK;
	return FAIL;
}


/*
 * Turn a filename into its canonical form.  Replace slashes with backslashes.
 * This used to replace backslashes with slashes, but that caused problems
 * when using the file name as a command.  We can't have a mix of slashes and
 * backslashes, because comparing file names will not work correctly.  The
 * commands that use file names should be prepared to handle the backslashes.
 */
	static void
canonicalize_filename(
	char* pszName)
{
	if (pszName == NULL)
		return;
	
	for ( ; *pszName;  pszName++)
	{
		if (*pszName == '/')
			*pszName = '\\';
#ifdef DOWNCASE_FILENAMES
		else
			*pszName = tolower(*pszName);
#endif /* DOWNCASE_FILENAMES */
	}
}


/*
 * fname_case(): Set the case of the filename, if it already exists.
 */
	void
fname_case(
	char_u *name)
{
#ifdef DOWNCASE_FILENAMES
	canonicalize_filename(name);
#else /* !DOWNCASE_FILENAMES */
	char szTrueName[_MAX_PATH + 1];
	char *psz, *pszPrev;
	const int len = (name != NULL)  ?  STRLEN(name)  :  0;
	
	if (len == 0)
		return;
	
	STRCPY(szTrueName, name);
	STRCAT(szTrueName, "\\");	/* sentinel */

	for (psz = szTrueName;  *psz != NUL;  psz++)
		if (*psz == '/')
			*psz = '\\';

	psz = pszPrev = szTrueName;

	/* Skip leading \\ in UNC name or drive letter */
	if (len > 2   &&  ((psz[0] == '\\'  &&  psz[1] == '\\')
					   || (isalpha(psz[0])  &&  psz[1] == ':')))
	{
		psz = pszPrev = szTrueName + 2;
	}
	
	while (*psz != NUL)
	{
		WIN32_FIND_DATA fb;
		HANDLE          hFind;

		while (*psz != '\\')
			psz++;
		*psz = NUL;
		
		if ((hFind = FindFirstFile(szTrueName, &fb)) != INVALID_HANDLE_VALUE)
		{
			/* avoid ".." and ".", etc */
			if ((size_t) (psz - pszPrev) == STRLEN(fb.cFileName))
				STRCPY(pszPrev, fb.cFileName);
			FindClose(hFind);
		}
		
		*psz++ = '\\';
		pszPrev = psz;
	}

	*--psz = NUL;	/* remove sentinel */

	STRCPY(name, szTrueName);
#endif /* !DOWNCASE_FILENAMES */
}


/*
 * mch_settitle(): set titlebar of our window
 * Can the icon also be set?
 */
	void
mch_settitle(
	char_u *title,
	char_u *icon)
{
    if (title != NULL)
        SetConsoleTitle(title);
}


/*
 * Restore the window/icon title.
 * which is one of:
 *	1: Just restore title
 *  2: Just restore icon (which we don't have)
 *	3: Restore title and icon (which we don't have)
 */
	void
mch_restore_title(
	int which)
{
	mch_settitle((which & 1) ? g_szOrigTitle : NULL, NULL);
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
	char_u *s,
	int len)
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
	return (long) GetCurrentProcessId();
}


/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return non-zero for success.
 */
	int
mch_dirname(
	char_u *buf,
	int len)
{
    return (getcwd(buf, len) != NULL);
}


/*
 * Get absolute filename into buffer 'buf' of length 'len' bytes,
 * turning all '/'s into '\\'s and getting the correct case of each
 * component of the filename.  Return OK or FAIL.
 */
	int
FullName(
	char_u *fname,
	char_u *buf,
	int len,
	int force)
{
	int nResult = FAIL;
	
    if (fname == NULL)          /* always fail */
        return FAIL;

    if (_fullpath(buf, fname, len) == NULL)
	{
        /* failed, use the relative path name */
        STRNCPY(buf, fname, len);
    }
	else
		nResult = OK;

	fname_case(buf);

	return nResult;
}


/*
 * return TRUE if `fname' is an absolute path name
 */
	int
isFullName(
	char_u *fname)
{
	char szName[_MAX_PATH];

	FullName(fname, szName, _MAX_PATH, FALSE);

#ifdef DOWNCASE_FILENAMES
	return stricmp(fname, szName) == 0;
#else /* !DOWNCASE_FILENAMES */
	return strcmp(fname, szName) == 0;
#endif /* !DOWNCASE_FILENAMES */
}


/*
 * get file permissions for `name'
 * -1 : error
 * else FILE_ATTRIBUTE_* defined in winnt.h
 */
	long
getperm(
	char_u *name)
{
    return GetFileAttributes(name);
}


/*
 * set file permission for `name' to `perm'
 */
	int
setperm(
	char_u *name,
	long perm)
{
	perm |= FILE_ATTRIBUTE_ARCHIVE;		/* file has changed, set archive bit */
	return SetFileAttributes(name, perm) ? OK : FAIL;
}


/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory or upon error
 */
	int
mch_isdir(
	char_u *name)
{
	int f = getperm(name);

	if (f == -1)
		return FALSE;              /* file does not exist at all */

	return (f & FILE_ATTRIBUTE_DIRECTORY) != 0;
}


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
		g_fCBrkPressed  = TRUE;
		return TRUE;

	/* fatal events: shut down gracefully */
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		windgoto((int)Rows - 1, 0);
		sprintf((char *)IObuff, "Vim: Caught %s event\n",
				(dwCtrlType == CTRL_CLOSE_EVENT ? "close"
				 : dwCtrlType == CTRL_LOGOFF_EVENT ? "logoff" : "shutdown"));

#ifdef DEBUG
		OutputDebugString(IObuff);
#endif /* DEBUG */

		preserve_exit();		/* output IObuff, preserve files and exit */

		return TRUE;			/* not reached */

	default:
		return FALSE;
	}
}


/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 */
	void
mch_settmode(
	int raw)
{
    DWORD cmodein;
    DWORD cmodeout;

    GetConsoleMode(g_hConIn,  &cmodein);
	GetConsoleMode(g_hCurOut, &cmodeout);

    if (raw)
	{
        cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
                     ENABLE_ECHO_INPUT);
        cmodein |= (
#ifdef USE_MOUSE
			(g_fMouseActive ? ENABLE_MOUSE_INPUT : 0) |
#endif /* USE_MOUSE */
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
				raw ? "raw" : "cooked",
				(g_hCurOut == g_hSavOut ? "Sav" : "Con"),
				cmodein, cmodeout);
		fflush(fdDump);
	}
#endif /* MCH_WRITE_DUMP */
}


/*
 * Get the size of the current window in `Rows' and `Columns'
 */
	int
mch_get_winsize()
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

    if (Columns < MIN_COLUMNS  ||  Rows < MIN_ROWS + 1)
	{
        /* these values are overwritten by termcap size or default */
        Rows = 25;
        Columns = 80;
    }

    check_winsize();
    set_scroll_region(0, 0, Columns - 1, Rows - 1);

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
    CONSOLE_SCREEN_BUFFER_INFO csbi;    /* hold current console buffer info */
    SMALL_RECT      srWindowRect;       /* hold the new console size */
    COORD           coordScreen;
	DWORD			dwErr = 0;

#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
		fprintf(fdDump, "ResizeConBufAndWindow(%d, %d)\n", xSize, ySize);
		fflush(fdDump);
	}
#endif /* MCH_WRITE_DUMP */

    GetConsoleScreenBufferInfo(hConsole, &csbi);

    /* get the largest size we can size the console window to */
    coordScreen = GetLargestConsoleWindowSize(hConsole);

    /* define the new console window size and scroll position */
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
    srWindowRect.Right =  (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);

    /* define the new console buffer size */
    coordScreen.X = xSize;
    coordScreen.Y = ySize;

	if (!SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect))
	{
#ifdef MCH_WRITE_DUMP
		if (fdDump)
		{
			fprintf(fdDump, "SetConsoleWindowInfo failed: %lx\n",
					GetLastError());
			fflush(fdDump);
		}
#endif /* MCH_WRITE_DUMP */
	}

	if (!SetConsoleScreenBufferSize(hConsole, coordScreen))
	{
#ifdef MCH_WRITE_DUMP
		if (fdDump)
		{
			fprintf(fdDump, "SetConsoleScreenBufferSize failed: %lx\n",
					GetLastError());
			fflush(fdDump);
		}
#endif /* MCH_WRITE_DUMP */
	}

}


/*
 * Set the console window to `Rows' * `Columns'
 */
	void
mch_set_winsize()
{
	COORD coordScreen = GetLargestConsoleWindowSize(g_hCurOut);

	/* Clamp Rows and Columns to reasonable values */
	if (Rows > coordScreen.Y)
		Rows = coordScreen.Y;
	if (Columns > coordScreen.X)
		Columns = coordScreen.X;

	ResizeConBufAndWindow(g_hCurOut, Columns, Rows);
	set_scroll_region(0, 0, Columns - 1, Rows - 1);
}


/*
 * We have no job control, so fake it by starting a new shell.
 */
	void
mch_suspend()
{
    MSG_OUTSTR("new shell started\n");
    call_shell(NULL, SHELL_COOKED);
	need_check_timestamps = TRUE;
}


/*
 * Either execute a command by calling the shell or start a new shell
 */
	int
call_shell(
    char_u *cmd,
    int options)			/* SHELL_FILTER if called by do_filter() */
							/* SHELL_COOKED if term needs cooked mode */
							/* SHELL_EXPAND if called by ExpandWildCards() */
{
    int		x;
	int		stopped_termcap_mode = FALSE;

    flushbuf();

	/*
	 * ALWAYS switch to non-termcap mode, otherwise ":r !ls" may crash.
	 */
	if (g_hCurOut == g_hConOut)
	{
		termcap_mode_end();
		stopped_termcap_mode = TRUE;
	}

#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
		fprintf(fdDump, "call_shell(\"%s\", %d)\n", cmd, options);
		fflush(fdDump);
	}
#endif /* MCH_WRITE_DUMP */

	signal(SIGINT, SIG_IGN);	/* we don't want to be killed here by Ctrl-C*/
	signal(SIGBREAK, SIG_IGN);	/* Nor by Ctrl-Break */

    if (options & SHELL_COOKED)
        settmode(0);            /* set to cooked mode */

    if (cmd == NULL)
	{
        x = system(p_sh);
	}
    else
	{
		/* we use "command" or "cmd" to start the shell; slow but easy */
		char newcmd[CMDBUFFSIZE + 100];

        sprintf(newcmd, "%s /c %s", p_sh, cmd);
        x = system(newcmd);
    }

	settmode(1);            	/* set to raw mode */

	if (x && !expand_interactively)
	{
        smsg("%d returned", x);
        msg_outchar('\n');
    }
    resettitle();

	signal(SIGINT, SIG_DFL);
	signal(SIGBREAK, SIG_DFL);

	if (stopped_termcap_mode)
		termcap_mode_start();

	return (x ? FAIL : OK);
}


#define FL_CHUNK 32

typedef struct filelist {
    char_u** file;
    int      nfiles;
    int      maxfiles;
} FileList;


/*
 * Add filename `f' to the list of files in `fl'
 */
	static void
addfile(
	FileList *fl,
	char *f,
	int isdir)
{
    char           *p, *pp;

    if (!fl->file)
	{
        fl->file = (char **) alloc(sizeof(char *) * FL_CHUNK);
        if (!fl->file)
            return;
        fl->nfiles = 0;
        fl->maxfiles = FL_CHUNK;
    }
    if (fl->nfiles >= fl->maxfiles)
	{
        char          **t;
        int             i;

        t = (char **) lalloc(sizeof(char *) * (fl->maxfiles + FL_CHUNK), TRUE);
        if (!t)
            return;
        for (i = fl->nfiles - 1; i >= 0; i--)
            t[i] = fl->file[i];
        vim_free(fl->file);
        fl->file = t;
        fl->maxfiles += FL_CHUNK;
    }
    p = alloc((unsigned) (strlen(f) + 1 + isdir));
    if (p)
	{
		/* replace slashes with backslashes while copying */
		for (pp = p; *f; ++f, ++pp)
		{
			if (*f == '/')
				*pp = '\\';
			else
				*pp = *f;
		}
        if (isdir)
			*pp++ = '\\';
		*pp = NUL;
    }
    fl->file[fl->nfiles++] = p;
}


/*
 * Does `s' contain a wildcard?
 */
	int
mch_has_wildcard(
	char_u *s)
{
    for ( ;  *s;  ++s)
        if (*s == '?' || *s == '*')
            return 1;
    return 0;
}


/*
 * Copy a string, forcing it to lowercase if DOWNCASE_FILENAMES is defined
 */
#ifdef DOWNCASE_FILENAMES
	static void
strlowcpy(
	char *d,
	char *s)
{
    while (*s)
        *d++ = tolower(*s++);
    *d = NUL;
}
#else /* !DOWNCASE_FILENAMES */
# define strlowcpy(d, s) STRCPY(d, s)
#endif /* !DOWNCASE_FILENAMES */


/*
 * comparison function for qsort in expandpath
 */
	static int
pstrcmp(
	const void *a,
	const void *b)
{
#ifdef DOWNCASE_FILENAMES
    return (stricmp(* (const char **) a, * (const char **) b));
#else /* !DOWNCASE_FILENAMES */
    return (strcmp(* (const char **) a, * (const char **) b));
#endif /* !DOWNCASE_FILENAMES */
}


/*
 * recursively build up a list of files in `fl' matching the first wildcard
 * in `path'.  `fonly' and `donly' are not used (files only and directories
 * only flags?).  If `notf' is set, we add `path' to `fl' even when no such
 * file exists.
 */
	static int
expandpath(
	FileList *fl,
	char *path,
	int fonly,
	int donly,
	int notf)
{
    char            buf[_MAX_PATH];
    char           *p,
                   *s,
                   *e;
    int             lastn,
                    c = 1,
                    r;
    WIN32_FIND_DATA fb;
    HANDLE          hFind;
	int				found_one = FALSE;

    lastn = fl->nfiles;

	/*
	 * Find the first part in the path name that contains a wildcard.
	 * Copy it into `buf', including the preceding characters.
	 */
    p = buf;
    s = NULL;
    e = NULL;
    while (*path)
	{
        if (*path == '\\' || *path == ':' || *path == '/')
		{
            if (e)
                break;
            else
                s = p;
        }
        if (*path == '*' || *path == '?')
            e = p;
        *p++ = *path++;
    }
    e = p;
    if (s)
        s++;
    else
        s = buf;

    /* now we have one wildcard component between `s' and `e' */
    *e = NUL;
    r = 0;

    /* If we are expanding wildcards, we try both files and directories */
    if ((hFind = FindFirstFile(buf, &fb)) != INVALID_HANDLE_VALUE)
		while (c)
		{
			strlowcpy(s, fb.cFileName);

			/*
			 * Ignore "." and "..".
			 * When more follows, this must be a directory.
			 */
			if ((s[0] != '.'  ||
					(s[1] != NUL  &&  (s[1] != '.' || s[2] != NUL))) &&
					(*path == NUL || mch_isdir(buf)))
			{
				strcat(buf, path);
				if (!mch_has_wildcard(path))
					addfile(fl, buf, mch_isdir(buf));
				else
					r |= expandpath(fl, buf, fonly, donly, notf);
				found_one = TRUE;
			}

			c = FindNextFile(hFind, &fb);
		}

	if (!found_one)
	{
        /* not found */
        STRCPY(e, path);

        if (notf)
            addfile(fl, buf, FALSE);

        return 1;               /* unexpanded or empty */
    }

    qsort(fl->file + lastn, fl->nfiles - lastn, sizeof(char *), pstrcmp);
    FindClose(hFind);

    return r;
}


/*
 * MSDOS rebuild of Scott Ballantyne's ExpandWildCards for amiga/arp. -- jw
 *
 * return OK for success, FAIL for error (you may lose some memory) and
 *       put an error message in *file.
 *
 * `num_pat' is number of input patterns
 * `pat' is array of pointers to input patterns
 * `num_file' is pointer to number of matched file names
 * `file' is pointer to array of pointers to matched file names
 * On Unix/Win32, we do not check for `files_only' yet
 * `list_notfound' is ignored
 */
	int
ExpandWildCards(
	int num_pat,
	char_u **pat,
	int *num_file,
	char_u ***file,
    int files_only,
	int list_notfound)
{
    int             i,
                    r = 0;
    FileList        f;

    f.file = NULL;
    f.nfiles = 0;

    for (i = 0; i < num_pat; i++)
	{
        if (!mch_has_wildcard(pat[i]))
            addfile(&f, pat[i], files_only ? FALSE : mch_isdir(pat[i]));
        else
            r |= expandpath(&f, pat[i], files_only, 0, list_notfound);
    }

	*num_file = f.nfiles;
	*file = (*num_file > 0) ? f.file : (char_u **)"";

	return (*num_file > 0) ? OK : FAIL;
}


#ifdef vim_chdir
# undef vim_chdir
#endif

/*
 * The normal _chdir() does not change the default drive.  This one does.
 * Returning 0 implies success; -1 implies failure.
 */
	int
vim_chdir(
	char *path)
{
    if (path[0] == NUL)         /* just checking... */
        return -1;

    if (isalpha(path[0])  &&  path[1] == ':')	/* has a drive name */
	{
        if (_chdrive(toupper(path[0]) - 'A' + 1) != 0)
            return -1;          /* invalid drive name */
        path += 2;
    }

    if (*path == NUL)           /* drive name only */
        return 0;

    return chdir(path);        /* let the normal chdir() do the rest */
}


/*
 * Copy the contents of screen buffer hSrc to the bottom-left corner
 * of screen buffer hDst.
 */
	static void
copy_screen_buffer_text(
	HANDLE hSrc,
	HANDLE hDst)
{
	int     i, j, nSrcWidth, nSrcHeight, nDstWidth, nDstHeight;
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
#endif /* MCH_WRITE_DUMP */

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
static SMALL_RECT g_srOrigWindowRect;
static COORD      g_coordOrig;
static WORD       g_attrSave = 0;


/*
 * Start termcap mode by switching to our allocated screen buffer
 */
	static void
termcap_mode_start()
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
    }

	coord.X = coord.Y = 0;
    FillConsoleOutputCharacter(g_hConOut, ' ', Rows * Columns, coord, &dwDummy);
    FillConsoleOutputAttribute(g_hConOut, wAttr, Rows*Columns, coord, &dwDummy);

	copy_screen_buffer_text(g_hSavOut, g_hConOut);
	
	g_hCurOut = g_hConOut;
	SetConsoleActiveScreenBuffer(g_hCurOut);

	ResizeConBufAndWindow(g_hCurOut, Columns, Rows);
	set_scroll_region(0, 0, Columns - 1, Rows - 1);
	check_winsize();

	resettitle();

	textattr(wAttr);
}


/*
 * Turn off termcap mode by restoring the screen buffer we had upon startup
 */
	static void
termcap_mode_end()
{
	g_attrSave = g_attrCurrent;

	ResizeConBufAndWindow(g_hCurOut, g_nOldColumns, g_nOldRows);
	check_winsize();

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
	
	mch_restore_title(3);

	SetConsoleMode(g_hConIn,  g_cmodein);
	SetConsoleMode(g_hSavOut, g_cmodeout);
}

/*
 * Switching off termcap mode is only allowed when Columns is 80, otherwise a
 * crash may result.  It's always allowed on NT.
 */
	int
can_end_termcap_mode(
	int give_msg)
{
	if (g_PlatformId == VER_PLATFORM_WIN32_NT  ||  Columns == 80)
		return TRUE;
	if (give_msg)
		msg("'columns' is not 80, cannot execute external commands");
	return FALSE;
}

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
clear_screen()
{
    g_coord.X = g_coord.Y = 0;
	clear_chars(g_coord, Rows * Columns);
}


/*
 * Clear to end of display
 */
	static void
clear_to_end_of_display()
{
	clear_chars(g_coord, (Rows-g_coord.Y-1) * Columns + (Columns-g_coord.X));
}


/*
 * Clear to end of line
 */
	static void
clear_to_end_of_line()
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
	if (left >= right  ||  top >= bottom
		||  right > (unsigned) Columns - 1
		||  bottom > (unsigned) Rows - 1)
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
    SMALL_RECT      source;
    COORD           dest;
    CHAR_INFO       fill;

    dest.X = 0;
    dest.Y = g_coord.Y + cLines;

    source.Left   = 0;
    source.Top    = g_coord.Y;
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
    SMALL_RECT      source;
    COORD           dest;
    CHAR_INFO       fill;
	int				nb;

    dest.X = 0;
    dest.Y = g_coord.Y;

    source.Left   = 0;
    source.Top    = g_coord.Y + cLines;
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
	
	if (x < 1  ||  x > (unsigned) Columns  ||  y < 1  ||  y > (unsigned) Rows)
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
 *
 * COLOR		FOREGROUND	BACKGROUND
 *  black		    0		    0
 *  blue		    1		   16
 *  green		    2		   32
 *  cyan		    3		   48
 *  red			    4		   64
 *  magenta		    5		   80
 *  brown		    6		   96
 *  lightgray	    7		  112
 *  darkgray	    8		  128
 *  lightblue	    9		  144
 *  lightgreen	   10		  160
 *  lightcyan	   11		  176
 *  lightred	   12		  192
 *  lightmagenta   13		  208
 *  yellow		   14		  224
 *  white		   15		  240
 */
	static void
textattr(
	WORD wAttr)
{
    g_attrCurrent = wAttr;

    SetConsoleTextAttribute(g_hCurOut, wAttr);
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
standout()
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

	Sleep(15);		/* wait for 15 msec */
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
    CONSOLE_CURSOR_INFO cci;

    cci.bVisible = fVisible;
    /* make cursor big and visible (100 on Win95 makes it disappear)  */
    cci.dwSize = 99;           /* 100 percent cursor */
    SetConsoleCursorInfo(g_hCurOut, &cci);
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
	char_u *s,
	int len)
{
	s[len] = NUL;
	
    if (! term_console)
	{
        write(1, s, (unsigned) len);
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
#endif /* MCH_WRITE_DUMP */
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
#endif /* MCH_WRITE_DUMP */
			s++;
		}
		else if (s[0] == '\r')
		{
			/* \r, carriage return: go to beginning of line */
			gotoxy(g_srScrollRegion.Left+1, g_coord.Y + 1);
#ifdef MCH_WRITE_DUMP
			if (fdDump)
				fputs("\\r\n", fdDump);
#endif /* MCH_WRITE_DUMP */
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
#endif /* MCH_WRITE_DUMP */
			s++;
		}
		else if (s[0] == '\a')
		{
			/* \a, bell */
			MessageBeep(0xFFFFFFFF);
#ifdef MCH_WRITE_DUMP
			if (fdDump)
				fputs("\\a\n", fdDump);
#endif /* MCH_WRITE_DUMP */
			s++;
		}
		else if (s[0] == ESC  &&  len >= 3-1  &&  s[1] == '|')
		{
#ifdef MCH_WRITE_DUMP
			char_u* old_s = s;
#endif /* MCH_WRITE_DUMP */
			char_u* p;
			int arg1 = 0, arg2 = 0;
			
			switch (s[2])
			{
			/* one or two numeric arguments, separated by ';' */

			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				p = s + 2;
				arg1 = getdigits(&p);        /* no check for length! */
				if (p > s + len)
					break;
				
				if (*p == ';')
				{
					++p;
					arg2 = getdigits(&p);    /* no check for length! */
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
#endif /* MCH_WRITE_DUMP */
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
#endif /* MCH_WRITE_DUMP */

				len -= (nWritten - 1);
				s += nWritten;
			}
		}
	}

#ifdef MCH_WRITE_DUMP
	if (fdDump)
		fflush(fdDump);
#endif /* MCH_WRITE_DUMP */
}


/*
 * Delay for half a second.
 */
	void
mch_delay(
	long	msec,
	int		ignoreinput)
{
	if (ignoreinput)
    	Sleep((int)msec);
	else
		WaitForChar(msec);
}


#ifdef vim_remove
# undef vim_remove
#endif

/*
 * this version of remove is not scared by a readonly (backup) file
 */
	int
vim_remove(
	char_u *name)
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
    if (g_fCtrlCPressed || g_fCBrkPressed)
	{
        g_fCtrlCPressed = g_fCBrkPressed = FALSE;
        got_int = TRUE;
    }
}


/*
 * How much memory is available?
 */
	long
mch_avail_mem(
	int special)
{
	return LONG_MAX;		/* virtual memory, eh? */
}


/*
 * return non-zero if a character is available
 */
	int
mch_char_avail()
{
	return WaitForChar(0L);
}


/*
 * set screen mode, always fails.
 */
	int
mch_screenmode(
	char_u *arg)
{
	EMSG("Screen mode setting not supported");
	return FAIL;
}


/*
 * win95rename works around a bug in rename (aka MoveFile) in
 * Windows 95: rename("foo.bar", "foo.bar~") will generate a
 * file whose shortfilename is "FOO.BAR" (its longfilename will
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
win95rename(
	const char* pszOldFile,
	const char* pszNewFile)
{
	char szTempFile[_MAX_PATH];
	char szNewPath[_MAX_PATH];
	char* pszFilePart;
	HANDLE hf;

#undef rename

	/* get base path of new filename */
	if (GetFullPathName(pszNewFile, _MAX_PATH, szNewPath, &pszFilePart) == 0)
		return -1;
	else
		*pszFilePart = NUL;
	
	/* Get (and create) a unique temporary filename in directory of new file */
	if (GetTempFileName(szNewPath, "VIM", 0, szTempFile) == 0)
		return -2;

	/* blow the temp file away */
	if (! DeleteFile(szTempFile))
		return -3;

	/* rename old file to the temp file */
	if (! MoveFile(pszOldFile, szTempFile))
		return -4;

	/* now create an empty file called pszOldFile; this prevents
	 * the operating system using pszOldFile as an alias (SFN)
	 * if we're renaming within the same directory.  For example,
	 * we're editing a file called filename.asc.txt by its SFN,
	 * filena~1.txt.  If we rename filena~1.txt to filena~1.txt~
	 * (i.e., we're making a backup while writing it), the SFN
	 * for filena~1.txt~ will be filena~1.txt, by default, which
	 * will cause all sorts of problems later in buf_write.  So, we
	 * create an empty file called filena~1.txt and the system will have
	 * to find some other SFN for filena~1.txt~, such as filena~2.txt
	 */
	if ((hf = CreateFile(pszOldFile, GENERIC_WRITE, 0, NULL, CREATE_NEW,
						 FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return -5;
	if (! CloseHandle(hf))
		return -6;
	
	/* rename the temp file to the new file */
	if (! MoveFile(szTempFile, pszNewFile))
		return -7;

	/* finally, remove the empty old file */
	if (! DeleteFile(pszOldFile))
		return -8;

	return 0;	/* success */
}

/*
 * Special version of getenv(): use $HOME when $VIM not defined.
 */
	char_u *
vim_getenv(
	char_u *var)
{
	char_u	*retval;

	retval = (char_u *)getenv((char *)var);

	if (retval == NULL && STRCMP(var, "VIM") == 0)
		retval = (char_u *)getenv("HOME");

#ifdef MCH_WRITE_DUMP
	if (fdDump)
	{
		fprintf(fdDump, "$%s = \"%s\"\n", var, retval);
		fflush(fdDump);
	}
#endif

	return retval;
}


/*
 * Get the default shell for the current hardware platform
 */
	char*
default_shell()
{
	char* psz = NULL;

	PlatformId();

	if (g_PlatformId == VER_PLATFORM_WIN32_NT)            /* Windows NT */
		psz = "cmd.exe";
	else if (g_PlatformId == VER_PLATFORM_WIN32_WINDOWS)  /* Windows 95 */
		psz = "command.com";

	return psz;
}


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
# endif /* MCH_WRITE_DUMP */
}
