/**
 * @file User.c
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief User's module implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "User.h"
#include "Activity.h"
#include "Pack.h"
#include "list.h"
#include "m-utils.h"

#define DEBUG /**< For debugging throughout the code */

#define USER_FIFO_SZ 1024 /**< FIFO's size is unknown; define a sufficiently larger one */

/**
 * @brief User's structure: contains the relevant data members
 */
struct User_T // Atomic
{
    char *username; /**< username: unique */
    char *pass; /**< password */
    char *nome; /**< name */
    int idade; /**< age */
    char sexo; /**< sex */
    float altura; /**< height */
    float peso; /**< weight */
    float bmi; /**< bmi */
    float saldo; /**< balance */
    enum User_type tipo; /**< type: @see User_type */
    Pack_T pack; /**< single Pack for User */
    List_T activities; /**< list of activities the user is signed in */
};

/**
 * @brief Get user type converted to a readable string
 * @param user: a constructed user
 * @return user's type string 
 */
static char * user_get_tipo_str(const User_T user)
{
    char *tipo_str = malloc(sizeof("Funcionario") + 1);
    assert(tipo_str);
    switch (user->tipo)
    {
    case Gerente:
        strcpy(tipo_str, "Gerente");
        break;
    case Func:
        strcpy(tipo_str, "Funcionario");
        break;
    default:
        strcpy(tipo_str, "Cliente");
    }
    return tipo_str;
}

/**
 * @brief Calculates User Body Mass Index (BMI)
 * @param user: a constructed user
 * @return user's type string 
 * 
 * BMI is calculated as: BMI = weight/height^2
 */
static void user_calc_bmi(User_T user)
{
    user->bmi = user->peso / (user->altura * user->altura);
}

/**
 * @brief User's set function pointers
 */
typedef int(*user_func)(User_T );
/**
 * @brief Array of User's function pointers (used for creation)
 */
static user_func user_set_functions[] = {
    &user_set_username,
    &user_set_pass,
    &user_set_name,
    &user_set_idade,
    &user_set_sexo,
    &user_set_altura,
    &user_set_peso,
    &user_set_saldo,
    NULL};

/**
 * @brief Allocates memory for an User's instance
 * @return initialized memory for User
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static User_T user_new()
{
   User_T user = malloc(sizeof(*user));
   assert(user);
   return user;
}

User_T user_ctor(enum User_type tipo)
{
   User_T user = user_new();
   /* Construct User */
   user->tipo = tipo;
   user->username = NULL;
   user->pass = NULL;
   user->nome = NULL;
   user->idade = 18;
   user->sexo = 0;
   user->altura = 1.5;
   user->peso = 50.0;
   user->saldo = 0.0;
   user->pack = NULL;
   user->activities = List_ctor((void *)activity_ctor,
                                (void *)activity_cmp_time, 
                                (void *)activity_dtor, 
                                (void *)activity_print);
   user_calc_bmi(user); 

   return user;
}

/* Destroy user */
void user_dtor(User_T user)
{
    if(!user)
        return;

#ifdef DEBUG
    printf("Dtor\n");
    printf("%p\n", user);
    user_print_info( user );
#endif

    /* Release dynamically allocated memory first */
    free(user->nome);
    free(user->pass);
    free(user->username);

    /* Release user */
    free(user);

    if(user)
        printf("%p\n", user);
}

bool user_clone(const User_T user, User_T clone)
{
    if(!user || !clone)
        return false;

   /* Copy info to clone from user */
   if(user->nome)
   {
       clone->nome = malloc(strlen(user->nome) + 1);
       strcpy(clone->nome, user->nome);
   }
   if(user->pass)
   {
       clone->pass = malloc(strlen(user->pass) + 1);
       strcpy(clone->pass, user->pass);
   }
   if(user->username)
   {
       clone->username = malloc(strlen(user->username) + 1);
       strcpy(clone->username, user->username);
   }

   clone->idade = user->idade;
   clone->sexo = user->sexo;
   clone->altura = user->altura;
   clone->peso = user->peso;
   clone->saldo = user->saldo;
   user_calc_bmi(clone);

//#ifdef DEBUG
//   printf("user = %p,\t clone = %p\n", user, clone);
//#endif

   return true;
}

