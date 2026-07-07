#include "ThreadSafeStack.h"

namespace SimpleStructs
{
    const char* EmptyStack::what() const {
        return "Stack is empty\n";
    }
}