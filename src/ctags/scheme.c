/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for Scheme language
*   files.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <string.h>
#include <ctype.h>	/* to define isalpha(), isalnum(), isspace() */

#include "entry.h"
#include "read.h"
#include "vstring.h"

/*============================================================================
=   Function prototypes
============================================================================*/
static void makeFunctionTag __ARGS((const vString *const name));
static void makeSetTag __ARGS((const vString *const name));
static void readIdentifier __ARGS((vString *const name, const unsigned char *cp));

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

static void makeSetTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "set";
    e.kind     = 's';

    makeTagEntry(&e);
}

/* Algorithm adapted from from GNU etags.
 * Scheme tag functions
 * look for (def... xyzzy
 * look for (def... (xyzzy
 * look for (def ... ((...(xyzzy ....
 * look for (set! xyzzy
 */
static void readIdentifier( name, cp )
    vString *const name;
    const unsigned char *cp;
{
    const unsigned char *p;
    vStringClear(name);
    /* Go till you get to white space or a syntactic break */
    for (p = cp; *p != '\0' && *p != '(' && *p != ')' && !isspace(*p); p++)
	vStringPut(name, (int)*p);
    vStringTerminate(name);
}

extern void createSchemeTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	const unsigned char *cp = (const unsigned char*)line;

	if (cp[0] == '(' &&
	    (cp[1] == 'D' || cp[1] == 'd') &&
	    (cp[2] == 'E' || cp[2] == 'e') &&
	    (cp[3] == 'F' || cp[3] == 'f'))
	{
	    while (!isspace (*cp))
		cp++;
	    /* Skip over open parens and white space */
	    while (*cp != '\0' && (isspace (*cp) || *cp == '('))
		cp++;
	    readIdentifier(name, cp);
	    makeFunctionTag(name);
	}
	if (cp[0] == '(' &&
	    (cp[1] == 'S' || cp[1] == 's') &&
	    (cp[2] == 'E' || cp[2] == 'e') &&
	    (cp[3] == 'T' || cp[3] == 't') &&
	    (cp[4] == '!' || cp[4] == '!') &&
	    (isspace (cp[5])))
	{
	    while (*cp != '\0'  &&  !isspace (*cp))
		cp++;
	    /* Skip over white space */
	    while (isspace (*cp))
		cp++;
	    readIdentifier(name, cp);
	    makeSetTag(name);
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
