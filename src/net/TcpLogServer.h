#pragma once
#include "LogBuffer.h"
#include "FileLogger.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <thread>

class TcpLogServer
{
private:
    LogBuffer &m_logBuffer;
    FileLogger &m_fileLogger;

    int m_serverSocket{-1};
    int m_clientSocket{-1};

    sockaddr_in m_serverAddress{};

    char m_receiveBuffer[1024] = {0};
    const std::string m_host{"127.0.0.1"};
    const uint16_t m_port{8000};

    std::atomic<bool> m_running{false};
    std::thread m_thread;

    void run()
    {
        initialize();
        if (m_serverSocket == -1)
        {
            return;
        }

        while (m_running.load())
        {
            m_clientSocket = accept(m_serverSocket, nullptr, nullptr);
            if (m_clientSocket == -1)
            {

                if (m_running.load())
                    perror("accept");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }

            std::string pending;
            while (m_running.load())
            {
                ssize_t n = recv(m_clientSocket, m_receiveBuffer, sizeof(m_receiveBuffer) - 1, 0);
                if (n > 0)
                {
                    pending.append(m_receiveBuffer, m_receiveBuffer + n);

                    size_t nl;
                    while ((nl = pending.find('\n')) != std::string::npos)
                    {
                        std::string line = pending.substr(0, nl);
                        pending.erase(0, nl + 1);

                        if (auto e = parseEntryJson(std::string_view(line)))
                        {
                            m_logBuffer.push(*e);
                            m_fileLogger.log(*e);
                        }
                    }

                    m_receiveBuffer[n] = '\n';
                    std::cout << "Message from client: " << m_receiveBuffer << std::endl;
                }
                else if (n == 0)
                {
                    break;
                }
                else
                {
                    perror("recv");
                    break;
                }
            }

            ::close(m_clientSocket);
            m_clientSocket = -1;
        }
    }

public:
    TcpLogServer(LogBuffer &buffer, FileLogger &fileLogger) : m_logBuffer(buffer), m_fileLogger(fileLogger) {}
    ~TcpLogServer() { stop(); }

    bool start()
    {
        if (m_running.exchange(true))
            return true;

        m_thread = std::thread([this]
                               { this->run(); });

        return true;
    }

    void stop()
    {
        if (!m_running.exchange(false))
            return;

        // Shutdown and close server
        if (m_serverSocket != -1)
        {
            ::shutdown(m_serverSocket, SHUT_RDWR);
            ::close(m_serverSocket);
        }

        // Shutdown and close client
        if (m_clientSocket != -1)
        {
            ::shutdown(m_clientSocket, SHUT_RDWR);
            ::close(m_clientSocket);
        }

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    void initialize()
    {
        m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        m_serverAddress.sin_family = AF_INET;
        m_serverAddress.sin_port = htons(m_port);
        m_serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        if (::bind(m_serverSocket, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress)) == -1)
        {
            perror("bind");
            ::close(m_serverSocket);
            m_serverSocket = -1;
            return;
        }
        std::cout << "Bind successful..." << std::endl;

        if (::listen(m_serverSocket, 5) == -1)
        {
            perror("listen");
            ::close(m_serverSocket);
            m_serverSocket = -1;
            return;
        }
        std::cout << "Listen successful..." << std::endl;
    }
};