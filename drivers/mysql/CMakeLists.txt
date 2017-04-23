cmake_minimum_required(VERSION 3.2.2)
project(MySQL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=c++0x")
include_directories(${PROJECT_NAME} PUBLIC
    ../../
    ../../include/
    include
    /usr/local/include/cppconn/
)

set(MYSQL_SOURCE_FILES
    include/DatabaseConnection.h
    source/DatabaseConnection.cpp
)

link_directories(
    /usr/local/lib,
)

add_library(${PROJECT_NAME} STATIC ${MYSQL_SOURCE_FILES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    mysqlcppconn-static
    mysqlclient
)
