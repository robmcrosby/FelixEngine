#! /bin/sh

find build -type 'f' | grep -v "build/_deps/*" | xargs rm
find build -maxdepth 1 -type 'd' | grep "build/" | grep -v "build/_deps" | xargs rm -rf
