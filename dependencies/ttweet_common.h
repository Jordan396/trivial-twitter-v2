/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/* Connections */
#define MAX_PENDING 5   /* Maximum outstanding connection requests */
#define MAX_CONC_CONN 5 /* Maximum number of concurrent connections */

/* Restrictions on user input */
#define MAX_USERNAME_LEN 30 /* Maximum length of username */
#define MAX_SUBSCRIPTIONS 3
#define MAX_TWEET_LEN 150
#define MAX_HASHTAG_CNT 8 /* Limit for hashtag count */
#define MAX_HASHTAG_LEN 25
#define RCV_BUF_SIZE 32   /* Size of receive buffer */
#define MAX_RESP_LEN 5000 /* Maximum number of characters in response */

/* Request codes */
#define REQ_INVALID 0
#define REQ_TWEET 1
#define REQ_SUBSCRIBE 2
#define REQ_UNSUBSCRIBE 3
#define REQ_TIMELINE 4
#define REQ_EXIT 5
#define REQ_VALIDATE_USER 6

/* Response codes */
#define RES_INVALID 10
#define RES_TWEET 11
#define RES_SUBSCRIBE 12
#define RES_UNSUBSCRIBE 13
#define RES_TIMELINE 14
#define RES_EXIT 15
#define RES_VALIDATE_USER 16

/* Standard libraries */
#define _GNU_SOURCE
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>     /* for sigaction() */
#include <ctype.h>      /* for char validation */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <sys/wait.h>   /* for waitpid() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

/* External libraries */
#include <cJSON.h>
#include <linked_lists_str.h>

/**
  * @file ttweet_common.h
  * @author Jordan396
  * @date 29 March 2019
  * @brief Documentation for error handling functions.
  *
  * This header file describes functions used in both client and server side for trivial-twitter-v2.
  */

/**
 * @brief Error handling function for major errors
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void die_with_error(char *errorMessage);

/**
 * @brief Error handling function for minor errors
 *
 * Description Prints the error message and continue program execution.
 *
 * @param errorMessage Error message to be printed.
 * @return -1
 */
int persist_with_error(char *errorMessage);

int send_payload(int sock, cJSON *jobjPayload);
void receive_response(int sock, cJSON *jobjResponse);
