/**
 * @file App.c
 * @author Jose Pires
 * @date 12 Jan 2019
 *
 * @brief App's module implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include "App.h"
#include "list.h"
#include "User.h"
#include "Activity.h"
#include "Menu.h"
#include "Pack.h"
#include "Database.h"
#include "m-utils.h"

#define DEBUG /**< For debugging throughout the code */

/* Menus titles */
#define MENU_LOGIN "LOGIN" /**< Menu title for Login state */
#define MENU_GERENTE "GERENTE" /**< Menu title for Gerente state */
#define MENU_FUNC "FUNCIONARIO" /**< Menu title for Func state */
#define MENU_CLIENTE "CLIENTE" /**< Menu title for Client state */ 
#define MENU_MANAGE_CLI "GERIR CLIENTE" /**< Menu title for ManageClient state */ 
#define MENU_MANAGE_ACT "GERIR ACTIVIDADE" /**< Menu title for ManageActivity state */ 
#define MENU_MANAGE_PACK "GERIR PACK" /**< Menu title for Manage Pack state */ 
#define MENU_EDIT_USER "EDITAR UTILIZADOR" /**< Menu title for Edit User state */ 
#define MENU_EDIT_ACT "EDITAR ACTIVIDADE" /**< Menu title for Edit Activity state */ 
#define MENU_SEARCH_ACT "PROCURAR ACTIVIDADE" /**< Menu title for search Activity state */ 
#define MENU_EDIT_PACK "EDITAR PACK" /**< Menu title for Edit Pack state */ 
#define MENU_ACTIV "ACTIVIDADES" /**< Menu title for Client Activities state */ 

/* Database files */
#define DATABASE_USERS "user.db" /**< Database file for users */
#define DATABASE_ACTIVITIES "act.db" /**< Database file for activities */  
#define DATABASE_PACKS "pack.db" /**< Database file for packs */


/**
 * @brief Constants for the App's states. Used in FSM management.
 */
enum App_state{
    S_Login, /**< Login state */
    S_Gerente, /**< Manager state */
    S_Func,/**< Employee state */
    S_Cliente,/**< Client state */
    S_Manage_Cli,/**< Manage Client state */
    S_Manage_Act,/**< Manage Activity state */
    S_Manage_Pack,/**< Manage Pack state */
    S_Edit_User,/**< Edit User state */
    S_Edit_Act,/**< Edit Activity state */
    S_Edit_Pack,/**< Edit Pack state */
    S_Activities,/**< Client Activities state */
    S_Logout,/**< Logout state */
    S_Quit/**< Quit state */
};

/**
 * @brief App's struct: contains the relevant data members
 */
struct App_T
{
    List_T menus; /**< Menus list */
    List_T users;  /**< Users list */
    List_T activities; /**< Activities list */
    List_T packs; /**< List of available packs */
    User_T cur_user; /**< Current user */
    enum App_state state; /**< App's current state */
    enum App_state prev_state; /**< App's previous state */
    void * userdata; /**< ptr to generic data (used in App_state_functions) */
    Database_T db_user; /**< Users database */
    Database_T db_act; /**< Activities database */
    Database_T db_pack; /**< Packs database */
};

/**
 * @brief Allocates memory for an App's instance
 * @return initialized memory for App
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static App_T App_new()
{
    App_T app = malloc(sizeof(*app));
    assert(app);
    return app;
}

/**
 * @brief Deserializes a packet of generic data from the database
 * @param db: a constructed database
 * @param deserialize: pointer to generic function capable of deserializing the specific data of the database
 * @return packet of data; NULL if failure occured
 *
 * *deserialize* functions must be implemented by clients.
 * @see User.h
 * @see Activity.h
 * @see Pack.h
 */
static void * App_deserialize(Database_T db,
                              void *( *deserialize)(Fifo_T fifo))
{
    if(!db || !deserialize)
        return NULL;
    size_t sz = 0;
    Fifo_T fifo = NULL;
    void *data = NULL;

// read/update: Open a file for update (both for input 
// and output). The file must exist.
    if( Database_open(db, "rb") )
    {
//        printf("\nFile exists!\n");
/* Reading size of buffer to allocate and allocate */
        if( !Database_read(db, &sz, sizeof(sz), SEEK_CUR))
            return NULL;
        fifo = Fifo_ctor(sz);
/* Reading data to buffer to update idx externally */
        if( !Database_read(db, Fifo_get_data(fifo), sz, SEEK_CUR) )
            return NULL;
        Fifo_set_write_idx(fifo, sz);
/* Deserialize data and destroy buffer*/
        data = deserialize(fifo);
        Fifo_dtor(fifo);
        return data;
    }
   return NULL; 
}

/**
 * @brief Serializes a packet of generic data to the database
 * @param db: a constructed database
 * @param serialize: pointer to generic function capable of serializing the specific data of the database
 * @param data: packet of generic data to serialize
 * @return true, if successfull; false otherwise
 *
 * *serialize* functions must be implemented by clients.
 * @see User.h
 * @see Activity.h
 * @see Pack.h
 */
