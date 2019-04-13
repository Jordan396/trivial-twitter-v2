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
  * Code is documented according to GNOME and Doxygen standards.
  * <https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en>
  * <http://www.doxygen.nl/manual/docblocks.html>
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

int parse_hashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);                                             /* Parses hashtags from user input */
void save_current_hashtag(char *currentHashtagBuffer, int *currentHashtagBufferIdx, char *validHashtags[], int *numValidHashtags); /* Save current hashtag buffer */
void reset_client_variables(int *clientCommandSuccess, char *validHashtags[], int *numValidHashtags, cJSON *jobjToSend);           /* Resets client variables to prepare for the next command */
void deallocate_string_array(char *stringArray[], int numStringsInArray);                                                          /* Deallocates memory from a dynamic string array */
int has_duplicate_string(char *stringArray[], int numStringsInArray);                                                              /* Checks for duplicates in string array */
int parse_client_command(char inputHashtags[], char ttweetString[]);
int check_tweet_cmd(char clientInput[], int charIdx, char inputHashtags[], char ttweetString[]);
int check_subscribe_cmd(char clientInput[], int charIdx, char inputHashtags[]);
int check_unsubscribe_cmd(char clientInput[], int charIdx, char inputHashtags[]);
int check_timeline_cmd(int endOfCmd);
int check_exit_cmd(int endOfCmd);
void create_json_client_payload(cJSON *jobjToSend, int commandCode, char *username, int userIdx, char *ttweetString, char *validHashtags[], int numValidHashtags);
void handle_server_response(cJSON *jobjReceived, int *userIdx);
int get_client_input(char *clientInput);
int check_hashtag_all_exists(char *validHashtags[], int numValidHashtags);

int main(int argc, char *argv[])
{
  /* Socket variables */
  int sock;                          /* Socket descriptor */
  struct sockaddr_in ttweetServAddr; /* ttweet server address */
  unsigned short ttweetServPort;     /* ttweet server port */
  char *servIP;                      /* Server IP address (dotted quad) */

  /* Variables to handle user commands */
  int clientCommandCode;
  int clientCommandSuccess;
  char ttweetString[MAX_TWEET_LEN]; /* String to be send to ttweet server */
  char inputHashtags[MAX_HASHTAG_LEN];
  char *username;
  char *validHashtags[MAX_HASHTAG_CNT]; /* Array of valid hashtags */
  int numValidHashtags;                 /* Number of valid hashtags */

  /* Variables used to handle transfer of data over TCP */
  cJSON *jobjToSend;   /* JSON payload to be sent */
  cJSON *jobjReceived; /* JSON response received */
  char objReceived[MAX_RESP_LEN];

  /* Variables for server to recognize client */
  int userIdx = INVALID_USER_INDEX;

  if (argc != 4) /* Test for correct number of arguments */
  {
    die_with_error("Command not recognized!\nUsage: $./ttweetcli <ServerIP> <ServerPort> <Username>");
  }

  servIP = argv[1];               /* Server IP address (dotted quad) */
  ttweetServPort = atoi(argv[2]); /* Use given port, if any */
  username = argv[3];             /* Parse username */

  /* Create a reliable, stream socket using TCP */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    die_with_error("socket() failed");

  /* Construct the server address structure */
  memset(&ttweetServAddr, 0, sizeof(ttweetServAddr)); /* Zero out structure */
  ttweetServAddr.sin_family = AF_INET;                /* Internet address family */
  ttweetServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  ttweetServAddr.sin_port = htons(ttweetServPort);    /* Server port */

  /* Establish the connection to the ttweet server */
  if (connect(sock, (struct sockaddr *)&ttweetServAddr, sizeof(ttweetServAddr)) < 0)
    die_with_error("connect() failed");

  /* Upload username to server for validation */
  jobjToSend = cJSON_CreateObject();
  create_json_client_payload(jobjToSend, REQ_VALIDATE_USER, username, INVALID_USER_INDEX, ttweetString, validHashtags, numValidHashtags);
  send_payload(sock, jobjToSend);

  /* Process validation code from server */
  receive_response(sock, objReceived);
  jobjReceived = cJSON_Parse(objReceived);
  handle_server_response(jobjReceived, &userIdx);

  while (1)
  { /* Loop continuously */
    reset_client_variables(&clientCommandSuccess, validHashtags, &numValidHashtags, jobjToSend);
    jobjToSend = cJSON_CreateObject();
    clientCommandCode = parse_client_command(inputHashtags, ttweetString);

    switch (clientCommandCode)
    {
    case REQ_TWEET:
      /* tweet */
      clientCommandSuccess = parse_hashtags(validHashtags, &numValidHashtags, inputHashtags);
      clientCommandSuccess = check_hashtag_all_exists(validHashtags, numValidHashtags);
      break;
    case REQ_SUBSCRIBE:
      /* subscribe */
    case REQ_UNSUBSCRIBE:
      /* unsubscribe */
      clientCommandSuccess = parse_hashtags(validHashtags, &numValidHashtags, inputHashtags);
      if (numValidHashtags != 1)
      {
        clientCommandSuccess = persist_with_error("Subscribe/Unsubscribe only accepts one hashtag as the argument.");
        break;
      }
      break;
    case REQ_TIMELINE:
    case REQ_EXIT:
      break;
    case REQ_INVALID:
      clientCommandSuccess = persist_with_error("Invalid command entered.");
      break;
    default:
      die_with_error("An error occured. Exiting client...");
      break;
    }

    if (clientCommandSuccess)
    {
      create_json_client_payload(jobjToSend, clientCommandCode, username, userIdx, ttweetString, validHashtags, numValidHashtags);
      clientCommandSuccess = send_payload(sock, jobjToSend);
    }

    if (clientCommandCode == REQ_EXIT)
    {
      printf("Exiting client...\n");
      waitFor(3);
      close(sock);
      exit(0);
    }

    if (clientCommandSuccess)
    {
      receive_response(sock, objReceived);
      jobjReceived = cJSON_Parse(objReceived);
      handle_server_response(jobjReceived, &userIdx);
    }
  }
}

