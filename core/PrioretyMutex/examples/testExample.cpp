#include "Priotex.h"
#include <iostream>

int main()
{
    mutex::Priotex p(1000);
    std::cout << "Priotex example" << std::endl;

    return 0;
}