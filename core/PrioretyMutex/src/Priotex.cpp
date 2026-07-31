#include <format>
#include "Priotex.h"

namespace mutex
{   
void Priotex::UpdatePriority() noexcept
{
    m_prevPriorityValue = m_thisThreadPriorityValue;
    m_thisThreadPriorityValue = m_priorityValue;
}

void Priotex::CheckPriority(std::source_location loc) const
{    
    if (m_priorityValue < m_thisThreadPriorityValue)
        return;

    auto msg = std::format(
        "Priority violation at {}:{} — attempted lock with priority {} while thread holds {}",
        loc.file_name(), loc.line(),
        m_priorityValue,
        m_thisThreadPriorityValue
    );
    throw std::logic_error(msg);
}

void Priotex::lock(std::source_location loc)
{
    CheckPriority(loc);
    m_innerMutex.lock();
    UpdatePriority();
}

void Priotex::unlock()
{
    m_thisThreadPriorityValue = m_prevPriorityValue;
    m_innerMutex.unlock();
}

bool Priotex::try_lock(std::source_location loc)
{
    CheckPriority(loc);
    if (!m_innerMutex.try_lock())
        return false;

    UpdatePriority();
    return true;
}
}