cmake_minimum_required(VERSION 3.15)
project(Diplom)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(libs)

add_executable(Diplom main.cpp PATree1.h)
target_link_libraries(Diplom sptl)
target_link_libraries(Diplom cmdline)
target_link_libraries(Diplom chunkedseq)
target_link_libraries(Diplom pbbs-pctl)
target_link_libraries(Diplom pctl)
