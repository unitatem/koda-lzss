cmake_minimum_required(VERSION 3.8)
project(koda_lzss)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

set(SOURCE_FILES
        src/codec.cpp
        src/Dictionary.cpp
        src/Dictionary.hpp
        src/Logger.cpp
        src/Logger.hpp
        src/LZSS.cpp
        src/LZSS.hpp
        src/MediaOperations.cpp
        src/MediaOperations.hpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
