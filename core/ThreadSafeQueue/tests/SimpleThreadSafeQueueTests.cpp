#include "gtest/gtest.h"
#include "SimpleThreadSafeQueue.h"

TEST(SimpleThreadSafeQueue, SingleThreadMethodsTest)
{
    ThreadSafeStructs::SimpleThreadSafeQueue<int> queue;
    EXPECT_TRUE(queue.empty());
    queue.push(10);
    queue.push(200);
    EXPECT_EQ(*queue.try_pop(), 10);
    
    int second_value = 0;
    EXPECT_TRUE(queue.try_pop(second_value));
    EXPECT_EQ(second_value, 200);
}