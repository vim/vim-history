/*
    PUSHKEYS 1.0 for C

    original version 1.0 for Visual Basic
    Copyright by Data Solutions Pty Ltd (ACN 010 951 498)
    All rights reserved.
    Email for info: clatta@ozemail.com.au

    Ported to C by Alexander Frink (Alexander.Frink@Uni-Mainz.DE) January 1998.

    This port is as close as possible to the original Visual Basic code.
    See the documentation that comes with the Visual Basic version.
    It uses the same API calls, so it should work under the same circumstances
    as the original version. However, not much effort was made to let the code
    look C-like.

    Successfully compiled with Visual C++ 4.2 under Windows NT 4.0 (Intel).

    Usage:
    e.g. PushKeys("Hello World!{enter}");
    Sends 'Hello World!', followed by a Return to the window which currently
    receives the keyboard input.
    To select the window which should receive the keyboard input, use the
    SetForegroundWindow() API (SetActiveWindow() is not always sufficient).
    You can open a handle to an open window based on its title with the
    EnumWindows() and GetWindowText() APIs.
*/

#define MAXLEN 1024 // maximum length of a string passed to PushKeys(), may be increased

void PushKeys(LPCTSTR Source);

// the following functions are used internally only
void PressKey(BYTE Vk, UINT Scan);
void PushAKey(CHAR ch);
void PushFnKey(LPTSTR KeyCode);
void PushShiftKey(CHAR ch);
char *strchrcat(char *strDest, const char ch);

