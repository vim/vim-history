/*****************************************************************************
*   $Id: strstr.c,v 5.1 1998/02/19 03:47:18 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains a substitute for a potentially missing ANSI C
*   function.
*****************************************************************************/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <string.h>

#ifndef HAVE_STRSTR

extern char * strstr( str, substr )
    const char *const str;
    const char *const substr;
{
    const size_t length = strlen(substr);
    const char *match = NULL;
    const char *p;

    for (p = str  ;  *p != '\0'  ;  ++p)
	if (strncmp(p, substr, length) == 0)
	{
	    match = p;
	    break;
	}
    return match;
}

#endif

/* vi:set tabstop=8 shiftwidth=4: */
