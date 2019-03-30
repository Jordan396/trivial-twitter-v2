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
  * @file ttweetcli.h
  * @author Jordan396
  * @date 14 February 2019
  * @brief Documentation for functions in ttweetcli.c.
  *
  * This header file has been created to describe the functions in ttweetcli.c.
  */

/**
 * TODO:
 * Add documentation for:
 * 
 */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <ctype.h>      /* for char validation */

#define RCVBUFSIZE 32      /* Size of receive buffer */
#define MAX_HASHTAG_CNT 15 /* Limit for hashtag count */
#define MAX_TWEET_LEN 151
#define MAX_HASHTAG_LEN 25
#define MAX_REPLY_LEN 1000

/**
 * @brief Handles connection errors
 *
 * Description Prints the error message, closes the connection
 * to the server and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void throwConnectionError(char *errorMessage);

/**
 * @brief Handles input errors during connection
 *
 * Description Prints the error message and resets
 * ttweet variables. Connection to server persists.
 *
 * @param errorMessage Error message to be printed.
 * @param validHashtags Hashtags in a string array
 * @param numValidHashtags Number of hashtags in validHashtags
 * @return void
 */
void RejectWithError(char *errorMessage, char *validHashtags[], int *numValidHashtags);

/**
 * @brief Parses hashtags from user input
 *
 * Description Validates hashtag input. Valid hashtags are stored in
 * the string array validHashtags. Hashtag count is tracked using numValidHashtags.
 *
 * @param validHashtags Hashtags in a string array
 * @param numValidHashtags Number of hashtags in validHashtags
 * @param inputHashtags Hashatag input from the user.
 * @return void
 */
int parse_hashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);

/**
 * @brief Save current hashtag buffer
 *
 * Description Allocates memory to validHashtag and stores the current hashtag buffer.
 * Prepare the hashtag buffer to receive the next hashtag.
 *
 * @param currentHashtagBuffer Buffer to store the current hashtag
 * @param currentHashtagBufferIndex Index of the last known char in hashtag buffer
 * @param validHashtags Hashtags in a string array
 * @param numValidHashtags Number of hashtags in validHashtags
 * @return void
 */
void save_current_hashtag(char *currentHashtagBuffer, int *currentHashtagBufferIndex, char *validHashtags[], int *numValidHashtags);

/**
 * @brief Checks for duplicates in string array
 *
 * Description If a hashtag appears more than once, an error is thrown.
 *
 * @param stringArray Strings in an array
 * @param numStringsInArray Number of strings in stringArray
 * @return int 1 if duplicates exist; 0 otherwise. 
 */
int checkDuplicatesExists(char *stringArray[], int numStringsInArray);

/**
 * @brief Converts user input to a ttweet JSON object
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param jobj cJSON object to store ttweet
 * @param ttweetString Message to be ttweeted
 * @param validHashtags Hashtags in a string array
 * @param numValidHashtags Number of hashtags in validHashtags
 * @return void
 */
void ttweet_to_json(cJSON *jobj, char *ttweetString, char *validHashtags[], int numValidHashtags);

/**
 * @brief Deallocates memory from a dynamic string array
 *
 * Description First numStringsInArray elements from stringArray are freed.
 *
 * @param stringArray String array which memory is to be deallocated
 * @param numStringsInArray Number of elements to free in the array
 * @return void
 */
void deallocate_string_array(char *stringArray[], int numStringsInArray);

/**
 * @brief Resets client variables to prepare for the next command
 *
 * Description Variables relating to a particular ttweet command
 * are cleared/reset, making it ready for the next command.
 *
 * @param validHashtags Hashtags in a string array
 * @param numValidHashtags Number of hashtags in validHashtags
 * @param jobj cJSON object to store ttweet
 * @return void
 */
void reset_client_variables(int *clientCommandSuccess, char *validHashtags[], int *numValidHashtags, cJSON *jobjPayload);