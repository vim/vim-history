# Makefile for Vim on Unix and Unix-like systems	vim:ts=8:sw=8:tw=78
#
# This Makefile is loosely based on the GNU Makefile conventions found in
# standards.info.
#
# Compiling Vim, summary:
#
#	3. make
#	5. make install
#
# Compiling Vim, details:
#
# Edit this file for adjusting to your system. You should not need to edit any
# other file for machine specific things!
# The name of this file MUST be Makefile (note the uppercase 'M').
#
# 1. Edit this Makefile
#	The defaults for Vim should work on most machines, but you may want to
#	uncomment some lines or make other changes below to tune it to your
#	system, compiler or preferences.  Uncommenting means that the '#' in
#	the first column of a line is removed.
#	- If you want a version of Vim that is small and starts up quickly,
#	  you might want to disable the GUI, X11, Perl, Python and Tcl.
#	- Uncomment the line with --disable-gui if you have Motif, GTK and/or
#	  Athena but don't want to make gvim (the GUI version of Vim with nice
#	  menus and scrollbars, but makes Vim bigger and startup slower).
#	- Uncomment the line "CONF_OPT_X = --without-x" if you have X11 but
#	  want to disable using X11 libraries.	This speeds up starting Vim,
#	  but the window title will not be set and the X11 selection can not
#	  used.
#	- Uncomment one or more of these lines to include an interface;
#	  each makes Vim quite a bit bigger:
#		--enable-perlinterp	for Perl interpreter
#		--enable-pythoninterp	for Python interpreter
#		--enable-rubyinterp	for Ruby interpreter
#		--enable-tclinterp	for Tcl interpreter
#		--enable-cscope		for Cscope interface
#	- Uncomment one of the lines with --with-features= to enable a set of
#	  features (but not the interfaces just mentioned).
#	- Uncomment the line with --disable-gpm to disable gpm support
#	  even though you have gpm libraries and includes
#	- Uncomment one of the lines with CFLAGS and/or CC if you have
#	  something very special or want to tune the optimizer.
#	- Search for the name of your system to see if it needs anything
#	  special.
#	- A few versions of make use '.include "file"' instead of 'include
#	  file'.  Adjust the include line below if yours does.
#
# 2. Edit feature.h
#	Only if you do not agree with the default compile features, e.g.:
#	- you want Vim to be as vi compatible as it can be
#	- you want to use Emacs tags files
#	- you want right-to-left editing (Hebrew)
#	- you want 'langmap' support (Greek)
#	- you want to remove features to make Vim smaller
#
# 3. "make"
#	Will first run ./configure with the options in this file. Then it will
#	start make again on this Makefile to do the compiling. You can also do
#	this in two steps with:
#		make config
#		make
#	The configuration phase creates/overwrites auto/config.h and
#	auto/config.mk.
#	The configure script is created with autoconf.  It can detect
#	different features of your system and act accordingly.  However, it is
#	not correct for all systems.  Check this:
#	- If you have X windows, but configure could not find it or reported
#	  another include/library directory then you wanted to use, you have
#	  to set CONF_OPT_X below.  You might also check the installation of
#	  xmkmf.
#	- If you have --enable-gui=motif and have Motif on your system, but
#	  configure reports "checking for location of gui... <not found>", you
#	  have to set GUI_INC_LOC and GUI_LIB_LOC below.
#	If you changed something, do this to run configure again:
#		make distclean
#		make config
#		make
#
#	- If you do not trust the automatic configuration code, then inspect
#	  auto/config.h and auto/config.mk, before starting the actual build
#	  phase. If possible edit this Makefile, rather than auto/config.mk --
#	  especially look at the definition of VIMLOC below. Note that the
#	  configure phase overwrites auto/config.mk and auto/config.h again.
#	- If you get error messages, find out what is wrong and try to correct
#	  it in this Makefile. You may need to do "make distclean" when you
#	  change anything that configure uses (e.g. switching from an old C
#	  compiler to an ANSI C compiler). Only when auto/configure does
#	  something wrong you may need to change one of the other files. If
#	  you find a clean way to fix the problem, consider sending a note to
#	  the author of autoconf (bug-gnu-utils@prep.ai.mit.edu) or Vim
#	  (Bram@vim.org). Don't bother to do that when you made a hack
#	  solution for a non-standard system.
#
# 4. "make test"
#	This is optional.  This will run Vim scripts on a number of test
#	files, and compare the produced output with the expected output.
#	If all is well, you will get the "ALL DONE" message in the end.  See
#	below (search for "/^test").
#
# 5. "make install"
#	If the new Vim seems to be working OK you can install it and the
#	documentation in the appropriate location. The default is
#	"/usr/local".  Change "prefix" below to change the location.
#	"auto/pathdef.c" will be compiled again after changing this to make
#	the executable know where the help files are located.
#	Note that any existing executable is removed or overwritten.  If you
#	want to keep it you will have to make a backup copy first.
#	The runtime files are in a different directory for each version.  You
#	might want to delete an older version.
#	If you don't want to install everything, there are other targets:
#		make installvim		only installs Vim, not the tools
#		make installvimbin	only installs the Vim executable
#		make installruntime	only installs the Vim help and
#							runtime files
#		make installlinks	only installs the Vim binary links
#		make installhelplinks	only installs the Vim manpage links
#		make installmacros	only installs the Vim macros
#		make installtutor	only installs the Vim tutor
#		make installtools	only installs xxd
#
# 6. Use Vim until a new version comes out.
#
# 7. "make uninstall_runtime"
#	Will remove the runtime files for the current version.	This is safe
#	to use while another version is being used, only version-specific
#	files will be deleted.
#	To remove the runtime files of another version:
#		make uninstall_runtime VIMRTDIR=/vim54
#	If you want to delete all installed files, use:
#		make uninstall
#	Note that this will delete files have the same name for any version,
#	thus you might need to do a "make install" soon after this.
#	Be careful not to remove a version of Vim that is still being used!
#	To find out which files and directories will be deleted, use:
#		make -n uninstall

### This Makefile has been succesfully tested on these systems.
### Check the (*) column for remarks, listed below.
### Later code changes may cause small problems, otherwise Vim is supposed to
### compile and run without problems.

#system:	      configurations:		     version (*) tested by:
#-------------	      ------------------------	     -------  -  ----------
#AIX 3.2.5	      cc (not gcc)   -			4.5  (M) Will Fiveash
#AIX 4		      cc	     +X11 -GUI		3.27 (4) Axel Kielhorn
#AIX 4.1.4	      cc	     +X11 +GUI		4.5  (5) Nico Bakker
#AIX_4.1	      gcc 2.7.2      +X11 +Motif	5.0s	 Karsten Sievert
#AIX 4.1.1	      cc	     +X11 +GUI		5.0c	 Jeff Walker
#AIX 4.2	      gcc 2.7.2.1    +X11 +GUI_Motif	5.2	 D. Rothkamp
#AIX 4.2.1	      cc				5.2k (C) Will Fiveash
#AIX 4.3	      cc	     +X11 +GUI_Athena	5.3	 Glauber Ribeiro
#AIX 4.3.3.12	      xic 3.6.6				5.6  (5) David R. Favor
#A/UX 3.1.1	      gcc	     +X11		4.0  (6) Jim Jagielski
#BeOS PR	      mwcc DR3				5.0n (T) Olaf Seibert
#BSDI 2.1 (x86)       shlicc2 gcc-2.6.3 -X11 X11R6	4.5  (1) Jos Backus
#BSD/OS 3.0 (x86)     gcc gcc-2.7.2.1 -X11 X11R6	4.6c (1) Jos Backus
#Cygwin 1.0 Win-NT    2.9-cygwin-990830 -X11 -GUI	5.6	 Bram Moolenaar
#CX/UX 6.2	      cc	     +X11 +GUI_Mofif	5.4  (V) Kipp E. Howard
#DG/UX 5.4*	      gcc 2.5.8      GUI		5.0e (H) Jonas Schlein
#DG/UX 5.4R4.20       gcc 2.7.2      GUI		5.0s (H) Rocky Olive
#DigitalUnix OSF 4.0b cc (not gcc)   +X11 +GUI +GTK	5.5b	 Peter Turcan
#HP-UX (most)	      c89 cc				5.1  (2) Bram Moolenaar
#HP-UX_9.04	      cc	     +X11 +Motif	5.0  (2) Carton Lao
#HP-UX_10.01	      gcc 2.7.2      +X11 -GUI		5.2	 D. Rothkamp
#HP-UX_10.20	      gcc 2.7.2.1    +X11 +Motif	5.0s	 Karsten Sievert
#HP-UX_10.20	      gcc 2.7.2.1    +X11R6 +Motif	5.1	 Kayhan Demirel
#HP-UX_10.20	      gcc 2.8.1      +X11R6 +Motif	5.5a	 Axel Kielhorn
#HP-UX 11.00 64 bit   egcs 1.1.1     -X11		5.4c	 Heiko Nardmann
#Irix 6.3 (O2)	      cc	     ?			4.5  (L) Edouard Poor
#Irix 6.4	      cc	     ?			5.0m (S) Rick Sayre
#Irix 6.5	      cc	     ?			5.3  (S) David Harrison
#Irix 64 bit						4.5  (K) Jon Wright
#FreeBSD 2.1.6	      gcc-2.6.3      X11R6 Moo-Tiff	5.0c	 Bram Moolenaar
#FreeBSD 2.1.6	      gcc-2.6.3      +X11 -GUI Athena	5.0c	 Bram Moolenaar
#FreeBSD 2.2.5	      gcc-2.7.2.1    X11R6 Moo-Tiff	5.4d	 Bram Moolenaar
#FreeBSD 2.2.5	      gcc-2.7.2.1    +X11 -GUI Athena	5.4d	 Bram Moolenaar
#FreeBSD 3.3	      gcc-2.7.2.1    +X11 GTK 1.2.5	5.7	 Bram Moolenaar
#FreeBSD 3.3	      gcc-2.7.2.1    +X11 Athena	5.7	 Bram Moolenaar
#FreeBSD 3.3	      gcc-2.7.2.1    +X11 Lesstif0.89.4 5.7	 Bram Moolenaar
#Linux 2.0	      gcc-2.7.2      Infomagic Motif	4.3  (3) Ronald Rietman
#Linux 2.0.31	      gcc	     +X11 +GUI Athena	5.0w (U) Darren Hiebert
#Linux 2.0.33	      gcc-2.7.2.1    X11R6, Lesstif	5.0s	 Tony Nugent
#Linux 2.0.34	      gcc-2.90.29    +X11 +GTK 1.2.2	5.4m	 Ant. Colombo
#LynxOS 2.5.0	      gcc-2.7-96q1   +X11 +GUI Athena	5.0  (O) Lorenz Hahn
#LynxOS 2.5.0	      gcc-2.7-96q1   +X11 -GUI Athena	5.0  (O) Lorenz Hahn
#MacOS		 CodeWarrior Pro 2   -			5.4i	 Axel Kielhorn
#NEC UP4800 UNIX_SV 4.2MP  cc	     +X11R6 Motif,Athena4.6b (Q) Lennart Schultz
#NetBSD 1.0A	      gcc-2.4.5      -X11 -GUI		3.21 (X) Juergen Weigert
#NetBSD 1.2.1	      gcc-2.7.2.2    +X11 +GUI Athena	5.0n	 Olaf Seibert
#NetBSD 1.2.1	      gcc-2.7.2.2    +X11 +GUI Lesstiff	5.0n	 Olaf Seibert
#QNX 4.2	      wcc386-10.6    -X11		4.2  (D) G.F. Desrochers
#QNX 4.23	      Watcom	     -X11		4.2  (F) John Oleynick
#SCO Unix v3.2.5      cc	     +X11 Motif		3.27 (C) M. Kuperblum
#SCO Open Server 5    gcc 2.7.2.3    +X11 +GUI Motif	5.3  (A) Glauber Ribeiro
#SINIX-N 5.43 RM400 R4000   cc	     +X11 +GUI		5.0l (I) Martin Furter
#SINIX-Z 5.42 i386    gcc 2.7.2.3    +X11 +GUI Motif	5.1  (I) Joachim Fehn
#SINIX-Y 5.43 RM600 R4000  gcc 2.7.2.3 +X11 +GUI Motif	5.1  (I) Joachim Fehn
#Reliant/SINIX 5.44   cc	     +X11 +GUI		5.5a (I) B. Pruemmer
#SNI Targon31 TOS 4.1.11 gcc-2.4.5   +X11 -GUI		4.6c (B) Paul Slootman
#Solaris 2.3 (sun4m)  cc	     +X11 +GUI Athena	5.0w	 Darren Hiebert
#Solaris 2.4 (Sparc)  cc	     +X11 +GUI		3.29 (9) Glauber
#Solaris 2.4/2.5      clcc	     +X11 -GUI openwin	3.20 (7) Robert Colon
#Solaris 2.5 (sun4m)  cc (SC4.0)     +X11R6 +GUI (CDE)	4.6b (E) Andrew Large
#Solaris 2.5	      cc	     +X11 +GUI Athena	4.2  (9) Sonia Heimann
#Solaris 2.5	      gcc 2.5.6      +X11 Motif		5.0m (R) Ant. Colombo
#Solaris 2.5 (Sparc)  gcc-2.7.2.3    +X11 +GUI Motif	5.5	 Ant. Colombo
#Solaris 2.5 (Sparc)  gcc-2.7.2.3    +X11 +GUI Motif    6.0d     Ant. Colombo
#Solaris 2.6 (sun4u)  gcc 2.7.2.3    +X11 +GUI_Motif	5.2	 D. Rothkamp
#Solaris 2.6 (sun4u)  cc (SC4.2)     +X11R6 +GUI (CDE)	5.0v	 Andrew Large
#Solaris 2.6	      gcc 2.8.1      ncursus		5.3  (G) Larry W. Virden
#Solaris with -lthread					5.5  (W) K. Nagano
#SunOS_5.5.1	      gcc 2.7.2      +X11 +Motif	5.0s	 Karsten Sievert
#SunOS 4.1.x			     +X11 -GUI		5.1b (J) Bram Moolenaar
#SunOS 4.1.3_U1 (sun4c) gcc	     +X11 +GUI Athena	5.0w (J) Darren Hiebert
#SUPER-UX 6.2 (NEC SX-4) cc	     +X11R6 Motif,Athena4.6b (P) Lennart Schultz
#Unisys 6035	      cc	     +X11 Motif		5.3  (8) Glauber Ribeiro

