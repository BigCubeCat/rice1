#include <string>

#include <gtest/gtest.h>

#include "md5.hpp"

TEST(MD5Test, EmptyString) {
    EXPECT_EQ(
        crack_hash_worker::back::md5(""), "d41d8cd98f00b204e9800998ecf8427e"
    );
}

TEST(MD5Test, HelloWorld) {
    EXPECT_EQ(
        crack_hash_worker::back::md5("Hello, world!"),
        "6cd3556deb0da54bca060b4c39479839"
    );
}

TEST(MD5Test, Consistency) {
    std::string input = "test_string";
    EXPECT_EQ(
        crack_hash_worker::back::md5(input), crack_hash_worker::back::md5(input)
    );
}
