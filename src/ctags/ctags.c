/*  $Revision: 2.2 $		$Date: 1996/08/18 06:41:34 $
 *
 *  vim:set ts=4 sw=4 tw=78:
 *
 *	Copyright (c) 1996, Darren Hiebert
 *
 *	Author: Darren Hiebert (darren@sirsi.com, darren@hiwaay.net,
 *							http://fly.hiwaay.net/~darren/)
 *
 *	This source code is released into the public domain. It is provided on an
 *	as-is basis and no responsibility is accepted for its failure to perform
 *	as expected. It is worth at least as much as you paid for it!
 *
 *	This is a reimplementation of the ctags(1) program. It is an attempt to
 *	provide a fully featured ctags program which is free of the limitations
 *	which most (all?) others are subject to.
 *
 *	It is derived from and inspired by the ctags program by Steve Kirkendall
 *	(kirkenda@cs.pdx.edu) that comes with the Elvis vi clone (though almost
 *	none of the original code remains). This, too, was freely available.
 *
 *	This program provides the following features:
 *
 *	Support for both K&R style and new ANSI style function definitions.
 *
 *	Generates tags for the following objects:
 *		- macro definitions
 *		- enumeration values
 *		- function definitions (and C++ methods)
 *		- function declarations (optional)
 *		- enum, struct and union tags and C++ class names
 *		- typedefs
 *		- variables
 */

/*============================================================================
=	Include files
============================================================================*/
#include <stdio.h>
#include <string.h>			/* to declare strchr(), strcmp() */
#include <ctype.h>			/* to declare isalnum(), isalpha(), isspace() */
#if __MWERKS__
# include <stat.h>			/* there is no sys directory on the Mac */
#else
# include <sys/types.h>		/* for a number of types used in sys/stat.h */
# include <sys/stat.h>		/* to declare struct stat */
#endif

#if defined(__STDC__)
# include <stdlib.h>		/* to declare malloc() */
# if defined(__BORLANDC__)
#  define MSDOS
#  include <io.h>			/* to declare unlink() */
#  include <dir.h>			/* to declare mktemp() */
# else
#  if defined(AMIGA)
#   include <fcntl.h>		/* to declare mktemp() */
#  else
#   include <unistd.h>		/* to declare mktemp(), stat(), unlink() */
#  endif
# endif
#else
# if defined(WIN32)
#  include <stdlib.h>		/* to declare malloc() */
#  include <io.h>			/* to declare mktemp() */
# else
#  define const				/* not guaranteed to be available on non-ANSI */
# endif
#endif

#ifdef DEBUG
# include <assert.h>
#endif

/*============================================================================
=	Defines
============================================================================*/

#define VERSION		"Exuberant Ctags, Version 1.4, by Darren Hiebert"
#define INVOCATION	"\
Usage: ctags [-aBdFnsStTuwWx] [-{f|o} name] [-h list] [-i [+-=]types]\n\
             [-I file] [-L file] [--help] file(s)"

#if defined(__MSDOS__) && ! defined(MSDOS)
# define MSDOS
#endif

#if ! defined (__ARGS) && (defined(__STDC__) || defined(WIN32))
# define __ARGS(x)	x
# define USING_PROTOTYPES
#else
# define __ARGS(x)	()
# undef USING_PROTOTYPES
#endif

/*	Tells us how to open source file.
 */
#if defined(MSDOS) || defined(OS2) || defined(WIN32)
# define SOURCE_MODE	"rb"		/* open in binary or fseek() misbehaves */
#else
# define SOURCE_MODE	"r"
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(AMIGA)
# define INTERNAL_SORT
# define TMP_TAGS	"tagsXXXXXX"
#else
# define TMP_TAGS	"/tmp/ctagsXXXXXX"
#endif

#if __MWERKS__
/* Using the Metrowerks compiler on the Mac */
/* My Un*x-box says
 * __EXTERN char * tmpnam __PROTO((char *));
 * is the ANSI-name of this funktion.
 */
# define mktemp(x)	tmpnam(x)
#endif

#define ENV_VARIABLE_NAME		"CTAGS"
#define MAX_HEADER_EXTENSIONS	100
#define MAX_ENV_ARGS			20
#define CPP_NESTING_LEVEL		20

#ifndef MAXPATH
# define MAXPATH	256			/* maximum length of file path name with null */
#endif

#ifndef MAXNAME
# define MAXNAME	256			/* maximum length of token with null */
#endif

/*	Maximum length of tag line and CTAGS option, including null.
 */
#ifndef MAXLINE
# define MAXLINE	1024
#endif

/*	The following separators are permitted for list options.
 */
#define HEADER_SEPARATORS	".,;"		/* header extensions */

/*	White space characters.
 */
#define SPACE			' '
#define NEWLINE			'\n'
#define CRETURN			'\r'
#define FORMFEED		'\f'
#define TAB				'\t'
#define VTAB			'\v'

/*	Some hard to read characters.
 */
#define DOUBLE_QUOTE	'"'
#define SINGLE_QUOTE	'\''
#define BACKSLASH		'\\'

/*============================================================================
=	Macros
============================================================================*/

/*	These may not be defined in a non-UNIX environment.
 */
#ifndef S_ISREG
# ifdef AMIGA
#  define S_ISREG(mode)		TRUE			/* Amiga only has regular files */
# else
#  define S_ISREG(mode)		((mode) & S_IFREG)
# endif
#endif

#ifndef S_ISLNK
# define S_ISLNK(mode)		FALSE
#endif

#ifdef AMIGA
# define REMOVE(filename)	remove(filename)
#else
# define REMOVE(filename)	unlink(filename)
#endif

/*  Is the character valid as a character of a C identifier?
 */
#define isident(c)		(isalnum(c) || (c) == '_')

/*  Is the character valid as the first character of a C identifier?
 */
#define isident1(c)		(isalpha(c) || (c) == '_')

#define isspacetab(c)	((c) == ' ' || (c) == '\t')
#define cppIgnore()		(Cpp.directive.ifdef[Cpp.directive.level].ignore)
#define tagTypeName(type)	(((type) < TAG_NUMTYPES) ? TagTypeNames[type] : "?")

/*============================================================================
=	Data declarations
============================================================================*/

#undef FALSE
#undef TRUE
typedef enum { FALSE, TRUE } boolean;

typedef enum { COMMENT_NONE, COMMENT_C, COMMENT_CPLUS } Comment;

/*	Describes the type of tag being generated. This is used for debugging
 *	purposes only.
 */
typedef enum {
	TAG_BLOCKTAG,			/* enum/struct/union tag or C++ class name */
	TAG_DEFINE,				/* C pre-processor define */
	TAG_ENUM,				/* enumeration value */
	TAG_FUNCDECL,			/* function declaration */
	TAG_FUNCTION,			/* function definition */
	TAG_TYPEDEF,			/* typedef name */
	TAG_VARIABLE,			/* variable defintion */
	TAG_NUMTYPES			/* must be last */
} tag_t;

/*----------------------------------------------------------------------------
 *	Used for describing a statement
 *--------------------------------------------------------------------------*/

/*	Used for reporting the type of object parsed by nextToken().
 */
typedef enum {
	 TOK_ARGS,				/* a parenthetical pair and its contents */
	 TOK_BODY,				/* a brace enclosed block */
	 TOK_COMMA,				/* the comma character */
	 TOK_IGNORE,			/* a sequence not to be seen by createTags() */
	 TOK_ENUM_BODY_END,		/* the beginning of a list of enumeration values */
	 TOK_EOF,				/* end of file */
	 TOK_NAME,				/* an unknown name */
	 TOK_SEMICOLON,			/* the semicolon character */
	 TOK_SPEC				/* a storage class, qualifier, type, etc. */
} token_t;

/*	This describes the scoping of the current statement.
 */
typedef enum {
	SCOPE_GLOBAL,			/* no storage class specified */
	SCOPE_STATIC,			/* static storage class */
	SCOPE_EXTERN,			/* external storage class */
	SCOPE_TYPEDEF			/* scoping depends upon context */
} scope_t;

/*	Describes the specifier associated with TOK_SPEC: either unspecified or
 *	one of class, enum, struct or union.
 */
typedef enum {
	DECL_UNSPEC,			/* unspecified (non-specific) */
	DECL_CLASS,				/* C++ class */
	DECL_ENUM,				/* enumeration */
	DECL_STRUCT,			/* structure */
	DECL_UNION				/* union */
} decl_t;

typedef struct {
	long	location;		/* file position of line containing name */
	long	lineNumber;		/* line number of tag */
	char	name[MAXNAME];	/* the name of the token */
} tag_info;

typedef enum {
	DRCTV_NONE,
	DRCTV_HASH,
	DRCTV_DEFINE,
	DRCTV_IF
} drctv_state;

/*	Describes the statement currently undergoing analysis.
 */
typedef struct {
	scope_t	scope;
	decl_t	declaration;
	token_t	token;			/* the most recent type of token */
	token_t	prev[2];		/* the previous tokens */
	boolean	funcPtr;		/* whether 'name' is a function pointer */
	boolean	gotName;		/* whether a name has yet been parsed */
	boolean	inEnumBody;		/* currently within enumeration value list */
	boolean buf1;			/* is tag[1] the primary buffer? */
	tag_info tag[2];		/* information regarding current tag candidate */
} statement_t;

/*============================================================================
=	Data definitions
============================================================================*/

/*	Holds a modified copy of the environment option.
 */
static char EnvArgList[MAXLINE];

/*	Holds a list of tokens to ignore.
 */
static char **IgnoreList = NULL;

/*	Note that the strings in this array must correspond to the types in the
 *	tag_t enumeration.
 */
static const char *TagTypeNames[] = {
	"tag", "def", "enum", "proto", "func", "type", "var"
};

