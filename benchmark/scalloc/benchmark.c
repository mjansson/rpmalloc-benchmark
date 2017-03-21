
#include <benchmark.h>

#include <stdlib.h>

extern void
_scalloc_benchmark_init(void);

extern void
_scalloc_thread_init(void);

extern void*
scalloc_malloc(size_t size);

extern void
scalloc_free(void* p);

extern void*
scalloc_memalign(size_t alignment, size_t size);

int
benchmark_initialize() {
	_scalloc_benchmark_init();
	return 0;
}

int
benchmark_finalize(void) {
	return 0;
}

int
benchmark_thread_initialize(void) {
	_scalloc_thread_init();
	return 0;
}

int
benchmark_thread_finalize(void) {
	return 0;
}

void*
benchmark_malloc(size_t alignment, size_t size) {
#ifdef __APPLE__
	if (alignment) {
		void* ptr = 0;
		posix_memalign(&ptr, alignment, size);
		return ptr;
	}
	return malloc(size);
#else
	return alignment ? memalign(alignment, size) : malloc(size);
#endif
}

void
benchmark_free(void* ptr) {
	free(ptr);
}

const char*
benchmark_name(void) {
	return "scalloc";
}

void
benchmark_thread_collect(void) {
}
