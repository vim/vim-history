/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"

/*
 * Vim originated from Stevie version 3.6 (Fish disk 217) by GRWalter (Fred)
 * It has been changed beyond recognition since then.
 *
 * Differences between version 5.x and 6.x can be found with ":help version6".
 * Differences between version 4.x and 5.x can be found with ":help version5".
 * Differences between version 3.0 and 4.x can be found with ":help version4".
 * All the remarks about older versions have been removed, they are not very
 * interesting.
 */

#include "version.h"

char	*Version = VIM_VERSION_SHORT;
char	*mediumVersion = VIM_VERSION_MEDIUM;

#if defined(HAVE_DATE_TIME) || defined(PROTO)
# if (defined(VMS) && defined(VAXC)) || defined(PROTO)
char	longVersion[sizeof(VIM_VERSION_LONG_DATE) + sizeof(__DATE__)
						      + sizeof(__TIME__) + 3];
    void
make_version()
{
    /*
     * Construct the long version string.  Necessary because
     * VAX C can't catenate strings in the preprocessor.
     */
    strcpy(longVersion, VIM_VERSION_LONG_DATE);
    strcat(longVersion, __DATE__);
    strcat(longVersion, " ");
    strcat(longVersion, __TIME__);
    strcat(longVersion, ")");
}
# else
char	*longVersion = VIM_VERSION_LONG_DATE __DATE__ " " __TIME__ ")";
# endif
#else
char	*longVersion = VIM_VERSION_LONG;
#endif

static void version_msg __ARGS((char *s));

