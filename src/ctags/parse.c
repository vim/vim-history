/*****************************************************************************
*   $Id: parse.c,v 5.2 1998/02/26 05:32:07 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
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
#define activeTag(st)		((st)->tag[(st)->buf1])
#define activeName(st)		(activeTag(st).name)
#define swapNameBuffers(st)	((st)->buf1 = (boolean)!(st)->buf1)

#define isExternCDecl(st,c)	((c) == STRING_SYMBOL  &&  !(st)->gotName  && \
				 (st)->scope == SCOPE_EXTERN)

/*============================================================================
=   Data declarations
============================================================================*/

/*  Used to specify type of keyword.
 */
typedef enum _keywordType {
    KEYWORD_UNKNOWN, KEYWORD_ATTRIBUTE,
    KEYWORD_CHAR, KEYWORD_CLASS, KEYWORD_CONST, KEYWORD_DOUBLE,
    KEYWORD_ENUM, KEYWORD_EXPLICIT, KEYWORD_EXTERN,
    KEYWORD_FLOAT, KEYWORD_FRIEND,
    KEYWORD_INLINE, KEYWORD_INT, KEYWORD_LONG, KEYWORD_MUTABLE,
    KEYWORD_NAMESPACE, KEYWORD_NEW, KEYWORD_OPERATOR, KEYWORD_OVERLOAD,
    KEYWORD_PRIVATE, KEYWORD_PROTECTED, KEYWORD_PUBLIC,
    KEYWORD_SHORT, KEYWORD_SIGNED, KEYWORD_STATIC, KEYWORD_STRUCT,
    KEYWORD_TEMPLATE, KEYWORD_THROW, KEYWORD_TYPEDEF, KEYWORD_TYPENAME,
    KEYWORD_UNION, KEYWORD_UNSIGNED, KEYWORD_USING,
    KEYWORD_VIRTUAL, KEYWORD_VOID, KEYWORD_VOLATILE, KEYWORD_WCHAR_T
} keywordType;

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
=   Function prototypes
============================================================================*/
/*  Parsing functions.
 */
static int skipToNonWhite __ARGS((void));
static int skipToCharacter __ARGS((const int findchar));
static void skipToFormattedBraceMatch __ARGS((void));
static boolean skipToMatch __ARGS((const char *const pair));
static void readIdendifier __ARGS((const int firstChar, char *const name));
static void readOperator __ARGS((char *const name));
static keywordType analyzeKeyword __ARGS((const char *const name));
static void processKeyword __ARGS((statementInfo *const st, keywordType keyword));
static int skipPostArgumentStuff __ARGS((int c, statementInfo *const st, const boolean emptyArgList));
static boolean analyzePostParens __ARGS((statementInfo *const st, const parenInfo *const paren, const boolean emptyArgList));
static void initParenInfo __ARGS((parenInfo *const paren));
static boolean saveParenInfo __ARGS((parenInfo *const paren, int c));
static boolean doubleParens __ARGS((statementInfo *const st));
static boolean analyzeParens __ARGS((statementInfo *const st));
static boolean isIgnoreToken __ARGS((const char *const name));
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

/*  Scanning functions.
 */
static void reinitStatement __ARGS((statementInfo *const st));
static void initStatement __ARGS((statementInfo *const st, const statementInfo *const parent));
static void qualifyBlockTag __ARGS((const statementInfo *const st, const tagInfo *const tag, const tagScope declScope));
static void qualifyEnumeratorTag __ARGS((const statementInfo *const st, const tagInfo *const tag, const tagScope declScope));
static void qualifyFunctionTag __ARGS((statementInfo *const st, const tagInfo *const tag));
static void qualifyVariableTag __ARGS((const statementInfo *const st, const tagInfo *const tag, const unsigned int nesting));
static void qualifyFunctionDeclTag __ARGS((const statementInfo *const st, const tagInfo *const tag));

/*============================================================================
=   Function definitions
============================================================================*/

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
static void readIdendifier( firstChar, name )
    const int firstChar;
    char *const name;
{
    int c, i;

    name[0] = firstChar;
    for (i = 1, c = cppGetc() ;
	 i < MaxNameLength - 1  &&  isident(c) ;
	 i++, c = cppGetc())
    {
	name[i] = c;
    }
    name[i] = '\0';		/* null terminate name */

    cppUngetc(c);		/* unget non-identifier character */
}