static const char *const Help[] = {
 "    --help     Prints a more detailed help message.",
 "    -a         Append tags to existing tag file.",
 "    -B         Use backward searching patterns (?...?).",
#ifdef DEBUG
 "    -D <level> Set debug level.",
#endif
 "    -f <name>  Name for output tag file (default \"tags\").",
 "    -F         Use forward searching patterns (/.../) (default).",
 "    -h <list>  List of header file extensions (default \".h.H.hpp.hxx.h++\").",
 "    -i <types> List of tag types to include [defgptvPS] (default \"=defgtvS\").",
 "    -I <file>  List of tokens to ignore are read from specified file.",
 "    -L <file>  List of source file names are read from specified file.",
 "    -n         Use line numbers in tag file instead of search patterns.",
#ifdef DEBUG
 "    -N <line>  Set break line.",
#endif
 "    -o <name>  Alternative for -f.",
 "    -u         Unsorted; do not sort tags. Disables duplicate tag warnings.",
 "    -w         Exclude warnings about duplicate tags (default).",
 "    -W         Generate warnings about duplicate tags.",
 "    -x         Print tabular cross reference file to standard output.",
 "    -[dsStT]   Backward compatibility options. See --help for more info.",
 NULL,
};

static const char *const LongHelp[] = {
 "    -a   Append the tags to an existing tag file.",
 "    -B   Use backward searching patterns (?...?).",
#ifdef DEBUG
 "    -D <level> Set debug level.",
#endif
 "    -f <name>",
 "         Output tags to the specified file (default is \"tags\").",
 "         If specified as \"-\", tags are written to standard output.",
 "    -F   Use forward searching patterns (/.../) (default).",
 "    -h <list>",
 "         Specifies a list of file extensions used for headers.",
 "         The default list is \".h.H.hpp.hxx.h++\".",
 "    -i <types>",
 "         Specifies the list of tag types to include in the output file.",
 "         \"Types\" is a group of letters designating the types of tags",
 "         affected. Each letter or group of letters may be preceded by",
 "         either a '+' sign (default, if omitted) to add it to those already",
 "         included, a '-' sign to exclude it from the list (e.g. to exclude",
 "         a default tag type), or an '=' sign to include its corresponding",
 "         tag type at the exclusion of those not listed. A space separating",
 "         the option letter from the list is optional. The following tag",
 "         types are supported:",
 "            d   macro definitions",
 "            e   enumerated values (values inside enum{...})",
 "            f   function and method definitions",
 "            g   enum/struct/union tags (or new C++ types)",
 "            p   external function prototypes",
 "            t   typedefs",
 "            v   variable declarations",
 "         In addition, the following two modifiers are accepted:",
 "            P   prefix static tags with \"filename:\" (Elvis style).",
 "            S   include static tags (special case of above types)",
 "         The default value of list is \"=defgtvS\" (i.e all tag types",
 "         except for function prototypes; include static tags but do not",
 "         prefix them).",
 "    -I <file>",
 "         A list of tokens to ignore are read from the specified file.",
 "         Particularly useful when a function definition or declaration",
 "         contains a prototyping macro before the parameter list.",
 "    -L <file>",
 "         A list of source file names are read from the specified file.",
 "         If specified as \"-\", then standard input is read.",
 "    -n   Use line numbers in tag file instead of search patterns.",
#ifdef DEBUG
 "    -N <line>  Set break line.",
#endif
 "    -o   Alternative for -f.",
 "    -u   Unsorted; do not sort tags. Disables warnings for duplicate tags",
 "    -w   Exclude warnings about duplicate tags (default).",
 "    -W   Generate warnings about duplicate tags.",
 "    -x   Print a tabular cross reference file to standard output.",
 "",
 "  The following tag control options are accepted only for backwards",
 "  compatibility with other versions of ctags and (except for -s) are",
 "  selected by default in this version. Use of the -i option is preferred.",
 "    -d   Include macro definitions (equiv. to -i+d).",
 "    -s   Include static tags prefixed by \"filename:\" (equiv. to -i+SP).",
 "    -S   Include static tags without filename prefix (equiv. to -i+S-P).",
 "    -t   Include typedefs (equiv. to -i+t).",
 "    -T   Include typedefs and enum/struct/union tags (equiv. to -i+gt).",
 NULL,
};

/*  This contains the command line options.
 */
static struct {
	struct {
		boolean	defines;	/* -id  include tags for defines */
		boolean	enumValues;	/* -ie  include tags for enumeration value */
		boolean	functions;	/* -if  include tags for functions */
		boolean	blockTags;	/* -ig  include tags for enum, struct and union */
		boolean	prototypes;	/* -ip  include tags for external func. prototypes*/
		boolean	typedefs;	/* -it  include tags for typedefs */
		boolean	variables;	/* -iv  include tags for variables */
		boolean	prefix;		/* -iP  prefix static tags with filename */
		boolean	statics;	/* -iS  include static tags */
	} include;
	boolean	append;			/* -a  append to "tags" files */
	boolean	backward;		/* -B  regexp patterns search backwards */
	boolean	lineNumbers;	/* -n  line # in tag file instead of patterns */
	boolean	unsorted;		/* -u  do not sort tags */
	boolean warnings;		/* -w  generate warnings about duplicate tags */
	boolean xref;			/* -x  generate xref output instead */
	const char *ignoreList;	/* -I  name of file containing tokens to ignore */
	const char *fileList;	/* -L  name of file containing names of files */
	const char *tagFile;	/* -o  name of tags file */
	const char *headerExt[MAX_HEADER_EXTENSIONS + 1];/* -h  header extensions */
#ifdef DEBUG
	int debugLevel;			/* -D  debugging output */
	long breakLine;			/* -N  source line at which to call lineBreak() */
#endif
	boolean	braceFormat;	/* use brace formatting to detect end of block */
} Option = {
	{
		TRUE,			/* -id */
		TRUE,			/* -ie */
		TRUE,			/* -if */
		TRUE,			/* -ig */
		FALSE,			/* -ip */
		TRUE,			/* -it */
		TRUE,			/* -iv */
		FALSE,			/* -iP */
		TRUE			/* -iS */
	},
	FALSE,				/* -a */
	FALSE,				/* -B */
	FALSE,				/* -n */
	FALSE,				/* -u */
	FALSE,				/* -w */
	FALSE,				/* -x */
	NULL,				/* -I */
	NULL,				/* -L */
	"tags",				/* -o */
	{ "h", "H", "hpp", "hxx", "h++", NULL },		/* -h */
#ifdef DEBUG
	0, 0,				/* -D, -N */
#endif
	FALSE				/* brace formatting */
};

static struct {
	const char *name;
	FILE *fp;
	size_t numTags;
	struct { size_t line, tag, file; } max;
} TagFile = { NULL, NULL, 0, { 0, 0, 0 } };

/*	These are used for reading a source File.  It keeps track of line numbers.
 */
static struct {
	const char *name;		/* name of the current file */
	FILE	*fp;			/* stream used for reading the file */
	long	lineNumber;		/* line number in the current file */
	long	seek;			/* fseek() offset to the start of current line */
	boolean	afterNL;		/* boolean: was previous character a newline? */
	int		ungetch;		/* a single character that was ungotten */
	int		header;			/* boolean: is the current file a header file? */
	boolean warned;			/* format warning kludge */
} File = { NULL, NULL, 0, -1, FALSE, 0, FALSE, FALSE };


/*	Defines the state of the pre-processor.
 */
static struct {
	int		ungetch;			/* an ungotten character, if any */
	struct {
		drctv_state	state;
		boolean		accept;		/* is a directive syntatically permitted? */
		tag_info	tag;		/* the name associated with the directive */
		boolean		resolve;	/* must resolve if/else/elif/endif branch */
		int			level;		/* level 0 is not used */
		struct {
			boolean ignore;
			boolean pathChosen;
		} ifdef[CPP_NESTING_LEVEL];
	} directive;
} Cpp = {
	0,
	{
		DRCTV_NONE,
		FALSE,
		{ 0, 0, "" },
		FALSE,
		0,
		{{FALSE,FALSE}}
	}
};

/*	Contains a set of strings describing the set of "features" compiled into
 *	the code.
 */
static const char *const Features[] = {
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
#ifdef __EMX__
	"emx",
#endif
#ifdef INTERNAL_SORT
	"internal_sort",
#endif
#ifdef DEBUG
	"debug",
#endif
	NULL
};

/*============================================================================
=	Function prototypes
============================================================================*/
#if !defined(__STDC__) && !defined(WIN32)
extern char *getenv();
extern void free();
extern void *malloc();
extern int stat();
extern int system();
extern int unlink();
#endif

/*	This declaration is often missing from GCC installs.
 */
#ifndef NO_MKTEMP_DECL
extern char *mktemp __ARGS((char *template));
#endif

#ifdef DEBUG
static void lineBreak __ARGS((void)); 
static void debugOpen __ARGS((const char *const name)); 
static void debugPutc __ARGS((const int c, const int level)); 
static void debugLabel __ARGS((const tag_t tagType, const char *const tagName));
static void clearString __ARGS((char *const string, const int length));
#endif

