// server.cpp
#include <iostream>
#include <tchar.h>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#define BUFFER_SIZE 1024
#define PORT 55555

#pragma comment(lib, "ws2_32.lib") // linking program with ws2_32.lib library

void cerrMessageAndCleanup(const std::string& message, const int& error, const SOCKET& socket);

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
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Error creating socket", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Server socket is created\n";

    // sockaddr_in structure filling
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr.s_addr);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT); // Select port

    // Socket binding
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Bind has failed", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Socket binding is successful\n";

    // Listening on a socket
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Listen failed", WSAGetLastError(), serverSocket);
        return 1;
    }
    
    std::cout << "Server listening on the port " << PORT << "\n";

    // Waiting for client connection
    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Accept failed", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Client accepted connection\n";

    // Data exchange
    char buffer[BUFFER_SIZE];
    std::cin.getline(buffer, sizeof(buffer));
    while(true)
    {
        int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer), 0);
        
        if(bytesReceived > 0)
        {
            std::cout << "Client: " << buffer << "\n";
        }
        
        std::cout << "Server: ";
        std::cin.getline(buffer, sizeof(buffer));
        
        bytesReceived = send(acceptSocket, buffer, sizeof(buffer), 0);
    }
}

void cerrMessageAndCleanup(const std::string& message, const int& error, const SOCKET& socket)
{
    std::cerr << message << "\n";
    std::cerr << "Error: " << error << "\n";

    if (socket != INVALID_SOCKET)
    {
        closesocket(socket);
    }
    
    WSACleanup();
}