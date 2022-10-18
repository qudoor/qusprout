#定义最新cmake的编译版本
cmake_minimum_required(VERSION 3.7)

#指定项目的名称
project(QUSPROUT)

#添加子模块
add_subdirectory(work)
add_subdirectory(slave)
add_subdirectory(master)
add_subdirectory(unittest)
