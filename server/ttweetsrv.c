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
void reject_ttweet_client(int clntSocket);
int handle_client_response(int clntSocket, cJSON *jobjReceived, int *clientUserIdx);
void handle_validate_user_request(cJSON *jobjToSend, char *senderUsername, int *clientUserIdx);
void handle_tweet_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);
void handle_subscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);
void handle_unsubscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);
void handle_timeline_request(cJSON *jobjToSend, int *clientUserIdx);
int handle_exit_request(int *userIdx);
int handle_invalid_request();
void handle_tweet_updates();
void add_tweet_to_user(int userIdx, char *senderUsername, char *ttweetString, char *originHashtag);
void initialize_user_array();
void initialize_latest_tweet();
void create_json_server_payload(cJSON *jobjToSend, int commandCode, int userIdx, char *detailedMessage);
void add_pending_tweets_to_jobj(cJSON *jobj, int userIdx);
void store_latest_tweet(cJSON *jobjReceived, char *senderUsername);
void print_active_users();
void print_latest_tweet();
void print_pending_tweets(int userIdx);
void clear_user_at_index(int *userIdx);

/* Global variables */
unsigned int childProcCount; /* Number of child processes */
LatestTweet *latestTweet;
User *activeUsers;

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

  /* Set child_exit_signal_handler() as handler function */
  signalHandler.sa_handler = child_exit_signal_handler;
  if (sigfillset(&signalHandler.sa_mask) < 0) /* mask all signals */
    die_with_error("sigfillset() failed");
  /* SA_RESTART causes interrupted system calls to be restarted */
  signalHandler.sa_flags = SA_RESTART;

  /* Set signal disposition for child-termination signals */
  if (sigaction(SIGCHLD, &signalHandler, 0) < 0)
    die_with_error("sigaction() failed");

  childProcCount = 0;
  latestTweet = mmap(NULL, sizeof(LatestTweet), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  activeUsers = mmap(NULL, sizeof(User) * MAX_CONC_CONN, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  /* Initialize activeUsers array */
  initialize_user_array();
  initialize_latest_tweet();

  while (1) /* run forever */
  {
    clntSock = accept_tcp_connection(servSock);
    /* Fork child process and report any errors */
    if ((processID = fork()) < 0)
      die_with_error("fork() failed");
    else if (processID == 0) /* If this is the child process */
    {
      close(servSock); /* Child closes parent socket file descriptor */
      if (childProcCount < MAX_CONC_CONN)
      {
        handle_ttweet_client(clntSock);
      }
      else
      {
        reject_ttweet_client(clntSock);
      }

      exit(0); /* Child process done */
    }

    printf("with child process: %d\n", (int)processID);
    close(clntSock);  /* Parent closes child socket descriptor */
    childProcCount++; /* Increment number of outstanding child processes */
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
    {
      childProcCount--; /* Cleaned up after a child */
    }
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
  char objReceived[MAX_RESP_LEN];
  int clientUserIdx = INVALID_USER_INDEX;
  int loop = 1;

  while (loop)
  {
    // print_active_users();
    cJSON *jobjReceived = cJSON_CreateObject();
    receive_response(clntSocket, objReceived);
    jobjReceived = cJSON_Parse(objReceived);
    loop = handle_client_response(clntSocket, jobjReceived, &clientUserIdx);
    cJSON_Delete(jobjReceived);
  }

  close(clntSocket); /* Close client socket */
}

void reject_ttweet_client(int clntSocket)
{
  char objReceived[MAX_RESP_LEN];
  int clientUserIdx = INVALID_USER_INDEX;

  cJSON *jobjReceived = cJSON_CreateObject();
  receive_response(clntSocket, objReceived);
  jobjReceived = cJSON_Parse(objReceived);
  handle_client_response(clntSocket, jobjReceived, &clientUserIdx);
  cJSON_Delete(jobjReceived);

  close(clntSocket); /* Close client socket */
}

int handle_client_response(int clntSocket, cJSON *jobjReceived, int *clientUserIdx)
{
  int requestCode;
  char *senderUsername;
  char *receiverUsername;
  cJSON *jobjToSend = cJSON_CreateObject();

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
  case REQ_TIMELINE:
    handle_timeline_request(jobjToSend, clientUserIdx);
    break;
  case REQ_EXIT:
    return handle_exit_request(clientUserIdx);
    break;
  case REQ_INVALID:
  default:
    return handle_invalid_request(clientUserIdx);
  }

  send_payload(clntSocket, jobjToSend);

  cJSON_Delete(jobjToSend);
  return 1;
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
        return;
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
        activeUsers[userIdx].isOccupied = 1; /* mark index as occupied */
        strcpy(activeUsers[userIdx].username, senderUsername);
        *clientUserIdx = userIdx;
        create_json_server_payload(jobjToSend, RES_USER_VALID, userIdx, "Username is valid.");
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
  //print_latest_tweet();
  handle_tweet_updates();
  create_json_server_payload(jobjToSend, RES_TWEET, *clientUserIdx, "Tweeted successfully.\n");
}

void handle_subscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx)
{
  int isSubscriptionExists = 0;
  int isSubscriptionsFull = 1;
  char *subscriptionHashtag = cJSON_GetObjectItemCaseSensitive(jobjReceived, "subscriptionHashtag")->valuestring;

  for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
  {
    if (strcmp(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], "") != 0)
    { /* subscription exists in this position of the user's subscriptions array */
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
    create_json_server_payload(jobjToSend, RES_SUBSCRIBE, *clientUserIdx, "Subscription list full. Please unsubscribe to a hashtag first!\n");
  }
  else if (isSubscriptionExists)
  {
    create_json_server_payload(jobjToSend, RES_SUBSCRIBE, *clientUserIdx, "Subscription already exists.\n");
  }
  else
  {
    for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
    {
      if (strcmp(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], "") == 0)
      { /* found an empty slot for subscription */
        strcpy(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], subscriptionHashtag);
        if (strcmp(subscriptionHashtag, "ALL") == 0)
        {
          activeUsers[*clientUserIdx].isSubscribedAll = 1;
        }
        break;
      }
    }
    create_json_server_payload(jobjToSend, RES_SUBSCRIBE, *clientUserIdx, "Successfully subscribed.\n");
  }
}

