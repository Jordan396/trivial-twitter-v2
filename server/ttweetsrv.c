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
  * @file ttweetser.c
  * @author Jordan396
  * @date 14 February 2019
  * @brief ttweetser handles upload/download requests from ttweetcli clients.
  *
  * This file is to be compiled and executed on the server side.
  * ttweetser runs continuously to listen for client requests. When a client
  * sends a request, ttweetser checks if request is unauthorized or authorized.
  * If authorized, check if request is to upload or download. If download, send ttweetString
  * to the client. If upload, receive message from the client and store in ttweetString.
  * If unauthorized, notify client of unauthorized request via unauthorizedRequestMessage.
  */

#include "ttweetsrv.h"
#include "shared_functions.h"
#include "cJSON.h"

void child_exit_signal_handler();     /* Function to clean up zombie child processes */
void die_with_error(char *errorMessage);                                                                      /* Error handling function */
void handle_ttweet_client(int clntSocket, char *ttweetString, char *uploadRequestStr, char *downloadRequestStr); /* TCP client handling function */

/* Global so accessable by SIGCHLD signal handler */
unsigned int childProcCount = 0;   /* Number of child processes */

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    unsigned short ttweetServPort;     /* Server port */
    pid_t processID;                 /* Process ID from fork() */
    struct sigaction myAction;       /* Signal handler specification structure */
 
    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: ./ttweetsrv <Port>\n", argv[0]);
        exit(1);
    }

    ttweetServPort = atoi(argv[1]);  /* First arg:  local port */

    servSock = CreateTCPServerSocket(ttweetServPort);

    /* Set child_exit_signal_handler() as handler function */
    myAction.sa_handler =  child_exit_signal_handler;
    if (sigfillset(&myAction.sa_mask) < 0)   /* mask all signals */
        die_with_error("sigfillset() failed");
    /* SA_RESTART causes interrupted system calls to be restarted */
    myAction.sa_flags = SA_RESTART;

    /* Set signal disposition for child-termination signals */
    if (sigaction(SIGCHLD, &myAction, 0) < 0)
        die_with_error("sigaction() failed");

    for (;;) /* run forever */
    {
	clntSock = AcceptTCPConnection(servSock);
        /* Fork child process and report any errors */
        if ((processID = fork()) < 0)
            die_with_error("fork() failed");
        else if (processID == 0)  /* If this is the child process */
        {
            close(servSock);   /* Child closes parent socket file descriptor */
            handle_ttweet_client(clntSock);
            exit(0);              /* Child process done */
        }

	printf("with child process: %d\n", (int) processID);
        close(clntSock);       /* Parent closes child socket descriptor */
        childProcCount++;      /* Increment number of outstanding child processes */
    }
    /* NOT REACHED */
}

void child_exit_signal_handler()
{
    pid_t processID;           /* Process ID from fork() */

    while (childProcCount) /* Clean up all zombies */
    {
	processID = waitpid((pid_t) -1, NULL, WNOHANG);  /* Non-blocking wait */
	if (processID < 0)  /* waitpid() error? */
	    die_with_error("waitpid() failed");
	else if (processID == 0)  /* No child to wait on */
	    break;
	else
	    childProcCount--;  /* Cleaned up after a child */
    }
}