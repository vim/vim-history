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
#include "env.h"
/*
 * Short explanation of the tilde: it stands for the previous replacement
 * pattern. If that previous pattern also contains a ~ we should go back
 * a step further... or insert the previous pattern into the current one
 * and remember that.
 * This still does not handle the case where "magic" changes. TODO?
 *
 * On the other hand, this definition is not so useful. We can always retype
 * the previous pattern... especially with command history or in files.
 *
 * It would seem much more useful to remember the previously substituted
 * text. There is generally no other way to get at this. This is useful
 * when you want to do several substitutions on one line, and skip for
 * the second whatever you changed in the first.
 */
#define TILDE
/*#define VITILDE			/* Too painful to do right... */
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
typedef void *(*fptr)(char *, char);

	static fptr
do_copy(char *d, char c)
{
	*d = c;

	return (fptr)do_copy;
}

	static fptr
do_upper(char *d, char c)
{
	*d = toupper(c);

	return (fptr)do_copy;
}

	static fptr
do_Upper(char *d, char c)
{
	*d = toupper(c);

	return (fptr)do_Upper;
}

	static fptr
do_lower(char *d, char c)
{
	*d = tolower(c);

	return (fptr)do_copy;
}

	static fptr
do_Lower(char *d, char c)
{
	*d = tolower(c);

	return (fptr)do_Lower;
}

	static fptr
strnfcpy(fptr f, char *d, char *s, int n)
{
	while (n-- > 0) {
		f = f(d, *s);
		if (!*s++)
			break;
		d++;
	}

	return f;
}
#endif

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
#ifdef VITILDE
	char		   *tmp_sub = NULL;
#endif

	if (prog == NULL || source == NULL || dest == NULL) {
		emsg("NULL parm to regsub");
		return 0;
	}
	if (UCHARAT(prog->program) != MAGIC) {
		emsg("damaged regexp fed to regsub");
		return 0;
	}
	src = source;
	dst = dest;

	while ((c = *src++) != '\0') {
		no = -1;
		if (c == '&' && magic)
	doampersand:
			no = 0;
		else if (c == '\\') {
			if ('0' <= *src && *src <= '9') {
				no = *src++ - '0';
#ifdef CASECONVERT
			} else if (*src == 'u') {
				src++;
				func = (fptr)do_upper;
				continue;
			} else if (*src == 'U') {
				src++;
				func = (fptr)do_Upper;
				continue;
			} else if (*src == 'l') {
				src++;
				func = (fptr)do_lower;
				continue;
			} else if (*src == 'L') {
				src++;
				func = (fptr)do_Lower;
				continue;
			} else if (*src == 'e' || *src == 'E') {
				src++;
				func = (fptr)do_copy;
				continue;
			}
#endif
		}
#ifdef TILDE
		if (c == '~' && magic) {
	dotilde:
			if (reg_prev_sub) {
# ifdef VITILDE
				/*
				 * We should now insert the previous pattern at
				 * this location in the current pattern, and remember that
				 * for next time... this is very painful to do right.
				 */
				if (copy) {
					char		   *newsub;
					int				len;

#ifdef DEBUG
					printf("Old ~: '%s'\r\n", reg_prev_sub);
#endif
					/* length = len(current) - 1 + len(previous) + 1 */
					newsub = alloc(strlen(source) + strlen(reg_prev_sub));
					if (newsub) {
						/* copy prefix */
						len = (src - source) - 1;	/* not including ~ */
						if (!magic)
							len--;					/* back off \ */
						strncpy(newsub, source, len);
						/* interpolate tilde */
						strcpy(newsub + len, reg_prev_sub);
						/* copy postfix */
						strcat(newsub + len, src);

						if (tmp_sub)
							free(tmp_sub);
						tmp_sub = newsub;
						source = newsub;
						src = newsub + len;
					}
#ifdef DEBUG
					printf("New  : '%s'\r\n", newsub);
					printf("Todo : '%s'\r\n", src);
#endif
				} else {
					dst += regsub(prog, reg_prev_sub, dst, copy, magic) - 1;
				}
# else /* no VITILDE */
				if (copy) {
#  ifdef CASECONVERT
					func = strnfcpy(func, dst, reg_prev_sub, ((unsigned)~0)>>1);
#  else
					(void) strcpy(dst, reg_prev_sub);
#  endif
				}
				dst += strlen(reg_prev_sub);
# endif /* def VITILDE */
			}
		} else
#endif  /* def TILDE */
		if (no < 0) {           /* Ordinary character. */
			if (c == '\\') {
				if (*src == '&' && !magic)
					goto doampersand;
#ifdef TILDE
				else if (*src == '~' && !magic)
					goto dotilde;
#endif
				else
					c = *src++;
			}
			if (copy) {
#ifdef CASECONVERT
				func = func(dst, c);
#else
				*dst = c;
#endif
			}
			dst++;
		} else if (prog->startp[no] != NULL && prog->endp[no] != NULL) {
			len = prog->endp[no] - prog->startp[no];
			if (copy) {
#ifdef CASECONVERT
				func = strnfcpy(func, dst, prog->startp[no], len);
#else
				(void) strncpy(dst, prog->startp[no], len);
#endif
			}
			dst += len;
			if (copy && len != 0 && *(dst - 1) == '\0') { /* strncpy hit NUL. */
				emsg("damaged match string");
				goto exit;
			}
		}
	}
	if (copy)
		*dst = '\0';

#ifdef TILDE
# ifdef VITILDE
	if (copy) {
		if (reg_prev_sub)
			free(reg_prev_sub);
		if (tmp_sub)
			reg_prev_sub = tmp_sub;		/* tmp_sub == source */
		else
			reg_prev_sub = strdup(source);
	}
# else
	if (copy) {
		if (reg_prev_sub)
			free(reg_prev_sub);
		reg_prev_sub = strdup(dest);
	}
# endif
#endif

exit:
	return (dst - dest) + 1;
}
