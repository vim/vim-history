/*****************************************************************************
*   $Id: main.c,v 5.2 1998/02/26 05:32:07 darren Exp $
*
*   Copyright (c) 1996-1997, Darren Hiebert
*
*   Author: Darren Hiebert <darren@hiebert.com>
*           http://home.hiwaay.net/~darren
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License. It is provided on an as-is basis and no
*   responsibility is accepted for its failure to perform as expected.
*
*   This is a reimplementation of the ctags(1) program. It is an attempt to
*   provide a fully featured ctags program which is free of the limitations
*   which most (all?) others are subject to.
*
*   It is derived from and inspired by the ctags program by Steve Kirkendall
*   (kirkenda@cs.pdx.edu) that comes with the Elvis vi clone (though almost
*   none of the original code remains). This, too, was freely available.
*
*   This program provides the following features:
*
*   Support for both K&R style and new ANSI style function definitions.
*
*   Generates tags for the following objects:
*	- macro definitions
*	- enumeration values
*	- function definitions (and C++ methods)
*	- function declarations (optional)
*	- enum, struct and union tags and C++ class names
*	- typedefs
*	- variables
*
*   This module contains top level start-up and portability functions.
*****************************************************************************/

/*============================================================================
=   Include files
============================================================================*/
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
# define HAVE_TIME_H
# define HAVE_CLOCK
# define HAVE_CHSIZE
# define HAVE_STRERROR
#endif

#ifdef DJGPP
# define HAVE_UNISTD_H
# define HAVE_TRUNCATE
#endif

#ifdef AMIGA
# define HAVE_TIME_H
# define HAVE_SYS_STAT_H
# define HAVE_CLOCK
# define HAVE_STRERROR
# define PATH_MAX 127
#endif

#if defined(HAVE_UNISTD_H)
# include <unistd.h>	/* to declare close(), ftruncate(), truncate() */
#endif

#include <errno.h>

#ifdef __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif

/*  To declare "struct stat" and stat().
 */
#if defined(__MWERKS__) && defined(__MACINTOSH__)
# include <stat.h>		/* there is no sys directory on the Mac */
#else
# include <sys/types.h>
# include <sys/stat.h>
#endif

/*  To define the maximum path length
 */
#include <limits.h>			/* to define PATH_MAX (hopefully) */
#if !defined(PATH_MAX)  &&  defined(HAVE_DIRENT_H)
# include <dirent.h> 
# ifdef MAXNAMLEN
#  define PATH_MAX MAXNAMLEN
# endif
#endif
#if !defined(PATH_MAX) && defined(MSDOS) && defined(__BORLANDC__)
# include <dir.h>			/* to define MAXPATH */
# ifdef MAXPATH				/* found in Borland C <dir.h> */
#  define PATH_MAX MAXPATH
# endif
#endif
#ifndef PATH_MAX
# define PATH_MAX 127			/* ultimate fall-back */
#endif

/*  To provide timings features if available.
 */
#ifdef HAVE_CLOCK
# ifdef HAVE_TIME_H
#  include <time.h>
# endif
#else
# ifdef HAVE_TIMES
#  ifdef HAVE_SYS_TIMES_H
#   include <sys/times.h>
#  endif
# endif
#endif

/*  These header files provide for the functions necessary to do file
 *  truncation.
 */
#include <fcntl.h>
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
# include <io.h>
#endif

#ifdef DEBUG
# include <assert.h>
#endif

#include "ctags.h"

/*============================================================================
=   Defines
============================================================================*/

/*----------------------------------------------------------------------------
 *  Portability defines
 *--------------------------------------------------------------------------*/
#if !defined(HAVE_TRUNCATE) && !defined(HAVE_FTRUNCATE) && !defined(HAVE_CHSIZE)
# define USE_REPLACEMENT_TRUNCATE
#endif

/*  File type tests.
 */
#ifndef S_ISREG
# if defined(S_IFREG) && !defined(AMIGA)
#  define S_ISREG(mode)	    ((mode) & S_IFREG)
# else
#  define S_ISREG(mode)	    TRUE	/* assume regular file */
# endif
#endif

#ifndef S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(mode)	    (((mode) & S_IFMT) == S_IFLNK)
# else
#  define S_ISLNK(mode)	    FALSE	/* assume no soft links */
# endif
#endif

/*  Hack for rediculous practice of Microsoft Visual C++.
 */
