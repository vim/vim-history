/*****************************************************************************
*   $Id$
*
*   Copyright (c) 1999-2000, Mjølner Informatics
*
*   Written by Erik Corry <corry@mjolner.dk>
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for BETA language
*   files.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <string.h>

#include "entry.h"
#include "options.h"
#include "parse.h"
#include "read.h"
#include "vstring.h"

/*============================================================================
=   Macros
============================================================================*/
#define isbident(c) (identarray[(unsigned char)(c)])

/*============================================================================
=   Data definitions
============================================================================*/

/* [A-Z_a-z0-9] */
static const char identarray[256] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0-15  */
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 16-31 */
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 32-47    !"#$%&'()*+'-./ */
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  /* 48-63   0123456789:;<=>? */
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 64-79   @ABCDEFGHIJKLMNO */
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,  /* 80-95   PQRSTUVWXYZ[\]^_ */
0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 96-111  `abcdefghijklmno */
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,  /* 112-127  pqrstuvwxyz{|}~ */
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 128-  */
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /* -255  */

static const unsigned char strcasecmptable [256] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 
16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 
64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 91, 92, 93, 94, 95, 
96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 
112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 
128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 
144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 
160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 
176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 
192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 
208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 
224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 
240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255};

/*============================================================================
=   Function prototypes
============================================================================*/
static int mystrcasecmp __ARGS((const char *a, const char *b));
static int mystrncasecmp __ARGS((const char *a, const char *b, int count));

/*============================================================================
=   Function definitions
============================================================================*/

static int
mystrcasecmp(a, b)
    const char *a;
    const char *b;
{
    unsigned char ca, cb;
    for(; (ca = *a) | (cb = *b); a++, b++)
    {
        if (strcasecmptable[ca] != strcasecmptable[cb])
	    return strcasecmptable[ca] - strcasecmptable[cb];
    }
    return 0;
}

static int
mystrncasecmp(a, b, count)
    const char *a;
    const char *b;
    int count;
{
    unsigned char ca, cb;
    for(; (!!count) & ((ca = *a) | (cb = *b)); a++, b++, count--)
    {
        if (strcasecmptable[ca] != strcasecmptable[cb])
	    return strcasecmptable[ca] - strcasecmptable[cb];
    }
    return 0;
}

extern void createBetaTags()
{
    vString *line = vStringNew();
    boolean incomment = FALSE;
    boolean inquote = FALSE;
    boolean dofragments = Option.include.betaTypes.fragments;
    boolean dovirtuals = Option.include.betaTypes.virtuals;
    boolean dopatterns = Option.include.betaTypes.patterns;
    boolean doslots = Option.include.betaTypes.slots;

    do
    {
	boolean foundfragmenthere = FALSE;
	/* find fragment definition (line that starts and ends with --) */
	int last;
	int first;
	int c;

	vStringClear(line);

	while((c = fileGetc()) != EOF && c != '\n' && c != '\r')
	    vStringPut(line, c);

	vStringTerminate(line);

	last = vStringLength(line) - 1;
	first = 0;
	/* skip white space at start and end of line */
	while (last && isspace(vStringChar(line, last))) last--;
	while (first < last && isspace(vStringChar(line, first))) first++;
	/* if line still has a reasonable length and ... */
	if(last - first > 4 &&
	   (vStringChar(line, first)     == '-' && 
	    vStringChar(line, first + 1) == '-' && 
	    vStringChar(line, last)      == '-' && 
	    vStringChar(line, last - 1)  == '-'))
	{
	    if (!incomment && !inquote)
	    {
	        foundfragmenthere = TRUE;
		/* skip past -- and whitespace.  Also skip back past 'dopart'
		   or 'attributes' to the :.  We have to do this because there
		   is no sensible way to include whitespace in a ctags token
		   so the conventional space after the ':' would mess us up */
		last -= 2;
		first += 2;
		while (last && vStringChar(line, last) != ':') last--;
		while (last && (isspace(vStringChar(line, last-1)))) last--;
		while (first < last &&
		       (isspace(vStringChar(line, first)) ||
		        vStringChar(line, first) == '-'))
		    first++;
		/* If there's anything left it is a fragment title */
		if (first < last - 1)
		{
		    tagEntryInfo e;
		    vStringChar(line, last) = 0;
		    initTagEntry(&e, vStringValue(line) + first);
		    e.kindName = "fragment";
		    e.kind = 'f';
		    if (dofragments &&
		        mystrcasecmp("LIB", vStringValue(line) + first) &&
		        mystrcasecmp("PROGRAM", vStringValue(line) + first))
		        makeTagEntry(&e);
		}
	    }
	} else {
	    int pos = 0;
	    int len = vStringLength(line);
	    if (inquote) goto stringtext;
	    if (incomment) goto commenttext;
	programtext:
	    for ( ; pos < len; pos++)
	    {
		if (vStringChar(line, pos) == '\'')
		{
		    pos++;
		    inquote = TRUE;
		    goto stringtext;
		}
		if (vStringChar(line, pos) == '{')
		{
		    pos++;
		    incomment = TRUE;
		    goto commenttext;
		}
		if (vStringChar(line, pos) == '(' && pos < len - 1 &&
		    vStringChar(line, pos+1) == '*')
		{
		    pos +=2;
		    incomment = TRUE;
		    goto commenttext;
		}
		/*
		 * SLOT definition looks like this: 
		 * <<SLOT nameofslot: dopart>> 
		 * or
		 * <<SLOT nameofslot: descriptor>> 
		 */
		if (!foundfragmenthere &&
		    vStringChar(line, pos) == '<' &&
		    pos+1 < len &&
		    vStringChar(line, pos+1) == '<' &&
		    strstr(vStringValue(line) + pos, ">>"))
		{
		    /* Found slot name, get start and end */
		    int eoname;
		    char c2;
		    pos += 2; /* skip past << */
		    /* skip past space before SLOT */
		    while (pos < len &&
		           isspace(vStringChar(line, pos)))
		        pos++;
		    /* skip past SLOT */
		    if (pos+4 <= len &&
		        !mystrncasecmp(vStringValue(line) + pos, "SLOT", 4))
		        pos += 4;
		    /* skip past space after SLOT */
		    while (pos < len &&
		           isspace(vStringChar(line, pos)))
		        pos++;
		    eoname = pos;
		    /* skip to end of name */
		    while (eoname < len &&
		           (c2 = vStringChar(line, eoname)) != '>' &&
		           c2 != ':' &&
			   !isspace(c2))
		    	eoname++;
		    if (eoname < len && doslots)
		    {
			tagEntryInfo e;
		        vStringChar(line, eoname) = 0;
			initTagEntry(&e, vStringValue(line) + pos);
			e.kindName = "slot";
			e.kind = 's';
			if (mystrcasecmp("LIB", vStringValue(line) + pos) &&
			    mystrcasecmp("PROGRAM", vStringValue(line) + pos) &&
			    mystrcasecmp("SLOT", vStringValue(line) + pos))
			    makeTagEntry(&e);
		    }
		    if (eoname+1 < len) {
		       pos = eoname + 1;
		    } else {
		       pos = len;
		       continue;
		    }
		}
		/* Only patterns that are virtual, extensions of virtuals or
		 * final bindings are normally included so as not to overload
                 * totally.
		 * That means one of the forms name:: name:< or name::<
		 */
		if (!foundfragmenthere &&
		    vStringChar(line, pos) == ':' &&
                    (dopatterns ||
		     (dovirtuals &&
		      (vStringChar(line, pos+1) == ':' ||
		       vStringChar(line, pos+1) == '<')
                     )
		    )
                   )
		{
		    /* Found pattern name, get start and end */
		    int eoname = pos;
		    int soname;
		    while (eoname && isspace(vStringChar(line, eoname-1)))
			eoname--;
		foundanothername:
		    /* terminate right after name */
		    vStringChar(line, eoname) = 0;
		    soname = eoname;
		    while (soname &&
			isbident(vStringChar(line, soname-1)))
		    {
			soname--;
		    }
		    if(soname != eoname)
		    {
			tagEntryInfo e;
			initTagEntry(&e, vStringValue(line) + soname);
			e.kindName = "pattern";
			e.kind = 'p';
			makeTagEntry(&e);
			/* scan back past white space */
			while (soname &&
				isspace(vStringChar(line, soname-1)))
			    soname--;
			if (soname && vStringChar(line, soname-1) == ',')
			{
			    /* we found a new pattern name before comma */
			    eoname = soname;
			    goto foundanothername;
			}
		    }
		}
	    }
	    goto endofline;
	commenttext:
	    for ( ; pos < len; pos++)
	    {
		if (vStringChar(line, pos) == '*' && pos < len - 1 &&
		    vStringChar(line, pos+1) == ')')
		{
		    pos += 2;
		    incomment = FALSE;
		    goto programtext;
		}
		if (vStringChar(line, pos) == '}')
		{
		    pos++;
		    incomment = FALSE;
		    goto programtext;
		}
	    }
	    goto endofline;
	stringtext:
	    for ( ; pos < len; pos++)
	    {
		if (vStringChar(line, pos) == '\\')
		{
		    if (pos < len - 1) pos++;
		}
		else if (vStringChar(line, pos) == '\'')
		{
		    pos++;
		    /* support obsolete '' syntax */
		    if (pos < len && vStringChar(line, pos) == '\'')
		    {
			continue;
		    }
		    inquote = FALSE;
		    goto programtext;
		}
	    }
	}
	endofline:
	inquote = FALSE;  /* This shouldn't really make a difference */
    } while (!feof(File.fp));
}

/* vi:set tabstop=8 shiftwidth=4: */
