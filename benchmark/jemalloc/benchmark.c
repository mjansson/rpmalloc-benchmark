
#include <benchmark.h>

extern void*
je_malloc(size_t size);

extern void*
je_memalign(size_t alignment, size_t size);

extern void
je_free(void* ptr);

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

void*
benchmark_malloc(size_t alignment, size_t size) {
	//return alignment ? je_memalign(alignment, size) : je_malloc(size);
	return je_malloc(size);
}

void
benchmark_free(void* ptr) {
	je_free(ptr);
}

const char*
benchmark_name(void) {
	return "jemalloc";
}

void
benchmark_thread_collect(void) {
}
