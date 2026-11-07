cmake_minimum_required(VERSION 3.15)
project(mecha_drive CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

set(TDLIB_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../td")

include_directories("${TDLIB_BASE_DIR}")
include_directories("${TDLIB_BASE_DIR}/build")
include_directories("${TDLIB_BASE_DIR}/td/generate/auto")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

link_directories("${TDLIB_BASE_DIR}/build")
link_directories("${TDLIB_BASE_DIR}/build/tdactor")
link_directories("${TDLIB_BASE_DIR}/build/tddb")
link_directories("${TDLIB_BASE_DIR}/build/tdnet")
link_directories("${TDLIB_BASE_DIR}/build/tdutils")
link_directories("${TDLIB_BASE_DIR}/build/tde2e")
link_directories("${TDLIB_BASE_DIR}/build/tdmtproto")
link_directories("${TDLIB_BASE_DIR}/build/sqlite") 

file(GLOB_RECURSE PROJECT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(mecha_drive 
    main.cpp 
    src/Maru.cpp 
    src/MaruAuth.cpp 
    src/MaruCommands.cpp
)


target_link_libraries(mecha_drive PRIVATE 
    tdclient
    tdcore
    tdapi
    tdjson
    tde2e
    tdmtproto
    tdactor
    tddb
    tdsqlite     
    tdnet
    tdutils
    OpenSSL::SSL 
    OpenSSL::Crypto 
    Threads::Threads
    z
    dl
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.txt ${CMAKE_CURRENT_BINARY_DIR}/config.txt COPYONLY)

