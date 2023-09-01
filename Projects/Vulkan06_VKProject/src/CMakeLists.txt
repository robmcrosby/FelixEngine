add_subdirectory(graphics)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux
  set(LDFLAGS "-lvulkan")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS}")

  find_package(SDL2 REQUIRED)
  target_include_directories(graphics
   PUBLIC
    ${SDL2_INCLUDE_DIRS}
  )

  target_include_directories(ResizeSDL
   PRIVATE
    graphics
    ${SDL2_INCLUDE_DIRS}
  )
  target_link_libraries(ResizeSDL
   PRIVATE
    graphics
    vulkan
    ${SDL2_LIBRARIES}
  )

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # MacOS
  find_library(VK_FRAMEWORK MoltenVK "${THIRD_PARTY_DIR}/MoltenVK/macOS/")
  find_library(SDL2_FRAMEWORK SDL2 "${THIRD_PARTY_DIR}/SDL2/macOS/")

  set(VK_INCLUDES "${THIRD_PARTY_DIR}/MoltenVK/include/")
  set(SDL2_INCLUDES "${SDL2_FRAMEWORK}/Headers/")

  target_include_directories(graphics
   PUBLIC
    ${VK_INCLUDES}
    ${SDL2_INCLUDES}
  )

  target_include_directories(ResizeSDL
   PRIVATE
    graphics
    ${glm_SOURCE_DIR}
  )
  target_link_libraries(ResizeSDL
   PRIVATE
    graphics
    ${VK_FRAMEWORK}
    ${SDL2_FRAMEWORK}
  )
endif()