static char *(features[]) =
{
#ifdef AMIGA		/* only for Amiga systems */
# ifdef FEAT_ARP
	"+ARP",
# else
	"-ARP",
# endif
#endif
#ifdef FEAT_AUTOCMD
	"+autocmd",
#else
	"-autocmd",
#endif
#ifdef FEAT_BEVAL
	"+balloon_eval",
#else
	"-balloon_eval",
#endif
#ifdef FEAT_BROWSE
	"+browse",
#else
	"-browse",
#endif
#ifdef NO_BUILTIN_TCAPS
	"-builtin_terms",
#endif
#ifdef SOME_BUILTIN_TCAPS
	"+builtin_terms",
#endif
#ifdef ALL_BUILTIN_TCAPS
	"++builtin_terms",
#endif
#ifdef FEAT_BYTEOFF
	"+byte_offset",
#else
	"-byte_offset",
#endif
#ifdef FEAT_GUI_MOTIF
# ifdef FEAT_CDE_COLORS
	"+cde_colors",
# else
	"-cde_colors",
# endif
#endif
#ifdef FEAT_CINDENT
	"+cindent",
#else
	"-cindent",
#endif
#ifdef FEAT_CMDL_COMPL
	"+cmdline_compl",
#else
	"-cmdline_compl",
#endif
#ifdef FEAT_CMDHIST
	"+cmdline_hist",
#else
	"-cmdline_hist",
#endif
#ifdef FEAT_CMDL_INFO
	"+cmdline_info",
#else
	"-cmdline_info",
#endif
#ifdef FEAT_COMMENTS
	"+comments",
#else
	"-comments",
#endif
#ifdef FEAT_CRYPT
	"+cryptv",
#else
	"-cryptv",
#endif
#ifdef FEAT_CSCOPE
	"+cscope",
#else
	"-cscope",
#endif
#if defined(FEAT_CON_DIALOG) && defined(FEAT_GUI_DIALOG)
	"+dialog_con_gui",
#else
# if defined(FEAT_CON_DIALOG)
	"+dialog_con",
# else
#  if defined(FEAT_GUI_DIALOG)
	"+dialog_gui",
#  else
	"-dialog",
#  endif
# endif
#endif
#ifdef FEAT_DIGRAPHS
	"+digraphs",
#else
	"-digraphs",
#endif
#ifdef EBCDIC
	"+ebcdic",
#else
	"-ebcdic",
#endif
#ifdef FEAT_EMACS_TAGS
	"+emacs_tags",
#else
	"-emacs_tags",
#endif
#ifdef FEAT_EVAL
	"+eval",
#else
	"-eval",
#endif
#ifdef FEAT_EX_EXTRA
	"+ex_extra",
#else
	"-ex_extra",
#endif
#ifdef FEAT_SEARCH_EXTRA
	"+extra_search",
#else
	"-extra_search",
#endif
#ifdef FEAT_FKMAP
	"+farsi",
#else
	"-farsi",
#endif
#ifdef FEAT_SEARCHPATH
	"+file_in_path",
#else
	"-file_in_path",
#endif
#ifdef FEAT_OSFILETYPE
	"+osfiletype",
#else
	"-osfiletype",
#endif
#ifdef FEAT_FIND_ID
	"+find_in_path",
#else
	"-find_in_path",
#endif
#ifdef FEAT_FOLDING
	"+folding",
#else
	"-folding",
#endif
#ifdef FEAT_FOOTER
	"+footer",
#else
	"-footer",
#endif
	    /* only interesting on Unix systems */
#if !defined(USE_SYSTEM) && defined(UNIX)
	"+fork()",
#endif
#ifdef FEAT_GETTEXT
	"+gettext",
#else
	"-gettext",
#endif
#ifdef FEAT_HANGULIN
	"+hangul_input",
#else
	"-hangul_input",
#endif
#ifdef FEAT_INS_EXPAND
	"+insert_expand",
#else
	"-insert_expand",
#endif
#ifdef FEAT_KEYMAP
	"+keymap",
#else
	"-keymap",
#endif
#ifdef FEAT_LANGMAP
	"+langmap",
#else
	"-langmap",
#endif
#ifdef FEAT_LIBCALL
	"+libcall",
#else
	"-libcall",
#endif
#ifdef FEAT_LINEBREAK
	"+linebreak",
#else
	"-linebreak",
#endif
#ifdef FEAT_LISP
	"+lispindent",
#else
	"-lispindent",
#endif
#ifdef FEAT_LISTCMDS
	"+listcmds",
#else
	"-listcmds",
#endif
#ifdef FEAT_LOCALMAP
	"+localmap",
#else
	"-localmap",
#endif
#ifdef FEAT_KEYMAP
	"+keymap",
#else
	"-keymap",
#endif
#ifdef FEAT_MENU
	"+menu",
#else
	"-menu",
#endif
#ifdef FEAT_SESSION
	"+mksession",
#else
	"-mksession",
#endif
#ifdef FEAT_MODIFY_FNAME
	"+modify_fname",
#else
	"-modify_fname",
#endif
#ifdef FEAT_MOUSE
	"+mouse",
#  ifdef FEAT_MOUSESHAPE
	"+mouseshape",
#  else
	"-mouseshape",
#  endif
# else
	"-mouse",
#endif
#if defined(UNIX) || defined(VMS)
# ifdef FEAT_MOUSE_DEC
	"+mouse_dec",
# else
	"-mouse_dec",
# endif
# ifdef FEAT_MOUSE_GPM
	"+mouse_gpm",
# else
	"-mouse_gpm",
# endif
# ifdef FEAT_MOUSE_JSB
	"+mouse_jsbterm",
# else
	"-mouse_jsbterm",
# endif
# ifdef FEAT_MOUSE_NET
	"+mouse_netterm",
# else
	"-mouse_netterm",
# endif
# ifdef FEAT_MOUSE_XTERM
	"+mouse_xterm",
# else
	"-mouse_xterm",
# endif
#endif
#ifdef FEAT_MBYTE_IME
	"+multi_byte_ime",
#else
# ifdef FEAT_MBYTE
	"+multi_byte",
# else
	"-multi_byte",
# endif
#endif
#ifdef FEAT_MULTI_LANG
	"+multi_lang",
#else
	"-multi_lang",
#endif
#ifdef FEAT_GUI_W32
# ifdef FEAT_OLE
	"+ole",
# else
	"-ole",
# endif
#endif
#ifdef FEAT_PATH_EXTRA
	"+path_extra",
#else
	"-path_extra",
#endif
#ifdef FEAT_PERL
	"+perl",
#else
	"-perl",
#endif
#ifdef FEAT_PYTHON
	"+python",
#else
	"-python",
#endif
#ifdef FEAT_QUICKFIX
	"+quickfix",
#else
	"-quickfix",
#endif
#ifdef FEAT_RIGHTLEFT
	"+rightleft",
#else
	"-rightleft",
#endif
#ifdef FEAT_RUBY
	"+ruby",
#else
	"-ruby",
#endif
#ifdef FEAT_SCROLLBIND
	"+scrollbind",
#else
	"-scrollbind",
#endif
#ifdef FEAT_SIGNS
	"+signs",
#else
	"-signs",
#endif
#ifdef FEAT_SMARTINDENT
	"+smartindent",
#else
	"-smartindent",
#endif
#ifdef FEAT_SNIFF
	"+sniff",
#else
	"-sniff",
#endif
#ifdef FEAT_STL_OPT
	"+statusline",
#else
	"-statusline",
#endif
#ifdef FEAT_SUN_WORKSHOP
	"+sun_workshop",
#else
	"-sun_workshop",
#endif
#ifdef FEAT_SYN_HL
	"+syntax",
#else
	"-syntax",
#endif
	    /* only interesting on Unix systems */
#if defined(USE_SYSTEM) && (defined(UNIX) || defined(__EMX__))
	"+system()",
#endif
#ifdef FEAT_TAG_BINS
	"+tag_binary",
#else
	"-tag_binary",
#endif
#ifdef FEAT_TAG_OLDSTATIC
	"+tag_old_static",
#else
	"-tag_old_static",
#endif
#ifdef FEAT_TAG_ANYWHITE
	"+tag_any_white",
#else
	"-tag_any_white",
#endif
#ifdef FEAT_TCL
	"+tcl",
#else
	"-tcl",
#endif
#if defined(UNIX) || defined(__EMX__)
/* only Unix (or OS/2 with EMX!) can have terminfo instead of termcap */
# ifdef TERMINFO
	"+terminfo",
# else
	"-terminfo",
# endif
#else		    /* unix always includes termcap support */
# ifdef HAVE_TGETENT
	"+tgetent",
# else
	"-tgetent",
# endif
#endif
#ifdef FEAT_TERMRESPONSE
	"+termresponse",
#else
	"-termresponse",
#endif
#ifdef FEAT_TEXTOBJ
	"+textobjects",
#else
	"-textobjects",
#endif
#ifdef FEAT_TITLE
	"+title",
#else
	"-title",
#endif
#ifdef FEAT_TOOLBAR
	"+toolbar",
#else
	"-toolbar",
#endif
#ifdef FEAT_USR_CMDS
	"+user_commands",
#else
	"-user_commands",
#endif
#ifdef FEAT_VERTSPLIT
	"+vertsplit",
#else
	"-vertsplit",
#endif
#ifdef FEAT_VIRTUALEDIT
	"+virtualedit",
#else
	"-virtualedit",
#endif
#ifdef FEAT_VISUAL
	"+visual",
# ifdef FEAT_VISUALEXTRA
	"+visualextra",
# else
	"-visualextra",
# endif
#else
	"-visual",
#endif
#ifdef FEAT_VIMINFO
	"+viminfo",
#else
	"-viminfo",
#endif
#ifdef FEAT_WILDIGN
	"+wildignore",
#else
	"-wildignore",
#endif
#ifdef FEAT_WILDMENU
	"+wildmenu",
#else
	"-wildmenu",
#endif
#ifdef FEAT_WINDOWS
	"+windows",
#else
	"-windows",
#endif
#ifdef FEAT_WRITEBACKUP
	"+writebackup",
#else
	"-writebackup",
#endif
#if defined(UNIX) || defined(VMS)
# ifdef FEAT_X11
	"+X11",
# else
	"-X11",
# endif
#endif
#ifdef FEAT_XFONTSET
	"+xfontset",
#else
	"-xfontset",
#endif
#ifdef FEAT_XIM
	"+xim",
#else
	"-xim",
#endif
#ifdef UNIX
# ifdef FEAT_XCLIPBOARD
	"+xterm_clipboard",
# else
	"-xterm_clipboard",
# endif
#endif
#ifdef FEAT_XTERM_SAVE
	"+xterm_save",
#else
	"-xterm_save",
#endif
	NULL
};

