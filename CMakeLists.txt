#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名字
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

file(GLOB copyResources "./assets" "./thirdParty/bin/assimp/assimp-vc143-mtd.dll")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include
)


link_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib/assimp
)

add_subdirectory(application)
add_subdirectory(gpu)

#本工程所有cpp文件编译链接，生成exe
add_executable(softRenderer "main.cpp")

target_link_libraries(softRenderer applicationLib gpuLib assimp-vc143-mtd.lib)

