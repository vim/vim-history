/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * uninstal.c:	Minimalistic uninstall program for Vim on MS-Windows
 *		Only removes the "Edit with Vim" popup menu entry.
 *
 * Compile with Makefile.mvc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef WIN32
# include <windows.h>
#endif
#include "version.h"

/*
 * Return TRUE if the user types a 'y' or 'Y', FALSE otherwise.
 */
    int
confirm(void)
{
    char	answer[10];

    return (scanf(" %c", answer) == 1 && toupper(answer[0]) == 'Y');
}

    int
main(int argc, char *argv[])
{
    int	    fail = 0;
    HKEY    kh;

    printf("This program will remove the \"Edit with Vim\" entry from the popup menu.\n");
    printf("It will NOT delete the Vim executable or runtime files.\n");
    printf("Continue (y/n)? ");
    if (confirm())
    {
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
	if (RegDeleteKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT) != ERROR_SUCCESS)
	    ++fail;

	if (fail == 7)
	    printf("No Vim registry entries could be removed\n");
	else if (fail)
	    printf("Some Vim registry entries could not be removed\n");
	else
	    printf("The Vim registry entries have been removed\n");
    }
    else
	printf("Nothing changed\n");

    while (getchar() != '\n')	/* eat the newline for the confirm() above */
	;
    printf("\nHit return to exit...");
    (void)getchar();
}

