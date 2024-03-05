#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include <time.h>

// Function to initialize a new linked list
struct LinkedList createLinkedList(int *arr, int arr_size) {
    struct LinkedList newList = {arr, arr_size, arr_size};
    return newList;
}

// Function to remove a node from the list
void deleteElement(struct LinkedList *list, int index) {
    list->data[index] = -2;
    list->size--;
}

void updateElement(struct LinkedList *list, int index, int value) {
    list->data[index] = value;
}

int getNextIndex(struct LinkedList *list, int index){
    int curr_index = index+1;
    while (list->data[curr_index] == -2 && curr_index < list->capacity){
        curr_index++;
    }
    if (curr_index >= list->capacity){
        return -1;
    }
    return curr_index;
}

int getPrevIndex(struct LinkedList *list, int index){
    int curr_index = index-1;
    while (list->data[curr_index] == -2 && curr_index >= 0){
        curr_index--;
    }
    if (curr_index < 0){
        return -1;
    }
    return curr_index;
}

int getNextElement(struct LinkedList *list, int index){
    int next_index = getNextIndex(list, index);
    if (next_index != -1){
        return list->data[next_index];
    }
    return -1;
}

int getPrevElement(struct LinkedList *list, int index){
    int prev_index = getPrevIndex(list, index);
    if (prev_index != -1){
        return list->data[prev_index];
    }
    return -1;
}

// Function to display the doubly linked list
void displayList(struct LinkedList list, int raw) {
    int curr = getNextIndex(&list, -1);
    while (curr != -1){
        printf("%d -- ", list.data[curr]);
        if (raw){
            curr++;
            if (curr >= list.size){
                curr = -1;
            }
        }
        else{
            curr = getNextIndex(&list, curr);
        }
    }
    printf("\n");
}

void shuffleArray(int arr[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1); // Generate a random index from 0 to i
        // Swap arr[i] and arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

// int main() {
//     int arr_size = 10;
//     int *arr = (int*)malloc(arr_size * sizeof(int));
//     // Check if memory allocation is successful
//     if (arr == NULL) {
//         printf("Memory allocation failed.\n");
//         return 1; // Return an error code indicating failure
//     }

//     for (int i = 0; i < arr_size; i++) {
//         arr[i] = i;
//     }
//     srand(time(NULL));
//     // Shuffle the array
//     shuffleArray(arr, arr_size);
//     LinkedList ll = createLinkedList(arr, arr_size);
//     displayList(ll, 0);
//     removeNode(&ll, 2);
//     removeNode(&ll, 3);
//     removeNode(&ll, 4);
//     removeNode(&ll, 6);
//     displayList(ll, 0);
//     printf("%d", ll.size);
//     return 0;
// }