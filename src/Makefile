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
#	  you might want to disable the GUI, X11, Perl and Python.
#	- Uncomment the line with --enable-gui=no if you have Motif and/or
#	  Athena but don't want to make gvim (the GUI version of Vim with nice
#	  menus and scrollbars, but makes Vim bigger and startup slower).
#	- Uncomment the line "CONF_OPT_X = --without-x" if you have X11 but
#	  want to disable using X11 libraries.  Vim won't set the window
#	  titles then.  This speeds up starting Vim.
#	- Unocmment the line with --disable-perlinterp to disable Perl, even
#	  though you have it.  Makes Vim a lot smaller.
#	- Uncomment the line with --disable-pythoninterp to disable Python,
#	  even though you have it.  Makes Vim a lot smaller.
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
#	The configuration phase creates/overwrites config.h and config.mk.
#	Configure is created with autoconf.  It can detect different features
#	of your system and act accordingly.  However, it is not correct for
#	all systems.  Check this:
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
#	  config.h and config.mk, before starting the actual build phase. If
#	  possible edit this Makefile, rather than config.mk -- especially
#	  look at the definition of VIMLOC below. Note that the configure
#	  phase overwrites config.mk and config.h again.
#	- If you get error messages, find out what is wrong and try to correct
#	  it in this Makefile. You may need to do "make distclean" when you
#	  change anything that configure uses (e.g. switching from an old C
#	  compiler to an ANSI C compiler). Only when ./configure does
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
#	"pathdef.c" will be compiled again after changing this to make the
#	executable know where the help files are located.
#	Note that any existing executable and documentation is removed or
#	overwritten.  If you want to keep it you will have to make a backup
#	copy first.
#	If you don't want to install everything, there are other targets:
#		make installvim		only installs Vim, not the tools
#		make installvimbin	only installs the Vim executable
#		make installvimhelp	only installs the Vim help files
#		make installlinks	only installs the Vim binary links
#		make installhelplinks	only installs the Vim manpage links
#		make installtools	only installs the tools ctags and xxd

### This Makefile has been succesfully tested on these systems.
### Check the (*) column for remarks, listed below.
### Later code changes may cause small problems, otherwise Vim is supposed to
### compile and run without problems.

#system:	      configurations:		     version (*) tested by:
#-------------	      ------------------------	     -------  -  ----------
#AIX 3.2.5	      cc (not gcc)   -			4.5  (M) Will Fiveash
#AIX 4		      cc	     +X11 -GUI		3.27 (4) Axel Kielhorn
#AIX 4.1.4	      cc	     +X11 +GUI		4.5  (5) Nico Bakker
#AIX_4.1              gcc 2.7.2      +X11 +Motif        5.0s     Karsten Sievert
#AIX 4.1.1	      cc	     +X11 +GUI		5.0c	 Jeff Walker
#AIX 4.2              gcc 2.7.2.1    +X11 +GUI_Motif    5.0w     D. Rothkamp
#A/UX 3.1.1	      gcc	     +X11		4.0  (6) Jim Jagielski
#BeOS PR	      mwcc DR3                        	5.0n (T) Olaf Seibert
#BSDI 2.1 (x86)       shlicc2 gcc-2.6.3 -X11 X11R6	4.5  (1) Jos Backus
#BSD/OS 3.0 (x86)     gcc gcc-2.7.2.1 -X11 X11R6	4.6c (1) Jos Backus
#DG/UX 5.4*           gcc 2.5.8      GUI                5.0e (H) Jonas Schlein
#DG/UX 5.4R4.20       gcc 2.7.2      GUI                5.0s (H) Rocky Olive
#HP-UX (most)	      c89 cc				5.0v (2) Bram Moolenaar
#HP-UX_10.20          gcc 2.7.2.1    +X11 +Motif        5.0s     Karsten Sievert
#Irix 6.3 (O2)	      cc	     ?			4.5  (L) Edouard Poor
#Irix 6.4             cc             ?                  5.0m (S) Rick Sayre
#Irix 64 bit						4.5  (K) Jon Wright
#FreeBSD 2.1.6	      gcc-2.6.3      X11R6 Moo-Tiff	5.0c	 Bram Moolenaar
#FreeBSD 2.1.6	      gcc-2.6.3      +X11 -GUI Athena	5.0c	 Bram Moolenaar
#FreeBSD 2.2.1	      gcc-2.7.2.1    X11R6 Moo-Tiff	5.0s	 Bram Moolenaar
#FreeBSD 2.2.1	      gcc-2.7.2.1    +X11 -GUI Athena	5.0s	 Bram Moolenaar
#FreeBSD 2.2.5	      gcc-2.7.2.1    X11R6 Moo-Tiff	5.0v	 Bram Moolenaar
#FreeBSD 2.2.5	      gcc-2.7.2.1    +X11 -GUI Athena	5.0v	 Bram Moolenaar
#Linux 2.0	      gcc-2.7.2      Infomagic Motif	4.3  (3) Ronald Rietman
#Linux 2.0.29	      gcc-2.7.2.1    +X11 +GUI Athena	5.0x     Ant. Colombo
#Linux 2.0.31         gcc            +X11 +GUI Athena   5.0w (U) Darren Hiebert
#Linux 2.0.33	      gcc-2.7.2.1    X11R6, Lesstif	5.0s	 Tony Nugent
#NEC UP4800 UNIX_SV 4.2MP  cc	     +X11R6 Motif,Athena4.6b (Q) Lennart Schultz
#NetBSD 1.0A	      gcc-2.4.5      -X11 -GUI		3.21 (X) Juergen Weigert
#NetBSD 1.2.1	      gcc-2.7.2.2    +X11 +GUI Athena	5.0n     Olaf Seibert
#NetBSD 1.2.1	      gcc-2.7.2.2    +X11 +GUI Lesstiff	5.0n     Olaf Seibert
#QNX 4.2	      wcc386-10.6    -X11		4.2  (D) G.F. Desrochers
#QNX 4.23	      Watcom	     -X11		4.2  (F) John Oleynick
#SCO Unix v3.2.5      cc	     +X11 Motif		3.27 (C) M. Kuperblum
#SCO Open Server 5		     +X11 +GUI Motif	3.28 (A) J.C. van Winkel
#SINIX-N 5.43 RM400 R4000   cc       +X11 +GUI          5.0l (I) Martin Furter
#SNI Targon31 TOS 4.1.11 gcc-2.4.5   +X11 -GUI		4.6c (B) Paul Slootman
#Solaris 2.3 (sun4m)  cc             +X11 +GUI Athena   5.0w     Darren Hiebert
#Solaris 2.4 (Sparc)  cc	     +X11 +GUI		3.29 (9) Glauber
#Solaris 2.4/2.5      clcc	     +X11 -GUI openwin	3.20 (7) Robert Colon
#Solaris 2.5 (sun4m)  cc (SC4.0)     +X11R6 +GUI (CDE)	4.6b (E) Andrew Large
#Solaris 2.5	      cc	     +X11 +GUI Athena	4.2  (9) Sonia Heimann
#Solaris 2.5          gcc 2.5.6      +X11 Motif         5.0m (R) Ant. Colombo
#Solaris 2.5 (sparc)  gcc 2.7.2.1 +X11 +GUI_Motif (CDE) 5.0s     D. Rothkamp
#Solaris 2.6 (sun4u)  gcc 2.7.2.3    +X11 +GUI (CDE)    5.0w     D. Rothkamp
#Solaris 2.6 (sun4u)  cc (SC4.2)     +X11R6 +GUI (CDE)	5.0v     Andrew Large
#SunOS_5.5.1          gcc 2.7.2      +X11 +Motif        5.0s     Karsten Sievert
#SunOS 4.1.x			     +X11 -GUI		5.0v (J) Bram Moolenaar
#SunOS 4.1.3_U1 (sun4c) gcc          +X11 +GUI Athena   5.0w (J) Darren Hiebert
#SUPER-UX 6.2 (NEC SX-4) cc	     +X11R6 Motif,Athena4.6b (P) Lennart Schultz
#Unisys 6035	      cc	     +X11 Motif		3.29 (8) Glauber

