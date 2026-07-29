#include "Priotex.h"

namespace mutex
{   
void Priotex::UpdatePriorety() noexcept
{
    m_prevPrioretyValue = m_thisThreadPrioretyValue;
    m_thisThreadPrioretyValue = m_prioretyValue;
}

void Priotex::CheckPriorety() const
{    
    if (m_prioretyValue >= m_thisThreadPrioretyValue)
        throw std::logic_error("incorrect order by priorety");
}

void Priotex::lock()
{
    CheckPriorety();
    m_innerMutex.lock();
    UpdatePriorety();
}

void Priotex::unlock()
{
    m_thisThreadPrioretyValue = m_prevPrioretyValue;
    m_innerMutex.unlock();
}

bool Priotex::try_lock()
{
    CheckPriorety();
    if (!m_innerMutex.try_lock())
        return false;

    UpdatePriorety();
    return true;
}
}