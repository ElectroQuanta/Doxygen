/**
 * @file Menu.c
 * @author Jose Pires
 * @date 13 Jan 2019
 *
 * @brief Menu's module implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "Menu.h"
#include "m-utils.h"

//#define DEBUG

/**
 * @brief Menu's structure: contains the relevant data attributes
 */
struct Menu_T
{
    char *title; /**< Title */ 
    char **opts; /**< Options array */ 
    char bord; /**< Border */
    unsigned count; /**< Options count */
};

/**
 * @brief Prints the menu borders
 * @param c: character to print the border
 */
static void menu_print_borders(char c)
{
  int i;
  for(i = 0; i < BORDER_SIZE; i++)
    printf("%c", c);
  printf("\n");

}

/**
 * @brief Prints the menu
 * @param menu: a valid menu
 */
static void Menu_print(const Menu_T menu)
{
  unsigned i = 0;
  menu_print_borders(menu->bord);
  printf("\t\t%s\n\n", menu->title);

  while( i < menu->count)
    printf("\t%s\n", menu->opts[i++]);

  menu_print_borders(menu->bord);
  printf("\n\t\tOpcao: ");
}

/**
 * @brief Allocates memory for an Menu's instance
 * @return initialized memory for Menu
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static Menu_T Menu_new()
{
// allocate memory for struct
    Menu_T menu = malloc(sizeof( *menu ));
    assert(menu) ;
    return menu;
}

Menu_T Menu_ctor(const char *title, ...)
{ 
    char border = '#';
    int i = 0; // nr of elems in menu
    Menu_T menu =  NULL;
	
    va_list app, copy;
    const char *arg;

/* Initialize variable list */
    va_start(app, title); // Initialize variable arguments
    va_copy(copy, app);   // Copy list for the 2nd pass

// allocate memory for struct
    menu = Menu_new();

  // initialize
    menu->bord = border;
    // allocate memory for title and copy it
    menu->title = malloc( strlen(title) + 1);
    assert(menu->title);
    strcpy(menu->title, title);

    // count the nr of items
    menu->count = count_vararg(app);
#ifdef DEBUG
    printf("\nBorder: %c\t", menu->bord);
    printf("Title: %s\t", menu->title);
    printf("Count: %d\n", menu->count);
#endif
    va_end(app); // Reset variable argument list
    
  // allocate memory for strings
  menu->opts = malloc( sizeof(char *) * menu->count);
  assert(menu->opts);

/* Copy options passed to menu */
  while( (arg = va_arg(copy, const char *)))
  {
#ifdef DEBUG
      printf("arg: %s\n", arg);
#endif
     menu->opts[i] = malloc(strlen(arg)+ OPT_STR_SIZE +1);
     assert(menu->opts[i]);
     sprintf(menu->opts[i], "%d. %s", i, arg);
     i++;
  }
  va_end(app); // Reset copy of argument list


  return menu;
}

// Releases all allocated memory
void Menu_dtor(Menu_T menu)
{
  unsigned i;
  if(!menu)
      return;

  for(i = 0; i < menu->count; i++)
      free(menu->opts[i]);

  free(menu->opts);
  free(menu->title);
  free(menu);
}

// Process the menu
//  - returns the options selected
char Menu_process(const Menu_T menu)
{
  unsigned i;
  char *input = NULL;
  while(1)
  {
/* Clear screen and print menu */      
    CLEARSCR;
    Menu_print(menu);
/* Get input */
    input = get_input(NULL);

/* Check for abnormal input */
    if(strlen(input) > 1) // more than one char was passed
        continue; // iterate

/* Print menu options */
    for(i = 0; i < menu->count; i++)
        if(menu->opts[i][0] == input[0])
            return input[0];
  }

}

/* Compare two users */
int Menu_compare(const Menu_T menu1, const Menu_T menu2)
{
    if(!menu1->title || !menu2->title)
        return -1;
#ifdef POSIX
    return strcasecmp(menu1->title, menu2->title);
#else
    return strcmpi(menu1->title, menu2->title);
#endif
}
