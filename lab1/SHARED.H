#ifndef SHARED_H
#define SHARED_H

#define CLIENT_BUFFER_MESSAGE_SIZE 64
#define SOCKET_SHORT_LIVING 0
#define IMG_BUFFER_SIZE 400

#include "IPX.H"

void destroy_ecb(ECB *ecb_to_del)
{
   if (ecb_to_del == NULL)
      return;

   free(ecb_to_del->descs[0].addr);
   free(ecb_to_del->descs[1].addr);
   free(ecb_to_del);
}

#endif