#pragma once
#include <atomic>
#include <memory>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4324)
#endif

namespace ThreadSafeStructs
{
template <typename T>
class WaitFreeThreadSafeQueue
{
    struct Cell
    {
        std::atomic<size_t> seq;
        alignas(alignof(T)) char storage[sizeof(T)];
    };

public:
    explicit WaitFreeThreadSafeQueue(size_t capacity_) : capacity(capacity_), buffer(new Cell[capacity_])
    {
        for (size_t i = 0; i < capacity; i++)
            buffer[i].seq.store(i, std::memory_order_relaxed);
    }

    ~WaitFreeThreadSafeQueue()
    {
        size_t head_pos = head.load(std::memory_order_relaxed);
        size_t tail_pos = tail.load(std::memory_order_relaxed);
        while (head_pos != tail_pos) 
        {
            Cell& cell = buffer[head_pos % capacity];
            if (cell.seq.load(std::memory_order_relaxed) == head_pos + 1)
                reinterpret_cast<T*>(cell.storage)->~T();

            head_pos++;
        }
    }

    WaitFreeThreadSafeQueue(const WaitFreeThreadSafeQueue&) = delete;
    WaitFreeThreadSafeQueue& operator=(const WaitFreeThreadSafeQueue&) = delete;

    WaitFreeThreadSafeQueue(WaitFreeThreadSafeQueue&&) noexcept = default;
    WaitFreeThreadSafeQueue& operator=(WaitFreeThreadSafeQueue&&) noexcept = default;

    template <typename U> requires std::constructible_from<T, U&&>
    bool    try_push(U&& value);
    bool    try_pop(T& value);

private:
    const size_t capacity;
    std::unique_ptr<Cell[]> buffer;

    alignas(64) std::atomic<size_t> tail = 0;
    alignas(64) std::atomic<size_t> head = 0;
};

template <typename T>
template <typename U> requires std::constructible_from<T, U&&>
bool WaitFreeThreadSafeQueue<T>::try_push(U&& value)
{
    Cell* cell = nullptr;
    size_t pos = tail.load(std::memory_order_relaxed);
    for (;;)
    {
        cell = &buffer[pos % capacity];
        size_t seq = cell->seq.load(std::memory_order_acquire);
        std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos);
        if (diff == 0)
        {
            if (tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                break;
        }
        else if (diff < 0)
            return false;
        else
            pos = tail.load(std::memory_order_relaxed);
    }

    try
    {
        new (cell->storage) T(std::forward<U>(value));
    }
    catch (...)
    {
        cell->seq.store(pos + capacity, std::memory_order_release);
        throw;
    }

    cell->seq.store(pos + 1, std::memory_order_release);
    return true;
}

template <typename T>
bool WaitFreeThreadSafeQueue<T>::try_pop(T& value)
{
    Cell* cell = nullptr;
    size_t pos = head.load(std::memory_order_relaxed);
    for (;;)
    {
        cell = &buffer[pos % capacity];
        size_t seq = cell->seq.load(std::memory_order_acquire);
        std::intptr_t diff = static_cast<std::intptr_t>(seq) - static_cast<std::intptr_t>(pos + 1);
        if (diff == 0)
        {
            if (head.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                break;
        }    
        else if (diff < 0)
            return false;

        else
            pos = head.load(std::memory_order_relaxed);
    }

    T* out_ptr = reinterpret_cast<T*>(cell->storage);
    try
    {
        value = std::move(*out_ptr);
    }
    catch (...)
    {
        out_ptr->~T();
        cell->seq.store(pos + capacity, std::memory_order_release);
        throw;
    }

    out_ptr->~T();
    cell->seq.store(pos + capacity, std::memory_order_release);

    return true;
}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif