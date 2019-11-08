################################################################################
## Authors: Vitor Bandeira, Eder Matheus Monteiro e Isadora Oliveira
##          (Advisor: Ricardo Reis)
##
## BSD 3-Clause License
##
## Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice, this
##   list of conditions and the following disclaimer.
##
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and#or other materials provided with the distribution.
##
## * Neither the name of the copyright holder nor the names of its
##   contributors may be used to endorse or promote products derived from
##   this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
################################################################################

cmake_minimum_required(VERSION 3.1)

project(FastRoute4-lefdef)
set(BIN_NAME "FRlefdef")
message(STATUS "FastRoute4.1 Lef/Def version")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

# Print info on build
if( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
        message( "INFO: DEBUG BUILD" )
        # for debug type builds, turn on verbose makefiles
        set(CMAKE_VERBOSE_MAKEFILE ON)

        # Tell other CMake files that we're doing a debug build
        set(DEBUG_BUILD 1)

        # Tell C/C++ that we're doing a debug build
        add_definitions(-DDEBUG)
endif()
if( CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" )
        message( "INFO: RELEASE BUILD" )
endif()

set(BUILD_TCL OFF CACHE BOOL "Build TCL SWIG module" FORCE)

add_subdirectory(third_party/fastroute)
add_subdirectory(third_party/OpenDB)
add_subdirectory(third_party/flute3)

set(CMAKE_CXX_STANDARD 11)
set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

# Finds tcl
include(FindTCL.cmake)

# Loads swig and generates the tcl wrapper file
find_package(SWIG REQUIRED)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/TclInterfaceWrap.cpp
        COMMAND ${SWIG_EXECUTABLE} -tcl8 -c++ -o ${CMAKE_CURRENT_SOURCE_DIR}/src/TclInterfaceWrap.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/TclInterface.i
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating swig wrapper"
)
add_custom_target(tclInterfaceWrapper DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/TclInterfaceWrap.cpp)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/src/TclInterfaceWrap.cpp)

add_executable(${BIN_NAME} ${SRC_FILES})

add_dependencies(${BIN_NAME} tclInterfaceWrapper)

target_include_directories(${BIN_NAME} PUBLIC
        ${Boost_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fastroute/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/flute
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/OpenDB/include/opendb
        )

target_link_libraries(${BIN_NAME}
        ${Boost_LIBRARIES}
        opendb
        lefin
        defin
        FastRoute
        flute
        tm
        zlib
        zutil
        rt
        )