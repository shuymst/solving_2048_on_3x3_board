add_executable(enumeration enumerate.cc)
target_link_libraries(enumeration PRIVATE mini2048)

add_executable(calculation calculate.cc)
target_link_libraries(calculation PRIVATE mini2048)