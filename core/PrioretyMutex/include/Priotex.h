#pragma once
#include <mutex>
#include <limits>

namespace mutex
{
class Priotex
{
public:
    using Priorety = unsigned long;

    explicit Priotex(Priorety value);

    Priotex(const Priotex&) noexcept = delete;
    Priotex(Priotex&&) noexcept = delete;
    Priotex& operator=(const Priotex&) = delete;
    Priotex& operator=(Priotex&&) = delete;

    void    lock();
    void    unlock() noexcept;
    bool    try_lock();

private:
    void    UpdatePriorety() noexcept;
    void    CheckPriorety() const;

    std::mutex      m_innerMutex;

    const Priorety  m_prioretyValue;
    Priorety        m_prevPrioretyValue;
    inline static thread_local Priorety   m_thisThreadPrioretyValue = std::numeric_limits<Priorety>::max();
};
}