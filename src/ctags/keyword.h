/*****************************************************************************
*   $Id$
*
*   Copyright (c) 1998-2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   External interface to keyword.c
*****************************************************************************/
#ifndef _KEYWORD_H
#define _KEYWORD_H

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include "parse.h"

/*============================================================================
=   Function prototypes
============================================================================*/
extern void addKeyword __ARGS((const char *const string, langType language, int value));
extern int lookupKeyword __ARGS((const char *const string, langType language));
extern void freeKeywordTable __ARGS((void));
#ifdef DEBUG
extern void printKeywordTable __ARGS((void));
#endif

#endif	/* _KEYWORD_H */

/* vi:set tabstop=8 shiftwidth=4: */
