/*****************************************************************************
*   $Id: sort.c,v 5.1 1998/02/19 03:47:18 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
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
#ifdef EXTERNAL_SORT
static void reportWarnings __ARGS((void));
#else
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

static void reportWarnings()
{
#ifdef AWK
    if (strlen(AWK) > (size_t)0)
    {
	const char *const awkProgTemplate = "%s{if ($1==prev){printf(\"Duplicate entry in \") | \"cat>&2\"; if ($%d!=prevfile) printf(\"%%s and \",prevfile) | \"cat>&2\"; printf(\"%%s: %%s\\n\",$%d,$1) | \"cat>&2\"; } else {prev=$1;prevfile=$%d}}";
	char *awkProg;
	const char *begin;
	size_t length;
	int fileArg;

	if (Option.xref)    { fileArg = 4;  begin = ""; }
	else		    { fileArg = 2;  begin = "BEGIN{FS=\"\\t\"}"; }

	length = strlen(awkProgTemplate) + strlen(begin) + 3;
	awkProg = (char *)malloc(length);
	if (awkProg != NULL)
	{
	    const char *const cmdTemplate = "%s '%s' %s";
	    char *cmd;
	    int ret;

	    sprintf(awkProg, awkProgTemplate, begin, fileArg, fileArg, fileArg);
	    length = strlen(cmdTemplate) + strlen(AWK) +
		    strlen(awkProg) + strlen(TagFile.name);
	    cmd = (char *)malloc(length);
	    if (cmd != NULL)
	    {
		sprintf(cmd, cmdTemplate, AWK, awkProg, TagFile.name);
		ret = system(cmd);
		free(cmd);
	    }
	    free(awkProg);
	}
    }
#endif
}

extern void externalSortTags( toStdout )
    const boolean toStdout;
{
    const char *const sortTemplate = Option.warnings ? "sort -o %s %s" :
						       "sort -u -o %s %s";
    const size_t length	= strlen(sortTemplate) + 2 * strlen(TagFile.name);
    char *const cmd	= (char *)malloc(length);

    if (cmd != NULL)
    {
	int ret;

	sprintf(cmd, sortTemplate, TagFile.name, TagFile.name);
	ret = system(cmd);
	free(cmd);

	if (ret == 0  &&  Option.warnings)
	    reportWarnings();
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
    char *thisTag = NULL, *prevTag = NULL, *thisFile = NULL, *prevFile = NULL;
    size_t i;

    if (Option.warnings)
    {
	thisTag  = (char *)malloc(TagFile.max.tag  + (size_t)1);
	prevTag  = (char *)malloc(TagFile.max.tag  + (size_t)1);
	thisFile = (char *)malloc(TagFile.max.file + (size_t)1);
	prevFile = (char *)malloc(TagFile.max.file + (size_t)1);

	if (thisTag  == NULL  ||   prevTag == NULL  ||
	    thisFile == NULL  ||  prevFile == NULL)
	{
	    error(WARNING | PERROR, "cannot generate duplicate tag warnings");
	}
	*prevTag  = *thisTag  = '\0';
	*prevFile = *thisFile = '\0';
    }

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

	if (Option.warnings)
	{
	    int fields;

	    if (Option.xref)
		fields = sscanf(table[i],"%s %*s %*s %s", thisTag, thisFile);
	    else
		fields = sscanf(table[i],"%[^\t]\t%[^\t]", thisTag, thisFile);

	    if (fields == 2  &&  strcmp(thisTag, prevTag) == 0)
	    {
		fprintf(errout, "Duplicate entry in ");
		if (strcmp(thisFile, prevFile) != 0)
		    fprintf(errout, "%s and ", prevFile);
		fprintf(errout, "%s: %s\n", thisFile, thisTag);
	    }
	    strcpy(prevTag , thisTag );
	    strcpy(prevFile, thisFile);
	}
    }
    if (Option.warnings)
    {
	free(thisTag);
	free(prevTag);
	free(thisFile);
	free(prevFile);
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
