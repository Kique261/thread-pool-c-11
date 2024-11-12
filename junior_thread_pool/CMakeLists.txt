# 设置库的名称和源文件
add_library(junior_thread_pool STATIC
    thread_pool.cpp
)

# 添加头文件目录
target_include_directories(junior_thread_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})