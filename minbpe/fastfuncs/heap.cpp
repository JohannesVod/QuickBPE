#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include <vector>
#include <set>
#include <unordered_map>
#include <math.h>
#include <memory>

struct HeapNode {
    int key; // size of the set stored at this pair
    int pair_index; // index of the pair
};

typedef struct HeapNode heapNode;

struct Heap {
    heapNode* arr; // pointer to the data
    std::unordered_map<int, int> pairPositions;
    std::unordered_map<int, std::unique_ptr<std::set<int>>> pairSets;
    int size;
    int capacity;
    int vocabSize;
};

typedef struct Heap heap;
void heapify(heap* h, int index);

// returns a heap object by value instead of by pointer
heap createHeap(int vocab_size, struct LinkedList *ids)
{
    heap h; // create a heap object on the stack
    int capacity = vocab_size*vocab_size;
    // maybe replace with array, but then a lot of memory
    h.pairSets = std::unordered_map<int, std::unique_ptr<std::set<int>>>();

    struct ListNode *curr_node = ids->head;
    for (int i = 0; i < ids->size-1; i++)
    {
        struct ListNode *next_node = curr_node->next;
        int first_el = curr_node->data;
        int second_el = next_node->data;
        int index = first_el*vocab_size + second_el;
        if (first_el != 0 && second_el != 0){
            if (h.pairSets.find(index) == h.pairSets.end()){
                h.pairSets[index] = std::make_unique<std::set<int>>();
            }
            h.pairSets[index]->insert(i);
        }
        curr_node = curr_node->next;
    }
    
    h.pairPositions = std::unordered_map<int, int>(); // initialize the map with the constructor
    h.size = 0;
    h.capacity = capacity;
    h.vocabSize = vocab_size;
    h.arr = (heapNode*)malloc(capacity * sizeof(heapNode));
 
    if (h.arr == NULL) {
        printf("Memory error");
        return h; // return an empty heap
    }
    int count = 0;
    for (const auto& pair : h.pairSets) {
        int key = pair.first;
        int size = pair.second->size();
        h.arr[count].key = size;
        h.arr[count].pair_index = key;
        h.pairPositions[key] = count;
        count++;
    }

    h.size = count;
    count = (h.size - 2) / 2;
    while (count >= 0) {
        heapify(&h, count); // pass the address of the heap object
        count--;
    }
    return h; // return the heap object by value
}

void RemovePosition(heap *h, int index, int num_1, int num_2){
    // removes stored pair position and updates the heap
    int pair_index = num_1 * h->vocabSize + num_2;
    auto& pairSet = h->pairSets[pair_index];
    pairSet->erase(index);
    int new_len = pairSet->size();
    decreaseKey(h, h->pairPositions[pair_index], new_len);
}

void AddPosition(heap *h, int index, int num_1, int num_2){
    // adds a new pair position and updates the heap
    int pair_index = num_1 * h->vocabSize + num_2;
    if (h->pairSets.find(pair_index) == h->pairSets.end()){
        // if not added yet (unseen pair), add it to the heap
        h->pairSets[pair_index] = std::make_unique<std::set<int>>();
        insert(h, 0, pair_index);
    }
    auto& pairSet = h->pairSets[pair_index];
    pairSet->insert(index);
    int new_len = pairSet->size();
    increaseKey(h, h->pairPositions[pair_index], new_len);
}

void swap(heap *h, int key1, int key2){
    // save the new positions in the pair_index map:
    heapNode temp = h->arr[key1];
    h->pairPositions[temp.pair_index] = key2;
    h->pairPositions[h->arr[key2].pair_index] = key1;
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
void insert(heap* h, int key, int pair_index)
{
    if (h->size < h->capacity) {
        h->arr[h->size].key = key;
        h->arr[h->size].pair_index = pair_index;
        bubbleUp(h, h->size);
        h->size++;
    }
}

void decreaseKey(heap* h, int position, int newKey){
    h->arr[position].key = newKey;
    heapify(h, position);
}

void increaseKey(heap* h, int position, int newKey){
    h->arr[position].key = newKey;
    bubbleUp(h, position);
}

void printHeap(heap* h)
{
    for (int i = 0; i < h->size; i++) {
        int m_len = h->vocabSize;
        int pair_1 = (int)(h->arr[i].pair_index/m_len);
        int pair_2 = (h->arr[i].pair_index)%m_len;
        printf("%d, pair: %d, %d\n", h->arr[i].key, pair_1, pair_2);
    }
    printf("\nPair indices:\n");
    for (int i = 0; i < h->size; i++) {
        printf("position in heap: %d\n", h->pairPositions[h->arr[i].pair_index]);
    }
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
    hp->pairPositions.clear();
    hp->pairSets.clear();
}

int main() {
    // Create a new linked list
    int ids[] = {8, 3, 1, 5, 0, 7, 8, 3, 8, 3};
    int num_ids = sizeof(ids) / sizeof(ids[0]);
    // Create a new linked list
    struct LinkedList* list = arrayToLinkedList(ids, num_ids);
    heap h = createHeap(100, list);
    printHeap(&h);
    freeHeap(&h);
    freeLinkedList(list);
    // Freeing the memory allocated for the linked list
    return 0;
}