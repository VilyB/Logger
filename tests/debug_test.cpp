#include <gtest/gtest.h>
#include <iostream>
#include "Entry.h"
#include "LogBuffer.h"

// Simple debug test to isolate the issue
TEST(DebugTest, EntryCreation)
{
    Entry e("Hello World");
    std::cout << "Entry created:" << std::endl;
    std::cout << "  message: '" << e.message << "'" << std::endl;
    std::cout << "  app: '" << e.app << "'" << std::endl;
    std::cout << "  timestamp: '" << e.timestamp << "'" << std::endl;

    EXPECT_EQ(e.message, "Hello World");
}

TEST(DebugTest, BasicLogBuffer)
{
    std::cout << "Creating LogBuffer with capacity 3" << std::endl;
    LogBuffer buffer(3);

    std::cout << "Creating entry" << std::endl;
    Entry e("Test Message");
    std::cout << "Entry message: '" << e.message << "'" << std::endl;

    std::cout << "Pushing entry" << std::endl;
    buffer.push(e);

    std::cout << "Getting snapshot" << std::endl;
    auto snapshot = buffer.snapshot();
    std::cout << "Snapshot size: " << snapshot.size() << std::endl;

    if (!snapshot.empty())
    {
        std::cout << "First entry message: '" << snapshot[0].message << "'" << std::endl;
    }

    EXPECT_EQ(snapshot.size(), 1);
    if (!snapshot.empty())
    {
        EXPECT_EQ(snapshot[0].message, "Test Message");
    }
}