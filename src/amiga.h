/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * Amiga Machine-dependent things
 */

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ifndef SYSVIMRC_FILE
# define SYSVIMRC_FILE	"s:.vimrc"
#endif

#ifndef SYSEXRC_FILE
# define SYSEXRC_FILE	"s:.exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE		".vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE		".exrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP		"vim:vim.hlp"
#endif

#ifndef DEF_DIR
# define DEF_DIR		"t:"
#endif

#define TMPNAME1		"t:viXXXXXX"
#define TMPNAME2		"t:voXXXXXX"
#define TMPNAMELEN		12

#ifndef MAXMEM
# define MAXMEM			256		/* use up to 256Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT		0		/* decide in set_init */
#endif

#define BASENAMELEN		26		/* Amiga */
