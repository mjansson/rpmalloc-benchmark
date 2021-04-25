#!/usr/bin/env python

"""Ninja build configurator for foundation library"""

import sys
import os
import copy

sys.path.insert(0, os.path.join('build', 'ninja'))

import generator

generator = generator.Generator(project = 'rpmalloc', variables = {'bundleidentifier': 'com.rampantpixels.rpmalloc.$(binname)', 'nowarning': True})
target = generator.target
writer = generator.writer
toolchain = generator.toolchain

variables = {'defines': ['NDEBUG=1'], 'cflags': ['-fno-builtin-malloc']}

def merge_variables(a, b):
	merged = copy.deepcopy(a)
	for k, v in b.items():
		if k in merged:
			merged[k] = list(merged[k]) + list(v)
		else:
			merged[k] = v
	return merged

includepaths = ['test', 'benchmark']
test_lib = generator.lib(module = 'test', sources = ['thread.c', 'timer.c'], includepaths = includepaths, variables = variables)
benchmark_lib = generator.lib(module = 'benchmark', sources = ['main.c'], includepaths = includepaths, variables = variables)

#Build one binary per benchmark
generator.bin(module = 'rpmalloc', sources = ['benchmark.c', 'rpmalloc.c'], binname = 'benchmark-rpmalloc', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = ['benchmark', 'test'], includepaths = includepaths, variables = variables)

if target.is_android():
	resources = [os.path.join('all', 'android', item) for item in [
		'AndroidManifest.xml', os.path.join('layout', 'main.xml'), os.path.join('values', 'strings.xml'),
		os.path.join('drawable-ldpi', 'icon.png'), os.path.join('drawable-mdpi', 'icon.png'), os.path.join('drawable-hdpi', 'icon.png'),
		os.path.join('drawable-xhdpi', 'icon.png'), os.path.join('drawable-xxhdpi', 'icon.png'), os.path.join('drawable-xxxhdpi', 'icon.png')
	]]
	appsources = [os.path.join('test', 'all', 'android', 'java', 'com', 'rampantpixels', 'foundation', 'test', item) for item in [
		'TestActivity.java'
	]]
	generator.app(module = '', sources = appsources, binname = 'benchmark-rpmalloc', basepath = '', implicit_deps = [benchmark_lib, test_lib], libs = ['benchmark', 'test'], resources = resources, includepaths = includepaths, variables = variables)

generator.bin(module = 'crt', sources = ['benchmark.c'], binname = 'benchmark-crt', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = ['benchmark', 'test'], includepaths = includepaths, variables = {'defines': ['NDEBUG=1']})
if not target.is_android():
	generator.bin(module = 'nedmalloc', sources = ['benchmark.c', 'nedmalloc.c'], binname = 'benchmark-nedmalloc', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = ['benchmark', 'test'], includepaths = includepaths, variables = variables)

platform_includepaths = [os.path.join('benchmark', 'ptmalloc3')]
if target.is_windows():
	platform_includepaths += [os.path.join('benchmark', 'ptmalloc3', 'sysdeps', 'windows')]
else:
	platform_includepaths += [os.path.join('benchmark', 'ptmalloc3', 'sysdeps', 'pthread')]
if not target.is_android():
	generator.bin(module = 'ptmalloc3', sources = ['benchmark.c', 'ptmalloc3.c', 'malloc.c'], binname = 'benchmark-ptmalloc3', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = ['benchmark', 'test'], includepaths = includepaths + platform_includepaths, variables = variables)

hoardincludepaths = [
	os.path.join('benchmark', 'hoard', 'include'),
	os.path.join('benchmark', 'hoard', 'include', 'hoard'),
	os.path.join('benchmark', 'hoard', 'include', 'util'),
	os.path.join('benchmark', 'hoard', 'include', 'superblocks'),
	os.path.join('benchmark', 'hoard'),
	os.path.join('benchmark', 'hoard', 'Heap-Layers')
]
hoardsources = ['source/libhoard.cpp']
if target.is_macos() or target.is_ios():
	hoardsources += ['Heap-Layers/wrappers/macwrapper.cpp']
