/*****************************************************************************
*   $Id: options.c,v 6.11 1998/08/04 03:25:29 darren Exp $
*
*   Copyright (c) 1996-1998, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions to process command line options.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef DEBUG
# include <assert.h>
#endif

#include "ctags.h"

/*============================================================================
=   Defines
============================================================================*/

/*----------------------------------------------------------------------------
 *  Miscellaneous defines
 *--------------------------------------------------------------------------*/

#define CTAGS_INVOCATION  "\
  Usage: ctags [-aBeFnNRux] [-{f|o} name] [-h list] [-i [+-=]types]\n\
               [-I list] [-L file] [-p path] [--append] [--excmd=n|p|m]\n\
               [--format=level] [--help] [--if0] [--lang=lang] [--recurse]\n\
	       [--sort] [--totals] [--version] file(s)\n"

#define ETAGS_INVOCATION  "\
  Usage: etags [-aRx] [-{f|o} name] [-h list] [-i [+-=]types] [-I list]\n\
               [-L file] [-p path] [--append] [--help] [--if0] [--lang=lang]\n\
	       [--recurse] [--totals] [--version] file(s)\n"

#define CTAGS_ENVIRONMENT	"CTAGS"
#define ETAGS_ENVIRONMENT	"ETAGS"

/*  The following separators are permitted for list options.
 */
#define EXTENSION_SEPARATORS   "."
#define IGNORE_SEPARATORS   ", \t\n"

#ifndef DEFAULT_FILE_FORMAT
# define DEFAULT_FILE_FORMAT	2
#endif

/*============================================================================
=   Data declarations
============================================================================*/
typedef struct {
    int usedByEtags;
    const char *const description;
} optionDescription;

/*============================================================================
=   Data definitions
============================================================================*/

/*----------------------------------------------------------------------------
-   Globally shared
----------------------------------------------------------------------------*/

static const char *const CExtensionList[] = {
    "c", NULL
};
static const char *const CppExtensionList[] = {
#if !defined(MSDOS) && !defined(WIN32) && !defined(OS2)
    "C",
#endif
    "c++", "cc", "cpp", "cxx", NULL
};
static const char *const JavaExtensionList[] = {
    "java", NULL
};
static const char *const HeaderExtensionList[] = {
    "h", "H", "hh", "hpp", "hxx", "h++", NULL
};