static int included_patches[] =
{   /* Add new patch number below this line */
/**/
    0
};

    int
highest_patch()
{
    int		i;
    int		h = 0;

    for (i = 0; included_patches[i] != 0; ++i)
	if (included_patches[i] > h)
	    h = included_patches[i];
    return h;
}

    void
ex_version(eap)
    exarg_t	*eap;
{
    /*
     * Ignore a ":version 9.99" command.
     */
    if (*eap->arg == NUL)
    {
	msg_putchar('\n');
	list_version();
    }
}

    void
list_version()
{
    int		i;
    int		first;
    char	*s = "";

    /*
     * When adding features here, don't forget to update the list of
     * internal variables in eval.c!
     */
    MSG(longVersion);
#ifdef WIN32
# ifdef FEAT_GUI_W32
#  if (_MSC_VER <= 1010)    /* Only MS VC 4.1 and earlier can do Win32s */
    MSG_PUTS(_("\nMS-Windows 16/32 bit GUI version"));
#  else
    MSG_PUTS(_("\nMS-Windows 32 bit GUI version"));
#  endif
    if (gui_is_win32s())
	MSG_PUTS(_(" in Win32s mode"));
# ifdef FEAT_OLE
    MSG_PUTS(_(" with OLE support"));
# endif
# else
    MSG_PUTS(_("\nMS-Windows 32 bit console version"));
# endif
#endif
#ifdef WIN16
    MSG_PUTS(_("\nMS-Windows 16 bit version"));
#endif
#ifdef MSDOS
# ifdef DJGPP
    MSG_PUTS(_("\n32 bit MS-DOS version"));
# else
    MSG_PUTS(_("\n16 bit MS-DOS version"));
# endif
#endif
#ifdef macintosh
    MSG_PUTS(_("\nMacOS version"));
#endif
#ifdef RISCOS
    MSG_PUTS(_("\nRISC OS version"));
#endif
#ifdef VMS
    MSG_PUTS("\nOpenVMS version");
#endif

    /* Print the list of patch numbers if there is at least one. */
    /* Print a range when patches are consecutive: "1-10, 12, 15-40, 42-45" */
    if (included_patches[0] != 0)
    {
	MSG_PUTS(_("\nIncluded patches: "));
	first = -1;
	/* find last one */
	for (i = 0; included_patches[i] != 0; ++i)
	    ;
	while (--i >= 0)
	{
	    if (first < 0)
		first = included_patches[i];
	    if (i == 0 || included_patches[i - 1] != included_patches[i] + 1)
	    {
		MSG_PUTS(s);
		s = ", ";
		msg_outnum((long)first);
		if (first != included_patches[i])
		{
		    MSG_PUTS("-");
		    msg_outnum((long)included_patches[i]);
		}
		first = -1;
	    }
	}
    }

#ifdef HAVE_PATHDEF
    if (*compiled_user != NUL || *compiled_sys != NUL)
    {
	MSG_PUTS(_("\nCompiled "));
	if (*compiled_user != NUL)
	{
	    MSG_PUTS(_("by "));
	    MSG_PUTS(compiled_user);
	}
	if (*compiled_sys != NUL)
	{
	    MSG_PUTS("@");
	    MSG_PUTS(compiled_sys);
	}
    }
#endif

#ifdef FEAT_HUGE
    MSG_PUTS(_("\nHuge version "));
#else
# ifdef FEAT_BIG
    MSG_PUTS(_("\nBig version "));
# else
#  ifdef FEAT_NORMAL
    MSG_PUTS(_("\nNormal version "));
#  else
#   ifdef FEAT_SMALL
    MSG_PUTS(_("\nSmall version "));
#   else
    MSG_PUTS(_("\nTiny version "));
#   endif
#  endif
# endif
#endif
#ifndef FEAT_GUI
    MSG_PUTS(_("without GUI."));
#else
# ifdef FEAT_GUI_GTK
#  ifdef FEAT_GUI_GNOME
    MSG_PUTS(_("with GTK-GNOME GUI."));
#  else
    MSG_PUTS(_("with GTK GUI."));
#  endif
# else
#  ifdef FEAT_GUI_MOTIF
    MSG_PUTS(_("with X11-Motif GUI."));
#  else
#   ifdef FEAT_GUI_ATHENA
    MSG_PUTS(_("with X11-Athena GUI."));
#   else
#    ifdef FEAT_GUI_BEOS
    MSG_PUTS(_("with BeOS GUI."));
#    else
#     if defined(MSWIN) || defined(macintosh)
    MSG_PUTS(_("with GUI."));
#     endif
#    endif
#   endif
#  endif
# endif
#endif
    MSG_PUTS(_("  Features included (+) or not (-):\n"));

    /* print all the features */
    for (i = 0; features[i] != NULL; ++i)
    {
	version_msg(features[i]);
	if (msg_col > 0)
	    msg_putchar(' ');
    }

    msg_putchar('\n');
#ifdef SYS_VIMRC_FILE
    version_msg(_("   system vimrc file: \""));
    version_msg(SYS_VIMRC_FILE);
    version_msg(_("\"\n"));
#endif
#ifdef USR_VIMRC_FILE
    version_msg(_("     user vimrc file: \""));
    version_msg(USR_VIMRC_FILE);
    version_msg(_("\"\n"));
#endif
#ifdef USR_VIMRC_FILE2
    version_msg(_(" 2nd user vimrc file: \""));
    version_msg(USR_VIMRC_FILE2);
    version_msg(_("\"\n"));
#endif
#ifdef USR_VIMRC_FILE3
    version_msg(_(" 3rd user vimrc file: \""));
    version_msg(USR_VIMRC_FILE3);
    version_msg(_("\"\n"));
#endif
#ifdef USR_EXRC_FILE
    version_msg(_("      user exrc file: \""));
    version_msg(USR_EXRC_FILE);
    version_msg(_("\"\n"));
#endif
#ifdef USR_EXRC_FILE2
    version_msg(_("  2nd user exrc file: \""));
    version_msg(USR_EXRC_FILE2);
    version_msg(_("\"\n"));
#endif
#ifdef FEAT_GUI
# ifdef SYS_GVIMRC_FILE
    version_msg(_("  system gvimrc file: \""));
    version_msg(SYS_GVIMRC_FILE);
    MSG_PUTS(_("\"\n"));
# endif
    version_msg(_("    user gvimrc file: \""));
    version_msg(USR_GVIMRC_FILE);
    version_msg(_("\"\n"));
# ifdef USR_GVIMRC_FILE2
    version_msg(_("2nd user gvimrc file: \""));
    version_msg(USR_GVIMRC_FILE2);
    version_msg(_("\"\n"));
# endif
# ifdef USR_GVIMRC_FILE3
    version_msg(_("3rd user gvimrc file: \""));
    version_msg(USR_GVIMRC_FILE3);
    version_msg(_("\"\n"));
# endif
#endif
#ifdef FEAT_GUI
# ifdef SYS_MENU_FILE
    version_msg(_("    system menu file: \""));
    version_msg(SYS_MENU_FILE);
    MSG_PUTS(_("\"\n"));
# endif
#endif
#ifdef HAVE_PATHDEF
    if (*default_vim_dir != NUL)
    {
	version_msg(_("  fall-back for $VIM: \""));
	version_msg((char *)default_vim_dir);
	MSG_PUTS(_("\"\n"));
    }
    if (*default_vimruntime_dir != NUL)
    {
	version_msg(_(" f-b for $VIMRUNTIME: \""));
	version_msg((char *)default_vimruntime_dir);
	MSG_PUTS(_("\"\n"));
    }
    version_msg(_("Compilation: "));
    version_msg((char *)all_cflags);
    msg_putchar('\n');
#ifdef VMS
    if (*compiler_version != NUL)
    {
	version_msg(_("Compiler: "));
	version_msg((char *)compiler_version);
	msg_putchar('\n');
    }
#endif
    version_msg(_("Linking: "));
    version_msg((char *)all_lflags);
#endif
#ifdef DEBUG
    msg_putchar('\n');
    version_msg(_("  DEBUG BUILD"));
#endif
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

/*
 * Give an introductory message about Vim.
 * Only used when starting Vim on an empty file, without a file name.
 * Or with the ":intro" command (for Sven :-).
 */
    void
intro_message()
{
    int		i;
    int		row;
    int		col;
    int		blanklines;
    char_u	vers[20];
    static char	*(lines[]) =
    {
	N_("VIM - Vi IMproved"),
	"",
	N_("version "),
	N_("by Bram Moolenaar et al."),
	"",
	N_("Vim is freely distributable"),
	N_("type  :help uganda<Enter>     if you like Vim "),
	"",
	N_("type  :q<Enter>               to exit         "),
	N_("type  :help<Enter>  or  <F1>  for on-line help"),
	N_("type  :help version6<Enter>   for version info"),
	NULL,
	"",
	N_("Running in Vi compatible mode"),
	N_("type  :set nocp<Enter>        for Vim defaults"),
	N_("type  :help cp-default<Enter> for info on this"),
    };

    /* blanklines = screen height - # message lines */
    blanklines = (int)Rows - ((sizeof(lines) / sizeof(char *)) - 1);
    if (!p_cp)
	blanklines += 4;  /* add 4 for not showing "Vi compatible" message */
#if defined(WIN32) && !defined(FEAT_GUI_W32)
    if (mch_windows95())
	blanklines -= 3;  /* subtract 3 for showing "Windows 95" message */
#endif
#if defined(__BEOS__) && defined(__INTEL__)
    blanklines -= 3;      /* subtract 3 for showing "BEOS on Intel" message */
#endif

    /* start displaying the message lines after half of the blank lines */
    row = blanklines / 2;
    if (row >= 2 && Columns >= 50)
    {
	for (i = 0; i < (int)(sizeof(lines) / sizeof(char *)); ++i)
	{
	    if (lines[i] == NULL)
	    {
		if (!p_cp)
		    break;
		continue;
	    }
	    if (*lines[i] != NUL)
	    {
		col = strlen(_(lines[i]));
		if (i == 2)
		{
		    STRCPY(vers, mediumVersion);
		    if (highest_patch())
		    {
			/* Check for 9.9x, alpha/beta version */
			if (isalpha((int)mediumVersion[3]))
			    sprintf((char *)vers + 4, ".%d%s", highest_patch(),
							   mediumVersion + 4);
			else
			    sprintf((char *)vers + 3, ".%d", highest_patch());
		    }
		    col += STRLEN(vers);
		}
		col = (Columns - col) / 2;
		if (col < 0)
		    col = 0;
		screen_puts((char_u *)_(lines[i]), row, col, 0);
		if (i == 2)
		    screen_puts(vers, row, col + 8, 0);
	    }
	    ++row;
	}
#if defined(WIN32) && !defined(FEAT_GUI_W32)
	if (mch_windows95())
	{
	    screen_puts((char_u *)_("WARNING: Windows 95/98/ME detected"),
					    row + 1, col + 6, hl_attr(HLF_E));
	    screen_puts((char_u *)_("type  :help windows95<Enter>  for info on this"),
							     row + 2, col, 0);
	}
#endif
#if defined(__BEOS__) && defined(__INTEL__)
	screen_puts((char_u *)_("     WARNING: Intel CPU detected.    "),
					    row + 1, col + 4, hl_attr(HLF_E));
	screen_puts((char_u *)_(" PPC has a much better architecture. "),
					    row + 2, col + 4, hl_attr(HLF_E));
#endif
    }
}

/*
 * ":intro": clear screen, display intro screen and wait for return.
 */
/*ARGSUSED*/
    void
ex_intro(eap)
    exarg_t	*eap;
{
    screenclear();
    intro_message();
    wait_return(TRUE);
}
