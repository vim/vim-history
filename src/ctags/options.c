/*****************************************************************************
*   $Id: options.c,v 8.4 1999/05/29 17:56:51 darren Exp $
*
*   Copyright (c) 1996-1999, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions to process command line options.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"

#ifdef HAVE_STDLIB_H
# include <stdlib.h>	    /* to declare malloc(), realloc() */
#endif
#include <string.h>
#include <stdio.h>
#include <ctype.h>	/* to declare toupper() */

#include "ctags.h"
#define OPTION_WRITE
#include "options.h"

#include "main.h"
#include "debug.h"

/*============================================================================
=   Defines
============================================================================*/

#if defined(MSDOS) || defined(WIN32) || defined(OS2) || defined(__vms)
# define CASE_INSENSITIVE_OS
#endif

#define INVOCATION  "Usage: %s [options] [file(s)]\n"

#define CTAGS_ENVIRONMENT	"CTAGS"
#define ETAGS_ENVIRONMENT	"ETAGS"

#define CTAGS_FILE	"tags"
#define ETAGS_FILE	"TAGS"

#ifndef ETAGS
# define ETAGS	"etags"		/* name which causes default use of to -e */
#endif

/*  The following separators are permitted for list options.
 */
#define EXTENSION_SEPARATORS   "."
#define IGNORE_SEPARATORS   ", \t\n"

#ifndef DEFAULT_FILE_FORMAT
# define DEFAULT_FILE_FORMAT	2
#endif

#if defined(MSDOS) || defined(WIN32) || defined(OS2) || defined(AMIGA) || defined(HAVE_OPENDIR)
# define RECURSE_SUPPORTED
#endif

#define SLIST_INCREASE	10

/*============================================================================
=   Data declarations
============================================================================*/

enum eOptionLimits {
    MaxHeaderExtensions	= 100,	/* maximum number of extensions in -h option */
    MaxSupportedTagFormat = 2
};

typedef struct sOptionDescription {
    int usedByEtags;
    const char *const description;
} optionDescription;

typedef struct sLangMapDefault {
    langType language;
    const char *const *const defaultMap;
} langMapping;

/*============================================================================
=   Data definitions
============================================================================*/

static boolean StartedAsEtags = FALSE;

static const char *const CExtensions[] = {
    "c", NULL
};
static const char *const CppExtensions[] = {
    "c++", "cc", "cpp", "cxx", "h", "hh", "hpp", "hxx", "h++",
#ifndef CASE_INSENSITIVE_OS
    "C", "H",
#endif
    NULL
};
static const char *const EiffelExtensions[] = {
    "e", NULL
};
static const char *const FortranExtensions[] = {
    "f", "for", "ftn", "f77", "f90",
#ifndef CASE_INSENSITIVE_OS
    "F", "FOR", "FTN", "F77", "F90",
#endif
    NULL
};
static const char *const JavaExtensions[] = {
    "java", NULL
};
static const char *const HeaderExtensions[] = {
    "h", "hh", "hpp", "hxx", "h++", "inc", "def",
#ifndef CASE_INSENSITIVE_OS
    "H",
#endif
    NULL
};

static const langMapping langMapDefaults[] = {
    { LANG_C,		CExtensions		},
    { LANG_CPP,		CppExtensions		},
    { LANG_EIFFEL,	EiffelExtensions	},
    { LANG_FORTRAN,	FortranExtensions	},
    { LANG_JAVA,	JavaExtensions		}
};

optionValues Option = {
    {	    /* include */
	{	/* c */
	    TRUE,		/* -ic */
	    TRUE,		/* -id */
	    TRUE,		/* -ie */
	    TRUE,		/* -if */
	    TRUE,		/* -ig */
	    TRUE,		/* -im */
	    TRUE,		/* -in */
	    FALSE,		/* -ip */
	    TRUE,		/* -is */
	    TRUE,		/* -it */
	    TRUE,		/* -iu */
	    TRUE,		/* -iv */
	    FALSE,		/* -ix */
	    FALSE,		/* -iA */
	    FALSE		/* -iC */
	},
	{	/* eiffel */
	    TRUE,		/* -c */
	    TRUE,		/* -f */
	    FALSE		/* -l */
	},
	{	/* fortran */
	    TRUE,		/* -b */
	    TRUE,		/* -c */
	    TRUE,		/* -e */
	    TRUE,		/* -f */
	    TRUE,		/* -i */
	    TRUE,		/* -l */
	    TRUE,		/* -m */
	    TRUE,		/* -n */
	    TRUE,		/* -p */
	    TRUE,		/* -s */
	    TRUE		/* -t */
	},
	{	/* java */
	    TRUE,		/* -c */
	    TRUE,		/* -f */
	    TRUE,		/* -i */
	    TRUE,		/* -m */
	    TRUE,		/* -p */
	    FALSE,		/* -iA */
	    FALSE		/* -iC */
	},
	FALSE,		/* -iF */
	TRUE,		/* -iS */
    },
    EMPTY_SLIST,	/* -I */
    FALSE,		/* -a */
    FALSE,		/* -B */
    FALSE,		/* -e */
#ifdef MACROS_USE_PATTERNS
    EX_PATTERN,		/* -n, --excmd */
#else
    EX_MIX,		/* -n, --excmd */
#endif
    NULL,		/* -p */
    FALSE,		/* -R */
    TRUE,		/* -u, --sort */
    FALSE,		/* -V */
    FALSE,		/* -x */
    NULL,		/* -L */
    NULL,		/* -o */
    EMPTY_SLIST,	/* -h */
    EMPTY_SLIST,	/* --etags-include */
    DEFAULT_FILE_FORMAT,/* --format */
    FALSE,		/* --if0 */
    FALSE,		/* --kind-long */
    LANG_AUTO,		/* --lang */
    {			/* --langmap */
	EMPTY_SLIST, EMPTY_SLIST, EMPTY_SLIST, EMPTY_SLIST, EMPTY_SLIST
    },
    TRUE,		/* --links */
    FALSE,		/* --totals */
    FALSE,		/* --line-directives */
#ifdef DEBUG
    0, 0		/* -D, -b */
#endif
};

/*----------------------------------------------------------------------------
-   Locally used only
----------------------------------------------------------------------------*/