/** \copydoc parse_hashtags */
int parse_hashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags)
{
  int inputHashtagsCharIdx = 1;               /* Track char index in inputHashtags string */
  int numConsecutiveHashes = 1;               /* Check for duplicate # within hashtag */
  int lenInputHashtags;                       /* Length of inputHashtags string */
  char currentHashtagBuffer[MAX_HASHTAG_LEN]; /* Current hashtag being parsed */
  int currentHashtagBufferIdx;                /* char index of current hashtag being parsed */
  char *errorMessage;

  *numValidHashtags = 0;       /* Initialize number of valid hashtags */
  currentHashtagBufferIdx = 0; /* Initialize buffer index */

  lenInputHashtags = strlen(inputHashtags); /* Assign length of input hashtags */

  if (inputHashtags[0] != '#')
  { /* Hashtag must begin with # */
    return persist_with_error("Invalid hashtag(s)! Hashtag(s) must begin with #.");
  }

  if (inputHashtags[lenInputHashtags - 1] == '#')
  { /* Hashtag cannot end with # */
    return persist_with_error("Invalid hashtag(s)! Hashtag(s) cannot end with #.");
  }

  if (lenInputHashtags < 2 || lenInputHashtags > 25)
  { /* Hashtag must be between 2 to 25 chars long */
    return persist_with_error("Invalid hashtag(s)! Hashtag(s) must be between 2 to 25 chars long.");
  }

  while (inputHashtags[inputHashtagsCharIdx] != '\0')
  { /* Not yet reached end of inputHashtags */
    if (inputHashtags[inputHashtagsCharIdx] == '#')
    {
      numConsecutiveHashes++;
      if (numConsecutiveHashes > 1)
      { /* Hashtag contains consecutive # */
        return persist_with_error("Invalid hashtag(s)! Hashtag(s) cannot contain consecutive #.");
      }
      else
      { /* Reached end of current hashtag. Save and prepare to parse next hashtag. */
        if (*numValidHashtags == MAX_HASHTAG_CNT)
        { /* Hashtag limit exceeded */
          return persist_with_error("Invalid hashtag(s)! Hashtag limit exceeded.");
        }
        else
        {
          save_current_hashtag(currentHashtagBuffer, &currentHashtagBufferIdx, validHashtags, numValidHashtags);
        }
      }
    }
    else if (isalnum(inputHashtags[inputHashtagsCharIdx]) != 0)
    { /* char is alphanumeric */
      currentHashtagBuffer[currentHashtagBufferIdx] = inputHashtags[inputHashtagsCharIdx];
      currentHashtagBufferIdx++;
      numConsecutiveHashes = 0;
    }
    else
    { /* char is not alphanumeric or # */
      return persist_with_error("Invalid hashtag(s)! Hashtag(s) contains invalid characters.");
    }
    inputHashtagsCharIdx++;
  }

  /* Reached end of inputHashtags */
  if (*numValidHashtags == MAX_HASHTAG_CNT)
  { /* Hashtag limit exceeded */
    return persist_with_error("Invalid hashtag(s)! Hashtag limit exceeded.");
  }
  else
  {
    save_current_hashtag(currentHashtagBuffer, &currentHashtagBufferIdx, validHashtags, numValidHashtags);
  }

  if (has_duplicate_string(validHashtags, *numValidHashtags))
  { /* validHashtags contain duplicate hashtags */
    return persist_with_error("Invalid hashtag(s)! Duplicate hashtags detected.");
  }
  return 1;
}

