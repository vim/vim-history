/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for COBOL language
*   files.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <ctype.h>	/* to define isalpha(), isalnum(), isspace() */

#include "entry.h"
#include "parse.h"
#include "read.h"
#include "vstring.h"

/*============================================================================
=   Function prototypes
============================================================================*/
static void makeParagraphTag __ARGS((const vString* const name));

/*============================================================================
=   Function definitions
============================================================================*/

static void makeParagraphTag( name )
    const vString* const name;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(name));

    e.kindName = "paragraph";
    e.kind     = 'p';

    makeTagEntry(&e);
}

/* Algorithm adapted from from GNU etags.
 * Idea by Corny de Souza
 * Cobol tag functions
 * We could look for anything that could be a paragraph name.
 * i.e. anything that starts in column 8 is one word and ends in a full stop.
 */
extern void createCobolTags()
{
    vString *vLine = vStringNew();
    vString *name = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	const unsigned char *cp = (const unsigned char*)line;
	const unsigned char *dbp = cp + 8;

	/* If eoln, compiler option or comment ignore whole line. */
	if (vStringLength(vLine) > 8 && dbp[-1] == ' ' && isalnum((int)dbp[0]))
	{
	    for (cp = dbp  ;  isalnum((int)*cp) || *cp == '-'  ;  cp++)
		vStringPut(name, (int)*cp);

	    if (*cp++ == '.')
	    {
		vStringPut(name, (int)*cp);
		makeParagraphTag(name);
	    }
	    vStringTerminate(name);
	    vStringClear(name);
	}
    }
    vStringDelete(name);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
