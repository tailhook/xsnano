# just my current version, feel free to lower it
cmake_minimum_required(VERSION 2.8)
project(xsnano)
enable_testing()

find_package(Threads)

add_subdirectory(src)
add_subdirectory(tests)

install(FILES include/xs.h DESTINATION include/xs)
