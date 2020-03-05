#include <gtest/gtest.h>
#include <misc/misc.h>

int main(int argc, char* argv[])
{
    Misc::saveArgv0(argv[0]);
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
