#include "gtest/gtest.h"
#include "SimpleThreadSafeMap.h"
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <latch>
#include <atomic>

using namespace std::chrono_literals;

TEST(SimpleThreadSafeMap, SingleThreadMethodsTest)
{
    ThreadSafeStructs::SimpleThreadSafeMap<std::string, int> m;

    m.set_or_update_value("first", 10);
    m.set_or_update_value("second", 200);
    m.set_or_update_value("third", 3000);
    EXPECT_EQ(*m.get_value("first"), 10);
    EXPECT_EQ(*m.get_value("second"), 200);
    EXPECT_EQ(*m.get_value("third"), 3000);
    m.set_or_update_value("first", 40000);
    EXPECT_EQ(*m.get_value("first"), 40000);
    EXPECT_EQ(m.remove_value("second"), 1);
    EXPECT_EQ(m.remove_value("second"), 0);

    auto value = m.get_value("second");
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), ThreadSafeStructs::MapError::NotFound);
}

constexpr int calculate_expected_value(int iter_count)
{
    return static_cast<int>((iter_count * (iter_count + 1)) / 2); 
}

TEST(SimpleThreadSafeMap, MultiThreadTest)
{
    ThreadSafeStructs::SimpleThreadSafeMap<std::string, int> threadSafeMap;
    constexpr int total_threads = 6;
    std::latch start(total_threads);

    auto addPair = [&start, &threadSafeMap](std::string name, int value, std::chrono::milliseconds duration_time)
    {
        start.arrive_and_wait();
        std::this_thread::sleep_for(duration_time);
        threadSafeMap.set_or_update_value(name, value);
    };

    auto readAndDecrease = [&start, &threadSafeMap](std::string name, int& out)
    {
        start.arrive_and_wait();
        for (;;) 
        {
            if (auto value = threadSafeMap.get_value(name); !value.has_value())
                std::this_thread::yield();

            else if (*value == 0)
                break;

            else
            {
                out += *value;
                threadSafeMap.set_or_update_value(name, *value - 1);
            }
        }
    };

    int first_out = 0;
    int second_out = 0;
    int third_out = 0;
    
    std::vector<std::jthread> pool;
    pool.reserve(total_threads);
    pool.emplace_back(readAndDecrease, "first", std::ref(first_out));
    pool.emplace_back(readAndDecrease, "second", std::ref(second_out));
    pool.emplace_back(readAndDecrease, "third", std::ref(third_out));
    pool.emplace_back(addPair, "first", 1000, 100ms);
    pool.emplace_back(addPair, "second", 500, 300ms);
    pool.emplace_back(addPair, "third", 200, 500ms);

    pool.clear();

    EXPECT_EQ(first_out, calculate_expected_value(1000));
    EXPECT_EQ(second_out, calculate_expected_value(500));
    EXPECT_EQ(third_out, calculate_expected_value(200));
}