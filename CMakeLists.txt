cmake_minimum_required (VERSION 2.8)

#project (data_struct)
project (DataStruct)

set(ASAN_CHECK OFF)
set(CMAKE_BUILD_TYPE "debug")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#add_executable(run_test main.cpp mathFunctions.cpp)

#查找当前目录下的所有源文件，并讲名称保存到DIR_STCS变量
aux_source_directory(. DIR_SRCS)

#添加 math 子目录
#add_subdirectory(math)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

message("ASAN_CHECK ${ASAN_CHECK}")
if (ASAN_CHECK MATCHES "ON")
  message("asan_check on")
  set(CMAKE_ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_ASAN_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ASAN_FLAGS}")
endif()

#指定生成目标
add_executable(run_test ${DIR_SRCS})

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ DIR_LIB_SRC)
add_library(mathFunctions ${DIR_LIB_SRC})

#添加链接库
#表明可执行文件run_test 需要链接一个名为 mathFunctions的链接库
target_link_libraries(run_test mathFunctions)
