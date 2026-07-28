#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

namespace ThreadSafeStructs
{
template <typename T>
class SimpleThreadSafeQueue
{
public:
    SimpleThreadSafeQueue() = default;
    SimpleThreadSafeQueue(const SimpleThreadSafeQueue& other) 
    {
        std::lock_guard lock(other.mtx);
        main_queue = other.main_queue;
    }
    SimpleThreadSafeQueue& operator=(const SimpleThreadSafeQueue&) = delete;

    void push(T value);

    std::shared_ptr<T> try_pop();
    bool try_pop(T& value);

    std::shared_ptr<T> wait_pop();
    void wait_pop(T& value);

    bool empty() const;
    
private:
    std::queue<std::shared_ptr<T>> main_queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
};

template <typename T>
void SimpleThreadSafeQueue<T>::push(T value)
{
    auto value_ptr = std::make_shared<T>(std::move(value));
    std::lock_guard lock(mtx);
    main_queue.push(value_ptr);
    cv.notify_one();
}

template <typename T>
std::shared_ptr<T> SimpleThreadSafeQueue<T>::try_pop()
{
    std::lock_guard lock(mtx);
    if (main_queue.empty())
        return nullptr;

    auto return_value = main_queue.front();
    main_queue.pop();
    return return_value;
}
template <typename T>
bool SimpleThreadSafeQueue<T>::try_pop(T& value)
{
    std::lock_guard lock(mtx);
    if (main_queue.empty())
        return false;

    value = std::move(*main_queue.front());
    main_queue.pop();
    return true;
}

template <typename T>
std::shared_ptr<T> SimpleThreadSafeQueue<T>::wait_pop()
{
    std::unique_lock lock(mtx);
    cv.wait(lock, [&] { return !main_queue.empty(); });
    auto return_value = main_queue.front();
    main_queue.pop();
    return return_value;
}
template <typename T>
void SimpleThreadSafeQueue<T>::wait_pop(T& value)
{
    std::unique_lock lock(mtx);
    cv.wait(lock, [&] { return !main_queue.empty(); });
    
    value = std::move(*main_queue.front());
    main_queue.pop();
}
template <typename T>
bool SimpleThreadSafeQueue<T>::empty() const
{
    std::lock_guard lock(mtx);
    return main_queue.empty();
}
}