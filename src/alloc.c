/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
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

/*
 * Some memory is reserved for error messages and for being able to
 * call mf_release_all(), which needs some memory for mf_trans_add().
 */
#define KEEP_ROOM 8192L

/*
 * Note: if unsinged is 16 bits we can only allocate up to 64K with alloc().
 * Use lalloc for larger blocks.
 */
	char_u *
alloc(size)
	unsigned		size;
{
	return (lalloc((long_u)size, TRUE));
}

	char_u *
lalloc(size, message)
	long_u			size;
	int				message;
{
	register char_u   *p;			/* pointer to new storage space */
	static int	releasing = FALSE;	/* don't do mf_release_all() recursive */
	int			try_again;

#ifdef MSDOS
	if (size >= 0xfff0)			/* in MSDOS we can't deal with >64K blocks */
		p = NULL;
	else
#endif

	/*
	 * If out of memory, try to release some memfile blocks.
	 * If some blocks are released call malloc again.
	 */
	for (;;)
	{
		if ((p = (char_u *)malloc(size)) != NULL)
		{
			if (mch_avail_mem(TRUE) < KEEP_ROOM && !releasing)
			{ 								/* System is low... no go! */
					free((char *)p);
					p = NULL;
			}
		}
	/*
	 * Remember that mf_release_all() is being called to avoid an endless loop,
	 * because mf_release_all() may call alloc() recursively.
	 */
		if (p != NULL || releasing)
			break;
		releasing = TRUE;
		try_again = mf_release_all();
		releasing = FALSE;
		if (!try_again)
			break;
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
	char_u *
strsave(string)
	char_u		   *string;
{
	char_u *p;

	p = alloc((unsigned) (STRLEN(string) + 1));
	if (p != NULL)
		STRCPY(p, string);
	return p;
}

	char_u *
strnsave(string, len)
	char_u		*string;
	int 		len;
{
	char_u *p;

	p = alloc((unsigned) (len + 1));
	if (p != NULL)
	{
		STRNCPY(p, string, (size_t)len);
		p[len] = NUL;
	}
	return p;
}

/*
 * copy a number of spaces
 */
	void
copy_spaces(ptr, count)
	char_u	*ptr;
	size_t	count;
{
	register size_t	i = count;
	register char_u	*p = ptr;

	while (i--)
		*p++ = ' ';
}

/*
 * delete spaces at the end of the string
 */
	void
del_spaces(ptr)
	char_u *ptr;
{
	char_u	*q;

	q = ptr + STRLEN(ptr);
	while (--q > ptr && isspace(q[0]) && q[-1] != '\\' && q[-1] != Ctrl('V'))
		*q = NUL;
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
#ifdef __sgi
memmove(desti, source, len)
	void	*source, *desti;
	size_t	len;
#else
memmove(desti, source, len)
	void	*source, *desti;
	int		len;
#endif
{
	char	*src = (char *)source;
	char	*dst = (char *)desti;

	if (dst > src && dst < src + len)	/* overlap, copy backwards */
	{
		src +=len;
		dst +=len;
		while (len-- > 0)
			*--dst = *--src;
	}
	else								/* copy forwards */
		while (len-- > 0)
			*dst++ = *src++;
	return desti;
}
#endif

/*
 * compare two strings, ignoring case
 * return 0 for match, 1 for difference
 */
	int
vim_strnicmp(s1, s2, len)
	char_u	*s1;
	char_u	*s2;
	size_t	len;
{
	while (len)
	{
		if (TO_UPPER(*s1) != TO_UPPER(*s2))
			return 1;						/* this character different */
		if (*s1 == NUL)
			return 0;						/* strings match until NUL */
		++s1;
		++s2;
		--len;
	}
	return 0;								/* strings match */
}
