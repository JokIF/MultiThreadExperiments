#include <thread>
#include <atomic>
#include <print>
#include <cassert>

bool x = false;
std::atomic<bool> y = false;
std::atomic<int> z = 0;

void write_x_then_y()
{
    x = true;
    std::atomic_thread_fence(std::memory_order_release);
    y.store(true, std::memory_order_relaxed);
    y.notify_one();
}

void read_y_then_x()
{
    y.wait(false, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (x) z++;
}

int main()
{
    std::thread t_read(read_y_then_x);
    std::thread t_write(write_x_then_y);

    t_read.join();
    t_write.join();

    assert(z.load() == 1);

    std::println("All right!");
}