static optionDescription LongOptionDescription[] = {
 {1,"  -a   Append the tags to an existing tag file."},
#ifdef DEBUG
 {1,"  -b <line>"},
 {1,"       Set break line."},
#endif
 {0,"  -B   Use backward searching patterns (?...?)."},
#ifdef DEBUG
 {1,"  -D <level>"},
 {1,"       Set debug level."},
#endif
 {0,"  -e   Output tag file for use with Emacs."},
 {1,"  -f <name>"},
 {1,"       Output tags to the specified file (default is \"tags\"; or \"TAGS\""},
 {1,"       if -e is specified). If specified as \"-\", tags are written to"},
 {1,"       standard output."},
 {0,"  -F   Use forward searching patterns (/.../) (default)."},
 {1,"  -h <list>"},
 {1,"       Specifies a list of file extensions used for headers."},
 {1,"       The default list is \".h.H.hh.hpp.hxx.h++\"."},
 {1,"  -i <types>"},
 {1,"       Nearly equivalent to --c-types=<types>."},
 {1,"  -I <list | file>"},
 {1,"       A list of tokens to be specially handled is read from either the"},
 {1,"       command line or the specified file."},
 {1,"  -L <file>"},
 {1,"       A list of source file names are read from the specified file."},
 {1,"       If specified as \"-\", then standard input is read."},
 {0,"  -n   Equivalent to --excmd=number."},
 {0,"  -N   Equivalent to --excmd=pattern."},
 {1,"  -o   Alternative for -f."},
 {1,"  -p <path>"},
 {1,"       Default path to use for all (relative path) filenames."},
#ifdef RECURSE_SUPPORTED
 {1,"  -R   Equivalent to --recurse=yes."},
#else
 {1,"  -R   Not supported on this platform."},
#endif
 {0,"  -u   Equivalent to --sort=no."},
 {1,"  -V   Enable verbose messages describing actions on each source file."},
 {1,"  -x   Print a tabular cross reference file to standard output."},
 {1,"  --append=[yes|no]"},
 {1,"       Indicates whether tags should be appended to existing tag file"},
 {1,"       (default=no)."},
 {1,"  --c-types=types"},
 {1,"       Specifies a list of C/C++ language tag types to include in the"},
 {1,"       output file. \"Types\" is a group of one-letter flags designating"},
 {1,"       types of tags to either include or exclude from the output. Each"},
 {1,"       letter or group of letters may be preceded by either '+' (default,"},
 {1,"       if omitted) to add it to those already included, or '-' to exclude"},
 {1,"       it from the output. In the absence of any preceding '+' or '-'"},
 {1,"       sign, only those types listed in \"types\" will be included in the"},
 {1,"       output. Tags for the following language contructs are supported"},
 {1,"       (types are enabled by default except as noted):"},
 {1,"          c   classes"},
 {1,"          d   macro definitions"},
 {1,"          e   enumerators (values inside an enumeration)"},
 {1,"          f   function definitions"},
 {1,"          g   enumeration names"},
 {1,"          m   class, struct, and union members"},
 {1,"          n   namespaces"},
 {1,"          p   function prototypes [off]"},
 {1,"          s   structure names"},
 {1,"          t   typedefs"},
 {1,"          u   union names"},
 {1,"          v   variable definitions"},
 {1,"          x   external variable declarations [off]"},
 {1,"       In addition, the following modifiers are accepted:"},
 {1,"          A   record the access of members into the tag file [off]"},
 {1,"          C   include extra, class-qualified tag entries for members [off]"},
 {1,"  --etags-include=file"},
 {1,"      Include reference to 'file' in Emacs-style tag file (requires -e)."},
 {0,"  --excmd=number|pattern|mix"},
#ifdef MACROS_USE_PATTERNS
 {0,"       Uses the specified type of EX command to locate tags (default=pattern)."},
#else
 {0,"       Uses the specified type of EX command to locate tags (default=mix)."},
#endif
 {1,"  --eiffel-types=types"},
 {1,"       Specifies a list of Eiffel language tag types to be included in the"},
 {1,"       output. See --c-types for the definition of the format of \"types\"."},
 {1,"       Tags for the following Eiffel language contructs are supported"},
 {1,"       (types are enabled by default except as noted):"},
 {1,"          c   classes"},
 {1,"          f   features"},
 {1,"          l   local entities [off]"},
 {1,"  --file-scope=[yes|no]"},
 {1,"       Indicates whether tags scoped only for a single file (e.g. \"static\""},
 {1,"       tags) should be included in the output (default=yes)."},
 {1,"  --file-tags=[yes|no]"},
 {1,"       Indicates whether tags should be generated for source file names"},
 {1,"       (default=no)."},
 {0,"  --format=level"},
#if DEFAULT_FILE_FORMAT==1
 {0,"       Forces output of specified tag file format (default=1)."},
#else
 {0,"       Forces output of specified tag file format (default=2)."},
#endif
 {1,"  --fortran-types=types"},
 {1,"       Specifies a list of Fortran language tag types to be included in the"},
 {1,"       output. See --c-types for the definition of the format of \"types\"."},
 {1,"       Tags for the following Fortran language contructs are supported"},
 {1,"       (all are enabled by default):"},
 {1,"          b   block data"},
 {1,"          c   common blocks"},
 {1,"          e   entry points"},
 {1,"          f   functions"},
 {1,"          i   interfaces"},
 {1,"          l   labels"},
 {1,"          m   modules"},
 {1,"          m   namelists"},
 {1,"          p   programs"},
 {1,"          s   subroutines"},
 {1,"          t   derived types"},
 {1,"  --help"},
 {1,"       Prints this option summary."},
 {1,"  --if0=[yes|no]"},
 {1,"       Indicates whether code within #if 0 conditional branches should"},
 {1,"       be examined for tags (default=no)."},
 {1,"  --java-types=types"},
 {1,"       Specifies a list of Java language tag types to be included in the"},
 {1,"       output. See --c-types for the definition of the format of \"types\"."},
 {1,"       Tags for the following Java language contructs are supported (all"},
 {1,"       are enabled by default):"},
 {1,"          c   classes"},
 {1,"          f   fields"},
 {1,"          i   interfaces"},
 {1,"          m   methods"},
 {1,"          p   packages"},
 {1,"       In addition, the following modifiers are accepted:"},
 {1,"          A   record the access of fields into the tag file [off]"},
 {1,"          C   include extra, class-qualified tag entries for fields [off]"},
 {1,"  --kind-long=[yes|no]"},
 {1,"       Indicates whether verbose tag descriptions are placed into tag file"},
 {1,"       (default=no)."},
 {1,"  --lang=[c|c++|eiffel|fortran|java]"},
 {1,"       Forces specified language, disabling automatic selection."},
 {1,"  --langmap=map(s)"},
 {1,"       Overrides the default mapping of language to source file extension."},
 {0,"  --line-directives=[yes|no]"},
 {0,"       Indicates whether #line directives should be processed (default=no)."},
 {1,"  --links=[yes|no]"},
 {1,"       Indicates whether symbolic links should be followed (default=yes)."},
 {1,"  --recurse=[yes|no]"},
#ifdef RECURSE_SUPPORTED
 {1,"       Recurse into directories supplied on command line (default=no)."},
#else
 {1,"       Not supported on this platform."},
#endif
 {0,"  --sort=[yes|no]"},
 {0,"       Indicates whether tags should be sorted (default=yes)."},
 {1,"  --totals=[yes|no]"},
 {1,"       Prints statistics about source and tag files (default=no)."},
 {1,"  --version"},
 {1,"       Prints a version identifier to standard output."},
 {1, NULL}
};

