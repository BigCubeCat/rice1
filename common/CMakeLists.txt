project(CommonLib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

qt_add_library(${PROJECT_NAME} STATIC
    task.hpp
    utils.cpp
    utils.hpp
    worker.cpp
    worker.hpp
)
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::HttpServer
    Qt6::Xml
    Qt6::Network
    DtoLib
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
