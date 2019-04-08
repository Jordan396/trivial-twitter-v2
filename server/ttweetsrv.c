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
  * @file ttweetser.c
  * @author Jordan396
  * @date 14 February 2019
  * @brief ttweetser handles upload/download requests from ttweetcli clients.
  *
  * This file is to be compiled and executed on the server side.
  * ttweetser runs continuously to listen for client requests. When a client
  * sends a request, ttweetser checks if request is unauthorized or authorized.
  * If authorized, check if request is to upload or download. If download, send ttweetString
  * to the client. If upload, receive message from the client and store in ttweetString.
  * If unauthorized, notify client of unauthorized request via unauthorizedRequestMessage.
  */

#include "ttweetsrv.h"

void child_exit_signal_handler(); /* Function to clean up zombie child processes */
int create_tcp_serv_socket(unsigned short port);
int accept_tcp_connection(int servSock);
void handle_ttweet_client(int clntSocket);
int handle_client_response(int clntSocket, cJSON *jobjReceived, int *clientUserIdx);
void handle_validate_user_request(cJSON *jobjToSend, char *senderUsername, int *clientUserIdx);
void handle_tweet_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);
void handle_subscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);
void handle_unsubscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);
void handle_timeline_request(cJSON *jobjToSend, int *clientUserIdx);
int handle_exit_request(int *userIdx);
int handle_invalid_request();
void handle_tweet_updates();
void add_tweet_to_user(User client, char *senderUsername, char *ttweetString, char *originHashtag);
void waitFor(unsigned int secs);
void initializeUserArray();
void create_json_server_payload(cJSON *jobjToSend, int commandCode, int userIdx, char *detailedMessage);
void add_pending_tweets_to_jobj(cJSON *jobj, ListNode *head);
void reset_server_variables(cJSON *jobjToSend);
void store_latest_tweet(cJSON *jobjReceived, char *senderUsername);

/* Global variables */
unsigned int childProcCount = 0; /* Number of child processes */
LatestTweet latestTweet;
User activeUsers[MAX_CONC_CONN]; /* Keeps track of active users */

int main(int argc, char *argv[])
{
  int servSock;                   /* Socket descriptor for server */
  int clntSock;                   /* Socket descriptor for client */
  unsigned short ttweetServPort;  /* Server port */
  pid_t processID;                /* Process ID from fork() */
  struct sigaction signalHandler; /* Signal handler specification structure */

  if (argc != 2) /* Test for correct number of arguments */
  {
    die_with_error("Usage: ./ttweetsrv <Port>\n");
  }

  ttweetServPort = atoi(argv[1]); /* First arg:  local port */
  servSock = create_tcp_serv_socket(ttweetServPort);

  /* Initialize activeUsers array */
  initializeUserArray();

  /* Set child_exit_signal_handler() as handler function */
  signalHandler.sa_handler = child_exit_signal_handler;
  if (sigfillset(&signalHandler.sa_mask) < 0) /* mask all signals */
    die_with_error("sigfillset() failed");
  /* SA_RESTART causes interrupted system calls to be restarted */
  signalHandler.sa_flags = SA_RESTART;

  /* Set signal disposition for child-termination signals */
  if (sigaction(SIGCHLD, &signalHandler, 0) < 0)
    die_with_error("sigaction() failed");

  /* Spawn a dedicated child process to check for latest tweets */
  if ((processID = fork()) < 0)
    die_with_error("fork() failed");
  else if (processID == 0) /* If this is the child process */
  {
    handle_tweet_updates();
    exit(0); /* Child process done */
  }

  while (1) /* run forever */
  {
    if (childProcCount < MAX_CONC_CONN)
    { /* Server supports up to MAX_CONC_CONN connections */
      clntSock = accept_tcp_connection(servSock);
      /* Fork child process and report any errors */
      if ((processID = fork()) < 0)
        die_with_error("fork() failed");
      else if (processID == 0) /* If this is the child process */
      {
        close(servSock); /* Child closes parent socket file descriptor */
        handle_ttweet_client(clntSock);
        exit(0); /* Child process done */
      }

      printf("with child process: %d\n", (int)processID);
      close(clntSock);  /* Parent closes child socket descriptor */
      childProcCount++; /* Increment number of outstanding child processes */
    }
  }
}

