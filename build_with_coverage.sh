#!/bin/sh
set -e

BUILD_SYSTEM='Unix Makefiles'

# if command -v ninja; then 
# 	BUILD_SYSTEM='Ninja'
# fi

cmake -DCMAKE_BUILD_TYPE=Coverage -S src -B build_coverage -G "$BUILD_SYSTEM" && \
		cmake --build build_coverage -j $(nproc --ignore 1) -t coverage && open ./build_coverage/coverage/index.html

