find_package(GTest REQUIRED)

add_executable(back_tests
    md5_test.cc
    hashworker_test.cc
    main.cc
)

target_link_libraries(back_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        BackLib
)

target_include_directories(back_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/back
)

add_test(NAME back_tests COMMAND back_tests)