int user_call_set_fcn(User_T user, int idx)
{
    int nr_opts = 0;
    /* Get total nr. of options */
    while (user_set_functions[nr_opts++] != NULL)
        ;
    
    if(idx > nr_opts - 2  )
        return -1; // Invalid idx

    /* Invoke designated set function */
    return user_set_functions[idx](user);
}

/* Create a new user */
int user_create(User_T user)
{
    if(!user)
        return B_FALSE;
    /* The user may abort the input anytime, so the 
       setting should only be done at the end */
    User_T tmp = user_ctor(user->tipo);
    int i = 0, nr_opts = 0;

    /* Print a blank line for visualization purposes */
    printf("\n--------- Novo Utilizador --------\n");

/* Build user */
    /* Get total nr. of options */
    while (user_set_functions[nr_opts++] != NULL)
        ;
    
    /* Invoke every user function */
    for (; i < (nr_opts - 1); i++)
        if( !user_set_functions[i](tmp) )
            return B_FALSE;

/* Copying from temporary structure to final destination*/
    /* Nome */
    user->nome = malloc(strlen(tmp->nome) + 1);
    strcpy(user->nome, tmp->nome);
    /* Pass */
    user->pass = malloc(strlen(tmp->pass) + 1);
    strcpy(user->pass, tmp->pass);
    /* Username */
    user->username = malloc(strlen(tmp->username) + 1);
    strcpy(user->username, tmp->username);

    user->idade = tmp->idade;
    user->sexo = tmp->sexo;
    user->altura = tmp->altura;
    user->peso = tmp->peso;
    user->saldo = tmp->saldo;

    /* Calculate bmi */
    user_calc_bmi(user);
    printf("----------------------------------\n");

    return B_TRUE;
}

/* Compare two users by username*/
int user_cmp_username(const User_T user1, const User_T user2)
{
    if(!user1->username || !user2->username)
        return -1;
#ifdef POSIX
    return strcasecmp(user1->username, user2->username);
#else
    return strcmpi( user1->username, user2->username);
#endif
}

int user_cmp_type(const User_T user1, const User_T user2)
{
    return (user1->tipo - user2->tipo);
}

int user_set_username(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;
    
    char *input = NULL;
    do
    {
        input = get_input("Username");
        // if User wants to abort, the option should be available and
        // signaled to to the caller
        if (input[0] == ABORT_INPUT)
            return B_FALSE; // invalid
    } while (!validateString(input));

    // allocate dinamic memory for string
    if( !user->username )
    {
//#ifdef DEBUG
//        printf("Username = NULL\n");
//#endif
        user->username = malloc( strlen(input) + 1 );
    }
    else
    {
//#ifdef DEBUG
//        printf("Username != NULL\n");
//#endif
        user->username = realloc(user->username, strlen(input) + 1);
    }

    // copy string
    strcpy(user->username, input);

    return B_TRUE; // valid
}

int user_set_name(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;
    
    char *input = NULL;
    //int *valid = NULL;
    // Enquanto o user nao inserir uma string valida
    // obter input
    //do{
    //    valid = prompt_input("Nome", input, (void *)validateString);
    //    if(input && input[0] == ABORT_INPUT)
    //        return B_FALSE;
    //}while( (*valid) != B_TRUE ); 
    do
    {
        input = get_input("Nome");
        // if User wants to abort, the option should be available and
        // signaled to to the caller
        if (input[0] == ABORT_INPUT)
            return B_FALSE; // invalid
    } while (!validateString(input));


    // allocate dinamic memory for string
    if( !user->nome )
        user->nome = malloc( strlen(input) + 1 );
    else
        user->nome = realloc(user->nome, strlen(input) + 1);

    // copy string
    strcpy(user->nome, input);

    return B_TRUE; // valid
}

int user_set_pass(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;

    char *input = NULL;
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Pass");

        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
    } while (!validateString(input));

    // allocate dinamic memory for string
    if( !user->pass )
        user->pass = malloc( strlen(input) + 1 );
    else
        user->pass = realloc(user->pass, strlen(input) + 1);

    // copy string
    strcpy(user->pass, input);

    return B_TRUE; // valid
}

int user_set_idade(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;

    char *input = NULL;
    int idade = 0;
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Idade (dos 8-88)");
        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
        idade = validateInt(input);
    } while ((idade < 8 || idade > 88));
    // E' valido, logo podemos fazer o set
    user->idade = idade;

    return B_TRUE; // valid
}

