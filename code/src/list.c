/**
 * @file list.c
 * @author Jose Pires
 * @date 15 Jan 2019
 *
 * @brief List's module implementation
 */

#include "list.h"
#include <stdlib.h> // malloc
#include <assert.h> // malloc
#include "m-utils.h" // for print_header

#include <stdio.h> // printf
//#define DEBUG

/* Node          Node              Object
||||||||||    ||||||||||       |||||||||||||
|  data  |    |  data  | ----> |   data    |
|--------|    |--------|       |-----------| 
|  next  | <- |  next  |       | functions |
||||||||||    ||||||||||       |||||||||||||
*/

/**
 * @brief opaque pointer to struct Node_T. 
 * It hides the implementation details (allows modularity)
 */
typedef struct Node_T *Node_T;

/**
 * @brief structure Node: atomic unit that composes the list
 * The list is doubly-linked, i.e., with pointers to prev and next elements
 * and with a pointer to generic data.
 */
struct Node_T{
    void *data; /**< generic data */
    Node_T prev; /**< pointer to previous node */
    Node_T next; /**< pointer to next node */
};

/*          Node          Node
first -> ||||||||||    |||||||||| <- last
         |  data  |    |  data  |
         |--------|    |--------|
 NULL <- |  prev  | <- |  prev  | 
         |  next  | -> |  next  | -> NULL (end of list)
         ||||||||||    ||||||||||
*/

/**
 * @brief List's structure: generic container for doubly linked list
 *
 * detailed
 */
struct List_T{
    Node_T first; /**< Pointer to first node of the list */
    Node_T last; /**< Pointer to last node in the list */
    Node_T it; /**< Internal iterator */
    unsigned count; /**< nr of elements in the list */
    bool dirty; /**< flags that an update was made or is required */
    /* function pointers for comparing and destructor */
    void* (*Data_ctor)(void); /**< pointer to Data constructor function */
    int (*Data_cmp)(const void *data1, const void *data2); /**< pointer to Data compare function */
    void (*Data_dtor)(void *data); /**< pointer to Data destructor function */
    void (*Data_print)(const void *data); /**< pointer to data print function */
};

/**
 * @brief Allocates memory for an Node's instance
 * @return initialized memory for Node
 *
 * It is checked by assert to determine if memory was allocated.
 * If assertion is valid, returns a valid memory address
 */
static Node_T node_new()
{
    Node_T node = malloc(sizeof(struct Node_T));
    assert(node);
#ifdef DEBUG
    printf("Node_new: %p\n", node);
#endif
    return node;
}

/**
 * @brief Destructs an node
 * @param node: a valid node
 */
static void node_delete(Node_T node)
{
    // assumes data from node was already deallocated
//    if( !node->data )
//        return; // exit immediately
    if(node)
        free(node);
}

/**
 * @brief Destructs an node
 * @param node: a valid node
 */
static void Node_print(const Node_T node)
{
    if(!node)
        return;
#ifdef DEBUG
    printf("|---- Node: %p -----|\n", node);
    printf("| data = %p\t\n", node->data); 
    printf("| prev = %p\t\n", node->prev); 
    printf("| next = %p\t\n", node->next); 
    printf("|-------------------------------|\n");
#endif
}

/**
 * @brief Checks if list is empty
 * @param self: a valid list
 * @return true, if empty; false, otherwise
 */
static bool List_isEmpty(const List_T self)
{
    return (self->first == NULL);
}

//static bool match_data(Node_T n1, Node_T n2)
//{
//    return (n1->data == n2->data);
//}

//static void * List_get_node_match_data(List_T self, const void *elem)
//{
//   Node_T tmp, node;
//   node = node_new();
//   node->data = (void *)elem;
//   tmp = self->first;
//
//   while(tmp)
//   {
//       // match found
//       if(tmp->data == node->data)
//           return tmp;
//
//       tmp = tmp->next;
//   }
//   return NULL; // no element found
//}

