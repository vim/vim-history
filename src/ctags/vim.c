/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for Vim functions.
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

extern void createVimTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	if (strncmp(line, "fu", (size_t)2) == 0)
	{
	    const unsigned char *cp = (const unsigned char*)line + 1;
	    if ((int)*++cp == 'n'  &&  (int)*++cp == 'c'  &&
		(int)*++cp == 't'  &&  (int)*++cp == 'i'  &&
		(int)*++cp == 'o'  &&  (int)*++cp == 'n')
		    ++cp;
	    if ((int)*cp == '!')
		++cp;
	    if (isspace((int)*cp))
	    {
		while (isspace((int)*cp))
		    ++cp;
		if (isupper((int)*cp))
		{
		    do
		    {
			vStringPut(name, (int)*cp);
			++cp;
		    } while (isalnum((int)*cp)  ||  *cp == '_');
		    vStringTerminate(name);
		    makeFunctionTag(name);
		    vStringClear(name);
		}
	    }
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
