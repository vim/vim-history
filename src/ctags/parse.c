/*****************************************************************************
*   $Id: parse.c,v 6.6 1998/08/20 04:50:36 darren Exp $
*
*   Copyright (c) 1996-1998, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for parsing and scanning of a source file.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "ctags.h"

/*============================================================================
=   Macros
============================================================================*/
#define hashIndex(c)		((c) - '_')
#define activeTag(st)		((st)->tag[(int)(st)->buf1])
#define activeName(st)		(activeTag(st).name)
#define swapNameBuffers(st)	((st)->buf1 = (boolean)!(st)->buf1)

#define isExternCDecl(st,c)	((c) == STRING_SYMBOL  &&  !(st)->gotName  && \
				 (st)->scope == SCOPE_EXTERN)

/*============================================================================
=   Data declarations
============================================================================*/

/*  Used to specify type of keyword.
 */
typedef enum _keywordId {
    KEYWORD_UNKNOWN,
    KEYWORD_ABSTRACT, KEYWORD_ATTRIBUTE,
    KEYWORD_BOOLEAN, KEYWORD_BYTE,
    KEYWORD_CHAR, KEYWORD_CLASS, KEYWORD_CONST,
    KEYWORD_DOUBLE,
    KEYWORD_ENUM, KEYWORD_EXPLICIT, KEYWORD_EXTERN, KEYWORD_EXTENDS,
    KEYWORD_FINAL, KEYWORD_FLOAT, KEYWORD_FRIEND,
    KEYWORD_IMPLEMENTS, KEYWORD_IMPORT, KEYWORD_INLINE, KEYWORD_INT,
    KEYWORD_INTERFACE,
    KEYWORD_LONG,
    KEYWORD_MUTABLE,
    KEYWORD_NAMESPACE, KEYWORD_NEW, KEYWORD_NATIVE,
    KEYWORD_OPERATOR, KEYWORD_OVERLOAD,
    KEYWORD_PACKAGE, KEYWORD_PRIVATE, KEYWORD_PROTECTED, KEYWORD_PUBLIC,
    KEYWORD_REGISTER,
    KEYWORD_SHORT, KEYWORD_SIGNED, KEYWORD_STATIC, KEYWORD_STRUCT,
    KEYWORD_SYNCHRONIZED,
    KEYWORD_TEMPLATE, KEYWORD_THROW, KEYWORD_THROWS, KEYWORD_TRANSIENT,
    KEYWORD_TYPEDEF, KEYWORD_TYPENAME,
    KEYWORD_UNION, KEYWORD_UNSIGNED, KEYWORD_USING,
    KEYWORD_VIRTUAL, KEYWORD_VOID, KEYWORD_VOLATILE,
    KEYWORD_WCHAR_T
} keywordId;

/*  Used to determine whether keyword is valid for the current language and
 *  what its ID is.
 */
typedef struct _keywordDesc {
    const char *name;
    keywordId id;
    short isValid[LANG_COUNT]; /* indicates languages for which kw is valid */
} keywordDesc;

/*  Used for reporting the type of object parsed by nextToken().
 */
typedef enum _tokenType {
    TOK_NONE,		/* none */
    TOK_ARGS,		/* a parenthetical pair and its contents */
    TOK_BODY,		/* a brace enclosed block */
    TOK_COMMA,		/* the comma character */
    TOK_IGNORE,		/* a sequence not to be seen by createTags() */
    TOK_ENUM_BODY_END,	/* the beginning of a list of enumeration values */
    TOK_EOF,		/* end of file */
    TOK_NAME,		/* an unknown name */
    TOK_SEMICOLON,	/* the semicolon character */
    TOK_SPEC		/* a storage class specifier, qualifier, type, etc. */
} tokenType;

/*  Describes the statement currently undergoing analysis.
 */
typedef struct _statementInfo {
    tagScope	scope;
    enum _declaration {
	DECL_BASE,		/* base type (default) */
	DECL_CLASS,		/* C++ class */
	DECL_ENUM,		/* enumeration */
	DECL_IGNORE,		/* non-taggable "declaration" */
	DECL_INTERFACE,		/* interface */
	DECL_NAMESPACE,		/* namespace */
	DECL_STRUCT,		/* structure */
	DECL_UNION,		/* union */
	DECL_NOMANGLE		/* C++ name demangling block */
    } declaration;	/* describes specifier associated with TOK_SPEC */
    tokenType token;	/* the most recent type of token */
    tokenType prev[2];	/* the previous tokens */
    boolean gotName;	/* was a name parsed yet? */
    boolean isFuncPtr;	/* is 'name' a pointer? */
    boolean inEnumBody;	/* currently within enumeration value list? */
    boolean buf1;	/* is tag[1] the primary buffer? */
    tagInfo tag[2];	/* information regarding last 2 tag candidates */
    tagInfo class;	/* class declaration name info */
    memberInfo member;	/* information regarding parent class/struct */
} statementInfo;

/*  Information about an identifier within parentheses.
 */
typedef struct _parenInfo {
    char name[MaxNameLength];
    boolean gotName;
    long location;
    long lineNumber;
} parenInfo;

/*============================================================================
=   Data definitions
============================================================================*/

enum { HashSize = ('z' - '_' + 1) };	/* '_' through 'z' */
static short KeywordHash[(int)HashSize];

