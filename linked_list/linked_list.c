#include <stdio.h>
#include <stdlib.h>
#include <time.h>  

#define ARR_SIZE 100 // used to set the size of the linked list
#define RANGE 100    // used to determine the range of numbers that should be in linked list [0-RANGE] inclusive

// sanity checker to make sure malloc() and free() are called the same number of times
int malloc_num = 0;


// linked list struct (typedef'd to just list)

typedef struct list {
    int value;
    struct list *next;
} list;


// prints the list by iterating over it - every 5 elements it begins a new line

void print_list(list *head) {
    int count = 0;
    while (head != NULL) {
        count++;
        printf("%6d ", head->value);
        if (count == 5) {
            count = 0;
            printf("\n");
        }
        head = head->next;
    }
    if (count % 5 != 0) printf("\n");
}


// personalmalloc() function - calls malloc() and then INCREMENT the global variable malloc_num
// I did this because my code always leaks if I don't

void* my_malloc(int size) {
    void* ret = malloc(size);
    malloc_num++;
    return ret;
}


// personal free() function - calls free() and then DECREMENTS the global variable malloc_num

void my_free(void* val) {
    free(val);
    malloc_num--;
}


// bottom function makes an appenable list item from an int
// -
// args:
// list *head: pointer to head
// data: int value
// -
// returns:
// list with data as it internal value variable (see struct above) and NULL as its internal next variable

void make_list(list *head, int data) {
    head->value = data;
    head->next = NULL;
}


// bottom function appends data to front of list
// -
// args:
// list *h: list
// int data: int value to be appended
// -
// returns:
// appended list instance

list* append(list *h, int data) {
    list* head = my_malloc(sizeof(list));
    make_list(head, data);
    head->next = h;
    return head;
}


// bottom function combines two functions above - make_list, append - to create a linked list from an array
// -
// args:
// list *head: NULL list
// int (*arr)[]: reference to an array
// int size: size of array
// - 
// returns:
// new list instance that contains items in same order as the array.
// e.g. if (*arr)[0] = 10, then the root node of the linked list will have the value 10

list* fromarray(list *head, int (*arr)[], int size) {
    for (int i = size - 1; i > -1; i--) {
        head = append(head, *(*arr + i));
    }
    return head;
}


// deep copies the list (creates a list with the same values but the memory addresses of each element is 
// completely different
// -
// args:
// list *head: root node of linked list to be copied
// -
// returns:
// deep copied linked list

list* list_d_copy(list *head) {
    if (head == NULL) return head;
    list *copy = my_malloc(sizeof(list));
    copy->value = head->value;
    copy->next = list_d_copy(head->next);
    return copy;
}


// recursively frees elements in a linked list and uses the my_free() function above (see line 47) to keep
// track of the number of times free() is called
// -
// args:
// list *head: root node of the linked list to be deleted
// -
// returns:
// nothing

void free_list(list *head) {
    while (head != NULL) {
        list *temp = head;
        head = head->next;
        my_free(temp);
    }
}


// merge function of merge sort - it merges two sorted arrays using the indexes lo and hi
// e.g. if head = [0, 5, 7, 1, 6, 8] and lo = 0 and hi = 5, the function will return [0, 1, 5, 6, 7, 8]
// -
// args:
// list *head: root node of linked list
// lo: beginning of first portion of head that's sorted
// hi: end of second portion of head that's sorted
// -
// returns:
// a new list instance where the internally sorted portions have been merged. This is done out-of-place so a new 
// head instance is returned and the previous one is freed using free_list (see line 131)

list* merge(list *head, int lo, int hi) {
    list *head_lo;
    list *head_hi;
    list *aux = list_d_copy(head);
    list *aux_copy = aux;
    list *head_freer = head;

    int i = lo;
    int j = hi;
    int mid = hi - (hi - lo) / 2;

    while (head != NULL) {
        if (lo > 0) aux = aux->next;
        if (lo-- == 0) head_lo = head;
        if (mid-- == 0) {
            head_hi = head;
            break;
        }
        head = head->next;
    }
    head = aux_copy;

    mid = j - (j - i) / 2;
    int num_reps = (j - i) + 1;
    
    j = mid;
    for (int k = 0; k < num_reps; k++) {
        if (i > mid-1) {
            aux->value = head_hi->value;
            head_hi = head_hi->next;
            j++;
        }
        else if (j > hi) {
            aux->value = head_lo->value;
            head_lo = head_lo->next;
            i++;
        }
        else if (head_hi->value < head_lo->value) {
            aux->value = head_hi->value;
            head_hi = head_hi->next;
            j++;
        }
        else {
            aux->value = head_lo->value;
            head_lo = head_lo->next;
            i++;
        }
        aux = aux->next;
    }
    free_list(head_freer);
    return head;
}


// merge-sort function that recursively sorts halves of the array then merges them with the merge() function (see line 152)
// it's not actually that much better than bubble sort :( since linked lists are used, merge has to first iterate
// to the specified index which takes N time so it's a low slower than merge-sort on an array
// - 
// args: 
// list *head: root node of linked list to be sorted
// int lo: lower bound of portion of array to be sorted (starts at 0)
// int hi: upper bound of portion of array to be sorted (starts as the length of head)
// -
// returns:
// root node of sorted array

list* msort(list *head, int lo, int hi) {
    if (lo >= hi) return head;
    int mid = hi - (hi - lo) / 2;

    head = msort(head, lo, mid-1);
    head = msort(head, mid, hi);    

    return merge(head, lo, hi);
}



int main() {

    // set a random seed at the beginning of each run
    // this ensures that it's not the exact same sequence of numbers in the array everytime you run the program
    srand((unsigned) time(NULL)); 
    
    // declare and initialize array with random values using rand() 
    int a[ARR_SIZE];
    for (int i = 0; i < ARR_SIZE; i++) {
        a[i] = rand() % (RANGE + 1);
    }

    // initialize linked list and assign its root node to list *root
    list *root = NULL;
    int size = sizeof(a) / sizeof(a[0]);
    root = fromarray(root, &a, size); 

    // print the list prior to sorting
    printf("\noriginal array:\n");
    print_list(root);
    root = msort(root, 0, size - 1);
    printf("\n\n");
    // print list after sorting
    printf("after sorting: \n");
    print_list(root);

    // free linked list by passing its root node to free_list() (see line 134)
    free_list(root);
    // this should hopefully say "0 memory leaks"
    printf("\n\n%d memory leaks\n\n", malloc_num);
    return 0;
}
