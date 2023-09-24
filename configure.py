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
	gperf_variables = merge_variables({'runtime': 'c++', 'defines': ['NO_TCMALLOC_SAMPLES', 'NO_HEAP_CHECK'], 'nowarning': True}, variables)
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

snmallocincludepaths = [
	os.path.join('benchmark', 'snmalloc', 'src'),
]
snmallocsources = [os.path.join('src', 'override', 'malloc.cc')]
snvariables = merge_variables({'defines': ['SNMALLOC_STATIC_LIBRARY=1', 'SNMALLOC_STATIC_LIBRARY_PREFIX=sn_'], 'cflags': ['-mcx16'], 'runtime': 'c++'}, variables)
snmalloc_lib = generator.lib(module = 'snmalloc', sources = snmallocsources, basepath = 'benchmark', includepaths = includepaths + snmallocincludepaths, variables = snvariables)
snmalloc_depend_libs = ['snmalloc', 'benchmark', 'test', 'WindowsApp']
generator.bin(module = 'snmalloc', sources = ['benchmark.cc'], binname = 'benchmark-snmalloc', basepath = 'benchmark', implicit_deps = [snmalloc_lib, benchmark_lib, test_lib], libs = snmalloc_depend_libs, includepaths = includepaths + snmallocincludepaths, variables = snvariables)

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
		os.path.join('benchmark', 'bmalloc', 'bmalloc'),
		os.path.join('benchmark', 'bmalloc', 'libpas', 'src', 'libpas')
	]
	libpassources = [
		'bmalloc_heap.c', 'bmalloc_heap_config.c', 'bmalloc_type.c', 'hotbit_heap.c', 'hotbit_heap_config.c', 'iso_heap.c',
		'iso_heap_config.c', 'iso_test_heap.c',	'iso_test_heap_config.c', 'jit_heap.c', 'jit_heap_config.c', 'minalign32_heap.c',
		'minalign32_heap_config.c', 'pagesize64k_heap.c', 'pagesize64k_heap_config.c', 'pas_alignment.c', 'pas_all_heaps.c',
		'pas_allocation_callbacks.c', 'pas_allocation_result.c', 'pas_all_shared_page_directories.c', 'pas_baseline_allocator.c',
		'pas_baseline_allocator_table.c', 'pas_basic_heap_config_enumerator_data.c', 'pas_bitfit_allocator.c', 'pas_bitfit_directory.c',
		'pas_bitfit_heap.c', 'pas_bitfit_page.c', 'pas_bitfit_page_config_kind.c', 'pas_bitfit_size_class.c', 'pas_bitfit_view.c',
		'pas_bootstrap_free_heap.c', 'pas_bootstrap_heap_page_provider.c', 'pas_coalign.c', 'pas_commit_span.c', 'pas_committed_pages_vector.c',
		'pas_compact_bootstrap_free_heap.c', 'pas_compact_expendable_memory.c', 'pas_compact_heap_reservation.c', 'pas_compact_large_utility_free_heap.c',
		'pas_compute_summary_object_callbacks.c', 'pas_create_basic_heap_page_caches_with_reserved_memory.c', 'pas_deallocate.c',
		'pas_debug_spectrum.c', 'pas_deferred_decommit_log.c', 'pas_designated_intrinsic_heap.c', 'pas_dyld_state.c',
		'pas_dynamic_primitive_heap_map.c', 'pas_ensure_heap_forced_into_reserved_memory.c', 'pas_ensure_heap_with_page_caches.c',
		'pas_enumerable_page_malloc.c', 'pas_enumerable_range_list.c', 'pas_enumerate_bitfit_heaps.c', 'pas_enumerate_initially_unaccounted_pages.c',
		'pas_enumerate_large_heaps.c', 'pas_enumerate_segregated_heaps.c', 'pas_enumerate_unaccounted_pages_as_meta.c', 'pas_enumerator.c',
		'pas_enumerator_region.c', 'pas_epoch.c', 'pas_exclusive_view_template_memo_table.c', 'pas_expendable_memory.c', 'pas_extended_gcd.c',
		'pas_fast_large_free_heap.c', 'pas_fast_megapage_cache.c', 'pas_fast_megapage_table.c', 'pas_fd_stream.c', 'pas_free_granules.c',
		'pas_heap.c', 'pas_heap_config.c', 'pas_heap_config_kind.c', 'pas_heap_config_utils.c', 'pas_heap_for_config.c',
		'pas_heap_lock.c', 'pas_heap_ref.c', 'pas_heap_runtime_config.c', 'pas_heap_summary.c', 'pas_heap_table.c', 'pas_immortal_heap.c',
		'pas_large_expendable_memory.c', 'pas_large_free_heap_deferred_commit_log.c', 'pas_large_free_heap_helpers.c', 'pas_large_heap.c',
		'pas_large_heap_physical_page_sharing_cache.c', 'pas_large_map.c', 'pas_large_sharing_pool.c', 'pas_large_utility_free_heap.c',
		'pas_lenient_compact_unsigned_ptr.c', 'pas_local_allocator.c', 'pas_local_allocator_scavenger_data.c', 'pas_local_view_cache.c',
		'pas_lock.c', 'pas_lock_free_read_ptr_ptr_hashtable.c', 'pas_log.c', 'pas_malloc_stack_logging.c', 'pas_medium_megapage_cache.c',
		'pas_megapage_cache.c', 'pas_monotonic_time.c', 'pas_page_base.c', 'pas_page_base_config.c', 'pas_page_header_table.c',
		'pas_page_malloc.c', 'pas_page_sharing_participant.c', 'pas_page_sharing_pool.c', 'pas_payload_reservation_page_list.c',
		'pas_physical_memory_transaction.c', 'pas_primitive_heap_ref.c', 'pas_probabilistic_guard_malloc_allocator.c',
		'pas_ptr_worklist.c', 'pas_race_test_hooks.c', 'pas_random.c', 'pas_red_black_tree.c', 'pas_redundant_local_allocator_node.c',
		'pas_report_crash.c', 'pas_reserved_memory_provider.c', 'pas_root.c', 'pas_scavenger.c', 'pas_segregated_directory.c',
		'pas_segregated_exclusive_view.c', 'pas_segregated_heap.c', 'pas_segregated_page.c', 'pas_segregated_page_config.c',
		'pas_segregated_page_config_kind_and_role.c', 'pas_segregated_page_config_kind.c', 'pas_segregated_partial_view.c',
		'pas_segregated_shared_handle.c', 'pas_segregated_shared_page_directory.c', 'pas_segregated_shared_view.c',
		'pas_segregated_size_directory.c', 'pas_segregated_view.c', 'pas_shared_page_directory_by_size.c', 'pas_simple_free_heap_helpers.c',
		'pas_simple_large_free_heap.c', 'pas_simple_type.c', 'pas_status_reporter.c', 'pas_stream.c', 'pas_string_stream.c',
		'pas_thread_local_cache.c', 'pas_thread_local_cache_layout.c', 'pas_thread_local_cache_layout_node.c', 'pas_thread_local_cache_node.c',
		'pas_thread_suspend_lock.c', 'pas_utility_heap.c', 'pas_utility_heap_config.c', 'pas_utils.c', 'pas_versioned_field.c',
		'pas_virtual_range.c', 'thingy_heap.c', 'thingy_heap_config.c'
	]
	libpassources = [os.path.join('libpas', 'src', 'libpas', path) for path in libpassources]
	bmallocsources = [
		'AllIsoHeaps.cpp', 'Allocator.cpp', 'AvailableMemory.cpp', 'bmalloc.cpp', 'Cache.cpp', 'CryptoRandom.cpp',
		'Deallocator.cpp', 'DebugHeap.cpp', 'Environment.cpp', 'FreeList.cpp', 'Gigacage.cpp', 'Heap.cpp',
		'HeapKind.cpp', 'IsoHeapImpl.cpp', 'IsoPage.cpp', 'IsoSharedHeap.cpp', 'IsoSharedPage.cpp', 'IsoTLS.cpp',
		'IsoTLSEntry.cpp', 'IsoTLSLayout.cpp', 'LargeMap.cpp', 'Logging.cpp', 'mbmalloc.cpp', 'Mutex.cpp',
		'ObjectType.cpp', 'PerProcess.cpp', 'Scavenger.cpp'
	]
	if target.is_macos() or target.is_ios():
		bmallocsources += ['Zone.cpp']
	bmallocsources = [os.path.join('bmalloc', path) for path in bmallocsources]
	if not target.is_android():
		bmalloc_variables = merge_variables({'runtime': 'c++', 'defines': ['PAS_BMALLOC=1', 'NDEBUG=1']}, variables)
		bmalloc_lib = generator.lib(module = 'bmalloc', sources = bmallocsources + libpassources, basepath = 'benchmark', includepaths = includepaths + bmallocincludepaths, variables = bmalloc_variables)
		bmalloc_depend_libs = ['bmalloc', 'benchmark', 'test', 'atomic']
		generator.bin(module = 'bmalloc', sources = ['benchmark.cc'], binname = 'benchmark-bmalloc', basepath = 'benchmark', implicit_deps = [bmalloc_lib, benchmark_lib, test_lib], libs = bmalloc_depend_libs, includepaths = includepaths + bmallocincludepaths, variables = bmalloc_variables)

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
	'static.c'
]
mimallocsources = [os.path.join('src', path) for path in mimallocsources]
mimalloc_variables = {'defines': ['MI_DEBUG=0']}
mimallocincludepaths = [
	os.path.join('benchmark', 'mimalloc', 'include')
]
mimalloc_depend_libs = ['benchmark', 'test']
generator.bin(module = 'mimalloc', sources = ['benchmark.c'] + mimallocsources, binname = 'benchmark-mimalloc', basepath = 'benchmark', implicit_deps = [benchmark_lib, test_lib], libs = mimalloc_depend_libs, includepaths = includepaths + mimallocincludepaths, variables = mimalloc_variables)
