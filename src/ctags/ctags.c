/*  $Revision: 2.25 $		$Date: 1996/10/08 02:42:25 $
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

#if defined(__STDC__) || defined(WIN32)
# include <stdlib.h>		/* to declare malloc() */
#else
# define const				/* not guaranteed to be available on non-ANSI */
#endif

/*	To declare "struct stat" and stat().
 */
#if __MWERKS__
# include <stat.h>			/* there is no sys directory on the Mac */
#else
# include <sys/types.h>
# include <sys/stat.h>
#endif

/*	To define the maximum path length (hopefully)
 */
#include <limits.h>			/* to define PATH_MAX (hopefully) */
#ifdef __BORLANDC__
# include <dir.h>			/* to define MAXPATH */
#else
# if !defined(AMIGA) && !defined(WIN32)
#  include <dirent.h>		/* to define MAXNAMLEN (hopefully) */
# endif
#endif

#ifdef DEBUG
# include <assert.h>
#endif

/*============================================================================
=	Defines
============================================================================*/

#define VERSION		"Exuberant Ctags, Version 1.5, by Darren Hiebert"

/*----------------------------------------------------------------------------
 *	Portability defines
 *--------------------------------------------------------------------------*/

/*	Determine whether or not to use prototypes.
 */
#if !defined (__ARGS) && (defined(__STDC__) || defined(WIN32))
# define __ARGS(x)	x
# define USING_PROTOTYPES
#else
# define __ARGS(x)	()
# undef USING_PROTOTYPES
#endif

#if !defined(MSDOS) && defined(__MSDOS__)
# define MSDOS
#endif

/*	External "sort" utility not used (available) on these platforms.
 */
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(AMIGA)
# ifndef INTERNAL_SORT
#  define INTERNAL_SORT
# endif
#endif

/*	Tells us how to open source files.
 */
#if defined(MSDOS) || defined(OS2) || defined(WIN32)
# ifndef BINARY_OPEN_MODE
#  define BINARY_OPEN_MODE
# endif
#endif

#ifdef BINARY_OPEN_MODE
# define OPEN_MODE	"rb"		/* open in binary or fseek() misbehaves */
#else
# define OPEN_MODE	"r"
#endif

/*	MS-DOS doesn't allow manipulation of standard error, so we send it to
 *	stdout instead.
 */
#if defined(MSDOS) || defined(WIN32)
# define errout		stdout
#else
# define errout		stderr
#endif

/*----------------------------------------------------------------------------
 *	Miscellaneous defines
 *--------------------------------------------------------------------------*/
#define INVOCATION	"\
Usage: ctags [-aBdFnNsStTuwWx] [-{f|o} name] [-h list] [-i [+-=]types]\n\
             [-I list] [-L file] [--help] file(s)"

#define ENV_VARIABLE_NAME		"CTAGS"
#define MAX_HEADER_EXTENSIONS	100
#define MAX_ENV_ARGS			30
#define CPP_NESTING_LEVEL		20
#define TAG_MEMORY				2

#ifndef PATH_MAX
# ifdef MAXNAMLEN
#  define PATH_MAX MAXNAMLEN
# else
#  ifdef MAXPATH                /* found in Borland C dir.h */
#   define PATH_MAX MAXPATH
#  else
#   define PATH_MAX 256         /* fall-back */
#  endif
# endif
#endif

#ifndef MAX_NAME_LENGTH
# define MAX_NAME_LENGTH	256		/* maximum length of token with null */
#endif

/*	Maximum length of tag line and CTAGS option, including null.
 */
#ifndef MAX_TAG_LINE
# define MAX_TAG_LINE	1024
#endif

/*	Maximum command line.
 */
#ifndef MAX_CMD_LINE
# define MAX_CMD_LINE	1024
#endif

/*	The following separators are permitted for list options.
 */
#define HEADER_SEPARATORS	".,;"
#define IGNORE_SEPARATORS	",; \t\n"

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

#define STRING_SYMBOL	'S'
#define CHAR_SYMBOL		'C'

/*============================================================================
=	Macros
============================================================================*/
#ifdef DEBUG
#define debug(level)	((Option.debugLevel & (level)) != 0)
#endif

/*	These may not be defined in a non-UNIX environment.
 */
#ifndef S_ISREG
# if defined(S_IFREG) && !defined(AMIGA)
#  define S_ISREG(mode)		((mode) & S_IFREG)
# else
#  define S_ISREG(mode)		TRUE		/* assume regular file */
# endif
#endif

#ifndef S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(mode)		(((mode) & S_IFMT) == S_IFLNK)
# else
#  define S_ISLNK(mode)		FALSE		/* assume no soft links */
# endif
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
#ifdef DEBUG
enum { DEBUG_VISUAL = 1, DEBUG_CPP = 2, DEBUG_STATUS = 4 };
#endif

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

typedef struct {
	long	location;		/* file position of line containing name */
	long	lineNumber;		/* line number of tag */
	char	name[MAX_NAME_LENGTH];	/* the name of the token */
} tagInfo;

/*	Describes the statement currently undergoing analysis.
 */
