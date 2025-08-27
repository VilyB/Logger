#include <FileLogger.h>

FileLogger::FileLogger(const std::string &filepath)
    : m_filepath(filepath)
{
}

void FileLogger::log(Entry entry)
{
    std::ofstream ofs(m_filepath, std::ios::out | std::ios::app);
    if (!ofs)
    {
        return;
    }

    ofs << '{'
        << "\"timestamp\":\"" << entry.timestamp << "\","
        << "\"app\":\"" << entry.app << "\","
        << "\"message\":\"" << entry.message << "\""
        << "}\n";
}