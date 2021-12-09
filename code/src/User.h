/**
 * @file User.h
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Interface for the module User
 *
 * Contains the public services that clients can request from User
 */

#ifndef USER_H
#define USER_H

#include "m-utils.h"
#include "list.h"
#include <stdio.h>
#include <stdbool.h>
#include "fifo.h"


/**
 * @brief User types
 */
enum User_type { Gerente, /**< Manager */
                 Func, /**< Employee */
                 Cliente /**< Client */
}; // User types

/**
 * @brief opaque pointer to struct User_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct User_T *User_T;
/**
 * @brief Repeat the forward declaration of Act_T 
 * to avoid circular dependency
 */
typedef struct Act_T *Act_T ;

/*----------  Functions --------------- */
/**
 * @brief Constructs an User
 * @param tipo: a valid User_type 
 * @return a constructed User with default values
 */
User_T user_ctor(enum User_type tipo);

/**
 * @brief Destructs an User
 * @param user: a valid User
 */
void user_dtor(User_T user);

/**
 * @brief Clones an User 
 * @param user: source User to perform the copying
 * @param clone: destination User to perform the copying
 * @return operation success: false, if either of the input parameters is
 * invalid; true otherwise
 *
 * The cloning operation is used for editing operation, while the end user has
 * the opportunity to abort; without cloning, data corruption could occur
 */
bool user_clone(const User_T user, User_T clone);

/**
 * @brief Invokes an indexed set function from User
 * @param user: object to perform the setting
 * @param idx: index of the function to invoke
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Used for the editing of the User's attributes, based on the 
 * menu option processed.
 */
int user_call_set_fcn(User_T user, int idx);

/**
 * @brief Creates an User
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Used for the initial population of the User, setting all its
 * attributes
 */
int user_create(User_T user);

/*---------------------------- Setters ------------------------------- */

/**
 * @brief Sets the User's username
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid User's username; no regular expression 
 * parsing is performed, thus, name can contain any symbol
 */
int user_set_username(User_T user);

/**
 * @brief Sets the User's password
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid User's password; no regular expression 
 * parsing is performed, thus, name can contain any symbol
 */
int user_set_pass(User_T user);

/**
 * @brief Sets the User's name
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid User's name; no regular expression 
 * parsing is performed, thus, name can contain any symbol
 */
int user_set_name(User_T user);

/**
 * @brief Sets the User's age
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid age.
 */
int user_set_idade(User_T user);

/**
 * @brief Sets the User's sex
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid sex.
 */
int user_set_sexo(User_T user);

/**
 * @brief Sets the User's height
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid height.
 */
int user_set_altura(User_T user);

/**
 * @brief Sets the User's weight
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid weight.
 */
int user_set_peso(User_T user);

/**
 * @brief Sets the User's balance
 * @param user: a constructed User
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid balance.
 */
int user_set_saldo(User_T user);

/**
 * @brief Updates the User's balance
 * @param user: a constructed User
 * @param amount: value to pay
 * @return B_TRUE on success; B_FALSE on failure
 */
int user_pay(User_T user, float amount);
/* ------------------------------------------------------------------- */

/*---------------------------- Getters ------------------------------- */
/**
 * @brief Gets the User's type
 * @param user: a constructed User
 * @return User's type
 */
enum User_type user_get_type(const User_T user);

/**
 * @brief Gets the User's password
 * @param user: a constructed User
 * @return User's password
 */
const char * user_get_pass(const User_T user);

/**
 * @brief Gets the User's balance
 * @param user: a constructed User
 * @return User's balance
 */
float user_get_saldo(const User_T user);

/**
 * @brief Gets a list of User's activities
 * @param user: a constructed User
 * @return User's list of activities
 */
List_T user_get_activities(const User_T user);
/* ------------------------------------------------------------------- */

/*---------------------------- Printers ------------------------------ */
/**
 * @brief Print the User info in a block
 * @param user: a constructed User
 * 
 * Used to print info from a single User in a block styled way.
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void user_print_info(const User_T user);

/**
 * @brief Print extra User info in a block
 * @param user: a constructed User
 * 
 * Used to print Client's info from a single User in a block styled way.
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void user_print_info_extra(const User_T user);

/**
 * @brief Print User's name
 * @param user: a constructed User
 * 
 * Used for debugging
 */
void user_print_username(const User_T user);

/**
 * @brief Print the User info in a line
 * @param user: a constructed User
 * 
 * Used to print info from a single User in a line styled way, ideal
 * to build tables. 
 * @see m-utils.h for table headers
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void user_print_line(const User_T user);

/* ------------------------------------------------------------------- */

/*------------------------- Comparators ------------------------------ */
/**
 * @brief Compares activities by username
 * @param user1: a constructed User
 * @param user2: a constructed User
 * @return value of comparison:
 * - 0: activities are equal 
 * - < 0: User1 is smaller than User2
 * - > 0: User1 is greater than User2
 *
 * The comparison is made from *username* attribute.
 * @see list.h: *Comparators* functions are required by *List* to 
 * perform sorted insertion and sorting.
 */
int user_cmp_username(const User_T user1, const User_T user2);

/**
 * @brief Compares activities by type
 * @param user1: a constructed User
 * @param user2: a constructed User
 * @return value of comparison:
 * - 0: activities are equal 
 * - < 0: User1 is smaller than User2
 * - > 0: User1 is greater than User2
 *
 * The comparison is made from *type* attribute. This is useful to obtain
 * lists of Managers, Employees and Clients, from the main list
 * @see list.h: *Comparators* functions are required by *List* to 
 * perform sorted insertion and sorting.
 */
int user_cmp_type(const User_T user1, const User_T user2);
/* ------------------------------------------------------------------- */

/*-------------------------- List related ---------------------------- */
/**
 * @brief Adds Activity to the User activities list
 * @param user: a constructed User
 * @param activity: a constructed activity
 * @return success value:
 * - false: fail
 * - true: success
 *
 * Used when reserving a vacancy in the Activity.
 */
bool user_add_activity(User_T user, const Act_T activity);

/**
 * @brief Removes activity from the User activities list
 * @param user: a constructed User
 * @param activity: a constructed activity
 * @return success value:
 * - false: fail
 * - true: success
 *
 * Used when canceling a vacancy in the Activity.
 */
bool user_remove_activity(User_T user, const Act_T activity);

/**
 * @brief Prints the User activities list
 * @param user: a constructed User
 */
void user_list_activities(const User_T user);
/* ------------------------------------------------------------------- */

/*-------------------- Serializers/Deserializers --------------------- */
/**
 * @brief Serializes User attributes into a FIFO
 * @param user: a constructed User
 * @return fifo: FIFO buffer containing the serialized User; NULL in error
 *
 * The serialization is useful for writing to binary files with unknown 
 * size at compilation time, i.e., for objects whose memory was
 * dynamically allocated.
 * @see fifo.h
 */
Fifo_T user_serialize(User_T user);

/**
 * @brief Deserializes User attributes into a FIFO
 * @param fifo: FIFO buffer containing the serialized User
 * @return User: a constructed User
 *
 * The deserialization is useful for reading from binary files
 * with unknown size at compilation time, i.e., for objects whose memory was
 * dynamically allocated.
 * @see fifo.h
 */
User_T user_deserialize(Fifo_T fifo);

#endif // USER_H