/** \copydoc save_current_hashtag */
void save_current_hashtag(char *currentHashtagBuffer, int *currentHashtagBufferIdx, char *validHashtags[], int *numValidHashtags)
{
  currentHashtagBuffer[*currentHashtagBufferIdx] = '\0';
  validHashtags[*numValidHashtags] = (char *)malloc((*currentHashtagBufferIdx + 1) * sizeof(char));
  strcpy(validHashtags[*numValidHashtags], currentHashtagBuffer);
  *currentHashtagBufferIdx = 0;
  (*numValidHashtags)++;
}

/** \copydoc has_duplicate_string */
int has_duplicate_string(char *stringArray[], int numStringsInArray)
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

/** \copydoc reset_client_variables */
void reset_client_variables(int *clientCommandSuccess, char *validHashtags[], int *numValidHashtags, cJSON *jobjToSend)
{
  *clientCommandSuccess = 1;
  deallocate_string_array(validHashtags, *numValidHashtags);
  *numValidHashtags = 0;
  if (!jobjToSend)
  {
    cJSON_Delete(jobjToSend);
  }
}

/** \copydoc deallocate_string_array */
void deallocate_string_array(char *stringArray[], int numStringsInArray)
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

int parse_client_command(char inputHashtags[], char ttweetString[])
{
  char clientInput[MAX_CLI_INPUT_LEN];
  char clientCommand[20];
  int charIdx = 0;
  int endOfCmd = 0;

  char *unknownCmdMsg = "Command not recognized. Here are the available commands:\n\
                        1. tweet​ \"<150 char max tweet>\" <Hashtag>\n\
                        2. subscribe​ <Hashtag>\n\
                        3. unsubscribe​ <Hashtag>\n\
                        4. timeline\n\
                        5. exit\n";

  if (!get_client_input(clientInput))
  {
    return persist_with_error("Your input is too long. Please try again.\n");
  }

  while (clientInput[charIdx] != ' ')
  {
    if (clientInput[charIdx] == '\0')
    {
      endOfCmd = 1;
      break;
    }
    if (charIdx > 19)
    { /* None of the valid commands exceed 19 chars */
      return persist_with_error(unknownCmdMsg);
    }
    strncpy(clientCommand + charIdx, clientInput + charIdx, sizeof(char));
    charIdx++;
  }

  clientCommand[charIdx] = '\0';
  charIdx++;

  if (strcmp(clientCommand, "tweet") == 0)
  {
    return check_tweet_cmd(clientInput, charIdx, inputHashtags, ttweetString);
  }
  else if (strcmp(clientCommand, "subscribe") == 0)
  {
    return check_subscribe_cmd(clientInput, charIdx, inputHashtags);
  }
  else if (strcmp(clientCommand, "unsubscribe") == 0)
  {
    return check_unsubscribe_cmd(clientInput, charIdx, inputHashtags);
  }
  else if (strcmp(clientCommand, "timeline") == 0)
  {
    return check_timeline_cmd(endOfCmd);
  }
  else if (strcmp(clientCommand, "exit") == 0)
  {
    return check_exit_cmd(endOfCmd);
  }
  else
  {
    return persist_with_error(unknownCmdMsg);
  }
}

