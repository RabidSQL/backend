cmake_minimum_required(VERSION 3.2.2)
project(Logic)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++0x")
include_directories(${PROJECT_NAME} PUBLIC
    include/
    ../
    ../libs/drivers/
)

set(LOGIC_SOURCE_FILES
    source/application.cpp
    source/connectionsettings.cpp
    source/console.cpp
    source/databaseconnection.cpp
    source/databaseconnectionfactory.cpp
    source/databaseconnectionmanager.cpp
    source/smartobject.cpp
    source/thread.cpp
    source/variant.cpp
    source/uuid.cpp
    include/application.h
    include/connectionsettings.h
    include/console.h
    include/databaseconnection.h
    include/databaseconnectionfactory.h
    include/databaseconnectionmanager.h
    include/smartobject.h
    include/structs.h
    include/thread.h
    include/threadlocal.h
    include/variant.h
    include/uuid.h
    source/filestream.cpp
    include/filestream.h
)

add_library(${PROJECT_NAME} STATIC ${LOGIC_SOURCE_FILES})