List_T List_ctor( void *(*Data_ctor)(void),
                  int (*Data_cmp)(const void *data1, const void *data2),
                  void (*Data_dtor)(void *data),
                  void (*Data_print)(const void *data))
{
    List_T list = malloc(sizeof(struct List_T));
    assert(list);
    list->Data_ctor = Data_ctor;
    list->Data_cmp = Data_cmp;
    list->Data_dtor = Data_dtor;
    list->Data_print = Data_print;
    
    list->first = list->last = list->it = NULL;
    list->count = 0;
    list->dirty = false;
    return list;
}

void List_dtor(List_T self)
{
    if(self)
        free(self);
}

/* Debug version */
//void * List_insert_ascend(List_T *self, const void *elem, const bool order,
//                          int(*cmp)(const void *data1, const void *data2))
//{
//    if( !(*self) ) // empty list? -> return
//        return NULL;
//
//    if(!elem) // empty elem -> return
//        return NULL;
//
//    Node_T node, *it = NULL;
//    int cmp_val = 0, level = (order) ? 1 : -1;
//
//    // Assign compare function
//   if(cmp == NULL)
//       cmp = (*self)->Data_cmp;
//  
//   /* Create new node with the elem data */
//    node = node_new();
//    node->data =(void *) elem;
//    node->prev = node->next = NULL;
//
//    /* Initialize iterator to 1st elem of the list */
//    it = &( (*self)->first );
//
//    // Increment list size
//    (*self)->count++;
//    
//    if( List_isEmpty(*self) ) // insert at head
//    {
//#ifdef DEBUG
//    printf("List is empty! Insert at head\n");
//#endif
//       (*self)->first = (*self)->last = node;
//       return node->data; 
//    }
//    else
//    {
//#ifdef DEBUG
//        printf("List is NOT empty!\n");
//#endif
//        while(1)
//        {
//            // cmp_val = 0 -> equals
//            // cmp_val > 0 -> node > iter
//            // cmp_val < 0 -> node < iter
//            cmp_val = cmp(node->data, (*it)->data);
//            cmp_val *= level;
//
//#ifdef DEBUG
//             printf("---------- Prev: \n");
//             if( (*it)->prev)
//                 (*self)->Data_print( (*it)->prev->data); 
//             printf("---------- Cur: \n");
//             (*self)->Data_print((*it)->data);
//             printf("---------- Next: \n");
//             if((*it)->next)
//                 (*self)->Data_print( (*it)->next->data); 
//#endif
//
//            /* Repeated element */
//            if(!cmp_val)
//            {
//                (*self)->count--; // revert list size
//                return NULL; // equal elems
//            }
//
//            /* Insert before */
//            if(cmp_val < 0)
//            {
//#ifdef DEBUG
//                printf("Insert before\n");
//#endif
//
//                /* Update middle links */
//                node->next = *it;
//                node->prev = (*it)->prev;
//                (*it)->prev = node;
//                
//                if(!node->prev)  // first link inserted (redo head)
//                {
//#ifdef DEBUG
//                    printf("Insert head\n");
//#endif
//                   (*self)->first = node; 
//                }
//                else
//                    (*it)->prev->prev->next = node;
//
//                return node->data;
//            }
//            // Reached the end of list; break
//            if(! (*it)->next)
//              break;
//
//            // Iterate
//            it = &((*it)->next); 
//        }
//    }
//    // Reached the end of the list (insert at tail)
//#ifdef DEBUG
//    printf("Insert at tail\n");
//#endif
//    node->prev = *it;
//    (*it)->next = node;
//    (*it)->next->prev = node->prev;
//    (*self)->last = node; 
//
//    return node->data;
//}