#if defined(WIN32) && !defined(__BORLANDC__)
# define chsize		_chsize
# define open		_open
# define close		_close
# define stat		_stat
# define O_RDWR 	_O_RDWR
#endif

/*----------------------------------------------------------------------------
 *  Miscellaneous defines
 *--------------------------------------------------------------------------*/
#ifndef ETAGS
# define ETAGS	"etags"		/* name which causes default use of to -e */
#endif

#define selected(var,feature)	(((var) & (feature)) == feature)

#define plural(value)		(((unsigned long)(value) == 1L) ? "" : "s")

/*============================================================================
=   Data definitions
============================================================================*/
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
const char PathDelimiters[] = ":/\\";
#endif

static const char *ExecutableName = NULL;

static struct { long files, lines, bytes; } Totals = { 0, 0, 0 };

tagFile TagFile = {
    NULL,		/* file name */
    NULL,		/* file pointer */
    { 0, 0 },		/* numTags */
    { 0, 0, 0 },	/* max */
    { "", NULL },	/* etags */
    { 0, NULL }		/* line */
};

memberInfo NoClass = { MEMBER_NONE, FALSE, "" };

/*============================================================================
=   Function prototypes
============================================================================*/
#ifdef NEED_PROTO_STAT
extern int stat __ARGS((const char *, struct stat *));
#endif

static boolean isValidTagAddress __ARGS((const char *const tag));
static boolean isCtagsLine __ARGS((const char *const tag));
static boolean isEtagsLine __ARGS((const char *const tag));
static boolean isTagFile __ARGS((const char *const filename));
static void openTagFile __ARGS((const boolean toStdout));
static void closeTagFile __ARGS((const boolean resize));

static void beginEtagsFile __ARGS((void));
static void endEtagsFile __ARGS((const char *const name));
static const char *baseFilename __ARGS((const char *const path));
static boolean createTagsForFile __ARGS((const char *const name));
static const char *getNextListFile __ARGS((FILE *const fp));
static boolean isAbsolutePath __ARGS((const char *const path));
static const char *sourceFilePath __ARGS((const char *const file));
static boolean createTagsWithFallback __ARGS((const char *const fileName));
static boolean createTagsForList __ARGS((const char *const listFile));
static boolean createTagsForArgs __ARGS((char *const *const argList));
static void printTotals __ARGS((const clock_t *const timeStamps));
static void makeTags __ARGS((char *const *const argList));

static void setExecutableName __ARGS((const char *const path));
static void setDefaultTagFileName __ARGS((void));
static void setOptionDefaults __ARGS((void));
static void testEtagsInvocation __ARGS((void));

extern int main __ARGS((int argc, char **argv));

/*============================================================================
=   Function definitions
============================================================================*/

#ifdef __STDC__
extern void error( const errorSelection selection,
		   const char *const format, ... )
#else
extern void error( va_alist )
    va_dcl
#endif
{
    va_list ap;

#ifdef __STDC__
    va_start(ap, format);
#else
    const char *format;
    errorSelection selection;

    va_start(ap);
    selection = va_arg(ap, errorSelection);
    format = va_arg(ap, char *);
#endif
    fprintf(errout, "%s: %s", getExecutableName(),
	    selected(selection, WARNING) ? "warning: " : "");
    vfprintf(errout, format, ap);
    if (selected(selection, PERROR))
#ifdef HAVE_STRERROR
	fprintf(errout, " : %s", strerror(errno));
#else
	perror(" ");
#endif
    fputs("\n", errout);
    va_end(ap);
    if (selected(selection, FATAL))
	exit(1);
}

extern unsigned long getFileSize( name )
    const char *const __unused__ name;
{
    struct stat fileStatus;
    unsigned long size = 0;

    if (stat(name, &fileStatus) == 0)
	size = fileStatus.st_size;

    return size;
}

extern boolean isNormalFile( name )
    const char *const name;
{
    boolean isNormal = FALSE;
    struct stat fileStatus;

    if (stat(name, &fileStatus) == 0)
	isNormal = (boolean)(S_ISLNK(fileStatus.st_mode) ||
			     S_ISREG(fileStatus.st_mode)  );

    return isNormal;
}

extern boolean doesFileExist( fileName )
    const char *const fileName;
{
    struct stat fileStatus;

    return (boolean)(stat(fileName, &fileStatus) == 0);
}

extern void addTotals( files, lines, bytes )
    const unsigned int files;
    const unsigned long lines;
    const unsigned long bytes;
{
    Totals.files += files;
    Totals.lines += lines;
    Totals.bytes += bytes;
}

