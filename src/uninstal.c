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
 * Compile with Makefile.w32.
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

    printf("This program will remove the \"Edit with Vim\" entry from the popup menu\n");
    printf("Continue (y/n)? ");
    if (confirm())
    {
	if (RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shell\\Vim\\command")
							      != ERROR_SUCCESS)
	    ++fail;
	if (RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shell\\Vim") != ERROR_SUCCESS)
	    ++fail;
	if (RegDeleteKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Vim " VIM_VERSION_SHORT) != ERROR_SUCCESS)
	    ++fail;
	if (fail == 3)
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

