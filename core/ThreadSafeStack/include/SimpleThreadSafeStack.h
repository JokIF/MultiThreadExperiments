#pragma once
#include <stack>
#include <mutex>
#include <memory>
#include <expected>

namespace ThreadSafeStructs
{
enum class StackError { Empty };

template<typename T>
class SimpleThreadSafeStack
{
public:
    explicit SimpleThreadSafeStack() = default;

    SimpleThreadSafeStack(const SimpleThreadSafeStack& other)
    {
        std::lock_guard lock(other.mtx);
        main_stack = other.main_stack;
    }
    SimpleThreadSafeStack& operator=(const SimpleThreadSafeStack&) = delete;

    void push(T new_value);

    std::expected<T, StackError>    try_pop();
    bool    try_pop(T& value);

    bool    empty() const;

private:
    std::stack<T> main_stack;
    mutable std::mutex mtx;
};

template <typename T>
void SimpleThreadSafeStack<T>::push(T new_value)
{
    std::lock_guard lock(mtx);
    main_stack.push(std::move(new_value));
}

template <typename T>
std::expected<T, StackError> SimpleThreadSafeStack<T>::try_pop()
{
    std::lock_guard lock(mtx);
    if (main_stack.empty())
        return std::unexpected(StackError::Empty);

    T outValue = std::move(main_stack.top());
    main_stack.pop();
    return outValue;
}

template <typename T>
bool SimpleThreadSafeStack<T>::try_pop(T& value)
{
    std::lock_guard lock(mtx);
    if (main_stack.empty())
        return false;
    
    value = std::move(main_stack.top());
    main_stack.pop();
    return true;
}

template <typename T>
bool SimpleThreadSafeStack<T>::empty() const
{
    std::lock_guard lock(mtx);
    return main_stack.empty();
}
}