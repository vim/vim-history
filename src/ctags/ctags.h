/*****************************************************************************
*   $Id: ctags.h,v 6.13 1998/08/13 01:32:38 darren Exp $
*
*   Copyright (c) 1996-1998, Darren Hiebert
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

#if defined(MSDOS) || defined(WIN32) || defined(OS2) || defined(AMIGA)
# define HAVE_STDLIB_H
#endif

#if defined(__STDC__) || defined(MSDOS) || defined(WIN32) || defined(OS2)
# define ENABLE_PROTOTYPES
#endif

#if defined(HAVE_STDLIB_H)
# include <stdlib.h>	/* to declare alloc funcs and getenv() */
#endif

/*============================================================================
=   Portability defines
============================================================================*/
#if defined(MSDOS) || defined(WIN32) || defined(OS2) || defined(AMIGA) || defined(HAVE_OPENDIR)
# define RECURSE_SUPPORTED
#endif

/*  Determine whether to use prototypes or simple declarations.
 */
#ifndef __ARGS
# ifdef ENABLE_PROTOTYPES
#  define __ARGS(x) x
# else
#  define __ARGS(x) ()
# endif
#endif

/*  This is a helpful internal feature of later versions (> 2.7) of GCC
 *  to prevent warnings about unused variables.
 */
#if __GNUC__ > 2  ||  (__GNUC__ == 2  &&  __GNUC_MINOR__ >= 7)
# define __unused__	__attribute__((unused))
#else
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
# define PROGRAM_VERSION	"2.3"
#endif
#define PROGRAM_NAME	"Exuberant Ctags"
#define PROGRAM_URL	"http://darren.hiebert.com/ctags/index.html"
#define AUTHOR_NAME	"Darren Hiebert"
#define AUTHOR_EMAIL	"darren@hiebert.com"

#define CTAGS_FILE	"tags"
#define ETAGS_FILE	"TAGS"

#define PSEUDO_TAG_PREFIX	"!_"

/*============================================================================
=   Macros
============================================================================*/

#ifdef DEBUG
# define debug(level)	((Option.debugLevel & (int)(level)) != 0)
# define DebugStatement(x)	x
#else
# define DebugStatement(x)
# ifndef NDEBUG
#  define NDEBUG
# endif
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

typedef enum _langType {
    LANG_AUTO = -2,		/* automatically determine language */
    LANG_IGNORE = -1,		/* ignore file (uunknown/nsupported language) */
    LANG_C,			/* ANSI C */
    LANG_CPP,			/* C++ */
    LANG_JAVA,			/* Java */
    LANG_COUNT			/* count of languages */
} langType;

/*  Maintains the state of the current source file.
 */
typedef struct _sourceFile {
    const char	*name;		/* name of the current file */
    FILE	*fp;		/* stream used for reading the file */
    unsigned long lineNumber;	/* line number in the current file */
    long	seek;		/* fseek() offset to start of current line */
    int		ungetch;	/* a single character that was ungotten */
    boolean	afterNL;	/* was previous character a newline? */
    boolean	isHeader;	/* is current file a header file? */
    langType	language;	/* language of current file */
} sourceFile;

/*  This stores the command line options.
 */
