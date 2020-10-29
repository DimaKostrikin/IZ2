#include "gtest/gtest.h"

extern "C" {
#include "vectlib/utils.h"
}

TEST(vector_norm, ok) {
    double a[4] = {1, 1, 1, 1};
    EXPECT_EQ(vector_norm(a, 4), 2);
}