/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * os_win16.c
 *
 * Win16 (Windows 3.1x) system-dependent routines.
 * Carved brutally from os_win32.c by Vince Negri <vn@aslnet.co.uk>
 */
#ifdef __BORLANDC__
# pragma warn -par
# pragma warn -ucp
# pragma warn -use
# pragma warn -aus
# pragma warn -obs
#endif

#include <io.h>
#include "vim.h"

#include <fcntl.h>
#include <dos.h>
#include <string.h>
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
#include <direct.h>
#include <shellapi.h>	/* required for FindExecutable() */


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
# define _cdecl
#endif

#ifdef __BORLANDC__
/* being a more ANSI compliant compiler, BorlandC doesn't define _stricoll:
 * but it does in BC 5.02! */
# if __BORLANDC__ < 0x502
int _stricoll(char *a, char *b);
# endif
#endif

/* cproto doesn't create a prototype for main() */
int _cdecl
VimMain
__ARGS((int argc, char **argv));
int (_cdecl *pmain)(int, char **);

#ifndef PROTO
void _cdecl SaveInst(HINSTANCE hInst);
void (_cdecl *pSaveInst)(HINSTANCE);

int WINAPI
WinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst,
    LPSTR	lpszCmdLine,
    int		nCmdShow)
{
    int		argc;
    char	**argv;

    int		i;
    char	*pch;
    char	*pszNewCmdLine;
    char	prog[256];
    char	*p;
    int		fIsQuote;

    /*
     * Ron: added full path name so that the $VIM variable will get set to our
     * startup path (so the .vimrc file can be found w/o a VIM env. var.)
     * Remove the ".exe" extension, and find the 1st non-space.
     */
    GetModuleFileName(hInstance, prog, 255);
    if (*prog != NUL)
	exe_name = FullName_save((char_u *)prog, FALSE);

    p = strrchr(prog, '.');
    if (p != NULL)
	*p = '\0';
    for (p = prog; *p != '\0' && *p == ' '; ++p)
	;

    /*
     * Add the size of the string, two quotes, the separating space, and a
     * terminating '\0'.
     */
    pszNewCmdLine = (char *)malloc(STRLEN(lpszCmdLine) + STRLEN(prog) + 4);
    if (pszNewCmdLine == NULL)
	return 0;

    /* put double quotes around the prog name, it could contain spaces */
    pszNewCmdLine[0] = '"';
    STRCPY(pszNewCmdLine + 1, p);
    STRCAT(pszNewCmdLine, "\" ");
    STRCAT(pszNewCmdLine, lpszCmdLine);

    /*
     * Isolate each argument and put it in argv[].
     */
    pch = pszNewCmdLine;
    argc = 0;
    while ( *pch != '\0' )
    {
	/* Ron: Handle quoted strings in args list */
	fIsQuote = (*pch == '\"');
	if (fIsQuote)
	    ++pch;

	argc++;			    /* this is an argument */
	if (fIsQuote)
	{
	    while (*pch != '\0' && *pch != '\"')
		pch++;		    /* advance until a closing quote */
	    if (*pch)
		pch++;
	}
	else
	{
	    while ((*pch != '\0') && (*pch != ' '))
		pch++;		    /* advance until a space */
	}
	while (*pch && *pch == ' ' )
	    pch++;		    /* advance until a non-space */
    }

    argv = (char**) malloc((argc+1) * sizeof(char*));
    if (argv == NULL )
	return 0;		   /* malloc error */

    i = 0;
    pch = pszNewCmdLine;

    while ((i < argc) && (*pch != '\0'))
    {
	fIsQuote = (*pch == '\"');
	if (fIsQuote)
	    ++pch;

	argv[i++] = pch;
	if (fIsQuote)
	{
	    while (*pch != '\0' && *pch != '\"')
		pch++;		    /* advance until the closing quote */
	}
	else
	{
	    while (*pch != '\0' && *pch != ' ')
		pch++;		    /* advance until a space */
	}
	if (*pch != '\0')
	    *(pch++) = '\0';	    /* parse argument here */
	while (*pch && *pch == ' ')
	    pch++;		    /* advance until a non-space */
    }

    argv[argc] = (char *) NULL;    /* NULL-terminated list */

    pSaveInst = SaveInst;
    pmain = VimMain ;
    pSaveInst(
	    hInstance
	    );
    pmain (argc, argv);

    free(argv);
    free(pszNewCmdLine);

    return 0;
}
#endif






