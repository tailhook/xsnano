include_directories(${xsnano_SOURCE_DIR}/include ${xsnano_SOURCE_DIR}/src)
link_directories(${xsnano_BINARY_DIR}/src)

macro(add_libxs_test NAME)
    add_executable(${NAME} ${NAME}.c)
    target_link_libraries(${NAME} xs)
    add_test(${NAME} ${NAME})
endmacro(add_libxs_test)

add_libxs_test(basic)
add_libxs_test(pipe)
