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
    EXPECT_EQ(queue.pop(), 10);
    EXPECT_EQ(queue.pop(), 200);
    EXPECT_EQ(queue.pop(), 3000);
    EXPECT_THROW(queue.pop(), NonThreadSafeStructs::EmptyQueue);
    EXPECT_TRUE(queue.empty());
}