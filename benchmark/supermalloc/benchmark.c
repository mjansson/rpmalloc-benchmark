
#include <benchmark.h>

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
	//TODO: supermalloc seems to segfault if using memalign, investigate but ignore for now
	(void)sizeof(alignment);
	return malloc(size);//alignment ? memalign(alignment, size) : malloc(size);
}

void
benchmark_free(void* ptr) {
	free(ptr);
}

const char*
benchmark_name(void) {
	return "supermalloc";
}

void
benchmark_thread_collect(void) {
}
