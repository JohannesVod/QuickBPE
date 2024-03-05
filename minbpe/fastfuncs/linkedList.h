#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

struct LinkedList {
    int *data;
    int capacity;
    int size;
};

struct LinkedList createLinkedList(int *arr, int arr_size);
void deleteElement(struct LinkedList *list, int index);
void updateElement(struct LinkedList *list, int index, int value);
int getNextIndex(struct LinkedList *list, int index);
int getPrevIndex(struct LinkedList *list, int index);
void displayList(struct LinkedList list, int raw);
int getPrevElement(struct LinkedList *list, int index);
int getNextElement(struct LinkedList *list, int index);
#endif
