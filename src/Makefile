#
# Makefile for Borland C++ 2.0
# Can also be used for Turbo C++
#
# The options are at the end of this file
#

.AUTODEPEND

#		*Translator Definitions*
#   use tcc for Turbo C++
CC = bcc +VIM.CFG
#CC = tcc +VIM.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
#
# Adjust the paths for your environment
#
LIBPATH = E:\BORLANDC\LIB
INCLUDEPATH = E:\BORLANDC\INCLUDE
 
 
#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }
 
.cpp.obj:
  $(CC) -c {$< }
 
#		*List Macros*
 
 
EXE_dependencies =  \
 alloc.obj \
 buffers.obj \
 charset.obj \
 cmdline.obj \
 csearch.obj \
 digraph.obj \
 edit.obj \
 fileio.obj \
 help.obj \
 linefunc.obj \
 main.obj \
 mark.obj \
 message.obj \
 misccmds.obj \
 msdos.obj \
 normal.obj \
 ops.obj \
 param.obj \
 quickfix.obj \
 regexp.obj \
 regsub.obj \
 screen.obj \
 script.obj \
 search.obj \
 storage.obj \
 tag.obj \
 term.obj \
 undo.obj \
 version.obj
 
#		*Explicit Rules*
#  add /v for source degubbing
vim.exe: vim.cfg $(EXE_dependencies)
  $(CC) -c version.c
  $(TLINK) /x/c/L$(LIBPATH) @&&|
c0l.obj+
alloc.obj+
buffers.obj+
charset.obj+
cmdline.obj+
csearch.obj+
digraph.obj+
edit.obj+
fileio.obj+
help.obj+
linefunc.obj+
main.obj+
mark.obj+
message.obj+
misccmds.obj+
msdos.obj+
normal.obj+
ops.obj+
param.obj+
quickfix.obj+
regexp.obj+
regsub.obj+
screen.obj+
script.obj+
search.obj+
storage.obj+
tag.obj+
term.obj+
undo.obj+
version.obj
vim
		# no map file
fp87.lib+
mathl.lib+
cl.lib
|
 
 
#		*Individual File Dependencies*
alloc.obj: alloc.c 
 
buffers.obj: buffers.c 
 
charset.obj: charset.c 
 
cmdline.obj: cmdline.c cmdtab.h
 
cmdtab.h: cmdtab.tab mkcmdtab.exe
	mkcmdtab cmdtab.tab cmdtab.h

mkcmdtab.exe: mkcmdtab.obj
	$(CC) -ml -omkcmdtab mkcmdtab.obj

mkcmdtab.obj: mkcmdtab.c
	$(CC) $(CFLAGS) mkcmdtab.c

csearch.obj: csearch.c 
 
digraph.obj: digraph.c 
 
edit.obj: edit.c 
 
fileio.obj: fileio.c 
 
help.obj: help.c 
 
linefunc.obj: linefunc.c 
 
main.obj: main.c globals.h param.h
 
mark.obj: mark.c mark.h
 
message.obj: message.c 
 
misccmds.obj: misccmds.c 
 
msdos.obj: msdos.c 
 
normal.obj: normal.c ops.h
 
ops.obj: ops.c ops.h
 
param.obj: param.c 
 
quickfix.obj: quickfix.c 
 
regexp.obj: regexp.c 
 
regsub.obj: regsub.c 
 
screen.obj: screen.c 
 
script.obj: script.c 
 
search.obj: search.c 
 
storage.obj: storage.c 
 
tag.obj: tag.c mark.h
 
term.obj: term.c term.h
 
undo.obj: undo.c 
 
version.obj: version.c 
 
#		*Compiler Configuration File*
# The following compile options can be changed for better machines.
#	replace -1- with -2 to produce code for a 80286 or higher
#	replace -1- with -3 to produce code for a 80386 or higher
#	add -v for source debugging
vim.cfg: makefile
  copy &&|
-ml
-1-
-f-
-C
-N
-O
-Z
-k-
-d
-h
-vi-
-H=VIM.SYM
-w-par
-w-pro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DPC;MSDOS;DIGRAPHS
| vim.cfg

#
# options:
### -DDIGRAPHS		digraph support (at the cost of 1.6 Kbyte code)
### -DNO_ARP		do not use arp.library, DOS 2.0 required
### -DCOMPATIBLE	start in vi-compatible mode
### -DNOBACKUP		default is no backup file
### -DDEBUG		output a lot of debugging garbage
### -DTERMCAP		include termcap file support
### -DNO_BUILTIN_TCAPS	do not include builtin termcap entries
###				(use only with -DTERMCAP)
### -DSOME_BUILTIN_TCAPS include most useful builtin termcap entries
###				(use only without -DNO_BUILTIN_TCAPS)
### -DALL_BUILTIN_TCAPS	include all builtin termcap entries
###				(use only without -DNO_BUILTIN_TCAPS)
### -DVIMRC_FILE	name of the .vimrc file in current dir
### -DEXRC_FILE		name of the .exrc file in current dir
### -DSYSVIMRC_FILE	name of the global .vimrc file
### -DSYSEXRC_FILE	name of the global .exrc file
### -DDEFVIMRC_FILE	name of the system-wide .vimrc file
### -DVIM_HLP		name of the help file
