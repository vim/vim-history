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

#ifdef WIN32
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
#ifdef WIN32
# include <shlobj.h>
#endif

#ifdef DJGPP
# define vim_mkdir(x, y) mkdir((char *)(x), y)
#else
# ifdef WIN32
#  define vim_mkdir(x, y) _mkdir((char *)(x))
# else
#  define vim_mkdir(x, y) mkdir((char *)(x))
# endif
#endif
/* ---------------------------------------- */

#include "version.h"

#define BUFSIZE 512		/* long enough to hold a file name path */
#define NUL 0

char	installdir[BUFSIZE];	/* top of the installation dir, where the
				   install.exe is located, E.g.:
				   "c:\vim\vim60" */
int	runtimeidx;		/* index in installdir[] where "vim60" starts */
int	has_gvim = 0;		/* installable gvim.exe exists */
int	has_vim = 0;		/* installable vim.exe exists */

char	*sysdrive;		/* system drive or "c:\" */

char	*oldvimbat;		/* path to vim.bat or NULL */
char	*oldgvimbat;		/* path to gvim.bat or NULL */
char	*oldvimexe;		/* path to vim.exe or NULL */
char	*oldgvimexe;		/* path to gvim.exe or NULL */

char	vimbat[BUFSIZE];	/* Name of Vim batch file to write.  Not
				   installed when it's empty. */
char	gvimbat[BUFSIZE];	/* Name of GVim batch file to write.  Not
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

struct choice	choices[20];		/* choices the user can make */
int		choice_count = 0;	/* number of choices available */

#define TABLE_SIZE(s)	sizeof(s) / sizeof(char *)

char	*(compat_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "with some Vim ehancements",
    "with syntax highlighting and other features switched on",
};
int	compat_choice_default = 3;
int	compat_choice;
char	*compat_text = "- run Vim %s";

char	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (<C-V>, <C-C>, etc)",
};
int	remap_choice_default = 2;
int	remap_choice;
char	*remap_text = "- %s";

char	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has a popup menu (the Windows way)",
};
int	mouse_choice_default = 2;
int	mouse_choice;
char	*mouse_text = "- The mouse %s";

char	*(shortcut_choices[]) =
{
    "\nChoose where and whether to create shortcuts to Vim:",
    "no shortcuts to Vim",
    "a shortcut to Vim on the desktop",
    "a shortcut to Vim in the Start Menu",
    "shortcuts to Vim both on the desktop and in the Start Menu",
};
int	shortcut_choice_default = 1;
int	shortcut_choice;
char	*shortcut_text = "Create %s";


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
#ifdef WIN32
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
    if (path[0] == NUL)		    /* just checking... */
	return 0;
    if (path[1] == ':')		    /* has a drive name */
    {
	if (change_drive(mytoupper(path[0]) - 'A' + 1))
	    return -1;		    /* invalid drive name */
	path += 2;
    }
    if (*path == NUL)		    /* drive name only */
	return 0;
    return chdir(path);		    /* let the normal chdir() do the rest */
}

/*
 * Expand the executable name into a full path name.
 */
#if defined(__BORLANDC__) && !defined(WIN32)

/* Only Borland C++ has this. */
# define my_fullpath(b, n, l) _fullpath(b, n, l)

#else
    static char *
