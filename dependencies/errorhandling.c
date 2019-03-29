/****************************************************************************
 * @author: Jordan396 <https://github.com/Jordan396/trivial-twitter-v2>     *
 *                                                                          *
 *   You should have received a copy of the MIT License when cloning this   *
 *   repository. If not, see <https://opensource.org/licenses/MIT>.         *
 ****************************************************************************/

/**
  * @file errorhandling.c
  * @author Jordan396
  * @date 29 March 2019
  * @brief This file contains error handling functions for trivial twitter v2.
  */

/** \copydoc DieWithError */
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

/** \copydoc PersistWithError */
int PersistWithError(char *errorMessage)
{
    perror(errorMessage);
    return -1;
}