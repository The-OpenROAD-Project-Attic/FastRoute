# Rsyn
# CMake Examples: https://github.com/ttroy50/cmake-examples

#########
# Setup #
#########

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

# Message
message(STATUS "Rsyn")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Project's name
project(fastroute-lefdef)

# Add db as a sub-project
add_subdirectory(fastroute-lib)
add_subdirectory(rsyn)
