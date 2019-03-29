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
#include "errorhandling.h"
#include "cJSON.h"

void parseHashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);                                             /* Parses hashtags from user input */
void saveCurrentHashtag(char *currentHashtagBuffer, int *currentHashtagBufferIndex, char *validHashtags[], int *numValidHashtags); /* Save current hashtag buffer */
void ttweetToJson(cJSON *jobj, char ttweetString[], char *validHashtags[], int numValidHashtags);                                  /* Converts user input to a ttweet JSON object */
void resetClientVariables(char *validHashtags[], int *numValidHashtags, cJSON *jobj);                                              /* Resets client variables to prepare for the next command */
void deallocateStringArray(char *stringArray[], int numStringsInArray);                                                            /* Deallocates memory from a dynamic string array */
int duplicateStringExists(char *stringArray[], int numStringsInArray);                                                             /* Checks for duplicates in string array */

int main(int argc, char *argv[])
{
  /* Socket variables */
  int sock;                          /* Socket descriptor */
  struct sockaddr_in ttweetServAddr; /* ttweet server address */
  unsigned short ttweetServPort;     /* ttweet server port */
  char *servIP;                      /* Server IP address (dotted quad) */

  /* Variables to process user commands */
  int clientCommandCode;
  cJSON *jobj;                          /* JSON object to store ttweet */
  char ttweetString[MAX_TWEET_LEN];     /* String to be send to ttweet server */
  unsigned int ttweetStringLen;         /* Length of string to ttweet */
  char *validHashtags[MAX_HASHTAG_CNT]; /* Array of valid hashtags */
  int numValidHashtags;                 /* Number of valid hashtags */
  char targetHashtag[MAX_HASHTAG_LEN];
  char *inputHashtags; /* User input for hashtags */
  char username;
  int usernameLen;

  /* Variables used to handle transfer of data over TCP */
  char ttweetBuffer[RCVBUFSIZE]; /* Buffer for ttweet string */

  if (argc != 3) /* Test for correct number of arguments */
  {
    DieWithError("Command not recognized!\nUsage: $./ttweetcl <ServerIP> <ServerPort> <Username>");
  }

  servIP = argv[1];               /* Server IP address (dotted quad) */
  ttweetServPort = atoi(argv[2]); /* Use given port, if any */
  username = argv[3];             /* Parse username */
  usernameLen = strlen(username); /* Assign username length */

  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithError("socket() failed");

  /* Construct the server address structure */
  memset(&ttweetServAddr, 0, sizeof(ttweetServAddr)); /* Zero out structure */
  ttweetServAddr.sin_family = AF_INET;                /* Internet address family */
  ttweetServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  ttweetServAddr.sin_port = htons(ttweetServPort);    /* Server port */

  /* Establish the connection to the ttweet server */
  if (connect(sock, (struct sockaddr *)&ttweetServAddr, sizeof(ttweetServAddr)) < 0)
    DieWithError("connect() failed");

  /* Username length is sent to inform server when send() completes */
  short sz = htons(usernameLen); /* deals with possible endianness problems */
  if (send(sock, &sz, sizeof(short), 0) != sizeof(short))
    DieWithError("Block size: send() sent a different number of bytes than expected");
  /* Send username to server for validation */
  if (send(sock, username, usernameLen, 0) != usernameLen)
    DieWithError("Block contents: send() sent a different number of bytes than expected");
  /* Process validation code from server */
  validateUsername(sock);

  while (1)
  { /* Loop continuously */
    clientCommandCode = parseClientCommand(targetHashtag, ttweetString);

    switch (clientCommandCode)
    {
    case -1:
      break;
    case 1:
      /* tweet */
      parseHashtags(validHashtags, &numValidHashtags, targetHashtag);
      break;
    case 2:
      /* subscribe */
      break;
    case 3:
      /* unsubscribe */
      break;
    case 4:
      /* timeline */
      break;
    case 5:
      /* exit */
      close(sock);
      exit(0);
    default:
      DieWithError("Unexpected error occured.");
    }
  }
  
  ttweetStringLen = strlen(ttweetString); /* Determine message length */

  inputHashtags = "#1#2#pop#mom";
  jobj = cJSON_CreateObject();
  
  ttweetToJson(jobj, "ttweet message!", validHashtags, numValidHashtags);
  resetClientVariables(validHashtags, numValidHashtags, jobj);
}

