/*****************************************************************************
*   $Id: get.c,v 5.2 1998/02/26 05:32:07 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains the high level source read functions (preprocessor
*   directives are handled within this level).
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "ctags.h"

/*============================================================================
=   Data declarations
============================================================================*/
typedef enum { COMMENT_NONE, COMMENT_C, COMMENT_CPLUS } Comment;

/*============================================================================
=   Data definitions
============================================================================*/

cppState Cpp = {
    0,				/* ungetch */
    FALSE,			/* resolveRequired */
    {
	DRCTV_NONE,		/* state */
	FALSE,			/* accept */
	{ 0, 0, "" },		/* tag info */
	0,			/* nestLevel */
	{ {FALSE,FALSE,FALSE,FALSE} }	/* ifdef array */
    }				/* directive */
};

/*============================================================================
=   Function prototypes
============================================================================*/
static boolean readDirective __ARGS((int c, char *const name, unsigned int maxLength));
static boolean readDefineTag __ARGS((int c, tagInfo *const tag, boolean *const parameterized));
static conditionalInfo *currentConditional __ARGS((void));
static boolean isIgnore __ARGS((void));
static boolean setIgnore __ARGS((const boolean ignore));
static boolean isIgnoreBranch __ARGS((void));
static void chooseBranch __ARGS((void));
static boolean pushConditional __ARGS((const boolean firstBranchChosen));
static boolean popConditional __ARGS((void));
static boolean handleDirective __ARGS((const int c));
static Comment isComment __ARGS((void));
static int skipOverCComment __ARGS((void));
static int skipOverCplusComment __ARGS((void));
static int skipToEndOfString __ARGS((void));
static int skipToEndOfChar __ARGS((void));

/*============================================================================
=   Function definitions
============================================================================*/

/*----------------------------------------------------------------------------
*   Scanning functions	
*
*   This section handles preprocessor directives.  It strips out all
*   directives and may emit a tag for #define directives.
*--------------------------------------------------------------------------*/

extern boolean cppOpen( name )
    const char *const name;
{
    boolean opened;

    opened = fileOpen(name);
    if (opened)
    {
	Cpp.ungetch = '\0';
	Cpp.resolveRequired	= FALSE;
	Cpp.directive.state	= DRCTV_NONE;
	Cpp.directive.accept	= TRUE;
	Cpp.directive.nestLevel	= 0;
    }
    return opened;
}

extern void cppClose()
{
    fileClose();
}

/*  This puts a character back into the input queue for the source File.
 */
extern void cppUngetc( c )
    const int c;
{
    Cpp.ungetch = c;
}

/*  Reads a directive, whose first character is given by "c", into "name".
 */
static boolean readDirective( c, name, maxLength )
    int c;
    char *const name;
    unsigned int maxLength;
{
    unsigned int i;

    for (i = 0  ;  i < maxLength - 1  ;  ++i)
    {
	if (i > 0)
	{
	    c = fileGetc();
	    if (c == EOF  ||  ! isalpha(c))
	    {
		fileUngetc(c);
		break;
	    }
	}
	name[i] = c;
    }
    name[i] = '\0';					/* null terminate */

    return (boolean)isspacetab(c);
}

/*  Reads an identifier, whose first character is given by "c", into "tag",
 *  together with the file location and corresponding line number.
 */
static boolean readDefineTag( c, tag, parameterized )
    int c;
    tagInfo *const tag;
    boolean *const parameterized;
{
    char *name = tag->name;

    do
    {
	*name++ = c;
    } while (c = fileGetc(), (c != EOF  &&  isident(c)));
    fileUngetc(c);
    *name = '\0';					/* null terminate */
    tag->location   = File.seek;
    tag->lineNumber = File.lineNumber;

    *parameterized = (boolean)(c == '(');
    return (boolean)(isspace(c)  ||  c == '(');
}

static conditionalInfo *currentConditional()
{
    return &Cpp.directive.ifdef[Cpp.directive.nestLevel];
}

static boolean isIgnore()
{
    return Cpp.directive.ifdef[Cpp.directive.nestLevel].ignoring;
}

