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
static std::optional<Entry> parseEntryJson(const std::string &message)
{
    auto get = [&](const std::string &key) -> std::string
    {
        std::string pattern = "\"" + std::string(key) + "\"";

        auto k = message.find(pattern);
        if (k == std::string::npos)
        {
            return {};
        }

        auto colon = message.find(':', k + pattern.size());
        if (colon == std::string::npos)
        {
            return {};
        }

        auto startQuote = message.find('"', colon + 1);
        if (startQuote == std::string::npos)
        {
            return {};
        }

        auto endQuote = message.find('"', startQuote + 1);
        if (endQuote == std::string::npos)
        {
            return {};
        }

        return std::string(message.substr(startQuote + 1, endQuote - startQuote - 1));
    };

    Entry e{get("timestamp"), get("app"), get("message")};
    if (e.timestamp.empty() && e.app.empty() && e.message.empty())
    {
        return std::nullopt;
    }

    return e;
}
