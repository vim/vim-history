/* vim: set sw=4 sts=4 : */
/*
 * SendToVim.c
 *
 * Naive port of SendToVim.bas to C
 *
 * When       Who       What
 * 2001-08-03 W.Briscoe Original derived from Christian Schaller's work
 */

#pragma warning(disable: 4100 4115 4201 4214 4514)
#include <windows.h>
#pragma warning(default: 4100 4115 4201 4214)
#include <direct.h>
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
    fail("SendToVim 1.1 (2001-08-04)"
	    "\n"
	    "\n" "Usage: SendToVim [+cd] <vim command>"
	    "\n"
	    "\n" "Options:"
	    "\n" "+cd" "\t" "change to current directory before executing command");
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
    size_t	length;
    char	*commandline;
    char	*buffer;
    char	*cp;
    char	path[FILENAME_MAX];
    int		start;
    char	*cd_opt;
    char	*cmd;
    (void)a, (void)b, (void)c;

    commandline = malloc(strlen(gcl) + 1);
    buffer      = malloc(strlen(gcl) + 1);
    cmd         = malloc(strlen(gcl) + 1);
    cd_opt      = malloc(strlen(gcl) + 1);

    if (commandline == NULL
	    ||  buffer      == NULL
	    ||  cmd         == NULL
	    ||  cd_opt      == NULL)
	fail("No memory!");

    strcpy(commandline, gcl);
    length = strlen(commandline);

    /* no arguments? */
    if (length == 0)
	usage();

    /* leading +? */
    if (*commandline == '+')
    {
	/* remove + */
	strcpy(buffer, commandline + 1);
	strcpy(commandline, buffer);
	cp = strchr(commandline, ' ');
	start = (cp == NULL) ? (int)strlen(commandline) : cp - commandline;

	/* get cd_opt */
	strcpy(cd_opt, commandline);
	cd_opt[start] = 0;
	/* get command (without leading and trailing spaces) */
	strcpy(cmd, commandline + start);

	if (*cd_opt == 0 || strcmp(cd_opt, "cd") != 0 || *cmd == 0)
	    usage();

	if (strcmp(cd_opt, "cd") == 0)
	{
	    /* get current path */
	    if (getcwd(path, sizeof path) == NULL)
		fail("Cannot find current working folder");

	    add_keys(":cd ");
	    add_keys(path);
	    add_keys("<c-m>");
	}
    }
    else
    {
	for (cp = commandline; *cp == ' '; ++cp)
	    ;
	strcpy(cmd, commandline);
    }

    /* check for leading " */
    if (*cmd == '"')
    {
	/* if so, remove the last character (") */
	char *cmdlast = cmd + strlen(cmd) - 1;

	if (*cmdlast != '"' || cmd == cmdlast)
	    usage();

	*cmdlast = 0;
    }

    add_keys(cmd);
    add_keys("<c-m>");

    free(commandline);
    free(buffer);
    free(cmd);
    free(cd_opt);

    if (atexit(tidy) != 0)
	fail("Cannot register a function to tidy resources");

    vim = touvim();
    vim.SetForeground(&vim);
    vim.SendKeys(&vim, keys);

    myexit(0);
    /*NOTREACHED*/
    return 0;
}
