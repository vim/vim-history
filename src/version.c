/* vi:set ts=8 sts=4 sw=4:
 * vi:set comments=sbl\:*\ -,mb\:*,el\:*\ -,sr\:/\*,mb\:*,el\:*\/,fb\:- :
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include "vim.h"

/*
 * Vim originated from Stevie version 3.6 (Fish disk 217) by GRWalter (Fred)
 * It has been changed beyond recognition since then.
 *
 * Differences between version 4.x and 5.0 can be found with ":help version5".
 * Differences between version 3.0 and 4.x can be found with ":help version4".
 * All the remarks about older versions have been removed, they are not very
 * interesting.
 */

#include "version.h"

char	*Version = VIM_VERSION_SHORT;
char	*mediumVersion = VIM_VERSION_MEDIUM;
#ifdef HAVE_DATE_TIME
char	*longVersion = VIM_VERSION_LONG_DATE __DATE__ " " __TIME__ ")";
#else
char	*longVersion = VIM_VERSION_LONG;
#endif

static void version_msg __ARGS((char *s));

    int
get_version()
{
    return (VIM_VERSION_MAJOR * 100 + VIM_VERSION_MINOR);
}

    void
do_version(arg)
    char_u  *arg;
{
    /*
     * Ignore a ":version 9.99" command.
     */
    if (*arg == NUL)
    {
	/*
	 * When adding features here, don't forget to update the list of
	 * internal variables in eval.c!
	 */
	msg_putchar('\n');
	MSG(longVersion);
#ifdef WIN32
	MSG_PUTS("\nWindows NT / Windows 95 version");
#endif
#ifdef MSDOS
# ifdef DJGPP
	MSG_PUTS("\n32 bit MS-DOS version");
# else
	MSG_PUTS("\n16 bit MS-DOS version");
# endif
#endif
	MSG_PUTS("\nCompiled with (+) or without (-):\n");
#ifdef AMIGA		/* only for Amiga systems */
# ifdef NO_ARP
	version_msg("-ARP ");
# else
	version_msg("+ARP ");
# endif
#endif
#ifdef AUTOCMD
	version_msg("+autocmd ");
#else
	version_msg("-autocmd ");
#endif
#ifdef NO_BUILTIN_TCAPS
	version_msg("-builtin_terms ");
#endif
#ifdef SOME_BUILTIN_TCAPS
	version_msg("+builtin_terms ");
#endif
#ifdef ALL_BUILTIN_TCAPS
	version_msg("++builtin_terms ");
#endif
#ifdef CINDENT
	version_msg("+cindent ");
#else
	version_msg("-cindent ");
#endif
#ifdef DIGRAPHS
	version_msg("+digraphs ");
#else
	version_msg("-digraphs ");
#endif
#ifdef EMACS_TAGS
	version_msg("+emacs_tags ");
#else
	version_msg("-emacs_tags ");
#endif
#ifdef WANT_EVAL
	version_msg("+eval ");
#else
	version_msg("-eval ");
#endif
#ifdef EX_EXTRA
	version_msg("+ex_extra ");
#else
	version_msg("-ex_extra ");
#endif
#ifdef EXTRA_SEARCH
	version_msg("+extra_search ");
#else
	version_msg("-extra_search ");
#endif
#ifdef FKMAP
	version_msg("+farsi ");
#else
	version_msg("-farsi ");
#endif
#ifdef FILE_IN_PATH
	version_msg("+file_in_path ");
#else
	version_msg("-file_in_path ");
#endif
#ifdef FIND_IN_PATH
	version_msg("+find_in_path ");
#else
	version_msg("-find_in_path ");
#endif
	    /* only interesting on Unix systems */
#if !defined(USE_SYSTEM) && defined(UNIX)
	version_msg("+fork() ");
#endif
#ifdef UNIX
# ifdef USE_GUI_MOTIF
	version_msg("+GUI_Motif ");
# else
#  ifdef USE_GUI_ATHENA
	version_msg("+GUI_Athena ");
#  else
#   ifdef USE_GUI_BEOS
	version_msg("+GUI_BeOS ");
#    else
	version_msg("-GUI ");
#   endif
#  endif
# endif
#endif
#ifdef INSERT_EXPAND
	version_msg("+insert_expand ");
#else
	version_msg("-insert_expand ");
#endif
#ifdef HAVE_LANGMAP
	version_msg("+langmap ");
#else
	version_msg("-langmap ");
#endif
#ifdef LISPINDENT
	version_msg("+lispindent ");
#else
	version_msg("-lispindent ");
#endif
#ifdef UNIX
# ifdef DEC_MOUSE
	version_msg("+mouse_dec ");
# else
	version_msg("-mouse_dec ");
# endif
# ifdef NETTERM_MOUSE
	version_msg("+mouse_netterm ");
# else
	version_msg("-mouse_netterm ");
# endif
# ifdef XTERM_MOUSE
	version_msg("+mouse_xterm ");
# else
	version_msg("-mouse_xterm ");
# endif
#endif
#ifdef USE_GUI_WIN32
# ifdef HAVE_OLE
	version_msg("+ole ");
# else
	version_msg("-ole ");
# endif
#endif
#ifdef HAVE_PERL_INTERP
	version_msg("+perl ");
#else
	version_msg("-perl ");
#endif
#ifdef QUICKFIX
	version_msg("+quickfix ");
#else
	version_msg("-quickfix ");
#endif
#ifdef HAVE_PYTHON
	version_msg("+python ");
#else
	version_msg("-python ");
#endif
#ifdef RIGHTLEFT
	version_msg("+rightleft ");
#else
	version_msg("-rightleft ");
#endif
#ifdef SHOWCMD
	version_msg("+showcmd ");
#else
	version_msg("-showcmd ");
#endif
#ifdef SMARTINDENT
	version_msg("+smartindent ");
#else
	version_msg("-smartindent ");
#endif
#ifdef USE_SNIFF
	version_msg("+sniff ");
#else
	version_msg("-sniff ");
#endif
#ifdef SYNTAX_HL
	version_msg("+syntax ");
#else
	version_msg("-syntax ");
#endif
	    /* only interesting on Unix systems */
#if defined(USE_SYSTEM) && (defined(UNIX) || defined(__EMX__))
	version_msg("+system() ");
#endif
#ifdef BINARY_TAGS
	version_msg("+tag_binary ");
#else
	version_msg("-tag_binary ");
#endif
#ifdef OLD_STATIC_TAGS
	version_msg("+tag_old_static ");
#else
	version_msg("-tag_old_static ");
#endif
#ifdef TAG_ANY_WHITE
	version_msg("+tag_any_white ");
#else
	version_msg("-tag_any_white ");
#endif
#if defined(UNIX) || defined(__EMX__)
/* only Unix (or OS/2 with EMX!) can have terminfo instead of termcap */
# ifdef TERMINFO
	version_msg("+terminfo ");
# else
	version_msg("-terminfo ");
# endif
#else		    /* unix always includes termcap support */
# ifdef HAVE_TGETENT
	version_msg("+tgetent ");
# else
	version_msg("-tgetent ");
# endif
#endif
#ifdef TEXT_OBJECTS
	version_msg("+textobjects ");
#else
	version_msg("-textobjects ");
#endif
#ifdef VIMINFO
	version_msg("+viminfo ");
#else
	version_msg("-viminfo ");
#endif
#ifdef SAVE_XTERM_SCREEN
	version_msg("+xterm_save ");
#else
	version_msg("-xterm_save ");
#endif
#ifdef WRITEBACKUP
	version_msg("+writebackup ");
#else
	version_msg("-writebackup ");
#endif
#ifdef UNIX
# if defined(WANT_X11) && defined(HAVE_X11)
	version_msg("+X11 ");
# else
	version_msg("-X11 ");
# endif
#endif
	msg_putchar('\n');
#ifdef USR_VIMRC_FILE
	version_msg("   user vimrc file: \"");
	version_msg(USR_VIMRC_FILE);
	version_msg("\" ");
#endif
#ifdef USR_VIMRC_FILE2
	version_msg("2nd user vimrc file: \"");
	version_msg(USR_VIMRC_FILE2);
	version_msg("\" ");
#endif
#ifdef USR_EXRC_FILE
	version_msg("    user exrc file: \"");
	version_msg(USR_EXRC_FILE);
	version_msg("\" ");
#endif
#ifdef USR_EXRC_FILE2
	version_msg("2nd user exrc file: \"");
	version_msg(USR_EXRC_FILE2);
	version_msg("\" ");
#endif
#ifdef USE_GUI
	version_msg("  user gvimrc file: \"");
	version_msg(USR_GVIMRC_FILE);
	version_msg("\" ");
# ifdef USR_GVIMRC_FILE2
	version_msg("2nd user gvimrc file: \"");
	version_msg(USR_GVIMRC_FILE2);
	version_msg("\" ");
# endif
#endif
#ifdef SYS_VIMRC_FILE
	msg_putchar('\n');
	version_msg(" system vimrc file: \"");
	version_msg(SYS_VIMRC_FILE);
	version_msg("\"");
#endif
#ifdef USE_GUI
# ifdef SYS_GVIMRC_FILE
	msg_putchar('\n');
	version_msg("system gvimrc file: \"");
	version_msg(SYS_GVIMRC_FILE);
	MSG_PUTS("\"");
# endif
# ifdef SYS_MENU_FILE
	msg_putchar('\n');
	version_msg("  system menu file: \"");
	version_msg(SYS_MENU_FILE);
	MSG_PUTS("\"");
# endif
#endif
#ifdef HAVE_PATHDEF
	msg_putchar('\n');
	version_msg("  default for $VIM: \"");
	version_msg((char *)default_vim_dir);
	MSG_PUTS("\"\n");
	version_msg("Compilation: ");
	version_msg((char *)all_cflags);
	msg_putchar('\n');
	version_msg("Linking: ");
	version_msg((char *)all_lflags);
#endif
#ifdef DEBUG
	msg_putchar('\n');
	version_msg("  DEBUG BUILD");
#endif
    }
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 */
    static void
version_msg(s)
    char	*s;
{
    int		len = strlen(s);

    if (len < (int)Columns && msg_col + len >= (int)Columns)
	msg_putchar('\n');
    MSG_PUTS(s);
}
