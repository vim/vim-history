/* vi:set ts=8 sts=4 sw=4:
 *
 * if_sniff.c Interface between Vim and SNiFF+
 *
 * $Id$
 */

#include "vim.h"

#ifdef WIN32
# include <windows.h>
# include <stdio.h>
# include <fcntl.h>
# include <io.h>
# include <process.h>
# include <string.h>
# include <assert.h>
#else
# ifdef USE_GUI_X11
#  include "gui_x11.pro"
# endif
#  include "os_unixx.h"
#endif

int sniffemacs_pid;
int fd_from_sniff;
int sniff_connected = 0;
int sniff_request_waiting = 0;
int want_sniff_request = 0;

#define NEED_SYMBOL	2
#define EMPTY_SYMBOL	4
#define NEED_FILE	8
#define SILENT		16
#define DISCONNECT	32
#define CONNECT		64

#define RQ_NONE		0
#define RQ_SIMPLE	1
#define RQ_CONTEXT	NEED_FILE + NEED_SYMBOL
#define RQ_SCONTEXT	NEED_FILE + NEED_SYMBOL + EMPTY_SYMBOL
#define RQ_NOSYMBOL	NEED_FILE
#define RQ_SILENT	RQ_NOSYMBOL + SILENT
#define RQ_CONNECT	RQ_NONE + CONNECT
#define RQ_DISCONNECT	RQ_SIMPLE + DISCONNECT

struct sn_cmd
{
    char *longname;
    char cmd_char;
    char *msg_txt;
    int  cmd_type;
};

static struct sn_cmd sniff_cmds[] =
{
    { "toggle",		'e', "Toggle implementation/definition",RQ_SCONTEXT },
    { "superclass",	's', "Show base class of",		RQ_CONTEXT },
    { "overridden",	'm', "Show overridden member function",	RQ_SCONTEXT },
    { "retrieve-file",	'r', "Retrieve from file",		RQ_CONTEXT },
    { "retrieve-project",'p', "Retrieve from project",		RQ_CONTEXT },
    { "retrieve-all-projects",
			'P', "Retrieve from all projects",	RQ_CONTEXT },
    { "retrieve-next",	'R', "Retrieve next symbol",		RQ_CONTEXT },
    { "goto-symbol",	'g', "Show source of",			RQ_CONTEXT },
    { "find-symbol",	'f', "Find symbol",			RQ_CONTEXT },
    { "browse-class",	'w', "Browse class",			RQ_CONTEXT },
    { "hierarchy",	't', "Show class in hierarchy",		RQ_CONTEXT },
    { "restr-hier",	'T', "Show class in restricted hierarchy",RQ_CONTEXT },
    { "xref-to",	'x', "Xref refers to",			RQ_CONTEXT },
    { "xref-by",	'X', "Xref referred by",		RQ_CONTEXT },
    { "xref-has",	'c', "Xref has a",			RQ_CONTEXT },
    { "xref-used-by",	'C', "Xref used by",			RQ_CONTEXT },
    { "show-docu",	'd', "Show docu of",			RQ_CONTEXT },
    { "gen-docu",	'D', "Generate docu for",		RQ_CONTEXT },
    { "connect",	'y', NULL,				RQ_CONNECT },
    { "disconnect",	'q', NULL,				RQ_DISCONNECT },
    { "font-info",	'z', NULL,				RQ_SILENT },
    { "update",		'u', NULL,				RQ_SILENT },
    { NULL,		'\0', NULL, 0}
};

static char *SniffEmacs[2] = {"sniffemacs", (char *)NULL};  /* Yes, Emacs! */
static int fd_to_sniff;
static int sniff_will_disconnect = 0;
static char msg_sniff_disconnect[] = "aCannot connect to SNiFF+. Check environment.\n";
/* Initializing vim commands
 * executed each time vim connects to Sniff
 */