static bool App_serialize(Database_T db,
                          void * (*serialize)(Fifo_T fifo), 
                          void *data)
{
    if(!data || !db || !serialize)
        return false;

    size_t sz = 0;
    Fifo_T fifo = NULL;
// read/update: Open a file for update (both for input 
// and output). The file must exist.
    if( Database_open(db, "wb") )
    {
/* Serialize pack */    
        fifo = serialize(data);

/* Update size of file beforehand (so fifo can be allocated on the
   deserialization) */    
//        sz = Fifo_get_size(fifo) + Database_get_size(db);
//        printf("Serialize: sz = %zu", sz);
//        print_msg_wait("Ser", -1);
//        Database_write(db, &sz, sizeof(sz), SEEK_SET); 
/* write size of object beforehand (so fifo can be allocated on the
   deserialization) */    
        sz = Fifo_get_size(fifo);
        Database_write(db, &sz, sizeof(sz), SEEK_END); 
/* write data */
        Database_write(db, Fifo_get_data(fifo), sz, SEEK_END);
        return true;
    }
   return false; 
}

/**
 * @brief Creates the static menus for the application
 * @return A list of initialized to menus to be owned by App
 */
static List_T App_create_menus()
{
/* Construct menus */
    List_T menus = List_ctor( (void *)Menu_ctor,
                              (void *)Menu_compare,
                              (void *)Menu_dtor,
                              (void *)Menu_process);
/* Initialize menus */ 
    // Menu_T Menu_ctor(char *title, opt1, opt2, ..., optN, 0);
    Menu_T m0 = Menu_ctor(MENU_LOGIN, "Login", "Sair", 0);

    Menu_T m1 = Menu_ctor(MENU_GERENTE,
                          "Adicionar Funcionario", "Editar Funcionario",
                          "Listar Funcionario", "Remover Funcionario",
                          "Listar Funcionarios", "Sair", NULL);
    
    Menu_T m2 = Menu_ctor(MENU_FUNC,
                          "Gerir Cliente", "Gerir Actividade",
                          "Gerir Pack", "Sair", NULL);

    Menu_T m3 = Menu_ctor(MENU_CLIENTE,
                          "Editar info", "Carregar saldo",
                          "Actividades", "Seleccionar Pack",
                          "Sair", NULL);
    
    Menu_T m4 = Menu_ctor(MENU_MANAGE_CLI,
                          "Adicionar Cliente", "Editar Cliente",
                          "Listar Cliente", "Remover Cliente",
                          "Listar Clientes", "Sair", NULL);
    
    Menu_T m5 = Menu_ctor(MENU_MANAGE_ACT,
                          "Adicionar Actividade", "Editar Actividade",
                          "Listar Actividade", "Remover Actividade",
                          "Listar Actividades", "Sair", NULL);

    Menu_T m6 = Menu_ctor(MENU_MANAGE_PACK,
                          "Adicionar Pack", "Editar Pack",
                          "Listar Pack", "Remover Pack",
                          "Listar Packs", "Sair", NULL);
    
    Menu_T m7 = Menu_ctor(MENU_EDIT_USER,
                          "Username", "Pass", "Nome", "Idade",
                          "Sexo", "Altura", "Peso", "Saldo",
                          "Sair", NULL);

    Menu_T m8 = Menu_ctor(MENU_EDIT_ACT,
                          "Nome", "Data", "Duracao", "Custo", "Max. Vagas",
                          "Sair", NULL);
    
    Menu_T m9 = Menu_ctor(MENU_EDIT_PACK,
                          "Nome", "Duracao", "Custo",
                          "Sair", NULL);
    
    Menu_T m10 = Menu_ctor(MENU_ACTIV,
                          "Minhas", "Todas",
                          "Reservar", "Cancelar Reserva",
                          "Sair", NULL);
    
    Menu_T m11 = Menu_ctor(MENU_SEARCH_ACT,
                          "Por Nome", "Por Data", "Sair", NULL);

/* Insert menus in list */
    List_insert_ascend(&menus, m0, true, false, NULL);
    List_insert_ascend(&menus, m1, true, false, NULL);
    List_insert_ascend(&menus, m2, true, false, NULL);
    List_insert_ascend(&menus, m3, true, false, NULL);
    List_insert_ascend(&menus, m4, true, false, NULL);
    List_insert_ascend(&menus, m5, true, false, NULL);
    List_insert_ascend(&menus, m6, true, false, NULL);
    List_insert_ascend(&menus, m7, true, false, NULL);
    List_insert_ascend(&menus, m8, true, false, NULL);
    List_insert_ascend(&menus, m9, true, false, NULL);
    List_insert_ascend(&menus, m10, true,false, NULL);
    List_insert_ascend(&menus, m11, true, false, NULL);
   
    return menus;
}

/**
 * @brief Constructs memory for an App's instance
 * @return construct memory for App
 *
 * It initializes default members for App's instance.
 */
static App_T App_ctor()
{
    App_T app = App_new();
   /* Initialize memory */
    app->menus = App_create_menus();
    app->users = NULL;
    app->activities = NULL;
    app->packs = NULL;
    app->cur_user = NULL;
    app->prev_state = S_Logout; // previous state
    app->state = S_Login;
    app->userdata = NULL;
    app->db_user = Database_ctor(DATABASE_USERS);
    app->db_act = Database_ctor(DATABASE_ACTIVITIES);
    app->db_pack = Database_ctor(DATABASE_PACKS);

    return app;
}

/**
 * @brief Loads users from the database to the application
 * @param db: a constructed database
 * @return A list of users to be owned by App
 *
 * If the database exists, it loads the users. Otherwise, it prompts to create
 * the manager user. It can only be one Manager in the current paradigm, so this
 * is enforced by design.
 */
