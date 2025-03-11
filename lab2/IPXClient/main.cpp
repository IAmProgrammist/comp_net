#define WIN32_LEAN_AND_MEAN

#include <time.h>
#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <wsipx.h>
#include <stdlib.h>
#include <conio.h>

#define CLIENT_REQUEST_SIZE 512
#define IMAGE_PART_SIZE 546

SOCKET socket_descriptor;
SOCKADDR_IPX name = {};
SOCKADDR_IPX server_sockaddr = {};

FILE *source;

void send_request() {
    auto data = "I need a cat image!";

    while (true) {
        std::cout << "Input netnum: ";
        std::cout.flush();
        scanf("%02hhx %02hhx %02hhx %02hhx", server_sockaddr.sa_netnum, server_sockaddr.sa_netnum + 1, server_sockaddr.sa_netnum + 2, server_sockaddr.sa_netnum + 3);
        std::cout << "Input nodenum: ";
        std::cout.flush();
        scanf("%02hhx %02hhx %02hhx %02hhx %02hhx %02hhx", server_sockaddr.sa_nodenum, server_sockaddr.sa_nodenum + 1, server_sockaddr.sa_nodenum + 2, server_sockaddr.sa_nodenum + 3, server_sockaddr.sa_nodenum + 4, server_sockaddr.sa_nodenum + 5);
        std::cout << "Input socket: ";
        std::cout.flush();
        std::cin >> server_sockaddr.sa_socket;
        server_sockaddr.sa_socket = htons(server_sockaddr.sa_socket);

        server_sockaddr.sa_family = AF_IPX;

        if (sendto(socket_descriptor,
                   data,
                   sizeof(data),
                   0,
                   (sockaddr*)&server_sockaddr,
                   sizeof(server_sockaddr)) == SOCKET_ERROR) {
            printf("Unable to connect to server: %d\nTry again\n", WSAGetLastError());
        } else {
            return;
        }
    }
}

void mainloop() {
    std::cout << "Starting file accept..." << std::endl;
    char* buffer = (char*)malloc(sizeof(char) * IMAGE_PART_SIZE);
    while (1) {
        int bytes_received;
        if (bytes_received = recvfrom(
                 socket_descriptor,
                 buffer,
                 sizeof(char) * IMAGE_PART_SIZE,
                 0,
                 nullptr, nullptr) != SOCKET_ERROR) {
                    fwrite(buffer, 1, sizeof(char) * IMAGE_PART_SIZE, source);
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
    err = bind(socket_descriptor, (sockaddr*)&name, sizeof(name));

    if (err != 0) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    int namelen;
    getsockname(socket_descriptor, (sockaddr*)(&name), &namelen);

    std::cout << "Client is inited:\n";
    printf("Net number: %02hhx %02hhx %02hhx %02hhx\n", name.sa_netnum[0], name.sa_netnum[1], name.sa_netnum[2], name.sa_netnum[3]);
    printf("Node number: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx\n", name.sa_nodenum[0], name.sa_nodenum[1], name.sa_nodenum[2], name.sa_nodenum[3], name.sa_nodenum[4], name.sa_nodenum[5]);
    std::cout << "Socket num: " << htons(name.sa_socket) << "\n";
    std::cout.flush();

    int timeout_time = 4000;

    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout_time, sizeof(timeout_time)) == SOCKET_ERROR) {
        printf("Unable to set timeout: %d\n", WSAGetLastError());
        return 1;
    }

    srand(time(NULL));
    sprintf(filename, "img%d.jpg", rand() % 100 + 1);
    source = fopen(filename, "wb");

    send_request();
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

    std::cout << "Have a good day!" << std::endl;

    return 0;
}
