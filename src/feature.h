/* vi:set ts=8 sts=0 sw=8:
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
 * To include specific options, change the "#if*" and "#endif" into comments,
 * or uncomment the "#define".
 * To exclude specific options, change the "#define" into a comment.
 */

/*
 * Basic choices:
 * ==============
 *
 * MIN_FEAT		minimal features enabled, as basic as possible (DOS16)
 * MAX_FEAT		maximal features enabled, as rich as possible.
 * default		A selection of features enabled.
 *
 * These executables are made available with MAX_FEAT defined, because they
 * are supposed to have enough RAM: Win32 (console & GUI), dos32 and OS/2.
 * The dos16 version has very little RAM available, use MIN_FEAT.
 */
#if !defined(MIN_FEAT) && !defined(MAX_FEAT)
/* #define MIN_FEAT */
/* #define MAX_FEAT */
# if defined(WIN32) || defined(DJGPP) || defined(OS2)
#  define MAX_FEAT
# else
#  if defined(MSDOS)
#   define MIN_FEAT
#  endif
# endif
#endif

/*
 * Optional code (see ":help +feature-list")
 * =============
 */

/*
 * +digraphs		When DIGRAPHS defined: Include digraph support.
 *			In insert mode and on the command line you will be
 *			able to use digraphs. The CTRL-K command will work.
 */
#ifndef MIN_FEAT
# define DIGRAPHS
#endif

/*
 * +langmap		When HAVE_LANGMAP defined: Include support for
 *			'langmap' option.  Only useful when you put your
 *			keyboard in a special language mode, e.g. for typing
 *			greek.
 */
#ifdef MAX_FEAT
# define HAVE_LANGMAP
#endif

/*
 * +insert_expand 	When INSERT_EXPAND defined: Support for
 *			CTRL-N/CTRL-P/CTRL-X in insert mode. Takes about
 *			4Kbyte of code.
 */
#ifndef MIN_FEAT
# define INSERT_EXPAND
#endif

/*
 * +textobjects		When TEXT_OBJECTS defined: Support for text objects:
 *			"vaw", "das", etc.
 */
#ifndef MIN_FEAT
# define TEXT_OBJECTS
#endif

/*
 * +showcmd		When SHOWCMD defined: Support for 'showcmd' option.
 */
#ifndef MIN_FEAT
# define SHOWCMD
#endif

/*
 * +ex_extra		When EX_EXTRA defined: Support for ":retab", ":right",
 *			":left", ":center", ":normal".
 */
#ifndef MIN_FEAT
# define EX_EXTRA
#endif

/*
 * +extra_search	When EXTRA_SEARCH defined: Support for 'hlsearch' and
 *			'incsearch'.
 */
#ifndef MIN_FEAT
# define EXTRA_SEARCH
#endif

/*
 * +quickfix		When QUICKFIX defined: Support for quickfix commands.
 */
#ifndef MIN_FEAT
# define QUICKFIX
#endif

/*
 * +file_in_path	When FILE_IN_PATH defined: Support for "gf" and
 *			"<cfile>".
 */
#ifndef MIN_FEAT
# define FILE_IN_PATH
#endif

/*
 * +find_in_path	When FIND_IN_PATH defined: Support for "[I" ":isearch"
 *			"^W^I", ":checkpath", etc.
 */
#ifndef MIN_FEAT
# ifdef FILE_IN_PATH	/* FILE_IN_PATH is required */
#  define FIND_IN_PATH
# endif
#endif

/*
 * +rightleft		When RIGHTLEFT defined: Right-to-left typing and
 *			Hebrew support.  Takes some code.
 */
#ifdef MAX_FEAT
# define RIGHTLEFT
#endif

/*
 * +farsi		When FKMAP defined: Farsi (Persian language) Keymap
 *			support Takes some code.  Needs RIGHTLEFT.
 */
#ifdef MAX_FEAT
# ifndef RIGHTLEFT
#  define RIGHTLEFT
# endif
# define FKMAP
#endif

