/*****************************************************************************
*   $Id: ctags.h,v 5.3 1998/02/26 05:32:07 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module is a global include file.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>	/* to declare isalnum(), isalpha(), isspace() */

#if defined(HAVE_STDLIB_H) || defined(__STDC__)
# include <stdlib.h>	/* to declare alloc funcs and getenv() */
#endif

/*============================================================================
=   Portability defines
============================================================================*/

/*  Determine whether to use prototypes or simple declarations.
 */
#ifndef __ARGS 
# ifdef __STDC__
#  define __ARGS(x) x
# else
#  define __ARGS(x) ()
# endif
#endif

#ifndef __unused__
# define __unused__
#endif

/*  MS-DOS doesn't allow manipulation of standard error, so we send it to
 *  stdout instead.
 */
#if defined(MSDOS) || defined(WIN32)
# define errout	stdout
#else
# define errout	stderr
#endif

/*============================================================================
=   General defines
============================================================================*/
#ifndef PROGRAM_VERSION
# define PROGRAM_VERSION	"2.0.2"
#endif
#define PROGRAM_NAME	"Exuberant Ctags"
#define PROGRAM_URL	"http://home.hiwaay.net/~darren/ctags"
#define AUTHOR_NAME	"Darren Hiebert"
#define AUTHOR_EMAIL	"darren@hiebert.com"

#define CTAGS_FILE	"tags"
#define ETAGS_FILE	"TAGS"

#define PSEUDO_TAG_PREFIX	"!_"

/*============================================================================
=   Macros
============================================================================*/

#ifdef DEBUG
# define debug(level)	((Option.debugLevel & (level)) != 0)
# define DebugStatement(x)	x
#else
# define NDEBUG
# define DebugStatement(x)
#endif

/*  For convenience and safety.
 */
#define stringMatch(s1,s2)	(strcmp(s1,s2) == 0)

/*  Is the character valid as a character of a C identifier?
 */
#define isident(c)	(isalnum(c) || (c) == '_')

/*  Is the character valid as the first character of a C identifier?
 */
#define isident1(c)	(isalpha(c) || (c) == '_' || (c) == '~')

/*  Is that character a space or a tab?
 */
#define isspacetab(c)	((c) == ' ' || (c) == '\t')

/*============================================================================
=   Data declarations
============================================================================*/

#undef FALSE
#undef TRUE
typedef enum { FALSE, TRUE } boolean;

typedef int errorSelection;
enum _errorTypes { FATAL = 1, WARNING = 2, PERROR = 4 };

enum _limits {
    MaxNameLength	= 256,	/* maximum length of token with null */
    MaxHeaderExtensions	= 100,	/* maximum number of extensions in -h option */
    MaxCppNestingLevel	= 20,
    MaxSupportedTagFormat = 2
};

enum _characters {
    /*  White space characters.
     */
    SPACE	= ' ',
    NEWLINE	= '\n',
    CRETURN	= '\r',
    FORMFEED	= '\f',
    TAB		= '\t',
    VTAB	= '\v',

    /*  Some hard to read characters.
     */
    DOUBLE_QUOTE  = '"',
    SINGLE_QUOTE  = '\'',
    BACKSLASH	  = '\\',

    STRING_SYMBOL = ('S' + 0x80),
    CHAR_SYMBOL	  = ('C' + 0x80)
};

typedef struct _lineBuf {
    int size;		/* size of buffer; 'int' because passed to fgets() */
    char *buffer;	/* buffer for placing line into */
} lineBuf;

/*  Maintains the state of the tag file.
 */
typedef struct _tagFile {
    const char *name;
    FILE *fp;
    struct _numTags { unsigned long added, prev; } numTags;
    struct _max { size_t line, tag, file; } max;
    struct _etags {
	char name[L_tmpnam];
	FILE *fp;
	size_t byteCount;
    } etags;
    lineBuf line;
} tagFile;

/*  Maintains the state of the current source file.
 */
typedef struct _sourceFile {
    const char *name;	    /* name of the current file */
    FILE    *fp;	    /* stream used for reading the file */
    unsigned long lineNumber;/* line number in the current file */
    long    seek;	    /* fseek() offset to the start of current line */
    boolean afterNL;	    /* boolean: was previous character a newline? */
    int	    ungetch;	    /* a single character that was ungotten */
    int	    header;	    /* boolean: is the current file a header file? */
    boolean warned;	    /* format warning kludge */
} sourceFile;

/*  This stores the command line options.
 */