void * List_insert_ascend(List_T *self, const void *elem, 
                          const bool order, const bool allowDups,
                          int(*cmp)(const void *data1, const void *data2))
{
    if( !(*self) ) // empty list? -> return
        return NULL;

    if(!elem) // empty elem -> return
        return NULL;

    Node_T node, *it = NULL;
    int cmp_val = 0, level = (order) ? 1 : -1;

    // Assign compare function
   if(cmp == NULL)
       cmp = (*self)->Data_cmp;
  
   /* Create new node with the elem data */
    node = node_new();
    node->data =(void *) elem;
    node->prev = node->next = NULL;

    /* Initialize iterator to 1st elem of the list */
    it = &( (*self)->first );

    // Increment list size
    (*self)->count++;
    
    if( List_isEmpty(*self) ) // FIRST: insert at head
    {
       (*self)->first = (*self)->last = node;
       (*self)->dirty = true; // an item was added
       return node->data; 
    }
    //else
    while(1)
    {   // cmp_val = 0 -> equals
        // cmp_val > 0 -> node > iter
        // cmp_val < 0 -> node < iter
        cmp_val = cmp(node->data, (*it)->data);
        cmp_val *= level;

        /* Repeated element */
        if(!cmp_val)
        {
            (*self)->count--; // revert list size
            /* No dups allowed */
            if(!allowDups)
                return NULL; // equal elems
            /* update current element */
            (*it)->data = node->data;
            return node->data;
        }

        /* Insert before */
        if(cmp_val < 0)
        {
            /* Update middle links */
            node->next = *it;
            node->prev = (*it)->prev;
            (*it)->prev = node;
                
            if(!node->prev)  // first link inserted (redo head)
                (*self)->first = node; 
            else // middle node inserted (redo prev->next)
                (*it)->prev->prev->next = node;

            (*self)->dirty = true; // an item was added
            return node->data;
        }
        // Reached the end of list; break
        if(! (*it)->next)
            break;

        // Iterate
        it = &((*it)->next); 
    }
    
/* TAIL: Reached the end of the list (insert at tail) */
//    node->prev = *it;
    (*it)->next = node;
    (*it)->next->prev = *it;
    (*self)->last = node; 

    (*self)->dirty = true; // an item was added
    return node->data;
}

void * List_search(const List_T self, const void *elem, 
                   int(*cmp)(const void *data1, const void *data2))
{
    if(!self || !elem)
        return NULL;
    
    /* Empty list, return immediately */
    if( List_isEmpty(self) )
        return NULL;

   // Initialize iterator to 1st elem of the list
    Node_T it = self->first;
    int cmp_val;
    
   if(!cmp)
       cmp = self->Data_cmp;

   while(1)
   {
       cmp_val = cmp( elem, it->data ) ;
/* Match found */
       if(!cmp_val) 
           return (it->data);
/* Array is sorted; exceeded rank */
       if(cmp_val < 0) 
           return NULL;
/* Break */
       if(! it->next)
           break;
/* Iterate */
       it = it->next;
   }
  return NULL; 
}

List_T List_search_all(const List_T self, const void *elem,
                       int(*cmp)(const void *data1, const void *data2))
{
    if(!self || !elem)
        return NULL;

    if(!self->Data_ctor || !self->Data_dtor ||
       !self->Data_cmp || !self->Data_print)
        return NULL;

    /* Empty list, return immediately */
    if( List_isEmpty(self) )
        return NULL;

/* Construct a new list */
    List_T dest = List_ctor(self->Data_ctor, self->Data_cmp,
                            self->Data_dtor, self->Data_print);

/* Update compare function */
    if( cmp )
        dest->Data_cmp = cmp;

    void *data; // elem found in the list

/* Store initial position of the original list */
    Node_T it, head;
    it = head = self->first;
    
/* Search similar elems in original list and insert them in destination list*/
    while(it)
    {
        /* Update head for search */
        self->first = it;

        /* Search elem in the original list with the new compare function */
        data = List_search(self, elem, cmp);

//#ifdef DEBUG
//        printf("\n------- Search all------\n");
//        List_print_elem(self, it->data);
//        List_print_elem(self, data);
//        getchar();
//#endif

        /* Insert the elem in the dest list with the default cmp function */
        List_insert_ascend( &dest, data, true, false, self->Data_cmp);

        /* Iterate */
        it = it->next;
    }

    /* Restore original list head */
    self->first = head;

/* Return list */
    return dest;
}

