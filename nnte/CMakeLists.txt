# Enforce some CMake policies
cmake_minimum_required(VERSION 3.14)


project(nnte)



set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# 生成头文件结构json文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 声明头文件所在位置
include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/include/*)

# 声明 源文件 所在位置
aux_source_directory(${CMAKE_SOURCE_DIR}/src/ DIR_LIB_SRCS)

# 使用源文件生成 动态库
add_library(nnte SHARED ${DIR_LIB_SRCS})

set(ASAN_CHECK OFF)
set(CMAKE_BUILD_TYPE "debug")

message("ASAN_CHECK ${ASAN_CHECK}")
if (ASAN_CHECK MATCHES "ON")
  message("asan_check on")
  set(CMAKE_ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_ASAN_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ASAN_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fPIC ")

# 此两行不能与set cmake_build_type 搭配使用。
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS} -g -o0")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS} -o3")

message("cmake__cxx_flags: ${CMAKE_CXX_FLAGS}\n")
message("cmake build type: ${CMAKE_BUILD_TYPE}\n")

#SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)       #设置可执行文件的输出目录
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/lib)    #设置库文件的输出目录

# cmake -DCMAKE_INSTALL_PREFIX=/usr .. 运行时指定install位置


include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG release-1.11.0)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

set(UNIT_TESTS "Build the unit tests" OFF)
if(UNIT_TESTS)
  aux_source_directory(${CMAKE_SOURCE_DIR}/tests/* UNIT_TESTS)
  add_executable(op_test ${CMAKE_SOURCE_DIR}/tests/test_embedding.cpp)
  #target_sources(op_test
  #	PRIVATE
  #  tests/test.cpp
  #)

  target_link_libraries(op_test nnte gtest_main)

  enable_testing()
  add_test(
    	NAME google_test
    	COMMAND $<TARGET_FILE:op_test>
   )
endif()


add_executable(evalt main.cpp)

# target_link_libraies 需要再 add_executable 之后
target_link_libraries(evalt nnte)

#add_executable(evalt main.cpp, nnte)

