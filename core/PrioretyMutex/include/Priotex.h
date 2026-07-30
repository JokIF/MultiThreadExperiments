#pragma once
#include <mutex>
#include <limits>

namespace unitTests { class UnitTestDataPriotex; }

namespace mutex
{
class Priotex
{
public:
    using Priority = unsigned long;

    constexpr explicit Priotex(Priority value) : m_PriorityValue(value) {}

    Priotex(const Priotex&) = delete;
    Priotex(Priotex&&) = delete;
    Priotex& operator=(const Priotex&) = delete;
    Priotex& operator=(Priotex&&) = delete;

    void    lock();
    void    unlock();
    bool    try_lock();

private:
    void    UpdatePriority() noexcept;
    void    CheckPriority() const;

    std::mutex      m_innerMutex;

    const Priority  m_PriorityValue;
    Priority        m_prevPriorityValue;
    inline static thread_local Priority   m_thisThreadPriorityValue = std::numeric_limits<Priority>::max();

    friend class unitTests::UnitTestDataPriotex;
};
}