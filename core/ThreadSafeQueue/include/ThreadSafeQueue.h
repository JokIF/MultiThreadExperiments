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

    ThreadSafeQueue(ThreadSafeQueue&&) noexcept = default;

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(ThreadSafeQueue&&) noexcept = delete;

    void push(T value);
    std::shared_ptr<T> try_pop();

    bool empty() const;

private:
    bool empty_locked_head(const std::lock_guard<std::mutex>& head_lock) const;

    std::unique_ptr<Node> head_node;
    Node* tail_node;

    mutable std::mutex head_mtx;
    mutable std::mutex tail_mtx;
};

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::scoped_lock lock(head_mtx, tail_mtx);
    return head_node.get() == tail_node;
}

template <typename T>
bool ThreadSafeQueue<T>::empty_locked_head(
    const std::lock_guard<std::mutex>& head_lock) const
{
    std::lock_guard tail_lock(tail_mtx);
    return head_node.get() == tail_node;
}

template <typename T>
void ThreadSafeQueue<T>::push(T value)
{
    const auto new_data = std::make_shared<T>(std::move(value));
    auto new_node = std::make_unique<Node>();
    Node* new_tail = new_node.get();

    std::lock_guard lock(tail_mtx);

    tail_node->value = new_data;
    tail_node->next_node = std::move(new_node);
    tail_node = new_tail;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::try_pop()
{
    std::lock_guard head_lock(head_mtx);
    if (empty_locked_head(head_lock))
        return nullptr;

    auto old_head_node = std::move(head_node);
    head_node = std::move(old_head_node->next_node);

    return old_head_node->value;
}
}