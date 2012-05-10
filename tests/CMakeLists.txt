include_directories(${XSNANO_SOURCE_DIR}/include)
link_directories(${XSNANO_BINARY_DIR}/src)

add_executable(basic basic.c)
target_link_libraries(basic xs)
add_test(basic basic)