static List_T App_load_users(Database_T db)
{
    List_T users = List_ctor((void *)user_ctor,
                             (void *)user_cmp_username, 
                             (void *)user_dtor, 
                             (void *)user_print_info);

    User_T user1 = NULL, user2 = NULL, user3=NULL;

// read/update: Open a file for update (both for input 
// and output). The file must exist.
/* Unpack pack objects from file and load them to list */
    if( Database_open(db, "rb") )
        while( (user1 = App_deserialize(db, (void *)user_deserialize) ) != NULL)
        {
            user_print_line(user1);
            List_insert_ascend(&users, user1, true, false, NULL);
            print_msg_wait("Hold\n", -1);
        }
    else
    { /* first execution */
/* Construct Gerente, Func and Cliente */
    user1 = user_ctor(Gerente);
//    user2 = user_ctor(Func);
//    user3 = user_ctor(Cliente);
/* Create Gerente, Func and Cliente */
    user_create(user1);
    user_create(user2);
    user_create(user3);
/* Insert in the list Gerente, Func and Cliente */
    List_insert_ascend(&users, user1, true, false, NULL);
    List_insert_ascend(&users, user2, true, false, NULL);
    List_insert_ascend(&users, user3, true, false, NULL);
/* Serialize and push to file */
//    App_serialize(db, (void *)user_serialize, user1);
    user_print_line(user2);
    App_serialize(db, (void *)user_serialize, user2);
//    App_serialize(db, (void *)user_serialize, user3);

//    List_print_all(users, (void *)user_print_line, true, table_header_user);

    }
    return users;
}

/**
 * @brief Loads activities from the database to the application
 * @param db: a constructed database
 * @return A list of activities to be owned by App
 *
 * If the database exists, it loads the activities. Otherwise, it prompts to 
 * create an activity (purely as an example - this is not required for 
 * application operation).
 */
static List_T App_load_schedule(Database_T db)
{
    List_T activities = List_ctor((void *)activity_ctor,
                                  (void *)activity_cmp_time, 
                                  (void *)activity_dtor, 
                                  (void *)activity_print);
    Act_T act = NULL;
/* Unpack activity objects from file and load them to list */
    while( (act = App_deserialize(db, (void *)activity_deserialize) ) != NULL)
        List_insert_ascend(&activities, act, true, false, NULL);
#ifdef DEBUG
    if( !Database_open(db, "r+b") )
    {
/* Construct pack */
        printf("\n------------ Criando base de dados Activity ----------- \n");
        act = activity_ctor();
        if( !activity_create(act) )
            print_msg_wait("Insercao abortada!", 1);
        else
        {
/* Add to list */
            activity_print_line(act);
            print_msg_wait("Prima qq tecla para continuar", -1);
            List_insert_ascend(&activities, act, true, false, NULL);
/* Serialize and push to file */
            App_serialize(db, (void *)activity_serialize, act);
        }
    }
#endif
    return activities; 
}

/**
 * @brief Loads Packs from the database to the application
 * @param db: a constructed database
 * @return A list of Packs to be owned by App
 *
 * If the database exists, it loads the Packs. Otherwise, returns NULL.
 */
static List_T App_load_packs(Database_T db)
{
    List_T packs = List_ctor((void *)pack_ctor,
                             (void *)pack_cmp_name, 
                             (void *)pack_dtor, 
                             (void *)pack_print);
    Pack_T pack = NULL;
/* Unpack pack objects from file and load them to list */
    while( (pack = App_deserialize(db, (void *)pack_deserialize) ) != NULL)
        List_insert_ascend(&packs , pack, true, false, NULL);
#ifdef TEST_PACK
    if( !Database_open(db, "r+b") )
    {
/* Construct pack */
        printf("\n------------ Criando base de dados Pack ----------- \n");
        pack = pack_ctor();
        if( !pack_create(pack) )
            print_msg_wait("Insercao abortada!", 1);
        else
        {
/* Serialize and push to file */
            List_insert_ascend(&packs , pack, true, false, NULL);
            App_serialize(db, (void *)pack_serialize, pack);
        }
    }
#endif
    return packs; 
}

/**
 * @brief Checks if the user's credentials match in the database
 * @param app: valid app instance
 * @param user: user with a name set
 * @return user in database if password matches; else NULL
 *
 * It uses a user's with a name set to search the database. 
 * If found, it compares the passwords.
 */
static User_T App_validate_user(App_T app, User_T user)
{
/* Search user in database (by username)*/
    User_T user_db = List_search( app->users , user, NULL);
    if(user_db)
    {
        /* Check password */
        if( strcmp(user_get_pass(user_db), user_get_pass(user)) )
            return NULL; // different password
    }

   return user_db;
}

/**
 * @brief Menu to search the provided =activities= list by name or by time.
 * @param app: valid app instance
 * @param activities: valid list of activities
 * @param activity: activity found
 * @return true if found; false otherwise
 */
static bool App_search_Act(const App_T app, const List_T activities,
                           Act_T *activity)
{
    if(!activity || !app || !activities)
        return false;

    int resp;
    Menu_T menu = Menu_ctor(MENU_SEARCH_ACT, NULL);
    Act_T act = activity_ctor();
/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);

