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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "pushkeys.h"

#define VK_BREAK    VK_CANCEL
#define VK_BELL     0x07
#define VK_LINEFEED 0x0A

#if defined(UNICODE)||defined(_UNICODE)
#error sorry, this program is not unicode clean
#endif

// These variables indicate whether these keys are pressed
BOOL AltOn, ShiftOn, ControlOn;

char *strchrcat(char *strDest, const char ch)
{
    // append a character to a string

    CHAR tmp[2];

    tmp[0]=ch;
    tmp[1]='\0';
    strcat(strDest,tmp);
    return strDest;
}

void PressKey(BYTE Vk, UINT Scan)
{
    /*
    Copyright by Data Solutions Pty Ltd (ACN 010 951 498)
    All rights reserved.
    Email for info: clatta@ozemail.com.au
    */

    // Presses the appropriate key specified
    keybd_event(Vk, (BYTE)Scan, 0, 0);
    keybd_event(Vk, (BYTE)Scan, KEYEVENTF_KEYUP, 0);
}

void PushAKey(CHAR ch)
{
    /*
    Copyright by Data Solutions Pty Ltd (ACN 010 951 498)
    All rights reserved.
    Email for info: clatta@ozemail.com.au
    */

    // Pushes a normal (non-function) key

    INT Vk;
    INT Scan;
    CHAR character[2];
    CHAR oemchar[2];

    character[0]=ch;
    character[1]='\0';

    // Get the virtual key code for this character
    Vk = VkKeyScan(character[0]) & 0xFF;
    CharToOem(character, oemchar);

    // Get the scan code for this key
    Scan = OemKeyScan(oemchar[0]) & 0xFF;

    // Press the key
    PressKey((BYTE)Vk, (BYTE)Scan);
}


