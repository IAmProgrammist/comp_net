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
#define BACKLOG_MAX_SIZE

#define IPX_SOCKET (0x8060)
#define SPX_SOCKET (0x5E86)


SOCKET socket_descriptor;
SOCKADDR_IPX name = {};
SOCKADDR_IPX server_sockaddr = {};

SOCKADDR_IPX server_address;

void send_request() {
    char data[CLIENT_REQUEST_SIZE] = "What is the Music of Life?";

    while (true) {
        std::cout << "Input netnum: ";
        std::cout.flush();
        scanf("%02hhx %02hhx %02hhx %02hhx", server_sockaddr.sa_netnum, server_sockaddr.sa_netnum + 1, server_sockaddr.sa_netnum + 2, server_sockaddr.sa_netnum + 3);

        memset(server_sockaddr.sa_nodenum, 0xFF, 6);
        server_sockaddr.sa_socket = htons(IPX_SOCKET);
        server_sockaddr.sa_family = AF_IPX;

        if (sendto(socket_descriptor,
                   data,
                   CLIENT_REQUEST_SIZE * sizeof(char),
                   0,
                   (sockaddr*)&server_sockaddr,
                   sizeof(server_sockaddr)) == SOCKET_ERROR) {
            printf("Unable to connect to server: %d\nTry again\n", WSAGetLastError());
        } else {
            return;
        }
    }
}

void rec_request() {
    std::cout << "Starting answer accept..." << std::endl;
    char* buffer = (char*)malloc(sizeof(char) * IMAGE_PART_SIZE);
    while (1) {
        SOCKADDR_IPX receive_name = {};
        receive_name.sa_family = AF_IPX;
        int receive_name_size = sizeof(receive_name);

        int bytes_received;
        if (bytes_received = recvfrom(
                 socket_descriptor,
                 buffer,
                 sizeof(char) * IMAGE_PART_SIZE,
                 0,
                 (SOCKADDR*) &receive_name,
                 &receive_name_size) != SOCKET_ERROR) {
                     if (strcmp(buffer, "Silence, my Brother.") == 0) {
                        server_address = receive_name;
                        server_address.sa_socket = htons(SPX_SOCKET);
                        server_address.sa_family = AF_IPX;
                        break;
                     }
        } else {
            break;
        }
    }
}

void request_file() {
    SOCKADDR_IPX rf_name = {};

    SOCKET socket_descriptor = socket(
                                    AF_IPX,
                                    SOCK_SEQPACKET,
                                    NSPROTO_SPX
                                    );
    rf_name.sa_family = AF_IPX;
    if (bind(socket_descriptor, (sockaddr*)&rf_name, sizeof(rf_name)) == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        return;
    }

    if (connect(socket_descriptor, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        printf("Failed to connect to server %d\n", WSAGetLastError());
        closesocket(socket_descriptor);
        return;
    }

    std::cout << "Server connection established\n";

    std::string filename;
    std::cout << "Input filename: ";
    std::cout.flush();
    std::cin >> filename;

    FILE* save_file = fopen(filename.c_str(), "wb");

    if (!save_file) {
        std::cout << "Unable to open file for saving" << std::endl;
        closesocket(socket_descriptor);
        return;
    }

    if (send(socket_descriptor, filename.c_str(), filename.size() + 1, 0) == SOCKET_ERROR) {
        printf("Failed to send request for file %d\n", WSAGetLastError());
        closesocket(socket_descriptor);
        return;
    }

    int diff = 0;
    int total_diff = 0;
    int bytes_read;
    char image_buffer[IMAGE_PART_SIZE];
    while ((bytes_read = recv(socket_descriptor, image_buffer, sizeof(char) * IMAGE_PART_SIZE, 0)) > 0) {
        diff += bytes_read;
        total_diff += bytes_read;

        while (diff > 1000000) {
            std::cout << "Accepted " << total_diff / 1024 << " KBs..." << std::endl;
            diff -= 1000000;
        }

        fwrite(image_buffer, sizeof(char), bytes_read, save_file);
    }

    std::cout << "File accepted succesfully! Saving result into '" << filename << "'" << std::endl;

    fflush(save_file);
    fclose(save_file);

    closesocket(socket_descriptor);
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

    int timeout_time = 4000;
    bool allow_broadcast = true;

    setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout_time, sizeof(timeout_time));
    setsockopt(socket_descriptor, SOL_SOCKET, SO_BROADCAST, (char*) &allow_broadcast, sizeof(allow_broadcast));

    send_request();
    rec_request();
    request_file();

    err = WSACleanup();

    if (err != 0) {
        printf("WSACleanup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    closesocket(socket_descriptor);

    std::cout << "Have a good day!" << std::endl;

    return 0;
}
