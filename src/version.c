/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * Version	Changes (and person who did them)
 * -------	---------------------------------
 * 3.10 	- version that started it all. Found on comp.sources.unix
 *			  Jun88 Volume 15 i037, i038, i039, i040, i042, and INF3
 *			- Tim Thompson and Tony Andrews
 *
 * 3.10A	- took version of STEVIE posted to usenet and added Amiga
 *			  and BSD support; added undo and redo commands; sped up
 *			  output to screen; sped up on-screen activities (such as
 *			  cursoring); fixed miscellaneous small bugs and changed some
 *			  commands so that they more closely resembled vi.
 *			- GRWalter (Fred)
 *
 * 3.11B	- added the ability to be run in the background (STEVIE will
 *			  attempt to use the current window, but if it can't then it
 *			  will open its own window). Fixed some other miscellaneous
 *			  bugs (some to do with re-sizing the screen, one to do with
 *			  undo'ing changes on lines that start with whitespace).
 *			- GRWalter (Fred)
 *
 * 3.11C	- fixed a bug that was causing the entire screen to be refreshed
 *			  at the wrong times sometimes. Input mode was sped up as well
 *			  as a bug involving lines that wrapped was fixed. Changed :ta
 *			  a bit. Fixed bug triggered when files are > 6000 lines.
 *			- GRWalter (Fred)
 *
 * 3.31A	- Tony Andrews put out a divergent version of STEVIE (version 3.31).
 *			  I moved the important stuff over into my version.
 *
 *			  Here is a list of what was moved over :
 *
 *************************************************************************
 * Revision 3.29  88/06/26	14:53:19  tony
 * Added support for a simple form of the "global" command. It supports
 * commands of the form "g/pat/d" or "g/pat/p", to delete or print lines
 * that match the given pattern. A range spec may be used to limit the
 * lines to be searched.
 *
 * Revision 3.28  88/06/25	21:44:22  tony
 * Fixed a problem in the processing of colon commands that caused
 * substitutions of patterns containing white space to fail.
 *
 * Revision 3.26  88/06/10	13:44:06  tony
 * Fixed a bug involving writing out files with long pathnames. A small
 * fixed size buffer was being used. The space for the backup file name
 * is now allocated dynamically.
 *
 * Revision 1.12  88/05/03	14:39:52  tony
 * Also merged in support for DOS.
 *
 * Revision 1.11  88/05/02	21:38:21  tony
 * The code that reads files now handles boundary/error conditions much
 * better, and generates status/error messages that are compatible with
 * the real vi. Also fixed a bug in repeated reverse searches that got
 * inserted in the recent changes to search.c.
 *
 * Revision 1.10  88/05/02	07:35:41  tony
 * Fixed a bug in the routine plines() that was introduced during changes
 * made for the last version.
 *
 * Revision 1.9  88/05/01  20:10:19  tony
 * Fixed some problems with auto-indent, and added support for the "number"
 * parameter.
 *
 * Revision 1.8  88/04/30  20:00:49  tony
 * Added support for the auto-indent feature.
 *
 * Revision 1.6  88/04/28  08:19:35  tony
 * Modified Henry Spencer's regular expression library to support new
 * features that couldn't be done easily with the existing interface.
 * This code is now a direct part of the editor source code. The editor
 * now supports the "ignorecase" parameter, and multiple substitutions
 * per line, as in "1,$s/foo/bar/g".
 *
 * Revision 1.5  88/04/24  21:38:00  tony
 * Added preliminary support for the substitute command. Full range specs.
 * are supported, but only a single substitution is allowed on each line.
 *
 * Revision 1.4  88/04/23  20:41:01  tony
 * Worked around a problem with adding lines to the end of the buffer when
 * the cursor is at the bottom of the screen (in misccmds.c). Also fixed a
 * bug that caused reverse searches from the start of the file to bomb.
 *
 * Revision 1.3  88/03/24  08:57:00  tony
 * Fixed a bug in cmdline() that had to do with backspacing out of colon
 * commands or searches. Searches were okay, but colon commands backed out
 * one backspace too early.
 *
 * Revision 1.2  88/03/21  16:47:55  tony
 * Fixed a bug in renum() causing problems with large files (>6400 lines).
 *************************************************************************
 *			- GRWalter (Fred)
 *
 * 3.32A	- added the :[range]d command. Played with 'p' and 'P'.
 *			  Added undo capability to :s and :g//d commands.
 *			  Added '%' as a line range specifier (entire file).
 *			  Fixed search so that tags file from real ctags could be used.
 *			  Fixed undo after delete everything operation.
 *			  Made prt_line work in nu mode (so :g//p works right).
 *			  Fixed ai mode (when there was text after the cursor it didn't ai).
 *			  Fixed 'J' (didn't handle next line just having whitespace).
 *			  Fixed :[range] so it behaves like the real vi (goes to highest
 *			  line number in the given range).
 *			  Made it so that the cursor is on the last char inserted instead
 *			  the one right after when there is exactly 1 char right after.
 *			  Made change operator work properly when it ended on the
 *			  end of the line.
 *			- GRWalter (Fred)
 *
 * 3.33A	- no longer updateNextscreen when putting or undoing or
 *			  redoing until I am done. 'p', 'u' and '.' thus sped up.
 *			- no longer recalculate line lengths when cursupdate() called,
 *			  which speeds up lots'a things (like on-screen cursoring).
 *			- avoid redrawing (in updateNextscreen) as much as possible, which
 *			  speeds up (among other things) cursoring (off screen), put, undo,
 *			  redo, etc.
 *			- GRWalter (Fred)
 *
 * 3.34A	- rewrote updateNextscreen and updatenextline so they won't do as
 *			  much work. Sped up cursoring off-screen. Fixed bug in cursupdate
 *			  (could index through NULL pointer).
 *			- GRWalter (Fred)
 *
 * 3.35A	- Compiles with Lattice 5.0 now - needed miscellaneous changes.
 *			- Environment variables (EXINIT) finally used.
 *			- Stevie is now compiled so it's residentable.
 *			- Fixed bug in insstr() (caused problems with redo of inserts).
 *			- Fixed buffer overflow/corrupt messages.
 *			- Tweaked updateNextscreen routine some more.
 *			- Added __DATE__ and __TIME__ of compilation to help screen.
 *			- GRWalter (Fred)
 *
 * 3.35F	- Removed Nextscreen, Realscreen, updateRealscreen and
 *			- redrawline. Re-wrote all screen I/O to directly update the
 *			- screen when necessary, with a resulting memory and speed savings.
 *			- Fixed bug in amiga.c that caused Stevie to (occasionlly) lock-up
 *			  upon startup until the window was resized.
 *			- Removed T_SC and T_RC from term.h file (no longer used).
 *			- Miscellaneous little changes.
 *			- GRWalter (Fred)
 *
 * 3.35H	- Fixed all routines to check memory allocation return status
 *			  (except for strsave() function).
 *			- Fixed bug with macro outone() in amiga.c and bsd.c.
 *			- GRWalter (Fred)
 *
 * 3.6		- Big jump in revision number to do last version on a Fish Disk
 *			  being descibed as version 3.5 (thus people will know this is
 *			  a later version).
 *			- Miscellaneous little changes.
 *			- GRWalter (Fred)
 *
 * VIM 1.0	- Changed so many things that I felt that a new name was required
 *			  (I didn't like the name Stevie that much: I don't have an ST).
 *			- VIM stands for "Vi IMitation".
 *			- New storage structure, MULTI-LEVEL undo and redo,
 *			  improved screen output, removed an awful number of bugs,
 *			  removed fixed size buffers, added counts to a lot of commands,
 *			  added new commands, added new options, added 'smart indent',
 *			  added recording mode, added script files, moved help to a file,
 *			  etc. etc. etc.
 *			- Compiles under Manx/Aztec C 5.0. You can use "rez" to make VIM
 *			  resident.
 *			- Bram Moolenaar (Mool)
 *
 * VIM 1.09 - spaces can be used in tags file instead of tabs (compatible with
 *			  Manx ctags).
 *
 * VIM 1.10 - Csh not required anymore for CTRL-D. Search options /e and /s added.
 *			  Shell option implemented. BS in replace mode does not delete
 *			  character. Backspace, wrapmargin and tags options added.
 *			  Added support for Manx's QuickFix mode (just like "Z").
 *			  The ENV: environment variables instead of the Old Manx environment
 *			  variables are now used, because Vim was compiled with version 5.0d
 *			  of the compiler. "mool" library not used anymore. Added index to
 *			  help screens.
 *
 * VIM 1.11 - removed bug that caused :e of same file, but with name in upper case,
 *			  to re-edit that file.
 *
 * VIM 1.12 - The second character of several commands (e.g. 'r', 't', 'm') not
 *			  :mapped anymore (UNIX vi does it like this, don't know why); Some
 *			  operators did not work when doing a 'l' on the last character in
 *			  a line (e.g. 'yl'); Added :mapping when executing registers;
 *			  Removed vi incompatibility from 't' and 'T' commands; :mapping! also
 *			  works for command line editing; Changed a few details to have Vim
 *			  run the macros for solving a maze and Towers of Hanoi! It now also
 *			  runs the Turing machine macros!
 *
 * VIM 1.13 - Removed a bug for !! on empty line. "$" no longer puts cursor at
 *			  the end of the line when combined with an operator. Added
 *			  automatic creation of a script file for recovery after a crash.
 *			  Added "-r" option. Solved bug for not detecting end of script file.
 *			  ".bak" is now appended, thus "main.c" and "main.h" will have
 *			  separate backup files.
 *
 * VIM 1.14 - Removed a few minor bugs. Added "-n" option to skip autoscript.
 *			  Made options more Vi compatible. Improved ^C handling. On serious
 *			  errors typahead and scripts are discarded. 'U' is now correctly
 *			  undone with 'u'. Fixed showmatch() handling of 'x' and '\x'.
 *			  Solved window size dependency for scripts by adding ":winsize"
 *			  commands to scripts. Released on Fish disk 591.
 *
 * VIM 1.15 - No extra return in recording mode (MCHAR instead of MLINE buffer).
 *			  plural() argument is now a long. Search patterns shared between
 *			  :g, :s and /. After recovery a message is given. Overflow of mapbuf
 *			  is detected. Line number possible with :read. Error message when
 *			  characters follow a '$' in a search pattern. Cause for crash
 *			  removed: ":s/pat/repl/g" allocated not enough memory. Option
 *			  "directory" added. Option "expandtab" added. Solved showmode non-
 *			  functioning. Solved bug with window resizing. Removed some *NULL
 *			  references. CTRL-], * and # commands now skips non-identifier
 *			  characters. Added tag list, CTRL-T, :pop and :tags commands.
 *			  Added jump list, CTRL-O and CTRL-I commands. Added "shiftround"
 *			  option. Applied AUX and Lattice mods from Juergen Weigert.
 *			  Finally made linenr_t a long, files can be > 65000 lines!
 *			  :win command could be written to script file halfway a command.
 *			  Option shelltype added. With ^V no mapping anymore.
 *			  Added :move, :copy, :t, :mark and :k. Improved Ex address parsing.
 *			  Many delimiters possible with :s.
 *
 * VIM 1.16 - Solved bug with zero line number in Ex range. Added file-number to
 *			  jump list. Solved bug when scrolling downwards. Made tagstack vi
 *			  compatible. Cmdline editing: CTRL-U instead of '@'. Made Vim DICE
 *			  compatible. Included regexp improvements from Olaf Seibert,
 *			  mapstring() removed. Removed bug with CTRL-U in insert mode.
 *			  Count allowed before ". Added uppercase (file) marks. Added
 *			  :marks command. Added joinspaces option. Added :jumps command. Made
 *			  jumplist compatible with tag list. Added count to :next and :Next.
 *
 * VIM 1.17 - Removed '"' for Ex register name. Repaired stupid bug in tag code.
 *			  Now compiled with Aztec 5.2a. Changed Arpbase.h for use with 2.04
 *			  includes. Added repdel option. Improved :set listing. Added count
 *			  to 'u' and CTRL-R commands. "vi:" and "ex:" in modelines must now
 *			  be preceded with a blank. Option "+command" for command line and
 *		      :edit command added.
 */

char		   *Version = "VIM v1.17";
char		   *longVersion = "Vi IMitation v1.17 (" __DATE__ " " __TIME__ ") by Bram Moolenaar";