# (*)  Remarks:
#
# (1)  Uncomment line below for shlicc2
# (2)  HPUX with compile problems or wrong digraphs, uncomment line below
# (3)  Infomagic Motif needs GUI_LIB_LOC and GUI_INC_LOC set, see below.
#      And add "-lXpm" to MOTIF_LIBS2.
# (4)  For cc the optimizer must be disabled (use CFLAGS= after running
#      configure) (symptom: ":set termcap" output looks weird).
# (5)  Compiler may need -qmaxmem argument, see below.
# (6)  See below for a few lines to uncomment
# (7)  See below for lines which enable the use of clcc
# (8)  Needs some EXTRA_LIBS, search for Unisys below
# (9)  Needs an extra compiler flag to compile gui_at_sb.c, see below.
# (A)  needs EXTRA_LIBS, see below
# (B)  Can't compile GUI because there is no waitpid()...  Disable GUI below.
# (C)  Force the use of curses instead of termcap, see below.
# (D)  Uncomment lines below for QNX
# (E)  You might want to use termlib instead of termcap, see below.
# (F)  See below for instructions.
# (H)  See line for EXTRA_LIBS below.
# (I)  SINIX-N 5.42 and 5.43 need some EXTRA_LIBS
# (J)  If you get undefined symbols, see below for a solution.
# (K)  See lines to uncomment below for machines with 64 bit pointers.
# (L)  For Silicon Graphics O2 workstations remove "-lnsl" from config.mk
# (M)  gcc version cygnus-2.0.1 does NOT work (symptom: "dl" deletes two
#      characters instead of one).
# (N)  SCO with decmouse.
# (P)  For SuperUX 6.2 on NEC SX-4 see a few lines below to uncomment.
# (Q)  For UNIXSVR 4.2MP on NEC UP4800 see below for lines to uncomment.
# (R)  For Solaris 2.5 (or 2.5.1) with gcc > 2.5.6, uncomment line below.
# (S)  For Irix 6.4, use -OPT:Olimit.  See line below.
# (T)  See ../doc/os_beos.txt.
# (U)  Must uncomment CONF_OPT_PYTHON option below to disable Python
#      detection, since the configure script runs into an error when it
#      detects Python (probably because of the bash shell).
# (X)  Need to use the .include "config.mk" line below
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

######################## config.mk ########################
# At this position config.mk is included. When starting from the
# distribution it is almost empty. After running ./configure it contains
# settings that have been discovered for your system. Settings below this
# include override settings in config.mk!

# (X) How to include config.mk depends on the version of "make" you have, if
#     the current choice doesn't work, try the other one.

include config.mk
#.include "config.mk"

# Include the configuration choices first, so we can override everything
# below. As shipped, this file contains a target that causes to run
# configure. Once configure was run, this file contains a list of
# make variables with predefined values instead. Thus any second invocation
# of make, will buid Vim.

# You can give a lot of options to configure.
# Change this to your desire and do 'make config' afterwards

### For creating Vim with GUI (gvim)
# Uncomment the first line when you don't want to get the GUI version,
# although you have Motif and/or Athena. (B)
# When using "yes" configure will use Motif if it can be found, otherwise use
# Athena when it can be found (this is the default).
# If the Motif and Athena files are not found, GUI is disabled automatically
#CONF_OPT_GUI = --enable-gui=no
#CONF_OPT_GUI = --enable-gui=motif
#CONF_OPT_GUI = --enable-gui=athena

### For creating Vim with Perl interface
# Uncomment this when you don't want to include the Perl interface, although
# you have Perl.
# The Perl option sometimes causes problems, because it adds extra flags
# to the command line.  If you see strange flags during compilation, check in
# config.mk where they come from.  If it's PERL_CFLAGS, try uncommenting the
# next line.
#CONF_OPT_PERL = --disable-perlinterp

### For creating Vim with Python interface
# Uncomment this when you don't want to include the Python interface,
# although you have Python.
#CONF_OPT_PYTHON = --disable-pythoninterp

### Extra configure arguments - just put anything here, just an example
#CONF_ARGS = --exec-prefix=/usr

