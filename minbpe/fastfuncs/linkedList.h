#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

struct ListNode {
    int data;
    struct ListNode* prev;
    struct ListNode* next;
};

struct LinkedList {
    struct ListNode* head;
    int size;
};

struct ListNode* createNode(int data);
struct LinkedList* createLinkedList();
struct LinkedList* arrayToLinkedList(int *arr, int size);
void insertAtBeginning(struct LinkedList* list, int data);
void removeNode(struct LinkedList* list, struct ListNode* node);
void freeLinkedList(struct LinkedList* list);
void displayList(struct LinkedList* list);
#endif
