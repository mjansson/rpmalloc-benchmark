
#include <benchmark.h>

extern void* __wrap_malloc(size_t size, void const *);
extern void* __wrap_memalign(size_t align, size_t size);
extern void __wrap_free(void *p, void const *);

int
benchmark_initialize() {
	return 0;
}

int
benchmark_finalize(void) {
	return 0;
}

int
benchmark_thread_initialize(void) {
	return 0;
}

int
benchmark_thread_finalize(void) {
	return 0;
}

void
benchmark_thread_collect(void) {
}

void*
benchmark_malloc(size_t alignment, size_t size) {
	//return alignment ? __wrap_memalign(alignment, size) : __wrap_malloc(size, 0);
	return __wrap_malloc(size, 0);
}

extern void
benchmark_free(void* ptr) {
	__wrap_free(ptr, 0);
}

const char*
benchmark_name(void) {
	return "lockfree-malloc";
}
