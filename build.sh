#!/bin/sh
set -e

BUILD_SYSTEM='Unix Makefiles'

if command -v ninja; then 
	BUILD_SYSTEM='Ninja'
fi

cmake -S src -B build -G "$BUILD_SYSTEM" && cmake --build build -j $(nproc --ignore 1) -t "test" && ./build/"test" $1