# (*)  Remarks:
#
# (1)  Uncomment line below for shlicc2
# (2)  HPUX with compile problems or wrong digraphs, uncomment line below
# (3)  Infomagic Motif needs GUI_LIB_LOC and GUI_INC_LOC set, see below.
#      And add "-lXpm" to MOTIF_LIBS2.
# (4)  For cc the optimizer must be disabled (use CFLAGS= after running
#      configure) (symptom: ":set termcap" output looks weird).
# (5)  Compiler may need extra argument, see below.
# (6)  See below for a few lines to uncomment
# (7)  See below for lines which enable the use of clcc
# (8)  Needs some EXTRA_LIBS, search for Unisys below
# (9)  Needs an extra compiler flag to compile gui_at_sb.c, see below.
# (A)  May need EXTRA_LIBS, see below
# (B)  Can't compile GUI because there is no waitpid()...  Disable GUI below.
# (C)  Force the use of curses instead of termcap, see below.
# (D)  Uncomment lines below for QNX
# (E)  You might want to use termlib instead of termcap, see below.
# (F)  See below for instructions.
# (G)  Using ncursus version 4.2 has reported to cause a crash.  Use the
#      Sun cursus library instead.
# (H)  See line for EXTRA_LIBS below.
# (I)  SINIX-N 5.42 and 5.43 need some EXTRA_LIBS.  Also for Reliant-Unix.
# (J)  If you get undefined symbols, see below for a solution.
# (K)  See lines to uncomment below for machines with 64 bit pointers.
# (L)  For Silicon Graphics O2 workstations remove "-lnsl" from auto/config.mk
# (M)  gcc version cygnus-2.0.1 does NOT work (symptom: "dl" deletes two
#      characters instead of one).
# (N)  SCO with decmouse.
# (O)  LynxOS needs EXTRA_LIBS, see below.
# (P)  For SuperUX 6.2 on NEC SX-4 see a few lines below to uncomment.
# (Q)  For UNIXSVR 4.2MP on NEC UP4800 see below for lines to uncomment.
# (R)  For Solaris 2.5 (or 2.5.1) with gcc > 2.5.6, uncomment line below.
# (S)  For Irix 6.x with MipsPro compiler, use -OPT:Olimit.  See line below.
# (T)  See ../doc/os_beos.txt.
# (U)  Must uncomment CONF_OPT_PYTHON option below to disable Python
#      detection, since the configure script runs into an error when it
#      detects Python (probably because of the bash shell).
# (V)  See lines to uncomment below.
# (X)  Need to use the .include "auto/config.mk" line below
# (Y)  See line with c89 below
# (Z)  See lines with cc or c89 below


#DO NOT CHANGE the next line, we need it for configure to find the compiler
#instead of using the default from the "make" program.
#Use a line further down to change the value for CC.
CC=

# Change and use these defines if configure cannot find your Motif stuff.
# Unfortunately there is no "standard" location for Motif.
# These defines can contain a single directory (recommended) or a list of
# directories (for when you are working with several systems). The LAST
# directory that exists is used.
# When changed, run "make distclean" next!
#GUI_INC_LOC = /usr/include/Motif2.0 /usr/include/Motif1.2
#GUI_LIB_LOC = /usr/lib/Motif2.0 /usr/lib/Motif1.2
### Use these two lines for Infomagic Motif (3)
#GUI_INC_LOC = /usr/X11R6/include
#GUI_LIB_LOC = /usr/X11R6/lib

######################## auto/config.mk ########################
# At this position auto/config.mk is included. When starting from the
# distribution it is almost empty. After running auto/configure it contains
# settings that have been discovered for your system. Settings below this
# include override settings in auto/config.mk!

# Note: if auto/config.mk is lost somehow (e.g., because configure was
# interrupted), create an empty auto/config.mk file and do "make config".

# (X) How to include auto/config.mk depends on the version of "make" you have,
#     if the current choice doesn't work, try the other one.

include auto/config.mk
#.include "auto/config.mk"
CClink = $(CC)

# Include the configuration choices first, so we can override everything
# below. As shipped, this file contains a target that causes to run
# configure. Once configure was run, this file contains a list of
# make variables with predefined values instead. Thus any second invocation
# of make, will buid Vim.

# CONFIGURE - configure arguments
# You can give a lot of options to configure.
# Change this to your desire and do 'make config' afterwards

# just an example:
#CONF_ARGS = --exec-prefix=/usr

# GUI - For creating Vim with GUI (gvim) (B)
# Uncomment this line when you don't want to get the GUI version, although you
# have GTK, Motif and/or Athena.  Also use --without-x if you don't want X11
# at all.
#CONF_OPT_GUI = --disable-gui

# Uncomment one of these lines if you have that GUI but don't want to use it.
# The automatic check will use another one that can be found
#CONF_OPT_GUI = --disable-gtk-check
#CONF_OPT_GUI = --disable-gnome-check
#CONF_OPT_GUI = --disable-motif-check
#CONF_OPT_GUI = --disable-athena-check

# Uncomment one of these lines to select a specific GUI to use.
# When using "yes" or nothing, configure will use the first one found: GTK+,
# Motif or Athena.
#
# Use "--disable-gtktest" to allow an older GTK version.
#
# GNOME means GTK with Gnome support.  If using GTK, then GNOME will
# automatically be used if it is found.  If you have GNOME, but do not want to
# use it (e.g., want a GTK-only version), then use --enable-gui=gtk.
#
# If the selected GUI isn't found, the GUI is disabled automatically
#CONF_OPT_GUI = --enable-gui=gtk
#CONF_OPT_GUI = --enable-gui=gtk --disable-gtktest
#CONF_OPT_GUI = --enable-gui=gnome
#CONF_OPT_GUI = --enable-gui=motif
#CONF_OPT_GUI = --enable-gui=motif --with-motif-lib="-static -lXm -shared"
#CONF_OPT_GUI = --enable-gui=athena

# PERL - For creating Vim with Perl interface
# Uncomment this when you want to include the Perl interface.
# The Perl option sometimes causes problems, because it adds extra flags
# to the command line.	If you see strange flags during compilation, check in
# auto/config.mk where they come from.  If it's PERL_CFLAGS, try commenting
# the next line.
# When you get an error for a missing "perl.exp" file, try creating an emtpy
# one: "touch perl.exp".
# This requires at least "small" features, "tiny" doesn't work.
#CONF_OPT_PERL = --enable-perlinterp

# PYTHON - For creating Vim with Python interface
# Uncomment this when you want to include the Python interface.
#CONF_OPT_PYTHON = --enable-pythoninterp

# TCL - For creating Vim with Tcl interface
# Uncomment this when you want to include the Tcl interface.
#CONF_OPT_TCL = --enable-tclinterp

# RUBY - For creating Vim with Ruby interface
# Uncomment this when you want to include the Ruby interface.
#CONF_OPT_RUBY = --enable-rubyinterp

# CSCOPE - For creating Vim with Cscope interface
# Uncomment this when you want to include the Cscope interface.
#CONF_OPT_CSCOPE = --enable-cscope

# WORKShOP - Sun Visual Workshop interface
#CONF_OPT_WORKSHOP = --enable-workshop

# SNIFF - Include support for SNiFF+.
#CONF_OPT_SNIFF = --enable-sniff

# MULTIBYTE - To edit multi-byte characters.  Well, actually only two-byte
# characters at the moment.
# Uncomment this when you want to edit a multibyte language.
# Note: Compile on a machine where setlocale() actually works, otherwise the
# configure tests may fail.
#CONF_OPT_MULTIBYTE = --enable-multibyte

# XIM - X Input Method.  Special character input support for X11 (chinese,
# japanese, special symbols, etc).  Also needed for dead-key support.
# When omitted it's automatically enabled for the X-windows GUI.
# HANGUL - Input Hangul (korean) language using internal routines.
# Uncomment one of these when you want to input a multibyte language.
#CONF_OPT_INPUT = --enable-xim
#CONF_OPT_INPUT = --disable-xim
#CONF_OPT_INPUT = --enable-hangulinput

# FONTSET - X fontset support for output of languages with many characters.
# Uncomment this when you want to output a multibyte language.
#CONF_OPT_OUTPUT = --enable-fontset

# gpm - For mouse support on Linux console via gpm
# Uncomment this when you do not want to include gpm support, even
# though you have gpm libraries and includes
#CONF_OPT_GPM = --disable-gpm

# FEATURES - For creating Vim with more or less features
# Uncomment one of these lines when you want to include few to many features.
# The default is "normal".
#CONF_OPT_FEAT = --with-features=tiny
#CONF_OPT_FEAT = --with-features=small
#CONF_OPT_FEAT = --with-features=normal
#CONF_OPT_FEAT = --with-features=big
#CONF_OPT_FEAT = --with-features=huge

# X WINDOWS DISABLE - For creating a plain Vim without any X11 related fancies
# (otherwise Vim configure will try to include xterm titlebar access)
# Also disable the GUI above, otherwise it will be included anyway.
# When both GUI and X11 have been disabled this may save about 15% of the
# code and make Vim startup quicker.
#CONF_OPT_X = --without-x

# X WINDOWS DIRECTORY - specify X directories
# If configure can't find you X stuff, or if you have multiple X11 derivates
# installed, you may wish to specify which one to use.
# Select nothing to let configure choose.
# This here selects openwin (as found on sun).
#XROOT = /usr/openwin
#CONF_OPT_X = --x-include=$(XROOT)/include --x-libraries=$(XROOT)/lib

# COMPILER - Name of the compiler
# The default from configure will mostly be fine, no need to change this, just
# an example. If a compiler is defined here, configure will use it rather than
# probing for one. It is dangerous to change this after configure was run.
# Make will use your choice then -- but beware: Many things may change with
# another compiler. It is wise to run 'make distclean' and start all over
# again.
#CC = cc
#CC = gcc

# COMPILER FLAGS - change as you please. Either before running configure
# or afterwards. For examples see below.
# When using -g with some older versions of Linux you might get a
# statically linked executable.
# When not defined, configure will try to use -O2 -g for gcc and -O for cc.
#CFLAGS = -g
#CFLAGS = -O

# Optimization limits - depends on the compiler.  Automatic check in configure
# doesn't work very well, because many compilers only give a warning for
# unrecognized arguments.
#CFLAGS = -O -OPT:Olimit=2600
#CFLAGS = -O -Olimit 2000
#CFLAGS = -O -FOlimit,2000

# Often used for GCC: mixed optimizing, lot of optimizing, debugging
#CFLAGS = -g -O2 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#CFLAGS = -g -O2 -fno-strength-reduce -Wall -Wmissing-prototypes
#CFLAGS = -O6 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#CFLAGS = -g -DDEBUG -Wall -Wshadow -Wmissing-prototypes

# EFENCE - Electric-Fence malloc debugging: catches memory accesses beyond
# allocated memory (and makes every malloc()/free() very slow).
# Electric Fence is free (search ftp sites).
# On FreeBSD you might need to enlarge the number of mmaps allowed.  Do this
# as root: sysctl -w vm.max_proc_mmap=30000
#EXTRA_LIBS = /usr/local/lib/libefence.a

# PURIFY - remove the # to use the "purify" program (hoi Nia++!)
#PURIFY = purify

