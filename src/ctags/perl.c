/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for PERL language
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
static void makeSubTag __ARGS((const vString* const name));

/*============================================================================
=   Function definitions
============================================================================*/

static void makeSubTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "sub";
    e.kind     = 's';

    makeTagEntry(&e);
}

/* Algorithm adapted from from GNU etags.
 * Perl support by Bart Robinson <lomew@cs.utah.edu>
 * Perl sub names: look for /^sub[ \t\n]+[^ \t\n{]+/
 */
extern void createPerlTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	const unsigned char *cp = (const unsigned char*)line;

	if (line[0] != '#'  &&
	    *cp++ == 's' && *cp++ == 'u' && *cp++ == 'b' && isspace(*cp))
	{
	    while (isspace (*cp))
		cp++;
	    while (! isspace ((int)*cp) && *cp != '\0' && *cp != '{')
	    {
		vStringPut(name, (int)*cp);
		cp++;
	    }
	    vStringTerminate(name);
	    makeSubTag(name);
	    vStringClear(name);
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
