/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
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
 * deallocation of memory. And some funcions for copying text.
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

/*
 * Note: if unsinged is 16 bits we can only allocate up to 64K with alloc().
 * Use lalloc for larger blocks.
 */
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

#ifdef MSDOS
	if (size >= 0xfff0)			/* in MSDOS we can't deal with >64K blocks */
		p = NULL;
	else
#endif

	if ((p = (char *)malloc(size)) != NULL)
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
	/*
	 * Avoid repeating the error message many times (they take 1 second each).
	 * Did_outofmem_msg is reset when a character is read.
	 */
	if (message && p == NULL && !did_outofmem_msg)
	{
		emsg(e_outofmem);
		did_outofmem_msg = TRUE;
	}
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
	register size_t	i = count;
	register char	*p = ptr;

	while (i--)
		*p++ = ' ';
}

#ifdef NO_FREE_NULL
#undef free
/*
 * replacement for free() that cannot handle NULL pointers
 */
	void
nofreeNULL(x)
	void *x;
{
	if (x != NULL)
		free(x);
}
#endif

#ifdef BSD_UNIX
	char *
bsdmemset(ptr, c, size)
	char	*ptr;
	int		c;
	long	size;
{
	register char *p = ptr;

	while (size-- > 0)
		*p++ = c;
	return ptr;
}
#endif

#ifdef MEMMOVE
/*
 * Version of memmove that handles overlapping source and destination.
 * For systems that don't have a function that is guaranteed to do that (SYSV).
 */
	void *
memmove(desti, source, len)
	void	*source, *desti;
#ifdef __sgi
	size_t	len;
#else
	int		len;
#endif
{
	char *src = (char *)source;
	char *dst = (char *)desti;

	if (dst > src && dst < src + len)	/* overlap, copy backwards */
	{
		src +=len;
		dst +=len;
		while (--len >= 0)
			*--dst = *--src;
	}
	else								/* copy forwards */
		while (--len >= 0)
			*dst++ = *src++;
	return desti;
}
#endif
