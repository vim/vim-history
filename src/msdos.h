/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * MSDOS Machine-dependent things.
 */

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ifndef SYSVIMRC_FILE
# define SYSVIMRC_FILE	"$VIM\\_vimrc"
#endif

#ifndef SYSEXRC_FILE
# define SYSEXRC_FILE	"$VIM\\_exrc"
#endif

#ifndef VIMRC_FILE
# define VIMRC_FILE		"_vimrc"
#endif

#ifndef EXRC_FILE
# define EXRC_FILE		"_exrc"
#endif

#ifndef VIM_HLP
# define VIM_HLP		"$VIM\\vim.hlp"
#endif

#ifndef DEF_DIR
# define DEF_DIR		"c:\\tmp"
#endif

#define TMPNAME1		"viXXXXXX"		/* put it in current dir */
#define TMPNAME2		"voXXXXXX"		/*  is there a better place? */
#define TMPNAMELEN		10

#ifndef MAXMEM
# define MAXMEM			256				/* use up to 256Kbyte for buffer */
#endif
#ifndef MAXMEMTOT
# define MAXMEMTOT		0				/* decide in set_init */
#endif

#define BASENAMELEN		8				/* length of base of file name */

/*
 * MSDOS Machine-dependent routines.
 */

#ifdef remove
# undef remove                   /* MSDOS remove()s when not readonly */
#endif
#define remove vim_remove

/* use chdir() that also changes the default drive */
#define chdir vim_chdir
