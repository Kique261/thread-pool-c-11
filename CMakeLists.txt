cmake_minimum_required(VERSION 3.10)

project(ThreadPoolExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(thread_pool)
add_subdirectory(timer)
add_subdirectory(mysql_connect_pool)
add_executable(ThreadPoolExample test.cpp)

target_link_libraries(ThreadPoolExample thread_pool timer mysql_connect_pool)