# LINT - for running lint
LINT_OPTIONS = -beprxzF

# PROFILING - Uncomment the next three lines to do profiling with gcc and
# gprof.  Only works when using termcap library.  Doesn't work with GUI or
# Perl.
# Need to recompile everything after changing this: "make clean" "make".
#CFLAGS = -g -pg -O2 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#LDFLAGS = -pg
#LIBS = -ltermcap_p

#####################################################
###  Specific systems, check if yours is listed!  ###
#####################################################

### Uncomment things here only if the values chosen by configure are wrong.
### It's better to adjust configure.in and run autoconf, if you can!
### Then send the required changes to configure.in to the bugs list.

### (1) BSD/OS 2.0.1, 2.1 or 3.0 using shared libraries
###
#CC = shlicc2
#CFLAGS = -O2 -g -m486 -Wall -Wshadow -Wmissing-prototypes -fno-builtin

### (2) HP-UX with a non-ANSI cc, use the c89 ANSI compiler
###	The first probably works on all systems
###	The second should work a bit better on newer systems
###	Information provided by: Richard Allen <ra@rhi.hi.is>
#CC = c89 -D_HPUX_SOURCE
#CC = c89 -O +Onolimit +ESlit -D_HPUX_SOURCE

### (2) For HP-UX: Enable the use of a different set of digraphs.  Use this
###	when the default (ISO) digraphs look completely wrong.
###	After changing this do "touch digraph.c; make".
#EXTRA_DEFS = -DHPUX_DIGRAPHS

### (2) For HP-UX: 9.04 cpp default macro definition table of 128000 bytes
###	is too small to compile many routines.	It produces too much defining
###	and no space errors.
###	Uncomment the following to specify a larger macro definition table.
#CFLAGS = -Wp,-H256000

### (2) For HP-UX 10.20 using the HP cc, with X11R6 and Motif 1.2, with
###	libraries in /usr/lib instead of /lib (avoiding transition links).
###	Information provided by: David Green
#XROOT = /usr
#CONF_OPT_X = --x-include=$(XROOT)/include/X11R6 --x-libraries=$(XROOT)/lib/X11R6
#GUI_INC_LOC = /usr/include/Motif1.2
#GUI_LIB_LOC = /usr/lib/Motif1.2_R6

### (5) AIX 4.1.4 with cc
#CFLAGS = -O -qmaxmem=8192

###     AIX with c89 (Walter Briscoe)
#CC = c89
#CPPFLAGS = -D_ALL_SOURCE

###     AIX 4.3.3.12 with xic 3.6.6 (David R. Favor)
#       needed to avoid a problem where strings.h gets included
#CFLAGS = -qsrcmsg -O2 -qmaxmem=8192 -D__STR31__

### (W) Solaris with multi-threaded libraries (-lthread):
###	If suspending doesn't work properly, try using this line:
#EXTRA_DEFS = -D_REENTRANT

### (7) Solaris 2.4/2.5 with Centerline compiler
#CC = clcc
#X_LIBS_DIR = -L/usr/openwin/lib -R/usr/openwin/lib
#CFLAGS = -O

### (8) Unisys 6035 (Glauber Ribeiro)
#EXTRA_LIBS = -lnsl -lsocket -lgen

### (9) Solaris 2.x with cc (SunPro), using Athena.
###	Only required for compiling gui_at_sb.c.
###	Symptom: "identifier redeclared: vim_XawScrollbarSetThumb"
###	Use one of the lines (either Full ANSI or no ANSI at all)
#CFLAGS = $(CFLAGS) -Xc
#CFLAGS = $(CFLAGS) -Xs

### When builtin functions cause problems with gcc (for Sun 4.1.x)
#CFLAGS = -O2 -Wall -traditional -Wno-implicit

### Apollo DOMAIN (with SYSTYPE = bsd4.3) (TESTED for version 3.0)
#EXTRA_DEFS = -DDOMAIN
#CFLAGS= -O -A systype,bsd4.3

### Coherent 4.2.10 on Intel 386 platform
#EXTRA_DEFS = -Dvoid=int
#EXTRA_LIBS = -lterm -lsocket

### SCO 3.2, with different library name for terminfo
#EXTRA_LIBS = -ltinfo

### Solaris 2.3 with X11 and specific cc
#CC=/opt/SUNWspro/bin/cc -O -Xa -v -R/usr/openwin/lib

### Solaris with /usr/ucb/cc (it is rejected by autoconf as "cc")
#CC	    = /usr/ucb/cc
#EXTRA_LIBS = -R/usr/ucblib

### Solaris with Forte Developer and FEAT_SUN_WORKSHOP
# The Xpm library is available from ftp://koala.inria.fr/pub/xpm.
#CC		= cc
#XPM_DIR		= /usr/local/xpm/xpm-3.4k-solaris
#XPM_LIB		= -L$(XPM_DIR)/lib -R$(XPM_DIR)/lib -lXpm
#XPM_IPATH	= -I$(XPM_DIR)/include
#EXTRA_LIBS	= $(XPM_LIB)
#EXTRA_IPATHS	= $(XPM_IPATH)
#EXTRA_DEFS	= -xCC -DHAVE_X11_XPM_H

### UTS2 for Amdahl UTS 2.1.x
#EXTRA_DEFS = -DUTS2
#EXTRA_LIBS = -lsocket

### UTS4 for Amdahl UTS 4.x
#EXTRA_DEFS = -DUTS4 -Xa

### USL for Unix Systems Laboratories (SYSV 4.2)
#EXTRA_DEFS = -DUSL

### RISCos on MIPS without X11
#EXTRA_DEFS = -DMIPS

### RISCos on MIPS with X11
#EXTRA_LIBS = -lsun

### (6)  A/UX 3.1.1 with gcc (Jim Jagielski)
#CC= gcc -D_POSIX_SOURCE
#CFLAGS= -O2
#EXTRA_LIBS = -lposix -lbsd -ltermcap -lX11

### (A)  Some versions of SCO Open Server 5 (Jan Christiaan van Winkel)
###	 Also use the CONF_TERM_LIB below!
#EXTRA_LIBS = -lgen

### (D)  QNX (by G.F. Desrochers)
#CFLAGS = -g -O -mf -4

### (F)  QNX (by John Oleynick)
# 1. If you don't have an X server: Comment out CONF_OPT_GUI and uncomment
#    CONF_OPT_X = --without-x.
# 2. make config
# 3. edit auto/config.mk and remove -ldir and -ltermcap from LIBS.  It doesn't
#	have -ldir (does config find it somewhere?) and -ltermcap has at
#	least one problem so I use termlib.o instead.  The problem with
#	termcap is that it segfaults if you call it with the name of
#	a non-existent terminal type.
# 4. edit auto/config.h and add #define USE_TMPNAM
# 5. add termlib.o to OBJ
# 6. make

### (H)  for Data general DG/UX 5.4.2 and 5.4R3.10 (Jonas J. Schlein)
#EXTRA_LIBS = -lgen

### (I) SINIX-N 5.42 or 5.43 RM400 R4000 (also SINIX-Y and SINIX-Z)
#EXTRA_LIBS = -lgen -lnsl
###   For SINIX-Y this is needed for the right prototype of gettimeofday()
#EXTRA_DEFS = -D_XPG_IV

### (I) Reliant-Unix (aka SINIX) 5.44 with standard cc
#	Use both "-F O3" lines for optimization or the "-g" line for debugging
#EXTRA_LIBS = -lgen -lsocket -lnsl -lSM -lICE
#CFLAGS = -F O3 -DSINIXN
#LDFLAGS = -F O3
#CFLAGS = -g -DSINIXN

### (P)  SCO 3.2.42, with different termcap names for some useful keys DJB
#EXTRA_DEFS = -DSCOKEYS -DNETTERM_MOUSE -DDEC_MOUSE -DXTERM_MOUSE -DHAVE_GETTIMEOFDAY
#EXTRA_LIBS = -lsocket -ltermcap -lmalloc -lc_s

### (P)  SuperUX 6.2 on NEC SX-4 (Lennart Schultz)
#GUI_INC_LOC = /usr/include
#GUI_LIB_LOC = /usr/lib
#EXTRA_LIBS = -lgen

### (Q) UNIXSVR 4.2MP on NEC UP4800 (Lennart Schultz)
#GUI_INC_LOC = /usr/necccs/include
#GUI_LIB_LOC = /usr/necccs/lib/X11R6
#XROOT = /usr/necccs
#CONF_OPT_X = --x-include=$(XROOT)/include --x-libraries=$(XROOT)/lib/X11R6
#EXTRA_LIBS = -lsocket -lgen

### (R) for Solaris 2.5 (or 2.5.1) with gcc > 2.5.6 you might need this:
#LDFLAGS = -lw -ldl

### Irix 4.0 & 5.2 (Silicon Graphics Machines, __sgi will be defined)
# Not needed for Irix 5.3, Ives Aerts reported
#EXTRA_LIBS = -lmalloc -lc_s
# Irix 4.0, when regexp and regcmp cannot be found when linking:
#EXTRA_LIBS = -lmalloc -lc_s -lPW

### (S) Irix 6.x (MipsPro compiler): Uses different Olimit flag:
# Note:	This newer option style is used with the MipsPro compilers ONLY if
#	you are compiling an "n32" or "64" ABI binary (use either a -n32
#	flag or a -64 flag for CFLAGS).  If you explicitly use a -o32 flag,
#	then the CFLAGS option format will be the typical style (i.e.
#	-Olimit 3000).
#CFLAGS = -OPT:Olimit=3000 -O

### (S) Irix 6.5 with MipsPro C compiler.  Try this as a test to see new
#	compiler features!  Beware, the optimization is EXTREMELY thorough
#	and takes quite a long time.
# Note: See the note above.  Here, the -mips3 option automatically
#	enables either the "n32" or "64" ABI, depending on what machine you
#	are compiling on (n32 is explicitly enabled here, just to make sure).
#CFLAGS = -OPT:Olimit=3500 -O -n32 -mips3 -IPA:aggr_cprop=ON -INLINE:dfe=ON:list=ON:must=screen_char,out_char,ui_write,out_flush
#LDFLAGS= -OPT:Olimit=3500 -O -n32 -mips3 -IPA:aggr_cprop=ON -INLINE:dfe=ON:list=ON:must=screen_char,out_char,ui_write,out_flush

### (K) for SGI Irix machines with 64 bit pointers ("uname -s" says IRIX64)
###	Suggested by Jon Wright <jon@gate.sinica.edu.tw>.
###	Tested on R8000 IRIX6.1 Power Indigo2.
###	Check /etc/compiler.defaults for your compiler settings.
# either (for 64 bit pointers) uncomment the following line
#GUI_LIB_LOC = /usr/lib64
# then
# 1) make config
# 2) edit auto/config.mk and delete the -lelf entry in the LIBS line
# 3) make
#
# or (for 32bit pointers) uncomment the following line
#EXTRA_DEFS = -n32
#GUI_LIB_LOC = /usr/lib32
# then
# 1) make config
# 2) edit auto/config.mk, add -n32 to LDFLAGS
# 3) make
###

### (C)  On SCO Unix v3.2.5 (and probably other versions) the termcap library,
###	 which is found by configure, doesn't work correctly.  Symptom is the
###	 error message "Termcap entry too long".  Uncomment the next line.
###	 On AIX 4.2.1 (and other versions probably), libtermcap is reported
###	 not to display properly.
### after changing this, you need to do "make distclean; make".
#CONF_TERM_LIB = --with-tlib=curses

### (E)  If you want to use termlib library instead of the automatically found
###	 one.  After changing this, you need to do "make distclean; make".
#CONF_TERM_LIB = --with-tlib=termlib

### If you want to use ncurses library instead of the automatically found one
### after changing this, you need to do "make distclean; make".
#CONF_TERM_LIB = --with-tlib=ncurses

### For GCC on MSDOS, the ".exe" suffix will be added.
#SUFFIX = .exe

### (O)  For LynxOS 2.5.0, tested on PC.
#EXTRA_LIBS = -lXext -lSM -lICE -lbsd
###	 For LynxOS 3.0.1, tested on PPC
#EXTRA_LIBS= -lXext -lSM -lICE -lnetinet -lXmu -liberty
###	 For LynxOS 3.0.1, tested on PC
#EXTRA_LIBS= -lXext -lSM -lICE -lnetinet -lXmu


### (V)  For CX/UX 6.2	(on Harris/Concurrent NightHawk 4800, 5800). Remove
###	 -Qtarget if only in a 5800 environment.  (Kipp E. Howard)
#CFLAGS = -O -Qtarget=m88110compat
#EXTRA_LIBS = -lgen

##################### end of system specific lines ###################

