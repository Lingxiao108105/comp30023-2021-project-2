#ifndef _PQUEUE_H
#define _PQUEUE_H

#include <stdio.h>
#include <stdlib.h>

/** a Priority Queue which can reuse by change the data
 * 
*/
#define EQUAL 0
#define SMALLER (-1)
#define BIGGER 1

// Node
typedef struct node{
    void *data;
    struct node *next;
}Node;

// Priority Queue
typedef struct pqueue{
    Node *head;
    unsigned int length;
}Pqueue;

// create an empty priority queue
Pqueue *create_pqueue();
// create a new node
Node *new_node(void *data);
//free a node and data inside
void free_node(Node *node, void (*free_data)(void *data));
//free a priority queue and data inside
void free_pqueue(Pqueue *pqueue, void (*free_data)(void *data));
// return the data at head
void *head(Pqueue *pqueue);
// remove the element with the highest priority from the list
void *pop(Pqueue *pqueue);
/**push a node into priority queue
 * a function of how to compare the data is required
 * return -1 if data1 is smaller, 0 if equal, 1 if is bigger
*/
void push_node(Pqueue *pqueue, Node *temp, 
                int (*compare)(void *data1, void *data2));
/**push data into priority queue
 * a function of how to compare the data is required
 * return -1 if data1 is smaller, 0 if equal, 1 if is bigger
*/
void push_data(Pqueue *pqueue, void *data, 
                int (*compare)(void *data1, void *data2));
// return whether the priority queue is empty
int isEmpty(Pqueue *pqueue);
// return  the length of priority queue
unsigned int pqueue_length(Pqueue *pqueue);
/**remove a node which contain a specific data 
 * will not free data
 */
void remove_node(Pqueue *pqueue, void *data);
/**find the specific data using compare function
 * and pop it
 * will not free data
 */
void *find_data(Pqueue *pqueue, void *data,
            int (*compare)(void *data1, void *data2));
/**sort the priority queue
 */
Pqueue *pqueue_sort(Pqueue *pqueue, int (*compare)(void *data1, void *data2));

#endif