/* Process menu and obtain it's integer value */
    resp = Menu_process(menu) - '0';

    switch(resp)
    {
    case 0: // search by name
        activity_set_nome(act);
        *activity = List_search(activities, act,
                               (void *)activity_cmp_name);
        activity_dtor(act);
        break;
    case 1: // search by name
        activity_set_time(act);
        *activity = List_search(activities, act,
                               (void *)activity_cmp_time);
        activity_dtor(act);
        break;
    default: // Sair
        return false;
    }

    return true;
}

/**
 * @brief App's *Login* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The Login state is the initial state of the application, where each user
 * initiates its session. It's responsible for validating the user or exiting
 * the application.
 */
static enum App_state App_Login(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_LOGIN, NULL);
    User_T user = user_ctor(Cliente);

/* Define previous state */
    enum App_state prev_state = S_Logout;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* User chose to quit the application */
    if(resp == '1')
        return prev_state;

/* Else, prompt for username and pass */
    printf("\n------------- Credenciais ------------\n");
    user_set_username(user);
    user_set_pass(user);
    printf("---------------------------------------\n");
    
/* Validate user */
    user = App_validate_user(app, user);
/* Decide next state */    
    if(!user)
    {
        print_msg_wait("Invalid user!", 1);
        return app->state;
    }

/* User valid: Define current user */
    app->cur_user = user;
    print_msg_wait("Valid user!", 1);

    switch( user_get_type(user)  )
    {
    case Gerente:
        return S_Gerente;
    case Func:
        return S_Func;
    default:
        return S_Cliente;
    }
}

/**
 * @brief App's *Manager*(Gerente) state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Manager state if the validated user is the 
 * *Manager*. It allows Manager to manage Employees.
 */
static enum App_state App_Gerente(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_GERENTE, NULL);
    User_T func = NULL; 
    List_T funcs = NULL;

/* Define previous state */
    enum App_state prev_state = S_Logout;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* Return to Login */
    if(resp == '5')
        return prev_state;

/* Retrieve list of employees */
    func = user_ctor(Func);
    funcs = List_search_all(app->users, func, (void *)user_cmp_type);

/* Print all employes */
    if(resp == '4') 
    {
        printf("\n-------------- Funcionarios -----------------\n");
        List_print_all(funcs, (void *)user_print_line, true, table_header_user);
        printf("---------------------------------------------\n");
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        return app->state; // return to this state
    }

/* Add user */
    if(resp == '0')
    {
        user_create(func);
        List_insert_ascend(&(app->users), func, true, false, NULL);
        List_print_elem(app->users, func, NULL);
        print_msg_wait("Funcionario inserido", 1);
        return app->state; // return to this state
    }

/* Search for an employee by username: 
 * - Edit, List and Remove require a valid user */
    printf("\n-------------- Procurar Utilizador -----------------\n");
    user_set_username(func);
    func = List_search(funcs, func, (void *)user_cmp_username);
    printf("\n----------------------------------------------------\n");
    if(!func)
    {
        print_msg_wait("Utilizador nao encontrado!", 1);
        return app->state; // return to this state
    }
    print_msg_wait("Utilizador encontrado!", 1);

/* Valid employee - Continue */
    switch(resp)
    {
    case '1': // Edit Func
/* Store the employee found before jumping */
        app->userdata = func; 
        return S_Edit_User;
    case '2': // List Func
        printf("\n-------------- Utilizador -----------------\n");
        List_print_elem(app->users, func, NULL);
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        printf("-------------------------------------------\n");
        break;
    default: // remove func
        List_remove( &(app->users), func );
        print_msg_wait("Utilizador removido!", 1);
        break;
    }
    return app->state; // return to this state
}

/**
 * @brief App's *Func*(Employee) state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Employee state if the validated user is the 
 * *Employee*. It allows Employee to manage Clients, Activities and Packs.
 */
static enum App_state App_Func(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_FUNC, NULL);
    
/* Define previous state */
    enum App_state prev_state = S_Logout;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* User chose to logout */
    if(resp == '3')
        return prev_state;

    switch(resp)
    {
    case '0':
        return S_Manage_Cli;
    case '1':
        return S_Manage_Act;
    case '2':
        return S_Manage_Pack;
    }
    return app->state;
}

/**
 * @brief App's *Client* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Client state if the validated user is the 
 * *Client*. It allows Client to manage its information, activities and packs,
 * and also charge its balance.
 */
static enum App_state App_Cliente(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_CLIENTE, NULL);

/* Define previous state */
    enum App_state prev_state = S_Logout;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* User chose to logout */
    if(resp == '4')
        return prev_state;

    switch(resp)
    {
    case '0': // Editar info
/* Store the client found before jumping */
        app->userdata = app->cur_user; 
        return S_Edit_User;
    case '1': // Carregar saldo
        /* Current balance */
        printf("\nSaldo = %f [EURO]\n", user_get_saldo(app->cur_user));
        /* Update balance */
        if( user_set_saldo(app->cur_user) )
        {
            printf("\nSaldo actualizado!\n");
            printf("Saldo = %f [EURO]\n", user_get_saldo(app->cur_user));
        }

        print_msg_wait("Prima qq tecla para voltar", 0);
        break;
    case '2': // Actividades
        return S_Activities;
    case '3': // Seleccionar pack
        break;
    }
    return app->state;
}

/**
 * @brief App's *Manage Client* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Manage Client state if the *Employee* chooses 
 * this option on state *S_Func*. Allows Employee to manage clients. 
 */