### Names of the programs and targets
VIMTARGET	= $(VIMNAME)$(SUFFIX)
EXTARGET	= $(EXNAME)$(SUFFIX)
VIEWTARGET	= $(VIEWNAME)$(SUFFIX)
GVIMNAME	= g$(VIMNAME)
GVIMTARGET	= $(GVIMNAME)$(SUFFIX)
GVIEWNAME	= g$(VIEWNAME)
GVIEWTARGET	= $(GVIEWNAME)$(SUFFIX)
RVIMNAME	= r$(VIMNAME)
RVIMTARGET	= $(RVIMNAME)$(SUFFIX)
RVIEWNAME	= r$(VIEWNAME)
RVIEWTARGET	= $(RVIEWNAME)$(SUFFIX)
RGVIMNAME	= r$(GVIMNAME)
RGVIMTARGET	= $(RGVIMNAME)$(SUFFIX)
RGVIEWNAME	= r$(GVIEWNAME)
RGVIEWTARGET	= $(RGVIEWNAME)$(SUFFIX)

### Names of the tools that are also made
TOOLS = xxd/xxd$(SUFFIX)

### Installation directories.  The defaults come from configure.
#
### prefix	the top directory for the data (default "/usr/local")
#
# Uncomment the next line to install Vim in your home directory.
#prefix = $(HOME)

### exec_prefix	is the top directory for the executable (default $(prefix))
#
# Uncomment the next line to install the Vim executable in "/usr/machine/bin"
#exec_prefix = /usr/machine

### BINDIR	dir for the executable	 (default "$(exec_prefix)/bin")
### MANDIR	dir for the manual pages (default "$(prefix)/man")
### DATADIR	dir for the other files  (default "$(prefix)/lib" or
#						  "$(prefix)/share")
# They may be different when using different architectures for the
# executable and a common directory for the other files.
#
# Uncomment the next line to install Vim in "/usr/bin"
#BINDIR   = /usr/bin
# Uncomment the next line to install Vim manuals in "/usr/share/man/man1"
#MANDIR   = /usr/share/man
# Uncomment the next line to install Vim help files in "/usr/share/vim"
#DATADIR  = /usr/share

### Location of man page
MANSUBDIR = $(MANDIR)/man1

### Location of Vim files (should not need to be changed, and some things
### might not work when they are changed!)
VIMDIR = /vim
VIMRTDIR = /vim60p
HELPSUBDIR = /doc
SYNSUBDIR = /syntax
INDSUBDIR = /indent
PLUGSUBDIR = /plugin
FTPLUGSUBDIR = /ftplugin
LANGSUBDIR = /lang
MACROSUBDIR = /macros
TOOLSSUBDIR = /tools
TUTORSUBDIR = /tutor
PODIR = po

### VIMLOC	common root of the Vim files (all versions)
### VIMRTLOC	common root of the runtime Vim files (this version)
### VIMRCLOC	compiled-in location for global [g]vimrc files (all versions)
### VIMRUNTIMEDIR  compiled-in location for runtime files (optional)
### HELPSUBLOC	location for help files
### SYNSUBLOC	location for syntax files
### INDSUBLOC	location for indent files
### PLUGSUBLOC	location for standard plugin files
### FTPLUGSUBLOC  location for ftplugin files
### LANGSUBLOC	location for syntax files
### MACROSUBLOC	location for macro files
### TOOLSSUBLOC	location for tools files
### TUTORSUBLOC	location for tutor files
### SCRIPTLOC	location for script files (menu.vim, bugreport.vim, ..)
### You can override these if you want to install them somewhere else.
### Edit feature.h for compile-time settings.
VIMLOC		= $(DATADIR)$(VIMDIR)
VIMRTLOC	= $(DATADIR)$(VIMDIR)$(VIMRTDIR)
VIMRCLOC	= $(VIMLOC)
HELPSUBLOC	= $(VIMRTLOC)$(HELPSUBDIR)
SYNSUBLOC	= $(VIMRTLOC)$(SYNSUBDIR)
INDSUBLOC	= $(VIMRTLOC)$(INDSUBDIR)
PLUGSUBLOC	= $(VIMRTLOC)$(PLUGSUBDIR)
FTPLUGSUBLOC	= $(VIMRTLOC)$(FTPLUGSUBDIR)
LANGSUBLOC	= $(VIMRTLOC)$(LANGSUBDIR)
MACROSUBLOC	= $(VIMRTLOC)$(MACROSUBDIR)
TOOLSSUBLOC	= $(VIMRTLOC)$(TOOLSSUBDIR)
TUTORSUBLOC	= $(VIMRTLOC)$(TUTORSUBDIR)
SCRIPTLOC	= $(VIMRTLOC)

### Only set VIMRUNTIMEDIR when VIMRCLOC is set to a different location and
### the runtime directory is not below it.
#VIMRUNTIMEDIR = $(VIMRTLOC)

### Added for the Debian GNU/Linux packaged version of Vim, to get around
### some sed trickery. (the files are installed into a temporary directory
### for building the package, but they eventually end up in /usr/... . This
### ensures that the correct path is put into the man page.)
VIMRCENDLOC = $(VIMRCLOC)
#VIMRCENDLOC = /etc
HELPENDLOC = $(HELPSUBLOC)
#HELPENDLOC = /usr/doc/vim
SYNTAXENDLOC = $(SYNSUBLOC)
#SYNTAXENDLOC = /usr/lib/vim/syntax
INDENTENDLOC = $(INDSUBLOC)
#INDENTENDLOC = /usr/lib/vim/indent
PLUGINFTENDLOC = $(PLUGSUBLOC)
#PLUGINFTENDLOC = /usr/lib/vim/plugin
FTPLUGINFTENDLOC = $(FTPLUGSUBLOC)
#FTPLUGINFTENDLOC = /usr/lib/vim/ftplugin
LANGENDLOC = $(LANGSUBLOC)
#LANGENDLOC = /usr/share/locale/vim
SCRIPTENDLOC = $(SCRIPTLOC)
#SCRIPTENDLOC = /etc
TUTORENDLOC = $(TUTORSUBLOC)
#TUTORENDLOC = /usr/lib/vim/tutor

### Name of the menu file target.
SYS_MENU_FILE	= $(SCRIPTLOC)/menu.vim

### Name of the bugreport file target.
SYS_BUGR_FILE	= $(SCRIPTLOC)/bugreport.vim

### Name of the file type detection file target.
SYS_FILETYPE_FILE = $(SCRIPTLOC)/filetype.vim

### Name of the file type detection file target.
SYS_FTOFF_FILE	= $(SCRIPTLOC)/ftoff.vim

### Name of the file type detection script file target.
SYS_SCRIPTS_FILE = $(SCRIPTLOC)/scripts.vim

### Name of the ftplugin-on file target.
SYS_FTPLUGIN_FILE = $(SCRIPTLOC)/ftplugin.vim

### Name of the ftplugin-off file target.
SYS_FTPLUGOF_FILE = $(SCRIPTLOC)/ftplugof.vim

### Name of the indent-on file target.
SYS_INDENT_FILE = $(SCRIPTLOC)/indent.vim

### Name of the indent-off file target.
SYS_INDOFF_FILE = $(SCRIPTLOC)/indoff.vim

### Name of the option window script file target.
SYS_OPTWIN_FILE = $(SCRIPTLOC)/optwin.vim

# Program to install the program in the target directory.  Could also be "mv".
INSTALL_PROG	= cp

# Program to install the data in the target directory.	Cannot be "mv"!
INSTALL_DATA	= cp
INSTALL_DATA_R	= cp -r

### Program to run on installed binary
#STRIP = strip

### Permissions for binaries
BINMOD = 755

### Permissions for man page
MANMOD = 644

### Permissions for help files
HELPMOD = 644

### Permissions for Perl and shell scripts
SCRIPTMOD = 755

### Permission for Vim script files (menu.vim, bugreport.vim, ..)
VIMSCRIPTMOD = 644

### Permissions for all directories that are created
DIRMOD = 755

### Permissions for all other files that are created
FILEMOD = 644

# Where to copy the man and help files from
HELPSOURCE = ../runtime/doc

# Where to copy the script files from (menu, bugreport)
SCRIPTSOURCE = ../runtime

# Where to copy the syntax files from
SYNSOURCE = ../runtime/syntax

# Where to copy the indent files from
INDSOURCE = ../runtime/indent

# Where to copy the standard plugin files from
PLUGSOURCE = ../runtime/plugin

# Where to copy the ftplugin files from
FTPLUGSOURCE = ../runtime/ftplugin

# Where to copy the macro files from
MACROSOURCE = ../runtime/macros

# Where to copy the tools files from
TOOLSSOURCE = ../runtime/tools

# Where to copy the tutor files from
TUTORSOURCE = ../runtime/tutor

# Where to look for language specific files
LANGSOURCE = ../runtime/lang

# If you are using Linux, you might want to use this to make vim the
# default vi editor, it will create a link from vi to Vim when doing
# "make install".  An existing file will be overwritten!
# When not using it, some make programs can't handle an undefined $(LINKIT).
#LINKIT = -ln -f -s $(BINDIR)/$(VIMTARGET) /usr/bin/vi
LINKIT = @echo >/dev/null

###
### GRAPHICAL USER INTERFACE (GUI).
### 'configure --enable-gui' can enable one of these for you if you did set
### a corresponding CONF_OPT_GUI above and have X11.
### Override configures choice by uncommenting all the following lines.
### As they are, the GUI is disabled.  Replace "NONE" with "ATHENA" or "MOTIF"
### for enabling the Athena or Motif GUI.
#GUI_SRC	= $(NONE_SRC)
#GUI_OBJ	= $(NONE_OBJ)
#GUI_DEFS	= $(NONE_DEFS)
#GUI_IPATH	= $(NONE_IPATH)
#GUI_LIBS_DIR	= $(NONE_LIBS_DIR)
#GUI_LIBS1	= $(NONE_LIBS1)
#GUI_LIBS2	= $(NONE_LIBS2)
#GUI_TARGETS	= $(NONE_TARGETS)
#GUI_MAN_TARGETS= $(NONE_MAN_TARGETS)
#GUI_TESTTARGET = $(NONE_TESTTARGET)

### GTK GUI interface.
GTK_SRC		= gui.c gui_gtk.c gui_gtk_x11.c pty.c gui_gtk_f.c
GTK_OBJ		= objects/gui.o objects/gui_gtk.o objects/gui_gtk_x11.o \
			objects/pty.o objects/gui_gtk_f.o
GTK_DEFS	= -DFEAT_GUI_GTK $(NARROW_PROTO)
GTK_IPATH	= $(GUI_INC_LOC)
GTK_LIBS_DIR	= $(GUI_LIB_LOC)
GTK_LIBS1	=
GTK_LIBS2	= $(GTK_LIBNAME)
GTK_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
GTK_MAN_TARGETS = $(MANSUBDIR)/$(GVIMNAME).1 \
			$(MANSUBDIR)/$(GVIEWNAME).1 \
			$(MANSUBDIR)/$(RGVIMNAME).1 \
			$(MANSUBDIR)/$(RGVIEWNAME).1
GTK_TESTTARGET = gui

### Motif GUI interface.
MOTIF_SRC	= gui.c gui_motif.c gui_x11.c pty.c
MOTIF_OBJ	= objects/gui.o objects/gui_motif.o objects/gui_x11.o \
			objects/pty.o
MOTIF_DEFS	= -DFEAT_GUI_MOTIF $(NARROW_PROTO)
MOTIF_IPATH	= -I$(GUI_INC_LOC)
MOTIF_LIBS_DIR	= -L$(GUI_LIB_LOC)
MOTIF_LIBS1	=
MOTIF_LIBS2	= $(MOTIF_LIBNAME) -lXt
MOTIF_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
MOTIF_MAN_TARGETS = $(MANSUBDIR)/$(GVIMNAME).1 \
			$(MANSUBDIR)/$(GVIEWNAME).1 \
			$(MANSUBDIR)/$(RGVIMNAME).1 \
			$(MANSUBDIR)/$(RGVIEWNAME).1
MOTIF_TESTTARGET = gui

#For SunOS (Solaris) you might want to use this line:
#MOTIF_LIBS_DIR = -L$(GUI_LIB_LOC) -R$(GUI_LIB_LOC)

### Athena Widget GUI interface.
### Use Xaw3d to make the menus look a little bit nicer
#XAW_LIB = -lXaw3d
XAW_LIB = -lXaw

### When using Xaw3d, uncomment/comment the following lines to also get the
### scrollbars from Xaw3d.
#ATHENA_SRC	= gui.c gui_athena.c gui_x11.c pty.c gui_at_fs.c
#ATHENA_OBJ	= objects/gui.o objects/gui_athena.o objects/gui_x11.o \
#			objects/pty.o objects/gui_at_fs.o
#ATHENA_DEFS	= -DFEAT_GUI_ATHENA $(NARROW_PROTO) \
#		    -Dvim_scrollbarWidgetClass=scrollbarWidgetClass \
#		    -Dvim_XawScrollbarSetThumb=XawScrollbarSetThumb
ATHENA_SRC	= gui.c gui_athena.c gui_x11.c pty.c gui_at_sb.c gui_at_fs.c
ATHENA_OBJ	= objects/gui.o objects/gui_athena.o objects/gui_x11.o \
			objects/pty.o objects/gui_at_sb.o objects/gui_at_fs.o
