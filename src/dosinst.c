/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
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
#include <direct.h>
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
#endif

#include "version.h"

#define BUFSIZE 512
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

char	vimbat[BUFSIZE];	/* name of Vim batch file to write */
char	gvimbat[BUFSIZE];	/* name of GVim batch file to write */

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

char	*(def_choices[]) =
{
    "\nChoose the default way to run Vim:",
    "Vi compatible",
    "with some Vim ehancements",
    "with syntax highlighting and other features switched on",
};
int	def_choices_default = 3;
int	def_choice;

char	*(remap_choices[]) =
{
    "\nChoose:",
    "Do not remap keys for Windows behavior",
    "Remap a few keys for Windows behavior (<C-V>, <C-C>, etc)",
};
int	remap_choice_default = 2;
int	remap_choice;

char	*(mouse_choices[]) =
{
    "\nChoose the way how Vim uses the mouse:",
    "right button extends selection (the Unix way)",
    "right button has popup menu (the Windows way)",
};
int	mouse_choice_default = 2;
int	mouse_choice;

/*
 * EMX doesn't have a global way of making open() use binary I/O.
 * Use O_BINARY for all open() calls.
 */
#if defined(__EMX__) || defined(__CYGWIN32__)
# define O_EXTRA    O_BINARY
#else
# define O_EXTRA    0
#endif

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
 * Also make sure it ends in a backslash.
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
    *d = 0;
}

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 * Eats white space first.
 */
    int
confirm(void)
{
    char	answer[10];

    return (scanf(" %c", answer) == 1 && mytoupper(answer[0]) == 'Y');
}

/*
 * Obtain a choice from a table.
 * First entry is a question, others are choices.
 */
    int
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
		printf("[%d] ", idx);
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
    temp[0] = drive + 'A' - 1;
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
 * Expand a file name into a full path name.
 */
#ifdef __BORLANDC__	/* Only Borland C++ has this */

# define my_fullpath(b, n, l) _fullpath(b, n, l)

#else
    char *
