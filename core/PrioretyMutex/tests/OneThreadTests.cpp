#include "Priotex.h"
#include "PriotexTestUtils.h"
#include "gtest/gtest.h"

using namespace unitTests;

TEST(PriotexTest, CorrectTwoPriotexLock)
{
    mutex::Priotex firstPtx(10);
    mutex::Priotex secondPtx(8);

    UnitTestDataPriotex firstPtxTestWrapper(firstPtx);
    UnitTestDataPriotex secondPtxTestWrapper(secondPtx);

    EXPECT_EQ(firstPtxTestWrapper.GetPriotexPriorety(), 10);
    EXPECT_EQ(secondPtxTestWrapper.GetPriotexPriorety(), 8);

    EXPECT_NO_THROW(firstPtx.lock());
    EXPECT_EQ(firstPtxTestWrapper.GetCurrentThreadPriorety(), 10);
    
    EXPECT_NO_THROW(secondPtx.lock());
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriorety(), 8);
    EXPECT_EQ(secondPtxTestWrapper.GetLastPriorety(), 10);

    secondPtx.unlock();
    firstPtx.unlock();
}

TEST(PriotexTest, CorrectTwoPriotexUnLock)
{
    mutex::Priotex firstPtx(2);
    mutex::Priotex secondPtx(1);
    UnitTestDataPriotex secondPtxTestWrapper(secondPtx);

    firstPtx.lock();
    secondPtx.lock();

    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriorety(), 1);
    EXPECT_NO_THROW(secondPtx.unlock());
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriorety(), 2);
    EXPECT_NO_THROW(firstPtx.unlock());
}

TEST(PriotexTest, UncorrectTwoPriotexLock)
{
    mutex::Priotex firstPtx(2000000);
    mutex::Priotex secondPtx(100);
    UnitTestDataPriotex secondPtxTestWrapper(secondPtx);

    EXPECT_NO_THROW(secondPtx.lock());
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriorety(), 100);
    EXPECT_THROW(firstPtx.lock(), std::logic_error);
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriorety(), 100);
}