/*  Contains a set of strings describing the set of "features" compiled into
 *  the code.
 */
static const char *const Features[] = {
#ifdef DEBUG
    "debug",
#endif
#ifdef WIN32
    "win32",
#endif
#ifdef DJGPP
    "msdos_32",
#else
# ifdef MSDOS
    "msdos_16",
# endif
#endif
#ifdef OS2
    "os2",
#endif
#ifdef AMIGA
    "amiga",
#endif
#ifdef __vms
    "vms",
#endif
#ifndef EXTERNAL_SORT
    "internal_sort",
#endif
    NULL
};

/*============================================================================
=   Function prototypes
============================================================================*/
static boolean stringListAdd __ARGS((stringList *const slist, vString *string));
static void stringListClear __ARGS((stringList *const slist));
static void stringListDelete __ARGS((stringList *const slist));
static boolean stringListHas __ARGS((stringList *const slist, const char *const string, const boolean caseInsensitive));
static const char *findExtension __ARGS((const char *const fileName));
static langType getExtensionLanguage __ARGS((const char *const extension));
static void printfFeatureList __ARGS((FILE *const where));
static void printProgramIdentification __ARGS((FILE *const where));
static void printInvocationDescription __ARGS((FILE *const where));
static void printOptionDescriptions __ARGS((const optionDescription *const optDesc, FILE *const where));
static void printHelp __ARGS((const optionDescription *const optDesc));
static void saveExtensionList __ARGS((stringList *const slist, const char *const elist, const boolean clear));
static void processCTypesOption __ARGS((const char *const optionName, const char *const parameter, const boolean longOption));
static void processEiffelTypesOption __ARGS((const char *const optionName, const char *const parameter));
static void processFortranTypesOption __ARGS((const char *const optionName, const char *const parameter));
static void processJavaTypesOption __ARGS((const char *const optionName, const char *const parameter));
static void saveIgnoreToken __ARGS((vString *const ignoreToken));
static void readIgnoreList __ARGS((char *const list));
static void readIgnoreListFromFile __ARGS((const char *const fileName));
static void processHeaderListOption __ARGS((const int option, char **const argP, char *const *const argList, int *const argNumP));
static void processIgnoreOption __ARGS((const int option, char **const argP, char *const *const argList, int *const argNumP));
static void processExcmdOption __ARGS((const char *const optionName, const char *const parameter));
static void processFormatOption __ARGS((const char *const optionName, const char *const parameter));
static langType getLangType __ARGS((const char *const name));
static void processLangOption __ARGS((const char *const optionName, const char *const parameter));
static boolean installLangMap __ARGS((char *const map));
static void processLangMapOption __ARGS((const char *const optionName, const char *const parameter));
static void processEtagsInclude __ARGS((const char *const optionName, const char *const parameter));
static boolean getBooleanOption __ARGS((const char *const optionName, const char *const parameter, const boolean defaultValue));
static boolean validateOption __ARGS((const char *const ref, const char *const optionName, const char *const parameter, const boolean parameterRequired));
static void processLongOption __ARGS((const char *const optionString));
static char *readOptionArg __ARGS((const int option, char **const pArg, char *const *const argList, int *const pArgNum));
static void processCompoundOption __ARGS((const int option, char **const pArg, char *const *const argList, int *const pArgNum));
static boolean processSimpleOption __ARGS((const int option));
static void parseStringToArgs __ARGS((const char *const string, char *parsedBuffer, char **const argList, const unsigned int maxArgs));
static unsigned int countStringWords __ARGS((const char *const string));
static char **creatArgListForString __ARGS((const char *const string));
static void installExtensionList __ARGS((stringList *const slist, const char *const *const list));
static void installLangMapDefaults __ARGS((void));
static void freeLangMap __ARGS((void));

/*============================================================================
=   Function definitions
============================================================================*/

extern void setDefaultTagFileName()
{
    if (Option.tagFileName != NULL)
	;		/* accept given name */
    else if (Option.etags)
	Option.tagFileName = ETAGS_FILE;
    else
	Option.tagFileName = CTAGS_FILE;
}

extern void setOptionDefaults()
{
    if (Option.xref)
	Option.include.fileNames = FALSE;

    setDefaultTagFileName();
}

extern void testEtagsInvocation()
{
    if (strncmp(getExecutableName(), ETAGS, strlen(ETAGS)) == 0)
    {
	StartedAsEtags = TRUE;

	Option.etags		= TRUE;
	Option.sorted		= FALSE;
	Option.lineDirectives	= FALSE;
    }
}

/*----------------------------------------------------------------------------
 *  Generic resizable string list management
 *--------------------------------------------------------------------------*/

