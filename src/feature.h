/* vi:set ts=8 sw=8:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 * feature.h: Defines for optional code and preferences
 *
 * Edit this file to include/exclude parts of Vim, before compiling.
 * The only other file that may be edited is Makefile, it contains machine
 * specific options.
 *
 * When you want to include a define, change the "#if 0" into "#if 1".
 */

/*
 * Optional code:
 * ==============
 */

/*
 * DIGRAPHS		When defined: Include digraph support.
 * 			In insert mode and on the command line you will be
 * 			able to use digraphs. The CTRL-K command will work.
 */
#if 1
# define DIGRAPHS
#endif

/*
 * HAVE_LANGMAP		When defined: Include support for 'langmap' option.
 * 			Only useful when you put your keyboard in a special
 * 			language mode, e.g. for typing greek.
 */
#if 0
# define HAVE_LANGMAP
#endif

/*
 * INSERT_EXPAND	When defined: Support for CTRL-N/CTRL-P/CTRL-X in
 *			insert mode. Takes about 4Kbyte of code.
 */
#if 1
# define INSERT_EXPAND
#endif

/*
 * RIGHTLEFT		When defined: Right-to-left typing and Hebrew support
 * 			Takes some code.
 */
#if 0
# define RIGHTLEFT
#endif

/*
 * EMACS_TAGS		When defined: Include support for emacs style
 *			TAGS file. Takes some code.
 */
#if 0
# define EMACS_TAGS
#endif

/*
 * AUTOCMD		When defined: Include support for ":autocmd"
 */
#if 1
# define AUTOCMD
#endif

/*
 * VIMINFO		When defined: Include support for reading/writing
 *			the viminfo file. Takes about 8Kbyte of code.
 */
#if 1
# define VIMINFO
#endif

/*
 * Choose one out of the following four:
 *
 * NO_BUILTIN_TCAPS	When defined: Do not include any builtin termcap
 *			entries (used only with HAVE_TGETENT defined).
 *
 * (nothing)		Machine specific termcap entries will be included.
 *
 * SOME_BUILTIN_TCAPS	When defined: Include most useful builtin termcap
 *			entries (used only with NO_BUILTIN_TCAPS not defined).
 *			This is the default.
 *
 * ALL_BUILTIN_TCAPS	When defined: Include all builtin termcap entries
 * 			(used only with NO_BUILTIN_TCAPS not defined).
 */
#ifdef HAVE_TGETENT
# if 0
#  define NO_BUILTIN_TCAPS
# endif
#endif

#ifndef NO_BUILTIN_TCAPS
# if 0
#  define ALL_BUILTIN_TCAPS
# else
#  if 1
#   define SOME_BUILTIN_TCAPS		/* default */
#  endif
# endif
#endif

/*
 * LISPINDENT		When defined: Include lisp indenting (From Eric
 *			Fischer). Doesn't completely work like vi (yet).
 * CINDENT		When defined: Include C code indenting (From Eric
 *			Fischer).
 * SMARTINDENT		When defined: Do smart C code indenting when the 'si'
 *			option is set. It's not as good as CINDENT, only
 *			included to keep the old code.
 *
 * These two need to be defined when making prototypes.
 */
#if 1 || defined(PROTO)
# define LISPINDENT
#endif

#if 1 || defined(PROTO)
# define CINDENT
#endif

#if 1
# define SMARTINDENT
#endif

/*
 * Preferences:
 * ============
 */

/*
 * COMPATIBLE		When defined: Start in vi-compatible mode.
 *			Sets all option defaults to their vi-compatible value.
 */
#if 0
# define COMPATIBLE
#endif

/*
 * WRITEBACKUP		When defined: 'writebackup' is default on: Use
 *			a backup file while overwriting a file.
 */
#if 1
# define WRITEBACKUP
#endif

/*
 * SAVE_XTERM_SCREEN	When defined: The t_ti and t_te entries for the
 *			builtin xterm will be set to save the screen when
 *			starting Vim and restoring it when exiting.
 */
#if 0
# define SAVE_XTERM_SCREEN
#endif

/*
 * DEBUG		When defined: Output a lot of debugging garbage.
 */
#if 0
# define DEBUG
#endif

/*
 * VIMRC_FILE		Name of the .vimrc file in current dir.
 */
#if 0
# define VIMRC_FILE  	".vimrc"
#endif

/*
 * EXRC_FILE		Name of the .exrc file in current dir.
 */
#if 0
# define EXRC_FILE	".exrc"
#endif

/*
 * GVIMRC_FILE		Name of the .gvimrc file in current dir.
 */
#if 0
# define GVIMRC_FILE	".gvimrc"
#endif

/*
 * USR_VIMRC_FILE	Name of the user .vimrc file.
 */
#if 0
# define USR_VIMRC_FILE		"~/somewhere/.vimrc"
#endif

/*
 * USR_EXRC_FILE	Name of the user .exrc file.
 */
#if 0
# define USR_EXRC_FILE		"~/somewhere/.exrc"
#endif

/*
 * USR_GVIMRC_FILE	Name of the user .gvimrc file.
 */
#if 0
# define USR_GVIMRC_FILE	"~/somewhere/.gvimrc"
#endif

/*
 * SYS_VIMRC_FILE	Name of the system-wide .vimrc file.
 */
#if 0
# define SYS_VIMRC_FILE		"/somewhere/.vimrc"
#endif

/*
 * SYS_GVIMRC_FILE	Name of the system-wide .gvimrc file.
 */
#if 0
# define SYS_GVIMRC_FILE	"/somewhere/.gvimrc"
#endif
  
/*
 * VIM_HLP		Name of the help file.
 */
#if 0
# define VIM_HLP	"/somewhere/vim_help.txt"
#endif


/*
 * Machine dependent:
 * ==================
 */

/*
 * USE_SYSTEM		Unix only. When defined: Use system() instead of
 *			fork/exec for starting a shell.
 */
#if 0
# define USE_SYSTEM
#endif

/*
 * WANT_X11		Unix only. When defined: Include code for xterm title
 *			saving. Only works if HAVE_X11 is also defined.
 */
#if 1
# define WANT_X11
#endif

/*
 * WANT_GUI		Would be nice, but that doesn't work. To compile Vim
 *			with the GUI (gvim) you have to edit Makefile.
 */

/*
 * NO_ARP		Amiga only. When defined: Do not use arp.library, DOS
 *			2.0 required. 
 */
#if 0
# define NO_ARP
#endif