bool List_remove(List_T *self, const void *elem)
{
    if( !(*self) ) // empty list? -> return
        return false;
    if( !elem )
        return false; // invalid object passed
    if(!(*self)->Data_dtor)
        return false;

   // Initialize iterator to 1st elem of the list
    Node_T *it = &( (*self)->first ); 

    /* Empty list, return immediately */
    if( List_isEmpty(*self) )
    {
       (*self)->first = (*self)->last = NULL;
       return false; 
    }

   while(1)
   {
#ifdef DEBUG
       printf("\nElem: %p\n", elem);
       Node_print(*it);
       (*self)->Data_print((*it)->data);
       getchar();
#endif
       // data match
       if( elem == (*it)->data )
       {/* redo links */
           if( (*it)->next )
               (*it)->next->prev = (*it)->prev;
           else // redo tail
               (*self)->last = (*it)->prev;
           if( (*it)->prev )
               (*it)->prev->next = (*it)->next;
           else // redo head
               (*self)->first = (*it)->next;

           /* Destroy data, delete node and update count*/
           //(*self)->Data_dtor( (*it)->data );
//           node_delete(*it);
           (*self)->count--;

           /* empty List; destroy it */
           if(! (*self)->count)
           {
               (*self)->first = (*self)->last = NULL;
//               List_dtor( *self );
           }

           (*self)->dirty = true; // an item was removed
           return true;
       }
/* Break */
       if(! (*it)->next)
           break;

/* Iterate */
       it = &((*it)->next);
   }
  return false; 
}

void * List_pop(List_T self)
{
    /* Empty list, return immediately */
    if( List_isEmpty(self) )
        return NULL;

    Node_T it = self->it;
    if(self->it)
    {
/* Iterate */
       self->it = self->it->next;
       return it->data;
    }
  return NULL; 

}
void List_rewind(List_T self)
{
    self->it = self->first;
}

void List_sort(List_T *self, 
               int(*cmp)(const void *data1, const void *data2))
{
    if( !(*self) )
        return;

    /* Empty list, return immediately */
    if( List_isEmpty(*self) )
        return;

    // Assign compare function
   if( !cmp )
       cmp = (*self)->Data_cmp;

   Node_T prev, next, next_next, *it = NULL;
   int cmp_val;
   
#ifdef DEBUG
   printf("Sort - Self: %p", self);
   getchar();
#endif
   
   // Initialize iterator to 1st elem of the list
   it = &(*self)->first;

   while(1)
   {
/* Break */
       if(! (*it)->next)
           break;

       cmp_val = cmp((*it)->data, (*it)->next->data);
#ifdef DEBUG
       printf("cmp_val: %d\n", cmp_val);
       (*self)->Data_print((*it)->data);
       (*self)->Data_print((*it)->next->data);
       getchar();
#endif
       
       /* Sort */
       if(cmp_val > 0) // cur > next --> swap nodes
       {
/* Store links */
           prev = (*it)->prev;
           next = (*it);
           next_next = (*it)->next->next;
#ifdef DEBUG
           printf("Prev: %p\t Next: %p\n", prev, next);
#endif
/* Swap pointers */
           (*it) = (*it)->next;
#ifdef DEBUG
           printf("It: %p\n", (*it));
#endif
/* Update Links */
           (*it)->prev = prev;
           (*it)->next = next;
#ifdef DEBUG
           printf("Prev: %p\t Next: %p\n", (*it)->prev, (*it)->next);
#endif
           (*it)->next->next = next_next;
           (*it)->next->prev = (*it);
#ifdef DEBUG
           printf("Next->next: %p\t Next->prev: %p\n", (*it)->next->next, (*it)->next->prev);

#endif
           if( (*it)->prev )
               (*it)->prev->next = (*it);
           else // Update Head
               (*self)->first = (*it);
           if( !(*it)->next->next ) // update tail
               (*self)->last = (*it)->next;
       }

/* Iterate */
       it = &((*it)->next);
   }

#ifdef DEBUG
       printf("Sort: First & Last\n");
       (*self)->Data_print((*self)->first->data);
       (*self)->Data_print((*self)->last->data);
       getchar();
#endif
    
}

bool List_replace(List_T *self, const void *new_elem, const void *old_elem)
{
    if( !(*self) )
        return false;
    if( !new_elem || !old_elem )
        return false; // invalid object passed

    /* Empty list, return immediately */
    if( List_isEmpty(*self) )
        return false;
 

   // Initialize iterator to 1st elem of the list
    Node_T *it = &(*self)->first;

#ifdef DEBUG
   printf("Sort - Self: %p", self);
   getchar();
#endif

   while(1)
   {
       /* data ptrs are equal */
       if( old_elem == (*it)->data)
       {
           (*it)->data = (void *)new_elem;
           return true;
       }
/* Break */
       if(! (*it)->next)
           break;

/* Iterate */
       it = &((*it)->next);
   }
  return false; 
}

