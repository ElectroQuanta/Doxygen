/**
 * @file Activity.c
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Activity's module implementation
 */

#include "Activity.h"
#include "User.h"
#include "list.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define DEBUG /**< For debugging throughout the code */

#define DAY_INI 2 /**< Initial day */ 
#define HOUR_INI 8	/**< Initial hour */
#define NR_DAYS 6 /**< Nr of working days */ 
#define NR_HOURS 14 /**< Nr of working hours */

#define STR_SZ 30 /**< Buffer size for sprintf */
#define ACT_FIFO_SZ 1024 /**< FIFO's size is unknown; define a sufficiently larger one */


/**
 * @brief Constants retrieving the activity's time
 */
enum act_time{act_day, /**< Activity's day */
              act_hour, /**< Activity's hour */
              act_mins /**< Activity's mins */
};

/**
 * @brief Activity's structure: contains the relevant data members
 */
struct Act_T // Atomic
{
    char *nome; /**< nome */
    int mins_from_start; /**< mins from start of the week */
    int duracao; /**< duration [mins] */
    float custo; /**< cost [€] */
    unsigned vagas; /**< nr of vacancies */
    unsigned max_vagas; /**< Nr of max vacancies */
    List_T users; /**< List of users enlisted to the activity */
};

/**
 * @brief Activity's set function pointers
 */
typedef int(*activity_func)(Act_T );
/**
 * @brief Array of activity's function pointers (used for creation)
 */
static activity_func activity_set_functions[] = {
    &activity_set_nome,
    &activity_set_time,
    &activity_set_duracao,
    &activity_set_custo,
    &activity_set_max_vagas,
    NULL};

/**
 * @brief Allocates memory for an Activity's instance
 * @return initialized memory for Activity
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static Act_T activity_new()
{
   Act_T activity = malloc(sizeof(*activity));
   assert(activity);
   return activity;
}

/**
 * @brief Prompts for the desired temporal variable
 * @param tim: *enum act_time* day, hour or mins 
 * @return valid day of day, hour or mins
 */
static int activity_prompt_time(enum act_time tim)
{
    char *input = NULL, time_str[STR_SZ];
    int val = 0, lbound = 0, ubound = 0;

    switch(tim)
    {
    case act_day:
        strncpy(time_str, "Dia (2: Seg.; 7: Sab):", STR_SZ);
        lbound = DAY_INI; ubound = DAY_INI + NR_DAYS - 1;
        //printf("Hello");
        break;
    case act_hour:
        strncpy(time_str, "Hora (8-21):", STR_SZ);
        lbound = HOUR_INI; ubound = HOUR_INI + NR_HOURS - 1;
        break;
    default: // act_mins
        strncpy(time_str, "Mins (0-59):", STR_SZ);
        lbound = 0; ubound = 59;
    }

    // Enquanto string invalida, obter input
    do
    {
        input = get_input(time_str);
        if (input[0] == ABORT_INPUT) // check if activity wants to abort
            return -1; // invalid
        val = validateInt(input);
    } while ( (val < lbound) || (val > ubound) );

    return val; // return time
}

/**
 * @brief Calculates *mins_from_start* from day, hour and mins
 * @param day: valid day
 * @param hour: valid hour
 * @param mins: valid mins
 * @return *mins_from_start*
 */
static int activity_calc_mins(int day, int hour, int mins)
{
    return ( ((day - DAY_INI)*NR_HOURS + (hour - HOUR_INI))*60 + mins ); 
}

/**
 * @brief Gets activity's day
 * @param activity: valid activity
 * @return *day*
 */
static int activity_get_day(const Act_T activity)
{
    return ( DAY_INI + (activity->mins_from_start / 60) / NR_HOURS);
}

/**
 * @brief Gets activity's hour
 * @param activity: valid activity
 * @return *hour*
 */
static int activity_get_hour(const Act_T activity)
{
    return ( HOUR_INI + (activity->mins_from_start / 60) % NR_HOURS);
}

