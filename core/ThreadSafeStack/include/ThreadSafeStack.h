#pragma once
#include <stack>
#include <mutex>
#include <memory>
#include <exception>

namespace SimpleStructs
{
class EmptyStack : std::exception {
    const char* what() const throw();
};

template<typename T>
class ThreadSafeStack
{
public:
    explicit ThreadSafeStack() = default;

    ThreadSafeStack(const ThreadSafeStack& other)
    {
        std::lock_guard lock(other.mtx);
        main_stack = other.main_stack;
    }
    ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

    void push(T new_value);

    std::shared_ptr<T> pop();
    void pop(T& value);

    size_t empty() const;
private:
    std::stack<std::shared_ptr<T>> main_stack;
    std::mutex mtx;
};

template <typename T>
void ThreadSafeStack<T>::push(T new_value)
{
    auto new_value_ptr = std::make_shared<T>(std::move(new_value));
    std::lock_guard lock(mtx);
    main_stack.push(new_value_ptr);
}

template <typename T>
std::shared_ptr<T> ThreadSafeStack<T>::pop()
{
    std::lock_guard lock(mtx);
    if (main_stack.empty()) throw EmptyStack();

    auto outValue = main_stack.top();
    main_stack.pop();
    return outValue;
}

template <typename T>
void ThreadSafeStack<T>::pop(T& value)
{
    std::lock_guard lock(mtx);
    if (main_stack.empty()) throw EmptyStack();
    
    value = *main_stack.top();
    main_stack.pop();
}

template <typename T>
size_t ThreadSafeStack<T>::empty() const 
{
    std::lock_guard lock();
    return main_stack.empty();
}
}