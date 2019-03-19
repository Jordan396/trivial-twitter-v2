/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
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
  * @file ttweetcli.c
  * @author Jordan396
  * @date 14 February 2019
  * @brief ttweetcli sends upload/download requests to ttweetser server.
  *
  * This file is to be compiled and executed on the client side.
  * ttweetcli is to be executed once for each download/upload request.
  * When a request is sent, ttweetser checks if request is unauthorized or authorized.
  * If authorized, ttweetser checks if request is to upload or download.
  * If download, ttweetcli receives and displays the last stored message on ttweetser.
  * If upload, ttweetcli sends the user's message to ttweetser where it is stored.
  * Note that ttweetMessage cannot exceed 150 characters, or an error will be thrown.
  * If unauthorized, client receives and displays an unauthorizedRequestMessage.
  */

#include "ttweetcli.h"

void DieWithError(char *errorMessage);  /* Error handling function */

int main(int argc, char *argv[])
{
    char uploadRequestStr[32] = "vQa&yXWS5V!6P+dF-%$ArTz4$dwbebC\0";        /* Passphrase to initiate upload */
    char downloadRequestStr[32] = "Uep5tubUccXb=?u-x?BbsL2U-vb6j6s\0";      /* Passphrase to initiate download */
    int err_flag = 0;                                                       /* Flag to indicate error */
    int u_flag = 0;                                                         /* Flag to indicate upload */
    int d_flag = 0;                                                         /* Flag to indicate download */
    int sock;                                                               /* Socket descriptor */
    struct sockaddr_in ttweetServAddr;                                      /* ttweet server address */
    unsigned short ttweetServPort;                                          /* ttweet server port */
    char *servIP;                                                           /* Server IP address (dotted quad) */
    char *ttweetString;                                                     /* String to be send to ttweet server */
    char ttweetBuffer[RCVBUFSIZE];                                          /* Buffer for ttweet string */
    unsigned int ttweetStringLen;                                           /* Length of string to ttweet */
    int bytesRcvd;                                                          /* Bytes read in single recv() */

    if ((argc < 4) || (argc > 5))    /* Test for correct number of arguments */
    {
       err_flag = 1;
    }

    if (strcmp(argv[1], "-u") == 0){                 /* Upload flag set */
      if (argc == 5){                                /* Upload command expects 5 args */
        servIP = argv[2];                            /* Server IP address (dotted quad) */
        ttweetServPort = atoi(argv[3]);              /* Use given port, if any */
        ttweetString = argv[4];                      /* String to upload */
        ttweetStringLen = strlen(ttweetString);      /* Determine message length */
        if (ttweetStringLen > 150){                  /* Throw error if length exceeds 150 chars */
          DieWithError("Tweet exceeded 150 characters in length.");
        }
        u_flag = 1;                                  /* No issues with input. Set u_flag */
      }
      else {
        err_flag = 1;                                /* Issues found with input. Set err_flag */
      }
    } else if (strcmp(argv[1], "-d") == 0){          /* Download flag set */
      if (argc == 4){                                /* Download command expects 4 args */
        servIP = argv[2];                            /* Server IP address (dotted quad) */
        ttweetServPort = atoi(argv[3]);              /* Use given port, if any */
        d_flag = 1;                                  /* No issues with input. Set d_flag */
      }
      else {
        err_flag = 1;                                /* Issues found with input. Set err_flag */
      }
    } else {
      err_flag = 1;                                  /* Unrecognized input flag. Set err_flag */
    }

    if (err_flag)                                    /* Check if errors exist in command line args */
    {                                                /* If exists, inform user of proper usage */
       fprintf(stderr, "Usage for upload: %s -u <ServerIP> <ServerPort> \"message\"\n", argv[0]);
       fprintf(stderr, "Usage for download: %s -d <ServerIP> <ServerPort>\n", argv[0]);
       exit(1);
    }

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&ttweetServAddr, 0, sizeof(ttweetServAddr));     /* Zero out structure */
    ttweetServAddr.sin_family      = AF_INET;               /* Internet address family */
    ttweetServAddr.sin_addr.s_addr = inet_addr(servIP);     /* Server IP address */
    ttweetServAddr.sin_port        = htons(ttweetServPort); /* Server port */

    /* Establish the connection to the ttweet server */
    if (connect(sock, (struct sockaddr *) &ttweetServAddr, sizeof(ttweetServAddr)) < 0)
        DieWithError("connect() failed");

    /* Client is uploading a message */
    if (u_flag){
      /* Send an authorization request to upload to the server */
      send(sock, uploadRequestStr, strlen(uploadRequestStr), 0);

      /* Send the ttweetString to the server */
      if (send(sock, ttweetString, ttweetStringLen, 0) != ttweetStringLen)
          DieWithError("send() sent a different number of bytes than expected");
    }

    /* Client is downloading a message */
    if (d_flag){
      /* Send an authorization request to download from the server */
      send(sock, downloadRequestStr, strlen(downloadRequestStr), 0);
    }

    /* Client finished sending messages */
    shutdown(sock, SHUT_WR);   /* Shutdown WR to send a FIN packet to server.
                                  Server can then begin writing to the client. */

    /* Receive message from server */
    while ((bytesRcvd = recv(sock, ttweetBuffer, RCVBUFSIZE - 1, 0)) > 0){
      /* Receive up to the buffer size (minus 1 to leave space for
         a null terminator) bytes from the sender */
      ttweetBuffer[bytesRcvd] = '\0';  /* Terminate the string! */
      printf("%s", ttweetBuffer);      /* Print the ttweetBuffer */
    }

    printf("\n");    /* Print a final linefeed */

    close(sock);
    exit(0);
}

/** \copydoc DieWithError */
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
