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

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/wait.h>   /* for waitpid() */
#include <signal.h>     /* for sigaction() */

/**
 * @brief Error handling function
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void die_with_error(char *errorMessage);

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
void handle_ttweet_client(int clntSocket, char *ttweetMessage, char *uploadRequestStr, char *downloadRequestStr);
