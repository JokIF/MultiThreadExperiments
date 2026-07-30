#pragma once
#include "Priotex.h"

namespace unitTests
{
class UnitTestDataPriotex
{
public:
    explicit UnitTestDataPriotex(const mutex::Priotex& ptx) : ptx(ptx) {}

    mutex::Priotex::Priority GetPriotexPriority() const { return ptx.m_PriorityValue; }
    mutex::Priotex::Priority GetLastPriority() const { return ptx.m_prevPriorityValue; }
    mutex::Priotex::Priority GetCurrentThreadPriority() const { return ptx.m_thisThreadPriorityValue; }

private:
    const mutex::Priotex& ptx;
};
}