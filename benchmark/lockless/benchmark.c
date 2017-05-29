
#include <benchmark.h>

extern void* llallocmalloc(size_t size, void const *);
extern void* llallocaligned_alloc(size_t align, size_t size);
extern void llallocfree(void *p, void const *);

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
	return alignment ? llallocaligned_alloc(alignment, size) : llallocmalloc(size, 0);
}

extern void
benchmark_free(void* ptr) {
	llallocfree(ptr, 0);
}

const char*
benchmark_name(void) {
	return "lockless";
}
