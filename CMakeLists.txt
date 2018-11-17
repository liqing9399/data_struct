cmake_minimum_required (VERSION 2.8)

#project (data_struct)
project (Demo1)

#add_executable(run_test main.cpp mathFunctions.cpp)
aux_source_directory(. DIR_SRCS)

add_executable(run_test ${DIR_SRCS})