typedef struct {
	scope_t	scope;
	enum {
		DECL_UNSPEC,			/* unspecified (non-specific) */
		DECL_CLASS,				/* C++ class */
		DECL_ENUM,				/* enumeration */
		DECL_STRUCT,			/* structure */
		DECL_UNION,				/* union */
		DECL_NOMANGLE			/* C++ name demangling block */
	} declaration;			/* describes specifier associated with TOK_SPEC */
	token_t	token;			/* the most recent type of token */
	token_t	prev[2];		/* the previous tokens */
	boolean	funcPtr;		/* whether 'name' is a function pointer */
	boolean	gotName;		/* whether a name has yet been parsed */
	boolean	inEnumBody;		/* currently within enumeration value list */
	boolean buf1;			/* is tag[1] the primary buffer? */
	tagInfo tag[2];			/* information regarding last 2 tag candidates */
} statementInfo;

/*============================================================================
=	Data definitions
============================================================================*/

/*	Holds a modified copy of the environment option.
 */
static char EnvArgList[MAX_CMD_LINE];

/*	Note that the strings in this array must correspond to the types in the
 *	tag_t enumeration.
 */
static const char *TagTypeNames[] = {
	"tag", "def", "enum", "proto", "func", "type", "var"
};

static const char *const Help[] = {
 "    --help     Prints a more detailed help message.",
 "    -a         Append tags to existing tag file.",
#ifdef DEBUG
 "    -b <line>  Set break line.",
#endif
 "    -B         Use backward searching patterns (?...?).",
#ifdef DEBUG
 "    -D <level> Set debug level.",
#endif
 "    -f <name>  Name for output tag file (default \"tags\").",
 "    -F         Use forward searching patterns (/.../) (default).",
 "    -h <list>  List of header file extensions (default \".h.H.hpp.hxx.h++\").",
 "    -i <types> List of tag types to include [defgptvPS] (default \"=defgtvS\").",
 "    -I <list>  List of tokens to ignore is read from command line or file.",
 "    -L <file>  List of source file names are read from specified file.",
 "    -n         Use line numbers in tag file instead of search patterns.",
 "    -N         Use search patterns in tag file instead of line numbers.",
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
#ifdef DEBUG
 "    -b <line>  Set break line.",
#endif
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
 "    -I <list | file>",
 "         A list of tokens to ignore is read from either the command line,",
 "         or the specified file (if leading character is '.', '/', or '\\').",
 "         Particularly useful when a function definition or declaration",
 "         contains some special macro before the parameter list.",
 "    -L <file>",
 "         A list of source file names are read from the specified file.",
 "         If specified as \"-\", then standard input is read.",
 "    -n   Use line numbers in tag file instead of search patterns.",
 "    -N   Use search patterns in tag file instead of line numbers.",
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
	struct {
		char **list;
		unsigned int count, max;
	} ignore;				/* -I  name of file containing tokens to ignore */
	boolean	append;			/* -a  append to "tags" files */
	boolean	backward;		/* -B  regexp patterns search backwards */
	enum {
		EX_MIX,				/* default Ex tag location method */
		EX_LINENUM,			/* -n  only line numbers in tag file */
		EX_PATTERN			/* -N  only patterns in tag file */
	} locate;
	boolean	unsorted;		/* -u  do not sort tags */
	boolean warnings;		/* -w  generate warnings about duplicate tags */
	boolean xref;			/* -x  generate xref output instead */
	const char *fileList;	/* -L  name of file containing names of files */
	const char *tagFile;	/* -o  name of tags file */
	const char *headerExt[MAX_HEADER_EXTENSIONS + 1];/* -h  header extensions */
#ifdef DEBUG
	int debugLevel;			/* -D  debugging output */
	long breakLine;			/* -b  source line at which to call lineBreak() */
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
	{ NULL, 0, 0 },		/* -I */
	FALSE,				/* -a */
	FALSE,				/* -B */
	EX_MIX,				/* -n */
	FALSE,				/* -u */
	FALSE,				/* -w */
	FALSE,				/* -x */
	NULL,				/* -L */
	"tags",				/* -o */
	{ "h", "H", "hpp", "hxx", "h++", NULL },		/* -h */
#ifdef DEBUG
	0, 0,				/* -D, -b */
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
		enum {
			DRCTV_NONE,
			DRCTV_HASH,
			DRCTV_DEFINE,
			DRCTV_IF
		} state;
		boolean		accept;		/* is a directive syntatically permitted? */
		tagInfo	tag;		/* the name associated with the directive */
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
#ifdef INTERNAL_SORT
	"internal_sort",
#endif
	NULL
};

/*============================================================================
=	Function prototypes
============================================================================*/
#if !defined(__STDC__) && !defined(WIN32)
extern char *getenv();
extern void *malloc();
extern void free();
extern int stat();
extern int system();
#endif

/*	GCC on SunOS 4.x is missing these prototypes.
 */
#if defined(sun) && defined(FILE) && !defined(__SVR4) && defined(__STDC__)
#define __P(a) a
/* excerpt from sun_stdlib.h */
extern int _filbuf __P((FILE *));
extern int _flsbuf __P((unsigned char, FILE *));
extern int fclose  __P((FILE *));
extern int fprintf __P((FILE *, char *, ...));
extern int fputc   __P((int, FILE *));
extern int fputs   __P((char *, FILE *));
extern int fscanf  __P((FILE *, const char *, ...));
extern int fseek   __P((FILE *, long, int));
extern int rewind  __P((FILE *));
extern int sscanf  __P((const char *, const char *, ...));
extern int system  __P((const char *));
extern int ungetc  __P((int, FILE *));
extern void perror __P((char *));
#endif