void handle_unsubscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx)
{
  int isSubscriptionExists = 0;
  char *subscriptionHashtag = cJSON_GetObjectItemCaseSensitive(jobjReceived, "subscriptionHashtag")->valuestring;

  for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
  {
    if (strcmp(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], "") != 0)
    { /* subscription exists in this position of the user's subscriptions array */
      if (strcmp(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], subscriptionHashtag) == 0)
      { /* subscription hashtag exists */
        isSubscriptionExists = 1;
        strcpy(activeUsers[*clientUserIdx].subscriptions[subscriptionIdx], "");
        if (strcmp(subscriptionHashtag, "ALL") == 0)
        {
          activeUsers[*clientUserIdx].isSubscribedAll = 0;
        }
        break;
      }
    }
  }
  if (isSubscriptionExists)
  {
    create_json_server_payload(jobjToSend, RES_UNSUBSCRIBE, *clientUserIdx, "Successfully unsubscribed.\n");
  }
  else
  {
    create_json_server_payload(jobjToSend, RES_UNSUBSCRIBE, *clientUserIdx, "You were not subscribed to that hashtag.\n");
  }
}

void handle_timeline_request(cJSON *jobjToSend, int *clientUserIdx)
{
  create_json_server_payload(jobjToSend, RES_TIMELINE, *clientUserIdx, "");
}

int handle_exit_request(int *userIdx)
{
  /* mark entry as unoccupied */
  clear_user_at_index(userIdx);
  printf("Client at index %d disconnected.\n", *userIdx);
  return 0;
}
int handle_invalid_request()
{
  return 0;
}

