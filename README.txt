README.txt for version 5.3 of Vim: Vi IMproved.


WHAT IS VIM

Vim is an almost compatible version of the UNIX editor Vi.  Many new features
have been added: multi level undo, syntax highlighting, command line history,
on-line help, filename completion, block operations, etc.  There is also a
Graphical User Interface (GUI) available.  See doc/vi_diff.txt.

This editor is very useful for editing programs and other plain ASCII files.
All commands are given with normal keyboard characters, so those who can type
with ten fingers can work very fast.  Additionally, function keys can be
defined by the user, and the mouse can be used.

Vim currently runs under Amiga DOS, MS-DOS, Windows NT, Windows 95, Atari
MiNT, Macintosh, VMS, RISC OS, OS/2 and almost all flavours of UNIX.
Porting to other systems should not be very difficult.


DISTRIBUTION

There are separate distributions for Unix, PC, Amiga and some other systems.
This README.txt file comes with the runtime archive.  It includes the
documentation, syntax files and other files that are used at runtime.  To run
Vim you must get either one of the binary archives or a source archive.
Which one you need depends on the system you want to run it on and whether you
want or must compile it yourself.  Check the ftp site for an overview of
currently available distributions.


DOCUMENTATION

The best is to use ":help" in Vim.  If you don't have an executable (yet),
read doc/help.txt.  It contains pointers to the other documentation files.

"tutor/README" is a one hour training course for beginners.


COPYING

Vim is Charityware.  You can use and copy it as much as you like, but you are
encouraged to make a donation to orphans in Uganda.  Please read the file
"doc/uganda.txt" for details.

If you include Vim on a CD-ROM, I would like to receive a copy.  Just so I
know which Vim distributions exists in the world (and to show off to my
friends :-)).

There are no restrictions on distributing an unmodified copy of Vim on a
CD-ROM or in any other way.  You are also allowed to include executables, made
from the unmodified Vim sources.

If you distribute a modified version of Vim, you must send the maintainer a
copy of the source code.  Or make it available to the maintainer through ftp;
let him know where It can be found.  If the number of changes is small (e.g.,
a modified Makefile) e-mailing the diffs will do.  The maintainer preserves
the right to include any changes in the official version of Vim.  It is not
allowed to distribute a modified version of Vim without making the source code
available to the maintainer.  The current maintainer is Bram Moolenaar
(Bram@vim.org).  If this changes, it will be announced in appropriate places
(most likely www.vim.org and comp.editors).
It is not allowed to remove these restrictions from the distribution of the
Vim sources.


COMPILING

All stuff for compiling Vim is in the "src" directory.  See src/INSTALL for
instructions.


INFORMATION

The latest news about Vim can be found on the Vim home page:
	<URL:http://www.vim.org/>

If you have problems, have a look at the Vim FAQ:
	<URL:http://www.vim.org/faq/>

Send bug reports to:
	Bram Moolenaar <Bram@vim.org>

There are three mailing lists for Vim:
<vim@vim.org>
	For discussions about using existing versions of Vim: Useful mappings,
	questions, answers, where to get a specific version, etc.
<vim-dev@vim.org>
	For discussions about changing Vim: New features, porting, beta-test
	versions, etc.
<vim-announce@vim.org>
	Announcements about new versions of Vim; also beta-test versions and
	ports to different systems.

See <URL:http://www.vim.org/mail.html>.

NOTE:
- You can only send messages to these lists if you have subscribed!
- You need to send the messages from the same location as where you subscribed
  from (to avoid spam mails).
- Maximum message size is 40000 characters.

If you want to join, send a message to
	<vim-help@vim.org>
Make sure that your "From:" address is correct.  Then the list server will
send you a help message.


MAIN AUTHOR

Send any other comments, patches, pizza and suggestions to:

	Bram Moolenaar		E-mail:	Bram@vim.org
	Clematisstraat 30
	5925 BE  Venlo		Fax: +31 77 3595473
	The Netherlands		Tel: +31 77 3872340
