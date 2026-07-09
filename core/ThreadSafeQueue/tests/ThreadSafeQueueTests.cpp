#include "gtest/gtest.h"
#include "ThreadSafeQueue.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <future>

TEST(ThreadSafeQueue, SingleThreadMethodsTest)
{
    ThreadSafeStructs::ThreadSafeQueue<int> queue;
    
    EXPECT_TRUE(queue.empty());
    queue.push(10);
    queue.push(200);
    queue.push(3000);
    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(*queue.try_pop(), 10);
    EXPECT_EQ(*queue.try_pop(), 200);
    EXPECT_EQ(*queue.try_pop(), 3000);
    
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.try_pop(), nullptr);
    auto f = std::async([&]
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        queue.push(40000);
    });
    EXPECT_EQ(*queue.wait_pop(), 40000);
}

TEST(ThreadSafeQueue, MultiThreadTest)
{
    ThreadSafeStructs::ThreadSafeQueue<unsigned> queue;
    unsigned write_iter_count = 1000u; 
    
    std::atomic<bool> go = false;
    std::atomic<unsigned> out_sum = 0;
    int writers_count = 3;
    std::atomic<int> writers_ended = 0;

    auto WriteQueue = [&go, &queue, &writers_ended, write_iter_count] 
    {
        while (!go.load()) std::this_thread::yield();

        for (unsigned i = 0; i < write_iter_count; i++)
            queue.push(i);

        writers_ended++;
    };

    auto ReadQueue = [&go, &queue, &writers_ended, writers_count, &out_sum] 
    {
        while (!go.load()) std::this_thread::yield();

        unsigned inner_sum = 0;
        while (writers_ended.load() != writers_count || !queue.empty())
            if (auto queue_value = queue.try_pop(); queue_value != nullptr)
                inner_sum += *queue_value;
            else
                std::this_thread::yield();

        out_sum.fetch_add(inner_sum);
    };

    std::vector<std::thread> writers_array(writers_count);
    std::vector<std::thread> readers_array(2);

    for (auto& writer : writers_array) {
        writer = std::thread(WriteQueue);
    }

    for (auto& reader : readers_array) {
        reader = std::thread(ReadQueue);
    }

    go = true;

    for (auto& writer : writers_array) {
        writer.join();
    }

    for (auto& reader : readers_array) {
        reader.join();
    }

    unsigned expected_value = static_cast<unsigned>((write_iter_count * (write_iter_count - 1)) / 2) * writers_count; 
    EXPECT_EQ(out_sum.load(), expected_value);
}