static boolean isFileHeader __ARGS((const char *const name)); 
static boolean fileOpen __ARGS((const char *const name)); 
static void fileClose __ARGS((void)); 
static void fileUngetc __ARGS((int c)); 
static void fileNewline __ARGS((void)); 
static int fileGetc __ARGS((void)); 
static size_t fileCopyLine __ARGS((FILE *const fp, const long int seek)); 
static size_t fileCopyLineShort __ARGS((FILE *const fp, const long int seek)); 
static void writeXrefEntry __ARGS((const tag_info *const tag, const tag_t tagType)); 
static void makeTagEntry __ARGS((const tag_info *const tag, const scope_t scope, const tag_t tagType, const boolean useLineNumber)); 
static void makeTag __ARGS((const tag_info *const tag, const scope_t scope, const tag_t tagType)); 
static void makeDefineTag __ARGS((const tag_info *const tag, const scope_t scope)); 
static boolean cppOpen __ARGS((const char *const name)); 
static void cppClose __ARGS((void)); 
static void cppUngetc __ARGS((const int c)); 
static boolean cppReadDirective __ARGS((int c, char *name)); 
static boolean cppReadIdentifier __ARGS((int c, tag_info *const tag)); 
static boolean pushCppIgnore __ARGS((const boolean ignore, const boolean pathChosen)); 
static boolean popCppIgnore __ARGS((void)); 
static boolean prevCppIgnore __ARGS((void)); 
static boolean setCppIgnore __ARGS((const boolean ignore)); 
static boolean wasPathChosen __ARGS((void)); 
static boolean handleDirective __ARGS((const int c)); 
static Comment isComment __ARGS((void)); 
static int skipOverCComment __ARGS((void)); 
static int skipOverCplusComment __ARGS((void)); 
static int skipToEndOfString __ARGS((void)); 
static int skipToEndOfChar __ARGS((void)); 
static int cppGetc __ARGS((void)); 
static int skipToNonWhite __ARGS((void)); 
static int skipToCharacter __ARGS((const int findchar)); 
static void skipToFormattedBraceMatch __ARGS((void)); 
static boolean skipToMatch __ARGS((const char *const pair)); 
static int skipInitializer __ARGS((const boolean inEnumBody)); 
static void readIdendifier __ARGS((const int firstChar, char *const name)); 
static int skipParameterDeclarations __ARGS((int c)); 
static boolean analyzeParens __ARGS((statement_t *const st)); 
static boolean isIgnoreToken __ARGS((const char *const name)); 
static void analyzeIdentifier __ARGS((statement_t *const st)); 
static boolean nextToken __ARGS((statement_t *const st, const int nesting)); 
static void initStatement __ARGS((statement_t *const st)); 
static boolean createTags __ARGS((const int nesting)); 
static boolean createTagsForFile __ARGS((const char *const name));
static boolean isSourceFile __ARGS((const char *const filename)); 
static const char *getNextListFile __ARGS((FILE *const fp)); 
static void createTagsForList __ARGS((const char *const listFile)); 
static void createTagsForArgs __ARGS((char *const *const argList)); 
#ifdef INTERNAL_SORT
static void fatal __ARGS((const char *const msg)); 
static int compareTags __ARGS((const void *const one, const void *const two)); 
static void writeSortedTags __ARGS((char **const table, const boolean toStdout)); 
static void internalSortTags __ARGS((const boolean toStdout)); 
#else
static void externalSortTags __ARGS((const boolean toStdout));
#endif
static void printHelp __ARGS((const char *const *const help, FILE *const where)); 
static void printUsage __ARGS((const char *const error)); 
static void readExtensionList __ARGS((char *const list)); 
static void clearTagList __ARGS((void)); 
static void applyTagInclusionList __ARGS((const char *const list)); 
static void readIgnoreList __ARGS((const char *const fileName)); 
static void freeIgnoreList __ARGS((void)); 
static char *readOptionArg __ARGS((const int option, char **const arg, char *const *const argList, int *const argNum)); 
static int parseOptions __ARGS((char *const *const argList)); 
static void parseEnvironmentOptions __ARGS((void)); 

/*============================================================================
=	Function definitions
============================================================================*/

#ifdef DEBUG

static void lineBreak() {}

static void debugOpen( name )
	const char *const name;
{
	if ((Option.debugLevel & 4) != 0)
	{
		printf("Opening: %s\n", name);
		fflush(stdout);
	}
}

static void debugPutc( c, level )
	const int c;
	const int level;
{
	if ((Option.debugLevel & level) != 0  &&  c != EOF)
	{
		putchar(c);
		fflush(stdout);
	}
}

static void debugLabel( tagType, tagName )
	const tag_t tagType;
	const char *const tagName;
{
	if ((Option.debugLevel & 3) != 0)
	{
		printf("<#%s:%s#>", tagTypeName(tagType), tagName);
		fflush(stdout);
	}
}

static void clearString( string, length )
	char *const string;
	const int length;
{
	int i;

	for (i = 0 ; i < length ; ++i)
		string[i] = '\0';
}

#endif

/*	Determines whether the specified file name is considered to be a header
 *	file for the purposes of determining whether enclosed tags are global or
 *	static.
 */
static boolean isFileHeader( name )
	const char *const name;
{
	boolean header = FALSE;					/* default unless match found */
	const char *extension;

	extension = strrchr(name, '.');			/* find last '.' */
	if (extension != NULL)
	{
		int i;

		++extension;						/* skip to character after '.' */
		for (i = 0 ; Option.headerExt[i] != NULL ; ++i)
		{
			if (strcmp(Option.headerExt[i], extension) == 0)
			{
				header = TRUE;				/* found in list */
				break;
			}
		}
	}
	return header;
}

/*	This function opens a file, and resets the line counter.  If it fails,
 *	it will display an error message and leave the File.fp set to NULL.
 */
static boolean fileOpen( name )
	const char *const name;
{
	boolean opened = FALSE;
	struct stat status;

	/*	If another file was already open, then close it.
	 */
	if (File.fp != NULL)
	{
		fclose(File.fp);						/* close any open source file */
		File.fp = NULL;
	}

	if (stat(name, &status) != 0)
		perror(name);
	else if (S_ISLNK(status.st_mode) || S_ISREG(status.st_mode))
	{
		File.fp = fopen(name, SOURCE_MODE);
		if (File.fp == NULL)
			perror(name);
		else
		{
			opened = TRUE;
			File.name		= name;
			File.lineNumber	= 0L;
			File.seek		= 0L;
			File.afterNL	= TRUE;
			File.warned		= FALSE;

			if (strlen(name) > TagFile.max.file)
				TagFile.max.file = strlen(name);

			/*	Determine whether this is a header File.
			 */
			File.header = isFileHeader(name);
#ifdef DEBUG
			debugOpen(name);
#endif
		}
	}
	return opened;
}

static void fileClose()
{
	if (File.fp != NULL)
	{
		fclose(File.fp);
		File.fp = NULL;
	}
}

static void fileUngetc( c )
	int c;
{
	File.ungetch = c;
}

static void fileNewline()
{
	File.afterNL = FALSE;
	File.seek	 = ftell(File.fp);
	File.lineNumber++;
#ifdef DEBUG
	if (Option.breakLine == File.lineNumber) lineBreak();
#endif
}

/*	This function reads a single character from the stream.  If the
 *	previous character was a newline, then it also increments File.lineNumber
 *	and sets file_offset.
 */
static int fileGetc()
{
	boolean escaped = FALSE;
	int	c;

	/*	If there is an ungotten character, then return it.  Don't do any
	 *	other processing on it, though, because we already did that the
	 *	first time it was read.
	 */
	if (File.ungetch != 0)
	{
		c = File.ungetch;
		File.ungetch = 0;
		return c;			/* return here to avoid re-calling debugPutc() */
	}

nextChar:		/* not structured, but faster for this critical path */

	/*	If previous character was a newline, then we're starting a line.
	 */
	if (File.afterNL)
		fileNewline();

	c = getc(File.fp);
	switch (c)
	{
	default:
		if (escaped)
		{
			ungetc(c, File.fp);			/* return character after BACKSLASH */
			c = BACKSLASH;
		}
		break;

	case BACKSLASH:								/* test for line splicing */
		if (escaped)
			ungetc(c, File.fp);					/* push back one just read */
		else
		{
			escaped = TRUE;				/* defer test until next character */
			goto nextChar;
		}
		break;

	/*	The following cases turn line breaks into a canonical form. All
	 *	those below are turned into a generic line break (newline).
	 */
	case CRETURN:
		{
			const int next = getc(File.fp);		/* is CR followed by LF? */

			/*	If this is a carriage-return/line-feed pair, treat it as one
			 *	newline, throwing away the line-feed.
			 */
			if (next != NEWLINE)
				ungetc(next, File.fp);
		}
	case VTAB:
	case FORMFEED:
		c = NEWLINE;				/* turn those above into generic newline */
	case NEWLINE:
		File.afterNL = TRUE;
		if (escaped)						/* check for line splicing */
		{
#ifdef DEBUG
			debugPutc(BACKSLASH, 1);		/* print the characters */
			debugPutc(c, 1);				/*   we are throwing away */
#endif
			escaped = FALSE;				/* BACKSLASH now fully processed */
			goto nextChar;					/* through away "\NEWLINE" */
		}
		break;
	}

#ifdef DEBUG
	debugPutc(c, 1);
#endif
	return c;
}

/*----------------------------------------------------------------------------
*	Tag line creation functions	
*--------------------------------------------------------------------------*/

/*	This function copies the current line out some other fp.  It has no
 *	effect on the fileGetc() function.  During copying, any '\' characters
 *	are doubled and a leading '^' or trailing '$' is also quoted.  The '\n'
 *	character is not copied.  If the '\n' is preceded by a '\r', then the
 *	'\r' isn't copied.
 *
 *	This is meant to be used when generating a tag line.
 */
static size_t fileCopyLine( fp, seek )
	FILE *const fp;
	const long seek;
{
	const long oldseek = ftell(File.fp);	/* remember original position */
	size_t	length = 0;
	int		c;

	fseek(File.fp, seek, 0);
	c = getc(File.fp);

	/*	Write everything up to, but not including, the newline.
	 */
	while (c != NEWLINE  &&  c != EOF)
	{
		const int next = getc(File.fp);		/* preread next character */

		/*	If character is '\', or a terminal '$', then quote it.
		 */
		if (c == BACKSLASH  ||  c == (Option.backward ? '?' : '/')  ||
			(c == '$'  &&  next == NEWLINE))
		{
			putc(BACKSLASH, fp);
			++length;
		}

		/*	Copy the character, unless it is a terminal '\r'.
		 */
		if (c != CRETURN || next != NEWLINE)
		{
			putc(c, fp);
			++length;
		}
		c = next;
	}
	fseek(File.fp, oldseek, 0);				/* return to original position */
	return length;
}

static size_t fileCopyLineShort( fp, seek )
	FILE *const fp;		/* the output stream to copy it to */
	const long seek;	/* where the lines starts in the source file */
{
	const long oldseek = ftell(File.fp);	/* remember original position */
	boolean	lineStarted = FALSE;
	size_t	length = 0;
	int		c;

	fseek(File.fp, seek, 0);
	c = getc(File.fp);

	/*	Write everything up to, but not including, the newline.
	 */
	while (c != NEWLINE  &&  c != EOF)
	{
		int next = getc(File.fp);

		if (lineStarted  || ! isspace(c))		/* ignore leading spaces */
		{
			lineStarted = TRUE;
			if (isspace(c))
			{
				c = ' ';
				while (isspace(next)  &&  next != NEWLINE)
					next = getc(File.fp);
			}
			if (c != CRETURN  ||  next != NEWLINE)
			{
				putc(c, fp);
				++length;
			}
		}
		c = next;
	}
	fseek(File.fp, oldseek, 0);			/* seek back to the old position */
	return length;
}