### For creating a plain Vim without any X11 related fancies use this one:
### (otherwise Vim configure will try to include xterm titlebar access)
### Also disable the GUI above, otherwise it will be included anyway.
### When both GUI and X11 have been disabled this may save about 15% of the
### code and make Vim startup quicker.
#CONF_OPT_X = --without-x

### If you have multiple X11 derivates installed, you may wish to specify
### which one to use. This here selects openwin (as found on sun).
### Select nothing to let configure choose.
#XROOT = /usr/openwin
#CONF_OPT_X = --x-include=$(XROOT)/include --x-libraries=$(XROOT)/lib

### COMPILER -- the default from configure will mostly be fine, no need to
### change this, just an example. If a compiler is defined here, configure
### will use it rather than probing for one. It is dangerous to change this
### after configure was run. Make will use your choice then -- but beware:
### Many things may change with another compiler. It is wise to run 'make
### distclean' and start all over again.
#CC = cc
#CC = gcc

### COMPILER FLAGS - change as you please. Either before running configure
### or afterwards. For examples see below.
### When using -g with some older versions of Linux you might get a
### statically linked executable.
### When not defined, configure will try to use -O2 -g for gcc and -O for cc.
#CFLAGS = -g
#CFLAGS = -O

### Often used for GCC: mixed optimizing, lot of optimizing, debugging
#CFLAGS = -g -O2 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#CFLAGS = -O6 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#CFLAGS = -g -DDEBUG -Wall -Wshadow -Wmissing-prototypes


### Electric-Fence malloc debugging - catches memory accesses beyond allocated
### memory.  Electric Fence is free (search ftp sites).
#EXTRA_LIBS = /usr/local/lib/libefence.a

### PURIFY - remove the # to use the "purify" program (hoi Nia++!)
#PURIFY = purify

### for running lint
LINT_OPTIONS = -bepruxzF

### PROFILING - Uncomment the next three lines to do profiling with gcc and
### gprof.  Only works when using termcap library.  Doesn't work with GUI or
### Perl.
### Need to recompile everything after changing this: "make clean" "make".
#CFLAGS = -g -pg -O2 -fno-strength-reduce -Wall -Wshadow -Wmissing-prototypes
#LDFLAGS = -pg
#LIBS = -ltermcap_p

### Include support for SNiFF+.
#SNIFF_INCL = if_sniff.h
#SNIFF_SRC  = if_sniff.c
#SNIFF_OBJ  = if_sniff.o
#SNIFF_DEFS = -DUSE_SNIFF

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

### (5) AIX 4.1.4 with cc
#CFLAGS = -O -qmaxmem=8092

### (7) Solaris 2.4/2.5 with Centerline compiler
#CC = clcc
#X_LIBS_DIR = -L/usr/openwin/lib -R/usr/openwin/lib
#CFLAGS = -O

### (8) Unisys 6035 (use the first with GUI, the second without)
#EXTRA_LIBS = -lsocket -lgen
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

### Irix 4.0 & 5.2 (Silicon Graphics Machines, __sgi will be defined)
# Not needed for Irix 5.3, Ives Aerts reported
#EXTRA_LIBS = -lmalloc -lc_s

### Coherent 4.2.10 on Intel 386 platform
#EXTRA_DEFS = -Dvoid=int
#EXTRA_LIBS = -lterm -lsocket

### SCO 3.2, with different library name for terminfo
#EXTRA_LIBS = -ltinfo

### Solaris 2.3 with X11 and specific cc
#CC=/opt/SUNWspro/bin/cc -O -Xa -v -R/usr/openwin/lib

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

### (A)  SCO Open Server 5 (Jan Christiaan van Winkel)
###	 Also use the TERM_LIB below!
#EXTRA_LIBS = -lgen

### (D)  QNX (by G.F. Desrochers)
###	 Also change "STRIP = strip" into "STRIP = echo" below
#CFLAGS = -g -O -mf -4

### (F)  QNX (by John Oleynick)
# 1. If you don't have an X server: Comment out CONF_OPT_GUI and uncomment
#    CONF_OPT_X = --without-x.
# 2. make config
# 3. edit config.mk and remove -ldir and -ltermcap from LIBS.  It doesn't
#	have -ldir (does config find it somewhere?) and -ltermcap has at
#	least one problem so I use termlib.o instead.  The problem with
#	termcap is that it segfaults if you call it with the name of
#	a non-existent terminal type.
# 4. edit config.h and add #define USE_TMPNAM
# 5. add termlib.o to OBJ
# 6. make

### (H)  for Data general DG/UX 5.4.2 and 5.4R3.10 (Jonas J. Schlein)
#EXTRA_LIBS = -lgen

### (I) SINIX-N 5.42 or 5.43 RM400 R4000
#EXTRA_LIBS = -lgen -lnsl

### (K)  for SGI Irix machines with 64 bit pointers ("uname -s" says IRIX64)
###	 suggested by Jon Wright <jon@gate.sinica.edu.tw>
###	 tested on R8000 IRIX6.1 Power Indigo2
# either (for 64 bit pointers) uncomment the following line
#GUI_LIB_LOC = /usr/lib64
# then
# 1) make config
# 2) edit config.mk and delete the -lelf entry in the LIBS line
# 3) make
#
# or (for 32bit pointers) uncomment the following line
#EXTRA_DEFS = -n32
#GUI_LIB_LOC = /usr/lib32
# then
# 1) make config
# 2) edit config.mk, add -n32 to LDFLAGS
# 3) make
###

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

### (R) Solaris 2.5 (or 2.5.1) with gcc > 2.5.6
#LDFLAGS = -lw -ldl

### (S) Irix 6.4 (and later?): Uses different Olimit flag:
#CFLAGS = -OPT:Olimit=2000 -O

### (C)  On SCO Unix v3.2.5 (and probably other versions) the termcap library,
###	 which is found by configure, doesn't work correctly.  Symptom is the
###	 error message "Termcap entry too long".  Uncomment the next line.
### after changing this, you need to do "make distclean; make".
#TERM_LIB = -lcurses

