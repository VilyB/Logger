#include "WindowManager.h"
#include "LoggerUI.h"
#include "LogBuffer.h"
#include "TcpLogServer.h"
#include "FileLogger.h"

#include <atomic>
#include <thread>

int main(int, char **)
{
    LogBuffer buffer(1000);
    LoggerUI ui(buffer);
    FileLogger fileLogger("./logs/my_log.txt");

    TcpLogServer tcpLogServer(buffer, fileLogger);
    tcpLogServer.start();

    WindowManager windowManager;
    if (!windowManager.initialize("DT Engine Test & Validation: Proof of Concept Exercise", 800, 600, false))
    {
        return 1;
    }

    windowManager.loop(ui);

    tcpLogServer.stop();
    return 0;
}
