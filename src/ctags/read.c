/*****************************************************************************
*   $Id: read.c,v 5.1 1998/02/19 03:47:18 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains low level source and tag file read functions (line
*   splicing and newline conversion for source files are performed at this
*   level).
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <limits.h>		/* to define INT_MAX or MAXINT */

#include "ctags.h"

/*============================================================================
=   Data definitions
============================================================================*/

sourceFile File = { NULL, NULL, 0, -1, FALSE, 0, FALSE, FALSE };

/*============================================================================
=   Function prototypes
============================================================================*/
static boolean isFileHeader __ARGS((const char *const name));
static void fileNewline __ARGS((void));
static boolean resizeLineBuffer __ARGS((lineBuf *const pLine));

/*============================================================================
=   Function definitions
============================================================================*/

/*----------------------------------------------------------------------------
 *	Generic line reading with automatic buffer sizing
 *--------------------------------------------------------------------------*/

extern void freeLineBuffer( pLine )
    lineBuf *const pLine;
{
    if (pLine->buffer != NULL)
	free(pLine->buffer);
    pLine->buffer = NULL;
    pLine->size = 0;
}

static boolean resizeLineBuffer( pLine )
    lineBuf *const pLine;
{
    boolean ok = FALSE;

    if (pLine->size <= INT_MAX / 2)
    {
	const int newSize = pLine->size * 2;
	char *const newBuffer = (char *)realloc(pLine->buffer,(size_t)newSize);

	if (newBuffer != NULL)
	{
	    pLine->size = newSize;
	    pLine->buffer = newBuffer;
	    ok = TRUE;
	}
    }
    return ok;
}

/*  Read a newline terminated line from 'fp' and if it overflows the buffer
 *  specified by 'pLine', increase the buffer. The buffer never shrinks. The
 *  size of the buffer will always be the longest line encountered so far.
 */
extern char *readLine( pLine, fp )
    lineBuf *const pLine;
    FILE *const fp;
{
    char *line = NULL;

    if (fp != NULL)		/* to free memory allocated to buffer */
    {
	boolean reReadLine;

	if (pLine->buffer == NULL)	/* if buffer not yet allocated... */
	{
	    pLine->size = 32;
	    pLine->buffer = (char *)malloc((size_t)pLine->size);
	}
	/*  If reading the line places any character other than a null or a
	 *  newline at the last character position in the buffer (one less
	 *  than the buffer size), then we must resize the buffer and
	 *  reattempt to read the line.
	 */
	if (pLine->buffer != NULL) do 
	{
	    const long startOfLine = ftell(fp);
	    char *const pLastChar = pLine->buffer + pLine->size - 2;

	    reReadLine = FALSE;
	    *pLastChar = '\0';
	    line = fgets(pLine->buffer, pLine->size, fp);
	    if (*pLastChar != '\0'  &&  *pLastChar != '\n')	/* overflow */
	    {
		if ((reReadLine = resizeLineBuffer(pLine)))
		    fseek(fp, startOfLine, SEEK_SET);
	    }
	} while (reReadLine);
    }
    return line;
}

/*----------------------------------------------------------------------------
 *	Source file access functions
 *--------------------------------------------------------------------------*/

/*  Determines whether the specified file name is considered to be a header
 *  file for the purposes of determining whether enclosed tags are global or
 *  static.
 */
static boolean isFileHeader( name )
    const char *const name;
{
    boolean header = FALSE;		    /* default unless match found */
    const char *extension;

    extension = strrchr(name, '.');	    /* find last '.' */
    if (extension != NULL)
    {
	int i;

	++extension;			    /* skip to character after '.' */
	for (i = 0 ; Option.headerExt[i] != NULL ; ++i)
	{
	    if (strcmp(Option.headerExt[i], extension) == 0)
	    {
		header = TRUE;		    /* found in list */
		break;
	    }
	}
    }
    return header;
}

/*  This function opens a source file, and resets the line counter.  If it
 *  fails, it will display an error message and leave the File.fp set to NULL.
 */
