/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * install.c: Minimalistic install program for Vim on MS-DOS/Windows
 *
 * Compile with Makefile.w32, Makefile.bcc or Makefile.djg.
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
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    int
confirm(void)
{
    char	answer[10];

    return (scanf(" %c", answer) == 1 && toupper(answer[0]) == 'Y');
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
 * Append a line to the autoexec.bat file.
 */
    void
append_autoexec(char *s, char *v)
{
    FILE    *fd;

    fd = fopen("c:\\autoexec.bat", "a");
    if (fd == NULL)
    {
	printf("ERROR: Cannot open c:\\autoexec.bat for appending\n");
	exit(1);
    }
    fprintf(fd, s, v);
    fclose(fd);
    printf("This line has been appended to c:\\autoexec.bat:\n");
    printf(s, v);
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
	if (change_drive(tolower(path[0]) - 'a' + 1))
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
    int		l;
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
    l = strlen(buf);
    if (l && buf[l - 1] != '/' && buf[l - 1] != '\\')
	strcat(buf, "/");
    if (p)
	mch_chdir(olddir);
    strcat(buf, fname);

    return retval;
# endif
}
#endif

#ifdef WIN32
/* This symbol is not defined in older versions of the SDK or Visual C++ */

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

static void PlatformId(void);
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
#endif

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

    buf = malloc(COPYBUFSIZE);
    if (buf == NULL)
    {
	printf("ERROR: Out of memory!\n");
	return;
    }

    _fmode = O_BINARY;	    /* Use binary I/O */

    /* make the destination file name: "dir\fname" */
    strcpy(new_name, dir);
    if (dir[strlen(dir) - 1] != '\\' && dir[strlen(dir) - 1] != '/')
	strcat(new_name, "\\");
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

/*
 * Ask for directory from $PATH to move the .exe files to.
 */
    char *
move_to_path(char *exedir)
{
    char	*path;
    char	**names = NULL;
    char	*p, *s;
    int		count;
    int		idx;

    path = getenv("PATH");
    if (path == NULL)
    {
	printf("ERROR: The variable $PATH is not set\n");
	return exedir;
    }

    /*
     * first round: count number of names in path;
     * second round: save names to path[].
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
		names[count] = malloc(s - p + 1);
		if (names[count] == NULL)
		{
		    printf("ERROR: out of memory\n");
		    exit(1);
		}
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
	names = malloc(count * sizeof(char *));
	if (names == NULL)
	{
	    printf("ERROR: out of memory\n");
	    exit(1);
	}
    }
    names[0] = "Select directory to move Vim executables to";
    idx = get_choice(names, count);

    printf("\nYou have selected the directory:\n");
    printf(names[idx]);
    printf("\nDo you want to move the Vim executables there? (Y/N) ");
    if (!confirm())
	printf("Skipping moving Vim executables\n");
    else
    {
	move_file("vim.exe", names[idx]);
	move_file("gvim.exe", names[idx]);
	move_file("xxd.exe", names[idx]);
	move_file("ctags.exe", names[idx]);
	move_file("vimrun.exe", names[idx]);
	exedir = "";	/* exe is now in path, don't need a dir */
    }

    return exedir;	/* return dir name where exe is */
}

/*
 * Copy a directory name from "dir" to "buf", doubling backslashes.
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

#define TABLE_SIZE(s)	sizeof(s) / sizeof(char *)

    int
main(int argc, char **argv)
{
    char	*(def_choices[]) =
    {
	"\nChoose the default way to run Vim:",
	"conventional Vim setup",
	"with syntax highlighting and other features switched on",
	"Vi compatible",
    };
    char	*(select_choices[]) =
    {
	"\nChoose the way how text is selected:",
	"with Visual mode (the Unix way)",
	"with Select mode (the Windows way)",
	"mouse with Select mode, keys with Visual mode",
    };
    char	*(exe_choices[]) =
    {
	"\nChoose the way find the Vim executables:",
	"adjust $PATH in c:\\autoexec.bat",
	"move executables to a directory already in $PATH",
	"do nothing (later use a full path to run Vim)",
    };
    int		def;
    int		select;
    int		exe;
    FILE	*fd;
    char	vimdir[BUFSIZE];
    char	*exedir = "";		/* dir name where exe can be found */
    char	vimrc[BUFSIZE];
    int		found_vimrc, skip_vimrc;
    int		len;
    int		i;
    char	*p;
    int		vimdirend;
    int		need_vimvar = 1;	/* need to set $VIM */
    int		has_gvim = 0;		/* gvim.exe exists */

#ifdef DJGPP
    /*
     * Use Long File Names by default, if $LFN not set.
     */
    if (getenv("LFN") == NULL)
	putenv("LFN=y");
