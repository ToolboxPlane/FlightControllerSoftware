#include <cfenv>
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    feenableexcept(FE_INVALID | FE_OVERFLOW | FE_DIVBYZERO); // Floating point exceptions

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
