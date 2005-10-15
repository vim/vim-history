README_os2.txt for version 6.4 of Vim: Vi IMproved.

This file explains the installation of Vim on OS/2 systems.
See "README.txt" for general information about Vim.


NOTE: You will need two archives:
  vim64rt.zip	contains the runtime files (same as for the PC version)
  vim64os2.zip	contains the OS/2 executables

1. Go to the directory where you want to put the Vim files.  Examples:
	cd C:\
	cd D:\editors

2. Unpack the zip archives.  This will create a new directory "vim/vim64",
   in which all the distributed Vim files are placed.  Since the directory
   name includes the version number, it is unlikely that you overwrite
   existing files.
   Examples:
	pkunzip -d vim64os2.zip
	unzip vim64os2.zip

   After you unpacked the files, you can still move the whole directory tree
   to another location.

3. Add the directory where vim.exe is to your path.  The simplest is to add a
   line to your autoexec.bat.  Examples:
	set path=%path%;C:\vim\vim64
	set path=%path%;D:\editors\vim\vim64

That's it!


Extra remarks:

- To avoid confusion between distributed files of different versions and your
  own modified vim scripts, it is recommended to use this directory layout:
  ("C:\vim" is used here as the root, replace with the path you use)
  Your own files:
	C:\vim\_vimrc			Your personal vimrc.
	C:\vim\_viminfo			Dynamic info for 'viminfo'.
	C:\vim\...			Other files you made.
  Distributed files:
	C:\vim\vim64\vim.exe		The Vim version 6.4 executable.
	C:\vim\vim64\doc\*.txt		The version 6.4 documentation files.
	C:\vim\vim64\bugreport.vim	A Vim version 6.4 script.
	C:\vim\vim64\...		Other version 6.4 distributed files.
  In this case the $VIM environment variable would be set like this:
	set VIM=C:\vim

- You can put your Vim executable anywhere else.  If the executable is not
  with the other distributed Vim files, you should set $VIM.  The simplest is
  to add a line to your autoexec.bat.  Examples:
	set VIM=c:\vim
	set VIM=d:\editors\vim

For further information, type this inside Vim:
	:help os2
