TARGETOS=BOTH
APPVER=4.0

!include <win32.mak>

all: gvimext.dll

gvimext.dll:    gvimext.obj    \
                gvimext.res
  $(implib) -machine:$(CPU) -def:gvimext.def $** -out:gvimext.lib
  $(link) $(dlllflags) -base:0x1C000000 -out:$*.dll $** $(olelibsdll) shell32.lib gvimext.lib comctl32.lib gvimext.exp

.cpp.obj:
    $(cc) $(cflags) $(cvarsdll) $*.cpp

gvimext.res: gvimext.rc
    $(rc) $(rcflags) $(rcvars)  gvimext.rc
