add_library(BackLib STATIC
    md5.cpp
    md5.hpp
    hashworker.hpp
    hashworker.cpp
)

target_link_libraries(BackLib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(BackLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