optionValues Option = {
    {
	TRUE,		/* -ic */
	TRUE,		/* -id */
	TRUE,		/* -ie */
	TRUE,		/* -if */
	TRUE,		/* -ig */
	TRUE,		/* -ii */
	FALSE,		/* -im */
	TRUE,		/* -in */
	FALSE,		/* -ip */
	TRUE,		/* -is */
	TRUE,		/* -it */
	TRUE,		/* -iu */
	TRUE,		/* -iv */
	FALSE,		/* -ix */
	FALSE,		/* -iC */
	FALSE,		/* -iF */
	TRUE,		/* -iS */
    },
    { NULL, 0, 0 },	/* -I */
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
    FALSE,		/* -x */
    NULL,		/* -L */
    NULL,		/* -o */
    HeaderExtensionList,/* -h */
#ifdef DEBUG
    0, 0,		/* -D, -b */
#endif
    FALSE,		/* started as etags */
    FALSE,		/* brace formatting */
    DEFAULT_FILE_FORMAT,/* --format */
    FALSE,		/* --if0 */
    LANG_AUTO,		/* --lang */
    {			/* --langmap */
	CExtensionList,
	CppExtensionList,
	JavaExtensionList,
    },
    FALSE		/* --totals */
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
 {1,"       Specifies the list of tag types to include in the output file."},
 {1,"       \"Types\" is a group of letters designating the types of tags"},
 {1,"       affected. Each letter or group of letters may be preceded by"},
 {1,"       either a '+' sign (default, if omitted) to add it to those already"},
 {1,"       included, a '-' sign to exclude it from the list (e.g. to exclude"},
 {1,"       a default tag type), or an '=' sign to include its corresponding"},
 {1,"       tag type at the exclusion of those not listed. A space separating"},
 {1,"       the option letter from the list is optional. The following tag"},
 {1,"       types are supported (default settings are on except as noted):"},
 {1,"          c   class names"},
 {1,"          d   macro definitions"},
 {1,"          e   enumerators (values inside an enumeration)"},
 {1,"          f   function (or method) definitions"},
 {1,"          g   enumeration names"},
 {1,"          i   interface names (Java only)"},
 {1,"          m   data members [off]"},
 {1,"          n   namespace names (C++ only)"},
 {1,"          p   function prototypes [off]"},
 {1,"          s   structure names"},
 {1,"          t   typedefs"},
 {1,"          u   union names"},
 {1,"          v   variable definitions"},
 {1,"          x   external variable declarations [off]"},
 {1,"       In addition, the following modifiers are accepted:"},
 {1,"          C   include extra, class-qualified member tag entries [off]"},
 {1,"          F   include source filenames as tags [off]"},
 {1,"          S   include static tags"},
 {1,"  -I <list | file>"},
 {1,"       A list of tokens to ignore is read from either the command line,"},
 {1,"       or the specified file (if leading character is '.', '/', or '\\')."},
 {1,"       Particularly useful when a function definition or declaration"},
 {1,"       contains some special macro before the parameter list."},
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
 {1,"  -x   Print a tabular cross reference file to standard output."},
 {1,"  --append=[yes|no]"},
 {1,"       Indicates whether tags should be appended to existing tag file"},
 {1,"       (default=no)."},
 {0,"  --excmd=number|pattern|mix"},
#ifdef MACROS_USE_PATTERNS
 {0,"       Uses the specified type of EX command to locate tags (default=pattern)."},
#else
 {0,"       Uses the specified type of EX command to locate tags (default=mix)."},
#endif
 {0,"  --format=level"},
#if DEFAULT_FILE_FORMAT==1
 {0,"       Forces output of specified tag file format (default=1)."},
#else
 {0,"       Forces output of specified tag file format (default=2)."},
#endif
 {1,"  --help"},
 {1,"       Prints this option summary."},
 {1,"  --if0=[yes|no]"},
 {1,"       Indicates whether code within #if 0 conditional branches should"},
 {1,"       be examined for tags (default=no)."},
 {1,"  --lang=[c|c++|java]"},
 {1,"       Forces specified language, disabling automatic selection."},
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
#ifndef EXTERNAL_SORT
    "internal_sort",
#endif
    NULL
};

/*============================================================================
=   Function prototypes
============================================================================*/
static void printfFeatureList __ARGS((FILE *const where));
static void printProgramIdentification __ARGS((FILE *const where));
static void printInvocationDescription __ARGS((FILE *const where));
static void printOptionDescriptions __ARGS((const optionDescription *const optDesc, FILE *const where));
static void printHelp __ARGS((const optionDescription *const optDesc, FILE *const where));
static char *readOptionArg __ARGS((const int option, char **const pArg, char *const *const argList, int *const pArgNum));
static unsigned int countExtensions __ARGS((const char *const list));
static const char *const *readExtensionList __ARGS((const char *const list));
static void clearTagList __ARGS((void));
static void applyTagInclusionList __ARGS((const char *const list));
static char *saveString __ARGS((const char *const string));
static void resizeIgnoreList __ARGS((void));
static void saveIgnoreToken __ARGS((const char *const ignoreToken));
static void readIgnoreList __ARGS((char *const list));
static void readIgnoreListFromFile __ARGS((const char *const fileName));
static void processHeaderListOption __ARGS((const int option, char **const argP, char *const *const argList, int *const argNumP));
static void processIgnoreOption __ARGS((const int option, char **const argP, char *const *const argList, int *const argNumP));
static boolean getBooleanOption __ARGS((const char *const optionName, const char *const parameter, const boolean defaultValue));
static void processExcmdOption __ARGS((const char *const optionName, const char *const parameter));
static void processFormatOption __ARGS((const char *const optionName, const char *const parameter));
static langType getLangType __ARGS((const char *const name));
static void processLangOption __ARGS((const char *const optionName, const char *const parameter));
static void installLangMap __ARGS((char *const map));
static void processLangMapOption __ARGS((const char *const optionName, const char *const parameter));
static void processLongOption __ARGS((const char *const optionString));
static void processCompoundOption __ARGS((const int option, char **const pArg, char *const *const argList, int *const pArgNum));
static boolean processSimpleOption __ARGS((const int option));
static void parseStringToArgs __ARGS((const char *const string, char *parsedBuffer, char **const argList, const unsigned int maxArgs));
static unsigned int countStringWords __ARGS((const char *const string));
static char **creatArgListForString __ARGS((const char *const string));