elif target.is_windows():
	hoardsources += ['Heap-Layers/wrappers/winwrapper.cpp']
else:
	hoardsources += ['Heap-Layers/wrappers/gnuwrapper.cpp']
if target.is_macos() or target.is_ios():
	hoardsources += ['source/mactls.cpp']
elif target.is_windows():
	hoardsources += ['source/wintls.cpp']
else:
	hoardsources += ['source/unixtls.cpp']
if not target.is_android():
	hoard_variables = merge_variables({'runtime': 'c++'}, variables)
	hoard_lib = generator.lib(module = 'hoard', sources = hoardsources, basepath = 'benchmark', includepaths = includepaths + hoardincludepaths, variables = hoard_variables)
	hoard_depend_libs = ['hoard', 'benchmark', 'test']
	generator.bin(module = 'hoard', sources = ['benchmark.c'], binname = 'benchmark-hoard', basepath = 'benchmark', implicit_deps = [hoard_lib, benchmark_lib, test_lib], libs = hoard_depend_libs, includepaths = includepaths, variables = hoard_variables)

gperftoolsincludepaths = [
	os.path.join('benchmark', 'gperftools', 'src'),
	os.path.join('benchmark', 'gperftools', 'src', 'base'),
	os.path.join('benchmark', 'gperftools', 'src', target.get())
]
gperftoolsbasesources = [
	'dynamic_annotations.c', 'linuxthreads.cc', 'logging.cc', 'low_level_alloc.cc', 'spinlock.cc',
	'spinlock_internal.cc', 'sysinfo.cc'
]
if not target.is_windows():
	gperftoolsbasesources += ['thread_lister.c']
gperftoolsbasesources = [os.path.join('src', 'base', path) for path in gperftoolsbasesources]
gperftoolssources = [
	'central_freelist.cc', 'common.cc', 'internal_logging.cc',
	'malloc_extension.cc', 'malloc_hook.cc', 'memfs_malloc.cc', 
	'page_heap.cc', 'sampler.cc', 'stack_trace_table.cc',
	'static_vars.cc', 'span.cc', 'symbolize.cc', 'tcmalloc.cc', 'thread_cache.cc'
]
if not target.is_windows():
	gperftoolssources += ['maybe_threads.cc', 'system-alloc.cc']
if target.is_windows():
	gperftoolssources += [os.path.join('windows', 'port.cc'), os.path.join('windows', 'system-alloc.cc')]
gperftoolssources = [os.path.join('src', path) for path in gperftoolssources]
if not target.is_android():
	gperf_variables = merge_variables({'runtime': 'c++', 'defines': ['NO_TCMALLOC_SAMPLES', 'NO_HEAP_CHECK']}, variables)
	gperftools_lib = generator.lib(module = 'gperftools', sources = gperftoolsbasesources + gperftoolssources, basepath = 'benchmark', includepaths = includepaths + gperftoolsincludepaths, variables = gperf_variables)
	gperftools_depend_libs = ['gperftools', 'benchmark', 'test']
	generator.bin(module = 'gperftools', sources = ['benchmark.c'], binname = 'benchmark-tcmalloc', basepath = 'benchmark', implicit_deps = [gperftools_lib, benchmark_lib, test_lib], libs = gperftools_depend_libs, includepaths = includepaths, variables = gperf_variables)

