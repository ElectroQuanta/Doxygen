/**
 * @file m-utils.c
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Implementation of functions and macros used globally 
 * throughout the application
 */

#include "m-utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef POSIX
#include <unistd.h> // for sleep
#else
#include <windows.h>
#endif


/* Get user input in a "safe" way
 * Prompts for the =msg= passed as param
 */
char * get_input(const char *msg)
{
/* Allocate memory for string */
    char *input = malloc(BUF_SZ);
    assert(input);
/* Print message to user */
    if(msg)
        printf("%s (# to abort): ", msg);
    // else msg==NULL --> used for Menus

/* Get input from stdin with max of BUF_SZ length*/
    fgets(input, BUF_SZ, stdin); // get input
/* Null-terminate string */
    input[strlen(input) - 1] = '\0';
/* Reopen stdin to clear the input buffer (prevents overrun of input) */
    stdin = freopen(NULL,"r",stdin);

    return input;
}

int validateString(const char *input)
{
    if(!strlen(input))
        return B_FALSE; // invalid

    if (input[0] == SPACE || input[0] == LF || input[0] == CR)
        return B_FALSE; // invalid

    return B_TRUE; // valid
}

int validateInt(const char *input)
{
    int val;
    // sscanf is used, because it can assign the integer and also test the nr.
// of values contained in the input buffer
    if (sscanf(input, "%d", &val) != 1)
        return ERROR_INVALID; // more than 1 value was entered, so return error

    return val; // return integer
}

int validateBool(const char *input)
{
    int val = 0;
    // sscanf is used, because it can assign the integer and also test the nr.
// of values contained in the input buffer
    if (sscanf(input, "%d", &val) != 1)
        return ERROR_INVALID; // more than 1 value was entered, so return error

    // if val != 0 OR val != 1 return error
    if (val != 0 && val != 1)
        return ERROR_INVALID; // more than 1 value was entered, so return error
    return val; // return integer
}

float validateFloat(const char *input)
{
    float val;
    // sscanf is used, because it can assign the integer and also test the nr.
// of values contained in the input buffer
    if (sscanf(input, "%f", &val) != 1)
        return -1.0; // more than 1 value was entered, so return error

    return val; // return integer
}

void * prompt_input(const char *msg,
                    void * (*validateFcn)(const char *input))
{
    char *inp = get_input(msg);
    return validateFcn(inp);
}

int count_vararg(va_list ap)
{ 
    int count = 0;
    while( va_arg(ap, const char *) )
        count++;

    return count;
}

void print_msg_wait(const char *msg, int secs)
{
    printf("\n\n%s\n", msg);
    if(secs > 0)
        sleep(secs);
    else
        getchar();
}

void print_header(const char *header)
{
    char delim = '-';
    int line = strlen(header) + 1;
/* Print line with delimiters */
    while(--line)
        putchar(delim);
    putchar('\n');

/* Print Header */
    printf("%s\n", header);

/* Print line with delimiters */
    line = strlen(header) + 1;
    while(--line)
        putchar(delim);
    putchar('\n');
}

//void purge_input(void)
//{
//  int ch;
//  while( ( (ch = getchar()) != EOF) && (ch != '\n') )
//    ;
//}
//
//int get_index(void)
//{
//    int idx;
//    char *input;
//    do
//    {
//        input = get_input("Indice (# to abort)");
//        if (input[0] == ABORT_INPUT) // check if user wants to abort
//            break;
//        idx = validateInt(input);
//    } while ( idx == ERROR_INVALID);
//
//    return idx;
//}
