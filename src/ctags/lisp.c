/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for LISP files.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <ctype.h>	/* to define isalpha(), isalnum(), isspace() */
#include <string.h>

#include "entry.h"
#include "parse.h"
#include "read.h"
#include "vstring.h"

/*============================================================================
=   Function prototypes
============================================================================*/
static void makeFunctionTag __ARGS((const vString* const name));
static int L_isdef __ARGS((unsigned char *strp));
static int L_isquote __ARGS((unsigned char *strp));
static void L_getit __ARGS((vString *const name, unsigned char *dbp));

/*============================================================================
=   Function definitions
============================================================================*/

static void makeFunctionTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "function";
    e.kind     = 'f';

    makeTagEntry(&e);
}

/*
 * lisp tag functions
 *  look for (def or (DEF, quote or QUOTE
 */
static int L_isdef (strp)
    unsigned char *strp;
{
    return ((strp[1] == 'd' || strp[1] == 'D')
	  && (strp[2] == 'e' || strp[2] == 'E')
	  && (strp[3] == 'f' || strp[3] == 'F'));
}

static int L_isquote (strp)
    unsigned char *strp;
{
    return ((*(++strp) == 'q' || *strp == 'Q')
	  && (*(++strp) == 'u' || *strp == 'U')
	  && (*(++strp) == 'o' || *strp == 'O')
	  && (*(++strp) == 't' || *strp == 'T')
	  && (*(++strp) == 'e' || *strp == 'E')
	  && isspace (*(++strp)));
}

static void L_getit( name, dbp )
    vString* const name;
    unsigned char* dbp;
{
    unsigned char *cp;

    if (*dbp == '\'')		/* Skip prefix quote */
	dbp++;
    else if (*dbp == '(' && L_isquote (dbp)) /* Skip "(quote " */
    {
	dbp += 7;
	while (isspace (*dbp))
	dbp++;
    }
    for (cp = dbp ; *cp != '\0' && *cp != '(' && *cp != ' ' && *cp != ')'; cp++)
	vStringPut(name, (int)*cp);
    vStringTerminate(name);

    if (vStringLength(name) > 0)
	makeFunctionTag(name);
    vStringClear(name);
}

/* Algorithm adapted from from GNU etags.
 */
extern void createLispTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    unsigned char* dbp;


    while ((dbp = (unsigned char*)fileReadLine(vLine)) != NULL)
    {
	if (*dbp == '(')
	{
	    if (L_isdef(dbp))
	    {
		while (!isspace ((int)*dbp))
		    dbp++;
		while (isspace ((int)*dbp))
		    dbp++;
		L_getit(name, dbp);
	    }
	    else
	    {
		/* Check for (foo::defmumble name-defined ... */
		do
		    dbp++;
		while (*dbp && !isspace ((int)*dbp)
			&& *dbp != ':' && *dbp != '(' && *dbp != ')');
		if (*dbp == ':')
		{
		    do
			dbp++;
		    while (*dbp == ':');

		    if (L_isdef(dbp - 1))
		    {
			while (!isspace ((int)*dbp))
			    dbp++;
			while (isspace ((int)*dbp))
			    dbp++;
			L_getit(name, dbp);
		    }
		}
	    }
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
