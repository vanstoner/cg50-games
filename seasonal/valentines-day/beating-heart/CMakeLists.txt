cmake_minimum_required(VERSION 3.15)
project(BeatingHeart C)

include(GenerateG3A)
find_package(Gint 2.9 REQUIRED)

add_executable(heart src/main.c)
target_compile_options(heart PRIVATE -Wall -Wextra -Os)
target_link_libraries(heart Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(
    TARGET heart
    OUTPUT "Heart.g3a"
    NAME "Heart"
  )
endif()
