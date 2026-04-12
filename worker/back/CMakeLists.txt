add_library(BackLib STATIC
    md5.cpp
    md5.hpp
    hashworker.hpp
    hashworker.cpp
)

target_link_libraries(BackLib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(BackLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

option(BUILD_TESTING OFF)
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
