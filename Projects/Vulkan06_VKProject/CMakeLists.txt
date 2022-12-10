cmake_minimum_required(VERSION 3.18.4)

project(VKProject)

set(CFLAGS "-std=c++17 -O2 -Wno-nullability-completeness")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CFLAGS}")

# Define Third Party Directories
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/")
set(STB_DIR "${THIRD_PARTY_DIR}/STB/")
set(VMA_DIR "${THIRD_PARTY_DIR}/VulkanMemoryAllocator/")
set(SPV_DIR "${THIRD_PARTY_DIR}/SpirvReflect/")


# Create Assets Directory
set(ASSETS_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets/")
add_custom_command(
  COMMAND
    mkdir -p ${ASSETS_DIR}
  OUTPUT ${ASSETS_DIR}
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Creating ${ASSETS_DIR}"
)

# Copy Assets to the Build Directory
file(GLOB IMAGES ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.png)
foreach(source IN LISTS IMAGES)
  get_filename_component(FILENAME ${source} NAME)
  add_custom_command(
    COMMAND
      cp ${source} ${ASSETS_DIR}/${FILENAME}
    OUTPUT ${ASSETS_DIR}/${FILENAME}
    DEPENDS ${source} ${ASSETS_DIR}
    COMMENT "Copying ${FILENAME}"
  )
  list(APPEND DST_IMAGES ${ASSETS_DIR}/${FILENAME})
endforeach()
add_custom_target(images ALL DEPENDS ${DST_IMAGES})


# Define Shaders
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(GLOB SHADERS
  ${SHADER_SOURCE_DIR}/*.comp
  ${SHADER_SOURCE_DIR}/*.vert
  ${SHADER_SOURCE_DIR}/*.frag
)

# Create Shader Directory
add_custom_command(
  COMMAND
    mkdir -p ${SHADER_BINARY_DIR}
  OUTPUT ${SHADER_BINARY_DIR}
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Creating ${SHADER_BINARY_DIR}"
)


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux

  # Compile Shaders to SPIR-V
  find_program(glslc_executable NAMES glslangValidator)
  foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME_WLE)
    add_custom_command(
      COMMAND
        ${glslc_executable} -V -s -o ${SHADER_BINARY_DIR}/${FILENAME}.spv ${source}
      OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
      DEPENDS ${source} ${SHADER_BINARY_DIR}
      COMMENT "Compiling ${FILENAME}.spv"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
  endforeach()
  add_custom_target(Shaders ALL DEPENDS ${SPV_SHADERS})

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # MacOS

  # Compile Shaders to SPIR-V
  set(converter_executable ${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/MoltenVK/tools/MoltenVKShaderConverter)
  foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME_WLE)
    add_custom_command(
      COMMAND
        ${converter_executable} -gi ${source} -so ${SHADER_BINARY_DIR}/${FILENAME}.spv -q
      OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
      DEPENDS ${source} ${SHADER_BINARY_DIR}
      COMMENT "Compiling ${FILENAME}.spv"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
  endforeach()
  add_custom_target(Shaders ALL DEPENDS ${SPV_SHADERS})

endif()


add_executable(ResizeSDL
  src/main.cpp
  ${SPV_DIR}/spirv_reflect.c
)
add_subdirectory(src)
