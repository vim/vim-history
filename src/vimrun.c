/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			this file by Vince Negri
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * vimrun.c - Tiny Win32 program to safely run an external command in a
 *	      DOS console.
 *	      This program is required to avoid that typing CTRL-C in the DOS
 *	      console kills Vim.  Now it only kills vimrun.
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#ifdef __BORLANDC__
extern char *
#ifdef _RTLDLL
__import
#endif
_oscmd;
# define _kbhit kbhit
# define _getch getch
#else
#ifdef __MINGW32__
# include <windows.h>
#else
extern char *_acmdln;
#endif
#endif

    int
main(void)
{
    const char	*p;
    int		retval;
    int		inquote = 0;

#ifdef __BORLANDC__
    p = _oscmd;
#elif defined(__MINGW32__)
    p = (const char *) GetCommandLine();
#else
    p = _acmdln;
#endif
    /*
     * Skip the executable name, which might be in "".
     */
    while (*p)
    {
	if (*p == '"')
	    inquote = !inquote;
	else if (!inquote && *p == ' ')
	{
	    ++p;
	    break;
	}
	++p;
    }

    /* Print the command, including quotes and redirection. */
    puts(p);
    retval = system(p);
    if (retval != 0)
	printf("%d returned\n", retval);

    puts("Hit any key to close this window...");

    while (_kbhit())
	(void)_getch();
    (void)_getch();

    return retval;
}
