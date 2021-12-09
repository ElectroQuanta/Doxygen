/**
 * @file Database.c
 * @author Jose Pires
 * @date 13 Jan 2019
 *
 * @brief Database's module implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Database.h"


/**
 * @brief Database's struct: contains the relevant data members
 */
struct Database_T
{
    char *name; /**< name of the database */
    FILE *fp; /**< File pointer to the database */
    size_t size; /**< size of the database */
//    fifo_T fifo; /**< FIFO buffer for packing/unpacking data*/
};

/**
 * @brief Allocates memory for an Database's instance
 * @return initialized memory for Database
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static Database_T Database_new()
{
    Database_T db = malloc(sizeof(*db));
    assert(db);
    return db;
}

Database_T Database_ctor(const char *name)
{
/* Construct instance */
    Database_T db = Database_new();
/* Construct attributes */
    db->fp = NULL;
    db->name = malloc(strlen(name) + 1);
    strcpy(db->name, name);
    db->size = 0;
    return db;
}

void Database_dtor(Database_T db)
{
    if(db->name)
        free(db->name);
    free(db);
}

bool Database_open(const Database_T db, const char *fmt)
{
    if(db->fp)
        return true; // already opened

    // a+b append in binary mode
    // w+b write&read in binary mode
/* Try to open the file to read and write in binary mode */
    db->fp = fopen(db->name, fmt);

    return (db->fp ? true : false);
}

bool Database_reopen(const Database_T db, const char *fmt)
{
    if(db->fp)
        if( !Database_close(db))
            return false; // already opened

    /* Reset database's size */
    db->size = 0;

    // a+b append in binary mode
    // w+b write&read in binary mode
/* Try to open the file to read and write in binary mode */
    db->fp = fopen(db->name, fmt);

    return (db->fp ? true : false);
}

//bool Database_init(const Database_T db)
//{
//    size_t sz = 0;
//    if( ! Database_read(db, &sz, sizeof(sz) ) )
//        return false; // could not read size of object
///* Check if fifo already exists */
//    if(db->fifo)
//        fifo_dtor(db->fifo);
//    db->fifo = fifo_ctor(sz);
//}

bool Database_close(const Database_T db)
{
    db->size = 0;
    // fclose should only be called if the fp returned by fopen is != NULL
    // using fclose on a NULL ptr will cause undefined behaviour
    if(db->fp)
        return !fclose(db->fp);
    return false; // cannot close an unopened file
}

void Database_rewind(const Database_T db)
{
    if(db->fp)
    {
        rewind(db->fp);
        /* Reset database's size */
        db->size = 0;
    }
}

bool Database_read(const Database_T db, void *elem, size_t sz, int origin)
{
    if(! db->fp)
        return false; // file needs to be open first

    fseek(db->fp, 0, origin);

/* If reading from the beginning of file, set its size */
    if(origin == SEEK_SET)
        db->size = sz;

    static size_t nr_elems = 1;
/* size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) */
    return (fread(elem, sz, nr_elems, db->fp) == nr_elems);
}

bool Database_write(const Database_T db, const void *elem, 
                    size_t sz, int origin)
{    
    if(! db->fp)
        return false; // file needs to be open first

    fseek(db->fp, 0, origin);

/* Writing to end of the file, increases its size */
    if(origin == SEEK_END)
        db->size += sz;

    static size_t nr_elems = 1;
/* size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) */
    return ( (fwrite(elem, sz, nr_elems, db->fp) ) == nr_elems );
}

size_t Database_get_size(const Database_T db)
{
    return db->size;
}
