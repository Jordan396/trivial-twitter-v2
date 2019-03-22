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
  * @brief ttweetcli creates a persistent connection to ttweetser server,
  * allowing tweet, subscribe, unsubscribe, timeline and exit commands to be executed.
  *
  * This file is to be compiled and executed on the client side. For an overview of 
  * what this program does, visit <https://github.com/Jordan396/trivial-twitter-v2>.
  * 
  * ttweetcli creates a persistent connection to a ttweetser server. Once a connection
  * has been established, the ttweetcli can run the following commands:
  * 1. tweet​ "<150 char max tweet>" <Hashtag>
  *   - Upload tweet to server.
  * 2. subscribe​ <Hashtag>
  *   - Subscribe to a hashtag (max of 3).
  * 3. unsubscribe​ <Hashtag>
  *   - Unsubscribes to a hashtag.
  * 4. timeline
  *   - Output all tweets that have been sent to it by the server since the last time the user has run the ​‘timeline’​ command.
  * 5. exit
  *   - Clean up any necessary state and close the client.
  */

#include "ttweetcli.h"
#include "cJSON.h"

void DieWithError(char *errorMessage);                                                                                             /* Error handling function */
void parseHashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);                                             /* Error handling function */
void saveCurrentHashtag(char *currentHashtagBuffer, int *currentHashtagBufferIndex, char *validHashtags[], int *numValidHashtags); /* Error handling function */
void checkDuplicatesExists(char *validHashtags[], int numValidHashtags);                                                           /* Error handling function */