void handle_tweet_updates()
{
  for (int userIdx = 0; userIdx < MAX_CONC_CONN; userIdx++)
  {

    if (activeUsers[userIdx].isOccupied)
    { /* if User array element is not occupied, there's no need to check it */
      if (activeUsers[userIdx].isSubscribedAll)
      { /* User is subscribed to ALL - simply add tweet and take first hashtag */
        add_tweet_to_user(userIdx, latestTweet->username, latestTweet->ttweetString, latestTweet->hashtags[0]);
      }
      else
      { /* User is not subscribed to ALL - check if any subscriptions and hashtags match */
        for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
        { /* Iterate over current user's subscriptions */
          for (int hashtagIdx = 0; hashtagIdx < latestTweet->numValidHashtags; hashtagIdx++)
          { /* Iterate over lastest tweet's hashtags */
            if (strcmp(activeUsers[userIdx].subscriptions[subscriptionIdx], latestTweet->hashtags[hashtagIdx]) == 0)
            { /* user is subscribed to hashtag */
              add_tweet_to_user(userIdx, latestTweet->username, latestTweet->ttweetString, latestTweet->hashtags[hashtagIdx]);
              subscriptionIdx = MAX_SUBSCRIPTIONS + 1;
              hashtagIdx = MAX_HASHTAG_CNT + 1;
            }
          }
        }
      }
    }
  }
}

void add_tweet_to_user(int userIdx, char *senderUsername, char *ttweetString, char *originHashtag)
{
  char tweetItem[MAX_TWEET_ITEM_LEN];

  strcpy(tweetItem, activeUsers[userIdx].username);
  strcat(tweetItem, " ");
  strcat(tweetItem, senderUsername);
  strcat(tweetItem, ": ");
  strcat(tweetItem, ttweetString);
  strcat(tweetItem, " #");
  strcat(tweetItem, originHashtag);

  for (int pendingTweetIdx = 0; pendingTweetIdx < MAX_TWEET_QUEUE; pendingTweetIdx++)
  {
    if (strcmp(activeUsers[userIdx].pendingTweets[pendingTweetIdx], "") == 0)
    { /* spot available */
      strcpy(activeUsers[userIdx].pendingTweets[pendingTweetIdx], tweetItem);
      return;
    }
  }
  printf("Client %s: Queue full. Tweet was not stored.\n", senderUsername);
}

void initialize_user_array()
{
  for (int i = 0; i < MAX_CONC_CONN; i++)
  {
    (activeUsers + i)->isOccupied = 0;
    (activeUsers + i)->isSubscribedAll = 0;
    strcpy((activeUsers + i)->username, "");
    for (int j = 0; j < MAX_SUBSCRIPTIONS; j++)
    {
      strcpy((activeUsers + i)->subscriptions[j], "");
    }

    for (int j = 0; j < MAX_TWEET_QUEUE - 1; j++)
    {
      strcpy((activeUsers + i)->pendingTweets[j], "");
    }
  }
}

void initialize_latest_tweet()
{
  latestTweet->tweetID = 0;
  strcpy(latestTweet->username, "");
  strcpy(latestTweet->ttweetString, "");
  latestTweet->numValidHashtags = 0;
  for (int hashtagIdx = 0; hashtagIdx < MAX_HASHTAG_CNT; hashtagIdx++)
  {
    strcpy(latestTweet->hashtags[hashtagIdx], "");
  }
}

void create_json_server_payload(cJSON *jobjToSend, int commandCode, int userIdx, char *detailedMessage)
{

  cJSON_AddItemToObject(jobjToSend, "responseCode", cJSON_CreateNumber(commandCode)); /*Add command to JSON object*/
  cJSON_AddItemToObject(jobjToSend, "clientUserIdx", cJSON_CreateNumber(userIdx));    /*Add user index to JSON object*/
  cJSON_AddItemToObject(jobjToSend, "detailedMessage", cJSON_CreateString(detailedMessage));

  switch (commandCode)
  {
  case RES_TIMELINE:
    add_pending_tweets_to_jobj(jobjToSend, userIdx);
    break;
  case RES_SUBSCRIBE:
  case RES_UNSUBSCRIBE:
  case RES_TWEET:
  case RES_EXIT:
  case RES_USER_VALID:
    cJSON_AddItemToObject(jobjToSend, "username", cJSON_CreateString(activeUsers[userIdx].username)); /*Add username to JSON object*/
    break;
  case RES_USER_INVALID:
    cJSON_AddItemToObject(jobjToSend, "username", cJSON_CreateString("Invalid username.")); /*Add username to JSON object*/
    break;
  default:
    die_with_error("Error! create_json_server_payload() received an invalid request.");
    break;
  }
}

