/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
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
 * GUI version of mch_shellinit().
 */
    void
mch_shellinit()
{
    extern int _fmode;


    /* Let critical errors result in a failure, not in a dialog box.  Required
     * for the timestamp test to work on removed floppies. */
    SetErrorMode(SEM_FAILCRITICALERRORS);

    _fmode = O_BINARY;		/* we do our own CR-LF translation */

    /* Specify window size.  Is there a place to get the default from? */
    Rows = 25;
    Columns = 80;


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

    if (x && !(options & SHELL_SILENT))
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


    static void
namelowcpy(
    char_u *d,
    char_u *s)
{
	while (*s)
	    *d++ = TO_LOWER(*s++);
    *d = NUL;
}


/*
 * Recursive function to expand one path section with wildcards.
 * This only expands '?' and '*'.
 * Return the number of matches found.
 * "path" has backslashes before chars that are not to be expanded, starting
 * at "wildc".
 */
    static int
dos_expandpath(
    garray_t		*gap,
    char_u		*path,
    char_u		*wildc,
    int			flags)
{
    char_u		*buf;
    char_u		*p, *s, *e;
    int			start_len, c;
    struct ffblk	fb;
    int			matches;
    int			len;
    int			dot_at_start;

    start_len = gap->ga_len;
    buf = alloc_clear(STRLEN(path) + BASENAMELEN + 5);   /* make room for file name */
    if (buf == NULL)
	return 0;
    /*
     * Find the first part in the path name that contains a wildcard.
     * Copy it into buf, including the preceding characters.
     */
    p = buf;
    s = NULL;
    e = NULL;
    while (*path)
    {
	if (path >= wildc && rem_backslash(path))	/* remove a backslash */
	    ++path;
	else if (*path == '\\' || *path == ':' || *path == '/')
	{
	    if (e != NULL)
		break;
	    else
		s = p + 1;
	}
	else if (*path == '*' || *path == '?')
	    e = p;
	*p++ = *path++;
    }
    e = p;
    if (s == NULL)
	s = buf;

    /* if the file name ends in "*" and does not contain a ".", addd ".*" */
    if (e[-1] == '*' && vim_strchr(s, '.') == NULL)
    {
	*e++ = '.';
	*e++ = '*';
    }
    /* now we have one wildcard component between s and e */
    *e = NUL;
    dot_at_start = (*s == '.');

    /* If we are expanding wildcards we try both files and directories */
    if ((c = findfirst((char *)buf, &fb,
			    (*path || (flags & EW_DIR)) ? FA_DIREC : 0)) != 0)
    {
	/* not found */
	vim_free(buf);
	return 0; /* unexpanded or empty */
    }

    while (!c)
    {
	namelowcpy((char *)s, fb.ff_name);
	/* ignore entries starting with "." unless when asked for */
	if (*s != '.' || dot_at_start)
	{
	    len = STRLEN(buf);
	    STRCPY(buf + len, path);
	    if (mch_has_wildcard(path))
	    {
		/* need to expand another component of the path */
		/* remove backslashes for the remaining components only */
		(void)dos_expandpath(gap, buf, buf + len + 1, flags);
	    }
	    else
	    {
		/* no more wildcards, check if there is a match */
		/* remove backslashes for the remaining components only */
		backslash_halve(buf + len + 1);
		if (mch_getperm(buf) >= 0)	/* add existing file */
		    addfile(gap, buf, flags);
	    }
	}
	c = findnext(&fb);
    }
    vim_free(buf);

    matches = gap->ga_len - start_len;
    if (matches)
	qsort(((char_u **)gap->ga_data) + start_len, (size_t)matches,
						   sizeof(char_u *), pstrcmp);
    return matches;
}


    int
mch_expandpath(
    garray_t		*gap,
    char_u		*path,
    int			flags)
{
    return dos_expandpath(gap, path, path, flags);
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



#ifdef FEAT_EVAL
/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 * Return OK if it worked, FAIL if not.
 */
typedef LPSTR (*MYSTRPROCSTR)(LPSTR);
typedef LPSTR (*MYINTPROCSTR)(int);
typedef int (*MYSTRPROCINT)(LPSTR);
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


#ifdef FEAT_CLIPBOARD
/*
 * Clipboard stuff, for cutting and pasting text to other windows.
 */

/*
 * Get the current selection and put it in the clipboard register.
 */
    void
clip_mch_request_selection()
{
    int	    type;
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
	}
	/* Otherwise, check for the normal text format */
	else if ((hMem = GetClipboardData(CF_TEXT)) != NULL)
	{
	    str = (char_u *)GlobalLock(hMem);
	    if (str != NULL)
		type = (vim_strchr((char*) str, '\r') != NULL) ? MLINE : MCHAR;
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