static boolean setIgnore( ignore )
    const boolean ignore;
{
    return Cpp.directive.ifdef[Cpp.directive.nestLevel].ignoring = ignore;
}

static boolean isIgnoreBranch()
{
    conditionalInfo *const ifdef = currentConditional();

    /*  Force a single branch if an incomplete statement is discovered
     *  en route. This may have allowed earlier branches containing complete
     *  statements to be followed, but we must follow no further branches.
     */
    if (Cpp.resolveRequired  &&  ! Option.braceFormat)
	ifdef->singleBranch = TRUE;

    /*  We will ignore this branch in the following cases:
     *
     *  1.  We are ignoring all branches (conditional was within an ignored
     *        branch of the parent conditional)
     *  2.  A branch has already been chosen and either of:
     *      a.  A statement was incomplete upon entering the conditional
     *      b.  A statement is incomplete upon encountering a branch
     */
    return (boolean)(ifdef->ignoreAllBranches ||
		     (ifdef->branchChosen  &&  ifdef->singleBranch));
}

static void chooseBranch()
{
    if (! Option.braceFormat)
    {
	conditionalInfo *const ifdef = currentConditional();

	ifdef->branchChosen = (boolean)(ifdef->singleBranch ||
					Cpp.resolveRequired);
    }
}

/*  Pushes one nesting level for an #if directive, indicating whether or not
 *  the branch should be ignored and whether a branch has already been chosen.
 */
static boolean pushConditional( firstBranchChosen )
    const boolean firstBranchChosen;
{
    const boolean ignoreAllBranches = isIgnore();	/* current ignore */
    boolean ignoreBranch = FALSE;

    if (Cpp.directive.nestLevel < MaxCppNestingLevel - 1)
    {
	conditionalInfo *ifdef;

	++Cpp.directive.nestLevel;
	ifdef = currentConditional();

	/*  We take a snapshot of whether there is an incomplete statement in
	 *  progress upon encountering the preprocessor conditional. If so,
	 *  then we will flag that only a single branch of the conditional
	 *  should be followed.
	 */
	ifdef->ignoreAllBranches= ignoreAllBranches;
	ifdef->singleBranch	= Cpp.resolveRequired;
	ifdef->branchChosen	= firstBranchChosen;
	ifdef->ignoring		= (boolean)(ignoreAllBranches || (
				    !firstBranchChosen && !Option.braceFormat &&
				    (ifdef->singleBranch || !Option.if0)));
	ignoreBranch = ifdef->ignoring;
    }
    return ignoreBranch;
}

/*  Pops one nesting level for an #endif directive.
 */
static boolean popConditional()
{
    if (Cpp.directive.nestLevel > 0)
	--Cpp.directive.nestLevel;

    return isIgnore();
}

/*  Handles a pre-processor directive whose first character is given by "c".
 */
static boolean handleDirective( c )
    const int c;
{
    enum { maxDirectiveName = 10 };
    char directive[maxDirectiveName];
    const tagScope scope = File.header ? SCOPE_GLOBAL : SCOPE_STATIC;
    boolean ignore = FALSE;
    DebugStatement( const boolean ignore0 = isIgnore(); )

    switch (Cpp.directive.state)
    {
    case DRCTV_NONE:	/* used to ignore rest of line past interesting part*/
	ignore = isIgnore();
	break;

    case DRCTV_HASH:			/* '#' read; determine directive */
	readDirective(c, directive, maxDirectiveName);
	if (stringMatch(directive, "define"))
	    Cpp.directive.state = DRCTV_DEFINE;
	else if (strncmp(directive, "if", (size_t)2) == 0)
	    Cpp.directive.state = DRCTV_IF;
	else if (stringMatch(directive, "elif")  ||
		 stringMatch(directive, "else"))
	{
	    ignore = setIgnore(isIgnoreBranch());
	    if (! ignore  &&  stringMatch(directive, "else"))
		chooseBranch();
	    Cpp.directive.state = DRCTV_NONE;
	    DebugStatement( if (ignore != ignore0) debugCppIgnore(ignore); )
	}
	else if (stringMatch(directive, "endif"))
	{
	    DebugStatement( debugCppNest(FALSE, Cpp.directive.nestLevel); )
	    ignore = popConditional();
	    Cpp.directive.state = DRCTV_NONE;
	    DebugStatement( if (ignore != ignore0) debugCppIgnore(ignore); )
	}
	else						/* "pragma", etc. */
	    Cpp.directive.state = DRCTV_NONE;
	break;

    case DRCTV_IF:				/* "if" or "ifdef" detected */
	ignore = pushConditional((boolean)(c != '0'));
	Cpp.directive.state = DRCTV_NONE;
	DebugStatement(	debugCppNest(TRUE, Cpp.directive.nestLevel);
			if (ignore != ignore0) debugCppIgnore(ignore); )
	break;

    case DRCTV_DEFINE:			/* "define" detected: generate tag */
	{
	    boolean parameterized;

	    readDefineTag(c, &Cpp.directive.tag, &parameterized);
	    makeDefineTag(&Cpp.directive.tag, scope, parameterized);
	}
	Cpp.directive.state = DRCTV_NONE;
	break;
    }
    return ignore;
}