ATHENA_DEFS	= -DFEAT_GUI_ATHENA $(NARROW_PROTO)

ATHENA_IPATH	= -I$(GUI_INC_LOC)
ATHENA_LIBS_DIR = -L$(GUI_LIB_LOC)
ATHENA_LIBS1	= $(XAW_LIB)
ATHENA_LIBS2	= -lXt
ATHENA_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
ATHENA_MAN_TARGETS = $(MANSUBDIR)/$(GVIMNAME).1 \
			$(MANSUBDIR)/$(GVIEWNAME).1 \
			$(MANSUBDIR)/$(RGVIMNAME).1 \
			$(MANSUBDIR)/$(RGVIEWNAME).1
ATHENA_TESTTARGET = gui

### (J)  Sun OpenWindows 3.2 (SunOS 4.1.x) or earlier that produce these ld
#	 errors:  ld: Undefined symbol
#		      _get_wmShellWidgetClass
#		      _get_applicationShellWidgetClass
# then you need to get patches 100512-02 and 100573-03 from Sun.  In the
# meantime, uncomment the following GUI_X_LIBS definition as a workaround:
#GUI_X_LIBS = -Bstatic -lXmu -Bdynamic -lXext
# If you also get cos, sin etc. as undefined symbols, try uncommenting this
# too:
#EXTRA_LIBS = /usr/openwin/lib/libXmu.sa -lm

### BeOS GUI interface.
BEOSGUI_SRC	= gui.c gui_beos.cc pty.c
BEOSGUI_OBJ	= objects/gui.o objects/gui_beos.o  objects/pty.o
BEOSGUI_DEFS	= -DFEAT_GUI_BEOS
BEOSGUI_IPATH	=
BEOSGUI_LIBS_DIR =
BEOSGUI_LIBS1	= -lbe -lroot
BEOSGUI_LIBS2	=
BEOSGUI_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
BEOSGUI_MAN_TARGETS = $(MANSUBDIR)/$(GVIMNAME).1 \
			$(MANSUBDIR)/$(GVIEWNAME).1 \
			$(MANSUBDIR)/$(RGVIMNAME).1 \
			$(MANSUBDIR)/$(RGVIEWNAME).1
BEOSGUI_TESTTARGET = gui

# GUI files used for making Vim
ALL_GUI_SRC  = gui.c gui_gtk.c gui_gtk_f.c gui_motif.c gui_athena.c gui_gtk_x11.c gui_x11.c gui_at_sb.c gui_at_fs.c pty.c
ALL_GUI_PRO  = gui.pro gui_gtk.pro gui_motif.pro gui_athena.pro gui_gtk_x11.pro gui_x11.pro gui_w16.pro gui_w32.pro gui_amiga.pro

### our grand parent directory should know who we are...
### only used for "make tar"
VIMVERSION = `eval "basename \`cd ../; pwd\`"`

### Command to create dependencies based on #include "..."
### prototype headers are ignored due to -DPROTO, system
### headers #include <...> are ignored if we use the -MM option, as
### e.g. provided by gcc-cpp.
### Include FEAT_GUI to get gependency on gui.h
CPP_DEPEND = $(CC) -I$(srcdir) -M$(CPP_MM) $(DEPEND_CFLAGS)

# flags for cproto
#     This is for cproto 3 patchlevel 8 or below
#     __inline and __attribute__ are not recognized by cproto
NO_ATTR = -D__inline= -D"__attribute__\\(x\\)="
#
#     This is for cproto 3 patchlevel 9 or above (currently 4.6)
#     __inline and __attribute__ are now recognized by cproto
#     -D"foo()=" is not supported by all compilers so do not use it
# NO_ATTR=
#
#     maybe the "/usr/bin/cc -E" has to be adjusted for some systems
# This is for cproto 3.5 patchlevel 3:
# PROTO_FLAGS = -f4 -m__ARGS -d -E"$(CPP)" $(NO_ATTR)
#
# Use this for cproto 3 patchlevel 6 or below (use "cproto -V" to check):
# PROTO_FLAGS = -f4 -m__ARGS -d -E"$(CPP)" $(NO_ATTR)
#
# Use this for cproto 3 patchlevel 7 or above (use "cproto -V" to check):
PROTO_FLAGS = -m -M__ARGS -d -E"$(CPP)" $(NO_ATTR)


################################################
##   no changes required below this line      ##
################################################

SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .cc .c .o .pro

PRE_DEFS = -Iproto $(DEFS) $(GUI_DEFS) $(GUI_IPATH) $(CPPFLAGS) $(EXTRA_IPATHS)
POST_DEFS = $(X_CFLAGS) $(PERL_CFLAGS) $(PYTHON_CFLAGS) $(TCL_CFLAGS) $(RUBY_CFLAGS) $(EXTRA_DEFS)

ALL_CFLAGS = $(PRE_DEFS) $(CFLAGS) $(POST_DEFS)

LINT_CFLAGS = -DLINT -I. $(PRE_DEFS) $(POST_DEFS) -Dinline= -D__extension__=

DEPEND_CFLAGS = -DPROTO -DDEPEND -DFEAT_GUI $(LINT_CFLAGS)

PFLAGS = $(PROTO_FLAGS) -DPROTO $(LINT_CFLAGS)

ALL_LIB_DIRS = $(GUI_LIBS_DIR) $(X_LIBS_DIR)
ALL_LIBS = $(GUI_LIBS1) $(GUI_X_LIBS) $(GUI_LIBS2) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(LIBS) $(EXTRA_LIBS) $(PERL_LIBS) $(PYTHON_LIBS) $(TCL_LIBS) $(RUBY_LIBS)

#	     BASIC_SRC: files that are always used
#	       GUI_SRC: extra GUI files for current configuration
#	   ALL_GUI_SRC: all GUI files for Unix
#
#		   SRC: files used for current configuration
#	      TAGS_SRC: source files used for make tags
#	     TAGS_INCL: include files used for make tags
#	       ALL_SRC: source files used for make depend and make lint

TAGS_INCL = *.h

BASIC_SRC = \
	buffer.c \
	charset.c \
	digraph.c \
	edit.c \
	eval.c \
	ex_cmds.c \
	ex_docmd.c \
	ex_getln.c \
	fileio.c \
	fold.c \
	getchar.c \
	if_cscope.c \
	main.c \
	mark.c \
	memfile.c \
	memline.c \
	menu.c \
	message.c \
	misc1.c \
	misc2.c \
	multibyte.c \
	normal.c \
	ops.c \
	option.c \
	os_unix.c \
	auto/pathdef.c \
	quickfix.c \
	regexp.c \
	screen.c \
	search.c \
	syntax.c \
	tag.c \
	term.c \
	ui.c \
	undo.c \
	version.c \
	window.c \
	$(OS_EXTRA_SRC)

SRC =	$(BASIC_SRC) $(GUI_SRC) $(HANGULIN_SRC) $(PERL_SRC) $(PYTHON_SRC) \
	$(TCL_SRC) $(RUBY_SRC) $(SNIFF_SRC) $(WORKSHOP_SRC) $(WSDEBUG_SRC)

TAGS_SRC = *.c *.cpp if_perl.xs

EXTRA_SRC = hangulin.c auto/if_perl.c if_perlsfio.c if_python.c if_tcl.c \
		if_ruby.c if_sniff.c gui_beval.c \
		workshop.c wsdebug.c integration.c

# All sources, also the ones that are not configured
ALL_SRC = $(BASIC_SRC) $(ALL_GUI_SRC) $(EXTRA_SRC)

# Which files to check with lint.  Select one of these three lines.  ALL_SRC
# checks more, but may not work well for checking a GUI that wasn't configured.
# The perl sources also don't work well with lint.
LINT_SRC = $(BASIC_SRC) $(GUI_SRC) $(HANGULIN_SRC) $(PYTHON_SRC) $(TCL_SRC) \
	$(SNIFF_SRC) $(WORKSHOP_SRC) $(WSDEBUG_SRC)
#LINT_SRC = $(SRC)
#LINT_SRC = $(ALL_SRC)

OBJ = \
	objects/buffer.o \
	objects/charset.o \
	objects/digraph.o \
	objects/edit.o \
	objects/eval.o \
	objects/ex_cmds.o \
	objects/ex_docmd.o \
	objects/ex_getln.o \
	objects/fileio.o \
	objects/fold.o \
	objects/getchar.o \
	$(HANGULIN_OBJ) \
	objects/if_cscope.o \
	objects/main.o \
	objects/mark.o \
	objects/memfile.o \
	objects/memline.o \
	objects/menu.o \
	objects/message.o \
	objects/misc1.o \
	objects/misc2.o \
	objects/multibyte.o \
	objects/normal.o \
	objects/ops.o \
	objects/option.o \
	objects/os_unix.o \
	objects/pathdef.o \
	objects/quickfix.o \
	objects/regexp.o \
	objects/screen.o \
	objects/search.o \
	objects/syntax.o \
	$(SNIFF_OBJ) \
	objects/tag.o \
	objects/term.o \
	objects/ui.o \
	objects/undo.o \
	objects/window.o \
	$(GUI_OBJ) \
	$(PERL_OBJ) \
	$(PYTHON_OBJ) \
	$(TCL_OBJ) \
	$(RUBY_OBJ) \
	$(OS_EXTRA_OBJ) \
	$(WORKSHOP_OBJ) \
	$(WSDEBUG_OBJ)

PRO_AUTO = \
	buffer.pro \
	charset.pro \
	digraph.pro \
	edit.pro \
	eval.pro \
	ex_cmds.pro \
	ex_docmd.pro \
	ex_getln.pro \
	fileio.pro \
	fold.pro \
	getchar.pro \
	hangulin.pro \
	if_cscope.pro \
	if_python.pro \
	if_ruby.pro \
	main.pro \
	mark.pro \
	memfile.pro \
	memline.pro \
	menu.pro \
	message.pro \
	misc1.pro \
	misc2.pro \
	multibyte.pro \
	normal.pro \
	ops.pro \
	option.pro \
	os_unix.pro \
	quickfix.pro \
	regexp.pro \
	screen.pro \
	search.pro \
	syntax.pro \
	tag.pro \
	term.pro \
	termlib.pro \
	ui.pro \
	undo.pro \
	version.pro \
	window.pro \
	gui_beval.pro \
	workshop.pro \
	$(ALL_GUI_PRO) \
	$(TCL_PRO)

PRO_MANUAL = os_amiga.pro os_msdos.pro os_win16.pro os_win32.pro \
	os_mswin.pro os_beos.pro os_vms.pro os_riscos.pro $(PERL_PRO)

# Default target is making the executable and tools
all: $(VIMTARGET) $(TOOLS) languages

tools: $(TOOLS)

# Run configure with all the setting from above.
#
# Note: auto/config.h doesn't depend on configure, because running configure
# doesn't always update auto/config.h.  The timestamp isn't changed if the
# file contents didn't change (to avoid recompiling everything).  Including a
# dependency on auto/config.h would cause running configure each time when
# auto/config.h isn't updated.  The dependency on auto/config.mk should make
# sure configure is run when it's needed.
#
config auto/config.mk: auto/configure
	GUI_INC_LOC="$(GUI_INC_LOC)" GUI_LIB_LOC="$(GUI_LIB_LOC)" \
		CC="$(CC)" CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" $(CONF_SHELL) srcdir="$(srcdir)" \
		./configure $(CONF_OPT_GUI) $(CONF_OPT_X) \
		$(CONF_OPT_PERL) $(CONF_OPT_PYTHON) \
		$(CONF_OPT_TCL) $(CONF_OPT_RUBY) \
		$(CONF_OPT_CSCOPE) $(CONF_OPT_MULTIBYTE) $(CONF_OPT_INPUT) \
		$(CONF_OPT_OUTPUT) $(CONF_OPT_GPM) $(CONF_OPT_WORKSHOP) \
		$(CONF_OPT_SNIFF) \
		$(CONF_OPT_FEAT) $(CONF_TERM_LIB) $(CONF_ARGS)

# When configure.in has changed, run autoconf to produce configure.
# If you don't have autoconf, use the existing configure script.
#
# Two tricks are required to make autoconf put its output in the "auto" dir:
# - Temporarily move the ./configure script to ./configure.save.  Don't
#   overwrite it, it's probably the result of an aborted autoconf.
# - Use sed to change ./config.log to auto/config.log in the configure script.
auto/configure: configure.in
	if test ! -f configure.save; then mv configure configure.save; fi
	autoconf
	sed -e 's+\./config.log+auto/config.log+' configure > auto/configure
	chmod 755 auto/configure
	mv -f configure.save configure

