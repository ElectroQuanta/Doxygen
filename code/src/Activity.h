/**
 * @file Activity.h
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Interface for the module Activity
 *
 * Contains the public services that clients can request from Activity
 */

#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "m-utils.h"
#include <stdio.h>
#include <stdbool.h>
#include "User.h"
#include "fifo.h"

/**
 * @brief opaque pointer to struct Act_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct Act_T *Act_T ;

/*----------  Functions --------------- */

/**
 * @brief Constructs an activity
 * @return a constructed Activity with default values
 */
Act_T activity_ctor();

/**
 * @brief Destructs an activity
 * @param activity: a valid Activity
 */
void activity_dtor(Act_T activity);

/**
 * @brief Clones an activity 
 * @param activity: source activity to perform the copying
 * @param clone: destination activity to perform the copying
 * @return operation success: false, if either of the input parameters is
 * invalid; true otherwise
 *
 * The cloning operation is used for editing operation, while the end user has
 * the opportunity to abort; without cloning, data corruption could occur
 */
bool activity_clone(const Act_T activity, Act_T  clone);


/**
 * @brief Invokes an indexed set function from activity
 * @param activity: object to perform the setting
 * @param idx: index of the function to invoke
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Used for the editing of the Activity's attributes, based on the 
 * menu option processed.
 */
int activity_call_set_fcn(Act_T activity, int idx);

/**
 * @brief Creates an activity
 * @param activity: a constructed activity
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Used for the initial population of the activity, setting all its
 * attributes
 */
int activity_create(Act_T activity);

/*---------------------------- Setters ------------------------------- */
/**
 * @brief Sets the activity's name
 * @param activity: a constructed activity
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid activity's name; no regular expression 
 * parsing is performed, thus, name can contain any symbol
 */
int activity_set_nome(Act_T activity);

/**
 * @brief Sets the activity's time
 * @param activity: a constructed activity
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid day, hour and minutes.
 */
int activity_set_time(Act_T activity);

/**
 * @brief Sets the activity's duration
 * @param activity: a constructed activity
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid duration [mins].
 */
int activity_set_duracao(Act_T activity);

/**
 * @brief Sets the activity's cost
 * @param activity: a constructed activity
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid cost [€].
 */
int activity_set_custo(Act_T activity);

/**
 * @brief Sets the activity's maximum vacancies
 * @param activity: a constructed activity
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid maximum vacancies.
 */
int activity_set_max_vagas(Act_T activity);
/* ------------------------------------------------------------------- */

/*---------------------------- Getters ------------------------------- */
/**
 * @brief Gets the activity's name
 * @param activity: a constructed activity
 * @return activity's name
 */
const char* activity_get_nome(const Act_T activity);

/**
 * @brief Gets the activity's mins from start
 * @param activity: a constructed activity
 * @return activity's mins from start
 *
 * This was used purely for debugging. *It should be a private function*
 */
int activity_get_mins_from_start(const Act_T activity);

/**
 * @brief Gets the activity's duration
 * @param activity: a constructed activity
 * @return activity's duration [mins]
 */
int activity_get_duracao(const Act_T activity);

/**
 * @brief Gets the activity's cost
 * @param activity: a constructed activity
 * @return activity's cost [€]
 */
float activity_get_custo(const Act_T activity);

/**
 * @brief Gets the activity's max vacancies
 * @param activity: a constructed activity
 * @return activity's max vacancies
 */
int activity_get_max_vagas(const Act_T activity);
/* ------------------------------------------------------------------- */

/*---------------------------- Printers ------------------------------ */

/**
 * @brief Print the activity info in a block
 * @param activity: a constructed activity
 * 
 * Used to print info from a single activity in a block styled way.
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void activity_print(const Act_T activity);

/**
 * @brief Print the activity info in a line
 * @param activity: a constructed activity
 * 
 * Used to print info from a single activity in a line styled way, ideal
 * to build tables. 
 * @see m-utils.h for table headers
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void activity_print_line(const Act_T activity);
/* ------------------------------------------------------------------- */

/*------------------------- Comparators ------------------------------ */
/**
 * @brief Compares activities by time
 * @param activity1: a constructed activity
 * @param activity2: a constructed activity
 * @return value of comparison:
 * - 0: activities are equal 
 * - < 0: activity1 is smaller than activity2
 * - > 0: activity1 is greater than activity2
 *
 * The comparison is made from *mins_to_start* attribute.
 * @see list.h: *Comparators* functions are required by *List* to 
 * perform sorted insertion and sorting.
 */
int activity_cmp_time(const Act_T activity1,const Act_T activity2);

/**
 * @brief Compares activities by name
 * @param activity1: a constructed activity
 * @param activity2: a constructed activity
 * @return value of comparison:
 * - 0: activities are equal 
 * - < 0: activity1 is smaller than activity2
 * - > 0: activity1 is greater than activity2
 *
 * The comparison is made from *name* attribute.
 * @see list.h: *Comparators* functions are required by *List* to 
 * perform sorted insertion and sorting.
 */
int activity_cmp_name(const Act_T activity1,const Act_T activity2);
/* ------------------------------------------------------------------- */

/*-------------------------- List related ---------------------------- */
/**
 * @brief Adds user to the activity users list
 * @param activity: a constructed activity
 * @param user: a constructed user
 * @return success value:
 * - false: fail
 * - true: success
 *
 * Used to reserve a vacancy in the activity.
 */
bool activity_add_user(Act_T activity, const User_T user);

/**
 * @brief Removes user to the activity users list
 * @param activity: a constructed activity
 * @param user: a constructed user
 * @return success value:
 * - false: fail
 * - true: success
 *
 * Used to cancel a vacancy in the activity.
 */
bool activity_remove_user(Act_T activity, const User_T user);
/* ------------------------------------------------------------------- */

/*-------------------- Serializers/Deserializers --------------------- */
/**
 * @brief Serializes activity attributes into a FIFO
 * @param activity: a constructed activity
 * @return fifo: FIFO buffer containing the serialized activity; NULL in error
 *
 * The serialization is useful for writing to binary files with unknown 
 * size at compilation time, i.e., for objects whose memory was
 * dynamically allocated.
 * @see fifo.h
 */
Fifo_T activity_serialize(Act_T activity);

/**
 * @brief Deserializes activity attributes into a FIFO
 * @param fifo: FIFO buffer containing the serialized activity
 * @return activity: a constructed activity
 *
 * The deserialization is useful for reading from binary files
 * with unknown size at compilation time, i.e., for objects whose memory was
 * dynamically allocated.
 * @see fifo.h
 */
Act_T activity_deserialize(Fifo_T fifo);
/* ------------------------------------------------------------------- */

#endif // ACTIVITY_H
