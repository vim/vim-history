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
 * alloc.c
 *
 * This file contains various routines dealing with allocation and
 * deallocation of data structures.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

#ifdef AMIGA
# undef FALSE			/* these are redefined in exec/types.h */
# undef TRUE
# include <exec/types.h>
# include <exec/memory.h>
# undef FALSE
# define FALSE 0		/* define FALSE and TRUE as ints instead of longs */
# undef TRUE
# define TRUE 1
#endif /* AMIGA */

#ifdef MSDOS
# include <alloc.h>
#endif /* MSDOS */

#define PANIC_FACTOR_CHIP 8192L

	char *
alloc(size)
	unsigned		size;
{
	return (lalloc((u_long)size, TRUE));
}

	char *
lalloc(size, message)
	u_long			size;
	int				message;
{
	register char   *p;			/* pointer to new storage space */

	if ((p = malloc(size)) != NULL)
	{
#ifdef AMIGA
		if (AvailMem((long)MEMF_CHIP) < PANIC_FACTOR_CHIP)
		{ 								/* System is low... no go! */
				free(p);
				p = NULL;
		}
#endif
#ifdef MSDOS
		if (coreleft() < PANIC_FACTOR_CHIP)
		{ 								/* System is low... no go! */
				free(p);
				p = NULL;
		}
#endif
	}
	if (message && p == NULL)
		emsg(e_outofmem);
	return (p);
}

/*
 * copy a string into newly allocated memory
 */
	char *
strsave(string)
	char		   *string;
{
	char *p;

	p = alloc((unsigned) (strlen(string) + 1));
	if (p != NULL)
		strcpy(p, string);
	return p;
}

	char *
strnsave(string, len)
	char		*string;
	int 		len;
{
	char *p;

	p = alloc((unsigned) (len + 1));
	if (p != NULL)
	{
		strncpy(p, string, (size_t)len);
		p[len] = NUL;
	}
	return p;
}

/*
 * copy a number of spaces
 */
	void
copy_spaces(ptr, count)
	char	*ptr;
	size_t	count;
{
	register size_t	j;

	while (count)		/* copy 15 spaces at a time */
	{
		j = count;
		if (j > 15)
			j = 15;
		memmove(ptr, spaces, j);
		ptr += j;
		count -= j;
	}
}

	char *
mkstr(c)
	unsigned	  c;
{
	static char	  s[2];

	s[0] = c;
	s[1] = NUL;

	return s;
}
