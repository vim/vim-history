#
# Borland C++ 5.02 makefile for vim, 16-bit windows gui version
# By Vince Negri
# *************************************************************
# * WARNING!
# * This was originally produced by the IDE, but has since been
# * modifed to make it work properly. Adjust with care!
# * In particular, leave LinkerLocalOptsAtW16_gvim16dexe alone
# * unless you are a guru.
# *************************************************************
#
# Look for ctl3dv2.lib below and adjust the paths as required.

.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCC     = Bcc +BccW16.cfg 
TLINK   = TLink
TLIB    = TLib
BRC     = Brc
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_ResFLAGS = 
LinkerLocalOptsAtW16_gvim16dexe = /Twe/x/l/c/C/k/Orcai /P=65535 /V3.10 /LD:\BC5\LIB
ResLocalOptsAtW16_gvim16dexe =  -k-
BLocalOptsAtW16_gvim16dexe = 
CompInheritOptsAt_gvim16dexe = -ID:\BC5\INCLUDE;PROTO;. -DFEAT_GUI;FEAT_GUI_MSWIN;FEAT_GUI_W16;MSWIN;WIN16;MSWIN16_FASTTEXT;FEAT_TOOLBAR;WIN16_3DLOOK
LinkerInheritOptsAt_gvim16dexe = -x
LinkerOptsAt_gvim16dexe = $(LinkerLocalOptsAtW16_gvim16dexe)
ResOptsAt_gvim16dexe = $(ResLocalOptsAtW16_gvim16dexe)
BOptsAt_gvim16dexe = $(BLocalOptsAtW16_gvim16dexe)

#
# Dependency List
#
Dep_Gvim16 = \
   gvim16.exe

Gvim16 : BccW16.cfg $(Dep_Gvim16)
  echo MakeNode

Dep_gvim16dexe = \
   fold.obj\
   ..\..\..\..\bc5\lib\ctl3dv2.lib\
   vimtbar.lib\
   vim16.def\
   vim16.res\
   getchar.obj\
   gui.obj\
   gui_w16.obj\
   main.obj\
   mark.obj\
   memfile.obj\
   memline.obj\
   menu.obj\
   message.obj\
   misc1.obj\
   misc2.obj\
   normal.obj\
   ops.obj\
   option.obj\
   os_win16.obj\
   quickfix.obj\
   regexp.obj\
   screen.obj\
   search.obj\
   syntax.obj\
   tag.obj\
   term.obj\
   ui.obj\
   undo.obj\
   version.obj\
   window.obj\
   buffer.obj\
   charset.obj\
   digraph.obj\
   edit.obj\
   eval.obj\
   ex_cmds.obj\
   ex_docmd.obj\
   ex_getln.obj\
   fileio.obj

gvim16.exe : $(Dep_gvim16dexe)
  $(TLINK)   @&&|
  $(LinkerOptsAt_gvim16dexe)  +
D:\BC5\LIB\c0wl.obj+
fold.obj+
getchar.obj+
gui.obj+
gui_w16.obj+
main.obj+
mark.obj+
memfile.obj+
memline.obj+
menu.obj+
message.obj+
misc1.obj+
misc2.obj+
normal.obj+
ops.obj+
option.obj+
os_win16.obj+
quickfix.obj+
regexp.obj+
screen.obj+
search.obj+
syntax.obj+
tag.obj+
term.obj+
ui.obj+
undo.obj+
version.obj+
window.obj+
buffer.obj+
charset.obj+
digraph.obj+
edit.obj+
eval.obj+
ex_cmds.obj+
ex_docmd.obj+
ex_getln.obj+
fileio.obj
$<

..\..\..\..\bc5\lib\ctl3dv2.lib+
vimtbar.lib+
D:\BC5\LIB\import.lib+
D:\BC5\LIB\mathwl.lib+
D:\BC5\LIB\cwl.lib
vim16.def
vim16.res

|
fold.obj :  fold.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ fold.c
|

vim16.res :  vim16.rc
  $(BRC) -R @&&|
 $(IDE_ResFLAGS) $(ROptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe)  -FO$@ vim16.rc
|
getchar.obj :  getchar.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ getchar.c
|

gui.obj :  gui.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ gui.c
|

gui_w16.obj :  gui_w16.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ gui_w16.c
|

main.obj :  main.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ main.c
|

mark.obj :  mark.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ mark.c
|

memfile.obj :  memfile.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ memfile.c
|

memline.obj :  memline.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ memline.c
|

menu.obj :  menu.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ menu.c
|

message.obj :  message.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ message.c
|

misc1.obj :  misc1.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ misc1.c
|

misc2.obj :  misc2.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ misc2.c
|

normal.obj :  normal.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ normal.c
|

ops.obj :  ops.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ ops.c
|

option.obj :  option.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ option.c
|

os_win16.obj :  os_win16.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ os_win16.c
|

quickfix.obj :  quickfix.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ quickfix.c
|

regexp.obj :  regexp.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ regexp.c
|

screen.obj :  screen.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ screen.c
|

search.obj :  search.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ search.c
|

syntax.obj :  syntax.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ syntax.c
|

tag.obj :  tag.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ tag.c
|

term.obj :  term.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ term.c
|

ui.obj :  ui.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ ui.c
|

undo.obj :  undo.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ undo.c
|

version.obj :  version.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ version.c
|

window.obj :  window.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ window.c
|

buffer.obj :  buffer.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ buffer.c
|

charset.obj :  charset.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ charset.c
|

digraph.obj :  digraph.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ digraph.c
|

edit.obj :  edit.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ edit.c
|

eval.obj :  eval.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ eval.c
|

ex_cmds.obj :  ex_cmds.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ ex_cmds.c
|

ex_docmd.obj :  ex_docmd.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ ex_docmd.c
|

ex_getln.obj :  ex_getln.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ ex_getln.c
|

fileio.obj :  fileio.c
  $(BCC)   -P- -c @&&|
 $(CompOptsAt_gvim16dexe) $(CompInheritOptsAt_gvim16dexe) -o$@ fileio.c
|

# Compiler configuration file
BccW16.cfg : 
   Copy &&|
-w
-R
-v
-vi
-H
-H=gvim16.csm
-ml
-WE
-f-
-ff
-v-
-R-
-k-
-3
-a
-pr
-dc
-Og
-Ot
-Z
-O
-Oe
-Ol
-Ob
-OW
-Ov
-O2
-Op
-w-par
-wuse
-w-sig
-w-ucp
-H
| $@


