/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * dosinst.h: Common code for dosinst.c and uninstal.c
 */

#define BUFSIZE 512		/* long enough to hold a file name path */
#define NUL 0

#define FAIL 0
#define OK 1

#define VIM_VERSION_STR   VIM_VERSION_MAJOR_STR "." VIM_VERSION_MINOR_STR
#define VIM_STARTMENU "Programs\\Vim " VIM_VERSION_STR

/*
 * Call malloc() and exit when out of memory.
 */
    static void *
alloc(int len)
{
    char *s;

    s = malloc(len);
    if (s == NULL)
    {
	printf("ERROR: out of memory\n");
	exit(1);
    }
    return (void *)s;
}

#ifdef WIN3264
/* This symbol is not defined in older versions of the SDK or Visual C++ */

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

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

# ifndef __BORLANDC__
    static char *
searchpath(char *name)
{
    static char widename[2 * BUFSIZE];
    static char location[2 * BUFSIZE + 2];

    /* There appears to be a bug in FindExecutableA() on Windows NT.
     * Use FindExecutableW() instead... */
    PlatformId();
    if (g_PlatformId == VER_PLATFORM_WIN32_NT)
    {
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
		(LPWSTR)widename, BUFSIZE);
	if (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		    (LPWSTR)location) > (HINSTANCE)32)
	{
	    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		    (LPSTR)widename, 2 * BUFSIZE, NULL, NULL);
	    return widename;
	}
    }
    else
    {
	if (FindExecutableA((LPCTSTR)name, (LPCTSTR)"",
		    (LPTSTR)location) > (HINSTANCE)32)
	    return location;
    }
    return NULL;
}
# endif
#endif

/*
 * Call searchpath() and save the result in allocated memory, or return NULL.
 */
    static char *
searchpath_save(char *name)
{
    char	*p;
    char	*s;

    p = searchpath(name);
    if (p == NULL)
	return NULL;
    s = alloc(strlen(p) + 1);
    strcpy(s, p);
    return s;
}

#ifdef WIN3264
/*
 * Get the path to a requested Windows shell folder.
 *
 * Return 0 on error, non-zero on success
 */
    int
get_shell_folder_path(
	char *shell_folder_path,
	const char *shell_folder_name)
{
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.
     */
    LPITEMIDLIST pidl = 0; /* Pointer to an Item ID list allocated below */
    LPMALLOC pMalloc; /* Pointer to an IMalloc interface */
    int csidl;

    if (strcmp(shell_folder_name, "desktop") == 0)
	csidl = CSIDL_DESKTOP;
    else if (strncmp(shell_folder_name, "Programs", 8) == 0)
	csidl = CSIDL_PROGRAMS;
    else
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	return FAIL;
    }

    /* Initialize pointer to IMalloc interface */
    if (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	return FAIL;
    }

    /* Get an ITEMIDLIST corresponding to the folder code */
    if (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl))
    {
	printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	return FAIL;
    }

    /* Translate that ITEMIDLIST to a string */
    if (!SHGetPathFromIDList(pidl, shell_folder_path))
    {
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n",
							   shell_folder_name);
	pMalloc->lpVtbl->Free(pMalloc, pidl);
	pMalloc->lpVtbl->Release(pMalloc);
	return FAIL;
    }

    /* Free the data associated with pidl */
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    /* Release the IMalloc interface */
    pMalloc->lpVtbl->Release(pMalloc);

    if (strncmp(shell_folder_name, "Programs\\", 9) == 0)
	strcat(shell_folder_path, shell_folder_name + 8);

    return OK;
}
#endif

/*
 * List of targets.  The first one (index zero) is used for the default path
 * for the batch files.
 */
#define TARGET_COUNT  8

struct
{
    char	*name;		/* Vim exe name (without .exe) */
    char	*batname;	/* batch file name */
    char	*lnkname;	/* shortcut file name */
    char	*exename;	/* exe file name */
    char	*exenamearg;	/* exe file name when using exearg */
    char	*exearg;	/* argument for vim.exe or gvim.exe */
    char	*oldbat;	/* path to existing xxx.bat or NULL */
    char	*oldexe;	/* path to existing xxx.exe or NULL */
    char	batpath[BUFSIZE];  /* path of batch file to create; not
				      created when it's empty */
} targets[TARGET_COUNT] =
{
    {"all",	"batch files"},
    {"vim",	"vim.bat",	"vim.lnk",	"vim.exe",    "vim.exe",  ""},
    {"gvim",	"gvim.bat",	"gvim.lnk",	"gvim.exe",   "gvim.exe", ""},
    {"evim",	"evim.bat",	"evim.lnk",	"evim.exe",   "gvim.exe", "-y"},
    {"view",	"view.bat",	"view.lnk",	"view.exe",   "vim.exe",  "-R"},
    {"gview",	"gview.bat",	"gview.lnk",	"gview.exe",  "gvim.exe", "-R"},
    {"vimdiff", "vimdiff.bat",	"vimdiff.lnk",	"vimdiff.exe","vim.exe",  "-d"},
    {"gvimdiff","gvimdiff.bat",	"gvimdiff.lnk","gvimdiff.exe","gvim.exe", "-d"},
};

#define ICON_COUNT 3
char *(icon_names[ICON_COUNT]) =
	{"gvim " VIM_VERSION_STR,
	 "evim " VIM_VERSION_STR,
	 "gview " VIM_VERSION_STR};
char *(icon_link_names[ICON_COUNT]) =
	{"gvim " VIM_VERSION_STR ".lnk",
	 "evim " VIM_VERSION_STR ".lnk",
	 "gview " VIM_VERSION_STR ".lnk"};
