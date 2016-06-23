#include <gtest/gtest.h>

#include <serial/bitstream.h>

TEST(Serial, TestBoolPingPong)
{
    std::vector<uint8_t> buf(10, 0);

    Serial::WriteBitStream write(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool success = write.handleBool(i % 2 == 0);
        ASSERT_TRUE(success);
    }

    Serial::ReadBitStream read(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool val;
        bool success = read.handleBool(val);
        ASSERT_TRUE(success);

        bool groundTruth = i % 2 == 0;
        ASSERT_EQ(val, groundTruth);
    }
}

TEST(Serial, TestBoolPredefined)
{
    std::vector<uint8_t> buf(10, 0);

    std::vector<bool> groundTruthVec(buf.size() * 8, false);

    groundTruthVec[4] = true;
    groundTruthVec[15] = true;
    groundTruthVec[3] = true;
    groundTruthVec[12] = true;

    Serial::WriteBitStream write(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool success = write.handleBool(groundTruthVec[i]);
        ASSERT_TRUE(success);
    }

    Serial::ReadBitStream read(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool val;
        bool success = read.handleBool(val);
        ASSERT_TRUE(success);

        bool groundTruth = groundTruthVec[i];
        ASSERT_EQ(val, groundTruth);
    }
}

TEST(Serial, TestBoolOverflow)
{
    uint8_t v = 0;
    Serial::WriteBitStream write(&v, 1);

    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));
    ASSERT_TRUE(write.handleBool(false));

    ASSERT_FALSE(write.handleBool(false));

    Serial::ReadBitStream read(&v, 1);

    bool testVal;
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));
    ASSERT_TRUE(read.handleBool(testVal));

    ASSERT_FALSE(read.handleBool(testVal));
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();
    return retval;
}