/*----------------------------------------------------------------------------
 *	This section handles preprocessor directives.  It strips out all of the
 *	directives, and may emit a tag for #define directives.
 *--------------------------------------------------------------------------*/

static void writeXrefEntry( tag, tagType )
	const tag_info *const tag;
	const tag_t tagType;
{
	size_t length;

	length = fprintf(TagFile.fp, "%-20s %-6s %4ld  %-14s ",
					 tag->name, tagTypeName(tagType),
					 tag->lineNumber, File.name);
	length += fileCopyLineShort(TagFile.fp, tag->location);
	putc(NEWLINE, TagFile.fp);
	++length;

	++TagFile.numTags;
	if (strlen(tag->name) > TagFile.max.tag)
		TagFile.max.tag = strlen(tag->name);
	if (length > TagFile.max.line)
		TagFile.max.line = length;
}

/*	This function generates a tag for the object in name, whose tag line is
 *	located at a given seek offset.
 */
static void makeTagEntry( tag, scope, tagType, useLineNumber )
	const tag_info *const tag;
	const scope_t scope;
	const tag_t tagType;
	const boolean useLineNumber;
{
	boolean include = FALSE;

	if (scope != SCOPE_EXTERN)		/* should never happen */
		switch (tagType)
		{
		case TAG_BLOCKTAG:	include = Option.include.blockTags;		break;
		case TAG_DEFINE:	include = Option.include.defines;		break;
		case TAG_ENUM:		include = Option.include.enumValues;	break;
		case TAG_FUNCTION:	include = Option.include.functions;		break;
		case TAG_FUNCDECL:	include = Option.include.prototypes;	break;
		case TAG_TYPEDEF:	include = Option.include.typedefs;		break;
		case TAG_VARIABLE:	include = Option.include.variables;		break;

		default: break;
		}

	if (include)
	{
		if (Option.xref)
			writeXrefEntry(tag, tagType);
		else
		{
			size_t length = 0;

			if (Option.include.prefix  &&  scope == SCOPE_STATIC)
			{
				fputs(File.name, TagFile.fp);
				putc(':', TagFile.fp);
				length = strlen(File.name) + 1;
			}
			if (useLineNumber)
				length += fprintf(TagFile.fp, "%s\t%s\t%ld\n",
								  tag->name, File.name, tag->lineNumber);
			else
			{
				length += fprintf(TagFile.fp, "%s\t%s\t", tag->name, File.name);
				length += fprintf(TagFile.fp, "%c^",
								  Option.backward ? '?' : '/');
				length += fileCopyLine(TagFile.fp, tag->location);
				length += fprintf(TagFile.fp, "$%c\n",
								  Option.backward ? '?' : '/');
			}

			++TagFile.numTags;
			if (strlen(tag->name) > TagFile.max.tag)
				TagFile.max.tag = strlen(tag->name);
			if (length > TagFile.max.line)
				TagFile.max.line = length;
		}

#ifdef DEBUG
		debugLabel(tagType, tag->name);
#endif
	}
}

static void makeTag( tag, scope, tagType )
	const tag_info *const tag;
	const scope_t scope;
	const tag_t tagType;
{
	makeTagEntry(tag, scope, tagType, Option.lineNumbers);
}

static void makeDefineTag( tag, scope )
	const tag_info *const tag;
	const scope_t scope;
{
	makeTagEntry(tag, scope, TAG_DEFINE, TRUE);
}

/*----------------------------------------------------------------------------
*	Scanning functions	
*--------------------------------------------------------------------------*/

static boolean cppOpen( name )
	const char *const name;
{
	boolean opened;

	opened = fileOpen(name);
	if (opened)
	{
		Cpp.ungetch = 0;
		Cpp.directive.state   = DRCTV_NONE;
		Cpp.directive.accept  = TRUE;
		Cpp.directive.resolve = FALSE;
		Cpp.directive.level   = 0;
	}
	return opened;
}

static void cppClose()
{
	fileClose();
}

/*	This puts a character back into the input queue for the source File.
 */
static void cppUngetc( c )
	const int c;
{
	Cpp.ungetch = c;
}

static boolean cppReadDirective( c, name )
	int c;
	char *name;
{
	do
	{
		*name++ = c;
	} while (c = fileGetc(), (c != EOF  &&  isalpha(c)));
	fileUngetc(c);
	*name = '\0';										/* null terminate */

	return isspacetab(c);
}

static boolean cppReadIdentifier( c, tag )
	int c;
	tag_info *const tag;
{
	char *name = tag->name;

	do
	{
		*name++ = c;
	} while (c = fileGetc(), (c != EOF  &&  isident(c)));
	fileUngetc(c);
	*name = '\0';										/* null terminate */
	tag->location	= File.seek;
	tag->lineNumber	= File.lineNumber;

	return (isspace(c)  ||  c == '(');
}

static boolean pushCppIgnore( ignore, pathChosen )
	const boolean ignore;
	const boolean pathChosen;
{
	if (Cpp.directive.level < CPP_NESTING_LEVEL - 1)
	{
		++Cpp.directive.level;
		Cpp.directive.ifdef[Cpp.directive.level].ignore = ignore;
		Cpp.directive.ifdef[Cpp.directive.level].pathChosen = pathChosen;
	}
	return cppIgnore();
}

static boolean popCppIgnore()
{
	if (Cpp.directive.level > 0)
		--Cpp.directive.level;

	return cppIgnore();
}

static boolean prevCppIgnore()
{
	boolean ignore;

	if (Cpp.directive.level <= 0)
		ignore = FALSE;
	else
		ignore = Cpp.directive.ifdef[Cpp.directive.level - 1].ignore;
	return ignore;
}

static boolean setCppIgnore( ignore )
	const boolean ignore;
{
	return Cpp.directive.ifdef[Cpp.directive.level].ignore = ignore;
}

static boolean wasPathChosen()
{
	return Cpp.directive.ifdef[Cpp.directive.level].pathChosen;
}

static boolean handleDirective( c )
	const int c;
{
	char *const name = Cpp.directive.tag.name;
	scope_t scope = File.header ? SCOPE_GLOBAL : SCOPE_STATIC;
	boolean ignore = FALSE;

	switch (Cpp.directive.state)
	{
	case DRCTV_NONE:
		ignore = cppIgnore();
		break;		/* ignore characters until newline */

	case DRCTV_HASH:
		cppReadDirective(c, name);
		if (strcmp(name, "define") == 0)
			Cpp.directive.state = DRCTV_DEFINE;
		else if (strncmp(name, "if", (size_t)2) == 0)
			Cpp.directive.state = DRCTV_IF;
		else
		{
			if (strcmp(name, "endif") == 0)
				ignore = popCppIgnore();
			else if (Cpp.directive.resolve  &&  ! Option.braceFormat)
			{
				if (strcmp(name, "elif") == 0)
					ignore = setCppIgnore(TRUE);
				else if (strcmp(name, "else") == 0)
					ignore = setCppIgnore(prevCppIgnore() || wasPathChosen());
			}
			else
			{
				if (strcmp(name, "elif") == 0  ||
					strcmp(name, "else") == 0)
					ignore = setCppIgnore(FALSE);
			}
			Cpp.directive.state = DRCTV_NONE;
		}
		break;

	case DRCTV_DEFINE:
		cppReadIdentifier(c, &Cpp.directive.tag);
		if (File.header || Option.include.statics)
			makeDefineTag(&Cpp.directive.tag, scope);
		Cpp.directive.state = DRCTV_NONE;
		break;

	case DRCTV_IF:
		if (Option.braceFormat)
			ignore = pushCppIgnore(FALSE, TRUE);
		else if (c == '0')				/* special case: avoid first branch */
			ignore = pushCppIgnore(TRUE, FALSE);
		else
			ignore = pushCppIgnore(cppIgnore(), TRUE);
		Cpp.directive.state = DRCTV_NONE;
		break;
	}
	return ignore;
}

static Comment isComment()
{
	Comment comment;
	const int next = fileGetc();

	if (next == '*')
		comment = COMMENT_C;
	else if (next == '/')
		comment = COMMENT_CPLUS;
	else
	{
		fileUngetc(next);
		comment = COMMENT_NONE;
	}
	return comment;
}

static int skipOverCComment()
{
	int c = fileGetc();

	while (c != EOF)
	{
		if (c != '*')
			c = fileGetc();
		else
		{
			const int next = fileGetc();

			if (next != '/')					/* if end of comment */
				c = next;						/* could be another '*' */
			else
			{
				c = ' ';						/* replace comment with space */
				break;
			}
		}
	}
	return c;
}

static int skipOverCplusComment()
{
	int c;

	while ((c = fileGetc()) != EOF)
	{
		if (c == BACKSLASH)
			c = fileGetc();			/* throw away next character, too */
		else if (c == NEWLINE)
			break;
	}
	return c;
}

/*	ANSI C says a string may not contain a newline character.
 */
static int skipToEndOfString()
{
	int c;

	while ((c = fileGetc()) != EOF)
	{
		if (c == BACKSLASH)
			c = fileGetc();			/* throw away next character, too */
		else if (c == DOUBLE_QUOTE)
			break;
		else if (c == NEWLINE)
		{
			fileUngetc(c);
			break;
		}
	}
	return 'S';						/* symbolic representation of string */
}

static int skipToEndOfChar()
{
	int c;

	while ((c = fileGetc()) != EOF)
	{
		if (c == BACKSLASH)
			c = fileGetc();			/* throw away next character, too */
		else if (c == SINGLE_QUOTE)
			break;
		else if (c == NEWLINE)
		{
			fileUngetc(c);
			break;
		}
	}
	return 'C';						/* symbolic representation of character */
}

/*	This function returns the next character, stripping out comments,
 *	C pre-processor directives, and the contents of single and double
 *	quoted strings. In short, strip anything which places a burden upon
 *	the tokenizer.
 */
