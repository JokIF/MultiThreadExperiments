#include <thread>
#include <atomic>
#include <print>
#include <cassert>
#include <array>

std::array<int, 5> data;
std::atomic<int> sync = 0;

void first_thread_writter()
{
    data = { 0, 10, 200, 3000, 40000};
    sync.store(1, std::memory_order_release);
}

void second_thread_across_syncing()
{
    int expected = 1;
    while(!sync.compare_exchange_strong(expected, 2, std::memory_order_acq_rel))
        expected = 1;
}

void third_thread_reader()
{
    while(sync.load(std::memory_order_acquire) < 2);

    assert(data[0] == 0);
    assert(data[1] == 10);
    assert(data[2] == 200);
    assert(data[3] == 3000);
    assert(data[4] == 40000);

    std::println("All rights!");
}

int main()
{
    std::jthread tRead(third_thread_reader);
    std::jthread tSyncer(second_thread_across_syncing);
    std::jthread tWriter(first_thread_writter);
}