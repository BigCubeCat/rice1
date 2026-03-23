cmake_minimum_required(VERSION 3.16)

project(crack_hash_worker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core HttpServer Xml)

add_subdirectory(back)
add_subdirectory(front)
