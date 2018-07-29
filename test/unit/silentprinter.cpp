#include "silentprinter.h"

SilentPrinter::SilentPrinter(::testing::TestEventListener* listener) : listener(listener) {}

void SilentPrinter::OnTestProgramStart(const UnitTest& /*unit_test*/) {}

void SilentPrinter::OnTestIterationStart(const UnitTest& /*unit_test*/, int /*iteration*/) {}

void SilentPrinter::OnEnvironmentsSetUpStart(const UnitTest& /*unit_test*/) {}

void SilentPrinter::OnEnvironmentsSetUpEnd(const UnitTest& /*unit_test*/) {}

void SilentPrinter::OnTestCaseStart(const TestCase& /*test_case*/) {}

void SilentPrinter::OnTestStart(const TestInfo& /*test_info*/) {}

void SilentPrinter::OnTestPartResult(const TestPartResult& test_part_result)
{
    if (test_part_result.failed())
        listener->OnTestPartResult(test_part_result);
}

void SilentPrinter::OnTestEnd(const TestInfo& /*test_info*/) {}

void SilentPrinter::OnTestCaseEnd(const TestCase& /*test_case*/) {}

void SilentPrinter::OnEnvironmentsTearDownStart(const UnitTest& /*unit_test*/) {}

void SilentPrinter::OnEnvironmentsTearDownEnd(const UnitTest& /*unit_test*/) {}

void SilentPrinter::OnTestIterationEnd(const UnitTest& unit_test, int iteration) { listener->OnTestIterationEnd(unit_test, iteration); }

void SilentPrinter::OnTestProgramEnd(const UnitTest& unit_test) { listener->OnTestProgramEnd(unit_test); }
