/*****************************************************************************
*   $Id$
*
*   Copyright (c) 1998-2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   External interface to all language parsing modules.
*****************************************************************************/
#ifndef _PARSE_H
#define _PARSE_H

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include "strlist.h"

/*============================================================================
=   Data declarations
============================================================================*/

typedef void (*simpleParser) __ARGS((void));
typedef boolean (*rescanParser) __ARGS((const unsigned int passCount));

typedef enum eLangType {
    LANG_IGNORE = -1,		/* ignore file (unknown/unsupported language) */
    LANG_AUTO,			/* automatically determine language */
    LANG_ASM,
    LANG_AWK,
    LANG_BETA,
    LANG_C,
    LANG_CPP,
    LANG_COBOL,
    LANG_EIFFEL,
    LANG_FORTRAN,
    LANG_JAVA,
    LANG_LISP,
    LANG_PERL,
    LANG_PYTHON,
    LANG_SCHEME,
    LANG_SH,
    LANG_TCL,
    LANG_VIM,
    LANG_COUNT			/* count of languages */
} langType;

/*============================================================================
=   Function prototypes
============================================================================*/
extern stringList *languageExtensions __ARGS((const langType language));
extern langType getFileLanguage __ARGS((const char *const fileName));
extern const char *getLanguageName __ARGS((const langType language));
extern langType getLangType __ARGS((const char *const name));
extern void installLangMapDefault __ARGS((const langType language));
extern void installLangMapDefaults __ARGS((void));
extern boolean parseFile __ARGS((const char *const fileName));
extern void initParsers __ARGS((void));
extern void freeParserResources __ARGS((void));
extern boolean parseFile __ARGS((const char* const fileName));

/* Parsers */
extern void createAsmTags __ARGS((void));
extern void createAwkTags __ARGS((void));
extern void createBetaTags __ARGS((void));
extern void createCobolTags __ARGS((void));
extern void createEiffelTags __ARGS((void));
extern void createLispTags __ARGS((void));
extern void createPerlTags __ARGS((void));
extern void createPythonTags __ARGS((void));
extern void createSchemeTags __ARGS((void));
extern void createShTags __ARGS((void));
extern void createTclTags __ARGS((void));
extern void createVimTags __ARGS((void));

extern boolean createCTags __ARGS((const unsigned int passCount));
extern boolean createFortranTags __ARGS((const unsigned int passCount));

#endif	/* _PARSE_H */

/* vi:set tabstop=8 shiftwidth=4: */