static int cppGetc()
{
	Comment comment = COMMENT_NONE;
	boolean directive = FALSE;
	boolean ignore = FALSE;
	int c;

	if (Cpp.ungetch != 0)
	{
		c = Cpp.ungetch;
		Cpp.ungetch = 0;
		return c;			/* return here to avoid re-calling debugPutc() */
	}
	else do
	{
		c = fileGetc();
		switch (c)
		{
		case EOF:
			ignore		= FALSE;
			directive	= FALSE;
			break;
		case TAB:
		case SPACE:
			break;							/* ignore most white space */
		case NEWLINE:
			if (directive  &&  ! ignore)
				directive = FALSE;
			Cpp.directive.accept = TRUE;
			break;
		case DOUBLE_QUOTE:
			Cpp.directive.accept = FALSE;
			c = skipToEndOfString();
			break;
		case '#':
			if (Cpp.directive.accept)
			{
				directive = TRUE;
				Cpp.directive.state = DRCTV_HASH;
				Cpp.directive.accept = FALSE;
			}
			break;
		case SINGLE_QUOTE:
			Cpp.directive.accept = FALSE;
			c = skipToEndOfChar();
			break;
		case '/':
			comment = isComment();
			if (comment == COMMENT_C)
				c = skipOverCComment();
			else if (comment == COMMENT_CPLUS)
			{
				c = skipOverCplusComment();
				if (c == NEWLINE)
					fileUngetc(c);
			}
			else
				Cpp.directive.accept = FALSE;
			break;
		default:
			Cpp.directive.accept = FALSE;
			if (directive)
				ignore = handleDirective(c);
			break;
		}
	} while (directive || ignore);

#ifdef DEBUG
	debugPutc(c, 2);
#endif
	return c;
}

/*----------------------------------------------------------------------------
 *	Parsing functions
 *--------------------------------------------------------------------------*/

/*	Skip to the next non-white character.
 */
static int skipToNonWhite()
{
	int c;

	do
	{
		c = cppGetc();
	} while (c != EOF  &&  isspace(c));

	return c;
}

/*	Skip to the next occurance of the specified character.
 */
static int skipToCharacter( findchar )
	const int findchar;
{
	int c;

	do
		c = cppGetc();
	while (c != EOF  &&  c != findchar);

	return c;
}

/*	Skips to the next brace in column 1. This is intended for cases where
 *	preprocessor constructs result in unbalanced braces.
 */
static void skipToFormattedBraceMatch()
{
	int c, next;

	c = cppGetc();
	next = cppGetc();
	while (c != EOF  &&  (c != '\n'  ||  next != '}'))
	{
		c = next;
		next = cppGetc();
	}
}

/*	Skip to the matching character indicated by the pair string. If skipping
 *	to a matching brace and any brace is found within a different level of a
 *	#if conditional statement while brace formatting is in effect, we skip to
 *	the brace matched by its formatting.
 */
static boolean skipToMatch( pair )
	const char *const pair;
{
	const int begin = pair[0], end = pair[1];
	const int initialLevel = Cpp.directive.level;
	const boolean braceFormatting =(Option.braceFormat && strcmp("{}",pair)==0);
	boolean ok = TRUE;
	int matchLevel = 1;
	int c = 0;

	while (matchLevel > 0  &&  (c = cppGetc()) != EOF)
	{
		if (c == begin)
		{
			++matchLevel;
			if (braceFormatting  &&  Cpp.directive.level != initialLevel)
			{
				skipToFormattedBraceMatch();
				break;
			}
		}
		else if (c == end)
		{
			--matchLevel;
			if (braceFormatting  &&  Cpp.directive.level != initialLevel)
			{
				skipToFormattedBraceMatch();
				break;
			}
		}
	}
	if (c == EOF)
		ok = FALSE;

	return ok;
}

/*	Skips over any initializing value which may follow a '=' character in a
 *	variable definition.
 */
static int skipInitializer( inEnumBody )
	const boolean inEnumBody;
{
	boolean done = FALSE;
	int c;

	while (! done  &&  (c = cppGetc()) != EOF)
	{
		switch (c)
		{
		default:	break;
		case ',':
		case ';':	done = TRUE; break;

		case '[':	if (! skipToMatch("[]"))
						c = EOF;
					break;
		case '(':	if (! skipToMatch("()"))
						c = EOF;
					break;
		case '{':	if (! skipToMatch("{}"))
						c = EOF;
					break;
		case '}':	if (inEnumBody)
						done = TRUE;
					else if (! Option.braceFormat)
						c = EOF;
					break;
		}
	}
	return c;
}

/*	Read a C identifier beginning with "firstChar" and places it into
 *	"name".
 */
static void readIdendifier( firstChar, name )
	const int firstChar;
	char *const name;
{
	int c, i;

	name[0] = firstChar;
	for (i = 1, c = cppGetc() ; i < MAXNAME - 1 && isident(c) ;
		 i++, c = cppGetc())
	{
		name[i] = c;
	}
	name[i] = '\0';				/* null terminate name */

	cppUngetc(c);				/* unget non-identifier character */
}

static int skipParameterDeclarations( c )
	int c;
{
	while (c != EOF  &&  c != ';'  &&  c != ','  &&  c != '{')
	{
		/*	Check to see if there are old-style parameter
		 *	declarations following the parentheses.
		 */
		if (isident1(c))
		{
			do
			{
				if (c == '{'  &&  ! skipToMatch("{}"))
					c = EOF;
				else
					c = cppGetc();
			} while (c != EOF  &&  c != ';');
		}
		if (c != EOF)
			c = skipToNonWhite();
	}
	return c;
}

static boolean analyzeParens( st )
	statement_t *const st;
{
	boolean ok = TRUE;
	int c;

	c = skipToNonWhite();
	if (c == '*')							/* this is a function pointer */
	{
		st->gotName	= FALSE;				/* invalidate previous name */
		st->funcPtr = TRUE;
		st->token	= TOK_IGNORE;
	}
	else
	{
		char name[MAXNAME];
		boolean gotName = FALSE;
		long location = 0, lineNumber = 0;

#ifdef DEBUG
		clearString(name, MAXNAME);
#endif
		if (isident1(c))
		{
			readIdendifier(c, name);
			gotName		= TRUE;
			location	= File.seek;
			lineNumber	= File.lineNumber;
		}
		else
		{
			/*	A double paren almost certainly means one of those conditional
			 *	prototype macro thingies (e.g. __ARGS((void)) ). If found, we
			 *	will use the previous name, if it is not empty.
			 */
			if (c == '('  &&  st->gotName  &&  *st->tag[!st->buf1].name != 0)
				st->buf1 = !st->buf1;				/* swap name buffers */
			cppUngetc(c);
		}
		ok = skipToMatch("()");
		if (ok)
		{
			/*	At this point we should be at the character following the
			 *	closing parenthesis.
			 */
			c = skipToNonWhite();
			if (st->gotName  &&  strchr("{;,", c) != NULL)
			{
				st->token = TOK_ARGS;	/* the parameter list to a function */
				st->declaration = DECL_UNSPEC;	/* clear any other decl. */
			}
			else if (isident1(c))
			{
				st->token = TOK_ARGS;	/* the parameter list to a function */
				st->declaration = DECL_UNSPEC;	/* clear any other decl. */
				c = skipParameterDeclarations(c);
			}
			/*	The name inside the parentheses must have been a function or
			 *	variable name.
			 */
			else if (gotName)
			{
				tag_info *const tag = &st->tag[st->buf1];

				st->gotName		= TRUE;
				st->token		= TOK_NAME;
				tag->location	= location;
				tag->lineNumber	= lineNumber;
				strcpy(tag->name, name);
			}
			cppUngetc(c);
		}
	}
	return ok;
}

static boolean isIgnoreToken( name )
	const char *const name;
{
	boolean ignore = FALSE;

	if (IgnoreList != NULL)
	{
		char **p;

		for (p = IgnoreList ; *p != NULL ; ++p)
		{
			if (strcmp(*p, name) == 0)
			{
				ignore = TRUE;
				break;
			}
		}
	}
	return ignore;
}

/*	Analyzes the identifier contained in a statement described by the
 *	statement structure and adjusts the structure according the significance
 *	of the identifier.
 */
static void analyzeIdentifier( st )
	statement_t *const st;
{
	tag_info *const tag = &st->tag[st->buf1];
	const char *const name = tag->name;
	boolean keyWord = FALSE;

#define match(word)	((strcmp(name,(word)) == 0) && (keyWord = TRUE))

	st->token = TOK_SPEC;		/* default unless otherwise */

	if (isIgnoreToken(name))
	{
		keyWord = TRUE;
		st->token = TOK_IGNORE;
	}
	else switch (name[0])		/* is it a reserved word? */
	{
	case 'c':
		     if (match("class"	))	st->declaration = DECL_CLASS;
		else if (match("const"	))	st->token		= TOK_IGNORE;
		else if (match("char"	))	st->declaration = DECL_UNSPEC;
		break;
	case 'd':if (match("double"	))	st->declaration = DECL_UNSPEC;	break;
	case 'e':
		     if (match("enum"	))	st->declaration = DECL_ENUM;
		else if (match("extern"	))	st->scope		= SCOPE_EXTERN;
		break;
	case 'f':if (match("float"	))	st->declaration = DECL_UNSPEC;	break;
	case 'i':if (match("int"	))	st->declaration = DECL_UNSPEC;	break;
	case 'l':if (match("long"	))	st->declaration = DECL_UNSPEC;	break;
	case 'p':if (match("private"))	st->scope		= SCOPE_STATIC;	break;
	case 's':
		     if (match("static"	))	st->scope		= SCOPE_STATIC;
		else if (match("struct"	))	st->declaration = DECL_STRUCT;
		else if (match("short"	))	st->declaration = DECL_UNSPEC;
		else if (match("signed"	))	st->declaration = DECL_UNSPEC;
		break;
	case 't':if (match("typedef"))	st->scope		= SCOPE_TYPEDEF; break;
	case 'u':
		     if (match("union"	))	st->declaration = DECL_UNION;
		else if (match("unsigned"))	st->declaration = DECL_UNSPEC;
		break;
	case 'v':
		     if (match("void"	))	st->declaration = DECL_UNSPEC;
		else if (match("volatile"))	st->token		= TOK_IGNORE;
		break;
	}

	if (! keyWord)
	{
		st->token		= TOK_NAME;
		st->gotName		= TRUE;
		tag->location	= File.seek;
		tag->lineNumber	= File.lineNumber;
	}
}

/*	Reads characters from the pre-processor and assembles tokens, setting
 *	the current statement state.
 */
