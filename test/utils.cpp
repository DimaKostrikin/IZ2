#include "gtest/gtest.h"

extern "C" {
#include "vectlib/utils.h"
}

bool vect_cmp(double *first, double *second, int size) {
    double EPS = 0.000001;
    for (int i = 0; i < size; ++i) {
        if (fabs(first[i] - second[i]) > EPS) {
            return false;
        }
    }
    return true;
}

TEST(vector_norm, ok) {
    double a[4] = {1, 1, 1, 1};
    EXPECT_EQ(vector_norm(a, 4), 2);
}

TEST(get_vect, ok) {
    FILE *test_file = fopen("test.txt", "r");
    if (!test_file) {
        printf("error\n");
    }
    sizes_of_base sizes;
    sizes.vect_size = 4;
    sizes.base_size = 4;
    sizes.width_elemenet = 9;
    int stroke = 1;
    double *vect = get_vect(stroke, &sizes, test_file);
    double expecting_vect[4] = {2, 1, 3, 4};

    EXPECT_EQ(vect_cmp(vect, expecting_vect, sizes.vect_size), true);
}