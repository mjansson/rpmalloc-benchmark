#!/bin/sh

os="linux"

for name in rpmalloc jemalloc lockfree-malloc tcmalloc ptmalloc3 scalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 0 2 20000 50000 5000 16 1000
	$executable 2 0 0 2 20000 50000 5000 16 1000
	$executable 3 0 0 2 20000 50000 5000 16 1000
	$executable 4 0 0 2 20000 50000 5000 16 1000
	$executable 5 0 0 2 20000 50000 5000 16 1000
	$executable 6 0 0 2 20000 50000 5000 16 1000
	$executable 7 0 0 2 20000 50000 5000 16 1000
	$executable 8 0 0 2 20000 50000 5000 16 1000
	$executable 9 0 0 2 20000 50000 5000 16 1000
	$executable 10 0 0 2 20000 50000 5000 16 1000
done

for name in rpmalloc jemalloc lockfree-malloc tcmalloc ptmalloc3 scalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 1 2 20000 50000 5000 16 8000
	$executable 2 0 1 2 20000 50000 5000 16 8000
	$executable 3 0 1 2 20000 50000 5000 16 8000
	$executable 4 0 1 2 20000 50000 5000 16 8000
	$executable 5 0 1 2 20000 50000 5000 16 8000
	$executable 6 0 1 2 20000 50000 5000 16 8000
	$executable 7 0 1 2 20000 50000 5000 16 8000
	$executable 8 0 1 2 20000 50000 5000 16 8000
	$executable 9 0 1 2 20000 50000 5000 16 8000
	$executable 10 0 1 2 20000 50000 5000 16 8000
done

for name in rpmalloc jemalloc lockfree-malloc tcmalloc ptmalloc3 scalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 1 2 20000 50000 5000 16 16000
	$executable 2 0 1 2 20000 50000 5000 16 16000
	$executable 3 0 1 2 20000 50000 5000 16 16000
	$executable 4 0 1 2 20000 50000 5000 16 16000
	$executable 5 0 1 2 20000 50000 5000 16 16000
	$executable 6 0 1 2 20000 50000 5000 16 16000
	$executable 7 0 1 2 20000 50000 5000 16 16000
	$executable 8 0 1 2 20000 50000 5000 16 16000
	$executable 9 0 1 2 20000 50000 5000 16 16000
	$executable 10 0 1 2 20000 50000 5000 16 16000
done

for name in rpmalloc jemalloc lockfree-malloc tcmalloc ptmalloc3 scalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 2 2 20000 30000 3000 128 64000
	$executable 2 0 2 2 20000 30000 3000 128 64000
	$executable 3 0 2 2 20000 30000 3000 128 64000
	$executable 4 0 2 2 20000 30000 3000 128 64000
	$executable 5 0 2 2 20000 30000 3000 128 64000
	$executable 6 0 2 2 20000 30000 3000 128 64000
	$executable 7 0 2 2 20000 30000 3000 128 64000
	$executable 8 0 2 2 20000 30000 3000 128 64000
	$executable 9 0 2 2 20000 30000 3000 128 64000
	$executable 10 0 2 2 20000 30000 3000 128 64000
done

for name in rpmalloc jemalloc lockfree-malloc tcmalloc ptmalloc3 scalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 2 2 20000 20000 2000 512 160000
	$executable 2 0 2 2 20000 20000 2000 512 160000
	$executable 3 0 2 2 20000 20000 2000 512 160000
	$executable 4 0 2 2 20000 20000 2000 512 160000
	$executable 5 0 2 2 20000 20000 2000 512 160000
	$executable 6 0 2 2 20000 20000 2000 512 160000
	$executable 7 0 2 2 20000 20000 2000 512 160000
	$executable 8 0 2 2 20000 20000 2000 512 160000
	$executable 9 0 2 2 20000 20000 2000 512 160000
	$executable 10 0 2 2 20000 20000 2000 512 160000
done
