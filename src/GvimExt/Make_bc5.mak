### USEDLL: no for statically linked version of run-time, yes for DLL runtime
### (requires cc3250.dll be available in %PATH%)
!if ("$(USEDLL)"=="")
USEDLL = no
!endif

!if ("$(USEDLL)"=="yes")
RT_DEF = -D_RTLDLL
RT_LIB = cw32i.lib
!else
RT_DEF =
RT_LIB = cw32.lib
!endif


all : gvimext.dll

gvimext.obj : gvimext.cpp gvimext.h
	bcc32 -tWD -c -DFEAT_GETTEXT $(RT_DEF) -w- gvimext.cpp

gvimext.res : gvimext.rc
	brc32 -r gvimext.rc

gvimext.dll : gvimext.obj gvimext.res
	ilink32 -aa gvimext.obj, gvimext.dll, , c0d32.obj $(RT_LIB) import32.lib, gvimext.def, gvimext.res
