/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
 *
 */

/*
 * winnt.c
 *
 * Windows NT system-dependent routines.
 * A reasonable approximation of the amiga dependent code.
 * Portions lifted from SDK samples, from the MSDOS dependent code,
 * and from NetHack 3.1.3.
 *
 * rogerk@wonderware.com
 */

#include <io.h>
#include "vim.h"
#include "globals.h"
#include "param.h"
#include "proto.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>

static int WaitForChar __ARGS((int));
static int cbrk_handler __ARGS(());

/* Win32 Console handles for input and output */
HANDLE          hConIn;
HANDLE          hConOut;

/* Win32 Screen buffer,coordinate,console I/O information */
CONSOLE_SCREEN_BUFFER_INFO csbi;
COORD           ntcoord;
INPUT_RECORD    ir;

/* The attribute of the screen when the editor was started */
WORD            DefaultAttribute;

typedef struct filelist {
    char          **file;
    int             nfiles;
    int             maxfiles;
}               FileList;

static void addfile __ARGS((FileList *, char *, int));
static int      pstrcmp();      /* __ARGS((char **, char **)); BCC does not
                                 * like this */
static void strlowcpy __ARGS((char *, char *));
static int expandpath __ARGS((FileList *, char *, int, int, int));

static int      cbrk_pressed = FALSE;   /* set by ctrl-break interrupt */
static int      ctrlc_pressed = FALSE;  /* set when ctrl-C or ctrl-break
                                         * detected */

void            vim_delay()
{
    delay(500);
}

/*
 * this version of remove is not scared by a readonly (backup) file
 */
int             vim_remove(name)
    char           *name;
{
    setperm(name, _S_IWRITE);   /* default permissions */
    return unlink(name);
}

/*
 * mch_write(): write the output buffer to the screen
 */
void            mch_write(s, len)
    char           *s;
    int             len;
{
    char           *p;
    int             row,
                    col;

	s[len] = '\0';
    if (term_console)           /* translate ESC | sequences into bios calls */
        while (len--) {

			/* optimization: use one single WriteConsole for runs of text,
			   rather than calling putch() multiple times.  It ain't curses,
			   but it helps. */

            DWORD           prefix = strcspn(s, "\n\r\a\033");

            if (prefix) {
                DWORD           nwritten;

                if (WriteConsole(hConOut, s, prefix, &nwritten, 0)) {

                    len -= (nwritten - 1);
                    s += nwritten;
                }
                continue;
            }

            if (s[0] == '\n') {
                if (ntcoord.Y == (Rows - 1)) {
                    gotoxy(1, ntcoord.Y + 1);
                    scroll();
                } else {
                    gotoxy(1, ntcoord.Y + 2);
                }
                s++;
                continue;
            } else if (s[0] == '\r') {
                gotoxy(1, ntcoord.Y + 1);
                s++;
                continue;
            } else if (s[0] == '\a') {
                vbell();
                s++;
                continue;
            } else if (s[0] == ESC && len > 1 && s[1] == '|') {
                switch (s[2]) {

                case 'v':
                    cursor_visible(0);
                    goto got3;

                case 'V':
                    cursor_visible(1);
                    goto got3;

                case 'J':
                    clrscr();
                    goto got3;

                case 'K':
                    clreol();
                    goto got3;

                case 'L':
                    insline(1);
                    goto got3;

                case 'M':
                    delline(1);
            got3:   s += 3;
                    len -= 2;
                    continue;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    p = s + 2;
                    row = getdigits(&p);        /* no check for length! */
                    if (p > s + len)
                        break;
                    if (*p == ';') {
                        ++p;
                        col = getdigits(&p);    /* no check for length! */
                        if (p > s + len)
                            break;
                        if (*p == 'H') {
                            gotoxy(col, row);
                            len -= p - s;
                            s = p + 1;
                            continue;
                        }
                    } else if (*p == 'm') {
                        if (row == 0)
                            normvideo();
                        else
                            textattr(row);
                        len -= p - s;
                        s = p + 1;
                        continue;
                    } else if (*p == 'L') {
                        insline(row);
                        len -= p - s;
                        s = p + 1;
                        continue;
                    } else if (*p == 'M') {
                        delline(row);
                        len -= p - s;
                        s = p + 1;
                        continue;
                    }
                }
            }
            putch(*s++);
        }
    else
        write(1, s, (unsigned) len);
}
/*
 *  Keyboard translation tables.
 *  (Adopted from the MSDOS port)
 */

