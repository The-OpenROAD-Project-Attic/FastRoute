cmake_minimum_required (VERSION 2.8)
project (GlobalRouter)

set(BIN_NAME "FastRoute")
set(LIB_NAME "FastRoute")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O0")

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/flute-net.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/flute-ckt.cpp)
list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/rand-pts.cpp)

if(DEFINED ENV{TYPE_CALL})
        message("-- Will generate lib")

        add_definitions(-DLIBRARY_MODE)
        add_LIBRARY(${LIB_NAME} STATIC ${SRC_FILES})

        target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

else()
        message("-- Will generate binary")

        add_definitions(-DSTANDALONE_MODE)

        add_executable(${BIN_NAME} ${SRC_FILES})

        target_link_libraries(${BIN_NAME} m)

endif()
