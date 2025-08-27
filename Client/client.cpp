#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <thread>

static void producer(std::atomic<bool> &alive)
{
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("socket");
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8000);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) != 1)
    {
        std::cerr << "inet_pton failed\n";
        close(clientSocket);
        return;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress)) == -1)
    {
        perror("connect"); // No server listening -> ECONNREFUSED
        close(clientSocket);
        return;
    }

    while (alive.load(std::memory_order_relaxed))
    {
        const char *message = "{\"timestamp\":\"123123213\", \"app\":\"some_other_app\",\"message\":\"Some OTHER message\"}\n";
        ssize_t n = send(clientSocket, message, std::strlen(message), 0);
        if (n == -1)
        {
            perror("send");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    close(clientSocket);
}

int main()
{
    std::atomic<bool> producer_alive{true};
    std::thread t(producer, std::ref(producer_alive));

    char a;
    std::cin >> a; // will pause until you type a char + Enter (or use std::cin.get())

    producer_alive = false;
    t.join();
    return 0;
}
