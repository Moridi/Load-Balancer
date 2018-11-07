cmake_minimum_required(VERSION 2.8)

#set (CMAKE_CXX_COMPILER "/usr/bin/g++")
project(Presenter)

set (CMAKE_CXX_STANDARD 11)

include_directories(
    "include")

file (GLOB SOURCES
    "src/*.cpp"
    "include/*.h")

add_executable(${PROJECT_NAME}
    ${SOURCES})
