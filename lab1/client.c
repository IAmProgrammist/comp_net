#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "IPX.H"
#include "SHARED.H"

unsigned int socket_num = 0;
unsigned int server_num = 0;

// ECB для чтения сообщений
FILE *source;
ECB *reader_ecb;
ECB *to_send_esb;

// Функция-коллбек (ESR) для обработки сообщений на получение изображения.
// Здесь нам, на самом деле, важен сам факт того что мы получили сообщение.
// Теперь можем отправлять по полученному адресу картинку
void on_message_received_callback()
{
    printf("A new request for a message received\n");
    if (reader_ecb->ccode == 0xFC)
    {
        printf("A request was canceled\n");
    }
    else if (reader_ecb->ccode != 0)
    {
        printf("A request was damaged during transportation");
    }
}

// Функция для создания ECB для получения сообщений
ECB *construct_receiver_ecb()
{
    ECB *new_esb = (ECB *)malloc(sizeof(ECB));
    new_esb->ESRAddress = NULL;
    new_esb->socket = _REVERSE_BYTES(socket_num);
    memset(new_esb->immAddress, 0xff, 6);
    new_esb->fragmentCnt = 2;
    new_esb->descs[0].addr = malloc(sizeof(IPXHeader));
    new_esb->descs[0].size = sizeof(IPXHeader);
    new_esb->descs[1].addr = malloc(IMG_BUFFER_SIZE);
    new_esb->descs[1].size = IMG_BUFFER_SIZE;

    return new_esb;
}

ECB *construct_request_ecb(int chosen_socket_num, char *chosen_immediate_address, IPXHeader *chosen_ipx_header)
{
    ECB *new_esb = (ECB *)calloc(1, sizeof(ECB));
    new_esb->ESRAddress = NULL;
    new_esb->socket = _REVERSE_BYTES(socket_num);
    new_esb->fragmentCnt = 2;
    new_esb->descs[0].addr = chosen_ipx_header;
    new_esb->descs[0].size = sizeof(IPXHeader);
    new_esb->descs[1].addr = malloc(CLIENT_BUFFER_MESSAGE_SIZE);
    new_esb->descs[1].size = CLIENT_BUFFER_MESSAGE_SIZE;
    sprintf(new_esb->descs[1].addr, "I need cat picture pls ._.");

    return new_esb;
}

void send_request_for_image()
{
    IPXHeader header;
    printf("Sending request to server...\n");

    header.packetType = 4;
    memset(header.destNetwork, 0, 4);
    memset(header.destNode, 0xff, 6);
    header.destSocket = _REVERSE_BYTES(server_num);

    to_send_esb = construct_request_ecb(header.destSocket, header.destNode, &header);

    IPXSendPacket(to_send_esb);

    while (to_send_esb->inUse)
    {
        IPXRelinquishControl();
    }

    if (to_send_esb->ccode == 0)
    {
        printf("A request is accepted successfully\n");
    }
    else
    {
        printf("Unable to send a request to a server. No cat image for you, sorry :(\n");
        exit(1);
    }

    return;
}

void go_main_loop()
{
    ECB *accept_ecb = construct_receiver_ecb();
    printf("Starting file accept");

    while (1)
    {
        IPXListenForPacket(accept_ecb);

        while (accept_ecb->inUse)
        {
            IPXRelinquishControl();
            if (kbhit())
            {
               IPXCancelEvent(accept_ecb);
               return;
            }
        }

        if (!accept_ecb->ccode)
        {
            fwrite(accept_ecb->descs[1].addr, 1, accept_ecb->descs[1].size, source);
        }
    }

    destroy_ecb(accept_ecb);
}

int main()
{
    char filename[20];
    unsigned char socket_opening_result = 0;

    // Проверяем, доступен ли IPX
    if (IPXInit() != 0xFF)
    {
        printf("IPX is not found");

        return 1;
    }

    // Пытаемся открыть сокет
    if ((socket_opening_result = IPXOpenSocket(SOCKET_SHORT_LIVING, &socket_num)) == 0)
    {
        printf("Socket succesfully opened at: %hu (%hx in hex)\n", socket_num, socket_num);
    }
    else if (socket_opening_result == 0xFF)
    {
        printf("Socket is already taken\n");
        return 1;
    }
    else if (socket_opening_result == 0xFE)
    {
        printf("Socket table overflow\n");
        return 1;
    }

    printf("Input server socket: ");
    fflush(stdout);

    scanf("%hu", &server_num);

    srand(time(NULL));
    sprintf(filename, "img%d.jpg", rand() % 100 + 1);
    source = fopen(filename, "wb");

    send_request_for_image();
    go_main_loop();

    printf("A file was saved as %s\n", filename);

    fflush(source);
    fclose(source);
    IPXCloseSocket(socket_num);

    return 0;
}