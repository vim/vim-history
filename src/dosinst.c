/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * install.c: Minimalistic install program for Vim on DOS/MS-Windows
 *
 * Compile with Makefile.bcc or Makefile.djg.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#ifdef WIN32
# include <direct.h>
#else
# include <dir.h>
#endif

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
 * Move a file to another directory.
 */
    void
move_file(char *fname, char *dir)
{
    struct stat	st;
    char	cmd[1000];

    /* if the file doesn't exist, silently skip it */
    if (stat(fname, &st) < 0)
	return;

    sprintf(cmd, "move %s %s", fname, dir);
    system(cmd);

    if (stat(fname, &st) >= 0)
	printf("ERROR: Moving \"%s\" to \"%s\" failed\n", fname, dir);
}

/*
 * Ask for directory from $PATH to move the .exe files to.
 */
    void
move_to_path(void)
{
    char	*path;
    char	**names = NULL;
    char	*p, *s;
    int		count;
    int		idx;
    char	answer[10];

    path = getenv("PATH");
    if (path == NULL)
    {
	printf("ERROR: The variable $PATH is not set\n");
	return;
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
		names[count][s - p] = 0;
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
    if (scanf(" %c", answer) < 1 || toupper(answer[0]) != 'Y')
	printf("Skipping moving Vim executables\n");
    else
    {
	move_file("vim.exe", names[idx]);
	move_file("gvim.exe", names[idx]);
	move_file("xxd.exe", names[idx]);
	move_file("ctags.exe", names[idx]);
	move_file("vimrun.exe", names[idx]);
    }
}

#define TABLE_SIZE(s)	sizeof(s) / sizeof(char *)

    int
main(int argc, char **argv)
{
    char	answer[10];
    char	*(def_choices[]) =
    {
	"\nChoose the default way to run Vim:",
	"normal Vim setup",
	"with syntax highlighting and other features",
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
	"\nChoose the way to run Vim:",
	"set $PATH in c:\\autoexec.bat",
	"move executables to a directory already in $PATH",
	"do nothing",
    };
    int		def;
    int		select;
    int		exe;
    FILE	*fd;
    char	cwd[1000];

    if (
#ifdef WIN32
	_getcwd
#else
	getcwd
#endif
		(cwd, 1000) == NULL)
    {
	printf("ERROR: Cannot get name of current directory\n");
	exit(1);
    }

    /*
     * Ask the user if he really wants to install Vim.
     */
    printf("This program will set up the installation of Vim\n");
    printf("It prepares the _vimrc file, $VIM and the executables.\n");
    printf("Do you want to continue? (Y/N) ");
    if (scanf(" %c", answer) < 1 || toupper(answer[0]) != 'Y')
	exit(0);

    /*
     * Ask for contents of _vimrc.
     */
    def = get_choice(def_choices, TABLE_SIZE(def_choices));
    select = get_choice(select_choices, TABLE_SIZE(select_choices));

    printf("\nYou have chosen:\n");
    printf("[%d] %s\n", def, def_choices[def]);
    printf("[%d] %s\n", select, select_choices[select]);
    if ((fd = fopen("_vimrc", "r")) != NULL)
    {
	fclose(fd);
	printf("\nThere already exists a _vimrc in the current directory.");
	printf("\nDo you want to overwrite it? (Y/N) ");
    }
    else
	printf("\nDo you want to write the _vimrc in the current directory? (Y/N) ");
    if (scanf(" %c", answer) < 1 || toupper(answer[0]) != 'Y')
	printf("Skipping writing of _vimrc\n");
    else
    {
	fd = fopen("_vimrc", "w");
	if (fd == NULL)
	{
	    printf("ERROR: Cannot open _vimrc for writing\n");
	    exit(1);
	}
	switch (def)
	{
	    case 1:     fprintf(fd, "set nocompatible\n");
			break;
	    case 2:     fprintf(fd, "set nocompatible\n");
			fprintf(fd, "source $VIM/vimrc_example\n");
			break;
	    case 3:     fprintf(fd, "set compatible\n");
			break;
	}
	switch (select)
	{
	    case 1:	fprintf(fd, "behave xterm\n");
			break;
	    case 2:	fprintf(fd, "source $VIM/mswin.vim\n");
			break;
	    case 3:	fprintf(fd, "behave xterm\n");
			fprintf(fd, "set selectmode=mouse\n");
			break;
	}
	fclose(fd);
	printf("_vimrc has been written\n");
    }

    /*
     * Set $VIM somehow
     */
    printf("\nI can append a command to c:\\autoexec.bat to set $VIM.\n");
    printf("(this will not work if c:\\autoexec.bat contains sections)\n");
    printf("Do you want me to append to your c:\\autoexec.bat? (Y/N) ");
    if (scanf(" %c", answer) < 1 || toupper(answer[0]) != 'Y')
	printf("Skipping appending to c:\\autoexec.bat\n");
    else
	append_autoexec("set VIM=%s\n", cwd);

    /*
     * Set PATH or move executables.
     */
    printf("\nTo be able to run Vim it must be in your $PATH.");
    exe = get_choice(exe_choices, TABLE_SIZE(exe_choices));
    switch (exe)
    {
	case 1:	    append_autoexec("set PATH=%%PATH%%;%s\n", cwd);
		    break;

	case 2:	    move_to_path();
		    break;

	case 3:	    printf("Skipping setting $PATH\n");
		    break;
    }

    printf("\nThat finishes the installation.  Happy Vimming!\n");
    return 0;
}
