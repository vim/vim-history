/*****************************************************************************
*   $Id$
*
*   Copyright (c) 1996-2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for managing source languages and
*   dispatching files to the appropriate language parser.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <string.h>

#include "debug.h"
#include "entry.h"
#include "main.h"
#include "options.h"
#include "parse.h"
#include "read.h"
#include "vstring.h"

/*============================================================================
=   Data declarations
============================================================================*/

typedef struct {
    const langType language;
    const char* const name;
    const char *const *const defaultExtensions;
    stringList* extensions;
    const simpleParser parseSimple;
    const rescanParser parseRescan;
} Language;

/*============================================================================
=   Data definitions
============================================================================*/

/*  File extension mappings.
 */
static const char *const AsmExtensions[] = { "asm", "s", "S", NULL };
static const char *const AwkExtensions[] = { "awk", "gawk", "mawk", NULL };
static const char *const BetaExtensions[] = { "bet", NULL };
static const char *const CExtensions[] = { "c", NULL };
static const char *const CppExtensions[] = {
    "c++", "cc", "cpp", "cxx", "h", "hh", "hpp", "hxx", "h++",
#ifndef CASE_INSENSITIVE_OS
    "C", "H",
#endif
    NULL
};
static const char *const CobolExtensions[] = { "cob", "COB", NULL };
static const char *const EiffelExtensions[] = { "e", NULL };
static const char *const FortranExtensions[] = {
    "f", "for", "ftn", "f77", "f90", "f95",
    "F", "FOR", "FTN", "F77", "F90", "F95",
    NULL
};
static const char *const JavaExtensions[] = { "java", NULL };
static const char *const LispExtensions[] = {
  "cl", "clisp", "el", "l", "lisp", "lsp", "ml", NULL
};
static const char *const PerlExtensions[] = { "pl", "pm", "perl", NULL };
static const char *const PythonExtensions[] = { "py", "python", NULL };
static const char *const SchemeExtensions[] = {
  "SCM", "SM", "sch", "scheme", "scm", "sm", NULL
};
static const char *const ShExtensions[] = { "sh", "SH", "bsh", "bash", "ksh", NULL };
static const char *const TclExtensions[] = { "tcl", "wish", NULL };
static const char *const VimExtensions[] = { "vim", NULL };

/* Entries must be in the same order as the langType enumerators.
 */
static Language LanguageTable[] = {
    { LANG_AUTO,    "auto",    NULL,              NULL, NULL,             NULL             },
    { LANG_ASM,     "asm",     AsmExtensions,     NULL, createAsmTags,    NULL             },
    { LANG_AWK,     "AWK",     AwkExtensions,     NULL, createAwkTags,    NULL             },
    { LANG_BETA,    "BETA",    BetaExtensions,    NULL, createBetaTags,   NULL             },
    { LANG_C,       "C",       CExtensions,       NULL, NULL,             createCTags      },
    { LANG_CPP,     "C++",     CppExtensions,     NULL, NULL,             createCTags      },
    { LANG_COBOL,   "COBOL",   CobolExtensions,   NULL, createCobolTags,  NULL             },
    { LANG_EIFFEL,  "Eiffel",  EiffelExtensions,  NULL, createEiffelTags, NULL             },
    { LANG_FORTRAN, "Fortran", FortranExtensions, NULL, NULL,             createFortranTags},
    { LANG_JAVA,    "Java",    JavaExtensions,    NULL, NULL,             createCTags      },
    { LANG_LISP,    "Lisp",    LispExtensions,    NULL, createLispTags,   NULL             },
    { LANG_PERL,    "Perl",    PerlExtensions,    NULL, createPerlTags,   NULL             },
    { LANG_PYTHON,  "Python",  PythonExtensions,  NULL, createPythonTags, NULL             },
    { LANG_SCHEME,  "Scheme",  SchemeExtensions,  NULL, createSchemeTags, NULL             },
    { LANG_SH,      "sh",      ShExtensions,      NULL, createShTags,     NULL             },
    { LANG_TCL,     "Tcl",     TclExtensions,     NULL, createTclTags,    NULL             },
    { LANG_VIM,     "Vim",     VimExtensions,     NULL, createVimTags,    NULL             }
};

/*============================================================================
=   Function prototypes
============================================================================*/
static langType getExtensionLanguage __ARGS((const char *const extension));
static langType getInterpreterLanguage __ARGS((const char *const fileName));
static void freeLanguageMaps __ARGS((void));
static boolean createTagsForFile __ARGS((const char *const fileName, const langType language, const unsigned int passCount));
static boolean createTagsWithFallback __ARGS((const char *const fileName, const langType language));
static void makeFileTag __ARGS((const char* const fileName));

/*============================================================================
=   Function definitions
============================================================================*/

/*----------------------------------------------------------------------------
*-	Language mapping management
----------------------------------------------------------------------------*/

extern const char *getLanguageName( language )
    const langType language;
{
    return LanguageTable[(int)language].name;
}

extern stringList* languageExtensions( language )
    const langType language;
{
    return LanguageTable[(int)language].extensions;
}

extern langType getLangType( name )
    const char *const name;
{
    unsigned int i;
    langType language = LANG_IGNORE;

    for (i = 0  ;  i < LANG_COUNT  ;  ++i)
    {
	if (strequiv(name, getLanguageName((langType)i)))
	{
	    language = (langType)i;
	    break;
	}
    }
    return language;
}

