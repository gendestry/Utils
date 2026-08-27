cmake_minimum_required(VERSION 3.10)

# Project setup
project(test VERSION 1.0 LANGUAGES CXX)
# include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/lulcek/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE utils)

add_executable(regex_groups ${CMAKE_CURRENT_SOURCE_DIR}/regex_groups/main.cpp)

target_link_libraries(regex_groups PRIVATE utils)