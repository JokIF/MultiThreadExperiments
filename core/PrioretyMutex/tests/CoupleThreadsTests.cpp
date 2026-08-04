#include "Priotex.h"
#include "PriotexTestUtils.h"
#include "gtest/gtest.h"
#include <thread>
#include <future>
#include <chrono>
#include <latch>
#include <atomic>
#include <ranges>

using namespace std::chrono_literals;

TEST(PriotexTest, CorrectMultiThreadUsing)
{
    long long unsigned int overall = 0;
    mutex::Priotex deepest_ptx(10);
        

    auto add = [&](unsigned int value)
    {
        std::lock_guard lock(deepest_ptx);
        overall += value;
        return overall;
    };

    auto get = [&]
    {
        std::lock_guard lock(deepest_ptx);
        return overall;
    };

    auto hard_work = [&] (std::chrono::milliseconds timeout)
    {
        for (auto _ : std::views::iota(0, 10))
        {
            (void)_;
            add(1);
            std::this_thread::sleep_for(1ms);
        }

        std::this_thread::sleep_for(timeout);
    };

    mutex::Priotex outer_ptx(100);
    constexpr int main_loop_iter = 5;
    constexpr size_t main_loop_count = 3;
    std::latch start(main_loop_count);

    auto main_loop = [&] (std::chrono::milliseconds timeout)
    {
        start.arrive_and_wait();

        for (auto _ : std::views::iota(0, main_loop_iter))
        {
            (void)_;
            // start hard work
            hard_work(timeout);
            //end hard owrk

            std::lock_guard lock(outer_ptx);

            // work with other shared memory space
            [[maybe_unused]] auto&& to_add = get();
            // end work with other shared memory space
        }
    };

    std::vector<std::jthread> thread_pool;
    for (auto delay : { 13ms, 27ms, 6ms })
        thread_pool.emplace_back(main_loop, delay);

    thread_pool.clear();

    long long unsigned int expected = main_loop_iter * main_loop_count * 10; // 10 - sum value per iter
    EXPECT_EQ(expected, overall);
}

TEST(PriotexTest, TryLockUnderContention)
{
    constexpr size_t worker_count = 5;
    constexpr int work_iter = 100;

    mutex::Priotex shared_resource(10);
    std::latch start(worker_count + 1); // workers + one holder
    std::atomic holding = true;

    std::atomic success_counter = 0u;
    std::atomic failer_counter = 0u;

    auto worker = [&]
    {
        start.arrive_and_wait();

        for (auto _ : std::views::iota(0, work_iter))
        {
            (void)_;
            if (shared_resource.try_lock())
            {
                success_counter.fetch_add(1, std::memory_order_relaxed);
                shared_resource.unlock();
            }
            else
                failer_counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::jthread holder([&] {
        std::lock_guard lock(shared_resource);
        start.arrive_and_wait();
        holding.wait(true, std::memory_order_relaxed);
    });

    auto workers =  std::views::iota(0uz, worker_count)
                |   std::views::transform([&](auto) {
                        return std::jthread(worker);
                    })
                |   std::ranges::to<std::vector>();

    workers.clear();

    EXPECT_EQ(success_counter.load(), 0u);
    EXPECT_EQ(failer_counter.load(), work_iter * worker_count);

    holding.store(false, std::memory_order_relaxed);
    holding.notify_one();
    holder.join();

    EXPECT_TRUE(shared_resource.try_lock());
    shared_resource.unlock();
}