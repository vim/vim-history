/* vi:set ts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

/*
 * Vim originated from Stevie version 3.6 (Fish disk 217) by GRWalter (Fred)
 * It has been changed beyond recognition since then.
 *
 * All the remarks about older versions have been removed, they are not very
 * interesting.  Differences between version 3.0 and 4.0 can be found in
 * "../doc/vim_40.txt".
*/

/* Don't forget to update the numbers in version.h for Win32!!! */

char		   *Version = "VIM 4.0";
#ifdef HAVE_DATE_TIME
char		   *longVersion = "VIM - Vi IMproved 4.0 (1996 May 29, compiled " __DATE__ " " __TIME__ ")";
#else
char		   *longVersion = "VIM - Vi IMproved 4.0 (1996 May 29)";
#endif

static void version_msg __ARGS((char *s));

	void
do_version(arg)
	char_u	*arg;
{
	long		n;

	if (*arg != NUL)
	{
		found_version = getdigits(&arg) * 100;
		if (*arg == '.' && isdigit(arg[1]))
		{
			/* "4.1"  -> 401, "4.10" -> 410 */
			n = arg[1] - '0';
			if (isdigit(arg[2]))
				found_version += (arg[2] - '0') + n * 10;
			else
				found_version += n;
		}
		if (found_version > 400)
		{
			MSG("Warning: Found newer version command");
			if (sourcing_name != NULL)
			{
				MSG_OUTSTR(" in: \"");
				msg_outstr(sourcing_name);
				MSG_OUTSTR("\" line: ");
				msg_outnum((long)sourcing_lnum);
			}
		}
	}
	else
	{
		msg_outchar('\n');
		MSG(longVersion);
#ifdef WIN32
		MSG_OUTSTR("\nWindows NT / Windows 95 version");
#endif
#ifdef MSDOS
# ifdef DJGPP
		MSG_OUTSTR("\n32 bit MS-DOS version");
# else
		MSG_OUTSTR("\n16 bit MS-DOS version");
# endif
#endif
		MSG_OUTSTR("\nCompiled with (+) or without (-):\n");
#ifdef AMIGA			/* only for Amiga systems */
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
#ifdef COMPATIBLE
		version_msg("+compatible ");
#else
		version_msg("-compatible ");
#endif
#ifdef DEBUG
		version_msg("+debug ");
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
		version_msg("-GUI ");
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
#ifdef RIGHTLEFT
		version_msg("+rightleft ");
#else
		version_msg("-rightleft ");
#endif
#ifdef SMARTINDENT
		version_msg("+smartindent ");
#else
		version_msg("-smartindent ");
#endif
			/* only interesting on Unix systems */
#if defined(USE_SYSTEM) && (defined(UNIX) || defined(__EMX__))
		version_msg("+system() ");
#endif
#if defined(UNIX) || defined(__EMX__)
/* only unix (or OS/2 with EMX!) can have terminfo instead of termcap */
# ifdef TERMINFO
		version_msg("+terminfo ");
# else
		version_msg("-terminfo ");
# endif
#else				/* unix always includes termcap support */
# ifdef HAVE_TGETENT
		version_msg("+tgetent ");
# else
		version_msg("-tgetent ");
# endif
#endif
#ifdef VIMINFO
		version_msg("+viminfo ");
#else
		version_msg("-viminfo ");
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
		msg_outchar('\n');
#ifdef USR_VIMRC_FILE
		version_msg("user vimrc file: \"");
		version_msg(USR_VIMRC_FILE);
		version_msg("\" ");
#endif
#ifdef USR_EXRC_FILE
		version_msg("user exrc file: \"");
		version_msg(USR_EXRC_FILE);
		version_msg("\" ");
#endif
#ifdef USE_GUI
		version_msg("user gvimrc file: \"");
		version_msg(USR_GVIMRC_FILE);
		version_msg("\" ");
#endif
#if defined(HAVE_CONFIG_H) || defined(OS2)
		msg_outchar('\n');
		version_msg("system vimrc file: \"");
		version_msg((char *)sys_vimrc_fname);
		version_msg("\"");
# ifdef USE_GUI
		msg_outchar('\n');
		version_msg("system gvimrc file: \"");
		version_msg((char *)sys_gvimrc_fname);
		MSG_OUTSTR("\"");
# endif
		msg_outchar('\n');
		version_msg("Compilation: ");
		version_msg((char *)all_cflags);
#endif
	}
}

/*
 * Output a string for the version message.  If it's going to wrap, output a
 * newline, unless the message is too long to fit on the screen anyway.
 */
	static void
version_msg(s)
	char		*s;
{
	int		len = strlen(s);

	if (len < (int)Columns && msg_col + len >= (int)Columns)
		msg_outchar('\n');
	MSG_OUTSTR(s);
}
