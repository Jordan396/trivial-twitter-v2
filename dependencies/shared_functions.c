/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file shared_functions.c
  * @author Jordan396
  * @date 29 March 2019
  * @brief This file contains functions used in BOTH client and server side for trivial-twitter-v2.
  */

#include "shared_functions.h"

/** \copydoc DieWithError */
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

/** \copydoc PersistWithError */
int PersistWithError(char *errorMessage)
{
    perror(errorMessage);
    return 0;
}

void receiveResponse(int sock, char *buffer, char *message)
{
  int bytesToRecv;
  int messageIdx = 0;
  recv(sock, &bytesToRecv, sizeof(int), 0);
  while (bytesToRecv > 0)
  {
    recv(sock, buffer, RCVBUFSIZE, 0);
    if (bytesToRecv > 32)
    {
      strncpy(message + messageIdx, buffer, 32);
      messageIdx += 32;
    }
    else
    {
      strncpy(message + messageIdx, buffer, bytesToRecv);
      messageIdx += bytesToRecv;
      message[messageIdx] = '\0';
    }
    bytesToRecv -= 32;
  }
}