unsigned List_count(const List_T self)
{
   return self->count; 
}

void List_print_all(const List_T self,
                    void(*print)(const void *data), bool numbered,
                    const char *header)
{
    if(! self)
        return;

    if(!print)
        print = self->Data_print;
/* Version with const char ** */
    //if(header)
    //{
    //    while(*header)
    //        printf("%s, ", (*header)++);
    //    printf("\n");
    //}

    int i = 0;
   // Initialize iterator to 1st elem of the list
    Node_T it = self->first; 

/* Print size */
    printf("\n\t\tTotal de elementos: %u\n\n", self->count);
/* Print header */
    if(header)
        print_header(header);
    while(it)
    {
/* Print numbering */
        if(numbered)
            printf("%.2d, ", ++i);
/* Print data */
        print( it->data);
/* Iterate */
       it = it->next;
    }
}

void List_debug(const List_T self)
{
    if(!self)
        return;
    
   // Initialize iterator to 1st elem of the list
    Node_T it = self->first;

    // Print list initial data
    printf("!--------- List ---------!\n");
    printf("! head = %p\n", self->first);
    printf("! tail = %p\n", self->last);
    printf("! count = %d\n", self->count);
    printf("!------------------------!\n\n");

    /* Print nodes info */
    while(it)
    {
        Node_print(it);
/* Iterate */
       it = it->next;
    }
}

void List_print_elem(const List_T self, const void *elem,
                     void(*print)(const void *data) )
{
    if(!self || !elem)
        return;
/* Define the printing function */
    if(!print)
        print = self->Data_print;
/* Print the element */
    print( elem );
}

bool List_isDirty(const List_T self)
{
    return self->dirty;
}

void List_set_dirty(List_T self, const bool dirty)
{
    if(!self)
        return;

    self->dirty = dirty;
}

/* -------- Aux & Util functions (for debug) ------------*/
//bool List_replace(List_T *self, const void *elem)
//{
//    if( !(*self) ) // empty list? -> return
//        return false;
//    if( !elem)
//        return false; // invalid object passed
//
//#ifdef DEBUG
//   printf("Replace (before) - Self: %p", self);
//   getchar();
//#endif
//
//    /* Sort, because User could have replaced the key (name) */
//    List_sort(self, NULL); // use default cmp function for sort
//
//   // Initialize iterator to 1st elem of the list
//    Node_T *it = &(*self)->first;
//
//#ifdef DEBUG
//   printf("Replace (after) - Self: %p", self);
//   getchar();
//#endif
//
//   while(1)
//   {
//       /* data ptrs are equal */
//       if( (void *)elem == (*it)->data)
//       {
//           (*it)->data = (void *)elem;
//           return true;
//       }
///* Break */
//       if(! (*it)->next)
//           break;
//
///* Iterate */
//       it = &((*it)->next);
//   }
//  return false; 
//}


//void * List_search(const List_T *self, const void *elem, 
//                   int(*cmp)(const void *data1, const void *data2))
//{
//    if(!(*self) || !elem)
//        return NULL;
//
//   // Initialize iterator to 1st elem of the list
//    Node_T *it = &(*self)->first;
//   int cmp_val;
//    
//   if(!cmp)
//       cmp = (*self)->Data_cmp;
//
//   while(1)
//   {
//       cmp_val = cmp( elem, (*it)->data ) ;
///* Match found */
//       if(!cmp_val) 
//           return ((*it)->data);
///* Array is sorted; exceeded rank */
//       if(cmp_val < 0) 
//           return NULL;
///* Break */
//       if(! (*it)->next)
//           break;
///* Iterate */
//       it = &((*it)->next);
//   }
//  return NULL; 
//}


