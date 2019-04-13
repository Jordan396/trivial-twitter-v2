/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file ttweet_common.c
  * @author Jordan396
  * @date 29 March 2019
  * @brief This file contains functions used in both client and server side for trivial-twitter-v2.
  */

#include "ttweet_common.h"

/** \copydoc die_with_error */
void die_with_error(char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}

/** \copydoc persist_with_error */
int persist_with_error(char *errorMessage)
{
  perror(errorMessage);
  return 0;
}

int send_payload(int sock, cJSON *jobjToSend)
{
  char buffer[RCV_BUF_SIZE];
  char *request = cJSON_PrintUnformatted(jobjToSend);
  int requestSize = strlen(request) + 1;

  sprintf(buffer, "%d", requestSize);
  if (send(sock, buffer, RCV_BUF_SIZE, 0) != RCV_BUF_SIZE)
    return persist_with_error("Block size: send() sent a different number of bytes than expected.\n");
  if (send(sock, request, requestSize, 0) != requestSize)
    return persist_with_error("Block contents: send() sent a different number of bytes than expected.\n");
  return 1;
}

void waitFor(unsigned int secs)
{
  unsigned int retTime = time(0) + secs; // Get finishing time.
  while (time(0) < retTime)
    ; // Loop until it arrives.
}

void receive_response(int sock, char *objReceived)
{
  int bytesToRecv = 0;
  int responseIdx = 0;
  char buffer[RCV_BUF_SIZE];   /* Buffer for ttweet string */
  char response[MAX_RESP_LEN]; /* Stores the entire response */

  while (bytesToRecv <= 0)
  {
    recv(sock, buffer, RCV_BUF_SIZE, 0);
    bytesToRecv = atoi(buffer);
    waitFor(3);
  }

  while (bytesToRecv > 0)
  {
    if (bytesToRecv > RCV_BUF_SIZE)
    {
      recv(sock, buffer, RCV_BUF_SIZE, 0);
      strncpy(response + responseIdx, buffer, RCV_BUF_SIZE);
      responseIdx += RCV_BUF_SIZE;
    }
    else
    {
      recv(sock, buffer, bytesToRecv, 0);
      strncpy(response + responseIdx, buffer, bytesToRecv);
      responseIdx += bytesToRecv;
    }
    bytesToRecv -= RCV_BUF_SIZE;
  }
  strncpy(objReceived, response, sizeof(response));
}

