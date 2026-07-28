#include "gtest/gtest.h"
#include "SimpleThreadSafeMap.h"
#include <thread>
#include <chrono>
#include <string>

using namespace std::chrono_literals;

TEST(SimpleThreadSafeMap, SingleThreadMethodsTest)
{
    ThreadSafeStructs::SimpleThreadSafeMap<std::string, int> m;
    int value_empty = 0;
    m.set_or_update_value_for("first", 10);
    m.set_or_update_value_for("second", 200);
    m.set_or_update_value_for("third", 3000);
    EXPECT_EQ(m.value_for("first"), 10);
    EXPECT_EQ(m.value_for("second"), 200);
    EXPECT_EQ(m.value_for("third"), 3000);
    m.set_or_update_value_for("first", 40000);
    EXPECT_EQ(m.value_for("first"), 40000);
    EXPECT_EQ(m.remove_value_for("second"), 1);
    EXPECT_EQ(m.remove_value_for("second"), 0);
    EXPECT_EQ(m.value_for("second", value_empty), value_empty);
}

constexpr int calculate_expected_value(int iter_count)
{
    return static_cast<int>((iter_count * (iter_count + 1)) / 2); 
}

TEST(SimpleThreadSafeMap, MultiThreadTest)
{
    ThreadSafeStructs::SimpleThreadSafeMap<std::string, int> threadSafeMap;
    
    auto addPair = [&threadSafeMap](std::string name, int value, std::chrono::milliseconds duration_time)
    {
        std::this_thread::sleep_for(duration_time);
        threadSafeMap.set_or_update_value_for(name, value);
    };

    auto readAndDecrease = [&threadSafeMap](std::string name, int& out)
    {
        for (;;) 
        {
            int value = threadSafeMap.value_for(name, -1);
            if (value == -1)
                std::this_thread::yield();

            else if (value == 0)
                break;

            else
            {
                out += value;
                threadSafeMap.set_or_update_value_for(name, value - 1);
            }
        }
    };

    int first_out = 0;
    int second_out = 0;
    int third_out = 0;
    
    {
        std::jthread reader_first(readAndDecrease, "first", std::ref(first_out));
        std::jthread reader_second(readAndDecrease, "second", std::ref(second_out));
        std::jthread reader_third(readAndDecrease, "third", std::ref(third_out));

        std::jthread writter_first(addPair, "first", 1000, 100ms);
        std::jthread writter_second(addPair, "second", 500, 300ms);
        std::jthread writter_third(addPair, "third", 200, 500ms);
    }

    constexpr int first_expected = calculate_expected_value(1000);
    constexpr int second_expected = calculate_expected_value(500);
    constexpr int third_expected = calculate_expected_value(200);

    EXPECT_EQ(first_out, first_expected);
    EXPECT_EQ(second_out, second_expected);
    EXPECT_EQ(third_out, third_expected);
}