#define KEYPADLO	0x47
#define KEYPADHI	0x53

#define PADKEYS 	(KEYPADHI - KEYPADLO + 1)
#define iskeypad(x)	(KEYPADLO <= (x) && (x) <= KEYPADHI)

/*
 * Wait until console input is available
 */

static int      WaitForChar(msec)
    int             msec;
{
    int             count;
    int             ch;
    int             scan;
    int             shiftstate;
    int             altseq;
    int             retval = 0;

    if (WaitForSingleObject(hConIn, msec) == WAIT_OBJECT_0) {
        count = 0;
        PeekConsoleInput(hConIn, &ir, 1, &count);
        if (count > 0) {
            ch = ir.Event.KeyEvent.uChar.AsciiChar;
            scan = ir.Event.KeyEvent.wVirtualScanCode;
            shiftstate = ir.Event.KeyEvent.dwControlKeyState;
            if (((ir.EventType == KEY_EVENT) && ir.Event.KeyEvent.bKeyDown) &&
                (ch || (iskeypad(scan)))) {
                retval = 1;     /* Found what we sought */
            }
        } else {                /* There are no events in console event queue */
            retval = 0;
        }
    }
    return retval;
}

static int pending = 0;

int             tgetch()
{
    int             valid = 0;
    int             metaflags = 0;
    int             count;
    unsigned short int scan;
    unsigned char   ch;
    unsigned long   shiftstate;
    const struct pad *kpad;
    char            keymess[100];

    if (pending)
    {
        ch = pending;
        pending = 0;
    }
    else
    {

        valid = 0;
        while (!valid) {
            ReadConsoleInput(hConIn, &ir, 1, &count);
            if (ir.EventType == WINDOW_BUFFER_SIZE_EVENT) {
                set_winsize(Rows, Columns, FALSE);
            } 
            else
            {
                if ((ir.EventType == KEY_EVENT) && ir.Event.KeyEvent.bKeyDown) 
                {
                    ch = ir.Event.KeyEvent.uChar.AsciiChar;
                    scan = ir.Event.KeyEvent.wVirtualScanCode;
                    if (ch || (iskeypad(scan)))
                        valid = 1;
                }
            }
        }
        if (!ch)
        {
            pending = scan;
            ch = 0;
        }
    }
    return ch;
}


int             kbhit()
{
    int             done = 0;   /* true =  "stop searching"        */
    int             retval;     /* true =  "we had a match"        */
    int             count;
    unsigned short int scan;
    unsigned char   ch;
    unsigned long   shiftstate;

    if (pending)
        return 1;

    done = 0;
    retval = 0;
    while (!done) {
        count = 0;
        PeekConsoleInput(hConIn, &ir, 1, &count);
        if (count > 0) {
            ch = ir.Event.KeyEvent.uChar.AsciiChar;
            scan = ir.Event.KeyEvent.wVirtualScanCode;
            shiftstate = ir.Event.KeyEvent.dwControlKeyState;
            if (((ir.EventType == KEY_EVENT) && ir.Event.KeyEvent.bKeyDown) &&
                (ch || (iskeypad(scan)) )) {
                done = 1;       /* Stop looking         */
                retval = 1;     /* Found what we sought */
            } else              /* Discard it, its an insignificant event */
                ReadConsoleInput(hConIn, &ir, 1, &count);
        } else {                /* There are no events in console event queue */
            done = 1;           /* Stop looking               */
            retval = 0;
        }
    }
    return retval;
}


/*
 * GetChars(): low level input funcion.
 * Get a characters from the keyboard.
 * If time == 0 do not wait for characters.
 * If time == n wait a short time for characters.
 * If time == -1 wait forever for characters.
 */