/*----------------------------------------------------------------------------
 *  Tag file management
 *--------------------------------------------------------------------------*/

static boolean isValidTagAddress( excmd )
    const char *const excmd;
{
    boolean isValid = FALSE;

    if (strchr("/?", excmd[0]) != NULL)
	isValid = TRUE;
    else
    {
	char address[sizeof(long) * 3 + 1];

    	if (sscanf(excmd, "%[^;\n]", address) == 1  &&
	    strspn(address,"0123456789") == strlen(address))
		isValid = TRUE;
    }
    return isValid;
}

static boolean isCtagsLine( line )
    const char *const line;
{
    enum fieldList { TAG, TAB1, SRC_FILE, TAB2, EXCMD, NUM_FIELDS };
    boolean ok = FALSE;			/* we assume not unless confirmed */
    const size_t fieldLength = strlen(line) + 1;
    char *const fields = (char *)malloc((size_t)NUM_FIELDS * fieldLength);

    if (fields == NULL)
	error(FATAL, "Cannot analyze tag file");
    else
    {
#define field(x)	(fields + ((x) * fieldLength))

	const int numFields = sscanf(line, "%[^\t]%[\t]%[^\t]%[\t]%[^\n]",
				     field(TAG), field(TAB1), field(SRC_FILE),
				     field(TAB2), field(EXCMD));

	/*  There must be exactly five fields: two tab fields containing
	 *  exactly one tab each, the tag must not begin with "#", and the
	 *  file name should not end with ";", and the excmd must be
	 *  accceptable.
	 *
	 *  These conditions will reject tag-looking lines like:
	 *      int	a;	<C-comment>
	 *      #define	LABEL	<C-comment>
	 */
	if (numFields == NUM_FIELDS   &&
	    strlen(field(TAB1)) == 1  &&
	    strlen(field(TAB2)) == 1  &&
	    field(TAG)[0] != '#'      &&
	    field(SRC_FILE)[strlen(field(SRC_FILE)) - 1] != ';'  &&
	    isValidTagAddress(field(EXCMD)))
		ok = TRUE;

	free(fields);
    }
    return ok;
}

static boolean isEtagsLine( line )
    const char *const line;
{
    const char *const magic = "\f\n";

    return (boolean)(strncmp(line, magic, strlen(magic)) == 0);
}

static boolean isTagFile( filename )
    const char *const filename;
{
    boolean ok = FALSE;			/* we assume not unless confirmed */
    FILE *const fp = fopen(filename, "r");

    if (fp == NULL  &&  errno == ENOENT)
	ok = TRUE;
    else if (fp != NULL)
    {
	const char *line = readLine(&TagFile.line, fp);
	
	if (line == NULL)
	    ok = TRUE;
	else if (Option.etags)		/* check for etags magic number */
	    ok = isEtagsLine(line);
	else
	    ok = isCtagsLine(line);
	fclose(fp);
    }
    return ok;
}

static void openTagFile( toStdout )
    const boolean toStdout;
{
    static char tempName[L_tmpnam];

    /*  Open the tags File.
     */
    if (toStdout)
    {
	TagFile.name = tmpnam(tempName);
	TagFile.fp = fopen(TagFile.name, "w");
    }
    else
    {
	const char *const fname  = Option.tagFileName;
	const boolean fileExists = doesFileExist(fname);

	TagFile.name = fname;
	if (fileExists  &&  ! isTagFile(fname))
	    error(FATAL,
	      "\"%s\" doesn't look like a tag file; I refuse to overwrite it.",
		  fname);
	if (Option.append  &&  fileExists)
	{
	    TagFile.fp = fopen(fname, "r+");
	    if (TagFile.fp != NULL)
	    {
		TagFile.numTags.prev = updatePseudoTags();
		fseek(TagFile.fp, 0L, SEEK_END);
	    }
	}
	else
	{
	    TagFile.fp = fopen(fname, "w");
	    if (TagFile.fp != NULL)
		addPseudoTags();
	}
    }
    if (TagFile.fp == NULL)
    {
	error(FATAL | PERROR, "cannot open tag file");
	exit(1);
    }
}

#ifdef USE_REPLACEMENT_TRUNCATE

static int copyChars __ARGS((FILE *const fpFrom, FILE *const fpTo, const size_t size));
static int copyFile __ARGS((const char *const from, const char *const to, const size_t size));
static int replacementTruncate __ARGS((const char *const name, const size_t size));