static const keywordDesc KeywordTable[] = {
    /* 						    C++		*/
    /* 					     ANSI C  |  Java	*/
    /* keyword		keyword ID		 \   |   /  	*/
    { "__attribute__",	KEYWORD_ATTRIBUTE,	{ 1, 1, 0 } },
    { "abstract",	KEYWORD_ABSTRACT,	{ 0, 0, 1 } },
    { "boolean",	KEYWORD_BOOLEAN,	{ 0, 0, 1 } },
    { "byte",		KEYWORD_BYTE,		{ 0, 0, 1 } },
    { "char",		KEYWORD_CHAR,		{ 1, 1, 1 } },
    { "class",		KEYWORD_CLASS,		{ 0, 1, 1 } },
    { "const",		KEYWORD_CONST,		{ 1, 1, 1 } },
    { "double",		KEYWORD_DOUBLE,		{ 1, 1, 1 } },
    { "enum",		KEYWORD_ENUM,		{ 1, 1, 0 } },
    { "explicit",	KEYWORD_EXPLICIT,	{ 0, 1, 0 } },
    { "extends",	KEYWORD_EXTENDS,	{ 0, 0, 1 } },
    { "extern",		KEYWORD_EXTERN,		{ 1, 1, 0 } },
    { "final",		KEYWORD_FINAL,		{ 0, 0, 1 } },
    { "float",		KEYWORD_FLOAT,		{ 1, 1, 1 } },
    { "friend",		KEYWORD_FRIEND,		{ 0, 1, 0 } },
    { "implements",	KEYWORD_IMPLEMENTS,	{ 0, 0, 1 } },
    { "import",		KEYWORD_IMPORT,		{ 0, 0, 1 } },
    { "inline",		KEYWORD_INLINE,		{ 0, 1, 0 } },
    { "int",		KEYWORD_INT,		{ 1, 1, 1 } },
    { "interface",	KEYWORD_INTERFACE,	{ 0, 0, 1 } },
    { "long",		KEYWORD_LONG,		{ 1, 1, 1 } },
    { "mutable",	KEYWORD_MUTABLE,	{ 0, 1, 0 } },
    { "namespace",	KEYWORD_NAMESPACE,	{ 0, 1, 0 } },
    { "native",		KEYWORD_NATIVE,		{ 0, 0, 1 } },
    { "new",		KEYWORD_NEW,		{ 0, 1, 1 } },
    { "operator",	KEYWORD_OPERATOR,	{ 0, 1, 0 } },
    { "overload",	KEYWORD_OVERLOAD,	{ 0, 1, 0 } },
    { "package",	KEYWORD_PACKAGE,	{ 0, 0, 1 } },
    { "private",	KEYWORD_PRIVATE,	{ 0, 1, 1 } },
    { "protected",	KEYWORD_PROTECTED,	{ 0, 1, 1 } },
    { "public",		KEYWORD_PUBLIC,		{ 0, 1, 1 } },
    { "register",	KEYWORD_REGISTER,	{ 1, 1, 0 } },
    { "short",		KEYWORD_SHORT,		{ 1, 1, 1 } },
    { "signed",		KEYWORD_SIGNED,		{ 1, 1, 0 } },
    { "static",		KEYWORD_STATIC,		{ 1, 1, 1 } },
    { "struct",		KEYWORD_STRUCT,		{ 1, 1, 0 } },
    { "synchronized",	KEYWORD_SYNCHRONIZED,	{ 0, 0, 1 } },
    { "template",	KEYWORD_TEMPLATE,	{ 0, 1, 0 } },
    { "throw",		KEYWORD_THROW,		{ 0, 1, 1 } },
    { "throws",		KEYWORD_THROWS,		{ 0, 0, 1 } },
    { "transient",	KEYWORD_TRANSIENT,	{ 0, 0, 1 } },
    { "typedef",	KEYWORD_TYPEDEF,	{ 1, 1, 0 } },
    { "typename",	KEYWORD_TYPENAME,	{ 0, 1, 0 } },
    { "union",		KEYWORD_UNION,		{ 1, 1, 0 } },
    { "unsigned",	KEYWORD_UNSIGNED,	{ 1, 1, 0 } },
    { "using",		KEYWORD_USING,		{ 0, 1, 0 } },
    { "virtual",	KEYWORD_VIRTUAL,	{ 0, 1, 0 } },
    { "void",		KEYWORD_VOID,		{ 1, 1, 1 } },
    { "volatile",	KEYWORD_VOLATILE,	{ 1, 1, 1 } },
    { "wchar_t",	KEYWORD_WCHAR_T,	{ 1, 1, 0 } }
};

static const size_t KeywordTableSize =
				sizeof(KeywordTable)/sizeof(KeywordTable[0]);

/*============================================================================
=   Function prototypes
============================================================================*/
static void initTag __ARGS((tagInfo *const tag));
static void initMemberInfo __ARGS((memberInfo *const pMember));
static void reinitStatement __ARGS((statementInfo *const st));
static void initStatement __ARGS((statementInfo *const st, const statementInfo *const parent));

/*  Tag generation functions.
 */
