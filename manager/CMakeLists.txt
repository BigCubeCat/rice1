cmake_minimum_required(VERSION 3.16)

project(crack_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Network HttpServer Xml)

add_executable(crack_manager
    main.cpp
    manager.hpp
    manager.cpp
    dto/dto.hpp
    dto/status.hpp
    dto/crack.hpp
)

target_link_libraries(crack_manager PUBLIC
    Qt6::Core
    Qt6::HttpServer
    Qt6::Xml
    Qt6::Network
)
