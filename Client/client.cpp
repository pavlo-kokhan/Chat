// client.cpp
#include <iostream>
#include <tchar.h>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#define BUFFER_SIZE 1024
#define PORT 55555

#pragma comment(lib, "ws2_32.lib") // linking program with ws2_32.lib library

void cerrMessageAndCleanup(const std::string& message, int error, const SOCKET& socket);

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize winsock\n";
        return 1;
    }

    std::cout << "The winsock dll is found\n";
    std::cout << "Status: " << wsaData.szSystemStatus << "\n";

    // Creating a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Error creating socket", WSAGetLastError(), clientSocket);
        return 1;
    }

    std::cout << "Client socket is created\n";

    // sockaddr_in structure filling
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr.s_addr);
    serverAddr.sin_port = htons(PORT); // Port of the server

    // Connecting to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Connection failed", WSAGetLastError(), clientSocket);
        return 1;
    }

    std::cout << "Connected to the server\n";

    // Data exchange
    char buffer[BUFFER_SIZE];
    while (true)
    {
        std::cout << "Client: ";
        std::cin.getline(buffer, sizeof(buffer));

        send(clientSocket, buffer, sizeof(buffer), 0);

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0)
        {
            std::cout << "Server: " << buffer << "\n";
        }
    }
}

void cerrMessageAndCleanup(const std::string& message, int error, const SOCKET& socket)
{
    std::cerr << message << "\n";
    std::cerr << "Error: " << error << "\n";

    if (socket != INVALID_SOCKET)
    {
        closesocket(socket);
    }
    
    WSACleanup();
}