/*============================================================================
=   Function definitions
============================================================================*/

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
    fprintf(where, "%s %s, by %s", PROGRAM_NAME, PROGRAM_VERSION, AUTHOR_NAME);
    printfFeatureList(where);
    fputs("\n", where);
}

static void printInvocationDescription( where )
    FILE *const where;
{
    if (Option.startedAsEtags)
	fprintf(where, ETAGS_INVOCATION);
    else
	fprintf(where, CTAGS_INVOCATION);
}

static void printOptionDescriptions( optDesc, where )
    const optionDescription *const optDesc;
    FILE *const where;
{
    int i;

    for (i = 0 ; optDesc[i].description != NULL ; ++i)
    {
	if (! Option.startedAsEtags || optDesc[i].usedByEtags)
	{
	    fputs(optDesc[i].description, where);
	    fputc('\n', where);
	}
    }
}

static void printHelp( optDesc, where )
    const optionDescription *const optDesc;
    FILE *const where;
{

    printProgramIdentification(where);
    fputs("\n", where);
    printInvocationDescription(where);
    fputs("\n", where);
    printOptionDescriptions(optDesc, where);
}

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
    }
    else if ((list = argList[++(*pArgNum)]) == NULL) /* at least 1 more arg? */
	error(FATAL, "-%c: Parameter missing", option);

    DebugStatement( if (debug(DEBUG_OPTION)) fputs(list, errout); )

    return list;
}

/*  Reads a list of file extensions.
 */
static unsigned int countExtensions( list )
    const char *const list;
{
    unsigned int count = 0;
    const char *p;

    /*  Increase count by one if list does not begin with a separator.
     */
    if (strchr(EXTENSION_SEPARATORS, list[0]) == NULL)
	++count;

    for (p = list  ;  *p != '\0'  ;  ++p)
    {
	if (strchr(EXTENSION_SEPARATORS, *p) != NULL)
	    ++count;
    }
    return count + 1;
}

static const char *const *readExtensionList( list )
    const char *const list;
{
    int extIndex = 0;
    const char *extension;
    const unsigned int numExtensions = countExtensions(list);
    char *const extensionList  = (char *)malloc(strlen(list) + 1);
    const char **const extensionArray = (const char **)malloc(
					(numExtensions + 1) * sizeof(char *));

    if (extensionList == NULL  ||  extensionArray == NULL)
	error(FATAL | PERROR, "");
    strcpy(extensionList, list);
    extension = strtok(extensionList, EXTENSION_SEPARATORS);
    while (extension != NULL)
    {
	DebugStatement( if (debug(DEBUG_STATUS))
			    printf("extension: %s\n", extension); )
	extensionArray[extIndex++] = extension;
	extension = strtok(NULL, EXTENSION_SEPARATORS);
    }
    extensionArray[extIndex] = NULL;

    return extensionArray;
}

static void clearTagList()
{
    Option.include.classNames		= FALSE;	/* -ic */
    Option.include.defines		= FALSE;	/* -id */
    Option.include.enumerators		= FALSE;	/* -ie */
    Option.include.functions		= FALSE;	/* -if */
    Option.include.enumNames		= FALSE;	/* -ig */
    Option.include.interfaceNames	= FALSE;	/* -ii */
    Option.include.members		= FALSE;	/* -im */
    Option.include.namespaceNames	= FALSE;	/* -in */
    Option.include.prototypes		= FALSE;	/* -ip */
    Option.include.structNames		= FALSE;	/* -is */
    Option.include.typedefs		= FALSE;	/* -it */
    Option.include.unionNames		= FALSE;	/* -iu */
    Option.include.variables		= FALSE;	/* -iC */
    Option.include.sourceFiles		= FALSE;	/* -iF */
    Option.include.statics		= FALSE;	/* -iS */
}

