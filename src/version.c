/* vi:ts=4:sw=4:tw=78
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 Started with Stevie version 3.6 (Fish disk 217) - GRWalter (Fred)

 VIM 1.0	- Changed so many things that I felt that a new name was required
			(I didn't like the name Stevie that much: I don't have an ST).
			- VIM stands for "Vi IMitation".
			- New storage structure, MULTI-LEVEL undo and redo,
			improved screen output, removed an awful number of bugs,
			removed fixed size buffers, added counts to a lot of commands,
			added new commands, added new options, added 'smart indent',
			added recording mode, added script files, moved help to a file,
			etc. etc. etc.
			- Compiles under Manx/Aztec C 5.0. You can use "rez" to make VIM
			resident.
			- Bram Moolenaar (Mool)

 VIM 1.09 - spaces can be used in tags file instead of tabs (compatible with
			Manx ctags).

 VIM 1.10 - Csh not required anymore for CTRL-D. Search options /e and /s added.
			Shell option implemented. BS in replace mode does not delete
			character. Backspace, wrapmargin and tags options added.
			Added support for Manx's QuickFix mode (just like "Z").
			The ENV: environment variables instead of the Old Manx environment
			variables are now used, because Vim was compiled with version 5.0d
			of the compiler. "mool" library not used anymore. Added index to
			help screens.

 VIM 1.11 - removed bug that caused :e of same file, but with name in upper case,
			to re-edit that file.

 VIM 1.12 - The second character of several commands (e.g. 'r', 't', 'm') not
			:mapped anymore (UNIX vi does it like this, don't know why); Some
			operators did not work when doing a 'l' on the last character in
			a line (e.g. 'yl'); Added :mapping when executing registers;
			Removed vi incompatibility from 't' and 'T' commands; :mapping! also
			works for command line editing; Changed a few details to have Vim
			run the macros for solving a maze and Towers of Hanoi! It now also
			runs the Turing machine macros!

 VIM 1.13 - Removed a bug for !! on empty line. "$" no longer puts cursor at
			the end of the line when combined with an operator. Added
			automatic creation of a script file for recovery after a crash.
			Added "-r" option. Solved bug for not detecting end of script file.
			".bak" is now appended, thus "main.c" and "main.h" will have
			separate backup files.

 VIM 1.14 - Removed a few minor bugs. Added "-n" option to skip autoscript.
			Made options more Vi compatible. Improved ^C handling. On serious
			errors typahead and scripts are discarded. 'U' is now correctly
			undone with 'u'. Fixed showmatch() handling of 'x' and '\x'.
			Solved window size dependency for scripts by adding ":winsize"
			commands to scripts. This version released on Fish disk 591.

 VIM 1.15 - No extra return in recording mode (MCHAR instead of MLINE buffer).
			plural() argument is now a long. Search patterns shared between
			:g, :s and /. After recovery a message is given. Overflow of mapbuf
			is detected. Line number possible with :read. Error message when
			characters follow a '$' in a search pattern. Cause for crash
			removed: ":s/pat/repl/g" allocated not enough memory. Option
			"directory" added. Option "expandtab" added. Solved showmode non-
			functioning. Solved bug with window resizing. Removed some *NULL
			references. CTRL-], * and # commands now skips non-identifier
			characters. Added tag list, CTRL-T, :pop and :tags commands.
			Added jump list, CTRL-O and CTRL-I commands. Added "shiftround"
			option. Applied AUX and Lattice mods from Juergen Weigert.
			Finally made linenr_t a long, files can be > 65000 lines!
			:win command could be written to script file halfway a command.
			Option shelltype added. With ^V no mapping anymore.
			Added :move, :copy, :t, :mark and :k. Improved Ex address parsing.
			Many delimiters possible with :s.

 VIM 1.16 - Solved bug with zero line number in Ex range. Added file-number to
			jump list. Solved bug when scrolling downwards. Made tagstack vi
			compatible. Cmdline editing: CTRL-U instead of '@'. Made Vim DICE
			compatible. Included regexp improvements from Olaf Seibert,
			mapstring() removed. Removed bug with CTRL-U in insert mode.
			Count allowed before ". Added uppercase (file) marks. Added
			:marks command. Added joinspaces option. Added :jumps command. Made
			jumplist compatible with tag list. Added count to :next and :Next.

 VIM 1.17 - Removed '"' for Ex register name. Repaired stupid bug in tag code.
			Now compiled with Aztec 5.2a. Changed Arpbase.h for use with 2.04
			includes. Added repdel option. Improved :set listing. Added count
			to 'u' and CTRL-R commands. "vi:" and "ex:" in modelines must now
			be preceded with a blank. Option "+command" for command line and
		    :edit command added.

 VIM 1.18 - Screen was not updated when all lines deleted. Readfile() now
			puts cursor on first new line. Catch strange disk label.
			Endless "undo line missing" loop removed. With empty file 'O' would
			cause this. Added window size reset in windexit(). Flush .vim file
			only when buffer has been changed. Added the nice things from
			Elvis 1.5: Added "equalprg" and "ruler" option. Added quoting.
			Added third meaning to 'backspace' option: backspacing over start
			of insert. Added "-c {command}" command line option. Made generating
			of prototypes automatically. Added insert mode command CTRL-O and
			arrow keys. CTRL-T/CTRL-D now always insert/delete indent. When
			starting an edit on specified lnum there was redraw of first lines.
			Added 'inputmode' option. Added CTRL-A and CTRL-S commands. '`' is
			now exclusive (as it should be). Added digraphs as an option.
			Changed implementation of parameters. Added :wnext command.
			Added ':@r' command. Changed handling of CTRL-V in command line.
			Block macros now work. Added keyword lookup command 'K'. Added
			CTRL-N and CTRL-P to command line editing. For DOS 2.0x the Flush
			function is used for the autoscript file; this should solve the
			lockup bug. Added wait_return to msg() for long messages.

 VIM 1.19 - Changes from Juergen Weigert:
			Terminal type no longer restricted to machine console. New
			option -T terminal. New set option "term". Builtin termcap 
			entries for "amiga", "ansi", "atari", "nansi", "pcterm". 
			Ported to MSDOS. New set option "textmode" ("tx") to control 
			CR-LF translation. CTRL-U and CTRL-D scroll half a screen full,
			rather than 12 lines. New set option "writebackup" ("wb") to 
			disable even the 'backup when writing' feature.
			Ported to SunOS. Full termcap support. Does resize on SIGWINCH.

			Made storage.c portable. Added reading of ".vimrc". Added
			'helpfile' option. With quoting the first char of an empty line
			is inverted. Optimized screen updating a bit. Startup code 
			looks for VIMINIT variable and .vimrc file first. Added option
			helpfile. Solved bug of inserting deletes: redefined ISSPECIAL.
			Changed inchar() to use key codes from termcap. Added parameters
			for terminal codes. Replaced aux device handling by amiga window
			handling. Added optional termcap code. Added 'V', format
			operator.

 VIM 1.20 - wait_return only ignores CR, LF and space. 'V' also works for
            single line. No redrawing while formatting text. Added CTRL-Z.
			Added usage of termcap "ks" and "ke". Fixed showmatch().
			Added timeout option. Added newfile argument to readfile().

 VIM 1.21 - Added block mode. Added 'o' command for quoting. Added :set inv.
			Added pos2ptr(). Added repeating and '$' to Quoting.

 VIM 1.22 - Fixed a bug in doput() with count > 1.
			Port to linux by Juergen Weigert included.
			More unix semantics in writeit(), forceit flag ignores errors while 
			preparing backup file. For UNIX, backup is now copied, not moved.
			When the current directory is not writable, vim now tries a backup
			in the directory given with the backupdir option. For UNIX, raw mode
			has now ICRNL turned off, that allowes ^V^M. Makefiles for BSD,
			SYSV, and linux unified in makefile.unix. For MSDOS
			mch_get_winsize() implemented. Reimplemented builtin termcaps in
			term.c and term.h. set_term() now handles all cases. Even builtins
			when TERMCAP is defined. Show "..." while doing filename completion.

 VIM 1.23 -	Improved MSDOS version: Added function and cursor keys to builtin 
			pcterm. Replaced setmode by settmode, delay by vim_delay and 
			delline by dellines to avoid name conflicts. Made F1 help key.
			Renamed makecmdtab to mkcmdtab and cmdsearch to csearch for 
			8 char name limit. Wildcard expansion adds *.* to names without a 
			dot. Added shell execution.
			For unix: writeit() overwrites readonly files when forced write,
			more safety checks. Termcap buffer for linux now 2048 bytes.
			Expandone() no longer appends "*" to file name. Added "graphic"
			option. Added ':' command to quoting.
			
 VIM 1.24	Adjusted number of spaces inserted by dojoin(). MSDOS version uses 
			searchpath() to find helpfile. Fixed a few small problems. Fixed 
			nasty bug in getperm() for SAS 6.0. Removed second argument from 
			wait_return(). Script files accessed in binary mode with MSDOS. 
			Added 'u' and 'U' commands to quoting (make upper or lower case). 
			Added "CTRL-V [0-9]*" to enter any byte value. Fixed doput().
			Dodis() displays register 0. Added CTRL-B to insert mode. Attempt 
			to fix the lockup bug by adding Delay() to startscript(). -v 
			option now implies -n option. doformat() added to improve 'V' 
			command. Replace bool_t with int. Fixed handling of \& and ~ in
			regsub(). Added interrupt handling in msdos.c for ctrl-break and
			critical errors. Added scrolljump option. Added :stop. Added -d
			argument. Fixed bug in quickfix startup from cli. Fixed enforcer
			hit with aux:. Added CTRL-C handling to unix.c. Fixed "O<BS><CR>" 
			bug with autoindent. Worked around :cq not working by adding a 
			printf()!? Added default mapping for MSDOS PageUp etc. Fixed 
			cursor position after 'Y'. Added shift-cursor commands. Changed 
			ExpandFile() to keep names with errors. Added CLEAR and CURSUPD 
			arguments to updateScreen(). Fixed CTRL-@ after a change command.
			modname() changes '.' into '_'. Added emptyrows to screen.c. 
			Fixed redo of search with offset. Added count to 'z' command. 
			Made :so! work with :global. Added writing of cursor postition to 
			startscript(). Minimized terminal requirements. Fixed problem 
			with line in tags file with mixed spaces and tabs. Fixed problem 
			with pattern "\\" in :s and :g. This version posted on Usenet.

 VIM 1.25	Improved error messages for :set. Open helpfile in binary mode 
			for MSDOS. Fixed ignorecase for Unix in cstrncmp(). Fixed read 
			from NULL with :tags after vim -t. Repaired 'z' command. Changed 
			outnum() for >32767. In msdos.c flushbuf did write(1, .. instead 
			of write(0, .. Added secure to fix security. Fixed pointer 
			use after free() bug in regsub() (made :s fail under MSDOS). 
			Added nofreeNULL(), needed for some UNIXes. Improved window 
			resizing for Unix. Fixed messages for report == 0. Added 
			bsdmemset(). Changed a few small things for portability. Added 
			:list. Made '0' and '^' exclusive. Fixed regexp for /pattern* 
			(did /(pattern)* instead of /pattern(n)*). Added "']" and "'[". 
			Changed Delay(2L) into Delay(10L). Made 'timeout' option 
			vi-compatible, added 'ttimeout' option. Changed TIOCSETP to 
			TIOCSETN in unix.c. Added "ti" and "te" termcap entries, makes 
			sun cmdtool work. Added stop- and starttermcap(). Use cooked 
			output for listings on Amiga only. Added "starting" flag, no ~s 
			anymore with every startup. Modname made portable; Added 
			'shortname' option, Fixed problems with .vim file on messydos. 
			Global .exrc/.vimrc for Unix added. Added patches for SCO Xenix. 
			Add :w argument to list of alternate file names. Applied a few 
			changes for HPUX. Added Flock in writeit() for safety. Command 
			":'a,'bm." moved to 'b instead of current line. Argument in 
			'shell' option allowed. Re-implemented :copy and :move. Fixed 
			BS-CR-BS on empty line bug in edit.c. -t option was ignored if 
			there is a file ".vim". Changed amiga.c to work without 
			arp.library for dos 2.0. Fixed "\$" and "\^" in regexp. Fixed 
			pipe in filter command. Fixed CTRL-U and CTRL-D. With '}' indent 
			in front of the cursor is included in the operated text. Fixed 
			tag with '[' in search pattern. Added CTRL-V to 'r'. Fixed "tc" 
			entry in termlib.c. term_console now default off. Added :noremap 
			and ^V in :map argument. Replaced CTRL by Ctrl because some 
			unixes have this already. Fixed "Empty file" message disappearing 
			when there is no .exrc file. Added CTRL-K for entering digraphs. 
			Removed escape codes from vim.hlp, added handling of inversion to 
			help().

 VIM 1.26	For Unix: Removed global .exrc; renamed global .vimrc to vimrc.
 			Moved names of *rc and help files to makefile. Added various 
			little changes for different kinds of Unix. Changed CR-LF 
			handling in dosource() for MSDOS. Added :mkvimrc. Fixed 
			WildExpand in unix.c for empty file. Fixed incompatibility with 
			msdos share program (removed setperm(fname, 0) from fileio.c).
			Added ":set compatible". Fixed 'history=0'.

 VIM 1.27	Added USE_LOCALE. Changed swapchar() to use toupper() and 
			tolower(). Changed init order: .vimrc before EXINIT. Half-fixed 
			lines that do not fit on screen. A few minor bug fixes. Fixed 
			typehead bug in Read() in unix.c. Added :number. Reset IXON flag 
			in unix.c for CTRL-Q. In tags file any Ex command can be used. Ex 
			search command accepts same flags as normal search command. Fixed 
			'?' in tag search pattern. 'New file' message was wrong when 'bk' 
			and 'wb' options were both off.

 Vim 1.29 to 1.31 and Vim 2.0	See ../readme2.0.

 Vim 2.0	When reading and writing files and in some other cases use short
 			filename if ":cd" not used. Fixes problem with networks. Deleted
			"#include <ctype.h>" from regexp.c. ":v" without argument was not
			handled correctly in doglob(). Check for tail recursion removed
			again, because it forbids ":map! foo ^]foo", which is OK. Removed
			redraw on exit for msdos. Fixed return value for FullName in
			unix.c. Call_shell does not always use cooked mode, fixes problem
			with typing CR while doing filename completion in unix. "r<TAB>"
			now done by edit() to make expandtab works. Implemented FullName
			for msdos. Implemented the drive specifier for the :cd command for
			MSDOS. Added CTRL-B and CTRL-E to command line editing. Del key
			for msdos not mapped to "x" in command mode, could not delete last
			char of count. Fixed screen being messed up with long commands
			when 'sc' is set. Fixed use of CR-LF in tags file. Added check
			for abbreviation when typing ESC or CTRL-O in insert mode. Doing
			a ":w file" does overwrite when "file" is the current file. Unmap
			will check for 'to' string if there is no match with 'from'
			string; Fixes ":unab foo" after ":ab foo bar". Fixed problem in
			addstar() for msdos: Check for negative index. Added possibility
			to switch off undo ":set ul=-1". Allow parameters to be set to
			numbers >32000 for machines with 16 bit ints.

 Vim 2.1	Fixed problem in memmove for __sgi. Cursor up/down in insert mode
			keep the same column. Fixed ":cd" not working for unix. Added
			column number to :file. Made incrementing hex numbers with CTRL-A
			work on systems that don't accept 0x in scanf for "%x". Added
			check for negative line numbers in colon commands. Reset p_ro in
			readfile() when starting to edit a new file. inchar() also
			calls flushbuf when waiting for a second, fixes screen not
			updated while waiting for a mapping to be completed. Fixed
			":/pat/s/x/X/"; 's' was seen as option to search command. Removed
			'n' offset to search, added tag_busy flag. ":>", ":<" and ":yank"
			leave cursor on the last line. Use iswhite() instead of isspace()
			for "^" and set_indent(). Allow pipe in ":r !cmd" and ":w !cmd".
			Fixed insertchar() lookahead for digraph not working when using
			mapped characters. Improved error messages for file access. Fixed
			:unab and :unmap for rhs with spaces. Fixed not accepting
			ambiguous abbreviations. :args does not call wait_return() when
			message is short. "dTx" and "dFx" do not include cursor position.
			"d]]" also stops at '{'. ']' and '[' are not linewise. Added
			shellpipe option, fixes problem with :make when not using csh.
			Fixed crash for AmigaDOS 1.3 when giving more than one file on
			the command line. When doing :paste twice the option values are
			saved only once. Renamed u_char to char_u. Replaced almost all
			char by char_u, solves some problems with meta chars. Updated
			isidchar for meta chars. Adjusted check for '|' in ":e +cmd". Put
			commands cannot be redone with '.'. "\?" in expressions replaced
			by "\=", makes "?\?\?" work. Accept '\n' to separate Ex commands,
			just like '|'. With search also accept 'b' for begin, like
			's' for start. Number after search is line offset. ":>>" shifts
			two indents, ":>>>" three, etc. Visual shift accepts a count for
			number of indents. "cc<ESC>" when 'ai' is set, deletes the
			indent. ":r" reads current file. No wait_return while filtering
			and only once when reading/writing files. Fixed ":s/1/1^M/gc" on
			"111" and ":s/O/oO/gc" looping. Fixed trailing "\" in substitute
			string (produced garbage). Fixed yanking "^B" with block visual
			mode. In regexp: made '$' in front of '\|' also end-of-line. "ce"
			when on the end of a word changes until the end of the next word
			(but "cw" not). Added "+command" argument to ":next", ":prev",
			":rewind" and ":Next". Give better error message when "/" did not
			found pattern and 'wrapscan' off. Added message when search
			wraps. Made ":&" work like ":s//~/". Fixed cursor left of line
			with "i<ESC>" when 'number' set. Abbreviations are never
			recursive, makes ":ab f f-o" work. Consider mapped characters for
			terminal codes, makes "map <key-code1> <key-code2>" work. msdos:
			isdir() adjusted for trailing backslash. Added t_ms, t_so and
			t_se. Use t_ms in screenchar(). Use t_so for highlighting
			directories. Fixed call to lalloc() in addfile() in msdos.c,
			could only do 32 files. Fixed setting archive bit in setperm() in
			msdos.c. Added a few changes for USL in unix.c. Remember two
			search patterns: one for substitute and one for the rest. Global
			command aborts after first line with error message. Don't put
			:win commands in the commandline history, fixes problem with
			recovery. Don't redisplay the "INSERT" message when getting out
			of insert mode.

 Vim 2.2	Moved all objects files into "obj" directory. Fixed bug in
 			makescriptname: alloc 1 byte extra. Added multiple windows!
			Now also "%<", "#31<", alternate file name without extension.
			Added 'ttyfast' for fast (non-serial line) terminals. Fixed bug
			in doaddsub(): line truncated 30 chars after number. '#' is not
			the start of an ex comment. Fixed CTRL-O in insert mode moving
			cursor when it is after end of line. Added 'smarttab' option.
			Vertical movement in insert mode can put cursor one char after
			the line. With escape in insert mode and auto-indent the cursor
			is put in the correct column. Remember b_changed flag in undo
			buffers, reset it when all changes undone. Could get ":win"
			command in script when resizing while entering "23/". Fixed not
			inverting 0xa0 in visual mode when scrolling. Fixed bug for "?$"
			command. Removed ptr2nr(); marks remember line number instead of
			pointer; undo uses line count to compute ue_bot.

 Vim 2.3	Fixed keyword macros: ^B must be ^R. ":set all" and ":set
			termcap" do not look for other arguments. [count]% puts cursor on
			first non-blank. ":r" when no file name specified gave problems.
			Added memfile.c and memline.c. Deleted autoscript file. Renamed
			buffers.c to getchar.c. Created buffer.c. deleted script.c. Added
			"Permission denied" message to readfile(). Use 'terse' option for
			search messages. Moved what was remaining of storage.c to undo.c.
			Added preserve command. When ":ta" do tag in same file, start
			search in line 1; fixes problem with 'nows'. Moved a few functions
			from cmdline.c to cmdcmds.c to reduce the size of cmdline.c. Added
			':' register. Named marks are saved for undo/redo. Added :qall and
			:close. Added 'winheight' option. Added ":exit" as synonym for
			":xit". Don't set curswant in edit() when returning with
			restart-edit set, makes CTRL-O j work correctly. Fixed setting of
			pcmark for "(" and ")". Fixed setting pcmark with :e command.
			Added optional '>' to 'directory' and 'backupdir' options, use the
			directory always. Added ":swapname". After "dd" put cursor
			on first non-blank in line. For MSDOS ignore trailing CTRL-Z in
			files in textmode. Added count to :cn and :cp. :cl and :cf can be
			interrupted by CTRL-C. Default 'updatetime' is 10sec,
			'updatecount' is 1000. Fixed ":g/T/s//TEE/" giving "no previous
			substitute". Fixed ignoring 'ic' for "/[a-z]". Allow abbreviation
			of "#i" to "#include", made recognition of abbreviation
			vi-compatible. Added check for valid abbreviation. Fixed eating
			backslash with ":next" for MSDOS. Fixed "/[/]", char in range now
			not seen as end of regexp. Don't map CR when waiting for "hit
			return to continue". Use 'ignorecase' when searching for tag. Made
			"-c command" and "+command" options for Vim, makes it possible to
			do "vim "+set ic" -t FuncTion". Improved Usage message. Added
			window title for xterm. "z." and "z-" move cursor to start
			of line. "zz" does the same as "z.". Improved ":set" listing.
			CTRL-S handling not offed when 'ix' termcap flag set. Split long
			lines in readfile(). Added ":screenmode" command. Set Changed flag
			for :move and :copy. Made search to end of pattern "/pat/e"
			inclusive. When count given with undo, update screen only once.

 Vim 2.4

Included small patches for gcc on HPUX 9.

When end of Visual area is on an empty line, minlc is FALSE, makes
"d}P" and "v}dP" do the same.

Dont' complain about .swp file existing without a file name.

For xterm also set icon title.

Added "-- more --" message to listing of mappings.

Don't try to preserve if there is no swap file.

Fixed bug: Errors while reading file caused big problems.

Use CTRL-X instead of CTRL-S. Do not switch off xon/xoff in unix.c.

No "hit return to continue" while editing a command line
(e.g. when filename completion fails).

Fixed problem with redraw when filename completion fails under unix.

Fixed cursor positioning error when cursor is on first char of non-empty cmdline.

emsg() no longer aborts :global, this is vi compatible.

Don't reset msg_scrolled in UpdateScreen() when redrawing is disabled.

Moved #endif in param.c from below to above p_wh, fixes problem when
COMPATIBLE is defined.

Highlighting of directories for CTRL-D on command line no longer highlights
some spaces.

Fixed chmod for help file in makefile.unix.

Fixed truncated displaying of string options.

'o' and 'O' commands did not update marks for the line just below the new
line.

Added 'title' option. When off, window title is not set. Default is on.

Check_winsize makes Rows at least 2.

sc_col and ru_col cannot become negative anymore, fixes crash with ":win 1".

Accept 'q' with "-- more --" message. Interrupts current listing.

Included patches for SCO 3.2.

Moved column number in CTRL-G to the end and adjusted the number.

If CTRL-W CTRL-] is done on a tag that does not exist, the window is not
split.

If a tag is found, but the associated file does not exist, do not start
editing an empty file.

Scroll options adjusted after CTRL-W =.

Added some changes for USL and picky gcc.

Added "zz", "zt" and "zb", same as "z.", "zCR" and "z-" but without moving the
cursor to the start of the line.

":3|" prints line 3 instead of jumping to it.

When lines are deleted/inserted, adjust topline and cursor position of other
windows on same buffer to stay in the same place (if possible).

Added CTRL-W CTRL-P and CTRL-W p, jump to previous window.

Made 'winheight' option a "minimal window height" option, only set height if
current height is smaller.

Added 'patchmode' option.

Fixed a problem for 'r' with a count. Could corrupt the text in the buffer!

Print message about the file when starting to edit an existing buffer. Fixes
not giving a message when doing ":n" to file being edited in other window.

Don't do textwidth formatting when in replace mode and replacing an existing
character.

Fixed bug when in replace mode and beyond the end of the line.

Added ":sleep" command.

Update mf_neg_count in mf_free, fixes "line count wrong in block 1" errors.

Added 'endofline' 'eol' option. If not set and in binary mode, last line will
not get an end of line when writing.

Added 'bswrap' option: Backspace (CTRL-H) and space wrap to previous/next line
in command mode. Should have been added long ago!

Fixed fname freed twice in mf_open(). Fixes crash/hang when editing a file in
a readonly directory.

When deleting and a register is specified, also yank into buffer "1.

With change command put deleted text in "1, even with text within one line.

Added 'invertweird' option, for terminals that have a weird inversion method.

Status lines also redrawn after ":set". Needed when if 'ruler' option was changed.

A few patches, the file "winnt.c" and "makefile.nt" for windows NT.

Don't check for abbreviation when a (non-id) character is entered with CTRL-V.

Replace MAX_INT by INT_MAX, fixes problem with "$" not getting very far.

Fixed plines_win() and chartabsize() for lines that are almost 32768 characters.

Made "search hit BOT.." messages redisplay after screen redraw.

Changed "short_u" to "unsigned" in data block. Fixes problem with lines longer
than 64K characters on Unix. Disadvantage: Need 2 bytes extra for each line.

Made argument list global, not local to each window. ":quit" works when there
are more files to edit, except when there is only one window.

Added hidden buffers. 'hidden' option can be set to create a hidden buffer
instead of abandoning a buffer. Added ":bnext", ":bprev", ":bNext" and
"brewind" commands to go to other buffer. Added ":buffers" command to show all
buffers. Added ":bmod" command: go to next modified buffer. Added ":bdelete"
command: delete buffer from list.

Added ":snext", ":sNext", ":sprev" and ":srewind" commands, split window and
go to next/previous file in argument list.

Included code to save the xterm title. Need to define USE_X11, because
specific include file and library are needed.

Added 'icon' option (default off): when set xterm icon is set to file name.

Changed Visual into VIsual, because it is also used by X11.

Stop Visual mode before exiting a window.

Fixed not inverting one char with "1v" when cursor cannot be made invisible.

Don't reset yank buffer with "." command. Makes dwu"a. work.

When a window is split, redraw both windows with cursor in middle.

Added default digraph: <space> <char> is meta-char.

Improved check for readonly files, now "466" is not readonly for others.

MSDOS: Trailing CTRL-V in sourced ex file now handled correctly.

Fixed problem with join: Deleted leading spaces in the 2nd line when it shouldn't.

When no file name yet and giving a ":read fname" command, use that file name.

Fixed invering Visual part when 'nowrap' set and start is left of window.

Added 'gdefault' option: make /g default for :s command.

*/

char		   *Version = "VIM 2.4 beta";
#if !defined(__DATE__) || !defined(__TIME__)
char		   *longVersion = "Vi IMproved 2.4 beta by Bram Moolenaar (1994 July 13)";
#else
char		   *longVersion = "Vi IMproved 2.4 beta by Bram Moolenaar (1994 July 13, compiled " __DATE__ " " __TIME__ ")";
#endif