int user_set_sexo(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;

    char *input = NULL;
    int sexo = 0;
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Sexo (0: M; 1: F)");
        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
        sexo = validateBool(input);
    } while (sexo == ERROR_INVALID);
    // E' valido, logo podemos fazer o set
    user->sexo = sexo;

    return B_TRUE; // valid
}

int user_set_altura(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;

    char *input = NULL;
    float altura = 0.0;
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Altura [m]");
        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
        altura = validateFloat(input);
    } while (altura == -1.0 || altura < 0.2 || altura > 2.6);
    // E' valido, logo podemos fazer o set
    user->altura = altura;

    return B_TRUE; // valid
}

int user_set_peso(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;

    char *input = NULL;
    float peso = 0.0;
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Peso [kg]");
        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
        peso = validateFloat(input);
    } while (peso == -1.0 || peso < 10.0 || peso > 300.0);
    // E' valido, logo podemos fazer o set
    user->peso = peso;

    return B_TRUE; // valid
}

int user_set_saldo(User_T user)
{
    if(!user) // invalid user
        return B_FALSE;

    char *input = NULL;
    float amount = 0.0;
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Montante a carregar [EURO]");
        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
        amount = validateFloat(input);
    } while (amount <= 0.0);

    /* Charge saldo */
    user->saldo += amount;
    return B_TRUE; // valid
}

int user_pay(User_T user, float amount)
{
    if(!user) // invalid user
        return B_FALSE;

    user->saldo += amount;
    return B_TRUE;
}

enum User_type user_get_type(const User_T user)
{
    return user->tipo;
}

const char * user_get_pass(const User_T user)
{
    return user->pass;
}

float user_get_saldo(const User_T user)
{
    return user->saldo;
}

/* Prints minimal info about the user */
void user_print_info(const User_T user)
{
    if(!user)
        return;

    const char *tipo = user_get_tipo_str(user);

    printf("--------------USER INFO--------------\n");
    printf("Tipo: %s\n\n", tipo);
    printf("Nome: %s\n", user->nome);
    printf("Idade: %d\n", user->idade);
    printf("Sexo: %c\n", ( user->sexo ? 'F' : 'M'  )  );
#ifdef DEBUG
    printf("Username: %s\n", user->username);
    printf("Pass: %s\n", user->pass);
#endif
    printf("-------------------------------------\n\n");

/* Release dynamically allocated memory */
    free((void *)tipo);
}

/* Prints extra info about the user */
void user_print_info_extra(const User_T user)
{
    if(!user) // invalid user
        return;

    printf("Altura: %.2f\n", user->altura);
    printf("Peso: %.2f\n", user->peso);
    printf("BMI: %.2f\n", user->bmi);
}

void user_print_username(const User_T user)
{
    if(!user)
        return;

    printf("\tUsername: %s\n", user->username);
}

void user_print_line(const User_T user)
{
    if(!user)
        return;
    
    const char *tipo = user_get_tipo_str(user);
    printf("%s", tipo);
    printf(", %s", user->username);
    printf(", %s", user->nome);
    printf(", %.2d", user->idade);
    printf(", %c", ( user->sexo ? 'F' : 'M'  )  );
    printf(", %.2f", user->altura);
    printf(", %.2f", user->peso);
    printf(", %.2f", user->bmi);
    printf(", %.2f\n", user->saldo);
}

bool user_add_activity(User_T user, const Act_T activity)
{
    if(!activity || ! user)
        return false;

/* Insert user in the list of activity's users */
    if(! List_insert_ascend( &user->activities, activity, true, false, NULL) )
        return false; // user already inserted

    return true;
}

bool user_remove_activity(User_T user, const Act_T activity)
{
    if(!activity || ! user)
        return false;

/* Check if activity is empty */
    if( List_count(user->activities) < 1)
        return false;

/* Remove user in the list of activity's users */
    if(! List_remove( &user->activities, activity) )
        return false; // user already removed

    return true;
}

void user_list_activities(const User_T user)
{
    List_print_all(user->activities, (void *)activity_print_line,
                   false, table_header_activity);
}

List_T user_get_activities(const User_T user)
{
    return user->activities;
}

