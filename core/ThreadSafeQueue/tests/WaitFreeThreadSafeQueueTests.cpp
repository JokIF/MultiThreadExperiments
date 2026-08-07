#include "WaitFreeThreadSafeQueue.h"
#include "gtest/gtest.h"
#include <thread>
#include <vector>
#include <latch>


TEST(WaitFreeThreadSafeQueue, SingleThreadMethodsTest)
{
    ThreadSafeStructs::WaitFreeThreadSafeQueue<int> queue(3000);
    EXPECT_TRUE(queue.try_push(10));
    EXPECT_TRUE(queue.try_push(200));
    EXPECT_TRUE(queue.try_push(3000));

    int val;

    EXPECT_TRUE(queue.try_pop(val));
    EXPECT_EQ(val, 10);
    
    EXPECT_TRUE(queue.try_pop(val));
    EXPECT_EQ(val, 200);

    EXPECT_TRUE(queue.try_pop(val));
    EXPECT_EQ(val, 3000);

    EXPECT_FALSE(queue.try_pop(val));
}

TEST(WaitFreeThreadSafeQueue, MultiThreadTest)
{
    ThreadSafeStructs::WaitFreeThreadSafeQueue<int> queue(3000);
    std::atomic produced_count = 0;
    std::atomic producer_out = 0;
    std::atomic consumer_out = 0;
    std::latch start(5);

    auto producer = [&] (int value)
    {
        start.arrive_and_wait();

        int produced = 0;
        while (produced_count.fetch_add(1, std::memory_order_relaxed) < 10'000)
        {
            if (queue.try_push(value))
                produced += value;
            else
            {
                produced_count.fetch_sub(1, std::memory_order_relaxed);
                std::this_thread::yield();
            }
        }
        producer_out.fetch_add(produced);
    };

    auto consumer = [&]
    {
        start.arrive_and_wait();

        int consumed = 0;
        short spin = 0;
        int out_value = 0;
        while (spin < 32)
        {
            if (queue.try_pop(out_value))
            {
                consumed += out_value;
                spin = 0;
            }
            else
            {
                spin++;
                std::this_thread::yield();
            }
        }

        consumer_out.fetch_add(consumed);
    };

    std::vector<std::jthread> pool;

    pool.emplace_back(producer, 10);
    pool.emplace_back(producer, 20);
    pool.emplace_back(producer, 30);
    
    pool.emplace_back(consumer);
    pool.emplace_back(consumer);

    pool.clear();

    EXPECT_EQ(producer_out.load(), consumer_out.load());
}