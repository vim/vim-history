/*****************************************************************************
*   $Id: main.c,v 6.23 1998/08/18 04:26:50 darren Exp $
*
*   Copyright (c) 1996-1998, Darren Hiebert
*
*   Author: Darren Hiebert <darren@hiebert.com>, <darren@hiwaay.net>
*           http://darren.hiebert.com
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

#if defined(__STDC__) || defined(MSDOS) || defined(WIN32) || defined(OS2)
# define ENABLE_STDARG
#endif

#if defined(MSDOS) || defined(WIN32)
# define HAVE_DOS_H
# define HAVE_IO_H
# define HAVE_TIME_H
# define HAVE_CLOCK
# define HAVE_CHSIZE
# define HAVE_STRERROR
# define HAVE_FINDNEXT
# ifdef __BORLANDC__
#  define HAVE_DIR_H
#  define HAVE_DIRENT_H
#  define HAVE_FINDFIRST
# else
#  ifdef _MSC_VER
#   define HAVE__FINDFIRST
#  endif
# endif
#endif

#ifdef DJGPP
# define HAVE_DIR_H
# define HAVE_UNISTD_H
# define HAVE_FINDFIRST
# define HAVE_TRUNCATE
#endif

#if defined(OS2)
# define HAVE_DIRENT_H
# define HAVE_TIME_H
# define HAVE_IO_H
# define HAVE_CLOCK
# define HAVE_CHSIZE
# define HAVE_OPENDIR
# define HAVE_STRERROR
#endif

#ifdef AMIGA
# define HAVE_TIME_H
# define HAVE_SYS_STAT_H
# define HAVE_CLOCK
# define HAVE_STRERROR
# define PATH_MAX 255
# include <dos/dosasl.h>	/* for struct AnchorPath */
# include <clib/dos_protos.h>	/* function prototypes */
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (sizeof(struct AnchorPath) + ANCHOR_BUF_SIZE)
#endif

#if defined(HAVE_UNISTD_H)
# include <unistd.h>	/* to declare close(), ftruncate(), truncate() */
#endif

#include <errno.h>

#ifdef ENABLE_STDARG
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

#ifdef HAVE_DIRENT_H
# ifdef __BORLANDC__
#  define boolean BORLAND_boolean
# endif
# include <dirent.h>
# undef boolean
#endif

/*  These header files provide for the functions necessary to do file
 *  truncation.
 */
#include <fcntl.h>
#ifdef HAVE_IO_H
# include <io.h>
#endif
#ifdef HAVE_DOS_H
# include <dos.h>
#endif
#ifdef HAVE_DIR_H
# include <dir.h>
#endif

/*  To define the maximum path length
 */
#include <limits.h>			/* to define PATH_MAX (hopefully) */
#ifndef PATH_MAX
# ifdef MAXNAMLEN
#  define PATH_MAX MAXNAMLEN
# endif
#endif
#if !defined(PATH_MAX) && defined(MAXPATH)	/* found in Borland C <dir.h> */
# define PATH_MAX MAXPATH
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

#if defined(MSDOS) || defined(WIN32) || defined(OS2)
# define PATH_SEPARATOR	'\\'
#else
# define PATH_SEPARATOR	'/'
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

#ifndef S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(mode)	    (((mode) & S_IFMT) == S_IFDIR)
# else
#  define S_ISDIR(mode)	    FALSE	/* assume no soft links */
# endif
#endif

/*  Hack for rediculous practice of Microsoft Visual C++.
 */
#if defined(WIN32) && defined(_MSC_VER)
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

#define selected(var,feature)	(((int)(var) & (int)(feature)) == (int)feature)

#define plural(value)		(((unsigned long)(value) == 1L) ? "" : "s")

/*============================================================================
=   Data definitions
============================================================================*/
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
static const char DosPathDelimiters[] = ":/\\";
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

memberInfo NoClass = { MEMBER_NONE, VIS_UNDEFINED, FALSE, "" };

/*============================================================================
=   Function prototypes
============================================================================*/
#ifdef NEED_PROTO_STAT
extern int stat __ARGS((const char *, struct stat *));
#endif

