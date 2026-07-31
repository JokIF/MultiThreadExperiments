#include "gtest/gtest.h"
#include "NonThreadSafeQueue.h"

TEST(NonThreadSafeQueue, MethodsTest)
{
    NonThreadSafeStructs::NonThreadSafeQueue<int> queue;
    EXPECT_TRUE(queue.empty());
    queue.push(10);
    queue.push(200);
    queue.push(3000);
    EXPECT_FALSE(queue.empty());

    auto first_value = queue.pop();
    EXPECT_TRUE(first_value.has_value());
    EXPECT_EQ(*first_value, 10);

    EXPECT_EQ(*queue.try_pop(), 200);
    EXPECT_EQ(*queue.pop(), 3000);

    auto second_value = queue.pop();
    EXPECT_FALSE(second_value.has_value());
    EXPECT_EQ(second_value.error(), NonThreadSafeStructs::QueueError::Empty);

    EXPECT_EQ(queue.try_pop(), nullptr);
    EXPECT_TRUE(queue.empty());
}