//void List_print_all(const List_T self)
//{
//    if(!self)
//        return;
//
//    if(!self->Data_print)
//        return;
//    
//    Node_T tmp = node_new();
//    tmp = self->first;
//
//    printf("\n\t\tSize: %u\n\n", self->count);
//    while(tmp)
//    {
//        self->Data_print(tmp->data);
//        tmp = tmp->next;
//    }
//}

//void * List_remove(List_T self, const void *elem, 
//                   int(*cmp)(const void *data1, const void *data2))
//{
//   if(cmp == NULL)
//       cmp = self->Data_cmp;
//   
//   Node_T node = node_new();
//   node->data =(void *) elem;
//   Node_T tmp = self->first;
//
//   while(tmp)
//   {
//       if(!( cmp(node->data, tmp->data) ) )
//           return tmp->data;
//       tmp = tmp->next;
//   }
//  return NULL; 
//}

/* Replaces data ptr */
//bool List_replace(List_T *self, const void *old_elem, const void *new_elem)
//{
//    if( !(*self) ) // empty list? -> return
//        return false;
//    if( !old_elem || !new_elem)
//        return false; // invalid object passed
//
//    Node_T node_o, node_n, *it = NULL;
//   
//   // Create new node with the elem data
//   node_o = node_new(); node_n = node_new();
//   node_o->data =(void *) old_elem;
//   node_n->data =(void *) new_elem;
//   node_o->prev = node_o->next = NULL;
//   // Initialize iterator to 1st elem of the list
//   it = &(*self)->first;
//
//   while(1)
//   {
//       /* data ptrs are equal; replace and exit */
//       if(node_o->data == (*it)->data)
//       {
//           (*it)->data = node_n->data;
//           return true;
//       }
///* Break */
//       if(! (*it)->next)
//           break;
//
///* Iterate */
//       it = &((*it)->next);
//   }
//  return false; 
//}


/* Inserts node at the beginning */
//bool List_remove(List_T *self, const void *elem)
//{
//    if( !(*self) ) // empty list? -> return
//        return false;
//    if( !elem )
//        return false; // invalid object passed
//    if(!(*self)->Data_dtor)
//        return false;
//
//   // Initialize iterator to 1st elem of the list
//    Node_T *it = &((*self)->first), *tmp = NULL; 
//
//
//   while(1)
//   {
//#ifdef DEBUG
//       printf("\nElem: %p\n", elem);
//       Node_print(it);
//       (*self)->Data_print((*it)->data);
//       getchar();
//#endif
//       // data match
//       if( elem == (*it)->data )
//       {/* redo links */
//           if( (*it)->next )
//           {
//               printf("---- Next (before)\n");
//               printf(" *it \n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               printf(" (*it)->next \n");
//               Node_print( &(*it)->next );
//               (*self)->Data_print((*it)->next->data);
//               getchar();
// 
//               (*it)->next->prev = (*it)->prev;
//
//               printf("Next (after)\n");
//               printf(" *it \n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               printf(" (*it)->next \n");
//               Node_print( &(*it)->next );
//               (*self)->Data_print((*it)->next->data);
//               getchar();
//           }
//           else // redo tail
//           {
//               printf("Tail (before)\n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               getchar();
//               
//               (*self)->last = (*it)->prev;
//
//               printf("Tail (after)\n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               getchar();
//           }
//
//           if( (*it)->prev )
//           {
//               printf("Prev (before)\n");
//               printf(" *it \n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               printf(" (*it)->prev \n");
//               Node_print( &(*it)->prev );
//               (*self)->Data_print((*it)->prev->data);
//               getchar();
//
//
//               (*it)->prev->next = ((*it)->next);
//
//
//               printf("Prev (after)\n");
//               printf(" *it \n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               printf(" (*it)->prev \n");
//               Node_print( &(*it)->prev );
//               (*self)->Data_print((*it)->prev->data);
//               getchar();
//               
//           }
//           else // redo head
//           {
//               printf("Head (before)\n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               getchar();
//               
//               
//               (*self)->first = (*it)->next;
//
//
//               printf("Head (after)\n");
//               Node_print(it);
//               (*self)->Data_print((*it)->data);
//               getchar();
//           }
//
//           /* Destroy data, delete node and update count*/
//           (*self)->Data_dtor( (*it)->data );
//           node_delete(*it);
//           (*self)->count--;
//
//           return true;
//       }
///* Break */
//       if(! (*it)->next)
//           break;
//
///* Iterate */
//       it = &((*it)->next);
//   }
//  return false; 
//}

