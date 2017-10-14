cmake_minimum_required(VERSION 3.8)
project(koda_lzss)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/codec.cpp
        src/Dictionary.cpp
        src/Dictionary.hpp
        src/Logger.cpp
        src/Logger.hpp
        src/LZSS.cpp
        src/LZSS.hpp)

add_executable(koda_lzss ${SOURCE_FILES})