CFLAGS := -fomit-frame-pointer -Wcast-qual -Wmissing-format-attribute -Wlogical-op -Wstrict-aliasing -Wsign-compare -Wdeclaration-after-statement -Wnested-externs -Wdisabled-optimization -Winline -Wundef -Wimplicit -Wunused -Wfloat-equal -Winit-self -Wformat=2 -Wswitch -Wsequence-point -Wparentheses -Wimplicit -Wchar-subscripts -Wredundant-decls -Wstrict-prototypes -Wbad-function-cast -Wpointer-arith -Wwrite-strings -Wno-long-long -Wmissing-declarations -Wmissing-prototypes -Wextra -Wall -pedantic -ggdb3 -std=gnu99 -O3
CPPFLAGS := 
LIBS := 
LDFLAGS :=

# To compile the stub on windows, you'll need to set up these batch files to
# call MSVC with the right (64bit) environment.
CL_WIN := cl64.bat
LINK_WIN := link64.bat
# The main .c file is cross-compiled with mingw-w64
CC_WIN := x86_64-w64-mingw32-gcc -mwin32
WIN_CFLAGS = $(CFLAGS) -fno-leading-underscore
WIN_STRIP = x86_64-w64-mingw32-strip --strip-debug --strip-unneeded

ALLOCLIBMAJOR := 1
ALLOCLIBMINOR := 3

ALLOCSFX := .so.$(ALLOCLIBMAJOR).$(ALLOCLIBMINOR)

ALLOCLIBM := libllalloc.so.$(ALLOCLIBMAJOR)
ALLOCLIB := $(ALLOCLIBM).$(ALLOCLIBMINOR)

AR := ar
RANLIB := ranlib
STRIP := strip

# Expand dependencies one level
dependless = %.o %.a %.d %.h
expand = $($(var)) $(var) $(var).d
depend_test = $(if $(filter $(dependless),$(var)),$(var),$(expand))
depend = $(sort $(foreach var,$(1),$(depend_test)))

& = $(filter-out %.h %.d,$^)

include $(wildcard *.d)

DEPEND = $(SHELL) -ec 'gcc -MM $(CPPFLAGS) $< | sed -n "H;$$ {g;s@.*:\(.*\)@$< := \$$\(wildcard\1\)\n$*.o $@: $$\($<\)@;p}" > $@'

default: staticlib dynamiclib

staticlib: libllalloc.a

dynamiclib: libllalloc$(ALLOCSFX)

%.S.d: %.S
	$(DEPEND)

%.c.d: %.c
	$(DEPEND)

libllalloc.o: $(call depend,ll_alloc.c)
	$(CC) $& $(CFLAGS) $(LDFLAGS) -fPIC -pthread -c -o $@ $(LIBS)

libllalloc.a: libllalloc.o
	$(STRIP) -g $^
	$(AR) rcs $@ $^
	$(RANLIB) $@

libllalloc$(ALLOCSFX): $(call depend,ll_alloc.c)
	$(CC) $& $(CFLAGS) $(LDFLAGS) -shared -fpic -Wl,-soname,libllalloc$(ALLOCSFX) -Wl,-z,interpose -o $@ $(LIBS)
	$(STRIP) $@


# Create the object files for the windows version via cross-compiling
llalloc.obj: $(call depend,ll_alloc.c)
	$(CC_WIN) $(WIN_CFLAGS) $& -c -o $@
	$(WIN_STRIP) $@

lldalloc.obj: $(call depend,ll_alloc.c)
	$(CC_WIN) $(WIN_CFLAGS) $& -c -o $@ -DUSE_DLL
	$(WIN_STRIP) $@

# Use these commands within windows to compile the stubs
win_stub.obj: win_stub.cc
	$(CL_WIN) /nologo /O2 win_stub.cc -c /Fowin_stub.obj

win_stubd.obj: win_stub.cc
	$(CL_WIN) /nologo /O2 win_stub.cc -c /Fowin_stubd.obj /DUSE_DLL

# Link the stubs with the main object file to create the library
llalloc.lib: llalloc.obj win_stub.obj
	$(LINK_WIN) /nologo /OUT:llalloc.lib llalloc.obj win_stub.obj

llalloc.dll: lldalloc.obj win_stubd.obj
	$(LINK_WIN) /nologo /OUT:lldalloc.dll /DLL /DEF:alexport.def /BASE:0x63800000 ldalloc.obj win_stubd.obj
	mv lldalloc.dll llalloc.dll

clean:
	rm -f *.o *.a *.so *.so.* *.d *.obj