static boolean stringListAdd( slist, string )
    stringList *const slist;
    vString *string;
{
    boolean added = TRUE;

    if (slist->list == NULL)
    {
	Assert(slist->max == 0);
	slist->count = 0;
	slist->max   = SLIST_INCREASE;
	slist->list  = (vString **)eMalloc((size_t)slist->max *
					   sizeof(vString *));
    }
    else if (slist->count == slist->max)
    {
	slist->max += SLIST_INCREASE;
	slist->list = (vString **)eRealloc(slist->list, (size_t)slist->max *
					   sizeof(vString *));
    }
    slist->list[slist->count++] = string;

    return added;
}

static void stringListClear( slist )
    stringList *const slist;
{
    unsigned int i;

    for (i = 0  ;  i < slist->count  ;  ++i)
	vStringDelete(slist->list[i]);

    slist->count = 0;
}

static void stringListDelete( slist )
    stringList *const slist;
{
    stringListClear(slist);
    free(slist->list);
}

static boolean stringListHas( slist, string, caseInsensitive )
    stringList *const slist;
    const char *const string;
    const boolean caseInsensitive;
{
    boolean found = FALSE;
    unsigned int i;

    for (i = 0  ;  ! found  &&  i < slist->count  ;  ++i)
    {
	vString *const listEntry = slist->list[i];

	if (caseInsensitive)
	{
	    if (strequiv(string, vStringValue(listEntry)))
		found = TRUE;
	}
	else
	{
	    if (strcmp(string, vStringValue(listEntry)) == 0)
		found = TRUE;
	}
    }
    return found;
}

#ifdef DEBUG

static void stringListPrint __ARGS((stringList *const slist));

static void stringListPrint( slist )
    stringList *const slist;
{
    unsigned int i;

    for (i = 0  ;  i < slist->count  ;  ++i)
    {
	vString *const listEntry = slist->list[i];

	printf("%s%s", (i > 0) ? ", " : "", vStringValue(listEntry));
    }
}
#endif

/*----------------------------------------------------------------------------
 *  File extension and language handling
 *--------------------------------------------------------------------------*/

static const char *findExtension( fileName )
    const char *const fileName;
{
    const char *extension;
    const char *pDelimiter = NULL;
#ifdef QDOS
    pDelimiter = strrchr(fileName, '_');
#endif
    if (pDelimiter == NULL)
        pDelimiter = strrchr(fileName, '.');

    if (pDelimiter == NULL)
	extension = "";
    else
	extension = pDelimiter + 1;	/* skip to first char of extension */

    return extension;
}

/*  Determines whether the specified file name is considered to be a header
 *  file for the purposes of determining whether enclosed tags are global or
 *  static.
 */
extern boolean isFileHeader( fileName )
    const char *const fileName;
{
    const char *const extension = findExtension(fileName);
#if defined(MSDOS) || defined(WIN32) || defined(OS2) || defined(__vms)
    const boolean caseInsensitive = TRUE;
#else
    const boolean caseInsensitive = FALSE;
#endif
    boolean header=stringListHas(&Option.headerExt, extension, caseInsensitive);

    return header;
}

static langType getExtensionLanguage( extension )
    const char *const extension;
{
    unsigned int i;
    langType language = LANG_IGNORE;
#if defined(MSDOS) || defined(WIN32) || defined(OS2) || defined(__vax)
    const boolean caseInsensitive = TRUE;
#else
    const boolean caseInsensitive = FALSE;
#endif

    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
    {
	if (stringListHas(&Option.langMap[i], extension, caseInsensitive))
	{
	    language = (langType)i;
	    break;
	}
    }
    return language;
}

extern langType getFileLanguage( fileName )
    const char *const fileName;
{
    const char *const extension = findExtension(fileName);
    langType language;

    if (Option.language != LANG_AUTO)
	language = Option.language;
    else if (extension[0] == '\0')
	language = LANG_IGNORE;		/* ignore files with no extension */
    else
	language = getExtensionLanguage(extension);

    return language;
}

/*----------------------------------------------------------------------------
 *  Option processing
 *--------------------------------------------------------------------------*/

static void printfFeatureList( where )
    FILE *const where;
{
    int i;

    for (i = 0 ; Features[i] != NULL ; ++i)
    {
	if (i == 0)
	    fputs(" (", where);
	fprintf(where, "%s+%s", (i>0 ? ", " : ""), Features[i]);
    }
    fputs(i>0 ? ")" : "", where);
}

static void printProgramIdentification( where )
    FILE *const where;
{
    fprintf(where, "%s %s, by %s <%s>",
	    PROGRAM_NAME, PROGRAM_VERSION, AUTHOR_NAME, AUTHOR_EMAIL);
    printfFeatureList(where);
    fputs("\n", where);
}

static void printInvocationDescription( where )
    FILE *const where;
{
    fprintf(where, INVOCATION, getExecutableName());
}

static void printOptionDescriptions( optDesc, where )
    const optionDescription *const optDesc;
    FILE *const where;
{
    int i;

    for (i = 0 ; optDesc[i].description != NULL ; ++i)
    {
	if (! StartedAsEtags || optDesc[i].usedByEtags)
	{
	    fputs(optDesc[i].description, where);
	    fputc('\n', where);
	}
    }
}

static void printHelp( optDesc )
    const optionDescription *const optDesc;
{

    printProgramIdentification(stdout);
    putchar('\n');
    printInvocationDescription(stdout);
    putchar('\n');
    printOptionDescriptions(optDesc, stdout);
}

static void saveExtensionList( slist, elist, clear )
    stringList *const slist;
    const char *const elist;
    const boolean clear;
{
    char *const extensionList = (char *)eMalloc(strlen(elist) + 1);
    const char *extension;

    if (clear)
	stringListClear(slist);
    DebugStatement( else if (debug(DEBUG_OPTION))
		    {
			printf("current: ");
			stringListPrint(slist);
			putchar ('\n');
		    } )
    strcpy(extensionList, elist);
    extension = strtok(extensionList, EXTENSION_SEPARATORS);
    while (extension != NULL)
    {
	DebugStatement( if (debug(DEBUG_OPTION))
			    printf("  adding : %s\n", extension); )
	stringListAdd(slist, vStringNewInit(extension));
	extension = strtok(NULL, EXTENSION_SEPARATORS);
    }
    free(extensionList);
}