static boolean nextToken( st, nesting )
	statement_t *const st;
	const int nesting;
{
	int	c;
	boolean	ok = TRUE;

	do
	{
		c = cppGetc();

		switch (c)
		{
		case ',':	st->token = TOK_COMMA;		break;
		case ';':	st->token = TOK_SEMICOLON;	break;
		case ':':
			if (st->declaration != DECL_CLASS)
				st->token = TOK_IGNORE;
			else
			{
				c = skipToCharacter('{');		/* skip over intervening junk */
				cppUngetc(c);
			}
			break;
		case '*':
			st->gotName = FALSE;
			break;
		case '(':
			ok = analyzeParens(st);
			break;

		case '{':
			if (st->declaration == DECL_ENUM)
			{
				st->inEnumBody = TRUE;
				st->token = TOK_BODY;
			}
			else
			{
				if (st->declaration == DECL_STRUCT  ||
					st->declaration == DECL_UNION   ||
					st->declaration == DECL_CLASS)
				{
					ok = createTags(nesting + 1);
				}
				else
				{
					ok = skipToMatch("{}");
				}
				st->token = TOK_BODY;
			}
			break;

		case '}':
			if (st->inEnumBody)
			{
				st->inEnumBody = FALSE;
				st->token = TOK_ENUM_BODY_END;
			}
			else if (nesting > 0)
			{
				st->token = TOK_EOF;		/* fake out */
				return TRUE;
			}
			else
			{
				st->token = TOK_IGNORE;
				ok = FALSE;
			}
			break;

		case '[':
			ok = skipToMatch("[]");
			st->token = TOK_IGNORE;
			break;

		case '=':
			c = skipInitializer(st->inEnumBody);
			if (c == EOF)
				ok = FALSE;
			else if (c == ';')
				st->token = TOK_SEMICOLON;
			else if (c == ',')
				st->token = TOK_COMMA;
			else if (c == '}'  &&  st->inEnumBody)
				st->token = TOK_ENUM_BODY_END;
			break;

		case EOF:
			st->token = TOK_EOF;
			break;

		default:
			if (! isident1(c))
				st->token = TOK_IGNORE;
			else							/* start of a name or keyword */
			{
				if (st->gotName)
					st->buf1 = !st->buf1;				/* swap name buffers */
				readIdendifier(c, st->tag[st->buf1].name);
				analyzeIdentifier(st);
				if (st->token == TOK_IGNORE)
					st->buf1 = !st->buf1;				/* swap name buffers */
			}
		}
	} while (ok  &&  st->token == TOK_IGNORE);

	return ok;
}

/*----------------------------------------------------------------------------
 *	Tag generation functions
 *--------------------------------------------------------------------------*/

static void initStatement( st )
	statement_t *const st;
{
	int i;

	st->scope			= SCOPE_GLOBAL;
	st->declaration		= DECL_UNSPEC;
	st->token			= TOK_SEMICOLON;
	st->prev[0]			= TOK_SEMICOLON;
	st->prev[1]			= TOK_SEMICOLON;
	st->gotName			= FALSE;
	st->funcPtr			= FALSE;
	st->inEnumBody		= FALSE;
	st->buf1			= FALSE;

	for (i = 0 ; i < 2 ; ++i)
	{
		tag_info *const tag = &st->tag[i];

		tag->location	= 0;
		tag->lineNumber	= 0;
		tag->name[0]	= '\0';
#ifdef DEBUG
		clearString(tag->name, MAXNAME);
#endif
	}
}

/*	Parses the current file and decides whether to write out and tags that
 *	are discovered.
 */
static boolean createTags( nesting )
	const int nesting;
{
	statement_t st;
	boolean ok = TRUE;

#ifdef DEBUG
	if (nesting > 0  &&  (Option.debugLevel & 3) != 0)
		printf("<#++nesting:%d#>", nesting);
#endif
	initStatement(&st);

	while ((ok = nextToken(&st, nesting)))
	{
		tag_info *const tag = &st.tag[st.buf1];

		if (st.token == TOK_EOF)
			break;

		/*	If NAME BODY, then NAME is possibly an enum/struct/union tag or
		 *	new type (class/enum/struct/union in C++).
		 */
		if (st.token == TOK_BODY  &&  st.prev[0] == TOK_NAME)
		{
			if (st.declaration == DECL_CLASS  ||
				st.declaration == DECL_ENUM   ||
				st.declaration == DECL_STRUCT ||
				st.declaration == DECL_UNION)
			{
				if (File.header)
					makeTag(tag, SCOPE_GLOBAL, TAG_BLOCKTAG);
				else if (Option.include.statics)
					makeTag(tag, SCOPE_STATIC, TAG_BLOCKTAG);
			}
		}

		/*	If ENUM (BODY_END || (inEnumBody && NAME COMMA)), then any
		 *	previous NAME is an enumeration value.
		 */
		else if (st.token == TOK_ENUM_BODY_END  ||
				 (st.inEnumBody  &&  st.token == TOK_COMMA))
		{
			if (st.prev[0] == TOK_NAME)
			{
				if (File.header)
					makeTag(tag, SCOPE_GLOBAL, TAG_ENUM);
				else if (Option.include.statics)
					makeTag(tag, SCOPE_STATIC, TAG_ENUM);
			}
		}
		else if (nesting == 0)
		{
			/*	If NAME ARGS BODY, then NAME is a function (ANSI style).
			 */
			if (st.token == TOK_BODY  &&  st.prev[0] == TOK_ARGS  && st.gotName)
			{
				if (st.scope != SCOPE_STATIC  ||  Option.include.statics)
				{
					if (st.scope == SCOPE_EXTERN)  /* allowed for func. def. */
						st.scope = SCOPE_GLOBAL;
					makeTag(tag, st.scope, TAG_FUNCTION);
				}
			}

			/*	If SEMICOLON or COMMA, then NAME is either a variable,
			 *	typedef, or function declaration;
			 */
			else if (st.token == TOK_SEMICOLON  ||  st.token == TOK_COMMA)
			{
				if (st.prev[0] == TOK_NAME  ||  (st.funcPtr  &&  st.gotName))
				{
					if (st.scope == SCOPE_TYPEDEF)
					{
						if (File.header)
							makeTag(tag, SCOPE_GLOBAL, TAG_TYPEDEF);
						else if (Option.include.statics)
							makeTag(tag, SCOPE_STATIC, TAG_TYPEDEF);
					}
					/*	We have to watch that we do not interpret a
					 *	declaration of the form "struct tag;" as a variable
					 *	definition. In such a case, the declaration will be
					 *	either class, enum, struct or union, and prev[1] will
					 *	be empty (i.e. SEMICOLON).
					 */
					else if (st.declaration == DECL_UNSPEC  ||
							 st.prev[1] != TOK_SPEC)
					{
						if (st.scope == SCOPE_GLOBAL || Option.include.statics)
							makeTag(tag, st.scope, TAG_VARIABLE);
					}
				}
				else if (st.prev[0] == TOK_ARGS)
				{
					if (File.header  &&
						(st.scope == SCOPE_GLOBAL || st.scope == SCOPE_EXTERN))
					{
						makeTag(tag, SCOPE_GLOBAL, TAG_FUNCDECL);
					}
				}
			}
		}

		/*	Reset after a semicolon or ARGS BODY pair.
		 */
		if (st.token == TOK_SEMICOLON  ||
			(st.token == TOK_BODY  &&  st.prev[0] == TOK_ARGS))
		{
			initStatement(&st);
			Cpp.directive.resolve = FALSE;		/* end of statement */
		}
		else
			Cpp.directive.resolve = TRUE;		/* in middle of statement */

	st.prev[1] = st.prev[0];
	st.prev[0] = st.token;
	}
#ifdef DEBUG
	if (nesting > 0  &&  (Option.debugLevel & 3) != 0)
		printf("<#--nesting:%d#>", nesting - 1);
#endif
	return ok;
}

static boolean createTagsForFile( name )
	const char *const name;
{
	boolean ok = TRUE;

	if (cppOpen(name))
	{
		ok = createTags(0);
		cppClose();
	}
	return ok;
}

/*	Checks whether the supplied filename contains something other than valid
 *	tag lines. A tag line MUST fit the following regular expression:
 *
 *	^\([^\t]\+:\)[a-zA-Z_][a-zA-Z0-9_]*\t[^\t]\t[/?].*[/?]$
 *
 *  or, in more readable terms:
 *
 *	[filename:]<tag>	<filename>	/pattern/
 */
static boolean isSourceFile( filename )
	const char *const filename;
{
	static char line[MAXLINE];
	boolean isSource = FALSE;		/* we assume not until found otherwise */
	FILE *const fp = fopen(filename, "r");

	if (fp != NULL  &&  fgets(line, MAXLINE, fp) != NULL)
	{
		char *tagPrefix, *tagFname, *pattern;
		char *separator1, *separator2;
		char *const buffer = malloc((size_t)(5 * MAXLINE));

		if (buffer == NULL)
		{
			fprintf(stderr, "Insufficient memory\n");
			exit(1);
		}
		tagPrefix	= &buffer[0 * MAXLINE];
		separator1	= &buffer[1 * MAXLINE];
		tagFname	= &buffer[2 * MAXLINE];
		separator2	= &buffer[3 * MAXLINE];
		pattern		= &buffer[4 * MAXLINE];
	    if (sscanf(line, "%[^\t]%[\t]%[^\t]%[\t]%[^\n]",
				   tagPrefix, separator1, tagFname, separator2, pattern) < 5)
			isSource = TRUE;
		else if (strlen(separator1) != 1  ||  strlen(separator2) != 1)
			isSource = TRUE;		/* exactly one tab permitted as separators*/
		else
		{
			/*	Check for filename:tag format.
			 */
			char *tag = strchr(tagPrefix, ':');

			if (tag == NULL)
			{
				tag = tagPrefix;
				tagPrefix = NULL;		/* no tag filename prefix */
			}
			else
			{
				*tag = 0;				/* null terminate the tag prefix */
				++tag;					/* tag follows tag prefix */

				/*	The filename portion of the prefixed tags must match
				 *	the supplied filename.
				 */
				if (strcmp(tagPrefix, tagFname) != 0)
					isSource = TRUE;
			}
			if (! isSource)
			{
				const char *p;

				/*	The tag must be a valid C identifier.
				 */
				if (! isident1(tag[0]))
					isSource = TRUE;
				else for (p = tag + 1 ; *p != 0 && !isSource ; ++p)
				{
					if (! isident(*p))
						isSource = TRUE;
				}

				/*	If the pattern does not begin with either '/' or '?',
				 *	or the first character and last characters do not match,
				 *	then the pattern must be a line number.
				 */
				if (! isSource  &&
					((pattern[0] != '/'  &&  pattern[0] != '?') ||
					  pattern[0] != pattern[strlen(pattern) - 1]))
				{
					for (p = pattern ; *p != 0  && ! isSource ; ++p)
						if (! isdigit(*p))
							isSource = TRUE;
				}
			}
		}
		free(buffer);
	}
	if (fp != NULL)
		fclose(fp);
	return isSource;
}