int             GetChars(buf, maxlen, time)
    char           *buf;
    int             maxlen;
    int             time;
{
    int             len = 0;
    int             c;

    if (time >= 0) {
        if (time == 0)          /* don't know if time == 0 is allowed */
            time = 1;
        if (WaitForChar(time) == 0)     /* no character available */
            return 0;
    } else {                    /* time == -1 */
        /* If there is no character available within 2 seconds (default)
         * write the autoscript file to disk */
        if (WaitForChar((int) p_ut) == 0)
            updatescript(0);
    }

/*
 * Try to read as many characters as there are.
 * Works for the controlling tty only.
 */
    --maxlen;                   /* may get two chars at once */
    /* we will get at least one key. Get more if they are available After a
     * ctrl-break we have to read a 0 (!) from the buffer. bioskey(1) will
     * return 0 if no key is available and when a ctrl-break was typed. When
     * ctrl-break is hit, this does not always implies a key hit. */
    cbrk_pressed = FALSE;
    while ((len == 0 || kbhit()) && len < maxlen) {
        switch (c = tgetch()) {
        case 0:
            *buf++ = K_NUL;
            break;
        case 3:
            cbrk_pressed = TRUE;
            /* FALLTHROUGH */
        default:
            *buf++ = c;
        }
        len++;
    }
    return len;
}

/*
 * We have no job control, fake it by starting a new shell.
 */
void            mch_suspend()
{
    outstr("new shell started\n");
    call_shell(NULL, 0, TRUE);
}

extern int      _fmode;
char            OrigTitle[256];
/*
 */
void            mch_windinit()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    _fmode = O_BINARY;          /* we do our own CR-LF translation */
    flushbuf();

    /* Obtain handles for the standard Console I/O devices */
    hConIn = CreateFile("CONIN$",
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL, OPEN_EXISTING, 0, NULL);

    hConOut = CreateFile("CONOUT$",
                         GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL, OPEN_EXISTING, 0, NULL);

    GetConsoleTitle(OrigTitle, sizeof(OrigTitle));

    /* get current attributes and fill out CHAR_INFO structure for fill char */
    GetConsoleScreenBufferInfo(hConOut, &csbi);
    DefaultAttribute = csbi.wAttributes;

    mch_get_winsize();
}

void            check_win(argc, argv)
    int             argc;
    char          **argv;
{
    if (!isatty(0) || !isatty(1)) {
        fprintf(stderr, "VIM: no controlling terminal\n");
        exit(2);
    }
    /* In some cases with DOS 6.0 on a NEC notebook there is a 12 seconds
     * delay when starting up that can be avoided by the next two lines.
     * Don't ask me why! This could be fixed by removing setver.sys from
     * config.sys. Forget it. gotoxy(1,1); cputs(" "); */
}

/*
 * fname_case(): Set the case of the filename, if it already exists.
 *				 msdos filesystem is far to primitive for that. do nothing.
 */
void            fname_case(name)
    char           *name;
{
}


/*
 * mch_settitle(): set titlebar of our window
 * Can the icon also be set?
 */