static void processCTypesOption( optionName, parameter, longOption )
    const char *const optionName;
    const char *const parameter;
    const boolean longOption;
{
    struct sCInclude *const inc = &Option.include.c;
    boolean defaultClear = longOption;
    const char *p = parameter;
    boolean mode = TRUE;
    int c;

    if (*p == '=')
    {
	defaultClear = TRUE;
	++p;
    }
    if (defaultClear  &&  strchr("+-", *p) == NULL)
    {
	inc->classNames		= FALSE;
	inc->defines		= FALSE;
	inc->enumerators	= FALSE;
	inc->functions		= FALSE;
	inc->enumNames		= FALSE;
	inc->members		= FALSE;
	inc->namespaceNames	= FALSE;
	inc->prototypes		= FALSE;
	inc->structNames	= FALSE;
	inc->typedefs		= FALSE;
	inc->unionNames		= FALSE;
	inc->variables		= FALSE;
	inc->access		= FALSE;
	inc->classPrefix	= FALSE;

	if (! longOption)
	{
	    Option.include.fileNames	= FALSE;
	    Option.include.fileScope	= FALSE;
	}
    }
    while ((c = *p++) != '\0') switch (c)
    {
	case '+': mode = TRUE;			break;
	case '-': mode = FALSE;			break;

	case 'c': inc->classNames	= mode;	break;
	case 'd': inc->defines		= mode;	break;
	case 'e': inc->enumerators	= mode;	break;
	case 'f': inc->functions	= mode;	break;
	case 'g': inc->enumNames	= mode;	break;
	case 'm': inc->members		= mode;	break;
	case 'n': inc->namespaceNames	= mode;	break;
	case 'p': inc->prototypes	= mode;	break;
	case 's': inc->structNames	= mode;	break;
	case 't': inc->typedefs		= mode;	break;
	case 'u': inc->unionNames	= mode;	break;
	case 'v': inc->variables	= mode;	break;
	case 'x': inc->externVars	= mode;	break;
	case 'A': inc->access		= mode;	break;
	case 'C': inc->classPrefix	= mode;	break;

	case 'F':
	    if (longOption)
		error(FATAL, "Unsupported parameter '%c' for \"%s\" option",
		      c, optionName);
	    else
		Option.include.fileNames = mode;
	    break;

	case 'S':
	    if (longOption)
		error(FATAL, "Unsupported parameter '%c' for \"%s\" option",
		      c, optionName);
	    else
		Option.include.fileScope = mode;
	    break;

	default:
	    error(FATAL, "Unsupported parameter '%c' for \"%s\" option",
		  c, optionName);
	    break;
    }
}

static void processEiffelTypesOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    struct sEiffelInclude *const inc = &Option.include.eiffel;
    const char *p = parameter;
    boolean mode = TRUE;
    int c;

    if (strchr("+-", *p) == NULL)
    {
	inc->classNames		= FALSE;
	inc->features		= FALSE;
	inc->localEntities	= FALSE;
    }
    while ((c = *p++) != '\0') switch (c)
    {
	case '+': mode = TRUE;			break;
	case '-': mode = FALSE;			break;

	case 'c': inc->classNames	= mode;	break;
	case 'f': inc->features		= mode;	break;
	case 'l': inc->localEntities	= mode;	break;

	default:
	    error(FATAL, "Unsupported parameter '%c' for \"%s\" option",
		    c, optionName);
	    break;
    }
}

static void processFortranTypesOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    struct sFortranInclude *const inc = &Option.include.fortran;
    const char *p = parameter;
    boolean mode = TRUE;
    int c;

    if (strchr("+-", *p) == NULL)
    {
	inc->blockData		= FALSE;
	inc->commonBlocks	= FALSE;
	inc->entryPoints	= FALSE;
	inc->functions		= FALSE;
	inc->interfaces		= FALSE;
	inc->labels		= FALSE;
	inc->modules		= FALSE;
	inc->namelists		= FALSE;
	inc->programs		= FALSE;
	inc->subroutines	= FALSE;
	inc->types		= FALSE;
    }
    while ((c = *p++) != '\0') switch (c)
    {
	case '+': mode = TRUE;			break;
	case '-': mode = FALSE;			break;

	case 'b': inc->blockData	= mode;	break;
	case 'c': inc->commonBlocks	= mode;	break;
	case 'e': inc->entryPoints	= mode;	break;
	case 'f': inc->functions	= mode;	break;
	case 'i': inc->interfaces	= mode;	break;
	case 'l': inc->labels		= mode;	break;
	case 'm': inc->modules		= mode;	break;
	case 'n': inc->namelists	= mode;	break;
	case 'p': inc->programs		= mode;	break;
	case 's': inc->subroutines	= mode;	break;
	case 't': inc->types		= mode;	break;

	default:
	    error(FATAL, "Unsupported parameter '%c' for \"%s\" option",
		    c, optionName);
	    break;
    }
}

static void processJavaTypesOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    struct sJavaInclude *const inc = &Option.include.java;
    const char *p = parameter;
    boolean mode = TRUE;
    int c;

    if (strchr("+-", *p) == NULL)
    {
	inc->classNames		= FALSE;
	inc->fields		= FALSE;
	inc->interfaceNames	= FALSE;
	inc->methods		= FALSE;
	inc->packageNames	= FALSE;
	inc->access		= FALSE;
	inc->classPrefix	= FALSE;
    }
    while ((c = *p++) != '\0') switch (c)
    {
	case '+': mode = TRUE;			break;
	case '-': mode = FALSE;			break;

	case 'c': inc->classNames	= mode;	break;
	case 'f': inc->fields		= mode;	break;
	case 'i': inc->interfaceNames	= mode;	break;
	case 'm': inc->methods		= mode;	break;
	case 'p': inc->packageNames	= mode;	break;
	case 'A': inc->access		= mode;	break;
	case 'C': inc->classNames	= mode;	break;

	default:
	    error(FATAL, "Unsupported parameter '%c' for \"%s\" option",
		  c, optionName);
	    break;
    }
}

/*  Determines whether or not "name" should be ignored, per the ignore list.
 */