static void applyTagInclusionList( list )
    const char *const list;
{
    boolean mode = TRUE;	/* default mode is to add following types */
    const char *p;

    for (p = list  ;  *p != '\0'  ;  ++p)
	switch (*p)
	{
	    case '=':	/* exclusive mode; ONLY types following are included */
		clearTagList();
		mode = TRUE;
		break;

	    case '+':	mode = TRUE;	break;	/* include types following */
	    case '-':	mode = FALSE;	break;	/* exclude types following */

	    case 'c':	Option.include.classNames	= mode;		break;
	    case 'd':	Option.include.defines		= mode;		break;
	    case 'e':	Option.include.enumerators	= mode;		break;
	    case 'f':	Option.include.functions	= mode;		break;
	    case 'g':	Option.include.enumNames	= mode;		break;
	    case 'i':	Option.include.interfaceNames	= mode;		break;
	    case 'm':	Option.include.members		= mode;		break;
	    case 'n':	Option.include.namespaceNames	= mode;		break;
	    case 'p':	Option.include.prototypes	= mode;		break;
	    case 's':	Option.include.structNames	= mode;		break;
	    case 't':	Option.include.typedefs		= mode;		break;
	    case 'u':	Option.include.unionNames	= mode;		break;
	    case 'v':	Option.include.variables	= mode;		break;
	    case 'x':	Option.include.externVars	= mode;		break;
	    case 'C':	Option.include.classPrefix	= mode;		break;
	    case 'F':	Option.include.sourceFiles	= mode;		break;
	    case 'S':	Option.include.statics		= mode;		break;

	    default: error(FATAL, "-i: Invalid tag option '%c'", *p);	break;
	}
}

/*  Determines whether or not "name" should be ignored, per the ignore list.
 */
extern boolean isIgnoreToken( name )
    const char *const name;
{
    boolean ignore = FALSE;
    unsigned int i;

    for (i = 0  ;  i < Option.ignore.count ; ++i)
    {
	if (strcmp(Option.ignore.list[i], name) == 0)
	{
	    ignore = TRUE;
	    break;
	}
    }
    return ignore;
}

static char *saveString( string )
    const char *const string;
{
    char *const here = (char *)malloc(strlen(string) + 1);

    if (here == NULL)
	error(FATAL | PERROR, "");
    strcpy(here, string);

    return here;
}

static void resizeIgnoreList()
{
    size_t newSize;

    Option.ignore.max = Option.ignore.count + 10;
    newSize = Option.ignore.max * sizeof(char *);

    if (Option.ignore.list == NULL)
	Option.ignore.list = (char **)malloc(newSize);
    else
	Option.ignore.list = (char **)realloc(Option.ignore.list, newSize);
    if (Option.ignore.list == NULL)
	error(FATAL | PERROR, "cannot create ignore list");
}

static void saveIgnoreToken( ignoreToken )
    const char *const ignoreToken;
{
    const unsigned int i = Option.ignore.count++;

    if (Option.ignore.count > Option.ignore.max)
	resizeIgnoreList();
    Option.ignore.list[i] = saveString(ignoreToken);
    DebugStatement( if (debug(DEBUG_STATUS))
			printf("ignore token: %s\n", ignoreToken); )
}

static void readIgnoreList( list )
    char *const list;
{
    const char *token = strtok(list, IGNORE_SEPARATORS);

    while (token != NULL)
    {
	saveIgnoreToken(token);
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
	char ignoreToken[MaxNameLength];

	while (fscanf(fp, "%255s", ignoreToken) == 1)
	    saveIgnoreToken(ignoreToken);
    }
}

extern void freeIgnoreList()
{
    while (Option.ignore.count > 0)
	free(Option.ignore.list[--Option.ignore.count]);

    if (Option.ignore.list != NULL)
	free(Option.ignore.list);

    Option.ignore.list = NULL;
    Option.ignore.max = 0;
}