### (E)  If you want to use termlib library instead of the automatically found
###	 one.  After changing this, you need to do "make distclean; make".
#TERM_LIB = -ltermlib

### If you want to use ncurses library instead of the automatically found one
### after changing this, you need to do "make distclean; make".
#TERM_LIB = -lncurses

### For GCC on MSDOS, the ".exe" suffix will be added.
#SUFFIX = .exe

##################### end of system specific lines ###################

### Name of the targets
VIMTARGET	= vim
EXTARGET	= ex
VIEWTARGET	= view
GVIMTARGET	= g$(VIMTARGET)
GVIEWTARGET	= g$(VIEWTARGET)
RVIMTARGET	= r$(VIMTARGET)
RVIEWTARGET	= r$(VIEWTARGET)
RGVIMTARGET	= r$(GVIMTARGET)
RGVIEWTARGET	= r$(GVIEWTARGET)

### Names of the tools that are also made
TOOLS = ctags/ctags xxd/xxd

### Installation directories.  The defaults come from configure.
#
### prefix	the top directory for the data (default "/usr/local")
#
# Uncomment the next line to install Vim in your home directory.
#prefix = $(HOME)

### exec_prefix	is the top directory for the executable (default ${prefix})
#
# Uncomment the next line to install the Vim executable in "/usr/machine/bin"
#exec_prefix = /usr/machine

### BINDIR	dir for the executable	 (default "${exec_prefix}/bin")
### MANDIR	dir for the manual pages (default "${prefix}/man")
### DATADIR	dir for the other files  (default "${prefix}/lib" or
#						  "${prefix}/share")
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

### Location of Vim files (should not need to be changed)
VIMDIR = /vim
HELPSUBDIR = /doc
SYNSUBDIR = /syntax

### VIMLOC	common root of the Vim files
### HELPSUBLOC	location for help files
### SYNSUBLOC	location for syntax files
### SCRIPTLOC	location for script files (menu.vim, bugreport.vim)
### You can override these if you want to install them somewhere else.
### Edit feature.h for compile-time settings.
VIMLOC		= $(DATADIR)$(VIMDIR)
HELPSUBLOC	= $(VIMLOC)$(HELPSUBDIR)
SCRIPTLOC	= $(VIMLOC)
SYNSUBLOC	= $(VIMLOC)$(SYNSUBDIR)

### Added for the Debian GNU/Linux packaged version of Vim, to get around
### some sed trickery. (the files are installed into a temporary directory
### for building the package, but they eventually end up in /usr/... . This
### ensures that the correct path is put into the man page.)
HELPENDLOC = $(HELPSUBLOC)

### Name of the menu file target.
SYS_MENU_FILE	= $(SCRIPTLOC)/menu.vim

### Name of the bugreport file target.
SYS_BUGR_FILE	= $(SCRIPTLOC)/bugreport.vim

### Program to run on installed binary
STRIP = strip

### Permissions for vim binary
BINMOD = 755

### Permissions for man page
MANMOD = 644

### Permissions for help files
HELPMOD = 644

### Permission for script files (menu.vim, bugreport.vim)
SCRIPTMOD = 644

### Permissions for bin/man/help directories that are created
DIRMOD = 755

# Where to copy the man and help files from
HELPSOURCE = ../doc

# Where to copy the script files from (menu, bugreport)
SCRIPTSOURCE = ..

# Where to copy the syntax files from
SYNSOURCE = ../syntax

# If you are using Linux, you might want to use this to make vim the
# default vi editor, it will create a link from vi to Vim when doing
# "make install".  An existing file will be overwritten!
# When not using it, some make programs can't handle an undefined $(LINKIT).
#LINKIT = -ln -f -s $(BINDIR)/$(VIMTARGET) /usr/bin/vi
LINKIT = echo >/dev/null

###
### GRAPHICAL USER INTERFACE (GUI).
### 'configure --enable-gui' can enable one of these for you if you did set
### a corresponding CONF_OPT_GUI above and have X11.
### Override configures choice by uncommenting all the following lines.
### As they are, the GUI is disabled.  Replace "NONE" with "ATHENA" or "MOTIF"
### for enabling the Athena or Motif GUI.
#GUI_INCL	= $(NONE_INCL)
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

### Motif GUI interface.
MOTIF_INCL	= gui.h
MOTIF_SRC	= gui.c gui_motif.c gui_x11.c pty.c
MOTIF_OBJ	= gui.o gui_motif.o gui_x11.o pty.o
MOTIF_DEFS	= -DUSE_GUI_MOTIF $(NARROW_PROTO)
MOTIF_IPATH	= -I$(GUI_INC_LOC)
MOTIF_LIBS_DIR	= -L$(GUI_LIB_LOC)
MOTIF_LIBS1	=
MOTIF_LIBS2	= -lXm -lXt
MOTIF_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
MOTIF_MAN_TARGETS = $(MANSUBDIR)/$(GVIMTARGET).1 \
			$(MANSUBDIR)/$(GVIEWTARGET).1 \
			$(MANSUBDIR)/$(RGVIMTARGET).1 \
			$(MANSUBDIR)/$(RGVIEWTARGET).1
MOTIF_TESTTARGET = gui

#For SunOS (Solaris) you might want to use this line:
#MOTIF_LIBS_DIR = -L$(GUI_LIB_LOC) -R$(GUI_LIB_LOC)

### Athena Widget GUI interface.
### Use Xaw3d to make the menus look a little bit nicer
#XAW_LIB = -lXaw3d
XAW_LIB = -lXaw

### When using Xaw3d, uncomment/comment the following lines to also get the
### scrollbars from Xaw3d.
#ATHENA_SRC	= gui.c gui_athena.c gui_x11.c pty.c
#ATHENA_OBJ	= gui.o gui_athena.o gui_x11.o pty.o
#ATHENA_DEFS	= -DUSE_GUI_ATHENA $(NARROW_PROTO) \
#		    -Dvim_scrollbarWidgetClass=scrollbarWidgetClass \
#		    -Dvim_XawScrollbarSetThumb=XawScrollbarSetThumb
ATHENA_SRC	= gui.c gui_athena.c gui_x11.c pty.c gui_at_sb.c
ATHENA_OBJ	= gui.o gui_athena.o gui_x11.o pty.o gui_at_sb.o
ATHENA_DEFS	= -DUSE_GUI_ATHENA $(NARROW_PROTO)

