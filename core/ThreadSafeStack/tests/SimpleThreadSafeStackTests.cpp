#include <thread>
#include <stdexcept>
#include <atomic>

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

        ThrowOnCopy(const ThrowOnCopy&) { throw std::logic_error("It can not copy"); }
        ThrowOnCopy& operator=(const ThrowOnCopy&) { throw std::logic_error("It can not copy"); }
        
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
    constexpr unsigned writeIterCount = 1000;
    unsigned sumFromStack = 0;
    std::atomic<bool> go = false;
    std::atomic<short> write_in_progress = 0;
    
    auto WriteStack = [&go, &write_in_progress, &stack, writeIterCount](bool odd)
    {
        while (!go.load()) std::this_thread::yield();

        for (unsigned i = odd ? 1 : 0; i < writeIterCount; i += 2) {
            stack.push(i);
        }

        write_in_progress.fetch_add(1, std::memory_order_release);
    };

    auto ReadStack = [&go, &write_in_progress, &stack, &sumFromStack]
    {
        while (!go.load()) std::this_thread::yield();

        while (write_in_progress.load(std::memory_order_acquire) != 2 || !stack.empty())
            if (auto value = stack.try_pop(); value.has_value())
                sumFromStack += *value;
            else
                std::this_thread::yield(); 
    };

    std::thread tFirstWrite(WriteStack, true);
    std::thread tSecondWrite(WriteStack, false);
    std::thread tRead(ReadStack);

    go.store(true);

    tFirstWrite.join();
    tSecondWrite.join();
    tRead.join();

    unsigned expectSum = 0;
    for (unsigned i = 0; i < writeIterCount; i++) 
        expectSum += i;

    EXPECT_EQ(expectSum, sumFromStack);
}