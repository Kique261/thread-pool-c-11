cmake_minimum_required(VERSION 3.10)

project(ThreadPoolExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 启用调试符号
set(CMAKE_BUILD_TYPE Debug)

include_directories(${PROJECT_SOURCE_DIR})

# 添加 libc6-dev 的头文件目录
include_directories(/usr/include)

# 添加 libc6-dev 的库文件目录
link_directories(/usr/lib)

add_subdirectory(thread_pool)
add_subdirectory(timer)
add_subdirectory(mysql_connect_pool)
add_subdirectory(IOsystem)
add_subdirectory(log)

# 使用 pkg-config 查找 MySQL 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient)

include_directories(${MYSQL_INCLUDE_DIRS})
link_directories(${MYSQL_LIBRARY_DIRS})

add_executable(ThreadPoolExample init.cpp)

target_link_libraries(ThreadPoolExample 
                      thread_pool timer
                      mysql_connect_pool
                      IOsystem
                      log
                      ${MYSQL_LIBRARIES})
