/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file errorhandling.h
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
 * @return void
 */
void PersistWithError(char *errorMessage);
