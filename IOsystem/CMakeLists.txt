add_library(IOsystem STATIC
    epoller.cpp
    IOsystem.cpp
)

target_include_directories(IOsystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})