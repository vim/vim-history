This builds a one-click install for Vim for Win32 using the Nullsoft
Installation System (NSIS), available at http://www.nullsoft.com/free/nsis/

To build the installable .exe:

1.  Unpack the runtime and source archives for the PC
	You can generate these from the Unix sources and runtime plus the
	extra archive.

2.  Go to the src directory and build:
	gvim.exe,		(the OLE version)
	vimrun.exe,
	install.exe,
	uninstal.exe,
	xxd.exe,

3.  Go to the GvimExt directory and build gvimext.dll.

4.  Go to the VisVim directory and build VisVim.dll.

5.  Go to the OleVim directory and build OpenWithVim.exe and SendToVim.exe.


Install NSIS if you didn't do that already.

To build then, enter:

> makensis gvim.nsi
