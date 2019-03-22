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
 * - parseHashtags
 * - saveCurrentHashtag
 * - checkDuplicatesExists
 * - ttweetToJson
 * - freeDynamicStringArray
 * 
 */

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <ctype.h>      /* for char validation */

#define RCVBUFSIZE 32 /* Size of receive buffer */
#define MAX_HASHTAG_CNT 15

/**
 * @brief Error handling function
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void DieWithError(char *errorMessage);

/**
 * @brief 
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void parseHashtags(char *validHashtags[], int *numValidHashtags, char *inputHashtags);

/**
 * @brief 
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void saveCurrentHashtag(char *currentHashtagBuffer, int *currentHashtagBufferIndex, char *validHashtags[], int *numValidHashtags);

/**
 * @brief 
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void checkDuplicatesExists(char *validHashtags[], int numValidHashtags);

/**
 * @brief 
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void ttweetToJson(cJSON *jobj, char *ttweetString, char *validHashtags[], int numValidHashtags);

/**
 * @brief 
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void freeDynamicStringArray(char *stringArray[], int numElementsInArray);