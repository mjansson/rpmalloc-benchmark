
#include <benchmark.h>

#include "smmalloc.h"

static sm_allocator allocator;

extern "C" {

int
benchmark_initialize() {
	allocator = _sm_allocator_create(4, (16 * 1024 * 1024));
	return 0;
}

int
benchmark_finalize(void) {
	_sm_allocator_destroy(allocator);
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
	return _sm_malloc(allocator, size, 0);//alignment);
}

extern void
benchmark_free(void* ptr) {
	_sm_free(allocator, ptr);
}

const char*
benchmark_name(void) {
	return "smmalloc";
}

}