typedef struct _optionValues {
    struct _include {
	boolean	classNames;	/* -ic  include tags for class names */
	boolean	defines;	/* -id  include tags for defines */
	boolean	enumerators;	/* -ie  include tags for enumeration value */
	boolean	functions;	/* -if  include tags for functions */
	boolean	enumNames;	/* -ig  include tags for enum names */
	boolean	members;	/* -im  include tags for struct/class members */
	boolean	prototypes;	/* -ip  include tags for function prototypes */
	boolean	structNames;	/* -is  include tags for struct names */
	boolean	typedefs;	/* -it  include tags for typedefs */
	boolean	unionNames;	/* -iu  include tags for unions */
	boolean	variables;	/* -iv  include tags for variables */

	boolean sourceFiles;	/* -iF  include tags for source files */
	boolean	statics;	/* -iS  include static tags */
    } include;
    struct _ignore {
	char **list;
	unsigned int count, max;
    } ignore;		    /* -I  name of file containing tokens to ignore */
    boolean append;	    /* -a  append to "tags" files */
    boolean backward;	    /* -B  regexp patterns search backwards */
    boolean etags;	    /* -e  output Emacs style tags file */
    enum _locate {
	EX_MIX,		    /* line numbers for defines, patterns otherwise */
	EX_LINENUM,	    /* -n  only line numbers in tag file */
	EX_PATTERN	    /* -N  only patterns in tag file */
    } locate;		    /* --excmd  EX command used to locate tag */
    const char *path;	    /* -p  default path for source files */
    boolean sorted;	    /* -u,--sort  sort tags */
    boolean warnings;	    /* -w  generate warnings about duplicate tags */
    boolean xref;	    /* -x  generate xref output instead */
    const char *fileList;   /* -L  name of file containing names of files */
    const char *tagFileName;/* -o  name of tags file */
    const char *headerExt[MaxHeaderExtensions + 1];/* -h  header extensions */
#ifdef DEBUG
    int debugLevel;	    /* -D  debugging output */
    unsigned long breakLine;/* -b  source line at which to call lineBreak() */
#endif
    boolean startedAsEtags;
    boolean braceFormat;    /* use brace formatting to detect end of block */
    unsigned int tagFileFormat; /* --format  tag file format (level) */
    boolean if0;	    /* --if0  examine code within "#if 0" branch */
    boolean printTotals;    /* --totals  print cumulative statistics */
} optionValues;

/*  Describes the type of tag being generated. This is used for debugging
 *  purposes only.
 */
typedef enum _tagType {
    TAG_CLASS,			/* class name */
    TAG_DEFINE_OBJ,		/* pre-processor define (object-like) */
    TAG_DEFINE_FUNC,		/* pre-processor define (function-like) */
    TAG_ENUM,			/* enumeration name */
    TAG_ENUMERATOR,		/* enumerator (enumeration value) */
    TAG_FUNCDECL,		/* function declaration */
    TAG_FUNCTION,		/* function (or method) definition */
    TAG_MEMBER,			/* structure or class member */
    TAG_SOURCE_FILE,		/* source file */
    TAG_STRUCT,			/* structure name */
    TAG_TYPEDEF,		/* typedef name */
    TAG_UNION,			/* union name */
    TAG_VARIABLE,		/* variable defintion */
    TAG_NUMTYPES		/* must be last */
} tagType;

#ifdef DEBUG

/*  Defines the debugging levels.
 */
enum _debugLevels {
    DEBUG_VISUAL = 1, DEBUG_CPP = 2, DEBUG_STATUS = 4, DEBUG_OPTION = 8
};

#endif

/*----------------------------------------------------------------------------
 *  Used for describing a statement
 *--------------------------------------------------------------------------*/

/*  This describes the scoping of the current statement.
 */
typedef enum _tagScope {
    SCOPE_GLOBAL,	    /* no storage class specified */
    SCOPE_STATIC,	    /* static storage class */
    SCOPE_EXTERN,	    /* external storage class */
    SCOPE_FRIEND,	    /* declares visibility only */
    SCOPE_TYPEDEF	    /* scoping depends upon context */
} tagScope;

/*  Information about the current tag candidate.
 */
typedef struct _tagInfo {
    long    location;		/* file position of line containing name */
    unsigned long lineNumber;		/* line number of tag */
    char    name[MaxNameLength];	/* the name of the token */
} tagInfo;

typedef enum _memberType {
    MEMBER_NONE, MEMBER_ENUM, MEMBER_CLASS, MEMBER_STRUCT, MEMBER_UNION
} memberType;

/*  Information about the parent class of a member (if any).
 */
typedef struct _memberInfo {
    memberType type;
    boolean persistent;		/* persistent across multiple statements? */
    char parent[MaxNameLength];		/* name of the parent data type */
} memberInfo;

/*  Defines the one nesting level of a preprocessor conditional.
 */
