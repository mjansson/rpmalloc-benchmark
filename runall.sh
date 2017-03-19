#!/bin/sh

os="linux"

for name in rpmalloc jemalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 0 2 500000 50000 300 16 1000
	$executable 2 0 0 2 500000 50000 300 16 1000
	$executable 3 0 0 2 500000 50000 300 16 1000
	$executable 4 0 0 2 500000 50000 300 16 1000
	$executable 5 0 0 2 500000 50000 300 16 1000
	$executable 6 0 0 2 500000 50000 300 16 1000
	$executable 7 0 0 2 500000 50000 300 16 1000
	$executable 8 0 0 2 500000 50000 300 16 1000
	$executable 9 0 0 2 500000 50000 300 16 1000
	$executable 10 0 0 2 500000 50000 300 16 1000
done

for name in rpmalloc jemalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 1 2 600000 15000 100 16 8000
	$executable 2 0 1 2 600000 15000 100 16 8000
	$executable 3 0 1 2 600000 15000 100 16 8000
	$executable 4 0 1 2 600000 15000 100 16 8000
	$executable 5 0 1 2 600000 15000 100 16 8000
	$executable 6 0 1 2 600000 15000 100 16 8000
	$executable 7 0 1 2 600000 15000 100 16 8000
	$executable 8 0 1 2 600000 15000 100 16 8000
	$executable 9 0 1 2 600000 15000 100 16 8000
	$executable 10 0 1 2 600000 15000 100 16 8000
done

for name in rpmalloc jemalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 1 2 500000 12000 80 16 16000
	$executable 2 0 1 2 500000 12000 80 16 16000
	$executable 3 0 1 2 500000 12000 80 16 16000
	$executable 4 0 1 2 500000 12000 80 16 16000
	$executable 5 0 1 2 500000 12000 80 16 16000
	$executable 6 0 1 2 500000 12000 80 16 16000
	$executable 7 0 1 2 500000 12000 80 16 16000
	$executable 8 0 1 2 500000 12000 80 16 16000
	$executable 9 0 1 2 500000 12000 80 16 16000
	$executable 10 0 1 2 500000 12000 80 16 16000
done

for name in rpmalloc jemalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 2 2 400000 10000 60 128 64000
	$executable 2 0 2 2 400000 10000 60 128 64000
	$executable 3 0 2 2 400000 10000 60 128 64000
	$executable 4 0 2 2 400000 10000 60 128 64000
	$executable 5 0 2 2 400000 10000 60 128 64000
	$executable 6 0 2 2 400000 10000 60 128 64000
	$executable 7 0 2 2 400000 10000 60 128 64000
	$executable 8 0 2 2 400000 10000 60 128 64000
	$executable 9 0 2 2 400000 10000 60 128 64000
	$executable 10 0 2 2 400000 10000 60 128 64000
done

for name in rpmalloc jemalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 2 2 300000 8000 40 512 160000
	$executable 2 0 2 2 300000 8000 40 512 160000
	$executable 3 0 2 2 300000 8000 40 512 160000
	$executable 4 0 2 2 300000 8000 40 512 160000
	$executable 5 0 2 2 300000 8000 40 512 160000
	$executable 6 0 2 2 300000 8000 40 512 160000
	$executable 7 0 2 2 300000 8000 40 512 160000
	$executable 8 0 2 2 300000 8000 40 512 160000
	$executable 9 0 2 2 300000 8000 40 512 160000
	$executable 10 0 2 2 300000 8000 40 512 160000
done
