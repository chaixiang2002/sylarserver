cmake_minimum_required(VERSION 3.10)
project(YourProjectName)

set(CMAKE_CXX_STANDARD 20)

include_directories(config) # 添加头文件目录
include_directories(src/include) # 添加头文件目录

file(GLOB LIB_SOURCES "src/*.cpp")  # 定义 文件变量 LIB_SOURCES
add_library(squeue ${LIB_SOURCES})  # 变成一个库

file(GLOB TEST_SOURCES "Test/*.cpp") # 定义 文件变量 TEST_SOURCES
foreach(test_source ${TEST_SOURCES}) #循环
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source}) # 制作成 可执行文件
    target_link_libraries(${test_name} squeue) # 链接  squeue库
endforeach()