static const char *getNextListFile( fp )
	FILE *const fp;
{
	static char fileName[MAXPATH];
	const char *const buf = fgets(fileName, MAXPATH, fp);

	if (buf != NULL)
	{
		char *const newline = strchr(fileName, '\n');

		if (newline == NULL)
			fileName[MAXPATH - 1] = 0;
		else
			*newline = 0;
	}
	return buf;
}

/*	Create tags for the source files listed in the file specified by
 *	"listFile".
 */
static void createTagsForList( listFile )
	const char *const listFile;
{
	const char *fileName;
	FILE *const fp = (strcmp(listFile,"-") == 0) ? stdin : fopen(listFile, "r");

	if (fp == NULL)
	{
		perror(listFile);
		exit(1);
	}
	else
	{
		fileName = getNextListFile(fp);
		while (fileName != NULL)
		{
			const long startPos = ftell(TagFile.fp);
			const size_t numTags = TagFile.numTags;

			if (! createTagsForFile(fileName)  &&  ! Option.braceFormat)
			{
				fseek(TagFile.fp, startPos, 0);
				TagFile.numTags = numTags;			/* restore previous count */
				Option.braceFormat = TRUE;
#ifdef DEBUG
				if (Option.debugLevel & 4)
					printf("%s: Formatting error; retrying\n", fileName);
#endif
			}
			else
			{
				Option.braceFormat = FALSE;
				fileName = getNextListFile(fp);
			}
		}
		if (fp != stdin)
			fclose(fp);
	}
}

static void createTagsForArgs( argList )
	char *const *const argList;
{
	int argNum = 0;

	/*	Generate tags for each source file on the command line.
	 */
	while (argList[argNum] != NULL)
	{
		const long startPos = ftell(TagFile.fp);
		const size_t numTags = TagFile.numTags;

		if (! createTagsForFile(argList[argNum])  &&  ! Option.braceFormat)
		{
			fseek(TagFile.fp, startPos, 0);
			TagFile.numTags = numTags;			/* restore previous count */
			Option.braceFormat = TRUE;
#ifdef DEBUG
			if (Option.debugLevel & 4)
				printf("%s: Formatting error; retrying\n", argList[argNum]);
#endif
		}
		else
		{
			++argNum;
			Option.braceFormat = FALSE;
		}
	}
}

/*----------------------------------------------------------------------------
*	Tag sorting functions
*--------------------------------------------------------------------------*/

#ifdef INTERNAL_SORT
/*----------------------------------------------------------------------------
 *	These functions provide a basic internal sort. No great memory
 *	optimization is performed (e.g. recursive subdivided sorts),
 *	so have lots of memory if you have large tag files.
 *--------------------------------------------------------------------------*/

static void fatal( msg )
	const char *const msg;
{
	if (TagFile.fp != NULL)
		fclose(TagFile.fp);
	perror(msg);
	fprintf(stderr, "Unable to sort tag file.\n");
	exit(1);
}

/*	This is the sorting function called by qsort().
 */
static int compareTags( one, two )
	const void *const one;
	const void *const two;
{
	const char *const line1 = *(const char *const *const)one;
	const char *const line2 = *(const char *const *const)two;

	return strcmp(line1, line2);
}

static void writeSortedTags( table, toStdout )
	char **const table;
	const boolean toStdout;
{
	char *thisTag = NULL, *prevTag = NULL, *thisFile = NULL, *prevFile = NULL;
	size_t i;

	if (Option.warnings)
	{
		thisTag  = malloc(TagFile.max.tag  + (size_t)1);
		prevTag  = malloc(TagFile.max.tag  + (size_t)1);
		thisFile = malloc(TagFile.max.file + (size_t)1);
		prevFile = malloc(TagFile.max.file + (size_t)1);

		if (thisTag  == NULL  ||   prevTag == NULL  ||
			thisFile == NULL  ||  prevFile == NULL)
		{
			fprintf(stderr,
					"Insufficient memory to generate duplicate warnings\n");
		}
		*prevTag  = *thisTag  = 0;
		*prevFile = *thisFile = 0;
	}

	/*	Write the sorted lines back into the tag file.
	 */
	if (toStdout)
		TagFile.fp = stdout;
	else
	{
		TagFile.fp = fopen(TagFile.name, "w");
		if (TagFile.fp == NULL)
			fatal(TagFile.name);
	}
	for (i = 0 ; i < TagFile.numTags ; ++i)
	{
		/*	Here we filter out identical tag *lines* (including search
		 *	pattern) if this is not an xref file.
		 */
		if (i == 0  ||  Option.xref  ||  strcmp(table[i], table[i-1]) != 0)
			if (fputs(table[i], TagFile.fp) == EOF)
				fatal(TagFile.name);

		if (Option.warnings)
		{
			int fields;

			if (Option.xref)
				fields = sscanf(table[i],"%s %*s %*s %s", thisTag, thisFile);
			else
				fields = sscanf(table[i],"%[^\t]\t%[^\t]", thisTag, thisFile);

			if (fields == 2  &&  strcmp(thisTag, prevTag) == 0)
			{
				fprintf(stderr, "Duplicate entry in ");
				if (strcmp(thisFile, prevFile) != 0)
					fprintf(stderr, "%s and ", prevFile);
				fprintf(stderr, "%s: %s\n", thisFile, thisTag);
			}
			strcpy(prevTag , thisTag );
			strcpy(prevFile, thisFile);
		}
	}
	if (Option.warnings)
	{
		free(thisTag);
		free(prevTag);
		free(thisFile);
		free(prevFile);
	}
	if (! toStdout)
		fclose(TagFile.fp);
}

static void internalSortTags(toStdout)
	const boolean toStdout;
{
	char *buffer, **table;
	const size_t maxLength = TagFile.max.line + 1;	/* include room for null */
	size_t tableSize, mallocSize;
	size_t i;

	/*	Allocate the memory buffer for the tag file lines. This includes a
	 *	table of line pointers which will be sorted.
	 */
	tableSize  = TagFile.numTags * sizeof(*table);
	buffer = malloc(maxLength);						/* read buffer */
	table  = malloc(tableSize);
	if (buffer == NULL  ||  table == NULL)
		fatal("ctags");

	mallocSize = maxLength + tableSize;

	/*	Open the tag file and place its lines into allocated buffers.
	 */
	TagFile.fp = fopen(TagFile.name, "r");
	if (TagFile.fp == NULL)
		fatal(TagFile.name);
	for (i = 0 ; i < TagFile.numTags && !feof(TagFile.fp) ; ++i)
	{
		size_t stringSize;

		if (fgets(buffer, (int)maxLength, TagFile.fp) == NULL  &&
															! feof(TagFile.fp))
			fatal(TagFile.name);
		stringSize = strlen(buffer) + 1;
		table[i] = (char *)malloc(stringSize);
		if (table[i] == NULL)
			fatal(TagFile.name);
		mallocSize += stringSize;
		strcpy(table[i], buffer);
	}
#ifdef DEBUG
	assert(i == TagFile.numTags);		/* this should always be the case */
#endif
	fclose(TagFile.fp);

	/*	Sort the lines.
	 */
	qsort(table, TagFile.numTags, sizeof(*table),
#ifndef USING_PROTOTYPES
		  (int (*)())
#endif
		  compareTags);

	writeSortedTags(table, toStdout);

#ifdef DEBUG
	if (Option.debugLevel & 4)
		printf("malloc: %ld bytes\n", (long)mallocSize);
#endif
	for (i = 0 ; i < TagFile.numTags ; ++i)
		free(table[i]);
	free(table);
	free(buffer);
}

#else

static void externalSortTags(toStdout)
	const boolean toStdout;
{
	static char cmd[MAXLINE];
	int ret;

	sprintf(cmd, "sort -u -o %s %s", TagFile.name, TagFile.name);
	ret = system(cmd);
	if (ret == 0  &&  Option.warnings)
	{
		char awkProg[MAXLINE];
		const char *begin;
		int fileArg;

		if (Option.xref)
		{
			begin = "";
			fileArg = 4;
		}
		else
		{
			begin = "BEGIN{FS=\"\\t\"}";
			fileArg = 2;
		}
		sprintf(awkProg, "%s{if ($1==prev){printf(\"Duplicate entry in \") | \"cat>&2\"; if ($%d!=prevfile) printf(\"%%s and \",prevfile) | \"cat>&2\"; printf(\"%%s: %%s\\n\",$%d,$1) | \"cat>&2\"; } else {prev=$1;prevfile=$%d}}", begin, fileArg, fileArg, fileArg);
		sprintf(cmd, "awk '%s' %s", awkProg, TagFile.name);
		ret = system(cmd);
	}
	if (toStdout)
	{
		sprintf(cmd, "cat %s", TagFile.name);
		ret = system(cmd);
	}
}

#endif

/*----------------------------------------------------------------------------
 *	Start up and terminate functions
 *--------------------------------------------------------------------------*/

static void printHelp( help, where )
	const char *const *const help;
	FILE *const where;
{
	int i;

	fprintf(where, "%s", VERSION);
	for (i = 0 ; Features[i] != NULL ; ++i)
	{
		if (i == 0)
			fputs(" (", where);
		fprintf(where, "%s+%s", (i>0 ? ", " : ""), Features[i]);
	}
	fprintf(where, "%s\n", (i>0 ? ")" : ""));
	fprintf(where, "%s\n", INVOCATION);
	for (i = 0 ; help[i] != NULL ; ++i)
	{
		fputs(help[i], where);
		fputc('\n', where);
	}
}

