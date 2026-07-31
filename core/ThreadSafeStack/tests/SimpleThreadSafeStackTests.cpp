#include <thread>
#include <atomic>
#include <latch>
#include <ranges>

#include "SimpleThreadSafeStack.h"
#include "gtest/gtest.h"

TEST(SimpleThreadSafeStack, SingleThreadCorrectUsingMethodsTest)
{
    ThreadSafeStructs::SimpleThreadSafeStack<int> firstStack;

    EXPECT_TRUE(firstStack.empty());
    
    firstStack.push(10);
    firstStack.push(200);

    EXPECT_FALSE(firstStack.empty());

    auto first_value = firstStack.try_pop();
    EXPECT_TRUE(first_value.has_value());
    EXPECT_EQ(*first_value, 200);

    ThreadSafeStructs::SimpleThreadSafeStack<int> secondStack(firstStack);

    int stack_value = 0;
    EXPECT_TRUE(secondStack.try_pop(stack_value));
    EXPECT_EQ(stack_value, 10);

    EXPECT_TRUE(secondStack.empty());
    EXPECT_FALSE(firstStack.empty());
}

TEST(SimpleThreadSafeStack, EmptyStackPopTest)
{
    ThreadSafeStructs::SimpleThreadSafeStack<int> stack;

    ASSERT_TRUE(stack.empty());

    stack.push(10);
    (void)stack.try_pop();

    ASSERT_TRUE(stack.empty());
    auto value = stack.try_pop();
    EXPECT_FALSE(value.has_value());
    EXPECT_EQ(value.error(), ThreadSafeStructs::StackError::Empty);
    EXPECT_TRUE(stack.empty());
}

TEST(SimpleThreadSafeStack, ThrowableCopyingTest)
{
    struct ThrowOnCopy
    {
        int someValue = 0;

        ThrowOnCopy() = default;
        ThrowOnCopy(int someValue) : someValue(someValue) {}

        ThrowOnCopy(const ThrowOnCopy&) { throw std::logic_error("copy denied"); }
        ThrowOnCopy& operator=(const ThrowOnCopy&) { throw std::logic_error("copy denied"); }
        
        ThrowOnCopy(ThrowOnCopy&&) noexcept = default;
        ThrowOnCopy& operator=(ThrowOnCopy&&) noexcept = default;
    };

    ThreadSafeStructs::SimpleThreadSafeStack<ThrowOnCopy> stack;
    
    EXPECT_NO_THROW(stack.push(0));
    EXPECT_NO_THROW(stack.push(10));

    std::expected<ThrowOnCopy, ThreadSafeStructs::StackError> firstValue;

    EXPECT_NO_THROW(firstValue = stack.try_pop());
    EXPECT_TRUE(firstValue.has_value());
    EXPECT_EQ(firstValue->someValue, 10);

    ThrowOnCopy secondValue(200);
    EXPECT_NO_THROW(stack.try_pop(secondValue));
    EXPECT_EQ(secondValue.someValue, 0);
    ASSERT_TRUE(stack.empty());
}

TEST(SimpleThreadSafeStack, MultiThreadWorkTest)
{
    ThreadSafeStructs::SimpleThreadSafeStack<int> stack; 
    constexpr unsigned write_iters = 1000;
    constexpr unsigned writer_count = 2;
    std::latch start(writer_count + 1); // writters(2) + reader(1)

    std::atomic writters_done = 0u;
    std::atomic outSum = 0uz;
    
    auto WriteStack = [&start, &writters_done, &stack, write_iters](bool odd)
    {
        start.arrive_and_wait();
        for (auto i :   std::views::iota(0u, write_iters) |
                        std::views::filter([odd](unsigned val) { return (val % 2 == 1) == odd; }))
            stack.push(i);

        writters_done.fetch_add(1, std::memory_order_release);
        writters_done.notify_all();
    };

    auto ReadStack = [&start, &writters_done, &stack, &outSum]
    {
        start.arrive_and_wait();

        while (writters_done.load(std::memory_order_acquire) != 2 || !stack.empty())
            if (auto value = stack.try_pop(); value.has_value())
                outSum.fetch_add(*value, std::memory_order_relaxed);
            else
                writters_done.wait(writer_count, std::memory_order_relaxed); // If there are more readers, the wait could become endless
    };

    std::vector<std::jthread> pool;

    pool.emplace_back(WriteStack, true);
    pool.emplace_back(WriteStack, false);
    pool.emplace_back(ReadStack);

    pool.clear();

    unsigned expectSum = write_iters * (write_iters - 1u) / 2u;
    EXPECT_EQ(expectSum, outSum.load());
}