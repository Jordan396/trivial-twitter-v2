/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/Trivial-Twitter-Sockets>*
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

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 32 /* Size of receive buffer */

/**
 * @brief Error handling function
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void DieWithError(char *errorMessage);
