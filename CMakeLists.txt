cmake_minimum_required(VERSION 3.10)

project(ThreadPoolExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(junior_thread_pool)

add_executable(Example async_sort.cc)

target_link_libraries(Example junior_thread_pool)