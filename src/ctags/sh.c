/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for Bourne shell
*   scripts.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <ctype.h>	/* to define isalpha(), isalnum(), isspace() */
#include <string.h>

#include "entry.h"
#include "main.h"
#include "parse.h"
#include "read.h"
#include "vstring.h"

/*============================================================================
=   Function prototypes
============================================================================*/
static void makeFunctionTag __ARGS((const vString* const name));
static boolean hackReject __ARGS((const vString* const tagName));

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

/*  Reject any tag "main" from a file named "configure". These appear in
 *  here-documents in GNU autoconf scripts and will add a haystack to the
 *  needle.
 */
static boolean hackReject( tagName )
    const vString* const tagName;
{
    const char *const scriptName = baseFilename(vStringValue(File.name));
    boolean result = (boolean)(strcmp(scriptName, "configure") == 0  &&
			       strcmp(vStringValue(tagName), "main") == 0);
    return result;
}

extern void createShTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	if (line[0] == '#')
	    continue;

	if (strchr(line, '(') != NULL)
	{
	    const unsigned char* cp = (const unsigned char*)line;

	    while (isspace (*cp))
		cp++;

	    if (strncmp((const char*)cp, "function", (size_t)8) == 0  &&
		isspace((int)cp[8]))
	    {
		cp += 8;
	    }
	    while (isspace((int)*cp))
		++cp;
	    while (isalnum((int)*cp)  ||  *cp == '_')
	    {
		vStringPut(name, (int)*cp);
		++cp;
	    }
	    vStringTerminate(name);
	    while (isspace((int)*cp))
		++cp;
	    if (*cp++ == '(')
	    {
		while (isspace((int)*cp))
		    ++cp;
		if (*cp == ')'  && ! hackReject (name))
		    makeFunctionTag(name);
	    }
	    vStringClear(name);
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
