#!/bin/sh

os="linux"

for name in rpmalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 0 2 800000 20000 100 16 1000
	$executable 2 0 0 2 800000 20000 100 16 1000
	$executable 3 0 0 2 800000 20000 100 16 1000
	$executable 4 0 0 2 800000 20000 100 16 1000
	$executable 5 0 0 2 800000 20000 100 16 1000
	$executable 6 0 0 2 800000 20000 100 16 1000
	$executable 7 0 0 2 800000 20000 100 16 1000
	$executable 8 0 0 2 800000 20000 100 16 1000
	$executable 9 0 0 2 800000 20000 100 16 1000
	$executable 10 0 0 2 800000 20000 100 16 1000
done

for name in rpmalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 1 2 600000 15000 100 16 8000 4
	$executable 2 0 1 2 600000 15000 100 16 8000 4
	$executable 3 0 1 2 600000 15000 100 16 8000 4
	$executable 4 0 1 2 600000 15000 100 16 8000 4
	$executable 5 0 1 2 600000 15000 100 16 8000 4
	$executable 6 0 1 2 600000 15000 100 16 8000 4
	$executable 7 0 1 2 600000 15000 100 16 8000 4
	$executable 8 0 1 2 600000 15000 100 16 8000 4
	$executable 9 0 1 2 600000 15000 100 16 8000 4
	$executable 10 0 1 2 600000 15000 100 16 8000 4
done

for name in rpmalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 1 2 500000 12000 80 16 16000 3
	$executable 2 0 1 2 500000 12000 80 16 16000 3
	$executable 3 0 1 2 500000 12000 80 16 16000 3
	$executable 4 0 1 2 500000 12000 80 16 16000 3
	$executable 5 0 1 2 500000 12000 80 16 16000 3
	$executable 6 0 1 2 500000 12000 80 16 16000 3
	$executable 7 0 1 2 500000 12000 80 16 16000 3
	$executable 8 0 1 2 500000 12000 80 16 16000 3
	$executable 9 0 1 2 500000 12000 80 16 16000 3
	$executable 10 0 1 2 500000 12000 80 16 16000 3
done

for name in rpmalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 2 2 400000 10000 60 128 64000 2
	$executable 2 0 2 2 400000 10000 60 128 64000 2
	$executable 3 0 2 2 400000 10000 60 128 64000 2
	$executable 4 0 2 2 400000 10000 60 128 64000 2
	$executable 5 0 2 2 400000 10000 60 128 64000 2
	$executable 6 0 2 2 400000 10000 60 128 64000 2
	$executable 7 0 2 2 400000 10000 60 128 64000 2
	$executable 8 0 2 2 400000 10000 60 128 64000 2
	$executable 9 0 2 2 400000 10000 60 128 64000 2
	$executable 10 0 2 2 400000 10000 60 128 64000 2
done

for name in rpmalloc tcmalloc crt nedmalloc; do
	executable=bin/$os/release/x86-64/benchmark-$name

	$executable 1 0 2 2 300000 8000 40 512 160000 2
	$executable 2 0 2 2 300000 8000 40 512 160000 2
	$executable 3 0 2 2 300000 8000 40 512 160000 2
	$executable 4 0 2 2 300000 8000 40 512 160000 2
	$executable 5 0 2 2 300000 8000 40 512 160000 2
	$executable 6 0 2 2 300000 8000 40 512 160000 2
	$executable 7 0 2 2 300000 8000 40 512 160000 2
	$executable 8 0 2 2 300000 8000 40 512 160000 2
	$executable 9 0 2 2 300000 8000 40 512 160000 2
	$executable 10 0 2 2 300000 8000 40 512 160000 2
done
