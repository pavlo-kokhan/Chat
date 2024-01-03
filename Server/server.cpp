// server.cpp
#include <iostream>
#include <tchar.h>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#define BUFFER_SIZE 1024 // Розмір буферу символів, який передається до клієнта
#define PORT 55555 // Номер порту для підключення

#pragma comment(lib, "ws2_32.lib") // Для отримання доступу до необхідних функцій

void cerrMessageAndCleanup(const std::string& message, const int& error, const SOCKET& socket);

int main()
{
    WSADATA wsaData;
    WORD dllVersion = MAKEWORD(2, 2); // Версія бібліотеки winsock
    
    if (WSAStartup(dllVersion, &wsaData) != 0) // Завантаження бібліотеки winsock
    {
        std::cerr << "Winsock dll is not found\n";
        return 1;
    }
    
    std::cout << "The winsock dll is found\n";
    std::cout << "Status: " << wsaData.szSystemStatus << "\n";

    // Налаштування структури sockaddr_in для визначення IP адреси і порту сервера
    sockaddr_in serverAddr;
    int serverAddrSize = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET; // Встановлення сімейства протоколів (для інтернет протоколів AF_INET)
    InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr.s_addr); // Встановлення IP адреси
    serverAddr.sin_port = htons(PORT); // Встановлення порту сервера

    // Створення сокету
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (serverSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Error creating socket", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Server socket is created\n";

    // Прив'язка адреса сокету
    if (bind(serverSocket, (SOCKADDR*)&serverAddr, serverAddrSize) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Bind has failed", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Socket binding is successful\n";

    // Прослуховування порту в очікуванні приєднання клієнта
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Listen failed", WSAGetLastError(), serverSocket);
        return 1;
    }
    
    std::cout << "Server listening on the port " << PORT << "\n";

    // Новий сокет для з'єднання з клієнтом
    SOCKET acceptSocket = accept(serverSocket, (SOCKADDR*)&serverAddr, &serverAddrSize);
    
    if (acceptSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Accept failed", WSAGetLastError(), serverSocket);
        return 1;
    }

    std::cout << "Client accepted connection\n";

    // Алгоритм обміну даними
    char buffer[BUFFER_SIZE];
    while(true)
    {
        int bytesReceived = recv(acceptSocket, buffer, sizeof(buffer), 0);
        
        if(bytesReceived > 0)
        {
            if (strcmp(buffer, "exit") == 0)
            {
                std::cerr << "Exit key word is used. Program exits\n";
                closesocket(serverSocket);
                WSACleanup();
                break;
            }
            
            std::cout << "Client: " << buffer << "\n";
        }
        else
        {
            cerrMessageAndCleanup("Error receiving data from client", GetLastError(), serverSocket);
            return 1;
        }
        
        std::cout << "Server: ";
        std::cin.getline(buffer, sizeof(buffer));
        
        send(acceptSocket, buffer, sizeof(buffer), 0);

        if (strcmp(buffer, "exit") == 0)
        {
            std::cerr << "Exit key word is used. Program exits\n";
            closesocket(serverSocket);
            WSACleanup();
            break;
        }
    }

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