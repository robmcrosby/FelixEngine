cmake_minimum_required(VERSION 3.18.4)

project(ResizeSDL)

set(CFLAGS "-std=c++17 -O2 -Wno-nullability-completeness")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${CFLAGS}")

set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/")
set(STB_DIR "${THIRD_PARTY_DIR}/STB/")
set(VMA_DIR "${THIRD_PARTY_DIR}/VulkanMemoryAllocator/")
set(SPV_DIR "${THIRD_PARTY_DIR}/SpirvReflect/")

add_executable(${PROJECT_NAME}
  main.cpp
  ${SPV_DIR}/spirv_reflect.c
)

add_subdirectory(graphics)

target_link_directories(${PROJECT_NAME}
  PRIVATE graphics
)

target_include_directories(graphics
  PUBLIC ${STB_DIR}
  PUBLIC ${VMA_DIR}
  PUBLIC ${SPV_DIR}
)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${STB_DIR}
  PUBLIC ${VMA_DIR}
  PUBLIC ${SPV_DIR}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/graphics/"
)

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
  set(LDFLAGS "-lvulkan")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

  find_package(SDL2 REQUIRED)
  target_include_directories(graphics PRIVATE ${SDL2_INCLUDE_DIRS})
  target_include_directories(${PROJECT_NAME} PRIVATE graphics ${SDL2_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} graphics vulkan ${SDL2_LIBRARIES})

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
  find_library(VK_FRAMEWORK MoltenVK "${THIRD_PARTY_DIR}/MoltenVK/macOS/")
  find_library(SDL2_FRAMEWORK SDL2 "${THIRD_PARTY_DIR}/SDL2/macOS/")

  set(VK_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/include/")
  set(SDL2_INCLUDES "${SDL2_FRAMEWORK}/Headers/")

  target_include_directories(graphics PRIVATE ${VK_INCLUDES} ${SDL2_INCLUDES})
  target_include_directories(${PROJECT_NAME} PRIVATE graphics ${VK_INCLUDES} ${SDL2_INCLUDES})
  target_link_libraries(${PROJECT_NAME} PRIVATE graphics ${VK_FRAMEWORK} ${SDL2_FRAMEWORK})

  # Compile Shaders to SPIR-V
  set(converter_executable ${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParty/MoltenVK/tools/MoltenVKShaderConverter)
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
