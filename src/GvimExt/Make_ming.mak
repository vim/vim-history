# Project: gvimext
# Generates gvimext.dll with mingw
#

CPP  := g++.exe
CC   := gcc.exe
WINDRES := windres.exe
RES  := gvimext.res
OBJ  := gvimext.o $(RES)
LIBS :=  -luuid

INCS :=
DLL  := gvimext.dll

.PHONY: all all-before all-after clean clean-custom

all: all-before $(DLL) all-after


clean: clean-custom
	${RM}  $(OBJ) $(DLL) ${RES} ${EXPLIB} $(STATICLIB)

DLLWRAP=dllwrap.exe
DEFFILE=gvimext_ming.def
STATICLIB=gvimext.a
EXPLIB=gvimext.exp

$(DLL): $(OBJ)
	$(DLLWRAP)  --def $(DEFFILE) \
		--output-exp ${EXPLIB} \
		--image-base 0x1C000000 \
		--driver-name c++ \
		--implib $(STATICLIB) \
		$(OBJ) $(LIBS) \
		--target=i386-mingw32 -mno-cygwin \
		-o $(DLL) -s

gvimext.o: gvimext.cpp
	$(CPP) -c $? -o $@ $(CXXFLAGS) -DFEAT_GETTEXT

${RES}: gvimext_ming.rc
	$(WINDRES) $? -I rc -o $@ -O coff  -DMING