/*
 * +emacs_tags		When EMACS_TAGS defined: Include support for emacs
 *			style TAGS file. Takes some code.
 */
#ifdef MAX_FEAT
# define EMACS_TAGS
#endif

/*
 * +tag_binary		When BINARY_TAGS defined: Use a binary search instead
 *			of a linear search when search a tags file.
 */
#ifndef MIN_FEAT
# define BINARY_TAGS
#endif

/*
 * +tag_old_static	When OLD_STATIC_TAGS defined: Include support for old
 *			style static tags: "file:tag  file  ..".  Slows down
 *			tag searching a bit.
 */
#ifndef MIN_FEAT
# define OLD_STATIC_TAGS
#endif

/*
 * +tag_any_white	When TAG_ANY_WHITE defined: Allow any white space to
 *			separate the fields in a tags file.	When not
 *			defined, only a TAB is allowed.
 */
/* #define TAG_ANY_WHITE */

/*
 * +eval		When WANT_EVAL defined: Include built-in script
 *			language and expression evaluation, ":let", ":if",
 *			etc.
 */
#ifndef MIN_FEAT
# define WANT_EVAL
#endif

/*
 * +autocmd		When defined: Include support for ":autocmd"
 */
#ifndef MIN_FEAT
# define AUTOCMD
#endif

/*
 * +viminfo		When VIMINFO defined: Include support for
 *			reading/writing the viminfo file. Takes about 8Kbyte
 *			of code.
 * VIMINFO_FILE		Location of user .viminfo file (should start with $).
 * VIMINFO_FILE2	Location of alternate user .viminfo file.
 */
#ifndef MIN_FEAT
# define VIMINFO
/* #define VIMINFO_FILE	"$HOME/foo/.viminfo" */
/* #define VIMINFO_FILE2 "~/bar/.viminfo" */
#endif

/*
 * +syntax		When SYNTAX_HL defined: Include support for syntax
 *			highlighting.  When using this, it's a good idea to
 *			have AUTOCMD too.
 */
#if !defined(MIN_FEAT) || defined(PROTO)
# define SYNTAX_HL
#endif

/*
 * +sniff		When USE_SNIFF defined: Include support for Sniff
 *			interface.  This needs to be defined in the Makefile.
 */

/*
 * +builtin_terms	Choose one out of the following four:
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
 *			(used only with NO_BUILTIN_TCAPS not defined).
 */
#ifdef HAVE_TGETENT
/* #define NO_BUILTIN_TCAPS */
#endif

#ifndef NO_BUILTIN_TCAPS
# ifdef MAX_FEAT
#  define ALL_BUILTIN_TCAPS
# else
#  define SOME_BUILTIN_TCAPS		/* default */
# endif
#endif

/*
 * +lispindent		When LISPINDENT defined: Include lisp indenting (From
 *			Eric Fischer). Doesn't completely work like Vi (yet).
 * +cindent		When CINDENT defined: Include C code indenting (From
 *			Eric Fischer).
 * +smartindent		When SMARTINDENT defined: Do smart C code indenting
 *			when the 'si' option is set. It's not as good as
 *			CINDENT, only included to keep the old code.
 *
 * These two need to be defined when making prototypes.
 */
#if !defined(MIN_FEAT) || defined(PROTO)
# define LISPINDENT
#endif

#if !defined(MIN_FEAT) || defined(PROTO)
# define CINDENT
#endif

#ifndef MIN_FEAT
# define SMARTINDENT
#endif

/*
 * Preferences:
 * ============
 */

/*
 * +writebackup		When WRITEBACKUP defined: 'writebackup' is default on:
 *			Use a backup file while overwriting a file.  But it's
 *			deleted again when 'backup' is not set.  Changing this
 *			is strongly discouraged: You can loose all your
 *			changes when the computer crashes while writing the
 *			file.
 */
#ifndef VMS		/* doesn't work on VMS */
# define WRITEBACKUP
#endif

/*
 * +xterm_save		When SAVE_XTERM_SCREEN defined: The t_ti and t_te
 *			entries for the builtin xterm will be set to save the
 *			screen when starting Vim and restoring it when
 *			exiting.
 */
