all : gvimext.dll

gvimext.obj : gvimext.cpp gvimext.h
	bcc32 -tWD -c -DFEAT_GETTEXT -w- gvimext.cpp

gvimext.res : gvimext.rc
	brc32 -r gvimext.rc

gvimext.dll : gvimext.obj gvimext.res
	ilink32 -aa gvimext.obj, gvimext.dll, , c0d32.obj cw32i.lib import32.lib, gvimext.def, gvimext.res
