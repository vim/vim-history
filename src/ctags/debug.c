/*****************************************************************************
*   $Id: debug.c,v 6.6 1998/08/06 04:57:55 darren Exp $
*
*   Copyright (c) 1996-1998, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains debugging functions.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(__STDC__) || defined(MSDOS) || defined(WIN32) || defined(OS2)
# define ENABLE_STDARG
#endif

#ifdef ENABLE_STDARG
# include <stdarg.h>
#else
# include <varargs.h>
#endif

#include "ctags.h"

/*============================================================================
=   Function definitions
============================================================================*/

#ifdef DEBUG

extern void lineBreak() {}	/* provides a line-specified break point */

#ifdef ENABLE_STDARG
extern void debugPrintf( const enum _debugLevels level,
			 const char *const format, ... )
#else
extern void debugPrintf( va_alist )
    va_dcl
#endif
{
    va_list ap;

#ifdef ENABLE_STDARG
    va_start(ap, format);
#else
    enum _debugLevels level;
    const char *format;

    va_start(ap);
    level = va_arg(ap, enum _debugLevels);
    format = va_arg(ap, char *);
#endif

    if (debug(level))
	vprintf(format, ap);
    fflush(stdout);

    va_end(ap);
}

extern void debugOpen( fileName, isHeader, language )
    const char *const fileName;
    const boolean isHeader;
    const langType language;
{
    if (debug(DEBUG_STATUS))
    {
	if (language == LANG_IGNORE)
	    printf("  ignoring %s (unknown extension)\n", fileName);
	else
	{
	    const char *name = getLanguageName(language);

	    printf("OPENING %s as a %c%s language %sfile\n",
		fileName, toupper(name[0]), name + 1, isHeader ? "header ":"");
	}
	fflush(stdout);
    }
}

extern void debugPutc( c, level )
    const int c;
    const int level;
{
    if (debug(level)  &&  c != EOF)
    {
    	     if (c == STRING_SYMBOL)	printf("\"string\"");
    	else if (c == CHAR_SYMBOL)	printf("'c'");
	else				putchar(c);

	fflush(stdout);
    }
}

extern void debugEntry( scope, type, tagName, pMember )
    const tagScope scope;
    const tagType type;
    const char *const tagName;
    const memberInfo *const pMember;
{
    if (debug(DEBUG_PARSE))
    {
	printf("<#%s%s:%s", (scope == SCOPE_STATIC ? "static:" : ""),
	       tagTypeName(type), tagName);

	if (pMember->type != MEMBER_NONE)
	{
	    printf("[%s:%s]", getTypeString(pMember->type), pMember->parent);

	    if ((File.language == LANG_CPP  ||  File.language == LANG_JAVA) &&
		pMember->visibility != VIS_UNDEFINED)
	    {
		printf("{visibility:%s}",
		       getVisibilityString(pMember->visibility));
	    }
	}
	printf("#>");
	fflush(stdout);
    }
}

extern void debugParseNest( increase, level )
    const boolean increase;
    const unsigned int level;
{
    debugPrintf(DEBUG_PARSE, "<*%snesting:%d*>", increase ? "++" : "--", level);
}

extern void debugCppNest( begin, level )
    const boolean begin;
    const unsigned int level;
{
    debugPrintf(DEBUG_CPP, "<*cpp:%s level %d*>", begin ? "begin":"end", level);
}

extern void debugCppIgnore( ignore )
    const boolean ignore;
{
    debugPrintf(DEBUG_CPP, "<*cpp:%s ignore*>", ignore ? "begin":"end");
}

extern void clearString( string, length )
    char *const string;
    const int length;
{
    int i;

    for (i = 0 ; i < length ; ++i)
	string[i] = '\0';
}

#endif

/* vi:set tabstop=8 shiftwidth=4: */
