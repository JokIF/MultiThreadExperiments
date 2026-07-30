#include "Priotex.h"

namespace mutex
{   
void Priotex::UpdatePriority() noexcept
{
    m_prevPriorityValue = m_thisThreadPriorityValue;
    m_thisThreadPriorityValue = m_priorityValue;
}

void Priotex::CheckPriority() const
{    
    if (m_priorityValue >= m_thisThreadPriorityValue)
        throw std::logic_error("incorrect order by Priority");
}

void Priotex::lock()
{
    CheckPriority();
    m_innerMutex.lock();
    UpdatePriority();
}

void Priotex::unlock()
{
    m_thisThreadPriorityValue = m_prevPriorityValue;
    m_innerMutex.unlock();
}

bool Priotex::try_lock()
{
    CheckPriority();
    if (!m_innerMutex.try_lock())
        return false;

    UpdatePriority();
    return true;
}
}