ATHENA_INCL	= gui.h
ATHENA_IPATH	= -I$(GUI_INC_LOC)
ATHENA_LIBS_DIR = -L$(GUI_LIB_LOC)
ATHENA_LIBS1	= $(XAW_LIB)
ATHENA_LIBS2	= -lXt
ATHENA_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
ATHENA_MAN_TARGETS = $(MANSUBDIR)/$(GVIMTARGET).1 \
			$(MANSUBDIR)/$(GVIEWTARGET).1 \
			$(MANSUBDIR)/$(RGVIMTARGET).1 \
			$(MANSUBDIR)/$(RGVIEWTARGET).1
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
BEOSGUI_INCL	= gui.h
BEOSGUI_SRC	= gui.c gui_beos.cc pty.c
BEOSGUI_OBJ	= gui.o gui_beos.o  pty.o
BEOSGUI_DEFS	= -DUSE_GUI_BEOS
BEOSGUI_IPATH	=
BEOSGUI_LIBS_DIR =
BEOSGUI_LIBS1	=
BEOSGUI_LIBS2	=
BEOSGUI_TARGETS	= $(BINDIR)/$(GVIMTARGET) \
			$(BINDIR)/$(GVIEWTARGET) \
			$(BINDIR)/$(RGVIMTARGET) \
			$(BINDIR)/$(RGVIEWTARGET)
BEOSGUI_MAN_TARGETS = $(MANSUBDIR)/$(GVIMTARGET).1 \
			$(MANSUBDIR)/$(GVIEWTARGET).1 \
			$(MANSUBDIR)/$(RGVIMTARGET).1 \
			$(MANSUBDIR)/$(RGVIEWTARGET).1
BEOSGUI_TESTTARGET = gui

# GUI files used for making ctags
ALL_GUI_INCL = gui.h gui_at_sb.h
ALL_GUI_SRC  = gui.c gui_motif.c gui_athena.c gui_x11.c gui_at_sb.c
ALL_GUI_PRO  = gui.pro gui_motif.pro gui_athena.pro gui_x11.pro gui_w32.pro

### our grand parent directory should know who we are...
### only used for "make tar"
VIMVERSION = `eval "basename \`cd ../; pwd\`"`

### Command to create dependencies based on #include "..."
### prototype headers are ignored due to -DPROTO, system
### headers #include <...> are ignored if we use the -MM option, as
### e.g. provided by gcc-cpp.
### Include USE_GUI to get gependency on gui.h
CPP_DEPEND = $(CC) -M$(CPP_MM) $(DEPEND_CFLAGS)

# flags for cproto
#     __inline and __attribute__ are not recognized by cproto
#     maybe the "/usr/bin/cc -E" has to be adjusted for some systems

NO_ATTR = -D__inline= -D"__attribute__\\(x\\)="

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

PRE_DEFS = -Iproto $(DEFS) $(SNIFF_DEFS) $(GUI_DEFS) $(GUI_IPATH)
POST_DEFS = $(X_CFLAGS) $(PERL_CFLAGS) $(PYTHON_CFLAGS) $(EXTRA_DEFS)

ALL_CFLAGS = $(PRE_DEFS) $(CFLAGS) $(POST_DEFS)

LINT_CFLAGS = $(PRE_DEFS) $(POST_DEFS)

DEPEND_CFLAGS = -DPROTO -DUSE_GUI $(LINT_CFLAGS)

PFLAGS = $(PROTO_FLAGS) -DPROTO $(LINT_CFLAGS)

ALL_LIBS = $(GUI_LIBS_DIR) $(X_LIBS_DIR) $(GUI_LIBS1) $(GUI_X_LIBS) $(GUI_LIBS2) $(X_PRE_LIBS) $(X_LIBS) $(X_EXTRA_LIBS) $(LIBS) $(PYTHON_LIBS) $(EXTRA_LIBS)

#     BASIC_INCL and BASIC_SRC: files that are always used
#	  GUI_INCL and GUI_SRC: extra GUI files for current configuration
# ALL_GUI_INCL and ALL_GUI_SRC: all GUI files for Unix
#
#		  INCL and SRC: files used for current configuration
#		      TAGS_SRC: source files used for make tags
#		     TAGS_INCL: include files used for make tags
#		       ALL_SRC: source files used for make depend and make lint

BASIC_INCL = ascii.h ex_cmds.h config.h feature.h globals.h \
		keymap.h macros.h option.h osdef.h proto.h regexp.h \
		structs.h term.h os_unix.h os_unixx.h version.h vim.h \
		$(SNIFF_INCL)

INCL = $(BASIC_INCL) $(GUI_INCL)

TAGS_INCL = $(BASIC_INCL) $(ALL_GUI_INCL) if_perl.xs farsi.h

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
	getchar.c \
	main.c \
	mark.c \
	memfile.c \
	memline.c \
	message.c \
	misc1.c \
	misc2.c \
	normal.c \
	ops.c \
	option.c \
	os_unix.c \
	pathdef.c \
	quickfix.c \
	regexp.c \
	screen.c \
	search.c \
	syntax.c \
	$(SNIFF_SRC) \
	tag.c \
	term.c \
	ui.c \
	undo.c \
	version.c \
	window.c \
	$(OS_EXTRA_SRC)

SRC =	$(BASIC_SRC) $(GUI_SRC) $(PERL_SRC) $(PYTHON_SRC)

TAGS_SRC = $(SRC) farsi.c

ALL_SRC = $(BASIC_SRC) $(ALL_GUI_SRC) $(PERL_SRC) $(PYTHON_SRC) if_sniff.c

