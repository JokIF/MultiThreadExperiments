#include <thread>
#include <atomic>
#include <iostream>
#include <assert.h>

bool x = 0;
std::atomic<bool> y = 0;
std::atomic<int> z = 0;

void write_x_then_y()
{
    x = true;
    std::atomic_thread_fence(std::memory_order_release);
    y.store(true, std::memory_order_relaxed);
}

void read_y_then_x()
{
    while (!y.load(std::memory_order_relaxed));
    std::atomic_thread_fence(std::memory_order_acquire);
    if (x)
        z++;
}

int main()
{
    std::thread tRead(read_y_then_x);
    std::thread tWrite(write_x_then_y);

    tRead.join();
    tWrite.join();

    assert(z.load() == 1);

    std::cout << "All right!" << std::endl;
}