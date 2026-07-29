#include "Priotex.h"
#include "PriotexTestUtils.h"
#include "gtest/gtest.h"
#include <thread>
#include <future>
#include <chrono>

using namespace std::chrono_literals;

TEST(PriotexTest, CorrectMultiThreadUsing)
{
    long long unsigned int overallSummation = 0;
    mutex::Priotex deepestSumPtx(10);
        

    auto OverallSum = [&overallSummation, &deepestSumPtx](unsigned int toSumValue) 
    {
        std::lock_guard lock(deepestSumPtx);        
        overallSummation += toSumValue;
        return overallSummation;
    };

    auto GetOverallSummation = [&overallSummation, &deepestSumPtx]
    {
        std::lock_guard lock(deepestSumPtx);
        return overallSummation;
    };

    auto HardWork = [&OverallSum] (std::chrono::milliseconds hardWorkTimeout) 
    {
        for (size_t times = 0; times < 10; times++)
        {
            OverallSum(1);
            std::this_thread::sleep_for(1ms);
        }

        std::this_thread::sleep_for(hardWorkTimeout);
    };

    mutex::Priotex outerMainLoopPtx(100);
    std::promise<void> startProm;
    auto toSyncFuture = startProm.get_future().share();

    size_t iterTimesInMainLoop = 5;
    auto MainLoop = [&outerMainLoopPtx, &OverallSum, &GetOverallSummation, &HardWork, iterTimesInMainLoop, toSyncFuture] (std::chrono::milliseconds hardWorkTimeout)
    {
        toSyncFuture.wait();

        for (size_t i = 0; i < iterTimesInMainLoop; i++) {
            // start hard work
            HardWork(hardWorkTimeout);
            //end hard owrk

            std::lock_guard lock(outerMainLoopPtx);

            // work with other shared memory space
            [[maybe_unused]] auto&& overallSum = GetOverallSummation();
            // end work with other shared memory space
        }
    };
    
    std::thread t1(MainLoop, 13ms);
    std::thread t2(MainLoop, 27ms);
    std::thread t3(MainLoop, 6ms);

    std::this_thread::sleep_for(15ms);
    startProm.set_value();

    t1.join(); t2.join(); t3.join();
    long long unsigned int expectedOverallSummation = iterTimesInMainLoop * 3 * 10; // 3 - threads count; 10 - sum value per iter 
    EXPECT_EQ(GetOverallSummation(), expectedOverallSummation);
}

TEST(PriotexTest, ThreadLauncher) //need to move in other subdir
{
    threadWorks::ThreadLauncher tl;
    EXPECT_EQ(tl.GetThreadCount(), 0);
    auto th1 = tl.CreateThread([] { for (size_t i = 0; i < 1000; i++); });
    EXPECT_EQ(tl.GetThreadCount(), 1);
    th1.join();
    EXPECT_EQ(tl.GetThreadCount(), 0);
}

TEST(PriotexTest, TryLockMultiThreadUsing) //In doubt
{
    unsigned int GlobalCounter = 0;
    unsigned int GlobalAltCounter = 0;

    mutex::Priotex MainLoopPtx(3000);
    mutex::Priotex SomeOperationPtx(200);
    mutex::Priotex CounterPtx(10);

    auto IncreaseCounter = [&CounterPtx, &GlobalCounter]
    {
        std::lock_guard lock(CounterPtx);
        GlobalCounter++;
    };

    auto IncreaseAltCounter = [&CounterPtx, &GlobalAltCounter]
    {
        std::lock_guard lock(CounterPtx);
        GlobalAltCounter++;
    };

    auto SomeOpWithCounter = [&SomeOperationPtx, &IncreaseCounter, &IncreaseAltCounter]
    {
        std::unique_lock lock(SomeOperationPtx, std::defer_lock);
        if (lock.try_lock())
            std::this_thread::sleep_for(5ms);
        else
            IncreaseAltCounter();

        IncreaseCounter();
    };
    auto SomeOp = [&SomeOperationPtx]
    {
        std::lock_guard lock(SomeOperationPtx);
        std::this_thread::sleep_for(5ms);
    };


    size_t IterTimes = 5;
    auto MainLoopFirst = [IterTimes, &MainLoopPtx, &SomeOpWithCounter, &SomeOp]
    {
        for (size_t i = 0; i < IterTimes; i++) {
            std::lock_guard lock(MainLoopPtx);
            SomeOpWithCounter();
        }
    };

    auto MainLoopSecond = [IterTimes, &MainLoopPtx, &SomeOpWithCounter, &IncreaseCounter, &SomeOp]
    {
        std::unique_lock lock(MainLoopPtx);
        for (size_t i = 0; i < IterTimes; i++) {
            lock.unlock();
            SomeOp();
            lock.lock();
        }
    };
    
    {
        std::jthread th4(MainLoopSecond);
        std::jthread th5(MainLoopSecond);


        std::jthread th1(MainLoopFirst);
        std::jthread th2(MainLoopFirst);
        std::jthread th3(MainLoopFirst);
    }
    
    EXPECT_EQ(GlobalCounter, GlobalAltCounter);
}