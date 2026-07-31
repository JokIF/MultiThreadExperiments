#pragma once
#include <mutex>
#include <limits>
#include <source_location>

namespace unitTests { class UnitTestDataPriotex; }

namespace mutex
{
class Priotex
{
public:
    using Priority = unsigned long;

    explicit Priotex(Priority value) : m_priorityValue(value) {}

    Priotex(const Priotex&) = delete;
    Priotex(Priotex&&) = delete;
    Priotex& operator=(const Priotex&) = delete;
    Priotex& operator=(Priotex&&) = delete;

    void    lock(std::source_location loc = std::source_location::current());
    void    unlock();
    bool    try_lock(std::source_location loc = std::source_location::current());

private:
    void    UpdatePriority() noexcept;
    void    CheckPriority(std::source_location loc) const;

    std::mutex      m_innerMutex;

    const Priority  m_priorityValue;
    Priority        m_prevPriorityValue;
    inline static thread_local Priority   m_thisThreadPriorityValue = std::numeric_limits<Priority>::max();

    friend class unitTests::UnitTestDataPriotex;
};
}