extern boolean fileOpen( name )
    const char *const name;
{
    boolean opened = FALSE;

    /*	If another file was already open, then close it.
     */
    if (File.fp != NULL)
    {
	fclose(File.fp);		/* close any open source file */
	File.fp = NULL;
    }

    if (! doesFileExist(name))
	error(WARNING | PERROR, "cannot open \"%s\"", name);
    else if (isNormalFile(name))
    {
	File.fp = fopen(name, "rb");	/* must be binary mode for fseek() */
	if (File.fp == NULL)
	    error(WARNING | PERROR, "cannot open \"%s\"", name);
	else
	{
	    opened = TRUE;
	    File.name	    = name;
	    File.lineNumber = 0L;
	    File.seek	    = 0L;
	    File.afterNL    = TRUE;
	    File.warned	    = FALSE;

	    if (strlen(name) > TagFile.max.file)
		TagFile.max.file = strlen(name);

	    /*	Determine whether this is a header File.
	     */
	    File.header = isFileHeader(name);
	    DebugStatement( debugOpen(name); )
	}
    }
    return opened;
}

extern void fileClose()
{
    if (File.fp != NULL)
    {
	/*  The line count of the file is 1 too big, since it is one-based
	 *  and is incremented upon each newline.
	 */
	if (Option.printTotals)
	    addTotals(0, File.lineNumber - 1L, getFileSize(File.name));

	fclose(File.fp);
	File.fp = NULL;
    }
}

/*  Action to take for each encountered source newline.
 */
static void fileNewline()
{
    File.afterNL = FALSE;
    File.seek	 = ftell(File.fp);
    ++File.lineNumber;
    DebugStatement( if (Option.breakLine == File.lineNumber) lineBreak(); )
}

/*  This function reads a single character from the stream. 
 */
extern int fileGetc()
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
	return c;	    /* return here to avoid re-calling debugPutc() */
    }

nextChar:	/* not structured, but faster for this critical path */

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
	    ungetc(c, File.fp);		/* return character after BACKSLASH */
	    c = BACKSLASH;
	}
	break;

    case BACKSLASH:				/* test for line splicing */
	if (escaped)
	    ungetc(c, File.fp);			/* push back one just read */
	else
	{
	    escaped = TRUE;		/* defer test until next character */
	    goto nextChar;
	}
	break;

    /*	The following cases turn line breaks into a canonical form. The three
     *	commonly used forms if line breaks: LF (UNIX), CR (MacIntosh), and
     *	CR-LF (MS-DOS) are converted into a generic newline.
     */
    case CRETURN:
	{
	    const int next = getc(File.fp);	/* is CR followed by LF? */

	    /*	If this is a carriage-return/line-feed pair, treat it as one
	     *	newline, throwing away the line-feed.
	     */
	    if (next != NEWLINE)
		ungetc(next, File.fp);
	}
	c = NEWLINE;				/* convert CR into newline */
    case NEWLINE:
	File.afterNL = TRUE;
	if (escaped)				/* check for line splicing */
	{
	    DebugStatement(
		debugPutc(BACKSLASH, DEBUG_VISUAL);  /* print the characters */
		debugPutc(c, DEBUG_VISUAL);	     /*  we're throwing away */
	    )
	    escaped = FALSE;		    /* BACKSLASH now fully processed */
	    goto nextChar;		    /* through away "\NEWLINE" */
	}
	break;
    }

    DebugStatement( debugPutc(c, DEBUG_VISUAL); )
    return c;
}

extern void fileUngetc( c )
    int c;
{
    File.ungetch = c;
}

/*  Places into the line buffer the contents of the line referenced by
 *  "location".
 */
extern char *getSourceLine( pLine, location )
    lineBuf *const pLine;
    const long location;
{
    const long orignalPosition = ftell(File.fp);
    char *line;

    fseek(File.fp, location, SEEK_SET);
    line = readLine(pLine, File.fp);	
    fseek(File.fp, orignalPosition, SEEK_SET);

    return line;
}

/* vi:set tabstop=8 shiftwidth=4: */
