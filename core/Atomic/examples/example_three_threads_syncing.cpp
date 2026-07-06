#include <thread>
#include <atomic>
#include <iostream>
#include <assert.h>

int data[5];
std::atomic<int> sync = 0;

void first_thread_writter()
{
    data[0] = 0;
    data[1] = 10;
    data[2] = 200;
    data[3] = 3000;
    data[4] = 40000;

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
}

int main()
{
    std::thread tRead(third_thread_reader);
    std::thread tSyncer(second_thread_across_syncing);
    std::thread tWriter(first_thread_writter);

    tRead.join();
    tSyncer.join();
    tWriter.join();

    std::cout << "All rights!" << std::endl;
}