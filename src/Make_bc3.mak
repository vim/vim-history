#
# Makefile for Borland C++ 2.0, 3.1 or 4.0 to compile a 16 bit version of Vim.
# Can also be used for Turbo C++
#
# The compilation options are at the end of this file

# Command line variables:
# BOR		path to root of Borland C (E:\BORLANDC)
# DEBUG		set to "yes" for debugging (no)
# SPAWNO	path to the spawno library directory, empty if you do not have
#		it; use 8.3 filenames! (C:\CC\SPAWN)

.AUTODEPEND

!ifndef BOR
BOR = E:\BORLANDC
# Bcc 3.1 doesn't work for xxd
BORXXD = E:\BC4
!else
BORXXD = $(BOR)
!endif

!if ("$(DEBUG)" == "yes")
DEBUG_FLAG = -v
!else
DEBUG_FLAG =
!endif

#   use tcc for Turbo C++
CC = $(BOR)\bin\bcc.exe +VIM.CFG
#CC = $(BOR)\bin\tcc.exe +VIM.CFG

TLINK = $(BOR)\bin\tlink.exe

!ifndef SPAWNO
SPAWNO = C:\CC\SPAWN
!endif

!if ("$(SPAWNO)" == "")
LIBPATH = $(BOR)\LIB
INCLUDEPATH = $(BOR)\INCLUDE
SPAWND =
SPAWNL =
!else
LIBPATH = $(BOR)\LIB;$(SPAWNO)
INCLUDEPATH = $(BOR)\INCLUDE;$(SPAWNO)
SPAWND = ;SPAWNO
SPAWNL = spawnl.lib
!endif


#		*Implicit Rules*
#
# use -v for debugging
#
.c.obj:
  $(CC) -c $(DEBUG_FLAG) {$< }

#		*List Macros*


EXE_dependencies =  \
 buffer.obj \
 charset.obj \
 diff.obj \
 digraph.obj \
 edit.obj \
 eval.obj \
 ex_cmds.obj \
 ex_cmds2.obj \
 ex_docmd.obj \
 ex_eval.obj \
 ex_getln.obj \
 fileio.obj \
 fold.obj \
 getchar.obj \
 main.obj \
 mark.obj \
 memfile.obj \
 memline.obj \
 menu.obj \
 message.obj \
 misc1.obj \
 misc2.obj \
 move.obj \
 os_msdos.obj \
 normal.obj \
 ops.obj \
 option.obj \
 quickfix.obj \
 regexp.obj \
 screen.obj \
 search.obj \
 syntax.obj \
 tag.obj \
 term.obj \
 ui.obj \
 undo.obj \
 window.obj \
 version.obj

all: vim.exe install.exe uninstal.exe xxd/xxd.exe

#		*Explicit Rules*
#  add /v to TLINK for debugging
vim.exe: vim.cfg $(EXE_dependencies)
  $(CC) $(DEBUG_FLAG) -c version.c
  $(TLINK) /x/c/L$(LIBPATH) $(DEBUG_FLAG) @&&|
c0l.obj+
buffer.obj+
charset.obj+
diff.obj+
digraph.obj+
edit.obj+
eval.obj+
ex_cmds.obj+
ex_cmds2.obj+
ex_docmd.obj+
ex_eval.obj+
ex_getln.obj+
fileio.obj+
fold.obj+
getchar.obj+
main.obj+
mark.obj+
memfile.obj+
memline.obj+
menu.obj+
message.obj+
misc1.obj+
misc2.obj+
move.obj+
os_msdos.obj+
normal.obj+
ops.obj+
option.obj+
quickfix.obj+
regexp.obj+
screen.obj+
search.obj+
syntax.obj+
tag.obj+
term.obj+
ui.obj+
undo.obj+
window.obj+
version.obj
vim
		# no map file
fp87.lib+
mathl.lib+
$(SPAWNL) cl.lib
|

install.exe: dosinst.c
	$(CC) -einstall $(DEBUG_FLAG) dosinst.c

uninstal.exe: uninstal.c
	$(CC) $(DEBUG_FLAG) uninstal.c

# This may fail for older make versions, building xxd will fail anyway then.
xxd/xxd.exe: xxd/xxd.c
	cd xxd
	$(MAKE) -f Make_bc3.mak BOR=$(BORXXD) DEBUG=$(DEBUG)
	cd ..

# cleaning up: Delete all generated files
clean:
	-del *.obj
	-del vim.exe
	-del vim.sym
	-del install.exe
	-del xxd\*.obj
	-del xxd\xxd.exe
	-del vim.cfg
	-del testdir\*.out

#		*Individual File Dependencies*
buffer.obj: buffer.c

charset.obj: charset.c

diff.obj: diff.c

digraph.obj: digraph.c

edit.obj: edit.c

eval.obj: eval.c

ex_cmds.obj: ex_cmds.c

ex_cmds2.obj: ex_cmds2.c

ex_docmd.obj: ex_docmd.c ex_cmds.h

ex_eval.obj: ex_eval.c ex_cmds.h

ex_getln.obj: ex_getln.c

fileio.obj: fileio.c

fold.obj: fold.c

getchar.obj: getchar.c

main.obj: main.c globals.h option.h

mark.obj: mark.c

memfile.obj: memfile.c

memline.obj: memline.c

menu.obj: menu.c

message.obj: message.c

misc1.obj: misc1.c

misc2.obj: misc2.c

move.obj: move.c

os_msdos.obj: os_msdos.c

normal.obj: normal.c

ops.obj: ops.c

option.obj: option.c

quickfix.obj: quickfix.c

regexp.obj: regexp.c

screen.obj: screen.c

search.obj: search.c

syntax.obj: syntax.c

tag.obj: tag.c

term.obj: term.c term.h

ui.obj: ui.c

undo.obj: undo.c

window.obj: window.c

version.obj: version.c version.h

#		*Compiler Configuration File*
# The following compile options can be changed for better machines.
#	replace -1- with -2 to produce code for a 80286 or higher
#	replace -1- with -3 to produce code for a 80386 or higher
#	add -v for source debugging
vim.cfg: Makefile
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
-weas
-wpre
-Iproto
-I$(INCLUDEPATH)
-L$(LIBPATH)
-DPC;MSDOS$(SPAWND)
| vim.cfg

test:
	cd testdir
	copy make_dos.mak makefile
	$(MAKE) small
	cd ..
