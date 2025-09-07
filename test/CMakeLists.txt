include_directories(AFTER ${CMAKE_SOURCE_DIR}/test)
link_libraries(modernlib)
link_libraries(cwinapi)

add_executable(io_behavior_test
  io_behavior_test.c
)
target_compile_definitions(io_behavior_test
  PRIVATE UNICODE _UNICODE _WIN32_WINNT=_WIN32_WINNT_WINXP
)
add_test(NAME io_behavior_test
  COMMAND io_behavior_test)
