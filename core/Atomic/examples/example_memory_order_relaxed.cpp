#include <thread>
#include <atomic>
#include <array>
#include <latch>
#include <vector>
#include <print>
#include <ranges>

namespace
{
constexpr size_t    threads_count = 5;
constexpr size_t    loop_count = 10;

std::latch start(threads_count);

std::atomic<int> x(0), y(0), z(0);

struct read_values 
{
    int x{}, y{}, z{};
};
using values_array = std::array<read_values, loop_count>;

void increment(std::atomic<int>& var_to_inc, values_array& values)
{
    start.arrive_and_wait();

    for (auto i : std::views::iota(0uz, loop_count))
    {
        values[i].x = ::x.load(std::memory_order_relaxed);
        values[i].y = ::y.load(std::memory_order_relaxed);
        values[i].z = ::z.load(std::memory_order_relaxed);
        var_to_inc.store(static_cast<int>(i) + 1, std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

void read_vals(values_array& values)
{
    start.arrive_and_wait();

    for (auto i : std::views::iota(0uz, loop_count))
    {
        values[i].x = ::x.load(std::memory_order_relaxed);
        values[i].y = ::y.load(std::memory_order_relaxed);
        values[i].z = ::z.load(std::memory_order_relaxed);
        std::this_thread::yield();
    }
}

void print(const values_array& v)
{
    for (auto i : std::views::iota(0uz, loop_count))
    {
        if (i != 0uz) std::print(",");

        std::print("({},{},{})", v[i].x, v[i].y, v[i].z);
    }
    std::println("");
}
}

int main()
{
    std::array<values_array, threads_count> all_values{};

    std::vector<std::jthread> pool;

    pool.emplace_back(increment, std::ref(x), std::ref(all_values[0]));
    pool.emplace_back(increment, std::ref(y), std::ref(all_values[1]));
    pool.emplace_back(increment, std::ref(z), std::ref(all_values[2]));
    pool.emplace_back(read_vals, std::ref(all_values[3]));
    pool.emplace_back(read_vals, std::ref(all_values[4]));

    pool.clear();

    for (const auto& arr : all_values)
        ::print(arr);
}
