cmake_minimum_required(VERSION 3.18.4)

project(HelloVK)

set(CFLAGS "-std=c++17 -O2")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CFLAGS}")

#set(LDFLAGS "-lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr")
#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

add_executable(${PROJECT_NAME}
  main.cpp
)
