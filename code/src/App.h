/**
 * @file App.h
 * @author Jose Pires
 * @date 12 Jan 2019
 *
 * @brief App module containing the application logic
 *
 * It contains only two public functions:
 * 1. Init - to initialize the app's memory
 * 2. Exec - contains all application logic
 */

#ifndef App_H
#define App_H

/**
 * @brief opaque pointer to struct App_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct App_T *App_T;

/* ========== External accessable functions =============== */

/**
 * allocate dynamic memory and initialze App
 * @return initialized App_T
 */
App_T App_init();

/**
 * @brief App controller: handles all application's logic
 * @param app - an initialized application instance
 * @return an integer signaling the execution state
 *
 * Its the execution loop controlling the FSM machine behind the application's
 * logic. It starts in S_Login state. When end user quits the application,
 * the databases are saved, for future restoration at subsequent program
 * executions.
 */
int App_exec(App_T app);
/* ======================================================== */

#endif // App_H