# Re-execute this Makefile to include the new auto/config.mk produced by
# configure Only used when typing "make" with a fresh auto/config.mk.
myself:
	$(MAKE) -f Makefile all


# The normal command to compile a .c file to its .o file.
CCC = $(CC) -c -I$(srcdir) $(ALL_CFLAGS)


# Link the target for normal use or debugging.
# A shell script is used to try linking without unneccesary libraries.
$(VIMTARGET): auto/config.mk objects $(OBJ) version.c version.h
	$(CCC) version.c -o objects/version.o
	@LINK="$(PURIFY) $(SHRPENV) $(CClink) $(LDFLAGS) $(ALL_LIB_DIRS) \
		-o $(VIMTARGET) $(OBJ) objects/version.o $(ALL_LIBS)" \
		MAKE="$(MAKE)" sh $(srcdir)/link.sh

xxd/xxd$(SUFFIX): xxd/xxd.c
	cd xxd; CC="$(CC)" CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" \
		$(MAKE) -f Makefile

# build the language specific files if they were unpacked
languages:
	if test -n "$(MAKEMO)"; then \
		cd $(PODIR); $(MAKE) prefix=$(prefix); \
	fi

# Update the *.po files for changes in the sources.  Only run manually.
update-po:
	cd $(PODIR); $(MAKE) prefix=$(prefix) update-po

# Generate function prototypes.  This is not needed to compile vim, but if
# you want to use it, cproto is out there on the net somewhere -- Webb
#
# When generating os_amiga.pro, os_msdos.pro and os_win32.pro there will be a
# few include files that can not be found, that's OK.

proto: $(PRO_AUTO) $(PRO_MANUAL)

### Would be nice if this would work for "normal" make.
### Currently it only works for (Free)BSD make.
#$(PRO_AUTO): $$(*F).c
#	cproto $(PFLAGS) -DFEAT_GUI $(*F).c > $@

# Always define FEAT_GUI.  This will generate a few warnings if it's also
# defined in auto/config.h, you can ignore that.
.c.pro:
	cproto $(PFLAGS) -DFEAT_GUI $< > proto/$@

os_amiga.pro: os_amiga.c
	cproto $(PFLAGS) -DAMIGA -UHAVE_CONFIG_H -DBPTR=char* os_amiga.c > proto/os_amiga.pro

os_msdos.pro: os_msdos.c
	cproto $(PFLAGS) -DMSDOS -UHAVE_CONFIG_H os_msdos.c > proto/os_msdos.pro

os_win16.pro: os_win16.c
	cproto $(PFLAGS) -DWIN16 -UHAVE_CONFIG_H os_win16.c > proto/os_win16.pro

os_win32.pro: os_win32.c
	cproto $(PFLAGS) -DWIN32 -UHAVE_CONFIG_H os_win32.c > proto/os_win32.pro

os_mswin.pro: os_mswin.c
	cproto $(PFLAGS) -DWIN16 -DWIN32 -UHAVE_CONFIG_H os_mswin.c > proto/os_mswin.pro

os_beos.pro: os_beos.c
	cproto $(PFLAGS) -D__BEOS__ -UHAVE_CONFIG_H os_beos.c > proto/os_beos.pro

os_vms.pro: os_vms.c
# must use os_vms_conf.h for auto/config.h
	mv auto/config.h auto/config.h.save
	cp os_vms_conf.h auto/config.h
	cproto $(PFLAGS) -DVMS -UFEAT_GUI_ATHENA -UFEAT_GUI_MOTIF -UFEAT_GUI_GTK os_vms.c > proto/os_vms.pro
	rm auto/config.h
	mv auto/config.h.save auto/config.h

# if_perl.pro is special: Use the generated if_perl.c for input and remove
# prototypes for local functions.
if_perl.pro: auto/if_perl.c
	cproto $(PFLAGS) -DFEAT_GUI auto/if_perl.c | sed "/_VI/d" > proto/$@


notags:
	-rm -f tags

# Note: tags is made for the currently configured version, can't include both
#	Motif and Athena GUI
# You can ignore error messages for missing files.
tags TAGS: notags
	$(TAGPRG) $(TAGS_SRC) $(TAGS_INCL)

# Make a highlight file for types.  Requires Exuberant ctags and awk
types: types.vim
types.vim: $(TAGS_SRC) $(TAGS_INCL)
	ctags -i=gstuS -o- $(TAGS_SRC) $(TAGS_INCL) |\
		awk 'BEGIN{printf("syntax keyword Type\t")}\
			{printf("%s ", $$1)}END{print ""}' > $@

# Execute the test scripts.  Run these after compiling Vim, before installing.
#
# This will produce a lot of garbage on your screen, including a few error
# messages.  Don't worry about that.
# If there is a real error, there will be a difference between "test.out" and
# a "test99.ok" file.
# If everything is allright, the final message will be "ALL DONE".
#
test check:
	cd testdir; $(MAKE) -f Makefile $(GUI_TESTTARGET)

testclean:
	-rm -f testdir/*.out testdir/test.log

#
# Avoid overwriting an existing executable, somebody might be running it and
# overwriting it could cause it to crash.  Deleting it is OK, it won't be
# really deleted until all running processes for it have exited.  It is
# renamed first, in case the deleting doesn't work.
#
# If you want to keep an older version, rename it before running "make
# install".
#
install: installvim installtools install-languages

installvim: installvimbin installruntime installlinks installhelplinks installmacros installtutor

installvimbin: $(VIMTARGET) $(exec_prefix) $(BINDIR)
	-if test -f $(BINDIR)/$(VIMTARGET); then \
	  mv -f $(BINDIR)/$(VIMTARGET) $(BINDIR)/$(VIMNAME).rm; \
	  rm -f $(BINDIR)/$(VIMNAME).rm; \
	fi
	$(INSTALL_PROG) $(VIMTARGET) $(BINDIR)
	$(STRIP) $(BINDIR)/$(VIMTARGET)
	chmod $(BINMOD) $(BINDIR)/$(VIMTARGET)
# may create a link to the new executable from /usr/bin/vi
	-$(LINKIT)

# install the help files; first adjust the contents for the location
installruntime: $(HELPSOURCE)/vim.1 $(MANSUBDIR) $(VIMLOC) $(VIMRTLOC) \
		$(HELPSUBLOC) $(SYNSUBLOC) $(INDSUBLOC) $(FTPLUGSUBLOC) \
		$(PLUGSUBLOC) $(TUTORSUBLOC)
	@echo generating $(MANSUBDIR)/$(VIMNAME).1
	@sed -e s+/usr/local/lib/vim+$(VIMLOC)+ \
		-e s+$(VIMLOC)/doc+$(HELPENDLOC)+ \
		-e s+$(VIMLOC)/syntax+$(SYNTAXENDLOC)+ \
		-e s+$(VIMLOC)/tutor+$(TUTORENDLOC)+ \
		-e s+$(VIMLOC)/vimrc+$(VIMRCENDLOC)/vimrc+ \
		-e s+$(VIMLOC)/gvimrc+$(VIMRCENDLOC)/gvimrc+ \
		-e s+$(VIMLOC)/menu.vim+$(SCRIPTENDLOC)/menu.vim+ \
		-e s+$(VIMLOC)/bugreport.vim+$(SCRIPTENDLOC)/bugreport.vim+ \
		-e s+$(VIMLOC)/filetype.vim+$(SCRIPTENDLOC)/filetype.vim+ \
		-e s+$(VIMLOC)/ftoff.vim+$(SCRIPTENDLOC)/ftoff.vim+ \
		-e s+$(VIMLOC)/scripts.vim+$(SCRIPTENDLOC)/scripts.vim+ \
		-e s+$(VIMLOC)/optwin.vim+$(SCRIPTENDLOC)/optwin.vim+ \
		$(HELPSOURCE)/vim.1 > $(MANSUBDIR)/$(VIMNAME).1
	chmod $(MANMOD) $(MANSUBDIR)/$(VIMNAME).1
	@echo generating $(MANSUBDIR)/$(VIMNAME)tutor.1
	@sed -e s+/usr/local/lib/vim+$(VIMLOC)+ \
		-e s+$(VIMLOC)/tutor+$(TUTORENDLOC)+ \
		$(HELPSOURCE)/vimtutor.1 > $(MANSUBDIR)/$(VIMNAME)tutor.1
	chmod $(MANMOD) $(MANSUBDIR)/$(VIMNAME)tutor.1
	cd $(HELPSOURCE); $(INSTALL_DATA) *.txt tags $(HELPSUBLOC)
	cd $(HELPSUBLOC); chmod $(HELPMOD) *.txt tags
	$(INSTALL_DATA)  $(HELPSOURCE)/*.pl $(HELPSUBLOC)
	chmod $(SCRIPTMOD) $(HELPSUBLOC)/*.pl
# install the menu file
	$(INSTALL_DATA) $(SCRIPTSOURCE)/menu.vim $(SYS_MENU_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_MENU_FILE)
# install the bugreport file
	$(INSTALL_DATA) $(SCRIPTSOURCE)/bugreport.vim $(SYS_BUGR_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_BUGR_FILE)
# install the example vimrc files
	$(INSTALL_DATA) $(SCRIPTSOURCE)/vimrc_example.vim $(SCRIPTLOC)
	chmod $(VIMSCRIPTMOD) $(SCRIPTLOC)/vimrc_example.vim
	$(INSTALL_DATA) $(SCRIPTSOURCE)/gvimrc_example.vim $(SCRIPTLOC)
	chmod $(VIMSCRIPTMOD) $(SCRIPTLOC)/gvimrc_example.vim
# install the file type detection files
	$(INSTALL_DATA) $(SCRIPTSOURCE)/filetype.vim $(SYS_FILETYPE_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_FILETYPE_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/ftoff.vim $(SYS_FTOFF_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_FTOFF_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/scripts.vim $(SYS_SCRIPTS_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_SCRIPTS_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/ftplugin.vim $(SYS_FTPLUGIN_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_FTPLUGIN_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/ftplugof.vim $(SYS_FTPLUGOF_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_FTPLUGOF_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/indent.vim $(SYS_INDENT_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_INDENT_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/indoff.vim $(SYS_INDOFF_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_INDOFF_FILE)
	$(INSTALL_DATA) $(SCRIPTSOURCE)/optwin.vim $(SYS_OPTWIN_FILE)
	chmod $(VIMSCRIPTMOD) $(SYS_OPTWIN_FILE)
# install the syntax files
	cd $(SYNSOURCE); $(INSTALL_DATA) *.vim README.txt $(SYNSUBLOC)
	cd $(SYNSUBLOC); chmod $(HELPMOD) *.vim README.txt
# install the indent files
	cd $(INDSOURCE); $(INSTALL_DATA) *.vim $(INDSUBLOC)
	cd $(INDSUBLOC); chmod $(HELPMOD) *.vim
# install the standard plugin files
	cd $(PLUGSOURCE); $(INSTALL_DATA) *.vim README.txt $(PLUGSUBLOC)
	cd $(PLUGSUBLOC); chmod $(HELPMOD) *.vim README.txt
# install the ftplugin files
	cd $(FTPLUGSOURCE); $(INSTALL_DATA_R) * $(FTPLUGSUBLOC)
	cd $(FTPLUGSUBLOC); chmod $(DIRMOD) *
	cd $(FTPLUGSUBLOC); chmod $(HELPMOD) */*.vim

