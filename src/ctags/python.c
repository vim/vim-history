/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for Python language
*   files.
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
static void makeClassTag __ARGS((const vString* const name));
static void makeFunctionTag __ARGS((const vString* const name));

/*============================================================================
=   Function definitions
============================================================================*/

static void makeClassTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "class";
    e.kind     = 'c';

    makeTagEntry(&e);
}

static void makeFunctionTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "function";
    e.kind     = 'f';

    makeTagEntry(&e);
}

extern void createPythonTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;
    boolean inMultilineString = FALSE;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	const unsigned char *cp = (const unsigned char*)line;

	while (*cp != '\0')
	{
	    if (*cp=='"' && strncmp((const char*)cp, "\"\"\"", (size_t)3) == 0)
	    {
		inMultilineString = (boolean)!inMultilineString;
		cp += 3;
	    }
	    if (inMultilineString  ||  isspace((int)*cp))
		++cp;
	    else if (*cp == '#')
		break;
	    else if (strncmp((const char*)cp, "class", (size_t)5) == 0)
	    {
		cp += 5;
		if (isspace((int)*cp))
		{
		    while (isspace((int)*cp))
			++cp;
		    while (isalnum((int)*cp)  ||  *cp == '_')
		    {
			vStringPut(name, (int)*cp);
			++cp;
		    }
		    vStringTerminate(name);
		    makeClassTag(name);
		    vStringClear(name);
		}
	    }
	    else if (strncmp((const char*)cp, "def", (size_t)3) == 0)
	    {
		cp += 3;
		if (isspace((int)*cp))
		{
		    while (isspace((int)*cp))
			++cp;
		    while (isalnum((int)*cp)  ||  *cp == '_')
		    {
			vStringPut(name, (int)*cp);
			++cp;
		    }
		    vStringTerminate(name);
		    makeFunctionTag(name);
		    vStringClear(name);
		}
	    }
	    else
		++cp;
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
