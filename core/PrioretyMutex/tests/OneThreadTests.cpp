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

    EXPECT_EQ(firstPtxTestWrapper.GetPriotexPriority(), 10);
    EXPECT_EQ(secondPtxTestWrapper.GetPriotexPriority(), 8);

    EXPECT_NO_THROW(firstPtx.lock());
    EXPECT_EQ(firstPtxTestWrapper.GetCurrentThreadPriority(), 10);
    
    EXPECT_NO_THROW(secondPtx.lock());
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriority(), 8);
    EXPECT_EQ(secondPtxTestWrapper.GetLastPriority(), 10);

    secondPtx.unlock();
    firstPtx.unlock();
}

TEST(PriotexTest, CorrectTwoPriotexUnLock)
{
    mutex::Priotex firstPtx(3);
    mutex::Priotex secondPtx(2);
    mutex::Priotex thirdPtx(1);
    UnitTestDataPriotex ptxTestWrapper(secondPtx);

    firstPtx.lock();
    secondPtx.lock();
    thirdPtx.lock();

    EXPECT_EQ(ptxTestWrapper.GetCurrentThreadPriority(), 1);
    EXPECT_NO_THROW(thirdPtx.unlock());
    EXPECT_EQ(ptxTestWrapper.GetCurrentThreadPriority(), 2);
    EXPECT_NO_THROW(secondPtx.unlock());
    EXPECT_EQ(ptxTestWrapper.GetCurrentThreadPriority(), 3);
    EXPECT_NO_THROW(firstPtx.unlock());
}

TEST(PriotexTest, UncorrectTwoPriotexLock)
{
    mutex::Priotex firstPtx(2000000);
    mutex::Priotex secondPtx(100);
    UnitTestDataPriotex secondPtxTestWrapper(secondPtx);

    EXPECT_NO_THROW(secondPtx.lock());
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriority(), 100);
    EXPECT_THROW(firstPtx.lock(), std::logic_error);
    EXPECT_EQ(secondPtxTestWrapper.GetCurrentThreadPriority(), 100);
}