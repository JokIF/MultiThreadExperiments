#include "Priotex.h"
#include "PriotexTestUtils.h"
#include "gtest/gtest.h"

TEST(PriotexTest, CorrectTwoPriotexLock)
{
    mutex::Priotex firstPtx(10);
    mutex::Priotex secondPtx(8);

    EXPECT_EQ(firstPtx.GetTestWrapper()->GetPriotexPriorety(), 10);
    EXPECT_EQ(secondPtx.GetTestWrapper()->GetPriotexPriorety(), 8);

    EXPECT_NO_THROW(firstPtx.lock());
    EXPECT_EQ(firstPtx.GetTestWrapper()->GetCurrentThreadPriorety(), 10);
    
    EXPECT_NO_THROW(secondPtx.lock());
    EXPECT_EQ(secondPtx.GetTestWrapper()->GetCurrentThreadPriorety(), 8);
    EXPECT_EQ(secondPtx.GetTestWrapper()->GetLastPriorety(), 10);

    secondPtx.unlock();
    firstPtx.unlock();
}

TEST(PriotexTest, CorrectTwoPriotexUnLock)
{
    mutex::Priotex firstPtx(2);
    mutex::Priotex secondPtx(1);

    firstPtx.lock();
    secondPtx.lock();

    EXPECT_EQ(secondPtx.GetTestWrapper()->GetCurrentThreadPriorety(), 1);
    EXPECT_NO_THROW(secondPtx.unlock());
    EXPECT_EQ(secondPtx.GetTestWrapper()->GetCurrentThreadPriorety(), 2);
    EXPECT_NO_THROW(firstPtx.unlock());
}

TEST(PriotexTest, UncorrectTwoPriotexLock)
{
    mutex::Priotex firstPtx(2000000);
    mutex::Priotex secondPtx(100);

    EXPECT_NO_THROW(secondPtx.lock());
    EXPECT_EQ(secondPtx.GetTestWrapper()->GetCurrentThreadPriorety(), 100);
    EXPECT_THROW(firstPtx.lock(), std::logic_error);
    EXPECT_EQ(secondPtx.GetTestWrapper()->GetCurrentThreadPriorety(), 100);
}