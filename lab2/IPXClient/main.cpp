#define WIN32_LEAN_AND_MEAN

#include <time.h>
#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <wsipx.h>
#include <stdlib.h>
#include <conio.h>

#define IPX_SOCKET (0x8060)
#define CLIENT_REQUEST_SIZE 512
#define IMAGE_PART_SIZE 546

SOCKET socket_descriptor;
SOCKADDR_IPX name = {};
SOCKADDR_IPX server_sockaddr = {};

FILE *source;

void mainloop() {
    std::cout << "Starting file accept..." << std::endl;
    char* buffer = (char*)malloc(sizeof(char) * IMAGE_PART_SIZE);
    while (1) {
        int bytes_received;
        if ((bytes_received = recvfrom(
                 socket_descriptor,
                 buffer,
                 sizeof(char) * IMAGE_PART_SIZE,
                 0,
                 nullptr, nullptr)) != SOCKET_ERROR) {
                    fwrite(buffer, sizeof(char), IMAGE_PART_SIZE, source);
                 } else {
                    break;
                 }
    }
}

int main()
{
    char filename[20];

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 0);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    socket_descriptor = socket(
                                    AF_IPX,
                                    SOCK_DGRAM,
                                    NSPROTO_IPX
                                    );
    name.sa_family = AF_IPX;
    name.sa_socket = htons(IPX_SOCKET);
    err = bind(socket_descriptor, (sockaddr*)&name, sizeof(name));

    if (err != 0) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    int namelen = sizeof(name);
    getsockname(socket_descriptor, (sockaddr*)(&name), &namelen);

    std::cout << "Client is inited:\n";
    printf("Net number: %02hhx %02hhx %02hhx %02hhx\n", name.sa_netnum[0], name.sa_netnum[1], name.sa_netnum[2], name.sa_netnum[3]);
    printf("Node number: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx\n", name.sa_nodenum[0], name.sa_nodenum[1], name.sa_nodenum[2], name.sa_nodenum[3], name.sa_nodenum[4], name.sa_nodenum[5]);
    std::cout << "Socket num: " << htons(name.sa_socket) << "\n";
    std::cout.flush();

    int timeout_time = 10000;

    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout_time, sizeof(timeout_time)) == SOCKET_ERROR) {
        printf("Unable to set timeout: %d\n", WSAGetLastError());
        return 1;
    }

    srand(time(NULL));
    sprintf(filename, "img%d.jpg", rand() % 100 + 1);
    source = fopen(filename, "wb");

    mainloop();

    fflush(source);
    fclose(source);

    err = WSACleanup();

    if (err != 0) {
        printf("WSACleanup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    closesocket(socket_descriptor);

    std::cout << "A file was saved at " << filename << "\nPress any button to exit" << std::endl;
    getchar();
    getchar();

    std::cout << "Have a good day!" << std::endl;

    return 0;
}