#ifdef NEED_PROTO_TRUNCATE
extern int truncate __ARGS((const char *path, off_t length));
#endif

#ifdef NEED_PROTO_FTRUNCATE
extern int ftruncate __ARGS((int fd, off_t length));
#endif

static const char *baseFilename __ARGS((const char *const path));
static boolean isAbsolutePath __ARGS((const char *const path));
static void combinePathAndFile __ARGS((char *const filePath, const char *const path, const char *const file));

static const char *findExtension __ARGS((const char *const fileName));
static boolean isFileHeader __ARGS((const char *const FileName));
static boolean isExtensionInList __ARGS((const char *const extension, const char *const *const list));
static langType getExtensionLanguage __ARGS((const char *const extension));
static langType getFileLanguage __ARGS((const char *const FileName));

static boolean isValidTagAddress __ARGS((const char *const tag));
static boolean isCtagsLine __ARGS((const char *const tag));
static boolean isEtagsLine __ARGS((const char *const tag));
static boolean isTagFile __ARGS((const char *const filename));
static void openTagFile __ARGS((const boolean toStdout));
static void closeTagFile __ARGS((const boolean resize));

static void beginEtagsFile __ARGS((void));
static void endEtagsFile __ARGS((const char *const name));
static boolean createTagsForFile __ARGS((const char *const filePath, const langType language));
static const char *getNextListFile __ARGS((FILE *const fp));
static const char *sourceFilePath __ARGS((const char *const file));
static boolean createTagsWithFallback __ARGS((const char *const fileName, const langType language));
static boolean createTagsForDirectory __ARGS((const char *const dirName));
static boolean createTagsForEntry __ARGS((const char *const entryName));
static boolean createTagsForList __ARGS((const char *const listFile));
static boolean createTagsForArgs __ARGS((const char *const *const argList));
static void printTotals __ARGS((const clock_t *const timeStamps));
static void makeTags __ARGS((const char *const *const argList));

static void setExecutableName __ARGS((const char *const path));
static void setDefaultTagFileName __ARGS((void));
static void setOptionDefaults __ARGS((void));
static void testEtagsInvocation __ARGS((void));

extern int main __ARGS((int argc, char **argv));

/*============================================================================
=   Function definitions
============================================================================*/

#ifdef ENABLE_STDARG
extern void error( const errorSelection selection,
		   const char *const format, ... )
#else
extern void error( va_alist )
    va_dcl
#endif
{
    va_list ap;

#ifdef ENABLE_STDARG
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
    struct stat fileStatus;
    boolean isNormal = FALSE;

    if (stat(name, &fileStatus) == 0)
	isNormal = (boolean)(S_ISLNK(fileStatus.st_mode) ||
			     S_ISREG(fileStatus.st_mode)  );

    return isNormal;
}

