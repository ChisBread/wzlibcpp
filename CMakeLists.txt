cmake_minimum_required(VERSION 3.17)
project(wzlibcpp)

set(CMAKE_CXX_STANDARD 17)
set(VCPKG_CMAKE_PATH "F:/vcpkg/scripts/buildsystems/vcpkg.cmake")

if (WIN32)
    include(${VCPKG_CMAKE_PATH})
endif (WIN32)

set (CMAKE_CXX_FLAGS   "${CMAKE_CXX_FLAGS} -fpermissive")

find_package(ZLIB REQUIRED)

include_directories(
        .
        include
        include/wz
        mio/include
        ${ZLIB_INCLUDE_DIR})

file(GLOB SOURCE_FILES src/*.cpp)
file(GLOB AES_SOURCE_FILES AES/AES.cpp)

add_library(wzlibcpp ${SOURCE_FILES} ${AES_SOURCE_FILES})
target_link_libraries(ZLIB::ZLIB)

add_executable(crackimg crackimg/main.cc)

target_link_libraries(crackimg wzlibcpp)