/*  Read a C++ operator and appends to "name" (which should contain "operator").
 */
static void readOperator( name )
    char *const name;
{
    int c, i;

    for (c = cppGetc(), i = strlen(name) ;
	 i < MaxNameLength - 1  &&  ! isspace(c)  &&  c != '(' ;
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
static keywordType analyzeKeyword( name )
    const char *const name;
{
    keywordType keyword = KEYWORD_UNKNOWN;

#define match(word) ((strcmp(name,(word)) == 0))

    switch ((unsigned char)name[0])	/* is it a reserved word? */
    {
    case '_':	     if (match("__attribute__"))keyword = KEYWORD_ATTRIBUTE;
		break;
    case 'c':	     if (match("class"	))	keyword = KEYWORD_CLASS;
		else if (match("const"	))	keyword = KEYWORD_CONST;
		else if (match("char"	))	keyword = KEYWORD_CHAR;
		break;
    case 'd':	     if (match("double"	))	keyword = KEYWORD_DOUBLE;
		break;
    case 'e':	     if (match("enum"	))	keyword = KEYWORD_ENUM;
		else if (match("explicit"))	keyword = KEYWORD_EXPLICIT;
		else if (match("extern"	))	keyword = KEYWORD_EXTERN;
		break;
    case 'f':	     if (match("float"	))	keyword = KEYWORD_FLOAT;
		else if (match("friend"	))	keyword = KEYWORD_FRIEND;
		break;
    case 'i':	     if (match("int"	))	keyword = KEYWORD_INT;
		else if (match("inline"	))	keyword = KEYWORD_INLINE;
		break;
    case 'l':	     if (match("long"	))	keyword = KEYWORD_LONG;
		break;
    case 'm':	     if (match("mutable"))	keyword = KEYWORD_MUTABLE;
		break;
    case 'n':	     if (match("namespace"))	keyword = KEYWORD_NAMESPACE;
		else if (match("new"))		keyword = KEYWORD_NEW;
		break;
    case 'o':	     if (match("operator"))	keyword = KEYWORD_OPERATOR;
		else if (match("overload"))	keyword = KEYWORD_OVERLOAD;
		break;
    case 'p':	     if (match("private"))	keyword = KEYWORD_PRIVATE;
		else if (match("protected"))	keyword = KEYWORD_PROTECTED;
		else if (match("public"	))	keyword = KEYWORD_PUBLIC;
		break;
    case 's':	     if (match("static"	))	keyword = KEYWORD_STATIC;
		else if (match("struct"	))	keyword = KEYWORD_STRUCT;
		else if (match("short"	))	keyword = KEYWORD_SHORT;
		else if (match("signed"	))	keyword = KEYWORD_SIGNED;
		break;
    case 't':	     if (match("template"))	keyword = KEYWORD_TEMPLATE;
		else if (match("throw"	))	keyword = KEYWORD_THROW;
		else if (match("typedef"))	keyword = KEYWORD_TYPEDEF;
		else if (match("typename"))	keyword = KEYWORD_TYPENAME;
		break;
    case 'u':	     if (match("union"	))	keyword = KEYWORD_UNION;
		else if (match("unsigned"))	keyword = KEYWORD_UNSIGNED;
		else if (match("using"))	keyword = KEYWORD_USING;
		break;
    case 'v':	     if (match("virtual"))	keyword = KEYWORD_VIRTUAL;
		else if (match("void"	))	keyword = KEYWORD_VOID;
		else if (match("volatile"))	keyword = KEYWORD_VOLATILE;
		break;
    case 'w':	     if (match("wchar_t"))	keyword = KEYWORD_WCHAR_T;
		break;
    }
#undef match

    return keyword;
}

static void processKeyword( st, keyword )
    statementInfo *const st;
    keywordType keyword;
{
    st->token = TOK_SPEC;			/* default unless otherwise */

    switch (keyword)		/* is it a reserved word? */
    {
	default:		st->token	= TOK_IGNORE;	break;

	case KEYWORD_CHAR:	st->declaration = DECL_BASE;	break;
	case KEYWORD_CLASS:	st->declaration = DECL_CLASS;	break;
	case KEYWORD_DOUBLE:	st->declaration = DECL_BASE;	break;
	case KEYWORD_ENUM:	st->declaration = DECL_ENUM;	break;
	case KEYWORD_EXTERN:	st->scope	= SCOPE_EXTERN;	break;
	case KEYWORD_FLOAT:	st->declaration = DECL_BASE;	break;
	case KEYWORD_FRIEND:	st->scope	= SCOPE_FRIEND;	break;
	case KEYWORD_INT:	st->declaration = DECL_BASE;	break;
	case KEYWORD_LONG:	st->declaration = DECL_BASE;	break;
	case KEYWORD_SHORT:	st->declaration = DECL_BASE;	break;
	case KEYWORD_SIGNED:	st->declaration = DECL_BASE;	break;
	case KEYWORD_STATIC:	st->scope	= SCOPE_STATIC;	break;
	case KEYWORD_STRUCT:	st->declaration = DECL_STRUCT;	break;
	case KEYWORD_TYPEDEF:	st->scope	= SCOPE_TYPEDEF;break;
	case KEYWORD_UNION:	st->declaration = DECL_UNION;	break;
	case KEYWORD_UNSIGNED:	st->declaration = DECL_BASE;	break;
	case KEYWORD_VOID:	st->declaration = DECL_BASE;	break;

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

	case KEYWORD_OPERATOR:
	    readOperator(activeName(st));
	    /* fall through to unknown keyword case */
	case KEYWORD_UNKNOWN:
	{
	    tagInfo *const tag = &activeTag(st);

	    st->token	= TOK_NAME;
	    st->gotName	= TRUE;

	    tag->location   = File.seek;
	    tag->lineNumber = File.lineNumber;

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
	    keywordType keyword;

	    readIdendifier(c, name);
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
		DebugStatement( if (debug(DEBUG_VISUAL)) printf("<ES>"); )
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
		    DebugStatement( if (debug(DEBUG_VISUAL)) printf("<ES>"); )
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

    readIdendifier(c, paren->name);
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

/*  Determines whether or not "name" should be ignored, per the ignore list.
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
    readIdendifier(c, activeName(st));
    analyzeIdentifier(st);
    if (st->gotName  &&  st->token == TOK_IGNORE)
	swapNameBuffers(st);
}

static void processColon( st )
    statementInfo *const st;
{
    if (st->declaration == DECL_CLASS)
    {
	const int c = skipToCharacter('{');	/* skip over intervening junk */

	cppUngetc(c);
    }
    else
    {
	const int c = skipToNonWhite();

	if (c == ':')			/* this is a method declaration */
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
    const tagScope declScope = File.header ? SCOPE_GLOBAL : SCOPE_STATIC;
    tagInfo *const tag = &activeTag(st);
    boolean ok;

    switch (st->declaration)
    {
	case DECL_ENUM:
	case DECL_CLASS:
	case DECL_STRUCT:
	    qualifyBlockTag(st, tag, declScope);
	    ok = createTags(nesting + 1, st);
	    break;

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
    {
	tagInfo *const tag = &st->tag[i];

	tag->location	= 0;
	tag->lineNumber	= 0;
	tag->name[0]	= '\0';
	DebugStatement( clearString(tag->name, MaxNameLength); )
    }

    if (st->member.type != MEMBER_NONE  &&  ! st->member.persistent)
    {
	DebugStatement( clearString(st->member.parent, MaxNameLength); )
	st->member.type = MEMBER_NONE;
	st->member.parent[0] = '\0';
    }
}

static void initStatement( st, parent )
    statementInfo *const st;
    const statementInfo *const parent;
{
    reinitStatement(st);

    /*  Set the member information. If there is a parent statement, inherit
     *  the parent member information from it.
     */
    if (parent == NULL)
    {
	DebugStatement( clearString(st->member.parent, MaxNameLength); )
	st->member.type = MEMBER_NONE;
	st->member.parent[0] = '\0';
	st->inEnumBody = FALSE;
    }
    else
    {
	st->inEnumBody = (boolean)(parent->declaration == DECL_ENUM);
	switch (parent->declaration)
	{
	    case DECL_ENUM:	st->member.type = MEMBER_ENUM;		break;
	    case DECL_CLASS:	st->member.type = MEMBER_CLASS;		break;
	    case DECL_STRUCT:	st->member.type = MEMBER_STRUCT;	break;
	    case DECL_UNION:	st->member.type = MEMBER_UNION;		break;
	    default:		st->member.type = MEMBER_NONE;		break;
	}
	DebugStatement( clearString(st->member.parent, MaxNameLength); )
	if (st->member.type != MEMBER_NONE)
	{
	    st->member.persistent = TRUE;
	    strcpy(st->member.parent,
		(parent->prev[0] == TOK_NAME) ? activeName(parent) : "");
	}
    }
}

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

static void qualifyVariableTag( st, tag, nesting )
    const statementInfo *const st;
    const tagInfo *const tag;
    const unsigned int nesting;
{
    /*	We have to watch that we do not interpret a declaration of the
     *	form "struct tag;" as a variable definition. In such a case, the
     *	declaration will be either class, enum, struct or union, and prev[1]
     *	will be empty.
     */
    if (nesting == 0  ||  st->member.type == MEMBER_NONE)
    {
	if (st->declaration == DECL_BASE  ||  st->prev[1] != TOK_SPEC)
	{
	    if (st->scope != SCOPE_EXTERN)
		makeTag(tag, &NoClass, st->scope, TAG_VARIABLE);
	}
    }
    else if (nesting > 0  &&  st->member.type != MEMBER_NONE)
    {
	if (st->scope == SCOPE_GLOBAL)
	    makeTag(tag, &st->member, st->scope, TAG_MEMBER);
    }
}

static void qualifyFunctionDeclTag( st, tag )
    const statementInfo *const st;
    const tagInfo *const tag;
{
    if (! File.header)
	makeTag(tag, &st->member, SCOPE_STATIC, TAG_FUNCDECL);
    else if (st->scope == SCOPE_GLOBAL  ||  st->scope == SCOPE_EXTERN)
	makeTag(tag, &st->member, SCOPE_GLOBAL, TAG_FUNCDECL);
}

/*  Parses the current file and decides whether to write out and tags that
 *  are discovered.
 */
extern boolean createTags( nesting, parent )
    const unsigned int nesting;
    const void *const parent;
{
    const tagScope declScope = File.header ? SCOPE_GLOBAL : SCOPE_STATIC;
    statementInfo st;
    boolean ok;

    DebugStatement( if (nesting > 0) debugParseNest(TRUE, nesting); )
    initStatement(&st, (const statementInfo *)parent);

    while ((ok = nextToken(&st, nesting)))
    {
	tagInfo *const tag = &activeTag(&st);

	if (st.token == TOK_EOF)
	    break;
	else if (! st.gotName)
	    ;
	else if (st.inEnumBody)
	    qualifyEnumeratorTag(&st, tag, declScope);
	else if (st.token == TOK_BODY  &&  st.prev[0] == TOK_ARGS)
	    qualifyFunctionTag(&st, tag);
	else if (st.token == TOK_SEMICOLON  ||  st.token == TOK_COMMA)
	{
	    if (st.scope == SCOPE_TYPEDEF)
		makeTag(tag, &st.member, declScope, TAG_TYPEDEF);
	    else if (st.prev[0] == TOK_NAME  ||  st.isFuncPtr)
		qualifyVariableTag(&st, tag, nesting);
	    else if (st.prev[0] == TOK_ARGS)
		qualifyFunctionDeclTag(&st, tag);
	}

	/*  Reset after a semicolon or ARGS BODY pair.
	 */
	if (st.token == TOK_SEMICOLON  ||  (st.token == TOK_BODY  &&
	     (st.prev[0] == TOK_ARGS  ||  st.declaration == DECL_NOMANGLE)))
	{
	    DebugStatement( if (debug(DEBUG_VISUAL)) printf("<ES>"); )
	    reinitStatement(&st);
	    Cpp.resolveRequired = FALSE;	/* end of statement */
	}
	else
	    Cpp.resolveRequired = TRUE;		/* in middle of statement */

	st.prev[1] = st.prev[0];
	st.prev[0] = st.token;
    }
    DebugStatement( if (nesting > 0) debugParseNest(FALSE, nesting - 1); )
    return ok;
}

/* vi:set tabstop=8 shiftwidth=4: */