/*	Debug functions.
 */
#ifdef DEBUG
static void lineBreak __ARGS((void));
static void debugOpen __ARGS((const char *const name));
static void debugPutc __ARGS((const int c, const int level));
static void debugLabel __ARGS((const tag_t tagType, const char *const tagName));
static void clearString __ARGS((char *const string, const int length));
#endif

/*	Low level source file read functions (line splicing and newline conversion
 *	are performed at this level).
 */
static boolean isFileHeader __ARGS((const char *const name));
static boolean fileOpen __ARGS((const char *const name));
static void fileClose __ARGS((void));
static void fileNewline __ARGS((void));
static int fileGetc __ARGS((void));
static void fileUngetc __ARGS((int c));
static void getFileLine __ARGS((const long int seek, char *const line_buffer, const unsigned int maxLength));

/*	Tag entry creation functions.
 */
static size_t writeSourceLine __ARGS((FILE *const fp, const char *const line));
static size_t writeCompactSourceLine __ARGS((FILE *const fp, const char *const line));
static void writeXrefEntry __ARGS((const tagInfo *const tag, const tag_t tagType));
static void truncateTagLine __ARGS((char *const line, const char *const token));
static void makeTagEntry __ARGS((const tagInfo *const tag, const scope_t scope, const tag_t tagType, const boolean useLineNumber));
static void makeTag __ARGS((const tagInfo *const tag, const scope_t scope, const tag_t tagType));
static void makeDefineTag __ARGS((const tagInfo *const tag, const scope_t scope));

/*	High level source read functions (preprocessor directives are handled
 *	within this level).
 */
static boolean cppOpen __ARGS((const char *const name));
static void cppClose __ARGS((void));
static void cppUngetc __ARGS((const int c));
static boolean cppReadDirective __ARGS((int c, char *name));
static boolean cppReadIdentifier __ARGS((int c, tagInfo *const tag));
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

/*	Parsing functions.
 */
static int skipToNonWhite __ARGS((void));
static int skipToCharacter __ARGS((const int findchar));
static void skipToFormattedBraceMatch __ARGS((void));
static boolean skipToMatch __ARGS((const char *const pair));
static int skipInitializer __ARGS((const boolean inEnumBody));
static void readIdendifier __ARGS((const int firstChar, char *const name));
static int skipParameterDeclarations __ARGS((int c));
static boolean analyzeParens __ARGS((statementInfo *const st));
static boolean isIgnoreToken __ARGS((const char *const name));
static void analyzeIdentifier __ARGS((statementInfo *const st));
static boolean nextToken __ARGS((statementInfo *const st, const int nesting));

/*	Scanning functions.
 */
static void initStatement __ARGS((statementInfo *const st));
static boolean createTags __ARGS((const int nesting));
static boolean createTagsForFile __ARGS((const char *const name));
static boolean isSourceFile __ARGS((const char *const filename));
static const char *getNextListFile __ARGS((FILE *const fp));
static void createTagsForList __ARGS((const char *const listFile));
static void createTagsForArgs __ARGS((char *const *const argList));

/*	Tag sorting functions.
 */
#ifdef INTERNAL_SORT
static void failedSort __ARGS((const char *const msg));
static int compareTags __ARGS((const void *const one, const void *const two));
static void writeSortedTags __ARGS((char **const table, const boolean toStdout));
static void internalSortTags __ARGS((const boolean toStdout));
#else
static void externalSortTags __ARGS((const boolean toStdout));
#endif

/*	Start up and terminate functions.
 */
static void printHelp __ARGS((const char *const *const help, FILE *const where));
static void printUsage __ARGS((const char *const error));
static void readExtensionList __ARGS((char *const list));
static void clearTagList __ARGS((void));
static void applyTagInclusionList __ARGS((const char *const list));
static void readIgnoreList __ARGS((char *const list));
static void readIgnoreListFromFile __ARGS((const char *const fileName));
static void freeIgnoreList __ARGS((void));
static char *readOptionArg __ARGS((const int option, char **const arg, char *const *const argList, int *const argNum));
static int parseOptions __ARGS((char *const *const argList));
static void parseEnvironmentOptions __ARGS((void));
static void openTagFile __ARGS((const boolean toStdout));

extern int main __ARGS((int argc, char **argv));

/*============================================================================
=	Function definitions
============================================================================*/

#ifdef DEBUG

static void lineBreak() {}		/* provides a line-specified break point */

static void debugOpen( name )
	const char *const name;
{
	if (debug(DEBUG_STATUS))
	{
		printf("Opening: %s\n", name);
		fflush(stdout);
	}
}

static void debugPutc( c, level )
	const int c;
	const int level;
{
	if (debug(level)  &&  c != EOF)
	{
		putchar(c);
		fflush(stdout);
	}
}

static void debugLabel( tagType, tagName )
	const tag_t tagType;
	const char *const tagName;
{
	if (debug(DEBUG_VISUAL | DEBUG_CPP))
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

/*----------------------------------------------------------------------------
*	Source file reading functions
*--------------------------------------------------------------------------*/

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
		File.fp = fopen(name, OPEN_MODE);
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

/*	Action to take for each encountered source newline.
 */
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
	if (File.ungetch != '\0')
	{
		c = File.ungetch;
		File.ungetch = '\0';
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
			debugPutc(BACKSLASH, DEBUG_VISUAL);		/* print the characters */
			debugPutc(c, DEBUG_VISUAL);				/*   we are throwing away */
#endif
			escaped = FALSE;				/* BACKSLASH now fully processed */
			goto nextChar;					/* through away "\NEWLINE" */
		}
		break;
	}

