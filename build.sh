#!/usr/bin/env bash

set -e

CXX=g++

SANITIZER=""
# SANITIZER="-fsanitize=address"

CXXFLAGS="-Wall -Wpedantic -Werror -g -Og -std=c++20 $SANITIZER"
LDFLAGS="$SANITIZER -lraylib"

mkdir -p bin

TARGET=./hex

RUN=true
CLEAN=false

for arg in $@; do
	case $arg in
	run) RUN=true;;
	clean) CLEAN=true;;
	esac
done

if $CLEAN; then
	rm -fr bin/*.o
fi

objects=""
PIDS=""
for file in src/*.cpp; do
	(
		out=${file/src/bin}.o

		if test $out -nt $file; then
			echo "skipping $file"
			exit 0
		fi

		echo $CXX -c $CXXFLAGS $file -o $out
		if ! $CXX -c $CXXFLAGS $file -o $out; then
			echo "failed to compile $file"
			exit 1
		fi
	) &
	PIDS="$PIDS $!"
done

for pid in $PIDS; do
	if ! wait $pid; then
		echo "exiting"
		exit 1
	fi
done

echo $CXX $LDFLAGS bin/*.o -o $TARGET
$CXX $LDFLAGS bin/*.o -o $TARGET

if $RUN; then
	./$TARGET
fi

