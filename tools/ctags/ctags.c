/* vi:set ts=4 sw=4:
 *
 *	Copyright (c) 1996, Darren Hiebert
 *
 *	Author: Darren Hiebert (darren@sirsi.com, darren@hiwaay.net)
 *
 *	Currently provided for beta testing only and not for redistribution.
 *	Once released, it will be freely available.
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
 *	Creates tags for the following objects:
 *		- macro definitions
 *		- enumeration values
 *		- function definitions (and C++ methods)
 *		- function declarations (optional)
 *		- enum, struct and union tags and C++ class names
 *		- typedefs
 *		- variables
 *
 *	Installation:
 * Unix:	cc -O ctags.c -o ctags
 * NT/Win95 (Visual C++):  cl ctags.c -DWIN32 /link setargv.obj
 * NT/Win95 (Borland C++): bcc32 ctags.c -DWIN32 c:\bc45\lib\32bit\wildargs.obj
 * Amiga:	cc -so -wapruq -ps -qf -DAMIGA -Dconst= ctags.c -o ctags.o
 *			ln +q -m -o ctags ctags.o -lc16
 *
 *	If you are on a host which uses CR-LF newlines, you will need to make sure
 *	that one of these labels is defined:  MSDOS, OS2, WIN32
 *
 *	Compile with -DINTERNAL_SORT if you desire sorted tags and/or warnings for
 *	duplicate tags, but do not have the external programs "sort" and "awk"
 *	available (i.e. other than UNIX). This label is automatically defined if
 *	any of MSDOS, OS2, WIN32, or AMIGA are defined.
 *
 *	Please report any problems you find. The two problems I expect to be
 *	most likely are either a tag which you expected but is missing, or a tag
 *	created in error (shouldn't really be a tag). Please include a sample of
 *	code (the definition) for the object which misbehaves.
 */

/*============================================================================
=	Include files
============================================================================*/
#include <stdio.h>
#include <string.h>			/* to declare strchr(), strcmp() */
#include <ctype.h>			/* to declare isalnum(), isalpha(), isspace() */
#ifdef __EMX__
# include <sys/types.h>		/* for a number of types used in sys/stat.h */
#endif
#include <sys/stat.h>		/* to declare struct stat */

#if defined(__STDC__)
# include <stdlib.h>		/* to declare malloc() */
# if defined(__BORLANDC__)
#  define MSDOS
#  include <io.h>			/* to declare unlink() */
#  include <dir.h>			/* to declare mktemp() */
# elif !defined(AMIGA)
#  include <unistd.h>		/* to declare mktemp(), stat(), unlink() */
# endif
#elif defined(WIN32)
# include <stdlib.h>		/* to declare malloc() */
# include <io.h>			/* to declare mktemp() */
#else
# define const				/* not guaranteed to be available on non-ANSI */
#endif

#ifdef DEBUG
# include <assert.h>
#endif

/*============================================================================
=	Defines
============================================================================*/

#define VERSION		"Ctags, Ver. 0.43 Beta, by Darren Hiebert"

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

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(AMIGA)
# define INTERNAL_SORT
# define XREF_TMP	"tagsXXXXXX"
#else
# define XREF_TMP	"/tmp/ctagsXXXXXX"
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

#ifndef MAXLINE
# define MAXLINE	1024		/* maximum length of CTAGS option with null */
#endif

/*	The following separators are permitted for list options.
 */
#define HEADER_SEPARATORS	".,;"		/* header extensions */

/*	Some hard to read characters.
 */
#define NEWLINE			'\n'
#define CR				'\r'
#define DOUBLE_QUOTE	'"'
#define SINGLE_QUOTE	'\''
#define BACKSLASH		'\\'

/*============================================================================
=	Macros
============================================================================*/

/*  Is the character valid as a character of a C identifier?
 */
#define isident(c)		(isalnum(c) || (c) == '_')

/*  Is the character valid as the first character of a C identifier?
 */
#define isident1(c)		(isalpha(c) || (c) == '_')

#define isspacetab(c)	((c) == ' ' || (c) == '\t')

#define tagTypeName(type)	(((type) < TAG_NUMTYPES) ? TagTypeNames[type] : "?")

#define cppIgnore()		(Cpp.directive.ifdef[Cpp.directive.level].ignore)

/*	These may not be defined in a non-UNIX environment.
 */
#ifndef S_ISREG
# ifdef AMIGA
#  define S_ISREG(mode)		TRUE		/* Amiga only has regular files */
# else
#  define S_ISREG(mode)		((mode) & S_IFREG)
# endif
#endif

