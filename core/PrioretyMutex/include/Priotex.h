#pragma once
#include <mutex>
#include <limits>

namespace unitTests { class UnitTestDataPriotex; }

namespace mutex
{
class Priotex
{
public:
    using Priorety = unsigned long;

    constexpr explicit Priotex(Priorety value) : m_prioretyValue(value) {}

    Priotex(const Priotex&) = delete;
    Priotex(Priotex&&) = delete;
    Priotex& operator=(const Priotex&) = delete;
    Priotex& operator=(Priotex&&) = delete;

    void    lock();
    void    unlock();
    bool    try_lock();

private:
    void    UpdatePriorety() noexcept;
    void    CheckPriorety() const;

    std::mutex      m_innerMutex;

    const Priorety  m_prioretyValue;
    Priorety        m_prevPrioretyValue;
    inline static thread_local Priorety   m_thisThreadPrioretyValue = std::numeric_limits<Priorety>::max();

    friend class unitTests::UnitTestDataPriotex;
};
}