/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for TCL scripts.
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
static void makeProcedureTag __ARGS((const vString* const name));

/*============================================================================
=   Function definitions
============================================================================*/

static void makeProcedureTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "procedure";
    e.kind     = 'p';

    makeTagEntry(&e);
}

extern void createTclTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	if (strncmp(line, "proc", (size_t)4) == 0  &&  isspace((int)line[4]))
	{
	    const unsigned char *cp = (const unsigned char*)line + 4;
	    while (isspace((int)*cp))
		++cp;
	    while (isalnum((int)*cp) || *cp == '_' || *cp == '-' || *cp == ':')
	    {
		if (*cp == ':')
		    vStringClear(name);
		else
		    vStringPut(name, (int)*cp);
		++cp;
	    }
	    vStringTerminate(name);
	    makeProcedureTag(name);
	    vStringClear(name);
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