OBJ = \
	buffer.o \
	charset.o \
	digraph.o \
	edit.o \
	eval.o \
	ex_cmds.o \
	ex_docmd.o \
	ex_getln.o \
	fileio.o \
	getchar.o \
	main.o \
	mark.o \
	memfile.o \
	memline.o \
	message.o \
	misc1.o \
	misc2.o \
	normal.o \
	ops.o \
	option.o \
	os_unix.o \
	pathdef.o \
	quickfix.o \
	regexp.o \
	screen.o \
	search.o \
	syntax.o \
	$(SNIFF_OBJ) \
	tag.o \
	term.o \
	ui.o \
	undo.o \
	window.o \
	$(GUI_OBJ) \
	$(PERL_OBJ) \
	$(PYTHON_OBJ) \
	$(OS_EXTRA_OBJ)

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
	getchar.pro \
	main.pro \
	mark.pro \
	memfile.pro \
	memline.pro \
	message.pro \
	misc1.pro \
	misc2.pro \
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
	$(ALL_GUI_PRO) \
	$(PERL_PRO)

PRO_MANUAL = os_amiga.pro os_msdos.pro os_win32.pro os_beos.pro

# Default target is making the executable and tools
all: $(VIMTARGET) $(TOOLS)

tools: $(TOOLS)

# Run ./configure with all the setting above.
config: configure
	GUI_INC_LOC="$(GUI_INC_LOC)" GUI_LIB_LOC="$(GUI_LIB_LOC)" \
		CC="$(CC)" CFLAGS="$(CFLAGS)" TERM_LIB="$(TERM_LIB)" \
	$(CONF_SHELL) ./configure $(CONF_OPT_GUI) $(CONF_OPT_X) \
	$(CONF_OPT_PERL) $(CONF_OPT_PYTHON) $(CONF_ARGS)

# When configure.in has changed, run autoconf to produce configure
# If you don't have autoconf, use the configure that's there
configure: configure.in
	autoconf
	chmod 755 configure

# Re-execute this Makefile to include the new config.mk produced by configure
# Only used when typing "make" with a fresh config.mk.
myself:
	$(MAKE) -f Makefile all


# Link the target for normal use or debugging.
# A shell script is used to try linking without unneccesary libraries.
$(VIMTARGET): $(OBJ) version.c version.h
	$(CC) -c $(ALL_CFLAGS) version.c
	@LINK="$(PURIFY) $(SHRPENV) $(CC) $(LDFLAGS) -o $(VIMTARGET) $(OBJ) version.o $(ALL_LIBS)" \
		sh $(srcdir)/link.sh

CTAGSFILES =	ctags/entry.c ctags/main.c ctags/parse.c ctags/sort.c \
		ctags/get.c ctags/options.c ctags/read.c ctags/strstr.c \
		ctags/config.h ctags/ctags.h

ctags/ctags: ctags/Makefile $(CTAGSFILES)
	cd ctags; CC="$(CC)" CFLAGS="$(CFLAGS)" $(MAKE) -f Makefile

ctags/Makefile: ctags/Makefile.in
	cd ctags; CC="$(CC)" CFLAGS="$(CFLAGS)" ./configure --mandir="$(MANDIR)" --exec_prefix="$(exec_prefix)"

xxd/xxd: xxd/xxd.c
	cd xxd; CC="$(CC)" CFLAGS="$(CFLAGS)" $(MAKE) -f Makefile.unix

# Generate function prototypes.  This is not needed to compile vim, but if
# you want to use it, cproto is out there on the net somewhere -- Webb
#
# When generating os_amiga.pro, os_msdos.pro and os_win32.pro there will be a
# few include files that can not be found, that's OK.

proto: $(PRO_AUTO) $(PRO_MANUAL)

### Would be nice if this would work for "normal" make.
### Currently it only works for (Free)BSD make.
#$(PRO_AUTO): $$(*F).c
#	cproto $(PFLAGS) -DUSE_GUI $(*F).c > $@

# Always define USE_GUI.  This will generate a few warnings if it's also
# defined in config.h, you can ignore that.
.c.pro:
	cproto $(PFLAGS) -DUSE_GUI $< > proto/$@

os_amiga.pro: os_amiga.c
	cproto $(PFLAGS) -DAMIGA -UHAVE_CONFIG_H -DBPTR=char* os_amiga.c > proto/os_amiga.pro

os_msdos.pro: os_msdos.c
	cproto $(PFLAGS) -DMSDOS -UHAVE_CONFIG_H os_msdos.c > proto/os_msdos.pro

os_win32.pro: os_win32.c
	cproto $(PFLAGS) -DWIN32 -UHAVE_CONFIG_H os_win32.c > proto/os_win32.pro

os_beos.pro: os_beos.c
	cproto $(PFLAGS) -D__BEOS__ -UHAVE_CONFIG_H os_beos.c > proto/os_beos.pro

notags:
	rm -f tags

# Note: tags is made for the currently configured version, can't include both
#	Motif and Athena GUI
# You can ignore error messages for missing files.
ctags tags TAGS: notags
	$(TAGPRG) $(TAGS_SRC) $(TAGS_INCL)

# Make a highlight file for types.  Requires Exhuberant ctags and awk
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
test:
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
install: installvim installtools

installvim: installvimbin installvimhelp installlinks installhelplinks

installvimbin: $(VIMTARGET) $(exec_prefix) $(BINDIR)
	-mv -f $(BINDIR)/$(VIMTARGET)$(SUFFIX) $(BINDIR)/$(VIMTARGET).old
	-rm -f $(BINDIR)/$(VIMTARGET).old
	cp $(VIMTARGET)$(SUFFIX) $(BINDIR)
	$(STRIP) $(BINDIR)/$(VIMTARGET)$(SUFFIX)
	chmod $(BINMOD) $(BINDIR)/$(VIMTARGET)$(SUFFIX)
# may create a link to the new executable from /usr/bin/vi
	-$(LINKIT)