static char *init_cmds[]= {
    "augroup sniff",
    "autocmd BufWritePost * sniff update",
    "autocmd BufReadPost * sniff font-info",
    "autocmd VimLeave * sniff disconnect",
    "augroup END",

    "let sniff_connected = 1",

    "if ! exists('sniff_mappings_sourced')|"
	"if ! exists('sniff_mappings')|"
	    "let sniff_mappings='$SNIFF_DIR/config/sniff.vim'|"
	"endif|"
	"let sniff_mappings=expand(sniff_mappings)|"
	"if file_readable(sniff_mappings)|"
	    "execute 'source' sniff_mappings|"
	    "let sniff_mappings_sourced=1|"
	"endif|"
    "endif",

    NULL
};

#define IS_SNIFF_BUF	curbuf->b_sniff

/*-------- Function Prototypes ----------------------------------*/

static int ConnectToSniffEmacs __ARGS((void));
static void sniff_connect __ARGS((void));
static void HandleSniffRequest __ARGS((char* buffer));
static int get_request __ARGS((int fd, char *buf, int maxlen));
static void WriteToSniff __ARGS((char *str));
static void SendRequest __ARGS((struct sn_cmd *command, char* symbol));
static void vi_msg __ARGS((char *));
static void vi_error_msg __ARGS((char *));
static char *vi_symbol_under_cursor __ARGS((void));
static void vi_open_file __ARGS((char *));
static char *vi_buffer_name __ARGS((void));
static BUF  *vi_find_buffer __ARGS((char *));
static void vi_exec_cmd __ARGS((char *));
static void vi_set_cursor_pos __ARGS((long char_nr));
static long vi_cursor_pos __ARGS((void));

/*-------- Windows Only Declarations -----------------------------*/
#ifdef WIN32

#define READTHREADMAX 256

int  sniff_request_processed=1;
HANDLE sniffemacs_handle;
HANDLE handle_to_sniff;
HANDLE handle_from_sniff;

struct sniffBufNode {
    struct sniffBufNode *next;
    int    bufLen;
    char   buf[READTHREADMAX];
};
struct sniffBufNode *sniffBufStart=NULL;
struct sniffBufNode *sniffBufEnd=NULL;
HANDLE hBufferMutex;

# ifdef USE_GUI_WIN32
    extern HWND s_hwnd;       /* gvim's Window handle */
# else
    extern HANDLE g_hConIn;   /* handle of Console Input */
    static struct _INPUT_RECORD sniff_ir;
    DWORD keys_written;
# endif

/*
 * some helper functions for Windows port only
 */

    static HANDLE
ExecuteDetachedProgram(char *szBinary, char *szCmdLine,
    HANDLE hStdInput, HANDLE hStdOutput)
{
    BOOL bResult;
    DWORD nError;
    PROCESS_INFORMATION aProcessInformation;
    PROCESS_INFORMATION *pProcessInformation= &aProcessInformation;
    STARTUPINFO aStartupInfo;
    STARTUPINFO *pStartupInfo= &aStartupInfo;
    DWORD dwCreationFlags= CREATE_NEW_CONSOLE;
    char szPath[512];
    HINSTANCE hResult;

    hResult = FindExecutable(szBinary, ".", szPath);
    if((int)hResult <= 32) {
	/* can't find the exe file */
	return NULL;
    }

    ZeroMemory(pStartupInfo, sizeof(*pStartupInfo));
    pStartupInfo->dwFlags= STARTF_USESHOWWINDOW;
    if(hStdInput) {
	pStartupInfo->hStdInput = hStdInput;
	pStartupInfo->dwFlags |=  STARTF_USESTDHANDLES;
    }
    if(hStdOutput) {
	pStartupInfo->hStdOutput = hStdOutput;
	pStartupInfo->dwFlags |=  STARTF_USESTDHANDLES;
    }
    pStartupInfo->wShowWindow= SW_HIDE;
    pStartupInfo->cb = sizeof(STARTUPINFO);

    bResult= CreateProcess(
	szPath,
	szCmdLine,
	NULL,    /* security attr for process */
	NULL,    /* security attr for primary thread */
	TRUE,    /* DO inherit stdin and stdout */
	dwCreationFlags, /* creation flags */
	NULL,    /* environment */
	".",    /* current directory */
	pStartupInfo,   /* startup info: NULL crashes  */
	pProcessInformation /* process information: NULL crashes */
    );
    nError= GetLastError();
    if ( bResult ) {
	CloseHandle(pProcessInformation->hThread);
	return(pProcessInformation->hProcess);
    } else {
	return(NULL);
    }
}

