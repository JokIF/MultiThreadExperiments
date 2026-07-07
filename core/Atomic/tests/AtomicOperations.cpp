#include "gtest/gtest.h"
#include <functional>
#include <thread>
#include <chrono>

TEST(AtomicTest, AtomicLoading)
{
    std::atomic<int> readValue = 10;
    std::atomic<bool> go = false;

    auto reading = [&go, &readValue] {
        while (!go.load()) std::this_thread::yield();

        while (go.load())
            EXPECT_EQ(readValue.load(std::memory_order_relaxed), 10); 
    };

    constexpr size_t thread_count = 10;
    std::jthread threads[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
        threads[i] = std::jthread(reading); 
    }

    go.store(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    go.store(false);
}

TEST(AtomicTest, AtomicStoring)
{
    std::atomic<long> readValue = 0;
    std::atomic<bool> go = false;

    auto writing = [&go, &readValue] (unsigned times)
    {
        while (!go.load()) std::this_thread::yield();

        for (unsigned i = 0; i < times; i++)
        {
            readValue.fetch_add(1, std::memory_order_acq_rel);
            std::this_thread::yield();
        }
    };

    constexpr size_t thread_count = 20;
    constexpr unsigned write_iters = 1000;

    std::jthread threads[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
        threads[i] = std::jthread(writing, write_iters); 
    }
    
    go.store(true);

    for (size_t i = 0; i < thread_count; i++) {
        if (threads[i].joinable()) threads[i].join(); 
    }

    EXPECT_EQ(readValue.load(), thread_count * write_iters);
}

TEST(AtomicTest, AtomicExchange)
{
    std::atomic<int> sync_flag = 0;
    int success_count = 0;

    auto exchangeLocking = [&sync_flag, &success_count]
    {
        int old_flag = sync_flag.exchange(1);
        if (old_flag == 0)
            success_count++;
    };

    
    constexpr size_t thread_count = 10;
    std::thread threads[thread_count];

    for (size_t i = 0; i < thread_count; i++)
        threads[i] = std::thread(exchangeLocking);

    for (size_t i = 0; i < thread_count; i++)
        if (threads[i].joinable()) 
            threads[i].join();

    EXPECT_EQ(success_count, 1);
}

TEST(AtomicTest, AtomicExchangeWeak)
{
    std::atomic<int> counter = 0;
    int thread_loop_iter_count = 100;
    auto exchange_weak_write = [&counter, thread_loop_iter_count]
    {
        for (size_t i = 0; i < thread_loop_iter_count; i++)
        {
            int expected;
            do {
                expected = counter.load(std::memory_order_relaxed);
            } while (!counter.compare_exchange_weak(expected, expected + 1, std::memory_order_relaxed));
        }
    };

    constexpr size_t thread_count = 10;
    std::thread threads[thread_count];

    for (size_t i = 0; i < thread_count; i++)
        threads[i] = std::thread(exchange_weak_write);

    for (size_t i = 0; i < thread_count; i++)
        if (threads[i].joinable()) 
            threads[i].join();

    EXPECT_EQ(counter.load(), thread_count * thread_loop_iter_count);
}

TEST(AtomicTest, AtomicExchangeStrong)
{
    std::atomic<int> sync_value = 0;
    int success_count = 0;

    auto strong_once_exchange = [&sync_value, &success_count]
    {
        int expected = 0;
        int desired = 10;

        if (sync_value.compare_exchange_strong(expected, desired))
            success_count++;
    };

    constexpr size_t thread_count = 10;
    std::thread threads[10];

    for (size_t i = 0; i < thread_count; i++)
        threads[i] = std::thread(strong_once_exchange);

    for (size_t i = 0; i < thread_count; i++)
        if (threads[i].joinable()) 
            threads[i].join();

    EXPECT_EQ(success_count, 1);
    EXPECT_EQ(sync_value.load(), 10);
}