static void processHeaderListOption( option, argP, argList, argNumP )
    const int option;
    char **const argP;
    char *const *const argList;
    int *const argNumP;
{
    char *const list = readOptionArg(option, argP, argList, argNumP);

    /*  Check to make sure that the user did not enter "ctags -h *.c"
     *  by testing to see if the list is a filename that exists.
     */
    if (doesFileExist(list) == 0)
	error(FATAL, "-h: Invalid list");
    else
    {
	DebugStatement( if (debug(DEBUG_STATUS))
			    printf("Header Extensions:\n"); )
	Option.headerExt = readExtensionList(list);
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

static boolean getBooleanOption( optionName, parameter, defaultValue )
    const char *const optionName;
    const char *const parameter;
    const boolean defaultValue;
{
    boolean selection = defaultValue;

    if (parameter[0] == '\0')
	selection = defaultValue;
    else if (strcmp(parameter, "0") == 0  ||  strcmp(parameter, "no") == 0)
	selection = FALSE;
    else if (strcmp(parameter, "1") == 0  ||  strcmp(parameter, "yes") == 0)
	selection = TRUE;
    else
	error(FATAL, "Invalid value for option --%s", optionName);

    return selection;
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
	    error(FATAL, "Invalid value for option --%s", optionName);
	    break;
    }
}

static void processFormatOption( optionName, parameter )
    const char *const optionName;
    const char *const parameter;
{
    unsigned int format;

    if (sscanf(parameter, "%u", &format) < 1)
	error(FATAL, "Missing or invalid value for \"--%s\" option",optionName);
    else if (format <= (unsigned int)MaxSupportedTagFormat)
	Option.tagFileFormat = format;
    else
	error(FATAL, "Unsupported value for \"--%s\" option", optionName);
}

extern const char *getLanguageName( language )
    const langType language;
{
    static const char *const names[] = { "c", "c++", "java" };

    DebugStatement( if (sizeof(names)/sizeof(names[0]) != LANG_COUNT)
	error(FATAL, "LangNames array not consistent with LANG enumeration"); )

    return names[(int)language];
}

extern boolean strequiv( s1, s2 )
    const char *s1;
    const char *s2;
{
    boolean equivalent;

    if (strcmp(s1, s2) == 0)
	equivalent = TRUE;
    else
    {
	equivalent = TRUE;
	do
	{
	    if (toupper(*s1) != toupper(*s2))
	    {
		equivalent = FALSE;
		break;
	    }
	} while (*s1++ != '\0'  &&  *s2++ != '\0');
    }
    return equivalent;
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
	error(FATAL, "Invalid value for option --%s", optionName);
    else
	Option.language = language;
}

static void installLangMap( map )
    char *const map;
{
    char *const separator = strchr(map, ':');

    if (separator != NULL)
    {
	langType language;

	*separator = '\0';
	language = getLangType(map);
	if (language == LANG_IGNORE)
	    error(FATAL, "Invalid language specified for option --langmap");
	DebugStatement( if (debug(DEBUG_STATUS))
			    printf("%s map:\n", map); )
	Option.langMap[(int)language] = readExtensionList(separator + 1);
    }
}

static void processLangMapOption( optionName, parameter )
    const char *const __unused__ optionName;
    const char *const parameter;
{
    char *const maps = (char *)malloc(strlen(parameter) + 1);
    char *map = maps;

    if (maps == NULL)
	error(FATAL | PERROR, "");
    strcpy(maps, parameter);

    DebugStatement( if (debug(DEBUG_STATUS))
			printf("Language-extension maps:\n"); )
    while (map != NULL)
    {
	char *end = strchr(parameter, ',');

	if (end != NULL)
	    *end = '\0';
	installLangMap(map);
	if (end != NULL)
	    map = end + 1;
	else
	    map = NULL;
    }
    free(maps);
}

static void processLongOption( optionString )
    const char *const optionString;
{
    enum { MaxOptionName = 10 };
    char optionName[MaxOptionName + 1];
    const char *const equal = strchr(optionString, '=');
    const char *parameter = (equal == NULL) ? "" : equal + 1;
    const size_t optionLength = (equal == NULL) ? strlen(optionString) :
	    					  (equal - optionString);

    DebugStatement( if (debug(DEBUG_OPTION))
			fprintf(errout, "Option: --%s\n", optionString); )

    strncpy(optionName, optionString, optionLength);
    if (optionLength < (size_t)MaxOptionName)
	optionName[optionLength] = '\0';
    else
	optionName[(size_t)MaxOptionName] = '\0';

#define isOption(string)	(strcmp(optionName, string) == 0)
    if (isOption("append"))
	Option.append = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("excmd"))
	processExcmdOption(optionName, parameter);
    else if (isOption("format"))
	processFormatOption(optionName, parameter);
    else if (isOption("help"))
	{ printHelp(LongOptionDescription, stdout); exit(0); }
    else if (isOption("if0"))
	Option.if0 = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("lang"))
	processLangOption(optionName, parameter);
    else if (isOption("langmap"))
	processLangMapOption(optionName, parameter);
    else if (isOption("recurse"))