/*
 * write to the internal Thread / Thread communications buffer.
 * Return TRUE if successful, FALSE else.
 */
    BOOL
writeToBuffer(char *msg, int len)
{
    DWORD dwWaitResult;     /* Request ownership of mutex. */
    struct sniffBufNode *bn;
    int bnSize;

    bnSize = sizeof(struct sniffBufNode) - READTHREADMAX + len + 1;
    if(bnSize < 128) bnSize = 128; /* minimum length to avoid fragmentation */
    bn = (struct sniffBufNode *)malloc(bnSize);
    if(!bn)
	return FALSE;

    memcpy(bn->buf, msg, len);
    bn->buf[len]='\0';    /* terminate CString for added safety */
    bn->next = NULL;
    bn->bufLen = len;
    /* now, acquire a Mutex for adding the string to our linked list */
    dwWaitResult = WaitForSingleObject(
	hBufferMutex,   /* handle of mutex */
	1000L);   /* one-second time-out interval */
    if(dwWaitResult == WAIT_OBJECT_0)
    {
	/* The thread got mutex ownership. */
	if(sniffBufEnd) {
	    sniffBufEnd->next = bn;
	    sniffBufEnd = bn;
	}
	else {
	    assert(sniffBufStart == NULL);
	    sniffBufStart = sniffBufEnd = bn;
	}
	/* Release ownership of the mutex object. */
	if (! ReleaseMutex(hBufferMutex)) {
	    /* Deal with error. */
	}
	return TRUE;
    }

    /* Cannot get mutex ownership due to time-out or mutex object abandoned. */
    free(bn);
    return FALSE;
}

/*
 * read from the internal Thread / Thread communications buffer.
 * Return TRUE if successful, FALSE else.
 */
    static int
ReadFromBuffer(char *buf, int maxlen)
{
    DWORD dwWaitResult;     /* Request ownership of mutex. */
    int   theLen;
    struct sniffBufNode *bn;

    dwWaitResult = WaitForSingleObject(
	hBufferMutex,   /* handle of mutex */
	1000L);		/* one-second time-out interval */
    if(dwWaitResult == WAIT_OBJECT_0)
    {
	if(!sniffBufStart) {
	    /* all pending Requests Processed */
	    sniff_request_processed = 1;
	    theLen = 0;
	}
	else {
	    bn = sniffBufStart;
	    theLen = bn->bufLen;
	    if(theLen >= maxlen) {
		/* notify the user of buffer overflow? */
		theLen = maxlen-1;
	    }
	    memcpy(buf, bn->buf, theLen);
	    buf[theLen] = '\0';
	    if (! (sniffBufStart = bn->next)) {
		sniffBufEnd = NULL;
	    }
	    free(bn);
	}
	if (! ReleaseMutex(hBufferMutex)) {
	    /* Deal with error. */
	}
	return theLen;
    }

    /* Cannot get mutex ownership due to time-out or mutex object abandoned. */
    return -1;
}

/* on Win32, a separate Thread reads the input pipe. get_request is not needed here. */
    void __cdecl