#ifdef FEAT_MOUSE

/*
 * For the GUI the mouse handling is in gui_w32.c.
 */
    void
mch_setmouse(
    int on)
{
}
#endif /* FEAT_MOUSE */



/*
 * GUI version of mch_init().
 */
    void
mch_init()
{
    extern int _fmode;


    /* Let critical errors result in a failure, not in a dialog box.  Required
     * for the timestamp test to work on removed floppies. */
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		/* we do our own CR-LF translation */

    /* Specify window size.  Is there a place to get the default from? */
    Rows = 25;
    Columns = 80;


    set_option_value((char_u *)"grepprg", 0, (char_u *)"grep -n", 0);

#ifdef FEAT_CLIPBOARD
    clip_init(TRUE);

    /*
     * Vim's own clipboard format recognises whether the text is char, line, or
     * rectangular block.  Only useful for copying between two Vims.
     */
    clip_star.format = RegisterClipboardFormat("VimClipboard");
#endif
}



/*
 * Do we have an interactive window?
 */
    int
mch_check_win(
    int argc,
    char **argv)
{
    int		i;

    return OK;	    /* GUI always has a tty */
}


/*
 * Turn a file name into its canonical form.  Replace slashes with backslashes.
 * This used to replace backslashes with slashes, but that caused problems
 * when using the file name as a command.  We can't have a mix of slashes and
 * backslashes, because comparing file names will not work correctly.  The
 * commands that use file names should be prepared to handle the backslashes.
 */
    static void
canonicalize_filename(
    char *pszName)
{
    if (pszName == NULL)
	return;

    for ( ; *pszName;  pszName++)
    {
	if (*pszName == '/')
	    *pszName = '\\';
    }
}



/*
 * Insert user name in s[len].
 */
    int
mch_get_user_name(
    char_u	*s,
    int		len)
{
    *s = NUL;
    return FAIL;
}

/*
 * Insert host name is s[len].
 */
    void
mch_get_host_name(
    char_u	*s,
    int		len)
{
    STRNCPY(s, "PC (16 bits Vim)", len);
}


/*
 * return process ID
 */
    long
mch_get_pid()
{
    return (long)GetCurrentTask();
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
    return (getcwd(buf, len) != NULL ? OK : FAIL);
}


/*
 * get file permissions for 'name'
 * -1 : error
 * else FA_attributes defined in dos.h
 */
    long
mch_getperm(char_u *name)
{
    return (long)_chmod((char *)name, 0, 0);	 /* get file mode */
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    int
mch_setperm(
    char_u	*name,
    long	perm)
{
    perm |= FA_ARCH;	    /* file has changed, set archive bit */
    return (_chmod((char *)name, 1, (int)perm) == -1 ? FAIL : OK);
}


/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name)
{
    /* DOS 6.2 share.exe causes "seek error on file write" errors when making
     * the swap file hidden.  Thus don't do it. */
}

/*
 * return TRUE if "name" is a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 *
 * beware of a trailing backslash
 */
    int
mch_isdir(char_u *name)
{
    int	    f;
    char_u  *p;

    p = name + strlen((char *)name);
    if (p > name)
	--p;
    if (*p == '\\')		    /* remove trailing backslash for a moment */
	*p = NUL;
    else
	p = NULL;
    f = _chmod((char *)name, 0, 0);
    if (p != NULL)
	*p = '\\';		    /* put back backslash */
    if (f == -1)
	return FALSE;		    /* file does not exist at all */
    if ((f & FA_DIREC) == 0)
	return FALSE;		    /* not a directory */
    return TRUE;
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Return 1 if "name" can be executed, 0 if not.
 * Return -1 if unknown.
 */
    int
mch_can_exe(char_u *name)
{
    return (searchpath(name) != NULL);
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
    if (STRICMP(name, "AUX") == 0
	    || STRICMP(name, "CON") == 0
	    || STRICMP(name, "CLOCK$") == 0
	    || STRICMP(name, "NUL") == 0
	    || STRICMP(name, "PRN") == 0
	    || ((STRNICMP(name, "COM", 3) == 0
		    || STRNICMP(name, "LPT", 3) == 0)
		&& isdigit(name[3])
		&& name[4] == NUL))
	return NODE_WRITABLE;
    /* TODO: NODE_OTHER? */
    return NODE_NORMAL;
}

/*
 * Specialised version of system().
 * This version proceeds as follows:
 *    1. Start the program with WinExec
 *    2. Wait for the module use count of the program to go to 0
 *	 (This is the best way of detecting the program has finished)
 */

    static int
mch_system(char *cmd, int options)
{
    DWORD		ret = 0;
    UINT		wShowWindow;
    UINT		h_module;
    MSG			msg;
    BOOL		again = TRUE;

    /*
     * It's nicer to run a filter command in a minimized window, but in
     */
    if (options & SHELL_DOOUT)
	wShowWindow = SW_SHOWMINIMIZED;
    else
	wShowWindow = SW_SHOWNORMAL;

    /* Now, run the command */
    h_module = WinExec((LPCSTR)cmd, wShowWindow);

    if (h_module < 32)
    {
	/*error*/
	ret = -h_module;
    }
    else
    {
	/* Wait for the command to terminate before continuing */
	while (GetModuleUsage(h_module) > 0 && again )
        {
             while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) && again )
             {
                if(msg.message == WM_QUIT)

                {
                   PostQuitMessage(msg.wParam);
                   again = FALSE;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
             }
        }
    }

    return ret;
}

