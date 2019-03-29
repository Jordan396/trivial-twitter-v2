/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

#define MAXPENDING 5  /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32 /* Size of receive buffer */

#define CMD_VALIDATE_USER 0
#define CMD_TWEET 1
#define CMD_SUBSCRIBE 2
#define CMD_UNSUBSCRIBE 3
#define CMD_TIMELINE 4
#define CMD_EXIT 5

/**
  * @file shared_functions.h
  * @author Jordan396
  * @date 29 March 2019
  * @brief Documentation for error handling functions.
  *
  * This header file has been created to describe the error handling functions in trivial twitter v2.
  */

/**
 * @brief Error handling function for major errors
 *
 * Description Prints the error message and exits the program gracefully.
 *
 * @param errorMessage Error message to be printed.
 * @return void
 */
void DieWithError(char *errorMessage);

/**
 * @brief Error handling function for minor errors
 *
 * Description Prints the error message and continue program execution.
 *
 * @param errorMessage Error message to be printed.
 * @return -1
 */
int PersistWithError(char *errorMessage);

void receiveResponse(int sock, char *ttweetBuffer, char *ttweetserReply);