extern boolean isIgnoreToken( name, pIgnoreParens, replacement )
    const char *const name;
    boolean *const pIgnoreParens;
    const char **const replacement;
{
    const size_t nameLen = strlen(name);
    boolean ignore = FALSE;
    unsigned int i;

    if (pIgnoreParens != NULL)
	*pIgnoreParens = FALSE;

    for (i = 0  ;  i < Option.ignore.count  ;  ++i)
    {
	vString *token = Option.ignore.list[i];

	if (strncmp(vStringValue(token), name, nameLen) == 0)
	{
	    const size_t tokenLen = vStringLength(token);

	    if (nameLen == tokenLen)
	    {
		ignore = TRUE;
		break;
	    }
	    else if (tokenLen == nameLen + 1  &&
		     vStringChar(token, tokenLen - 1) == '+')
	    {
		ignore = TRUE;
		if (pIgnoreParens != NULL)
		    *pIgnoreParens = TRUE;
		break;
	    }
	    else if (vStringChar(token, nameLen) == '=')
	    {
		if (replacement != NULL)
		    *replacement = vStringValue(token) + nameLen + 1;
		break;
	    }
	}
    }
    return ignore;
}

static void saveIgnoreToken( ignoreToken )
    vString *const ignoreToken;
{
    stringListAdd(&Option.ignore, ignoreToken);
    DebugStatement( if (debug(DEBUG_OPTION))
			printf("ignore token: %s\n",
				vStringValue(ignoreToken)); )
}

static void readIgnoreList( list )
    char *const list;
{
    const char *token = strtok(list, IGNORE_SEPARATORS);

    while (token != NULL)
    {
	vString *const entry = vStringNewInit(token);

	saveIgnoreToken(entry);
	token = strtok(NULL, IGNORE_SEPARATORS);
    }
}

static void readIgnoreListFromFile( fileName )
    const char *const fileName;
{
    FILE *const fp = fopen(fileName, "r");

    if (fp == NULL)
	error(FATAL | PERROR, "cannot open \"%s\"", fileName);
    else
    {
	vString *entry = NULL;
	int c;

	do
	{
	    c = fgetc(fp);

	    if (isspace(c)  ||  c == EOF)
	    {
		if (entry != NULL  &&  vStringLength(entry) > 0)
		{
		    vStringTerminate(entry);
		    saveIgnoreToken(entry);
		    entry = NULL;
		}
	    }
	    else
	    {
		if (entry == NULL)
		    entry = vStringNew();
		vStringPut(entry, c);
	    }
	} while (c != EOF);
	Assert(entry == NULL);
    }
}

static void processHeaderListOption( option, argP, argList, argNumP )
    const int option;
    char **const argP;
    char *const *const argList;
    int *const argNumP;
{
    char *list = readOptionArg(option, argP, argList, argNumP);

    /*  Check to make sure that the user did not enter "ctags -h *.c"
     *  by testing to see if the list is a filename that exists.
     */
    if (doesFileExist(list))
	error(FATAL, "-h: Invalid list");
    else
    {
	boolean clear = TRUE;

	if (list[0] == '+')
	{
	    ++list;
	    clear = FALSE;
	}
	DebugStatement( if (debug(DEBUG_OPTION))
			    printf("Header Extensions:\n"); )
	saveExtensionList(&Option.headerExt, list, clear);
    }
}

static void processIgnoreOption( option, argP, argList, argNumP )
    const int option;
    char **const argP;
    char *const *const argList;
    int *const argNumP;
{
    char *const list = readOptionArg(option, argP, argList, argNumP);

    if (strchr("./\\", list[0]) != NULL)
	readIgnoreListFromFile(list);
    else
	readIgnoreList(list);
}

static void processExcmdOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    switch (*parameter)
    {
	case 'm':	Option.locate = EX_MIX;		break;
	case 'n':	Option.locate = EX_LINENUM;	break;
	case 'p':	Option.locate = EX_PATTERN;	break;
	default:
	    error(FATAL, "Invalid value for \"%s\" option", optionName);
	    break;
    }
}

static void processFormatOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    unsigned int format;

    if (sscanf(parameter, "%u", &format) < 1)
	error(FATAL, "Invalid value for \"%s\" option",optionName);
    else if (format <= (unsigned int)MaxSupportedTagFormat)
	Option.tagFileFormat = format;
    else
	error(FATAL, "Unsupported value for \"%s\" option", optionName);
}

extern const char *getLanguageName( language )
    const langType language;
{
    static const char *const names[] = {
	"c", "c++", "eiffel", "fortran", "java"
    };

    DebugStatement( if (sizeof(names)/sizeof(names[0]) != LANG_COUNT)
	error(FATAL, "LangNames array not consistent with LANG enumeration"); )

    return names[(int)language];
}

static langType getLangType( name )
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

static void processLangOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    const langType language = getLangType(parameter);

    if (language == LANG_IGNORE)
	error(FATAL, "Invalid value for \"%s\" option", optionName);
    else
	Option.language = language;
}

static boolean installLangMap( map )
    char *const map;
{
    char *const separator = strchr(map, ':');
    boolean ok = TRUE;

    if (separator != NULL)
    {
	langType language;
	const char *list = separator + 1;
	boolean clear = TRUE;

	*separator = '\0';
	if (list[0] == '+')
	{
	    clear = FALSE;
	    ++list;
	}
	language = getLangType(map);
	if (language == LANG_IGNORE)
	    ok = FALSE;
	else
	{
	    DebugStatement( if (debug(DEBUG_OPTION)) printf("%s map:\n", map); )
	    saveExtensionList(&Option.langMap[(int)language], list, clear);
	}
    }
    return ok;
}

static void processLangMapOption( optionName, parameter )
    const char *const __unused__ optionName;
    const char *const parameter;
{
    char *const maps = (char *)eMalloc(strlen(parameter) + 1);
    char *map = maps;

    strcpy(maps, parameter);

    DebugStatement( if (debug(DEBUG_OPTION))
			printf("Language-extension maps:\n"); )
    while (map != NULL)
    {
	char *end = strchr(map, ',');

	if (end != NULL)
	    *end = '\0';
	if (! installLangMap(map))
	    error(FATAL, "Invalid language specified for \"%s\" option",
		  optionName);
	if (end != NULL)
	    map = end + 1;
	else
	    map = NULL;
    }
    free(maps);
}