/** \copydoc parseHashTags */
int parseHashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags)
{
  int inputHashtagsCharIndex = 1; /* Track char index in inputHashtags string */
  int numConsecutiveHashes = 1;   /* Check for duplicate # within hashtag */
  int lenInputHashtags;           /* Length of inputHashtags string */
  char currentHashtagBuffer[25];  /* Current hashtag being parsed */
  int currentHashtagBufferIndex;  /* char index of current hashtag being parsed */

  *numValidHashtags = 0;         /* Initialize number of valid hashtags */
  currentHashtagBufferIndex = 0; /* Initialize buffer index */

  lenInputHashtags = strlen(inputHashtags); /* Assign length of input hashtags */

  if (inputHashtags[0] != '#')
  { /* Hashtag must begin with # */
    return PersistWithError("Invalid hashtag(s)! Hashtag(s) must begin with #.");
  }

  if (inputHashtags[lenInputHashtags - 1] == '#')
  { /* Hashtag cannot end with # */
    return PersistWithError("Invalid hashtag(s)! Hashtag(s) cannot end with #.");
  }

  if (lenInputHashtags < 2 || lenInputHashtags > 25)
  { /* Hashtag must be between 2 to 25 chars long */
    return PersistWithError("Invalid hashtag(s)! Hashtag(s) must be between 2 to 25 chars long.");
  }

  while (inputHashtags[inputHashtagsCharIndex] != '\0')
  { /* Not yet reached end of inputHashtags */
    if (inputHashtags[inputHashtagsCharIndex] == '#')
    {
      numConsecutiveHashes++;
      if (numConsecutiveHashes > 1)
      { /* Hashtag contains consecutive # */
        return PersistWithError("Invalid hashtag(s)! Hashtag(s) cannot contain consecutive #.");
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
      return PersistWithError("Invalid hashtag(s)! Hashtag(s) contains invalid characters.");
    }
    inputHashtagsCharIndex++;
  }

  /* Reached end of inputHashtags */
  saveCurrentHashtag(currentHashtagBuffer, &currentHashtagBufferIndex, validHashtags, numValidHashtags);

  if (numValidHashtags > 8)
  { /* Limit of 8 hashtags exceeded */
    return PersistWithError("Invalid hashtag(s)! Limit of 8 hashtags exceeded.");
  }
  if (duplicateStringExists(validHashtags, numValidHashtags))
  { /* validHashtags contain duplicate hashtags */
    return PersistWithError("Invalid hashtag(s)! Duplicate hashtags detected.");
  }
}

/** \copydoc saveCurrentHashtag */
void saveCurrentHashtag(char *currentHashtagBuffer, int *currentHashtagBufferIndex, char *validHashtags[], int *numValidHashtags)
{
  currentHashtagBuffer[*currentHashtagBufferIndex] = '\0';
  if (strcmp(currentHashtagBuffer, "ALL") == 0)
  { /* Hashtag #ALL is not allowed */
    return PersistWithError("Invalid hashtag(s)! Hashtag #ALL is not allowed.");
  }
  validHashtags[*numValidHashtags] = (char *)malloc((*currentHashtagBufferIndex + 1) * sizeof(char));
  strcpy(validHashtags[*numValidHashtags], currentHashtagBuffer);
  *currentHashtagBufferIndex = 0;
  (*numValidHashtags)++;
}

/** \copydoc duplicateStringExists */
int duplicateStringExists(char *stringArray[], int numStringsInArray)
{
  for (int i = 0; i < numStringsInArray - 1; i++)
  {
    for (int j = i + 1; j < numStringsInArray; j++)
    {
      if (strcmp(stringArray[i], stringArray[j]) == 0)
      {
        return 1;
      }
    }
  }
  return 0;
}

/** \copydoc ttweetToJson */
void ttweetToJson(cJSON *jobj, char *ttweetString, char *validHashtags[], int numValidHashtags)
{
  cJSON *jstring = cJSON_CreateString(ttweetString); /*Creating a json string*/
  cJSON *jarray = cJSON_CreateArray();               /*Creating a json array*/
  for (int i = 0; i < numValidHashtags; i++)
  { /*Add hashtags to array*/
    cJSON_AddItemToArray(jarray, cJSON_CreateString(validHashtags[i]));
  }
  cJSON_AddItemToObject(jobj, "ttweetString", jstring);  /*Add ttweetString to JSON object*/
  cJSON_AddItemToObject(jobj, "ttweetHashtags", jarray); /*Add hashtags to JSON object*/
  printf("The json object created: %s\n", cJSON_Print(jobj));
}

/** \copydoc resetClientVariables */
void resetClientVariables(char *validHashtags[], int *numValidHashtags, cJSON *jobj)
{
  deallocateStringArray(validHashtags, *numValidHashtags);
  *numValidHashtags = 0;
  cJSON_Delete(jobj);
}

/** \copydoc deallocateStringArray */
void deallocateStringArray(char *stringArray[], int numStringsInArray)
{
  if (!numStringsInArray)
  {
    return;
  }
  for (int i = 0; i < numStringsInArray; i++)
  {
    free(stringArray[i]);
  }
}

void validateUsername(int sock)
{
  int usernameValidationCode;
  recv(sock, &usernameValidationCode, sizeof(int), 0);
  if (usernameValidationCode == 1)
  { /* Username is valid */
    printf("%s", "Username valid. Connection established successfully.\n");
    return;
  }
  else
  { /* Username is invalid */
    DieWithError("Username invalid. Please select a different username and try again.");
  }
}

int parseClientCommand(char targetHashtag[], char ttweetString[])
{
  char userInput[200];
  char clientCommand[20];
  int charIndex = 0;
  int endOfCmd = 0;

  char unknownCmdMsg = "Command not recognized. Here are the available commands:\n\
                        1. tweet​ \"<150 char max tweet>\" <Hashtag>\n\
                        2. subscribe​ <Hashtag>\n\
                        3. unsubscribe​ <Hashtag>\n\
                        4. timeline\n\
                        5. exit\n";

  scanf("%s", userInput);

  while (userInput[charIndex] != ' ')
  {
    if (userInput[charIndex] == '\0')
    {
      endOfCmd = 1;
      break;
    }
    if (charIndex > 19)
    { /* None of the valid commands exceed 19 chars */
      return PersistWithError(unknownCmdMsg);
    }
    clientCommand[charIndex] = userInput[charIndex];
    charIndex++;
  }

  clientCommand[charIndex] = '\0';
  charIndex++;

  if (strcmp(clientCommand, "tweet") == 0)
  {
    return validateTweetCmd(userInput, charIndex, targetHashtag, ttweetString);
  }
  else if (strcmp(clientCommand, "subscribe") == 0)
  {
    return validateSubscribeCmd(userInput, charIndex, targetHashtag);
  }
  else if (strcmp(clientCommand, "unsubscribe") == 0)
  {
    return validateUnsubscribeCmd(userInput, charIndex, targetHashtag);
  }
  else if (strcmp(clientCommand, "timeline") == 0)
  {
    return validateTimelineCmd(endOfCmd);
  }
  else if (strcmp(clientCommand, "exit") == 0)
  {
    return validateExitCmd(endOfCmd);
  }
  else
  {
    return PersistWithError(unknownCmdMsg);
  }
}

int validateTweetCmd(char userInput[], int charIndex, char targetHashtag[], char ttweetString[])
{
  int ttweetStringIndex = 0;
  int targetHashtagIndex = 0;
  char invalidTweetCmdMsg = "tweet command not formatted correctly. Please try again.";

  if (userInput[charIndex] != '\"')
  {
    return PersistWithError(invalidTweetCmdMsg);
  }
  charIndex++;
  while (userInput[charIndex] != '\"')
  {
    if (ttweetStringIndex > 149)
    {
      return PersistWithError("tweet message cannot exceed 150 chars. Please try again.");
    }
    ttweetString[ttweetStringIndex] = userInput[charIndex];
    charIndex++;
    ttweetStringIndex++;
  }
  charIndex++;
  if (userInput[charIndex] != ' ')
  {
    return PersistWithError(invalidTweetCmdMsg);
  }
  charIndex++;
  while (userInput[charIndex] != '\0')
  {
    if (userInput[charIndex] == ' ')
    {
      return PersistWithError("Invalid hashtag(s)! Hashtag cannot contain whitespaces.");
    }
    if (targetHashtagIndex > 25)
    {
      return PersistWithError("Invalid hashtag(s)! Hashtag cannot exceed 25 chars.");
    }
    targetHashtag[targetHashtagIndex] = userInput[charIndex];
    charIndex++;
    targetHashtagIndex++;
  }

  return 1;
}

int validateSubscribeCmd(char userInput[], int charIndex, char targetHashtag[])
{
  int targetHashtagIndex = 0;
  char invalidSubscribeCmdMsg = "subscribe command not formatted correctly. Please try again.";

  while (userInput[charIndex] != '\0')
  {
    if (userInput[charIndex] != ' ')
    {
      return PersistWithError(invalidSubscribeCmdMsg);
    }
    if (targetHashtagIndex > 25)
    {
      return PersistWithError("Invalid hashtag(s)! Hashtag cannot exceed 25 chars.");
    }
    targetHashtag[targetHashtagIndex] = userInput[charIndex];
    charIndex++;
    targetHashtagIndex++;
  }
  return 2;
}

int validateUnsubscribeCmd(char userInput[], int charIndex, char targetHashtag[])
{
  int targetHashtagIndex = 0;
  char invalidUnsubscribeCmdMsg = "unsubscribe command not formatted correctly. Please try again.";

  while (userInput[charIndex] != '\0')
  {
    if (userInput[charIndex] != ' ')
    {
      return PersistWithError(invalidUnsubscribeCmdMsg);
    }
    if (targetHashtagIndex > 25)
    {
      return PersistWithError("Invalid hashtag(s)! Hashtag cannot exceed 25 chars.");
    }
    targetHashtag[targetHashtagIndex] = userInput[charIndex];
    charIndex++;
    targetHashtagIndex++;
  }
  return 3;
}

int validateTimelineCmd(int endOfCmd)
{
  char invalidTimelineCmdMsg = "timeline command not formatted correctly. Please try again.";

  if (!endOfCmd)
  {
    return PersistWithError(invalidTimelineCmdMsg);
  }
  return 4;
}

int validateExitCmd(int endOfCmd)
{
  char invalidExitCmdMsg = "exit command not formatted correctly. Please try again.";

  if (!endOfCmd)
  {
    return PersistWithError(invalidExitCmdMsg);
  }
  return 5;
}

// while ((bytesRcvd = recv(sock, ttweetBuffer, RCVBUFSIZE - 1, 0)) > 0)
// {
//   /* Receive up to the buffer size (minus 1 to leave space for
//      a null terminator) bytes from the sender */
//   ttweetBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
//   printf("%s", ttweetBuffer);     /* Print the ttweetBuffer */
// }
