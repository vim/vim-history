/*****************************************************************************
*   $Id$
*
*   Copyright (c) 1998-2000, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   External interface to read.c
*****************************************************************************/
#ifndef _READ_H
#define _READ_H

#if defined(FILE_WRITE) || defined(VAXC)
# define CONST_FILE
#else
# define CONST_FILE const
#endif

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"	/* must always come first */

#include <stdio.h>
#include <ctype.h>

#include "parse.h"
#include "vstring.h"

/*============================================================================
=   Macros
============================================================================*/
#define fileLanguage()		(File.language)
#define isLanguage(lang)	(boolean)((lang) == File.language)
#define getInputFileLine()	File.lineNumber
#define getFileName()		vStringValue(File.source.name)
#define getFileLine()		File.source.lineNumber
#define getFilePosition()	File.filePosition
#define isHeaderFile()		File.source.isHeader

/*  Is the character valid as a character of a C identifier?
 */
#define isident(c)	(isalnum(c) || (c) == '_')

/*  Is the character valid as the first character of a C identifier?
 */
#define isident1(c)	(isalpha(c) || (c) == '_' || (c) == '~')

/*============================================================================
=   Data declarations
============================================================================*/

enum eCharacters {
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

/*  Maintains the state of the current source file.
 */
typedef struct sInputFile {
    vString	*name;		/* name of the input file */
    vString	*path;		/* the path of the input file (if any) */
    FILE	*fp;		/* stream used for reading the file */
    unsigned long lineNumber;	/* line number in the input file */
    fpos_t	filePosition;	/* file position of current line */
    int		ungetch;	/* a single character that was ungotten */
    boolean	eof;		/* have we reached the end of file? */
    boolean	newLine;	/* will the next character begin a new line? */
    langType	language;	/* language of input file */

    /*  Contains data pertaining to the original source file in which the tag
     *  was defined. This may be different from the input file when #line
     *  directives are processed (i.e. the input file is preprocessor output).
     */
    struct sSource {
	vString	*name;			/* name to report for source file */
	unsigned long lineNumber;	/* line number in the source file */
	boolean	 isHeader;		/* is source file a header file? */
	langType language;		/* language of source file */
    } source;
} inputFile;

/*============================================================================
=   Global variables
============================================================================*/
extern CONST_FILE inputFile File;

/*============================================================================
=   Function prototypes
============================================================================*/
#ifdef NEED_PROTO_FGETPOS
extern int fgetpos  __ARGS((FILE *stream, fpos_t *pos));
extern int fsetpos  __ARGS((FILE *stream, const fpos_t *pos));
#endif

extern void freeSourceFileResources __ARGS((void));
extern char *readLine __ARGS((vString *const vLine, FILE *const fp));
extern void setSourceFileName __ARGS((vString *const fileName));
extern void setSourceFileLine __ARGS((const long unsigned int lineNumber));
extern boolean fileEOF __ARGS((void));
extern boolean fileOpen __ARGS((const char *const fileName, const langType language));
extern void fileClose __ARGS((void));
extern int fileGetc __ARGS((void));
extern void fileUngetc __ARGS((int c));
extern char *fileReadLine __ARGS((vString *const vLine));
extern char *readSourceLine __ARGS((vString *const vLine, fpos_t location, long *const pSeekValue));

#endif	/* _READ_H */

/* vi:set tabstop=8 shiftwidth=4: */
