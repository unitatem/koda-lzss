cmake_minimum_required(VERSION 3.8)
project(koda_lzss)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

set(SOURCE_FILES
        src/codec.cpp
        src/Dictionary.cpp
        src/Logger.cpp
        src/LZSS.cpp
        src/MediaOperations.cpp)

set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} -pthread)