jemallocincludepaths = [
	os.path.join('benchmark', 'jemalloc', 'include'),
	os.path.join('benchmark', 'jemalloc', 'include', 'jemalloc'),
	os.path.join('benchmark', 'jemalloc', 'include', 'jemalloc', 'internal')
]
jemallocsources = [
	'arena.c', 'background_thread.c', 'base.c', 'bin.c', 'bitmap.c', 'ckh.c', 'ctl.c', 'div.c', 'extent.c',
	'extent_dss.c', 'extent_mmap.c', 'hash.c', 'hook.c', 'jemalloc.c', 'large.c', 'log.c', 'malloc_io.c',
	'mutex.c', 'mutex_pool.c', 'nstime.c', 'pages.c', 'prng.c', 'prof.c', 'rtree.c', 'safety_check.c',
	'sc.c', 'stats.c', 'sz.c', 'tcache.c', 'test_hooks.c', 'ticker.c', 'tsd.c', 'witness.c'
]
jemallocsources = [os.path.join('src', path) for path in jemallocsources]
if not target.is_windows() and not target.is_android():
	je_variables = merge_variables({'defines': ['JEMALLOC_NO_RENAME']}, variables)
	jemalloc_lib = generator.lib(module = 'jemalloc', sources = jemallocsources, basepath = 'benchmark', includepaths = includepaths + jemallocincludepaths, variables = je_variables)
	jemalloc_depend_libs = ['jemalloc', 'benchmark', 'test']
	generator.bin(module = 'jemalloc', sources = ['benchmark.c'], binname = 'benchmark-jemalloc', basepath = 'benchmark', implicit_deps = [jemalloc_lib, benchmark_lib, test_lib], libs = jemalloc_depend_libs, includepaths = includepaths, variables = je_variables)

scallocincludepaths = [
	os.path.join('benchmark', 'scalloc', 'src'),
	os.path.join('benchmark', 'scalloc', 'src', 'platform')
]
scallocsources = [
	'glue.cc'
]
scallocsources = [os.path.join('src', path) for path in scallocsources]
if not target.is_windows() and not target.is_android():
	scalloc_variables = merge_variables({'runtime': 'c++'}, variables)
	scalloc_lib = generator.lib(module = 'scalloc', sources = scallocsources, basepath = 'benchmark', includepaths = includepaths + scallocincludepaths, variables = scalloc_variables)
	scalloc_depend_libs = ['scalloc', 'benchmark', 'test']
	generator.bin(module = 'scalloc', sources = ['benchmark.c'], binname = 'benchmark-scalloc', basepath = 'benchmark', implicit_deps = [scalloc_lib, benchmark_lib, test_lib], libs = scalloc_depend_libs, includepaths = includepaths, variables = scalloc_variables)

if not target.is_windows():
	lockfree_malloc_depend_libs = ['benchmark', 'test']
	if not target.is_android():
		lockfree_variables = merge_variables({'runtime': 'c++'}, variables)
		generator.bin(module = 'lockfree-malloc', sources = ['benchmark.c', 'lite-malloc.cpp'], binname = 'benchmark-lockfree-malloc', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = lockfree_malloc_depend_libs, includepaths = includepaths, variables = lockfree_variables)

if not target.is_windows():
	bmallocincludepaths = [
		os.path.join('benchmark', 'bmalloc', 'bmalloc')
	]
	bmallocsources = [
		'AllIsoHeaps.cpp', 'Allocator.cpp', 'AvailableMemory.cpp', 'bmalloc.cpp', 'Cache.cpp', 'CryptoRandom.cpp',
		'Deallocator.cpp', 'DebugHeap.cpp', 'Environment.cpp', 'FreeList.cpp', 'Gigacage.cpp', 'Heap.cpp',
		'HeapKind.cpp', 'IsoHeapImpl.cpp', 'IsoPage.cpp', 'IsoSharedHeap.cpp', 'IsoSharedPage.cpp', 'IsoTLS.cpp',
		'IsoTLSEntry.cpp', 'IsoTLSLayout.cpp', 'LargeMap.cpp', 'Logging.cpp', 'mbmalloc.cpp', 'Mutex.cpp',
		'ObjectType.cpp', 'PerProcess.cpp', 'PerThread.cpp', 'Scavenger.cpp', 'StaticMutex.cpp', 'VMHeap.cpp'
	]
	if target.is_macos() or target.is_ios():
		bmallocsources += ['Zone.cpp']
	bmallocsources = [os.path.join('bmalloc', path) for path in bmallocsources]
	if not target.is_android():
		bmalloc_variables = merge_variables({'runtime': 'c++'}, variables)
		bmalloc_lib = generator.lib(module = 'bmalloc', sources = bmallocsources, basepath = 'benchmark', includepaths = includepaths + bmallocincludepaths, variables = bmalloc_variables)
		bmalloc_depend_libs = ['bmalloc', 'benchmark', 'test']
		generator.bin(module = 'bmalloc', sources = ['benchmark.cc'], binname = 'benchmark-bmalloc', basepath = 'benchmark', implicit_deps = [bmalloc_lib, benchmark_lib, test_lib], libs = bmalloc_depend_libs, includepaths = includepaths, variables = bmalloc_variables)

