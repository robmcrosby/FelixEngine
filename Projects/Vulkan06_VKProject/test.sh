#! /bin/sh

cmake -S . -B build -DBUILD_TESTING=ON
cd build
make
./tests/graphics_test