static void qualifyBlockTag __ARGS((const statementInfo *const st, const tagInfo *const tag, const tagScope declScope));
static void qualifyEnumeratorTag __ARGS((const statementInfo *const st, const tagInfo *const tag, const tagScope declScope));
static void qualifyFunctionTag __ARGS((statementInfo *const st, const tagInfo *const tag));
static void qualifyVariableTag __ARGS((const statementInfo *const st, const tagInfo *const tag));
static void qualifyFunctionDeclTag __ARGS((const statementInfo *const st, const tagInfo *const tag));

/*  Parsing functions.
 */
static int skipToNonWhite __ARGS((void));
static void skipToFormattedBraceMatch __ARGS((void));
static boolean skipToMatch __ARGS((const char *const pair));
static void readIdentifier __ARGS((const int firstChar, char *const name));
static void readOperator __ARGS((const int firstChar, char *const name));
static keywordId analyzeKeyword __ARGS((const char *const name));
static void processKeyword __ARGS((statementInfo *const st, keywordId keyword));
static int skipPostArgumentStuff __ARGS((int c, statementInfo *const st, const boolean emptyArgList));
static boolean analyzePostParens __ARGS((statementInfo *const st, const parenInfo *const paren, const boolean emptyArgList));
static void initParenInfo __ARGS((parenInfo *const paren));
static boolean saveParenInfo __ARGS((parenInfo *const paren, int c));
static boolean doubleParens __ARGS((statementInfo *const st));
static boolean analyzeParens __ARGS((statementInfo *const st));
static void analyzeIdentifier __ARGS((statementInfo *const st));
static boolean beginBlock __ARGS((statementInfo *const st, const unsigned int nesting));
static boolean endBlock __ARGS((statementInfo *const st, const unsigned int nesting));
static void processColon __ARGS((statementInfo *const st));
static int skipInitializer __ARGS((const boolean inEnumBody));
static boolean processInitializer __ARGS((statementInfo *const st));
static boolean processArray __ARGS((statementInfo *const st));
static boolean processTemplate __ARGS((statementInfo *const st));
static void processIdentifier __ARGS((statementInfo *const st, const int c));
static boolean nextToken __ARGS((statementInfo *const st, const unsigned int nesting));

/*============================================================================
=   Function definitions
============================================================================*/

static void initTag( tag )
    tagInfo *const tag;
{
    tag->location	= 0;
    tag->lineNumber	= 0;
    tag->name[0]	= '\0';
    DebugStatement( clearString(tag->name, MaxNameLength); )
}

static void initMemberInfo( pMember )
    memberInfo *const pMember;
{
    pMember->type	= MEMBER_NONE;
    pMember->visibility	= VIS_UNDEFINED;
    pMember->parent[0]	= '\0';
    DebugStatement( clearString(pMember->parent, MaxNameLength); )
}

static void reinitStatement( st )
    statementInfo *const st;
{
    int i;

    st->scope		= SCOPE_GLOBAL;
    st->declaration	= DECL_BASE;
    st->token		= TOK_NONE;
    st->prev[0]		= TOK_NONE;
    st->prev[1]		= TOK_NONE;
    st->gotName		= FALSE;
    st->isFuncPtr	= FALSE;
    st->buf1		= FALSE;

    for (i = 0  ;  i < 2  ;  ++i)
	initTag(&st->tag[i]);

    initTag(&st->class);

    if (st->member.type != MEMBER_NONE  &&  ! st->member.persistent)
	initMemberInfo(&st->member);
}

static void initStatement( st, parent )
    statementInfo *const st;
    const statementInfo *const parent;
{
    /*  Set the member information. If there is a parent statement, inherit
     *  the parent member information from it.
     */
    if (parent == NULL)
    {
	initMemberInfo(&st->member);
	st->inEnumBody = FALSE;
    }
    else
    {
	st->inEnumBody = (boolean)(parent->declaration == DECL_ENUM);
	st->member.visibility = VIS_UNDEFINED;
	switch (parent->declaration)
	{
	    case DECL_ENUM:	st->member.type = MEMBER_ENUM;		break;
	    case DECL_CLASS:	st->member.type = MEMBER_CLASS;
				st->member.visibility = VIS_PRIVATE;	break;
	    case DECL_INTERFACE:st->member.type = MEMBER_INTERFACE;	break;
	    case DECL_NAMESPACE:st->member.type = MEMBER_NAMESPACE;	break;
	    case DECL_STRUCT:	st->member.type = MEMBER_STRUCT;
				st->member.visibility = VIS_PUBLIC;	break;
	    case DECL_UNION:	st->member.type = MEMBER_UNION;		break;
	    default:		st->member.type = MEMBER_NONE;		break;
	}
	DebugStatement( clearString(st->member.parent, MaxNameLength); )
	if (st->member.type != MEMBER_NONE)
	{
	    st->member.persistent = TRUE;
	    if (parent->declaration == DECL_CLASS)
		strcpy(st->member.parent, parent->class.name);
	    else
		strcpy(st->member.parent,
		       (parent->prev[0] == TOK_NAME) ? activeName(parent) : "");
	}
    }
    reinitStatement(st);
}

/*----------------------------------------------------------------------------
*-	Tag generation functions
----------------------------------------------------------------------------*/