#ifndef S_ISLNK
# define S_ISLNK(mode)		FALSE
#endif

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
	char	*name;			/* the name of the token */
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
	token_t	prev;			/* the previous token */
	boolean	funcPtr;		/* whether 'name' is a function pointer */
	boolean	gotName;		/* whether a name has yet been parsed */
	boolean	inEnumBody;		/* currently within enumeration value list */
	tag_info tag;			/* information regarding current tag candidate */
	tag_info ptag;			/* information regarding previous tag candidate */
} statement_t;

/*============================================================================
=	Data definitions
============================================================================*/

/*	Holds a modified copy of the environment option.
 */
static char EnvArgList[MAXLINE];

/*	Note that the strings in this array must correspond to the types in the
 *	tag_t enumeration.
 */
static const char *TagTypeNames[] = {
	"tag", "def", "enum", "proto", "func", "type", "var"
};

static const char *const Help[] = {
 VERSION,
 "Usage: ctags [-aBdFLsStTwWx] [-{f|o} tagfile] [-h list] [-i types] file(s)",
 "    -a   Append the tags to an existing tag file.",
 "    -B   Use backward searching patterns (?...?).",
 "    -F   Use forward searching patterns (/.../) (default).",
 "    -o   Alternative for -f.",
 "    -w   Exclude warnings about duplicate tags (default).",
 "    -W   Generate warnings about duplicate tags.",
 "    -x   Print a tabular cross reference file to standard output.",
 "    -f <tagfile>",
 "         Output tags to the specified file (default is \"tags\").",
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
 "    -L <listfile>",
 "         A list of source file names are read from the specified file",
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

/*  Maintain separate name buffers for token names, defines (which are read by
 *  the local preprocessor) and names with parentheses.
 *
 *	Note: These buffers are here rather than explicitly in the tag_info
 *	structure because of how the Gnu debugger, gdb, prints out such a buffer.
 */
static struct {
	char	define[MAXNAME];		/* the name of the last define */
	char	name[MAXNAME];			/* the name of the last NAME token */
	char	prev[MAXNAME];			/* the name of the prev NAME token */
	char	pname[MAXNAME];			/* the name of the last NAME token in () */
} Buffer;

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
	boolean warnings;		/* -w  generate warnings about duplicate tags */
	boolean xref;			/* -x  generate xref output instead */
	const char *fileList;	/* -L  name of file containing names of files */
	const char *tagFile;	/* -o  name of tags file */
	const char *headerExt[MAX_HEADER_EXTENSIONS];	/* -h  header extensions */
#ifdef DEBUG
	int debugLevel;			/* -D  debugging output */
	long breakLine;			/* -#  source line at which to call lineBreak() */
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
	FALSE,				/* -w */
	FALSE,				/* -x */
	NULL,				/* -L */
	"tags",				/* -o */
	{ "h", "H", "hpp", "hxx", "h++", NULL },		/* -h */
#ifdef DEBUG
	0, 0,				/* -D, -# */
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
		{ 0, 0, Buffer.define },
		FALSE,
		0,
		{{FALSE,FALSE}}
	}
};

/*============================================================================
=	Function prototypes
============================================================================*/
#if !defined(__STDC__) && !defined(WIN32)
extern char *getenv();
extern void free();
extern void *malloc();
extern char *mktemp();
extern int stat();
extern int system();
extern int unlink();
#endif

#ifdef AMIGA
# include <fcntl.h>
#endif

#ifdef DEBUG
static void lineBreak __ARGS((void));
static void debugOpen __ARGS((const char name[]));
static void debugPutc __ARGS((const int c, const int level));
static void debugLabel __ARGS((const tag_t tagType, const char tagName[]));
#endif

static boolean isFileHeader __ARGS((const char name[]));
static boolean fileOpen __ARGS((const char name[]));
static void fileClose __ARGS((void));
static void fileUngetc __ARGS((int c));
static int fileGetc __ARGS((void));
static int fileCopyLine __ARGS((FILE *const fp, const long seek ));
static int fileCopyLineShort __ARGS((FILE *const fp, const long seek,
									 const int limit));

static void writeXrefEntry __ARGS((const tag_info *const tag,
								   const tag_t tag_type));
static void makeTag __ARGS((const tag_info *const tag, const scope_t scope,
							const tag_t tag_type));
static void makeDefineTag __ARGS((const tag_info *const tag,
								  const scope_t scope));

/*	Scanning functions.
 */
static boolean cppOpen __ARGS((const char name[]));
static void cppClose __ARGS((void));
static void cppUngetc __ARGS((const int c));
static boolean cppReadDirective __ARGS((int c, char *name));
static boolean cppReadIdentifier __ARGS((int c, tag_info *const tag));
static boolean pushCppIgnore __ARGS((const boolean ignore,
									 const boolean pathChosen));
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
static boolean skipToMatch __ARGS((const char pair[]));
static int skipInitializer __ARGS((const boolean inEnumBody));
static void readIdendifier __ARGS((const int firstChar, char name[]));
static int skipParameterDeclarations __ARGS((int c));
static boolean analyzeParens __ARGS((statement_t *const st));
static void analyzeIdentifier __ARGS((statement_t *const st));
static boolean nextToken __ARGS((statement_t *const st));

