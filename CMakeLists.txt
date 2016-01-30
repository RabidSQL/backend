cmake_minimum_required(VERSION 3.2.2)
project(Backend)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++0x")
include_directories(${PROJECT_NAME} PUBLIC
    include/
    drivers/
    .
)

set(LOGIC_SOURCE_FILES
    drivers/DatabaseConnection.h
    include/App.h
    include/Application.h
    include/ArbitraryPointer.h
    include/BinaryFileStream.h
    include/ConnectionSettings.h
    include/Console.h
    include/DatabaseConnectionFactory.h
    include/DatabaseConnectionManager.h
    include/Enums.h
    include/FileStream.h
    include/JsonFileStream.h
    include/JsonHandler.h
    include/Message.h
    include/NSEnums.h
    include/QueryCommand.h
    include/QueryError.h
    include/QueryResult.h
    include/SettingsField.h
    include/SmartObject.h
    include/Thread.h
    include/ThreadLocal.h
    include/UUID.h
    include/Variant.h
    source/Application.cpp
    source/ConnectionSettings.cpp
    source/Console.cpp
    source/DatabaseConnection.cpp
    source/DatabaseConnectionFactory.cpp
    source/DatabaseConnectionManager.cpp
    source/BinaryFileStream.cpp
    source/FileStream.cpp
    source/JsonFileStream.cpp
    source/JsonHandler.cpp
    source/Message.cpp
    source/SettingsField.cpp
    source/SmartObject.cpp
    source/Thread.cpp
    source/UUID.cpp
    source/Variant.cpp
)

OPTION(BUILD_TESTS "Build unit tests" ON)
IF(BUILD_TESTS)
    add_subdirectory(tests)
ENDIF()

add_library(${PROJECT_NAME} STATIC ${LOGIC_SOURCE_FILES})