static int copyChars( fpFrom, fpTo, size )
    FILE *const fpFrom;
    FILE *const fpTo;
    const size_t size;
{
    size_t count;
    int result = -1;

    for (count = 0  ;  count < size  ;  ++count)
    {
	const int c = getc(fpFrom);

	if (c == EOF  ||  putc(c, fpTo) == EOF)
	    break;
    }
    if (count == size)
	result = 0;
    return result;
}

static int copyFile( from, to, size )
    const char *const from;
    const char *const to;
    const size_t size;
{
    int result = -1;
    FILE *const fpFrom = fopen(from, "r");

    if (fpFrom != NULL)
    {
	FILE *const fpTo = fopen(to, "w");

	if (fpFrom != NULL)
	{
	    result = copyChars(fpFrom, fpTo, size);
	    if (result == 0)
		result = fclose(fpTo);
	}
	if (result == 0)
	    result = fclose(fpFrom);
    }
    return result;
}

/*  Replacement for missing library function.
 */
static int replacementTruncate( name, size )
    const char *const name;
    const size_t size;
{
    char tempName[L_tmpnam];
    int result = -1;

    if (tmpnam(tempName) != NULL)
    {
	result = copyFile(name, tempName, size);
	if (result == 0)
	    result = copyFile(tempName, name, size);
	if (result == 0)
	    result = remove(tempName);
    }
    return result;
}

#endif

static void closeTagFile( resize )
    const boolean resize;
{
    const long __unused__ size = ftell(TagFile.fp);

    fclose(TagFile.fp);
    if (resize)
    {
	int __unused__ result = 0;

#ifdef USE_REPLACEMENT_TRUNCATE
	result = replacementTruncate(TagFile.name, (size_t)size);
#else
# ifdef HAVE_TRUNCATE
	result = truncate(TagFile.name, (size_t)size);
# else
	const int fd = open(TagFile.name, O_RDWR);

	if (fd != -1)
	{
#  ifdef HAVE_FTRUNCATE
	    result = ftruncate(fd, (size_t)size);
#  else
#   ifdef HAVE_CHSIZE
	    result = chsize(fd, size);
#   endif
#  endif
	    close(fd);
	}
# endif
#endif
	if (result == -1)
	    fprintf(errout, "Cannot shorten tag file: errno = %d\n", errno);
    }
}

/*----------------------------------------------------------------------------
 *	Create tags
 *--------------------------------------------------------------------------*/

static void beginEtagsFile()
{
    tmpnam(TagFile.etags.name);
    TagFile.etags.fp = fopen(TagFile.etags.name, "w+");
    if (TagFile.etags.fp == NULL)
	error(FATAL | PERROR, "cannot open \"%s\"", TagFile.etags.name);
    TagFile.etags.byteCount = 0;
}

static void endEtagsFile( name )
    const char *const name;
{
    const char *line;

    fprintf(TagFile.fp, "\f\n%s,%ld\n", name, (long)TagFile.etags.byteCount);
    if (TagFile.etags.fp != NULL)
    {
	rewind(TagFile.etags.fp);
	while ((line = readLine(&TagFile.line, TagFile.etags.fp)) != NULL)
	    fputs(line, TagFile.fp);
	fclose(TagFile.etags.fp);
	remove(TagFile.etags.name);
    }
    TagFile.etags.name[0] = '\0';
}

static const char *baseFilename( path )
    const char *const path;
{
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    const char *tail = NULL;
    unsigned int i;

    /*  Find whichever of the path delimiters is last.
     */
    for (i = 0  ;  i < strlen(PathDelimiters)  ;  ++i)
    {
	const char *sep = strrchr(path, PathDelimiters[i]);

	if (sep > tail)
	    tail = sep;
    }
    if (tail == NULL)
	tail = path;
    else
	++tail;			/* step past last delimiter */

    return tail;
#else
    const char *tail = strrchr(path, '/');

    return (tail == NULL) ? path : tail + 1;
#endif
}

static boolean createTagsForFile( name )
    const char *const name;
{
    boolean ok;

    if (name == NULL)
	ok = FALSE;
    else
    {
	ok = TRUE;
	if (Option.etags)
	    beginEtagsFile();
	if (cppOpen(name))
	{
	    tagInfo tag;

	    DebugStatement( clearString(tag.name, MaxNameLength); )
	    strcpy(tag.name, baseFilename(name));
	    tag.location   = 0;
	    tag.lineNumber = 1;

	    makeTag(&tag, &NoClass, SCOPE_GLOBAL, TAG_SOURCE_FILE);
	    ok = createTags(0, NULL);
	    cppClose();
	}
	if (Option.etags)
	    endEtagsFile(name);
    }
    return ok;
}