void child_exit_signal_handler()
{
  pid_t processID; /* Process ID from fork() */

  while (childProcCount) /* Clean up all zombies */
  {
    processID = waitpid((pid_t)-1, NULL, WNOHANG); /* Non-blocking wait */
    if (processID < 0)                             /* waitpid() error? */
      die_with_error("waitpid() failed");
    else if (processID == 0) /* No child to wait on */
      break;
    else
      childProcCount--; /* Cleaned up after a child */
  }
}

int create_tcp_serv_socket(unsigned short port)
{
  int sock;                          /* socket to create */
  struct sockaddr_in ttweetServAddr; /* Local address */

  /* Create socket for incoming connections */
  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    die_with_error("socket() failed");

  /* Construct local address structure */
  memset(&ttweetServAddr, 0, sizeof(ttweetServAddr)); /* Zero out structure */
  ttweetServAddr.sin_family = AF_INET;                /* Internet address family */
  ttweetServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  ttweetServAddr.sin_port = htons(port);              /* Local port */

  /* Bind to the local address */
  if (bind(sock, (struct sockaddr *)&ttweetServAddr, sizeof(ttweetServAddr)) < 0)
    die_with_error("bind() failed");

  /* Mark the socket so it will listen for incoming connections */
  if (listen(sock, MAX_PENDING) < 0)
    die_with_error("listen() failed");

  return sock;
}

int accept_tcp_connection(int servSock)
{
  int clntSock;                      /* Socket descriptor for client */
  struct sockaddr_in ttweetClntAddr; /* Client address */
  unsigned int clntLen;              /* Length of client address data structure */

  /* Set the size of the in-out parameter */
  clntLen = sizeof(ttweetClntAddr);

  /* Wait for a client to connect */
  if ((clntSock = accept(servSock, (struct sockaddr *)&ttweetClntAddr,
                         &clntLen)) < 0)
    die_with_error("accept() failed");

  /* clntSock is connected to a client! */

  printf("Handling client %s\n", inet_ntoa(ttweetClntAddr.sin_addr));

  return clntSock;
}

void handle_ttweet_client(int clntSocket)
{
  cJSON *jobjReceived;
  int clientUserIdx = INVALID_USER_INDEX;
  int loop = 1;

  while (loop)
  {
    receive_response(clntSocket, jobjReceived);
    loop = handle_client_response(clntSocket, jobjReceived, &clientUserIdx);
  }

  close(clntSocket); /* Close client socket */
}

int handle_client_response(int clntSocket, cJSON *jobjReceived, int *clientUserIdx)
{
  int requestCode;
  cJSON *jobjToSend;
  char *senderUsername;
  char *receiverUsername;

  requestCode = cJSON_GetObjectItemCaseSensitive(jobjReceived, "requestCode")->valueint;
  senderUsername = cJSON_GetObjectItemCaseSensitive(jobjReceived, "username")->valuestring;

  switch (requestCode)
  {
  case REQ_VALIDATE_USER:
    handle_validate_user_request(jobjToSend, senderUsername, clientUserIdx);
    break;
  case REQ_TWEET:
    handle_tweet_request(jobjToSend, jobjReceived, senderUsername, clientUserIdx);
    break;
  case REQ_SUBSCRIBE:
    handle_subscribe_request(jobjToSend, jobjReceived, senderUsername, clientUserIdx);
    break;
  case REQ_UNSUBSCRIBE:
    handle_unsubscribe_request(jobjToSend, jobjReceived, senderUsername, clientUserIdx);
    break;
  case REQ_EXIT:
    return handle_exit_request(clientUserIdx);
    break;
  case REQ_INVALID:
  default:
    return handle_invalid_request(clientUserIdx);
  }

  if (!send_payload(clntSocket, jobjToSend))
  {
    printf("Server payload sent successfully.\n");
  }
  else
  {
    printf("Server payload failed to send.\n");
  }

  reset_server_variables(jobjToSend);
}