/*	Tag generation functions.
 */
static void initStatement __ARGS((statement_t *const st));
static boolean createTags __ARGS((const char name[]));
static boolean isSourceFile __ARGS((const char filename[]));
static const char *getNextListFile __ARGS((FILE *const fp));
static void createTagsForList __ARGS((const char listFile[]));
static void createTagsForArgs __ARGS((char *const argList[]));

/*	Tag sorting functions.
 */
#ifdef INTERNAL_SORT
static void fatal __ARGS(( const char msg[]));
static int compareTags __ARGS((const void *const one, const void *const two));
static void sortTags __ARGS((void));
#endif

/*	Start up code.
 */
static void printUsage __ARGS((const char error[]));
static void readExtensionList __ARGS((char list[]));
static void clearTagList __ARGS((void));
static void applyTagInclusionList __ARGS((const char list[]));
static char *readOptionArg __ARGS((const int option, char **const arg,
								   char *const argList[], int *const argNum));
static int parseOptions __ARGS((char *const argList[]));
static void parseEnvironmentOptions __ARGS((void));

/*============================================================================
=	Function definitions
============================================================================*/

#ifdef DEBUG

static void lineBreak() {}

static void debugOpen( name )
	const char name[];
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
	const char tagName[];
{
	if ((Option.debugLevel & 3) != 0)
	{
		printf("<#%s:%s#>", tagTypeName(tagType), tagName);
		fflush(stdout);
	}
}

#endif

/*	Determines whether the specified file name is considered to be a header
 *	file for the purposes of determining whether enclosed tags are global or
 *	static.
 */
static boolean isFileHeader( name )
	const char name[];
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
	const char name[];
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
		/*	Try to open the file for reading.  The file must be opened in
		 *	"binary" mode because otherwise fseek() would misbehave under DOS.
		 */
#if defined(MSDOS) || defined(OS2) || defined(WIN32)
		File.fp = fopen(name, "rb");
#else
		File.fp = fopen(name, "r");
#endif
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

/*	This function reads a single character from the stream.  If the
 *	previous character was a newline, then it also increments File.lineNumber
 *	and sets file_offset.
 */
static int fileGetc()
{
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

	/*	If previous character was a newline, then we're starting a line.
	 */
	if (File.afterNL)
	{
		File.afterNL = FALSE;
		File.seek	 = ftell(File.fp);
		File.lineNumber++;
#ifdef DEBUG
		if (Option.breakLine == File.lineNumber) lineBreak();
#endif
	}

	c = getc(File.fp);

	if (c == NEWLINE)
		File.afterNL = TRUE;

#ifdef DEBUG
	debugPutc(c, 1);
#endif
	return c;
}

/*	This function copies the current line out some other fp.  It has no
 *	effect on the fileGetc() function.  During copying, any '\' characters
 *	are doubled and a leading '^' or trailing '$' is also quoted.  The '\n'
 *	character is not copied.  If the '\n' is preceded by a '\r', then the
 *	'\r' isn't copied.
 *
 *	This is meant to be used when generating a tag line.
 */
static int fileCopyLine( fp, seek )
	FILE *const fp;		/* the output stream to copy it to */
	const long seek;	/* where the lines starts in the source file */
{
	long	oldseek;	/* where the file's pointer was before we changed it */
	char	c;			/* a single character from the file */
	char	next;		/* the next character from this file */
	int		length = 0;	/* number of charactes written */

	oldseek = ftell(File.fp);				/* go to the start of the line */
	fseek(File.fp, seek, 0);
	c = getc(File.fp);

	/*	Write everything up to, but not including, the newline.
	 */
	while (c != NEWLINE)
	{
		next = getc(File.fp);	/* preread the next character from this file */

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
		if (c != CR || next != NEWLINE)
		{
			putc(c, fp);
			++length;
		}

		c = next;
	}
	fseek(File.fp, oldseek, 0);			/* seek back to the old position */
	return length;
}