/*
 * Either execute a command by calling the shell or start a new shell
 */
    int
mch_call_shell(
    char_u *cmd,
    int options)	    /* SHELL_, see vim.h */
{
    int	    x;

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
		STRLEN(p_sh) + STRLEN(p_shcf) + STRLEN(cmd) + 10, TRUE);
	if (newcmd != NULL)
	{
	    if (STRNICMP(cmd, "start ", 6) == 0)
	    {
		sprintf((char *)newcmd, "%s\0", cmd+6);
		if (WinExec((LPCSTR)newcmd, SW_SHOWNORMAL) > 31)
		    x = 0;
		else
		    x = -1;
	    }
	    else
	    {
		sprintf((char *)newcmd, "%s%s %s %s",
			"",
			p_sh,
			p_shcf,
			cmd);
		x = mch_system((char *)newcmd, options);
	    }
	    vim_free(newcmd);
	}
    }

    settmode(TMODE_RAW);	    /* set to raw mode */

    if (x && !(options & SHELL_SILENT) && !emsg_silent)
    {
	smsg(_("shell returned %d"), x);
	msg_putchar('\n');
    }
#ifdef FEAT_TITLE
    resettitle();
#endif

    signal(SIGINT, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGABRT, SIG_DFL);


    return x;
}


/*
 * Delay for half a second.
 */
    void
mch_delay(
    long    msec,
    int	    ignoreinput)
{
#ifdef MUST_FIX
    Sleep((int)msec);	    /* never wait for input */
#endif
}


/*
 * this version of remove is not scared by a readonly (backup) file
 */
    int
mch_remove(char_u *name)
{
    (void)mch_setperm(name, 0);    /* default permissions */
    return unlink((char *)name);
}


/*
 * check for an "interrupt signal": CTRL-break or CTRL-C
 */
    void
mch_breakcheck()
{
    /* never used */
}


/*
 * How much memory is available?
 */
    long_u
mch_avail_mem(
    int special)
{
    return GetFreeSpace(0);
}


/*
 * Like rename(), returns 0 upon success, non-zero upon failure.
 * Should probably set errno appropriately when errors occur.
 */
    int
mch_rename(
    const char	*pszOldFile,
    const char	*pszNewFile)
{

    /*
     * No need to play tricks, this isn't rubbish like Windows 95 <g>
     */
    return rename(pszOldFile, pszNewFile);

}

/*
 * Special version of getenv(): use $HOME when $VIM not defined.
 */
    char_u *
mch_getenv(char_u *var)
{
    char_u  *retval;
    char_u  *var_copy, *p;

    /*
     * Take a copy of the argument, and force it to upper case before passing
     * to getenv().  On DOS systems, getenv() doesn't like lower-case argument
     * (unlike Win32 et al.)
     */
    var_copy = vim_strsave(var);
    if (var_copy == NULL)
	return NULL;
    for (p = var_copy; *p != NUL; p++)
	*p = toupper(*p);

    retval = (char_u *)getenv((char *)var_copy);

    if (retval == NULL && STRCMP(var_copy, "VIM") == 0)
	retval = (char_u *)getenv("HOME");

    vim_free(var_copy);

    return retval;
}


/*
 * Get the default shell for the current hardware platform
 */
    char*
default_shell()
{
    char* psz = NULL;

    psz = "command.com";

    return psz;
}
