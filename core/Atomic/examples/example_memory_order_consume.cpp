#include <thread>
#include <chrono>
#include <atomic>
#include <string>
#include <iostream>
#include <assert.h>

struct X
{
    int i;
    std::string s;
};

std::atomic<int> a = 0;
std::atomic<X*> x_ptr = nullptr;

void create_x_and_write_a()
{
    X* x = new X;
    x->i = 10;
    x->s = "200s";

    a.store(3000, std::memory_order_relaxed);
    x_ptr.store(x, std::memory_order_release);
}

void read_x_and_a()
{
    X* x = nullptr;
    while ((x = x_ptr.load(std::memory_order_consume)) == nullptr)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    int local_a = a.load(std::memory_order_relaxed);

    assert(x->i == 10);
    assert(x->s == "200s");

    if (local_a == 3000)
        std::cout << "The 'a' lucky reading" << std::endl;
    else 
        std::cout << "The 'a' data race. out value: " << local_a << std::endl;
}

int main()
{
    std::jthread tRead(read_x_and_a);
    std::jthread tWrite(create_x_and_write_a);
}