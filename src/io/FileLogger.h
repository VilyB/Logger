#pragma once

#include <string>
#include <fstream>
#include "Entry.h"

class ILogger
{
public:
    virtual ~ILogger() = default;
    virtual void log(Entry entry) = 0;
};

class FileLogger : public ILogger
{
private:
    std::string m_filepath;

public:
    FileLogger(const std::string &filepath);

    FileLogger(const FileLogger &) = delete;
    FileLogger &operator=(const FileLogger &) = delete;
    FileLogger(FileLogger &&) = delete;
    FileLogger &operator=(FileLogger &&) = delete;

    void log(Entry e) override;
};