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
project(rsyn)

# Add db as a sub-project
add_subdirectory(db)
add_subdirectory(third_party/lef)
add_subdirectory(third_party/def)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(WINDOWS true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(LINUX true)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOSX true)
endif()

# SET the output folder where your program will be created
set(CMAKE_BINARY_DIR bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 11)

set(PATH_RSYN ${CMAKE_CURRENT_SOURCE_DIR}/src)

###################
# Warnings/Errors #
###################

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-type")

######################
# Check Dependencies #
######################

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

# OpenMP (work around)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

###############
# Source Code #
###############

file(GLOB_RECURSE SRC_FILES
        ${PATH_RSYN}/*.cpp
        ${PATH_RSYN}/*.cc
        ${PATH_RSYN}/*.c
        ${PATH_RSYN}/../third_party/liberty/*.c
        ${PATH_RSYN}/../third_party/liberty/*.cc
        ${PATH_RSYN}/../third_party/liberty/*.cpp
        )

###########
# Targets #
###########

add_executable(rsyn ${SRC_FILES})

#######################
# Include Directories #
#######################

target_include_directories(rsyn PUBLIC
        "./db"
        "./include"
        "./third_party/lef/lef"
        "./third_party/def/def"
        "./../flute"
        "./src"
        "./src/contests"
        )

##################
# Compiler Flags #
##################

target_compile_options(rsyn PUBLIC "-O3")
target_compile_options(rsyn PUBLIC "-Wno-unused-result -Wno-format -Wno-return-type -Wno-unused-local-typedefs")
target_compile_options(rsyn PUBLIC "-DRSYN_NO_GUI")

#################
# Linker Flags  #
#################

target_link_libraries(rsyn
        FastRoute
        db
        lef
        def
        flute
        )

# Boost
target_include_directories(rsyn PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(rsyn ${Boost_LIBRARIES})
