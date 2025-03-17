#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <wsipx.h>
#include <stdlib.h>
#include <conio.h>
#include <iomanip>
#include <chrono>
#include <thread>

#define CLIENT_REQUEST_SIZE 512
#define IMAGE_PART_SIZE 400
#define IPX_SOCKET (0x8060)

SOCKET socket_descriptor;
SOCKADDR_IPX name = {};

FILE *source;

void mainloop() {
    bool should_run = true;
    int bytes_read;

    while(should_run) {
        std::string input;
        std::cout << "Input Y to send file\nType anything else to stop server: ";
        std::cout.flush();
        std::cin >> input;

        if (input != "Y") {
            break;
        }

        SOCKADDR_IPX client_sockaddr = {};
        client_sockaddr.sa_family = AF_IPX;
        memset(client_sockaddr.sa_netnum, 0, 4);
        memset(client_sockaddr.sa_nodenum, 0xFF, 6);
        client_sockaddr.sa_socket = htons(IPX_SOCKET);

        int client_sockaddr_size = sizeof(client_sockaddr);

        int packages_success = 0, packages_error = 0;
        fseek(source, 0, SEEK_SET);

        auto a = std::chrono::high_resolution_clock::now();
        char image_buffer[IMAGE_PART_SIZE];

        while ((bytes_read = fread(image_buffer, sizeof(char), IMAGE_PART_SIZE, source))) {
            if (sendto(socket_descriptor,
                   image_buffer,
                   bytes_read,
                   0,
                   (sockaddr*)&client_sockaddr,
                   client_sockaddr_size) == SOCKET_ERROR) {
                packages_error++;
            } else {
                packages_success++;
            }
        }
        auto b = std::chrono::high_resolution_clock::now();

        std::cout <<
        "Image sent\nSuccessfully sent: " << packages_success <<
        "\nFailed to send: " << packages_error <<
        "\nTime: " << std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count() / 1000.0 << " s." << std::endl;
    }
}

int main()
{
    source = fopen("cat.jpg", "rb");
    if (!source) {
        std::cout << "Cat image not found :(" << std::endl;
        return 1;
    }

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

    int namelen = sizeof(name);
    getsockname(socket_descriptor, (sockaddr*)(&name), &namelen);
    std::cout << "Server is inited" << std::endl;

    bool broadcast = true;
    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR) {
        printf("Unable to set broadcast\n");
        closesocket(socket_descriptor);
        WSACleanup();
    }

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