static enum App_state App_Manage_Cli(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_MANAGE_CLI, NULL);
    User_T cli = NULL; 
    List_T clis = NULL;
    
/* Define previous state */
    enum App_state prev_state = S_Func;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* Return to previous menu */
    if(resp == '5')
        return prev_state;

/* Retrieve list of employees */
    cli = user_ctor(Cliente);
    clis = List_search_all(app->users, cli, (void *)user_cmp_type);

/* Print all employees */
    if(resp == '4') 
    {
        printf("\n-------------- Clientes -----------------\n");
        List_print_all(clis,  (void *)user_print_line, true, table_header_user);
        printf("---------------------------------------------\n");
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        return app->state; // return to this state
    }

/* Add user */
    if(resp == '0')
    {
        if( !user_create(cli) )
        {
            print_msg_wait("Insercao abortada!", 1);
            return app->state; // return to this state
        }
        List_insert_ascend(&(app->users), cli, true, false, NULL);
        List_print_elem(app->users, cli, NULL);
        print_msg_wait("Cliente inserido", 1);
        return app->state; // return to this state
    }

/* Search for an employee by username: 
 * - Edit, List and Remove require a valid user */
    printf("\n-------------- Procurar Utilizador -----------------\n");
    user_set_username(cli);
    cli = List_search(clis, cli, (void *)user_cmp_username);
    printf("\n----------------------------------------------------\n");
    if(!cli)
    {
        print_msg_wait("Utilizador nao encontrado!", 1);
        return app->state; // return to this state
    }
    print_msg_wait("Utilizador encontrado!", 1);

/* Valid employee - Continue */
    switch(resp)
    {
    case '1': // Edit Cli
/* Store the client found before jumping */
        app->userdata = cli; 
        return S_Edit_User;
    case '2': // List Cli
        printf("\n-------------- Utilizador -----------------\n");
        List_print_elem(app->users, cli, NULL);
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        printf("-------------------------------------------\n");
        break;
    default: // remove cli
        List_remove( &(app->users), cli );
        print_msg_wait("Utilizador removido!", 1);
        break;
    }
    return app->state; // return to this state
}

/**
 * @brief App's *Manage Activity* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Manage Activity state if the *Employee* 
 * chooses this option on state *S_Func*. Allows Employee to manage activities. 
 */
static enum App_state App_Manage_Act(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_MANAGE_ACT, NULL);
    Act_T act = activity_ctor();

/* Define previous state */
    enum App_state prev_state = S_Func;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* Return to previous menu */
    if(resp == '5')
        return prev_state;

/* Print all activities */
    if(resp == '4') 
    {
        printf("\n-------------- Actividades -----------------\n");
        List_print_all(app->activities,  (void *)activity_print_line,
                       false, table_header_activity);
        printf("---------------------------------------------\n");
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        return app->state; // return to this state
    }

/* Add activity */
    if(resp == '0')
    {
        if( !activity_create(act) )
        {
            print_msg_wait("Insercao abortada!", 1);
            return app->state; // return to this state
        }
       /* Show creation resume */ 
        List_print_elem(app->activities, act, NULL);
        if( !List_insert_ascend(&(app->activities), act, true, false, NULL) )
            print_msg_wait("2 actividades no mesmo horario! PF insira novamente!", -1);
        else
            print_msg_wait("Actividade inserida!\n", -1);
        return app->state; // return to this state
    }

/* Search for an activity: 
 * - Edit, List and Remove require a valid activity */
    if( !App_search_Act(app, app->activities, &act))
        return app->state; // return to this state
    if(!act)
    {
        print_msg_wait("Actividade nao encontrada!", 1);
        return app->state; // return to this state
    }
    print_msg_wait("Actividade encontrada!", 1);

/* Valid Activity - Continue */
    switch(resp)
    {
    case '1': // Edit Act
/* Store the employee found before jumping */
        app->userdata = act; 
        return S_Edit_Act;
    case '2': // List Act
        printf("\n-------------- Actividade -----------------\n");
        List_print_elem(app->activities, act, NULL);
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        printf("---------------------------------------------\n");
        break;
    default: // remove Act
        List_remove( &(app->activities), act );
        print_msg_wait("Actividade removida!", 1);
        break;
    }
    return app->state; // return to this state
}

/**
 * @brief App's *Manage Pack* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Manage Pack state if the *Employee* 
 * chooses this option on state *S_Func*. Allows Employee to manage Packs. 
 */
static enum App_state App_Manage_Pack(App_T app)
{
    char resp;
    Menu_T menu = Menu_ctor(MENU_MANAGE_PACK, NULL);
    Pack_T pack = pack_ctor();

/* Define previous state */
    enum App_state prev_state = S_Func;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);
    resp = Menu_process(menu);

/* Return to previous menu */
    if(resp == '5')
        return prev_state;

/* Print all packs */
    if(resp == '4') 
    {
        printf("\n-------------- Packs -----------------\n");
        List_print_all(app->packs,  (void *)pack_print_line,
                       false, table_header_pack);
        printf("---------------------------------------------\n");
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        return app->state; // return to this state
    }

/* Add pack */
    if(resp == '0')
    {
        if( !pack_create(pack) )
        {
            print_msg_wait("Insercao abortada!", 1);
            return app->state; // return to this state
        }
            
        List_insert_ascend(&(app->packs), pack, true, false, NULL);
        List_print_elem(app->packs, pack, NULL);
        print_msg_wait("Pack inserido", 1);
        return app->state; // return to this state
    }