SniffEmacsReadThread(void *dummy)
{
    static char ReadThreadBuffer[READTHREADMAX];
    int  ReadThreadLen=0;
    int  result;
    int  msgLen=0;
    char *msgStart, *msgCur;

    /* Read from the pipe to SniffEmacs */
    while(sniff_connected) {
	if (! ReadFile(handle_from_sniff,
		ReadThreadBuffer + ReadThreadLen,    /* acknowledge rest in buffer */
		READTHREADMAX - ReadThreadLen,
		&result,
		NULL))
	{
	    DWORD err = GetLastError();
	    result = -1;
	}

	if(result == 0) continue;
	if(result < 0) {
	    /* probably sniffemacs died... log the Error? */
	    sniff_disconnect(1);
	}

	ReadThreadLen += result-1;   /* total length of valid chars */
	for(msgCur=msgStart=ReadThreadBuffer; ReadThreadLen > 0; msgCur++, ReadThreadLen--)
	{
	    switch(*msgCur) {
		case '\0':
		case '\r':
		case '\n':
		    msgLen = msgCur-msgStart; /* don't add the CR/LF chars */
		    if(msgLen > 0) {
			writeToBuffer(msgStart, msgLen);
		    }
		    msgStart = msgCur + 1; /* over-read single CR/LF chars */
		    break;
	    }
	}

	/* move incomplete message to beginning of buffer */
	ReadThreadLen = msgCur - msgStart;
	assert(ReadThreadLen >=0);
	if(ReadThreadLen > 0) {
	    memmove(ReadThreadBuffer, msgStart, ReadThreadLen);
	}

	if(sniff_request_processed) {
	    /* notify others that new data has arrived */
	    sniff_request_processed = 0;
	    sniff_request_waiting = 1;
#ifdef USE_GUI_WIN32
	    PostMessage(s_hwnd, WM_USER, (WPARAM)0, (LPARAM)0);
#else
	    /* simulate an Escape key pressed */
	    sniff_ir.EventType = KEY_EVENT;
	    sniff_ir.Event.KeyEvent.bKeyDown = TRUE;
	    sniff_ir.Event.KeyEvent.wRepeatCount = 1;
	    sniff_ir.Event.KeyEvent.wVirtualKeyCode = 0;
	    sniff_ir.Event.KeyEvent.wVirtualScanCode;
	    sniff_ir.Event.KeyEvent.uChar.AsciiChar = '\33';
	    sniff_ir.Event.KeyEvent.dwControlKeyState = 0;
	    WriteConsoleInput(g_hConIn, &sniff_ir, 1, &keys_written);
#endif
	}
    }
}
#endif /* WIN32 */
/*-------- End of Windows Only Declarations ------------------------*/


/* ProcessSniffRequests
 * Function that should be called from outside
 * to process the waiting sniff requests
 */
void ProcessSniffRequests()
{
    static char buf[256];
    int len;

    while (sniff_connected)
    {
#ifdef WIN32
	len = ReadFromBuffer(buf, sizeof(buf));
#else
	len = get_request(fd_from_sniff, buf, sizeof(buf));
#endif
	if (len<0)
	{
	    vi_error_msg("Sniff: Error during read. Disconnected");
	    sniff_disconnect(1);
	    break;
	}
	else if (len>0)
	{
	    HandleSniffRequest( buf );
	}
	else
	    break;
    }

    if (sniff_will_disconnect)	/* Now the last msg   */
    {				/* has been processed */
	sniff_disconnect(1);
    }
}



/* do_sniff
 * Handle ":sniff" command
 */
    void
do_sniff(arg)
    char_u *arg;
{
    char *symbol = NULL;
    char *cmd = NULL;
    int  len_cmd = 0;
    int  i;
    int  print_cmds = FALSE;

    if (ends_excmd(*arg))
    {				/* no request: print available commands */
	print_cmds = TRUE;
	msg_start();
	msg_outtrans_attr((char_u *)"-- SNiFF+ commands --",
		highlight_attr[HLF_T]);
    }
    else	/* extract command name and symbol if present */
    {
	for (symbol = (char *)arg; isalpha(*symbol); symbol++)
	    ;
	symbol = (char *)skiptowhite(arg);
	len_cmd = (int)(symbol-(char *)arg);
	cmd  = (char *)vim_strnsave(arg, len_cmd);
	symbol = (char *)skipwhite((char_u *)symbol);
	if (ends_excmd(*symbol))
	    symbol = NULL;
    }

    for(i=0; sniff_cmds[i].longname; i++)
    {
	if (print_cmds)
	{
	    msg_putchar('\n');
	    msg_outtrans((char_u *)":sniff ");
	    msg_outtrans((char_u *)sniff_cmds[i].longname);
	}
	else if (!strcmp(cmd, sniff_cmds[i].longname))
	    break;
    }
    if (print_cmds)
    {
	msg_putchar('\n');
	msg_outtrans((char_u *)"SNiFF+ is currently ");
	if (!sniff_connected)
	    msg_outtrans((char_u *)"not ");
	msg_outtrans((char_u *)"connected");
	msg_end();
	return;
    }
    if (sniff_cmds[i].longname)
    {
	SendRequest(&sniff_cmds[i], symbol);
    }
    else
    {
	EMSG2("Unknown SNiFF+ request: %s", cmd);
    }
    vim_free(cmd);
}


    static void
