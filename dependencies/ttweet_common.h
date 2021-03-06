/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file ttweet_common.h
  * @author Jordan396
  * @date 13 April 2019
  * @brief Documentation for functions in ttweet_common.h.
  *
  * This header file has been created to describe the functions 
  * and declare constants in ttweet_common.h.
  * 
  * For an overview of what this program does, visit <https://github.com/Jordan396/trivial-twitter-v2>.
  * 
  * Code is documented according to GNOME and Doxygen standards.
  * <https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en>
  * <http://www.doxygen.nl/manual/docblocks.html>
  */

/* Connections */
#define MAX_PENDING 5   /* Maximum outstanding connection requests */
#define MAX_CONC_CONN 5 /* Maximum number of concurrent connections */

/* Restrictions on user input */
#define MAX_USERNAME_LEN 30
#define MAX_SUBSCRIPTIONS 3
#define MAX_TWEET_LEN 150
#define MAX_HASHTAG_CNT 8
#define MAX_HASHTAG_LEN 25
#define RCV_BUF_SIZE 32   /* Size of receive buffer */
#define MAX_RESP_LEN 5000 /* Maximum number of characters in response */
#define MAX_TWEET_QUEUE 15
#define MAX_TWEET_ITEM_LEN 250
#define MAX_CLI_INPUT_LEN 300

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
#define RES_USER_VALID 16
#define RES_USER_INVALID 17

/* Other constants */
#define INVALID_USER_INDEX 72

/* Standard libraries */
#define _GNU_SOURCE
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <signal.h>     /* for sigaction() */
#include <ctype.h>      /* for char validation */
#include <time.h>       /* for waitFor() */
#include <sys/mman.h>   /* to create shared memory across child processes */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <sys/wait.h>   /* for waitpid() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */

/* External libraries */
#include "./cJSON.h"

/**
 * @brief Prints error message and closes the connection and program.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void die_with_error(char *errorMessage);

/**
 * @brief Prints error message but maintains the connection.
 *
 * @param errorMessage Error message to be printed.
 * @return int 0
 */
int persist_with_error(char *errorMessage);

/**
 * @brief Accepts a cJSON object and sends its string representation over a socket.
 *
 * This function converts a cJSON object to its string representation.
 * It then sends this string to the other party on the network.
 * 
 * This payload adopts the following structure:
 * The first RCV_BUF_SIZE bytes indicates the size of the actual payload.
 * The remaining bytes contain the actual cJSON string representation payload.
 *
 * @param sock Client socket assigned to the connection.
 * @param jobjToSend cJSON object to be sent.
 * @return int 0 if error occurred, 1 otherwise.
 */
int send_payload(int sock, cJSON *jobjToSend);

/**
 * @brief Receives a send_payload formatted response and saves it to objReceived.
 *
 * The socket listens for a send_payload formatted response.
 * It then saves the response to an objReceived string.
 * 
 * This reponse adopts the following structure:
 * The first RCV_BUF_SIZE bytes indicates the size of the actual payload.
 * The remaining bytes contain the actual cJSON string representation payload.
 *
 * @param sock Client socket assigned to the connection.
 * @param objReceived String to save the response recieved.
 * @return void
 */
void receive_response(int sock, char *objReceived);

/**
 * @brief Waits for secs amount of seconds.
 *
 * @param secs Number of seconds to wait for.
 * @return void
 */
void wait_for(unsigned int secs);