# install the help files; first adjust the contents for the location
installvimhelp: $(MANSUBDIR) $(VIMLOC) $(HELPSUBLOC) $(SYNSUBLOC)
	sed -e s+/usr/local/lib/vim+$(HELPENDLOC)+ $(HELPSOURCE)/vim.1 \
			> $(MANSUBDIR)/$(VIMTARGET).1
	chmod $(MANMOD) $(MANSUBDIR)/$(VIMTARGET).1
	cp $(HELPSOURCE)/*.txt $(HELPSOURCE)/tags $(HELPSUBLOC)
	chmod $(HELPMOD) $(HELPSUBLOC)/*.txt $(HELPSUBLOC)/tags
# install the menu file
	cp $(SCRIPTSOURCE)/menu.vim $(SYS_MENU_FILE)
	chmod $(SCRIPTMOD) $(SYS_MENU_FILE)
# install the bugreport file
	cp $(SCRIPTSOURCE)/bugreport.vim $(SYS_BUGR_FILE)
	chmod $(SCRIPTMOD) $(SYS_BUGR_FILE)
# install the syntax files
	cp $(SYNSOURCE)/*.vim $(SYNSUBLOC)
	chmod $(HELPMOD) $(SYNSUBLOC)/*.vim

# install helper programs ctags and xxd
installtools: $(TOOLS) $(exec_prefix) $(BINDIR) $(MANSUBDIR)
	cd ctags; mandir="$(MANDIR)" exec_prefix="$(exec_prefix)" \
		bindir="$(BINDIR)" $(MAKE) -e -f Makefile install
	-mv -f $(BINDIR)/xxd$(SUFFIX) $(BINDIR)/xxd.old
	-rm -f $(BINDIR)/xxd.old
	cp xxd/xxd$(SUFFIX) $(BINDIR)
	$(STRIP) $(BINDIR)/xxd$(SUFFIX)
	chmod $(BINMOD) $(BINDIR)/xxd$(SUFFIX)
	cp $(HELPSOURCE)/xxd.1 $(MANSUBDIR)
	chmod $(MANMOD) $(MANSUBDIR)/xxd.1

$(exec_prefix) $(BINDIR) $(MANSUBDIR) $(VIMLOC) $(HELPSUBLOC) $(SYNSUBLOC):
	-ctags/mkinstalldirs $@
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

# create links for the manual pages with various names to vim.  This is only
# done when the links (or manpages with the same name) don't exist yet.
installhelplinks: $(GUI_MAN_TARGETS) $(MANSUBDIR)/$(EXTARGET).1 $(MANSUBDIR)/$(VIEWTARGET).1 $(MANSUBDIR)/$(RVIMTARGET).1 $(MANSUBDIR)/$(RVIEWTARGET).1

$(MANSUBDIR)/$(EXTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(EXTARGET).1

$(MANSUBDIR)/$(VIEWTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(VIEWTARGET).1

$(MANSUBDIR)/$(GVIMTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(GVIMTARGET).1

$(MANSUBDIR)/$(GVIEWTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(GVIEWTARGET).1

$(MANSUBDIR)/$(RVIMTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(RVIMTARGET).1

$(MANSUBDIR)/$(RVIEWTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(RVIEWTARGET).1

$(MANSUBDIR)/$(RGVIMTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(RGVIMTARGET).1

$(MANSUBDIR)/$(RGVIEWTARGET).1:
	cd $(MANSUBDIR); ln -s $(VIMTARGET).1 $(RGVIEWTARGET).1

uninstall:
	-rm -f $(BINDIR)/$(VIMTARGET)$(SUFFIX)
	-rm -f $(BINDIR)/$(EXTARGET) $(BINDIR)/$(VIEWTARGET)
	-rm -f $(BINDIR)/$(GVIMTARGET) $(BINDIR)/$(GVIEWTARGET)
	-rm -f $(BINDIR)/$(RVIMTARGET) $(BINDIR)/$(RVIEWTARGET)
	-rm -f $(BINDIR)/$(RGVIMTARGET) $(BINDIR)/$(RGVIEWTARGET)
	-rm -f $(BINDIR)/xxd $(MANSUBDIR)/xxd.1
	-rm -f $(BINDIR)/ctags $(MANSUBDIR)/ctags.1
	-rm -f $(MANSUBDIR)/$(VIMTARGET).1
	-rm -f $(MANSUBDIR)/$(EXTARGET).1 $(MANSUBDIR)/$(VIEWTARGET).1
	-rm -f $(MANSUBDIR)/$(GVIMTARGET).1 $(MANSUBDIR)/$(GVIEWTARGET).1
	-rm -f $(MANSUBDIR)/$(RVIMTARGET).1 $(MANSUBDIR)/$(RVIEWTARGET).1
	-rm -f $(MANSUBDIR)/$(RGVIMTARGET).1 $(MANSUBDIR)/$(RGVIEWTARGET).1
	-rm -f $(HELPSUBLOC)/*.txt $(HELPSUBLOC)/tags
	-rm -f $(SYNSUBLOC)/*.vim
	-rm -f $(SYS_MENU_FILE) $(SYS_BUGR_FILE)
	-rmdir $(HELPSUBLOC) $(SYNSUBLOC) $(VIMLOC)

# Clean up all the files that have been produced, except configure's.
# We support common typing mistakes for Juergen! :-)
clean celan: testclean
	-rm -f *.o core $(VIMTARGET).core $(VIMTARGET)$(SUFFIX) xxd/*.o
	-rm -f $(TOOLS) osdef.h pathdef.c if_perl.c
	-rm -f conftest* *~ link.sed
	if test -f ctags/Makefile; then \
		cd ctags; $(MAKE) -f Makefile clean; \
	fi

shadow:	doc menu.vim bugreport.vim syntax
	mkdir shadow
	cd shadow; ln -s ../*.[ch] ../*.in ../*.sh ../*.xs ../toolcheck ../proto ../configure .
	cd shadow; rm -f osdef.h config.h pathdef.c link.sed
	cp Makefile shadow
	echo "the first targets to make vim are: scratch config myself" > shadow/config.mk
# TODO: how do we make a nice shadow for ctags?
	cd shadow; ln -s ../ctags .
	mkdir shadow/xxd
	cd shadow/xxd; ln -s ../../xxd/*.[ch] ../../xxd/Make* .
	mkdir shadow/testdir
	cd shadow/testdir; ln -s ../../testdir/Makefile \
				 ../../testdir/*.in  ../../testdir/*.ok .

# Links needed for doing "make install" in a shadow directory.
doc:
	-ln -s ../doc .

menu.vim:
	-ln -s ../menu.vim .

bugreport.vim:
	-ln -s ../bugreport.vim .

syntax:
	-ln -s ../syntax .

# Start configure from scratch
scrub scratch:
	rm -f config.status config.cache config.h config.log
	if test -f ctags/Makefile; then \
		cd ctags; $(MAKE) -f Makefile distclean; \
	fi

distclean: clean scratch
	echo "the first targets to make vim are: scratch config myself" > config.mk

tar: clean
	echo packing $(VIMVERSION) ...
	VIMVERSION=$(VIMVERSION); cd ../..; set -x; \
	  tar cvf $$VIMVERSION.tar $$VIMVERSION; \
	  gzip -nf $$VIMVERSION.tar || gzip -f $$VIMVERSION.tar

dist: distclean tar

check:

mdepend:
	-@rm -f Makefile~
	cp Makefile Makefile~
	sed -e '/\#\#\# Dependencies/q' < Makefile > tmp_make
	@for i in $(ALL_SRC) ; do \
	  echo "$$i" ; \
	  echo `echo "$$i" | sed -e 's/.c$$/.o/'`": $$i" `\
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
			$(CPP_DEPEND) $$i >> tmp_make; done
	mv tmp_make Makefile

lint:
	lint $(LINT_OPTIONS) $(LINT_CFLAGS) -DUSE_SNIFF $(ALL_SRC)

###########################################################################

# Used when .o files are in src directory
.c.o:
	$(CC) -c -I$(srcdir) $(ALL_CFLAGS) $<

.cc.o:
	$(CC) -c -I$(srcdir) $(ALL_CFLAGS) $<

# Used when .o files are in src/objects directory
#$(OBJ): $$(*F).c
#	$(CC) -c -I$(srcdir) $(ALL_CFLAGS) $(*F).c -o $@

if_perl.c: if_perl.xs
	$(PERL) $(PERLLIB)/ExtUtils/xsubpp -prototypes -typemap \
	    $(PERLLIB)/ExtUtils/typemap if_perl.xs > $@

# used when python is being built
py_getpath.o: $(PYTHON_CONFDIR)/getpath.c
	$(CC) -c -o $@ $(PYTHON_CONFDIR)/getpath.c \
		-I$(PYTHON_CONFDIR) -DHAVE_CONFIG_H -DNO_MAIN \
		$(ALL_CFLAGS) \
		$(PYTHON_GETPATH_CFLAGS)

py_config.o: $(PYTHON_CONFDIR)/config.c
	$(CC) -c -o $@ $(PYTHON_CONFDIR)/config.c \
		-I$(PYTHON_CONFDIR) -DHAVE_CONFIG_H -DNO_MAIN \
		$(ALL_CFLAGS)

osdef.h: osdef.sh config.h osdef1.h.in osdef2.h.in
	CC="$(CC) $(ALL_CFLAGS)" srcdir=${srcdir} sh $(srcdir)/osdef.sh

pathdef.c: Makefile config.mk
	-@echo creating pathdef.c
	-@echo '/* pathdef.c */' > pathdef.c
	-@echo '/* This file is automatically created by Makefile' >> pathdef.c
	-@echo ' * DO NOT EDIT!  Change Makefile only. */' >> pathdef.c
	-@echo '#include "vim.h"' >> pathdef.c
	-@echo 'char_u *default_vim_dir = (char_u *)"$(VIMLOC)";' >> pathdef.c
	-@echo 'char_u *all_cflags = (char_u *)"$(CC) -c -I$(srcdir) $(ALL_CFLAGS)";' >> pathdef.c
	-@echo 'char_u *all_lflags = (char_u *)"$(CC) $(LDFLAGS) -o $(VIMTARGET) $(ALL_LIBS)";' >> pathdef.c
	-@sh $(srcdir)/pathdef.sh