/* Search for a pack: 
 * - Edit, List and Remove require a valid activity */
    printf("\n-------------- Procurar Pack -----------------\n");
    pack_set_name(pack);
    pack = List_search(app->packs, pack, NULL);
    printf("\n----------------------------------------------\n");
    if(!pack)
    {
        print_msg_wait("Pack nao encontrado!", 1);
        return app->state; // return to this state
    }
    print_msg_wait("Pack encontrado!", 1);

/* Valid pack - Continue */
    switch(resp)
    {
    case '1': // Edit Pack
/* Store the employee found before jumping */
        app->userdata = pack; 
        return S_Edit_Pack;
    case '2': // List Pack
        printf("\n-------------- Pack -----------------\n");
        List_print_elem(app->packs, pack, NULL);
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        printf("---------------------------------------\n");
        break;
    default: // remove pack
        List_remove( &(app->packs), pack );
        print_msg_wait("Pack removido!", 1);
        break;
    }
    return app->state; // return to this state
}

/**
 * @brief App's *Edit User* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Edit User state if the *Manager, *Employee* 
 * or *Client* choose the respective option. Allows *users* to edit info with 
 * restricted privileges:
 * - Manager can edit Employee
 * - Employee can edit Client
 * - Client can edit its personal info
 */
static enum App_state App_Edit_User(App_T app)
{
    if(!app)
       return app->prev_state;
    
/* Retrieve user from generic data */
    User_T user = app->userdata;
    if(!user)
       return app->prev_state;

/* Clone user to prevent data corruption */
   /* Construct user by default with similar type */
    User_T clone = user_ctor(user_get_type(user));
    if( !user_clone(user, clone) )
    {
        print_msg_wait("Erro! PF tente outra vez!", 1);
        return app->state; // return to this state
    }

    int resp;
    Menu_T menu = Menu_ctor(MENU_EDIT_USER, NULL);
/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);

/* Process menu and obtain it's integer value */
    resp = Menu_process(menu) - '0';

    if(resp == 8) // Exit
    {
        return app->prev_state;
        user_dtor(clone); 
    }

/* Invoke 'callback' function */
    if( !user_call_set_fcn(clone, resp) )
    { // user chose to abort the edition
        print_msg_wait("Edicao abortada!", 1);
        user_dtor(clone); 
        return app->state; // return to this state
    }

/* If the username was updated, check for conflicts */
    if( resp == 0)
        if( List_search(app->users, clone, NULL) ) // already in list
        {
            print_msg_wait("Username ja existe! PF escolha outro!", 1);
            user_dtor(clone); 
            return app->state; // return to this state
        }

/* Copy back to original user */
    if( !user_clone(clone, user) )
    {
        print_msg_wait("Erro! PF tente outra vez!", 1);
        user_dtor(clone); 
        return app->state; // return to this state
    }

/* If the username (sort key) was update, sort the list */
    if( resp == 0)
        List_sort( &app->users, NULL);

/* User was updated; set dirty flag of list */
    List_set_dirty( app->users, true);
        
    //user_dtor(clone);
    //menu_dtor(menu);
    print_msg_wait("Dados editados", 1);
    user_dtor(clone); 
    return app->state; // return to this state
}

/**
 * @brief App's *Edit Activity* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Edit Activity state if the *Employee* 
 * chooses this option on state *Manage Activity*. Allows Employee to edit 
 * activity info. 
 */
static enum App_state App_Edit_Act(App_T app)
{
    if(!app)
       return app->prev_state;

/* Retrieve activity from generic data */
    Act_T activity = app->userdata;
    if(!activity)
       return app->prev_state;

/* Clone user to prevent data corruption */
   /* Construct user by default with similar type */
    Act_T clone = activity_ctor();
    if( !activity_clone(activity, clone) )
    {
        print_msg_wait("Erro! PF tente outra vez!", 1);
        return app->state; // return to this state
    }

    int resp;
    Menu_T menu = Menu_ctor(MENU_EDIT_ACT, NULL);
/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);

/* Process menu and obtain it's integer value */
    resp = Menu_process(menu) - '0';

/* Return to previous menu */
    if(resp == 5) // Exit
    {
        activity_dtor(clone); 
        return app->prev_state;
    }

/* Invoke 'callback' function */
    if( !activity_call_set_fcn(clone, resp) )
    { // user chose to abort the edition
        print_msg_wait("Edicao abortada!", 1);
        activity_dtor(clone); 
        return app->state; // return to this state
    }

/* If the username was updated, check for conflicts */
    if( resp == 1)
        if( List_search(app->activities, clone, (void *)activity_cmp_time) ) // already in list
        {
            print_msg_wait("Actividade ja existe neste horario! PF escolha outro!", 1);
            activity_dtor(clone); 
            return app->state; // return to this state
        }

/* Copy back to original user */
    if( !activity_clone(clone, activity) )
    {
        print_msg_wait("Erro! PF tente outra vez!", 1);
        activity_dtor(clone); 
        return app->state; // return to this state
    }

/* If the time (sort key) was update, sort the list */
    if( resp == 1)
        List_sort( &app->activities, NULL);
        
