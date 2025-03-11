#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <wsipx.h>
#include <stdlib.h>
#include <conio.h>

#define CLIENT_REQUEST_SIZE 512
#define IMAGE_PART_SIZE 400

SOCKET socket_descriptor;
SOCKADDR_IPX name = {};

FILE *source;

void send_request() {
    uint32_t netnum;
    uint64_t nodenum;
    SOCKADDR_IPX server_sockaddr = {};
    auto data = "I need a cat image!";

    while (true) {
        std::cout << "Input netnum: ";
        std::cout.flush();
        std::cin >> netnum;
        std::cout << "Input nodenum: ";
        std::cout.flush();
        std::cin >> std::hex >> nodenum >> std::dec;
        std::cout << "Input socket: ";
        std::cout.flush();
        std::cin >> server_sockaddr.sa_socket;
        server_sockaddr.sa_socket = htons(server_sockaddr.sa_socket);

        server_sockaddr.sa_family = AF_IPX;


        server_sockaddr.sa_netnum[3] = (netnum >> 24) & 0xFF;
        server_sockaddr.sa_netnum[2] = (netnum >> 16) & 0xFF;
        server_sockaddr.sa_netnum[1] = (netnum >> 8) & 0xFF;
        server_sockaddr.sa_netnum[0] = netnum & 0xFF;

        server_sockaddr.sa_nodenum[5] = (nodenum >> 40) & 0xFF;
        server_sockaddr.sa_nodenum[4] = (nodenum >> 32) & 0xFF;
        server_sockaddr.sa_nodenum[3] = (nodenum >> 24) & 0xFF;;
        server_sockaddr.sa_nodenum[2] = (nodenum >> 16) & 0xFF;;
        server_sockaddr.sa_nodenum[1] = (nodenum >> 8) & 0xFF;
        server_sockaddr.sa_nodenum[0] = nodenum & 0xFF;

        if (sendto(socket_descriptor,
                   data,
                   sizeof(data),
                   0,
                   (sockaddr*)&server_sockaddr,
                   sizeof(server_sockaddr)) == SOCKET_ERROR) {
            printf("Unable to connect to server: %d\nTry again", WSAGetLastError());
        } else {
            return;
        }
    }
}

void mainloop() {
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
    std::cout << "Net number: " << ((name.sa_netnum[3] << 24) | ((name.sa_netnum[2]) << 16) | ((name.sa_netnum[1]) << 8) | (name.sa_netnum[0])) << "\n";
    std::cout << "Node number: " << ((name.sa_netnum[5] << 40) | ((name.sa_netnum[4]) << 32) | ((name.sa_netnum[3]) << 24) | ((name.sa_netnum[2]) << 16) | ((name.sa_netnum[1]) << 8) | ((name.sa_netnum[0]))) << "\n";
    std::cout << "Socket num: " << htons(name.sa_socket) << "\n";
    std::cout.flush();

    send_request();
    mainloop();

    err = WSACleanup();

    if (err != 0) {
        printf("WSACleanup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    closesocket(socket_descriptor);

    std::cout << "Have a good day!" << std::endl;

    return 0;
}
