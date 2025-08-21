#pragma once

#include <string>

struct Entry
{
    std::string timestamp;
    std::string app;
    std::string message;
};

////////////////////////////////////////////////////////////////
// Utility Functions
////////////////////////////////////////////////////////////////
static std::optional<Entry> parseEntryJson(std::string_view s)
{
    auto get = [&](std::string_view key) -> std::string
    {
        std::string pat = "\"" + std::string(key) + "\"";

        auto k = s.find(pat);
        if (k == std::string::npos)
        {
            return {};
        }

        auto c = s.find(':', k + pat.size());
        if (c == std::string::npos)
        {
            return {};
        }

        auto q1 = s.find('"', c + 1);
        if (q1 == std::string::npos)
        {
            return {};
        }

        auto q2 = s.find('"', q1 + 1);
        if (q2 == std::string::npos)
        {
            return {};
        }

        return std::string(s.substr(q1 + 1, q2 - q1 - 1));
    };

    Entry e{get("timestamp"), get("app"), get("message")};
    if (e.timestamp.empty() && e.app.empty() && e.message.empty())
    {
        return std::nullopt;
    }

    return e;
}
