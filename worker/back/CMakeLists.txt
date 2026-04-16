add_library(BackLib STATIC
    md5.cpp
    md5.hpp
    hashworker.hpp
    hashworker.cpp
)

target_link_libraries(BackLib PRIVATE OpenSSL::SSL OpenSSL::Crypto Qt6::Core   
Qt6::Core
  Qt6::HttpServer
  Qt6::Xml
  Qt6::Network

)

target_include_directories(BackLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