void add_pending_tweets_to_jobj(cJSON *jobj, int userIdx)
{
  cJSON *jarray = cJSON_CreateArray(); /*Creating a json array*/

  if (strcmp(activeUsers[userIdx].pendingTweets[0], "") == 0)
  { /* no pending tweets */
    cJSON_AddItemToArray(jarray, cJSON_CreateString("No tweets available"));
  }
  else
  {
    for (int pendingTweetIdx = 0; pendingTweetIdx < MAX_TWEET_QUEUE; pendingTweetIdx++)
    {
      if (strcmp(activeUsers[userIdx].pendingTweets[pendingTweetIdx], "") == 0)
      { /* spot available */
        break;
      }
      else
      {
        cJSON_AddItemToArray(jarray, cJSON_CreateString(activeUsers[userIdx].pendingTweets[pendingTweetIdx]));
        strcpy(activeUsers[userIdx].pendingTweets[pendingTweetIdx], "");
      }
    }
  }
  cJSON_AddItemToObject(jobj, "storedTweets", jarray); /*Add tweets to JSON object*/
}

void store_latest_tweet(cJSON *jobjReceived, char *senderUsername)
{
  (latestTweet->tweetID)++;
  strcpy(latestTweet->username, senderUsername);
  strcpy(latestTweet->ttweetString, cJSON_GetObjectItemCaseSensitive(jobjReceived, "ttweetString")->valuestring);
  cJSON *jarray = cJSON_GetObjectItemCaseSensitive(jobjReceived, "ttweetHashtags");
  latestTweet->numValidHashtags = cJSON_GetArraySize(jarray);
  for (int i = 0; i < MAX_HASHTAG_CNT; i++)
  {
    strcpy((latestTweet->hashtags)[i], "");
  }
  for (int i = 0; i < latestTweet->numValidHashtags; i++)
  {
    strcpy(latestTweet->hashtags[i], cJSON_GetArrayItem(jarray, i)->valuestring);
  }
}

void print_active_users()
{
  printf("Active users:\n");
  for (int userIdx = 0; userIdx < MAX_CONC_CONN; userIdx++)
  {
    printf("User index %d:\n", userIdx);
    printf("isOccupied: %d\n", activeUsers[userIdx].isOccupied);
    printf("username: %s\n", activeUsers[userIdx].username);
    printf("isSubscribedAll: %d\n", activeUsers[userIdx].isSubscribedAll);
    printf("Subscriptions:\n");
    for (int subscriptionIdx = 0; subscriptionIdx < MAX_SUBSCRIPTIONS; subscriptionIdx++)
    {
      printf("%s\n", activeUsers[userIdx].subscriptions[subscriptionIdx]);
    }
    printf("\nPending Tweets:\n");
    print_pending_tweets(userIdx);
  }
}

void print_latest_tweet()
{
  printf("Latest Tweet:\n");
  printf("Tweet ID: %d\n", latestTweet->tweetID);
  printf("Username: %s\n", latestTweet->username);
  printf("ttweetString: %s\n", latestTweet->ttweetString);
  printf("Hashtags:\n");
  for (int hashtagIdx = 0; hashtagIdx < MAX_HASHTAG_CNT; hashtagIdx++)
  {
    printf("%s\n", latestTweet->hashtags[hashtagIdx]);
  }
}

void print_pending_tweets(int userIdx)
{
  for (int i = 0; i < MAX_TWEET_QUEUE; i++)
  {
    printf("%s\n", activeUsers[userIdx].pendingTweets[i]);
  }
}

void clear_user_at_index(int *userIdx)
{
  activeUsers[*userIdx].isOccupied = 0;
  activeUsers[*userIdx].isSubscribedAll = 0;
  strcpy(activeUsers[*userIdx].username, "");
  for (int j = 0; j < MAX_SUBSCRIPTIONS; j++)
  {
    strcpy(activeUsers[*userIdx].subscriptions[j], "");
  }

  for (int j = 0; j < MAX_TWEET_QUEUE - 1; j++)
  {
    strcpy(activeUsers[*userIdx].pendingTweets[j], "");
  }
}