static void processEtagsInclude( optionName, parameter )
    const char *const __unused__ optionName;
    const char *const parameter;
{
    vString *const file = vStringNewInit(parameter);
    stringListAdd(&Option.etagsInclude, file);
}

static boolean getBooleanOption( optionName, parameter, defaultValue )
    const char *const optionName;
    const char *const parameter;
    const boolean defaultValue;
{
    boolean selection = defaultValue;

    if (parameter[0] == '\0')
	selection = defaultValue;
    else if (strcmp(parameter, "0"  ) == 0  ||
	     strcmp(parameter, "no" ) == 0  ||
	     strcmp(parameter, "off") == 0)
	selection = FALSE;
    else if (strcmp(parameter, "1"  ) == 0  ||
	     strcmp(parameter, "yes") == 0  ||
	     strcmp(parameter, "on" ) == 0)
	selection = TRUE;
    else
	error(FATAL, "Invalid value for \"%s\" option", optionName);

    return selection;
}

static boolean validateOption( ref, optionName, parameter, parameterRequired )
    const char *const ref;
    const char *const optionName;
    const char *const parameter;
    const boolean parameterRequired;
{
    const boolean match = (boolean)(strcmp(ref, optionName + 2) == 0);

    if (match  &&  parameterRequired  &&  parameter[0] == '\0')
	error(FATAL, "Missing parameter for \"%s\" option", optionName);

    return match;
}

static void processLongOption( optionString )
    const char *const optionString;
{
    enum { MaxOptionName = 20 };
    char optionName[MaxOptionName + 1];
    const char *const equal = strchr(optionString, '=');
    const char *parameter = (equal == NULL) ? "" : equal + 1;
    const size_t optionLength = (equal == NULL) ? strlen(optionString) :
	    					  (equal - optionString);

    DebugStatement( if (debug(DEBUG_OPTION))
			printf("Option: %s\n", optionString); )

    strncpy(optionName, optionString, optionLength);
    if (optionLength < (size_t)MaxOptionName)
	optionName[optionLength] = '\0';
    else
	optionName[(size_t)MaxOptionName] = '\0';

#define isOption(option,req)	validateOption(option,optionName,parameter,req)
    if (isOption("append", FALSE))
	Option.append = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("c-types", TRUE) || isOption("c++-types", TRUE))
	processCTypesOption(optionName, parameter, TRUE);
    else if (isOption("eiffel-types", TRUE))
	processEiffelTypesOption(optionName, parameter);
    else if (isOption("etags-include", TRUE))
	processEtagsInclude(optionName, parameter);
    else if (isOption("excmd", TRUE))
	processExcmdOption(optionName, parameter);
    else if (isOption("file-scope", FALSE))
	Option.include.fileScope = getBooleanOption(optionName, parameter,TRUE);
    else if (isOption("file-tags", FALSE))
	Option.include.fileNames = getBooleanOption(optionName, parameter,TRUE);
    else if (isOption("format", TRUE))
	processFormatOption(optionName, parameter);
    else if (isOption("fortran-types", TRUE))
	processFortranTypesOption(optionName, parameter);
    else if (isOption("help", FALSE))
	{ printHelp(LongOptionDescription); exit(0); }
    else if (isOption("if0", FALSE))
	Option.if0 = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("kind-long", FALSE))
	Option.kindLong = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("java-types", TRUE))
	processJavaTypesOption(optionName, parameter);
    else if (isOption("lang", TRUE)  ||  isOption("language", TRUE))
	processLangOption(optionName, parameter);
    else if (isOption("langmap", TRUE))
	processLangMapOption(optionName, parameter);
    else if (isOption("line-directives", FALSE))
	Option.lineDirectives = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("links", FALSE))
	Option.followLinks = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("recurse", FALSE))
#ifdef RECURSE_SUPPORTED
	Option.recurse = getBooleanOption(optionName, parameter, TRUE);
#else
	error(FATAL, "%s option not supported on this host", optionName);
#endif
    else if (isOption("sort", FALSE))
	Option.sorted = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("totals", FALSE))
	Option.printTotals = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("version", FALSE))
    {
	printProgramIdentification(stdout);
	exit(0);
    }
    else
	error(FATAL, "Unknown option: %s", optionName);
#undef isOption
}

/*----------------------------------------------------------------------------
*-	Compound option handling
----------------------------------------------------------------------------*/

static char *readOptionArg( option, pArg, argList, pArgNum )
    const int option;
    char **const pArg;
    char *const *const argList;
    int *const pArgNum;
{
    char *list;

    if ((*pArg)[0] != '\0')	    /* does list immediately follow option? */
    {
	list = *pArg;
	*pArg += strlen(*pArg);
	DebugStatement( if (debug(DEBUG_OPTION)) printf("%s\n", list); )

    }
    else if ((list = argList[++(*pArgNum)]) != NULL) /* at least 1 more arg? */
    {
	DebugStatement( if (debug(DEBUG_OPTION)) printf("%s\n", list) );
    }
    else
	error(FATAL, "-%c: Parameter missing", option);

    return list;
}

static void processCompoundOption( option, pArg, argList, pArgNum )
    const int option;
    char **const pArg;
    char *const *const argList;
    int *const pArgNum;
{
    char *param;

    DebugStatement( if (debug(DEBUG_OPTION)) printf("Option: -%c ", option); )
    switch (option)
    {
    /*	Options requiring parameters.
     */
    case 'f':
    case 'o':	Option.tagFileName=readOptionArg(option, pArg, argList,pArgNum);
		break;
    case 'h':	processHeaderListOption(option, pArg, argList, pArgNum);
		break;
    case 'i':	param = readOptionArg(option, pArg, argList, pArgNum);
		processCTypesOption("-i", param, FALSE);
		break;
    case 'I':	processIgnoreOption(option, pArg, argList, pArgNum);
		break;
    case 'L':	Option.fileList = readOptionArg(option, pArg, argList, pArgNum);
		break;
    case 'p':	Option.path = readOptionArg(option, pArg, argList, pArgNum);
		break;
#ifdef DEBUG
    case 'D':	param = readOptionArg(option, pArg, argList, pArgNum);
		Option.debugLevel = atoi(param);
		if (debug(DEBUG_STATUS))
		    Option.verbose = TRUE;
		break;
    case 'b':	param = readOptionArg(option, pArg, argList, pArgNum);
		if (atol(param) < 0)
		    error(FATAL, "-%c: Invalid line number", option);
		Option.breakLine = atol(param);
		break;
#endif
    default: error(FATAL, "Unknown option: -%c", option); break;
    }
}