int main(int argc, char *argv[])
{
  char inputHashtags[] = "#1#2#pop#mom#pop";
  char *validHashtags[MAX_HASHTAG_CNT]; /* Error handling function */
  int numValidHashtags;
  '''
  json_object *jobj = json_object_new_object(); /*Creating a json object*/

  parseHashtags(validHashtags, &numValidHashtags, inputHashtags);
  for (int i = 0; i < numValidHashtags; i++)
  {
    printf("%s\n", validHashtags[i]);
  }

  json_object *jstring = json_object_new_string("Joys of Programming"); /*Creating a json string*/
  json_object *jarray = json_object_new_array();                        /*Creating a json array*/
  for (int i=0; i<numValidHashtags;i++){
    json_object_array_add(jarray, json_object_new_string(validHashtags[i]));
  }
  printf ("The json object created: %sn",json_object_to_json_string(jobj));
  '''

  //   char uploadRequestStr[32] = "vQa&yXWS5V!6P+dF-%$ArTz4$dwbebC\0";   /* Passphrase to initiate upload */
  //   char downloadRequestStr[32] = "Uep5tubUccXb=?u-x?BbsL2U-vb6j6s\0"; /* Passphrase to initiate download */
  //   int err_flag = 0;                                                  /* Flag to indicate error */
  //   int u_flag = 0;                                                    /* Flag to indicate upload */
  //   int d_flag = 0;                                                    /* Flag to indicate download */
  //   int sock;                                                          /* Socket descriptor */
  //   struct sockaddr_in ttweetServAddr;                                 /* ttweet server address */
  //   unsigned short ttweetServPort;                                     /* ttweet server port */
  //   char *servIP;                                                      /* Server IP address (dotted quad) */
  //   char *ttweetString;                                                /* String to be send to ttweet server */
  //   char ttweetBuffer[RCVBUFSIZE];                                     /* Buffer for ttweet string */
  //   unsigned int ttweetStringLen;                                      /* Length of string to ttweet */
  //   int bytesRcvd;                                                     /* Bytes read in single recv() */

  //   if ((argc < 4) || (argc > 5)) /* Test for correct number of arguments */
  //   {
  //     err_flag = 1;
  //   }

  //   if (strcmp(argv[1], "-u") == 0)
  //   { /* Upload flag set */
  //     if (argc == 5)
  //     {                                         /* Upload command expects 5 args */
  //       servIP = argv[2];                       /* Server IP address (dotted quad) */
  //       ttweetServPort = atoi(argv[3]);         /* Use given port, if any */
  //       ttweetString = argv[4];                 /* String to upload */
  //       ttweetStringLen = strlen(ttweetString); /* Determine message length */
  //       if (ttweetStringLen > 150)
  //       { /* Throw error if length exceeds 150 chars */
  //         DieWithError("Tweet exceeded 150 characters in length.");
  //       }
  //       u_flag = 1; /* No issues with input. Set u_flag */
  //     }
  //     else
  //     {
  //       err_flag = 1; /* Issues found with input. Set err_flag */
  //     }
  //   }
  //   else if (strcmp(argv[1], "-d") == 0)
  //   { /* Download flag set */
  //     if (argc == 4)
  //     {                                 /* Download command expects 4 args */
  //       servIP = argv[2];               /* Server IP address (dotted quad) */
  //       ttweetServPort = atoi(argv[3]); /* Use given port, if any */
  //       d_flag = 1;                     /* No issues with input. Set d_flag */
  //     }
  //     else
  //     {
  //       err_flag = 1; /* Issues found with input. Set err_flag */
  //     }
  //   }
  //   else
  //   {
  //     err_flag = 1; /* Unrecognized input flag. Set err_flag */
  //   }

  //   if (err_flag) /* Check if errors exist in command line args */
  //   {             /* If exists, inform user of proper usage */
  //     fprintf(stderr, "Usage for upload: %s -u <ServerIP> <ServerPort> \"message\"\n", argv[0]);
  //     fprintf(stderr, "Usage for download: %s -d <ServerIP> <ServerPort>\n", argv[0]);
  //     exit(1);
  //   }

  //   /* Create a reliable, stream socket using TCP */
  //   if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  //     DieWithError("socket() failed");

  //   /* Construct the server address structure */
  //   memset(&ttweetServAddr, 0, sizeof(ttweetServAddr)); /* Zero out structure */
  //   ttweetServAddr.sin_family = AF_INET;                /* Internet address family */
  //   ttweetServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  //   ttweetServAddr.sin_port = htons(ttweetServPort);    /* Server port */

  //   /* Establish the connection to the ttweet server */
  //   if (connect(sock, (struct sockaddr *)&ttweetServAddr, sizeof(ttweetServAddr)) < 0)
  //     DieWithError("connect() failed");

  //   /* Client is uploading a message */
  //   if (u_flag)
  //   {
  //     /* Send an authorization request to upload to the server */
  //     send(sock, uploadRequestStr, strlen(uploadRequestStr), 0);

  //     /* Send the ttweetString to the server */
  //     if (send(sock, ttweetString, ttweetStringLen, 0) != ttweetStringLen)
  //       DieWithError("send() sent a different number of bytes than expected");
  //   }

  //   /* Client is downloading a message */
  //   if (d_flag)
  //   {
  //     /* Send an authorization request to download from the server */
  //     send(sock, downloadRequestStr, strlen(downloadRequestStr), 0);
  //   }

  //   /* Client finished sending messages */
  //   shutdown(sock, SHUT_WR); /* Shutdown WR to send a FIN packet to server.
  //                                   Server can then begin writing to the client. */

  //   /* Receive message from server */
  //   while ((bytesRcvd = recv(sock, ttweetBuffer, RCVBUFSIZE - 1, 0)) > 0)
  //   {
  //     /* Receive up to the buffer size (minus 1 to leave space for
  //          a null terminator) bytes from the sender */
  //     ttweetBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
  //     printf("%s", ttweetBuffer);     /* Print the ttweetBuffer */
  //   }

  //   printf("\n"); /* Print a final linefeed */

  //   close(sock);
  //   exit(0);
}

/** \copydoc DieWithError */
void DieWithError(char *errorMessage)
{
  perror(errorMessage);
  exit(1);
}

