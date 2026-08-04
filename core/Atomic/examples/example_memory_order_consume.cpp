#include <thread>
#include <atomic>
#include <string>
#include <print>
#include <cassert>

namespace
{
struct X
{
    int i{};
    std::string s;
};

std::atomic<int> a = 0;
std::atomic<X*> x_ptr = nullptr;

void create_x_and_write_a()
{
    X* x = new X{10, "200s"};
    a.store(3000, std::memory_order_relaxed);
    x_ptr.store(x, std::memory_order_release);
    x_ptr.notify_one();
}

void read_x_and_a()
{
    x_ptr.wait(nullptr, std::memory_order_consume);
    X* x = x_ptr.load(std::memory_order_consume);

    int local_a = a.load(std::memory_order_relaxed);

    assert(x->i == 10);
    assert(x->s == "200s");

    if (local_a == 3000)
        std::println("The 'a' lucky reading");
    else 
        std::println("The 'a' data race. out value: {}", local_a);

    delete x;
}
}

int main()
{
    std::jthread t_read(read_x_and_a);
    std::jthread t_write(create_x_and_write_a);
}