int check_tweet_cmd(char clientInput[], int charIdx, char inputHashtags[], char ttweetString[])
{
  int ttweetStringIdx = 0;
  int inputHashtagsIdx = 0;
  char *invalidTweetCmdMsg = "tweet command not formatted correctly. Please try again.";
  char *errorMessage;

  if (clientInput[charIdx] != '\"')
  {
    return persist_with_error(invalidTweetCmdMsg);
  }
  charIdx++;
  if (clientInput[charIdx] == '\"')
  {
    return persist_with_error("Tweet message cannot be empty!");
  }
  while (clientInput[charIdx] != '\"')
  {
    if (ttweetStringIdx > MAX_TWEET_LEN - 1)
    {
      return persist_with_error("Tweet message is too long. Please try again");
    }
    strncpy(ttweetString + ttweetStringIdx, clientInput + charIdx, sizeof(char));
    charIdx++;
    ttweetStringIdx++;
  }
  charIdx++;
  ttweetString[ttweetStringIdx] = '\0';
  if (clientInput[charIdx] != ' ')
  {
    return persist_with_error(invalidTweetCmdMsg);
  }
  charIdx++;
  while (clientInput[charIdx] != '\0')
  {
    if (clientInput[charIdx] == ' ')
    {
      return persist_with_error("Invalid hashtag(s)! Hashtag cannot contain whitespaces.");
    }
    if (inputHashtagsIdx > 25)
    {
      return persist_with_error("Invalid hashtag(s)! Hashtag cannot exceed 25 chars.");
    }
    strncpy(inputHashtags + inputHashtagsIdx, clientInput + charIdx, sizeof(char));

    charIdx++;
    inputHashtagsIdx++;
  }
  inputHashtags[inputHashtagsIdx] = '\0';

  return REQ_TWEET;
}

int check_subscribe_cmd(char clientInput[], int charIdx, char inputHashtags[])
{
  int inputHashtagsIdx = 0;
  char *invalidSubscribeCmdMsg = "subscribe command not formatted correctly. Please try again.";

  while (clientInput[charIdx] != '\0')
  {
    if (clientInput[charIdx] == ' ')
    {
      return persist_with_error(invalidSubscribeCmdMsg);
    }
    if (inputHashtagsIdx > 25)
    {
      return persist_with_error("Invalid hashtag(s)! Hashtag cannot exceed 25 chars.");
    }
    strncpy(inputHashtags + inputHashtagsIdx, clientInput + charIdx, sizeof(char));

    charIdx++;
    inputHashtagsIdx++;
  }
  inputHashtags[inputHashtagsIdx] = '\0';
  return REQ_SUBSCRIBE;
}

int check_unsubscribe_cmd(char clientInput[], int charIdx, char inputHashtags[])
{
  int inputHashtagsIdx = 0;
  char *invalidUnsubscribeCmdMsg = "unsubscribe command not formatted correctly. Please try again.";

  while (clientInput[charIdx] != '\0')
  {
    if (clientInput[charIdx] == ' ')
    {
      return persist_with_error(invalidUnsubscribeCmdMsg);
    }
    if (inputHashtagsIdx > 25)
    {
      return persist_with_error("Invalid hashtag(s)! Hashtag cannot exceed 25 chars.");
    }
    inputHashtags[inputHashtagsIdx] = clientInput[charIdx];
    charIdx++;
    inputHashtagsIdx++;
  }
  inputHashtags[inputHashtagsIdx] = '\0';
  return REQ_UNSUBSCRIBE;
}

int check_timeline_cmd(int endOfCmd)
{
  char *invalidTimelineCmdMsg = "timeline command not formatted correctly. Please try again.";

  if (!endOfCmd)
  {
    return persist_with_error(invalidTimelineCmdMsg);
  }
  return REQ_TIMELINE;
}