void         mch_settitle(title, icon)
    char           *title;
    char           *icon;
{
    if (title != NULL)
        SetConsoleTitle(title);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *	1  Just restore title
 *  2  Just restore icon (which we don't have)
 *	3  Restore title and icon (which we don't have)
 */
	void
mch_restore_title(which)
	int which;
{
	mch_settitle((which & 1) ? OrigTitle : NULL, NULL);
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return non-zero for success.
 */
int             vim_dirname(buf, len)
    char           *buf;
    int             len;
{
    return (_getcwd(buf, len) != NULL);
}

/*
 * get absolute filename into buffer 'buf' of length 'len' bytes
 */
int             FullName(fname, buf, len)
    char           *fname,
                   *buf;
    int             len;
{
    if (fname == NULL)          /* always fail */
        return FAIL;

    if (_fullpath(buf, fname, len) == NULL) {
        strncpy(buf, fname, len);       /* failed, use the relative path name */
        return FAIL;
    }
    return OK;
}

/*
 * return TRUE is fname is an absolute path name
 */
	int
isFullName(fname)
	char_u		*fname;
{
	return (STRCHR(fname, ':') != NULL);
}

/*
 * get file permissions for 'name'
 * -1 : error
 * else FA_attributes defined in dos.h
 */
long            getperm(name)
    char           *name;
{
    int             r;
    struct stat     sb;

    r = _stat(name, &sb);       /* get file mode */

    if (r)
        return r;
    else
        return sb.st_mode;
}

/*
 * set file permission for 'name' to 'perm'
 */
int             setperm(name, perm)
    char           *name;
    long            perm;
{
    return _chmod(name, (int) perm);
}

/*
 * check if "name" is a directory
 */
int             isdir(name)
    char           *name;
{
    int             f;

    f = getperm(name);
    if (f == -1)
        return -1;              /* file does not exist at all */
    if ((f & _S_IFDIR) == 0)
        return FAIL;               /* not a directory */
    return OK;
}

/*
 * Careful: mch_windexit() may be called before mch_windinit()!
 */
void            mch_windexit(r)
    int             r;
{
    settmode(0);
    stoptermcap();
    flushbuf();
	ml_close_all(); 				/* remove all memfiles */
	mch_restore_title(3);
    exit(r);
}

/*
 * function for ctrl-break interrupt
 */
BOOL WINAPI     handler_routine(DWORD dwCtrlType)
{
    cbrk_pressed = TRUE;
    ctrlc_pressed = TRUE;
}

/*
 * set the tty in (raw) ? "raw" : "cooked" mode
 *
 */

void            mch_settmode(raw)
    int             raw;
{
    long            cmodein;
    long            cmodeout;
    long            mask;

    GetConsoleMode(hConIn, &cmodein);
    GetConsoleMode(hConOut, &cmodeout);

    if (raw) {
        if (term_console)
            outstr(T_TP);       /* set colors */

        cmodein &= ~(ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
                     ENABLE_ECHO_INPUT);
        cmodein |= ENABLE_WINDOW_INPUT;

        SetConsoleMode(hConIn, cmodein);

        cmodeout &= ~(ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
        SetConsoleMode(hConOut, cmodeout);
        SetConsoleCtrlHandler(handler_routine, TRUE);
    } else {

        if (term_console)
            normvideo();        /* restore screen colors */

        cmodein |= (ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT |
                    ENABLE_ECHO_INPUT);
        cmodein &= ~(ENABLE_WINDOW_INPUT);

        SetConsoleMode(hConIn, cmodein);

        cmodeout |= (ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

        SetConsoleMode(hConOut, cmodeout);

        SetConsoleCtrlHandler(handler_routine, FALSE);
    }
}

int             mch_get_winsize()
{
    int             i;
/*
 * Use the console mode API
 */
    if (GetConsoleScreenBufferInfo(hConOut, &csbi)) {
        Rows = csbi.dwSize.Y;
        Columns = csbi.dwSize.X;
        DefaultAttribute = csbi.wAttributes;
    } else {
        Rows = 25;
        Columns = 80;
    }

    if (Columns < 5 || Columns > MAX_COLUMNS ||
        Rows < 2 || Rows > MAX_COLUMNS) {
        /* these values are overwritten by termcap size or default */
        Columns = 80;
        Rows = 25;
        return OK;
    }
    /* Rows_max = Rows;            /* remember physical max height */

    check_winsize();
    /*script_winsize();*/

    return OK;
}

/*********************************************************************
* FUNCTION: perr(PCHAR szFileName, int line, PCHAR szApiName,        *
*                DWORD dwError)                                      *
*                                                                    *
* PURPOSE: report API errors. Allocate a new console buffer, display *
*          error number and error text, restore previous console     *
*          buffer                                                    *
*                                                                    *
* INPUT: current source file name, current line number, name of the  *
*        API that failed, and the error number                       *
*                                                                    *
* RETURNS: none                                                      *
*********************************************************************/

/* maximum size of the buffer to be returned from FormatMessage */
#define MAX_MSG_BUF_SIZE 512

void            perr(PCHAR szFileName, int line, PCHAR szApiName, DWORD dwError)
{
    CHAR            szTemp[1024];
    DWORD           cMsgLen;
    CHAR           *msgBuf;     /* buffer for message text from system */
    int             iButtonPressed;     /* receives button pressed in the
                                         * error box */

    /* format our error message */
    sprintf(szTemp, "%s: Error %d from %s on line %d:\n", szFileName,
            dwError, szApiName, line);
    /* get the text description for that error number from the system */
    cMsgLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                         FORMAT_MESSAGE_ALLOCATE_BUFFER | 40, NULL, dwError,
     MAKELANGID(0, SUBLANG_ENGLISH_US), (LPTSTR) & msgBuf, MAX_MSG_BUF_SIZE,
                            NULL);
    if (!cMsgLen)
        sprintf(szTemp + strlen(szTemp), "Unable to obtain error message text! \n"
                "%s: Error %d from %s on line %d", __FILE__,
                GetLastError(), "FormatMessage", __LINE__);
    else
        strcat(szTemp, msgBuf);
    strcat(szTemp, "\n\nContinue execution?");
    MessageBeep(MB_ICONEXCLAMATION);
    iButtonPressed = MessageBox(NULL, szTemp, "Console API Error",
                          MB_ICONEXCLAMATION | MB_YESNO | MB_SETFOREGROUND);
    /* free the message buffer returned to us by the system */
    if (cMsgLen)
        LocalFree((HLOCAL) msgBuf);
    if (iButtonPressed == IDNO)
        exit(1);
    return;
}
#define PERR(bSuccess, api) {if (!(bSuccess)) perr(__FILE__, __LINE__, \
    api, GetLastError());}


void            resizeConBufAndWindow(HANDLE hConsole, SHORT xSize, SHORT ySize)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;    /* hold current console buffer info */
    BOOL            bSuccess;
    SMALL_RECT      srWindowRect;       /* hold the new console size */
    COORD           coordScreen;

    bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
    PERR(bSuccess, "GetConsoleScreenBufferInfo");
    /* get the largest size we can size the console window to */
    coordScreen = GetLargestConsoleWindowSize(hConsole);
    PERR(coordScreen.X | coordScreen.Y, "GetLargestConsoleWindowSize");
    /* define the new console window size and scroll position */
    srWindowRect.Right = (SHORT) (min(xSize, coordScreen.X) - 1);
    srWindowRect.Bottom = (SHORT) (min(ySize, coordScreen.Y) - 1);
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
    /* define the new console buffer size */
    coordScreen.X = xSize;
    coordScreen.Y = ySize;
    /* if the current buffer is larger than what we want, resize the */
    /* console window first, then the buffer */
    if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y > (DWORD) xSize * ySize) {
        bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
        PERR(bSuccess, "SetConsoleWindowInfo");
        bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
        PERR(bSuccess, "SetConsoleScreenBufferSize");
    }
    /* if the current buffer is smaller than what we want, resize the */
    /* buffer first, then the console window */
    if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y < (DWORD) xSize * ySize) {
        bSuccess = SetConsoleScreenBufferSize(hConsole, coordScreen);
        PERR(bSuccess, "SetConsoleScreenBufferSize");
        bSuccess = SetConsoleWindowInfo(hConsole, TRUE, &srWindowRect);
        PERR(bSuccess, "SetConsoleWindowInfo");
    }
    /* if the current buffer *is* the size we want, don't do anything! */
    return;
}