/** \copydoc parseHashTags */
void parseHashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags)
{
  int inputHashtagsCharIndex = 1; /* Track char index in inputHashtags string */
  int numConsecutiveHashes = 1;   /* Check for duplicate # within hashtag */
  int lenInputHashtags;           /* Length of inputHashtags string */
  char currentHashtagBuffer[25];  /* Current hashtag being parsed */
  int currentHashtagBufferIndex;  /* char index of current hashtag being parsed */

  lenInputHashtags = strlen(inputHashtags);
  *numValidHashtags = 0;         /* Initialize number of valid hashtags */
  currentHashtagBufferIndex = 0; /* Initialize buffer index */

  if (inputHashtags[0] != '#')
  { /* Hashtag must begin with # */
    DieWithError("Invalid hashtag(s)! Hashtag(s) must begin with #.");
  }

  if (inputHashtags[lenInputHashtags - 1] == '#')
  { /* Hashtag cannot end with # */
    DieWithError("Invalid hashtag(s)! Hashtag(s) cannot end with #.");
  }

  if (lenInputHashtags < 2 || lenInputHashtags > 25)
  { /* Hashtag must be between 2 to 25 chars long */
    DieWithError("Invalid hashtag(s)! Hashtag(s) must be between 2 to 25 chars long.");
  }

  while (inputHashtags[inputHashtagsCharIndex] != '\0')
  { /* Not yet reached end of inputHashtags */
    if (inputHashtags[inputHashtagsCharIndex] == '#')
    {
      numConsecutiveHashes++;
      if (numConsecutiveHashes > 1)
      { /* Hashtag contains consecutive # */
        DieWithError("Invalid hashtag(s)! Hashtag(s) cannot contain consecutive #.");
      }
      else
      { /* Reached end of current hashtag. Save and prepare to parse next hashtag. */
        saveCurrentHashtag(currentHashtagBuffer, &currentHashtagBufferIndex, validHashtags, numValidHashtags);
      }
    }
    else if (isalnum(inputHashtags[inputHashtagsCharIndex]) != 0)
    { /* char is alphanumeric */
      currentHashtagBuffer[currentHashtagBufferIndex] = inputHashtags[inputHashtagsCharIndex];
      currentHashtagBufferIndex++;
      numConsecutiveHashes = 0;
    }
    else
    { /* char is not alphanumeric or # */
      DieWithError("Invalid hashtag(s)! Hashtag(s) contains invalid characters.");
    }
    inputHashtagsCharIndex++;
  }

  /* Reached end of inputHashtags */
  saveCurrentHashtag(currentHashtagBuffer, &currentHashtagBufferIndex, validHashtags, numValidHashtags);

  if (numConsecutiveHashes > 8)
  { /* Limit of 8 hashtags exceeded */
    DieWithError("Invalid hashtag(s)! Limit of 8 hashtags exceeded.");
  }

  checkDuplicatesExists(validHashtags, *numValidHashtags);
}

/** \copydoc saveCurrentHashtag */
void saveCurrentHashtag(char *currentHashtagBuffer, int *currentHashtagBufferIndex, char *validHashtags[], int *numValidHashtags)
{
  currentHashtagBuffer[*currentHashtagBufferIndex] = '\0';
  if (strcmp(currentHashtagBuffer, "ALL") == 0)
  { /* Hashtag #ALL is not allowed */
    DieWithError("Invalid hashtag(s)! Hashtag #ALL is not allowed.");
  }
  validHashtags[*numValidHashtags] = (char *)malloc((*currentHashtagBufferIndex + 1) * sizeof(char));
  strcpy(validHashtags[*numValidHashtags], currentHashtagBuffer);
  *currentHashtagBufferIndex = 0;
  (*numValidHashtags)++;
}

/** \copydoc checkDuplicatesExists */
void checkDuplicatesExists(char *validHashtags[], int numValidHashtags)
{
  for (int i = 0; i < numValidHashtags - 1; i++)
  {
    for (int j = i + 1; j < numValidHashtags; j++)
    {
      if (strcmp(validHashtags[i], validHashtags[j]) == 0)
      {
        DieWithError("Invalid hashtag(s)! Duplicate hashtags detected.");
      }
    }
  }
}
