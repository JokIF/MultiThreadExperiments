#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <print>
#include <format>
#include <array>
#include <ranges>

namespace
{
using queue_type = unsigned long int;

std::array<queue_type, 1000> gl_queue;
std::atomic<long> len_queue = 0;
std::string out_string;
std::atomic<bool> need_break = false;
std::mutex out_mtx;

void process_thread_data(const std::string& thread_name, queue_type queue_data)
{
    auto thread_data =  std::format("{} >> {}\n", thread_name, queue_data);
    std::lock_guard lock(out_mtx);
    out_string += thread_data;
}

void populate_queue()
{
    for (auto i : std::views::iota(0uz, gl_queue.size()))
        gl_queue[i] = static_cast<queue_type>(i);

    len_queue.store(static_cast<long>(gl_queue.size()), std::memory_order_release);
}

void read_queue(const std::string& thread_name)
{
    long index_queue;
    while (true)
    {
        index_queue = len_queue.fetch_sub(1, std::memory_order_acquire);
        if (index_queue <= 0)
        {
            if (index_queue < 0)
                len_queue.store(0, std::memory_order_relaxed);

            if (need_break.load())
                break;

            std::this_thread::yield();
            continue;
        }

        process_thread_data(thread_name, gl_queue[static_cast<size_t>(index_queue) - 1]);
    }
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

    std::println("out_string: {}", out_string);
}
