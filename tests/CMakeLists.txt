cmake_minimum_required(VERSION 3.2.2)
project(Backend_Test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++0x")
include_directories(${PROJECT_NAME} PUBLIC
    include
    ../include
    ../drivers
    ../
    ../libs/googletest/googletest/include/
    ../libs/googletest/googlemock/include/
)

set(TEST_SOURCE_FILES
    source/TestApplication.cpp
    source/TestConnectionSettings.cpp
    source/TestDatabaseConnection.cpp
    source/TestVariant.cpp
    source/TestSmartObject.cpp
    source/TestThread.cpp
    source/TestThreadLocal.cpp
    source/SmartObjectTester.cpp
    source/TestUUID.cpp
    source/TestDatabaseConnectionManager.cpp
    include/MockApplication.h
    include/MockConnectionSettings.h
    include/MockDatabaseConnection.h
    include/MockSmartObject.h
    include/SmartObjectTester.h
)

LINK_DIRECTORIES(
    /usr/local/mysql/lib
)

add_subdirectory(../libs/googletest/googlemock googlemock)
add_subdirectory(../drivers/mysql mysql)

add_executable(${PROJECT_NAME} ${TEST_SOURCE_FILES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    Backend
    MySQL
    gmock
    gtest
    gtest_main
    pthread
)
