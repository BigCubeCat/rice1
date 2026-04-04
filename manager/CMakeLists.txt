project(CrackManager LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(CrackManager
    main.cpp
    manager.hpp
    manager.cpp
    worker.hpp
    worker.cpp
    task.hpp
    dto/dto.hpp
    dto/status.hpp
    dto/crack.hpp
    utils.hpp
    utils.cpp
)

target_link_libraries(CrackManager PUBLIC
    Qt6::Core
    Qt6::HttpServer
    Qt6::Xml
    Qt6::Network
    DtoLib
)
