cmake_minimum_required(VERSION 3.13)

project(cwinapi)

include(ExternalProject)

set(NO_SONAME ON)
set(CMAKE_PLATFORM_NO_VERSIONED_SONAME ON)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  AFTER
  lib/include
  include
  deps/modernlib/include
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_subdirectory(deps/modernlib)
install(TARGETS modernlib DESTINATION ${CMAKE_BINARY_DIR})
add_subdirectory(src)


add_subdirectory(test)
