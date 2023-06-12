#include <stdio.h>
#include <stdlib.h>
#include <time.h>  

#define ARR_SIZE 200 // used to set the size of the linked node
#define RANGE 49    // used to determine the range of numbers that should be in linked node [0-RANGE] inclusive
#define FORMAT_COLUMNS 8 // used to formate columns in print_node
#define VERBOSE_PRINT 0 // print VERBOSE or not (if 1, then when printing, the following thing will be printed for each node:
                        // - value
                        // - prev node's address
                        // - node address
                        // - next node's address
                        // if you change this to 1, I'd recommend changing ARR_SIZE to like 20 or lower so it's easier to 
                        // check the connections


// sanity checker to make sure malloc() and free() are called the same number of times
// keeps track of number of variables currently on heap (to avoid memory leaks)
int malloc_num = 0;


// linked node struct (typedef to just node)
typedef struct node {
    int value;
    struct node *next;
    struct node *prev;
} node;

// doubly linked list struct (typedef to just double_list)
typedef struct double_list {
    node *first;
    node *last;
} double_list;


// my_free function used to keep track of memory deletions
// malloc_num counts the number of variables currently on heap
void my_free(void *p) {
    free(p);
    malloc_num--;
}


// free_node function recursively frees each node of the double node
// since the address itself is cleared, we don't have to worry about whether connections will mess anything up
// (in other words if a node is deleted, all connections to it will then point to NULL due to this)
// uses my_free() function (see line 38)
void free_list(double_list *root) {
    node *n = root->first;
    while (n != NULL) {
        node *temp = n;
        n = n->next;
        my_free(temp);
    }   
    my_free(root);
}


// my_malloc function that calls malloc() and also increments malloc_num (see line 19)
void* my_malloc(int size) {
    malloc_num++;
    return malloc(size);
}


// iterates over the node and prints:
// -  current node's value
// -  previous node's address
// -  current node's address
// -  next node's address
// if you want to make sure that the connections are correct you can use this function rather than print_node
// in main
void print_node_full(node *node) {
    while (node != NULL) {
        printf("value: %11d\tprev: %15p\taddress: %15p\tnext: %15p\n",node->value, (void *) node->prev, (void *) node, (void *) node->next);
        node = node->next;
    }
}


// prints the values in the node
void print_node(node *node) {
    int count = 0;
    while (node != NULL) {
        count = (count + 1) % FORMAT_COLUMNS;
        printf("%11d, ", node->value);
        if (count == 0) printf("\n");
        node = node->next;
    }
    printf("\n");
}


// append() appends an integer to the front of the node (like pushing to a stack)
// - 
// args:
// node *n: current root node of node
// int data: data to be appended
// -
// returns:
// pointer to new root node with data as its value
node* append(node *n, int data) {
    node *d = my_malloc(sizeof(node));
    d->value = data;
    d->prev = NULL;
    d->next = n;
    if (n != NULL) n->prev = d;
    return d;
}


// initialize() makes a doubly linked node from an array
double_list* initialize(int (*arr)[], int size) {
    double_list *ret = my_malloc(sizeof(double_list));
    node *root = NULL;
    for (int i = size - 1; i > -1; i--) {
        root = append(root, *(*arr + i));
        if (i == size - 1) ret->last = root;
    }
    ret->first = root;
    return ret;
}


// delete() deletes a node from the node and returns the previous node with the updated connections to next and prev
// -
// args:
// node *n: a pointer corresponding to the address of the node to be deleted
// -
// returns:
// a node* pointer corresponding to the address of the previous node. this node's internal variables for 
// next and prev are updated accordingly and edge cases are accounted for (deleting the first/last node)
node* delete(node *n) {
    // printf("%p\n", node);
    node *previous = n->prev;
    node *old_root = n;
    if (previous == NULL) {
        previous = n->next;
        previous->prev = NULL; 
    }
    else previous->next = n->next;
    if (n->next != NULL) n->next->prev = previous;
    my_free(old_root);
    return previous;
}


// delete_duplicates() deletes the duplicates in the node while maintaining the same order of elements
// -
// args:
// double_list *list: pointer to double_list instance 
// - 
// returns:
// nothing, it's done in place
// -
// an int[50] is used to keep track of the number of times each element shows up (the range of numbers in the node
// can only be between 0-49). If, when an element "n" is encountered, arr[n] == 0 then arr[n]++ and nothing happens
// else if an element "n" is encountered and arr[n] > 0 that node is deleted because we've already encountered an element
// with the same value. This also maintains the relative order to the node's elements and we don't have to sort
// or anything. It's done in linear time
void delete_duplicates(double_list *list) {
    int checker[RANGE + 1];
    node *root = list->first;
    for (int i = 0; i < RANGE + 1; i++) {
        checker[i] = 0;
    }
    while (root != NULL) {
        if (checker[root->value] == 0) checker[root->value]++;
        else root = delete(root);
        if (root->next == NULL) list->last = root;
        root = root->next;
    }
}

int main() {
    // initialize random seed so that random numbers are different every time you run main()
    // and initialize array of randomly generated numbers between 0-ARR_SIZE (inclusive) to be turned into a doubly-linked
    // node
    srand((unsigned)(time(NULL)));
    int arr[ARR_SIZE];
    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = rand() % (RANGE + 1);
    }

    // create doubly linked_node from array
    int size = sizeof(arr) / sizeof(arr[0]);
    double_list *list= initialize(&arr, size);
    
    // print node before and after deleting duplicates 
    // (if VERBOSE_PRINT is true then a more in-depth printing of the node will be done, else just the values will
    // be printed) see line 8
    printf("\nlist before deleting duplicates\n");
    if (VERBOSE_PRINT) print_node_full(list->first);
    else print_node(list->first);
    // print the head and tail node of the doubly linked list (these should correspond to the first/last numbers shown
    // the head address should be the exact same even after delete_duplicates
    // the tail address should change if the tail value changes
    // you can also change VERBOSE_PRINT to 1 at the top of the program to conclusively make sure head/tail addresses 
    // are correct if you'd like 
    printf("-----\ndouble list info:\nhead: %11d - (address = %p)\ntail: %11d - (address = %p)\n", list->first->value, (void *) list->first, list->last->value, (void *) list->last);

    printf("\n\nlist after deleting duplicates\n");
    delete_duplicates(list);
    if (VERBOSE_PRINT) print_node_full(list->first);
    else print_node(list->first);
    printf("-----\ndouble list info:\nhead: %11d - (address = %p)\ntail: %11d - (address = %p)\n", list->first->value, (void *) list->first, list->last->value, (void *) list->last);
    printf("tail's address SHOULD change if it's value changes and it's value should exactly match the last element in the list\n");
    printf("head's address should never change\n");
    
    // recursively free all items in doubly linked list
    free_list(list);
    printf("\nmemory leaks: %d\n\n", malloc_num);
    return 0;
}


