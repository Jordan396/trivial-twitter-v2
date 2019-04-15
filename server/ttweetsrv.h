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

/**
 * @brief Clean up zombie child processes
 *
 * Cleans up zombie child processes and tracks the number of active child processes
 * so as to limit the maximum number of concurrent connections with clients.
 * 
 * @return void
 */
void child_exit_signal_handler();

/**
 * @brief Creates TCP server socket
 *
 * @param port Port assigned to the server program
 * @return int The newly created socket number
 */
int create_tcp_serv_socket(unsigned short port);

/**
 * @brief Accepts and maintains a TCP connection 
 *
 * Performs the accept() step to establish
 * a persistent TCP connection with the client.
 *
 * @param servSock Server socket which was assigned to run the server program
 * @return int Server socket after accepting the connection
 */
int accept_tcp_connection(int servSock);

/**
 * @brief Handles connection with client
 *
 * This function is called when number of active connections is less than maximum number of 
 * concurrent connections. This function delegates work to other functions according to the request 
 * sent by the client. It loops continuously to perform the sending and receiving of data with the client.
 *
 * @param clntSocket Server socket after accepting the connection
 * @return void
 */
void handle_ttweet_client(int clntSocket);

/**
 * @brief Sends a rejection message and closes connection
 *
 * Sends a rejection message to the client and closes the connection.
 * This function is called when number of active connections is equal to or greater than
 * the maximum number of concurrent connections.
 *
 * @param clntSocket Server socket after accepting the connection
 * @return void
 */
void reject_ttweet_client(int clntSocket);

/**
 * @brief Initialize activeUsers array
 *
 * This function sets the structure fields to a predefined value to 
 * prevent unexpected behaviour due to uninitialized fields.
 *
 * @return void
 */
void initialize_user_array();

/**
 * @brief Initialize latestTweet
 *
 * This function sets the structure fields to a predefined value to 
 * prevent unexpected behaviour due to uninitialized fields.
 *
 * @return void
 */
void initialize_latest_tweet();

/**
 * @brief Creates a JSON payload to be send to client
 *
 * Depending on server response, additional fields are added to the JSON object. 
 * 
 * @param jobjToSend cJSON object to be sent
 * @param commandCode Response code of command
 * @param userIdx Client user index
 * @param detailedMessage String to provide client with a more informative response to its request
 * @return void
 */
void create_json_server_payload(cJSON *jobjToSend, int commandCode, int userIdx, char *detailedMessage);

/**
 * @brief Handles client response
 *
 * Handles response to the client by calling the handler 
 * functions corresponding to the client's request code.
 *
 * @param clntSocket Server socket after accepting the connection
 * @param jobjReceived cJSON object received
 * @param clientUserIdx Client user index
 * @return int 0 for requests leading to server shutting down connection; 1 otherwise.
 */
int handle_client_response(int clntSocket, cJSON *jobjReceived, int *clientUserIdx);

/**
 * @brief  Handles validate user request
 *
 * This function checks if the client's submitted username is valid. 
 * If so, it creates a payload with a flag indicating valid.
 * Otherwise, it creates a payload with a flag indicating invalid.
 *
 * @param jobjToSend cJSON object to be sent
 * @param senderUsername Client username
 * @param clientUserIdx Client user index
 * @return void
 */
void handle_validate_user_request(cJSON *jobjToSend, char *senderUsername, int *clientUserIdx);

/**
 * @brief Handles tweet request
 *
 * This function calls other functions which perform operations 
 * to handle receiving of a tweet from any client.
 *
 * @param jobjToSend cJSON object to be sent
 * @param jobjReceived cJSON object received
 * @param senderUsername Client username
 * @param clientUserIdx Client user index
 * @return void
 */
void handle_tweet_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);

/**
 * @brief Handles subscribe request
 *
 * @param jobjToSend cJSON object to be sent
 * @param jobjReceived cJSON object received
 * @param senderUsername Client username
 * @param clientUserIdx Client user index
 * @return void
 */
void handle_subscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);

/**
 * @brief Handles unsubscribe request
 *
 * @param jobjToSend cJSON object to be sent
 * @param jobjReceived cJSON object received
 * @param senderUsername Client username
 * @param clientUserIdx Client user index
 * @return void
 */
void handle_unsubscribe_request(cJSON *jobjToSend, cJSON *jobjReceived, char *senderUsername, int *clientUserIdx);

/**
 * @brief Handles timeline request
 *
 * @param jobjToSend cJSON object to be sent
 * @param clientUserIdx Client user index
 * @return void
 */
void handle_timeline_request(cJSON *jobjToSend, int *clientUserIdx);

/**
 * @brief Handles exit request
 *
 * @param userIdx Client user index
 * @return 0
 */
int handle_exit_request(int *userIdx);

/**
 * @brief Handles invalid request
 *
 * @return 0
 */
int handle_invalid_request();

/**
 * @brief Updates tweets across all clients
 *
 * This function updates pendingTweets in all clients that
 * are subscribed to a hashtag in the latest tweet received.
 *
 * @return int 0 if error occurred, 1 otherwise.
 */
void handle_tweet_updates();

/**
 * @brief Adds a tweet to a user
 *
 * Adds the latest tweet to the user at userIdx.
 *
 * @param userIdx Client user index
 * @param senderUsername Client username
 * @param ttweetString Latest tweet message
 * @param originHashtag The hashtag in latest tweet which also matches that in user's subscriptions
 * @return void
 */
void add_tweet_to_user(int userIdx, char *senderUsername, char *ttweetString, char *originHashtag);

/**
 * @brief Adds pending tweets to JSON obj
 *
 * While transferring tweets to a JSON obj, the user's
 * list of pending tweets are cleared.
 *
 * @param jobj A cJSON object
 * @param userIdx Client user index
 * @return void
 */
void add_pending_tweets_to_jobj(cJSON *jobj, int userIdx);

/**
 * @brief Stores to last received tweet 
 *
 * Saves the latest tweet to the globla latestTweet variable.
 * 
 * @param jobjReceived cJSON object received
 * @param senderUsername Client username
 * @return void
 */
void store_latest_tweet(cJSON *jobjReceived, char *senderUsername);

/**
 * @brief Clears user space at specified index
 *
 * @param userIdx Client user index
 * @return void
 */
void clear_user_at_index(int *userIdx);

/**
 * @brief Prints activeUsers
 *
 * @return void
 */
void print_active_users();

/**
 * @brief Print latest tweet
 *
 * @return void
 */
void print_latest_tweet(); /* Print latest tweet */

/**
 * @brief Print pending tweets for a specified user
 *
 * @return void
 */
void print_pending_tweets(int userIdx);
