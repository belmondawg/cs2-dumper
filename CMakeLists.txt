cmake_minimum_required(VERSION 3.16)

project(dumper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_VERBOSE_MAKEFILE ON)


add_executable(dumper 
    main.cpp
    src/schema/schema.cpp
)

target_link_libraries(dumper PRIVATE 
    winmm
    ws2_32
    crypt32 
    secur32 
    user32
)