int check_exit_cmd(int endOfCmd)
{
  char *invalidExitCmdMsg = "exit command not formatted correctly. Please try again.";

  if (!endOfCmd)
  {
    return persist_with_error(invalidExitCmdMsg);
  }
  return REQ_EXIT;
}

void create_json_client_payload(cJSON *jobjToSend, int commandCode, char *username, int userIdx, char *ttweetString, char *validHashtags[], int numValidHashtags)
{
  cJSON_AddItemToObject(jobjToSend, "requestCode", cJSON_CreateNumber(commandCode)); /*Add command to JSON object*/
  cJSON_AddItemToObject(jobjToSend, "username", cJSON_CreateString(username));       /*Add username to JSON object*/

  switch (commandCode)
  {
  case REQ_TWEET:
  {
    cJSON *jarray = cJSON_CreateArray(); /*Creating a json array*/
    for (int i = 0; i < numValidHashtags; i++)
    { /*Add hashtags to array*/
      cJSON_AddItemToArray(jarray, cJSON_CreateString(validHashtags[i]));
    }
    cJSON_AddItemToObject(jobjToSend, "ttweetString", cJSON_CreateString(ttweetString)); /*Add ttweetString to JSON object*/
    cJSON_AddItemToObject(jobjToSend, "numValidHashtags", cJSON_CreateNumber(numValidHashtags));
    cJSON_AddItemToObject(jobjToSend, "ttweetHashtags", jarray); /*Add hashtags to JSON object*/
    break;
  }
  case REQ_SUBSCRIBE:
  case REQ_UNSUBSCRIBE:
    cJSON_AddItemToObject(jobjToSend, "subscriptionHashtag", cJSON_CreateString(validHashtags[0])); /*Add target hashtag to JSON object*/
    break;
  case REQ_TIMELINE:
  case REQ_VALIDATE_USER:
  case REQ_EXIT:
    break;
  default:
    die_with_error("Error! Client attempted to create an invalid JSON payload.");
    break;
  }
}

void handle_server_response(cJSON *jobjReceived, int *userIdx)
{
  int responseCode = cJSON_GetObjectItemCaseSensitive(jobjReceived, "responseCode")->valueint;

  switch (responseCode)
  {
  case RES_USER_INVALID:
    die_with_error(cJSON_GetObjectItemCaseSensitive(jobjReceived, "detailedMessage")->valuestring);
    break;
  case RES_USER_VALID:
  {
    *userIdx = cJSON_GetObjectItemCaseSensitive(jobjReceived, "clientUserIdx")->valueint;
    printf("Username legal. Connection established.\n");
    break;
  }

  case RES_SUBSCRIBE:
  case RES_UNSUBSCRIBE:
  case RES_TWEET:
  {
    printf("Server response: %s", cJSON_GetObjectItemCaseSensitive(jobjReceived, "detailedMessage")->valuestring);
    break;
  }

  case RES_TIMELINE:
  {
    cJSON *jarray = cJSON_GetObjectItemCaseSensitive(jobjReceived, "storedTweets");
    for (int i = 0; i < cJSON_GetArraySize(jarray); i++)
    {
      printf("%s\n", cJSON_GetArrayItem(jarray, i)->valuestring);
    }
    break;
  }

  default:
    die_with_error("Error! Server sent an invalid response code.");
    break;
  }
}

int get_client_input(char *clientInput)
{
  int i = 0;
  while (1)
  {
    if (i > (MAX_CLI_INPUT_LEN - 50))
    {
      return 0;
    }
    scanf("%c", &clientInput[i]);
    if (clientInput[i] == '\n')
      break;
    i++;
  }
  clientInput[i] = '\0';
  return 1;
}

int check_hashtag_all_exists(char *validHashtags[], int numValidHashtags)
{
  for (int hashtagIdx = 0; hashtagIdx < numValidHashtags; hashtagIdx++)
  {
    if (strcmp(validHashtags[hashtagIdx], "ALL") == 0)
    return persist_with_error("Invalid hashtag(s)! Hashtag #ALL is not allowed when tweeting.");
  }
  return 1;
}