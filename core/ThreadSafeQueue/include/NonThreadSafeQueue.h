#pragma once
#include <memory>
#include <exception>

namespace NonThreadSafeStructs
{
class EmptyQueue : public std::exception {
    char const* what() const noexcept override { return "Queue is empty"; }
};

template <typename T>
class NonThreadSafeQueue
{
    struct Node {
        Node(T value) : value(std::move(value)) {}
        T value;
        std::unique_ptr<Node> next_node = nullptr;
    };

public:
    NonThreadSafeQueue() = default;
    
    NonThreadSafeQueue(NonThreadSafeQueue&&) noexcept = default;

    NonThreadSafeQueue(const NonThreadSafeQueue&) = delete;
    NonThreadSafeQueue& operator=(const NonThreadSafeQueue&) = delete;
    NonThreadSafeQueue& operator=(NonThreadSafeQueue&&) noexcept = delete;

    void push(T value);
    T pop();
    std::shared_ptr<T> try_pop();

    bool empty() const { return head_node == nullptr; }

private:
    std::unique_ptr<Node> head_node = nullptr;
    Node* tail_node = nullptr;
};

template <typename T>
void NonThreadSafeQueue<T>::push(T value)
{
    auto new_node = std::make_unique<Node>(std::move(value));
    Node* new_tail = new_node.get();

    if (tail_node == nullptr)
        head_node = std::move(new_node);
    else
        tail_node->next_node = std::move(new_node);

    tail_node = new_tail;
}

template <typename T>
T NonThreadSafeQueue<T>::pop()
{
    if (head_node == nullptr) throw EmptyQueue();

    auto old_head_node = std::move(head_node);
    head_node = std::move(old_head_node->next_node);

    return old_head_node->value;
}

template <typename T>
std::shared_ptr<T> NonThreadSafeQueue<T>::try_pop()
{
    if (head_node == nullptr)
        return nullptr;

    auto return_value = std::make_shared<T>(std::move(head_node->value));

    auto old_head_node = std::move(head_node);
    head_node = std::move(old_head_node->next_node);

    return return_value;
}
}