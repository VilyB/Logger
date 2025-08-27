#include "LogBuffer.h"
#include "Entry.h"

LogBuffer::LogBuffer(size_t capacity)
    : m_cap(capacity)
{
}

void LogBuffer::push(Entry entry)
{
    std::lock_guard<std::mutex> lock(m_mut);

    if (m_cap == 0)
    {
        return;
    }

    if (m_dq.size() == m_cap)
    {
        m_dq.pop_front();
    }

    m_dq.push_back(std::move(entry));
}

std::vector<Entry> LogBuffer::snapshot() const
{
    std::lock_guard<std::mutex> lock(m_mut);
    return {m_dq.begin(), m_dq.end()};
}

void LogBuffer::clear()
{
    std::lock_guard<std::mutex> lock(m_mut);
    m_dq.clear();
}