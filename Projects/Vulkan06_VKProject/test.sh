#! /bin/sh

cmake -S . -B build
cd build
make
./ResizeSDL