static const char *getNextListFile( fp )
    FILE *const fp;
{
    static char fileName[PATH_MAX + 1];
    const char *const buf = fgets(fileName, PATH_MAX + 1, fp);

    if (buf != NULL)
    {
	char *const newline = strchr(fileName, '\n');

	if (newline == NULL)
	    fileName[PATH_MAX] = '\0';
	else
	    *newline = '\0';
    }
    return buf;
}

static boolean isAbsolutePath( path )
    const char *const path;
{
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    return (path[0] == '/'  ||  path[0] == '\\'  ||  path[1] == ':');
#else
    return (boolean)(path[0] == '/');
#endif
}

static const char *sourceFilePath( file )
    const char *const file;
{
    static char filePath[PATH_MAX + 1];
    const char *result = NULL;

    if (Option.path == NULL  ||  isAbsolutePath(file))
	result = file;
    else if (strlen(Option.path) + strlen(file) < (size_t)PATH_MAX)
    {
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
	if (strchr(PathDelimiters, Option.path[strlen(Option.path)-1]) != NULL)
	    sprintf(filePath, "%s%s", Option.path, file);
	else
	    sprintf(filePath, "%s%c%s", Option.path, '\\', file);
#else
	if ((Option.path[strlen(Option.path) - 1]) == '/')
	    sprintf(filePath, "%s%s", Option.path, file);
	else
	    sprintf(filePath, "%s%c%s", Option.path, '/', file);
#endif
	result = filePath;
    }
    return result;
}

static boolean createTagsWithFallback( fileName )
    const char *const fileName;
{
    const char *const filePath	= sourceFilePath(fileName);
    const unsigned long numTags	= TagFile.numTags.added;
    const long tagFilePosition	= ftell(TagFile.fp);
    boolean ok;

    if (createTagsForFile(filePath)  ||  Option.braceFormat)
    {
	Option.braceFormat = FALSE;
	ok = TRUE;
    }
    else
    {
	Option.braceFormat = TRUE;	/* force fallback for next pass */

	/*  Restore prior state of tag file.
	 */
	fseek(TagFile.fp, tagFilePosition, SEEK_SET);
	TagFile.numTags.added = numTags;
	DebugStatement( 
	    if (debug(DEBUG_STATUS))
		printf("%s: formatting error; retrying\n", fileName); )
	ok = FALSE;
    }
    return ok;
}

/*  Create tags for the source files listed in the file specified by
 *  "listFile".
 */
static boolean createTagsForList( listFile )
    const char *const listFile;
{
    FILE *const fp = (strcmp(listFile,"-") == 0) ? stdin : fopen(listFile, "r");
    boolean resize = FALSE;
    const char *fileName;

    if (fp == NULL)
	error(FATAL | PERROR, "cannot open \"%s\"", listFile);
    else
    {
	fileName = getNextListFile(fp);
	while (fileName != NULL  &&  fileName[0] != '\0')
	{
	    if (createTagsWithFallback(fileName))
	    {
		addTotals(1, 0L, 0L);
		fileName = getNextListFile(fp);
	    }
	    else
		resize = TRUE;
	}
	if (fp != stdin)
	    fclose(fp);
    }
    return resize;
}

static boolean createTagsForArgs( argList )
    char *const *const argList;
{
    boolean resize = FALSE;
    int argNum = 0;

    /*	Generate tags for each source file on the command line.
     */
    while (argList[argNum] != NULL)
    {
	if (createTagsWithFallback(argList[argNum]))
	{
	    ++argNum;
	    addTotals(1, 0L, 0L);
	}
	else
	    resize = TRUE;
    }
    return resize;
}

#ifdef HAVE_CLOCK
# define CLOCK_AVAILABLE
# ifndef CLOCKS_PER_SEC
#  define CLOCKS_PER_SEC	1000000
# endif
#else
# ifdef HAVE_TIMES
#  define CLOCK_AVAILABLE
#  define CLOCKS_PER_SEC	60
static clock_t clock()
{
    struct tms buf;

    times(&buf);
    return (buf.tms_utime + buf.tms_stime);
}
# else
#  define clock()  (clock_t)0
# endif
#endif

