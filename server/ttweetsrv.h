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
  * @file ttweetsrv.h
  * @author Jordan396
  * @date 13 April 2019
  * @brief Documentation for functions in ttweetsrv.c.
  *
  * This header file has been created to describe the functions in ttweetsrv.c.
  * 
  * For an overview of what this program does, visit <https://github.com/Jordan396/trivial-twitter-v2>.
  * 
  * Code is documented according to GNOME and Doxygen standards.
  * <https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en>
  * <http://www.doxygen.nl/manual/docblocks.html>
  */

#ifndef TTWEET_COMMON_H
#define TTWEET_COMMON_H
#include "../dependencies/ttweet_common.h"
void die_with_error(char *errorMessage);
int persist_with_error(char *errorMessage);
int send_payload(int sock, cJSON *jobjToSend);
void receive_response(int sock, char *objReceived);
#endif

typedef struct LatestTweet
{
  int tweetID;
  char username[MAX_USERNAME_LEN];
  char ttweetString[MAX_TWEET_LEN + 1]; /* +1 is for null terminator */
  char hashtags[MAX_HASHTAG_CNT][MAX_HASHTAG_LEN];
  int numValidHashtags;
} LatestTweet;

typedef struct User
{
  int isOccupied;
  char username[MAX_USERNAME_LEN];
  char pendingTweets[MAX_TWEET_QUEUE][MAX_TWEET_ITEM_LEN];
  int pendingTweetsSize;
  char subscriptions[MAX_SUBSCRIPTIONS][MAX_HASHTAG_LEN];
  int isSubscribedAll;
} User;

/* functions to handle child processes */
void child_exit_signal_handler(); /* Clean up zombie child processes */

/* functions to handle connections */
int create_tcp_serv_socket(unsigned short port); /* Creates TCP server socket */
int accept_tcp_connection(int servSock);         /* Accepts and maintains a TCP connection */
void handle_ttweet_client(int clntSocket);       /* Handles connection with client */
void reject_ttweet_client(int clntSocket);       /* Sends a rejection message and closes connection */

/* functions to initialize global variables */
void initialize_user_array();   /* Initialize activeUser array */
void initialize_latest_tweet(); /* Initialize latest tweet */

/* functions to support transmission of data */
void create_json_server_payload(cJSON *jobjToSend, int commandCode, int userIdx, char *detailedMessage); /* Creates a JSON payload to be send to client */

/* functions to handle client commands */
int handle_client_response(int clntSocket, cJSON *jobjReceived, int *clientUserIdx);                               /* Handles client response */
void handle_validate_user_request(cJSON *jobjToSend, char *senderUsername, int *clientUserIdx);                    /* Handles validate user request */
void handle_tweet_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);       /* Handles tweet request */
void handle_subscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);   /* Handles subscribe request */
void handle_unsubscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx); /* Handles unsubscribe request */
void handle_timeline_request(cJSON *jobjToSend, int *clientUserIdx);                                               /* Handles timeline request */
int handle_exit_request(int *userIdx);                                                                             /* Handles exit request */
int handle_invalid_request();                                                                                      /* Handles invalid request */

/* functions to support above handling functions */
void handle_tweet_updates();                                                                        /* Updates tweets across all clients */
void add_tweet_to_user(int userIdx, char *senderUsername, char *ttweetString, char *originHashtag); /* Adds a tweet to a user */
void add_pending_tweets_to_jobj(cJSON *jobj, int userIdx);                                          /* Adds pending tweets to JSON obj */
void store_latest_tweet(cJSON *jobjReceived, char *senderUsername);                                 /* Stores to last received tweet */
void clear_user_at_index(int *userIdx);                                                             /* Clears user space at specified index */

/* functions for debugging */
void print_active_users();              /* Print activeUsers */
void print_latest_tweet();              /* Print latest tweet */
void print_pending_tweets(int userIdx); /* Print pending tweets for a specified user */