sniff_connect()
{
    if (sniff_connected)
	return;
    if (ConnectToSniffEmacs())
	vi_error_msg("Error connecting to SNiFF+");
    else
    {
	int i;
	for(i=0; init_cmds[i]; i++)
	    vi_exec_cmd(init_cmds[i]);
    }
}

    void
sniff_disconnect(immediately)
    int immediately;
{
#ifdef WIN32
    /* int termstat; */
#endif

    if (!sniff_connected)
	return;
    if (immediately)
    {
	vi_exec_cmd("augroup sniff");
	vi_exec_cmd("au!");
	vi_exec_cmd("augroup END");
	vi_exec_cmd("unlet sniff_connected");
	sniff_connected = 0;
	want_sniff_request = 0;
	sniff_will_disconnect = 0;
#ifdef USE_GUI
	if (gui.in_use)
	    gui_mch_wait_for_chars(0L);
#endif
#ifdef WIN32
	CloseHandle(handle_to_sniff);
	/* This one's already closed by the dying process
	 * CloseHandle(handle_from_sniff);
	 * _cwait(&termstat, sniffemacs_pid, _WAIT_CHILD);
	 * wait maximum 1 second
	 */
	WaitForSingleObject(sniffemacs_handle, 1000L);
#else
	close(fd_to_sniff);
	close(fd_from_sniff);
	wait(NULL);
#endif
    }
    else
    {
	sniff_will_disconnect = 1;  /* We expect disconnect msg in 2 secs */
#ifdef WIN32
	_sleep(2);
#else
	sleep(2);		    /* Incoming msg could disturb edit */
#endif
    }
}


/* ConnectToSniffEmacs
 * Connect to Sniff: returns 1 on error
 */
    static int
ConnectToSniffEmacs()
{
#ifdef WIN32		/* Windows Version of the Code */
    HANDLE ToSniffEmacs[2], FromSniffEmacs[2];
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    if(! CreatePipe(&ToSniffEmacs[0], &ToSniffEmacs[1], &sa, 512))
	return 1;
    if(! CreatePipe(&FromSniffEmacs[0], &FromSniffEmacs[1], &sa, 512))
	return 1;

    sniffemacs_handle = ExecuteDetachedProgram(SniffEmacs[0], SniffEmacs[0],
	ToSniffEmacs[0], FromSniffEmacs[1]);

    if(sniffemacs_handle) {
	handle_to_sniff  = ToSniffEmacs[1];
	handle_from_sniff = FromSniffEmacs[0];
	sniff_connected = 1;
	hBufferMutex = CreateMutex(
	    NULL,			/* no security attributes */
	    FALSE,			/* initially not owned */
	    "SniffReadBufferMutex");    /* name of mutex */
	if (hBufferMutex == NULL) {
	    /* Check for error. */
	}
	_beginthread(SniffEmacsReadThread, 0, NULL);
	return 0;
    }
    else {
	/* error in spawn() */
	return 1;
    }

#else		/* UNIX Version of the Code */
    int ToSniffEmacs[2], FromSniffEmacs[2];

    pipe(ToSniffEmacs);
    pipe(FromSniffEmacs);

    /* fork */
    if ((sniffemacs_pid=fork()) == 0)
    {
	/* child */

	/* prepare communication pipes */
	close(ToSniffEmacs[1]);
	close(FromSniffEmacs[0]);

	dup2(ToSniffEmacs[0],fileno(stdin));   /* write to ToSniffEmacs[1] */
	dup2(FromSniffEmacs[1],fileno(stdout));/* read from FromSniffEmacs[0] */

	close(ToSniffEmacs[0]);
	close(FromSniffEmacs[1]);

	/* start sniffemacs */
	execvp (SniffEmacs[0], SniffEmacs);
	{
/*	    FILE *out = fdopen(FromSniffEmacs[1], "w"); */
	    sleep(1);
	    fputs(msg_sniff_disconnect, stdout);
	    fflush(stdout);
	    sleep(3);
#ifdef USE_GUI
	    if (gui.in_use)
		gui_exit(1);
#endif
	    exit(1);
	}
	return 1;
    }
    else if (sniffemacs_pid > 0)
    {
	/* parent process */
	close(ToSniffEmacs[0]);
	fd_to_sniff  = ToSniffEmacs[1];
	close(FromSniffEmacs[1]);
	fd_from_sniff = FromSniffEmacs[0];
	sniff_connected = 1;
	return 0;
    }
    else   /* error in fork() */
    {
	return 1;
    }
#endif		/* UNIX Version of the Code */
}


