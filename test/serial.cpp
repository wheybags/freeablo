#include <gtest/gtest.h>

#include <serial/bitstream.h>

#define private public
#include "../apps/freeablo/faworld/position.h"


TEST(Serial, TestStringLength)
{
    std::string test = "asdf";

    std::vector<uint8_t> buf(test.length(), 255);

    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    Serial::Error::Error err = Serial::Error::Success;

    err = write.handleString((uint8_t*)&test[0], test.length());
    ASSERT_EQ(Serial::Error::Success, err);

    bool b = true;
    err = write.handleBool(b);
    ASSERT_EQ(Serial::Error::EndOfStream, err);

    err = Serial::Error::Success;
    write.seek(0, Serial::BSPos::Start);
    
    test = "asdfg";
    err = write.handleString((uint8_t*)&test[0], test.length());
    ASSERT_EQ(Serial::Error::EndOfStream, err);
}

TEST(Serial, TestStringPadding)
{
    std::string test = "a";

    std::vector<uint8_t> buf(64, 255);

    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    for (uint32_t i = 1; i < 8; i++)
    {
        write.seek(0, Serial::BSPos::Start);

        bool b = true;

        for (uint32_t j = 0; j < i; j++)
            write.handleBool(b);

        write.handleString((uint8_t*)&test[0], test.length());

        ASSERT_EQ(16, write.tell());
    }
}


TEST(Serial, TestWriteString)
{
    std::vector<uint8_t> buf(1024, 255);

    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    std::string test = "hello i am a test";
    uint32_t len = test.length();

    Serial::Error::Error err = Serial::Error::Success;

    err = write.handleInt32(len);
    ASSERT_EQ(Serial::Error::Success, err);
    uint32_t posAfterWritingLen = write.tell();

    write.handleString((uint8_t*)&test[0], test.length());
    ASSERT_EQ(Serial::Error::Success, err);



    uint32_t readLen = 0;
    err = read.handleInt32(readLen);
    ASSERT_EQ(Serial::Error::Success, err);
    ASSERT_EQ(len, readLen);

    std::string readStr(readLen, '\0');
    err = read.handleString((uint8_t*)&readStr[0], readLen);
    ASSERT_EQ(Serial::Error::Success, err);
    ASSERT_EQ(readStr, test);

    ASSERT_EQ(write.tell(), int64_t((test.length()*8) + posAfterWritingLen));
}

TEST(Serial, TestFillZeros)
{
    std::vector<uint8_t> buf(10, 255);

    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());
   
    bool b = true;
    for(size_t i = 0; i < 3; i++)
        write.handleBool(b);

    write.fillWithZeros();


    for (size_t i = 0; i < 3; i++)
    {
        read.handleBool(b);
        ASSERT_TRUE(b);
    }

    Serial::Error::Error err = Serial::Error::Success;
    
    while((err = read.handleBool(b)) != Serial::Error::EndOfStream)
        ASSERT_FALSE(b);


    // now write 8 bools so we can test the case where we start filling from a byte boundary
    read.seek(0, Serial::BSPos::Start);
    write.seek(0, Serial::BSPos::Start);


    b = true;
    for (size_t i = 0; i < 8; i++)
        write.handleBool(b);

    write.fillWithZeros();


    for (size_t i = 0; i < 8; i++)
    {
        read.handleBool(b);
        ASSERT_TRUE(b);
    }

    err = Serial::Error::Success;

    while ((err = read.handleBool(b)) != Serial::Error::EndOfStream)
        ASSERT_FALSE(b);
}


TEST(Serial, TestVerifyZeros)
{
    std::vector<uint8_t> buf(10, 255);

    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());
    
    ASSERT_FALSE(read.verifyZeros());

    write.fillWithZeros();

    read.seek(0, Serial::BSPos::Start);
    ASSERT_TRUE(read.verifyZeros());


    write.seek(0, Serial::BSPos::Start);
    read.seek(0, Serial::BSPos::Start);

    bool b = true;
    int32_t i = 87;
    write.handleBool(b);
    write.handleInt32(i);
    write.fillWithZeros();

    read.handleBool(b);
    ASSERT_TRUE(b);
    i = 0;
    read.handleInt32(i);
    ASSERT_EQ(i, 87);
    ASSERT_TRUE(read.verifyZeros());
}

TEST(Serial, TestBoolPingPong)
{
    std::vector<uint8_t> buf(10, 0);

    Serial::WriteBitStream write(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool tmp = i % 2 == 0;
        Serial::Error::Error success = write.handleBool(tmp);
        ASSERT_EQ(success, Serial::Error::Success);
    }

    Serial::ReadBitStream read(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool val;
        Serial::Error::Error success = read.handleBool(val);
        ASSERT_EQ(success, Serial::Error::Success);

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
        bool tmp = groundTruthVec[i];
        Serial::Error::Error success = write.handleBool(tmp);
        ASSERT_EQ(success, Serial::Error::Success);
    }

    Serial::ReadBitStream read(&buf[0], buf.size());
    for (uint32_t i = 0; i < buf.size() * 8; i++)
    {
        bool val;
        Serial::Error::Error success = read.handleBool(val);
        ASSERT_EQ(success, Serial::Error::Success);

        bool groundTruth = groundTruthVec[i];
        ASSERT_EQ(val, groundTruth);
    }
}

