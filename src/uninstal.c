/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * uninstal.c:	Minimalistic uninstall program for Vim on MS-Windows
 *		Removes:
 *		- the "Edit with Vim" popup menu entry
 *		- any Vim Batch files in the path
 *		- icons for Vim on the Desktop
 *		- the Vim entry in the Start Menu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#if defined(WIN32) || defined(_WIN64)
# define WIN3264
# include <windows.h>
# include <shlobj.h>
#else
#  include <dir.h>
#  include <bios.h>
#  include <dos.h>
#endif

#include "version.h"

/* Include common code for dosinst.c and uninstal.c. */
#include "dosinst.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    static int
confirm(void)
{
    char	answer[10];

    fflush(stdout);
    return (scanf(" %c", answer) == 1 && toupper(answer[0]) == 'Y');
}

#ifdef WIN3264
/*
 * Check if the popup menu entry exists and what gvim it refers to.
 * Returns non-zero when it's found.
 */
    static int
popup_gvim_path(char *buf)
{
    HKEY	key_handle;
    long	value_type;
    long	bufsize = BUFSIZE;
    int		r;
    char	*p;

    /* Open the key where the path to gvim.exe is stored. */
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0, KEY_READ,
						&key_handle) != ERROR_SUCCESS)
	return 0;

    /* get the DisplayName out of it to show the user */
    r = RegQueryValueEx(key_handle, "path", 0,
					  &value_type, (LPBYTE)buf, &bufsize);
    RegCloseKey(key_handle);

    return (r == ERROR_SUCCESS);
}

    static void
