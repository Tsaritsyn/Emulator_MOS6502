//
// Created by Mikhail on 29/08/2023.
//

#include "test.hpp"

TEST_F(MOS6502TestFixture, FirstTest) {
    printf("Hello test world!\n");
    EXPECT_EQ(1, 1) << "1 is not equal to 1";
}


TEST(FirstSuite, FirstTestInSuite) {
    printf("Hello test world!\n");
    EXPECT_EQ(2, 2) << "2 is not equal to 2";
}