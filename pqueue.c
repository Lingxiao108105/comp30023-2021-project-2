#include "pqueue.h"

// create an empty priority queue
Pqueue *create_pqueue(){
    Pqueue *temp = (Pqueue *)malloc(sizeof(Pqueue));
    temp->head = NULL;
    temp->length = 0;
    return temp;
}


// create a new node
Node *new_node(void *data){
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->data = data;
    temp->next = NULL;
 
    return temp;
}

//free a node and data inside
void free_node(Node *node, void (*free_data)(void *data)){
    if(node == NULL){
        return;
    }
    free_data(node->data);
    free(node);
}

//free a priority queue and data inside
void free_pqueue(Pqueue *pqueue, void (*free_data)(void *data)){
    if(pqueue == NULL){
        return;
    }
    Node *curr_node=pqueue->head;
    Node *temp_node;
    while(curr_node!=NULL){
        temp_node = curr_node;
        curr_node = curr_node->next;
        free_node(temp_node,free_data);
    }
    free(pqueue);
}

// return the data at head
void* head(Pqueue *pqueue){
    if(isEmpty(pqueue)){
        return NULL;
    }
    return pqueue->head->data;
}

// remove the element with the highest priority from the list and return data
void *pop(Pqueue *pqueue){

    if(pqueue->head==NULL){
        return NULL;
    }

    Node *temp = pqueue->head;
    void *data = temp->data;
    pqueue->head = pqueue->head->next;
    (pqueue->length)--;
    free(temp);
    return data;
}

/**push a node into priority queue
 * a function of how to compare the data is required
 * return -1 if data1 is smaller, 0 if equal, 1 if is bigger
*/
void push_node(Pqueue *pqueue, Node *temp, 
                int (*compare)(void *data1, void *data2)){

    //start from head
    Node *curr_node = pqueue->head;

    //case: empty queue
    if(pqueue->head == NULL){
        (pqueue->length)++;
        pqueue->head = temp;
        return;
    }

    //add new node at head
    if(compare(temp->data,pqueue->head->data)==SMALLER){
        temp->next = pqueue->head;
        pqueue->head = temp;
    }
    else{
        //find the place to insert new node
        while(curr_node->next != NULL &&
            compare(temp->data,curr_node->next->data)!=SMALLER){
            curr_node = curr_node->next;
        }

        //insert the new node
        temp->next = curr_node->next;
        curr_node->next = temp;

    }

    //increment the length of priority queue
    (pqueue->length)++;
}


/**push data into priority queue
 * a function of how to compare the data is required
 * return -1 if data1 is smaller, 0 if equal, 1 if is bigger
*/
void push_data(Pqueue *pqueue, void *data, 
                int (*compare)(void *data1, void *data2)){
    // Create new Node
    Node *temp = new_node(data);
    //push it into priority queue
    push_node(pqueue,temp,compare);
}


// return whether the priority queue is empty
int isEmpty(Pqueue *pqueue){
    return pqueue->length==0;
}

// return  the length of priority queue
unsigned int pqueue_length(Pqueue *pqueue){
    return pqueue->length;
}

/**remove a node which contains a specific data 
 * will not free data
 */
void remove_node(Pqueue *pqueue, void *data){

    Node *curr_node = pqueue->head;

    //the queue is empty
    if(curr_node == NULL){
        return;
    }


    //head is the node we want
    if(curr_node->data == data){
        pop(pqueue);
    }
    else{
        //find the node we need to remove
        while(curr_node->next!=NULL &&
            curr_node->next->data != data){
            curr_node = curr_node->next;
        }

        if(curr_node->next==NULL){
            return;
        }
        Node *temp = curr_node->next;
        curr_node->next = temp->next;
        (pqueue->length)--;
        free(temp);
    }
}

/**find the specific data using compare function
 * and pop it
 * will not free data
 */
void *find_data(Pqueue *pqueue, void *data,
            int (*compare)(void *data1, void *data2)){

    Node *curr_node = pqueue->head;
    void *target;
    //the queue is empty
    if(curr_node == NULL){
        return NULL;
    }


    //head is the node we want
    if(compare(curr_node->data, data) == EQUAL){
        target =  pop(pqueue);
    }
    else{
        //find the node we need to remove
        while(curr_node->next!=NULL &&
            compare(curr_node->next->data, data) != EQUAL){
            curr_node = curr_node->next;
        }

        if(curr_node->next==NULL){
            return NULL;
        }
        Node *temp = curr_node->next;
        target = temp->data;
        curr_node->next = temp->next;
        (pqueue->length)--;
        free(temp);
    }
    return target;
}

/**sort the priority queue
 */
Pqueue *pqueue_sort(Pqueue *pqueue, int (*compare)(void *data1, void *data2)){
    Pqueue *temp_pqueue = create_pqueue();
    while(!isEmpty(pqueue)){
        push_data(temp_pqueue,pop(pqueue),compare);
    }
    //here the pqueue is empty
    free(pqueue);
    pqueue = temp_pqueue;
    return temp_pqueue;
}