#endif

    printf("This program sets up the installation of Vim %s\n\n",
	    VIM_VERSION_MEDIUM);

    /* Find out the full path of our executable. */
    if (my_fullpath(vimdir, argv[0], BUFSIZE) == NULL)
    {
	printf("ERROR: Cannot get name of executable\n");
	exit(1);
    }
    /* remove the tail, the executable name */
    for (i = strlen(vimdir) - 1; i > 0; --i)
	if (vimdir[i] == '/' || vimdir[i] == '\\')
	{
	    vimdir[i] = NUL;
	    break;
	}
    /* check for presence of the version number in vimdir[] */
    len = strlen(VIM_VERSION_NODOT);
    vimdirend = strlen(vimdir) - len;
    if (vimdirend <= 0 || stricmp(vimdir + vimdirend, VIM_VERSION_NODOT) != 0
	    || (vimdir[vimdirend - 1] != '/' && vimdir[vimdirend - 1] != '\\'))
    {
	printf("ERROR: Install program not in directory \"%s\"\n",
		VIM_VERSION_NODOT);
	printf("This program can only work when it is located in its original directory\n");
	exit(1);
    }

    /* Check if an existing setting for $VIM is correct */
    p = getenv("VIM");
    if (p != NULL)
    {
	for (i = 0; i < vimdirend - 1; ++i)
	{
	    if (toupper(p[i]) == toupper(vimdir[i]))
		continue;
	    if ((p[i] == '/' || p[i] == '\\')
		    && (vimdir[i] == '/' || vimdir[i] == '\\'))
		continue;

	    printf("$VIM is set to \"%s\".\n", p);
	    printf("This is different from where this version of Vim is:\n");
	    strcpy(vimrc, vimdir);
	    *(vimrc + vimdirend - 1) = NUL;
	    printf("\"%s\"\n", vimrc);
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
	for (i = 0; p[i] || vimdir[i]; ++i)
	{
	    if (toupper(p[i]) == toupper(vimdir[i]))
		continue;
	    if ((p[i] == '/' || p[i] == '\\')
		    && (vimdir[i] == '/' || vimdir[i] == '\\'))
		continue;

	    printf("$VIMRUNTIME is set to \"%s\".\n", p);
	    printf("This is different from where this version of Vim is:\n");
	    printf("\"%s\"\n", vimdir);
	    printf("Please manually adjust the setting of $VIMRUNTIME or remove it.\n");
	    exit(1);
	}
    }

    /* change to the vimdir */
    mch_chdir(vimdir);

    /*
     * Check if vim.exe or gvim.exe is in the current directory.
     */
    if ((fd = fopen("gvim.exe", "r")) != NULL)
    {
	fclose(fd);
	has_gvim = 1;
    }
    else if ((fd = fopen("vim.exe", "r")) != NULL)
	fclose(fd);
    else
    {
	printf("ERROR: Cannot find vim.exe or gvim.exe in \"%s\"\n\n", vimdir);
	exit(1);
    }

    /*
     * Ask the user if he really wants to install Vim.
     */
    printf("It prepares the _vimrc file, $VIM and the executables.\n");
    printf("Do you want to continue? (Y/N) ");
    if (!confirm())
	exit(0);

    /*
     * Check if there is an existing ../_vimrc or ../.vimrc file.
     */
    /* create the name of the _vimrc file */
    strcpy(vimrc, vimdir);
    strcpy(vimrc + vimdirend, "_vimrc");
    found_vimrc = 0;
    skip_vimrc = 0;
    if ((fd = fopen(vimrc, "r")) == NULL)
    {
	strcpy(vimrc + vimdirend, "vimrc~1");	/* short version of .vimrc */
	if ((fd = fopen(vimrc, "r")) == NULL)
	{
	    strcpy(vimrc + vimdirend, ".vimrc");
	    fd = fopen(vimrc, "r");
	}
    }
    if (fd != NULL)
    {
	fclose(fd);
	found_vimrc = 1;
	printf("\nThere already exists a _vimrc file: \"%s\"", vimrc);
	printf("\nDo you want to overwrite it? (Y/N) ");
	if (!confirm())
	{
	    printf("Skipping writing of _vimrc\n");
	    skip_vimrc = 1;
	}
    }
    else
	strcpy(vimrc + vimdirend, "_vimrc");

    if (!skip_vimrc)
    {
	/*
	 * Ask for contents of _vimrc.
	 */
	def = get_choice(def_choices, TABLE_SIZE(def_choices));
	select = get_choice(select_choices, TABLE_SIZE(select_choices));

	printf("\nYou have chosen:\n");
	printf("[%d] %s\n", def, def_choices[def]);
	printf("[%d] %s\n", select, select_choices[select]);
	printf("(You can adjust your _vimrc file afterwards)\n");
	printf("\nDo you want to %swrite the file \"%s\"? (Y/N) ",
		found_vimrc ? "over" : "", vimrc);
	if (!confirm())
	    printf("Skipping writing of _vimrc\n");
	else
	{
	    fd = fopen(vimrc, "w");
	    if (fd == NULL)
	    {
		printf("\nERROR: Cannot open \"%s\" for writing.\n", vimrc);
		exit(1);
	    }
	    switch (def)
	    {
		case 1:     fprintf(fd, "set nocompatible\n");
			    break;
		case 2:     fprintf(fd, "set nocompatible\n");
			    fprintf(fd, "source $VIMRUNTIME/vimrc_example.vim\n");
			    break;
		case 3:     fprintf(fd, "set compatible\n");
			    break;
	    }
	    switch (select)
	    {
		case 1:	    fprintf(fd, "behave xterm\n");
			    break;
		case 2:	    fprintf(fd, "source $VIMRUNTIME/mswin.vim\n");
			    break;
		case 3:	    fprintf(fd, "behave xterm\n");
			    fprintf(fd, "set selectmode=mouse\n");
			    break;
	    }
	    fclose(fd);
	    printf("%s has been written\n", vimrc);
	}
    }

    /*
     * Set PATH or move executables, unless it's already in the $PATH.
     */
    mch_chdir("C:\\");	/* avoid looking in the "vimrdir" directory */
    p = searchpath("vim.exe");
    if (p == NULL)
	p = searchpath("gvim.exe");
    mch_chdir(vimdir);
    if (p != NULL)
    {
	printf("vim.exe or gvim.exe is already in $PATH: \"%s\"\n", p);
	printf("Please adjust your $PATH setting manually\n");
    }
    else
    {
	printf("\nTo be able to run Vim it must be in your $PATH.");
	exe = get_choice(exe_choices, TABLE_SIZE(exe_choices));
	switch (exe)
	{
	    case 1: append_autoexec("set PATH=%%PATH%%;%s\n", vimdir);
		    need_vimvar = 0;
		    break;

	    case 2: exedir = move_to_path(vimdir);
		    break;

	    case 3: printf("Skipping setting $PATH\n");
		    exedir = vimdir;
		    break;
	}
    }

    /*
     * Set $VIM, if it hasn't been set yet.
     */
    if (need_vimvar && getenv("VIM") == NULL)
    {
	printf("\nI can append a command to c:\\autoexec.bat to set $VIM.\n");
	printf("(this will not work if c:\\autoexec.bat contains sections)\n");
	printf("Do you want me to append to your c:\\autoexec.bat? (Y/N) ");
	if (!confirm())
	    printf("Skipping appending to c:\\autoexec.bat\n");
	else
	{
	    vimrc[vimdirend - 1] = NUL;
	    append_autoexec("set VIM=%s\n", vimrc);
	}
    }

    /*
     * Add some entries to the registry to add "Edit with Vim" to the context
     * menu.
     * Only do this when gvim.exe was found and regedit.exe exists.
     */
    if (has_gvim
#ifndef WIN32
	    && searchpath("regedit.exe") != NULL
#endif
       )
    {
	printf("\nI can install an entry in the popup menu for the right\n");
	printf("mouse button, so that you can edit any file with Vim.\n");
	printf("(NOTE: The MS-Office toolbar has a problem with this!)\n");
	printf("Do you want me to do this? (Y/N) ");
	if (!confirm())
	    printf("didn't change popup menu\n");
	else
	{
	    fd = fopen("vim.reg", "w");
	    if (fd == NULL)
		printf("ERROR: Could not open vim.reg for writing\n");
	    else
	    {
		char	buf[BUFSIZE];

		/* The registry entries for the "Edit with Vim" menu */
		fprintf(fd, "REGEDIT4\n\n");
		fprintf(fd, "[HKEY_CLASSES_ROOT\\*\\shell\\Vim]\n");
		fprintf(fd, "@=\"Edit with &Vim\"\n\n");
		fprintf(fd, "[HKEY_CLASSES_ROOT\\*\\shell\\Vim\\command]\n");
		double_bs(exedir, buf); /* double the backslashes */
		fprintf(fd, "@=\"%sgvim.exe \\\"%%L\\\"\"\n\n", buf);

		/* The registry entries for uninstalling the menu */
		fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\vim %s]\n", VIM_VERSION_SHORT);
		fprintf(fd, "\"DisplayName\"=\"Vim %s: Edit with Vim popup menu entry\"\n", VIM_VERSION_SHORT);
		double_bs(vimdir, buf); /* double the backslashes */
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
    }

    printf("\nThat finishes the installation.  Happy Vimming!\n");
    return 0;
}
