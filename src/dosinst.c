/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * install.c: Install program for Vim on MS-DOS and Windows
 *
 * Compile with Make_mvc.mak, Make_bc3.mak, Make_bc5.mak or Make_djg.mak.
 */

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef __CYGWIN__
# include <direct.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#if defined(_WIN64) || defined(WIN32)
# define WIN3264
# include <windows.h>
#else
# include <dir.h>
# include <bios.h>
# include <dos.h>
#endif

#ifdef DJGPP
# include <unistd.h>
# include <errno.h>
#endif

/* shlobj.h is needed for shortcut creation */
#ifdef WIN3264
# include <shlobj.h>
#endif

#ifdef DJGPP
# define vim_mkdir(x, y) mkdir((char *)(x), y)
#else
# ifdef WIN3264
#  define vim_mkdir(x, y) _mkdir((char *)(x))
# else
#  define vim_mkdir(x, y) mkdir((char *)(x))
# endif
#endif
/* ---------------------------------------- */

/* Macro to do an error check I was typing over and over */
#define CHECK_REG_ERROR(code) if( code != ERROR_SUCCESS ) { printf("%d error number:  %d\n", __LINE__, code); return; }

#include "version.h"

#define BUFSIZE 512		/* long enough to hold a file name path */
#define NUL 0

/* Numbers for batch choices since we use them in a few places. */
enum
{
    install_batfile_vim = 0,
    install_batfile_gvim,
    install_batfile_evim,
    install_batfile_view,
    install_batfile_gview,
    install_batfile_vimdiff,
    install_batfile_gvimdiff
};

char	installdir[BUFSIZE];	/* top of the installation dir, where the
				   install.exe is located, E.g.:
				   "c:\vim\vim60" */
int	runtimeidx;		/* index in installdir[] where "vim60" starts */
int	has_vim = 0;		/* installable vim.exe exists */
int	has_gvim = 0;		/* installable gvim.exe exists */
int	has_evim = 0;		/* installable evim.exe exists */
int	has_view = 0;		/* installable view.exe exists */
int	has_gview = 0;		/* installable gview.exe exists */
int	has_vimdiff = 0;	/* installable vimdiff.exe exists */
int	has_gvimdiff = 0;	/* installable gvimdiff.exe exists */

char	*sysdrive;		/* system drive or "c:\" */

char	*oldvimbat;		/* path to vim.bat or NULL */
char	*oldgvimbat;		/* path to gvim.bat or NULL */
char	*oldevimbat;		/* path to evim.bat or NULL */
char	*oldviewbat;		/* path to view.bat or NULL */
char	*oldgviewbat;		/* path to gview.bat or NULL */
char	*oldvimdiffbat;		/* path to vimdiff.bat or NULL */
char	*oldgvimdiffbat;	/* path to gvimdiff.bat or NULL */

char	*oldvimexe;		/* path to vim.exe or NULL */
char	*oldgvimexe;		/* path to gvim.exe or NULL */
char	*oldevimexe;		/* path to evim.exe or NULL */
char	*oldviewexe;		/* path to view.exe or NULL */
char	*oldgviewexe;		/* path to gview.exe or NULL */
char	*oldvimdiffexe;		/* path to vimdiff.exe or NULL */
char	*oldgvimdiffexe;	/* path to gvimdiff.exe or NULL */

char	vimbat[BUFSIZE];	/* Name of Vim batch file to write.  Not
				   installed when it's empty. */
char	gvimbat[BUFSIZE];	/* Name of GVim batch file to write.  Not
				   installed when it's empty. */
char	evimbat[BUFSIZE];	/* Name of EVim batch file to write.  Not
				   installed when it's empty. */
char	viewbat[BUFSIZE];	/* Name of View batch file to write.  Not
				   installed when it's empty. */
char	gviewbat[BUFSIZE];	/* Name of GView batch file to write.  Not
				   installed when it's empty. */
char	vimdiffbat[BUFSIZE];	/* Name of GVimdiff batch file to write.  Not
				   installed when it's empty. */
char	gvimdiffbat[BUFSIZE];	/* Name of GVimdiff batch file to write.  Not
				   installed when it's empty. */

char	oldvimrc[BUFSIZE];	/* name of existing vimrc file */
char	vimrc[BUFSIZE];		/* name of vimrc file to create */

/*
 * Structure used for each choice the user can make.
 */
struct choice
{
    int	    active;			/* non-zero when choice is active */
    char    *text;			/* text displayed for this choice */
    void    (*changefunc)(int idx);	/* function to change this choice */
    int	    arg;			/* argument for function */
    void    (*installfunc)(int idx);	/* function to install this choice */
};

struct choice	choices[30];		/* choices the user can make */
int		choice_count = 0;	/* number of choices available */

#define TABLE_SIZE(s)	sizeof(s) / sizeof(char *)

enum
{
    compat_vi = 1,
    compat_some_enhancements,
    compat_all_enhancements
};
char	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "with some Vim ehancements",
    "with syntax highlighting and other features switched on",
};
int     compat_choice_default = compat_all_enhancements;
int	compat_choice;
char	*compat_text = "- run Vim %s";

enum
{
    remap_no = 1,
    remap_win
};
char	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (<C-V>, <C-C>, etc)",
};
int     remap_choice_default = remap_win;
int	remap_choice;
char	*remap_text = "- %s";

enum
{
    mouse_xterm = 1,
    mouse_mswin
};
char	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu (the Windows way)",
};
int     mouse_choice_default = mouse_mswin;
int	mouse_choice;
char	*mouse_text = "- The mouse %s";

enum
{
    shortcuts_none = 1,
    shortcuts_desktop,
    shortcuts_start,
    shortcuts_both
};
char    *(shortcut_location_choices[]) =
{
    "\nChoose where and whether to create shortcuts to Vim:",
    "no shortcuts",
    "shortcut(s) only on the desktop",
    "shortcut(s) only in the Start Menu",
    "shortcuts both on the desktop and in the Start Menu",
};
int     shortcut_location_choice_default = shortcuts_none;
int     shortcut_location_choice;
char    *shortcut_location_text = "Create %s";

