/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

#define MAXPENDING 5      /* Maximum outstanding connection requests */
#define RCV_BUF_SIZE 32   /* Size of receive buffer */
#define MAX_RESP_LEN 5000 /* Maximum number of characters in response */

#define REQ_INVALID 0
#define REQ_TWEET 1
#define REQ_SUBSCRIBE 2
#define REQ_UNSUBSCRIBE 3
#define REQ_TIMELINE 4
#define REQ_EXIT 5
#define REQ_VALIDATE_USER 6

#define RES_INVALID 10
#define RES_TWEET 11
#define RES_SUBSCRIBE 12
#define RES_UNSUBSCRIBE 13
#define RES_TIMELINE 14
#define RES_EXIT 15
#define RES_VALIDATE_USER 16

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
