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
        Node(T value) : value(std::move(value)) {}
        T value;
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
void ThreadSafeQueue<T>::push(T value)
{
    auto new_node = std::make_unique<Node>();
    Node* new_tail = new_node.get();

    std::lock_guard lock(tail_mtx);

    tail_node->value = std::move(value);
    tail_node->next_node = std::move(new_node);
    tail_node = new_tail;
}

template <typename T>
std::shared_ptr<T> ThreadSafeQueue<T>::try_pop()
{
    std::lock_guard head_lock(head_mtx);
    std::unique_lock tail_lock(tail_mtx);
    if (head_node.get() == tail_node)
        return nullptr;
    tail_lock.unlock();

    auto return_value = std::make_shared<T>(std::move(head_node->value));
    auto old_head_node = std::move(head_node);
    head_node = std::move(old_head_node->next_node);

    return return_value;
}
}