/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define CASE_INSENSITIVE_FILENAME
#define WANT_MODIFY_FNAME
#define WANT_OSFILETYPE
#define OFT_DFLT "Text"
#define USE_TERM_CONSOLE
#define HAVE_AVAIL_MEM

#ifndef MIN_FEAT
# define VIM_BACKTICK		/* internal backtick expansion */
#endif

/* 10, but 4 char margin for extension. */
#ifndef BASENAMELEN
# define BASENAMELEN 6
#endif

#ifndef TEMNAME
# define TEMPNAME	"<Wimp$ScrapDir>.v?XXXXXX"
# define TEMPNAMELEN	25
#endif

#ifndef VIM_HLP
# define VIM_HLP "Vim:doc.help"
#endif

#ifndef DEF_BDIR
# define DEF_BDIR	".,<Wimp$ScrapDir>."	/* default for 'backupdir' */
#endif

/* Paths to try putting swap file in. */
#ifndef DEF_DIR
# define DEF_DIR	"<Wimp$ScrapDir>.,."	/* default for 'directory' */
#endif

#ifndef TERMCAPFILE
# define TERMCAPFILE	"Vim:TermCap"
#endif
#define HAVE_TGETENT

#ifndef SYNTAX_FNAME
# define SYNTAX_FNAME	"Vim:Syntax.%s"
#endif

#define VIMINFO

#ifndef VIMINFO_FILE
# define VIMINFO_FILE	"<Choices$Write>.VimInfo"
#endif
#ifndef VIMINFO_FILE2
# define VIMINFO_FILE2	"Choices:VimInfo"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE	"/vimrc"
#endif
#ifndef EXRC_FILE
# define EXRC_FILE	"/exrc"
#endif
#ifndef GVIMRC_FILE
# define GVIMRC_FILE	"/gvimrc"
#endif
#ifndef SESSION_FILE
# define SESSION_FILE	"/Session.vim"
#endif
#ifndef USR_VIMRC_FILE
# define USR_VIMRC_FILE	"Choices:UserVimRC"
#endif
#ifndef USR_GVIMRC_FILE
# define USR_GVIMRC_FILE    "Choices:GVimRC"
#endif
#ifndef USR_EXRC_FILE
# define USR_EXRC_FILE    "Choices:ExRC"
#endif
#ifndef SYS_VIMRC_FILE
# define SYS_VIMRC_FILE	    "Vim:VimRC"
#endif
#ifndef SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE    "Vim:GVimRC"
#endif
#ifndef SYS_MENU_FILE
# define SYS_MENU_FILE	    "Vim:Menu"
#endif
#ifndef SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE    "Vim:Optwin"
#endif
#ifndef FILETYPE_FILE
# define FILETYPE_FILE	    "Vim:Filetype"
#endif
#ifndef FTOFF_FILE
# define FTOFF_FILE	    "Vim:Ftoff"
#endif

#define ERRORFILE	"errors/vim"
#define MAKEEF		"<Wimp$ScrapDir>.vim##/err"

/*
 * RISC PCs have plenty of memory, use large buffers
 */
#define CMDBUFFSIZE 1024	/* size of the command processing buffer */
#define MAXPATHL    256		/* paths are always quite short though */

#ifndef MAXMEM
# define MAXMEM		(5*1024)    /* use up to 5 Mbyte for a buffer */
#endif

#ifndef MAXMEMTOT
# define MAXMEMTOT	(10*1024)    /* use up to 10 Mbyte for Vim */
#endif

#ifdef HAVE_SIGSET
# define signal sigset
#endif

#define SIZEOF_INT 4

#define n_flag (1<<31)
#define z_flag (1<<30)
#define c_flag (1<<29)
#define v_flag (1<<28)

/* These take r0-r7 as inputs, returns r0-r7 in global variables. */
void swi(int swinum, ...);      /* Handles errors itself */
int xswi(int swinum, ...);      /* Returns errors using v flag */
extern int r0, r1, r2, r3, r4, r5, r6, r7;  /* For return values */

#include <sys/swis.h>

#define mch_memmove(to, from, len) memmove((char *)(to), (char *)(from), len)
#define mch_rename(src, dst) rename(src, dst)
#define mch_getenv(x) (char_u *)getenv((char *)x)
#define mch_setenv(name, val, x) setenv(name, val, x)