void handle_validate_user_request(cJSON *jobjToSend, char *senderUsername, int *clientUserIdx)
{
  int isUserValid = 1;
  int isSpaceAvailable = 0;

  for (int userIdx = 0; userIdx < MAX_CONC_CONN; userIdx++)
  {
    if (activeUsers[userIdx].isOccupied)
    {
      if (strcmp(activeUsers[userIdx].username, senderUsername) == 0)
      { /* username already taken */
        create_json_server_payload(jobjToSend, RES_USER_INVALID, INVALID_USER_INDEX, "Username already taken.");
        isUserValid = 0;
        break;
      }
    }
    else
    {
      isSpaceAvailable = 1;
    }
  }

  if (isUserValid && isSpaceAvailable)
  {
    for (int userIdx = 0; userIdx < MAX_CONC_CONN; userIdx++)
    {
      if (!activeUsers[userIdx].isOccupied)
      {
        create_json_server_payload(jobjToSend, RES_USER_VALID, userIdx, "Username is valid.");
        activeUsers[userIdx].isOccupied = 1; /* mark index as occupied */
        break;
      }
    }
  }
  else if (isUserValid && !isSpaceAvailable)
  { /* all connections are active */
    create_json_server_payload(jobjToSend, RES_USER_INVALID, INVALID_USER_INDEX, "All connections occupied.");
  }
}

void handle_tweet_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx)
{
  store_latest_tweet(jobjReceived, senderUsername);
  create_json_server_payload(jobjToSend, RES_TWEET, *clientUserIdx, "");
}

void handle_subscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx)
{
  int isSubscriptionExists = 0;
  int isSubscriptionsFull = 1;
  char *subscriptionHashtag = cJSON_GetObjectItemCaseSensitive(jobjReceived, "subscriptionHashtag")->valuestring;
  char *existingSubscriptions = "";
  char *detailedMessage;

  for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
  {
    if (activeUsers[*clientUserIdx].subscriptions[subscriptionIdx])
    {                                                                                            /* subscription exists in this position of the user's subscriptions array */
      strcat(existingSubscriptions, activeUsers[*clientUserIdx].subscriptions[subscriptionIdx]); /* collect existing subscriptions */
      strcat(existingSubscriptions, "\n");

      if (strcmp(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], subscriptionHashtag) == 0)
      { /* subscription hashtag already exists */
        isSubscriptionExists = 1;
      }
    }
    else
    { /* not all subscription spaces are occupied */
      isSubscriptionsFull = 0;
    }
  }
  if (isSubscriptionsFull)
  {
    sprintf(detailedMessage, "Subscription list full. Please unsubscribe to one of the following hashtags first:\n%s", existingSubscriptions);
    create_json_server_payload(jobjToSend, RES_SUBSCRIBE, *clientUserIdx, detailedMessage);
  }
  else if (isSubscriptionExists)
  {
    create_json_server_payload(jobjToSend, RES_SUBSCRIBE, *clientUserIdx, "Subscription already exists.\n");
  }
}
void handle_unsubscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx)
{
  int isSubscriptionExists = 0;
  char *subscriptionHashtag = cJSON_GetObjectItemCaseSensitive(jobjReceived, "subscriptionHashtag")->valuestring;

  for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
  {
    if (activeUsers[*clientUserIdx].subscriptions[subscriptionIdx])
    { /* subscription exists in this position of the user's subscriptions array */
      if (strcmp(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], subscriptionHashtag) == 0)
      { /* subscription hashtag already exists */
        isSubscriptionExists = 1;
        free(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx]);
        break;
      }
    }
  }
  if (isSubscriptionExists)
  {
    create_json_server_payload(jobjToSend, RES_UNSUBSCRIBE, *clientUserIdx, "Successfully unsubscribed.");
  }
}

void handle_timeline_request(cJSON *jobjToSend, int *clientUserIdx)
{
  create_json_server_payload(jobjToSend, RES_TIMELINE, *clientUserIdx, "");
}

int handle_exit_request(int *userIdx)
{
  /* mark entry as unoccupied */
  activeUsers[*userIdx].isOccupied = 0;
  return 0;
}
int handle_invalid_request()
{
  return 0;
}

void handle_tweet_updates()
{
  int lastCheckedTweetID = 0;
  int matchFound;
  while (1)
  {
    if (lastCheckedTweetID != latestTweet.tweetID)
    {
      waitFor(2); /* Ensure all latestTweet fields have been updated */
      for (int userIdx = 0; userIdx < MAX_CONC_CONN; userIdx++)
      {
        if (!activeUsers[userIdx].isOccupied)
        { /* if User array element is not occupied, there's no need to check it */
          continue;
        }
        if (activeUsers[userIdx].isSubscribedAll)
        { /* User is subscribed to ALL - simply add tweet and take first hashtag */
          add_tweet_to_user(activeUsers[userIdx], latestTweet.username, latestTweet.ttweetString, latestTweet.hashtags[0]);
          continue;
        }
        else
        { /* User is not subscribed to ALL - check if any subscriptions and hashtags match */
          matchFound = 0;
          for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
          { /* Iterate over current user's subscriptions */
            for (int hashtagIdx = 0; hashtagIdx < latestTweet.numValidHashtags; hashtagIdx++)
            { /* Iterate over lastest tweet's hashtags */
              if (strcmp(activeUsers[userIdx].subscriptions[subscriptionIdx], latestTweet.hashtags[hashtagIdx]) == 0)
              { /* user is subscribed to hashtag */
                matchFound = 1;
                add_tweet_to_user(activeUsers[userIdx], latestTweet.username, latestTweet.ttweetString, latestTweet.hashtags[hashtagIdx]);
              }
              if (matchFound)
              {
                break;
              }
            }
          }
        }
      }
    }
  }
}

