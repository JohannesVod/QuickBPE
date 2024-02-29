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
heap* createHeap(int capacity, heapNode* nums);
void bubbleUp(heap* h, int index);
void heapify(heap* h, int index);
heapNode* extractMin(heap* h);
void insert(heap* h, heapNode data);
 
// Define a createHeap function
heap* createHeap(int capacity, heapNode* nums)
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
    for (i = 0; i < capacity; i++) {
        h->arr[i] = nums[i];
    }
 
    h->size = i;
    i = (h->size - 2) / 2;
    while (i >= 0) {
        heapify(h, i);
        i--;
    }
    return h;
}

void swap(heap *h, int key1, int key2){
    int temp = h->arr[key1].key;
    h->arr[key1].key = h->arr[key2].key;
    h->arr[key2].key = temp;
}

void bubbleUp(heap* h, int index)
{
    int parent = (index - 1) / 2;
    if (h->arr[parent].key > h->arr[index].key) {
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
 
heapNode* extractMin(heap* h)
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

int main()
{
    int arr[100];
    for (int i = 0; i < 100; i++) {
        arr[i] = i + 1; // Filling the array with numbers from 1 to 100 initially
    }
    srand(time(NULL)); // Seed for random number generation
    for (int i = 99; i > 0; i--) {
        int j = rand() % (i + 1); // Generate a random index from 0 to i
        // Swap arr[i] and arr[j]
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    // Creating heap nodes
    heapNode nodes[100];
    for (int i = 0; i < 100; i++) {
        element* data = (element*)malloc(sizeof(element));
        data->data = arr[i];
        nodes[i].key = arr[i];
        nodes[i].el = data;
    }

    // Creating the heap
    heap* hp = createHeap(200, nodes);

    // Printing the heap before extraction
    printf("Heap before extraction:\n");
    printHeap(hp);

    // Extracting the minimum element from the heap
    extractMin(hp);

    // Printing the heap after extraction
    printf("\nHeap after extraction:\n");
    printHeap(hp);

    return 0;
}