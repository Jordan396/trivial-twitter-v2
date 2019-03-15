/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/Trivial-Twitter-Sockets>*
 *                                                                          *
 * This file was heavily adapted from the source code in:                   *
 *   "TCP/IP Sockets in C: Practical Guide for Programmers"                 *
 *   by Michael J. Donahoo and Kenneth L. Calvert                           *
 *   <http://cs.baylor.edu/~donahoo/practical/CSockets/textcode.html>       *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

 /**
  * @file ttweetser.c
  * @author Jordan396
  * @date 14 February 2019
  * @brief ttweetser handles upload/download requests from ttweetcli clients.
  *
  * This file is to be compiled and executed on the server side.
  * ttweetser runs continuously to listen for client requests. When a client
  * sends a request, ttweetser checks if request is unauthorized or authorized.
  * If authorized, check if request is to upload or download. If download, send ttweetMessage
  * to the client. If upload, receive message from the client and store in ttweetMessage.
  * If unauthorized, notify client of unauthorized request via unauthorizedRequestMessage.
  */

#include "ttweetser.h"

void DieWithError(char *errorMessage);                                                                         /* Error handling function */
void HandleTCPClient(int clntSocket, char *ttweetMessage, char *uploadRequestStr, char *downloadRequestStr);   /* TCP client handling function */

int main(int argc, char *argv[])
{
    char uploadRequestStr[32] = "vQa&yXWS5V!6P+dF-%$ArTz4$dwbebC\0";    /* Passphrase to initiate upload */
    char downloadRequestStr[32] = "Uep5tubUccXb=?u-x?BbsL2U-vb6j6s\0";  /* Passphrase to initiate download */
    int servSock;                                                       /* Socket descriptor for server */
    int clntSock;                                                       /* Socket descriptor for client */
    struct sockaddr_in ttweetServAddr;                                  /* Local address */
    struct sockaddr_in ttweetClntAddr;                                  /* Client address */
    unsigned short ttweetServPort;                                      /* Server port */
    unsigned int clntLen;                                               /* Length of client address data structure */
    char ttweetMessage[151];                                            /* ttweetMessage stored on server.
                                                                           151st bit is for \0 terminator */

    /* Initialize ttweetMessage as empty */
    ttweetMessage[0] = '\0';

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    ttweetServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&ttweetServAddr, 0, sizeof(ttweetServAddr));   /* Zero out structure */
    ttweetServAddr.sin_family = AF_INET;                  /* Internet address family */
    ttweetServAddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* Any incoming interface */
    ttweetServAddr.sin_port = htons(ttweetServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &ttweetServAddr, sizeof(ttweetServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(ttweetClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &ttweetClntAddr,
                               &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(ttweetClntAddr.sin_addr));

        HandleTCPClient(clntSock, ttweetMessage, uploadRequestStr, downloadRequestStr);
    }
}

/** \copydoc DieWithError */
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

/** \copydoc HandleTCPClient */
void HandleTCPClient(int clntSocket, char *ttweetMessage, char *uploadRequestStr, char *downloadRequestStr)
{
    char ttweetBuffer[RCVBUFSIZE];                                /* Buffer for ttweet string */
    char clientMessage[200];                                      /* Raw message received from the client */
    int recvMsgSize;                                              /* Size of received message */
    int ttweetMessagePosition = 0;                                /* Cursor to track position in ttweetMessage */
    int clientMessagePosition = 0;                                /* Cursor to track position in clientMessage */
    char emptyMessage[] = "EMPTY Message";                        /* Message to be sent when ttweetMessage is empty */
    char uploadSuccessMessage[] = "Upload Successful";            /* Message to be sent when upload succeeds */
    char unauthorizedRequestMessage[] = "Unauthorized request!";  /* Message to be sent when client and server passphrases fail to match */
    unsigned int ttweetStringLen;                                 /* Length of string to ttweet */

    /* Receive entire message from client */
    while ((recvMsgSize = recv(clntSocket, ttweetBuffer, RCVBUFSIZE, 0)) > 0)      /* zero indicates end of transmission */
    {
      /* Transfer message to clientMessage */
      strncpy(clientMessage + clientMessagePosition, ttweetBuffer, recvMsgSize);
      clientMessagePosition += recvMsgSize;
    }

    /* Mark the end of clientMessage */
    clientMessage[clientMessagePosition] = '\0';

    /* clientMessage is smaller than passphrase */
    if (strlen(clientMessage) < 31){
      printf("Request unauthorized!\n");
    }

    /* Check if upload request is valid. If valid, store message in ttweetMessage */
    if(strncmp(clientMessage,uploadRequestStr, 31) == 0){           /* Upload request is valid */
      /* Transfer message to ttweetMessage */
      strcpy(ttweetMessage, clientMessage + 31);

      /* Send message to notify client of successful upload */
      int uploadSuccessMessageLen = strlen(uploadSuccessMessage);
      if (send(clntSocket, uploadSuccessMessage, uploadSuccessMessageLen, 0) != uploadSuccessMessageLen)
          DieWithError("send() failed");
    }
    /* Check if download request is valid. If valid, send ttweetMessage to client */
    else if (strncmp(clientMessage, downloadRequestStr, 31) == 0) {  /* Download request is valid */
      if (ttweetMessage[0] == '\0') {   /* ttweetMessage is empty */
        /* Send emptyMessage to client */
        if (send(clntSocket, emptyMessage, strlen(emptyMessage), 0) != strlen(emptyMessage))
            DieWithError("send() failed");
      }
      else {     /* ttweetMessage is not empty */
        ttweetStringLen = strlen(ttweetMessage);
        while (1)      /* Runs until ttweetMessage has been sent */
        {
          if (ttweetStringLen >= 32){ /* Remaining ttweetMessage exceeds buffer size. More messages will be sent after this */
            /* Transfer ttweetMessage to ttweetBuffer */
            strncpy(ttweetBuffer, ttweetMessage + ttweetMessagePosition, 31);

            /* Send message to client */
            if (send(clntSocket, ttweetBuffer, 31, 0) != 31)
                DieWithError("send() failed");

            /* Record last-sent position of ttweetMessage */
            ttweetMessagePosition += 31;
            ttweetStringLen = strlen(ttweetMessage + ttweetMessagePosition);
          }
          else { /* Remaining ttweetMessage fits into buffer size. This shall be the final message */
            /* Transfer ttweetMessage to ttweetBuffer */
            strncpy(ttweetBuffer, ttweetMessage + ttweetMessagePosition, ttweetStringLen);

            /* Send message to client */
            if (send(clntSocket, ttweetBuffer, ttweetStringLen, 0) != ttweetStringLen)
                DieWithError("send() failed");

            /* Entire ttweetMessage has been sent. Exit loop */
            break;
          }
      }
    }
  }
  else {    /* Passphrases do not match */
    printf("Request unauthorized!\n");

    /* Send unauthorizedRequestMessage to client */
    int unauthorizedRequestMessageLen = strlen(unauthorizedRequestMessage);
    if (send(clntSocket, unauthorizedRequestMessage, unauthorizedRequestMessageLen, 0) != unauthorizedRequestMessageLen)
        DieWithError("send() failed");
  }

  close(clntSocket);    /* Close client socket */
}