installmacros: $(MACROSOURCE) $(VIMLOC) $(VIMRTLOC) $(MACROSUBLOC)
	$(INSTALL_DATA_R) $(MACROSOURCE)/* $(MACROSUBLOC)
	chmod $(DIRMOD) `find $(MACROSUBLOC) -type d -print`
	chmod $(FILEMOD) `find $(MACROSUBLOC) -type f -print`

# install the tutor files
installtutor: $(TUTORSOURCE) $(VIMLOC) $(VIMRTLOC) $(TUTORSUBLOC)
	$(INSTALL_DATA) vimtutor $(BINDIR)
	chmod $(SCRIPTMOD) $(BINDIR)/vimtutor
	-$(INSTALL_DATA) $(TUTORSOURCE)/* $(TUTORSUBLOC)
	chmod $(HELPMOD) $(TUTORSUBLOC)/*

# install helper program xxd
installtools: $(TOOLS) $(exec_prefix) $(BINDIR) $(MANSUBDIR) \
		$(TOOLSSOURCE) $(VIMLOC) $(VIMRTLOC) $(TOOLSSUBLOC)
	if test -f $(BINDIR)/xxd$(SUFFIX); then \
	  mv -f $(BINDIR)/xxd$(SUFFIX) $(BINDIR)/xxd.rm; \
	  rm -f $(BINDIR)/xxd.rm; \
	fi
	$(INSTALL_PROG) xxd/xxd$(SUFFIX) $(BINDIR)
	$(STRIP) $(BINDIR)/xxd$(SUFFIX)
	chmod $(BINMOD) $(BINDIR)/xxd$(SUFFIX)
	$(INSTALL_DATA) $(HELPSOURCE)/xxd.1 $(MANSUBDIR)
	chmod $(MANMOD) $(MANSUBDIR)/xxd.1
# install the runtime tools
	$(INSTALL_DATA_R) $(TOOLSSOURCE)/* $(TOOLSSUBLOC)
	-chmod $(FILEMOD) $(TOOLSSUBLOC)/*
	-chmod $(SCRIPTMOD) `grep -l "^#!" $(TOOLSSUBLOC)/*`

# install the language specific files, if they were unpacked
install-languages: languages $(LANGSUBLOC)
	if test -n "$(MAKEMO)"; then \
	   cd $(PODIR); $(MAKE) prefix=$(prefix) LOCALEDIR=$(LANGSUBLOC) \
	   INSTALL_DATA=$(INSTALL_DATA) FILEMOD=$(FILEMOD) install; \
	fi
	if test -d $(LANGSOURCE); then \
	   $(INSTALL_DATA) $(LANGSOURCE)/*.vim $(LANGSUBLOC); \
	   chmod $(FILEMOD) $(LANGSUBLOC)/*.vim; \
	fi


$(HELPSOURCE)/vim.1 $(MACROSOURCE) $(TOOLSSOURCE):
	@echo Runtime files not found.
	@echo You need to unpack the runtime archive before running "make install".
	test -f error

$(exec_prefix) $(BINDIR) $(MANSUBDIR) $(VIMLOC) $(VIMRTLOC) $(HELPSUBLOC) \
		$(SYNSUBLOC) $(INDSUBLOC) $(FTPLUGSUBLOC) $(LANGSUBLOC) \
		$(MACROSUBLOC) $(TOOLSSUBLOC) $(TUTORSUBLOC) $(PLUGSUBLOC):
	-$(SHELL) ./mkinstalldirs $@
	-chmod $(DIRMOD) $@

# create links from various names to vim.  This is only done when the links
# (or executables with the same name) don't exist yet.
installlinks: $(GUI_TARGETS) $(BINDIR)/$(EXTARGET) $(BINDIR)/$(VIEWTARGET) $(BINDIR)/$(RVIMTARGET) $(BINDIR)/$(RVIEWTARGET)

$(BINDIR)/$(EXTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(EXTARGET)

$(BINDIR)/$(VIEWTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(VIEWTARGET)

$(BINDIR)/$(GVIMTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(GVIMTARGET)

$(BINDIR)/$(GVIEWTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(GVIEWTARGET)

$(BINDIR)/$(RVIMTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(RVIMTARGET)

$(BINDIR)/$(RVIEWTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(RVIEWTARGET)

$(BINDIR)/$(RGVIMTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(RGVIMTARGET)

$(BINDIR)/$(RGVIEWTARGET):
	cd $(BINDIR); ln -s $(VIMTARGET) $(RGVIEWTARGET)

# create links for the manual pages with various names to vim.	This is only
# done when the links (or manpages with the same name) don't exist yet.
installhelplinks: $(GUI_MAN_TARGETS) $(MANSUBDIR)/$(EXNAME).1 $(MANSUBDIR)/$(VIEWNAME).1 $(MANSUBDIR)/$(RVIMNAME).1 $(MANSUBDIR)/$(RVIEWNAME).1

$(MANSUBDIR)/$(EXNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(EXNAME).1

$(MANSUBDIR)/$(VIEWNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(VIEWNAME).1

$(MANSUBDIR)/$(GVIMNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(GVIMNAME).1

$(MANSUBDIR)/$(GVIEWNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(GVIEWNAME).1

$(MANSUBDIR)/$(RVIMNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(RVIMNAME).1

$(MANSUBDIR)/$(RVIEWNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(RVIEWNAME).1

$(MANSUBDIR)/$(RGVIMNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(RGVIMNAME).1

$(MANSUBDIR)/$(RGVIEWNAME).1:
	cd $(MANSUBDIR); ln -s $(VIMNAME).1 $(RGVIEWNAME).1

uninstall: uninstall_runtime
	-rm -f $(BINDIR)/$(VIMTARGET)
	-rm -f $(MANSUBDIR)/$(VIMNAME).1 $(MANSUBDIR)/$(VIMNAME)tutor.1
	-rm -f $(BINDIR)/vimtutor
	-rm -f $(BINDIR)/xxd$(SUFFIX) $(MANSUBDIR)/xxd.1
	-rm -f $(BINDIR)/$(EXTARGET) $(BINDIR)/$(VIEWTARGET)
	-rm -f $(BINDIR)/$(GVIMTARGET) $(BINDIR)/$(GVIEWTARGET)
	-rm -f $(BINDIR)/$(RVIMTARGET) $(BINDIR)/$(RVIEWTARGET)
	-rm -f $(BINDIR)/$(RGVIMTARGET) $(BINDIR)/$(RGVIEWTARGET)
	-rm -f $(MANSUBDIR)/$(EXNAME).1 $(MANSUBDIR)/$(VIEWNAME).1
	-rm -f $(MANSUBDIR)/$(GVIMNAME).1 $(MANSUBDIR)/$(GVIEWNAME).1
	-rm -f $(MANSUBDIR)/$(RVIMNAME).1 $(MANSUBDIR)/$(RVIEWNAME).1
	-rm -f $(MANSUBDIR)/$(RGVIMNAME).1 $(MANSUBDIR)/$(RGVIEWNAME).1

# Note: the "rmdir" will fail if any files were added after "make install"
uninstall_runtime:
	-rm -f $(HELPSUBLOC)/*.txt $(HELPSUBLOC)/tags $(HELPSUBLOC)/*.pl
	-rm -f $(SYS_MENU_FILE) $(SYS_BUGR_FILE)
	-rm -f $(SCRIPTLOC)/gvimrc_example.vim $(SCRIPTLOC)/vimrc_example.vim
	-rm -f $(SYS_FILETYPE_FILE) $(SYS_FTOFF_FILE) $(SYS_SCRIPTS_FILE)
	-rm -f $(SYS_INDOFF_FILE) $(SYS_INDENT_FILE)
	-rm -f $(SYS_FTPLUGOF_FILE) $(SYS_FTPLUGIN_FILE)
	-rm -f $(SYS_OPTWIN_FILE)
	-rm -f $(SYNSUBLOC)/*.vim $(INDSUBLOC)/*.vim
	-rm -rf $(MACROSUBLOC)
	-rm -rf $(TUTORSUBLOC)
	-rm -rf $(TOOLSSUBLOC)
	-rm -rf $(LANGSUBLOC)
	-rmdir $(HELPSUBLOC) $(SYNSUBLOC) $(INDSUBLOC)
	-rm -rf $(FTPLUGSUBLOC)/*/*.vim $(FTPLUGSUBLOC)/README.txt
	-rm -f $(PLUGSUBLOC)/*.vim $(PLUGSUBLOC)/README.txt
	-rmdir $(FTPLUGSUBLOC)/* $(FTPLUGSUBLOC) $(PLUGSUBLOC) $(VIMRTLOC)

# Clean up all the files that have been produced, except configure's.
# We support common typing mistakes for Juergen! :-)
clean celan: testclean
	-rm -f *.o objects/* core $(VIMTARGET).core $(VIMTARGET) xxd/*.o
	-rm -f $(TOOLS) auto/osdef.h auto/pathdef.c auto/if_perl.c
	-rm -f conftest* *~ auto/link.sed
	if test -d $(PODIR); then \
		cd $(PODIR); $(MAKE) prefix=$(prefix) clean; \
	fi

# Make a shadow directory for compilation on another system or with different
# features.
SHADOWDIR = shadow

shadow:	runtime pixmaps
	mkdir $(SHADOWDIR)
	cd $(SHADOWDIR); ln -s ../*.[ch] ../*.in ../*.sh ../*.xs ../*.xbm ../toolcheck ../proto ../vimtutor ../mkinstalldirs .
	mkdir $(SHADOWDIR)/auto
	cd $(SHADOWDIR)/auto; ln -s ../../auto/configure .
	cd $(SHADOWDIR); rm -f auto/link.sed
	cp Makefile configure $(SHADOWDIR)
	@echo generating auto/config.mk
	@echo "the first targets to make vim are: scratch config myself" > $(SHADOWDIR)/auto/config.mk
	@echo "srcdir = ." >> $(SHADOWDIR)/auto/config.mk
	@echo "VIMNAME = vim" >> $(SHADOWDIR)/auto/config.mk
	@echo "EXNAME = ex" >> $(SHADOWDIR)/auto/config.mk
	@echo "VIEWNAME = view" >> $(SHADOWDIR)/auto/config.mk
	mkdir $(SHADOWDIR)/xxd
	cd $(SHADOWDIR)/xxd; ln -s ../../xxd/*.[ch] ../../xxd/Make* .
	mkdir $(SHADOWDIR)/testdir
	cd $(SHADOWDIR)/testdir; ln -s ../../testdir/Makefile \
				 ../../testdir/vimrc.unix \
				 ../../testdir/*.in \
				 ../../testdir/*.ok .

# Link needed for doing "make install" in a shadow directory.
runtime:
	-ln -s ../runtime .

# Link needed for doing "make" using GTK in a shadow directory.
pixmaps:
	-ln -s ../pixmaps .

# Start configure from scratch
scrub scratch:
	-rm -f auto/config.status auto/config.cache auto/config.log auto/config.h
	touch auto/config.h

distclean: clean scratch
	@echo generating auto/config.mk
	@echo "the first targets to make vim are: scratch config myself" > auto/config.mk
	@echo "srcdir = ." >> auto/config.mk
	@echo "VIMNAME = vim" >> auto/config.mk
	@echo "EXNAME = ex" >> auto/config.mk
	@echo "VIEWNAME = view" >> auto/config.mk

dist: distclean tar

mdepend:
	-@rm -f Makefile~
	cp Makefile Makefile~
	sed -e '/\#\#\# Dependencies/q' < Makefile > tmp_make
	@for i in $(ALL_SRC) ; do \
	  echo "$$i" ; \
	  echo `echo "$$i" | sed -e 's/[^ ]*\.c$$/objects\/\1.o/'`": $$i" `\
	    $(CPP) $$i |\
	    grep '^# .*"\./.*\.h"' |\
	    sort -t'"' -u +1 -2 |\
	    sed -e 's/.*"\.\/\(.*\)".*/\1/'\
	    ` >> tmp_make ; \
	done
	mv tmp_make Makefile

depend:
	-@rm -f Makefile~
	cp Makefile Makefile~
	sed -e '/\#\#\# Dependencies/q' < Makefile > tmp_make
	-for i in $(ALL_SRC); do echo $$i; \
		$(CPP_DEPEND) $$i | \
		sed -e 's+^\([^ ]*\.o\)+objects/\1+' >> tmp_make; done
	mv tmp_make Makefile

lint:
	lint $(LINT_OPTIONS) $(LINT_CFLAGS) -DUSE_SNIFF -DHANGUL_INPUT $(LINT_SRC)

###########################################################################

.c.o:
	$(CCC) $<

.cc.o:
	$(CCC) $<

auto/if_perl.c: if_perl.xs
	$(PERL) -e 'unless ( $$] >= 5.005 ) { for (qw(na defgv errgv)) { print "#define PL_$$_ $$_\n" }}' > $@
	$(PERL) $(PERLLIB)/ExtUtils/xsubpp -prototypes -typemap \
	    $(PERLLIB)/ExtUtils/typemap if_perl.xs >> $@

auto/osdef.h: auto/config.h osdef.sh osdef1.h.in osdef2.h.in
	CC="$(CC) $(ALL_CFLAGS)" srcdir=$(srcdir) sh $(srcdir)/osdef.sh

auto/pathdef.c: Makefile auto/config.mk
	-@echo creating $@
	-@echo '/* pathdef.c */' > $@
	-@echo '/* This file is automatically created by Makefile' >> $@
	-@echo ' * DO NOT EDIT!  Change Makefile only. */' >> $@
	-@echo '#include "vim.h"' >> $@
	-@echo 'char_u *default_vim_dir = (char_u *)"$(VIMRCLOC)";' >> $@
	-@echo 'char_u *default_vimruntime_dir = (char_u *)"$(VIMRUNTIMEDIR)";' >> $@
	-@echo 'char_u *all_cflags = (char_u *)"$(CC) -c -I$(srcdir) $(ALL_CFLAGS)";' >>  $@
	-@echo 'char_u *all_lflags = (char_u *)"$(CC) $(LDFLAGS) $(ALL_LIB_DIRS) -o $(VIMTARGET) $(ALL_LIBS) ";' >>  $@
	-@echo 'char_u *compiled_user = (char_u *)"' | tr -d $(NL) >> $@
	-@whoami | tr -d $(NL) >> $@
	-@echo '";' >> $@
	-@echo 'char_u *compiled_sys = (char_u *)"' | tr -d $(NL) >> $@
	-@hostname | tr -d $(NL) >> $@
	-@echo '";' >> $@
	-@sh $(srcdir)/pathdef.sh

