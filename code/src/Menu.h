/**
 * @file Menu.h
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Interface for menu module. Used as main user interface.
 */

#ifndef MENU_H
#define MENU_H

#include "m-utils.h"

/**
 * @brief opaque pointer to struct Menu_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct Menu_T *Menu_T;

/* ========== External accessable functions =============== */

/**
 * @brief Constructs a Menu
 * @param title: name of the database (file)
 * @param ...: variable argument list of options and NULL terminated
 * @return a constructed Menu with default values
 */
extern Menu_T Menu_ctor(const char *title, ...);

/**
 * @brief Destructs a Menu
 * @param menu: a valid Menu
 */
extern void Menu_dtor(Menu_T menu);

/**
 * @brief Processes a Menu
 * @param menu: a valid Menu
 * @return the first char of the option selected
 */
extern char Menu_process(const Menu_T menu); 

/**
 * @brief Compares Menu by title
 * @param menu1: a constructed menu
 * @param menu2: a constructed menu
 * @return value of comparison:
 * - 0: activities are equal 
 * - < 0: menu1 is smaller than menu2
 * - > 0: menu1 is greater than menu2
 *
 * The comparison is made from *title* attribute.
 * @see list.h: *Comparators* functions are required by *List* to 
 * perform sorted insertion and sorting.
 */
int Menu_compare(const Menu_T menu1, const Menu_T menu2);
/* ======================================================== */


#endif // Menu_H