/* #define SAVE_XTERM_SCREEN */

/*
 * DEBUG		When defined: Output a lot of debugging garbage.
 */
/* #define DEBUG */

/*
 * VIMRC_FILE		Name of the .vimrc file in current dir.
 */
/* #define VIMRC_FILE	".vimrc" */

/*
 * EXRC_FILE		Name of the .exrc file in current dir.
 */
/* #define EXRC_FILE	".exrc" */

/*
 * GVIMRC_FILE		Name of the .gvimrc file in current dir.
 */
/* #define GVIMRC_FILE	".gvimrc" */

/*
 * USR_VIMRC_FILE	Name of the user .vimrc file.
 * USR_VIMRC_FILE2	Name of alternate user .vimrc file.
 */
/* #define USR_VIMRC_FILE	"~/foo/.vimrc" */
/* #define USR_VIMRC_FILE2	"~/bar/.vimrc" */

/*
 * USR_EXRC_FILE	Name of the user .exrc file.
 * USR_EXRC_FILE2	Name of the alternate user .exrc file.
 */
/* #define USR_EXRC_FILE	"~/foo/.exrc" */
/* #define USR_EXRC_FILE2	"~/bar/.exrc" */

/*
 * USR_GVIMRC_FILE	Name of the user .gvimrc file.
 * USR_GVIMRC_FILE2	Name of the alternate user .gvimrc file.
 */
/* #define USR_GVIMRC_FILE	"~/foo/.gvimrc" */
/* #define USR_GVIMRC_FILE2	"~/bar/.gvimrc" */

/*
 * SYS_VIMRC_FILE	Name of the system-wide .vimrc file.
 */
/* #define SYS_VIMRC_FILE	"/etc/vimrc" */

/*
 * SYS_GVIMRC_FILE	Name of the system-wide .gvimrc file.
 */
/* #define SYS_GVIMRC_FILE	"/etc/gvimrc" */

/*
 * VIM_HLP		Name of the help file.
 */
/* #define VIM_HLP	"/foo/doc/help.txt" */

/*
 * SYS_MENU_FILE	Name of the default menu.vim file.
 */
/* #define SYS_MENU_FILE	"/foo/menu.vim" */


/*
 * Machine dependent:
 * ==================
 */

/*
 * +fork		Unix only.  When USE_SYSTEM defined: Use system()
 * +system		instead of fork/exec for starting a shell.  Doesn't
 *			work for the GUI!
 */
/* #define USE_SYSTEM */

/*
 * +X11			Unix only.  When WANT_X11 defined: Include code for
 *			xterm title saving. Only works if HAVE_X11 is also
 *			defined.
 */
#ifndef MIN_FEAT
# define WANT_X11
#endif

/*
 * +mouse_xterm		Unix only. When XTERM_MOUSE defined: Include code for
 *			xterm mouse handling.
 * +mouse_netterm	idem, NETTERM_MOUSE, for Netterm mouse handling.
 * +mouse_dec		idem, DEC_MOUSE, for Dec mouse handling.
 */
#ifndef MIN_FEAT
# define XTERM_MOUSE
#endif
#ifdef MAX_FEAT
# define NETTERM_MOUSE
#endif
#ifdef MAX_FEAT
# define DEC_MOUSE
#endif

/*
 * +GUI_Athena		To compile Vim with or without the GUI (gvim) you have
 * +GUI_BeOS		to edit Makefile.
 * +GUI_Motif
 */

/*
 * +ole			Win32 OLE automation: Use if_ole_vc.mak.
 */

/*
 * +perl		Perl interface: edit the Makefile.
 */

/*
 * +python		Python interface: edit the Makefile.
 */

/*
 * +terminfo		(Automatically) defined in the Makefile.
 */

/*
 * +tgetent		(Automatically) defined in the Makefile.
 */

/*
 * +ARP			Amiga only. When defined: Do not use arp.library, DOS
 *			2.0 required.
 */
/* #define NO_ARP */
