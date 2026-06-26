#pragma once
#ifdef BUILD_TESTING
#include "Priotex.h"

namespace unitTests
{
class UnitTestDataPriotex
{
public:
    UnitTestDataPriotex(const mutex::Priotex& ptx) : ptx(ptx) {}

    mutex::Priotex::Priorety GetPriotexPriorety() const { return ptx.m_prioretyValue; }
    mutex::Priotex::Priorety GetLastPriorety() const { return ptx.m_prevPrioretyValue; }
    mutex::Priotex::Priorety GetCurrentThreadPriorety() const { return ptx.m_thisThreadPrioretyValue; }

private:
    const mutex::Priotex& ptx;
};
}

#endif