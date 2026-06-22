#include <iostream>
#include <string>
#include "MHope/Singleton.h"


class Some : public hope::Singleton<Some> 
{
public:
    Some(token) {};

    std::string HelloText() const { return "Hello World!!!"; }
};

int main()
{
    auto&& some = Some::Instance();
    std::cout << some.HelloText() << std::endl;
    
    return 0;
}