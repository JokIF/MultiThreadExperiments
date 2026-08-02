#include "gtest/gtest.h"
#include "ThreadSafeQueue.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <future>
#include <latch>
#include <ranges>

TEST(ThreadSafeQueue, SingleThreadMethodsTest)
{
    ThreadSafeStructs::ThreadSafeQueue<int> queue;
    
    EXPECT_TRUE(queue.empty());
    queue.push(10);
    queue.push(200);
    queue.push(3000);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(*queue.try_pop(), 10);

    int test_value = 0;
    EXPECT_TRUE(queue.try_pop(test_value));
    EXPECT_EQ(test_value, 200);

    EXPECT_EQ(*queue.wait_pop(), 3000);
    
    EXPECT_TRUE(queue.empty());

    test_value = 0;
    EXPECT_FALSE(queue.try_pop(test_value));
    EXPECT_EQ(test_value, 0);
    EXPECT_EQ(queue.try_pop(), nullptr);

    test_value = 0;

    auto f = std::async([&]
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        queue.push(40000);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        queue.push(500000);
    });
    EXPECT_EQ(*queue.wait_pop(), 40000);

    queue.wait_pop(test_value);
    EXPECT_EQ(test_value, 500000);
}

TEST(ThreadSafeQueue, MultiThreadTest)
{
    constexpr size_t writers_count = 3;
    constexpr size_t readers_count = 2;
    constexpr unsigned write_iter_count = 1000;

    ThreadSafeStructs::ThreadSafeQueue<unsigned> queue;
    
    std::atomic<unsigned> out_sum = 0;
    std::atomic<int> writers_done = 0;
    std::latch  start(writers_count + readers_count);

    auto WriteQueue = [&start, &queue, &writers_done, write_iter_count]
    {
        start.arrive_and_wait();
        for (unsigned i = 0; i < write_iter_count; i++)
            queue.push(i);

        writers_done.fetch_add(1, std::memory_order_release);
    };

    auto ReadQueue = [&start, &queue, &writers_done, writers_count, &out_sum] 
    {
        start.arrive_and_wait();

        unsigned inner_sum = 0;
        while (writers_done.load(std::memory_order_acquire) != writers_count || !queue.empty())
        {
            if (auto queue_value = queue.try_pop(); queue_value != nullptr)
                inner_sum += *queue_value;
            else
                std::this_thread::yield();
        }

        out_sum.fetch_add(inner_sum, std::memory_order_relaxed);
    };

    std::vector<std::jthread> pool;

    for ([[maybe_unused]] auto _ : std::views::iota(0uz, writers_count))
        pool.emplace_back(WriteQueue);

    for ([[maybe_unused]] auto _ : std::views::iota(0uz, readers_count))
        pool.emplace_back(ReadQueue);

    pool.clear();

    unsigned expected_value = static_cast<unsigned>((write_iter_count * (write_iter_count - 1)) / 2) * writers_count; 
    EXPECT_EQ(out_sum.load(), expected_value);
}