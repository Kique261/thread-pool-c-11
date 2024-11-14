add_library(mysql_connect_pool STATIC
    conn_pool.cpp
)

target_include_directories(mysql_connect_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})