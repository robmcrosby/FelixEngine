cmake_minimum_required(VERSION 3.18.4)

project(VKDrawImage)

set(CFLAGS "-std=c++17 -O2 -Wno-nullability-completeness")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CFLAGS}")

add_executable(${PROJECT_NAME}
  main.cpp
  VulkanInstance.cpp
  VulkanDevice.cpp
  VulkanBuffer.cpp
  VulkanImage.cpp
  VulkanSetLayout.cpp
  VulkanShader.cpp
  VulkanPipeline.cpp
  VulkanQueue.cpp
  VulkanCommand.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/STB/"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/VulkanMemoryAllocator/"
)

# Copy Images to Build Directory
file(GLOB IMAGES ${CMAKE_CURRENT_SOURCE_DIR}/*.png)
foreach(source IN LISTS IMAGES)
  get_filename_component(FILENAME ${source} NAME)
  add_custom_command(
    COMMAND
      cp ${source} ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}
    DEPENDS ${source} ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying ${FILENAME}"
  )
  list(APPEND DST_IMAGES ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME})
endforeach()
add_custom_target(images ALL DEPENDS ${DST_IMAGES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux
  set(LDFLAGS "-lvulkan")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")
  target_link_libraries(${PROJECT_NAME} vulkan)

  # Compile Shaders to SPIR-V
  file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.comp)
  find_program(glslc_executable NAMES glslangValidator)
  foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME_WLE)
    add_custom_command(
      COMMAND
        ${glslc_executable} -V -s -o ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv ${source}
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
      DEPENDS ${source} ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Compiling ${FILENAME}.spv"
    )
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv)
  endforeach()
  add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # MacOS
  target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/include/")
  find_library(VK_FRAMEWORK MoltenVK "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/macOS/")
  target_link_libraries(${PROJECT_NAME} PRIVATE ${VK_FRAMEWORK})

  # Compile Shaders to SPIR-V
  file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.comp)
  set(converter_executable ${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/tools/MoltenVKShaderConverter)
  foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME_WLE)
    add_custom_command(
      COMMAND
        ${converter_executable} -gi ${source} -so ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv -q
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
      DEPENDS ${source} ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Compiling ${FILENAME}.spv"
    )
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv)
  endforeach()
  add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})

endif()
