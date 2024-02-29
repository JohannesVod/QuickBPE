#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
// Define a structure for a node of the heap
struct Node {
    struct Node *leftChild;
    struct Node *rightChild;
    struct Node *parent;
    int data;
};

struct Heap {
    struct Node *head;
    struct Node *last_added;
};

struct Node *findLastElem(struct Heap *heap){
    // finds the element on which the node is inserted
    struct Node *curr = heap->last_added;

    while (curr->parent != NULL && curr->parent->rightChild == curr){
        curr = curr->parent;
    }
    if (curr->parent != NULL){ // if not head
        curr = curr->parent->rightChild;
    }
    while(curr->leftChild != NULL && curr->rightChild != NULL){
        curr = curr->leftChild;
    }
    return curr;
}

// Create a new node with a given data value and return a pointer to it
struct Node *newNode(int data) {
    struct Node *node = (struct Node *)malloc(sizeof(struct Node));
    node->leftChild = NULL;
    node->rightChild = NULL;
    node->parent = NULL;
    node->data = data;
    return node;
}

// Swap the data values of two nodes
void swap(struct Node *node1, struct Node *node2) {
    int temp = node1->data;
    node1->data = node2->data;
    node2->data = temp;
}

void heapify(struct Node *node) {
    struct Node *largest = node; // Initialize largest as root
    struct Node *left = node->leftChild;
    struct Node *right = node->rightChild;

    if (left != NULL && left->data > largest->data)
        largest = left;

    if (right != NULL && right->data > largest->data)
        largest = right;

    if (largest != node) {
        swap(node, largest);
        heapify(largest);
    }
}

// Bubble up a node to maintain the max heap property
void bubbleUp(struct Node *node) {
    while (node->parent != NULL && node->data > node->parent->data) {
        swap(node, node->parent);
        node = node->parent;
    }
}

// Insert a new node with given data into the heap
void insert(struct Heap *heap, int data) {
    struct Node *newN = newNode(data);
    if (heap->head == NULL) {
        heap->head = newN;
        return;
    }

    struct Node *appendNode = findLastElem(heap);

    if (appendNode->leftChild == NULL){
        appendNode->leftChild = newN;
    }
    else{
        appendNode->rightChild = newN;
    }

    // Fix the heap property by bubbling up the newly inserted node
    bubbleUp(newN);
}

