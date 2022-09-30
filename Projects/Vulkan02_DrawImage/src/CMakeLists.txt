cmake_minimum_required(VERSION 3.18.4)

project(VKDrawImage)

set(CFLAGS "-std=c++17 -O2 -Wno-nullability-completeness")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CFLAGS}")

add_executable(${PROJECT_NAME}
  main.cpp
)
