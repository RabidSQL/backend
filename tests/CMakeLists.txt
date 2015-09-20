cmake_minimum_required(VERSION 3.2.2)
project(Test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++0x")
include_directories(${PROJECT_NAME} PUBLIC
    include
    ../logic/include
    ../
    /development/libs/gtest-1.7.0/include/
)

set(TEST_SOURCE_FILES
    source/testapplication.cpp
    source/testconnectionsettings.cpp
    source/testdatabaseconnection.cpp
    source/testvariant.cpp
    source/testsmartobject.cpp
    source/testthread.cpp
    source/testthreadlocal.cpp
    source/smartobjecttester.cpp
    source/testuuid.cpp
    source/testdatabaseconnectionmanager.cpp
    include/smartobjecttester.h
)

LINK_DIRECTORIES(
    /usr/local/mysql/lib
)

add_subdirectory(../logic/ logic)
add_subdirectory(../libs/drivers/mysql mysql)

add_executable(${PROJECT_NAME} ${TEST_SOURCE_FILES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Logic
    MySQL
    gtest
    gtest_main
    pthread
)