TEST(Serial, TestBoolOverflow)
{
    uint8_t v = 0;
    Serial::WriteBitStream write(&v, 1);
    
    bool tmp = false;
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);
    ASSERT_EQ(write.handleBool(tmp), Serial::Error::Success);

    ASSERT_EQ(write.handleBool(tmp), Serial::Error::EndOfStream);

    Serial::ReadBitStream read(&v, 1);

    bool testVal;
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);
    ASSERT_EQ(read.handleBool(testVal), Serial::Error::Success);

    ASSERT_EQ(read.handleBool(testVal), Serial::Error::EndOfStream);
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
    Serial::Error::Error success = Serial::Error::Success;

    testVal = 0;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 1;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 1024;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 2047;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
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
    Serial::Error::Error success = Serial::Error::Success;

    testVal = -5;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = -1;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 0;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 3;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 1;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = -10;
    success = write.handleInt<min, max>(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt<min, max>(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);
}


TEST(Serial, TestInt32Range)
{
    std::vector<uint8_t> buf(100, 0);
    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    int32_t readVal = 0;
    Serial::Error::Error success = Serial::Error::Success;

    int64_t min = std::numeric_limits<int32_t>::min();
    int64_t max = std::numeric_limits<int32_t>::max();
    int64_t spacer = 100000; // can't run for every possible value, it would take too long

    // use 64-bit int for the counter to avoid overflow issues
    for (int64_t i = min; i < max; i += spacer)
    {
        int32_t val = (int32_t)i;
        success = write.handleInt32(val);
        ASSERT_EQ(success, Serial::Error::Success);
        success = read.handleInt32(readVal);
        ASSERT_EQ(success, Serial::Error::Success);
        ASSERT_EQ(i, readVal);

        write.seek(0, Serial::BSPos::Start);
        read.seek(0, Serial::BSPos::Start);
    }
}

TEST(Serial, TestInt32Basic)
{
    std::vector<uint8_t> buf(100, 0);
    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    int32_t testVal = 0;
    int32_t readVal = 0;
    Serial::Error::Error success = Serial::Error::Success;
    
    
    testVal = 78;
    success = write.handleInt32(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt32(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 200;
    success = write.handleInt32(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt32(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 40065;
    success = write.handleInt32(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt32(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);

    testVal = 4194304;
    success = write.handleInt32(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt32(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);
   
    testVal = 2147483647;
    success = write.handleInt32(testVal);
    ASSERT_EQ(success, Serial::Error::Success);
    success = read.handleInt32(readVal);
    ASSERT_EQ(success, Serial::Error::Success);
    ASSERT_EQ(testVal, readVal);
}

TEST(Serial, TestPos)
{
    std::vector<uint8_t> buf(100, 0);
    Serial::WriteBitStream write(&buf[0], buf.size());
    Serial::ReadBitStream read(&buf[0], buf.size());

    FAWorld::Position p(234, 754, 2);
    p.mDist = 50;
    Serial::Error::Error success = write.handleObject(p);
    ASSERT_EQ(success, Serial::Error::Success);

    FAWorld::Position p2;
    read.handleObject(p2);
    ASSERT_EQ(success, Serial::Error::Success);

    ASSERT_EQ(p.mCurrent, p2.mCurrent);
    ASSERT_EQ(p.mDist, p2.mDist);
    ASSERT_EQ(p.mDirection, p2.mDirection);
}

TEST(Serial, TestResizable)
{
    std::vector<uint8_t> buf(1, 0);
    Serial::WriteBitStream write(buf);
    Serial::Error::Error err = Serial::Error::Success;


    bool b = true;

    ASSERT_EQ(1U, buf.size());
    for (uint32_t i = 0; i < 8; ++i)
    {
        err = write.handleBool(b);
        ASSERT_EQ(err, Serial::Error::Success);
    }

    b = false;

    ASSERT_EQ(1U, buf.size());
    err = write.handleBool(b);
    ASSERT_EQ(err, Serial::Error::Success);
    ASSERT_EQ(2U, buf.size());

    buf.resize(1);
    buf[0] = 0;
    write = Serial::WriteBitStream(buf);

    int32_t i = 87;

    ASSERT_EQ(1U, buf.size());
    err = write.handleInt32(i);
    ASSERT_EQ(err, Serial::Error::Success);
    ASSERT_EQ(1U, buf.size());
    err = write.handleInt32(i);
    ASSERT_EQ(err, Serial::Error::Success);
    ASSERT_EQ(2U, buf.size());

    buf.resize(1);
    buf[0] = 0;
    write = Serial::WriteBitStream(buf);

    std::string str = "1234";
    ASSERT_EQ(1U, buf.size());
    err = write.handleString((uint8_t*)&str[0], 4);
    ASSERT_EQ(err, Serial::Error::Success);
    ASSERT_EQ(4U, buf.size());
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();
    return retval;
}
