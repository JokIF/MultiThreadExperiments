#include "SimpleThreadSafeStack.h"

namespace ThreadSafeStructs
{
    const char* EmptyStack::what() const noexcept {
        return "Stack is empty";
    }
}