# Makefile for GvimExt, using MSVC

TARGETOS=BOTH
APPVER=4.0

!include <win32.mak>

all: gvimext.dll

gvimext.dll:    gvimext.obj	\
                gvimext.res
  $(implib) -machine:$(CPU) -def:gvimext.def $** -out:gvimext.lib
  $(link) $(dlllflags) -base:0x1C000000 -out:$*.dll $** $(olelibsdll) shell32.lib gvimext.lib comctl32.lib gvimext.exp

gvimext.obj: gvimext.h

.cpp.obj:
    $(cc) $(cflags) -DFEAT_GETTEXT $(cvarsdll) $*.cpp

gvimext.res: gvimext.rc
    $(rc) $(rcflags) $(rcvars)  gvimext.rc

clean:
    - if exist gvimext.dll del gvimext.dll
    - if exist gvimext.obj del gvimext.obj
    - if exist gvimext.res del gvimext.res