Makefile:
	@echo The name of the makefile MUST be "Makefile" (with capital M)!!!!

###############################################################################
### (automatically generated by 'make depend')
### Dependencies:
buffer.o: buffer.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
charset.o: charset.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
digraph.o: digraph.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
edit.o: edit.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
eval.o: eval.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
ex_cmds.o: ex_cmds.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
ex_docmd.o: ex_docmd.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
ex_getln.o: ex_getln.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
fileio.o: fileio.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
getchar.o: getchar.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
main.o: main.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
mark.o: mark.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
memfile.o: memfile.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
memline.o: memline.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
message.o: message.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
misc1.o: misc1.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
misc2.o: misc2.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
normal.o: normal.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
ops.o: ops.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
option.o: option.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
os_unix.o: os_unix.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h os_unixx.h
pathdef.o: pathdef.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
quickfix.o: quickfix.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
regexp.o: regexp.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
screen.o: screen.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
search.o: search.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
syntax.o: syntax.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
tag.o: tag.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
term.o: term.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
ui.o: ui.c vim.h config.h feature.h os_unix.h osdef.h ascii.h keymap.h \
 term.h macros.h regexp.h structs.h gui.h globals.h option.h ex_cmds.h \
 proto.h
undo.o: undo.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
version.o: version.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h version.h
window.o: window.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
gui.o: gui.c vim.h config.h feature.h os_unix.h osdef.h ascii.h \
 keymap.h term.h macros.h regexp.h structs.h gui.h globals.h option.h \
 ex_cmds.h proto.h
gui_motif.o: gui_motif.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
gui_athena.o: gui_athena.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h gui_at_sb.h
gui_x11.o: gui_x11.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
gui_at_sb.o: gui_at_sb.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h gui_at_sb.h
if_perl.o: if_perl.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
if_perlsfio.o: if_perlsfio.c vim.h config.h feature.h os_unix.h \
 osdef.h ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h \
 globals.h option.h ex_cmds.h proto.h
if_python.o: if_python.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h
if_sniff.o: if_sniff.c vim.h config.h feature.h os_unix.h osdef.h \
 ascii.h keymap.h term.h macros.h regexp.h structs.h gui.h globals.h \
 option.h ex_cmds.h proto.h os_unixx.h
