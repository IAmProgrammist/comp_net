#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "IPX.H"
#include "SHARED.H"

// Картинка с котиком (очень нужна клиенту)
FILE *source;

// Порт или сокет
unsigned int socket_num = 0;

// Должен ли работать мейнлуп
unsigned char should_run = 1;

// ECB для чтения сообщений
ECB *reader_ecb = NULL;

// Функция для создания ECB для получения сообщений
ECB *construct_receiver_ecb();

// Очистка ECB для получения сообщений
void destroy_ecb(ECB *ecb_to_del);

// Мейнлуп, создает ECB-читалку и ожидает пользовательского
// ввода для прекращения работы сервера
void go_to_mainloop();

// Функция-коллбек (ESR) для обработки сообщений на получение изображения.
// Здесь нам, на самом деле, важен сам факт того что мы получили сообщение.
// Теперь можем отправлять по полученному адресу картинку
void on_message_received_callback();

ECB *construct_receiver_ecb()
{
   ECB *new_esb = (ECB *)malloc(sizeof(ECB));
   new_esb->ESRAddress = NULL;
   new_esb->socket = _REVERSE_BYTES(socket_num);
   memset(new_esb->immAddress, 0xff, 6);
   new_esb->fragmentCnt = 2;
   new_esb->descs[0].addr = malloc(sizeof(IPXHeader));
   new_esb->descs[0].size = sizeof(IPXHeader);
   new_esb->descs[1].addr = malloc(CLIENT_BUFFER_MESSAGE_SIZE);
   new_esb->descs[1].size = CLIENT_BUFFER_MESSAGE_SIZE;

   return new_esb;
}

void on_message_received_callback()
{
   int bytes_read;
   IPXHeader header;
   int packages_sent = 0, failed_packages_send = 0;
   ECB *construct_image_ecb = (ECB *)calloc(1, sizeof(ECB));
   printf("A new request for an image received\n");
   printf("A message: %s\n", reader_ecb->descs[1].addr);
   if (reader_ecb->ccode == 0xFC)
   {
      free(construct_image_ecb);
      printf("A request was canceled\n");

      return;
   }
   else if (reader_ecb->ccode != 0)
   {
      free(construct_image_ecb);
      printf("A request was damaged during transportation");

      return;
   }

   // Сделаем маленькую задержку чтобы клиент успел перевести своё состояние в прослушку.
   // лучше чем ничего
   delay(100);

   // Инициализация ECB
   construct_image_ecb->ESRAddress = NULL;
   construct_image_ecb->socket = _REVERSE_BYTES(socket_num);
   memset(construct_image_ecb->immAddress, 0xff, 6);
   construct_image_ecb->fragmentCnt = 2;
   construct_image_ecb->descs[0].addr = &header;
   construct_image_ecb->descs[0].size = sizeof(IPXHeader);
   construct_image_ecb->descs[1].addr = malloc(IMG_BUFFER_SIZE);
   construct_image_ecb->descs[1].size = IMG_BUFFER_SIZE;

   // Инициализация IPXHeader
   header.packetType = 4;
   memset(header.destNetwork, 0, 4);
   memset(header.destNode, 0xff, 6);
   header.destSocket = ((IPXHeader *)reader_ecb->descs[0].addr)->sourceSocket;

   fseek(source, 0, SEEK_SET);

   printf("Package sending set up complete\n");

   delay(100);

   while ((bytes_read = fread(construct_image_ecb->descs[1].addr, 1, IMG_BUFFER_SIZE, source)))
   {
      IPXSendPacket(construct_image_ecb);

      while (construct_image_ecb->inUse)
      {
         IPXRelinquishControl();
      }

      if (!construct_image_ecb->ccode)
      {
         packages_sent++;
      }
      else
      {
         failed_packages_send++;
      }
   }

   free(construct_image_ecb->descs[1].addr);
   free(construct_image_ecb);

   printf("Request was obtained\nTotal packages:     %d\nSucceeded packages: %d\nFailed packages:    %d\n\n",
          packages_sent + failed_packages_send,
          packages_sent,
          failed_packages_send);
}

void go_to_mainloop()
{
   reader_ecb = construct_receiver_ecb();

   IPXListenForPacket(reader_ecb);

   while (should_run)
   {
      printf("Awaiting requests...\n");
      while (reader_ecb->inUse && should_run)
      {
         IPXRelinquishControl();
         if (kbhit())
         {
            should_run = 0;
         }
      }
      on_message_received_callback();
      IPXListenForPacket(reader_ecb);
   }

   printf("Have a good day!\n");
   exit(0);
}

int main()
{
   unsigned char socket_opening_result = 0;
   int i = 0;

   source = fopen(".\\cat.jpg", "rb");
   if (!source)
   {
      printf("Cat image not found :(\n");
      return 1;
   }

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

   go_to_mainloop();

   IPXCancelEvent(reader_ecb);
   IPXCloseSocket(socket_num);

   fclose(source);

   return 0;
}
