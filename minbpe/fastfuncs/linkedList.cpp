#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"

// Function to create a new node
struct ListNode* createNode(int data) {
    struct ListNode* newNode = (struct ListNode*)malloc(sizeof(struct ListNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a new linked list
struct LinkedList* createLinkedList() {
    struct LinkedList* newList = (struct LinkedList*)malloc(sizeof(struct LinkedList));
    if (newList == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newList->head = NULL;
    newList->size = 0;
    return newList;
}

// Function to insert a new node at the beginning of the list
void insertAtBeginning(struct LinkedList* list, int data) {
    struct ListNode* newNode = createNode(data);
    if (list->head == NULL) {
        list->head = newNode;
    } else {
        newNode->next = list->head;
        list->head->prev = newNode;
        list->head = newNode;
    }
    list->size += 1;
}

// Function to remove a node from the list
void removeNode(struct LinkedList* list, struct ListNode* node) {
    if (list->head == NULL || node == NULL)
        return;

    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;

    list->size -= 1;
    free(node);
}

// Function to free the memory allocated for the linked list
void freeLinkedList(struct LinkedList* list) {
    struct ListNode* current = list->head;
    struct ListNode* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

// Function to display the doubly linked list
void displayList(struct LinkedList* list) {
    struct ListNode* temp = list->head;
    while (temp != NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

// Function to create a linked list from an array
struct LinkedList* arrayToLinkedList(int *arr, int size) {
    struct LinkedList* list = createLinkedList();
    for (int i = size-1; i >= 0; i--) {
        insertAtBeginning(list, arr[i]);
    }
    return list;
}

// int main() {
//     int arr_size = 1000000;
//     int *arr = (int*)malloc(arr_size * sizeof(int));
//     // Check if memory allocation is successful
//     if (arr == NULL) {
//         printf("Memory allocation failed.\n");
//         return 1; // Return an error code indicating failure
//     }

//     for (int i = 0; i < arr_size; i++) {
//         arr[i] = i + 1;
//     }
    
//     // Shuffle the array
//     shuffleArray(arr, arr_size);
//     heapNode *nodes = (heapNode*)malloc(arr_size * sizeof(heapNode));
//     // Check if memory allocation is successful
//     if (nodes == NULL) {
//         printf("Memory allocation failed.\n");
//         free(arr); // Free the previously allocated memory
//         return 1; // Return an error code indicating failure
//     }

//     for (int i = 0; i < arr_size; i++) {
//         nodes[i].key = arr[i];
//     }

//     heap* hp = createHeap(2 * arr_size, arr_size, nodes);
    
//     for (int i = 0; i < arr_size; i++) {
//         heapNode* max_el = extractMax(hp);
//         if (max_el->key != arr_size - i) {
//             printf("ERROR!!! Expected: %d, Actual: %d\n", arr_size - i, max_el->key);
//         }
//     }
//     printf("%d\n", hp->size);
//     // Free dynamically allocated memory
//     free(arr);

//     freeHeap(hp);
//     free(nodes);
//     return 0;
// }