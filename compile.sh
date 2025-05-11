#!/usr/bin/bash
set -e

rm -rf build
mkdir build
cmake -B build -S .

cmake --build build