/* HandleSniffRequest
 * Handle one request from SNiFF+
 */
    static void
HandleSniffRequest(buffer)
    char *buffer;
{
    static int first_time=0;
    char VICommand[256];
    char command;
    char *arguments;
    char file[256], new_path[256];
    int  position, writable, tab_width;
    BUF  *buf;

    const char* SetTab     = "set tabstop=%d";
    const char* SelectBuf  = "buf %s";
    const char* DeleteBuf  = "bd %s";
    const char* UnloadBuf  = "bun %s";
    const char* GotoLine   = "%d";

    command   = buffer[0];
    arguments = &buffer[1];

    switch (command)
    {
	case 'o' :  /* visit file at char pos */
	case 'O' :  /* visit file at line number */
#if defined(USE_GUI_X11) || defined(USE_GUI_WIN32)
	    if (gui.in_use && !gui.in_focus)  /* Raise Vim Window */
	    {
# ifdef USE_GUI_WIN32
		SetActiveWindow(s_hwnd);
# else
		extern Widget vimShell;

		XSetInputFocus(gui.dpy, XtWindow(vimShell), RevertToNone,
			CurrentTime);
		XRaiseWindow(gui.dpy, XtWindow(vimShell));
# endif
	    }
#endif
	    sscanf(arguments, "%s %d %d", file, &position, &writable);
	    buf = vi_find_buffer(file);
	    setpcmark();      /* insert current pos in jump list [mark.c]*/
	    if (!buf)
		vi_open_file(file);
	    else if (buf!=curbuf)
	    {
		sprintf(VICommand, SelectBuf, file);
		vi_exec_cmd(VICommand);
	    }
	    if (command == 'o')
		vi_set_cursor_pos((long)position);
	    else
	    {
		sprintf(VICommand, GotoLine, (int)position);
		vi_exec_cmd(VICommand);
	    }
	    checkpcmark();	/* [mark.c] */
	    break;

	case 'p' :  /* path of file has changed */
	    /* when changing from shared to private WS (checkout) */
	    sscanf(arguments, "%s %s", file, new_path);
	    buf = vi_find_buffer(file);
	    if (!buf)
		return;
	    if (!buf->b_changed)    /* delete buffer only if not modified */
	    {
		sprintf(VICommand, DeleteBuf, file);
		vi_exec_cmd(VICommand);
	    }
	    vi_open_file(new_path);
	    break;

	case 'w' :  /* writability has changed */
	    /* Sniff sends request twice,
	     * but only the last one is the right one */
	    first_time = !first_time;
	    if (first_time)	/* only process second request */
		return;
	    sscanf(arguments, "%s %d", file, &writable);
	    buf = vi_find_buffer(file);
	    if (!buf)       /* file not loaded */
		return;
	    buf->b_p_ro = !writable;
	    if (buf != curbuf)
	    {
		buf->b_flags |= BF_CHECK_RO + BF_NEVERLOADED;
		if (writable && !buf->b_changed)
		{
		    sprintf(VICommand, UnloadBuf, file);
		    vi_exec_cmd(VICommand);
		}
	    }
	    else if (writable && !buf->b_changed)
	    {
		vi_exec_cmd("e");
	    }
	    break;

	case 'h' :  /* highlight info */
	    break;  /* not implemented */

	case 't' :  /* Set tab width */
	    sscanf(arguments, "%s %d", file, &tab_width);
	    sprintf(VICommand, SetTab, tab_width);
	    vi_exec_cmd(VICommand);
	    break;

	case 'A' :  /* Warning/Info msg */
	    vi_msg(arguments);
	    if (!strncmp(arguments, "Discon", 6)) /* "Disconnected ..." */
		sniff_disconnect(1);	/* unexpected disconnection */
	    break;
	case 'a' :  /* Error msg */
	    vi_error_msg(arguments);
	    if (!strncmp(arguments, "Cannot", 6)) /* "Cannot connect ..." */
		sniff_disconnect(1);
	    break;

	case '\0':
	    break;
	default :
	    sprintf(VICommand, "Unrecognized sniff request [%s]", buffer );
	    vi_error_msg(VICommand);
	    break;
    }
}


