#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <iostream>

int gl_queue[1000];
std::atomic<int> len_queue = 0;
std::string outString;
std::atomic<bool> need_break = false;

void process_thread_data(const std::string& thread_name, int queue_data)
{
    std::string threadData = thread_name + " >> " + std::to_string(queue_data) + "\n";

    static std::mutex mtx;
    std::lock_guard lock(mtx);
    outString += threadData;
}

void populate_queue()
{
    size_t gl_queue_size = sizeof(gl_queue) / sizeof(int);

    for (size_t i = 0; i < gl_queue_size; i++)
        gl_queue[i] = i;

    len_queue.store(gl_queue_size, std::memory_order_release);
}

void read_queue(const std::string& thread_name)
{
    int index_queue;
    while (true)
    {
        if (index_queue = len_queue.fetch_sub(1, std::memory_order_acquire); index_queue <= 0) {
            if (index_queue < -1000000) len_queue.store(0, std::memory_order_relaxed);
            if (need_break.load()) break;
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            continue;
        }

        process_thread_data(thread_name, gl_queue[index_queue-1]);
    }
}

int main()
{
    std::thread t_populate(populate_queue);
    std::thread t_first_read(read_queue, "first");
    std::thread t_second_read(read_queue, "second");

    t_populate.join();

    need_break.store(true);

    t_second_read.join();
    t_first_read.join();

    std::cout << "outString" << std::endl;
    std::cout << outString << std::endl;
}
