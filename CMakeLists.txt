cmake_minimum_required(VERSION 3.10)

# Project setup
project(utils VERSION 1.0 LANGUAGES CXX)
# include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# find_package(fmt CONFIG REQUIRED)
# find_package(asio CONFIG REQUIRED)
# find_package(magic_enum CONFIG REQUIRED)
# --------------------------------------------------
# Your target
# --------------------------------------------------

# Collect source files
file(GLOB_RECURSE SOURCES src/*.cpp)


add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

# Define the executable
add_executable(${PROJECT_NAME}_lulcek ${CMAKE_CURRENT_SOURCE_DIR}/examples/lulcek/main.cpp)

# target_link_libraries(${PROJECT_NAME}
#         PRIVATE fmt::fmt asio::asio magic_enum::magic_enum
# )

# Attach include directories to the target (after it's created)
target_include_directories(${PROJECT_NAME}_lulcek
        PUBLIC
        include
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}_lulcek PRIVATE ${PROJECT_NAME})
