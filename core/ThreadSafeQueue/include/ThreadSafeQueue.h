#pragma once
#include <mutex>
#include <condition_variable>
#include <memory>

namespace ThreadSafeStructs
{
template <typename T>
class ThreadSafeQueue
{
    struct Node {
        Node() = default;

        std::shared_ptr<T> value = nullptr;
        std::unique_ptr<Node> next_node = nullptr;
    };

public:
    ThreadSafeQueue() 
        : head_node(std::make_unique<Node>()), tail_node(head_node.get()) {}

    ThreadSafeQueue(ThreadSafeQueue&&) = delete;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) = delete;

    void    push(T value);

    bool    try_pop(T& value);
    void    wait_pop(T& value);

    std::shared_ptr<T>  try_pop();
    std::shared_ptr<T>  wait_pop();

    bool    empty() const;

private:
    bool    empty_locked_head(const std::unique_lock<std::mutex>& head_lock);
    void    push_tail(std::shared_ptr<T> new_data);

    std::unique_ptr<Node>           pop_head();

    std::unique_lock<std::mutex>    wait_data();

    std::unique_ptr<Node>   head_node;
    Node*                   tail_node;

    mutable std::mutex      head_mtx;
    mutable std::mutex      tail_mtx;

    std::condition_variable cv;
};

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::scoped_lock lock(head_mtx, tail_mtx);
    return head_node.get() == tail_node;
}

template <typename T>
bool ThreadSafeQueue<T>::empty_locked_head([[maybe_unused]] const std::unique_lock<std::mutex>& head_lock)
{
    std::lock_guard tail_lock(tail_mtx);
    return head_node.get() == tail_node;
}
template <typename T>
std::unique_ptr<typename ThreadSafeQueue<T>::Node> ThreadSafeQueue<T>::pop_head()
{
    auto old_head_node = std::move(head_node);
    head_node = std::move(old_head_node->next_node);
    return old_head_node;
}

template <typename T>
void ThreadSafeQueue<T>::push_tail(const std::shared_ptr<T> new_data)
{
    auto new_node = std::make_unique<Node>();
    Node* new_tail = new_node.get();

    std::lock_guard lock(tail_mtx);

    tail_node->value = new_data;
    tail_node->next_node = std::move(new_node);
    tail_node = new_tail;
}

template <typename T>
std::unique_lock<std::mutex> ThreadSafeQueue<T>::wait_data()
{
    std::unique_lock lock(head_mtx);
    cv.wait(lock, [&] { return !empty_locked_head(lock); });
    return lock;
}

template <typename T>
void ThreadSafeQueue<T>::push(T value)
{
    const auto new_data = std::make_shared<T>(std::move(value));
    push_tail(new_data);
    cv.notify_one();
}

template <typename T>
bool ThreadSafeQueue<T>::try_pop(T& value)
{
    std::unique_lock head_lock(head_mtx);
    if (empty_locked_head(head_lock))
        return false;

    value = std::move(*pop_head()->value);
    return true;
}

template <typename T>
void ThreadSafeQueue<T>::wait_pop(T& value)
{
    std::unique_lock head_lock(wait_data());
    value = std::move(*pop_head()->value);
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::try_pop()
{
    std::unique_lock head_lock(head_mtx);
    if (empty_locked_head(head_lock))
        return nullptr;

    return pop_head()->value;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::wait_pop()
{
    std::unique_lock<std::mutex> lock(wait_data());
    auto old_head = pop_head();
    return old_head->value;
}
}