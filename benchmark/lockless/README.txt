To compile on Linux, use the makefile.

"make staticlib" will create the static version of the Lockless memory allocator.  You can also simply add the ll_alloc.c file to the list of other source files in your project.

"make dynamiclib" will create the dynamic version of the Lockless memory allocator.  This is the version you want if you want to use LD_PRELOAD to add the use of the library to already compiled applications.


Compiling for Microsoft Windows is much more complex.  First, you will need to cross-compile with mingw-w64 to create the object file for ll_alloc.c  Second, log onto windows, and then compile win_stub.cc with MS Visual C++.  Finally, link the two objects together using Microsoft's linker to create the library.  (This complex build method is due to mingw-64 not supporting thread local variables correctly, and MSVC not supporting inline asm.  If either compilers change, this could be simplified.)

If you want to create the dynamic (dll) version, use the -DUSE_DLL and /DUSE_DLL command line switches, as shown in the example compilation instructions within the makefile.


Remember, this library is licensed under the GPL version 3 (or any later version).  If you wish to redistribute programs linked to this library, all source code must be redistributed as well.  If you wish to create closed-source proprietory software using the Lockless Memory allocator, other licenses are available.  Contact us at locklessinc.com for details.