#ifdef RECURSE_SUPPORTED
	Option.recurse = getBooleanOption(optionName, parameter, TRUE);
#else
	error(FATAL, "--%s option not supported on this host", optionName);
#endif
    else if (isOption("sort"))
	Option.sorted = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("totals"))
	Option.printTotals = getBooleanOption(optionName, parameter, TRUE);
    else if (isOption("version"))
    {
	printProgramIdentification(stdout);
	exit(0);
    }
    else
	error(FATAL, "Unknown option: --%s", optionName);
#undef isOption
}

static void processCompoundOption( option, pArg, argList, pArgNum )
    const int option;
    char **const pArg;
    char *const *const argList;
    int *const pArgNum;
{
    char *param;

    DebugStatement( if (debug(DEBUG_OPTION) && option != '-')
			fprintf(errout, "Option: -%c ", option); )
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
		applyTagInclusionList(param);
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
		break;
    case 'b':	param = readOptionArg(option, pArg, argList, pArgNum);
		if (atol(param) < 0)
		    error(FATAL, "-%c: Invalid line number", option);
		Option.breakLine = atol(param);
		break;
#endif
    default: error(FATAL, "Unknown option: -%c", option); break;
    }
    DebugStatement( if (debug(DEBUG_OPTION)) fputs("\n", errout); )
}

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
			error(FATAL, "-R option not supported on this host");
#endif
	case 'u':	Option.sorted		= FALSE;	break;
	case 'w':
	case 'W':	break;
	case 'x':	Option.xref		= TRUE;		break;

	case '?':	printHelp(LongOptionDescription, stdout);
			exit(0);

	default:	handled = FALSE;			break;
    }

    DebugStatement( if (handled && debug(DEBUG_OPTION))
			fprintf(errout, "Option: -%c\n", option); )

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

	if (*arg++ != '-')		/* stop at first non-option switch */
	    break;
	else if (*arg == '-')		/* double dash: "--" */
	    processLongOption(arg + 1);
	else while ((c = *arg++) != '\0')
	{
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
    const char *const whiteSpace = " \t\n";
    const char *p = string;
    unsigned int numWords = 0;

    p += strspn(p, whiteSpace);			/* skip over leading spaces */
    while (*p != '\0')
    {
	++numWords;
	p += strcspn(p, whiteSpace);		/* skip to white space */
	p += strspn(p, whiteSpace);		/* skip to non-white space */
    }
    return numWords;
}

static char **creatArgListForString( string )
    const char *const string;
{
    const unsigned int numWords = countStringWords(string);
    char **argList = NULL;

    if (string != NULL  &&  string[0] != '\0')
    {
	/*  We place the parsed string at the end of the memory block, past
	 *  the bottom of the argument table.
	 */
	const size_t argListSize= (numWords + 1) * sizeof(char *);
	const size_t blockSize	= argListSize + strlen(string) + 1;

	argList	= (char **)malloc(blockSize);
	if (argList != NULL)
	    parseStringToArgs(string, (char *)argList + argListSize,
			      argList, numWords);
    }
    return argList;
}

extern void *parseEnvironmentOptions()
{
    const char *envOptions = NULL;
    char **argList = NULL;

    if (Option.startedAsEtags)
	envOptions = getenv(ETAGS_ENVIRONMENT);
    if (envOptions == NULL)
	envOptions = getenv(CTAGS_ENVIRONMENT);
    if (envOptions != NULL  &&  envOptions[0] != '\0')
    {
	argList = creatArgListForString(envOptions);
	parseOptions(argList);
    }
    return argList;
}

/* vi:set tabstop=8 shiftwidth=4: */
