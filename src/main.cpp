#include "WindowManager.h"
#include "LoggerUI.h"
#include "LogBuffer.h"

#include <atomic>
#include <thread>

static void producer(LogBuffer &buffer, std::atomic<bool> &alive)
{
    while (alive.load(std::memory_order_relaxed))
    {
        buffer.push({"12342132", "EXAMPLE_APP", "SOME MESSAGE"});
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

int main(int, char **)
{
    LogBuffer buffer(1000);
    LoggerUI ui(buffer);

    std::atomic<bool> producer_alive{true};
    std::thread t(producer, std::ref(buffer), std::ref(producer_alive));

    WindowManager windowManager;
    if (!windowManager.initialize("Hello World", 800, 600, false))
    {
        producer_alive = false;
        t.join();
        return 1;
    }

    windowManager.loop(ui);

    producer_alive = false;
    t.join();

    return 0;
}