static void qualifyBlockTag( st, tag, declScope )
    const statementInfo *const st;
    const tagInfo *const tag;
    const tagScope declScope;
{
    if (st->prev[0] == TOK_NAME)
    {
	boolean ok = TRUE;
	tagType type = TAG_NUMTYPES;	/* assignment to avoid warning */

	switch (st->declaration)
	{
	    case DECL_CLASS:	type = TAG_CLASS;	break;
	    case DECL_ENUM:	type = TAG_ENUM;	break;
	    case DECL_INTERFACE:type = TAG_INTERFACE;	break;
	    case DECL_NAMESPACE:type = TAG_NAMESPACE;	break;
	    case DECL_STRUCT:	type = TAG_STRUCT;	break;
	    case DECL_UNION:	type = TAG_UNION;	break;
	    default:		ok = FALSE;		break;
	}
	if (ok)
	    makeTag(tag, &st->member, declScope, type);
    }
}

static void qualifyEnumeratorTag( st, tag, declScope )
    const statementInfo *const st;
    const tagInfo *const tag;
    const tagScope declScope;
{
    if (st->token == TOK_NAME)
	makeTag(tag, &st->member, declScope, TAG_ENUMERATOR);
}

static void qualifyFunctionTag( st, tag )
    statementInfo *const st;
    const tagInfo *const tag;
{
    if (st->scope == SCOPE_EXTERN)		/* allowed for func. def. */
	st->scope = SCOPE_GLOBAL;
    makeTag(tag, &st->member, st->scope, TAG_FUNCTION);
}

static void qualifyVariableTag( st, tag )
    const statementInfo *const st;
    const tagInfo *const tag;
{
    /*	We have to watch that we do not interpret a declaration of the
     *	form "struct tag;" as a variable definition. In such a case, the
     *	declaration will be either class, enum, struct or union, and prev[1]
     *	will be empty.
     */
    if (st->declaration == DECL_IGNORE)
	;
    else if (st->declaration == DECL_BASE  ||  st->prev[1] != TOK_SPEC)
    {
	if (st->member.type == MEMBER_NONE)
	{
	    if (st->scope == SCOPE_EXTERN)
		makeTag(tag, &st->member, st->scope, TAG_EXTERN_VAR);
	    else
		makeTag(tag, &st->member, st->scope, TAG_VARIABLE);
	}
	else
	{
	    if (st->scope == SCOPE_GLOBAL)
		makeTag(tag, &st->member, st->scope, TAG_MEMBER);
	    else if (st->scope == SCOPE_STATIC)
		makeTag(tag, &st->member, SCOPE_EXTERN, TAG_MEMBER);
	}
    }
}

static void qualifyFunctionDeclTag( st, tag )
    const statementInfo *const st;
    const tagInfo *const tag;
{
    if (! File.isHeader)
	makeTag(tag, &st->member, SCOPE_STATIC, TAG_FUNCDECL);
    else if (st->scope == SCOPE_GLOBAL  ||  st->scope == SCOPE_EXTERN)
	makeTag(tag, &st->member, SCOPE_GLOBAL, TAG_FUNCDECL);
}

/*----------------------------------------------------------------------------
*-	Parsing functions
----------------------------------------------------------------------------*/

/*  Skip to the next non-white character.
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

#if 0
/*  Skip to the next occurance of the specified character.
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
#endif

/*  Skips to the next brace in column 1. This is intended for cases where
 *  preprocessor constructs result in unbalanced braces.
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

/*  Skip to the matching character indicated by the pair string. If skipping
 *  to a matching brace and any brace is found within a different level of a
 *  #if conditional statement while brace formatting is in effect, we skip to
 *  the brace matched by its formatting. It is assumed that we have already
 *  read the character which starts the group (i.e. the first character of
 *  "pair").
 */
