/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * proto.h: include the (automatically generated) function prototypes
 */

/*
 * Don't include these while generating prototypes.  Prevents problems when
 * files are missing.
 */
#ifndef PROTO

/*
 * Machine-dependent routines.
 */
# ifdef AMIGA
#  include "os_amiga.pro"
# endif
# if defined(UNIX) || defined(__EMX__)
#  include "os_unix.pro"
#  ifndef HAVE_RENAME
    int rename __ARGS((const char *, const char *));
#  endif
# endif
# ifdef MSDOS
#  include "os_msdos.pro"
# endif
# ifdef WIN32
#  include "os_win32.pro"
# endif
# ifdef VMS
#  include "os_vms.pro"
# endif
# ifdef __BEOS__
#  include "os_beos.pro"
# endif
# ifdef macintosh
#  include "os_mac.pro"
# endif

# include "buffer.pro"
# include "charset.pro"
# include "digraph.pro"
# include "edit.pro"
# include "eval.pro"
# include "ex_cmds.pro"
# include "ex_docmd.pro"
# include "ex_getln.pro"
# include "fileio.pro"
# include "getchar.pro"
# include "main.pro"
# include "mark.pro"
# ifndef MESSAGE_FILE
void
#ifdef __BORLANDC__
_RTLENTRYF
#endif
smsg __ARGS((char_u *, ...));	/* cannot be produced automatically */
void
#ifdef __BORLANDC__
_RTLENTRYF
#endif
smsg_attr __ARGS((int, char_u *, ...));
# endif
# include "memfile.pro"
# include "memline.pro"
# include "message.pro"
# include "misc1.pro"
# include "misc2.pro"
#ifndef HAVE_STRPBRK	    /* not generated automatically from misc2.c */
char_u *vim_strpbrk __ARGS((char_u *s, char_u *charset));
#endif
# include "normal.pro"
# include "ops.pro"
# include "option.pro"
# include "quickfix.pro"
# include "regexp.pro"
# include "screen.pro"
# include "search.pro"
# include "syntax.pro"
# include "tag.pro"
# include "term.pro"
# if defined(HAVE_TGETENT) && (defined(AMIGA) || defined(VMS))
#  include "termlib.pro"
# endif
# include "ui.pro"
# include "undo.pro"
# include "version.pro"
# include "window.pro"

# ifdef HAVE_PYTHON
#  include "if_python.pro"
# endif

# ifdef USE_GUI
#  include "gui.pro"
#  ifdef USE_GUI_WIN32
#   include "gui_w32.pro"
#   ifdef HAVE_OLE
#    include "if_ole.pro"
#   endif
#  endif
#  ifdef USE_GUI_MOTIF
#   include "gui_motif.pro"
#  endif
#  ifdef USE_GUI_ATHENA
#   include "gui_athena.pro"
#  endif
#  ifdef USE_GUI_BEOS
#   include "gui_beos.pro"
#  endif
#  ifdef USE_GUI_MAC
#   include "gui_mac.pro"
extern int putenv __ARGS((char *string));		/* from pty.c */
extern int OpenPTY __ARGS((char **ttyn));		/* from pty.c */
#  endif
#  ifdef USE_GUI_X11
#   include "gui_x11.pro"
extern int OpenPTY __ARGS((char **ttyn));	/* from pty.c */
#  endif
# endif	/* USE_GUI */

/*
 * The perl include files pollute the namespace, therfore proto.h must be
 * included before the perl include files.  But then CV is not defined, which
 * is used in if_perl.pro.  To get around this, the perl prototype files are
 * not included here for the perl files.  Use a dummy define for CV for the
 * other files.
 */
#if defined(HAVE_PERL_INTERP) && !defined(IN_PERL_FILE)
#   define CV void
# include "if_perl.pro"
# include "if_perlsfio.pro"
#endif

#ifdef __BORLANDC__
#define _PROTO_H
#endif
#endif /* PROTO */