#Requires transactional memory for full performance?
if not target.is_windows():
	supermallocincludepaths = [
		os.path.join('benchmark', 'supermalloc', 'src')
	]
	supermallocsources = [
		'bassert.cc', 'cache.cc', 'env.cc', 'footprint.cc', 'futex_mutex.cc', 'has_tsx.cc', 'huge_malloc.cc',
		'large_malloc.cc', 'makechunk.cc', 'malloc.cc', 'rng.cc', 'small_malloc.cc', 'stats.cc',
		'generated_constants.cc'
	]
	supermallocsources = [os.path.join('src', path) for path in supermallocsources]
	if not target.is_android():
		supermalloc_variables = {'cflags': ['-mrtm'], 'runtime': 'c++', 'defines': ['NDEBUG=1']}
		supermalloc_lib = generator.lib(module = 'supermalloc', sources = supermallocsources, basepath = 'benchmark', includepaths = includepaths + supermallocincludepaths, variables = supermalloc_variables)
		supermalloc_depend_libs = ['supermalloc', 'benchmark', 'test']
		generator.bin(module = 'supermalloc', sources = ['benchmark.c'], binname = 'benchmark-supermalloc', basepath = 'benchmark', implicit_deps = [supermalloc_lib, benchmark_lib, test_lib], libs = supermalloc_depend_libs, includepaths = includepaths, variables = supermalloc_variables)

#Lockless only seems to build with gcc
if toolchain.name() == "gcc":
	lockless_depend_libs = ['benchmark', 'test']
	if target.is_linux():
		lockless_variables = merge_variables({'defines': ['USE_PREFIX']}, variables)
		generator.bin(module = 'lockless', sources = ['benchmark.c', 'll_alloc.c'], binname = 'benchmark-lockless', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = lockless_depend_libs, includepaths = includepaths, variables = lockless_variables)

if not target.is_windows():
	smmallocsources = [
		'smmalloc.cpp', 'smmalloc_generic.cpp', 'smmalloc_tls.cpp'
	]
	smmalloc_variables = {'defines': ['_M_X64=1'], 'runtime': 'c++'}
	smmalloc_depend_libs = ['benchmark', 'test']
	generator.bin(module = 'smmalloc', sources = ['benchmark.cpp'] + smmallocsources, binname = 'benchmark-smmalloc', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = smmalloc_depend_libs, includepaths = includepaths, variables = smmalloc_variables)

mimallocsources = [
	'stats.c', 'os.c', 'segment.c', 'page.c', 'random.c', 'arena.c', 'bitmap.c', 'alloc.c', 'alloc-aligned.c',
    'segment-cache.c', 'heap.c', 'options.c', 'init.c'
]
mimallocsources = [os.path.join('src', path) for path in mimallocsources]
mimalloc_variables = {'defines': ['MI_DEBUG=0']}
mimallocincludepaths = [
	os.path.join('benchmark', 'mimalloc', 'include')
]
mimalloc_depend_libs = ['benchmark', 'test']
generator.bin(module = 'mimalloc', sources = ['benchmark.c'] + mimallocsources, binname = 'benchmark-mimalloc', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = mimalloc_depend_libs, includepaths = includepaths + mimallocincludepaths, variables = mimalloc_variables)