static boolean skipToMatch( pair )
    const char *const pair;
{
    const int begin = pair[0], end = pair[1];
    const unsigned int initialLevel = Cpp.directive.nestLevel;
    const boolean braceFormatting = (boolean)(Option.braceFormat &&
					      strcmp("{}", pair) == 0);
    boolean ok = TRUE;
    int matchLevel = 1;
    int c = '\0';

    while (matchLevel > 0  &&  (c = cppGetc()) != EOF)
    {
	if (c == begin)
	{
	    ++matchLevel;
	    if (braceFormatting  &&  Cpp.directive.nestLevel != initialLevel)
	    {
		skipToFormattedBraceMatch();
		break;
	    }
	}
	else if (c == end)
	{
	    --matchLevel;
	    if (braceFormatting  &&  Cpp.directive.nestLevel != initialLevel)
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

/*  Read a C identifier beginning with "firstChar" and places it into "name".
 */
static void readIdentifier( firstChar, name )
    const int firstChar;
    char *const name;
{
    int c, i;

    name[0] = firstChar;
    for (i = 1, c = cppGetc() ;
	 i < (int)MaxNameLength - 1  &&  isident(c) ;
	 i++, c = cppGetc())
    {
	name[i] = c;
    }
    name[i] = '\0';		/* null terminate name */

    cppUngetc(c);		/* unget non-identifier character */
}

/*  Read a C++ operator and appends to "name" (which should contain "operator").
 */
static void readOperator( firstChar, name )
    const int firstChar;
    char *const name;
{
    int c, i;

    for (c = firstChar, i = strlen(name) ;
	 i < (int)MaxNameLength - 1  &&  ! isspace(c)  &&  c != '(' ;
	 i++, c = cppGetc())
    {
	name[i] = c;
    }
    if (i > 0)
	name[i] = '\0';		/* null terminate operator */

    cppUngetc(c);		/* unget non-operator character */
}

/*  Analyzes the identifier contained in a statement described by the
 *  statement structure and adjusts the structure according the significance
 *  of the identifier.
 */
static keywordId analyzeKeyword( name )
    const char *const name;
{
    keywordId id = KEYWORD_UNKNOWN;

    if (name[0] == '_'  ||  islower(name[0]))
    {
	const short hash = KeywordHash[hashIndex(name[0])];

	if (hash >= 0)
	{
	    unsigned int i;

	    for (i = hash  ;  i < KeywordTableSize  ;  ++i)
	    {
		const keywordDesc *pKw = &KeywordTable[i];

		if (pKw->name[0] != name[0])
		    break;

		if (pKw->isValid[File.language] && strcmp(pKw->name, name) == 0)
		{
		    id = pKw->id;
		    break;
		}
	    }
	}
    }
    return id;
}

static void processKeyword( st, keyword )
    statementInfo *const st;
    keywordId keyword;
{
    st->token = TOK_SPEC;			/* default unless otherwise */

    switch (keyword)		/* is it a reserved word? */
    {
	default:		st->token	= TOK_IGNORE;		break;

	case KEYWORD_CHAR:	st->declaration = DECL_BASE;		break;
	case KEYWORD_CLASS:	st->declaration = DECL_CLASS;		break;
	case KEYWORD_DOUBLE:	st->declaration = DECL_BASE;		break;
	case KEYWORD_ENUM:	st->declaration = DECL_ENUM;		break;
	case KEYWORD_EXTERN:	st->scope	= SCOPE_EXTERN;		break;
	case KEYWORD_FLOAT:	st->declaration = DECL_BASE;		break;
	case KEYWORD_FRIEND:	st->scope	= SCOPE_FRIEND;		break;
	case KEYWORD_IMPORT:	st->declaration = DECL_IGNORE;		break;
	case KEYWORD_INT:	st->declaration = DECL_BASE;		break;
	case KEYWORD_INTERFACE:	st->declaration = DECL_INTERFACE;	break;
	case KEYWORD_LONG:	st->declaration = DECL_BASE;		break;
	case KEYWORD_NAMESPACE:	st->declaration = DECL_NAMESPACE;	break;
	case KEYWORD_PACKAGE:	st->declaration = DECL_IGNORE;		break;
	case KEYWORD_PRIVATE:	st->member.visibility = VIS_PRIVATE;	break;
	case KEYWORD_PROTECTED:	st->member.visibility = VIS_PROTECTED;	break;
	case KEYWORD_PUBLIC:	st->member.visibility = VIS_PUBLIC;	break;
	case KEYWORD_SHORT:	st->declaration = DECL_BASE;		break;
	case KEYWORD_SIGNED:	st->declaration = DECL_BASE;		break;
	case KEYWORD_STRUCT:	st->declaration = DECL_STRUCT;		break;
	case KEYWORD_TYPEDEF:	st->scope	= SCOPE_TYPEDEF;	break;
	case KEYWORD_UNION:	st->declaration = DECL_UNION;		break;
	case KEYWORD_UNSIGNED:	st->declaration = DECL_BASE;		break;
	case KEYWORD_VOID:	st->declaration = DECL_BASE;		break;

	case KEYWORD_ATTRIBUTE:
	{
	    const int c = skipToNonWhite();

	    if (c == '(')
		skipToMatch("()");
	    else
		cppUngetc(c);
	    st->token = TOK_IGNORE;
	    break;
	}

	case KEYWORD_EXTENDS:
	case KEYWORD_IMPLEMENTS:
	case KEYWORD_THROWS:
	{
	    char *const name = activeName(st);
	    int c = skipToNonWhite();

	    /*  Read and discard interface or class type-list (ident[, ident]).
	     */
	    while (isident1(c))
	    {
		readIdentifier(c, name);
		c = skipToNonWhite();
		if (c == '.'  ||  c == ',')
		    c = skipToNonWhite();
	    }
	    cppUngetc(c);
	    st->token = TOK_IGNORE;
	    break;
	}

	case KEYWORD_STATIC:
	    if (File.language != LANG_JAVA)
		st->scope = SCOPE_STATIC;
	    break;

	case KEYWORD_OPERATOR:
	{
	    char *const name = activeName(st);
	    const int c = skipToNonWhite();

	    if (isident1(c))
		readIdentifier(c, name);
	    else
		readOperator(c, name);
	} /* fall through to unknown keyword case */
	case KEYWORD_UNKNOWN:
	{
	    tagInfo *const tag = &activeTag(st);

	    st->token	= TOK_NAME;
	    st->gotName	= TRUE;

	    tag->location   = File.seek;
	    tag->lineNumber = File.lineNumber;

	    if (st->declaration == DECL_CLASS  &&  st->class.name[0] == '\0')
	    {
		strcpy(st->class.name, tag->name);
		st->class.location   = tag->location;
		st->class.lineNumber = tag->lineNumber;
	    }
	    break;
	}
    }
}

/*  Skips over characters following the parameter list. This might be non-ANSI
 *  style function declarations, a C++ exception specification, or another C++
 *  construct that I don't yet understand (e.g. "void Class::foo(int a):
 *  attr(v1), attr(v2)").
 */
static int skipPostArgumentStuff( c, st, emptyArgList )
    int c;
    statementInfo *const st;
    const boolean emptyArgList;
{
    boolean end = FALSE, firstSemicolon = TRUE;
    boolean skipCPlusStuff = (boolean)(c == ':');
    unsigned int tokenCount = 0;

    while (c != EOF  &&  ! end)
    {
	if (skipCPlusStuff)
	{
	    if (c == '{'  ||  c == ';')
		break;
	}
	else if (isident1(c))
	{
	    char name[MaxNameLength];
	    keywordId keyword;

	    readIdentifier(c, name);
	    ++tokenCount;
	    keyword = analyzeKeyword(name);
	    switch (keyword)
	    {
	    /*  These words are explicitly allowed following the closing
	     *  parenthesis in C++.
	     */
	    case KEYWORD_CONST:
		break;

	    case KEYWORD_THROW:
		skipCPlusStuff = TRUE;
		break;

	    /*  These words are never allowed within parameter declarations.
	     */
	    case KEYWORD_CLASS:
	    case KEYWORD_EXTERN:
	    case KEYWORD_STATIC:
	    case KEYWORD_TYPEDEF:
		DebugStatement( if (debug(DEBUG_PARSE)) printf("<ES>"); )
		reinitStatement(st);
		processKeyword(st, keyword);
		c = skipToNonWhite();
		end = TRUE;
		continue;		/* skip read of next character */

	    case KEYWORD_ATTRIBUTE:
		{
		    c = skipToNonWhite();
		    if (c == '('  &&  ! skipToMatch("()"))
			c = EOF;
		}
		break;

	    default:
		/*  If we encounter any other identifier immediately following
		 *  an empty parameter list, this is almost certainly one of
		 *  those Microsoft macro "thingies" that the automatic source
		 *  code generation sticks in. Terminate the current statement.
		 */
		if (emptyArgList)
		{
		    DebugStatement( if (debug(DEBUG_PARSE)) printf("<ES>"); )
		    reinitStatement(st);
		    processKeyword(st, keyword);
		    c = skipToNonWhite();
		    end = TRUE;
		    continue;		/* skip read of next character */
		}
		break;
	    }
	}
	else switch (c)
	{
	default:    break;	/* ignore */

	case ';':
	    /*  A lone word is most likely a preprocessor qualifier.
	     */
	    if (firstSemicolon  &&  tokenCount == 1)
	    {
		end = TRUE;
		continue;
	    }
	    else
	    {
		tokenCount = 0;
		firstSemicolon = FALSE;
		break;
	    }

	case '(':
	    ++tokenCount;
	    if (! skipToMatch("()"))
		c = EOF;
	    break;

	case '[':   if (! skipToMatch("[]"))  c = EOF;	break;
	case '{':
	case '}':   end = TRUE;
		    continue;		/* skip read of next character */
	}
	if (c != EOF)
	    c = cppGetc();
    }
    return c;
}

static boolean analyzePostParens( st, paren, emptyArgList )
    statementInfo *const st;
    const parenInfo *const paren;
    const boolean emptyArgList;
{
    boolean ok = TRUE;
    int c;

    /*	At this point we should be at the character following the
     *	closing parenthesis.
     */
    c = skipToNonWhite();
    if (st->gotName)
    {
	if (strchr("{;,", c) != NULL)
	{
	    st->token = TOK_ARGS;		/* parameter list to a func. */
	    st->declaration = DECL_BASE;	/* clear any other decl. */
	}
	else if (isident1(c)  ||  c == ':')
	{
	    st->token = TOK_ARGS;		/* parameter list to a func. */
	    st->declaration = DECL_BASE;	/* clear any other decl. */
	    if (File.language != LANG_JAVA)
		c = skipPostArgumentStuff(c, st, emptyArgList);
	}
	else
	    st->token = TOK_IGNORE;
    }
    /*	The name inside the parentheses must have been a function or
     *	variable name.
     */
    else if (paren->gotName)
    {
	tagInfo *const tag = &activeTag(st);

	st->gotName	= TRUE;
	st->token	= TOK_NAME;
	tag->location	= paren->location;
	tag->lineNumber	= paren->lineNumber;
	strcpy(tag->name, paren->name);
    }
    else
	st->token = TOK_IGNORE;
    if (c == EOF)
	ok = FALSE;
    else
	cppUngetc(c);

    return ok;
}

static void initParenInfo( paren )
    parenInfo *const paren;
{
    DebugStatement( clearString(paren->name, MaxNameLength); )
    paren->gotName	= FALSE;
    paren->location	= 0;
    paren->lineNumber	= 0;
}

static boolean saveParenInfo( paren, c )
    parenInfo *const paren;
    int c;
{
    boolean ok = TRUE;

    readIdentifier(c, paren->name);
    c = skipToNonWhite();
    if (c == ')')		/* saved if only identifier in parentheses */
    {
	paren->gotName    = TRUE;
	paren->location   = File.seek;
	paren->lineNumber = File.lineNumber;
    }
    else
    {
	if (c == '(')
	    cppUngetc(c);
	ok = skipToMatch("()");
    }
    return ok;
}

static boolean doubleParens( st )
    statementInfo *const st;
{
    /*	A double parenthesis almost certainly means one of those conditional
     *	prototype macro thingies (e.g. __ARGS((void)) ). If found, we will use
     *	the previous name, if it is not empty.
     */
    if (st->gotName  &&  *st->tag[!st->buf1].name != '\0')
	swapNameBuffers(st);

    cppUngetc('(');			/* put back for skipToMatch() */
    return skipToMatch("()");
}

/*  Analyzes the context and contents of parentheses.
 */
static boolean analyzeParens( st )
    statementInfo *const st;
{
    boolean ok = TRUE;
    int c;

    c = skipToNonWhite();
    if (c == '*')				/* this is a function pointer */
    {
	st->gotName	= FALSE;		/* invalidate previous name */
	st->isFuncPtr	= TRUE;
	st->token	= TOK_IGNORE;
    }
    if (! st->gotName)
    {
	st->token = TOK_IGNORE;
	cppUngetc(c);
    }
    else
    {
	boolean terminate = FALSE, emptyArgList = FALSE;
	parenInfo paren;

	initParenInfo(&paren);
	if (isident1(c))
	    ok = saveParenInfo(&paren, c);
	else if (c == '(')
	    ok = doubleParens(st);
	else if (c == ')')			/* empty parentheses... */
	    emptyArgList = TRUE;
	else
	{
	    /*	This is an invalid character to be inside a paren in this
	     *	context. This must be a macro call. After we read to the
	     *	end of the parenthesis seqence, force a termination of the
	     *	current statement,
	     */
	    st->token	= TOK_NONE;
	    st->gotName	= FALSE;
	    terminate	= TRUE;
	    ok = skipToMatch("()");
	}
	if (ok  &&  ! terminate)
	    ok = analyzePostParens(st, &paren, emptyArgList);
    }
    return ok;
}

/*  Analyzes the identifier contained in a statement described by the
 *  statement structure and adjusts the structure according the significance
 *  of the identifier.
 */
static void analyzeIdentifier( st )
    statementInfo *const st;
{
    tagInfo *const tag = &activeTag(st);
    char *const name = tag->name;

    if (isIgnoreToken(name))
	st->token = TOK_IGNORE;
    else
	processKeyword(st, analyzeKeyword(name));

    if (st->token == TOK_IGNORE)
	name[0] = '\0';
}

static void processIdentifier( st, c )
    statementInfo *const st;
    const int c;
{
    if (st->gotName)
	swapNameBuffers(st);
    readIdentifier(c, activeName(st));
    analyzeIdentifier(st);
    if (st->gotName  &&  st->token == TOK_IGNORE)
	swapNameBuffers(st);
}

static void processColon( st )
    statementInfo *const st;
{
    if (st->declaration != DECL_CLASS)
    {
	const int c = skipToNonWhite();

	if (c == ':')	/* this is a method or static member definition */
	{
	    st->member.type = MEMBER_CLASS;
	    strcpy(st->member.parent, activeName(st));
	    st->member.persistent = FALSE;
	}
	else
	{
	    cppUngetc(c);
	    st->token = TOK_IGNORE;
	}
    }
}

/*  Skips over any initializing value which may follow a '=' character in a
 *  variable definition.
 */
static int skipInitializer( enumInitializer )
    const boolean enumInitializer;
{
    boolean done = FALSE;
    int c;

    do
    {
	c = cppGetc();

	if (c != EOF) switch (c)
	{
	    case ',':
	    case ';':   done = TRUE; break;

	    case '[':   if (! skipToMatch("[]"))
			    c = EOF;
			break;
	    case '(':   if (! skipToMatch("()"))
			    c = EOF;
			break;
	    case '{':   if (! skipToMatch("{}"))
			    c = EOF;
			break;
	    case '}':   if (enumInitializer)
			{
			    cppUngetc(c);
			    done = TRUE;
			}
			else if (! Option.braceFormat)
			    c = EOF;
			break;

	    default:    break;
	}
    } while (! done  &&  c != EOF);

    return c;
}

static boolean processInitializer( st )
    statementInfo *const st;
{
    boolean ok = TRUE;
    const int c = skipInitializer(st->inEnumBody);

    if (c == EOF)
	ok = FALSE;
    else if (c == ';')
	st->token = TOK_SEMICOLON;
    else if (c == ',')
	st->token = TOK_COMMA;
    else if ('}'  &&  st->inEnumBody)
	st->token = TOK_COMMA;

    if (st->scope == SCOPE_EXTERN)
	st->scope = SCOPE_GLOBAL;

    return ok;
}

static boolean processArray( st )
    statementInfo *const st;
{
    st->token = TOK_IGNORE;
    return skipToMatch("[]");
}

static boolean processTemplate( st )
    statementInfo *const st;
{
    st->token = TOK_IGNORE;
    return skipToMatch("<>");
}

static boolean beginBlock( st, nesting )
    statementInfo *const st;
    const unsigned int nesting;
{
    const tagScope declScope = (File.isHeader || File.language == LANG_JAVA) ?
				SCOPE_GLOBAL : SCOPE_STATIC;
    tagInfo *const tag = &activeTag(st);
    boolean ok;

    switch (st->declaration)
    {
	case DECL_CLASS:
	    qualifyBlockTag(st, &st->class, declScope);
	    ok = createTags(nesting + 1, st);
	    break;

	case DECL_ENUM:
	case DECL_INTERFACE:
	case DECL_NAMESPACE:
	case DECL_STRUCT:
	case DECL_UNION:
	    qualifyBlockTag(st, tag, declScope);
	    ok = createTags(nesting + 1, st);
	    break;

	case DECL_NOMANGLE:
	    ok = createTags(nesting + 1, st);
	    break;

	default:
	    ok = skipToMatch("{}");
    }
    st->token = TOK_BODY;

    return ok;
}

static boolean endBlock( st, nesting )
    statementInfo *const st;
    const unsigned int nesting;
{
    boolean ok = TRUE;

    if (nesting > 0)
	st->token = TOK_EOF;		/* fake out */
    else
    {
	st->token = TOK_IGNORE;
	ok = FALSE;
    }
    return ok;
}

/*  Reads characters from the pre-processor and assembles tokens, setting
 *  the current statement state.
 */
static boolean nextToken( st, nesting )
    statementInfo *const st;
    const unsigned int nesting;
{
    int c;
    boolean ok = TRUE;

    do
    {
	c = cppGetc();

	switch (c)
	{
	    case EOF:   st->token = TOK_EOF;		break;
	    case '(':   ok = analyzeParens(st);		break;
	    case '*':   st->gotName = FALSE;		break;
	    case ',':   st->token = TOK_COMMA;		break;
	    case ':':   processColon(st);		break;
	    case ';':   st->token = TOK_SEMICOLON;	break;
	    case '=':   ok = processInitializer(st);	break;
	    case '[':   ok = processArray(st);		break;
	    case '{':   ok = beginBlock(st, nesting);	break;
	    case '}':   ok = endBlock(st, nesting);	break;
	    case '<':   ok = processTemplate(st);	break;

	    default:
		if (isident1(c))
		    processIdentifier(st, c);
		else if (isExternCDecl(st, c))
		{
		    st->declaration = DECL_NOMANGLE;
		    st->scope = SCOPE_GLOBAL;
		}
		else
		    st->token = TOK_IGNORE;
	}
    } while (ok  &&  st->token == TOK_IGNORE);

    return ok;
}

/*----------------------------------------------------------------------------
*-	Scanning functions
----------------------------------------------------------------------------*/

/*  Parses the current file and decides whether to write out and tags that
 *  are discovered.
 */
extern boolean createTags( nesting, parent )
    const unsigned int nesting;
    const void *const parent;
{
    const tagScope declScope = File.isHeader ? SCOPE_GLOBAL : SCOPE_STATIC;
    statementInfo *st;
    boolean ok;

    DebugStatement( if (nesting > 0) debugParseNest(TRUE, nesting); )
    st = (statementInfo *)malloc(sizeof(statementInfo));
    if (st == NULL)
	error(FATAL | PERROR, "cannot allocate statement info");
    initStatement(st, (const statementInfo *)parent);

    while ((ok = nextToken(st, nesting)))
    {
	tagInfo *const tag = &activeTag(st);

	if (st->token == TOK_EOF)
	    break;
	else if (! st->gotName)
	    ;
	else if (st->inEnumBody)
	    qualifyEnumeratorTag(st, tag, declScope);
	else if (st->token == TOK_BODY  &&  st->prev[0] == TOK_ARGS)
	    qualifyFunctionTag(st, tag);
	else if (st->token == TOK_SEMICOLON  ||  st->token == TOK_COMMA)
	{
	    if (st->scope == SCOPE_TYPEDEF)
		makeTag(tag, &st->member, declScope, TAG_TYPEDEF);
	    else if (st->prev[0] == TOK_NAME  ||  st->isFuncPtr)
		qualifyVariableTag(st, tag);
	    else if (st->prev[0] == TOK_ARGS)
		qualifyFunctionDeclTag(st, tag);
	}

	/*  Reset after a semicolon, or BODY preceeded by ARGS (a function),
	 *  a namspace definition, or an "extern" block.
	 */
	if (st->token == TOK_SEMICOLON  ||  (st->token == TOK_BODY  &&
	     (st->declaration == DECL_NAMESPACE  ||
	      st->declaration == DECL_NOMANGLE   ||
	      st->prev[0] == TOK_ARGS)))
	{
	    DebugStatement( if (debug(DEBUG_PARSE)) printf("<ES>"); )
	    reinitStatement(st);
	    Cpp.resolveRequired = FALSE;	/* end of statement */
	}
	else
	    Cpp.resolveRequired = TRUE;		/* in middle of statement */

	st->prev[1] = st->prev[0];
	st->prev[0] = st->token;
    }
    DebugStatement( if (nesting > 0) debugParseNest(FALSE, nesting - 1); )
    free(st);
    return ok;
}

extern void buildKeywordHash()
{
    int lastInitialChar = '\0';
    size_t i;

    /*  Clear all hash entries.
     */
    for (i = 0  ;  i < sizeof(KeywordHash)/sizeof(KeywordHash[0])  ;  ++i)
	KeywordHash[i] = -1;

    /*  Set those hash entries corresponding to keywords.
     */
    for (i = 0  ;  i < KeywordTableSize  ;  ++i)
    {
	const unsigned char initialChar = KeywordTable[i].name[0];

	if (initialChar != lastInitialChar)
	{
	    KeywordHash[hashIndex(initialChar)] = i;
	    lastInitialChar = initialChar;
	}
    }
}

/* vi:set tabstop=8 shiftwidth=4: */
