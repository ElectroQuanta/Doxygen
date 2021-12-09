/**
 * @file Pack.h
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Interface for the module Pack
 *
 * Contains the public services that clients can request from Pack
 */
#ifndef PACK_H
#define PACK_H

#include "m-utils.h"
#include <stdio.h>
#include <stdbool.h>
#include "fifo.h"

/**
 * @brief opaque pointer to struct Act_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct Pack_T *Pack_T;

/**
 * @brief Constructs an Pack
 * @return a constructed Pack with default values
 */
Pack_T pack_ctor();

/**
 * @brief Destructs an Pack
 * @param pack: a valid Pack
 */
void pack_dtor(Pack_T pack);

/**
 * @brief Clones an Pack 
 * @param pack: source Pack to perform the copying
 * @param clone: destination Pack to perform the copying
 * @return operation success: false, if either of the input parameters is
 * invalid; true otherwise
 *
 * The cloning operation is used for editing operation, while the end user has
 * the opportunity to abort; without cloning, data corruption could occur
 */
bool pack_clone(const Pack_T pack, Pack_T clone);

/**
 * @brief Invokes an indexed set function from Pack
 * @param pack: object to perform the setting
 * @param idx: index of the function to invoke
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Used for the editing of the Pack's attributes, based on the 
 * menu option processed.
 */
int pack_call_set_fcn(Pack_T pack, int idx);

/**
 * @brief Creates an Pack
 * @param pack: a constructed Pack
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Used for the initial population of the Pack, setting all its
 * attributes
 */
int pack_create(Pack_T pack);

/*---------------------------- Setters ------------------------------- */
/**
 * @brief Sets the Pack's name
 * @param pack: a constructed Pack
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid Pack's name; no regular expression 
 * parsing is performed, thus, name can contain any symbol
 */
int pack_set_name(Pack_T pack);

/**
 * @brief Sets the Pack's duration
 * @param pack: a constructed Pack
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid duration [mins].
 */
int pack_set_duration(Pack_T pack);

/**
 * @brief Sets the Pack's cost
 * @param pack: a constructed Pack
 * @return B_TRUE on success; B_FALSE on failure
 *
 * Prompts the end user for a valid cost [€].
 */
int pack_set_cost(Pack_T pack);
/* ------------------------------------------------------------------- */

/*---------------------------- Getters ------------------------------- */
/**
 * @brief Gets the Pack's name
 * @param pack: a constructed Pack
 * @return Pack's name
 */
const char* pack_get_name(const Pack_T pack);

/**
 * @brief Gets the Pack's duration
 * @param pack: a constructed Pack
 * @return Pack's duration [mins]
 */
int pack_get_duration(const Pack_T pack);

/**
 * @brief Gets the Pack's cost
 * @param pack: a constructed Pack
 * @return Pack's cost [€]
 */
float pack_get_cost(const Pack_T pack);
/* ------------------------------------------------------------------- */

/*---------------------------- Printers ------------------------------ */
/**
 * @brief Print the Pack info in a block
 * @param pack: a constructed Pack
 * 
 * Used to print info from a single Pack in a block styled way.
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void pack_print(const Pack_T pack);

/**
 * @brief Print the Pack info in a line
 * @param pack: a constructed Pack
 * 
 * Used to print info from a single Pack in a line styled way, ideal
 * to build tables. 
 * @see m-utils.h for table headers
 * @see list.h: *Printer* functions are required by *List* to 
 * print its contents
 */
void pack_print_line(const Pack_T pack);
/* ------------------------------------------------------------------- */

/*------------------------- Comparators ------------------------------ */
/**
 * @brief Compares activities by name
 * @param pack1: a constructed Pack
 * @param pack2: a constructed Pack
 * @return value of comparison:
 * - 0: activities are equal 
 * - < 0: Pack1 is smaller than Pack2
 * - > 0: Pack1 is greater than Pack2
 *
 * The comparison is made from *name* attribute.
 * @see list.h: *Comparators* functions are required by *List* to 
 * perform sorted insertion and sorting.
 */
int pack_cmp_name(const Pack_T pack1,const Pack_T pack2);
/* ------------------------------------------------------------------- */

/*-------------------- Serializers/Deserializers --------------------- */
/**
 * @brief Serializes Pack attributes into a FIFO
 * @param pack: a constructed Pack
 * @return fifo: FIFO buffer containing the serialized Pack; NULL in error
 *
 * The serialization is useful for writing to binary files with unknown 
 * size at compilation time, i.e., for objects whose memory was
 * dynamically allocated.
 * @see fifo.h
 */
Fifo_T pack_serialize(Pack_T pack);

/**
 * @brief Deserializes Pack attributes into a FIFO
 * @param fifo: FIFO buffer containing the serialized Pack
 * @return pack: a constructed Pack
 *
 * The deserialization is useful for reading from binary files
 * with unknown size at compilation time, i.e., for objects whose memory was
 * dynamically allocated.
 * @see fifo.h
 */
Pack_T pack_deserialize(Fifo_T fifo);

#endif // PACK_H
