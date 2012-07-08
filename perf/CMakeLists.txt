include_directories(${xsnano_SOURCE_DIR}/include ${xsnano_SOURCE_DIR}/src)
link_directories(${xsnano_BINARY_DIR}/src)

macro(add_perf_test NAME)
    add_executable(${NAME} ${NAME}.c)
    target_link_libraries(${NAME} xs rt)
endmacro(add_perf_test)

add_perf_test(inproc_latency)
add_perf_test(inproc_throughput)