#ifndef WIN32
/* get_request
 * read string from fd up to next newline (excluding the nl),
 * returns  length of string
 *	    0 if no data available or no complete line
 *	   <0 on error
 */
    static int
get_request(fd, buf, maxlen)
    int  fd;
    char *buf;
    int  maxlen;
{
    static char inbuf[1024];
    static int pos=0, bytes=0;
    register int len;
#ifdef HAVE_SELECT
    struct timeval tval;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    tval.tv_sec  = 0;
    tval.tv_usec = 0;
#else
    struct pollfd fds;
    fds.fd = fd;
    fds.events = POLLIN;
#endif

    for(len=0; len<maxlen; len++)
    {
	if (pos>=bytes)	    /* end of buffer reached? */
	{
#ifdef HAVE_SELECT
	    if (select(fd+1, &rfds, NULL, NULL, &tval)>0)
#else
		if (poll(&fds, 1, 0)>0)
#endif
		{
		    pos = 0;
		    bytes = read(fd, inbuf, sizeof(inbuf));
		    if (bytes<=0)
			return bytes;
		}
		else
		{
		    pos = pos-len;
		    buf[0] = '\0';
		    return 0;
		}
	}
	if ((buf[len] = inbuf[pos++]) =='\n')
	    break;
    }
    buf[len] = '\0';
    return len;
}
#endif     /* WIN32 */


    static void
SendRequest(command, symbol)
    struct sn_cmd *command;
    char *symbol;
{
    int  cmd_type = command->cmd_type;
    static char cmdstr[256];
    static char msgtxt[256];
    char *buffer_name = NULL;

    if (cmd_type == RQ_CONNECT)
    {
	sniff_connect();
	return;
    }
    if (!sniff_connected && !(cmd_type & SILENT))
    {
	vi_error_msg("SNiFF+ not connected");
	return;
    }

    if (cmd_type & NEED_FILE)
    {
	if (!IS_SNIFF_BUF)
	{
	    if (!(cmd_type & SILENT))
		vi_error_msg("Not a SNiFF+ buffer");
	    return;
	}
	buffer_name = vi_buffer_name();
	if (!buffer_name)
	    return;
	if (cmd_type & NEED_SYMBOL)
	{
	    if (cmd_type & EMPTY_SYMBOL)
		symbol = " ";
	    else if (!symbol && !(symbol = vi_symbol_under_cursor()))
		return;	    /* error msg already displayed */
	}

	if (symbol)
	    sprintf(cmdstr, "%c%s %ld %s\n", command->cmd_char, buffer_name,
		    vi_cursor_pos(), symbol);
	else
	    sprintf(cmdstr, "%c%s\n", command->cmd_char, buffer_name);
    }
    else    /* simple request */
    {
	cmdstr[0] = command->cmd_char;
	cmdstr[1] = '\n';
	cmdstr[2] = '\0';
    }
    if (command->msg_txt && !(cmd_type & SILENT))
    {
	if ((cmd_type & NEED_SYMBOL) && !(cmd_type & EMPTY_SYMBOL))
	{
	    sprintf(msgtxt, "%s: %s", command->msg_txt, symbol);
	    vi_msg(msgtxt);
	}
	else
	    vi_msg(command->msg_txt);
    }
    WriteToSniff(cmdstr);
    if (cmd_type & DISCONNECT)
	sniff_disconnect(0);
}



    static void
