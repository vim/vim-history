/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * makecmdtab.c: separate program that reads cmdtab.tab and produces cmdtab.h
 *
 *	call with: makecmdtab <cmdtab.tab >cmdtab.h
 */

#include <stdlib.h>
#include <stdio.h>

main()
{
	register int c;
	char buffer[100];
	int	count;
	int i;

	while ((c = getchar()) != '|' && c != EOF)
		putchar(c);
	printf("THIS FILE IS AUTOMATICALLY PRODUCED - DO NOT EDIT");
	while ((c = getchar()) != '|' && c != EOF)
		;
	while ((c = getchar()) != '|' && c != EOF)
		putchar(c);

	count = 0;
	while ((c = getchar()) != '|' && c != EOF)
	{
		putchar(c);
		while ((c = getchar()) != '"' && c != EOF)
			putchar(c);
		putchar(c);

		i = 0;
		while ((c = getchar()) != '"' && c != EOF)
		{
			putchar(c);
			buffer[i++] = c;
		}
		putchar(c);
		buffer[i] = 0;

		while ((c = getchar()) != '\n' && c != EOF)
			putchar(c);
		putchar(c);

		switch (buffer[0])
		{
			case '!':	strcpy(buffer, "bang");
						break;
			case '<':	strcpy(buffer, "lshift");
						break;
			case '>':	strcpy(buffer, "rshift");
						break;
			case '=':	strcpy(buffer, "equal");
						break;
			case '&':	strcpy(buffer, "and");
						break;
			case '~':	strcpy(buffer, "tilde");
						break;
		}
					
		printf("#define CMD_%s %d\n", buffer, count++);
	}

	printf("#define CMD_SIZE %d\n", count);

	while ((c = getchar()) != '|' && c != EOF)
		putchar(c);

	if (c != '|')
	{
		fprintf(stderr, "not enough |'s\n");
		exit(1);
	}
	exit(0);
}
