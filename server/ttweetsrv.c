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

#include <ttweetsrv.h>

void child_exit_signal_handler();          /* Function to clean up zombie child processes */
void die_with_error(char *errorMessage);   /* Error handling function */
void handle_ttweet_client(int clntSocket); /* TCP client handling function */

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

void handle_tcp_client(int clntSocket)
{
  cJSON *jobjResponse;

  while (1)
  {
    receive_response(clntSocket, jobjResponse);
    handle_client_response(jobjResponse);
  }
  

  

  close(clntSocket); /* Close client socket */
}

void handle_client_response(cJSON *jobjResponse)
{
  int requestCode = cJSON_GetObjectItemCaseSensitive(jobjResponse, "requestCode");
  switch (requestCode)
  {
  case REQ_TWEET:
    cJSON *jarray = cJSON_GetObjectItemCaseSensitive(jobjResponse, "storedTweets");
    ////////////////////////////
    for (int i = 0; i < cJSON_GetArraySize(jarray); i++)
    {
      printf("%s\n", cJSON_GetArrayItem(jarray, i));
    }
    break;
  case RES_VALIDATE_USER:
    int isUserValid = cJSON_GetObjectItemCaseSensitive(jobjResponse, "isUserValid");
    if (isUserValid)
    {
      printf("Username legal. Connection established.");
    }
    else
    {
      die_with_error("Username already taken. Please try again with a different username.");
    }
  default:
    die_with_error("Error! Server sent an invalid response code.");
    break;
  }
}

void handle_tweet_updates()
{
  int lastCheckedTweetID = 0;
  int matchFound;
  while (1)
  {
    if (lastCheckedTweetID != latestTweet.tweetID)
    {
      for (int userIdx = 0; userIdx < MAX_CONC_CONN; userIdx++)
      {
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

void add_tweet_to_user(User client, char *senderUsername, char *ttweetString, char originHashtag)
{
  char tweetItem[MAX_ITEM_LEN];
  strcpy(tweetItem, client.username);
  strcat(tweetItem, " ");
  strcat(tweetItem, senderUsername);
  strcat(tweetItem, ": ");
  strcat(tweetItem, ttweetString);
  strcat(tweetItem, " ");
  strcat(tweetItem, originHashtag);

  insertNode(client.pendingTweets, client.pendingTweetsSize, tweetItem);
}