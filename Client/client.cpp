// client.cpp
#include <iostream>
#include <winsock.h>

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

    // Creating a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Error creating socket", WSAGetLastError(), clientSocket);
        return 1;
    }

    // sockaddr_in structure filling
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address of the server
    serverAddr.sin_port = htons(12345); // Port of the server

    // Connecting to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Connection failed", WSAGetLastError(), clientSocket);
        return 1;
    }

    std::cout << "Connected to the server\n";

    // Data exchange
    std::string message = "Hello from the client!";
    int bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
    if (bytesSent == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Sending failed", WSAGetLastError(), clientSocket);
    }
    else
    {
        std::cout << "Message is sent to the server\n";
    }

    // Cleaning up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
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