/* Activity was updated; set dirty flag of list */
    List_set_dirty( app->activities, true);
    //user_dtor(clone);
    //menu_dtor(menu);
    print_msg_wait("Dados editados", 1);
    activity_dtor(clone); 
    return app->state; // return to this state
}

/**
 * @brief App's *Edit Pack* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Edit Pack state if the *Employee* 
 * chooses this option on state *Manage Pack*. Allows Employee to edit packs 
 * info. 
 */
static enum App_state App_Edit_Pack(App_T app)
{
    if(!app)
       return app->prev_state;

/* Retrieve pack from generic data */
    Pack_T pack = app->userdata;
    if(!pack)
       return app->prev_state;

/* Clone user to prevent data corruption */
   /* Construct user by default with similar type */
    Pack_T clone = pack_ctor();
    if( !pack_clone(pack, clone) )
    {
        print_msg_wait("Erro! PF tente outra vez!", 1);
        return app->state; // return to this state
    }

    int resp;
    Menu_T menu = Menu_ctor(MENU_EDIT_PACK, NULL);
/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);

/* Process menu and obtain it's integer value */
    resp = Menu_process(menu) - '0';

/* Return to previous menu */
    if(resp == 3) // Exit
        return app->prev_state;

/* Invoke 'callback' function */
    if( !pack_call_set_fcn(clone, resp) )
    { // user chose to abort the edition
        print_msg_wait("Edicao abortada!", 1);
        return app->state; // return to this state
    }

/* If the username was update, check for conflicts */
    if( resp == 0)
        if( List_search(app->packs, clone, NULL) ) // already in list
        {
            print_msg_wait("Pack com este nome ja existe! PF escolha outro!", 1);
            return app->state; // return to this state
        }

/* Copy back to original user */
    if( !pack_clone(clone, pack) )
    {
        print_msg_wait("Erro! PF tente outra vez!", 1);
        return app->state; // return to this state
    }

/* If the username (sort key) was update, sort the list */
    if( resp == 0)
        List_sort( &app->packs, NULL);
        
/* Pack was updated; set dirty flag of list */
    List_set_dirty( app->packs, true);

    //user_dtor(clone);
    //menu_dtor(menu);
    print_msg_wait("Dados editados", 1);
    return app->state; // return to this state
}

/**
 * @brief App's *Client's activities* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The application evolves to the Client Activities state if the *Client* 
 * chooses this option on state *Client*. Allows clients to view activities
 * (signed in or all) and make/cancel a reservation in an activity.
 */
static enum App_state App_Cli_Activities(App_T app)
{
    if( !app )
        return app->prev_state;

    int resp;
    Menu_T menu = Menu_ctor(MENU_ACTIV, NULL);
    Act_T act = activity_ctor();

/* Define previous state */
    enum App_state prev_state = S_Cliente;
    app->prev_state = app->state;

/* Search menu to process */
    menu = List_search( app->menus , menu, NULL);

/* Process menu and obtain it's integer value */
    resp = Menu_process(menu) - '0';

/* Return to previous menu */
    if(resp == 4)
        return prev_state;
    
    switch(resp)
    {
    case 0: // Mine
        printf("\n-------------- Minhas -----------------\n");
        user_list_activities(app->cur_user);
        printf("-----------------------------------------\n");
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        break;
    case 1: // All
        printf("\n-------------- Todas -----------------\n");
        List_print_all(app->activities,  (void *)activity_print_line,
                       false, table_header_activity);
        printf("----------------------------------------\n");
        print_msg_wait("\nPrima qq tecla para continuar", -1);
        break;
    case 2: // Do reservation (in All)
/* Search for an activity in app->activities */
        if( !App_search_Act(app, app->activities, &act))
            break;
        if(!act)
        {
            print_msg_wait("Actividade nao encontrada!", 1);
            return app->state; // return to this state
        }
/* Valid activity */
    /* Check balance */
        if(user_get_saldo(app->cur_user) < activity_get_custo(act) )
        {
            print_msg_wait("Saldo insuficiente!", 1);
            return app->state; // return to this state
        }
        /* Discount activity cost from saldo */
        user_pay(app->cur_user, -activity_get_custo(act));
        /* Add activity to user's activity */
        user_add_activity(app->cur_user, act);
        /* Add user to activity's user */
        activity_add_user(act, app->cur_user);
        break;
    case 3: // Cancel reservation (in Mine)
/* Search for an activity in user->activities */
        if( !App_search_Act(app, user_get_activities(app->cur_user), &act))
            break;
        if(!act)
        {
            print_msg_wait("Actividade nao encontrada!", 1);
            return app->state; // return to this state
        }
    /* Valid activity */
        /* Update saldo */
        user_pay(app->cur_user, activity_get_custo(act));
        /* Remove activity from user's activity */
        user_remove_activity(app->cur_user, act);
        /* Remove user from activity's user */
        activity_remove_user(act, app->cur_user);
        break;
    }
    return app->state; // return to this state
}