/*----------------------------------------------------------------------------
*-	Simple option handling
----------------------------------------------------------------------------*/

static boolean processSimpleOption( option )
    const int option;
{
    boolean handled = TRUE;

    switch (option)
    {
	case 'a':	Option.append		= TRUE;		break;
	case 'B':	Option.backward		= TRUE;		break;
	case 'e':	Option.etags		= TRUE;
			Option.sorted		= FALSE;	break;
	case 'F':	Option.backward		= FALSE;	break;
	case 'n':	Option.locate		= EX_LINENUM;	break;
	case 'N':	Option.locate		= EX_PATTERN;	break;
	case 'R':
#ifdef RECURSE_SUPPORTED
			Option.recurse		= TRUE;		break;
#else
			error(FATAL, "-%c option not supported on this host",
			      option);
#endif
	case 'u':	Option.sorted		= FALSE;	break;
	case 'V':	Option.verbose		= TRUE;		break;
	case 'w':
	case 'W':	break;
	case 'x':	Option.xref		= TRUE;		break;

	case '?':	printHelp(LongOptionDescription);
			exit(0);

	default:	handled = FALSE;			break;
    }

    DebugStatement( if (handled && debug(DEBUG_OPTION))
			printf("Option: -%c\n", option); )

    return handled;
}

extern char *const *parseOptions( argList )
    char *const *const argList;
{
    int	argNum;

    for (argNum = 0  ;  argList[argNum] != NULL  ;  ++argNum)
    {
	char *arg = argList[argNum];
	int c;

	if (*arg != '-')		/* stop at first non-option switch */
	    break;
	else if (*(arg + 1) == '-')		/* double dash: "--" */
	    processLongOption(arg);
	else
	{
	    ++arg;
	    while ((c = *arg++) != '\0')
		if (! processSimpleOption(c))
		    processCompoundOption(c, &arg, argList, &argNum);
	}
    }
    return &argList[argNum];
}

/*----------------------------------------------------------------------------
*-	Conversion of string into arg list
----------------------------------------------------------------------------*/

static void parseStringToArgs( string, parsedBuffer, argList, maxArgs )
    const char *const string;
    char *parsedBuffer;
    char **const argList;
    const unsigned int maxArgs;
{
    boolean argInProgress = FALSE;
    unsigned int count = 0;
    const char *src;

    for (src = string  ;  *src != '\0'  ;  ++src)
    {
	if (*src == ' ')			/* designates end of argument */
	{
	    if (argInProgress)
	    {
		*parsedBuffer++ = '\0';		/* terminate arg in progress */
		argInProgress = FALSE;
		if (count >= maxArgs)
		    break;
	    }
	}
	else
	{
	    if (! argInProgress)
	    {
		argInProgress = TRUE;
		argList[count++] = parsedBuffer;	/* point to new arg */
	    }
	    if (*src == '\\')			/* next character is literal */
		++src;				/* skip over '\\' */
	    *parsedBuffer++ = *src;
	}
    }
    *parsedBuffer = '\0';		/* null terminate last argument */
    argList[count] = NULL;		/* terminate list */
}

static unsigned int countStringWords( string )
    const char *const string;
{
    unsigned int numWords = 0;

    if (string != NULL)
    {
	const char *const whiteSpace = " \t\n";
	const char *p = string;

	p += strspn(p, whiteSpace);		/* skip over leading spaces */
	while (*p != '\0')
	{
	    ++numWords;
	    p += strcspn(p, whiteSpace);	/* skip to white space */
	    p += strspn(p, whiteSpace);		/* skip to non-white space */
	}
    }
    return numWords;
}

static char **creatArgListForString( string )
    const char *const string;
{
    const unsigned int numWords = countStringWords(string);
    char **argList = NULL;

    if (numWords > 0)
    {
	/*  We place the parsed string at the end of the memory block, past
	 *  the bottom of the argument table.
	 */
	const size_t argListSize= (numWords + 1) * sizeof(char *);
	const size_t blockSize	= argListSize + strlen(string) + 1;

	argList	= (char **)eMalloc(blockSize);
	parseStringToArgs(string, (char *)argList + argListSize,
			  argList, numWords);
    }
    return argList;
}

extern void *parseEnvironmentOptions()
{
    const char *envOptions = NULL;
    char **argList = NULL;

    if (StartedAsEtags)
	envOptions = getenv(ETAGS_ENVIRONMENT);
    if (envOptions == NULL)
	envOptions = getenv(CTAGS_ENVIRONMENT);
    if (envOptions != NULL  &&  envOptions[0] != '\0')
    {
	argList = creatArgListForString(envOptions);
	if (argList != NULL)
	    parseOptions(argList);
    }
    return argList;
}

/*----------------------------------------------------------------------------
*-	Option initialization
----------------------------------------------------------------------------*/

static void installExtensionList( slist, list )
    stringList *const slist;
    const char *const *const list;
{
    const char *const *p;

    for (p = list  ;  *p != NULL  ;  ++p)
	stringListAdd(slist, vStringNewInit(*p));
}

static void installLangMapDefaults()
{
    int i;

    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
    {
	const langMapping *const map = &langMapDefaults[i];

	installExtensionList(&Option.langMap[(int)map->language],
			     map->defaultMap);
    }
}

static void freeLangMap()
{
    int i;

    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
    {
	const langMapping *const map = &langMapDefaults[i];

	stringListDelete(&Option.langMap[(int)map->language]);
    }
}

extern void initOptions()
{
    installExtensionList(&Option.headerExt, HeaderExtensions);
    installLangMapDefaults();
}

extern void freeOptionResources()
{
    stringListDelete(&Option.ignore);
    stringListDelete(&Option.headerExt);
    stringListDelete(&Option.etagsInclude);
    freeLangMap();
}

/* vi:set tabstop=8 shiftwidth=4: */
