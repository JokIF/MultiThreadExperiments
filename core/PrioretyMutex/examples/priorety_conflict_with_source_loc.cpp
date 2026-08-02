#include "Priotex.h"
#include <print>

int main()
{
    mutex::Priotex p_first(2);
    mutex::Priotex p_second(1);

    p_second.lock();
    try
    {
        p_first.lock();
    }
    catch(const std::logic_error& e)
    {
        std::println("Caught exception: {}", e.what());
    }

    return 0;
}