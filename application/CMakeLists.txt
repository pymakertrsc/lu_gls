#递归将本文件夹下所有cpp放到FUNCS中
file(GLOB_RECURSE APP ./  *.cpp)

#将FUNCS中所有cpp编译为funcs这个lib库
add_library(applicationLib ${APP} )

