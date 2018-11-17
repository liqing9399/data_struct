cmake_minimum_required (VERSION 2.8)

#project (data_struct)
project (Demo1)

#add_executable(run_test main.cpp mathFunctions.cpp)

#查找当前目录下的所有源文件，并讲名称保存到DIR_STCS变量
aux_source_directory(. DIR_SRCS)

#添加 math 子目录
add_subdirectory(math)

#指定生成目标
add_executable(run_test ${DIR_SRCS})

#添加链接库
#表明可执行文件run_test 需要链接一个名为 mathFunctions的链接库
target_link_libraries(run_test mathFunctions)
