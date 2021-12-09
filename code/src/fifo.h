/**
 * @file fifo.h
 * @author Jose Pires
 * @date 14 Jan 2019
 *
 * @brief First-in, First-out (FIFO) module
 *
 * Module for FIFO management, used in serialization/deserialization of objects
 */

#include <stdlib.h>

/**
 * @brief opaque pointer to struct Fifo_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct Fifo_T *Fifo_T;

/**
 * @brief Constructs a FIFO
 * @param sz: size of the FIFO
 * @return a constructed Fifo with default values
 */
Fifo_T Fifo_ctor(size_t sz);

/**
 * @brief Destructs a FIFO
 * @param fifo: a valid FIFO
 */
void Fifo_dtor(Fifo_T fifo);

/**
 * @brief Prints the desired length of FIFO 
 * @param fifo: a valid FIFO
 * @param len: length of FIFO to print
 */
void Fifo_print(Fifo_T fifo, size_t len);

/**
 * @brief Pushes an element to FIFO 
 * @param fifo: a valid FIFO
 * @param data: elem to push
 * @param len: length of elem to push
 * @return the actual size pushed to FIFO; can be compared with the 
 * element's size to check for errors
 */
size_t Fifo_push(Fifo_T fifo, const void *data, size_t len);

/**
 * @brief Pop an element from FIFO 
 * @param fifo: a valid FIFO
 * @param data: elem to pop to
 * @param len: length of elem to push
 * @return the actual size pushed to FIFO; can be compared with the 
 * element's size to check for errors
 */
size_t Fifo_pop(const Fifo_T fifo, void *data, size_t len);

/**
 * @brief Gets FIFO's size 
 * @param fifo: a valid FIFO
 * @return size of FIFO
 */
size_t Fifo_get_size(const Fifo_T fifo);

/**
 * @brief Gets FIFO's data
 * @param fifo: a valid FIFO
 * @return data; is used for serialize/deserialize functions
 */
void * Fifo_get_data(const Fifo_T fifo);

/**
 * @brief Gets FIFO's write index
 * @param fifo: a valid FIFO
 * @return write index of FIFO
 */
size_t Fifo_get_write_idx(const Fifo_T fifo);

/**
 * @brief Sets FIFO's write index
 * @param fifo: a valid FIFO
 * @param idx: new write index value
 */
void Fifo_set_write_idx(Fifo_T fifo, const size_t idx);

/**
 * @brief Sets FIFO's size
 * @param fifo: a valid FIFO
 * @param idx: new size value
 */
void Fifo_set_size(Fifo_T fifo, const size_t idx);
