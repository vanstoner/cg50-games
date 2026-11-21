cmake_minimum_required(VERSION 3.15)
project(Tetris C)

include(GenerateG3A)
find_package(Gint 2.9 REQUIRED)

add_executable(tetris src/main.c)
target_compile_options(tetris PRIVATE -Wall -Wextra -Os)
target_link_libraries(tetris Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(
    TARGET tetris
    OUTPUT "Tetris.g3a"
    NAME "Tetris"
  )
endif()
