/**
 * @file Electric-gym.c
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Main driver for the application. Contains the main function.
 *
 * The main function is intended to only initialize memory and run an 
 * *exec* loop that should process all external and internal events of
 * the application.
 */

#include <stdio.h>
#include "App.h"

/**
 * @brief Driver function for program
 * @return success state of the program
 */
int main(void)
{
/* Initialize App */
    App_T app = App_init();
/* Execute App */
    App_exec(app);
    return 0;
}