void PushFnKey(LPTSTR KeyCode)
{
    /*
    Copyright by Data Solutions Pty Ltd (ACN 010 951 498)
    All rights reserved.
    Email for info: clatta@ozemail.com.au
    */

    // Outputs function key. KeyCode may have a number of times to be output
    UINT ScanKey;
    LONG NumPushes;
    CHAR *SpacePos;
    INT Ctr;
    CHAR FnKey[MAXLEN];
    CHAR OrigCode[MAXLEN];

    // On Error GoTo PushFnKey_ErrorHandler

    // Work out which function key to push and how many times
    strcpy(OrigCode,KeyCode);
    if ((SpacePos=strrchr(OrigCode,' '))!=NULL) {
        // We're doing this multpile times
        // Find the rightmost space. I do this because I want to allow people to
        // output multiple words with spaces in them. eg "{Seven times 7}"
        *SpacePos='\0';
        NumPushes=atoi(SpacePos+1);
        if (NumPushes<=0) NumPushes=1;
    } else {
        // Once only
        NumPushes=1;
    }
    strcpy(FnKey,OrigCode);
    _strupr(FnKey);

    // Function Keys
    // BTW, if you're reading my code, I do the for-next loop in each case rather
    // than outside the select because I figured it might be faster to do one Select
    // and the for-next loop n-times than do both the for-next and the Select n-times.
    if ((strcmp(FnKey,"BACKSPACE")==0)||(strcmp(FnKey,"BS")==0)||(strcmp(FnKey,"BKSP")==0)) {
        // Backspace
        ScanKey = MapVirtualKey(VK_BACK, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_BACK,ScanKey);
        }
    } else if (strcmp(FnKey,"BELL")==0) {
        // Bell = ^G
        ScanKey = MapVirtualKey(VK_BELL, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_BELL,ScanKey);
        }
    } else if (strcmp(FnKey,"BREAK")==0) {
        // Break = ^C
        ScanKey = MapVirtualKey(VK_BREAK, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_BREAK,ScanKey);
        }
    } else if ((strcmp(FnKey,"CAPSLOCK")==0)||(strcmp(FnKey,"CAPS")==0)) {
        // Capslock
        ScanKey = MapVirtualKey(VK_CAPITAL, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_CAPITAL,ScanKey);
        }
    } else if ((strcmp(FnKey,"DELETE")==0)||(strcmp(FnKey,"DEL")==0)) {
        // Delete
        ScanKey = MapVirtualKey(VK_DELETE, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_DELETE,ScanKey);
        }
    } else if (strcmp(FnKey,"DOWN")==0) {
        // Down Arrow
        ScanKey = MapVirtualKey(VK_DOWN, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_DOWN,ScanKey);
        }
    } else if (strcmp(FnKey,"END")==0) {
        // End
        ScanKey = MapVirtualKey(VK_END, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_END,ScanKey);
        }
    } else if ((strcmp(FnKey,"ENTER")==0)||(strcmp(FnKey,"RETURN")==0)) {
        // Enter = ^M
        ScanKey = MapVirtualKey(VK_RETURN, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_RETURN,ScanKey);
        }
    } else if ((strcmp(FnKey,"ESCAPE")==0)||(strcmp(FnKey,"ESC")==0)) {
        // Escape = ^[
        ScanKey = MapVirtualKey(VK_ESCAPE, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_ESCAPE,ScanKey);
        }
    } else if (strcmp(FnKey,"FF")==0) {
        // Form feed = ^L
        ScanKey = MapVirtualKey(VK_CLEAR, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_CLEAR,ScanKey);
        }
    } else if (strcmp(FnKey,"HELP")==0) {
        // Help
        ScanKey = MapVirtualKey(VK_HELP, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_HELP,ScanKey);
        }
    } else if (strcmp(FnKey,"HOME")==0) {
        // Home
        ScanKey = MapVirtualKey(VK_HOME, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_HOME,ScanKey);
        }
    } else if ((strcmp(FnKey,"INSERT")==0)||(strcmp(FnKey,"INS")==0)) {
        // Insert
        ScanKey = MapVirtualKey(VK_INSERT, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_INSERT,ScanKey);
        }
    } else if (strcmp(FnKey,"LEFT")==0) {
        // Left Arrow
        ScanKey = MapVirtualKey(VK_LEFT, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_LEFT,ScanKey);
        }
    } else if ((strcmp(FnKey,"LINEFEED")==0)||(strcmp(FnKey,"LF")==0)) {
        // Linefeed = ^J
        ScanKey = MapVirtualKey(VK_LINEFEED, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_LINEFEED,ScanKey);
        }
    } else if ((strcmp(FnKey,"NEWLINE")==0)||(strcmp(FnKey,"NL")==0)) {
        // New line = Carraige return & Line Feed = ^M^J
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            ScanKey = MapVirtualKey(VK_RETURN, 0);
            PressKey(VK_RETURN,ScanKey);
            ScanKey = MapVirtualKey(VK_LINEFEED, 0);
            PressKey(VK_LINEFEED,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMLOCK")==0) {
        // Numeric Lock
        ScanKey = MapVirtualKey(VK_NUMLOCK, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMLOCK,ScanKey);
        }
    } else if ((strcmp(FnKey,"PGDN")==0)||(strcmp(FnKey,"PAGEDOWN")==0)||(strcmp(FnKey,"NEXT")==0)) {
        // Page Down
        ScanKey = MapVirtualKey(VK_NEXT, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NEXT,ScanKey);
        }
    } else if ((strcmp(FnKey,"PGUP")==0)||(strcmp(FnKey,"PAGEUP")==0)||(strcmp(FnKey,"PRIOR")==0)) {
        // Page Up
        ScanKey = MapVirtualKey(VK_PRIOR, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_PRIOR,ScanKey);
        }
    } else if ((strcmp(FnKey,"PRINTSCREEN")==0)||(strcmp(FnKey,"PRTSC")==0)) {
        // Print Screen
        ScanKey = MapVirtualKey(VK_PRINT, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_PRINT,ScanKey);
        }
    } else if (strcmp(FnKey,"RIGHT")==0) {
        // Right Arrow
        ScanKey = MapVirtualKey(VK_RIGHT, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_RIGHT,ScanKey);
        }
    } else if ((strcmp(FnKey,"SCROLLLOCK")==0)||(strcmp(FnKey,"SCRLK")==0)) {
        // Scroll lock
        ScanKey = MapVirtualKey(VK_SCROLL, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_SCROLL,ScanKey);
        }
    } else if (strcmp(FnKey,"TAB")==0) {
        // TAB = ^I
        ScanKey = MapVirtualKey(VK_TAB, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_TAB,ScanKey);
        }
    } else if (strcmp(FnKey,"UP")==0) {
        // Up Arrow
        ScanKey = MapVirtualKey(VK_UP, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_UP,ScanKey);
        }
    } else if (strcmp(FnKey,"SLEEP")==0) {
        // We sleep for a few seconds
        Sleep(NumPushes*1000);
    } else if (strcmp(FnKey,"F1")==0) {
        // F1
        ScanKey = MapVirtualKey(VK_F1, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F1,ScanKey);
        }
    } else if (strcmp(FnKey,"F2")==0) {
        // F2
        ScanKey = MapVirtualKey(VK_F2, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F2,ScanKey);
        }
    } else if (strcmp(FnKey,"F3")==0) {
        // F3
        ScanKey = MapVirtualKey(VK_F3, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F3,ScanKey);
        }
    } else if (strcmp(FnKey,"F4")==0) {
        // F4
        ScanKey = MapVirtualKey(VK_F4, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F4,ScanKey);
        }
    } else if (strcmp(FnKey,"F5")==0) {
        // F5
        ScanKey = MapVirtualKey(VK_F5, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F5,ScanKey);
        }
    } else if (strcmp(FnKey,"F6")==0) {
        // F6
        ScanKey = MapVirtualKey(VK_F6, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F6,ScanKey);
        }
    } else if (strcmp(FnKey,"F7")==0) {
        // F7
        ScanKey = MapVirtualKey(VK_F7, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F7,ScanKey);
        }
    } else if (strcmp(FnKey,"F8")==0) {
        // F8
        ScanKey = MapVirtualKey(VK_F8, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F8,ScanKey);
        }
    } else if (strcmp(FnKey,"F9")==0) {
        // F9
        ScanKey = MapVirtualKey(VK_F9, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F9,ScanKey);
        }
    } else if (strcmp(FnKey,"F10")==0) {
        // F10
        ScanKey = MapVirtualKey(VK_F10, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F10,ScanKey);
        }
    } else if (strcmp(FnKey,"F11")==0) {
        // F11
        ScanKey = MapVirtualKey(VK_F11, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F11,ScanKey);
        }
    } else if (strcmp(FnKey,"F12")==0) {
        // F12
        ScanKey = MapVirtualKey(VK_F12, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F12,ScanKey);
        }
    } else if (strcmp(FnKey,"F13")==0) {
        // F13
        ScanKey = MapVirtualKey(VK_F13, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F13,ScanKey);
        }
    } else if (strcmp(FnKey,"F14")==0) {
        // F14
        ScanKey = MapVirtualKey(VK_F14, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F14,ScanKey);
        }
    } else if (strcmp(FnKey,"F15")==0) {
        // F15
        ScanKey = MapVirtualKey(VK_F15, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F15,ScanKey);
        }
    } else if (strcmp(FnKey,"F16")==0) {
        // F16
        ScanKey = MapVirtualKey(VK_F16, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F16,ScanKey);
        }
    } else if (strcmp(FnKey,"F17")==0) {
        // F17
        ScanKey = MapVirtualKey(VK_F17, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F17,ScanKey);
        }
    } else if (strcmp(FnKey,"F18")==0) {
        // F18
        ScanKey = MapVirtualKey(VK_F18, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F18,ScanKey);
        }
    } else if (strcmp(FnKey,"F19")==0) {
        // F19
        ScanKey = MapVirtualKey(VK_F19, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F19,ScanKey);
        }
    } else if (strcmp(FnKey,"F20")==0) {
        // F20
        ScanKey = MapVirtualKey(VK_F20, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F20,ScanKey);
        }
    } else if (strcmp(FnKey,"F21")==0) {
        // F21
        ScanKey = MapVirtualKey(VK_F21, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F21,ScanKey);
        }
    } else if (strcmp(FnKey,"F22")==0) {
        // F22
        ScanKey = MapVirtualKey(VK_F22, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F22,ScanKey);
        }
    } else if (strcmp(FnKey,"F23")==0) {
        // F23
        ScanKey = MapVirtualKey(VK_F23, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F23,ScanKey);
        }
    } else if (strcmp(FnKey,"F24")==0) {
        // F24
        ScanKey = MapVirtualKey(VK_F24, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_F24,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD0")==0) {
        // Numeric Keypad 0
        ScanKey = MapVirtualKey(VK_NUMPAD0, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD0,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD1")==0) {
        // Numeric Keypad 1
        ScanKey = MapVirtualKey(VK_NUMPAD1, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD1,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD2")==0) {
        // Numeric Keypad 2
        ScanKey = MapVirtualKey(VK_NUMPAD2, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD2,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD3")==0) {
        // Numeric Keypad 3
        ScanKey = MapVirtualKey(VK_NUMPAD3, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD3,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD4")==0) {
        // Numeric Keypad 4
        ScanKey = MapVirtualKey(VK_NUMPAD4, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD4,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD5")==0) {
        // Numeric Keypad 5
        ScanKey = MapVirtualKey(VK_NUMPAD5, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD5,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD6")==0) {
        // Numeric Keypad 6
        ScanKey = MapVirtualKey(VK_NUMPAD6, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD6,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD7")==0) {
        // Numeric Keypad 7
        ScanKey = MapVirtualKey(VK_NUMPAD7, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD7,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD8")==0) {
        // Numeric Keypad 8
        ScanKey = MapVirtualKey(VK_NUMPAD8, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD8,ScanKey);
        }
    } else if (strcmp(FnKey,"NUMPAD9")==0) {
        // Numeric Keypad 9
        ScanKey = MapVirtualKey(VK_NUMPAD9, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_NUMPAD9,ScanKey);
        }
    } else if ((strcmp(FnKey,"NUMPADMULTIPLY")==0)||(strcmp(FnKey,"NUMPAD*")==0)) {
        // Numeric Keypad Multiply
        ScanKey = MapVirtualKey(VK_MULTIPLY, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_MULTIPLY,ScanKey);
        }
    } else if ((strcmp(FnKey,"NUMPADADD")==0)||(strcmp(FnKey,"NUMPAD+")==0)) {
        // Numeric Keypad +
        ScanKey = MapVirtualKey(VK_ADD, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_ADD,ScanKey);
        }
    } else if ((strcmp(FnKey,"NUMPADSUBTRACT")==0)||(strcmp(FnKey,"NUMPAD-")==0)) {
        // Numeric Keypad -
        ScanKey = MapVirtualKey(VK_SUBTRACT, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_SUBTRACT,ScanKey);
        }
    } else if ((strcmp(FnKey,"NUMPADDECIMAL")==0)||(strcmp(FnKey,"NUMPAD.")==0)) {
        // Numeric Keypad .
        ScanKey = MapVirtualKey(VK_DECIMAL, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_DECIMAL,ScanKey);
        }
    } else if ((strcmp(FnKey,"NUMPADDIVIDE")==0)||(strcmp(FnKey,"NUMPAD/")==0)) {
        // Numeric Keypad /
        ScanKey = MapVirtualKey(VK_DIVIDE, 0);
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PressKey(VK_DIVIDE,ScanKey);
        }
    } else if ((strcmp(FnKey,"~")==0)||
               (strcmp(FnKey,"!")==0)||
               (strcmp(FnKey,"@")==0)||
               (strcmp(FnKey,"#")==0)||
               (strcmp(FnKey,"$")==0)||
               (strcmp(FnKey,"%")==0)||
               (strcmp(FnKey,"^")==0)||
               (strcmp(FnKey,"&")==0)||
               (strcmp(FnKey,"*")==0)||
               (strcmp(FnKey,"(")==0)||
               (strcmp(FnKey,")")==0)||
               (strcmp(FnKey,"_")==0)||
               (strcmp(FnKey,"+")==0)||
               (strcmp(FnKey,"|")==0)||
               (strcmp(FnKey,"{")==0)||
               (strcmp(FnKey,"}")==0)||
               (strcmp(FnKey,"\"")==0)||
               (strcmp(FnKey,":")==0)||
               (strcmp(FnKey,"?")==0)||
               (strcmp(FnKey,">")==0)||
               (strcmp(FnKey,"<")==0)) {
        // Shifted keys
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PushShiftKey(OrigCode[0]);
        }
    } else {
        // Ordinary keys
        for (Ctr=1; Ctr<=NumPushes; Ctr++) {
            PushKeys(OrigCode);
        }
    }
}

