cmake_minimum_required(VERSION 3.12)

project(yuv_modder
        LANGUAGES C CXX)
    
set(CMAKE_CXX_STANDARD 11)

# file(GLOB_RECURSE SOURCES
#     "src/*.cpp"
#     "*.cpp"
# )

set(SOURCES
    src/main.cpp
    src/bmp_reader.cpp
    src/YUVWorker.cpp
)


file(GLOB_RECURSE HEADERS
    "headers/*.hpp"
    "headers/*.h"
)

include_directories(headers)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})