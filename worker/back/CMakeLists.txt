add_library(back_lib
    md5.cpp
    md5.hpp
    hashworker.hpp
    hashworker.cpp
)

target_link_libraries(back_lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