void            mch_set_winsize()
{
    resizeConBufAndWindow(hConOut, Columns, Rows);
}

int             call_shell(cmd, filter, cooked)
    char           *cmd;
    int             filter;     /* if != 0: called by dofilter() */
    int             cooked;
{
    int             x;
    char            newcmd[200];

    flushbuf();

    if (cooked)
        settmode(0);            /* set to cooked mode */

    if (cmd == NULL)
        x = system(p_sh);
    else {                      /* we use "command" to start the shell, slow
                                 * but easy */
        sprintf(newcmd, "%s /c %s", p_sh, cmd);
        x = system(newcmd);
    }
    outchar('\n');
    if (cooked)
        settmode(1);            /* set to raw mode */

#ifdef WEBB_COMPLETE
	if (x && !expand_interactively)
#else
    if (x)
#endif
	{
        smsg("%d returned", x);
        outchar('\n');
    }
    resettitle();
    return x;
}

#define FL_CHUNK 32

static void     addfile(fl, f, isdir)
    FileList       *fl;
    char           *f;
    int             isdir;
{
    char           *p;

    if (!fl->file) {
        fl->file = (char **) alloc(sizeof(char *) * FL_CHUNK);
        if (!fl->file)
            return;
        fl->nfiles = 0;
        fl->maxfiles = FL_CHUNK;
    }
    if (fl->nfiles >= fl->maxfiles) {
        char          **t;
        int             i;

        t = (char **) lalloc(sizeof(char *) * (fl->maxfiles + FL_CHUNK), TRUE);
        if (!t)
            return;
        for (i = fl->nfiles - 1; i >= 0; i--)
            t[i] = fl->file[i];
        free(fl->file);
        fl->file = t;
        fl->maxfiles += FL_CHUNK;
    }
    p = alloc((unsigned) (strlen(f) + 1 + isdir));
    if (p) {
        strcpy(p, f);
        if (isdir)
            strcat(p, "/");
    }
    fl->file[fl->nfiles++] = p;
}