typedef struct _optionValues {
    struct _include {		/* include tags for: */
	boolean	classNames;	/* -ic  class names */
	boolean	defines;	/* -id  defines */
	boolean	enumerators;	/* -ie  enumeration value */
	boolean	functions;	/* -if  functions */
	boolean	enumNames;	/* -ig  enum names */
	boolean	interfaceNames;	/* -ii  interface names */
	boolean	members;	/* -im  data members */
	boolean	namespaceNames;	/* -in  namespace names */
	boolean	prototypes;	/* -ip  function prototypes */
	boolean	structNames;	/* -is  struct names */
	boolean	typedefs;	/* -it  typedefs */
	boolean	unionNames;	/* -iu  unions */
	boolean	variables;	/* -iv  variables */
	boolean	externVars;	/* -ix  extern variables */

	boolean classPrefix;	/* -iC  include tag entries of class::member */
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
    boolean recurse;	    /* -R  recurse into directories */
    boolean sorted;	    /* -u,--sort  sort tags */
    boolean xref;	    /* -x  generate xref output instead */
    const char *fileList;   /* -L  name of file containing names of files */
    const char *tagFileName;/* -o  name of tags file */
    const char *const *headerExt; /* -h  header extensions */
#ifdef DEBUG
    int debugLevel;	    /* -D  debugging output */
    unsigned long breakLine;/* -b  source line at which to call lineBreak() */
#endif
    boolean startedAsEtags;
    boolean braceFormat;    /* use brace formatting to detect end of block */
    unsigned int tagFileFormat; /* --format  tag file format (level) */
    boolean if0;	    /* --if0  examine code within "#if 0" branch */
    langType language;	    /* --lang specified language override */
    const char *const *langMap[(int)LANG_COUNT];
			    /* --langmap  language-extension map */
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
    TAG_INTERFACE,		/* interface declaration */
    TAG_MEMBER,			/* structure, class or interface member */
    TAG_NAMESPACE,		/* namespace name */
    TAG_SOURCE_FILE,		/* source file */
    TAG_STRUCT,			/* structure name */
    TAG_TYPEDEF,		/* typedef name */
    TAG_UNION,			/* union name */
    TAG_VARIABLE,		/* variable definition */
    TAG_EXTERN_VAR,		/* external variable declaration */
    TAG_NUMTYPES		/* must be last */
} tagType;

#ifdef DEBUG

/*  Defines the debugging levels.
 */
enum _debugLevels {
    DEBUG_CPP	 = 1,		/* echo characters out of pre-processor */
    DEBUG_PARSE	 = 2,		/* echo parsing results */
    DEBUG_STATUS = 4,		/* echo file status information */
    DEBUG_OPTION = 8,		/* echo option parsing */
    DEBUG_RAW	 = 16		/* echo raw (filtered) characters */
};

#endif

/*----------------------------------------------------------------------------
 *  Used for describing a statement
 *--------------------------------------------------------------------------*/

/*  This describes the scoping of the current statement.
 */
typedef enum _tagScope {
    SCOPE_GLOBAL,		/* no storage class specified */
    SCOPE_STATIC,		/* static storage class */
    SCOPE_EXTERN,		/* external storage class */
    SCOPE_FRIEND,		/* declares visibility only */
    SCOPE_TYPEDEF		/* scoping depends upon context */
} tagScope;

/*  Information about the current tag candidate.
 */
typedef struct _tagInfo {
    long    location;		/* file position of line containing name */
    unsigned long lineNumber;	/* line number of tag */
    char    name[MaxNameLength];/* the name of the token */
} tagInfo;

typedef enum _memberType {
    MEMBER_NONE,
    MEMBER_ENUM, MEMBER_CLASS, MEMBER_INTERFACE, MEMBER_NAMESPACE,
    MEMBER_STRUCT, MEMBER_UNION
} memberType;

typedef enum _visibilityType {
    VIS_UNDEFINED, VIS_PUBLIC, VIS_PROTECTED, VIS_PRIVATE
} visibilityType;

/*  Information about the parent class of a member (if any).
 */
typedef struct _memberInfo {
    memberType type;
    visibilityType visibility;	/* current visibility section, if known */
    boolean persistent;		/* persistent across multiple statements? */
    char parent[MaxNameLength];	/* name of the parent data type */
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
extern boolean isDirectory __ARGS((const char *const name));
extern boolean doesFileExist __ARGS((const char *const fileName));
extern void addTotals __ARGS(( const unsigned int files, const unsigned long lines, const unsigned long bytes));
extern char *readLine __ARGS((lineBuf *const pLineBuf, FILE *const fp));
extern const char *getExecutableName __ARGS((void));

extern const char *getTypeString __ARGS((const memberType mType));
extern const char *getVisibilityString __ARGS((const visibilityType visibility));
extern void makeTag __ARGS((const tagInfo *const tag,
			    const memberInfo *const pMember,
			    const tagScope scope, const tagType type));
extern void makeDefineTag __ARGS((const tagInfo *const tag,
				  const tagScope scope,
				  const boolean parameterized));
extern const char *tagTypeName __ARGS((const tagType type));
extern void addPseudoTags __ARGS((void));
extern unsigned long updatePseudoTags __ARGS((void));

extern boolean cppOpen __ARGS((const char *const name, const langType language, const boolean isHeader));
extern void cppClose __ARGS((void));
extern void cppUngetc __ARGS((const int c));
extern int cppGetc __ARGS((void));

extern boolean createTags __ARGS((const unsigned int nesting, const void *const parent));
extern void buildKeywordHash __ARGS((void));

extern boolean fileOpen __ARGS((const char *const name, const langType language, const boolean isHeader));
extern void fileClose __ARGS((void));
extern int fileGetc __ARGS((void));
extern void fileUngetc __ARGS((int c));
extern void freeLineBuffer __ARGS((lineBuf *const pLine));
extern char *getSourceLine __ARGS((lineBuf *const pLineBuffer, const long int location));
extern boolean isIgnoreToken __ARGS((const char *const name));
extern void freeIgnoreList __ARGS((void));
extern const char *getLanguageName __ARGS((const langType language));
extern boolean strequiv __ARGS((const char *s1, const char *s2));
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
extern void debugPrintf __ARGS((const enum _debugLevels level, const char *const format, ... ));
extern void debugOpen __ARGS((const char *const fileName, const boolean isHeader, const langType language));
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

/* vi:set tabstop=8 shiftwidth=4: */
