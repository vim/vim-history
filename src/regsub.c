/* vi:ts=4:sw=4
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
 *
 * This is NOT the original regular expression code as written by
 * Henry Spencer. This code has been modified specifically for use
 * with the VIM editor, and should not be used apart from compiling
 * VIM. If you want a good regular expression library, get the
 * original code. The copyright notice that follows is from the
 * original.
 *
 * NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE NOTICE
 *
 * regsub
 *
 *		Copyright (c) 1986 by University of Toronto.
 *		Written by Henry Spencer.  Not derived from licensed software.
 *
 *		Permission is granted to anyone to use this software for any
 *		purpose on any computer system, and to redistribute it freely,
 *		subject to the following restrictions:
 *
 *		1. The author is not responsible for the consequences of use of
 *				this software, no matter how awful, even if they arise
 *				from defects in it.
 *
 *		2. The origin of this software must not be misrepresented, either
 *				by explicit claim or by omission.
 *
 *		3. Altered versions must be plainly marked as such, and must not
 *				be misrepresented as being the original software.
 *
 * $Log:		regsub.c,v $
 * Revision 1.2  88/04/28  08:11:25  tony
 * First modification of the regexp library. Added an external variable
 * 'reg_ic' which can be set to indicate that case should be ignored.
 * Added a new parameter to regexec() to indicate that the given string
 * comes from the beginning of a line and is thus eligible to match
 * 'beginning-of-line'.
 *
 * Revisions by Olaf 'Rhialto' Seibert, rhialto@cs.kun.nl:
 * Changes for vi: (the semantics of several things were rather different)
 * - Added lexical analyzer, because in vi magicness of characters
 *   is rather difficult, and may change over time.
 * - Added support for \< \> \1-\9 and ~
 * - Left some magic stuff in, but only backslashed: \| \+
 * - * and \+ still work after \) even though they shouldn't.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

#ifdef MSDOS
# define __ARGS(a)	a
#endif

#define CASECONVERT

#include <stdio.h>
#include "regexp.h"
#include "regmagic.h"

#ifdef LATTICE
# include <sys/types.h>		/* for size_t */
#endif

#ifndef CHARBITS
#define UCHARAT(p)      ((int)*(unsigned char *)(p))
#else
#define UCHARAT(p)      ((int)*(p)&CHARBITS)
#endif

extern char 	   *reg_prev_sub;

#ifdef CASECONVERT
	/*
	 * We should define ftpr as a pointer to a function returning a pointer to
	 * a function returning a pointer to a function ...
	 * This is impossible, so we declare a pointer to a function returning a
	 * pointer to a function returning void. This should work for all compilers.
	 */
typedef void (*(*fptr) __ARGS((char *, int)))();
static fptr strnfcpy __ARGS((fptr, char *, char *, int));

static fptr do_copy __ARGS((char *, int));
static fptr do_upper __ARGS((char *, int));
static fptr do_Upper __ARGS((char *, int));
static fptr do_lower __ARGS((char *, int));
static fptr do_Lower __ARGS((char *, int));

	static fptr
do_copy(d, c)
	char *d;
	int c;
{
	*d = c;

	return (fptr)do_copy;
}

	static fptr
do_upper(d, c)
	char *d;
	int c;
{
	*d = TO_UPPER(c);

	return (fptr)do_copy;
}

	static fptr
do_Upper(d, c)
	char *d;
	int c;
{
	*d = TO_UPPER(c);

	return (fptr)do_Upper;
}

	static fptr
do_lower(d, c)
	char *d;
	int c;
{
	*d = TO_LOWER(c);

	return (fptr)do_copy;
}

	static fptr
do_Lower(d, c)
	char *d;
	int c;
{
	*d = TO_LOWER(c);

	return (fptr)do_Lower;
}

	static fptr
strnfcpy(f, d, s, n)
	fptr f;
	char *d;
	char *s;
	int n;
{
	while (n-- > 0) {
		f = (fptr)(f(d, *s));		/* Turbo C complains without the typecast */
		if (!*s++)
			break;
		d++;
	}

	return f;
}
#endif

