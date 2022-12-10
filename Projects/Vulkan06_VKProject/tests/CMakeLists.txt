include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(graphics_test
  graphics_test.cpp
  ${SPV_DIR}/spirv_reflect.c
)


if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  # MacOS
  target_link_libraries(graphics_test
   PRIVATE
    GTest::GTest
    graphics
    ${VK_FRAMEWORK}
    ${SDL2_FRAMEWORK}
  )

endif()

add_test(graphics_gtests graphics_test)