/**
 * @brief Gets activity's mins
 * @param activity: valid activity
 * @return *mins*
 */
static int activity_get_mins(const Act_T activity)
{
    return (activity->mins_from_start % 60);
}

Act_T activity_ctor()
{
   Act_T activity = activity_new();
   /* Construct Activity */
   activity->nome = NULL;
   activity->mins_from_start = 0;
   activity->duracao = 15; // mins
   activity->custo = 5.0; // EURO
   activity->vagas = 0;
   activity->max_vagas = 10;
   activity->users = List_ctor((void *)user_ctor,
                               (void *)user_cmp_username, 
                               (void *)user_dtor, 
                               (void *)user_print_username);

   return activity;
}

/* Destroy activity */
void activity_dtor(Act_T activity)
{
    if(!activity)
        return;

    /* Release dynamically allocated memory first */
    free(activity->nome);

    /* Release activity */
    free(activity);
}

bool activity_clone(const Act_T activity, Act_T clone)
{
    if(!activity || !clone)
        return false;

   /* Copy info to clone from activity */
   if(activity->nome)
   {
       clone->nome = malloc(strlen(activity->nome) + 1);
       strcpy(clone->nome, activity->nome);
   }
   clone->mins_from_start = activity->mins_from_start;
   clone->duracao = activity->duracao;
   clone->custo = activity->custo;
   clone->max_vagas = activity->max_vagas;

   return true;
}

int activity_call_set_fcn(Act_T activity, int idx)
{
    int nr_opts = 0;
    /* Get total nr. of options */
    while (activity_set_functions[nr_opts++] != NULL)
        ;
    
    if(idx > nr_opts - 2  )
        return -1; // Invalid idx

    /* Invoke designated set function */
    return activity_set_functions[idx](activity);
}

/* Create a new activity */
int activity_create(Act_T activity)
{
    if(!activity)
        return B_FALSE;
    /* The activity may abort the input anytime, so the 
       setting should only be done at the end */
    Act_T tmp = activity_ctor( );
    int i = 0, nr_opts = 0;

    /* Print a blank line for visualization purposes */
    printf("\n--------- Nova Actividade --------\n");

/* Build activity */
    /* Get total nr. of options */
    while (activity_set_functions[nr_opts++] != NULL)
        ;
    
    /* Invoke every activity function */
    for (; i < (nr_opts - 1); i++)
        if( !activity_set_functions[i](tmp) )
            return B_FALSE;

/* Copying from temporary structure to final destination*/
    /* Nome */
    activity->nome = malloc(strlen(tmp->nome) + 1);
    strcpy(activity->nome, tmp->nome);

    activity->mins_from_start = tmp->mins_from_start;
    activity->duracao = tmp->duracao;
    activity->custo = tmp->custo;
//    activity->vagas = tmp->vagas;
    activity->max_vagas = tmp->max_vagas;

    printf("----------------------------------\n");

    return B_TRUE;
}

/* Setters */
int activity_set_nome(Act_T activity)
{
    if(!activity) // invalid activity
        return B_FALSE;
    
    char *input = NULL;
    do
    {
        input = get_input("Nome");
        // if activity wants to abort, the option should be available and
        // signaled to to the caller
        if (input[0] == ABORT_INPUT)
            return B_FALSE; // invalid
    } while (!validateString(input));


    // allocate dinamic memory for string
    if( !activity->nome )
        activity->nome = malloc( strlen(input) + 1 );
    else
        activity->nome = realloc(activity->nome, strlen(input) + 1);

    // copy string
    strcpy(activity->nome, input);

    return B_TRUE; // valid
}

int activity_set_duracao(Act_T activity)
{
    if(!activity) // invalid activity
        return B_FALSE;

    char *input = NULL;
    int val = 0;
    // Enquanto o activity nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Duracao (15-60) [mins]: ");
        if (input[0] == ABORT_INPUT) // check if activity wants to abort
            return B_FALSE; // invalid
        val = validateInt(input);
    } while ( (val < 15) || (val > 60) );
    // E' valido, logo podemos fazer o set
    activity->duracao = val;

    return B_TRUE; // valid
}

