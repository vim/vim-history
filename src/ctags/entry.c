/*****************************************************************************
*   $Id: entry.c,v 5.2 1998/02/26 05:32:07 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for creating tag entries.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#include <stdio.h>
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#ifdef DEBUG
# include <assert.h>
#endif
#include "ctags.h"

/*============================================================================
 *=	Defines
============================================================================*/
#define includeExtensionFlags()		(Option.tagFileFormat > 1)

/*============================================================================
=   Data definitions
============================================================================*/

/*  Note that the strings and characters in these arrays must correspond to
 *  the types in the tagType enumeration.
 */
static const char *TagTypeNames[] = {
    "class", "define", "macro", "enum", "enumerator", "prototype", "function",
    "member", "file", "struct", "typedef", "union", "variable"
};

static const char TagTypeChars[] = "cddgepfmFstuv";

/*============================================================================
=   Function prototypes
============================================================================*/
static size_t writeSourceLine __ARGS((FILE *const fp, const char *const line));
static size_t writeCompactSourceLine __ARGS((FILE *const fp, const char *const line));
static void rememberMaxLengths __ARGS((const size_t nameLength, const size_t lineLength));
static void writeXrefEntry __ARGS((const tagInfo *const tag, const tagType type));
static void truncateTagLine __ARGS((char *const line, const char *const token, const boolean discardNewline));
static int writeEtagsEntry __ARGS((const tagInfo *const tag, const memberInfo *const pMember, const tagScope scope, const tagType type));
static const char *getTypeString __ARGS((const memberType mType));
static int addExtensionFlags __ARGS((const memberInfo *const pMember, const tagScope scope, const tagType type));
static int writeLineNumberEntry __ARGS((const tagInfo *const tag, const memberInfo *const pMember, const tagScope scope, const tagType type));
static int writePatternEntry __ARGS((const tagInfo *const tag, const memberInfo *const pMember, const tagScope scope, const tagType type));
static void writeTagEntry __ARGS((const tagInfo *const tag, const memberInfo *const pMember, const tagScope scope, const tagType type, const boolean useLineNumber));
static boolean includeTag __ARGS((const tagScope scope, const tagType type));
static void makeTagEntry __ARGS((const tagInfo *const tag, const memberInfo *const pMember, const tagScope scope, const tagType type, const boolean useLineNumber));
static void writePseudoTag __ARGS((const char *const tagName, const char *const fileName, const char *const pattern));
static void updateSortedFlag __ARGS((const char *const line, FILE *const fp, const long startOfLine));

/*============================================================================
=   Function definitions
============================================================================*/

extern const char *tagTypeName( type )
    const tagType type;
{
    const char *name;

    if (type < TAG_NUMTYPES)
	name = TagTypeNames[type];
    else
	name = "?";

    return name;
}

/*  This function copies the current line out some other fp.  It has no
 *  effect on the fileGetc() function.  During copying, any '\' characters
 *  are doubled and a leading '^' or trailing '$' is also quoted.  The '\n'
 *  character is not copied.  If the '\n' is preceded by a '\r', then the
 *  '\r' isn't copied.
 *
 *  This is meant to be used when generating a tag line.
 */
static size_t writeSourceLine( fp, line )
    FILE *const fp;
    const char *const line;
{
    size_t length = 0;
    const char *p;
    char c;

    /*	Write everything up to, but not including, the newline.
     */
    for (p = line, c = *p  ;  c != NEWLINE  &&  c != '\0'  ;  ++p, c = *p)
    {
	const int next = *(p + 1);

	/*  If character is '\', or a terminal '$', then quote it.
	 */
	if (c == BACKSLASH  ||  c == (Option.backward ? '?' : '/')  ||
	    (c == '$'  &&  next == NEWLINE))
	{
	    putc(BACKSLASH, fp);
	    ++length;
	}

	/*  Copy the character, unless it is a terminal '\r'.
	 */
	if (c != CRETURN  ||  next != NEWLINE)
	{
	    putc(c, fp);
	    ++length;
	}
    }
    return length;
}

/*  Writes "line", stripping leading and duplicate white space.
 */
