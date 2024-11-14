
add_library(timer STATIC
    timer_container.cpp
)

target_include_directories(timer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})