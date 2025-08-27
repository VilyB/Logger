#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include "LogBuffer.h"
#include "Entry.h"

// Test fixture for LogBuffer tests
class LogBufferTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create test entries for use in tests
        entry1 = Entry("Test message 1");
        entry2 = Entry("Test message 2");
        entry3 = Entry("Test message 3");
        entry4 = Entry("Test message 4");
    }

    Entry entry1, entry2, entry3, entry4;
};

// Test basic construction and capacity
TEST_F(LogBufferTest, Construction)
{
    LogBuffer buffer(10);
    auto snapshot = buffer.snapshot();
    EXPECT_TRUE(snapshot.empty());
}

// Test single push and snapshot
TEST_F(LogBufferTest, SinglePush)
{
    LogBuffer buffer(5);
    buffer.push(entry1);

    auto snapshot = buffer.snapshot();
    ASSERT_EQ(snapshot.size(), 1);
    EXPECT_EQ(snapshot[0].message, "Test message 1");
}

// Test multiple pushes within capacity
TEST_F(LogBufferTest, MultiplePushesWithinCapacity)
{
    LogBuffer buffer(5);
    buffer.push(entry1);
    buffer.push(entry2);
    buffer.push(entry3);

    auto snapshot = buffer.snapshot();
    ASSERT_EQ(snapshot.size(), 3);
    EXPECT_EQ(snapshot[0].message, "Test message 1");
    EXPECT_EQ(snapshot[1].message, "Test message 2");
    EXPECT_EQ(snapshot[2].message, "Test message 3");
}

// Test capacity overflow behavior (oldest entries should be removed)
TEST_F(LogBufferTest, CapacityOverflow)
{
    LogBuffer buffer(2); // Small capacity to test overflow
    buffer.push(entry1);
    buffer.push(entry2);
    buffer.push(entry3); // Should remove entry1

    auto snapshot = buffer.snapshot();
    ASSERT_EQ(snapshot.size(), 2);
    EXPECT_EQ(snapshot[0].message, "Test message 2");
    EXPECT_EQ(snapshot[1].message, "Test message 3");
}

// Test multiple overflows
TEST_F(LogBufferTest, MultipleOverflows)
{
    LogBuffer buffer(2);
    buffer.push(entry1);
    buffer.push(entry2);
    buffer.push(entry3);
    buffer.push(entry4);

    auto snapshot = buffer.snapshot();
    ASSERT_EQ(snapshot.size(), 2);
    EXPECT_EQ(snapshot[0].message, "Test message 3");
    EXPECT_EQ(snapshot[1].message, "Test message 4");
}

// Test clear functionality
TEST_F(LogBufferTest, Clear)
{
    LogBuffer buffer(5);
    buffer.push(entry1);
    buffer.push(entry2);
    buffer.push(entry3);

    // Verify entries exist
    auto snapshot_before = buffer.snapshot();
    EXPECT_EQ(snapshot_before.size(), 3);

    // Clear and verify empty
    buffer.clear();
    auto snapshot_after = buffer.snapshot();
    EXPECT_TRUE(snapshot_after.empty());
}

// Test clear on empty buffer
TEST_F(LogBufferTest, ClearEmptyBuffer)
{
    LogBuffer buffer(5);
    buffer.clear(); // Should not crash
    auto snapshot = buffer.snapshot();
    EXPECT_TRUE(snapshot.empty());
}

// Test with capacity of 1
TEST_F(LogBufferTest, CapacityOne)
{
    LogBuffer buffer(1);
    buffer.push(entry1);
    buffer.push(entry2); // Should replace entry1

    auto snapshot = buffer.snapshot();
    ASSERT_EQ(snapshot.size(), 1);
    EXPECT_EQ(snapshot[0].message, "Test message 2");
}

// Test with zero capacity (edge case)
TEST_F(LogBufferTest, ZeroCapacity)
{
    LogBuffer buffer(0);
    buffer.push(entry1); // Should not be stored

    auto snapshot = buffer.snapshot();
    EXPECT_TRUE(snapshot.empty());
}

