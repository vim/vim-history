/*****************************************************************************
*   $Id: options.h,v 8.1 1999/03/04 04:16:38 darren Exp $
*
*   Copyright (c) 1998-1999, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module is a global include file.
*****************************************************************************/
#ifndef _OPTIONS_H
#define _OPTIONS_H

#ifdef OPTION_WRITE
# define CONST_OPTION
#else
# define CONST_OPTION const
#endif

/*============================================================================
=   Include files
============================================================================*/
#include "general.h"
#include "ctags.h"	/* to define LANG_COUNT */
#include "vstring.h"

#define EMPTY_SLIST { 0, 0, NULL }

#define stringListCount(slist)	((slist)->count)
#define stringListItem(slist,i)	((slist)->list[(i)])

/*============================================================================
=   Data declarations
============================================================================*/

typedef struct sStringList {
    unsigned int max;
    unsigned int count;
    vString **list;
} stringList;

/*  This stores the command line options.
 */
typedef struct sOptionValues {
    struct sInclude {		/* include tags for: */
	struct sCInclude {
	    boolean	classNames;
	    boolean	defines;
	    boolean	enumerators;
	    boolean	functions;
	    boolean	enumNames;
	    boolean	members;
	    boolean	namespaceNames;
	    boolean	prototypes;
	    boolean	structNames;
	    boolean	typedefs;
	    boolean	unionNames;
	    boolean	variables;
	    boolean	externVars;
	    boolean	access;
	    boolean	classPrefix;
	} c;
	struct sEiffelInclude {
	    boolean	classNames;
	    boolean	features;
	    boolean	localEntities;
	} eiffel;
	struct sFortranInclude {
	    boolean	blockData;
	    boolean	commonBlocks;
	    boolean	entryPoints;
	    boolean	functions;
	    boolean	interfaces;
	    boolean	labels;
	    boolean	modules;
	    boolean	namelists;
	    boolean	programs;
	    boolean	subroutines;
	    boolean	types;
	} fortran;
	struct sJavaInclude {
	    boolean	classNames;
	    boolean	fields;
	    boolean	interfaceNames;
	    boolean	methods;
	    boolean	packageNames;
	    boolean	access;
	    boolean	classPrefix;
	} java;
	boolean fileNames;	/* include tags for source file names */
	boolean	fileScope;	/* include tags of file scope only */
    } include;
    stringList ignore;	    /* -I  name of file containing tokens to ignore */
    boolean append;	    /* -a  append to "tags" files */
    boolean backward;	    /* -B  regexp patterns search backwards */
    boolean etags;	    /* -e  output Emacs style tags file */
    enum eLocate {
	EX_MIX,		    /* line numbers for defines, patterns otherwise */
	EX_LINENUM,	    /* -n  only line numbers in tag file */
	EX_PATTERN	    /* -N  only patterns in tag file */
    } locate;		    /* --excmd  EX command used to locate tag */
    const char *path;	    /* -p  default path for source files */
    boolean recurse;	    /* -R  recurse into directories */
    boolean sorted;	    /* -u,--sort  sort tags */
    boolean verbose;	    /* -V  verbose */
    boolean xref;	    /* -x  generate xref output instead */
    const char *fileList;   /* -L  name of file containing names of files */
    const char *tagFileName;/* -o  name of tags file */
    stringList headerExt;   /* -h  header extensions */
    stringList etagsInclude;/* --etags-include  list of TAGS files to include */
    unsigned int tagFileFormat;/* --format  tag file format (level) */
    boolean if0;	    /* --if0  examine code within "#if 0" branch */
    boolean kindLong;	    /* --kind-long */
    langType language;	    /* --lang specified language override */
    stringList langMap[(int)LANG_COUNT];
			    /* --langmap  language-extension map */
    boolean followLinks;    /* --link  follow symbolic links? */
    boolean printTotals;    /* --totals  print cumulative statistics */
    boolean lineDirectives; /* --linedirectives  process #line directives */
#ifdef DEBUG
    int debugLevel;	    /* -D  debugging output */
    unsigned long breakLine;/* -b  source line at which to call lineBreak() */
#endif
} optionValues;

/*============================================================================
=   Global variables
============================================================================*/
extern CONST_OPTION optionValues	Option;

/*============================================================================
=   Function prototypes
============================================================================*/
extern void setDefaultTagFileName __ARGS((void));
extern void setOptionDefaults __ARGS((void));
extern void testEtagsInvocation __ARGS((void));
extern boolean isFileHeader __ARGS((const char *const fileName));
extern langType getFileLanguage __ARGS((const char *const fileName));
extern boolean isIgnoreToken __ARGS((const char *const name, boolean *const pIgnoreParens, const char **const replacement));
extern const char *getLanguageName __ARGS((const langType language));
extern char *const *parseOptions __ARGS((char *const *const argList));
extern void *parseEnvironmentOptions __ARGS((void));
extern void initOptions __ARGS((void));
extern void freeOptionResources __ARGS((void));

#endif	/* _OPTIONS_H */

/* vi:set tabstop=8 shiftwidth=4: */
