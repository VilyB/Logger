#pragma once

#include <string>
#include <mutex>
#include <deque>
#include <vector>
#include <optional>

#include "Entry.h"

class ILogBuffer
{
public:
    virtual ~ILogBuffer() = default;
    virtual void push(Entry) = 0;
    virtual std::vector<Entry> snapshot() const = 0;
    virtual void clear() = 0;
};

class LogBuffer : public ILogBuffer
{
private:
    mutable std::mutex m_mut;
    std::deque<Entry> m_dq;
    const size_t m_cap;

public:
    LogBuffer(size_t capacity);

    void push(Entry entry) override;
    std::vector<Entry> snapshot() const override;
    void clear() override;
};