#include "SimpleThreadSafeStack.h"

namespace ThreadSafeStructs
{
    const char* EmptyStack::what() const {
        return "Stack is empty\n";
    }
}