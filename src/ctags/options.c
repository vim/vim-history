/*****************************************************************************
*   $Id: options.c,v 5.2 1998/02/26 05:32:07 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
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
  Usage: ctags [-aBeFnNuwWx] [-{f|o} name] [-h list] [-i [+-=]types]\n\
               [-I list] [-L file] [-p path] [--append] [--excmd=n|p|m]\n\
               [--format=level] [--help] [--if0] [--sort] [--totals]\n\
               [--version] file(s)\n"

#define ETAGS_INVOCATION  "\
  Usage: etags [-ax] [-{f|o} name] [-h list] [-i [+-=]types] [-I list]\n\
               [-L file] [-p path] [--append] [--help] [--if0] [--totals]\n\
               [--version] file(s)\n"

#define CTAGS_ENVIRONMENT	"CTAGS"
#define ETAGS_ENVIRONMENT	"ETAGS"

/*  The following separators are permitted for list options.
 */
#define HEADER_SEPARATORS   ".,;"
#define IGNORE_SEPARATORS   ",; \t\n"

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

optionValues Option = {
    {
	TRUE,		/* -ic */
	TRUE,		/* -id */
	TRUE,		/* -ie */
	TRUE,		/* -if */
	TRUE,		/* -ig */
	FALSE,		/* -im */
	FALSE,		/* -ip */
	TRUE,		/* -is */
	TRUE,		/* -it */
	TRUE,		/* -iu */
	TRUE,		/* -iv */
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
    TRUE,		/* -u, --sort */
    FALSE,		/* -w */
    FALSE,		/* -x */
    NULL,		/* -L */
    NULL,		/* -o */
    { "h", "H", "hh", "hpp", "hxx", "h++", NULL },	    /* -h */
#ifdef DEBUG
    0, 0,		/* -D, -b */
#endif
    FALSE,		/* started as etags */
    FALSE,		/* brace formatting */
    DEFAULT_FILE_FORMAT,/* --format */
    FALSE,		/* --if0 */
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
 {1,"       types are supported (default settings are shown in brackets):"},
 {1,"          c   class names [on]"},
 {1,"          d   macro definitions [on]"},
 {1,"          e   enumerators (values inside an enumeration) [on]"},
 {1,"          f   function (or method) definitions [on]"},
 {1,"          g   enumeration names [on]"},
 {1,"          m   class, structure, and union data members [off]"},
 {1,"          p   external function prototypes [off]"},
 {1,"          s   structure names [on]"},
 {1,"          t   typedefs [on]"},
 {1,"          u   union names [on]"},
 {1,"          v   variable declarations [on]"},
 {1,"       In addition, the following modifiers are accepted:"},
 {1,"          F   include source filenames as tags [off]"},
 {1,"          S   include static tags (special case of above types) [on]"},
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
 {0,"  -u   Equivalent to --sort=no."},
 {0,"  -w   Exclude warnings about duplicate tags (default)."},
 {0,"  -W   Generate warnings about duplicate tags (disabled if not sorted)."},
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
static void readExtensionList __ARGS((char *const list));
static void clearTagList __ARGS((void));
static void applyTagInclusionList __ARGS((const char *const list));
static void readIgnoreList __ARGS((char *const list));
static void readIgnoreListFromFile __ARGS((const char *const fileName));
static char *readOptionArg __ARGS((const int option, char **const pArg, char *const *const argList, int *const pArgNum));
static void processHeaderListOption __ARGS((const int option, char **const argP, char *const *const argList, int *const argNumP));
static void processIgnoreOption __ARGS((const int option, char **const argP, char *const *const argList, int *const argNumP));
static boolean getBooleanOption __ARGS((const char *const optionName, const char *const parameter, const boolean defaultValue));
static void processExcmdOption __ARGS((const char *const optionName, const char *const parameter));
static void processFormatOption __ARGS((const char *const optionName, const char *const parameter));
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

/*  Reads a list of header file extensions.
 */
static void readExtensionList( list )
    char *const list;
{
    const char *extension = strtok(list, HEADER_SEPARATORS);
    int extIndex = 0;

    while (extension != NULL  &&  extIndex < MaxHeaderExtensions)
    {
	DebugStatement( if (debug(DEBUG_STATUS))
			    printf("header extension: %s\n", extension); )
	Option.headerExt[extIndex++] = extension;
	extension = strtok(NULL, HEADER_SEPARATORS);
    }
    Option.headerExt[extIndex] = NULL;
}

static void clearTagList()
{
    Option.include.classNames	= FALSE;
    Option.include.defines	= FALSE;
    Option.include.enumerators	= FALSE;
    Option.include.enumNames	= FALSE;
    Option.include.functions	= FALSE;
    Option.include.members	= FALSE;
    Option.include.prototypes	= FALSE;
    Option.include.structNames	= FALSE;
    Option.include.typedefs	= FALSE;
    Option.include.unionNames	= FALSE;
    Option.include.variables	= FALSE;
    Option.include.sourceFiles	= FALSE;
    Option.include.statics	= FALSE;
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
	    case 'm':	Option.include.members		= mode;		break;
	    case 'p':	Option.include.prototypes	= mode;		break;
	    case 's':	Option.include.structNames	= mode;		break;
	    case 't':	Option.include.typedefs		= mode;		break;
	    case 'u':	Option.include.unionNames	= mode;		break;
	    case 'v':	Option.include.variables	= mode;		break;
	    case 'F':	Option.include.sourceFiles	= mode;		break;
	    case 'S':	Option.include.statics		= mode;		break;

	    default: error(FATAL, "-i: Invalid tag option '%c'", *p);	break;
	}
}

static void readIgnoreList( list )
    char *const list;
{
    const char *token = strtok(list, IGNORE_SEPARATORS);

    while (token != NULL)
    {
	unsigned int i = Option.ignore.count++;

	if (Option.ignore.count > Option.ignore.max)
	{
	    Option.ignore.max = Option.ignore.count + 10;
	    Option.ignore.list = (char **)realloc(Option.ignore.list,
					    Option.ignore.max * sizeof(char *));
	}
	Option.ignore.list[i] = (char *)malloc(strlen(token) + 1);
	strcpy(Option.ignore.list[i], token);
	DebugStatement(	if (debug(DEBUG_STATUS))
			    printf("ignore token: %s\n", token); )
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
	unsigned int i = Option.ignore.count;

	while (fscanf(fp, "%255s", ignoreToken) == 1)
	    ++Option.ignore.count;
	rewind(fp);
	if (Option.ignore.count > Option.ignore.max)
	{
	    Option.ignore.max = Option.ignore.count;
	    Option.ignore.list = (char **)realloc(Option.ignore.list,
					    Option.ignore.max * sizeof(char *));
	}
	if (Option.ignore.list == NULL)
	    error(WARNING | PERROR, "cannot create ignore list");
	else while (fscanf(fp, "%255s", ignoreToken) == 1)
	{
	    DebugStatement( assert(i < Option.ignore.count); )
	    Option.ignore.list[i] = (char *)malloc(strlen(ignoreToken) + 1);
	    strcpy(Option.ignore.list[i], ignoreToken);
	    ++i;
	    DebugStatement( if (debug(DEBUG_STATUS))
				printf("ignore token: %s\n", ignoreToken); )
	}
    }
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
	readExtensionList(list);
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

    if (parameter == NULL  ||  parameter[0] == '\0')
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
    else if (format <= MaxSupportedTagFormat)
	Option.tagFileFormat = format;
    else
	error(FATAL, "Unsupported value for \"--%s\" option", optionName);
}

static void processLongOption( optionString )
    const char *const optionString;
{
    enum { MaxOptionName = 10 };
    char optionName[MaxOptionName + 1];
    const char *const equal = strchr(optionString, '=');
    const char *parameter = (equal == NULL) ? NULL : equal + 1;
    const size_t optionLength = (equal == NULL) ? strlen(optionString) :
	    					  (equal - optionString);

    DebugStatement( if (debug(DEBUG_OPTION))
			fprintf(errout, "Option: --%s\n", optionString); )

    strncpy(optionName, optionString, optionLength);
    if (optionLength < MaxOptionName)
	optionName[optionLength] = '\0';
    else
	optionName[MaxOptionName] = '\0';

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
	case 'u':	Option.sorted		= FALSE;	break;
	case 'w':	Option.warnings		= FALSE;	break;
	case 'W':	Option.warnings		= TRUE;		break;
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
    if (envOptions != NULL)
    {
	argList = creatArgListForString(envOptions);
	parseOptions(argList);
    }
    return argList;
}

/* vi:set tabstop=8 shiftwidth=4: */