int activity_set_custo(Act_T activity)
{
    if(!activity) // invalid activity
        return B_FALSE;

    char *input = NULL;
    float amount = 0.0;
    // Enquanto o activity nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Custo [EURO]");
        if (input[0] == ABORT_INPUT) // check if activity wants to abort
            return B_FALSE; // invalid
        amount = validateFloat(input);
    } while (amount <= 0.0);

    /* Set the value */
    activity->custo = amount;
    return B_TRUE; // valid
}

int activity_set_max_vagas(Act_T activity)
{
    if(!activity) // invalid activity
        return B_FALSE;

    char *input = NULL;
    int val = 0;
    // Enquanto o activity nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Max_Vagas (10-30): ");
        if (input[0] == ABORT_INPUT) // check if activity wants to abort
            return B_FALSE; // invalid
        val = validateInt(input);
    } while ( (val < 10) || (val > 30) );
    // E' valido, logo podemos fazer o set
    activity->max_vagas = val;

    return B_TRUE; // valid
}

int activity_set_time(Act_T activity)
{
    if(!activity)
        return B_FALSE;

    int day, hour, mins;

    /* Prompt for day */
    if( ( day = activity_prompt_time(act_day) ) == -1)
        return B_FALSE;
    /* Prompt for hour */
    if( ( hour = activity_prompt_time(act_hour) ) == -1)
        return B_FALSE;
    /* Prompt for mins */
    if( ( mins = activity_prompt_time(act_mins) ) == -1)
        return B_FALSE;

    /* Valid hour -> set mins_from_start */
    activity->mins_from_start = activity_calc_mins(day, hour, mins);
    
    return B_TRUE; // valid
}

/* Getters */
const char* activity_get_nome(const Act_T activity)
{
    return activity->nome;
}

int activity_get_mins_from_start(const Act_T activity)
{
    return activity->mins_from_start;
}

int activity_get_duracao(const Act_T activity)
{
    return activity->duracao; 
}

float activity_get_custo(const Act_T activity)
{
    return activity->custo; 
}

int activity_get_max_vagas(const Act_T activity)
{
    return activity->max_vagas; 
}

/* Printers */
void activity_print(const Act_T activity)
{
    int day = activity_get_day(activity);
    printf("--------------ACTIVITY INFO--------------\n");
    printf("Nome: %s\n", activity->nome);
    if(day > 6)
        printf("Data [DD; HH:MM]: [Sab.; %.2d:%.2d]\n",
               activity_get_hour(activity), activity_get_mins(activity));
    else
        printf("Data [DD; HH:MM]: [%dª; %.2d:%.2d]\n", day,
               activity_get_hour(activity), activity_get_mins(activity));
    printf("Mins from start [mins]: %.4d\n", activity->mins_from_start);
    printf("Duracao [mins]: %.2d\n", activity->duracao);
    printf("Custo [EURO]: %.2f\n", activity->custo);
    printf("Vagas: [%u/%u]\n", activity->vagas, activity->max_vagas);
    /* Users */
    printf("\t********** Inscritos *************\n");
    List_print_all(activity->users, NULL, true, NULL);
    printf("\t**********************************\n");
    printf("-------------------------------------\n\n");
}

void activity_print_line(const Act_T activity)
{
    if(!activity)
        return;
    
    int day = activity_get_day(activity);
    int hour = activity_get_hour(activity);
    int mins = activity_get_mins(activity);
    char str[30];
    if(day > 6)
        sprintf(str, "%s; %.2d:%.2d", "Sab.", hour, mins);
    else
        sprintf(str, "%dª; %.2d:%.2d", day, hour, mins);

    printf("[%s]", str);
    printf(", %s", activity->nome);
    printf(", %.2d", activity->duracao);
    printf(", [%u/%u]\n", activity->vagas, activity->max_vagas);
}

