/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * dosinst.c: Install program for Vim on MS-DOS and MS-Windows
 *
 * Compile with Make_mvc.mak, Make_bc3.mak, Make_bc5.mak or Make_djg.mak.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef UNIX_LINT
# include <io.h>
# include <ctype.h>

# ifndef __CYGWIN__
#  include <direct.h>
# endif

# if defined(_WIN64) || defined(WIN32)
#  define WIN3264
#  include <windows.h>
# else
#  include <dir.h>
#  include <bios.h>
#  include <dos.h>
# endif
#endif

#ifdef UNIX_LINT
static char *searchpath(char *name);
#endif

#if defined(DJGPP) || defined(UNIX_LINT)
# include <unistd.h>
# include <errno.h>
#endif

/* shlobj.h is needed for shortcut creation */
#ifdef WIN3264
# include <shlobj.h>
#endif

#if defined(DJGPP) || defined(UNIX_LINT)
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
#define CHECK_REG_ERROR(code) if (code != ERROR_SUCCESS) { printf("%d error number:  %d\n", __LINE__, code); return; }

#include "version.h"

/*
 * Include common code for dosinst.c and uninstal.c.
 */
#include "dosinst.h"

char	installdir[BUFSIZE];	/* top of the installation dir, where the
				   install.exe is located, E.g.:
				   "c:\vim\vim60" */
int	runtimeidx;		/* index in installdir[] where "vim60" starts */
int	has_vim = 0;		/* installable vim.exe exists */
int	has_gvim = 0;		/* installable gvim.exe exists */

char	*sysdrive;		/* system drive or "c:\" */

char	oldvimrc[BUFSIZE];	/* name of existing vimrc file */
char	vimrc[BUFSIZE];		/* name of vimrc file to create */

int	interactive;		/* non-zero when running interactively */

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
int	compat_choice = (int)compat_all_enhancements;
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
int	remap_choice = (int)remap_win;
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
int	mouse_choice = (int)mouse_mswin;
char	*mouse_text = "- The mouse %s";

enum
{
    vimfiles_dir_none = 1,
    vimfiles_dir_vim,
    vimfiles_dir_home
};
static char    *(vimfiles_dir_choices[]) =
{
    "\nCreate plugin directories:",
    "No",
    "In the VIM directory",
    "In your HOME directory",
};
static int     vimfiles_dir_choice;

/*
 * Definitions of the directory name (under $VIM) of the vimfiles directory
 * and its subdirectories:
 */
static char	*(vimfiles_subdirs[]) =
{
    "colors",
    "compiler",
    "doc",
    "ftplugin",
    "indent",
    "keymap",
    "plugin",
    "syntax",
};

    static void
myexit(int n)
{
    if (!interactive)
    {
	/* Present a prompt, otherwise error messages can't be read. */
	printf("Press Enter to continue\n");
	rewind(stdin);
	getchar();
    }
    exit(n);
}

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
 * Free allocated memory if the pointer isn't NULL.
 */
    static void
vim_free(void *p)
{
    if (p != NULL)
	free(p);
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

    if (len > 0 && name[len - 1] != '\\' && name[len - 1] != '/')
	strcat(name, "\\");
}

/*
 * The normal chdir() does not change the default drive.  This one does.
 */
/*ARGSUSED*/
    int