//bool List_remove(List_T *self, const void *elem)
//{
//    if( !(*self) ) // empty list? -> return
//        return false;
//    if( !elem )
//        return false; // invalid object passed
//    if(!(*self)->Data_dtor)
//        return false;
//
//   // Initialize iterator to 1st elem of the list
//    Node_T *it = &((*self)->first);
//
//   while(1)
//   {
//#ifdef DEBUG
//       Node_print(it);
//       (*self)->Data_print((*it)->data);
//       getchar();
//#endif
//       // data match
//       if( (void *)elem == (*it)->data )
//       {/* redo links */
//#ifdef DEBUG
//       Node_print(it);
//       (*self)->Data_print((*it)->data);
//       getchar();
//#endif
//       if( (*it)->prev )
//       {
//           Node_print(it);
//           (*self)->Data_print((*it)->data);
//           (*it)->prev->next = (*it)->next;
//           Node_print(it);
//           (*self)->Data_print((*it)->data);
//           
//       }
//       else // redo head
//           (*self)->first = (*it)->next;
//
//       if( (*it)->next )
//           (*it)->next->prev = (*it)->prev;
//       else // redo tail
//           (*self)->last = (*it)->prev;
//#ifdef DEBUG
//           printf("Destroying!\n");
//           Node_print(it);
//           (*self)->Data_print((*it)->data);
//           getchar();
//#endif
//
//           /* Destroy data, delete node and update count*/
//           (*self)->Data_dtor( (*it)->data );
//           node_delete(*it);
//           (*self)->count--;
//#ifdef DEBUG
//           printf("Destroyed!\n");
//           getchar();
//#endif
//           return true;
//       }
///* Break */
//       if(! (*it)->next)
//           break;
//
///* Iterate */
//       it = &((*it)->next);
//   }
//  return false; 
//}

//void * List_push_front(List_T self, const void *elem)
//{  
//#ifdef DEBUG
//    self->Data_print(elem);
//    getchar();
//#endif
//
//    Node_T node = node_new();
//    node->data =(void *) elem;
//    
//#ifdef DEBUG
//    self->Data_print(node->data);
//    getchar();
//#endif
//
//    if(self->first)
//        self->last = node;
//    else
//        self->last = node;
//
//    node->next = self->first;
//    self->first = node;
//    self->count++;
//    return node->data;
//}
//
///* Inserts node at the end */
//bool List_append(List_T self, void *data)
//{
//    Node_T node = NULL, tmp = NULL;
//    // Allocate note and store its data
//    node = node_new();   
//    node->data =(void *) data;
//
//    if( List_isEmpty(self) ) // empty
//    {
//        self->first = node;
//        node->next = self->last;
//    }
//    else /* Insert at the end (redo links) */
//    {
//       self->last = node;
//       node->next = NULL;
//    }
//    
//
//    tmp->next = node;
//    node->next = NULL;
//    
//    return node;
//}
//
//void *List_get_data(Node_T node)
//{
//    if(node)
//        return node->data;
//    return NULL;
//}
//
bool List_remove_elem(List_T self, const void *elem)
{
  Node_T node, list_iter;
  node = node_new();  
  node->data =(void *) elem;
  list_iter = node_new();
  list_iter = self->first;

  while(list_iter) // traverse list
  {
     if( !self->Data_cmp(node, list_iter) ) 
     {
         if(list_iter->next)
             list_iter = list_iter->next;
         self->Data_dtor((void *)node->data); // free memory for elem
         node_delete(node); // delete node
         return true;
     }

     list_iter = list_iter->next;
  }
  return false;
}
//
//int List_match_elem(List_T self, const void *elem)
//{
//    int idx = -1;
//    Node_T node = node_new();
//    node->data =(void *) elem;
//    Node_T tmp = self->first;
//    while(tmp)
//    {
//        idx++;
//        if(match_data(node, tmp))
//            return idx;
//        tmp = tmp->next;
//    }
//    return -1;
//}