/* the next enumeration applies to all shortcut creation */
enum
{
    shortcut_no_create = 0,
    shortcut_create
};
char    *(vim_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int     vim_shortcut_choice_default = shortcut_create;
int     vim_shortcut_choice;
char    *vim_shortcut_text = "    %s a shortcut to vim";

char    *(gvim_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int     gvim_shortcut_choice_default = shortcut_create;
int     gvim_shortcut_choice;
char    *gvim_shortcut_text = "    %s a shortcut to gvim";

char    *(evim_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int     evim_shortcut_choice_default = shortcut_create;
int     evim_shortcut_choice;
char    *evim_shortcut_text = "    %s a shortcut to evim";

char    *(view_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int view_shortcut_choice_default = shortcut_create;
int view_shortcut_choice;
char *view_shortcut_text = "    %s a shortcut to view";

char    *(gview_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int gview_shortcut_choice_default = shortcut_create;
int gview_shortcut_choice;
char *gview_shortcut_text = "    %s a shortcut to gview";

char    *(vimdiff_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int vimdiff_shortcut_choice_default = shortcut_create;
int vimdiff_shortcut_choice;
char *vimdiff_shortcut_text = "    %s a shortcut to vimdiff";

char    *(gvimdiff_shortcut_choices[]) =
{
    "Do not create",
    "Create",
};
int gvimdiff_shortcut_choice_default = shortcut_create;
int gvimdiff_shortcut_choice;
char *gvimdiff_shortcut_text = "    %s a shortcut to gvimdiff";

enum
{
    vimfiles_dir_vim = 1,
    vimfiles_dir_home,
    vimfiles_dir_none
};
char    *(vimfiles_dir_choices[]) =
{
    "\nCreate Vim plugin directories:",
    "in the VIM directory.",
    "in your HOME directory.",
    "nowhere. (Do NOT create a Vim plugins directory.)",
};
int     vimfiles_dir_choice_default = vimfiles_dir_vim;
int     vimfiles_dir_choice;
char    *vimfiles_dir_text = "Create a Vim plugins directory %s";

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
char	*vimfiles_dir_name = "vimfiles";
char	*(vimfiles_subdirs[]) =
{
    "plugin",
    "ftplugin",
    "doc",
    "colors",
    "indent",
    "syntax",
};

/*
 * The toupper() in Bcc 5.5 doesn't work, use our own implementation.
 */
    static int
mytoupper(int c)
{
    if (c >= 'a' && c <= 'z')
	return c - 'a' + 'A';
    return c;
}

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

/*
 * Copy a directory name from "dir" to "buf", doubling backslashes.
 * Also make sure it ends in a double backslash.
 */
    static void
double_bs(char *dir, char *buf)
{
    char *d = buf;
    char *s;

    for (s = dir; *s; ++s)
    {
	if (*s == '\\')
	    *d++ = '\\';
	*d++ = *s;
    }
    /* when dir is not empty, it must end in a double backslash */
    if (d > buf && d[-1] != '\\')
    {
	*d++ = '\\';
	*d++ = '\\';
    }
    *d = NUL;
}

/*
 * Obtain a choice from a table.
 * First entry is a question, others are choices.
 */
    static int
get_choice(char **table, int entries)
{
    int		answer;
    int		idx;
    char	dummy[100];

    do
    {
	for (idx = 0; idx < entries; ++idx)
	{
	    if (idx)
		printf("%2d  ", idx);
	    printf(table[idx]);
	    printf("\n");
	}
	printf("Choice: ");
	if (scanf("%d", &answer) != 1)
	{
	    scanf("%99s", dummy);
	    answer = 0;
	}
    }
    while (answer < 1 || answer >= entries);

    return answer;
}

/*
 * Append a backslash to "name" if there isn't one yet.
 */
    static void
add_pathsep(char *name)
{
    int		len = strlen(name);

    if (len == 0)
	return;
    if (name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
    int
change_drive(int drive)
{
#ifdef WIN3264
    char temp[3] = "-:";
    temp[0] = (char)(drive + 'A' - 1);
    return !SetCurrentDirectory(temp);
#else
    union REGS regs;

    regs.h.ah = 0x0e;
    regs.h.dl = drive - 1;
    intdos(&regs, &regs);   /* set default drive */
    regs.h.ah = 0x19;
    intdos(&regs, &regs);   /* get default drive */
    if (regs.h.al == drive - 1)
	return 0;
    return -1;
#endif
}

/*
 * Change directory to "path".
 * Return 0 for success, -1 for failure.
 */
    int
mch_chdir(char *path)
{
    if (path[0] == NUL)		/* just checking... */
	return 0;
    if (path[1] == ':')		/* has a drive name */
    {
	if (change_drive(mytoupper(path[0]) - 'A' + 1))
	    return -1;		/* invalid drive name */
	path += 2;
    }
    if (*path == NUL)		/* drive name only */
	return 0;
    return chdir(path);		/* let the normal chdir() do the rest */
}

/*
 * Expand the executable name into a full path name.
 */
#if defined(__BORLANDC__) && !defined(WIN3264)

/* Only Borland C++ has this. */
# define my_fullpath(b, n, l) _fullpath(b, n, l)

#else
    static char *
my_fullpath(char *buf, char *fname, int len)
{
# ifdef WIN3264
    /* Only GetModuleFileName() will get the long file name path.
     * GetFullPathName() may still use the short (FAT) name. */
    DWORD len_read = GetModuleFileName(NULL, buf, len);

    return (len_read > 0 && len_read < (DWORD)len) ? buf : NULL;
# else
    char	olddir[BUFSIZE];
    char	*p, *q;
    int		c;
    char	*retval = buf;

    if (strchr(fname, ':') != NULL)	/* allready expanded */
    {
	strncpy(buf, fname, len);
    }
    else
    {
	*buf = NUL;
	/*
	 * change to the directory for a moment,
	 * and then do the getwd() (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	p = strrchr(fname, '/');
	q = strrchr(fname, '\\');
	if (q != NULL && (p == NULL || q > p))
	    p = q;
	q = strrchr(fname, ':');
	if (q != NULL && (p == NULL || q > p))
	    p = q;
	if (p != NULL)
	{
	    if (getcwd(olddir, BUFSIZE) == NULL)
	    {
		p = NULL;		/* can't get current dir: don't chdir */
		retval = NULL;
	    }
	    else
	    {
		if (p == fname)		/* /fname		*/
		    q = p + 1;		/* -> /			*/
		else if (q + 1 == p)	/* ... c:\foo		*/
		    q = p + 1;		/* -> c:\		*/
		else			/* but c:\foo\bar	*/
		    q = p;		/* -> c:\foo		*/

		c = *q;			/* truncate at start of fname */
		*q = NUL;
		if (mch_chdir(fname))	/* change to the directory */
		    retval = NULL;
		else
		{
		    fname = q;
		    if (c == '\\')	/* if we cut the name at a */
			fname++;	/* '\', don't add it again */
		}
		*q = c;
	    }
	}
	if (getcwd(buf, len) == NULL)
	{
	    retval = NULL;
	    *buf = NUL;
	}
	/*
	 * Concatenate the file name to the path.
	 */
	if (strlen(buf) + strlen(fname) >= len - 1)
	{
	    printf("ERROR: File name too long!\n");
	    exit(1);
	}
	add_pathsep(buf);
	strcat(buf, fname);
	if (p)
	    mch_chdir(olddir);
    }

    /* Replace forward slashes with backslashes, required for the path to a
     * command. */
    while ((p = strchr(buf, '/')) != NULL)
	*p = '\\';

    return retval;
# endif
}
#endif

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

/*
 * Setup for using this program.
 * Sets "installdir[]".
 */
    static void
do_inits(char **argv)
{
    int		i;

#ifdef DJGPP
    /*
     * Use Long File Names by default, if $LFN not set.
     */
    if (getenv("LFN") == NULL)
	putenv("LFN=y");
#endif

    /* Find out the full path of our executable. */
    if (my_fullpath(installdir, argv[0], BUFSIZE) == NULL)
    {
	printf("ERROR: Cannot get name of executable\n");
	exit(1);
    }
    /* remove the tail, the executable name "install.exe" */
    for (i = strlen(installdir) - 1; i > 0; --i)
	if (installdir[i] == '/' || installdir[i] == '\\')
	{
	    installdir[i] = NUL;
	    break;
	}

    /* change to the installdir */
    mch_chdir(installdir);

    /* Find the system drive.  Only used for searching the Vim executable, not
     * very important. */
    sysdrive = getenv("SYSTEMDRIVE");
    if (sysdrive == NULL || *sysdrive == NUL)
	sysdrive = "C:\\";
}

/*
 * Check if the user unpacked the archives properly.
 * Sets "runtimeidx".
 */
    static void
check_unpack(void)
{
    int		len;
    char	buf[BUFSIZE];
    FILE	*fd;
    struct stat	st;

    /* check for presence of the correct version number in installdir[] */
    len = strlen(VIM_VERSION_NODOT);
    runtimeidx = strlen(installdir) - len;
    if (runtimeidx <= 0
	    || stricmp(installdir + runtimeidx, VIM_VERSION_NODOT) != 0
	    || (installdir[runtimeidx - 1] != '/'
		&& installdir[runtimeidx - 1] != '\\'))
    {
	printf("ERROR: Install program not in directory \"%s\"\n",
		VIM_VERSION_NODOT);
	printf("This program can only work when it is located in its original directory\n");
	exit(1);
    }

    /* check if filetype.vim is present, which means the runtime archive has
     * been unpacked  */
    sprintf(buf, "%s\\filetype.vim", installdir);
    if (stat(buf, &st) < 0)
    {
	printf("ERROR: Cannot find filetype.vim in \"%s\"\n", installdir);
	printf("It looks like you did not unpack the runtime archive.\n");
	printf("You must unpack the runtime archive \"vim%srt.zip\" before installing.\n",
		VIM_VERSION_NODOT + 3);
	exit(1);
    }

    /* Check if vim.exe or gvim.exe is in the current directory. */
    /* Also now, check for evim.exe, view.exe, vimdiff.exe       */
    if ((fd = fopen("gvim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_gvim = 1;
    }
    if ((fd = fopen("vim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_vim = 1;
    }
    if ((fd = fopen("evim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_evim = 1;
    }
    if ((fd = fopen("view.exe", "r")) != NULL)
    {
	fclose(fd);
	has_view = 1;
    }
    if ((fd = fopen("gview.exe", "r")) != NULL)
    {
	fclose(fd);
	has_gview = 1;
    }
    if ((fd = fopen("vimdiff.exe", "r")) != NULL)
    {
	fclose(fd);
	has_vimdiff = 1;
    }
    if ((fd = fopen("gvimdiff.exe", "r")) != NULL)
    {
	fclose(fd);
	has_gvimdiff = 1;
    }
    if (!has_gvim && !has_vim && !has_evim && !has_view && !has_gview && !has_vimdiff && !has_gvimdiff)
    {
	printf("ERROR: Cannot find any Vim executables in \"%s\"\n\n", installdir);
	exit(1);
    }
}

/*
 * Compare paths "p[plen]" to "q[qlen]".  Return 0 if they match.
 */
    static int
pathcmp(char *p, int plen, char *q, int qlen)
{
    int		i;

    for (i = 0; ; ++i)
    {
	/* End of "p": check if "q" also ends or just has a slash. */
	if (i == plen)
	{
	    if (i == qlen)  /* match */
		return 0;
	    if (i == qlen - 1 && (q[i] == '\\' || q[i] == '/'))
		return 0;   /* match with trailing slash */
	    return 1;	    /* no match */
	}

	/* End of "q": check if "p" also ends or just has a slash. */
	if (i == qlen)
	{
	    if (i == plen)  /* match */
		return 0;
	    if (i == plen - 1 && (p[i] == '\\' || p[i] == '/'))
		return 0;   /* match with trailing slash */
	    return 1;	    /* no match */
	}

	if (!(mytoupper(p[i]) == mytoupper(q[i])
		|| ((p[i] == '/' || p[i] == '\\')
		    && (q[i] == '/' || q[i] == '\\'))))
	    return 1;	    /* no match */
    }
    /*NOTREACHED*/
}

/*
 * On input **destination is the path of an executable.
 * If that executable is in the current directory, look for another one.
 * *destination is set to NULL or the location of that file.
 */

    static void
findoldfile(char **destination)
{
    char	*bp = *destination;
    size_t	indir_l = strlen(installdir);
    char	*cp = bp + indir_l;
    char	*tmpname;
    char	*farname;

    /*
     * No action needed if exe not found or not in this directory.
     */
    if (bp == NULL
	    || strnicmp(bp, installdir, indir_l) != 0
	    || strchr("/\\", *cp++) == NULL
	    || strchr(cp, '\\') != NULL
	    || strchr(cp, '/') != NULL)
	return;

    tmpname = alloc(strlen(cp) + 1);
    strcpy(tmpname, cp);
    tmpname[strlen(tmpname) - 1] = 'x';	/* .exe -> .exx */

    if (access(tmpname, 0) == 0)
    {
	printf("\nERROR: %s and %s clash. remove/move so only %s exists\n",
	    tmpname, cp, cp);
	exit(1);
    }

    if (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	exit(1);
    }

    farname = searchpath_save(cp);

    if (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	exit(1);
    }

    free(*destination);
    free(tmpname);
    *destination = farname;
}

/*
 * Find out information about the system.
 */
    static void
inspect_system(void)
{
    char	*p;
    char	buf[BUFSIZE];
    FILE	*fd;

    /*
     * If $VIM is set, check that it's pointing to our directory.
     */
    p = getenv("VIM");
    if (p != NULL && pathcmp(p, strlen(p), installdir, runtimeidx - 1) != 0)
    {
	printf("$VIM is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	strcpy(buf, installdir);
	*(buf + runtimeidx - 1) = NUL;
	printf("\"%s\"\n", buf);
	printf("Please manually adjust the setting of $VIM.\n");
	exit(1);
    }

    /*
     * If $VIMRUNTIME is set, check that it's pointing to our runtime directory.
     */
    p = getenv("VIMRUNTIME");
    if (p != NULL && pathcmp(p, strlen(p), installdir, strlen(installdir)) != 0)
    {
	printf("$VIMRUNTIME is set to \"%s\".\n", p);
	printf("This is different from where this version of Vim is:\n");
	printf("\"%s\"\n", installdir);
	printf("Please manually adjust the setting of $VIMRUNTIME or remove it.\n");
	exit(1);
    }

    /*
     * Check if there is a vim or gvim in the path.
     */
    mch_chdir(sysdrive);	/* avoid looking in the "installdir" */
    oldvimbat = searchpath_save("vim.bat");
    oldgvimbat = searchpath_save("gvim.bat");
    oldevimbat = searchpath_save("evim.bat");
    oldviewbat = searchpath_save("view.bat");
    oldgviewbat = searchpath_save("gview.bat");
    oldvimdiffbat = searchpath_save("vimdiff.bat");
    oldgvimdiffbat = searchpath_save("gvimdiff.bat");

    oldvimexe = searchpath_save("vim.exe");
    oldgvimexe = searchpath_save("gvim.exe");
    oldevimexe = searchpath_save("evim.exe");
    oldviewexe = searchpath_save("view.exe");
    oldgviewexe = searchpath_save("gview.exe");
    oldvimdiffexe = searchpath_save("vimdiff.exe");
    oldgvimdiffexe = searchpath_save("gvimdiff.exe");

    mch_chdir(installdir);

    /*
     * The technique used above to set oldvimexe and oldgvimexe
     * gives a spurious result for Windows 2000 Professional.
     * w.briscoe@ponl.com 2001-01-20
     */
    findoldfile(&oldvimexe);
    findoldfile(&oldgvimexe);
    findoldfile(&oldevimexe);
    findoldfile(&oldviewexe);
    findoldfile(&oldgviewexe);
    findoldfile(&oldvimdiffexe);
    findoldfile(&oldgvimdiffexe);

    if (oldvimexe != NULL || oldgvimexe != NULL || oldevimexe != NULL || oldviewexe != NULL || oldgviewexe != NULL || oldvimdiffexe != NULL || oldgvimdiffexe != NULL)
    {
	printf("Warning: Found a Vim executable in your $PATH:\n");
	if (oldvimexe != NULL)
	    printf("%s\n", oldvimexe);
	if (oldgvimexe != NULL)
	    printf("%s\n", oldgvimexe);
	if (oldevimexe != NULL)
	    printf("%s\n", oldevimexe);
	if (oldviewexe != NULL)
	    printf("%s\n", oldviewexe);
	if (oldgviewexe != NULL)
	    printf("%s\n", oldgviewexe);
	if (oldvimdiffexe != NULL)
	    printf("%s\n", oldvimdiffexe);
	if (oldgvimdiffexe != NULL)
	    printf("%s\n", oldgvimdiffexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please delete or rename it, or adjust your $PATH setting.");
    }

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    strcpy(oldvimrc, installdir);
    strcpy(oldvimrc + runtimeidx, "_vimrc");
    if ((fd = fopen(oldvimrc, "r")) == NULL)
    {
	strcpy(oldvimrc + runtimeidx, "vimrc~1"); /* short version of .vimrc */
	if ((fd = fopen(oldvimrc, "r")) == NULL)
	{
	    strcpy(oldvimrc + runtimeidx, ".vimrc");
	    fd = fopen(oldvimrc, "r");
	}
    }
    if (fd != NULL)
	fclose(fd);
    else
	*oldvimrc = NUL;
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    static void
toggle_bat_choice(int idx)
{
    char	*batname;
    char	*oldname;

    switch (choices[idx].arg)
    {
	case install_batfile_vim:
	{
	    batname = vimbat;
	    oldname = oldvimbat;
	    break;
	}
	case install_batfile_gvim:
	{
	    batname = gvimbat;
	    oldname = oldgvimbat;
	    break;
	}
	case install_batfile_evim:
	{
	    batname = evimbat;
	    oldname = oldevimbat;
	    break;
	}
	case install_batfile_view:
	{
	    batname = viewbat;
	    oldname = oldviewbat;
	    break;
	}
	case install_batfile_gview:
	{
	    batname = gviewbat;
	    oldname = oldgviewbat;
	    break;
	}
	case install_batfile_vimdiff:
	{
	    batname = vimdiffbat;
	    oldname = oldvimdiffbat;
	    break;
	}
	case install_batfile_gvimdiff:
	{
	    batname = gvimdiffbat;
	    oldname = oldgvimdiffbat;
	    break;
	}
	default:  /* Should not ever reach here */
	{
	    printf("ERROR: toggle_bat_choice got an invalid value of %d in choices[%d].arg", choices[idx].arg, idx);
	    return;
	}
    }

    free(choices[idx].text);
    choices[idx].text = alloc(strlen(oldname) + 20);
    if (*batname == NUL)
    {
	sprintf(choices[idx].text, "Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    else
    {
	sprintf(choices[idx].text, "Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Select a directory to write the batch file line.
 */
    static void
change_bat_choice(int idx)
{
    char	*path;
    char	*batname = NULL;
    char	*name = NULL;
    int		n;
    char	*s;
    char	*p;
    int		count;
    char	**names = NULL;
    int		i;

    switch (choices[idx].arg)
    {
	case install_batfile_vim:
	{
	    batname = vimbat;
	    name = "vim.bat";
	    break;
	}
	case install_batfile_gvim:
	{
	    batname = gvimbat;
	    name = "gvim.bat";
	    break;
	}
	case install_batfile_evim:
	{
	    batname = evimbat;
	    name = "evim.bat";
	    break;
	}
	case install_batfile_view:
	{
	    batname = viewbat;
	    name = "view.bat";
	    break;
	}
	case install_batfile_gview:
	{
	    batname = gviewbat;
	    name = "gview.bat";
	    break;
	}
	case install_batfile_vimdiff:
	{
	    batname = vimdiffbat;
	    name = "vimdiff.bat";
	    break;
	}
	case install_batfile_gvimdiff:
	{
	    batname = gvimdiffbat;
	    name = "gvimdiff.bat";
	    break;
	}
    }

    path = getenv("PATH");
    if (path == NULL)
    {
	printf("\nERROR: The variable $PATH is not set\n");
	return;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to names[].
     */
    for (;;)
    {
	count = 1;
	for (p = path; *p; )
	{
	    s = strchr(p, ';');
	    if (s == NULL)
		s = p + strlen(p);
	    if (names != NULL)
	    {
		names[count] = alloc(s - p + 1);
		strncpy(names[count], p, s - p);
		names[count][s - p] = NUL;
	    }
	    ++count;
	    p = s;
	    if (*p != NUL)
		++p;
	}
	if (names != NULL)
	    break;
	names = alloc((count + 1) * sizeof(char *));
    }
    names[0] = alloc(50);
    sprintf(names[0], "Select directory to create %s in:", name);
    names[count] = alloc(50);
    sprintf(names[count], "Do not create a %s file.", name);
    n = get_choice(names, count + 1);

    free(choices[idx].text);
    if (n == count)
    {
	*batname = NUL;
	choices[idx].text = alloc(30);
	sprintf(choices[idx].text, "Do NOT create a %s", name);
    }
    else
    {
	strcpy(batname, names[n]);
	add_pathsep(batname);
	strcat(batname, name);
	choices[idx].text = alloc(strlen(batname) + 20);
	sprintf(choices[idx].text, "Create %s", batname);
    }

    for (i = 1; i < count; ++i)
	free(names[i]);
    free(names);
}

/*
 * Install the vim.bat or gvim.bat file.
 */
    static void
install_bat_choice(int idx)
{
    char	*batname = NULL;
    char	*oldname = NULL;
    char	*exename = NULL;
    FILE	*fd;
    char	buf[BUFSIZE];

    switch (choices[idx].arg)
    {
	case install_batfile_vim:
	{
	    batname = vimbat;
	    oldname = oldvimbat;
	    exename = "vim.exe";
	    break;
	}
	case install_batfile_gvim:
	{
	    batname = gvimbat;
	    oldname = oldgvimbat;
	    exename = "gvim.exe";
	    break;
	}
	case install_batfile_evim:
	{
	    batname = evimbat;
	    oldname = oldevimbat;
	    exename = "evim.exe";
	    break;
	}
	case install_batfile_view:
	{
	    batname = viewbat;
	    oldname = oldviewbat;
	    exename = "view.exe";
	    break;
	}
	case install_batfile_gview:
	{
	    batname = gviewbat;
	    oldname = oldgviewbat;
	    exename = "gview.exe";
	    break;
	}
	case install_batfile_vimdiff:
	{
	    batname = vimdiffbat;
	    oldname = oldvimdiffbat;
	    exename = "vimdiff.exe";
	    break;
	}
	case install_batfile_gvimdiff:
	{
	    batname = gvimdiffbat;
	    oldname = oldgvimdiffbat;
	    exename = "gvimdiff.exe";
	    break;
	}
    }

    if (*batname != NUL)
    {
	fd = fopen(batname, "w");
	if (fd == NULL)
	    printf("\nERROR: Cannot open \"%s\" for writing.\n", batname);
	else
	{
	    fprintf(fd, "@echo off\n");
	    fprintf(fd, "rem -- Run Vim --\n\n");
	    fprintf(fd, "rem first collect the arguments in VIMARGS\n");
	    fprintf(fd, "set VIMARGS=\n");
	    fprintf(fd, ":loopstart\n");
	    fprintf(fd, "if .%%1==. goto loopend\n");
	    fprintf(fd, "set VIMARGS=%%VIMARGS%% %%1\n");
	    fprintf(fd, "shift\n");
	    fprintf(fd, "goto loopstart\n");
	    fprintf(fd, ":loopend\n\n");

	    strcpy(buf, installdir);
	    buf[runtimeidx - 1] = NUL;
	    /* Don't use double quotes for the value here, also when buf
	     * contains a space.  The quotes would be included in the value
	     * for MSDOS. */
	    fprintf(fd, "set VIM=%s\n", buf);

	    strcpy(buf, installdir + runtimeidx);
	    add_pathsep(buf);
	    strcat(buf, exename);
	    /* Do use quotes here if the path includes a space. */
	    if (strchr(installdir, ' ') != NULL)
		fprintf(fd, "\"%%VIM%%\\%s\" %%VIMARGS%%\n", buf);
	    else
		fprintf(fd, "%%VIM%%\\%s %%VIMARGS%%\n", buf);
	    fprintf(fd, "set VIMARGS=\n");

	    fclose(fd);
	    printf("%s has been %s\n", batname,
				 oldname == NULL ? "created" : "overwritten");
	}
    }
}

/*
 * Initialize a choice for vim.bat or gvim.bat.
 */
    static void
init_bat_choice(char *name, char *oldname, char *batname, int gvim)
{
    char	*p;
    int		i;

    if (oldname != NULL)
    {
	/* A [g]vim.bat exists: Only choice is to overwrite it or not. */
	choices[choice_count].text = alloc(strlen(oldname) + 20);
	sprintf(choices[choice_count].text, "Overwrite %s", oldname);
	strcpy(batname, oldname);
	choices[choice_count].changefunc = toggle_bat_choice;
    }
    else
    {
	/* No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	 * $WINDIR by default, if it's empty the first item in $PATH. */
	p = getenv("WINDIR");
	if (p != NULL && *p != NUL)
	    strcpy(batname, p);
	else
	{
	    p = getenv("PATH");
	    if (p == NULL || *p == NUL)		/* "cannot happen" */
		strcpy(batname, "C:/Windows");
	    else
	    {
		i = 0;
		while (*p != NUL && *p != ';')
		    batname[i++] = *p++;
		batname[i] = NUL;
	    }
	}
	add_pathsep(batname);
	strcat(batname, name);
	choices[choice_count].text = alloc(strlen(batname) + 20);
	sprintf(choices[choice_count].text, "Create %s", batname);
	choices[choice_count].changefunc = change_bat_choice;
    }
    choices[choice_count].arg = gvim;
    choices[choice_count].installfunc = install_bat_choice;
    choices[choice_count].active = 1;
    ++choice_count;
}

/*
 * Install the vimrc file.
 */
    static void
install_vimrc(int idx)
{
    FILE	*fd;
    char	*fname;

    /* If an old vimrc file exists, overwrite it.
     * Otherwise create a new one. */
    if (*oldvimrc != NUL)
	fname = oldvimrc;
    else
	fname = vimrc;

    fd = fopen(fname, "w");
    if (fd == NULL)
    {
	printf("\nERROR: Cannot open \"%s\" for writing.\n", fname);
	return;
    }
    switch (compat_choice)
    {
	case compat_vi:
		    fprintf(fd, "set compatible\n");
		    break;
	case compat_some_enhancements:
		    fprintf(fd, "set nocompatible\n");
		    break;
	case compat_all_enhancements:
		    fprintf(fd, "set nocompatible\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    break;
    }
    switch (remap_choice)
    {
	case remap_no:
		    break;
	case remap_win:
		    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    break;
    }
    switch (mouse_choice)
    {
	case mouse_xterm:
		    fprintf(fd, "behave xterm\n");
		    break;
	case mouse_mswin:
		    fprintf(fd, "behave mswin\n");
		    break;
    }
    fclose(fd);
    printf("%s has been written\n", fname);
}

    static void
change_vimrc_choice(int idx)
{
    free(choices[idx].text);

    if (choices[idx].installfunc != NULL)
    {
	/* Switch to NOT change or create a vimrc file. */
	if (*oldvimrc != NUL)
	{
	    choices[idx].text = alloc(strlen(oldvimrc) + 20);
	    sprintf(choices[idx].text, "Do NOT change %s", oldvimrc);
	}
	else
	{
	    choices[idx].text = alloc(strlen(vimrc) + 20);
	    sprintf(choices[idx].text, "Do NOT create %s", vimrc);
	}
	choices[idx].installfunc = NULL;
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
    }
    else
    {
	/* Switch to change or create a vimrc file. */
	if (*oldvimrc != NUL)
	{
	    choices[idx].text = alloc(strlen(oldvimrc) + 20);
	    sprintf(choices[idx].text, "Overwrite %s with:", oldvimrc);
	}
	else
	{
	    choices[choice_count].text = alloc(strlen(vimrc) + 20);
	    sprintf(choices[choice_count].text, "Create %s with:", vimrc);
	}
	choices[idx].installfunc = install_vimrc;
	choices[idx + 1].active = 1;
	choices[idx + 2].active = 1;
	choices[idx + 3].active = 1;
    }
}

/*
 * Change the choice how to run Vim.
 */
    static void
change_run_choice(int idx)
{
    compat_choice = get_choice(compat_choices, TABLE_SIZE(compat_choices));
    sprintf(choices[idx].text, compat_text, compat_choices[compat_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    static void
change_remap_choice(int idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    sprintf(choices[idx].text, remap_text, remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    static void
change_mouse_choice(int idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    sprintf(choices[idx].text, mouse_text, mouse_choices[mouse_choice]);
}

    static void
init_vimrc_choices(void)
{
    /* set path for a new _vimrc file (also when not used) */
    strcpy(vimrc, installdir);
    strcpy(vimrc + runtimeidx, "_vimrc");

    if (*oldvimrc != NUL)
    {
	/* There is an existing ../_vimrc or ../.vimrc file, the default is to
	 * keep it. */
	choices[choice_count].text = alloc(strlen(oldvimrc) + 20);
	sprintf(choices[choice_count].text, "Do NOT change %s", oldvimrc);
	choices[choice_count].installfunc = NULL;
    }
    else
    {
	choices[choice_count].text = alloc(strlen(vimrc) + 20);
	sprintf(choices[choice_count].text, "Create %s with:", vimrc);
	choices[choice_count].installfunc = install_vimrc;
    }
    choices[choice_count].changefunc = change_vimrc_choice;
    choices[choice_count].active = 1;
    ++choice_count;

    /* default way to run Vim */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, compat_text, compat_choices[compat_choice_default]);
    compat_choice = compat_choice_default;
    choices[choice_count].changefunc = change_run_choice;
    choices[choice_count].installfunc = NULL;;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    /* Wether to remap keys */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, remap_text, remap_choices[remap_choice_default]);
    remap_choice = remap_choice_default;
    choices[choice_count].changefunc = change_remap_choice;
    choices[choice_count].installfunc = NULL;;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    /* default way to use the mouse */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, mouse_text, mouse_choices[mouse_choice_default]);
    mouse_choice = mouse_choice_default;
    choices[choice_count].changefunc = change_mouse_choice;
    choices[choice_count].installfunc = NULL;;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;
}


#ifdef WIN3264
/*
 * If there are old "Edit with Vim" entries in the registry, uninstall them.
 */
    static void
uninstall_old_popups( char *current_uninstall_name )
{
    HKEY key_handle;
    HKEY uninstall_key_handle;
    char *uninstall_key = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    char subkey_name_buff[BUFSIZE];
    char temp_string_buffer[BUFSIZE];
    char uninstall_string[BUFSIZE];
    long local_bufsize = BUFSIZE;
    FILETIME temp_pfiletime;
    DWORD key_index;
    char input;
    long code;
    long value_type;
    long orig_num_keys;
    long new_num_keys;

    code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstall_key, 0, KEY_READ, &key_handle);
    CHECK_REG_ERROR(code);

    for(key_index = 0;
        RegEnumKeyEx(key_handle, key_index, subkey_name_buff, &local_bufsize, NULL, NULL, NULL, &temp_pfiletime) != ERROR_NO_MORE_ITEMS;
        key_index++)
    {
        local_bufsize = BUFSIZE;
        if(strncmp("Vim", subkey_name_buff, 3) == 0)
        {
            /* Open the key named Vim* */
            code = RegOpenKeyEx(key_handle, subkey_name_buff, 0, KEY_READ, &uninstall_key_handle);
            CHECK_REG_ERROR(code);

            /* get the DisplayName out of it to show the user */
            code = RegQueryValueEx(uninstall_key_handle, "displayname", 0, &value_type, temp_string_buffer, &local_bufsize);
            local_bufsize = BUFSIZE;
            CHECK_REG_ERROR(code);

            /* If this is the popup we just installed.  We don't want to
             * uninstall it now.
             */
            if(strcmp(current_uninstall_name, temp_string_buffer) == 0)
                continue;

            printf("\n*********************************************************\n");
            printf("Vim Install found what may be a previous version of the\n");
            printf("\"Edit with Vim\" popup menu entry.  The name of the entry is:\n");
            printf("\n        \"%s\"\n", temp_string_buffer);

            printf("\nYou will now be given the choice of whether to uninstall this entry.\n");
            printf("If you do not uninstall it now, the new \"Edit with Vim\" can still be\n");
            printf("installed, and you may uninstall old versions manually at a later date by\n");
            printf("Start Menu->Settings->Control Panel->Add/Remove Programs and selecting\n");
            printf("\"%s\" from the list.\n", subkey_name_buff);

            printf("\nDo you want to uninstall \"%s\" now?\n    (y)es (n)o)\n\n", temp_string_buffer);

            input = 'n';
            do
            {
                if (input != 'n')
                {
                    printf("%c is an invalid option.  Please enter either 'y' or 'n'\n", input);
                }

                scanf("%c", &input);
                switch (input)
                {
                    case 'y':
                    case 'Y':
                    {
                        /* save the number of uninstall keys so we can know if it changed */
                        RegQueryInfoKey(key_handle, NULL, NULL, NULL, &orig_num_keys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

                        /* Delete the uninstall key.  It has no subkeys, so
                         * this is easy.
                         */
                        RegDeleteKey(key_handle, subkey_name_buff);

                        /* Check if an unistall reg key was deleted.
                         * if it was, we want to decrement key_index.
                         * if we don't do this, we will skip the key
                         * immediately after any key that we delete.
                         */
                        RegQueryInfoKey(key_handle, NULL, NULL, NULL, &new_num_keys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                        if (new_num_keys < orig_num_keys)
                        {
                            key_index--;
                        }
                        key_index--;

                        break;
                    }
                    case 'n':
                    case 'N':
                        /* Do not uninstall */
                        break;
                    default: /* just drop through and redo the loop */
                        break;
                }

            } while ((input != 'n') && (input != 'y'));
            RegCloseKey(uninstall_key_handle);
        }
    }
    RegCloseKey(key_handle);
}
#endif /* WIN3264 */

/*
 * Add some entries to the registry to add "Edit with Vim" to the context
 * menu.
 */
    static void
install_popup(int idx)
{
#ifdef DJGPP
    FILE *fd;
    char *cmd_path;
    char *p;
#else
# ifdef WIN3264
    HKEY temp_key;
    HKEY InProcServer32_key;

    long disposition = 0;
    long code = 0;
# endif
#endif

#if defined(DJGPP) || defined(WIN3264)
    const char *vim_ext_ThreadingModel = "Apartment";
    const char *vim_ext_name = "Vim Shell Extension";
    const char *vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";
#endif

    char	buf[BUFSIZE];
    char	uninstall_DisplayName[BUFSIZE];

    sprintf(uninstall_DisplayName, "Vim %s: Edit with Vim popup menu entry", VIM_VERSION_SHORT);

#ifdef DJGPP
    /* DJGPP cannot uninstall old versions of the popup because we have no way
     * to access the registry.  Any uninstall must be done manually.
     * Tell the user:
     */

    printf("\nThis version of install.exe was compiled using the DJGPP.\n");
    printf("install.exe then is unable to uninstall old versions of the \"Edit\n");
    printf("with Vim\" popup menu automatically.  If you want to uninstall any\n");
    printf("old versions, you may do so now.  If you choose to do so at a later\n");
    printf("time, you will have to reinstall this version if you still want the\n");
    printf("menu.\n");
    printf("\nPress Enter when you are ready to install the current popup menu.\n");
    while(getchar() != '\n');
    getchar();

    fd = fopen("vim.reg", "w");
    if (fd == NULL)
	printf("ERROR: Could not open vim.reg for writing\n");
    else
    {
	/*
	 * Write the registry entries for the "Edit with Vim" menu.
	 */
	fprintf(fd, "REGEDIT4\n");
	fprintf(fd, "\n");
	fprintf(fd, "HKEY_CLASSES_ROOT\\CLSID\\%s\n", vim_ext_clsid);
	fprintf(fd, "@=\"%s\"\n", vim_ext_name);
	fprintf(fd, "[HKEY_CLASSES_ROOT\\CLSID\\%s\\InProcServer32]\n", vim_ext_clsid);
	double_bs(installdir, buf); /* double the backslashes */
	fprintf(fd, "@=\"%sgvimext.dll\"\n", buf);
	fprintf(fd, "\"ThreadingModel\"=\"%s\"\n", vim_ext_ThreadingModel);
	fprintf(fd, "\n");
	fprintf(fd, "[HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\gvim]\n");
	fprintf(fd, "@=\"%s\"\n", vim_ext_clsid);
	fprintf(fd, "\n");
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved]\n");
	fprintf(fd, "\"%s\"=\"%s\"\n", vim_ext_clsid, vim_ext_name);
	fprintf(fd, "\n");
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Vim\\Gvim]\n");
	fprintf(fd, "\"path\"=\"%sgvim.exe\"\n", buf);
	fprintf(fd, "\n");

	/* The registry entries for uninstalling the menu */
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim %s]\n", VIM_VERSION_SHORT);

	fprintf(fd, "\"DisplayName\"=\"%s\"\n", uninstall_DisplayName);
	fprintf(fd, "\"UninstallString\"=\"%suninstal.exe\"\n", buf);

	fclose(fd);
        /* On WinNT, 'start' is a shell built-in for cmd.exe rather than an executable
         * (start.exe) like in Win9x.  DJGPP, being a DOS program, is given the COMSPEC
         * command.com by WinNT, so we have to find cmd.exe manually and use it.
         */
         if( (cmd_path = searchpath_save("cmd.exe") ) )
         {
             /* There is a cmd.exe, so this might be Windows NT.  If it is,
              * we need to call cmd.exe explicitly.  If it is a later OS,
              * calling cmd.exe won't hurt if it is present.
              */
             /* Replace the slashes with backslashes. */
             while( (p = strchr(cmd_path, '/')) != NULL )
             {
                 *p = '\\';
             }
             sprintf(buf, "%s /c start /w regedit /s vim.reg", cmd_path);
             system(buf);
         }
         else
         {
             /* No cmd.exe, just make the call and let the system handle it. */
             system("start /w regedit /s vim.reg");
         }

        remove("vim.reg");
    }

#else /* #ifdef DJGPP */
# ifdef WIN3264
    /*
     * Write the registry entries for the "Edit with Vim" menu.
     */

    /*	HKEY_CLASSES_ROOT\CLSID\{51EEE242-AD87-11d3-9C1E-0090278BBD99}
     *      @ = "Vim Shell Extension"
     */
    sprintf(buf, "CLSID\\%s", vim_ext_clsid);
    code = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                          buf,
                          0,
                          "",
                          REG_OPTION_NON_VOLATILE,
                          KEY_WRITE,
                          NULL,
                          &temp_key,
                          &disposition);
    CHECK_REG_ERROR(code);

    code = RegSetValueEx(temp_key, "", 0, REG_SZ, vim_ext_name, strlen(vim_ext_name));

    /*  HKEY_CLASSES_ROOT\CLSID\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\InProcServer32]
     *      @ = "<Vim Path>\gvimext.dll"
     *      ThreadingModel = "Apartment"
     */
    sprintf(buf, "%s\\gvimext.dll", installdir);
    code = RegCreateKeyEx(temp_key,
                          "InProcServer32",
                          0,
                          "",
                          REG_OPTION_NON_VOLATILE,
                          KEY_WRITE,
                          NULL,
                          &InProcServer32_key,
                          &disposition);
    CHECK_REG_ERROR(code);

    code = RegSetValueEx(InProcServer32_key, "", 0, REG_SZ, buf, strlen(buf));
    CHECK_REG_ERROR(code);
    code = RegSetValueEx(InProcServer32_key,
                         "ThreadingModel",
                         0,
                         REG_SZ,
                         vim_ext_ThreadingModel,
                         strlen(vim_ext_ThreadingModel));
    CHECK_REG_ERROR(code);

    RegCloseKey(InProcServer32_key);
    RegCloseKey(temp_key);

    /*	HKEY_CLASSES_ROOT\*\shellex\ContextMenuHandlers\gvim]
     *	    @ = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}"
     */
    code = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                   "*\\shellex\\ContextMenuHandlers\\gvim",
                   0,
                   "",
                   REG_OPTION_NON_VOLATILE,
                   KEY_WRITE,
                   NULL,
                   &temp_key,
                   &disposition);
    CHECK_REG_ERROR(code);

    code = RegSetValueEx(temp_key, "", 0, REG_SZ, vim_ext_clsid, strlen(vim_ext_clsid));
    CHECK_REG_ERROR(code);

    RegCloseKey(temp_key);

    /*	HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved
     *	    {51EEE242-AD87-11d3-9C1E-0090278BBD99} = "Vim Shell Extension\"
     */
    code = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                 "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
                 0,
                 KEY_WRITE,
                 &temp_key);
    CHECK_REG_ERROR(code);

    code = RegSetValueEx(temp_key, vim_ext_clsid, 0, REG_SZ, vim_ext_name, strlen(vim_ext_name));
    CHECK_REG_ERROR(code);

    RegCloseKey(temp_key);

    /*	HKEY_LOCAL_MACHINE\Software\Vim\Gvim
     *	    path = "<Vim Path>\gvim.exe"
     */
    code = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                   "Software\\Vim\\Gvim",
                   0,
                   "",
                   REG_OPTION_NON_VOLATILE,
                   KEY_WRITE,
                   NULL,
                   &temp_key,
                   &disposition);
    CHECK_REG_ERROR(code);

    sprintf(buf, "%s\\gvim.exe", installdir);
    code = RegSetValueEx(temp_key, "path", 0, REG_SZ, buf, strlen(buf));
    CHECK_REG_ERROR(code);

    RegCloseKey(temp_key);

	/* The registry entries for uninstalling the menu */
    /* Changes on 6/18/2001:
     *   Removed VIM_VERSION_SHORT from the immediately following line so that all versions of the
     *   Vim popup will be installed under the same uninstall key.  This is done so to prevent
     *   artifact uninstall keys if multiple popups are installed and then one is uninstalled.
     *
     *   The VIM_VERSION_SHORT is left in the DisplayName line so that the Windows uninstall menu
     *   will show the most recently installed version number.
     */

    /*	HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\Vim
     *	    DisplayName = "Vim <Vim Version>: Edit with Vim popup menu entry"
     *	    UninstallString = "<Vim Path>\uninstal.exe"
     */
    code = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                   "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim",
                   0,
                   "",
                   REG_OPTION_NON_VOLATILE,
                   KEY_WRITE,
                   NULL,
                   &temp_key,
                   &disposition);
    CHECK_REG_ERROR(code);

    sprintf(uninstall_DisplayName,
            "Vim %s: Edit with Vim popup menu entry",
            VIM_VERSION_SHORT);
    code = RegSetValueEx(temp_key,
                  "DisplayName",
                  0,
                  REG_SZ,
                  uninstall_DisplayName,
                  strlen(uninstall_DisplayName));
    CHECK_REG_ERROR(code);

    sprintf(buf, "%s\\uninstal.exe", installdir);
    code = RegSetValueEx(temp_key, "UninstallString", 0, REG_SZ, buf, strlen(buf));
    CHECK_REG_ERROR(code);

    RegCloseKey(temp_key);

	printf("Vim has been installed in the popup menu.\n");
	printf("Use uninstal.exe if you want to remove it again.\n");
	printf("Also see \":help win32-popup-menu\" in Vim.\n");

    /* Call uninstall_old_popups() to give the user the option to remove the
     * uninstall keys for old popup entries from the registry.
     */
    uninstall_old_popups(uninstall_DisplayName);

# endif /* ifdef WIN3264 */
#endif /* ifdef DJGPP - else... */

}

    static void
change_popup_choice(int idx)
{
    if (choices[idx].installfunc == NULL)
    {
	strcpy(choices[idx].text, "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim");
	choices[idx].installfunc = install_popup;
    }
    else
    {
	strcpy(choices[idx].text, "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim");
	choices[idx].installfunc = NULL;
    }
}

/*
 * Add a dummy choice to avoid that the numbering changes depending on items
 * in the environment.  The user may type a number he remembered without
 * looking.
 */
    static void
add_dummy_choice(void)
{
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 0;
    choices[choice_count].changefunc = NULL;
    choices[choice_count].installfunc = NULL;
    ++choice_count;
}

/*
 * Only add the choice for the popup menu entry when gvim.exe was found and
 * both gvimext.dll and regedit.exe exist.
 */
    static void
init_popup_choice(void)
{
    struct stat	st;

    if (has_gvim
	    && stat("gvimext.dll", &st) >= 0
#ifndef WIN3264
	    && searchpath("regedit.exe") != NULL
#endif
       )
    {
	/* default way to use the mouse */
	choices[choice_count].text = alloc(150);
	choices[choice_count].changefunc = change_popup_choice;
	choices[choice_count].installfunc = NULL;
	choices[choice_count].active = 1;
	change_popup_choice(choice_count);
	++choice_count;
    }
    else
	add_dummy_choice();
}

#ifdef WIN3264
/* create_shortcut
 *
 * Create a shell link.
 *
 * returns 0 on failure, non-zero on successful completion.
 *
 * NOTE:  Currently, this may not work in DJGPP, MINGW
 *	  Untested in BC3, VC < 5, ming, djgpp
 */
    int
create_shortcut(
	const char *shortcut_name,
	const char *iconfile_path,
	int	    iconindex,
	const char *shortcut_target,
	const char *shortcut_args,
	const char *workingdir
	)
{
   IShellLink	    *shelllink_ptr;
   HRESULT	    hres;
   IPersistFile	    *persistfile_ptr;

   printf("Creating shortcut: %s\n", shortcut_name);
   /* Initialize COM library */
   hres = CoInitialize(NULL);
   if (!SUCCEEDED(hres))
   {
      printf("Error:  Could not open the COM library.  Not creating shortcut.\n");
      return 0;
   }

   /* Instantiate a COM object for the ShellLink, store a pointer to it
    * in shelllink_ptr.  */
   hres = CoCreateInstance(&CLSID_ShellLink,
			   NULL,
			   CLSCTX_INPROC_SERVER,
			   &IID_IShellLink,
			   (void **) &shelllink_ptr);

   if (SUCCEEDED(hres)) /* If the instantiation was successful... */
   {
      /* ...Then build a PersistFile interface for the ShellLink so we can
       * save it as a file after we build it.  */
      hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr, &IID_IPersistFile, (void **) &persistfile_ptr);

      if (SUCCEEDED(hres))
      {
	 wchar_t wsz[BUFSIZE];

	 /* translate the (possibly) multibyte shortcut filename to windows
	  * Unicode so it can be used as a file name.
	  */
	 MultiByteToWideChar(CP_ACP, 0, shortcut_name, -1, wsz, BUFSIZE);

	 /* set the attributes */
	 shelllink_ptr->lpVtbl->SetPath(shelllink_ptr, shortcut_target);
	 shelllink_ptr->lpVtbl->SetWorkingDirectory(shelllink_ptr, workingdir);
	 shelllink_ptr->lpVtbl->SetIconLocation(shelllink_ptr, iconfile_path,
								   iconindex);
	 shelllink_ptr->lpVtbl->SetArguments(shelllink_ptr, shortcut_args);

	 /* save the shortcut to a file and return the PersistFile object*/
	 persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, TRUE);
	 persistfile_ptr->lpVtbl->Release(persistfile_ptr);
      }
      else
      {
	 printf("QueryInterface Error\n");
	 return 0;
      }

      /* Return the ShellLink object */
      shelllink_ptr->lpVtbl->Release(shelllink_ptr);
   }
   else
   {
      printf("CoCreateInstance Error - hres = %08x\n", hres);
      return 0;
   }

   return 1;
}

/*
 * Give the user options and get a choice where to create shortcut(s).
 */
    static void
change_shortcut_location_choice(int idx)
{
    shortcut_location_choice = get_choice(shortcut_location_choices, TABLE_SIZE(shortcut_location_choices));
    sprintf(choices[idx].text, shortcut_location_text, shortcut_location_choices[shortcut_location_choice]);
    if (shortcut_location_choice == shortcuts_none)
    {
	choices[idx + 1].active = 0;
	choices[idx + 2].active = 0;
	choices[idx + 3].active = 0;
	choices[idx + 4].active = 0;
	choices[idx + 5].active = 0;
	choices[idx + 6].active = 0;
	choices[idx + 7].active = 0;
    }
    else
    {
	choices[idx + 1].active = has_vim;
	choices[idx + 2].active = has_gvim;
	choices[idx + 3].active = has_evim;
	choices[idx + 4].active = has_view;
	choices[idx + 5].active = has_gview;
	choices[idx + 6].active = has_vimdiff;
	choices[idx + 7].active = has_gvimdiff;
    }
}

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return 0 on error, non-zero on success
 */
    int
get_shell_folder_path(char shell_folder_path[BUFSIZE], const char *shell_folder_name)
{
#if 0
    /*
     * The following code does not work in W9X as it refers to registry
     * data which does not exist.
     */
    long path_length = BUFSIZE; /* this variable needs to be volatile so we
				    can't just use BUFSIZE */
    long value_type;
    long rtype;

    char unexpanded_shell_folder_path[BUFSIZE];
    char *user_shell_folders_key = "software\\microsoft\\windows\\currentversion\\explorer\\User Shell Folders";

    HKEY key_handle;

    rtype = RegOpenKeyEx(HKEY_CURRENT_USER, user_shell_folders_key, 0, KEY_QUERY_VALUE, &key_handle);
    if (rtype != ERROR_SUCCESS)
    {
	printf("\nERROR opening registry key: \"%s\"\n\n", user_shell_folders_key);
	return 0;
    }

    rtype = RegQueryValueEx(key_handle, shell_folder_name, NULL, &value_type, unexpanded_shell_folder_path, &path_length);
    if (rtype != ERROR_SUCCESS)
    {
	printf("\nERROR querying for shell folder \"%s\"\n\n", shell_folder_name);
	return 0;
    }

    if (value_type == REG_EXPAND_SZ)
    {
	/* There are environment variables (%WINDIR% for example) in the
	 * path */
	ExpandEnvironmentStrings(unexpanded_shell_folder_path, shell_folder_path, BUFSIZE);
    }
    else
    {
	/* no environment variables, just copy the result to the pointer we
	 * got */
	strcpy(shell_folder_path, unexpanded_shell_folder_path);
    }
    return 1;
#else
    /*
     * The following code was successfully built with make_mvc.mak.
     * The resulting executable worked on Windows 95, Millennium Edition, and
     * 2000 Professional.
     */
    LPITEMIDLIST pidl = 0; /* Pointer to an Item ID list allocated below */
    LPMALLOC pMalloc; /* Pointer to an IMalloc interface */
    int csidl;

    if (strcmp(shell_folder_name, "desktop") == 0)
    {
	csidl = CSIDL_DESKTOP;
    }
    else
    if (strcmp(shell_folder_name, "programs") == 0)
    {
	csidl = CSIDL_PROGRAMS;
    }
    else
    {
	printf("\nERROR (internal) unrecognised shell_folder_name: \"%s\"\n\n", shell_folder_name);
	return 0;
    }

    /* Initialize pointer to IMalloc interface */
    if (NOERROR != SHGetMalloc(&pMalloc))
    {
	printf("\nERROR getting interface for shell_folder_name: \"%s\"\n\n", shell_folder_name);
	return 0;
    }

    /* Get an ITEMIDLIST corresponding to the folder code */
    if (NOERROR != SHGetSpecialFolderLocation(0, csidl, &pidl)) {
	printf("\nERROR getting ITEMIDLIST for shell_folder_name: \"%s\"\n\n", shell_folder_name);
	return 0;
    }

    /* Translate that ITEMIDLIST to a string */
    if (!SHGetPathFromIDList(pidl, shell_folder_path)) {
	printf("\nERROR translating ITEMIDLIST for shell_folder_name: \"%s\"\n\n", shell_folder_name);
	pMalloc->lpVtbl->Free(pMalloc, pidl);
	pMalloc->lpVtbl->Release(pMalloc);
	return 0;
    }

    /* Free the data associated with pidl */
    pMalloc->lpVtbl->Free(pMalloc, pidl);
    /* Release the IMalloc interface */
    pMalloc->lpVtbl->Release(pMalloc);
    return 1;
#endif
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   int
build_link_name(
	char link_path[BUFSIZE],
	const char *link_name,
	const char *shell_folder_name)
{
    char	shell_folder_path[BUFSIZE];
    int		return_val = 0;

    return_val = get_shell_folder_path(shell_folder_path, shell_folder_name);
    if (return_val == 0)
    {
	printf("An error occurred while attempting to find the path to %s.\n", shell_folder_name);
	return 0;
    }

    /* build the path to the shortcut and the path to gvim.exe */
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    return return_val;
}


    int
build_shortcut(const char *name, const char *shell_folder)
{
    char executable_path [BUFSIZE];
    char link_name [BUFSIZE];
    int return_val = 0;

    sprintf(executable_path, "%s\\%s.exe", installdir, name);

    return_val = build_link_name(link_name, name, shell_folder);
    if (return_val == 0)
	return 0;

    /* Create the shortcut: */
    create_shortcut( link_name, executable_path, 0, executable_path, "", "");

    return 1;
}
/*
 * Function to actually create the shortcuts
 *
 * On shortcut creation:
 * 1. For now, I am assuming we only create a shortcut if gvim is present,
 *    as console Vim will usually be started from a console (I assume...)
 * 2. Currently, I supply no args to gvim in the shortcut, though this could be
 *    used if we want to make a shortcut to vimdiff/view/evim...
 * 3. Currently I am supplying no working directory to the shortcut.  This means
 *    that the initial working dir will be:
 *    - the location of the shortcut if no file is supplied
 *    - the location of the file being edited if a file is supplied (ie via drag
 *	and drop onto the shortcut).
 */
    void
install_shortcuts(int idx)
{
    int return_val = 0;

    /* Create the paths to the executables */
    switch (shortcut_location_choice)
    {
	case shortcuts_none: /* no shortcuts - do nothing */
	{
	    break;
	}
	case shortcuts_both: /* Create shortcuts in Start Menu\Programs AND Desktop */
	{
	    /* Since we have the code to create each of those individually,
	     * just drop through and execute each one.
	     */
	}
	case shortcuts_desktop: /* Create shortcut(s) on the desktop */
	{
	    if (has_vim && (vim_shortcut_choice == shortcut_create) )
	    {
		/* get the name of the link to put on the desktop */
		return_val = build_shortcut("vim", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to vim will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (has_gvim && (gvim_shortcut_choice == shortcut_create) )
	    {
		/* get the name of the link to put on the desktop */
		return_val = build_shortcut("gvim", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to gvim will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (has_evim && (evim_shortcut_choice == shortcut_create) )
	    {
		/* get the name of the link to put on the desktop */
		return_val = build_shortcut("evim", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to evim will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (has_view && (view_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("view", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to view will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (has_gview && (gview_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("gview", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to gview will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (has_vimdiff && (vimdiff_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("vimdiff", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to vimdiff will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (has_gvimdiff && (gvimdiff_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("gvimdiff", "desktop");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to gvimdiff will not be created on the desktop.\n");
		    break;
		}
	    }

	    if (shortcut_location_choice == shortcuts_desktop) /* Then we only want to create a shortcut here */
	    {
		break;
	    }
	    /* We got here by drop through from above:  Keep on going... */
	}
	case shortcuts_start: /* Create shortcut(s) in the Start Menu\Programs folder */
	{
	    if (has_vim && (vim_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("vim", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to vim will not be created in the Start Menu.\n");
		    break;
		}
	    }

	    if (has_gvim && (gvim_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("gvim", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to gvim will not be created in the Start Menu.\n");
		    break;
		}
	    }

	    if (has_evim && (evim_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("evim", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to evim will not be created in the Start Menu.\n");
		    break;
		}
	    }

	    if (has_view && (view_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("view", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to view will not be created in the Start Menu.\n");
		    break;
		}
	    }

	    if (has_gview && (gview_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("gview", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to gview will not be created in the Start Menu.\n");
		    break;
		}
	    }

	    if (has_vimdiff && (vimdiff_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("vimdiff", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to vimdiff will not be created in the Start Menu.\n");
		    break;
		}
	    }

	    if (has_gvimdiff && (gvimdiff_shortcut_choice == shortcut_create) )
	    {
		return_val = build_shortcut("gvimdiff", "programs");
		if (return_val == 0)
		{
		    printf("An error has occurred.  A shortcut to gvimdiff will not be created in the Start Menu.\n");
		    break;
		}
	    }
	    break;
	}
	default:
	{
	    /* We should never get here... famous last words... ;-)
	     * Just in case we DO end up here, with an invalid option selected
	     * let the user remake the selection.  Give them the choice again,
	     * then recall this function to finish installing the shortcut.
	     */
	    printf("\nYour choice of shortcuts has somehow become corrupted.  Please choose again:\n");
	    change_shortcut_location_choice(idx);
	    install_shortcuts(idx);
	    break;
	}
    }
}

    void
toggle_vim_shortcut_choice(int idx)
{
    if (vim_shortcut_choice == shortcut_no_create && has_vim)
    {
	vim_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, vim_shortcut_text, vim_shortcut_choices[vim_shortcut_choice]);
    }
    else
    {
	vim_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, vim_shortcut_text, vim_shortcut_choices[vim_shortcut_choice]);
    }
}

    void
toggle_gvim_shortcut_choice(int idx)
{
    if (gvim_shortcut_choice == shortcut_no_create && has_gvim)
    {
	gvim_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, gvim_shortcut_text, gvim_shortcut_choices[gvim_shortcut_choice]);
    }
    else
    {
	gvim_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, gvim_shortcut_text, gvim_shortcut_choices[gvim_shortcut_choice]);
    }
}

    void
toggle_evim_shortcut_choice(int idx)
{
    if (evim_shortcut_choice == shortcut_no_create && has_evim)
    {
	evim_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, evim_shortcut_text, evim_shortcut_choices[evim_shortcut_choice]);
    }
    else
    {
	evim_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, evim_shortcut_text, evim_shortcut_choices[evim_shortcut_choice]);
    }
}

    void
toggle_view_shortcut_choice(int idx)
{
    if (view_shortcut_choice == shortcut_no_create && has_view)
    {
	view_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, view_shortcut_text, view_shortcut_choices[view_shortcut_choice]);
    }
    else
    {
	view_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, view_shortcut_text, view_shortcut_choices[view_shortcut_choice]);
    }
}

    void
toggle_gview_shortcut_choice(int idx)
{
    if (gview_shortcut_choice == shortcut_no_create && has_gview)
    {
	gview_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, gview_shortcut_text, gview_shortcut_choices[gview_shortcut_choice]);
    }
    else
    {
	gview_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, gview_shortcut_text, gview_shortcut_choices[gview_shortcut_choice]);
    }
}

    void
toggle_vimdiff_shortcut_choice(int idx)
{
    if (vimdiff_shortcut_choice == shortcut_no_create && has_vimdiff)
    {
	vimdiff_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, vimdiff_shortcut_text, vimdiff_shortcut_choices[vimdiff_shortcut_choice]);
    }
    else
    {
	vimdiff_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, vimdiff_shortcut_text, vimdiff_shortcut_choices[vimdiff_shortcut_choice]);
    }
}

    void
toggle_gvimdiff_shortcut_choice(int idx)
{
    if (gvimdiff_shortcut_choice == shortcut_no_create && has_gvimdiff)
    {
	gvimdiff_shortcut_choice = shortcut_create;
	sprintf(choices[idx].text, gvimdiff_shortcut_text, gvimdiff_shortcut_choices[gvimdiff_shortcut_choice]);
    }
    else
    {
	gvimdiff_shortcut_choice = shortcut_no_create;
	sprintf(choices[idx].text, gvimdiff_shortcut_text, gvimdiff_shortcut_choices[gvimdiff_shortcut_choice]);
    }
}

/*
 * Add the choice for the shortcuts.
 */
    static void
init_shortcut_choice(void)
{
    choices[choice_count].text = alloc(150);
    shortcut_location_choice = shortcut_location_choice_default;
    sprintf(choices[choice_count].text, shortcut_location_text, shortcut_location_choices[shortcut_location_choice]);
    choices[choice_count].changefunc = change_shortcut_location_choice;
    choices[choice_count].installfunc = install_shortcuts;
    choices[choice_count].active = 1;
    ++choice_count;

    /* Shortcut to vim */
    choices[choice_count].text = alloc(150);
    vim_shortcut_choice = vim_shortcut_choice_default;
    sprintf(choices[choice_count].text, vim_shortcut_text, vim_shortcut_choices[vim_shortcut_choice]);
    choices[choice_count].changefunc = toggle_vim_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;

    /* Shortcut to gvim */
    choices[choice_count].text = alloc(150);
    gvim_shortcut_choice = gvim_shortcut_choice_default;
    sprintf(choices[choice_count].text, gvim_shortcut_text, gvim_shortcut_choices[gvim_shortcut_choice]);
    choices[choice_count].changefunc = toggle_gvim_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;

    /* Shortcut to evim */
    choices[choice_count].text = alloc(150);
    evim_shortcut_choice = evim_shortcut_choice_default;
    sprintf(choices[choice_count].text, evim_shortcut_text, evim_shortcut_choices[evim_shortcut_choice]);
    choices[choice_count].changefunc = toggle_evim_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;

    /* Shortcut to view */
    choices[choice_count].text = alloc(150);
    view_shortcut_choice = view_shortcut_choice_default;
    sprintf(choices[choice_count].text, view_shortcut_text, view_shortcut_choices[view_shortcut_choice]);
    choices[choice_count].changefunc = toggle_view_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;

    /* Shortcut to gview */
    choices[choice_count].text = alloc(150);
    gview_shortcut_choice = gview_shortcut_choice_default;
    sprintf(choices[choice_count].text, gview_shortcut_text, gview_shortcut_choices[gview_shortcut_choice]);
    choices[choice_count].changefunc = toggle_gview_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;

    /* Shortcut to vimdiff */
    choices[choice_count].text = alloc(150);
    vimdiff_shortcut_choice = vimdiff_shortcut_choice_default;
    sprintf(choices[choice_count].text, vimdiff_shortcut_text, vimdiff_shortcut_choices[vimdiff_shortcut_choice]);
    choices[choice_count].changefunc = toggle_vimdiff_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;

    /* Shortcut to gvimdiff */
    choices[choice_count].text = alloc(150);
    gvimdiff_shortcut_choice = gvimdiff_shortcut_choice_default;
    sprintf(choices[choice_count].text, gvimdiff_shortcut_text, gvimdiff_shortcut_choices[gvimdiff_shortcut_choice]);
    choices[choice_count].changefunc = toggle_gvimdiff_shortcut_choice;
    choices[choice_count].active = 0;
    ++choice_count;
}

/*
 * Attempt to register OLE for Vim.
 */
   static void
install_OLE_register(int idx)
{
    char register_command_string[BUFSIZE + 20];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("If Vim registers successfully, a message box will appear with a success message.\n");
    printf("If Vim does not register, a message box will appear with the error.\n");
    printf("To finish installation, click the OK button in the message box.\n");
    printf("If no message box appears at all, your gvim.exe is not OLE enabled.\n");

#ifndef __CYGWIN__
    sprintf(register_command_string, "\"%s\\gvim.exe\" -register", installdir);
#else
    /* handle this differently for Cygwin which sometimes has trouble with
     * Windows-style pathnames here. */
    sprintf(register_command_string, "./gvim.exe -register", installdir);
#endif
    system(register_command_string);
}

/*
 * Toggle whether to try registering OLE for Vim
 */
   static void
change_OLE_register_choice(int idx)
{
    if (choices[idx].installfunc == NULL)
    {
	strcpy(choices[idx].text, "Register OLE Vim if your gvim.exe is OLE enabled.");
	choices[idx].installfunc = install_OLE_register;
    }
    else
    {
	strcpy(choices[idx].text, "Do not try to register OLE Vim.  If Registration is necessary,\n    Vim will give you the option next time you run it in GUI mode.");
	choices[idx].installfunc = NULL;
    }
}

/*
 * Add the choice for registering OLE Vim
 */
    static void
init_OLE_register_choice(void)
{
    choices[choice_count].text = alloc(150);
    choices[choice_count].changefunc = change_OLE_register_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 1;
    change_OLE_register_choice(choice_count);
    ++choice_count;
}
#endif /* WIN3264 */

/*
 * Remove the last part of a directory path to get its parent.
 */
    static void
dir_remove_last(const char *path, char buffer[BUFSIZE])
{
    char c;
    long last_char_to_copy;
    long path_length = strlen(path);

    /* skip the last character just in case it is a '\\' */
    last_char_to_copy = path_length - 2;
    c = path[last_char_to_copy];

    while (c != '\\')
    {
	last_char_to_copy--;
	c = path[last_char_to_copy];
    }

    strncpy(buffer, path, last_char_to_copy);
    buffer[last_char_to_copy] = NUL;
}

/*
 * Change the directory that the vim plugin directories will be
 * created in.  (Either $HOME or $VIM, or nowhere.)
 */
    static void
change_vimfilesdir_choice(int idx)
{
    vimfiles_dir_choice = get_choice(vimfiles_dir_choices, TABLE_SIZE(vimfiles_dir_choices));
    sprintf(choices[idx].text, vimfiles_dir_text, vimfiles_dir_choices[vimfiles_dir_choice]);
}

/*
 * Create the vimfiles directories...
 */
    static void
install_vimfilesdir(int idx)
{
    int i;
    char *p;
    char vimdir_path[BUFSIZE];
    char vimfiles_path[BUFSIZE];
    char tmp_dirname[BUFSIZE];

    /* switch on the location that the user wants the plugins directories
     * built in */
    switch (vimfiles_dir_choice)
    {
	case vimfiles_dir_vim:
	{
	    /* Go to the %VIM% directory - check env first, then go one dir
	     *	   below installdir if there is no %VIM% environment variable.
	     *	   The accuracy of $VIM is checked in inspect_system(), so we
	     *	   can be sure it is ok to use here. */
	    p = getenv("VIM");
	    if (p == NULL) /* No $VIM in path */
		dir_remove_last(installdir, vimdir_path);
	    else
		strcpy(vimdir_path, p);

	    break;
	}
	case vimfiles_dir_home:
	{
	    /* Try to find the %HOME% directory.  If none exists, make the
	     * user choose again. */
	    p = getenv("HOME");
	    if (p == NULL) /* No %HOME% in path */
	    {
		printf("You selected to install the vim plugins directories in\n");
		printf("your home directory, but there is no HOME environment\n");
		printf("variable defined.  Please choose again, or define a HOME\n");
		printf("environment variable and run install.exe again.\n\n");
		change_vimfilesdir_choice(idx);
		install_vimfilesdir(idx);
	    }
	    else
	    {
		strcpy(vimdir_path, p);
	    }
	    break;
	}
	case vimfiles_dir_none:
	{
	    /* Do not create vim plugins directory */
	    return;
	}
    }

    /* Now, just create the directory.	If it already exists, it will fail
     * silently.
     */
    sprintf(vimfiles_path, "%s\\%s", vimdir_path, vimfiles_dir_name);
    vim_mkdir(vimfiles_path, 0755);

    printf("Now creating the following directories so that plugins and\n");
    printf("documentation can be dropped in easily:\n\n");
    for (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_path, vimfiles_subdirs[i]);
	printf("    %s\n", tmp_dirname);
	vim_mkdir(tmp_dirname, 0755);
    }
}

/*
 * Add the vimfiles dir creation to the setup sequence.
 * Not much to do here, since we don't currently give a choice.
 */
    static void
init_vimfilesdir_choice(void)
{
    choices[choice_count].text = alloc(150);
    choices[choice_count].changefunc = change_vimfilesdir_choice;
    choices[choice_count].installfunc = install_vimfilesdir;
    choices[choice_count].active = 1;
    vimfiles_dir_choice = vimfiles_dir_choice_default;
    sprintf(choices[choice_count].text, vimfiles_dir_text, vimfiles_dir_choices[vimfiles_dir_choice]);
    ++choice_count;
}

/*
 * Setup the default choices.
 */
    static void
setup_choices(void)
{
    /* (over) write vim.bat file */
    if (oldvimexe == NULL && has_vim)
	init_bat_choice("vim.bat", oldvimbat, vimbat, install_batfile_vim);
    else
	add_dummy_choice();

    /* (over) write gvim.bat file */
    if (oldgvimexe == NULL && has_gvim)
	init_bat_choice("gvim.bat", oldgvimbat, gvimbat, install_batfile_gvim);
    else
	add_dummy_choice();

    /* (over) write evim.bat file */
    if (oldevimexe == NULL && has_evim)
	init_bat_choice("evim.bat", oldevimbat, evimbat, install_batfile_evim);
    else
	add_dummy_choice();

    /* (over) write view.bat file */
    if (oldviewexe == NULL && has_view)
	init_bat_choice("view.bat", oldviewbat, viewbat, install_batfile_view);
    else
	add_dummy_choice();

    /* (over) write gview.bat file */
    if (oldgviewexe == NULL && has_gview)
	init_bat_choice("gview.bat", oldgviewbat, gviewbat, install_batfile_gview);
    else
	add_dummy_choice();

    /* (over) write vimdiff.bat file */
    if (oldvimdiffexe == NULL && has_vimdiff)
	init_bat_choice("vimdiff.bat", oldvimdiffbat, vimdiffbat, install_batfile_vimdiff);
    else
	add_dummy_choice();

    /* (over) write gvimdiff.bat file */
    if (oldgvimdiffexe == NULL && has_gvimdiff)
	init_bat_choice("gvimdiff.bat", oldgvimdiffbat, gvimdiffbat, install_batfile_gvimdiff);
    else
	add_dummy_choice();

    /* (over) write _vimrc file */
    init_vimrc_choices();

    /* Whether to add Vim to the popup menu */
    init_popup_choice();

#ifdef WIN3264
    /* Whether to add shortcuts to Vim on desktop or in Start Menu
     * Only available if gvim.exe is present (for now at least)
     */
    if (has_vim || has_gvim || has_evim || has_view || has_gview || has_vimdiff || has_vimdiff)
	init_shortcut_choice();
    else
#endif
	add_dummy_choice();

#ifdef WIN3264
    /* If gvim is present, it may be OLE enabled.  Since there is no easy way
     * to detect whether gvim is OLE enabled from install, we'll just give the
     * option to register it.  If it is not OLE enabled and we try to register,
     * nothing will happen, so we'll be ok.  */
    if (has_gvim)
	init_OLE_register_choice();
    else
#endif
	add_dummy_choice();

    init_vimfilesdir_choice();
}

/*
 * Show a few screens full of helpful information.
 */
    static void
show_help(void)
{
    static char *(items[]) =
    {
"The vim.bat and gvim.bat files\n"
"------------------------------\n"
"The vim.bat and/or gvim.bat files are written in one of the directories\n"
"in $PATH.  This makes it possible to start vim and/or gvim from the\n"
"command line.\n"
"If vim.exe can be found in $PATH, the choice or vim.bat will not be\n"
"present.  It is assumed you will use the existing vim.exe.\n"
"Similarly for gvim.bat and gvim.exe.\n"
"If vim.bat can already be found in $PATH this is probably for an old\n"
"version of Vim (but this is not checked!).\n"
"If you select this item, you can select one of the directories in $PATH\n"
"for writing the batch file in, or disable it being written.\n"
"\n"
"If you choose not to create the vim.bat and/or gvim.bat file, Vim\n"
"can still be executed in other ways, but not from the command line.\n"
"\n",
"The same applies to choices for evim, view, and vimdiff.\n"
,
"Creating a _vimrc file\n"
"----------------------\n"
"The install program can create a simple _vimrc file that contains a few\n"
"basic choices for how to run Vim.  You can edit this file later to\n"
"tune your preferences.\n"
"If you already have a _vimrc or .vimrc file it can be overwritten.  But\n"
"you probably don't want to do that.\n"
,
"Vim features\n"
"------------\n"
"(this choice is only available when creating a _vimrc file)\n"
"1. Vim can run in Vi-compatible mode.  Many nice Vim features are then\n"
"   disabled.  In the not-Vi-compatible mode Vim is still mostly Vi\n"
"   compatible, but adds nice features like multi-level undo.  Only\n"
"   choose Vi-compatible if you really need full Vi compatibility.\n"
"2. Running Vim with some enhancements is useful when you want some of\n"
"   the nice Vim features, but have a slow computer and want to keep it\n"
"   really fast.\n"
"3. Syntax highlighting shows many files in color.  Not only does this look\n"
"   nice, it also makes it easier to spot errors and you can work faster.\n"
"   The other features include editing compressed files.\n"
,
"Windows key mapping\n"
"-------------------\n"
"(this choice is only available when creating a _vimrc file)\n"
"Under MS-Windows the CTRL-C key copies text to the clipboard and CTRL-V\n"
"pastes text from the clipboard.  There are a few more keys like these.\n"
"Unfortunately, in Vim these keys normally have another meaning.\n"
"1. Choose to have the keys like they normally are in Vim (useful if you\n"
"   also use Vim on other systems).\n"
"2. Choose to have the keys work like they are used on MS-Windows (useful\n"
"   if you mostly work on MS-Windows).\n"
,
"Mouse use\n"
"---------\n"
"(this choice is only available when creating a _vimrc file)\n"
"The right mouse button can be used in two ways:\n"
"1. The Unix way is to extend an existing selection.  The popup menu is\n"
"   not available.\n"
"2. The MS-Windows way is to show a popup menu, which allows you to\n"
"   copy/paste text, undo/redo, etc.  Extending the selection can still be\n"
"   done by keeping SHIFT pressed while using the left mouse button\n"
,
"Edit with Vim menu entry\n"
"------------------------\n"
"(this choice is only available when gvim.exe and gvimext.dll are present)\n"
"You can associate different file types with Vim, so that you can (double)\n"
"click on a file to edit it with Vim.  This means you have to individually\n"
"select each file type.\n"
"An alternative is the option offered here: Install an \"Edit with Vim\"\n"
"entry in the popup menu for the right mouse button.  This means you can\n"
"edit any file you like with Vim\n"
,
"Shortcuts to GVim\n"
"-----------------\n"
"In Windows 95 and later, shortcuts to GVim can be created on the Desktop,\n"
"in the Start Menu, or both.  This option is turned off by default.\n"
,
"Registering OLE in GVim with OLE enabled\n"
"----------------------------------------\n"
"On by default if GVim is present.  This will register OLE for GVim if\n"
"your GVim is OLE enabled.  Otherwise it will do nothing.  This is not\n"
"available for console mode Vim.\n"
,
NULL
    };
    int		i;
    int		c;
    int		x;

    /* Eat input until we get the CR after the 'h'. */
    x = 0;
    while (x != '\n' && x != '\r')
	x = getchar();

    printf("\n");
    for (i = 0; items[i] != NULL; ++i)
    {
	printf(items[i]);
	printf("\n");
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	/* skip anything typed until a CR */
	x = c;
	while (x != '\n' && x != '\r')
	    x = getchar();
	if (c == 'b' || c == 'B')
	{
	    if (i == 0)
		--i;
	    else
		i -= 2;
	}
	if (c == 'q' || c == 'Q')
	    break;
	printf("\n");
    }
}

/*
 * Install the choices.
 */
    static void
install(void)
{
    int		i;

    for (i = 0; i < choice_count; ++i)
	if (choices[i].installfunc != NULL)
	    (choices[i].installfunc)(i);
}

/*
 * request_choice
 */
    static void
request_choice(void)
{
    int               i;

    printf("\n\nInstall will do for you:\n");
    for (i = 0; i < choice_count; ++i)
      if (choices[i].active)
          printf("%2d  %s\n", i + 1, choices[i].text);
    printf("To change an item, enter its number\n\n");
    printf("Enter item number, h (help), d (do it) or q (quit): ");
}

    int
main(int argc, char **argv)
{
    int		i;
    char	buf[BUFSIZE];

    /* Initialize this program. */
    do_inits(argv);

    printf("This program sets up the installation of Vim %s\n\n",
	    VIM_VERSION_MEDIUM);

    /* Check if the user unpacked the archives properly. */
    check_unpack();

    /* Find out information about the system. */
    inspect_system();

    /* Setup the default choices. */
    setup_choices();

    /* Let the user change choices and finally install (or quit). */
    for (;;)
    {
        request_choice();
	if (scanf("%99s", buf) == 1)
	{
	    if (isdigit(buf[0]))
	    {
		/* Change a choice. */
		i = atoi(buf);
		if (i > 0 && i <= choice_count && choices[i - 1].active)
		    (choices[i - 1].changefunc)(i - 1);
		else
		    printf("\nIllegal choice\n");
	    }
	    else if (buf[0] == 'h' || buf[0] == 'H')
	    {
		/* Help */
		show_help();
	    }
	    else if (buf[0] == 'd' || buf[0] == 'D')
	    {
		/* Install! */
		install();
		printf("\nThat finishes the installation.  Happy Vimming!\n");
		break;
	    }
	    else if (buf[0] == 'q' || buf[0] == 'Q')
	    {
		/* Quit */
		printf("\nExiting without anything done\n");
		break;
	    }
	    else
		printf("\nIllegal choice\n");
	}
    }
    printf("\n");

    return 0;
}