change_drive(int drive)
{
#ifdef WIN3264
    char temp[3] = "-:";
    temp[0] = (char)(drive + 'A' - 1);
    return !SetCurrentDirectory(temp);
#else
# ifndef UNIX_LINT
    union REGS regs;

    regs.h.ah = 0x0e;
    regs.h.dl = drive - 1;
    intdos(&regs, &regs);   /* set default drive */
    regs.h.ah = 0x19;
    intdos(&regs, &regs);   /* get default drive */
    if (regs.h.al == drive - 1)
	return 0;
# endif
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
	    myexit(1);
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
	myexit(1);
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
	myexit(1);
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
	myexit(1);
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
    if (!has_gvim && !has_vim)
    {
	printf("ERROR: Cannot find any Vim executables in \"%s\"\n\n",
								  installdir);
	myexit(1);
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
 * If the executable "**destination" is in the install directory, find another
 * one in $PATH.
 * On input "**destination" is the path of an executable in allocated memory
 * (or NULL).
 * "*destination" is set to NULL or the location of the file.
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
	printf("\nERROR: %s and %s clash.  Remove or rename %s.\n",
	    tmpname, cp, tmpname);
	myexit(1);
    }

    if (rename(cp, tmpname) != 0)
    {
	printf("\nERROR: failed to rename %s to %s: %s\n",
	    cp, tmpname, strerror(0));
	myexit(1);
    }

    farname = searchpath_save(cp);

    if (rename(tmpname, cp) != 0)
    {
	printf("\nERROR: failed to rename %s back to %s: %s\n",
	    tmpname, cp, strerror(0));
	myexit(1);
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
    int		i;
    int		foundone;

    printf("Inspecting system...\n");

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
	printf("You must adjust or remove the setting of $VIM,\n");
	if (interactive)
	{
	    printf("to be able to use this install program.\n");
	    myexit(1);
	}
	printf("otherwise Vim will NOT WORK!\n");
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
	printf("You must adjust or remove the setting of $VIMRUNTIME,\n");
	if (interactive)
	{
	    printf("to be able to use this install program.\n");
	    myexit(1);
	}
	printf("otherwise Vim will NOT WORK!\n");
    }

    /*
     * Check if there is a vim or gvim in the path.
     */
    mch_chdir(sysdrive);	/* avoid looking in the "installdir" */
    for (i = 1; i < TARGET_COUNT; ++i)
    {
	targets[i].oldbat = searchpath_save(targets[i].batname);
	targets[i].oldexe = searchpath_save(targets[i].exename);
    }

    mch_chdir(installdir);

    /*
     * A .exe in the install directory may be found anyway on Windows 2000.
     * Check for this situation and find another executable if necessary.
     * w.briscoe@ponl.com 2001-01-20
     */
    foundone = 0;
    for (i = 1; i < TARGET_COUNT; ++i)
    {
	findoldfile(&(targets[i].oldexe));
	if (targets[i].oldexe != NULL)
	    foundone = 1;
    }

    if (foundone)
    {
	printf("Warning: Found Vim executable(s) in your $PATH:\n");
	for (i = 1; i < TARGET_COUNT; ++i)
	    if (targets[i].oldexe != NULL)
		printf("%s\n", targets[i].oldexe);
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

/***********************************************
 * stuff for creating the batch files.
 */

/*
 * Install the vim.bat or gvim.bat file.
 */
    static void
install_bat_choice(int idx)
{
    char	*batpath = targets[choices[idx].arg].batpath;
    char	*oldname = targets[choices[idx].arg].oldbat;
    char	*exename = targets[choices[idx].arg].exenamearg;
    char	*vimarg = targets[choices[idx].arg].exearg;
    FILE	*fd;
    char	buf[BUFSIZE];

    if (*batpath != NUL)
    {
	fd = fopen(batpath, "w");
	if (fd == NULL)
	    printf("\nERROR: Cannot open \"%s\" for writing.\n", batpath);
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
		fprintf(fd, "\"%%VIM%%\\%s\" %s%%VIMARGS%%\n", buf, vimarg);
	    else
		fprintf(fd, "%%VIM%%\\%s %s%%VIMARGS%%\n", buf, vimarg);
	    fprintf(fd, "set VIMARGS=\n");

	    fclose(fd);
	    printf("%s has been %s\n", batpath,
				 oldname == NULL ? "created" : "overwritten");
	}
    }
}

/*
 * Toggle the "Overwrite .../vim.bat" to "Don't overwrite".
 */
    static void
toggle_bat_choice(int idx)
{
    char	*batname = targets[choices[idx].arg].batpath;
    char	*oldname = targets[choices[idx].arg].oldbat;

    vim_free(choices[idx].text);
    choices[idx].text = alloc(strlen(oldname) + 24);
    if (*batname == NUL)
    {
	sprintf(choices[idx].text, "    Overwrite %s", oldname);
	strcpy(batname, oldname);
    }
    else
    {
	sprintf(choices[idx].text, "    Do NOT overwrite %s", oldname);
	*batname = NUL;
    }
}

/*
 * Do some work for a batch file entry: Append the batch file name to the path
 * and set the text for the choice.
 */
    static void
set_bat_text(int idx, char *batpath, char *name)
{
    strcat(batpath, name);

    vim_free(choices[idx].text);
    choices[idx].text = alloc(strlen(batpath) + 20);
    sprintf(choices[idx].text, "    Create %s", batpath);
}

/*
 * Select a directory to write the batch file line.
 */
    static void
change_bat_choice(int idx)
{
    char	*path;
    char	*batpath;
    char	*name;
    int		n;
    char	*s;
    char	*p;
    int		count;
    char	**names = NULL;
    int		i;
    int		target = choices[idx].arg;

    name = targets[target].batname;
    batpath = targets[target].batpath;

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
    if (choices[idx].arg == 0)
	sprintf(names[count], "Do not create any .bat file.");
    else
	sprintf(names[count], "Do not create a %s file.", name);
    n = get_choice(names, count + 1);

    if (n == count)
    {
	/* Selected last item, don't create bat file. */
	*batpath = NUL;
	if (choices[idx].arg != 0)
	{
	    free(choices[idx].text);
	    choices[idx].text = alloc(30);
	    sprintf(choices[idx].text, "    Do NOT create %s", name);
	}
    }
    else
    {
	/* Selected one of the paths.  For the first item only keep the path,
	 * for the others append the batch file name. */
	strcpy(batpath, names[n]);
	add_pathsep(batpath);
	if (choices[idx].arg != 0)
	    set_bat_text(idx, batpath, name);
    }

    for (i = 0; i <= count; ++i)
	free(names[i]);
    free(names);
}

char *bat_text_yes = "Install .bat files to use Vim at the command line:";
char *bat_text_no = "do NOT install .bat files to use Vim at the command line";

    static void
change_main_bat_choice(int idx)
{
    int		i;

    /* let the user select a default directory or NONE */
    change_bat_choice(idx);

    if (targets[0].batpath[0] != NUL)
	choices[idx].text = bat_text_yes;
    else
	choices[idx].text = bat_text_no;

    /* update the individual batch file selections */
    for (i = 1; i < TARGET_COUNT; ++i)
    {
	/* Only make it active when the first item has a path and the vim.exe
	 * or gvim.exe exists (there is a changefunc then). */
	if (targets[0].batpath[0] != NUL
		&& choices[idx + i].changefunc != NULL)
	{
	    choices[idx + i].active = 1;
	    if (choices[idx + i].changefunc == change_bat_choice
		    && targets[i].batpath[0] != NUL)
	    {
		strcpy(targets[i].batpath, targets[0].batpath);
		set_bat_text(idx + i, targets[i].batpath, targets[i].batname);
	    }
	}
	else
	    choices[idx + i].active = 0;
    }
}

/*
 * Initialize a choice for creating a batch file.
 */
    static void
init_bat_choice(int target)
{
    char	*batpath = targets[target].batpath;
    char	*oldbat = targets[target].oldbat;
    char	*p;
    int		i;

    choices[choice_count].arg = target;
    choices[choice_count].installfunc = install_bat_choice;
    choices[choice_count].active = 1;
    choices[choice_count].text = NULL;	/* will be set below */
    if (oldbat != NULL)
    {
	/* A [g]vim.bat exists: Only choice is to overwrite it or not. */
	choices[choice_count].changefunc = toggle_bat_choice;
	*batpath = NUL;
	toggle_bat_choice(choice_count);
    }
    else
    {
	/* No [g]vim.bat exists: Write it to a directory in $PATH.  Use
	 * $WINDIR by default, if it's empty the first item in $PATH. */
	p = getenv("WINDIR");
	if (p != NULL && *p != NUL)
	    strcpy(batpath, p);
	else
	{
	    p = getenv("PATH");
	    if (p == NULL || *p == NUL)		/* "cannot happen" */
		strcpy(batpath, "C:/Windows");
	    else
	    {
		i = 0;
		while (*p != NUL && *p != ';')
		    batpath[i++] = *p++;
		batpath[i] = NUL;
	    }
	}
	add_pathsep(batpath);
	set_bat_text(choice_count, batpath, targets[target].batname);

	choices[choice_count].changefunc = change_bat_choice;
    }
    ++choice_count;
}

/*
 * Set up the choices for installing .bat files.
 * For these items "arg" is the index in targets[].
 */
    static void
init_bat_choices(void)
{
    int		i;

    /* The first item is used to switch installing batch files on/off and
     * setting the default path. */
    choices[choice_count].text = bat_text_yes;
    choices[choice_count].changefunc = change_main_bat_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 1;
    choices[choice_count].arg = 0;
    ++choice_count;

    /* Add items for each batch file target.  Only used when not disabled by
     * the first item.  When a .exe exists, don't offer to create a .bat. */
    for (i = 1; i < TARGET_COUNT; ++i)
	if (targets[i].oldexe == NULL
		&& (targets[i].exenamearg[0] == 'g' ? has_gvim : has_vim))
	    init_bat_choice(i);
	else
	    add_dummy_choice();
}

/*
 * Install the vimrc file.
 */
/*ARGSUSED*/
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
    vim_free(choices[idx].text);

    if (choices[idx].installfunc != NULL)
    {
	/* Switch to NOT change or create a vimrc file. */
	if (*oldvimrc != NUL)
	{
	    choices[idx].text = alloc(strlen(oldvimrc) + 20);
	    sprintf(choices[idx].text, "Do NOT change startup file %s",
								    oldvimrc);
	}
	else
	{
	    choices[idx].text = alloc(strlen(vimrc) + 20);
	    sprintf(choices[idx].text, "Do NOT create startup file %s", vimrc);
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
	    sprintf(choices[idx].text, "Overwrite startup file %s with:",
								    oldvimrc);
	}
	else
	{
	    choices[idx].text = alloc(strlen(vimrc) + 20);
	    sprintf(choices[idx].text, "Create startup file %s with:", vimrc);
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

    /* Set opposite value and then toggle it by calling change_vimrc_choice() */
    if (*oldvimrc == NUL)
	choices[choice_count].installfunc = NULL;
    else
	choices[choice_count].installfunc = install_vimrc;
    choices[choice_count].text = NULL;
    change_vimrc_choice(choice_count);
    choices[choice_count].changefunc = change_vimrc_choice;
    choices[choice_count].active = 1;
    ++choice_count;

    /* default way to run Vim */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, compat_text,
					       compat_choices[compat_choice]);
    choices[choice_count].changefunc = change_run_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    /* Whether to remap keys */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, remap_text
					       , remap_choices[remap_choice]);
    choices[choice_count].changefunc = change_remap_choice;
    choices[choice_count].installfunc = NULL;;
    choices[choice_count].active = (*oldvimrc == NUL);
    ++choice_count;

    /* default way to use the mouse */
    choices[choice_count].text = alloc(80);
    sprintf(choices[choice_count].text, mouse_text,
						 mouse_choices[mouse_choice]);
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
uninstall_old_popups(char *current_uninstall_name)
{
    HKEY	key_handle;
    HKEY	uninstall_key_handle;
    char	*uninstall_key = "software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    char	subkey_name_buff[BUFSIZE];
    char	temp_string_buffer[BUFSIZE];
    char	uninstall_string[BUFSIZE];
    long	local_bufsize = BUFSIZE;
    FILETIME	temp_pfiletime;
    DWORD	key_index;
    char	input;
    long	code;
    long	value_type;
    long	orig_num_keys;
    long	new_num_keys;

    code = RegOpenKeyEx(HKEY_LOCAL_MACHINE, uninstall_key, 0, KEY_READ,
								 &key_handle);
    CHECK_REG_ERROR(code);

    for (key_index = 0;
        RegEnumKeyEx(key_handle, key_index, subkey_name_buff, &local_bufsize,
		    NULL, NULL, NULL, &temp_pfiletime) != ERROR_NO_MORE_ITEMS;
        key_index++)
    {
        local_bufsize = BUFSIZE;
        if (strncmp("Vim", subkey_name_buff, 3) == 0)
        {
            /* Open the key named Vim* */
            code = RegOpenKeyEx(key_handle, subkey_name_buff, 0, KEY_READ,
						       &uninstall_key_handle);
            CHECK_REG_ERROR(code);

            /* get the DisplayName out of it to show the user */
            code = RegQueryValueEx(uninstall_key_handle, "displayname", 0,
			     &value_type, temp_string_buffer, &local_bufsize);
            local_bufsize = BUFSIZE;
            CHECK_REG_ERROR(code);

            /* If this is the popup we just installed.  We don't want to
             * uninstall it now.
             */
            if (strcmp(current_uninstall_name, temp_string_buffer) == 0)
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

            printf("\nDo you want to uninstall \"%s\" now?\n(y)es (n)o)  ", temp_string_buffer);
	    fflush(stdout);

            input = 'n';
            do
            {
                if (input != 'n')
                    printf("%c is an invalid option.  Please enter either 'y' or 'n'\n", input);

                rewind(stdin);
                scanf("%c", &input);
                switch (input)
                {
                    case 'y':
                    case 'Y':
                    {
			/* save the number of uninstall keys so we can know if
			 * it changed */
                        RegQueryInfoKey(key_handle, NULL, NULL, NULL,
					     &orig_num_keys, NULL, NULL, NULL,
						      NULL, NULL, NULL, NULL);

                        /* Delete the uninstall key.  It has no subkeys, so
                         * this is easy.
                         */
                        RegDeleteKey(key_handle, subkey_name_buff);

                        /* Check if an unistall reg key was deleted.
                         * if it was, we want to decrement key_index.
                         * if we don't do this, we will skip the key
                         * immediately after any key that we delete.
                         */
                        RegQueryInfoKey(key_handle, NULL, NULL, NULL,
					      &new_num_keys, NULL, NULL, NULL,
						      NULL, NULL, NULL, NULL);
                        if (new_num_keys < orig_num_keys)
                            key_index--;
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

            } while (input != 'n' && input != 'y');
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
/*ARGSUSED*/
    static void
install_popup(int idx)
{
#if defined(DJGPP) || defined(WIN3264) || defined(UNIX_LINT)
    FILE	*fd;
    char	*cmd_path;
    char	*p;

    const char	*vim_ext_ThreadingModel = "Apartment";
    const char	*vim_ext_name = "Vim Shell Extension";
    const char	*vim_ext_clsid = "{51EEE242-AD87-11d3-9C1E-0090278BBD99}";

    char	buf[BUFSIZE];
    char	uninstall_DisplayName[BUFSIZE];

    sprintf(uninstall_DisplayName, "Vim %s: Edit with Vim popup menu entry",
							   VIM_VERSION_SHORT);

# ifdef DJGPP
    /* DJGPP cannot uninstall old versions of the popup because we have no way
     * to access the registry.  Any uninstall must be done manually.
     * Tell the user:
     */

    printf("\nThis version of install.exe was compiled using the DJGPP.\n");
    printf("install.exe is unable to uninstall old versions of the \"Edit\n");
    printf("with Vim\" popup menu automatically.  If you want to uninstall any\n");
    printf("old versions, you may do so now.  If you choose to do so at a later\n");
    printf("time, you will have to reinstall this version if you still want the\n");
    printf("menu.\n");
    printf("\nPress Enter when you are ready to install the current popup menu.\n");
    /* Clear the input buffer */
    rewind(stdin);
    getchar();
# endif /* ifdef DJGPP */

    fd = fopen("vim.reg", "w");
    if (fd == NULL)
	printf("ERROR: Could not open vim.reg for writing\n");
    else
    {
	printf("Creating \"Edit with Vim\" popup menu entry\n");

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
	fprintf(fd, "[HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim]\n");

	fprintf(fd, "\"DisplayName\"=\"%s\"\n", uninstall_DisplayName);
	fprintf(fd, "\"UninstallString\"=\"%suninstal.exe\"\n", buf);

	fclose(fd);

	/* On WinNT, 'start' is a shell built-in for cmd.exe rather than an
	 * executable (start.exe) like in Win9x.  DJGPP, being a DOS program,
	 * is given the COMSPEC command.com by WinNT, so we have to find
	 * cmd.exe manually and use it. */
         cmd_path = searchpath_save("cmd.exe");
         if (cmd_path != NULL)
         {
             /* There is a cmd.exe, so this might be Windows NT.  If it is,
              * we need to call cmd.exe explicitly.  If it is a later OS,
              * calling cmd.exe won't hurt if it is present.
              */
             /* Replace the slashes with backslashes. */
             while ((p = strchr(cmd_path, '/')) != NULL)
                 *p = '\\';
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

# ifndef DJGPP
    uninstall_old_popups(uninstall_DisplayName);
# endif

#endif /* if defined(DJGPP) || defined(WIN3264) */
}

    static void
change_popup_choice(int idx)
{
    if (choices[idx].installfunc == NULL)
    {
	choices[idx].text = "Install an entry for Vim in the popup menu for the right\n    mouse button so that you can edit any file with Vim";
	choices[idx].installfunc = install_popup;
    }
    else
    {
	choices[idx].text = "Do NOT install an entry for Vim in the popup menu for the\n    right mouse button to edit any file with Vim";
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
#ifndef WIN3264
	    && searchpath("regedit.exe") != NULL
#endif
       )
    {
	choices[choice_count].changefunc = change_popup_choice;
	choices[choice_count].installfunc = NULL;
	choices[choice_count].active = 1;
	change_popup_choice(choice_count);  /* set the text */
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
      return FAIL;
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
      hres = shelllink_ptr->lpVtbl->QueryInterface(shelllink_ptr,
			       &IID_IPersistFile, (void **) &persistfile_ptr);

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
	 persistfile_ptr->lpVtbl->Save(persistfile_ptr, wsz, 1);
	 persistfile_ptr->lpVtbl->Release(persistfile_ptr);
      }
      else
      {
	 printf("QueryInterface Error\n");
	 return FAIL;
      }

      /* Return the ShellLink object */
      shelllink_ptr->lpVtbl->Release(shelllink_ptr);
   }
   else
   {
      printf("CoCreateInstance Error - hres = %08x\n", hres);
      return FAIL;
   }

   return OK;
}

/*
 * Build a path to where we will put a specified link.
 *
 * Return 0 on error, non-zero on success
 */
   int
build_link_name(
	char	   *link_path,
	const char *link_name,
	const char *shell_folder_name)
{
    char	shell_folder_path[BUFSIZE];

    if (get_shell_folder_path(shell_folder_path, shell_folder_name) == FAIL)
    {
	printf("An error occurred while attempting to find the path to %s.\n",
							   shell_folder_name);
	return FAIL;
    }

    /* Make sure the directory exists (create Start Menu\Programs\Vim).
     * Ignore errors if it already exists. */
    vim_mkdir(shell_folder_path, 0755);

    /* build the path to the shortcut and the path to gvim.exe */
    sprintf(link_path, "%s\\%s.lnk", shell_folder_path, link_name);

    return OK;
}

    int
build_shortcut(
	const char *name,	/* Name of the shortcut */
	const char *exename,	/* Name of the executable (e.g., vim.exe) */
	const char *args,
	const char *shell_folder)
{
    char	executable_path[BUFSIZE];
    char	link_name[BUFSIZE];

    sprintf(executable_path, "%s\\%s", installdir, exename);

    if (build_link_name(link_name, name, shell_folder) == FAIL)
    {
	printf("An error has occurred.  A shortcut to %s will not be created %s.\n",
		name,
		*shell_folder == 'd' ? "on the desktop" : "in the Start menu");
	return FAIL;
    }

    /* Create the shortcut: */
    return create_shortcut(link_name, executable_path, 0,
						   executable_path, args, "");
}

/*
 * Create shortcut(s) in the Start Menu\Programs\Vim folder.
 */
    static void
install_start_menu(int idx)
{
    if (has_vim)
    {
	if (build_shortcut("Vim", "vim.exe", "", VIM_STARTMENU) == FAIL)
	    return;
	if (build_shortcut("View", "vim.exe", "-R", VIM_STARTMENU) == FAIL)
	    return;
	if (build_shortcut("Vimdiff", "vim.exe", "-d", VIM_STARTMENU) == FAIL)
	    return;
    }
    if (has_gvim)
    {
	if (build_shortcut("GVim", "gvim.exe", "", VIM_STARTMENU) == FAIL)
	    return;
	if (build_shortcut("EVim", "gvim.exe", "-y", VIM_STARTMENU) == FAIL)
	    return;
	if (build_shortcut("GView", "gvim.exe", "-R", VIM_STARTMENU) == FAIL)
	    return;
	if (build_shortcut("GVimdiff", "gvim.exe", "-d", VIM_STARTMENU) == FAIL)
	    return;
    }
    if (build_shortcut("Uninstall", "uninstal.exe", "", VIM_STARTMENU) == FAIL)
	return;
    if (build_shortcut("Vim tutor", "vimtutor.bat", "", VIM_STARTMENU) == FAIL)
	return;
}

    static void
toggle_startmenu_choice(int idx)
{
    if (choices[idx].installfunc == NULL)
    {
	choices[idx].installfunc = install_start_menu;
	choices[idx].text = "Add Vim to the Start menu";
    }
    else
    {
	choices[idx].installfunc = NULL;
	choices[idx].text = "Do NOT add Vim to the Start menu";
    }
}

/*
 * Function to actually create the shortcuts
 *
 * Currently I am supplying no working directory to the shortcut.  This
 *    means that the initial working dir will be:
 *    - the location of the shortcut if no file is supplied
 *    - the location of the file being edited if a file is supplied (ie via
 *      drag and drop onto the shortcut).
 */
    void
install_shortcut_gvim(int idx)
{
    /* Create shortcut(s) on the desktop */
    if (choices[idx].arg)
	(void)build_shortcut(icon_names[0], "gvim.exe", "", "desktop");
}

    void
install_shortcut_evim(int idx)
{
    if (choices[idx].arg)
	(void)build_shortcut(icon_names[1], "gvim.exe", "-y", "desktop");
}

    void
install_shortcut_gview(int idx)
{
    if (choices[idx].arg)
	(void)build_shortcut(icon_names[2], "gvim.exe", "-R", "desktop");
}

    void
toggle_shortcut_choice(int idx)
{
    if (choices[idx].arg)
    {
	choices[idx].arg = 0;
	strcpy(choices[idx].text, "Do NOT create a desktop icon for ");
    }
    else
    {
	choices[idx].arg = 1;
	strcpy(choices[idx].text, "Create a desktop icon for ");
    }
    if (choices[idx].installfunc == install_shortcut_gvim)
	strcat(choices[idx].text, "gvim");
    else if (choices[idx].installfunc == install_shortcut_evim)
	strcat(choices[idx].text, "evim (Easy Vim)");
    else
	strcat(choices[idx].text, "gview (readonly gvim)");
}
#endif /* WIN3264 */

    static void
init_startmenu_choice(void)
{
#ifdef WIN3264
    /* Start menu */
    choices[choice_count].changefunc = toggle_startmenu_choice;
    choices[choice_count].installfunc = NULL;
    choices[choice_count].active = 1;
    toggle_startmenu_choice(choice_count);	/* set the text */
    ++choice_count;
#else
    add_dummy_choice();
#endif
}

/*
 * Add the choice for the desktop shortcuts.
 */
    static void
init_shortcut_choices(void)
{
#ifdef WIN3264
    /* Shortcut to gvim */
    choices[choice_count].text = alloc(80);
    choices[choice_count].arg = 0;
    choices[choice_count].active = has_gvim;
    choices[choice_count].changefunc = toggle_shortcut_choice;
    choices[choice_count].installfunc = install_shortcut_gvim;
    toggle_shortcut_choice(choice_count);
    ++choice_count;

    /* Shortcut to evim */
    choices[choice_count].text = alloc(80);
    choices[choice_count].arg = 0;
    choices[choice_count].active = has_gvim;
    choices[choice_count].changefunc = toggle_shortcut_choice;
    choices[choice_count].installfunc = install_shortcut_evim;
    toggle_shortcut_choice(choice_count);
    ++choice_count;

    /* Shortcut to gview */
    choices[choice_count].text = alloc(80);
    choices[choice_count].arg = 0;
    choices[choice_count].active = has_gvim;
    choices[choice_count].changefunc = toggle_shortcut_choice;
    choices[choice_count].installfunc = install_shortcut_gview;
    toggle_shortcut_choice(choice_count);
    ++choice_count;
#else
    add_dummy_choice();
    add_dummy_choice();
    add_dummy_choice();
#endif
}

#ifdef WIN3264
/*
 * Attempt to register OLE for Vim.
 */
   static void
install_OLE_register(void)
{
    char register_command_string[BUFSIZE + 20];

    printf("\n--- Attempting to register Vim with OLE ---\n");
    printf("A message box will appear if gvim is OLE enabled.\n");
    printf("To finish installation, click the OK button in the message box.\n");
    printf("If no message box appears, your gvim.exe is not OLE enabled.\n");

#ifndef __CYGWIN__
    sprintf(register_command_string, "\"%s\\gvim.exe\" -register", installdir);
#else
    /* handle this differently for Cygwin which sometimes has trouble with
     * Windows-style pathnames here. */
    sprintf(register_command_string, "./gvim.exe -register", installdir);
#endif
    system(register_command_string);
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

    strncpy(buffer, path, (size_t)last_char_to_copy);
    buffer[last_char_to_copy] = NUL;
}

    static void
set_directories_text(int idx)
{
    if (vimfiles_dir_choice == (int)vimfiles_dir_none)
	sprintf(choices[idx].text, "Do NOT create plugin directories");
    else
	sprintf(choices[idx].text, "Create plugin directories: %s",
				   vimfiles_dir_choices[vimfiles_dir_choice]);
}

/*
 * Change the directory that the vim plugin directories will be created in:
 * $HOME, $VIM or nowhere.
 */
    static void
change_directories_choice(int idx)
{
    int	    choice_count = TABLE_SIZE(vimfiles_dir_choices);

    /* Don't offer the $HOME choice if $HOME isn't set. */
    if (getenv("HOME") == NULL)
	--choice_count;
    vimfiles_dir_choice = get_choice(vimfiles_dir_choices, choice_count);
    set_directories_text(idx);
}

/*
 * Create the plugin directories...
 */
/*ARGSUSED*/
    static void
install_vimfilesdir(int idx)
{
    int i;
    char *p;
    char vimdir_path[BUFSIZE];
    char vimfiles_path[BUFSIZE];
    char tmp_dirname[BUFSIZE];

    /* switch on the location that the user wants the plugin directories
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
	    /* Find the $HOME directory.  Its existence was already checked. */
	    p = getenv("HOME");
	    if (p == NULL)
	    {
		printf("Internal error: $HOME is NULL\n");
		p = "c:\\";
	    }
	    strcpy(vimdir_path, p);
	    break;
	}
	case vimfiles_dir_none:
	{
	    /* Do not create vim plugin directory */
	    return;
	}
    }

    /* Now, just create the directory.	If it already exists, it will fail
     * silently.  */
    sprintf(vimfiles_path, "%s\\vimfiles", vimdir_path);
    vim_mkdir(vimfiles_path, 0755);

    printf("Now creating the following directories:\n");
    for (i = 0; i < TABLE_SIZE(vimfiles_subdirs); i++)
    {
	sprintf(tmp_dirname, "%s\\%s", vimfiles_path, vimfiles_subdirs[i]);
	printf("    %s\n", tmp_dirname);
	vim_mkdir(tmp_dirname, 0755);
    }
}

/*
 * Add the creation of runtime files to the setup sequence.
 */
    static void
init_directories_choice(void)
{
    struct stat	st;
    char	tmp_dirname[BUFSIZE];
    char	*p;

    choices[choice_count].text = alloc(150);
    choices[choice_count].changefunc = change_directories_choice;
    choices[choice_count].installfunc = install_vimfilesdir;
    choices[choice_count].active = 1;

    /* Check if the "compiler" directory already exists.  That's a good
     * indication that the plugin directories were already created. */
    if (getenv("HOME") != NULL)
    {
	vimfiles_dir_choice = (int)vimfiles_dir_home;
	sprintf(tmp_dirname, "%s\\vimfiles\\compiler", getenv("HOME"));
	if (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (int)vimfiles_dir_none;
    }
    else
    {
	vimfiles_dir_choice = (int)vimfiles_dir_vim;
	p = getenv("VIM");
	if (p == NULL) /* No $VIM in path, use the install dir */
	    dir_remove_last(installdir, tmp_dirname);
	else
	    strcpy(tmp_dirname, p);
	strcat(tmp_dirname, "\\vimfiles\\compiler");
	if (stat(tmp_dirname, &st) == 0)
	    vimfiles_dir_choice = (int)vimfiles_dir_none;
    }

    set_directories_text(choice_count);
    ++choice_count;
}

/*
 * Setup the choices and the default values.
 */
    static void
setup_choices(void)
{
    /* install the batch files */
    init_bat_choices();

    /* (over) write _vimrc file */
    init_vimrc_choices();

    /* Whether to add Vim to the popup menu */
    init_popup_choice();

    /* Whether to add Vim to the Start Menu. */
    init_startmenu_choice();

    /* Whether to add shortcuts to the Desktop. */
    init_shortcut_choices();

    /* Whether to create the runtime directories. */
    init_directories_choice();
}

    static void
print_cmd_line_help(void)
{
    printf("Vim installer non-interactive command line arguments:\n");
    printf("\n");
    printf("-create-batfiles  [vim gvim evim view gview vimdiff gvimdiff]\n");
    printf("    Create .bat files for Vim variants in the Windows directory.\n");
    printf("-create-vimrc\n");
    printf("    Create a default _vimrc file if one does not already exist.\n");
    printf("-install-popup\n");
    printf("    Install the Edit-with-Vim context menu entry\n");
#ifdef WIN3264
    printf("-add-start-menu");
    printf("    Add Vim to the start menu\n");
    printf("-create-shortcuts");
    printf("    Create shortcuts to all Vim executables on the desktop\n");
#endif
    printf("-create-directories [vim|home]\n");
    printf("    Create runtime directories to drop plugins into; in the $VIM\n");
    printf("    or $HOME directory\n");
#ifdef WIN3264
    printf("-register-OLE");
    printf("    Register gvim for OLE\n");
#endif
    printf("\n");
}

/*
 * Setup installation choices based on command line switches
 */
    static void
command_line_setup_choices(int argc, char **argv)
{
    int i, j;

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-create-batfiles") == 0)
        {

            if (i + 1 == argc)
                continue;
            while (argv[i + 1][0] != '-' && i < argc)
            {
                i++;
		for (j = 1; j < TARGET_COUNT; ++j)
		    if ((targets[j].exenamearg[0] == 'g' ? has_gvim : has_vim)
			    && strcmp(argv[i], targets[j].name) == 0)
		    {
			init_bat_choice(j);
			break;
		    }
		if (j == TARGET_COUNT)
                    printf("%s is not a valid choice for -create-batfiles\n",
								     argv[i]);

                if (i + 1 == argc)
                    break;
            }
        }
	else if (strcmp(argv[i], "-create-vimrc") == 0)
        {
            /* Setup default vimrc choices.  If there is already a _vimrc file,
             * it will NOT be overwritten.
             */
            init_vimrc_choices();
        }
        else if (strcmp(argv[i], "-install-popup") == 0)
        {
            init_popup_choice();
        }
        else if (strcmp(argv[i], "-add-start-menu") == 0)
	{
	    init_startmenu_choice();
	}
        else if (strcmp(argv[i], "-create-shortcuts") == 0)
        {
            init_shortcut_choices();
        }
        else if (strcmp(argv[i], "-create-directories") == 0)
        {
            init_directories_choice();
            if (argv[i + 1][0] != '-')
            {
                i++;
                if (strcmp(argv[i], "vimdir") == 0)
                    vimfiles_dir_choice = (int)vimfiles_dir_vim;
                else if (strcmp(argv[i], "home") == 0)
                {
                    if (getenv("HOME") == NULL) /* No $HOME in environment */
                        vimfiles_dir_choice = (int)vimfiles_dir_vim;
                    else
                        vimfiles_dir_choice = (int)vimfiles_dir_home;
                }
            }
            else /* No choice specified, default to vim directory */
                vimfiles_dir_choice = (int)vimfiles_dir_vim;
        }
#ifdef WIN3264
        else if (strcmp(argv[i], "-register-OLE") == 0)
        {
	    /* This is always done when gvim is found */
        }
#endif
        else /* Unknown switch */
        {
            printf("Got unknown argument argv[%d] = %s\n", i, argv[i]);
            print_cmd_line_help();
        }
    }
}


/*
 * Show a few screens full of helpful information.
 */
    static void
show_help(void)
{
    static char *(items[]) =
    {
"Installing .bat files\n"
"---------------------\n"
"The vim.bat file is written in one of the directories in $PATH.\n"
"This makes it possible to start Vim from the command line.\n"
"If vim.exe can be found in $PATH, the choice for vim.bat will not be\n"
"present.  It is assumed you will use the existing vim.exe.\n"
"If vim.bat can already be found in $PATH this is probably for an old\n"
"version of Vim (but this is not checked!).  You can overwrite it.\n"
"If no vim.bat already exists, you can select one of the directories in\n"
"$PATH for creating the batch file, or disable creating a vim.bat file.\n"
"\n"
"If you choose not to create the vim.bat file, Vim can still be executed\n"
"in other ways, but not from the command line.\n"
"\n"
"The same applies to choices for gvim, evim, (g)view, and (g)vimdiff.\n"
"The first item can be used to change the path for all of them.\n"
,
"Creating a _vimrc file\n"
"----------------------\n"
"The _vimrc file is used to set options for how Vim behaves.\n"
"The install program can create a _vimrc file with a few basic choices.\n"
"You can edit this file later to tune your preferences.\n"
"If you already have a _vimrc or .vimrc file it can be overwritten.\n"
"Don't do that if you have made changes to it.\n"
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
"Edit-with-Vim context menu entry\n"
"--------------------------------\n"
"(this choice is only available when gvim.exe and gvimext.dll are present)\n"
"You can associate different file types with Vim, so that you can (double)\n"
"click on a file to edit it with Vim.  This means you have to individually\n"
"select each file type.\n"
"An alternative is the option offered here: Install an \"Edit with Vim\"\n"
"entry in the popup menu for the right mouse button.  This means you can\n"
"edit any file with Vim\n"
,
"Add Vim to the Start menu\n"
"-------------------------\n"
"In Windows 95 and later, Vim can be added to the Start menu.  This will\n"
"create a submenu with an entry for vim, gvim, evim, vimdiff, etc..\n"
,
"Icons on the desktop\n"
"--------------------\n"
"(these choices are only available when installing gvim)\n"
"In Windows 95 and later, shortcuts (icons) can be created on the Desktop.\n"
,
"Create plugin directories\n"
"-------------------------\n"
"Plugin directories allow extending Vim by dropping a file into a directory.\n"
"This choice allows creating them in $HOME (if you have a home directory) or\n"
"$VIM (used for everybody on the system).\n"
,
NULL
    };
    int		i;
    int		c;

    rewind(stdin);
    printf("\n");
    for (i = 0; items[i] != NULL; ++i)
    {
	printf(items[i]);
	printf("\n");
	printf("Hit Enter to continue, b (back) or q (quit help): ");
	c = getchar();
	rewind(stdin);
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
	if (choices[i].installfunc != NULL && choices[i].active)
	    (choices[i].installfunc)(i);
#ifdef WIN3264
    if (has_gvim)
	install_OLE_register();
#endif
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

    /*
     * Run interactively if there are no command line arguments.
     */
    if (argc > 1)
	interactive = 0;
    else
	interactive = 1;

    /* Initialize this program. */
    do_inits(argv);

    printf("This program sets up the installation of Vim %s\n\n",
	    VIM_VERSION_MEDIUM);

    /* Check if the user unpacked the archives properly. */
    check_unpack();

    /* Find out information about the system. */
    inspect_system();

    if (argc == 1)
    {
        /* Setup all the choices. */
        setup_choices();

        /* Let the user change choices and finally install (or quit). */
        for (;;)
        {
            request_choice();
	    rewind(stdin);
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
    }
    else
    {
	/*
	 * Run non-interactive - setup according to the command line switches
	 */
        command_line_setup_choices(argc, argv);
        install();
    }

    myexit(0);
    /*NOTREACHED*/
    return 0;
}
