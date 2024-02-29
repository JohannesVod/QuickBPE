#include <stdio.h>
#include <stdlib.h>
 
// Declare a heap structure
struct Element{
    int data;
};
typedef struct Element element;

struct HeapNode {
    int key;
    element* el;
};

typedef struct HeapNode heapNode;

struct Heap {
    heapNode* arr;
    int size;
    int capacity;
};

typedef struct Heap heap;
// forward declarations
heap* createHeap(int capacity, int size, heapNode* nums);
void bubbleUp(heap* h, int index);
void heapify(heap* h, int index);
heapNode* extractMax(heap* h);
void insert(heap* h, heapNode data);
 
// Define a createHeap function
heap* createHeap(int capacity, int size, heapNode* nums)
{
    // Allocating memory to heap h
    heap* h = (heap*)malloc(sizeof(heap));
 
    // Checking if memory is allocated to h or not
    if (h == NULL) {
        printf("Memory error");
        return NULL;
    }
    // set the values to size and capacity
    h->size = 0;
    h->capacity = capacity;
 
    // Allocating memory to array
    h->arr = (heapNode*)malloc(capacity * sizeof(heapNode));
 
    // Checking if memory is allocated to h or not
    if (h->arr == NULL) {
        printf("Memory error");
        return NULL;
    }
    int i;
    for (i = 0; i < size; i++) {
        h->arr[i] = nums[i];
    }
 
    h->size = size;
    i = (h->size - 2) / 2;
    while (i >= 0) {
        heapify(h, i);
        i--;
    }
    return h;
}

void swap(heap *h, int key1, int key2){
    heapNode temp = h->arr[key1];
    h->arr[key1] = h->arr[key2];
    h->arr[key2] = temp;
}

void bubbleUp(heap* h, int index)
{
    int parent = (index - 1) / 2;
    if (h->arr[parent].key < h->arr[index].key) {
        // Swapping when child is smaller
        // than parent element
        swap(h, parent, index);
        // Recursively calling insertHelper
        bubbleUp(h, parent);
    }
}
 
void heapify(heap* h, int index)
{
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int max = index;
 
    // Checking whether our left or child element
    // is at right index or not to avoid index error
    if (left >= h->size || left < 0)
        left = -1;
    if (right >= h->size || right < 0)
        right = -1;
 
    // store left or right element in min if
    // any of these is smaller than its parent
    if (left != -1 && h->arr[left].key > h->arr[index].key)
        max = left;
    if (right != -1 && h->arr[right].key > h->arr[max].key)
        max = right;
 
    // Swapping the nodes
    if (max != index) {
        swap(h, max, index);
        heapify(h, max);
    }
}
 
heapNode* extractMax(heap* h)
{
    // Checking if the heap is empty or not
    if (h->size == 0) {
        printf("\nHeap is empty.");
        return NULL; // Return NULL to indicate an empty heap
    }

    // Allocate memory for the node to be deleted
    heapNode* deleteItem = (heapNode*)malloc(sizeof(heapNode));
    if (deleteItem == NULL) {
        printf("\nMemory error.");
        return NULL;
    }

    // Store the node in deleteItem that is to be deleted.
    *deleteItem = h->arr[0];

    // Replace the deleted node with the last node
    h->arr[0] = h->arr[h->size - 1];
    // Decrement the size of heap
    h->size--;

    // Call heapify from the root to maintain the heap property
    heapify(h, 0);

    return deleteItem;
}
 
// Define a insert function
void insert(heap* h, heapNode data)
{
    // Checking if heap is full or not
    if (h->size < h->capacity) {
        // Inserting data into an array
        h->arr[h->size] = data;
        // Calling insertHelper function
        bubbleUp(h, h->size);
        // Incrementing size of array
        h->size++;
    }
}

void printHeap(heap* h)
{
    for (int i = 0; i < h->size; i++) {
        printf("%d ", h->arr[i].key);
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

void freeHeap(heap *hp){
    free(hp->arr);
    free(hp);
}

int main() {
    int arr_size = 1000000;
    int *arr = (int*)malloc(arr_size * sizeof(int));
    // Check if memory allocation is successful
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return 1; // Return an error code indicating failure
    }

    for (int i = 0; i < arr_size; i++) {
        arr[i] = i + 1;
    }
    
    // Shuffle the array
    shuffleArray(arr, arr_size);
    heapNode *nodes = (heapNode*)malloc(arr_size * sizeof(heapNode));
    // Check if memory allocation is successful
    if (nodes == NULL) {
        printf("Memory allocation failed.\n");
        free(arr); // Free the previously allocated memory
        return 1; // Return an error code indicating failure
    }

    for (int i = 0; i < arr_size; i++) {
        element* data = (element*)malloc(sizeof(element));
        data->data = arr[i];
        nodes[i].key = arr[i];
        nodes[i].el = data;
    }

    heap* hp = createHeap(2 * arr_size, arr_size, nodes);
    
    for (int i = 0; i < arr_size; i++) {
        heapNode* max_el = extractMax(hp);
        if (max_el->key != arr_size - i) {
            printf("ERROR!!! Expected: %d, Actual: %d\n", arr_size - i, max_el->key);
        }
    }
    printf("%d\n", hp->size);
    // Free dynamically allocated memory
    free(arr);

    freeHeap(hp);
    free(nodes);
    return 0;
}