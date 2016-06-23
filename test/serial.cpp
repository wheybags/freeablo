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

TEST(Serial, TestIntBasic)
{
    std::vector<uint8_t> buf(10, 0);
    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    constexpr int64_t min = 0;
    constexpr int64_t max = 2047;

    int64_t testVal = 0;
    int64_t readVal = 0;
    bool success = true;

    testVal = 0;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = 1;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = 1024;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = 2047;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);
}

TEST(Serial, TestIntNegative)
{
    std::vector<uint8_t> buf(10, 0);
    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    constexpr int64_t min = -10;
    constexpr int64_t max = 10;

    int64_t testVal = 0;
    int64_t readVal = 0;
    bool success = true;

    testVal = -5;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = -1;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = 0;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = 3;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(true);
    ASSERT_EQ(testVal, readVal);

    testVal = 1;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);

    testVal = -10;
    success = write.handleInt<min, max>(testVal);
    ASSERT_TRUE(success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_TRUE(success);
    ASSERT_EQ(testVal, readVal);
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();
    return retval;
}