# All the object files are put in the "objects" directory.  Since not all make
# commands understand putting object files in another directory, it must be
# specified for each file separately.

objects:
	mkdir objects

objects/buffer.o: buffer.c
	$(CCC) -o $@ buffer.c

objects/charset.o: charset.c
	$(CCC) -o $@ charset.c

objects/digraph.o: digraph.c
	$(CCC) -o $@ digraph.c

objects/edit.o: edit.c
	$(CCC) -o $@ edit.c

objects/eval.o: eval.c
	$(CCC) -o $@ eval.c

objects/ex_cmds.o: ex_cmds.c
	$(CCC) -o $@ ex_cmds.c

objects/ex_docmd.o: ex_docmd.c
	$(CCC) -o $@ ex_docmd.c

objects/ex_getln.o: ex_getln.c
	$(CCC) -o $@ ex_getln.c

objects/fileio.o: fileio.c
	$(CCC) -o $@ fileio.c

objects/fold.o: fold.c
	$(CCC) -o $@ fold.c

objects/getchar.o: getchar.c
	$(CCC) -o $@ getchar.c

objects/gui.o: gui.c
	$(CCC) -o $@ gui.c

objects/gui_at_fs.o: gui_at_fs.c
	$(CCC) -o $@ gui_at_fs.c

objects/gui_at_sb.o: gui_at_sb.c
	$(CCC) -o $@ gui_at_sb.c

objects/gui_athena.o: gui_athena.c
	$(CCC) -o $@ gui_athena.c

objects/gui_beval.o: gui_beval.c
	$(CCC) -o $@ gui_beval.c

objects/gui_gtk.o: gui_gtk.c
	$(CCC) -o $@ gui_gtk.c

objects/gui_gtk_f.o: gui_gtk_f.c
	$(CCC) -o $@ gui_gtk_f.c

objects/gui_gtk_x11.o: gui_gtk_x11.c
	$(CCC) -o $@ gui_gtk_x11.c

objects/gui_motif.o: gui_motif.c
	$(CCC) -o $@ gui_motif.c

objects/gui_x11.o: gui_x11.c
	$(CCC) -o $@ gui_x11.c

objects/hangulin.o: hangulin.c
	$(CCC) -o $@ hangulin.c

objects/if_cscope.o: if_cscope.c
	$(CCC) -o $@ if_cscope.c

objects/if_perl.o: auto/if_perl.c
	$(CCC) -o $@ auto/if_perl.c

objects/if_perlsfio.o: if_perlsfio.c
	$(CCC) -o $@ if_perlsfio.c

objects/if_python.o: if_python.c
	$(CCC) -o $@ if_python.c

objects/if_ruby.o: if_ruby.c
	$(CCC) -o $@ if_ruby.c

objects/if_sniff.o: if_sniff.c
	$(CCC) -o $@ if_sniff.c

objects/if_tcl.o: if_tcl.c
	$(CCC) -o $@ if_tcl.c

objects/integration.o: integration.c
	$(CCC) -o $@ integration.c

objects/main.o: main.c
	$(CCC) -o $@ main.c

objects/mark.o: mark.c
	$(CCC) -o $@ mark.c

objects/memfile.o: memfile.c
	$(CCC) -o $@ memfile.c

objects/memline.o: memline.c
	$(CCC) -o $@ memline.c

objects/menu.o: menu.c
	$(CCC) -o $@ menu.c

objects/message.o: message.c
	$(CCC) -o $@ message.c

objects/misc1.o: misc1.c
	$(CCC) -o $@ misc1.c

objects/misc2.o: misc2.c
	$(CCC) -o $@ misc2.c

objects/multibyte.o: multibyte.c
	$(CCC) -o $@ multibyte.c

objects/normal.o: normal.c
	$(CCC) -o $@ normal.c

objects/ops.o: ops.c
	$(CCC) -o $@ ops.c

objects/option.o: option.c
	$(CCC) -o $@ option.c

objects/os_beos.o: os_beos.c
	$(CCC) -o $@ os_beos.c

objects/os_unix.o: os_unix.c
	$(CCC) -o $@ os_unix.c

objects/pathdef.o: auto/pathdef.c
	$(CCC) -o $@ auto/pathdef.c

objects/py_config.o: $(PYTHON_CONFDIR)/config.c
	$(CCC) -o $@ $(PYTHON_CONFDIR)/config.c \
		-I$(PYTHON_CONFDIR) -DHAVE_CONFIG_H -DNO_MAIN

objects/py_getpath.o: $(PYTHON_CONFDIR)/getpath.c
	$(CCC) -o $@ $(PYTHON_CONFDIR)/getpath.c \
		-I$(PYTHON_CONFDIR) -DHAVE_CONFIG_H -DNO_MAIN \
		$(PYTHON_GETPATH_CFLAGS)

objects/pty.o: pty.c
	$(CCC) -o $@ pty.c

objects/quickfix.o: quickfix.c
	$(CCC) -o $@ quickfix.c

objects/regexp.o: regexp.c
	$(CCC) -o $@ regexp.c

objects/screen.o: screen.c
	$(CCC) -o $@ screen.c

objects/search.o: search.c
	$(CCC) -o $@ search.c

objects/syntax.o: syntax.c
	$(CCC) -o $@ syntax.c

objects/tag.o: tag.c
	$(CCC) -o $@ tag.c

objects/term.o: term.c
	$(CCC) -o $@ term.c

objects/ui.o: ui.c
	$(CCC) -o $@ ui.c

objects/undo.o: undo.c
	$(CCC) -o $@ undo.c

objects/window.o: window.c
	$(CCC) -o $@ window.c

objects/workshop.o: workshop.c
	$(CCC) -o $@ workshop.c

objects/wsdebug.o: wsdebug.c
	$(CCC) -o $@ wsdebug.c


Makefile:
	@echo The name of the makefile MUST be "Makefile" (with capital M)!!!!

###############################################################################
### (automatically generated by 'make depend')
### Dependencies:
objects/buffer.o: buffer.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/charset.o: charset.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/digraph.o: digraph.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/edit.o: edit.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/eval.o: eval.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h version.h
objects/ex_cmds.o: ex_cmds.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/ex_docmd.o: ex_docmd.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/ex_getln.o: ex_getln.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/fileio.o: fileio.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/fold.o: fold.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/getchar.o: getchar.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/if_cscope.o: if_cscope.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h if_cscope.h
objects/main.o: main.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h farsi.c
objects/mark.o: mark.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/memfile.o: memfile.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/memline.o: memline.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/menu.o: menu.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/message.o: message.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/misc1.o: misc1.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h version.h
objects/misc2.o: misc2.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/multibyte.o: multibyte.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/normal.o: normal.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/ops.o: ops.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/option.o: option.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/os_unix.o: os_unix.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h os_unixx.h
objects/pathdef.o: auto/pathdef.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/quickfix.o: quickfix.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/regexp.o: regexp.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/screen.o: screen.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/search.o: search.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/syntax.o: syntax.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/tag.o: tag.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/term.o: term.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/ui.o: ui.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/undo.o: undo.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/version.o: version.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h version.h
objects/window.o: window.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/gui.o: gui.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/gui_gtk.o: gui_gtk.c gui_gtk_f.h vim.h auto/config.h feature.h \
 os_unix.h auto/osdef.h ascii.h keymap.h term.h macros.h structs.h \
 regexp.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h \
 ../pixmaps/tb_new.xpm ../pixmaps/tb_open.xpm ../pixmaps/tb_close.xpm \
 ../pixmaps/tb_save.xpm ../pixmaps/tb_print.xpm ../pixmaps/tb_cut.xpm \
 ../pixmaps/tb_copy.xpm ../pixmaps/tb_paste.xpm ../pixmaps/tb_find.xpm \
 ../pixmaps/tb_find_next.xpm ../pixmaps/tb_find_prev.xpm \
 ../pixmaps/tb_find_help.xpm ../pixmaps/tb_exit.xpm \
 ../pixmaps/tb_undo.xpm ../pixmaps/tb_redo.xpm ../pixmaps/tb_help.xpm \
 ../pixmaps/tb_macro.xpm ../pixmaps/tb_make.xpm \
 ../pixmaps/tb_save_all.xpm ../pixmaps/tb_jump.xpm \
 ../pixmaps/tb_ctags.xpm ../pixmaps/tb_load_session.xpm \
 ../pixmaps/tb_save_session.xpm ../pixmaps/tb_new_session.xpm \
 ../pixmaps/tb_blank.xpm ../pixmaps/tb_maximize.xpm \
 ../pixmaps/tb_split.xpm ../pixmaps/tb_minimize.xpm \
 ../pixmaps/tb_shell.xpm ../pixmaps/tb_replace.xpm \
 ../pixmaps/tb_vsplit.xpm ../pixmaps/tb_maxwidth.xpm \
 ../pixmaps/tb_minwidth.xpm
objects/gui_gtk_f.o: gui_gtk_f.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h gui_gtk_f.h
objects/gui_motif.o: gui_motif.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h \
 ../pixmaps/tb_new.xpm ../pixmaps/tb_open.xpm ../pixmaps/tb_close.xpm \
 ../pixmaps/tb_save.xpm ../pixmaps/tb_print.xpm ../pixmaps/tb_cut.xpm \
 ../pixmaps/tb_copy.xpm ../pixmaps/tb_paste.xpm ../pixmaps/tb_find.xpm \
 ../pixmaps/tb_find_next.xpm ../pixmaps/tb_find_prev.xpm \
 ../pixmaps/tb_find_help.xpm ../pixmaps/tb_exit.xpm \
 ../pixmaps/tb_undo.xpm ../pixmaps/tb_redo.xpm ../pixmaps/tb_help.xpm \
 ../pixmaps/tb_macro.xpm ../pixmaps/tb_make.xpm \
 ../pixmaps/tb_save_all.xpm ../pixmaps/tb_jump.xpm \
 ../pixmaps/tb_ctags.xpm ../pixmaps/tb_load_session.xpm \
 ../pixmaps/tb_save_session.xpm ../pixmaps/tb_new_session.xpm \
 ../pixmaps/tb_blank.xpm ../pixmaps/tb_maximize.xpm \
 ../pixmaps/tb_split.xpm ../pixmaps/tb_minimize.xpm \
 ../pixmaps/tb_shell.xpm ../pixmaps/tb_replace.xpm \
 ../pixmaps/tb_vsplit.xpm ../pixmaps/tb_maxwidth.xpm \
 ../pixmaps/tb_minwidth.xpm
objects/gui_athena.o: gui_athena.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h gui_at_sb.h
objects/gui_gtk_x11.o: gui_gtk_x11.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h version.h \
 gui_gtk_f.h ../runtime/vim32x32.xpm ../runtime/vim16x16.xpm \
 ../runtime/vim48x48.xpm
objects/gui_x11.o: gui_x11.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h vim_icon.xbm \
 vim_mask.xbm
objects/gui_at_sb.o: gui_at_sb.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h gui_at_sb.h
objects/gui_at_fs.o: gui_at_fs.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h gui_at_sb.h
objects/pty.o: pty.c vim.h auto/config.h feature.h os_unix.h auto/osdef.h \
 ascii.h keymap.h term.h macros.h structs.h regexp.h gui.h globals.h \
 farsi.h option.h ex_cmds.h proto.h
objects/hangulin.o: hangulin.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/if_perl.o: auto/if_perl.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/if_perlsfio.o: if_perlsfio.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/if_python.o: if_python.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/if_tcl.o: if_tcl.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h
objects/if_ruby.o: if_ruby.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h version.h
objects/if_sniff.o: if_sniff.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h os_unixx.h
objects/gui_beval.o: gui_beval.c auto/config.h feature.h vim.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h gui_beval.h \
 proto/gui_beval.pro
objects/workshop.o: workshop.c auto/config.h integration.h vim.h feature.h \
 os_unix.h auto/osdef.h ascii.h keymap.h term.h macros.h structs.h \
 regexp.h gui.h globals.h farsi.h option.h ex_cmds.h proto.h version.h \
 gui_beval.h proto/gui_beval.pro workshop.h
objects/wsdebug.o: wsdebug.c
objects/integration.o: integration.c vim.h auto/config.h feature.h os_unix.h \
 auto/osdef.h ascii.h keymap.h term.h macros.h structs.h regexp.h \
 gui.h globals.h farsi.h option.h ex_cmds.h proto.h integration.h
