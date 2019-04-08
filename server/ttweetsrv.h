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
  * @file ttweetser.h
  * @author Jordan396
  * @date 14 February 2019
  * @brief Documentation for functions in ttweetser.c.
  *
  * This header file has been created to describe the functions in ttweetser.c.
  */

#ifndef TTWEET_COMMON_H
#define TTWEET_COMMON_H
#include "../dependencies/ttweet_common.h"
void die_with_error(char *errorMessage);
int persist_with_error(char *errorMessage);
int send_payload(int sock, cJSON *jobjToSend);
void receive_response(int sock, cJSON *jobjReceived);
#endif

typedef struct LatestTweet
{
  int tweetID;
  char username[MAX_USERNAME_LEN];
  char ttweetString[MAX_TWEET_LEN + 1]; /* +1 is for null char if tweet is MAX_HASHTAG_LEN long */
  char *hashtags[MAX_HASHTAG_LEN];
  int numValidHashtags;
} LatestTweet;

typedef struct User
{
  int isOccupied;
  char username[MAX_USERNAME_LEN];
  ListNode *pendingTweets;
  int pendingTweetsSize;
  char subscriptions[MAX_SUBSCRIPTIONS][MAX_HASHTAG_LEN];
  int isSubscribedAll;
} User;

/**
 * @brief TCP client handling function
 *
 * Description Handles TCP client according to whether client's request is authorized.
 * If authorized, check if request is to upload or download. If download, send ttweetMessage
 * to the client. If upload, receive message from the client and store in ttweetMessage.
 * If unauthorized, notify client of unauthorized request via unauthorizedRequestMessage.
 *
 * @param clntSocket         Socket descriptor for client
 * @param ttweetMessage      Ttweet message stored on the server
 * @param uploadRequestStr   Passphrase to authenticate upload requests
 * @param downloadRequestStr Passphrase to authenticate download requests
 *
 * @return void
 */
void handle_ttweet_client(int clntSocket);
