// client.cpp
#include <iostream>
#include <fstream>
#include <tchar.h>
#include <string>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#define BUFFER_SIZE 1024 // Розмір буферу символів, який передається до клієнта
#define PORT 55555 // Номер порту для підключення

#pragma comment(lib, "ws2_32.lib") // Для отримання доступу до необхідних функцій

void cerrMessageAndCleanup(const std::string& message, int error, const SOCKET& socket);

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
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (clientSocket == INVALID_SOCKET)
    {
        cerrMessageAndCleanup("Error creating socket", WSAGetLastError(), clientSocket);
        return 1;
    }

    std::cout << "Client socket is created\n";

    // Спроба приєднатися до сервера
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, serverAddrSize) == SOCKET_ERROR)
    {
        cerrMessageAndCleanup("Connection failed", WSAGetLastError(), clientSocket);
        return 1;
    }

    std::cout << "Connected to the server\n";

    // Алгоритм обміну даними
    char buffer[BUFFER_SIZE];
    std::ofstream logFile("chat_log.txt", std::ios::app);
    
    while (true)
    {
        std::cout << "Client: ";
        std::cin.getline(buffer, sizeof(buffer));

        send(clientSocket, buffer, sizeof(buffer), 0);

        if (strcmp(buffer, "exit") == 0)
        {
            std::cerr << "Exit key word is used. Program exits\n";
            closesocket(clientSocket);
            WSACleanup();
            break;
        }

        logFile << "Client: " << buffer << "\n";

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0)
        {
            if (strcmp(buffer, "exit") == 0)
            {
                std::cerr << "Exit key word is used. Program exits\n";
                closesocket(clientSocket);
                WSACleanup();
                break;
            }
            
            std::cout << "Server: " << buffer << "\n";
            
            logFile << "Server: " << buffer << "\n";
        }
        else
        {
            cerrMessageAndCleanup("Error receiving data from server", GetLastError(), clientSocket);
            logFile.close();
            return 1;
        }
    }

    logFile.close();
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