/*
 * regtilde: replace tildes in the pattern by the old pattern
 *
 * Short explanation of the tilde: it stands for the previous replacement
 * pattern. If that previous pattern also contains a ~ we should go back
 * a step further... but we insert the previous pattern into the current one
 * and remember that.
 * This still does not handle the case where "magic" changes. TODO?
 *
 * New solution: The tilde's are parsed once before the first call to regsub().
 * In the old solution (tilde handled in regsub()) is was possible to get an
 * endless loop.
 */
	char *
regtilde(source, magic)
	char	*source;
	int		magic;
{
	char	*newsub = NULL;
	char	*tmpsub;
	char	*p;
	int		len;
	int		prevlen;

	for (p = source; *p; ++p)
	{
		if ((*p == '~' && magic) || (*p == '\\' && *(p + 1) == '~' && !magic))
		{
			if (reg_prev_sub)
			{
					/* length = len(current) - 1 + len(previous) + 1 */
				prevlen = strlen(reg_prev_sub);
				tmpsub = alloc((unsigned)(strlen(source) + prevlen));
				if (tmpsub)
				{
						/* copy prefix */
					len = (int)(p - source);	/* not including ~ */
					strncpy(tmpsub, source, (size_t)len);
						/* interpretate tilde */
					strcpy(tmpsub + len, reg_prev_sub);
						/* copy postfix */
					if (!magic)
						++p;					/* back off \ */
					strcat(tmpsub + len, p + 1);

					free(newsub);
					newsub = tmpsub;
					p = newsub + len + prevlen;
				}
			}
			else if (magic)
				strcpy(p, p + 1);				/* remove '~' */
			else
				strcpy(p, p + 2);				/* remove '\~' */
		}
		else if (*p == '\\' && p[1])			/* skip escaped characters */
			++p;
	}

	free(reg_prev_sub);
	if (newsub)
	{
		source = newsub;
		reg_prev_sub = newsub;
	}
	else
		reg_prev_sub = strsave(source);
	return source;
}

/*
 - regsub - perform substitutions after a regexp match
 *
 * Returns the size of the replacement, including terminating \0.
 */
	int
regsub(prog, source, dest, copy, magic)
	regexp		   *prog;
	char		   *source;
	char		   *dest;
	int 			copy;
	int 			magic;
{
	register char  *src;
	register char  *dst;
	register char	c;
	register int	no;
	register int	len;
#ifdef CASECONVERT
	fptr			func = (fptr)do_copy;
#endif

	if (prog == NULL || source == NULL || dest == NULL)
	{
		emsg(e_null);
		return 0;
	}
	if (UCHARAT(prog->program) != MAGIC)
	{
		emsg(e_re_corr);
		return 0;
	}
	src = source;
	dst = dest;

	while ((c = *src++) != '\0')
	{
		no = -1;
		if (c == '&' && magic)
			no = 0;
		else if (c == '\\')
		{
			if (*src == '&' && !magic)
			{
				++src;
				no = 0;
			}
			else if ('0' <= *src && *src <= '9')
			{
				no = *src++ - '0';
			}
#ifdef CASECONVERT
			else if (strchr("uUlLeE", *src))
			{
				switch (*src++)
				{
				case 'u':	func = (fptr)do_upper;
							continue;
				case 'U':	func = (fptr)do_Upper;
							continue;
				case 'l':	func = (fptr)do_lower;
							continue;
				case 'L':	func = (fptr)do_Lower;
							continue;
				case 'e':
				case 'E':	func = (fptr)do_copy;
							continue;
				}
			}
#endif
		}
		if (no < 0)           /* Ordinary character. */
		{
			if (c == '\\')
				c = *src++;
			if (copy)
			{
#ifdef CASECONVERT
				func = (fptr)(func(dst, c));
							/* Turbo C complains without the typecast */
#else
				*dst = c;
#endif
			}
			dst++;
		}
		else if (prog->startp[no] != NULL && prog->endp[no] != NULL)
		{
			len = (int)(prog->endp[no] - prog->startp[no]);
			if (copy)
			{
#ifdef CASECONVERT
				func = strnfcpy(func, dst, prog->startp[no], len);
#else
				(void) strncpy(dst, prog->startp[no], len);
#endif
			}
			dst += len;
			if (copy && len != 0 && *(dst - 1) == '\0') { /* strncpy hit NUL. */
				emsg(e_re_damg);
				goto exit;
			}
		}
	}
	if (copy)
		*dst = '\0';

exit:
	return (int)((dst - dest) + 1);
}