/*  Called upon reading of a slash ('/') characters, determines whether a
 *  comment is encountered, and its type.
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

/*  Skips over a C style comment. According to ANSI specification a comment
 *  is treated as white space, so we perform this subsitution.
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

	    if (next != '/')
		c = next;
	    else
	    {
		c = ' ';			/* replace comment with space */
		break;
	    }
	}
    }
    return c;
}

/*  Skips over a C++ style comment.
 */
static int skipOverCplusComment()
{
    int c;

    while ((c = fileGetc()) != EOF)
    {
	if (c == BACKSLASH)
	    fileGetc();			/* throw away next character, too */
	else if (c == NEWLINE)
	    break;
    }
    return c;
}

/*  Skips to the end of a string, returning a special character to
 *  symbolically represent a generic string.
 */
static int skipToEndOfString()
{
    int c;

    while ((c = fileGetc()) != EOF)
    {
	if (c == BACKSLASH)
	    fileGetc();			/* throw away next character, too */
	else if (c == DOUBLE_QUOTE)
	    break;
	else if (c == NEWLINE)
	{
	    fileUngetc(c);
	    break;
	}
    }
    return STRING_SYMBOL;		/* symbolic representation of string */
}

/*  Skips to the end of the three (possibly four) 'c' sequence, returning a
 *  special character to symbolically represent a generic character.
 */
static int skipToEndOfChar()
{
    int c;

    while ((c = fileGetc()) != EOF)
    {
	if (c == BACKSLASH)
	    fileGetc();			/* throw away next character, too */
	else if (c == SINGLE_QUOTE)
	    break;
	else if (c == NEWLINE)
	{
	    fileUngetc(c);
	    break;
	}
    }
    return CHAR_SYMBOL;		    /* symbolic representation of character */
}

/*  This function returns the next character, stripping out comments,
 *  C pre-processor directives, and the contents of single and double
 *  quoted strings. In short, strip anything which places a burden upon
 *  the tokenizer.
 */
extern int cppGetc()
{
    boolean directive = FALSE;
    boolean ignore = FALSE;
    int c;

    if (Cpp.ungetch != '\0')
    {
	c = Cpp.ungetch;
	Cpp.ungetch = '\0';
	return c;	    /* return here to avoid re-calling debugPutc() */
    }
    else do
    {
	c = fileGetc();
	switch (c)
	{
	case EOF:
	    ignore	= FALSE;
	    directive	= FALSE;
	    break;
	case TAB:
	case SPACE:
	    break;				/* ignore most white space */
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
	  {
	    const Comment comment = isComment();

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
	  }
	default:
	    Cpp.directive.accept = FALSE;
	    if (directive)
		ignore = handleDirective(c);
	    break;
	}
    } while (directive || ignore);

    DebugStatement( debugPutc(c, DEBUG_CPP); )
    return c;
}

/* vi:set tabstop=8 shiftwidth=4: */
