cmake_minimum_required(VERSION 3.18.4)

project(HelloVK)

set(CFLAGS "-std=c++17 -O2")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CFLAGS}")

add_executable(${PROJECT_NAME}
  main.cpp
  VulkanInstance.cpp
  VulkanDevice.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux
  set(LDFLAGS "-lvulkan")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")
  target_link_libraries(${PROJECT_NAME} vulkan)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # MacOS
  target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/include/")
  find_library(VK_FRAMEWORK MoltenVK "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/macOS/")
  target_link_libraries(${PROJECT_NAME} PRIVATE ${VK_FRAMEWORK})

endif()
