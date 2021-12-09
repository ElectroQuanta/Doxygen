/**
 * @file list.h
 * @author Jose Pires
 * @date 14 Jan 2019
 *
 * @brief Interface to list module
 *
 * *list* is a generic container used throughout the application
 * It does not own the data; but contains a pointer to it.
 * As a container the basic operations over the list are: *insertion*,
 * *deletion*, *search* and *sort*.
 * Thus, it requires client modules to implement the desired functions
 * like the comparators and printers that can be applied over the container.
 */

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/**
 * @brief opaque pointer to struct List_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct List_T *List_T;

/**
 * @brief Constructs an list
 * @param Data_ctor: a function pointer for a Data constructor
 * @param Data_cmp: a function pointer for a Data comparator
 * @param Data_dtor: a function pointer for a Data destructor
 * @param Data_print: a function pointer for a Data printer
 * @return a constructed List with default values
 */
List_T List_ctor( void *(*Data_ctor)(void),
                  int (*Data_cmp)(const void *data1, const void *data2),
                  void (*Data_dtor)(void *data),
                  void (*Data_print)(const void *data));

/**
 * @brief Destructs a list
 * @param self: a valid list
 */
void List_dtor(List_T self);

/**
 * @brief Inserts node in the list by the desired order
 * @param self: a pointer to valid list
 * @param elem: element to insert
 * @param order: true - ascending; false - descending
 * @param allowDups: true - duplicates allowed; false - duplicates disallowed
 * @param cmp: compare function to be used in the insertion; if NULL it uses the default compare function
 * @return data inserted or matching elements if allowDups is false
 *
 * The compare functions must be implemented by the client module
 */
void * List_insert_ascend(List_T *self, const void *elem, 
                          const bool order, const bool allowDups,
                          int(*cmp)(const void *data1, const void *data2));

/**
 * @brief Search a similar element in the list
 * @param self: a valid list
 * @param elem: comparable element to perform the search
 * @param cmp: compare function to be used in the insertion; if NULL it uses the default compare function
 * @return data found; NULL otherwise
 *
 * The compare functions must be implemented by the client module
 */
void * List_search(const List_T self, const void *elem, 
                   int(*cmp)(const void *data1, const void *data2));

/**
 * @brief Search all elements in the list matching /elem/
 * @param self: a valid list
 * @param elem: comparable element to perform the search
 * @param cmp: compare function to be used in the insertion; if NULL it uses the default compare function
 * @return list of elements; NULL otherwise
 *
 * The compare functions must be implemented by the client module
 */
List_T List_search_all(const List_T self, const void *elem,
                       int(*cmp)(const void *data1, const void *data2));

/**
 * @brief Removes element in the list returned by search
 * @param self: a pointer to valid list
 * @param elem: element returned by search
 * @return true, if deleted; false, otherwise
 */
bool List_remove(List_T *self, const void *elem);

/* Replaces old_elem by new_elem in the list (by data)
 * The old_elem can be retrieve using =search= or passing the elem directly
 * Returns: true if successful; false, otherwise
 */
/**
 * @brief Removes *old_elem* by *new_elem* in the list (by data) 
 * @param self: a pointer to valid list
 * @param old_elem: element to be replaced
 * @param new_elem: replacing object
 * @return true, if deleted; false, otherwise
 */
bool List_replace(List_T *self, const void *old_elem, const void *new_elem);

/**
 * @brief Sorts the list using a comparator function
 * @param self: a pointer to valid list that is updated after sorting
 * @param cmp: compare function to be used in the insertion; if NULL it uses the default compare function
 *
 * The compare functions must be implemented by the client module;
 * It is required when sorting key is updated in the list
 */
void List_sort(List_T *self, 
               int(*cmp)(const void *data1, const void *data2));

/**
 * @brief Gets the nr. of elements in the list
 * @param self: a valid list
 * @return nr. of elements in the list
 */
unsigned List_count(const List_T self);

/* Print all node's info with passed print function
 * - If print == NULL, it uses the default print function
 * - if numbered == true, it prints the elements with numbering
 * - If header != NULL, it prints the header
 */
/**
 * @brief Prints all elements in the list
 * @param self: a valid list
 * @param print: pointer to function print
 * @param numbered: true - prints the elements with numbering
 * @param header: header; used for tabled
 *
 * The print functions must be implemented by the client module
 */
void List_print_all(const List_T self,
                    void(*print)(const void *data), bool numbered,
                    const char *header);

/* Debug list: prints data and links addresses for all nodes in the list */
/**
 * @brief Debug list: prints data and links addresses for all nodes in the list
 * @param self: a valid list
 *
 * The print functions must be implemented by the client module
 */
void List_debug(const List_T self);

/**
 * @brief Prints elements in the list
 * @param self: a valid list
 * @param elem: element to print
 * @param print: pointer to function print
 *
 * The print functions must be implemented by the client module
 */
void List_print_elem(const List_T self, const void *elem,
                     void(*print)(const void *data) );

/**
 * @brief Checks if list is *dirty*, i.e., if it was updated or requires an update
 * @param self: a valid list
 * @return true, if dirty; false, otherwise
 */
bool List_isDirty(const List_T self);

/**
 * @brief Sets the *dirty* flag of the list, signaling 
 * it was updated or requires an update
 * @param self: a valid list
 * @param dirty: dirty flag
 */
void List_set_dirty(List_T self, const bool dirty);

/**
 * @brief Pops an element from the head of the list
 * @param self: a valid list
 * @return element of the list; NULL if empty
 */
void * List_pop(List_T self);

/**
 * @brief Rewinds the list internal iterator to the head of list
 * @param self: a valid list
 */
void List_rewind(List_T self);

/*---------------  Aux and Util functions (for debug) -------------- */

/* Replaces the elem in the list (by data) 
 * The elem can be retrieved using =search= or by passing the elem directly
 * Returns: true if successful; false, otherwise
 */
//bool List_replace(List_T *self, const void *elem);

/* Search by a node by name */
//Node_T List_searchByName(const char *name);
//
///* Search by a node by Val (used for time)*/
//Node_T *List_searchByVal(int val);
//
///* Retrieve data from Node */
//void *List_get_data(Node_T node);
//
//
///* Remove node by name */

/**
 * @brief Removes element from the list
 * @param self: a valid list
 * @param elem: element to be removed
 * @return true, if successful; false, otherwise
 */
bool List_remove_elem(List_T self, const void *elem);
//
///* Check if elem match in list */
//int List_match_elem(List_T self, const void *elem);
//
///* Inserts node at the beginning */
////bool List_push_front(List_T self, const void *data);
//void* List_push_front(List_T self, const void *elem);
//
///* Inserts node at the end */
//bool List_append(List_T self, void *data);

#endif // LIST_H
