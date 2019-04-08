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


#ifndef TTWEET_COMMON_H
#define TTWEET_COMMON_H
#include "../dependencies/ttweet_common.h"
void die_with_error(char *errorMessage);
int persist_with_error(char *errorMessage);
int send_payload(int sock, cJSON *jobjToSend);
void receive_response(int sock, cJSON *jobjReceived);
#endif

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
int parse_hashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);                                                                             /* Parses hashtags from user input */
void save_current_hashtag(char *currentHashtagBuffer, int *currentHashtagBufferIdx, char *validHashtags[], int *numValidHashtags);                                 /* Save current hashtag buffer */
void create_json_client_payload(cJSON *jobjToSend, int commandCode, char *username, int userIdx, char *ttweetString, char *validHashtags[], int numValidHashtags); /* Create a JSON client payload */
void reset_client_variables(int *clientCommandSuccess, char *validHashtags[], int *numValidHashtags, cJSON *jobjToSend);                                           /* Resets client variables to prepare for the next command */
void deallocate_string_array(char *stringArray[], int numStringsInArray);                                                                                          /* Deallocates memory from a dynamic string array */
int has_duplicate_string(char *stringArray[], int numStringsInArray);                                                                                              /* Checks for duplicates in string array */
int parse_client_command(char inputHashtags[], char ttweetString[]);
int check_tweet_cmd(char clientInput[], int charIdx, char inputHashtags[], char ttweetString[]);
int check_subscribe_cmd(char clientInput[], int charIdx, char inputHashtags[]);
int check_unsubscribe_cmd(char clientInput[], int charIdx, char inputHashtags[]);
int check_timeline_cmd(int endOfCmd);
int check_exit_cmd(int endOfCmd);
void create_json_client_payload(cJSON *jobjToSend, int commandCode, char *username, int userIdx, char *ttweetString, char *validHashtags[], int numValidHashtags);
void handle_server_response(cJSON *jobjReceived, int *userIdx);

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

/**
 * @brief Checks for duplicates in string array
 *
 * Description If a hashtag appears more than once, an error is thrown.
 *
 * @param stringArray Strings in an array
 * @param numStringsInArray Number of strings in stringArray
 * @return int 1 if duplicates exist; 0 otherwise. 
 */

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

/**
 * @brief Deallocates memory from a dynamic string array
 *
 * Description First numStringsInArray elements from stringArray are freed.
 *
 * @param stringArray String array which memory is to be deallocated
 * @param numStringsInArray Number of elements to free in the array
 * @return void
 */

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