//static enum App_state App_Logout(App_T app)
//{
//    char *input = NULL;
//    int val = 0;
///* Check if there unsaved modifications */
//    bool unsaved = false;
//(List_isDirty(app->users) || 
//                    List_isDirty(app->activities) || 
//                    List_isDirty(app->packs));
//    putchar('\n');
//    // Enquanto o user nao inserir uma string valida
//    // obter input
//    do
//    {
//        input = get_input("Deseja realmente sair? (0: N; 1: S)"); 
//        if (input[0] == ABORT_INPUT) // check if user wants to abort
//            return B_FALSE; // invalid
//        val = validateBool(input);
//    } while (val == ERROR_INVALID);
//
//    if(!val) // return to previous menu
//        return app->prev_state;
//
///* Check if there are unsaved modifications */
//    unsaved = (List_isDirty(app->users) || 
//               List_isDirty(app->activities) || 
//               List_isDirty(app->packs));
//
///* If unsaved, prompt for save changes */
//    if(unsaved)
//    {
//        do
//        {
//            input = get_input("Modificacoes nao guardadas. Deseja guarda-las? (0: N; 1: S)"); 
//            if (input[0] == ABORT_INPUT) // check if user wants to abort
//                return B_FALSE; // invalid
//            val = validateBool(input);
//        } while (val == ERROR_INVALID);
//
//        if(val)
//        { /* Save changes */
//            printf("---------- Salvaguardando bases de dados -------- ");
///* Save to files */
//            print_msg_wait("Bases de dados salvaguardadas!", 1);
//        }
//        else
//        { /* Reload databases from file if required */
//           if(List_isDirty(app->users))
//              printf("App users Dirty! Reloading\n");
//           if(List_isDirty(app->activities))
//              printf("App activities Dirty! Reloading\n");
//           if(List_isDirty(app->packs))
//              printf("App packs Dirty! Reloading\n");
//        }
//
//    }
//
///* Clear all dirty flags independent of the state */
//    List_set_dirty(app->users, false);
//    List_set_dirty(app->activities, false);
//    List_set_dirty(app->packs, false);
//
//
//    print_msg_wait("Sessao terminada!", 1);
//
//    if(app->prev_state == S_Login)
//        return S_Quit; // Quit program
//    else
//        return S_Login; // return to initial menu
//}

/**
 * @brief App's *Logout* state
 * @param app: valid app instance
 * @return next state of the application
 * 
 * The Logout state is the where each user decides to terminate, or not, its 
 * session.
 * - If logout is canceled: returns to previous state
 * - If logout is done: goes to Login state
 */
static enum App_state App_Logout(App_T app)
{
    char *input = NULL;
    int val = 0;
    putchar('\n');
    // Enquanto o user nao inserir uma string valida
    // obter input
    do
    {
        input = get_input("Deseja realmente sair? (0: N; 1: S)"); 
        if (input[0] == ABORT_INPUT) // check if user wants to abort
            return B_FALSE; // invalid
        val = validateBool(input);
    } while (val == ERROR_INVALID);

    if(!val) // return to previous menu
        return app->prev_state;

    if(app->prev_state == S_Login)
        return S_Quit; // Quit program

    print_msg_wait("Sessao terminada!", 1);
    return S_Login; // return to initial menu
}

/**
 * @brief Saves the database
 * @param db: a constructed database
 * @param list: a constructed list to save
 * @param serialize: pointer to generic function capable of serializing 
 * the specific data of the database
 * @param print: pointer to generic function to debug info
 *
 * Used to save users, activities and packs to the database.
 * *serialize* functions must be implemented by clients.
 * *print* functions must be implemented by clients.
 * @see User.h
 * @see Activity.h
 * @see Pack.h
 */
static void App_save_database(Database_T db, List_T list, 
                              void * (*serialize)(Fifo_T fifo),
                              void(*print)(void *data))
{
    void *data;
    if(List_isDirty(list))
    {
        /* Reopen database and rewind list */
        Database_close(db);
        Database_open(db, "wb");
        List_rewind(list);
        /* While an user exists */
        while ( (data = List_pop(list)) != NULL )
        {
            /* Serialize object to file and remove from the list */
            App_serialize(db, serialize, data);
            List_remove(&list, data);
            List_rewind(list);
            if(print)
            {
                print(data);
                print_msg_wait("Wait\n", -1); 
            }
        }
        
    }

}

/**
 * @brief App's function pointers used for Finite State Machine control
 */
typedef enum App_state(*App_func)(App_T );
/**
 * @brief Array of App's function pointers (used for FSM control)
 */
static App_func App_state_functions[] = {
    &App_Login,
    &App_Gerente,
    &App_Func,
    &App_Cliente,
    &App_Manage_Cli,
    &App_Manage_Act,
    &App_Manage_Pack,
    &App_Edit_User,
    &App_Edit_Act,
    &App_Edit_Pack,
    &App_Cli_Activities,
    &App_Logout,
    NULL};

App_T App_init()
{
/* Construct app's memory */
    App_T app = App_ctor();

/* Load users */
    app->users = App_load_users(app->db_user);

/* Load schedule */
    app->activities = App_load_schedule(app->db_act);

/* Load packs */
    app->packs = App_load_packs(app->db_pack);
    
    return app;
}

int App_exec(App_T app)
{
    while(1)
    {
        app->state = App_state_functions[app->state](app);
        if(app->state == S_Quit)
            break;
    }

/* Exitted */
    /* Saving databases */
    App_save_database(app->db_user, app->users, (void *)user_serialize, 
                      /*(void *)user_print_info*/ NULL);
    App_save_database(app->db_act, app->activities, 
                      (void *)activity_serialize, NULL);
    App_save_database(app->db_pack, app->packs, (void *)pack_serialize, NULL);
        
    /* Exitted -> print goodbye */
    print_msg_wait("Terminando aplicacao...", 1);

    return 0; 
}
