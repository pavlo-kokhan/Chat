// server.cpp
#include <iostream>
#include <string>
#include <winsock.h>

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

    // Creating a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Error creating socket", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Client socket is connected\n";

    // sockaddr_in structure filling
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345); // Select port

    // Socket binding
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Bind has failed", WSAGetLastError(), serverSocket);
        return 1;
    }

    // Listening on a socket
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Listen failed", WSAGetLastError(), serverSocket);
        return 1;
    }
    
    std::cout << "Server listening on port 12345...\n";

    // Waiting for client connection
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Accept failed", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Client is connected\n";

    // Data exchange
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        std::cerr << "Receive failed\n";
        std::cerr << "Error: " << WSAGetLastError() << "\n";
    }
    else
    {
        buffer[bytesReceived] = '\0';
        std::cout << "Received message from client: " << buffer << "\n";
    }

    // Cleaning up
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    
    return 0;
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