/**
 * @file fifo.c
 * @author Jose Pires
 * @date 13 Jan 2019
 *
 * @brief fifo's module implementation
 */

#ifndef FIFO_H
#define FIFO_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "fifo.h"

/**
 * @brief Define atomic unit for FIFO
 *
 * Why use uchar for byte representation?
 * https://stackoverflow.com/a/653621
 */
typedef unsigned char byte;

/**
 * @brief Fifo's struct: contains the relevant data members
 */
struct Fifo_T
{
    size_t rd; /**< FIFO's read index */
    size_t wr; /**< FIFO's write index */
    size_t size; /**< Size of the fifo */
    byte *data; /**< Fifo's data */
};

/**
 * @brief Allocates memory for an FIFO's instance
 * @return initialized memory for FIFO
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static Fifo_T Fifo_new()
{
    Fifo_T fifo = malloc(sizeof(*fifo));
    assert(fifo);
    return fifo;
}

/**
 * @brief Checks if FIFO is full
 * @param fifo: a valid FIFO
 * @return true if full; false otherwise
 */
static bool Fifo_isFull(const Fifo_T fifo)
{
    return (fifo->wr == fifo->size);
}

/**
 * @brief Checks if FIFO is empty
 * @param fifo: a valid FIFO
 * @return true if empty; false otherwise
 */
static bool Fifo_isEmpty(const Fifo_T fifo)
{
    return (fifo->wr == fifo->rd);
}

Fifo_T Fifo_ctor(size_t sz)
{
    Fifo_T fifo = Fifo_new();
    fifo->size = sz;
    fifo->rd = fifo->wr = 0;
    fifo->data = malloc(sz);
    assert(fifo->data);
    /* Clearing out trash from fifo */
    memset(fifo->data,'\0', sz);
    return fifo;
}

void Fifo_dtor(Fifo_T fifo) 
{
    if(!fifo)
        return;
    if(fifo->data)
        free(fifo->data);
       
    free(fifo);
}

void Fifo_print(Fifo_T fifo, size_t len)
{
    if(!fifo->data)
        return;
    
    byte *it = fifo->data;
    while(len--)
    {
       printf("%x ", *it);
       it++;
    }
}

size_t Fifo_push(Fifo_T fifo, const void *data, size_t len)
{
    if(!data || len < 1)
        return 0;
    
    if( Fifo_isFull(fifo) )
        return 0;

    memcpy( &(*(fifo->data + fifo->wr)), data, len);
//    printf("\nData: %s\nData: ", fifo->data + fifo->wr);
//    fifo_print(fifo, fifo->wr);
//    putchar('\n');
    return (fifo->wr += len);
}

size_t Fifo_pop(const Fifo_T fifo, void *data, size_t len)
{
    if(!data || len < 1)
        return 0;

    if( Fifo_isEmpty(fifo) )
        return 0;

    memcpy( data,  &(*(fifo->data + fifo->rd)), len);
//    printf("\nData: %s\nData: ", data);
//    fifo_print(fifo, fifo->rd);
//    putchar('\n');
    return (fifo->rd += len);
}

size_t Fifo_get_size(const Fifo_T fifo)
{
    return fifo->size;
}

void * Fifo_get_data(const Fifo_T fifo)
{
    return fifo->data;
}

size_t Fifo_get_write_idx(const Fifo_T fifo)
{
    return fifo->wr;
}

void Fifo_set_write_idx(Fifo_T fifo, const size_t idx)
{
   fifo->wr = idx; 
}

void Fifo_set_size(Fifo_T fifo, const size_t idx)
{
    fifo->size = idx;
}

#endif // FIFO_H