static size_t writeCompactSourceLine( fp, line )
    FILE *const fp;
    const char *const line;
{
    boolean lineStarted = FALSE;
    size_t  length = 0;
    const char *p;
    char c;

    /*	Write everything up to, but not including, the newline.
     */
    for (p = line, c = *p  ;  c != NEWLINE  &&  c != '\0'  ;  c = *++p)
    {
	if (lineStarted  || ! isspace(c))	/* ignore leading spaces */
	{
	    lineStarted = TRUE;
	    if (isspace(c))
	    {
		int next;

		/*  Consume repeating white space.
		 */
		while (next = *(p+1) , isspace(next)  &&  next != NEWLINE)
		    ++p;
		c = ' ';	/* force space character for any white space */
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

static void rememberMaxLengths( nameLength, lineLength )
    const size_t nameLength;
    const size_t lineLength;
{
    if (nameLength > TagFile.max.tag)
	TagFile.max.tag = nameLength;

    if (lineLength > TagFile.max.line)
	TagFile.max.line = lineLength;
}

static void writeXrefEntry( tag, type )
    const tagInfo *const tag;
    const tagType type;
{
    const char *const line = getSourceLine(&TagFile.line, tag->location);
    size_t length = fprintf(TagFile.fp, "%-20s %-10s %4lu  %-14s ",
			    tag->name, tagTypeName(type), tag->lineNumber,
			    File.name);

    length += writeCompactSourceLine(TagFile.fp, line);
    putc(NEWLINE, TagFile.fp);
    ++length;

    ++TagFile.numTags.added;
    rememberMaxLengths(strlen(tag->name), length);
}

/*  Truncates the text line containing the tag at the character following the
 *  tag, providing a character which designates the end of the tag.
 */
static void truncateTagLine( line, token, discardNewline )
    char *const line;
    const char *const token;
    const boolean discardNewline;
{
    char *p = strstr(line, token);

    if (p != NULL)
    {
	p += strlen(token);
	if (*p != '\0'  &&  !(*p == '\n'  &&  discardNewline))
	    ++p;		/* skip past character terminating character */
	*p = '\0';
    }
}

static int writeEtagsEntry( tag, pMember, scope, type )
    const tagInfo *const tag;
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
{
    char *const line = getSourceLine(&TagFile.line, tag->location);
    int length;

    truncateTagLine(line, tag->name, TRUE);
    if (pMember->type != MEMBER_NONE)
	length = fprintf(TagFile.etags.fp, "%s\177%s:%s\001%lu,%ld\n", line,
			 pMember->parent, tag->name, tag->lineNumber,
			 tag->location);
    else if (scope == SCOPE_STATIC)
	length = fprintf(TagFile.etags.fp, "%s\177%s:%s\001%lu,%ld\n", line,
			 File.name, tag->name, tag->lineNumber, tag->location);
    else if (type == TAG_CLASS   ||  type == TAG_DEFINE_FUNC  ||
	     type == TAG_ENUM    ||  type == TAG_STRUCT	      ||
	     type == TAG_TYPEDEF ||  type == TAG_UNION)
	length = fprintf(TagFile.etags.fp, "%s\177%s\001%lu,%ld\n", line,
			 tag->name, tag->lineNumber, tag->location);
    else
	length = fprintf(TagFile.etags.fp, "%s\177%lu,%ld\n", line,
			 tag->lineNumber, tag->location);
    TagFile.etags.byteCount += length;
    return length;
}

static const char *getTypeString( mType )
    const memberType mType;
{
    const char *typeString;

    switch (mType)
    {
	case MEMBER_ENUM:	typeString = "enum";	break;
	case MEMBER_CLASS:	typeString = "class";	break;
	case MEMBER_STRUCT:	typeString = "struct";	break;
	case MEMBER_UNION:	typeString = "union";	break;
	default:		typeString = NULL;	break;
    }
    return typeString;
}

static int addExtensionFlags( pMember, scope, type )
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
{
    const char *const prefix = ";\"\t";
    const char *const separator = "\t";
    int length = 0;
#ifdef LONG_FORM_TYPE
    const char *const format = "%stype:%c";
#else
    const char *const format = "%s%c";
#endif

    /*  Add an extension flag designating that the type of the tag.
     */
    length += fprintf(TagFile.fp, format, prefix, TagTypeChars[type]);

    /*  If this is a static tag, add the appropriate extension flag.
     */
    if (scope == SCOPE_STATIC)
	length += fprintf(TagFile.fp, "%sfile:", separator);

    /*  For selected tag types, append an extension flag designating the
     *  parent object in which the tag is defined.
     */
    switch (type)
    {
	case TAG_ENUMERATOR:
	case TAG_FUNCDECL:
	case TAG_FUNCTION:
	case TAG_MEMBER:
	case TAG_VARIABLE:
	{
	    const char *const typeString = getTypeString(pMember->type);

	    if (typeString != NULL)
		length += fprintf(TagFile.fp, "%s%s:%s", separator,
				  typeString, pMember->parent);
	}
	default: break;
    }
    return length;
}

static int writeLineNumberEntry( tag, pMember, scope, type )
    const tagInfo *const tag;
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
{
    int length = 0;

    length += fprintf(TagFile.fp, "%s\t%s\t%lu",
		      tag->name, File.name, tag->lineNumber);

    if (includeExtensionFlags())
	length += addExtensionFlags(pMember, scope, type);

    length += fprintf(TagFile.fp, "\n");

    return length;
}

static int writePatternEntry( tag, pMember, scope, type )
    const tagInfo *const tag;
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
{
    char *const line = getSourceLine(&TagFile.line, tag->location);
    const int searchChar = Option.backward ? '?' : '/';
    boolean newlineTerminated;
    int length = 0;

    if (type == TAG_DEFINE_OBJ  ||  type == TAG_DEFINE_FUNC)
	truncateTagLine(line, tag->name, FALSE);
    newlineTerminated = (boolean)(line[strlen(line) - 1] == '\n');

    length += fprintf(TagFile.fp, "%s\t%s\t", tag->name, File.name);
    length += fprintf(TagFile.fp, "%c^", searchChar);
    length += writeSourceLine(TagFile.fp, line);
    length += fprintf(TagFile.fp, "%s%c", newlineTerminated ? "$":"",
		      searchChar);

    if (includeExtensionFlags())
	length += addExtensionFlags(pMember, scope, type);

    length += fprintf(TagFile.fp, "\n");

    return length;
}

static void writeTagEntry( tag, pMember, scope, type, useLineNumber )
    const tagInfo *const tag;
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
    const boolean useLineNumber;
{
    size_t length;

    if (Option.etags)
	length = writeEtagsEntry(tag, pMember, scope, type);
    else if (useLineNumber || type == TAG_SOURCE_FILE)
	length = writeLineNumberEntry(tag, pMember, scope, type);
    else
	length = writePatternEntry(tag, pMember, scope, type);

    ++TagFile.numTags.added;
    rememberMaxLengths(strlen(tag->name), length);
}

static boolean includeTag( scope, type )
    const tagScope scope;
    const tagType type;
{
    boolean include;

    if (scope == SCOPE_EXTERN  ||		    /* should never happen */
	(scope == SCOPE_STATIC  &&  ! Option.include.statics))
    {
	include = FALSE;
    }
    else switch (type)
    {
	case TAG_CLASS:		include = Option.include.classNames;	break;
	case TAG_DEFINE_FUNC:
	case TAG_DEFINE_OBJ:    include = Option.include.defines;	break;
	case TAG_ENUM:		include = Option.include.enumNames;	break;
	case TAG_ENUMERATOR:	include = Option.include.enumerators;	break;
	case TAG_SOURCE_FILE:	include = Option.include.sourceFiles;	break;
	case TAG_FUNCTION:	include = Option.include.functions;	break;
	case TAG_FUNCDECL:	include = Option.include.prototypes;	break;
	case TAG_MEMBER:	include = Option.include.members;	break;
	case TAG_STRUCT:	include = Option.include.structNames;	break;
	case TAG_TYPEDEF:	include = Option.include.typedefs;	break;
	case TAG_UNION:		include = Option.include.unionNames;	break;
	case TAG_VARIABLE:	include = Option.include.variables;	break;
	default:		include = FALSE;			break;
    }
    return include;
}

/*  This function generates a tag for the object in name, whose tag line is
 *  located at a given seek offset.
 */
static void makeTagEntry( tag, pMember, scope, type, useLineNumber )
    const tagInfo *const tag;
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
    const boolean useLineNumber;
{
    if (includeTag(scope, type))
    {
	if (Option.xref)
	    writeXrefEntry(tag, type);
	else
	    writeTagEntry(tag, pMember, scope, type, useLineNumber);

	DebugStatement( debugEntry(scope, type, tag->name, pMember); )
    }
}

extern void makeTag( tag, pMember, scope, type )
    const tagInfo *const tag;
    const memberInfo *const pMember;
    const tagScope scope;
    const tagType type;
{
    makeTagEntry(tag, pMember, scope, type,
		 (boolean)(Option.locate == EX_LINENUM));
}

extern void makeDefineTag( tag, scope, parameterized )
    const tagInfo *const tag;
    const tagScope scope;
    const boolean parameterized;
{
    const tagType type = (parameterized ? TAG_DEFINE_FUNC : TAG_DEFINE_OBJ);

    makeTagEntry(tag, &NoClass, scope, type,
		 (boolean)(Option.locate != EX_PATTERN));
}

/*----------------------------------------------------------------------------
*-	Pseudo tag support
----------------------------------------------------------------------------*/

static void writePseudoTag( tagName, fileName, pattern )
    const char *const tagName;
    const char *const fileName;
    const char *const pattern;
{
    const size_t length = fprintf(TagFile.fp, "%s%s\t%s\t/%s/\n",
				 PSEUDO_TAG_PREFIX, tagName, fileName, pattern);
    ++TagFile.numTags.added;
    rememberMaxLengths(strlen(tagName), length);
}

extern void addPseudoTags()
{
    if (! Option.xref  &&  ! Option.etags)
    {
	char format[11];
	const char *formatComment = "unknown format";

	sprintf(format, "%u", Option.tagFileFormat);

	if (Option.tagFileFormat == 1)
	    formatComment = "original ctags format";
	else if (Option.tagFileFormat == 2)
	    formatComment =
		    "extended format; --format=1 will not append ;\" to lines";

	writePseudoTag("TAG_FILE_FORMAT", format, formatComment);
	writePseudoTag("TAG_FILE_SORTED", Option.sorted ? "1":"0",
		       "0=unsorted, 1=sorted");
	writePseudoTag("TAG_PROGRAM_AUTHOR",	AUTHOR_NAME,  AUTHOR_EMAIL);
	writePseudoTag("TAG_PROGRAM_NAME",	PROGRAM_NAME, "");
	writePseudoTag("TAG_PROGRAM_URL",	PROGRAM_URL,  "official site");
	writePseudoTag("TAG_PROGRAM_VERSION",	PROGRAM_VERSION,
		       "with C++ support");
    }
}

static void updateSortedFlag( line, fp, startOfLine )
    const char *const line;
    FILE *const fp;
    const long startOfLine;
{
    const char *const tab = strchr(line, '\t');

    if (tab != NULL)
    {
	const long boolOffset = tab - line + 1;	/* where it should be */

	if (line[boolOffset] == '0'  ||  line[boolOffset] == '1')
	{
	    const long nextLine = ftell(fp);

	    fseek(fp, startOfLine + boolOffset, SEEK_SET);
	    fputc(Option.sorted ? '1' : '0', fp);
	    fseek(fp, nextLine, SEEK_SET);
	}
    }
}

/*  Look through all line beginning with "!_TAG_FILE", and update those which
 *  require it.
 */
extern unsigned long updatePseudoTags()
{
    enum { maxClassLength = 20 };
    char class[maxClassLength + 1];
    FILE *const fp = TagFile.fp;
    long startOfLine = 0;
    unsigned long linesRead = 0;
    size_t classLength;
    const char *line;

    sprintf(class, "%sTAG_FILE", PSEUDO_TAG_PREFIX);
    classLength = strlen(class);
    DebugStatement( assert(classLength < maxClassLength); )
    rewind(fp);
    line = readLine(&TagFile.line, fp);
    while (line != NULL  &&  line[0] == class[0])
    {
	++linesRead;
	if (strncmp(line, class, classLength) == 0)
	{
	    char tab, classType[16];

	    if (sscanf(line + classLength, "%15s%c", classType, &tab) == 2  &&
		tab == '\t')
	    {
		if (strcmp(classType, "_SORTED") == 0)
		    updateSortedFlag(line, fp, startOfLine);
	    }
	    startOfLine = ftell(fp);
	}
	line = readLine(&TagFile.line, fp);
    }
    while (line != NULL)			/* skip to end of file */
    {
	++linesRead;
	line = readLine(&TagFile.line, fp);
    }
    return linesRead;
}

/* vi:set tabstop=8 shiftwidth=4: */