static void printTotals( timeStamps )
    const clock_t *const timeStamps;
{
    const unsigned long totalTags = TagFile.numTags.added +
				    TagFile.numTags.prev;

    fprintf(errout, "%ld file%s, %ld line%s (%ld kB) scanned",
	    Totals.files, plural(Totals.files),
	    Totals.lines, plural(Totals.lines),
	    Totals.bytes/1024L);
#ifdef CLOCK_AVAILABLE
    {
	const double interval = ((double)(timeStamps[1] - timeStamps[0])) /
				CLOCKS_PER_SEC;

	fprintf(errout, " in %.01f seconds", interval);
	if (interval != (double)0.0)
	    fprintf(errout, " (%lu kB/s)",
		    (unsigned long)(Totals.bytes / interval) / 1024L);
    }
#endif
    fputc('\n', errout);

    fprintf(errout, "%lu tag%s added to tag file",
	    TagFile.numTags.added, plural(TagFile.numTags.added));
    if (Option.append)
	fprintf(errout, " (now %lu tags)", totalTags);
    fputc('\n', errout);

    if (totalTags > 0  &&  Option.sorted)
    {
	fprintf(errout, "%lu tag%s sorted", totalTags, plural(totalTags));
	fputc('\n', errout);
    }

#ifdef DEBUG
    fprintf(errout, "longest tag line = %lu\n",
	    (unsigned long)TagFile.max.line);
#endif
}

static void makeTags( argList )
    char *const *const argList;
{
    boolean toStdout = FALSE;
    boolean resize = FALSE;
    clock_t timeStamps[2];

    if (Option.xref  ||  strcmp(Option.tagFileName, "-") == 0)
	toStdout = TRUE;

    openTagFile(toStdout);

    if (Option.printTotals) timeStamps[0] = clock();

    if (Option.fileList != NULL)
	resize = createTagsForList(Option.fileList);
    resize = (boolean)(createTagsForArgs(argList) || resize);

    if (Option.printTotals) timeStamps[1] = clock();

    closeTagFile(resize);
    freeIgnoreList();

    if (TagFile.numTags.added + TagFile.numTags.prev > 0L)
    {
	if (Option.sorted)
	{
#ifdef EXTERNAL_SORT
	    externalSortTags(toStdout);
#else
	    internalSortTags(toStdout);
#endif
	}
	else if (toStdout)
	    catFile(TagFile.name);
    }
    if (toStdout)
	remove(TagFile.name);		/* remove temporary file */

    if (Option.printTotals)
	printTotals(timeStamps);
}

/*----------------------------------------------------------------------------
 *	Start up code
 *--------------------------------------------------------------------------*/

static void setExecutableName( path )
    const char *const path;
{
    ExecutableName = baseFilename(path);
}

extern const char *getExecutableName()
{
    return ExecutableName;
}

static void setDefaultTagFileName()
{
    if (Option.tagFileName != NULL)
	;		/* accept given name */
    else if (Option.etags)
	Option.tagFileName = ETAGS_FILE;
    else
	Option.tagFileName = CTAGS_FILE;
}

static void setOptionDefaults()
{
    /*  Disable warnings, since these are only supported with sorted tags.
     */
    if (! Option.sorted)
	Option.warnings = FALSE;

    if (Option.xref)
	Option.include.sourceFiles = FALSE;

    setDefaultTagFileName();
}

static void testEtagsInvocation()
{
    if (strncmp(getExecutableName(), ETAGS, strlen(ETAGS)) == 0)
    {
	Option.startedAsEtags	= TRUE;
	Option.etags		= TRUE;
	Option.sorted		= FALSE;
    }
}

extern int main( argc, argv )
    int __unused__ argc;
    char **argv;
{
    char **envArgList;
    char *const *fileList;

#ifdef __EMX__
    _wildcard(&argc, &argv);		/* expand wildcards in argument list */
#endif

    setExecutableName(*argv++);
    testEtagsInvocation();

    /*  Parse options.
     */
    envArgList = parseEnvironmentOptions();
    fileList = parseOptions(argv);
    setOptionDefaults();

    /*	Generate tags if there is at least one source file or a file list.
     */
    if (*fileList != NULL  ||  Option.fileList != NULL)
	makeTags(fileList);
    else
	error(FATAL, "No files specified. Try \"%s --help\".",
	      getExecutableName());

    /*  Post tag generation clean-up.
     */
    freeLineBuffer(&TagFile.line);
    if (envArgList != NULL)
	free(envArgList);
    exit(0);
    return 0;
}

/* vi:set tabstop=8 shiftwidth=4: */
