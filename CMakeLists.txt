#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(LuGls LANGUAGES CXX)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

#设置头文件
set(HEADERS

 )

set(SOURCES
	"src/Main.cpp"
	)


set(TARGET LuGls)

#本工程所有cpp文件编译链接，生成exe
add_executable(${TARGET} ${HEADERS} ${SOURCES})