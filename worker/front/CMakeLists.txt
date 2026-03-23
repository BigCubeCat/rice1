project(CrackHashWorker VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(SOURCES main.cpp)
set(HEADERS frontserver.hpp httpconnection.hpp)

qt_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::HttpServer
    Qt6::Network
    Qt6::Xml
)
