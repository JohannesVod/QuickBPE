#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include <vector>
#include <set>
#include <unordered_map>

struct HeapNode {
    int key; // size of the set stored at this pair
    int pair_index; // index of the pair
};

typedef struct HeapNode heapNode;

struct Heap {
    heapNode* arr; // pointer to the data
    std::unordered_map<int, int> pairPositions;
    int size;
    int capacity;
};

typedef struct Heap heap;
void heapify(heap* h, int index);

heap* createHeap(int vocab_size, struct LinkedList *ids)
{
    int capacity = vocab_size*vocab_size;
    // maybe replace with array, but then a lot of memory
    std::unordered_map<int, std::set<int>*> pairSets; // stores set of positions for each pair

    struct ListNode *curr_node = ids->head;
    for (int i = 0; i < ids->size-1; i++)
    {
        struct ListNode *next_node = curr_node->next;
        int first_el = curr_node->data;
        int second_el = next_node->data;
        int index = first_el*vocab_size + second_el;
        if (pairSets.find(index) != pairSets.end()){
            pairSets[index] = new std::set<int>();
        }
        pairSets[index]->insert(index);
    }

    heap* h = (heap*)malloc(sizeof(heap));
    if (h == NULL) {
        printf("Memory error");
        return NULL;
    }

    // stores position inside heap. Gets updated by heap. Maybe replace with array
    h->pairPositions = std::unordered_map<int, int>();
    h->size = 0;
    h->capacity = capacity;
    h->arr = (heapNode*)malloc(capacity * sizeof(heapNode));
 
    if (h->arr == NULL) {
        printf("Memory error");
        return NULL;
    }
    int count = 0;
    for (const auto& pair : pairSets) {
        int key = pair.first;
        int size = pair.second->size();
        h->arr[count].key = size;
        h->arr[count].pair_index = key;
        count++;
    }

    h->size = count;
    count = (h->size - 2) / 2;
    while (count >= 0) {
        heapify(h, count);
        count--;
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
        swap(h, parent, index);
        bubbleUp(h, parent);
    }
}
 
void heapify(heap* h, int index)
{
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int max = index;
    if (left >= h->size || left < 0)
        left = -1;
    if (right >= h->size || right < 0)
        right = -1;
    if (left != -1 && h->arr[left].key > h->arr[index].key)
        max = left;
    if (right != -1 && h->arr[right].key > h->arr[max].key)
        max = right;
    if (max != index) {
        swap(h, max, index);
        heapify(h, max);
    }
}
 
heapNode* extractMax(heap* h)
{
    if (h->size == 0) {
        printf("\nHeap is empty.");
        return NULL; // Return NULL to indicate an empty heap
    }

    heapNode* deleteItem = (heapNode*)malloc(sizeof(heapNode));
    if (deleteItem == NULL) {
        printf("\nMemory error.");
        return NULL;
    }

    *deleteItem = h->arr[0];
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
    heapify(h, 0);
    return deleteItem;
}
 
// Define a insert function
void insert(heap* h, heapNode data)
{
    if (h->size < h->capacity) {
        h->arr[h->size] = data;
        bubbleUp(h, h->size);
        h->size++;
    }
}

void printHeap(heap* h)
{
    for (int i = 0; i < h->size; i++) {
        int m_len = h->capacity^(1/2);
        int pair_1 = (int)(h->arr[i].pair_index/m_len);
        int pair_2 = (h->arr[i].pair_index)%m_len;
        printf("%d, pair: %d, %d\n", h->arr[i].key, pair_1, pair_2);
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
    // Create a new linked list
    int ids[] = {3, 7, 9, 5, 3, 7, 5, 1, 8, 3};
    int num_ids = sizeof(ids) / sizeof(ids[0]);
    // Create a new linked list
    struct LinkedList* list = arrayToLinkedList(ids, num_ids);
    heap *h = createHeap(100, list);
    printHeap(h);
    // Freeing the memory allocated for the linked list
    return 0;
}