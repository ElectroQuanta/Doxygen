/**
 * @file Database.h
 * @author Jose Pires
 * @date 13 Jan 2019
 *
 * @brief Module that handles requests and writes to the the database
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief opaque pointer to struct Database_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct Database_T *Database_T;

/**
 * @brief Constructs a Database
 * @param name: name of the database (file)
 * @return a constructed Database with default values
 */
Database_T Database_ctor(const char *name);

/**
 * @brief Destructs a Database
 * @param db: a valid Database
 */
void Database_dtor(Database_T db);

/**
 * @brief Open a Database
 * @param db: a valid Database
 * @param fmt: format to open the database
 * @return true, if successfull; false, otherwise;
 */
bool Database_open(const Database_T db, const char *fmt);

/**
 * @brief Reopen a Database
 * @param db: a valid Database
 * @param fmt: format to reopen the database
 * @return true, if successfull; false, otherwise;
 */
bool Database_reopen(const Database_T db, const char *fmt);

/**
 * @brief Close a Database
 * @param db: a valid Database
 * @return true, if successfull; false, otherwise;
 */
bool Database_close(const Database_T db);

/**
 * @brief Rewind the Database
 * @param db: a valid Database
 */
void Database_rewind(const Database_T db);

/**
 * @brief Read data from the Database
 * @param db: a valid Database
 * @param elem: data to be filled in
 * @param sz: size of the data to read
 * @param origin: origin of the database to read from
 * @return true, if successfull; false, otherwise;
 */
bool Database_read(const Database_T db, void *elem, size_t sz, int origin);

/**
 * @brief Write data to the Database
 * @param db: a valid Database
 * @param elem: data to write to the database
 * @param sz: size of the data to write
 * @param origin: origin of the database to write
 * @return true, if successfull; false, otherwise;
 */
bool Database_write(const Database_T db, const void *elem, 
                    size_t sz, int origin);

/**
 * @brief Get size of the database
 * @return size of the database
 */
size_t Database_get_size(const Database_T db);

#endif // DATABASE_H