static int fileCopyLineShort( fp, seek, limit )
	FILE *const fp;		/* the output stream to copy it to */
	const long seek;	/* where the lines starts in the source file */
	const int limit;	/* maximum number of characters to write out */
{
	const long	oldseek = ftell(File.fp);	/* go to the start of the line */
	char	c;
	int		length;
	boolean	lineStarted = FALSE;

	fseek(File.fp, seek, 0);
	c = getc(File.fp);

	/*	Write everything up to, but not including, the newline.
	 */
	for (length = 0 ; c != NEWLINE  &&  length < limit ; )
	{
		char next = getc(File.fp);

		if (lineStarted  || ! isspace(c))		/* ignore leading spaces */
		{
			lineStarted = TRUE;
			if (isspace(c))
			{
				c = ' ';
				while (isspace(next)  &&  next != NEWLINE)
					next = getc(File.fp);
			}
			if (c != CR  ||  next != NEWLINE)
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
	length += fileCopyLineShort(TagFile.fp, tag->location, 30);
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
static void makeTag( tag, scope, tagType )
	const tag_info *const tag;
	const scope_t scope;
	const tag_t tagType;
{
	boolean include = FALSE;
	size_t length = 0;

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
			if (Option.include.prefix  &&  scope == SCOPE_STATIC)
			{
				fputs(File.name, TagFile.fp);
				putc(':', TagFile.fp);
				length = strlen(File.name) + 1;
			}
			length += fprintf(TagFile.fp, "%s\t%s\t", tag->name, File.name);
			length += fprintf(TagFile.fp, "%c^", Option.backward ? '?' : '/');
			length += fileCopyLine(TagFile.fp, tag->location);
			length += fprintf(TagFile.fp, "$%c\n", Option.backward ? '?' : '/');

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

static void makeDefineTag( tag, scope )
	const tag_info *const tag;
	const scope_t scope;
{
	if (Option.include.defines)
	{
		if (Option.xref)
			writeXrefEntry(tag, TAG_DEFINE);
		else
		{
			size_t length = 0;

			if (Option.include.prefix  &&  scope == SCOPE_STATIC)
			{
				fputs(File.name, TagFile.fp);
				putc(':', TagFile.fp);
				length = strlen(File.name) + 1;
			}
			length += fprintf(TagFile.fp, "%s\t%s\t%ld\n",
							  tag->name, File.name, tag->lineNumber);

			++TagFile.numTags;
			if (strlen(tag->name) > TagFile.max.tag)
				TagFile.max.tag = strlen(tag->name);
			if (length > TagFile.max.line)
				TagFile.max.line = length;
		}
#ifdef DEBUG
		debugLabel(TAG_DEFINE, tag->name);
#endif
	}
}

static boolean cppOpen( name )
	const char name[];
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
	int c, next;
	boolean ignore1char;				/* ignore 1 char and continue w/next */
	boolean ignore = FALSE;
	Comment comment = COMMENT_NONE;
	enum { RETURN, DIRECTIVE } mode = RETURN;

	if (Cpp.ungetch != 0)
	{
		c = Cpp.ungetch;
		Cpp.ungetch = 0;
		return c;			/* return here to avoid re-calling debugPutc() */
	}
	else do
	{
		ignore1char = FALSE;				/* reset each time through */

		c = fileGetc();
		switch (c)
		{
		case EOF:
			mode = RETURN;
			ignore1char = FALSE;
			ignore = FALSE;
			break;

		case NEWLINE:
			if (mode == DIRECTIVE  &&  ! ignore)
				mode = RETURN;
			Cpp.directive.accept = TRUE;
			break;
		case DOUBLE_QUOTE:
			Cpp.directive.accept = FALSE;
			c = skipToEndOfString();
			break;
		case '#':
			if (Cpp.directive.accept)
			{
				mode = DIRECTIVE;
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
		case BACKSLASH:
			next = fileGetc();
			if (next == NEWLINE)				/* check for line splicing */
				ignore1char = TRUE;
			else
			{
				Cpp.directive.accept = FALSE;
				fileUngetc(next);
			}
			break;
		default:
			if (! isspace(c))
			{
				Cpp.directive.accept = FALSE;
				if (mode == DIRECTIVE)
					ignore = handleDirective(c);
			}
			break;
		}
	} while (mode != RETURN  ||  ignore1char  ||  ignore);

#ifdef DEBUG
	debugPutc(c, 2);
#endif
	return c;
}

/*----------------------------------------------------------------------------
 *	Functions which analyze the file and break it into relevant tokens.
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
	const char pair[];
{
	const int initialLevel = Cpp.directive.level;
	boolean braceFormatting = (Option.braceFormat  &&  strcmp("{}", pair) == 0);
	int matchLevel = 1;
	boolean ok = TRUE;
	int c = 0;

	while (matchLevel > 0  &&  (c = cppGetc()) != EOF)
	{
		if (c == pair[0])
		{
			++matchLevel;
			if (braceFormatting  &&  Cpp.directive.level != initialLevel)
			{
				skipToFormattedBraceMatch();
				break;
			}
		}
		else if (c == pair[1])
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
	int c;

	c = skipToNonWhite();
	if (c == '{'  &&  ! skipToMatch("{}"))
		c = EOF;
	else while (c != EOF)
	{								/* skip to next ';' or ',' */
		c = cppGetc();
		if (c == ';'  ||  c == ','  ||  (c == '}'  &&  inEnumBody))
			break;
		else if ((c == '{'  ||  c == '}') && ! Option.braceFormat)
			c = EOF;
	}
	return c;
}

/*	Read a C identifier beginning with "firstChar" and places it into
 *	"name".
 */
static void readIdendifier( firstChar, name )
	const int firstChar;
	char name[];
{
	int c, i;

	name[0] = firstChar;
	for (i = 1, c = cppGetc(); i < MAXNAME - 1 && isident(c);
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
	if (c == '*')
	{
		st->gotName	= FALSE;				/* invalidate previous name */
		st->funcPtr = TRUE;
		st->token	= TOK_IGNORE;
	}
	else
	{
		boolean gotName = FALSE;
		long location = 0, lineNumber = 0;
		char *const name = Buffer.pname;

		if (isident1(c))
		{
			readIdendifier(c, name);
			location = File.seek;
			lineNumber = File.lineNumber;
			gotName = TRUE;
		}
		else
		{
			/*	A double paren almost certainly means on of those ANSI
			 *	prototype macro thingies.
			 */
			if (c == '('  &&  st->gotName  &&  Option.include.prototypes)
			{
				strcpy(st->tag.name, st->ptag.name);
				st->tag.location	= st->ptag.location;
				st->tag.lineNumber	= st->ptag.lineNumber;
			}
			cppUngetc(c);
		}
		ok = skipToMatch("()");
		if (ok)
		{
			/*	At this point we should be at the character following the
			*	closing parenthesis.
			*/
			c = skipToNonWhite();
			if (st->gotName  &&  (c == '{'  ||  c == ';'))
				st->token = TOK_ARGS;
			else if (isident1(c))
			{
				st->token = TOK_ARGS;
				c = skipParameterDeclarations(c);
			}
			/*	The name inside the parentheses must have been a function or
			*	variable name.
			*/
			else if (gotName)
			{
				st->gotName			= TRUE;
				st->token			= TOK_NAME;
				st->tag.location	= location;
				st->tag.lineNumber	= lineNumber;
				strcpy(st->tag.name, name);
			}
			cppUngetc(c);
		}
	}
	return ok;
}

/*	Analyzes the identifier contained in a statement described by the
 *	statement structure and adjusts the structure according the significance
 *	of the identifier.
 */
static void analyzeIdentifier( st )
	statement_t *const st;
{
	boolean matched = FALSE;

#define match(word)	((strcmp(st->tag.name,(word)) == 0) && (matched = TRUE))

	st->token = TOK_SPEC;			/* default unless otherwise */

	/*	Is it a reserved word?
	 */
	switch (st->tag.name[0])
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

	if (! matched)
	{
		st->gotName			= TRUE;
		st->token			= TOK_NAME;
		st->tag.location	= File.seek;
		st->tag.lineNumber	= File.lineNumber;
	}
}

/*	Reads characters from the pre-processor and assembles tokens, setting
 *	the current statement state.
 */
static boolean nextToken( st )
	statement_t *const st;
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
			if (st->declaration == DECL_ENUM  &&  st->prev != TOK_ARGS)
			{
				st->inEnumBody = TRUE;
				st->token = TOK_BODY;
			}
			else
			{
				st->token = TOK_BODY;
				ok = skipToMatch("{}");
			}
			break;

		case '}':
			if (st->inEnumBody)
				st->token = TOK_ENUM_BODY_END;
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
			else if (st->funcPtr  &&  st->gotName)
				st->token = TOK_IGNORE;		/* avoid possible distractions */
			else							/* start of a name or keyword */
			{
				if (Option.include.prototypes)
				{
					/*	Save away the current name as the previous name.
					 */
					strcpy(st->ptag.name, st->tag.name);
					st->ptag.location	= st->tag.location;
					st->ptag.lineNumber	= st->tag.lineNumber;
				}
				readIdendifier(c, st->tag.name);
				analyzeIdentifier(st);
			}
		}
	} while (ok  &&  st->token == TOK_IGNORE);

	return ok;
}

