# Project: gvimext
# Generates gvimext.dll with gcc.
# Can be used for Cygwin and MingW (MingW ignores -mno-cygwin)
#
# Originally, the DLL base address was fixed: -Wl,--image-base=0x1C000000 
# Now it is allocated dymanically by the linker by evaluating all DLLs 
# already loaded in memory. The binary image contains as well information
# for automatic pseudo-rebasing, if needed by the system. ALV 2004-02-29 

CXX := g++
CXXFLAGS := -O2 -mno-cygwin
LIBS :=  -luuid
WINDRES := windres.exe
RES  := gvimext.res
DEFFILE = gvimext_ming.def
OBJ  := gvimext.o 

DLL  := gvimext.dll

.PHONY: all all-before all-after clean clean-custom

all: all-before $(DLL) all-after

$(DLL): $(OBJ) $(RES) $(DEFFILE)
	$(CXX) -shared $(CXXFLAGS) -s -o $@  \
		-Wl,--enable-auto-image-base \
		-Wl,--enable-auto-import  \
		-Wl,--whole-archive  \
			$^  \
		-Wl,--no-whole-archive  \
			$(LIBS)

gvimext.o: gvimext.cpp
	$(CXX) $(CXXFLAGS) -DFEAT_GETTEXT -c $^ -o $@ 

$(RES): gvimext_ming.rc
	$(WINDRES) --input-format=rc --output-format=coff -DMING $^ -o $@

clean: clean-custom
	$(RM)  $(OBJ) $(RES) $(DLL)

