#pragma once
#include <stack>
#include <mutex>
#include <memory>
#include <exception>

namespace ThreadSafeStructs
{
class EmptyStack : std::exception {
    const char* what() const noexcept override;
};

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

    std::shared_ptr<T> pop();
    void pop(T& value);

    size_t empty() const;
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
std::shared_ptr<T> SimpleThreadSafeStack<T>::pop()
{
    std::lock_guard lock(mtx);
    if (main_stack.empty()) throw EmptyStack();

    auto outValue = std::make_shared<T>(std::move(main_stack.top()));
    main_stack.pop();
    return outValue;
}

template <typename T>
void SimpleThreadSafeStack<T>::pop(T& value)
{
    std::lock_guard lock(mtx);
    if (main_stack.empty()) throw EmptyStack();
    
    value = std::move(main_stack.top());
    main_stack.pop();
}

template <typename T>
size_t SimpleThreadSafeStack<T>::empty() const 
{
    std::lock_guard lock(mtx);
    return main_stack.empty();
}
}