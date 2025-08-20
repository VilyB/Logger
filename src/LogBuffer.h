#pragma once

#include <string>
#include <mutex>
#include <deque>
#include <vector>

struct Entry
{
    std::string timestamp;
    std::string app;
    std::string message;
};

class LogBuffer
{
private:
    mutable std::mutex m_mut;
    std::deque<Entry> m_dq;
    size_t m_cap;

public:
    LogBuffer(size_t capacity) : m_cap(capacity) {};

    void push(Entry e)
    {
        std::lock_guard<std::mutex> lock(m_mut);
        if (m_dq.size() == m_cap)
            m_dq.pop_front();
        m_dq.push_back(std::move(e));
    }

    std::vector<Entry> snapshot() const
    {
        std::lock_guard<std::mutex> lock(m_mut);
        return {m_dq.begin(), m_dq.end()};
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mut);
        m_dq.clear();
    }
};