typedef struct _conditionalInfo {
    boolean ignoreAllBranches;	/* ignoring parent conditional branch */
    boolean singleBranch;	/* choose only one branch */
    boolean branchChosen;	/* branch already selected */
    boolean ignoring;		/* current ignore state */
} conditionalInfo;

/*  Defines the current state of the pre-processor.
 */
typedef struct _cppState {
    int	    ungetch;		/* an ungotten character, if any */
    boolean resolveRequired;	/* must resolve if/else/elif/endif branch */
    struct _directive {
	enum _state {
	    DRCTV_NONE,
	    DRCTV_HASH,
	    DRCTV_IF,
	    DRCTV_DEFINE
	} state;
	boolean	accept;		/* is a directive syntatically permitted? */
	tagInfo tag;		/* the name associated with the directive */
	unsigned int nestLevel;	/* level 0 is not used */
	conditionalInfo ifdef[MaxCppNestingLevel];
    } directive;
} cppState;

/*============================================================================
=   Global variables
============================================================================*/
extern tagFile		TagFile;
extern sourceFile	File;
extern optionValues	Option;
extern cppState		Cpp;
extern memberInfo	NoClass;

/*============================================================================
=   Function prototypes
============================================================================*/

extern void error __ARGS(( const errorSelection selection,
			   const char *const format, ... ));
extern unsigned long getFileSize __ARGS((const char *const name));
extern boolean isNormalFile __ARGS((const char *const name));
extern boolean doesFileExist __ARGS((const char *const fileName));
extern void addTotals __ARGS(( const unsigned int files, const unsigned long lines, const unsigned long bytes));
extern char *readLine __ARGS((lineBuf *const pLineBuf, FILE *const fp));
extern const char *getExecutableName __ARGS((void));

extern void makeTag __ARGS((const tagInfo *const tag,
			    const memberInfo *const pMember,
			    const tagScope scope, const tagType type));
extern void makeDefineTag __ARGS((const tagInfo *const tag,
				  const tagScope scope,
				  const boolean parameterized));
extern const char *tagTypeName __ARGS((const tagType type));
extern void addPseudoTags __ARGS((void));
extern unsigned long updatePseudoTags __ARGS((void));

extern boolean cppOpen __ARGS((const char *const name));
extern void cppClose __ARGS((void));
extern void cppUngetc __ARGS((const int c));
extern int cppGetc __ARGS((void));

extern boolean createTags __ARGS((const unsigned int nesting, const void *const parent));

extern boolean fileOpen __ARGS((const char *const name));
extern void fileClose __ARGS((void));
extern int fileGetc __ARGS((void));
extern void fileUngetc __ARGS((int c));
extern void freeLineBuffer __ARGS((lineBuf *const pLine));
extern char *getSourceLine __ARGS((lineBuf *const pLineBuffer, const long int location));
extern void freeIgnoreList __ARGS((void));
extern char *const *parseOptions __ARGS((char *const *const argList));
extern void *parseEnvironmentOptions __ARGS((void));

extern void catFile __ARGS((const char *const name));
#ifdef EXTERNAL_SORT
extern void externalSortTags __ARGS((const boolean toStdout));
#else
extern void internalSortTags __ARGS((const boolean toStdout));
#endif

#ifdef DEBUG
extern void lineBreak __ARGS((void));
extern void debugOpen __ARGS((const char *const name));
extern void debugPutc __ARGS((const int c, const int level));
extern void debugEntry __ARGS((const tagScope scope, const tagType type, const char *const tagName, const memberInfo *const pMember));
extern void debugParseNest __ARGS((const boolean increase, const unsigned int level));
extern void debugCppNest __ARGS((const boolean begin, const unsigned int ignore));
extern void debugCppIgnore __ARGS((const boolean ignore));
extern void clearString __ARGS((char *const string, const int length));
#endif

/*  Possibly missing system prototypes.
 */
#if defined(NEED_PROTO_REMOVE) && defined(HAVE_REMOVE) 
extern int remove __ARGS((const char *));
#endif

#if defined(NEED_PROTO_UNLINK) && !defined(HAVE_REMOVE)
extern void *unlink __ARGS((const char *));
#endif

#ifdef NEED_PROTO_MALLOC
extern void *malloc __ARGS((size_t));
#endif

#ifdef NEED_PROTO_GETENV
extern char *getenv __ARGS((const char *));
#endif

#ifdef NEED_PROTO_STRSTR
extern char *strstr __ARGS((const char *str, const char *substr));
#endif

#ifdef NEED_PROTO_TRUNCATE
extern int truncate __ARGS((const char *path, size_t length));
#endif

#ifdef NEED_PROTO_FTRUNCATE
extern int ftruncate __ARGS((int fd, size_t length));
#endif

/* vi:set tabstop=8 shiftwidth=4: */
