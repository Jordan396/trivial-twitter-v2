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
  * @date 13 April 2019
  * @brief Documentation for functions in ttweetcli.c.
  *
  * This header file has been created to describe the functions in ttweetcli.c.
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
void waitFor(unsigned int secs);
void receive_response(int sock, char *objReceived);
#endif

/**
 * @brief Reads user input from stdin
 *
 * Description Reads user input from stdin. Also,
 * checks if user input exceeds maximum buffer size.
 *
 * @param clientInput Buffer to store user input.
 * @return int 0 if error occurred, 1 otherwise.
 */
int get_client_input(char *clientInput);

/**
 * @brief Parses command from user input
 *
 * Description Parses command from user input.
 * Hashtag and tweet message fields are saved accordingly.
 *
 * @param ttweetString Tweet message to be sent.
 * @param inputHashtags Raw hashtag input from the user.
 * @return int Request code of corresponding command, or error code if error thrown.
 */
int parse_client_command(char inputHashtags[], char ttweetString[]);

/**
 * @brief Parses hashtags from user command
 *
 * Description Parses hashtags from user command. Valid hashtags are stored in
 * the string array validHashtags. Hashtag count is tracked using numValidHashtags.
 *
 * @param validHashtags Valid hashtags
 * @param numValidHashtags Number of hashtags in validHashtags
 * @param inputHashtags Hashatag input from the user.
 * @return void
 */
int parse_hashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);

/**
 * @brief Checks for duplicates in string array
 *
 * Description If a hashtag appears more than once, an error is thrown.
 *
 * @param stringArray Strings in an array
 * @param numStringsInArray Number of strings in stringArray
 * @return int 1 if duplicates exist; 0 otherwise. 
 */
int has_duplicate_string(char *stringArray[], int numStringsInArray); /* Checks for duplicates in string array */

/**
 * @brief Checks if hashtag #ALL exists
 *
 * Description Checks if hashtag #ALL exists in validHashtags.
 * This is necessary since users cannot tweet with #ALL.
 *
 * @param validHashtags Valid hashtags
 * @param numValidHashtags Number of hashtags in validHashtags
 * @return int 0 if exists; 1 otherwise.
 */
int is_hashtag_all_exists(char *validHashtags[], int numValidHashtags);

/**
 * @brief  Resets client variables for next command 
 *
 * Description Variables relating to a particular ttweet command
 * are cleared/reset to prepare for next command.
 *
 * @param clientCommandSuccess Boolean to check command validity.
 * @param validHashtags Valid hashtags
 * @param numValidHashtags Number of hashtags in validHashtags
 * @param jobjToSend cJSON object to be sent
 * @return void
 */
void reset_client_variables(int *clientCommandSuccess, char *validHashtags[], int *numValidHashtags, cJSON *jobjToSend);

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
 * @brief Save current hashtag buffer
 *
 * Description Allocates memory to validHashtag and stores the current hashtag buffer.
 * Prepare the hashtag buffer to receive the next hashtag.
 *
 * @param currentHashtagBuffer Buffer to store the current hashtag
 * @param currentHashtagBufferIndex Index of the last known char in hashtag buffer
 * @param validHashtags Valid hashtags
 * @param numValidHashtags Number of hashtags in validHashtags
 * @return void
 */
void save_current_hashtag(char *currentHashtagBuffer, int *currentHashtagBufferIdx, char *validHashtags[], int *numValidHashtags);

/**
 * @brief Creates payload to send to server
 *
 * Description Creates a JSON object payload.
 * Depending on client command, additional fields are added
 * to the JSON object. This object is to be sent to the server.
 *
 * @param jobjToSend cJSON object to be sent
 * @param commandCode Request code of command
 * @username Client username
 * @param userIdx Client user index
 * @param ttweetString Tweet message to be sent.
 * @param validHashtags Valid hashtags
 * @param numValidHashtags Number of hashtags in validHashtags
 * @return void
 */
void create_json_client_payload(cJSON *jobjToSend, int commandCode, char *username, int userIdx, char *ttweetString, char *validHashtags[], int numValidHashtags);

/**
 * @brief Handles server response
 *
 * Description Handles server response according to response code.
 *
 * @param jobjReceived cJSON object received from server
 * @param userIdx Client user index
 * @return void
 */
void handle_server_response(cJSON *jobjReceived, int *userIdx);

/**
 * @brief Parses and validates tweet command 
 *
 * Description Parses tweet command and saves tweet message and
 * hashtags. Also checks for errors in user input.
 *
 * @param clientInput Buffer to store user input.
 * @param charIdx Index of character in clientInput
 * @param inputHashtags Raw hashtag input from the user.
 * @param ttweetString Tweet message to be sent.
 * @return int Tweet command request code if command valid; 0 otherwise.
 */
int check_tweet_cmd(char clientInput[], int charIdx, char inputHashtags[], char ttweetString[]);

/**
 * @brief Parses and validates subscribe command 
 *
 * Description Parses subscribe command and saves target hashtag. 
 * Also checks for errors in user input.
 *
 * @param clientInput Buffer to store user input.
 * @param charIdx Index of character in clientInput
 * @param inputHashtags Raw hashtag input from the user.
 * @return int Subscribe command request code if command valid; 0 otherwise.
 */
int check_subscribe_cmd(char clientInput[], int charIdx, char inputHashtags[]);

/**
 * @brief Parses and validates unsubscribe command 
 *
 * Description Parses unsubscribe command and saves target hashtag. 
 * Also checks for errors in user input.
 *
 * @param clientInput Buffer to store user input.
 * @param charIdx Index of character in clientInput
 * @param inputHashtags Raw hashtag input from the user.
 * @return int Unsubscribe command request code if command valid; 0 otherwise.
 */
int check_unsubscribe_cmd(char clientInput[], int charIdx, char inputHashtags[]);

/**
 * @brief Parses and validates timeline command 
 *
 * Description Parses timeline command.
 * Also checks for errors in user input.
 *
 * @param endOfCmd Boolean to check if end of command reached.
 * @return int Timeline command request code if command valid; 0 otherwise.
 */
int check_timeline_cmd(int endOfCmd);

/**
 * @brief Parses and validates exit command 
 *
 * Description Parses exit command.
 * Also checks for errors in user input.
 *
 * @param endOfCmd Boolean to check if end of command reached.
 * @return int Exit command request code if command valid; 0 otherwise.
 */
int check_exit_cmd(int endOfCmd);
