#include <gtest/gtest.h>

extern "C" {
#include <Drivers/ring_buffer.h>
}

TEST(TEST_NAME, init) {
    auto ringBufferData = ring_buffer_init();
    EXPECT_EQ(ringBufferData.len, 0);
    uint8_t tmp = 255;
    EXPECT_FALSE(ring_buffer_get(&ringBufferData, &tmp));
}

TEST(TEST_NAME, single) {
    auto ringBufferData = ring_buffer_init();
    uint8_t tmp = 255;

    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 17));
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 17);
    EXPECT_FALSE(ring_buffer_get(&ringBufferData, &tmp));

    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 37));
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 37);
    EXPECT_FALSE(ring_buffer_get(&ringBufferData, &tmp));
}

TEST(TEST_NAME, blockwise) {
    auto ringBufferData = ring_buffer_init();
    uint8_t tmp = 255;

    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 17));
    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 18));
    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 19));
    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 20));
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 17);
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 18);
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 19);
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 20);
    EXPECT_FALSE(ring_buffer_get(&ringBufferData, &tmp));

    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 37));
    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 38));
    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 39));
    EXPECT_TRUE(ring_buffer_put(&ringBufferData, 40));
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 37);
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 38);
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 39);
    EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
    EXPECT_EQ(tmp, 40);
    EXPECT_FALSE(ring_buffer_get(&ringBufferData, &tmp));
}

TEST(TEST_NAME, full) {
    auto ringBufferData = ring_buffer_init();
    uint8_t tmp = 255;

    for (uint16_t c = 0; c < RING_BUFFER_SIZE; ++c) {
        EXPECT_TRUE(ring_buffer_put(&ringBufferData, (c * 2) % 251));
    }
    EXPECT_FALSE(ring_buffer_put(&ringBufferData, 1));


    for (uint16_t c = 0; c < RING_BUFFER_SIZE; ++c) {
        EXPECT_TRUE(ring_buffer_get(&ringBufferData, &tmp));
        EXPECT_EQ(tmp, (c * 2) % 251);
    }
    EXPECT_FALSE(ring_buffer_get(&ringBufferData, &tmp));
}