/* Comparators */
int activity_cmp_time(const Act_T activity1, const Act_T activity2)
{
//    return (activity1->mins_from_start -
//            (activity2->mins_from_start + activity2->duracao));
    return (activity1->mins_from_start - activity2->mins_from_start);
}

int activity_cmp_name(const Act_T activity1, const Act_T activity2)
{
    return strcmp(activity1->nome, activity2->nome);
}

/* List related */
bool activity_add_user(Act_T activity, const User_T user)
{
    if(!activity || ! user)
        return false;

/* Check if activity is full */
    if( List_count(activity->users) >= activity->max_vagas)
        return false;

/* Insert user in the list of activity's users */
    if(! List_insert_ascend( &activity->users, user, true, false, NULL) )
        return false; // user already inserted

/* Increment vagas */
    activity->vagas++;

    return true;
}

bool activity_remove_user(Act_T activity, const User_T user)
{
    if(!activity || ! user)
        return false;

/* Check if activity is empty */
    if( List_count(activity->users) < 1)
        return false;

/* Remove user in the list of activity's users */
    if(! List_remove( &activity->users, user) )
        return false; // user already removed

/* Decrement vagas */
    activity->vagas--;

    return true;
}

Fifo_T activity_serialize(Act_T activity)
{
    if(!activity)
        return NULL;

//    char *nome; // rw
//    long mins_from_start; // rw (time from start of the week)
//    int duracao; // rw (mins)
//    float custo; // rw (euro)
//    unsigned vagas; // rw
//    unsigned max_vagas; // rw
//    List_T users; // rw

/* Unknown size -> fix a sufficiently large buffer */
    size_t sz = ACT_FIFO_SZ;
 
    Fifo_T fifo = Fifo_ctor(sz);
    //Fifo_push(fifo, &sz, sizeof(sz));

/* Nome */
    sz = strlen(activity->nome) + 1;
    Fifo_push(fifo, &sz, sizeof(sz));
    Fifo_push(fifo, activity->nome, sz);
/* mins_from_start */
    Fifo_push(fifo, &(activity->mins_from_start), 
              sizeof(activity->mins_from_start));
/* duracao */
    Fifo_push(fifo, &(activity->duracao), sizeof(activity->duracao));
/* custo */
    Fifo_push(fifo, &(activity->custo), sizeof(activity->custo));
/* vagas */
    Fifo_push(fifo, &(activity->vagas), sizeof(activity->vagas));
/* max_vagas */
    Fifo_push(fifo, &(activity->max_vagas), sizeof(activity->max_vagas));
/* users (List_T) */


/* Reset size to actual size (write position) */
    Fifo_set_size(fifo, Fifo_get_write_idx(fifo));

    return fifo;
}

Act_T activity_deserialize(Fifo_T fifo)
{
    if(!fifo)
        return NULL;
    size_t sz = 0;

    /* Construct a activity */
    Act_T activity = activity_ctor();
/* Retrieve size of nome */
    Fifo_pop(fifo, &sz, sizeof(sz));
/* Initialize nome */
    activity->nome = malloc(sz);
    assert(activity->nome);
    Fifo_pop(fifo, activity->nome, sz);
/* mins_from_start */
    Fifo_pop(fifo, &(activity->mins_from_start), 
              sizeof(activity->mins_from_start));
/* Initialize duracao */
    Fifo_pop(fifo, &(activity->duracao), sizeof(activity->duracao));
/* Initialize custo */
    Fifo_pop(fifo, &(activity->custo), sizeof(activity->custo));
/* vagas */
    Fifo_pop(fifo, &(activity->vagas), sizeof(activity->vagas));
/* max_vagas */
    Fifo_pop(fifo, &(activity->max_vagas), sizeof(activity->max_vagas));
/* users (List_T) */

    return activity;
}

