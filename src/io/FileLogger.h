#pragma once

#include <string>

#include "Entry.h"

class FileLogger
{
private:
    std::string m_filepath;

public:
    FileLogger(const std::string &filepath) : m_filepath(filepath) {};
    ~FileLogger() = default;

    FileLogger(const FileLogger &) = delete;
    FileLogger &operator=(const FileLogger &) = delete;
    FileLogger(FileLogger &&) = delete;
    FileLogger &operator=(FileLogger &&) = delete;

    void log(Entry e)
    {
    }
};