my_fullpath(char *buf, char *fname, int len)
{
# ifdef WIN32
    LPTSTR toss;

    return (char *)GetFullPathName(fname, len, buf, &toss);
# else
    char	olddir[BUFSIZE];
    char	*p, *q;
    int		c;
    char	*retval = buf;

    if (strchr(fname, ':') != NULL)	/* allready expanded */
    {
	strncpy(buf, fname, len);
	return buf;
    }

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
	    p = NULL;			/* can't get current dir: don't chdir */
	    retval = NULL;
	}
	else
	{
	    if (p == fname)		/* /fname	    */
		q = p + 1;		/* -> /		    */
	    else if (q + 1 == p)	/* ... c:\foo	    */
		q = p + 1;		/* -> c:\	    */
	    else			/* but c:\foo\bar   */
		q = p;			/* -> c:\foo	    */

	    c = *q;			/* truncate at start of fname */
	    *q = NUL;
	    if (mch_chdir(fname))	/* change to the directory */
		retval = NULL;
	    else
	    {
		fname = q;
		if (c == '\\')		/* if we cut the name at a */
		    fname++;		/* '\', don't add it again */
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
    add_pathsep(buf);
    strcat(buf, fname);
    if (p)
	mch_chdir(olddir);

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
    char *
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
    /* remove the tail, the executable name */
    for (i = strlen(installdir) - 1; i > 0; --i)
	if (installdir[i] == '/' || installdir[i] == '\\')
	{
	    installdir[i] = NUL;
	    break;
	}

    /* change to the installdir */
    mch_chdir(installdir);

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

    /* check if filetype.vim is present */
    sprintf(buf, "%s/filetype.vim", installdir);
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
 * Find out information about the system.
 */
    static void
inspect_system(void)
{
    char	*p;
    int		i;
    char	buf[BUFSIZE];
    FILE	*fd;

    /* Check if an existing setting for $VIM is correct */
    p = getenv("VIM");
    if (p != NULL)
    {
	for (i = 0; i < runtimeidx - 1; ++i)
	{
	    if (mytoupper(p[i]) == mytoupper(installdir[i]))
		continue;
	    if ((p[i] == '/' || p[i] == '\\')
		    && (installdir[i] == '/' || installdir[i] == '\\'))
		continue;

	    /* doesn't match */
	    printf("$VIM is set to \"%s\".\n", p);
	    printf("This is different from where this version of Vim is:\n");
	    strcpy(buf, installdir);
	    *(buf + runtimeidx - 1) = NUL;
	    printf("\"%s\"\n", buf);
	    printf("Please manually adjust the setting of $VIM.\n");
	    exit(1);
	}
    }

    /*
     * Check if $VIMRUNTIME is set, and it is pointing to our directory.
     */
    p = getenv("VIMRUNTIME");
    if (p != NULL)
    {
	for (i = 0; p[i] || installdir[i]; ++i)
	{
	    if (mytoupper(p[i]) == mytoupper(installdir[i]))
		continue;
	    if ((p[i] == '/' || p[i] == '\\')
		    && (installdir[i] == '/' || installdir[i] == '\\'))
		continue;

	    printf("$VIMRUNTIME is set to \"%s\".\n", p);
	    printf("This is different from where this version of Vim is:\n");
	    printf("\"%s\"\n", installdir);
	    printf("Please manually adjust the setting of $VIMRUNTIME or remove it.\n");
	    exit(1);
	}
    }

    /*
     * Check if there is a vim or gvim in the path.
     */
    mch_chdir(sysdrive);	/* avoid looking in the "installdir" */
    oldvimbat = searchpath("vim.bat");
    oldgvimbat = searchpath("gvim.bat");
    oldvimexe = searchpath("vim.exe");
    oldgvimexe = searchpath("gvim.exe");
    mch_chdir(installdir);
    if (oldvimexe != NULL || oldgvimexe != NULL)
    {
	printf("Warning: Found a Vim executable in your $PATH:\n");
	if (oldvimexe != NULL)
	    printf("%s", oldvimexe);
	if (oldgvimexe != NULL)
	    printf("%s", oldgvimexe);
	printf("It will be used instead of the version you are installing.\n");
	printf("Please adjust your $PATH setting manually.\n\n");
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

    if (choices[idx].arg == 0)
    {
	batname = vimbat;
	name = "vim.bat";
    }
    else
    {
	batname = gvimbat;
	name = "gvim.vat";
    }

    path = getenv("PATH");
    if (path == NULL)
    {
	printf("The variable $PATH is not set\n");
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
	    if (*p)
		++p;
	}
	if (names != NULL)
	    break;
	names = alloc((count + 1) * sizeof(char *));
    }
    names[0] = "Select directory to create (g)vim.bat in";
    names[count] = "Do not create a (g)vim.bat file";
    n = get_choice(names, count + 1);

    free(choices[idx].text);
    if (n == count)
    {
	*batname = NUL;
	choices[idx].text = alloc(30);
	sprintf(choices[idx].text, "Do not create a %s", name);
    }
    else
    {
	strcpy(batname, names[n]);
	add_pathsep(batname);
	strcat(batname, name);
	choices[idx].text = alloc(strlen(batname) + 20);
	sprintf(choices[idx].text, "Create %s", batname);
    }
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
	    fprintf(fd, "rem Start Vim\n");
	    strcpy(buf, installdir);
	    buf[runtimeidx] = NUL;
	    /* Don't use double quotes for the value here, also when buf
	     * contains a space.  The quotes would be included in the value
	     * for MSDOS. */
	    fprintf(fd, "set VIM=%s\n", buf);

	    add_pathsep(buf);
	    strcat(buf, exename);
	    /* Can only handle 8 arguments now.  Use "shift" to use more? */
	    fprintf(fd, "%s %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9\n", buf);

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
	choices[choice_count].text = alloc(strlen(oldname) + 20);
	sprintf(choices[choice_count].text, "Overwrite %s", oldname);
	strcpy(batname, oldname);
	choices[choice_count].changefunc = toggle_bat_choice;
    }
    else
    {
	p = getenv("PATH");
	if (p == NULL)
	    strcpy(batname, "C:/Windows");
	else
	{
	    /* TODO: use the windows directory instead of the first entry */
	    i = 0;
	    while (*p != NUL && *p != ';')
		batname[i++] = *p++;
	    batname[i] = NUL;
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

    /* If an old vimrc file exists, overwrite it.  Otherwise create a new one.
     */
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
    switch (def_choice)
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
	/* Change to NOT change or create a vimrc file. */
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
	/* Change to change or create a vimrc file. */
	if (*oldvimrc != NUL)
	{
	    choices[idx].text = alloc(strlen(oldvimrc) + 20);
	    sprintf(choices[idx].text, "Overwrite %s", oldvimrc);
	}
	else
	{
	    choices[choice_count].text = alloc(strlen(vimrc) + 20);
	    sprintf(choices[choice_count].text, "Create %s", vimrc);
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
    def_choice = get_choice(def_choices, TABLE_SIZE(def_choices));
    sprintf(choices[idx].text, "run Vim %s", def_choices[def_choice]);
}

/*
 * Change the choice if keys are to be remapped.
 */
    static void
change_remap_choice(int idx)
{
    remap_choice = get_choice(remap_choices, TABLE_SIZE(remap_choices));
    sprintf(choices[idx].text, "%s", remap_choices[remap_choice]);
}

/*
 * Change the choice how to select text.
 */
    static void
change_mouse_choice(int idx)
{
    mouse_choice = get_choice(mouse_choices, TABLE_SIZE(mouse_choices));
    sprintf(choices[idx].text, "select text %s", mouse_choices[mouse_choice]);
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
	sprintf(choices[choice_count].text, "Create %s", vimrc);
	choices[choice_count].installfunc = install_vimrc;
    }
    choices[choice_count].changefunc = change_vimrc_choice;
    choices[choice_count].active = 1;
    ++choice_count;

    /* default way to run Vim */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, "run Vim %s", def_choices[def_choices_default]);
    def_choice = def_choices_default;
    choices[choice_count].changefunc = change_run_choice;
    choices[choice_count].installfunc = NULL;;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    /* Wether to remap keys */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, "%s", remap_choices[remap_choice_default]);
    remap_choice = remap_choice_default;
    choices[choice_count].changefunc = change_remap_choice;
    choices[choice_count].installfunc = NULL;;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    /* default way to use the mouse */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, "The mouse %s", mouse_choices[mouse_choice_default]);
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
	strcpy(choices[idx].text, "Install an entry for Vim in the popup menu for the right mouse button\n    so that you can edit any file with Vim");
	choices[idx].installfunc = install_popup;
    }
    else
    {
	strcpy(choices[idx].text, "Do NOT install an entry for Vim in the popup menu for the right mouse button\n    to edit any file with Vim");
	choices[idx].installfunc = NULL;
    }
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

    /* (over) write gvim.bat file */
    if (oldgvimexe == NULL && has_gvim)
	init_bat_choice("gvim.bat", oldgvimbat, gvimbat, 1);

    /* (over) write _vimrc file */
    init_vimrc_choices();

    /* Whether to add Vim to the popup menu */
    init_popup_choice();
}

