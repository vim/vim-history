/*****************************************************************************
*   $Id: debug.c,v 5.1 1998/02/19 03:47:18 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
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
#include "ctags.h"

/*============================================================================
=   Function definitions
============================================================================*/

#ifdef DEBUG

extern void lineBreak() {}	/* provides a line-specified break point */

extern void debugOpen( name )
    const char *const name;
{
    if (debug(DEBUG_STATUS))
    {
	printf("Opening: %s\n", name);
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
    if (debug(DEBUG_VISUAL | DEBUG_CPP))
    {
	const char *typeString;

	printf("<#%s%s:%s", (scope == SCOPE_STATIC ? "static:" : ""),
	       tagTypeName(type), tagName);

	switch (pMember->type)
	{
	    case MEMBER_ENUM:	typeString = "enum";	break;
	    case MEMBER_CLASS:	typeString = "class";	break;
	    case MEMBER_STRUCT:	typeString = "struct";	break;
	    case MEMBER_UNION:	typeString = "union";	break;
	    default:		typeString = NULL;	break;
	}
	if (typeString != NULL)
	    printf("[%s:%s]", typeString, pMember->parent);
	printf("#>");
	fflush(stdout);
    }
}

extern void debugParseNest( increase, level )
    const boolean increase;
    const unsigned int level;
{
    if (debug(DEBUG_VISUAL))
    {
	printf("<*%snesting:%d*>", increase ? "++" : "--", level);
	fflush(stdout);
    }
}

extern void debugCppNest( begin, level )
    const boolean begin;
    const unsigned int level;
{
    if (debug(DEBUG_VISUAL | DEBUG_CPP))
    {
	printf("<*cpp:%s level %d*>", begin ? "begin" : "end", level);
	fflush(stdout);
    }
}

extern void debugCppIgnore( ignore )
    const boolean ignore;
{
    if (debug(DEBUG_VISUAL | DEBUG_CPP))
    {
	printf("<*cpp:%s ignore*>", ignore ? "begin" : "end");
	fflush(stdout);
    }
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
