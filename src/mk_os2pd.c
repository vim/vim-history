/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * mk_os2pd.c: separate program to create pathdef.c on OS/2
 *
 *		usage: mk_os2pd "$(CC)" "$(CFLAGS)" > pathdef.c
 *
 *				Created by Paul Slootman for the OS/2 - EMX port
 */

#include "vim.h"

	void
main(argc, argv)
	int		argc;
	char	**argv;
{
	puts("/* pathdef.c */");
	puts("/* This file should automatically be created by mk_os2pd.exe */");
	puts("/* Change mk_os2pd.c only (if at all...). */\n");
	puts("#include \"vim.h\"");
	puts("#ifndef SYS_VIMRC_FILE");
	puts("char_u *sys_vimrc_fname = (char_u *)\"$VIM/.vimrc\";");
	puts("#else");
	puts("char_u *sys_vimrc_fname = (char_u *)SYS_VIMRC_FILE;");
	puts("#endif");
	puts("#ifndef SYS_GVIMRC_FILE");
	puts("char_u *sys_gvimrc_fname = (char_u *)\"$VIM/.gvimrc\";");
	puts("#else");
	puts("char_u *sys_gvimrc_fname = (char_u *)SYS_GVIMRC_FILE;");
	puts("#endif");
	puts("#ifndef VIM_HLP");
	puts("char_u *help_fname = (char_u *)\"$VIM/vim_help.txt\";");
	puts("#else");
	puts("char_u *help_fname = (char_u *)VIM_HLP;");
	puts("#endif");
	printf("char_u *all_cflags = (char_u *)\"%s %s\";\n", argv[1], argv[2]);

	exit(0);
}
