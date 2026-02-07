cmake_minimum_required(VERSION 3.20)
project(visible_systems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VS_ENABLE_UB_DEMOS "Build undefined behavior demos" OFF)

add_subdirectory(labs/memory)