my_fullpath(char *buf, char *fname, int len)
{
# ifdef WIN32
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
		if (p == fname)		/* /fname	    */
		    q = p + 1;		/* -> /		    */
		else if (q + 1 == p)	/* ... c:\foo	    */
		    q = p + 1;		/* -> c:\	    */
		else			/* but c:\foo\bar   */
		    q = p;		/* -> c:\foo	    */

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

#ifdef WIN32
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
    if (!has_gvim && !has_vim)
    {
	printf("ERROR: Cannot find vim.exe and gvim.exe in \"%s\"\n\n", installdir);
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
	    cp, tmpname, strerror(errno));
	exit(1);
    }

    farname = searchpath_save(cp);

    if (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(errno));
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
    oldvimexe = searchpath_save("vim.exe");
    oldgvimexe = searchpath_save("gvim.exe");
    mch_chdir(installdir);

    /*
     * The technique used above to set oldvimexe and oldgvimexe
     * gives a spurious result for Windows 2000 Professional.
     * w.briscoe@ponl.com 2001-01-20
     */
    findoldfile(&oldvimexe);
    findoldfile(&oldgvimexe);

    if (oldvimexe != NULL || oldgvimexe != NULL)
    {
	printf("Warning: Found a Vim executable in your $PATH:\n");
	if (oldvimexe != NULL)
	    printf("%s\n", oldvimexe);
	if (oldgvimexe != NULL)
	    printf("%s\n", oldgvimexe);
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

    if (choices[idx].arg == 0)
    {
	batname = vimbat;
	oldname = oldvimbat;
    }
    else
    {
	batname = gvimbat;
	oldname = oldgvimbat;
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
 * Select a directory to write the batch file ine.
 */
    static void
change_bat_choice(int idx)
{
    char	*path;
    char	*batname;
    char	*name;
    int		n;
    char	*s;
    char	*p;
    int		count;
    char	**names = NULL;
    int		i;

    if (choices[idx].arg == 0)
    {
	batname = vimbat;
	name = "vim.bat";
    }
    else
    {
	batname = gvimbat;
	name = "gvim.bat";
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
    names[0] = "Select directory to create (g)vim.bat in:";
    names[count] = "Do not create a (g)vim.bat file";
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
    char	*batname;
    char	*oldname;
    char	*exename;
    FILE	*fd;
    char	buf[BUFSIZE];

    if (choices[idx].arg == 0)
    {
	batname = vimbat;
	oldname = oldvimbat;
	exename = "vim.exe";
    }
    else
    {
	batname = gvimbat;
	oldname = oldgvimbat;
	exename = "gvim.exe";
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
	case 1:     fprintf(fd, "set compatible\n");
		    break;
	case 2:     fprintf(fd, "set nocompatible\n");
		    break;
	case 3:     fprintf(fd, "set nocompatible\n");
		    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
		    break;
    }
    switch (remap_choice)
    {
	case 1:	    break;
	case 2:	    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
		    break;
    }
    switch (mouse_choice)
    {
	case 1:	    fprintf(fd, "behave xterm\n");
		    break;
	case 2:	    fprintf(fd, "behave mswin\n");
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

/*
 * Add some entries to the registry to add "Edit with Vim" to the context
 * menu.
 */
    static void
install_popup(int idx)
{
    FILE	*fd;

    fd = fopen("vim.reg", "w");
    if (fd == NULL)
	printf("ERROR: Could not open vim.reg for writing\n");
    else
    {
	char	buf[BUFSIZE];

	/*
	 * Write the registry entries for the "Edit with Vim" menu.
	 */
	fprintf(fd, "REGEDIT4\n");
	fprintf(fd, "\n");
	fprintf(fd, "HKEY_CLASSES_ROOT\\CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\n");
	fprintf(fd, "@=\"Vim Shell Extension\"\n");
	fprintf(fd, "[HKEY_CLASSES_ROOT\\CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32]\n");
	double_bs(installdir, buf); /* double the backslashes */
	fprintf(fd, "@=\"%sgvimext.dll\"\n", buf);
	fprintf(fd, "\"ThreadingModel\"=\"Apartment\"\n");
	fprintf(fd, "\n");
	fprintf(fd, "[HKEY_CLASSES_ROOT\\*\\shellex\\ContextMenuHandlers\\gvim]\n");
	fprintf(fd, "@=\"{51EEE242-AD87-11d3-9C1E-0090278BBD99}\"\n");
	fprintf(fd, "\n");
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved]\n");
	fprintf(fd, "\"{51EEE242-AD87-11d3-9C1E-0090278BBD99}\"=\"Vim Shell Extension\"\n");
	fprintf(fd, "\n");
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Vim\\Gvim]\n");
	fprintf(fd, "\"path\"=\"%sgvim.exe\"\n", buf);
	fprintf(fd, "\n");

	/* The registry entries for uninstalling the menu */
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim %s]\n", VIM_VERSION_SHORT);
	fprintf(fd, "\"DisplayName\"=\"Vim %s: Edit with Vim popup menu entry\"\n", VIM_VERSION_SHORT);
	fprintf(fd, "\"UninstallString\"=\"%suninstal.exe\"\n", buf);

	fclose(fd);
	system("regedit /s vim.reg");
	/* Can't delete the file, because regedit detaches itself,
	 * thus we don't know when it is finished. */

	printf("Vim has been installed in the popup menu.\n");
	printf("Use uninstal.exe if you want to remove it again.\n");
	printf("Also see \":help win32-popup-menu\" in Vim.\n");
    }
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
#ifndef WIN32
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

#ifdef WIN32
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
   IShellLinkA	    *shelllink_ptr;
   HRESULT	    hres;
   IPersistFile	    *persistfile_ptr;

   printf("Creating shortcut at path: %s\n\n", shortcut_name);

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
 * Give the user options and get a choice on shortcut creation.
 */
    void
change_shortcut_choice(int idx)
{
    shortcut_choice = get_choice(shortcut_choices, TABLE_SIZE(shortcut_choices));
    sprintf(choices[idx].text, shortcut_text, shortcut_choices[shortcut_choice]);
}

/*
 * Get the path to a requested Windows shell folder.
 *
 * Return 0 on error, non-zero on success
 */
    int
get_shell_folder_path(char shell_folder_path[BUFSIZE], const char *shell_folder_name)
{
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
    sprintf(link_path, "%s\\%s", shell_folder_path, link_name);

    return return_val;
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
    char gvim_path [BUFSIZE];
    char link_name [BUFSIZE];
    int return_val = 0;

    /* Create the path to gvim.exe */
    sprintf(gvim_path, "%s\\gvim.exe", installdir);

    switch (shortcut_choice)
    {
	case 1: /* no shortcuts - do nothing */
	{
	    break;
	}
	case 4: /* Create shortcuts in Start Menu\Programs AND Desktop */
	{
	    /* Since we have the code to create each of those individually,
	     * just drop through and execute each one.
	     */
	}
	case 2: /* Create a shortcut on the desktop */
	{
	    /* get the name of the link to put on the desktop */
	    return_val = build_link_name(link_name, "Vim.lnk", "desktop");
	    if (return_val == 0)
	    {
		printf("A shortcut to Vim will not be created on the destop.\n");
		break;
	    }

	    /* Create the shortcut: */
	    create_shortcut(link_name, gvim_path, 0, gvim_path, "", "");

	    if (shortcut_choice == 2) /* Then we only want to create _this_ shortcut */
	    {
		break;
	    }
	    else /* We got here by drop through from above:  clear link_name to use it again */
	    {
		/* Clear link_name so we can use it again: */
		sprintf(link_name, "");
	    }
	}
	case 3: /* Create a shortcut in the Start Menu\Programs folder */
	{
	    /* get the name of the link to put on the desktop */
	    return_val = build_link_name(link_name, "Vim.lnk", "programs");
	    if (return_val == 0)
	    {
		printf("A shortcut to Vim will not be created in Start Menu\\Programs.\n");
		break;
	    }

	    /* Create the shortcut: */
	    create_shortcut(link_name, gvim_path, 0, gvim_path, "", "");
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
	    change_shortcut_choice(idx);
	    install_shortcuts(idx);
	    break;
	}
    }
}

/*
 * Add the choice for the shortcuts.
 */
    static void
init_shortcut_choice(void)
{
    choices[choice_count].text = alloc(150);
    shortcut_choice = shortcut_choice_default;
    sprintf(choices[choice_count].text, shortcut_text, shortcut_choices[shortcut_choice]);
    choices[choice_count].changefunc = change_shortcut_choice;
    choices[choice_count].installfunc = install_shortcuts;
    choices[choice_count].active = 1;
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

    sprintf(register_command_string, "%s\\gvim.exe -register", installdir);
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
#endif /* WIN32 */

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

    /* 1.  Go to the $VIM directory - check env first, then go one dir
     *	   below installdir if there is no %VIM% environment variable.	The
     *	   accuracy of $VIM is checked in inspect_system(), so we can be sure
     *	   it is ok to use here.
     */
    p = getenv("VIM");
    if (p == NULL) /* No $VIM in path */
	dir_remove_last(installdir, vimdir_path);
    else
	strcpy(vimdir_path, p);

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
init_vimfilesdir_setup(void)
{
    choices[choice_count].installfunc = install_vimfilesdir;
    choices[choice_count].active = 0;
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
	init_bat_choice("vim.bat", oldvimbat, vimbat, 0);
    else
	add_dummy_choice();

    /* (over) write gvim.bat file */
    if (oldgvimexe == NULL && has_gvim)
	init_bat_choice("gvim.bat", oldgvimbat, gvimbat, 1);
    else
	add_dummy_choice();

    /* (over) write _vimrc file */
    init_vimrc_choices();

    /* Whether to add Vim to the popup menu */
    init_popup_choice();

#ifdef WIN32
    /* Whether to add shortcuts to Vim on desktop or in Start Menu
     * Only available if gvim.exe is present (for now at least) */
    if (has_gvim)
	init_shortcut_choice();
    else
#endif
	add_dummy_choice();

#ifdef WIN32
    /* If gvim is present, it may be OLE enabled.  Since there is no easy way
     * to detect whether gvim is OLE enabled from install, we'll just give the
     * option to register it.  If it is not OLE enabled and we try to register,
     * nothing will happen, so we'll be ok.  */
    if (has_gvim)
	init_OLE_register_choice();
    else
#endif
	add_dummy_choice();

    /* ALWAYS keep the vimfilesdir_setup at the end of setup_choices.  It needs
     * to increment choice_count, but it does not have a menu entry for the
     * user, so we don't want to take up unnecessary numbers */
    init_vimfilesdir_setup();
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
	printf("\n\nInstall will do for you:\n");
	for (i = 0; i < choice_count; ++i)
	    if (choices[i].active)
		printf("%2d  %s\n", i + 1, choices[i].text);
	printf("To change an item, enter its number\n\n");
	printf("Enter item number, h (help), d (do it) or q (quit): ");
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
