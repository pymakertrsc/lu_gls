#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(LuGls)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

#搜索所有的cpp，加入SRCS变量中
aux_source_directory(. SRCS)

#本工程所有cpp文件编译链接，生成exe
add_executable(glStudy ${SRCS})