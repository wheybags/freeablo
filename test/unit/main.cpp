#include "silentprinter.h"

#include <gtest/gtest.h>

void setSilent()
{
    auto& listeners = ::testing::UnitTest::GetInstance()->listeners();
    auto* listener = listeners.Release(listeners.default_result_printer());

    listeners.Append(new SilentPrinter(listener));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    setSilent();

    return RUN_ALL_TESTS();
}
