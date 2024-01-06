#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>
#include <string.h>
#include "utils.h"
#pragma comment(lib, "Ws2_32.lib")

#define PORT_NUMBER "80"
#define BUFFER_SIZE 2048

int createServerSocket(void);
void* handleConnection(void*);
int handleRequest(char*);
void decodeUrlStr(char*, char*);

WSADATA wsaData;
SOCKET serverSocket = INVALID_SOCKET;

int main(void) {
    if (createServerSocket() != 0) {
        // Exit with error if serverSocket is not created successfully.
        return 1;
    }

    while (1) {
        SOCKET clientSocket = INVALID_SOCKET;
        // Accept Client Socket
        clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        pthread_t thread;
        // Pass socket by value.
        pthread_create(&thread, NULL, handleConnection, (void*)clientSocket);
        pthread_detach(thread);
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}

int createServerSocket(void) {
    struct addrinfo* serverAddrInfo = NULL, hints;

    // Initialize Winsock
    int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // Init use of WS2_32.dll
    if (initResult != 0) {
        printf_s("WSAStartup failed: %d", initResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    // Set preferences for getaddrinfo
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // Select address compatible with TCP.
    hints.ai_protocol = IPPROTO_TCP;
    // Intend to use address information for server socket to listen for coming connections.
    hints.ai_flags = AI_PASSIVE;

    // NULL to determine localhost automatically.
    initResult = getaddrinfo(NULL, PORT_NUMBER, &hints, &serverAddrInfo);
    if (initResult != 0) {
        printf_s("getaddrinfo failed: %d", initResult);
        WSACleanup();
        return 1;
    }

    // Create server socket.
    serverSocket = socket(serverAddrInfo->ai_family, serverAddrInfo->ai_socktype, serverAddrInfo->ai_protocol);
    if (serverSocket == INVALID_SOCKET) {
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(serverAddrInfo);
        WSACleanup();
        return 1;
    }

    // Setup TCP listening socket.
    initResult = bind(serverSocket, serverAddrInfo->ai_addr, (int)serverAddrInfo->ai_addrlen);
    if (initResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(serverAddrInfo);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // After binding, address information is no longer needed.
    freeaddrinfo(serverAddrInfo);

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    return 0;
}

void* handleConnection(void* arg) {
    SOCKET clientSocket = (SOCKET)arg;
    char recvBuffer[BUFFER_SIZE + 1];
    int recvBufferSize = BUFFER_SIZE;

    int recvResult = recv(clientSocket, recvBuffer, recvBufferSize, 0);
    if (recvResult > 0) {
        recvBuffer[BUFFER_SIZE] = '\0';
        // Handle request and generate response.
        handleRequest(recvBuffer);
        // Process request
        size_t sendResult = send(clientSocket, "200 OK\r\nHello World!", 20, 0);
        if (sendResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
        }
    } else if (recvResult == 0) {
        printf("No data received.\n");
    } else {
        printf("recv failed: %d\n", WSAGetLastError());
    }

    if (shutdown(clientSocket, SD_SEND) == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
    }

    closesocket(clientSocket);
    return NULL;
}

int handleRequest(char* request) {
    char* header = strsplit(&request, "\r\n"); // Split by CRLF
    char* requestLine = strsplit(&header, "\n");
    char* requestType = strsplit(&requestLine, " ");
    char* requestURI = strsplit(&requestLine, " ");
    // Decode URI encoding.
    // Sanitize URI.

    if (strcmp(requestType, "GET") == 0) {
        printf("Handling GET request.\n");
    } else if (strcmp(requestType, "POST") == 0) {
        printf("Handling POST request.\n");
    } else if (strcmp(requestType, "HEAD") == 0) {
        printf("Handling HEAD request.\n");
    } else {
        printf("Bad request.\n");
        return 1;
    }
    printf("Request URI: %s\n", requestURI);
    return 0;
}

// Unsure how to decode URL strings
void decodeUrlStr(char* url, char* output) {
    // From https://rosettacode.org/wiki/URL_decoding#C
    char* endPtr = url + strlen(url);
    char* outPtr = output;
    // Loop through url
    // If char == '+', convert to ' '
}