void add_tweet_to_user(User client, char *senderUsername, char *ttweetString, char *originHashtag)
{
  char tweetItem[MAX_ITEM_LEN];
  strcpy(tweetItem, client.username);
  strcat(tweetItem, " ");
  strcat(tweetItem, senderUsername);
  strcat(tweetItem, ": ");
  strcat(tweetItem, ttweetString);
  strcat(tweetItem, " ");
  strcat(tweetItem, originHashtag);

  insertNode(&(client.pendingTweets), client.pendingTweetsSize, tweetItem);
}

void waitFor(unsigned int secs)
{
  unsigned int retTime = time(0) + secs; // Get finishing time.
  while (time(0) < retTime)
    ; // Loop until it arrives.
}

void initializeUserArray()
{
  for (int i = 0; i < MAX_CONC_CONN; i++)
  {
    activeUsers[i].isOccupied = 0;
  }
}

void create_json_server_payload(cJSON *jobjToSend, int commandCode, int userIdx, char *detailedMessage)
{
  jobjToSend = cJSON_CreateObject();
  cJSON_AddItemToObject(jobjToSend, "responseCode", cJSON_CreateNumber(commandCode));               /*Add command to JSON object*/
  cJSON_AddItemToObject(jobjToSend, "username", cJSON_CreateString(activeUsers[userIdx].username)); /*Add username to JSON object*/
  cJSON_AddItemToObject(jobjToSend, "clientUserIdx", cJSON_CreateNumber(userIdx));                  /*Add user index to JSON object*/
  cJSON_AddItemToObject(jobjToSend, "detailedMessage", cJSON_CreateString(detailedMessage));

  switch (commandCode)
  {
  case REQ_TIMELINE:
    add_pending_tweets_to_jobj(jobjToSend, activeUsers[userIdx].pendingTweets);
    break;
  case RES_SUBSCRIBE:
  case REQ_UNSUBSCRIBE:
  case REQ_TWEET:
    break;
  default:
    die_with_error("Error! create_json_server_payload() received an invalid request.");
    break;
  }

  printf("JSON payload: %s\n", cJSON_Print(jobjToSend));
}

void add_pending_tweets_to_jobj(cJSON *jobj, ListNode *head)
{
  cJSON *jarray = cJSON_CreateArray(); /*Creating a json array*/
  ListNode *current = head;
  ListNode *nextNode;
  if (current == NULL)
  { /* no pending tweets */
    cJSON_AddItemToArray(jarray, cJSON_CreateString("No tweets available"));
  }
  else
  {
    while (current)
    { /* frees the linked list while traversing */
      cJSON_AddItemToArray(jarray, cJSON_CreateString(current->item));
      nextNode = current->next;
      free(current);
      current = nextNode;
    }
  }
  cJSON_AddItemToObject(jobj, "storedTweets", jarray); /*Add tweets to JSON object*/
}

/** \copydoc reset_server_variables */
void reset_server_variables(cJSON *jobjToSend)
{
  if (!jobjToSend)
  {
    cJSON_Delete(jobjToSend);
  }
}

void store_latest_tweet(cJSON *jobjReceived, char *senderUsername)
{
  (latestTweet.tweetID)++;
  strcpy(latestTweet.username, senderUsername);
  cJSON *jarray = cJSON_GetObjectItemCaseSensitive(jobjReceived, "ttweetHashtags");
  latestTweet.numValidHashtags = cJSON_GetArraySize(jarray);
  for (int i = 0; i < latestTweet.numValidHashtags; i++)
  {
    strcpy(latestTweet.hashtags[i], cJSON_GetArrayItem(jarray, i)->valuestring);
  }
}