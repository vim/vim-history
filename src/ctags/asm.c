/*****************************************************************************
*   $Id$
*
*   Copyright (c) 2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for assembly language
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
static void makeLabelTag __ARGS((const vString* const label));

/*============================================================================
=   Function definitions
============================================================================*/

static void makeLabelTag( label )
    const vString* const label;
{
    tagEntryInfo e;
    initTagEntry(&e, vStringValue(label));

    e.kindName = "label";
    e.kind     = 'l';

    makeTagEntry(&e);
}

/* Algorithm adapted from from GNU etags.
 * By Bob Weiner, Motorola Inc., 4/3/94
 * Unix and microcontroller assembly tag handling
 * look for '^[a-zA-Z_.$][a-zA_Z0-9_.$]*[: ^I^J]'
 */
extern void createAsmTags()
{
    vString *vLine = vStringNew();
    vString *label = vStringNew();
    const char *line;

    while ((line = fileReadLine(vLine)) != NULL)
    {
	const unsigned char *cp = (const unsigned char*)line;
	int c = *cp;

	/*  If first char is alphabetic or one of [_.$], test for colon
	 *  following identifier.
	 */
	if (isalpha(c) || c == '_' || c == '.' || c == '$')
	{
	    vStringPut(label, c);
	    c = *++cp;
	    while (isalnum(c) || c == '_' || c == '.' || c == '$')
	    {
		vStringPut(label, c);
		c = *++cp;
	    }
	    vStringTerminate(label);
	    if (c == ':' || isspace(c))
		makeLabelTag(label);
	    vStringClear(label);
	}
    }
    vStringDelete(label);
    vStringDelete(vLine);
}

/* vi:set tabstop=8 shiftwidth=4: */
