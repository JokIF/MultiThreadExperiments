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
            auto&& overallSum = GetOverallSummation();
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
