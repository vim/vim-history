README_dos.txt for version 6.0q of Vim: Vi IMproved.

This file explains the installation of Vim on MS-DOS and MS-Windows systems.
See "README.txt" for general information about Vim.


These are the normal steps to install Vim:

1. Go to the directory where you want to put the Vim files.  Examples:
	cd C:\
	cd D:\editors
   If you already have a "vim" directory, go to the directory in which it is
   located.  Check the $VIM setting to see where it points to:
   	set VIM
   For example, if you have
	C:\vim\vim54
   do
	cd C:\
   Binary and runtime Vim archives are normally unpacked in the same location,
   on top of eachother.

2. Unpack the zip archives.  This will create a new directory "vim\vim60",
   in which all the distributed Vim files are placed.  Since the directory
   name includes the version number, it is unlikely that you overwrite
   existing files.
   Examples:
	pkunzip -d gvim60.zip
	unzip vim60w32.zip

   You need to unpack the runtime archive and at least one of the binary
   archives.  When using more than one binary version, be careful not to
   overwrite one version with the other, the names of the executables are the
   same.

   After you unpacked the files, you can still move the whole directory tree
   to another location.  That is where they will stay, the install program
   won't move or copy the runtime files.

   Only for the 32 bit DOS version on MS-DOS without DPMI support (trying to
   run install.exe will produce an error message): Unpack the CSDPMI4B.ZIP
   archive and follow the instructions in the documentation.

3. Change to the new directory:
	cd vim\vim60
   Run the "install.exe" program.  It will ask you a number of questions about
   how you would like to have your Vim setup.  You can have it append a few
   lines to your autoexec.bat, and write a "_vimrc" file with your preferences
   in the parent directory.  It can also install an "Edit with Vim" entry in
   the Windows Explorer popup menu.

   If you want to add Vim to the search path manually: The simplest is to add
   a line to your autoexec.bat.  Examples:
	set path=%path%;C:\vim\vim60
	set path=%path%;D:\editors\vim\vim60

That's it!

NOTE: The install.exe program appends to your autoexec.bat file.  If yours
contains sections, this will probably not work correctly.  You will have to
adjust the autoexec.bat file by hand afterwards.


Remarks:

- Make sure the Vim executable is in your search path.  Either copy the Vim
  executable to a directory that is in your search path, or (preferred) modify
  the search path to include the directory where the Vim executable is.
  Normally the install.exe program will do this for you.

- If Vim can't find the runtime files, ":help" won't work and the GUI version
  won't show a menubar.  Then you need to set the $VIM environment variable to
  point to the top directory of your Vim files.  Example:
    set VIM=C:\editors\vim
  Vim version 6.0 will look for your vimrc file in $VIM, and for the runtime
  files in $VIM/vim60.  See ":help $VIM" for more information.

- To avoid confusion between distributed files of different versions and your
  own modified vim scripts, it is recommended to use this directory layout:
  ("C:\vim" is used here as the root, replace with the path you use)
  Your own files:
	C:\vim\_vimrc			Your personal vimrc.
	C:\vim\_viminfo			Dynamic info for 'viminfo'.
	C:\vim\...			Other files you made.
  Distributed files:
	C:\vim\vim60\vim.exe		The Vim version 6.0 executable.
	C:\vim\vim60\doc\*.txt		The version 6.0 documentation files.
	C:\vim\vim60\bugreport.vim	A Vim version 6.0 script.
	C:\vim\vim60\...		Other version 6.0 distributed files.
  In this case the $VIM environment variable would be set like this:
	set VIM=C:\vim
  Then $VIMRUNTIME will automatically be set to "$VIM\vim60".  Don't add
  "vim60" to $VIM, that won't work.

- You can put your Vim executable anywhere else.  If the executable is not
  with the other Vim files, you should set $VIM. The simplest is to add a line
  to your autoexec.bat.  Examples:
	set VIM=c:\vim
	set VIM=d:\editors\vim
  The install.exe program can move the Vim executable or set $VIM for you.

- If you have told the "install.exe" program to add the "Edit with Vim" menu
  entry, you can remove it by running the "uninstal.exe".  See
  ":help win32-popup-menu".

- In Windows 95/98/NT you can create a shortcut to Vim.  This works for all
  DOS and Win32 console versions.  This gives you the opportunity to set
  defaults for the Console where Vim runs in.

  1. On the desktop, click right to get a menu.  Select New/Shortcut.
  2. In the dialog, enter Command line: "C:\command.com".  Click "Next".
  3. Enter any name.  Click "Finish".
     The new shortcut will appear on the desktop.
  4. With the mouse pointer on the new shortcut, click right to get a menu.
     Select Properties.
  5. In the Program tab, change the "Cmdline" to add "/c" and the name of the
     Vim executable.  Examples:
	C:\command.com /c C:\vim\vim60\vim.exe
	C:\command.com /c D:\editors\vim\vim60\vim.exe
  6. Select the font, window size, etc. that you like.  If this isn't
     possible, select "Advanced" in the Program tab, and deselect "MS-DOS
     mode".
  7. Click OK.

  For gvim, you can use a normal shortcut on the desktop, and set the size of
  the Window in your $VIM/_gvimrc:
	set lines=30 columns=90


For further information, type one of these inside Vim:
	:help dos
	:help msdos
	:help win32
