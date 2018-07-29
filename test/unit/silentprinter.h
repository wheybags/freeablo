#pragma once

#include <gtest/gtest.h>

using ::testing::TestCase;
using ::testing::TestEventListener;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

class SilentPrinter : public ::testing::EmptyTestEventListener
{
public:
    explicit SilentPrinter(::testing::TestEventListener* listener);

    void OnTestProgramStart(const UnitTest& unit_test) override;
    void OnTestIterationStart(const UnitTest& unit_test, int iteration) override;
    void OnEnvironmentsSetUpStart(const UnitTest& unit_test) override;
    void OnEnvironmentsSetUpEnd(const UnitTest& unit_test) override;
    void OnTestCaseStart(const TestCase& test_case) override;
    void OnTestStart(const TestInfo& test_info) override;
    void OnTestPartResult(const TestPartResult& test_part_result) override;
    void OnTestEnd(const TestInfo& test_info) override;
    void OnTestCaseEnd(const TestCase& test_case) override;
    void OnEnvironmentsTearDownStart(const UnitTest& unit_test) override;
    void OnEnvironmentsTearDownEnd(const UnitTest& unit_test) override;
    void OnTestIterationEnd(const UnitTest& unit_test, int iteration) override;
    void OnTestProgramEnd(const UnitTest& unit_test) override;

private:
    TestEventListener* listener;
};
