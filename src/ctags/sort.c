/*****************************************************************************
*   $Id: sort.c,v 6.2 1998/07/02 06:10:55 darren Exp $
*
*   Copyright (c) 1996-1998, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions to sort the tag entries.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "ctags.h"

/*============================================================================
=   Function prototypes
============================================================================*/

/*  Tag sorting functions.
 */
#ifndef EXTERNAL_SORT
static void failedSort __ARGS((void));
static int compareTags __ARGS((const void *const one, const void *const two));
static void writeSortedTags __ARGS((char **const table, const size_t numTags, const boolean toStdout));
#endif

/*============================================================================
=   Function definitions
============================================================================*/

extern void catFile( name )
    const char *const name;
{
    FILE *const fp = fopen(name, "r");

    if (fp != NULL)
    {
	int c;

	while ((c = getc(fp)) != EOF)
	    putchar(c);
	fclose(fp);
    }
}

#ifdef EXTERNAL_SORT

extern void externalSortTags( toStdout )
    const boolean toStdout;
{
    const char *const sortTemplate = "%ssort -u -o %s %s";
#ifndef NON_CONST_PUTENV_PROTOTYPE
    const
#endif
	  char *const sortOrder = "LC_COLLATE=C ";
    const char *env = "";
    const size_t length	= strlen(sortOrder) + strlen(sortTemplate) +
	    			2 * strlen(TagFile.name);
    char *const cmd = (char *)malloc(length);

    if (cmd != NULL)
    {
	int ret;

	/*  Ensure ASCII value sort order.
	 */
#ifdef HAVE_PUTENV
	putenv(sortOrder);	
#else
# ifdef HAVE_SETENV
	setenv("LC_COLLATE", "C", 1);
# else
	env = sortOrder;
# endif
#endif
	sprintf(cmd, sortTemplate, env, TagFile.name, TagFile.name);
	ret = system(cmd);
	free(cmd);

	if (ret != 0)
	    error(FATAL, "cannot sort tag file");
    }
    if (toStdout)
	catFile(TagFile.name);
}

#else

/*----------------------------------------------------------------------------
 *  These functions provide a basic internal sort. No great memory
 *  optimization is performed (e.g. recursive subdivided sorts),
 *  so have lots of memory if you have large tag files.
 *--------------------------------------------------------------------------*/

static void failedSort()
{
    if (TagFile.fp != NULL)
    {
	fclose(TagFile.fp);
	TagFile.fp = NULL;
    }
    error(FATAL | PERROR, "cannot sort tag file");
}

static int compareTags( one, two )
    const void *const one;
    const void *const two;
{
    const char *const line1 = *(const char *const *const)one;
    const char *const line2 = *(const char *const *const)two;

    return strcmp(line1, line2);
}

static void writeSortedTags( table, numTags, toStdout )
    char **const table;
    const size_t numTags;
    const boolean toStdout;
{
    size_t i;

    /*	Write the sorted lines back into the tag file.
     */
    if (toStdout)
	TagFile.fp = stdout;
    else
    {
	TagFile.fp = fopen(TagFile.name, "w");
	if (TagFile.fp == NULL)
	    failedSort();
    }
    for (i = 0 ; i < numTags ; ++i)
    {
	/*  Here we filter out identical tag *lines* (including search
	 *  pattern) if this is not an xref file.
	 */
	if (i == 0  ||  Option.xref  ||  strcmp(table[i], table[i-1]) != 0)
	    if (fputs(table[i], TagFile.fp) == EOF)
		failedSort();
    }
    if (! toStdout)
	fclose(TagFile.fp);
}

extern void internalSortTags( toStdout )
    const boolean toStdout;
{
    size_t i;
    const char *line;

    /*	Allocate a table of line pointers to be sorted.
     */
    size_t numTags	= TagFile.numTags.added + TagFile.numTags.prev;
    const size_t tableSize = numTags * sizeof(char *);
    char **const table	= (char **)malloc(tableSize);	/* line pointers */
    DebugStatement( size_t mallocSize = tableSize; )	/* cumulative total */

    if (table == NULL)
	failedSort();

    /*	Open the tag file and place its lines into allocated buffers.
     */
    TagFile.fp = fopen(TagFile.name, "r");
    if (TagFile.fp == NULL)
	failedSort();
    for (i = 0  ;  i < numTags  &&  ! feof(TagFile.fp)  ;  )
    {
	line = readLine(&TagFile.line, TagFile.fp);
	if (line == NULL)
	{
	    if (! feof(TagFile.fp))
		failedSort();
	    break;
	}
	else if (*line == '\0'  ||  strcmp(line, "\n") == 0)
	    ;		/* ignore blank lines */
	else
	{
	    const size_t stringSize = strlen(line) + 1;

	    table[i] = (char *)malloc(stringSize);
	    if (table[i] == NULL)
		failedSort();
	    DebugStatement( mallocSize += stringSize; )
	    strcpy(table[i], line);
	    ++i;
	}
    }
    numTags = i;
    fclose(TagFile.fp);

    /*	Sort the lines.
     */
    qsort(table, numTags, sizeof(*table),
	  (int (*)__ARGS((const void *, const void *)))compareTags);

    writeSortedTags(table, numTags, toStdout);

    DebugStatement( if (debug(DEBUG_STATUS))
			printf("sort memory: %ld bytes\n", (long)mallocSize); )
    for (i = 0 ; i < numTags ; ++i)
	free(table[i]);
    free(table);
}

#endif

/* vi:set tabstop=8 shiftwidth=4: */