remove_popup(void)
{
    int		fail = 0;
    HKEY	kh;

    if (RegDeleteKey(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}\\InProcServer32") != ERROR_SUCCESS)
        ++fail;
    if (RegDeleteKey(HKEY_CLASSES_ROOT, "CLSID\\{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
        ++fail;
    if (RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\gvim") != ERROR_SUCCESS)
        ++fail;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", 0, KEY_ALL_ACCESS, &kh) != ERROR_SUCCESS)
        ++fail;
    else
    {
        if (RegDeleteValue(kh, "{51EEE242-AD87-11d3-9C1E-0090278BBD99}") != ERROR_SUCCESS)
            ++fail;
        RegCloseKey(kh);
    }
    if (RegDeleteKey(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim") != ERROR_SUCCESS)
        ++fail;
    if (RegDeleteKey(HKEY_LOCAL_MACHINE, "Software\\Vim") != ERROR_SUCCESS)
        ++fail;

    if (fail == 6)
        printf("No Vim registry entries could be removed\n");
    else if (fail)
        printf("Some Vim registry entries could not be removed\n");
    else
        printf("The Vim registry entries have been removed\n");
}
#endif

/*
 * Check if a batch file is really for the current version.  Don't delete a
 * batch file that was written for another (possibly newer) version.
 */
    static int
batfile_thisversion(char *path)
{
    FILE	*fd;
    char	line[BUFSIZE];
    char	*p;
    int		found = FALSE;

    fd = fopen(path, "r");
    if (fd != NULL)
    {
	while (fgets(line, BUFSIZE, fd) != NULL)
	{
	    for (p = line; *p != 0; ++p)
		if (strnicmp(p, VIM_VERSION_NODOT, strlen(VIM_VERSION_NODOT))
									 == 0)
		    break;
	    if (*p != 0)
	    {
		/* don't accept "vim60an" when looking for "vim60". */
		p += strlen(VIM_VERSION_NODOT);
		if (!isdigit(*p) && !isalpha(*p))
		{
		    found = TRUE;
		    break;
		}
	    }
	}
	fclose(fd);
    }
    return found;
}

    static int
remove_batfiles(int doit)
{
    char *batfile_path;
    int	 i;
    int	 found = 0;

    for (i = 1; i < TARGET_COUNT; ++i)
    {
	batfile_path = searchpath_save(targets[i].batname);
	if (batfile_path != NULL && batfile_thisversion(batfile_path))
	{
	    ++found;
	    if (doit)
	    {
		printf("removing %s\n", batfile_path);
		remove(batfile_path);
	    }
	    else
		printf(" - the batch file %s\n", batfile_path);
	    free(batfile_path);
	}
    }
    return found;
}

#ifdef WIN3264
    static void
remove_if_exists(char *path, char *filename)
{
    char buf[BUFSIZE];
    FILE *fd;

    sprintf(buf, "%s\\%s", path, filename);

    fd = fopen(buf, "r");
    if (fd != NULL)
    {
        fclose(fd);
        printf("removing %s\n", buf);
        remove(buf);
    }
}

    static void
remove_icons()
{
    char path[BUFSIZE];
    int	 i;
    struct stat st;

    if (get_shell_folder_path(path, "desktop"))
	for (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);
}

    static void
remove_start_menu()
{
    char path[BUFSIZE];
    int	 i;
    struct stat st;

    if (get_shell_folder_path(path, "desktop"))
	for (i = 0; i < ICON_COUNT; ++i)
	    remove_if_exists(path, icon_link_names[i]);

    if (get_shell_folder_path(path, VIM_STARTMENU))
    {
	for (i = 1; i < TARGET_COUNT; ++i)
	    remove_if_exists(path, targets[i].lnkname);
	remove_if_exists(path, "uninstall.lnk");
	remove_if_exists(path, "Help.lnk");
	remove_if_exists(path, "Vim tutor.pif");
	remove_if_exists(path, "Vim online.url");
	if (stat(path, &st) == 0)
	{
	    printf("removing %s\n", path);
	    rmdir(path);
	}
    }
}
#endif

    int
main(int argc, char *argv[])
{
    int		found = 0;
    FILE	*fd;
#ifdef WIN3264
    int		i;
    struct stat st;
    char	icon[BUFSIZE];
    char	path[BUFSIZE];
    char	popup_path[BUFSIZE];
    int		nsis = 0;

    /* The nsis uninstaller calls us with a "-nsis" argument. */
    if (argc == 2 && stricmp(argv[1], "-nsis") == 0)
	nsis = 1;
#endif

    printf("This program will remove the following items:\n");

#ifdef WIN3264
    if (popup_gvim_path(popup_path))
    {
	printf(" - the \"Edit with Vim\" entry in the popup menu\n");
	printf("   which uses \"%s\"\n", popup_path);
	printf("\nRemove it (y/n)? ");
	if (confirm())
	    remove_popup();
    }

    if (get_shell_folder_path(path, "desktop"))
    {
	printf("\n");
	for (i = 0; i < ICON_COUNT; ++i)
	{
	    sprintf(icon, "%s\\%s", path, icon_link_names[i]);
	    if (stat(icon, &st) == 0)
	    {
		printf(" - the \"%s\" icon on the desktop\n", icon_names[i]);
		++found;
	    }
	}
	if (found > 0)
	{
	    if (!nsis)
		printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	    if (nsis || confirm())
		remove_icons();
	}
    }

    if (get_shell_folder_path(path, VIM_STARTMENU)
	    && stat(path, &st) == 0)
    {
	printf("\n - the \"%s\" entry in the Start Menu\n", VIM_STARTMENU);
	if (!nsis)
	    printf("\nRemove it (y/n)? ");
	if (nsis || confirm())
	    remove_start_menu();
    }
#endif

    printf("\n");
    found = remove_batfiles(0);
    if (found > 0)
    {
	printf("\nRemove %s (y/n)? ", found > 1 ? "them" : "it");
	if (confirm())
	    remove_batfiles(1);
    }

    fd = fopen("gvim.exe", "r");
    if (fd != NULL)
    {
	fclose(fd);
        printf("gvim.exe detected.  Attempting to unregister gvim with OLE\n");
	printf("If a message box appears telling you that Vim\n");
	printf("unregistered successfully, click OK.\n");
	system("gvim.exe -unregister");
    }

#ifdef WIN3264
    if (!nsis)
    {
	RegDeleteKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT) != ERROR_SUCCESS;

	printf("\nYou may now want to delete the Vim executables and runtime files.\n");
	printf("(They are still where you unpacked them.)\n");
    }
#endif

    rewind(stdin);
    printf("\nPress Enter to exit...");
    (void)getchar();

    return 0;
}
