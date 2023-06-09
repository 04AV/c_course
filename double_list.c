#include <stdio.h>
#include <stdlib.h>
#include <time.h>  

#define ARR_SIZE 200 // used to set the size of the linked list
#define RANGE 49    // used to determine the range of numbers that should be in linked list [0-RANGE] inclusive
#define FORMAT_COLUMNS 8 // used to formate columns in print_list
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


// linked list struct (typedef to just list)
typedef struct list {
    int value;
    struct list *next;
    struct list *prev;
} double_list;


// my_free function used to keep track of memory deletions
// malloc_num counts the number of variables currently on heap
void my_free(void *p) {
    free(p);
    malloc_num--;
}


// free_list function recursively frees each node of the double list
// since the address itself is cleared, we don't have to worry about whether connections will mess anything up
// (in other words if a node is deleted, all connections to it will then point to NULL due to this)
// uses my_free() function (see line 23)
void free_list(double_list *list) {
    while (list != NULL) {
        double_list *node = list;
        list = list->next;
        my_free(node);
    }   
}


// my_malloc function that calls malloc() and also increments malloc_num (see line 10)
void* my_malloc(int size) {
    malloc_num++;
    return malloc(size);
}


// iterates over the list and prints:
// -  current node's value
// -  previous node's address
// -  current node's address
// -  next node's address
// if you want to make sure that the connections are correct you can use this function rather than print_list
// in main
void print_list_full(double_list *list) {
    while (list != NULL) {
        printf("value: %11d\tprev: %15p\taddress: %15p\tnext: %15p\n",list->value, (void *) list->prev, (void *) list, (void *) list->next);
        list = list->next;
    }
}


// prints the values in the list
void print_list(double_list *list) {
    int count = 0;
    while (list != NULL) {
        count = (count + 1) % FORMAT_COLUMNS;
        printf("%11d, ", list->value);
        if (count == 0) printf("\n");
        list = list->next;
    }
    printf("\n");
}


// append() appends an integer to the front of the list (like pushing to a stack)
// - 
// args:
// double_list *list: current root node of list
// int data: data to be appended
// -
// returns:
// pointer to new root node with data as its value
double_list* append(double_list *list, int data) {
    double_list *d = my_malloc(sizeof(double_list));
    d->value = data;
    d->prev = NULL;
    d->next = list;
    if (list != NULL) list->prev = d;
    return d;
}


// initialize() makes a doubly linked list from an array
double_list* initialize(int (*arr)[], int size) {
    double_list *ret = NULL;
    for (int i = size - 1; i > -1; i--) {
        ret = append(ret, *(*arr + i));
    }
    return ret;
}


// delete() deletes a node from the list and returns the previous node with the updated connections to next and prev
// -
// args:
// double_list *list: a pointer corresponding to the address of the node to be deleted
// -
// returns:
// a double_list* pointer corresponding to the address of the previous node. this node's internal variables for 
// next and prev are updated accordingly and edge cases are accounted for (deleting the first/last node)
double_list* delete(double_list *list) {
    // printf("%p\n", list);
    double_list *previous = list->prev;
    double_list *old_root = list;
    if (previous == NULL) {
        previous = list->next;
        previous->prev = NULL; 
    }
    else previous->next = list->next;
    if (list->next != NULL) list->next->prev = previous;
    my_free(old_root);
    return previous;
}


// delete_duplicates() deletes the duplicates in the list while maintaining the same order of elements
// -
// args:
// double_list *list: pointer to root node
// - 
// returns:
// nothing, it's done in place
// -
// an int[50] is used to keep track of the number of times each element shows up (the range of numbers in the list
// can only be between 0-49). If, when an element "n" is encountered, arr[n] == 0 then arr[n]++ and nothing happens
// else if an element "n" is encountered and arr[n] > 0 that node is deleted because we've already encountered an element
// with the same value. This also maintains the relative order to the list's elements and we don't have to sort
// or anything. It's done in linear time
void delete_duplicates(double_list *list) {
    int checker[RANGE + 1];
    double_list *root = list;
    for (int i = 0; i < RANGE + 1; i++) {
        checker[i] = 0;
    }
    while (root != NULL) {
        if (checker[root->value] == 0) checker[root->value]++;
        else root = delete(root);
        root = root->next;
    }
}

int main() {
    // initialize random seed so that random numbers are different every time you run main()
    // and initialize array of randomly generated numbers between 0-ARR_SIZE (inclusive) to be turned into a doubly-linked
    // list
    srand((unsigned)(time(NULL)));
    int arr[ARR_SIZE];
    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = rand() % (RANGE + 1);
    }

    // create doubly linked_list from array
    int size = sizeof(arr) / sizeof(arr[0]);
    double_list *list = initialize(&arr, size);
    
    // print list before and after deleting duplicates 
    // (if VERBOSE_PRINT is true then a more in-depth printing of the list will be done, else just the values will
    // be printed) see line 8
    printf("\nlist before deleting duplicates\n");
    if (VERBOSE_PRINT) print_list_full(list);
    else print_list(list);

    printf("\nlist after deleting duplicates\n");
    delete_duplicates(list);
    if (VERBOSE_PRINT) print_list_full(list);
    else print_list(list);
    
    free_list(list);
    printf("\nmemory leaks: %d\n\n", malloc_num);
    return 0;
}


