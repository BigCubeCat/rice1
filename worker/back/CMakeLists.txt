qt_add_library(BackLib
    md5.cpp
    md5.hpp
    hashworker.hpp
    hashworker.cpp
)

target_link_libraries(BackLib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