static int      pstrcmp(a, b)
    char          **a,
                  **b;
{
    return (strcmp(*a, *b));
}

int             has_wildcard(s)
    char           *s;
{
    if (s)
        for (; *s; ++s)
            if (*s == '?' || *s == '*')
                return 1;
    return 0;
}

static void     strlowcpy(d, s)
    char           *d,
                   *s;
{
    while (*s)
        *d++ = tolower(*s++);
    *d = '\0';
}

static int      expandpath(fl, path, fonly, donly, notf)
    FileList       *fl;
    char           *path;
    int             fonly,
                    donly,
                    notf;
{
    char            buf[MAX_PATH];
    char           *p,
                   *s,
                   *e;
    int             lastn,
                    c = 1,
                    r;
    WIN32_FIND_DATA fb;
    HANDLE          hFind;

    lastn = fl->nfiles;

/*
 * Find the first part in the path name that contains a wildcard.
 * Copy it into buf, including the preceding characters.
 */
    p = buf;
    s = NULL;
    e = NULL;
    while (*path) {
        if (*path == '\\' || *path == ':' || *path == '/') {
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

    /* now we have one wildcard component between s and e */
    *e = '\0';
    r = 0;
    /* If we are expanding wildcards we try both files and directories */
    if ((hFind = FindFirstFile(buf, &fb)) == INVALID_HANDLE_VALUE) {
        /* not found */
        strcpy(e, path);
        if (notf)
            addfile(fl, buf, FALSE);
        return 1;               /* unexpanded or empty */
    }
    while (c) {
        strlowcpy(s, fb.cFileName);
        if (*s != '.' || (s[1] != '\0' && (s[1] != '.' || s[2] != '\0'))) {
            strcat(buf, path);
            if (!has_wildcard(path))
                addfile(fl, buf, (isdir(buf) > 0));
            else
                r |= expandpath(fl, buf, fonly, donly, notf);
        }
        c = FindNextFile(hFind, &fb);
    }
    qsort(fl->file + lastn, fl->nfiles - lastn, sizeof(char *), pstrcmp);
    FindClose(hFind);
    return r;
}

/*
 * MSDOS rebuilt of Scott Ballantynes ExpandWildCard for amiga/arp.
 * jw
 */

int             ExpandWildCards(num_pat, pat, num_file, file, files_only, list_notfound)
    int             num_pat;
    char          **pat;
    int            *num_file;
    char         ***file;
    int             files_only,
                    list_notfound;
{
    int             i,
                    r = 0;
    FileList        f;

    f.file = NULL;
    f.nfiles = 0;
    for (i = 0; i < num_pat; i++) {
        if (!has_wildcard(pat[i]))
            addfile(&f, pat[i], files_only ? FALSE : (isdir(pat[i]) > 0));
        else
            r |= expandpath(&f, pat[i], files_only, 0, list_notfound);
    }
    if (r == 0) {
        *num_file = f.nfiles;
        *file = f.file;
    } else {
        *num_file = 0;
        *file = NULL;
    }
    return (r ? FAIL : OK);
}

void            FreeWild(num, file)
    int             num;
    char          **file;
{
    if (file == NULL || num <= 0)
        return;
    while (num--)
        free(file[num]);
    free(file);
}

/*
 * The normal chdir() does not change the default drive.
 * This one does.
 */
#undef chdir
int             vim_chdir(path)
    char           *path;
{
    if (path[0] == NUL)         /* just checking... */
        return FAIL;
    if (path[1] == ':') {       /* has a drive name */
        if (_chdrive(toupper(path[0]) - 'A' + 1))
            return -1;          /* invalid drive name */
        path += 2;
    }
    if (*path == NUL)           /* drive name only */
        return OK;
    return _chdir(path);        /* let the normal chdir() do the rest */
}

clrscr()
{
    int             count;

    ntcoord.X = 0;
    ntcoord.Y = 0;
    FillConsoleOutputCharacter(hConOut, ' ', Columns * Rows,
                               ntcoord, &count);
    FillConsoleOutputAttribute(hConOut, DefaultAttribute, Rows * Columns,
                               ntcoord, &count);
}

clreol()
{
    int             count;
    FillConsoleOutputCharacter(hConOut, ' ',
                               Columns - ntcoord.X,
                               ntcoord, &count);
    FillConsoleOutputAttribute(hConOut, DefaultAttribute,
                               Columns - ntcoord.X,
                               ntcoord, &count);
}

insline(int count)
{
    SMALL_RECT      source,
                    clip;
    COORD           dest;
    CHAR_INFO       fill;

    dest.X = 0;
    dest.Y = ntcoord.Y + count;

    source.Left = 0;
    source.Top = ntcoord.Y;
    source.Right = Columns;
    source.Bottom = Rows - 1;

    fill.Char.AsciiChar = ' ';
    fill.Attributes = DefaultAttribute;

    ScrollConsoleScreenBuffer(hConOut, &source, (PSMALL_RECT) 0, dest,
                              &fill);
}

delline(int count)
{
    SMALL_RECT      source,
                    clip;
    COORD           dest;
    CHAR_INFO       fill;

    dest.X = 0;
    dest.Y = ntcoord.Y;

    source.Left = 0;
    source.Top = ntcoord.Y + count;
    source.Right = Columns;
    source.Bottom = Rows - 1;

    /* get current attributes and fill out CHAR_INFO structure for fill char */
    fill.Char.AsciiChar = ' ';
    fill.Attributes = DefaultAttribute;

    ScrollConsoleScreenBuffer(hConOut, &source, (PSMALL_RECT) 0, dest,
                              &fill);
}


scroll()
{
    SMALL_RECT      source,
                    clip;
    COORD           dest;
    CHAR_INFO       fill;

    dest.X = 0;
    dest.Y = 0;

    source.Left = 0;
    source.Top = 1;
    source.Right = Columns;
    source.Bottom = Rows - 1;

    /* get current attributes and fill out CHAR_INFO structure for fill char */
    fill.Char.AsciiChar = ' ';
    fill.Attributes = DefaultAttribute;

    ScrollConsoleScreenBuffer(hConOut, &source, (PSMALL_RECT) 0, dest,
                              &fill);
}

gotoxy(x, y)
    register int    x,
                    y;
{
    ntcoord.X = x - 1;
    ntcoord.Y = y - 1;
    SetConsoleCursorPosition(hConOut, ntcoord);
}

normvideo()
{
    int             count;
    WORD            attr = DefaultAttribute;

    SetConsoleTextAttribute(hConOut, attr);
}

textattr(int attr)
{
    int             count;
    WORD            attrw = attr;

    SetConsoleTextAttribute(hConOut, attr);
}

putch(char c)
{
    int             count;

    WriteConsole(hConOut, &c, 1, &count, 0);
    ntcoord.X += count;
}

delay(x)
{
    Sleep(x);
}

sleep(x)
{
    Sleep(x * 1000);
}

vbell()
{
    COORD           origin = {0, 0};
    WORD            flash = ~DefaultAttribute & 0xff;
    WORD            off = DefaultAttribute;

    int             count;
    LPWORD          oldattrs = alloc(Rows * Columns * sizeof(WORD));

    ReadConsoleOutputAttribute(hConOut, oldattrs, Rows * Columns, origin,
                                &count);
    FillConsoleOutputAttribute(hConOut, flash, Rows * Columns, origin,
                               &count);
    WriteConsoleOutputAttribute(hConOut, oldattrs, Rows * Columns, origin,
                                &count);
    free(oldattrs);
}

cursor_visible(int visible)
{
    CONSOLE_CURSOR_INFO cci;

    cci.bVisible = visible ? TRUE : FALSE;
    cci.dwSize = 100;           /* 100 percent cursor */
    SetConsoleCursorInfo(hConOut, &cci);
}

void            set_window(void)
{
}

/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
void            breakcheck()
{
    if (ctrlc_pressed) {
        ctrlc_pressed = FALSE;
        got_int = TRUE;
    }
}

	long
mch_avail_mem(special)
	int special;
{
	return 0x7fffffff;		/* virual memory eh */
}

/*
 * return non-zero if a character is available
 */
	int
mch_char_avail()
{
	return WaitForChar(0);
}

/*
 * set screen mode, always fails.
 */
	int
mch_screenmode(arg)
	char_u	 *arg;
{
	EMSG("Screen mode setting not supported");
	return FAIL;
}
