#include "gtest/gtest.h"
#include <atomic>
#include <thread>
#include <latch>
#include <chrono>
#include <vector>
#include <ranges>
#include <functional>
#include <algorithm>

TEST(AtomicTest, AtomicLoading)
{
    std::atomic<int> readValue = 10;
    constexpr size_t thread_count = 10;

    auto reading = [] (std::stop_token st, const std::atomic<int>& val) {
        while (!st.stop_requested())
            EXPECT_EQ(val.load(std::memory_order_relaxed), 10);
    };

    auto threads =  std::views::iota(0uz, thread_count)
                |   std::views::transform([&](auto) {
                        return std::jthread(reading, std::cref(readValue));
                    })
                |   std::ranges::to<std::vector>();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    std::ranges::for_each(threads, std::mem_fn(&std::jthread::request_stop));
}

TEST(AtomicTest, AtomicStoring)
{
    std::atomic<long> readValue = 0;
    constexpr size_t thread_count = 20;
    constexpr unsigned write_iters = 1000;
    std::latch start(thread_count);

    auto writing = [&start, write_iters] (std::atomic_long& val)
    {
        start.arrive_and_wait();
        for (auto _ : std::views::iota(0u, write_iters))
        {
            (void)_;
            val.fetch_add(1, std::memory_order_acq_rel);
            std::this_thread::yield();
        }
    };

    auto threads =  std::views::iota(0uz, thread_count)
                |   std::views::transform([&](auto) {
                        return std::jthread(writing, std::ref(readValue));
                    })
                |   std::ranges::to<std::vector>();

    threads.clear();

    EXPECT_EQ(readValue.load(), thread_count * write_iters);
}

TEST(AtomicTest, AtomicExchange)
{
    constexpr size_t thread_count = 10;
    std::atomic<int> sync_flag = 0;
    int success_count = 0;
    std::latch start(thread_count);

    auto exchangeLocking = [&start, &sync_flag, &success_count]
    {
        start.arrive_and_wait();
        if (sync_flag.exchange(1) == 0)
            success_count++;
    };

    auto threads =  std::views::iota(0uz, thread_count)
                |   std::views::transform([&](auto) {
                        return std::jthread(exchangeLocking);
                    })
                |   std::ranges::to<std::vector>();

    threads.clear();

    EXPECT_EQ(success_count, 1);
}

TEST(AtomicTest, AtomicExchangeWeak)
{
    constexpr size_t thread_count = 10;
    std::atomic<int> counter = 0;
    int thread_loop_iter_count = 100;
    std::latch start(thread_count);

    auto exchange_weak_write = [&start, &counter, thread_loop_iter_count]
    {
        start.arrive_and_wait();
        for (auto _ : std::views::iota(0, thread_loop_iter_count))
        {
            (void)_;
            int expected;
            do
                expected = counter.load(std::memory_order_relaxed);
            while (!counter.compare_exchange_weak(expected, expected + 1, std::memory_order_relaxed));
        }
    };

    auto threads =  std::views::iota(0uz, thread_count)
                |   std::views::transform([&](auto) {
                        return std::jthread(exchange_weak_write);
                    })
                |   std::ranges::to<std::vector>();

    threads.clear();

    EXPECT_EQ(counter.load(), thread_count * thread_loop_iter_count);
}

TEST(AtomicTest, AtomicExchangeStrong)
{
    constexpr size_t thread_count = 10;
    std::atomic<int> sync_value = 0;
    int success_count = 0;
    std::latch start(thread_count);


    auto strong_once_exchange = [&start, &sync_value, &success_count]
    {
        start.arrive_and_wait();

        int expected = 0;
        int desired = 10;

        if (sync_value.compare_exchange_strong(expected, desired))
            success_count++;
    };

    auto threads =  std::views::iota(0uz, thread_count)
                |   std::views::transform([&](auto){
                        return std::jthread(strong_once_exchange);
                    })
                |   std::ranges::to<std::vector>();

    threads.clear();

    EXPECT_EQ(success_count, 1);
    EXPECT_EQ(sync_value.load(), 10);
}
