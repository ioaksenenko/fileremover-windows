#define _CRT_SECURE_NO_WARNINGS
#define DEFAULT_BUFLEN 512

#include <sys/types.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>

#pragma comment (lib,"Ws2_32.lib")


int main() {
    WSADATA wsaData = { 0 };
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != NO_ERROR) {
        wprintf(L"WSAStartup function failed with error: %d\n", res);
        return 1;
    }
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(34543);
    res = bind(listen_socket, (SOCKADDR*)&addr, sizeof(addr));
    if (res == SOCKET_ERROR) {
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    printf("Listening...\n");
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    SOCKET client_socket = accept(listen_socket, NULL, NULL);
    if (listen_socket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }
    printf("Accepted client.\n");
    char filename[DEFAULT_BUFLEN];
    int bytes_num = recv(client_socket, filename, DEFAULT_BUFLEN, 0);
    if (bytes_num > 0) {
        printf("Bytes received: %d\n", bytes_num);
    }
    else if (res == 0)
        printf("Connection closing...\n");
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    printf("Deleting file '%s'\n", filename);
    res = remove(filename);
    if (res == -1) {
        perror("remove faliled");
        return 1;
    }
    char response[DEFAULT_BUFLEN];
    sprintf(response, "File '%s' deleted successfully.\n", filename);
    res = send(client_socket, response, DEFAULT_BUFLEN, 0);
    if (res == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    res = closesocket(client_socket);
    if (res == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    res = closesocket(listen_socket);
    if (res == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    WSACleanup();
    return 0;
}