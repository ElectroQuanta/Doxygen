/**
 * @file Pack.c
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief Pack's module implementation
 */

#include "Pack.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define DEBUG /**< For debugging throughout the code */

#define PACK_FIFO_SZ 64 /**< FIFO's size is unknown; define a sufficiently larger one */

/**
 * @brief Pack's structure: contains the relevant data members
 */
struct Pack_T
{
    char *nome; /**< name */
    int duracao; /**< duration (in months) */
    double custo; /**< cost [€] */
};

/**
 * @brief Pack's set function pointers
 */
typedef int(*pack_func)(Pack_T);
/**
 * @brief Array of pack's function pointers (used for creation)
 */
pack_func pack_set_functions[] = {
    &pack_set_name,
    &pack_set_duration,
    &pack_set_cost,
    NULL};
                             
/**
 * @brief Allocates memory for an Pack's instance
 * @return initialized memory for Pack
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static Pack_T pack_new()
{
	Pack_T pack = malloc(sizeof(*pack));
	assert(pack);
	return pack;
}

Pack_T pack_ctor()
{
   Pack_T pack = pack_new();
   /* Construct pack */
   pack->nome = NULL;
   pack->duracao = 1; // minimum duration
   pack->custo = 5.0; // minimum cost

   return pack;
}

void pack_dtor(Pack_T pack)
{
    free(pack->nome);
    free(pack);
}

bool pack_clone(const Pack_T pack, Pack_T clone)
{
    if(!pack || !clone)
        return false;

   /* Copy info to clone from pack */
   if(pack->nome)
   {
       clone->nome = malloc(strlen(pack->nome) + 1);
       strcpy(clone->nome, pack->nome);
   }
   clone->duracao = pack->duracao;
   clone->custo = pack->custo;

   return true;
}

int pack_call_set_fcn(Pack_T pack, int idx)
{
    int nr_opts = 0;
    /* Get total nr. of options */
    while (pack_set_functions[nr_opts++] != NULL)
        ;
    
    if(idx > nr_opts - 2  )
        return -1; // Invalid idx

    /* Invoke designated set function */
    return pack_set_functions[idx](pack);
}

int pack_create(Pack_T pack)
{
    if(!pack)
        return B_FALSE;
    /* The pack may abort the input anytime, so the 
       setting should only be done at the end */
    Pack_T tmp = pack_ctor();
    int i = 0, nr_opts = 0;

    /* Print a blank line for visualization purposes */
    printf("\n--------- Novo Pack --------\n");

/* Build pack */
    /* Get total nr. of options */
    while (pack_set_functions[nr_opts++] != NULL)
        ;
    
    /* Invoke every pack function */
    for (; i < (nr_opts - 1); i++)
        if( !pack_set_functions[i](tmp) )
            return B_FALSE;

/* Copying from temporary structure to final destination*/
    /* Nome */
    pack->nome = malloc(strlen(tmp->nome) + 1);
    strcpy(pack->nome, tmp->nome);
    /* Duracao and custo */
    pack->duracao = tmp->duracao;
    pack->custo = tmp->custo;
    printf("----------------------------------\n");

    return B_TRUE;
}

int pack_set_name(Pack_T pack)
{
    if(!pack) // invalid pack
        return B_FALSE;
    
    char *input = NULL;
    do
    {
        input = get_input("Nome");
        // if pack wants to abort, the option should be available and
        // signaled to to the caller
        if (input[0] == ABORT_INPUT)
            return B_FALSE; // invalid
    } while (!validateString(input));


    // allocate dinamic memory for string
    if( !pack->nome )
        pack->nome = malloc( strlen(input) + 1 );
    else
        pack->nome = realloc(pack->nome, strlen(input) + 1);

    // copy string
    strcpy(pack->nome, input);

    return B_TRUE; // valid
}

int pack_set_duration(Pack_T pack)
{
    if(!pack) // invalid pack
        return B_FALSE;

    char *input = NULL;
    int val = 0;
    // Enquanto o pack nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Duracao [meses]");
        if (input[0] == ABORT_INPUT) // check if pack wants to abort
            return B_FALSE; // invalid
        val = validateInt(input);
    } while ( val < 1 );
    // E' valido, logo podemos fazer o set
    pack->duracao = val;

    return B_TRUE; // valid
}

int pack_set_cost(Pack_T pack)
{
    if(!pack) // invalid pack
        return B_FALSE;

    char *input = NULL;
    float amount = 0.0;
    // Enquanto o pack nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Custo [EURO]");
        if (input[0] == ABORT_INPUT) // check if pack wants to abort
            return B_FALSE; // invalid
        amount = validateFloat(input);
    } while (amount <= 0.0);

    /* Set the value */
    pack->custo = amount;
    return B_TRUE; // valid
}

const char* pack_get_name(const Pack_T pack)
{
    return pack->nome; 
}

int pack_get_duration(const Pack_T pack)
{
    return pack->duracao; 
}

float pack_get_cost(const Pack_T pack)
{
    return pack->custo; 
}

void pack_print(const Pack_T pack)
{
    printf("--------------PACK INFO--------------\n");
    printf("Nome: %s\n", pack->nome);
    printf("Duracao [meses]: %d\n", pack->duracao);
    printf("Custo [EURO]: %.2f\n", pack->custo);
    printf("-------------------------------------\n\n");
}

void pack_print_line(const Pack_T pack)
{
    if(!pack)
        return;
    
    printf("%s", pack->nome);
    printf(", %d", pack->duracao);
    printf(", %.2f\n", pack->custo);
}

/* Comparators */
int pack_cmp_name(const Pack_T pack1, const Pack_T pack2)
{
    if (!pack1->nome || !pack2->nome)
        return -1;
    return strcmp(pack1->nome, pack2->nome);
}

Fifo_T pack_serialize(Pack_T pack)
{
    if(!pack)
        return NULL;

    size_t sz = strlen(pack->nome) + 1 
        + sizeof(pack->duracao) + sizeof(pack->custo);
 
    Fifo_T fifo = Fifo_ctor(sz);
    //Fifo_push(fifo, &sz, sizeof(sz));

/* Nome */
    sz = strlen(pack->nome) + 1;
    Fifo_push(fifo, &sz, sizeof(sz));
    Fifo_push(fifo, pack->nome, sz);
/* duracao */
    Fifo_push(fifo, &(pack->duracao), sizeof(pack->duracao));
/* custo */
    Fifo_push(fifo, &(pack->custo), sizeof(pack->custo));

    return fifo;
}

Pack_T pack_deserialize(Fifo_T fifo)
{
    if(!fifo)
        return NULL;
    size_t sz = 0;

    /* Construct a pack */
    Pack_T pack = pack_ctor();
/* Retrieve size of nome */
    Fifo_pop(fifo, &sz, sizeof(sz));
/* Initialize nome */
    pack->nome = malloc(sz);
    assert(pack->nome);
    Fifo_pop(fifo, pack->nome, sz);
/* Initialize duracao */
    Fifo_pop(fifo, &(pack->duracao), sizeof(pack->duracao));
/* Initialize custo */
    Fifo_pop(fifo, &(pack->custo), sizeof(pack->custo));

    return pack;
}