static langType getExtensionLanguage( extension )
    const char *const extension;
{
    langType language = LANG_IGNORE;
    unsigned int i;
    Assert(sizeof(LanguageTable)/sizeof(Language) == LANG_COUNT);
    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
    {
	const Language* const lang = &LanguageTable[i];
	Assert(lang->language == (langType)i);
	if (lang->extensions != NULL)
	{
#ifdef CASE_INSENSITIVE_OS
	    if (stringListHasInsensitive(lang->extensions, extension))
#else
	    if (stringListHas(lang->extensions, extension))
#endif
	    {
		language = (langType)i;
		break;
	    }
	}
    }
    return language;
}

static langType getInterpreterLanguage( fileName )
    const char *const fileName;
{
    langType result = LANG_IGNORE;

    if (isExecutable(fileName))
    {
	FILE* const fp = fopen(fileName, "r");
	if (fp != NULL)
	{
	    vString* const vLine = vStringNew();
	    const char* const line = readLine(vLine, fp);
	    if (line != NULL  &&  line[0] == '#'  &&  line[1] == '!')
	    {
		vString* const interpreter = vStringNew();
		const char* const lastSlash = strrchr(line, '/');
		const char *cp;
		if (lastSlash != NULL)
		    cp = lastSlash + 1;
		else
		    for (cp = line + 2  ;  isspace(*cp)  ;  ++cp)
			; /* no-op */
		for ( ;  *cp != '\0'  &&  ! isspace(*cp)  ;  ++cp)
		    vStringPut(interpreter, (int)*cp);
		result = getExtensionLanguage(vStringValue(interpreter));
		vStringDelete(interpreter);
	    }
	    vStringDelete(vLine);
	    fclose(fp);
	}
    }
    return result;
}

extern langType getFileLanguage( fileName )
    const char *const fileName;
{
    const char *const extension = findExtension(fileName);
    langType language;

    if (Option.language != LANG_AUTO)
	language = Option.language;
    else
	language = getExtensionLanguage(extension);

    return language;
}

extern void installLangMapDefault( language )
    const langType language;
{
    const int i = (int)language;
    freeList(&LanguageTable[i].extensions);
    if (LanguageTable[i].defaultExtensions != NULL)
    {
	LanguageTable[i].extensions =
	    stringListNewFromArgv(LanguageTable[i].defaultExtensions);
	if (Option.verbose)
	{
	    printf("    Setting default %s map: ", getLanguageName(language));
	    stringListPrint(LanguageTable[i].extensions);
	    putchar ('\n');
	}
    }
}

extern void installLangMapDefaults()
{
    unsigned int i;
    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
	installLangMapDefault((langType)i);
}

static void freeLanguageMaps()
{
    unsigned int i;
    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
	freeList(&LanguageTable[i].extensions);
}

extern void initParsers()
{
    installLangMapDefaults();
}

extern void freeParserResources()
{
    freeLanguageMaps();
}

/*----------------------------------------------------------------------------
*-	File parsing
----------------------------------------------------------------------------*/

static void makeFileTag( fileName )
    const char* const fileName;
{
    if (Option.include.fileNames)
    {
	tagEntryInfo tag;
	initTagEntry(&tag, baseFilename(fileName));

	tag.isFileEntry     = TRUE;
	tag.lineNumberEntry = TRUE;
	tag.lineNumber      = 1;
	tag.kindName        = "file";
	tag.kind            = 'F';

	makeTagEntry(&tag);
    }
}

static boolean createTagsForFile( fileName, language, passCount )
    const char *const fileName;
    const langType language;
    const unsigned int passCount;
{
    boolean retried = FALSE;

    if (Option.etags)
	beginEtagsFile();

    if (fileOpen(fileName, language))
    {
	const Language* const entry = &LanguageTable[(int)language];
	Assert(entry != NULL);
	Assert(entry->language == language);

	if (entry->parseSimple != NULL)
	    entry->parseSimple();
	else if (entry->parseRescan != NULL)
	    retried = entry->parseRescan(passCount);

	fileClose();
    }

    if (Option.etags)
	endEtagsFile(fileName);

    return retried;
}

static boolean createTagsWithFallback( fileName, language )
    const char *const fileName;
    const langType language;
{
    const unsigned long numTags	= TagFile.numTags.added;
    fpos_t tagFilePosition;
    unsigned int passCount = 0;
    boolean tagFileResized = FALSE;

    fgetpos(TagFile.fp, &tagFilePosition);
    while (createTagsForFile(fileName, language, ++passCount))
    {
	/*  Restore prior state of tag file.
	 */
	fsetpos(TagFile.fp, &tagFilePosition);
	TagFile.numTags.added = numTags;
	tagFileResized = TRUE;
    }
    return tagFileResized;
}

extern boolean parseFile( fileName )
    const char* const fileName;
{
    boolean tagFileResized = FALSE;
    langType language = Option.language == LANG_AUTO ?
	     getFileLanguage(fileName) : Option.language;

    if (Option.language != LANG_AUTO)
	language = Option.language;
    else
    {
	language = getFileLanguage(fileName);
	if (language == LANG_IGNORE)
	    language = getInterpreterLanguage(fileName);
    }
    Assert(language != LANG_AUTO);
    if (language != LANG_IGNORE)
    {
	if (Option.filter)
	    openTagFile();

	makeFileTag(fileName);
	tagFileResized = createTagsWithFallback(fileName, language);

	if (Option.filter)
	    closeTagFile(tagFileResized);
	addTotals(1, 0L, 0L);

	return tagFileResized;
    }
    else if (Option.verbose)
	printf("ignoring %s (unknown language)\n", fileName);

    return tagFileResized;
}

/* vi:set tabstop=8 shiftwidth=4 nowrap: */