static void printUsage( error )
	const char *const error;
{
	printHelp(Help, stderr);
	if (error != NULL)
		fprintf(stderr, "\nError: %s\n\n", error);
	exit(1);
}

/*	Reads a list of header file extensions.
 */
static void readExtensionList( list )
	char *const list;
{
	const char *extension = strtok(list, HEADER_SEPARATORS);
	int extIndex = 0;

	while (extension != NULL  &&  extIndex < MAX_HEADER_EXTENSIONS)
	{
#ifdef DEBUG
		if (Option.debugLevel & 4)
			printf("header extension: %s\n", extension);
#endif
		Option.headerExt[extIndex++] = extension;
		extension = strtok(NULL, HEADER_SEPARATORS);
	}
	Option.headerExt[extIndex] = NULL;
}

static void clearTagList()
{
	Option.include.defines		= FALSE;
	Option.include.enumValues	= FALSE;
	Option.include.functions	= FALSE;
	Option.include.blockTags	= FALSE;
	Option.include.prototypes	= FALSE;
	Option.include.typedefs		= FALSE;
	Option.include.variables	= FALSE;
	Option.include.prefix		= FALSE;
	Option.include.statics		= FALSE;
}

static void applyTagInclusionList( list )
	const char *const list;
{
	boolean mode = TRUE;		/* default mode is to add following types */
	const char *p;

	for (p = list  ;  *p != 0  ;  ++p)
	{
		switch (*p)
		{
		case '=':		/* exclusive mode; ONLY types following are included */
			clearTagList();
			mode = TRUE;
			break;
		case '+': mode = TRUE;	break;		/* types following are included */
		case '-': mode = FALSE;	break;		/* types following are excluded */

		case 'd': Option.include.defines	= mode;		break;
		case 'e': Option.include.enumValues	= mode;		break;
		case 'f': Option.include.functions	= mode;		break;
		case 'g': Option.include.blockTags	= mode;		break;
		case 'p': Option.include.prototypes	= mode;		break;
		case 't': Option.include.typedefs	= mode;		break;
		case 'v': Option.include.variables 	= mode;		break;
		case 'P': Option.include.prefix		= mode;		break;
		case 'S': Option.include.statics	= mode;		break;

		default:
			{
				char msg[80];

				sprintf(msg, "-i: Invalid tag option '%c'", *p);
				printUsage(msg);
				break;
			}
		}
	}
}

static void readIgnoreList( fileName )
	const char *const fileName;
{
	FILE *const fp = fopen(fileName, "r");

	if (fp == NULL)
	{
		perror(fileName);
		exit(1);
	}
	else
	{
		char ignoreToken[MAXNAME];
		int i, numTokens = 0;

		while (fscanf(fp, "%255s", ignoreToken) == 1)
			++numTokens;
		rewind(fp);
		IgnoreList = (char **)malloc((numTokens + 1) * sizeof(char *));
		if (IgnoreList == NULL)
			perror("Cannot create ignore list");
		else
		{
			for (i = 0 ; fscanf(fp, "%255s", ignoreToken) == 1 ; ++i)
			{
				IgnoreList[i] = (char *)malloc(strlen(ignoreToken) + 1);
				strcpy(IgnoreList[i], ignoreToken);
			}
			IgnoreList[i] = NULL;
		}
	}
}

static void freeIgnoreList()
{
	if (IgnoreList != NULL)
	{
		int i;

		for (i = 0 ; IgnoreList[i] != NULL ; ++i)
			free(IgnoreList[i]);
		free(IgnoreList);
	}
}

static char *readOptionArg(option, arg, argList, argNum)
	const int option;
	char **const arg;
	char *const *const argList;
	int *const argNum;
{
	char *list = NULL;

	if ((*arg)[0] != 0)			/* does list immediately follow option? */
	{
		list = *arg;
		*arg += strlen(*arg);
	}
	else if ((list = argList[++(*argNum)]) == NULL)	/* at least 1 more arg? */
	{
		char msg[80];

		sprintf(msg, "-%c: Parameter missing", option);
		printUsage(msg);
	}
	return list;
}

static int parseOptions( argList )
	char *const *const argList;
{
	int	argNum;

	/*	Parse the option flags.
	 */
	for (argNum = 0  ;  argList[argNum] != NULL  ;  ++argNum)
	{
		char c;
		char *arg = argList[argNum];
		char *param = NULL;

		if (*arg++ != '-')				/* stop at first non-option switch */
		  break;
		while (*arg != 0)  switch (c = *arg++)
		{
			/*	Simple options.
			 */
			case 'a': Option.append				= TRUE;		break;
			case 'B': Option.backward			= TRUE;		break;
			case 'd': Option.include.defines	= TRUE;		break;
			case 'F': Option.backward			= FALSE;	break;
			case 'n': Option.lineNumbers		= TRUE;		break;
			case 's': Option.include.statics	= TRUE;
					  Option.include.prefix		= TRUE;		break;
			case 'S': Option.include.statics	= TRUE;
					  Option.include.prefix		= FALSE;	break;
			case 't': Option.include.typedefs	= TRUE;		break;
			case 'T': Option.include.blockTags	= TRUE;
					  Option.include.typedefs	= TRUE;		break;
			case 'u': Option.unsorted			= TRUE;		break;
			case 'w': Option.warnings			= FALSE;	break;
			case 'W': Option.warnings			= TRUE;		break;
			case 'x': Option.xref				= TRUE;		break;
			case '?': printHelp(LongHelp, stdout);			exit(0);

			/*	Options requiring parameters.
			 */
			case '-':
				if (strcmp(arg,"help") == 0)
				{
					printHelp(LongHelp, stdout);
					exit(0);
				}
				break;

			case 'f':
			case 'o':
				Option.tagFile = readOptionArg(c, &arg, argList, &argNum);
				break;

			case 'h':
				{
					struct stat status;

					param = readOptionArg(c, &arg, argList, &argNum);

					/*	Check to make sure that the user did not enter
					 *	"ctags -h *.c" by testing to see if the list is
					 *	a filename that exists.
					 */
					if (stat(param, &status) == 0)
						printUsage("-h: Invalid list");
					else
						readExtensionList(param);
				}
				break;

			case 'i':
				param = readOptionArg(c, &arg, argList, &argNum);
				applyTagInclusionList(param);
				break;

			case 'I':
				Option.ignoreList = readOptionArg(c, &arg, argList, &argNum);
				break;

			case 'L':
				Option.fileList = readOptionArg(c, &arg, argList, &argNum);
				break;

#ifdef DEBUG
			case 'D':
				param = readOptionArg(c, &arg, argList, &argNum);
				Option.debugLevel = atoi(param);
				break;
			case 'N':
				param = readOptionArg(c, &arg, argList, &argNum);
				Option.breakLine = atol(param);
				break;
#endif
			default:
				{
				char msg[80];

				sprintf(msg, "Invalid option character: '%c'", arg[-1]);
				printUsage(msg);
				break;
				}
		}
	}
	return argNum;
}

static void parseEnvironmentOptions()
{
	const char *const envOptions = getenv(ENV_VARIABLE_NAME);

	if (envOptions != NULL)
	{
		char *envArgs[MAX_ENV_ARGS + 1];		/* arg list plus NULL */
		int i, j = 0, argCount = 0;
		boolean newArg = TRUE;

		for (i = 0  ;  envOptions[i] != 0  ;  ++i)
		{
			if (envOptions[i] == '\\')		/* next character is literal */
				EnvArgList[j++] = envOptions[++i];
			else if (envOptions[i] == ' ')
			{
				if (j > 0  &&  EnvArgList[j - 1] != 0)
				{
					EnvArgList[j++] = 0;
					if (++argCount > MAX_ENV_ARGS)
						break;
					newArg = TRUE;
				}
			}
			else
			{
				if (newArg)
				{
					envArgs[argCount] = &EnvArgList[j];
					newArg = FALSE;
				}
				EnvArgList[j++] = envOptions[i];
			}
			if (j >= MAXLINE)
				break;
		}
		EnvArgList[j] = 0;				/* null terminate last argument */
		envArgs[++argCount] = NULL;
		parseOptions(envArgs);
	}
}

extern int main( argc, argv )
	int argc;
	char **const argv;
{
	int argNum;

#ifdef __EMX__
	_wildcard(&argc, &argv);			/* expand wildcards in argument list */
#endif

	parseEnvironmentOptions();
	argNum = 1 + parseOptions(&argv[1]);		/* 0th arg is program name */

	/*  Disable warnings, since these are only possible with sorted tags.
	 */
	if (Option.unsorted)
		Option.warnings = FALSE;

	/*	There must always be at least one source file or a file list.
	 */
	if (argNum == argc  &&  Option.fileList == NULL)
		printUsage("No files specified");
	else
	{
		static char tempName[20];
		boolean toStdout = FALSE;

		if (Option.xref  ||  strcmp(Option.tagFile, "-") == 0)
			toStdout = TRUE;

		/*	Open the tags File.
		 */
		if (toStdout)
		{
			if (Option.unsorted)	/* if not sorting, tags go straight out */
			{
				TagFile.fp = stdout;
				TagFile.name = NULL;
			}
			else
			{
				strcpy(tempName, TMP_TAGS);
				TagFile.name = mktemp(tempName);
				TagFile.fp = fopen(TagFile.name, "w");
			}
		}
		else
		{
			TagFile.name = Option.tagFile;
			if (isSourceFile(TagFile.name))
				printUsage("-o: Attempt to overwrite a non-tag file");
			TagFile.fp = fopen(TagFile.name, Option.append ? "a" : "w");
		}
		if (TagFile.fp == NULL)
		{
			perror(TagFile.name);
			exit(1);
		}

		if (Option.ignoreList != NULL)
			readIgnoreList(Option.ignoreList);

		if (Option.fileList != NULL)
			createTagsForList(Option.fileList);
		createTagsForArgs(&argv[argNum]);

		fclose(TagFile.fp);
		freeIgnoreList();

		if (! Option.unsorted)
		{
#ifdef INTERNAL_SORT
			internalSortTags(toStdout);
#else
			externalSortTags(toStdout);
#endif
		}
		if (toStdout  &&  TagFile.name != NULL)
			REMOVE(TagFile.name);
	}

	exit(0);
	return 0;
}
