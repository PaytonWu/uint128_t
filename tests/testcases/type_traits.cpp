#include <type_traits>

#include <gtest/gtest.h>

#include "uint128.h"

TEST(Type_Traits, is_arithmetic){
    EXPECT_FALSE(std::is_arithmetic_v<uint128_t>);
}

TEST(Type_Traits, is_integral){
    EXPECT_FALSE(std::is_integral_v<uint128_t>);
}

TEST(Type_Traits, is_unsigned){
    EXPECT_FALSE(std::is_unsigned_v<uint128_t>);
}