extern boolean isDirectory( name )
    const char *const name;
{
    boolean isDir = FALSE;
#ifdef AMIGA
    struct FileInfoBlock *const fib = (struct FileInfoBlock *)
					malloc(sizeof(struct FileInfoBlock));

    if (fib != NULL)
    {
	const BPTR flock = Lock((UBYTE *)name, (long)ACCESS_READ);

	if (flock != (BPTR)NULL)
	{
	    if (Examine(flock, fib))
		isDir = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
	    UnLock(flock);
	}
	free(fib);
    }
#else
    struct stat fileStatus;

    if (stat(name, &fileStatus) == 0)
	isDir = (boolean)S_ISDIR(fileStatus.st_mode);
#endif
    return isDir;
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
 *  Pathname manipulation (O/S dependent!!!)
 *--------------------------------------------------------------------------*/

static const char *baseFilename( filePath )
    const char *const filePath;
{
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    const char *tail = NULL;
    unsigned int i;

    /*  Find whichever of the path delimiters is last.
     */
    for (i = 0  ;  i < strlen(DosPathDelimiters)  ;  ++i)
    {
	const char *sep = strrchr(filePath, DosPathDelimiters[i]);

	if (sep > tail)
	    tail = sep;
    }
#else
    const char *tail = strrchr(filePath, PATH_SEPARATOR);
#endif
    if (tail == NULL)
	tail = filePath;
    else
	++tail;			/* step past last delimiter */

    return tail;
}

static boolean isAbsolutePath( path )
    const char *const path;
{
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    return (strchr(DosPathDelimiters, path[0]) != NULL);
#else
    return (boolean)(path[0] == PATH_SEPARATOR);
#endif
}

static void combinePathAndFile( filePath, path, file )
    char *const filePath;
    const char *const path;
    const char *const file;
{
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
    if (strchr(DosPathDelimiters, path[strlen(path)-1]) != NULL)
	sprintf(filePath, "%s%s", path, file);
    else
	sprintf(filePath, "%s%c%s", path, PATH_SEPARATOR, file);
#else
    if ((path[strlen(path) - 1]) == PATH_SEPARATOR)
	sprintf(filePath, "%s%s", path, file);
    else
	sprintf(filePath, "%s%c%s", path, PATH_SEPARATOR, file);
#endif
}

/*----------------------------------------------------------------------------
 *  File extension and language handling
 *--------------------------------------------------------------------------*/

static const char *findExtension( fileName )
    const char *const fileName;
{
    const char *extension;
    const char *const start = strrchr(fileName, '.');	/* find last '.' */

    if (start == NULL)
	extension = "";
    else
	extension = start + 1;		/* skip to character after '.' */

    return extension;
}

/*  Determines whether the specified file name is considered to be a header
 *  file for the purposes of determining whether enclosed tags are global or
 *  static.
 */
static boolean isFileHeader( fileName )
    const char *const fileName;
{
    const char *const extension = findExtension(fileName);
    boolean header = FALSE;		    /* default unless match found */
    int i;

    for (i = 0 ; Option.headerExt[i] != NULL ; ++i)
    {
	if (strcmp(Option.headerExt[i], extension) == 0)
	{
	    header = TRUE;		    /* found in list */
	    break;
	}
    }
    return header;
}

static boolean isExtensionInList( extension, list )
    const char *const extension;
    const char *const *const list;
{
    boolean isKnown = FALSE;

    if (list != NULL)
    {
	const char *const *pExtension = list;

	while (! isKnown  &&  *pExtension != NULL)
	{
#if defined(MSDOS) || defined(WIN32) || defined(OS2)
	    if (strequiv(extension, *pExtension))
#else
	    if (strcmp(extension, *pExtension) == 0)
#endif
		isKnown = TRUE;
	    ++pExtension;
	}
    }
    return isKnown;
}

static langType getExtensionLanguage( extension )
    const char *const extension;
{
    unsigned int i;
    langType language = LANG_IGNORE;

    for (i = 0  ;  i < (int)LANG_COUNT  ;  ++i)
    {
	if (isExtensionInList(extension, Option.langMap[i]))
	{
	    language = (langType)i;
	    break;
	}
    }
    return language;
}

static langType getFileLanguage( fileName )
    const char *const fileName;
{
    const char *const extension = findExtension(fileName);
    langType language;

    if (Option.language != LANG_AUTO)
	language = Option.language;
    else if (isFileHeader(fileName))
	language = LANG_CPP;
    else if (extension[0] == '\0')
	language = LANG_IGNORE;		/* ignore files with no extension */
    else
	language = getExtensionLanguage(extension);

    return language;
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
	char *address = (char *)malloc(strlen(excmd) + 1);

	if (address != NULL)
	{
	    if (sscanf(excmd, "%[^;\n]", address) == 1  &&
		strspn(address,"0123456789") == strlen(address))
		    isValid = TRUE;
	    free(address);
	}
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
#define field(x)	(fields + ((size_t)(x) * fieldLength))

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

static int copyChars __ARGS((FILE *const fpFrom, FILE *const fpTo, const off_t size));
static int copyFile __ARGS((const char *const from, const char *const to, const off_t size));
static int replacementTruncate __ARGS((const char *const name, const off_t size));

static int copyChars( fpFrom, fpTo, size )
    FILE *const fpFrom;
    FILE *const fpTo;
    const off_t size;
{
    off_t count;
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
    const off_t size;
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
    const off_t size;
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
	result = replacementTruncate(TagFile.name, (off_t)size);
#else
# ifdef HAVE_TRUNCATE
	result = truncate(TagFile.name, (off_t)size);
# else
	const int fd = open(TagFile.name, O_RDWR);

	if (fd != -1)
	{
#  ifdef HAVE_FTRUNCATE
	    result = ftruncate(fd, (off_t)size);
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
    TagFile.etags.fp = fopen(TagFile.etags.name, "w+b");
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

static boolean createTagsForFile( filePath, language )
    const char *const filePath;
    const langType language;
{
    boolean ok;

    if (filePath == NULL)
	ok = FALSE;
    else
    {
	const boolean isHeader = isFileHeader(filePath);

	ok = TRUE;
	if (Option.etags)
	    beginEtagsFile();
	if (cppOpen(filePath, language, isHeader))
	{
	    tagInfo tag;

	    DebugStatement( clearString(tag.name, MaxNameLength); )
	    strcpy(tag.name, baseFilename(filePath));
	    tag.location   = 0;
	    tag.lineNumber = 1;

	    makeTag(&tag, &NoClass, SCOPE_GLOBAL, TAG_SOURCE_FILE);
	    ok = createTags(0, NULL);
	    cppClose();
	}
	if (Option.etags)
	    endEtagsFile(filePath);
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

static const char *sourceFilePath( file )
    const char *const file;
{
    const char *result = NULL;

    if (Option.path == NULL  ||  isAbsolutePath(file))
	result = file;
    else if (strlen(Option.path) + strlen(file) < (size_t)PATH_MAX)
    {
	static char filePath[PATH_MAX + 1];

	combinePathAndFile(filePath, Option.path, file);
	result = filePath;
    }
    return result;
}

static boolean createTagsWithFallback( fileName, language )
    const char *const fileName;
    const langType language;
{
    const char *const filePath	= sourceFilePath(fileName);
    const unsigned long numTags	= TagFile.numTags.added;
    const long tagFilePosition	= ftell(TagFile.fp);
    boolean resize = FALSE;

    if (! createTagsForFile(filePath, language))
    {
	/*  Restore prior state of tag file.
	 */
	fseek(TagFile.fp, tagFilePosition, SEEK_SET);
	TagFile.numTags.added = numTags;
	DebugStatement( debugPrintf(DEBUG_STATUS,
				"%s: formatting error; retrying\n", fileName); )

	Option.braceFormat = TRUE;
	    createTagsForFile(filePath, language);
	Option.braceFormat = FALSE;
	resize = TRUE;
    }
    return resize;
}

# if defined(MSDOS) || defined(WIN32)

static boolean createTagsForMatchingEntries __ARGS((char *const pattern));

static boolean createTagsForMatchingEntries( pattern )
    char *const pattern;
{
    boolean resize = FALSE;
    const size_t tailIndex = baseFilename(pattern) - (const char *)pattern;
    char *const tail = pattern + tailIndex;
#ifdef HAVE_FINDFIRST
    struct ffblk fileInfo;
    int result = findfirst(pattern, &fileInfo, FA_DIREC);

    while (result == 0)
    {
	const char *const entryName = fileInfo.ff_name;

	/*  We must not recurse into the directories "." or "..".
	 */
	if (strcmp(entryName, ".") != 0  &&  strcmp(entryName, "..") != 0)
	{
	    strcpy(tail, entryName);
	    resize |= createTagsForEntry(pattern);
	}
	result = findnext(&fileInfo);
    }
#else
# ifdef HAVE__FINDFIRST
    struct _finddata_t fileInfo;
    long hFile = _findfirst(pattern, &fileInfo);

    if (hFile != -1L)
    {
	do
	{
	    const char *const entryName = fileInfo.name;

	    /*  We must not recurse into the directories "." or "..".
	     */
	    if (strcmp(entryName, ".") != 0  &&  strcmp(entryName, "..") != 0)
	    {
		strcpy(tail, entryName);
		resize |= createTagsForEntry(pattern);
	    }
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
    }
# endif		/* HAVE__FINDFIRST */
#endif		/* HAVE_FINDFIRST */
    return resize;
}

#else
# ifdef AMIGA

static boolean createTagsForMatchingEntries __ARGS((char *const pattern));

static boolean createTagsForMatchingEntries( pattern )
    char *const pattern;
{
    boolean resize = FALSE;
    struct AnchorPath *const anchor = (struct AnchorPath *)
					calloc((size_t)1, (size_t)ANCHOR_SIZE);

    if (anchor != NULL)
    {
	LONG result;

	anchor->ap_Strlen = ANCHOR_BUF_SIZE; /* ap_Length no longer supported */

	/*  Allow '.' for current directory.
	 */
#ifdef APF_DODOT
	anchor->ap_Flags = APF_DODOT | APF_DOWILD;
#else
	anchor->ap_Flags = APF_DoDot | APF_DoWild;
#endif

	result = MatchFirst((UBYTE *)pattern, anchor);
	while (result == 0)
	{
	    resize |= createTagsForEntry((char *)anchor->ap_Buf);
	    result = MatchNext(anchor);
	}
	MatchEnd(anchor);
	free(anchor);
    }
    return resize;
}

# endif	/* AMIGA */
#endif	/* MSDOS || WIN32 */

static boolean createTagsForDirectory( dirName )
    const char *const dirName;
{
#ifdef HAVE_OPENDIR
    boolean resize = FALSE;
    DIR *const dir = opendir(dirName);

    if (dir == NULL)
	error(WARNING, "");
    else
    {
	struct dirent *entry;

	DebugStatement( debugPrintf(DEBUG_STATUS, "RECURSING into %s%c\n",
				    dirName, PATH_SEPARATOR); )
	while ((entry = readdir(dir)) != NULL)
	{
	    const char *const entryName = entry->d_name;

	    if (strcmp(entryName, ".")    != 0  &&
		strcmp(entryName, "..")   != 0  &&
		strcmp(entryName, "SCCS") != 0   )
	    {
		char filePath[PATH_MAX + 1];

		combinePathAndFile(filePath, dirName, entryName);
		resize |= createTagsForEntry(filePath);
	    }
	}
	closedir(dir);
    }
    return resize;
#else
# if defined(MSDOS) || defined(WIN32)
    char pattern[PATH_MAX + 1];
    boolean resize;

    DebugStatement( debugPrintf(DEBUG_STATUS, "RECURSING into %s%c\n",
				dirName, PATH_SEPARATOR); )
    strcpy(pattern, dirName);
    strcat(pattern, "\\*.*");
    resize = createTagsForMatchingEntries(pattern);

    return resize;
# else
#  ifdef AMIGA
    char pattern[PATH_MAX + 1];
    boolean resize = FALSE;

    DebugStatement( debugPrintf(DEBUG_STATUS, "RECURSING into %s%c\n",
				dirName, PATH_SEPARATOR); )
    strcpy(pattern, dirName);
    if (pattern[0] != '\0')
	strcat(pattern, "/");     /* "/#?" searches one directory upwards */
    strcat(pattern, "#?");
    resize = createTagsForMatchingEntries(pattern);

    return resize;
#  else /* !AMIGA */
    return FALSE;
#  endif
# endif
#endif	/* HAVE_OPENDIR */
}

static boolean createTagsForEntry( entryName )
    const char *const entryName;
{
    boolean resize = FALSE;
    langType language;

    if (! doesFileExist(entryName))
	error(WARNING | PERROR, "cannot open \"%s\"", entryName);
    else if (isDirectory(entryName))
    {
	if (Option.recurse)
	    resize = createTagsForDirectory(entryName);
	else
	{
	    DebugStatement( debugPrintf(DEBUG_STATUS,"  no recurse into %s%c\n",
					entryName, PATH_SEPARATOR) );
	    resize = FALSE;
	}
    }
    else if (! isNormalFile(entryName))
    {
	DebugStatement( if (debug(DEBUG_STATUS))
			printf("  ignoring %s (special file)\n", entryName) );
    }
    else if ((language = getFileLanguage(entryName)) == LANG_IGNORE)
	DebugStatement( debugOpen(entryName, FALSE, language) );
    else
    {
	resize = createTagsWithFallback(entryName, language);
	addTotals(1, 0L, 0L);
    }
    return resize;
}

static boolean createTagsForArgs( argList )
    const char *const *const argList;
{
    boolean resize = FALSE;
    const char *const *pArg;

    /*  Generate tags for each argument on the command line.
     */
    for (pArg = argList  ;  *pArg != NULL  ;  ++pArg)
    {
	const char *arg = *pArg;

#if defined(MSDOS) || defined(WIN32)
	char pattern[PATH_MAX + 1];

	strcpy(pattern, arg);

	/*  We must transform the "." and ".." forms into something that can
	 *  be expanded by the MSDOS/Windows functions.
	 */
	if (Option.recurse  &&
	    (strcmp(pattern, ".") == 0  ||  strcmp(pattern, "..") == 0))
	{
	    strcat(pattern, "\\*.*");
	}
	resize |= createTagsForMatchingEntries(pattern);
#else
	resize |= createTagsForEntry(arg);
#endif
    }

    return resize;
}

/*  Create tags for the source files listed in the file specified by
 *  "listFile".
 */
static boolean createTagsForList( listFile )
    const char *const listFile;
{
    boolean resize = FALSE;
    FILE *const fp = (strcmp(listFile,"-") == 0) ? stdin : fopen(listFile, "r");

    if (fp == NULL)
	error(FATAL | PERROR, "cannot open \"%s\"", listFile);
    else
    {
	const char *fileName = getNextListFile(fp);

	while (fileName != NULL  &&  fileName[0] != '\0')
	{
	    resize |= createTagsForEntry(fileName);
	    fileName = getNextListFile(fp);
	}
	if (fp != stdin)
	    fclose(fp);
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
static clock_t clock __ARGS((void));
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
#ifdef CLOCK_AVAILABLE
	fprintf(errout, " in %.02f seconds",
		((double)(timeStamps[3] - timeStamps[2])) / CLOCKS_PER_SEC);
#endif
	fputc('\n', errout);
    }

#ifdef DEBUG
    fprintf(errout, "longest tag line = %lu\n",
	    (unsigned long)TagFile.max.line);
#endif
}

static void makeTags( argList )
    const char *const *const argList;
{
    boolean toStdout = FALSE;
    boolean resize = FALSE;
    clock_t timeStamps[4];

    if (Option.xref  ||  strcmp(Option.tagFileName, "-") == 0
#if !defined(MSDOS) && !defined(WIN32) && !defined(OS2)
	|| strcmp(Option.tagFileName, "/dev/stdout") == 0
#endif
	)
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
	    if (Option.printTotals) timeStamps[2] = clock();
#ifdef EXTERNAL_SORT
	    externalSortTags(toStdout);
#else
	    internalSortTags(toStdout);
#endif
	    if (Option.printTotals) timeStamps[3] = clock();
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
    const char *const *fileList;

#ifdef __EMX__
    _wildcard(&argc, &argv);		/* expand wildcards in argument list */
#endif

    setExecutableName(*argv++);
    testEtagsInvocation();

    /*  Parse options.
     */
    envArgList = parseEnvironmentOptions();
    fileList = (const char *const *)parseOptions(argv);
    setOptionDefaults();
    buildKeywordHash();

    /*	Generate tags if there is at least one source file or a file list.
     */
    if (*fileList == NULL  &&  Option.fileList == NULL)
    {
	if (! Option.recurse)
	    error(FATAL, "No files specified. Try \"%s --help\".",
		getExecutableName());
	else
	{
	    static const char *defaultRecursionDir[] = { ".", NULL };
	    fileList = defaultRecursionDir;
	}
    }
    makeTags(fileList);

    /*  Post tag generation clean-up.
     */
    freeLineBuffer(&TagFile.line);
    if (envArgList != NULL)
	free(envArgList);
    exit(0);
    return 0;
}

/* vi:set tabstop=8 shiftwidth=4: */
