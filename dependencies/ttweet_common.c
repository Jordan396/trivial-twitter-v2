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

#include <ttweet_common.h>

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
  int jobjToSendSize = sizeof(jobjToSend);
  if (send(sock, &jobjToSendSize, sizeof(int), 0) != sizeof(int))
    return persist_with_error("Block size: send() sent a different number of bytes than expected");
  if (send(sock, jobjToSend, jobjToSendSize, 0) != jobjToSendSize)
    return persist_with_error("Block contents: send() sent a different number of bytes than expected");
}

void receive_response(int sock, cJSON *jobjReceived)
{
  int bytesToRecv;
  int responseIdx = 0;
  char buffer[RCV_BUF_SIZE];   /* Buffer for ttweet string */
  char response[MAX_RESP_LEN]; /* Stores the entire response */

  recv(sock, &bytesToRecv, sizeof(int), 0);
  while (bytesToRecv > 0)
  {
    recv(sock, buffer, RCV_BUF_SIZE, 0);
    if (bytesToRecv > RCV_BUF_SIZE)
    {
      strncpy(response + responseIdx, buffer, 32);
      responseIdx += RCV_BUF_SIZE;
    }
    else
    {
      strncpy(response + responseIdx, buffer, bytesToRecv);
      responseIdx += bytesToRecv;
      response[responseIdx] = '\0';
    }
    bytesToRecv -= RCV_BUF_SIZE;
  }

  jobjReceived = cJSON_Parse(response);
  printf("JSON response: %s\n", cJSON_Print(jobjReceived));
}