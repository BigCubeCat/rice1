cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(back)

qt_add_executable(CrackHashWorker
  main.cpp
  processor.hpp
  processor.cpp
)

target_link_libraries(CrackHashWorker PRIVATE
  Qt6::Core
  Qt6::HttpServer
  Qt6::Xml
  Qt6::Network
  BackLib
  DtoLib
)
