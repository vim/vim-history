/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Windows GUI: main program (EXE) entry point:
 *
 * Ron Aaron <ron@mossbayeng.com> wrote this and  the DLL support code.
 */
#include "vim.h"

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifdef __MINGW32__
# ifndef _cdecl
#  define _cdecl
# endif
#endif

/* cproto doesn't create a prototype for main() */
int _cdecl
#if defined(FEAT_GUI_W32)
VimMain
#else
    main
#endif
	__ARGS((int argc, char **argv));
int (_cdecl *pmain)(int, char **);

#ifndef PROTO
#ifdef FEAT_GUI
#ifndef VIMDLL
void _cdecl SaveInst(HINSTANCE hInst);
#endif
void (_cdecl *pSaveInst)(HINSTANCE);
#endif

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
#ifdef VIMDLL
    HANDLE	hLib;
#endif

    /*
     * Ron: added full path name so that the $VIM variable will get set to our
     * startup path (so the .vimrc file can be found w/o a VIM env. var.)
     * Remove the ".exe" extension, and find the 1st non-space.
     */
    GetModuleFileName(NULL, prog, 255);
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

    // ASSERT(i == argc);

    argv[argc] = (char *) NULL;    /* NULL-terminated list */

#ifdef DYNAMIC_GETTEXT
    /* Initialize gettext library */
    dyn_libintl_init(NULL);
#endif

#ifdef VIMDLL
	// LoadLibrary - get name of dll to load in here:
	p = strrchr(prog, '\\');
	if (p != NULL)
	{
#ifdef DEBUG
		strcpy(p+1, "vim32d.dll");
#else
		strcpy(p+1, "vim32.dll");
#endif
	}
	hLib = LoadLibrary(prog);
	if (hLib == NULL)
	{
		MessageBox(0, _("Could not load vim32.dll!"),_("VIM Error"),0);
		goto errout;
	}
	// fix up the function pointers
#ifdef FEAT_GUI
	pSaveInst = GetProcAddress(hLib, (LPCSTR)2);
#endif
	pmain = GetProcAddress(hLib, (LPCSTR)1);
	if (pmain == NULL)
	{
		MessageBox(0, _("Could not fix up function pointers to the DLL!"),_("VIM Error"),0);
		goto errout;
	}
#else
#ifdef FEAT_GUI
	pSaveInst = SaveInst;
#endif
	pmain =
#if defined(FEAT_GUI_W32)
	    //&& defined(__MINGW32__)
	    VimMain
#else
	    main
#endif
	    ;
#endif
#ifdef FEAT_GUI
	pSaveInst(
#ifdef __MINGW32__
		GetModuleHandle(NULL)
#else
		hInstance
#endif
		);
#endif
	pmain (argc, argv);

#ifdef VIMDLL
	FreeLibrary(hLib);
errout:
#endif
	free(argv);
	free(pszNewCmdLine);

	return 0;
}
#endif