/*
 * Install the choices.
 */
    static void
install(void)
{
    int		i;

    for (i = 0; i < choice_count; ++i)
    {
	if (choices[i].installfunc != NULL)
	    (choices[i].installfunc)(i);
    }

    printf("Installation finished\n");
}

/*
 * Move file "fname" to directory "dir".
 * We actually copy the file and then delete the original, to avoid depending
 * on an external program.
 */
    void
move_file(char *fname, char *dir)
{
    struct stat	st;
    char	new_name[256];
#define COPYBUFSIZE 4096
    char	*buf;
    long	len = 0;
    int		fdr, fdw;
#ifndef __MINGW32__
    extern int	_fmode;
#endif

    /* if the file doesn't exist, silently skip it */
    if (stat(fname, &st) < 0)
	return;

    buf = alloc(COPYBUFSIZE);

    _fmode = O_BINARY;	    /* Use binary I/O */

    /* make the destination file name: "dir\fname" */
    strcpy(new_name, dir);
    add_pathsep(new_name);
    strcat(new_name, fname);

    fdr = open(fname, O_RDONLY | O_EXTRA, 0);
    if (fdr >= 0)
    {
	fdw = open(new_name, O_WRONLY|O_CREAT|O_TRUNC|O_EXTRA, 0777);
	if (fdw >= 0)
	{
	    /* copy the file. */
	    while ((len = read(fdr, buf, COPYBUFSIZE)) > 0)
		if (write(fdw, buf, len) != len)
		    break;
	    close(fdw);
	}
	close(fdr);
    }
    if (fdr < 0 || fdw < 0 || len > 0 || len < 0)
	printf("ERROR: Moving \"%s\" to \"%s\" failed\n", fname, dir);
    else
    {
	printf("%s moved to %s\n", fname, new_name);
	unlink(fname);
    }
    free(buf);
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

    /* Let the user change choices. */
    for (;;)
    {
	printf("\n\nInstall will do for you:\n");
	for (i = 1; i <= choice_count; ++i)
	    if (choices[i - 1].active)
		printf("%2d  %s\n", i, choices[i - 1].text);
	printf(" d  do it!\n");
	printf(" q  quit, don't install anything\n");
	printf("Number of item to change, d or q: ");
	if (scanf("%99s", buf) == 1)
	{
	    if (isdigit(buf[0]))
	    {
		i = atoi(buf);
		if (i > 0 && i <= choice_count && choices[i - 1].active)
		    (choices[i - 1].changefunc)(i - 1);
		else
		    printf("\nIllegal choice\n");
	    }
	    else if (buf[0] == 'q' || buf[0] == 'Q')
	    {
		printf("Exiting without anything done\n");
		break;
	    }
	    else if (buf[0] == 'd' || buf[0] == 'D')
	    {
		install();
		printf("\nThat finishes the installation.  Happy Vimming!\n");
		break;
	    }
	    else
		printf("\nIllegal choice\n");
	}
    }
    printf("\n");

    return 0;
}