Fifo_T user_serialize(User_T user)
{
    if(!user)
        return NULL;

/* Unknown size -> fix a sufficiently large buffer */
    size_t sz = USER_FIFO_SZ;
 
    Fifo_T fifo = Fifo_ctor(sz);
//    Fifo_T fifo_pack = NULL;
    //Fifo_push(fifo, &sz, sizeof(sz));
//    char *username; // rw (unique)
//    char *pass; // rw
//    char *nome; // rw
//    int idade; // rw
//    char sexo; // rw
//    float altura; // rw
//    float peso; // rw
//    float bmi; // read
//    float saldo; // read
//    enum User_type tipo; // read
//    Pack_T pack; // single pack for user
//    List_T activities; // activities the user signed in

/* username */
    sz = strlen(user->username) + 1;
    Fifo_push(fifo, &sz, sizeof(sz));
    Fifo_push(fifo, user->username, sz);
/* pass */
    sz = strlen(user->pass) + 1;
    Fifo_push(fifo, &sz, sizeof(sz));
    Fifo_push(fifo, user->pass, sz);
/* Nome */
    sz = strlen(user->nome) + 1;
    Fifo_push(fifo, &sz, sizeof(sz));
    Fifo_push(fifo, user->nome, sz);
/* idade */
    Fifo_push(fifo, &(user->idade), sizeof(user->idade));
/* sexo */
    Fifo_push(fifo, &(user->sexo), sizeof(user->sexo));
/* altura */
    Fifo_push(fifo, &(user->altura), sizeof(user->altura));
/* peso */
    Fifo_push(fifo, &(user->peso), sizeof(user->peso));
/* saldo */
    Fifo_push(fifo, &(user->saldo), sizeof(user->saldo));
/* tipo */
    Fifo_push(fifo, &(user->tipo), sizeof(user->tipo));
/* Pack */
//    sz = 0;
//    if(user->pack)
//    {
//        /* Push TAG */
//        sz = PACK_TAG;
//        Fifo_push(fifo, &sz, sizeof(sz));
//        /* Serialize pack */
//        fifo_pack = pack_serialize(user->pack);
//        sz = Fifo_get_size(fifo_pack);
//        /* Write size of pack first */
////    Fifo_push(fifo, &sz, sizeof(sz));
//        /* Push pack */
//        Fifo_push(fifo, fifo_pack, sz);
//        /* Destroy fifo_pack (no longer required) */
//        Fifo_dtor(fifo_pack);
//    }
//    else  /* Push 0 */
//        Fifo_push(fifo, &sz, sizeof(sz));

/* Reset size to actual size (write position) */
    Fifo_set_size(fifo, Fifo_get_write_idx(fifo));

    return fifo;
}

User_T user_deserialize(Fifo_T fifo)
{
    if(!fifo)
        return NULL;
    size_t sz = 0;

    /* Construct a user (type will be corrected later on)*/
    User_T user = user_ctor(Cliente);

/* Retrieve size of username */
    Fifo_pop(fifo, &sz, sizeof(sz));
/* Initialize username */
    user->username = malloc(sz);
    assert(user->username);
    Fifo_pop(fifo, user->username, sz);
/* Retrieve size of pass */
    Fifo_pop(fifo, &sz, sizeof(sz));
/* Initialize pass */
    user->pass = malloc(sz);
    assert(user->pass);
    Fifo_pop(fifo, user->pass, sz);
/* Retrieve size of nome */
    Fifo_pop(fifo, &sz, sizeof(sz));
/* Initialize nome */
    user->nome = malloc(sz);
    assert(user->nome);
    Fifo_pop(fifo, user->nome, sz);
/* idade */
    Fifo_pop(fifo, &(user->idade), sizeof(user->idade));
/* sexo */
    Fifo_pop(fifo, &(user->sexo), sizeof(user->sexo));
/* altura */
    Fifo_pop(fifo, &(user->altura), sizeof(user->altura));
/* peso */
    Fifo_pop(fifo, &(user->peso), sizeof(user->peso));
/* saldo */
    Fifo_pop(fifo, &(user->saldo), sizeof(user->saldo));
/* tipo */
    Fifo_pop(fifo, &(user->tipo), sizeof(user->tipo));
/* BMI can be calculated */
    user_calc_bmi(user);
/* Pack */
    /* Check for pack */
//    Fifo_pop(fifo, &sz, sizeof(sz));
//    if(sz == PACK_TAG) /* Deserialize pack */
//        user->pack = pack_deserialize(fifo);
    //fifo_pack = Fifo_ctor(sz);
    /* Destroy fifo_pack (no longer required) */
//    Fifo_dtor(fifo_pack);

    return user;
}
