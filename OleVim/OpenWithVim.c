/* vim: set sw=4 sts=4 : */
/*
 * OpenWithVim.c
 *
 * Naive port of OpenWithVim.bas to C
 *
 * When       Who       What
 * 2001-07-25 W.Briscoe Original derived from Christian Schaller's work
 * 2001-08-02 W.Briscoe Absorbed ideas from C++ port.
 * 2001-08-03 W.Briscoe Factored out vim access to uvim.h. Removed Basic code
 */

#pragma warning(disable: 4100 4115 4201 4214 4514)
#include <windows.h>
#pragma warning(default: 4100 4115 4201 4214)

#include <direct.h> /* For getcwd */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "uvim.h"

  static void
MsgBox(const char *text)
{
  (void)MessageBox(	0,                  // handle of owner window
			(LPCSTR)text,       // address of text in message box
			(LPCSTR)"Project1", // address of title of message box
			MB_OK);             // the message box has 1 "OK" button
}

    static void
myexit(int n)
{
    exit(n);
}

    static void
fail(const char *text)
{
    MsgBox(text);
    myexit(0);
}

  static void
usage(void)
{
    fail("OpenWithVim 1.3 (2001-08-04)"
	    "\n"
	    "\n" "Usage: OpenWithVim [-r<char>] [+<line>] [<file>]"
	    "\n"
	    "\n" "Options:"
	    "\n" "+<line>" "\t" "goto line number <line>"
	    "\n" "-r<char>" "\t" "send the language dependent shortcut for"
	    "\n" "\t" "restoring vim's window (US: -rr, German: -rw)"
	    "\n" "-h" "\t" "display this dialog");
}

static char keys[FILENAME_MAX*4] = "";

    static void
add_keys(const char *s)
{
    strcat(keys, s);
}

static uvim vim;

    static void
tidy(void)
{
    unuvim(&vim);
}

    int WINAPI
WinMain(HINSTANCE a, HINSTANCE b, LPSTR gcl, int c)
{
    size_t	sgcl;
    char	*commandline;
    char	*param;
    char	*restoreCmd;
    char	*line;
    char	*file = 0;
    char	*cp;
    char	path[FILENAME_MAX];
    int		start;

    (void)a, (void)b, (void)c; /* Unused */

    while (isspace(*gcl))
	gcl++;

    if (*gcl == '\0')
	usage();

    sgcl = strlen(gcl) + 1;
    commandline = malloc(sgcl);
    param = malloc(sgcl);
    restoreCmd = malloc(sgcl);
    line = malloc(sgcl);

    if (commandline == NULL
	    ||  param       == NULL
	    ||  restoreCmd  == NULL
	    ||  line        == NULL)
	fail("No memory!");

    *restoreCmd = *param = *line = 0;
    strcpy(commandline, gcl);

    cp = strchr(commandline, ' ');
    start = (cp == NULL) ? (int)strlen(commandline) : cp - commandline;

    if (*commandline == '-')
    {
	sscanf(commandline + 1, "%[^ ]", param);

	if (*param != 'r')
	    usage();

	sscanf(param + 1, "%[^ ]", restoreCmd);

	if (*restoreCmd == 0)
	    usage();

	strcpy(param, commandline + 1);
	cp = param + 2; /* Skip -r */

	while (*cp == ' ')
	    ++cp; /* Skip space */

	strcpy(commandline, cp);
    }

    if (*commandline == '+')
    {
	if (start > 0)
	{
	    *line = 0, sscanf(commandline + 1, "%[^ ]", line);
	    file = commandline + strlen(line) + 1;
	    while (*file == ' ')
		++file;
	}

	/* no line number? */
	if (*line == 0 || file == 0 || *file == 0)
	    usage();
    }
    else
    {
	for (file = commandline; *file == ' '; ++file)
	    ;
    }

    if (*file == '"')
    {
	strcpy(param, file);
	sscanf(param, "\"%[^\"]", commandline);
	file = commandline;
    }

    if (getcwd(path, sizeof path) == NULL)
	fail("Cannot find current working folder");

    /* first, make sure, vim is in normal mode */
    add_keys("<c-\\><c-n>");

    /* if file name is relative, first change to that directory */
    if (*file != '\\' && *file != '/' && file[1] != ':')
    {
	add_keys(":cd ");
	add_keys(path);
	add_keys("<c-m> ");
    }

    if (*file != 0)
    {
	add_keys(":drop ");
	add_keys(file);
	add_keys("<c-m>");
    }

    if (*line != 0)
    {
	add_keys(":");
	add_keys(line);
	add_keys("<c-m>");
    }

    if (*restoreCmd != 0)
    {
	add_keys(":simalt ~");
	add_keys(restoreCmd);
	add_keys("<c-m>");
    }

    free(commandline);
    free(param);
    free(restoreCmd);
    free(line);

    if (atexit(tidy) != 0)
	fail("Cannot register a function to tidy resources");

    vim = touvim();
    vim.SetForeground(&vim);
    vim.SendKeys(&vim, keys);

    myexit(0);
    /*NOTREACHED*/
    return 0;
}