/*----------------------------------------------------------------------------
 *	Parsing functions
 *--------------------------------------------------------------------------*/

static void initStatement( st )
	statement_t *const st;
{
	st->scope			= SCOPE_GLOBAL;
	st->declaration		= DECL_UNSPEC;
	st->token			= TOK_SEMICOLON;
	st->prev			= TOK_SEMICOLON;
	st->gotName			= FALSE;
	st->funcPtr			= FALSE;
	st->inEnumBody		= FALSE;
	st->tag.location	= 0;
	st->tag.lineNumber	= 0;
	st->tag.name		= Buffer.name;
	st->tag.name[0]		= '\0';

	if (Option.include.prototypes)
	{
		st->ptag.location	= 0;
		st->ptag.lineNumber	= 0;
		st->ptag.name		= Buffer.prev;
		st->ptag.name[0]	= '\0';
	}
}

/*	Parses the source file and decides whether to write out and tags that
 *	are discovered.
 */
static boolean createTags( name )
	const char name[];
{
	statement_t st;
	boolean ok = TRUE;

	if (cppOpen(name))
	{
		initStatement(&st);

		for ( ; (ok = nextToken(&st)) != 0; st.prev = st.token)
		{
			if (st.token == TOK_EOF)
				break;

			/*	If NAME BODY, then NAME is possibly an enum/struct/union tag or
			 *	new type (class/enum/struct/union in C++).
			 */
			if (st.token == TOK_BODY  &&  st.prev == TOK_NAME)
			{
				if (st.declaration == DECL_CLASS  ||
					st.declaration == DECL_ENUM   ||
					st.declaration == DECL_STRUCT ||
					st.declaration == DECL_UNION)
				{
					if (File.header)
						makeTag(&st.tag, SCOPE_GLOBAL, TAG_BLOCKTAG);
					else if (Option.include.statics)
						makeTag(&st.tag, SCOPE_STATIC, TAG_BLOCKTAG);
				}
			}

			/*	If ENUM (BODY_END || (inEnumBody && NAME COMMA)), then any
			 *	previous NAME is an enumeration value.
			 */
			else if (st.token == TOK_ENUM_BODY_END  ||
					 (st.inEnumBody  &&  st.token == TOK_COMMA))
			{
				if (st.prev == TOK_NAME)
				{
					if (File.header)
						makeTag(&st.tag, SCOPE_GLOBAL, TAG_ENUM);
					else if (Option.include.statics)
						makeTag(&st.tag, SCOPE_STATIC, TAG_ENUM);
				}
			}

			/*	If NAME ARGS BODY, then NAME is a function (ANSI style).
			 */
			else if (st.token == TOK_BODY && st.prev == TOK_ARGS && st.gotName)
			{
				if (st.scope != SCOPE_STATIC  ||  Option.include.statics)
				{
					if (st.scope == SCOPE_EXTERN)  /* allowed for func. def. */
						st.scope = SCOPE_GLOBAL;
					makeTag(&st.tag, st.scope, TAG_FUNCTION);
				}
			}

			/*	If SEMICOLON or COMMA, then NAME is either a variable,
			 *	typedef, or function declaration;
			 */
			else if (st.token == TOK_SEMICOLON  ||  st.token == TOK_COMMA)
			{
				if (st.prev == TOK_NAME  ||  (st.funcPtr  &&  st.gotName))
				{
					if (st.scope == SCOPE_TYPEDEF)
					{
						if (File.header)
							makeTag(&st.tag, SCOPE_GLOBAL, TAG_TYPEDEF);
						else if (Option.include.statics)
							makeTag(&st.tag, SCOPE_STATIC, TAG_TYPEDEF);
					}
					else
					{
						if (st.scope == SCOPE_GLOBAL || Option.include.statics)
							makeTag(&st.tag, st.scope, TAG_VARIABLE);
					}
				}
				else if (st.prev == TOK_ARGS)
				{
					if (File.header  &&
						(st.scope == SCOPE_GLOBAL || st.scope == SCOPE_EXTERN))
					{
						makeTag(&st.tag, SCOPE_GLOBAL, TAG_FUNCDECL);
					}
				}
			}

			/*	Reset after a semicolon or ARGS BODY pair.
			 */
			if (st.token == TOK_SEMICOLON  ||
				(st.token == TOK_BODY  &&  st.prev == TOK_ARGS))
			{
				initStatement(&st);
				Cpp.directive.resolve = FALSE;		/* end of statement */
			}
			else
				Cpp.directive.resolve = TRUE;		/* in middle of statement */
		}
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
	const char filename[];
{
	static char line[BUFSIZ];
	boolean isSource = FALSE;		/* we assume not until found otherwise */
	FILE *fp;
	char *pattern;
	char *tag;
	char *tagPrefix;
	char *tagFname;

	fp = fopen(filename, "r");
	if (fp != NULL  &&  fgets(line, BUFSIZ, fp) != NULL)
	{
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = 0;		/* replace '\n' with null */
		tagFname = strchr(line, '\t');		/* find tab following tag */
		if (tagFname == NULL)
			isSource = TRUE;			/* missing tab means invalid tag line */
		else
		{
			char *p;

			*tagFname = 0;				/* null terminate the tag */
			++tagFname;					/* filename begins following 1st tab */
			tag = strchr(line, ':');	/* check for filename:tag format */
			if (tag == NULL)
			{
				tagPrefix = NULL;		/* no tag filename prefix */
				tag = line;
			}
			else
			{
				tagPrefix = line;		/* tag prefix begins at start of line */
				*tag = 0;				/* null terminate the tag prefix */
				++tag;					/* tag follows tag prefix */
			}
			pattern = strchr(tagFname, '\t');  /* find tab following filename */
			if (pattern == NULL)
				isSource = TRUE;		/* missing tab means invalid tag line */
			else
			{
				*pattern = 0;			/* null terminate the filename */
				++pattern;				/* pattern begins after filename */
			
				/*	If the tag has a filename prefix, it must be the same
				 *	filename as the required filename.
				 */
				if (tagPrefix != NULL  &&  strcmp(tagPrefix, tagFname) != 0)
					isSource = TRUE;

				/*	The tag must be a valid C identifier.
				 */
				if (! isident1(tag[0]))
					isSource = TRUE;
				else for (p = tag + 1 ; *p != 0 ; ++p)
				{
					if (! isident(*p))
					{
						isSource = TRUE;
						break;
					}
				}

				/*	If the pattern does not begin with either '/' or '?',
				 *	or the first character and last characters do not match,
				 *	then the pattern must be a line number.
				 */
				if ((pattern[0] != '/'  &&  pattern[0] != '?')  ||
					pattern[0] != pattern[strlen(pattern) - 1])
				{
					for (p = pattern ; *p != 0 ; ++p)
					{
						if (! isdigit(*p))
						{
							isSource = TRUE;
							break;
						}
					}
				}
			}
		}
	}
	if (fp != NULL)
		fclose(fp);
	return isSource;
}

static const char *getNextListFile( fp )
	FILE *const fp;
{
	static char fileName[MAXPATH];
	const char *buf = fgets(fileName, MAXPATH, fp);

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

static void createTagsForList( listFile )
	const char listFile[];
{
	const char *fileName;
	FILE *fp;

	/*	Read the file names from the specified file if provided.
	 */
	fp = fopen(listFile, "r");
	if (fp == NULL)
	{
		perror(listFile);
		exit(1);
	}
	fileName = getNextListFile(TagFile.fp);
	while (fileName != NULL)
	{
		const long startPos = ftell(TagFile.fp);
		const size_t numTags = TagFile.numTags;

		if (! createTags(fileName)  &&  ! Option.braceFormat)
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
			fileName = getNextListFile(TagFile.fp);
		}
	}
	fclose(fp);
}

static void createTagsForArgs( argList )
	char *const argList[];
{
	int argNum = 0;

	/*	Generate tags for each source file on the command line.
	 */
	while (argList[argNum] != NULL)
	{
		const long startPos = ftell(TagFile.fp);
		const size_t numTags = TagFile.numTags;

		if (! createTags(argList[argNum])  &&  ! Option.braceFormat)
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
 *	These functions provide a basic internal sort. No great memory
 *	optimization is performed (e.g. recursive subdivided sorts),
 *	so have lots of memory if you have large tag files.
 *--------------------------------------------------------------------------*/
#ifdef INTERNAL_SORT

static void fatal( msg )
	const char msg[];
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

static void sortTags()
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

	if (Option.xref)
	{
		/*	Copy the sorted lines to standard output.
		 */
		for (i = 0 ; i < TagFile.numTags ; ++i)
			fputs(table[i], stdout);
	}
	else
	{
		/*	Write the sorted lines back into the tag file.
		 */
		TagFile.fp = fopen(TagFile.name, "w");
		if (TagFile.fp == NULL)
			fatal(TagFile.name);
		for (i = 0 ; i < TagFile.numTags ; ++i)
		{
			if (i == 0  ||  strcmp(table[i], table[i-1]) != 0)
				if (fputs(table[i], TagFile.fp) == EOF)
					fatal(TagFile.name);
		}
		fclose(TagFile.fp);
	}

#ifdef DEBUG
	if (Option.debugLevel & 4)
		printf("malloc: %ld bytes\n", (long)mallocSize);
#endif
	for (i = 0 ; i < TagFile.numTags ; ++i)
		free(table[i]);
	free(table);
	free(buffer);
}

#endif

/*----------------------------------------------------------------------------
 *	Start up and terminate functions
 *--------------------------------------------------------------------------*/

static void printUsage( error )
	const char error[];
{
	int i;

	for (i = 0 ; Help[i] != NULL ; ++i)
	{
		fputs(Help[i], stderr);
		fputc('\n', stderr);
	}

	if (error != NULL)
		fprintf(stderr, "\nError: %s\n\n", error);

	exit(1);
}

/*	Reads a comma separated list of header file extensions.
 */
static void readExtensionList( list )
	char list[];
{
	int extIndex;			/* index of current extension */
	char *extension = list;	/* extension currently being examined */
	char *p;				/* current character of list under examination */

	/*	For each extension supplied in list...
	 */
	extension = list;
	for (extIndex = 0, p = 0 ;
		 *p != '\0'  &&  extIndex < MAX_HEADER_EXTENSIONS ; ++p)
	{
		if (strchr(HEADER_SEPARATORS, *p) != NULL)
		{
			*p = 0;								/* replace with null */
			if (strlen(extension) > 0)
			{
				Option.headerExt[extIndex++] = extension;
#ifdef DEBUG
				if (Option.debugLevel & 4)
					printf("header extension: %s\n", extension);
#endif
			}
			extension = p + 1;
		}
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
	const char list[];
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

static char *readOptionArg(option, arg, argList, argNum)
	const int option;
	char **const arg;
	char *const argList[];
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
	char *const argList[];
{
	int	argNum;

	/*	Parse the option flags.
	 */
	for (argNum = 0  ;  argList[argNum] != NULL  ;  ++argNum)
	{
		char c;
		char *arg = argList[argNum];
		char *list = NULL;

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
			case 's': Option.include.statics	= TRUE;
					  Option.include.prefix		= TRUE;		break;
			case 'S': Option.include.statics	= TRUE;
					  Option.include.prefix		= FALSE;	break;
			case 't': Option.include.typedefs	= TRUE;		break;
			case 'T': Option.include.blockTags	= TRUE;
					  Option.include.typedefs	= TRUE;		break;
			case 'w': Option.warnings			= FALSE;	break;
			case 'W': Option.warnings			= TRUE;		break;
			case 'x': Option.xref				= TRUE;		break;

			/*	Options requiring parameters.
			 */
			case 'f':
			case 'o':
				Option.tagFile = readOptionArg(c, &arg, argList, &argNum);
				break;

			case 'h':
				{
					struct stat status;

					list = readOptionArg(c, &arg, argList, &argNum);

					/*	Check to make sure that the user did not enter
					 *	"ctags -h *.c" by testing to see if the list is
					 *	a filename that exists.
					 */
					if (stat(list, &status) == 0)
						printUsage("-h: Invalid list");
					else
						readExtensionList(list);
				}
				break;

			case 'i':
				list = readOptionArg(c, &arg, argList, &argNum);
				applyTagInclusionList(list);
				break;

			case 'L':
				Option.fileList = readOptionArg(c, &arg, argList, &argNum);
				break;

#ifdef DEBUG
			case 'D':
				list = readOptionArg(c, &arg, argList, &argNum);
				Option.debugLevel = atoi(list);
				break;
			case 'N':
				list = readOptionArg(c, &arg, argList, &argNum);
				Option.debugLevel = atol(list);
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
#ifdef __EMX__
	int argc;
	char *argv[];
#else
	const int argc;
	char *const argv[];
#endif
{
	int ret = 0;
	int argNum;

#ifdef __EMX__
	_wildcard(&argc, &argv);    /* expand wildcards in argument list */
#endif

	parseEnvironmentOptions();
	argNum = 1 + parseOptions(&argv[1]);		/* 0th arg is program name */

	/*	There must always be at least one source file or a file list.
	*/
	if (argNum == argc  &&  Option.fileList == NULL)
		printUsage(NULL);
	else
	{
		static char tempName[20];

		/*	Open the tags File.
		*/
		if (Option.xref)
		{
			strcpy(tempName, XREF_TMP);
			TagFile.name = mktemp(tempName);
			TagFile.fp = fopen(TagFile.name, "w");
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

		if (Option.fileList != NULL)
			createTagsForList(Option.fileList);
		createTagsForArgs(&argv[argNum]);

		fclose(TagFile.fp);

#ifdef INTERNAL_SORT
		sortTags();
#else
	  {
		static char cmd[BUFSIZ];

		sprintf(cmd, "sort -u %s -o %s", TagFile.name, TagFile.name);
		ret = system(cmd);
		if (ret == 0  &&  Option.warnings)
		{
			sprintf(cmd, "awk 'BEGIN{FS=\"\\t\"}{if ($1==prev){printf(\"Duplicate entry in files %%s and %%s: %%s\\n\",$2,prevfile,$1) | \"cat 1>&2\"} else {prev=$1;prevfile=$2}}' %s", TagFile.name);
			ret = system(cmd);
		}
		if (Option.xref)
		{
			sprintf(cmd, "cat %s", TagFile.name);
			ret = system(cmd);
		}
	  }
#endif
		if (Option.xref)
#ifdef AMIGA
			remove(TagFile.name);
#else
			unlink(TagFile.name);
#endif
	}

	exit(ret);
	return 0;
}
