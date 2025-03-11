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

SOCKET socket_descriptor;
SOCKADDR_IPX name = {};

FILE *source;

void mainloop() {
    std::cout << "Mainloop started..." << std::endl;
    char* accept_client_data = (char*) malloc(sizeof(char) * CLIENT_REQUEST_SIZE);
    bool should_run = true;
    int bytes_read, bytes_sent;
    char* image_buffer = (char*) malloc(sizeof(char) * IMAGE_PART_SIZE);

    while(should_run) {
        SOCKADDR_IPX client_sockaddr = {};
        client_sockaddr.sa_family = AF_IPX;
        int client_sockaddr_size = sizeof(client_sockaddr);

        if (recvfrom(
                 socket_descriptor,
                 accept_client_data,
                 sizeof(char) * CLIENT_REQUEST_SIZE,
                 0,
                 (sockaddr*)&client_sockaddr,
                 &client_sockaddr_size
                 ) == SOCKET_ERROR) {
            printf("recvfrom failed with error: %d\n", WSAGetLastError());
        } else {
            std::cout << "A request was accepted:\n";
            printf("Net number: %02hhx %02hhx %02hhx %02hhx\n", client_sockaddr.sa_netnum[0], client_sockaddr.sa_netnum[1], client_sockaddr.sa_netnum[2], client_sockaddr.sa_netnum[3]);
            printf("Node number: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx\n", client_sockaddr.sa_nodenum[0], client_sockaddr.sa_nodenum[1], client_sockaddr.sa_nodenum[2], client_sockaddr.sa_nodenum[3], client_sockaddr.sa_nodenum[4], client_sockaddr.sa_nodenum[5]);
            std::cout << "Socket num: " << htons(client_sockaddr.sa_socket) << "\n";
            std::cout.flush();

            int packages_success = 0, packages_error = 0;
            fseek(source, 0, SEEK_SET);

            std::cout << "Waiting for client to set up..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            auto a = std::chrono::high_resolution_clock::now();
            while ((bytes_read = fread(image_buffer, 1, sizeof(char) * IMAGE_PART_SIZE, source))) {
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

        if (kbhit()) {
            should_run = false;
        }
    }

    std::cout << "Mainloop prevented" << std::endl;

    free(image_buffer);
    free(accept_client_data);
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

    int namelen;
    getsockname(socket_descriptor, (sockaddr*)(&name), &namelen);
    std::cout << "Server is inited:\n";
    printf("Net number: %02hhx %02hhx %02hhx %02hhx\n", name.sa_netnum[0], name.sa_netnum[1], name.sa_netnum[2], name.sa_netnum[3]);
    printf("Node number: %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx\n", name.sa_nodenum[0], name.sa_nodenum[1], name.sa_nodenum[2], name.sa_nodenum[3], name.sa_nodenum[4], name.sa_nodenum[5]);
    std::cout << "Socket num: " << htons(name.sa_socket) << "\n";
    std::cout.flush();

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
