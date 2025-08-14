cmake_minimum_required(VERSION 3.10)
project(chat)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 1. 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/muduo)
include_directories(${CMAKE_SOURCE_DIR}/include/client)
include_directories(${CMAKE_SOURCE_DIR}/include/server)
include_directories(${CMAKE_SOURCE_DIR}/include/server/model)
include_directories(${CMAKE_SOURCE_DIR}/include/db)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty)

# 2. 添加库文件搜索路径
link_directories(${CMAKE_SOURCE_DIR}/lib)
link_directories(${CMAKE_SOURCE_DIR}/lib/muduo)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 移除警告
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

add_subdirectory(src)