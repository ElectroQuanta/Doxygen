/**
 * @file m-utils.h
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Interface to functions and macros used globally throughout the
 * application
 */

#ifndef M_UTILS_H
#define M_UTILS_H
#include <stdlib.h>
#include <stdarg.h>

/* ============== Define platform specifics ================= */
/* - Clear screen defined as macro for platform compatibility */

/**
 * @brief Macro to clear screen; required for platform compatibility
 */
//#define WIN64 // Windows
#define POSIX // Unix
#if defined POSIX
#define CLEARSCR system ("clear") /**< Clear screen macro (for Posix) */
#elif defined MSDOS || defined WIN32 || defined WIN64
#define CLEARSCR system ("cls") /**< Clear screen macro (for Windows) */
#define _CRT_SECURE_NO_WARNINGS /**< Required for Visual Studio */
#endif


/* ======= Define Global constants  ============== */
/* BOOLEAN VALUES */
#define B_FALSE 0; /**< Boolean False */
#define B_TRUE 1; /**< Boolean true */

/* ERROR CODES */
#define ERROR_INVALID -1 /**< Error Invalid */
#define QUIT -2 /**< Quit */

/* ESCAPE CHARACTERS */
#define ABORT_INPUT '#' /**< Code to abort input */
#define EMPTY_STRING "" /**< Empty string */
#define LF '\n' /**< line feed */
#define CR '\r' /**< Carriage Return */
#define SPACE ' ' /**< Space */

#define BUF_SZ 20 /**< Buffer size for input */

/* UI constants */
#define BORDER_SIZE   50 /**< Border size for menu */
#define TAB_SIZE      8 /**< Tab size */
#define OPT_STR_SIZE      8 /**< Option string size */

/* Used for print_line functions */
#define table_header_user "NR, TIPO, USERNAME, NOME, IDADE, SEXO, ALTURA, PESO, BMI, SALDO" /**< Table header for user */
#define table_header_activity "DATA [DD; HH:MM], NOME, DURACAO (MINS), VAGAS" /**< Table header for activity */
#define table_header_pack "NOME, DURACAO (MESES), CUSTO [EURO]" /**< Table header for pack */

/* Used for serialization */
#define PACK_TAG 0x90 /**< Tag for Pack: used in serialization */

/*=================== Auxiliary functions ============================= */

/**
 * @brief Retrieves a fixed input from end user and clears the input buffer
 * @param msg: info printed to prompt the user
 * @return input buffer from end user ready for validation
 *
 * This function uses a fixed buffer to prevent buffer overrun and with stream
 * clearing after the usage to prevent traditional exploits.
 * Memory should be deallocated by the client
 */
extern char * get_input(const char *msg);
//extern void purge_input(void);
//extern int get_index(void);

/**
 * @brief Validates the input buffer as a string
 * @param input: input buffer to be validated
 * @return B_TRUE if valid; B_FALSE otherwise
 *
 * This function checks for spaces, line feeds and carriage returns as first
 * character to prevent empty strings
 */
extern int validateString(const char *input);

/**
 * @brief Validates the input buffer as a binary value (0 or 1)
 * @param input: input buffer to be validated
 * @return value if valid; ERROR_INVALID otherwise
 */
extern int validateBool(const char *input);

/**
 * @brief Validates the input buffer as a floating point value
 * @param input: input buffer to be validated
 * @return value if valid; ERROR_INVALID (converted to float) otherwise
 * 
 * This function scans the buffer for valid float values, and returns the value
 * if only 1 floating-point value is present in the buffer.
 */
extern float validateFloat(const char *input);

/**
 * @brief Validates the input buffer as a integer value
 * @param input: input buffer to be validated
 * @return value if valid; ERROR_INVALID otherwise
 * 
 * This function scans the buffer for valid integer values, and returns the
 * value if only 1 integer is present in the buffer.
 */
extern int validateInt(const char *input);

/**
 * @brief Interface function to prompot and validate input
 * @param msg: info printed to prompt the user
 * @param validateFcn: validator function to validade the input
 * @return return value of the validateFcn passed
 * 
 * Client must bind the appropriate function according 
 * to the type to be checked (NOT USED)
 */
void * prompt_input(const char *msg,  
                    void * (*validateFcn)(const char *input));

/**
 * @brief Counts the nr. of variable arguments passed to a function
 * @param ap: variable argument list containing the arguments
 * @return nr. of arguments in the list
 * 
 * @see menu.c: menus are flexible; they can contain a variable nr. of options
 */
int count_vararg(va_list ap);

/**
 * @brief Prints a msg and wait (block) for *sec* seconds
 * @param msg: info printed to prompt the user
 * @param secs: the nr. of seconds to wait:
 * - 0: returns immediately
 * - -1: waits for user press
 * - >0: waits for # seconds 
 * 
 * Used for UI interaction
 */
void print_msg_wait(const char *msg, int secs);

/**
 * @brief Prints a table header
 * @param header: table header to be printed
 * 
 * Used for User, Activity and Pack tables by the respective
 * *print_line* functions
 */
void print_header(const char *header);

#endif