WriteToSniff(str)
    char *str;
{
    int bytes;
#ifdef WIN32
    if (! WriteFile(handle_to_sniff, str, strlen(str), &bytes, NULL)) {
	DWORD err=GetLastError();
	bytes = -1;
    }
#else
    bytes = write(fd_to_sniff, str, strlen(str));
#endif
    if (bytes<0)
    {
	vi_msg("Sniff: Error during write. Disconnected");
	sniff_disconnect(1);
    }
}

/*-------- vim helping functions --------------------------------*/

    static void
vi_msg(str)
    char *str;
{
    if (str && *str)
	MSG((char_u *)str);
}

    static void
vi_error_msg(str)
    char *str;
{
    if (str && *str)
	EMSG((char_u *)str);
}

    static void
vi_open_file(fname)
    char *fname;
{
    ++no_wait_return;
    do_ecmd(0, (char_u *)fname, NULL, NULL, ECMD_ONE, ECMD_HIDE+ECMD_OLDBUF);
    curbuf->b_sniff = TRUE;
    --no_wait_return;					/* [ex_docmd.c] */
}

    static BUF *
vi_find_buffer(fname)
    char *fname;
{			    /* derived from buflist_findname() [buffer.c] */
    BUF		*buf;

    for (buf = firstbuf; buf != NULL; buf = buf->b_next)
	if (buf->b_sfname != NULL && fnamecmp(fname, buf->b_sfname) == 0)
	    return (buf);
    return NULL;
}


    static char *
vi_symbol_under_cursor()
{
    int    len;
    char   *symbolp;
    char   *p;
    static char sniff_symbol[256];

    len = find_ident_under_cursor((char_u **)&symbolp, FIND_IDENT);
    /* [normal.c] */
    if (len <= 0)
	return NULL;
    for(p=sniff_symbol; len; len--)
	*p++ = *symbolp++;
    *p = '\0';
    return sniff_symbol;
}


    static char *
vi_buffer_name()
{
    return (char *)curbuf->b_sfname;
}

    static void
vi_exec_cmd(vicmd)
    char *vicmd;
{
    do_cmdline((char_u *)vicmd, NULL, NULL, DOCMD_NOWAIT);  /* [ex_docmd.c] */
}

/*
 * Set cursor on character position
 * derived from cursor_pos_info() [buffer.c]
 */
    static void
vi_set_cursor_pos(char_pos)
    long char_pos;
{
    linenr_t	lnum;
    long	char_count = 1;  /* first position = 1 */
    int		line_size;
    int		eol_size;

    if (curbuf->b_p_tx)
	eol_size = 2;
    else
	eol_size = 1;
    for (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
    {
	line_size = STRLEN(ml_get(lnum)) + eol_size;
	if (char_count+line_size > char_pos) break;
	char_count += line_size;
    }
    curwin->w_cursor.lnum = lnum;
    curwin->w_cursor.col  = char_pos - char_count;
}

    static long
vi_cursor_pos()
{
    linenr_t	lnum;
    long	char_count=1;  /* sniff starts with pos 1 */
    int		line_size;
    int		eol_size;

    if (curbuf->b_p_tx)
	eol_size = 2;
    else
	eol_size = 1;
    for (lnum = 1; lnum < curwin->w_cursor.lnum; ++lnum)
    {
	line_size = STRLEN(ml_get(lnum)) + eol_size;
	char_count += line_size;
    }
    return char_count + curwin->w_cursor.col;
}