void PushKeys(LPCTSTR Source)
{
    /*
    Copyright by Data Solutions Pty Ltd (ACN 010 951 498)
    All rights reserved.
    Email for info: clatta@ozemail.com.au
    */

    // This is the routine that does all the work

    UINT Ctr;
    INT vkscan;
    CHAR SubStr[MAXLEN];
    INT BrCnt;
    CHAR BrOpen;
    CHAR BrClose;
    CHAR Ch;
    CHAR Next;

    if (Source==NULL) {
        fprintf(stderr,"Error in PushKeys(): null pointer.\n");
        return;
    }
    if (strlen(Source)>=MAXLEN) {
        fprintf(stderr,"Error in PushKeys(): string too long.\n");
        return;
    }

    ShiftOn = FALSE;
    ControlOn = FALSE;
    AltOn = FALSE;

    Ctr = 0;
    while (Ctr<strlen(Source)) {
        Next=Source[Ctr];
        if (Next=='+') {
            // Shift key pressed
            BrOpen=Source[Ctr+1];
            if ((BrOpen=='(')||(BrOpen=='{')) {
                if (BrOpen=='(') {
                    BrClose=')';
                } else {
                    BrClose='}';
                }
                Ctr++;
                SubStr[0]='\0';
                BrCnt=1;
                while ((Ctr<strlen(Source))&&(BrCnt!=0)) {
                    Ctr++;
                    Ch=Source[Ctr];
                    if (Ch!=BrClose) {
                        strchrcat(SubStr,Ch);
                    } else {
                        BrCnt--;
                    }
                    if (Ch==BrOpen) BrCnt++;
                }

                if (!ShiftOn) {
                    // Turn shift on
                    vkscan = MapVirtualKey(VK_SHIFT,0);
                    keybd_event(VK_SHIFT, (BYTE)vkscan, 0, 0);
                    ShiftOn=TRUE;
                }

                // Push the keys
                if (BrOpen=='(') {
                    PushKeys(SubStr);
                } else {
                    PushFnKey(SubStr);
                }

                // Turn shift off
                vkscan = MapVirtualKey(VK_SHIFT,0);
                keybd_event(VK_SHIFT, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
                ShiftOn=FALSE;
            } else {
                // The next key is shifted
                Ctr++;
                PushShiftKey(Source[Ctr]);
            }
        } else if (Next=='^') {
            // Control key pressed
            BrOpen=Source[Ctr+1];
            if ((BrOpen=='(')||(BrOpen=='{')) {
                if (BrOpen=='(') {
                    BrClose=')';
                } else {
                    BrClose='}';
                }
                Ctr++;
                SubStr[0]='\0';
                BrCnt=1;
                while ((Ctr<strlen(Source))&&(BrCnt!=0)) {
                    Ctr++;
                    Ch=Source[Ctr];
                    if (Ch!=BrClose) {
                        strchrcat(SubStr,Ch);
                    } else {
                        BrCnt--;
                    }
                    if (Ch==BrOpen) BrCnt++;
                }

                if (!ControlOn) {
                    // Turn control on
                    vkscan = MapVirtualKey(VK_CONTROL,0);
                    keybd_event(VK_CONTROL, (BYTE)vkscan, 0, 0);
                    ControlOn=TRUE;
                }

                // Push the keys
                if (BrOpen=='(') {
                    PushKeys(SubStr);
                } else {
                    PushFnKey(SubStr);
                }

                // Turn control off
                vkscan = MapVirtualKey(VK_CONTROL,0);
                keybd_event(VK_CONTROL, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
                ControlOn=FALSE;
            } else {
                // The next key is controlled
                Ctr++;
                if (!ControlOn) {
                    vkscan = MapVirtualKey(VK_CONTROL,0);
                    keybd_event(VK_CONTROL, (BYTE)vkscan, 0, 0);
                }
                PushAKey(Source[Ctr]);
                vkscan = MapVirtualKey(VK_CONTROL,0);
                keybd_event(VK_CONTROL, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
            }
        } else if (Next=='%') {
            // Alt key pressed
            BrOpen=Source[Ctr+1];
            if ((BrOpen=='(')||(BrOpen=='{')) {
                if (BrOpen=='(') {
                    BrClose=')';
                } else {
                    BrClose='}';
                }
                Ctr++;
                SubStr[0]='\0';
                BrCnt=1;
                while ((Ctr<strlen(Source))&&(BrCnt!=0)) {
                    Ctr++;
                    Ch=Source[Ctr];
                    if (Ch!=BrClose) {
                        strchrcat(SubStr,Ch);
                    } else {
                        BrCnt--;
                    }
                    if (Ch==BrOpen) BrCnt++;
                }

                if (!AltOn) {
                    // Turn Alt on
                    vkscan = MapVirtualKey(VK_MENU,0);
                    keybd_event(VK_MENU, (BYTE)vkscan, 0, 0);
                    AltOn=TRUE;
                }

                // Push the keys
                if (BrOpen=='(') {
                    PushKeys(SubStr);
                } else {
                    PushFnKey(SubStr);
                }

                // Turn Alt off
                vkscan = MapVirtualKey(VK_MENU,0);
                keybd_event(VK_MENU, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
                AltOn=FALSE;
            } else {
                // The next key is alted
                Ctr++;
                if (!AltOn) {
                    vkscan = MapVirtualKey(VK_MENU,0);
                    keybd_event(VK_MENU, (BYTE)vkscan, 0, 0);
                }
                PushAKey(Source[Ctr]);
                vkscan = MapVirtualKey(VK_MENU,0);
                keybd_event(VK_MENU, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
            }
        } else if (Next=='{') {
            // Function keys
            Ctr++;
            SubStr[0]='\0';
            while ((Ctr<strlen(Source)-1)&&(Source[Ctr]!='}')) {
                strchrcat(SubStr,Source[Ctr]);
                Ctr++;
            }
            if ((Source[Ctr]=='}')&&(Source[Ctr+1]=='}')) {
                // Right brace
                PushShiftKey('}');
                Ctr++;
            } else {
                PushFnKey(SubStr);
            }
        } else if (((Next>='A')&&(Next<='Z'))||
            (Next=='!')||(Next=='@')||(Next=='#')||(Next=='$')||(Next=='&')||(Next=='*')||
            (Next=='(')||(Next==')')||(Next=='_')||(Next=='|')||(Next=='\"')||(Next==':')||
            (Next==':')||(Next=='?')||(Next=='>')||(Next=='<')) {
            // Shifted keys
            PushShiftKey(Next);
        } else if (Next=='~') {
            // The enter key
            PushFnKey("ENTER");
        } else {
            PushAKey(Next);
        }
        Ctr++;
    }
    if (ShiftOn) {
        // Turn shift off if need be
        vkscan = MapVirtualKey(VK_SHIFT, 0);
        keybd_event(VK_SHIFT, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
    }
    if (ControlOn) {
        // Turn control off if need be
        vkscan = MapVirtualKey(VK_CONTROL, 0);
        keybd_event(VK_CONTROL, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
    }
    if (AltOn) {
        // Turn alt off if need be
        vkscan = MapVirtualKey(VK_MENU, 0);
        keybd_event(VK_MENU, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
    }
}

void PushShiftKey(CHAR ch)
{
    /*
    Copyright by Data Solutions Pty Ltd (ACN 010 951 498)
    All rights reserved.
    Email for info: clatta@ozemail.com.au
    */

    // Pushes a key with the shift key down
    // If ShiftOn is true, it does not press and release the shift key

    INT vkscan;

    if (!ShiftOn) {
        // Turn shift on
        vkscan = MapVirtualKey(VK_SHIFT, 0);
        keybd_event(VK_SHIFT, (BYTE)vkscan, 0, 0);
    }

    // Push key
    PushAKey(ch);

    if (!ShiftOn) {
        // Turn shift off
        vkscan = MapVirtualKey(VK_SHIFT, 0);
        keybd_event(VK_SHIFT, (BYTE)vkscan, KEYEVENTF_KEYUP, 0);
    }
}