#ifdef DEBUG
	debugPutc(c, DEBUG_VISUAL);
#endif
	return c;
}

static void fileUngetc( c )
	int c;
{
	File.ungetch = c;
}

/*	Places into "line_buffer" the contents of the line referenced by "seek".
 */
static void getFileLine( seek, line_buffer, maxLength )
	const long seek;
	char *const line_buffer;
	const unsigned int maxLength;
{
	const long oldseek = ftell(File.fp);	/* remember original position */

	fseek(File.fp, seek, 0);
	fgets(line_buffer, (int)maxLength, File.fp);		
	fseek(File.fp, oldseek, 0);				/* return to original position */
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
static size_t writeSourceLine( fp, line )
	FILE *const fp;
	const char *const line;
{
	size_t	length = 0;
	const char *p;
	char c;

	/*	Write everything up to, but not including, the newline.
	 */
	for (p = line, c = *p  ;  c != NEWLINE  &&  c != '\0'  ;  ++p, c = *p)
	{
		const int next = *(p + 1);

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
		if (c != CRETURN  ||  next != NEWLINE)
		{
			putc(c, fp);
			++length;
		}
	}
	return length;
}

/*	Writes "line", stripping leading and duplicate white space.
 */
static size_t writeCompactSourceLine( fp, line )
	FILE *const fp;
	const char *const line;
{
	boolean	lineStarted = FALSE;
	size_t	length = 0;
	const char *p;
	char c;

	/*	Write everything up to, but not including, the newline.
	 */
	for (p = line, c = *p  ;  c != NEWLINE  &&  c != '\0'  ;  c = *++p)
	{
		if (lineStarted  || ! isspace(c))		/* ignore leading spaces */
		{
			lineStarted = TRUE;
			if (isspace(c))
			{
				int next;

				/*	Consume repeating white space.
				 */
				while (next = *(p+1) , isspace(next)  &&  next != NEWLINE)
					++p;
				c = ' ';		/* force space character for any white space */
			}
			if (c != CRETURN  ||  *(p + 1) != NEWLINE)
			{
				putc(c, fp);
				++length;
			}
		}
	}
	return length;
}

static void writeXrefEntry( tag, tagType )
	const tagInfo *const tag;
	const tag_t tagType;
{
	char line[MAX_TAG_LINE];
	size_t length;

	getFileLine(tag->location, line, MAX_TAG_LINE);
	length = fprintf(TagFile.fp, "%-20s %-6s %4ld  %-14s ",
					 tag->name, tagTypeName(tagType),
					 tag->lineNumber, File.name);
	line[MAX_TAG_LINE - 2 - length] = '\0';		/* truncate line */
	length += writeCompactSourceLine(TagFile.fp, line);
	putc(NEWLINE, TagFile.fp);
	++length;

	++TagFile.numTags;
	if (strlen(tag->name) > TagFile.max.tag)
		TagFile.max.tag = strlen(tag->name);
	if (length > TagFile.max.line)
		TagFile.max.line = length;
}

static void truncateTagLine( line, token )
	char *const line;
	const char *const token;
{
	char *p = strstr(line, token);

	p += strlen(token);
	*p = '\0';
}

/*	This function generates a tag for the object in name, whose tag line is
 *	located at a given seek offset.
 */
static void makeTagEntry( tag, scope, tagType, useLineNumber )
	const tagInfo *const tag;
	const scope_t scope;
	const tag_t tagType;
	const boolean useLineNumber;
{
	boolean include = FALSE;

	if (scope != SCOPE_EXTERN  &&		/* should never happen */
		(scope != SCOPE_STATIC  ||  Option.include.statics))
	{
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
				char line[MAX_TAG_LINE];

				getFileLine(tag->location, line, MAX_TAG_LINE);
				if (tagType == TAG_DEFINE)
					truncateTagLine(line, tag->name);
				length += fprintf(TagFile.fp, "%s\t%s\t", tag->name, File.name);
				length += fprintf(TagFile.fp, "%c^", Option.backward ? '?':'/');
				length += writeSourceLine(TagFile.fp, line);
				length += fprintf(TagFile.fp, "%.*s%c\n",
								  (int)(MAX_TAG_LINE - 3 - length),
								  (line[strlen(line) - 1] == '\n') ? "$":"\\>",
								  Option.backward ? '?':'/');
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
	const tagInfo *const tag;
	const scope_t scope;
	const tag_t tagType;
{
	makeTagEntry(tag, scope, tagType, (Option.locate == EX_LINENUM));
}

static void makeDefineTag( tag, scope )
	const tagInfo *const tag;
	const scope_t scope;
{
	makeTagEntry(tag, scope, TAG_DEFINE, (Option.locate != EX_PATTERN));
}

/*----------------------------------------------------------------------------
*	Scanning functions	
*
*	This section handles preprocessor directives.  It strips out all
*	directives and may emit a tag for #define directives.
*--------------------------------------------------------------------------*/

static boolean cppOpen( name )
	const char *const name;
{
	boolean opened;

	opened = fileOpen(name);
	if (opened)
	{
		Cpp.ungetch = '\0';
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

/*	Reads a directive, whose first character is given by "c", into "name".
 */
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

/*	Reads an identifier, whose first character is given by "c", into "tag",
 *	together with the file location and corresponding line number.
 */
static boolean cppReadIdentifier( c, tag )
	int c;
	tagInfo *const tag;
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

/*	Pushes one nesting level for an #if directive, indicating whether or not
 *	the path should be ignored and whether a path has already been chosen.
 */
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

/*	Pops one nesting level for an #endif directive.
 */
static boolean popCppIgnore()
{
	if (Cpp.directive.level > 0)
		--Cpp.directive.level;

	return cppIgnore();
}

/*	Returns whether or not the parent of this nesting level was ignored.
 */
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

/*	Handles a pre-processor directive whose first character is given by "c".
 */
static boolean handleDirective( c )
	const int c;
{
	char *const name = Cpp.directive.tag.name;
	const scope_t scope = File.header ? SCOPE_GLOBAL : SCOPE_STATIC;
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

/*	Called upon reading of a slash ('/') characters, determines whether a
 *	comment is encountered, and its type.
 */
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

/*	Skips over a C style comment.
 */
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

/*	Skips over a C++ style comment.
 */
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

/*	Skips to the end of a string, returning a special character to
 *	symbolically represent a generic string.
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
	return STRING_SYMBOL;			/* symbolic representation of string */
}

/*	Skips to the end of the three (possibly four) 'c' sequence, returning a
 *	special character to symbolically represent a generic character.
 */
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
	return CHAR_SYMBOL;				/* symbolic representation of character */
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

	if (Cpp.ungetch != '\0')
	{
		c = Cpp.ungetch;
		Cpp.ungetch = '\0';
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
	debugPutc(c, DEBUG_CPP);
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
	int c = '\0';

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
	for (i = 1, c = cppGetc() ; i < MAX_NAME_LENGTH - 1 && isident(c) ;
		 i++, c = cppGetc())
	{
		name[i] = c;
	}
	name[i] = '\0';				/* null terminate name */

	cppUngetc(c);				/* unget non-identifier character */
}

/*	Skips over interveaning characters declaring function parameters
 *	(non-ANSI style function declarations).
 */
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

/*	Analyzes the context and contents of parentheses.
 */
static boolean analyzeParens( st )
	statementInfo *const st;
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
		char name[MAX_NAME_LENGTH];
		boolean gotParenName = FALSE;
		long location = 0, lineNumber = 0;

#ifdef DEBUG
		clearString(name, MAX_NAME_LENGTH);
#endif
		if (isident1(c))
		{
			readIdendifier(c, name);
			gotParenName= TRUE;
			location	= File.seek;
			lineNumber	= File.lineNumber;
		}
		else
		{
			/*	A double paren almost certainly means one of those conditional
			 *	prototype macro thingies (e.g. __ARGS((void)) ). If found, we
			 *	will use the previous name, if it is not empty.
			 */
			if (c == '('  &&  st->gotName  &&  *st->tag[!st->buf1].name != '\0')
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
			if (st->gotName)
			{
				if (strchr("{;,", c) != NULL)
				{
					st->token = TOK_ARGS;		/* parameter list to a func. */
					st->declaration = DECL_UNSPEC;	/* clear any other decl. */
				}
				else if (isident1(c))
				{
					st->token = TOK_ARGS;		/* parameter list to a func. */
					st->declaration = DECL_UNSPEC;	/* clear any other decl. */
					c = skipParameterDeclarations(c);
				}
				else
					st->token = TOK_IGNORE;
			}
			/*	The name inside the parentheses must have been a function or
			 *	variable name.
			 */
			else if (gotParenName)
			{
				tagInfo *const tag = &st->tag[st->buf1];

				st->gotName		= TRUE;
				st->token		= TOK_NAME;
				tag->location	= location;
				tag->lineNumber	= lineNumber;
				strcpy(tag->name, name);
			}
			else
				st->token = TOK_IGNORE;
			cppUngetc(c);
		}
	}
	return ok;
}

/*	Determines whether or not "name" should be ignored, per the ignore list.
 */
static boolean isIgnoreToken( name )
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

/*	Analyzes the identifier contained in a statement described by the
 *	statement structure and adjusts the structure according the significance
 *	of the identifier.
 */
static void analyzeIdentifier( st )
	statementInfo *const st;
{
	tagInfo *const tag = &st->tag[st->buf1];
	const char *const name = tag->name;
	boolean keyWord = FALSE;

#define match(word)	((strcmp(name,(word)) == 0) && (keyWord = TRUE))

	st->token = TOK_SPEC;		/* default unless otherwise */

	if (isIgnoreToken(name))
	{
		keyWord = TRUE;
		st->token = TOK_IGNORE;
		tag->name[0] = '\0';
	}
	else switch ((unsigned char)name[0])		/* is it a reserved word? */
	{
	case 'c':		 if (match("class"	))	st->declaration = DECL_CLASS;
				else if (match("const"	))	st->token		= TOK_IGNORE;
				else if (match("char"	))	st->declaration = DECL_UNSPEC;
				break;
	case 'd':		 if (match("double"	))	st->declaration = DECL_UNSPEC;
				break;
	case 'e':		 if (match("enum"	))	st->declaration = DECL_ENUM;
				else if (match("extern"	))	st->scope		= SCOPE_EXTERN;
				break;
	case 'f':		 if (match("float"	))	st->declaration = DECL_UNSPEC;
				break;
	case 'i':		 if (match("int"	))	st->declaration = DECL_UNSPEC;
				else if (match("inline"	))	st->token		= TOK_IGNORE;
				break;
	case 'l':		 if (match("long"	))	st->declaration = DECL_UNSPEC;
				break;
	case 'o':		 if (match("overload"))	st->token		= TOK_IGNORE;
				break;
	case 'p':		 if (match("private"))	st->scope		= SCOPE_STATIC;
				else if (match("public"	))	st->token		= TOK_IGNORE;
				break;
	case 's':		 if (match("static"	))	st->scope		= SCOPE_STATIC;
				else if (match("struct"	))	st->declaration = DECL_STRUCT;
				else if (match("short"	))	st->declaration = DECL_UNSPEC;
				else if (match("signed"	))	st->declaration = DECL_UNSPEC;
				break;
	case 't':		 if (match("typedef"))	st->scope		= SCOPE_TYPEDEF;
				break;
	case 'u':		 if (match("union"	))	st->declaration = DECL_UNION;
				else if (match("unsigned"))	st->declaration = DECL_UNSPEC;
				break;
	case 'v':		 if (match("virtual"))	st->token		= TOK_IGNORE;
				else if (match("void"	))	st->declaration = DECL_UNSPEC;
				else if (match("volatile"))	st->token		= TOK_IGNORE;
				break;
	case STRING_SYMBOL:
		if (name[1] == '\0')
		{
			keyWord = TRUE;
			if (! st->gotName  &&  st->scope == SCOPE_EXTERN)
				st->declaration = DECL_NOMANGLE;
			else
				st->token = TOK_IGNORE;
		}
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
	statementInfo *const st;
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
					st->declaration == DECL_CLASS	||
					st->declaration == DECL_NOMANGLE )
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
				if (st->gotName  &&  st->token == TOK_IGNORE)
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
	statementInfo *const st;
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
		tagInfo *const tag = &st->tag[i];

		tag->location	= 0;
		tag->lineNumber	= 0;
		tag->name[0]	= '\0';
#ifdef DEBUG
		clearString(tag->name, MAX_NAME_LENGTH);
#endif
	}
}

/*	Parses the current file and decides whether to write out and tags that
 *	are discovered.
 */
static boolean createTags( nesting )
	const int nesting;
{
	const scope_t declScope = File.header ? SCOPE_GLOBAL : SCOPE_STATIC;
	statementInfo st;
	boolean ok = TRUE;

#ifdef DEBUG
	if (nesting > 0  &&  debug(DEBUG_VISUAL | DEBUG_STATUS))
		printf("<#++nesting:%d#>", nesting);
#endif
	initStatement(&st);

	while ((ok = nextToken(&st, nesting)))
	{
		tagInfo *const tag = &st.tag[st.buf1];

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
				makeTag(tag, declScope, TAG_BLOCKTAG);
			}
		}

		/*	If ENUM (BODY_END || (inEnumBody && NAME COMMA)), then any
		 *	previous NAME is an enumeration value.
		 */
		else if (st.token == TOK_ENUM_BODY_END  ||
				 (st.inEnumBody  &&  st.token == TOK_COMMA))
		{
			if (st.prev[0] == TOK_NAME)
				makeTag(tag, declScope, TAG_ENUM);
		}

		/*	If NAME ARGS BODY, then NAME is a function.
		 */
		else if (st.token == TOK_BODY  &&  st.prev[0] == TOK_ARGS  &&
				 st.gotName)
		{
			if (st.scope == SCOPE_EXTERN)  /* allowed for func. def. */
				st.scope = SCOPE_GLOBAL;
			makeTag(tag, st.scope, TAG_FUNCTION);
		}

		/*	If SEMICOLON or COMMA, then NAME is either a variable,
		 *	typedef, or function declaration;
		 */
		else if (st.token == TOK_SEMICOLON  ||  st.token == TOK_COMMA)
		{
			if (st.prev[0] == TOK_NAME  ||  (st.funcPtr  &&  st.gotName))
			{
				if (st.scope == SCOPE_TYPEDEF)
					makeTag(tag, declScope, TAG_TYPEDEF);

				/*	We have to watch that we do not interpret a
				 *	declaration of the form "struct tag;" as a variable
				 *	definition. In such a case, the declaration will be
				 *	either class, enum, struct or union, and prev[1] will
				 *	be empty (i.e. SEMICOLON).
				 */
				else if (nesting == 0  &&
					 (st.declaration == DECL_UNSPEC  || st.prev[1] != TOK_SPEC))
				{
					if (st.scope != SCOPE_EXTERN)
						makeTag(tag, st.scope, TAG_VARIABLE);
				}
			}
			else if (st.prev[0] == TOK_ARGS)
			{
				if (! File.header)
					makeTag(tag, SCOPE_STATIC, TAG_FUNCDECL);
				else if (st.scope == SCOPE_GLOBAL || st.scope==SCOPE_EXTERN)
					makeTag(tag, SCOPE_GLOBAL, TAG_FUNCDECL);
			}
		}

		/*	Reset after a semicolon or ARGS BODY pair.
		 */
		if (st.token == TOK_SEMICOLON  ||  (st.token == TOK_BODY  &&
			 (st.prev[0] == TOK_ARGS  ||  st.declaration == DECL_NOMANGLE)))
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
	if (nesting > 0  &&  debug(DEBUG_VISUAL | DEBUG_STATUS))
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
	static char line[MAX_TAG_LINE];
	boolean isSource = FALSE;		/* we assume not until found otherwise */
	FILE *const fp = fopen(filename, "r");

	if (fp != NULL  &&  fgets(line, MAX_TAG_LINE, fp) != NULL)
	{
		char *tagPrefix, *tagFname, *pattern;
		char *separator1, *separator2;
		char *const buffer = malloc((size_t)(5 * MAX_TAG_LINE));

		if (buffer == NULL)
		{
			perror("Insufficient memory");
			exit(1);
		}
		tagPrefix	= &buffer[0 * MAX_TAG_LINE];
		separator1	= &buffer[1 * MAX_TAG_LINE];
		tagFname	= &buffer[2 * MAX_TAG_LINE];
		separator2	= &buffer[3 * MAX_TAG_LINE];
		pattern		= &buffer[4 * MAX_TAG_LINE];
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
				*tag = '\0';			/* null terminate the tag prefix */
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
				else for (p = tag + 1 ; *p != '\0' && !isSource ; ++p)
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
					for (p = pattern ; *p != '\0'  && ! isSource ; ++p)
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
	static char fileName[PATH_MAX + 1];
	const char *const buf = fgets(fileName, PATH_MAX + 1, fp);

	if (buf != NULL)
	{
		char *const newline = strchr(fileName, '\n');

		if (newline == NULL)
			fileName[PATH_MAX] = '\0';
		else
			*newline = '\0';
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
				if (debug(DEBUG_STATUS))
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
			if (debug(DEBUG_STATUS))
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

static void failedSort( msg )
	const char *const msg;
{
	if (TagFile.fp != NULL)
		fclose(TagFile.fp);
	if (msg == NULL)
		fputs("ctags: Insufficient memory\n", errout);
	else
		perror(msg);
	fprintf(errout, "Cannot sort tag file.\n");
	exit(1);
}

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
			perror("Cannot generate duplicate warnings\n");
		}
		*prevTag  = *thisTag  = '\0';
		*prevFile = *thisFile = '\0';
	}

	/*	Write the sorted lines back into the tag file.
	 */
	if (toStdout)
		TagFile.fp = stdout;
	else
	{
		TagFile.fp = fopen(TagFile.name, "w");
		if (TagFile.fp == NULL)
			failedSort(TagFile.name);
	}
	for (i = 0 ; i < TagFile.numTags ; ++i)
	{
		/*	Here we filter out identical tag *lines* (including search
		 *	pattern) if this is not an xref file.
		 */
		if (i == 0  ||  Option.xref  ||  strcmp(table[i], table[i-1]) != 0)
			if (fputs(table[i], TagFile.fp) == EOF)
				failedSort(TagFile.name);

		if (Option.warnings)
		{
			int fields;

			if (Option.xref)
				fields = sscanf(table[i],"%s %*s %*s %s", thisTag, thisFile);
			else
				fields = sscanf(table[i],"%[^\t]\t%[^\t]", thisTag, thisFile);

			if (fields == 2  &&  strcmp(thisTag, prevTag) == 0)
			{
				fprintf(errout, "Duplicate entry in ");
				if (strcmp(thisFile, prevFile) != 0)
					fprintf(errout, "%s and ", prevFile);
				fprintf(errout, "%s: %s\n", thisFile, thisTag);
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

static void internalSortTags( toStdout )
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
		failedSort(NULL);

	mallocSize = maxLength + tableSize;

	/*	Open the tag file and place its lines into allocated buffers.
	 */
	TagFile.fp = fopen(TagFile.name, "r");
	if (TagFile.fp == NULL)
		failedSort(TagFile.name);
	for (i = 0 ; i < TagFile.numTags && !feof(TagFile.fp) ; ++i)
	{
		size_t stringSize;

		if (fgets(buffer, (int)maxLength, TagFile.fp) == NULL  &&
															! feof(TagFile.fp))
			failedSort(TagFile.name);
		stringSize = strlen(buffer) + 1;
		table[i] = (char *)malloc(stringSize);
		if (table[i] == NULL)
			failedSort(NULL);
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
	if (debug(DEBUG_STATUS))
		printf("sort memory: %ld bytes\n", (long)mallocSize);
#endif
	for (i = 0 ; i < TagFile.numTags ; ++i)
		free(table[i]);
	free(table);
	free(buffer);
}

#else

static void externalSortTags( toStdout )
	const boolean toStdout;
{
	static char cmd[MAX_CMD_LINE];
	int ret;

	sprintf(cmd, "sort -u -o %s %s", TagFile.name, TagFile.name);
	ret = system(cmd);
	if (ret == 0  &&  Option.warnings)
	{
		char awkProg[MAX_CMD_LINE];
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
	printHelp(Help, errout);
	if (error != NULL)
	{
		fprintf(errout, "\nError: %s\n", error);
#if !defined(MSDOS) && !defined(WIN32) && !defined(OS2)
		fputs("\n", errout);
#endif
	}
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
		if (debug(DEBUG_STATUS))
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

	for (p = list  ;  *p != '\0'  ;  ++p)
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
#ifdef DEBUG
		if (debug(DEBUG_STATUS))
			printf("ignore token: %s\n", token);
#endif
		token = strtok(NULL, IGNORE_SEPARATORS);
	}
}

static void readIgnoreListFromFile( fileName )
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
		char ignoreToken[MAX_NAME_LENGTH];
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
			perror("Cannot create ignore list");
		else while (fscanf(fp, "%255s", ignoreToken) == 1)
		{
#ifdef DEBUG
			assert(i < Option.ignore.count);
#endif
			Option.ignore.list[i] = (char *)malloc(strlen(ignoreToken) + 1);
			strcpy(Option.ignore.list[i], ignoreToken);
			++i;
#ifdef DEBUG
			if (debug(DEBUG_STATUS))
				printf("ignore token: %s\n", ignoreToken);
#endif
		}
	}
}

static void freeIgnoreList()
{
	while (Option.ignore.count > 0)
	{
		free(Option.ignore.list[--Option.ignore.count]);
	}
	if (Option.ignore.list != NULL)
		free(Option.ignore.list);
	Option.ignore.list = NULL;
	Option.ignore.max = 0;
}

static char *readOptionArg( option, arg, argList, argNum )
	const int option;
	char **const arg;
	char *const *const argList;
	int *const argNum;
{
	char *list = NULL;

	if ((*arg)[0] != '\0')			/* does list immediately follow option? */
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
		while (*arg != '\0')  switch (c = *arg++)
		{
			/*	Simple options.
			 */
			case 'a': Option.append				= TRUE;			break;
			case 'B': Option.backward			= TRUE;			break;
			case 'd': Option.include.defines	= TRUE;			break;
			case 'F': Option.backward			= FALSE;		break;
			case 'n': Option.locate				= EX_LINENUM;	break;
			case 'N': Option.locate				= EX_PATTERN;	break;
			case 's': Option.include.statics	= TRUE;
					  Option.include.prefix		= TRUE;			break;
			case 'S': Option.include.statics	= TRUE;
					  Option.include.prefix		= FALSE;		break;
			case 't': Option.include.typedefs	= TRUE;			break;
			case 'T': Option.include.blockTags	= TRUE;
					  Option.include.typedefs	= TRUE;			break;
			case 'u': Option.unsorted			= TRUE;			break;
			case 'w': Option.warnings			= FALSE;		break;
			case 'W': Option.warnings			= TRUE;			break;
			case 'x': Option.xref				= TRUE;			break;
			case '?': printHelp(LongHelp, stdout);				exit(0);

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
				param = readOptionArg(c, &arg, argList, &argNum);
				if (strchr("./\\", param[0]) != NULL)
					readIgnoreListFromFile(param);
				else
					readIgnoreList(param);
				break;

			case 'L':
				Option.fileList = readOptionArg(c, &arg, argList, &argNum);
				break;

#ifdef DEBUG
			case 'D':
				param = readOptionArg(c, &arg, argList, &argNum);
				Option.debugLevel = atoi(param);
				break;
			case 'b':
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

		for (i = 0  ;  envOptions[i] != '\0'  ;  ++i)
		{
			if (envOptions[i] == '\\')		/* next character is literal */
				EnvArgList[j++] = envOptions[++i];
			else if (envOptions[i] == ' ')
			{
				if (j > 0  &&  EnvArgList[j - 1] != '\0')
				{
					EnvArgList[j++] = '\0';
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
			if (j >= MAX_CMD_LINE)
				break;
		}
		EnvArgList[j] = '\0';				/* null terminate last argument */
		envArgs[++argCount] = NULL;
		parseOptions(envArgs);
	}
}

static void openTagFile( toStdout )
	const boolean toStdout;
{
		static char tempName[L_tmpnam];

		/*	Open the tags File.
		 */
		if (toStdout)
		{
			if (Option.unsorted)	/* if not sorting, tags go straight out */
			{
				TagFile.name = NULL;
				TagFile.fp = stdout;
			}
			else
			{
				TagFile.name = tmpnam(tempName);
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
}

extern int main( argc, argv )
	int argc;
	char **argv;
{
	int argNum;

#ifdef __EMX__
	_wildcard(&argc, &argv);			/* expand wildcards in argument list */
#endif

	parseEnvironmentOptions();
	argNum = 1 + parseOptions(&argv[1]);		/* 0th arg is program name */

	/*  Disable warnings, since these are only supported with sorted tags.
	 */
	if (Option.unsorted)
		Option.warnings = FALSE;

	/*	There must always be at least one source file or a file list.
	 */
	if (argNum == argc  &&  Option.fileList == NULL)
		printUsage("No files specified");
	else
	{
		boolean toStdout = FALSE;

		if (Option.xref  ||  strcmp(Option.tagFile, "-") == 0)
			toStdout = TRUE;

		openTagFile(toStdout);

		if (Option.fileList != NULL)
			createTagsForList(Option.fileList);
		createTagsForArgs(&argv[argNum]);

		fclose(TagFile.fp);
		freeIgnoreList();

		if (TagFile.numTags > 0  &&  ! Option.unsorted)
		{
#ifdef INTERNAL_SORT
			internalSortTags(toStdout);
#else
			externalSortTags(toStdout);
#endif
		}
		if (toStdout  &&  TagFile.name != NULL)
			remove(TagFile.name);
	}

	exit(0);
	return 0;
}