// Test thread safety - multiple threads pushing simultaneously
TEST_F(LogBufferTest, ThreadSafety_MultiplePushers)
{
    LogBuffer buffer(100);
    const int num_threads = 4;
    const int entries_per_thread = 25;

    std::vector<std::thread> threads;

    // Start multiple threads pushing entries
    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&buffer, t, entries_per_thread]()
                             {
            for (int i = 0; i < entries_per_thread; ++i) {
                std::string msg = "Thread " + std::to_string(t) + " Entry " + std::to_string(i);
                buffer.push(Entry(msg));
                // Small delay to increase chance of contention
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            } });
    }

    // Wait for all threads to complete
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Verify total number of entries
    auto snapshot = buffer.snapshot();
    EXPECT_EQ(snapshot.size(), num_threads * entries_per_thread);
}

// Test thread safety - one pusher, one reader
TEST_F(LogBufferTest, ThreadSafety_PusherAndReader)
{
    LogBuffer buffer(50);
    std::atomic<bool> stop_flag{false};
    std::vector<size_t> snapshot_sizes;

    // Reader thread
    std::thread reader([&buffer, &stop_flag, &snapshot_sizes]()
                       {
        while (!stop_flag) {
            auto snapshot = buffer.snapshot();
            snapshot_sizes.push_back(snapshot.size());
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } });

    // Pusher thread
    std::thread pusher([&buffer, &stop_flag]()
                       {
        for (int i = 0; i < 100; ++i) {
            buffer.push(Entry("Message " + std::to_string(i)));
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        stop_flag = true; });

    pusher.join();
    reader.join();

    // Final snapshot should have at most 50 entries (buffer capacity)
    auto final_snapshot = buffer.snapshot();
    EXPECT_LE(final_snapshot.size(), 50);
    EXPECT_GT(final_snapshot.size(), 0);
}

// Test thread safety - clear while pushing
TEST_F(LogBufferTest, ThreadSafety_ClearWhilePushing)
{
    LogBuffer buffer(20);
    std::atomic<bool> stop_flag{false};

    // Pusher thread
    std::thread pusher([&buffer, &stop_flag]()
                       {
        int count = 0;
        while (!stop_flag && count < 100) {
            buffer.push(Entry("Message " + std::to_string(count++)));
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } });

    // Clearer thread
    std::thread clearer([&buffer, &stop_flag]()
                        {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        for (int i = 0; i < 5; ++i) {
            buffer.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        stop_flag = true; });

    pusher.join();
    clearer.join();

    // Should not crash, and buffer should be in valid state
    auto snapshot = buffer.snapshot();
    EXPECT_LE(snapshot.size(), 20);
}

// Test move semantics (Entry should be moved, not copied)
TEST_F(LogBufferTest, MoveSemantics)
{
    LogBuffer buffer(5);
    Entry movable_entry("Movable message");

    // Push by move
    buffer.push(std::move(movable_entry));

    auto snapshot = buffer.snapshot();
    ASSERT_EQ(snapshot.size(), 1);
    EXPECT_EQ(snapshot[0].message, "Movable message");

    // Original entry should be in moved-from state (behavior depends on Entry implementation)
    // This test mainly ensures the move works without compilation errors
}

// Parameterized test for various capacities
class LogBufferCapacityTest : public ::testing::TestWithParam<size_t>
{
};

TEST_P(LogBufferCapacityTest, VariousCapacities)
{
    size_t capacity = GetParam();
    LogBuffer buffer(capacity);

    // Push more entries than capacity
    size_t num_entries = capacity + 10;
    for (size_t i = 0; i < num_entries; ++i)
    {
        buffer.push(Entry("Entry " + std::to_string(i)));
    }

    auto snapshot = buffer.snapshot();
    if (capacity == 0)
    {
        EXPECT_TRUE(snapshot.empty());
    }
    else
    {
        EXPECT_EQ(snapshot.size(), capacity);
        // Check that we have the last 'capacity' entries
        for (size_t i = 0; i < capacity; ++i)
        {
            size_t expected_entry_num = (num_entries - capacity) + i;
            EXPECT_EQ(snapshot[i].message, "Entry " + std::to_string(expected_entry_num));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    CapacityTests,
    LogBufferCapacityTest,
    ::testing::Values(0, 1, 2, 5, 10, 50, 100));