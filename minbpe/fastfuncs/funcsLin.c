#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Node {
    int v;
    int index;
    struct Node *l;
    struct Node *r;
    struct Node *next_occ;
    struct Node *prev_occ;
    struct Pair *pair;
} Node;

typedef struct {
    Node *start;
    int size;
} LinkedList;

typedef struct Pair {
    int a;
    int b;
    // first occurrence:
    Node *first_occurrence;
    Node *last_occurrence;
    int num_occurrences;
    // points to next/prev pair, has same number of occurrences (later used for heap):
    struct Pair *next;
    struct Pair *prev;  
} Pair;

typedef struct {
    int id;
} Token;

void printL(LinkedList *L) {
    Node *curr = L->start;
    int i=0;
    while (curr != NULL) {
        printf("%d", curr->v);
        // if (curr->next_occ != NULL){
        //     printf("(next occ: %d)", curr->next_occ->index);
        // }
        // else{
        //     printf("*");
        // }
        // if (curr->prev_occ != NULL){
        //     printf("(prev occ: %d)", curr->prev_occ->index);
        // }
        if (curr->pair != NULL){
            //printf("(pair: (%d, %d))", curr->pair->a, curr->pair->b);
        }
        else{
            printf("*");
        }
        if (curr->next_occ != NULL){
            if (curr->next_occ->prev_occ != curr){
                printf("|!!!|");
            }
        }
        if (curr->r != NULL){
            if (curr->r->l != curr){
                printf("-!!!-");
            }
            else{
                printf("=");
            }
        }
        curr = curr->r;
        i++;
    }
    printf("\n");
}

void printPair(Pair *p){
    if (p->num_occurrences == 0){
        return;
    }
    printf("Pair: (%d, %d) Occurrences(%d): \n", p->a, p->b, p->num_occurrences);
    Node *curr = p->first_occurrence;
    while (curr != NULL) {
        printf("(pos: %d)", curr->index);
        if (curr->next_occ != NULL){
            if (curr->next_occ->prev_occ != curr) {
                printf("!!!!");
            }
            else {
                printf("=");
            }
        }
        curr = curr->next_occ;
    }
    printf("\n");
}

void printHeap(Pair **h, int heapsize){
    printf("HEAP:\n");
    for (int i=0; i<heapsize; i++){
        if (h[i] != NULL){
            Pair *curr = h[i];
            printf("size: %d: ", i);
            while (curr != NULL){
                printf("|pair: (%d, %d)|", curr->a, curr->b);
                if (curr->next != NULL){
                    if (curr->next->prev != curr){
                        printf("!!!!");
                    }
                    else{
                        printf("=");
                    }
                }
                curr = curr->next;
            }
            printf("\n");
        }
    }
}

/* Adds pair to heap at correct slot */
void addToHeap(Pair **heap, Pair *p){
    int index = p->num_occurrences;
    if (index > 0){
        if (heap[index] == NULL){
            heap[index] = p;
        }
        else{
            p->next = heap[index];
            heap[index]->prev = p;
            heap[index] = p;
        }
    }
}

/* removes pair from its slot in heap. Assumes the pair is present. */
void removeFromHeap(Pair **heap, Pair *pair){
    if (pair->prev != NULL){
        pair->prev->next = pair->next;
    } else {
        // make first in heap:
        heap[pair->num_occurrences] = pair->next;
    }
    if (pair->next != NULL)
        pair->next->prev = pair->prev;
}

/* removes occurrence from list and adjusts the the heap accordingly */
void removeOcc(Pair **heap, Node *node){
    if (node != NULL && node->pair != NULL){
        Pair *this_pair = node->pair;
        // remove occurrence from linked list:
        if (node->next_occ != NULL)
            node->next_occ->prev_occ = node->prev_occ;
        if (node->prev_occ != NULL)
            node->prev_occ->next_occ = node->next_occ;
        // make first and last if necessary
        if (this_pair->first_occurrence == node)
            this_pair->first_occurrence = this_pair->first_occurrence->next_occ;
        if (this_pair->last_occurrence == node)
            this_pair->last_occurrence = this_pair->last_occurrence->prev_occ;
        // move down the heap:
        removeFromHeap(heap, this_pair);
        this_pair->num_occurrences--;
        addToHeap(heap, this_pair);
    }
}

void addOcc(Pair **h, Pair *p, Node *node){
    removeFromHeap(h, p);
    node->pair = p;
    p->num_occurrences++;
    if (p->first_occurrence == NULL){
        p->first_occurrence = node;
        p->last_occurrence = node;
    }
    else{
        p->last_occurrence->next_occ = node;
        node->prev_occ = p->last_occurrence;
        p->last_occurrence = node;
    }
    addToHeap(h, p);
}

typedef struct {
    int prev_connector_l;
    int prev_connector_r;
    Pair* ref_l;
    Pair* ref_r;
} HashEntry;

Pair *getPairFromHash(HashEntry *hashes, int a, int b, int new_letter){
    Pair *res;
    if (b == new_letter){
        HashEntry this_entry = hashes[a];
        if (this_entry.prev_connector_l != new_letter){
            this_entry.prev_connector_l = new_letter;
            // create new pair:
            Pair new_p;
            new_p.num_occurrences = 0;
            new_p.a = a;
            new_p.b = b;
            new_p.first_occurrence = NULL;
            new_p.last_occurrence = NULL;
            new_p.next = NULL;
            new_p.prev = NULL;
            this_entry.ref_l = &new_p;
        } 
        res = this_entry.ref_l;
    }
    else{
        HashEntry this_entry = hashes[b];
        if (this_entry.prev_connector_r != new_letter){
            this_entry.prev_connector_r = new_letter;
            // create new pair:
            Pair new_p;
            new_p.num_occurrences = 0;
            new_p.a = a;
            new_p.b = b;
            new_p.first_occurrence = NULL;
            new_p.last_occurrence = NULL;
            new_p.next = NULL;
            new_p.prev = NULL;
            this_entry.ref_r = &new_p;
        }
        res = this_entry.ref_r;
    }
    return res;
}


Token* train(int *ids, int n, int k, int init_tokens) {
    Node *nodeMalloc = malloc(sizeof(Node) * n);
    LinkedList l;
    l.size = n;
    l.start = &nodeMalloc[0];
    Node *curr = NULL;
    
    // used for the initial counting:
    Pair *init_counts = malloc(sizeof(Pair)*init_tokens*init_tokens);
    for (int i=0; i<init_tokens; i++){
        for (int j=0; j<init_tokens; j++){
            int index = i*init_tokens+j;
            init_counts[index].num_occurrences = 0;
            init_counts[index].a = i;
            init_counts[index].b = j;
            init_counts[index].first_occurrence = NULL;
            init_counts[index].last_occurrence = NULL;
            init_counts[index].next = NULL;
            init_counts[index].prev = NULL;
        }
    }

    // count initial pairs and build linked list:
    int prev_a = -1;
    int prev_b = -1;
    int prev_added = -1;
    for (int i = 0; i < n; i++) {
        // create new node:
        Node *this_node = &nodeMalloc[i];
        this_node->v = ids[i];
        this_node->l = curr; 
        this_node->r = NULL;
        this_node->prev_occ = NULL;
        this_node->next_occ = NULL;
        this_node->pair = NULL;
        this_node->index = i;
        if (curr != NULL) {
            // link previous node:
            curr->r = this_node;
            int a = ids[i-1];
            int b = ids[i];
            // add occurrence (only if not overlapping):
            if (!(a == prev_a && b == prev_b && prev_added == i-1)){
                prev_a = a;
                prev_b = b;
                prev_added = i;
                int index = a*init_tokens+b;
                // link previous/next occurrence of this pair:
                Pair *curr_pair = &init_counts[index];
                addOcc2Pair(curr_pair, curr);
            }
        }
        curr = this_node;
    }
    
    // Build the heap:
    // find max occurrence first:
    int max_occ = 0;
    for (int i=0; i<init_tokens*init_tokens; i++){
        int this_occs = init_counts[i].num_occurrences;
        if (this_occs > max_occ)
            max_occ = this_occs;
    }
    max_occ++;
    printf("max occ: %d\n", max_occ);
    
    // init heap:
    Pair **heap = malloc(sizeof(Pair*)*(max_occ));
    for (int i=0; i<max_occ; i++)
        heap[i] = NULL;
    
    int heap_max = max_occ-1;
    // fill heap:
    for (int i=0; i<init_tokens*init_tokens; i++){
        Pair *this_pair = &init_counts[i];
        addToHeap(heap, this_pair);
    }
    printL(&l);
    for (int i=0; i<init_tokens; i++){
        for (int j=0; j<init_tokens; j++){
            int index = i*init_tokens+j;
            printPair(&init_counts[index]);
        }
    }
    printHeap(heap, max_occ);

    HashEntry *hashes = malloc(sizeof(HashEntry)*(init_tokens+k+1));
    for (int i=0; i<init_tokens+k+1; i++){
        hashes[i].prev_connector_l = NULL;
        hashes[i].prev_connector_r = NULL;
        hashes[i].ref_l = NULL;
        hashes[i].ref_r = NULL;
    }

    for (int i=0; i<k; i++){
        // extract most frequent pair:
        while (heap_max > 0 && heap[heap_max] == NULL){
            heap_max--;
        }
        if (heap_max < 0){
            printf("Empty\n");
            break;
        }
        Pair *max_pair = heap[heap_max];
        int new_letter = init_tokens+i;
        printf("(%d, %d) -> {%d}", max_pair->a, max_pair->b, new_letter);

        // delete each occurrence:
        Node *curr = max_pair->first_occurrence;
        // for avoiding overlapping counts:
        int prev_l = -1;
        int did_add = 0;
        while (curr != NULL) {
            // skip occurrence:
            removeOcc(heap, curr->l);
            removeOcc(heap, curr->r);
            // skip in doubly linked list:
            if (curr->l != NULL){
                curr->l->r = curr->r;
            }
            else {
                l.start = curr; // make first if necessary: 
            }
            if (curr->r != NULL)
                curr->r->l = curr->l;
            curr->v = new_letter;
            
            // add new occurrences
            if (curr->l != NULL){
                if (!(prev_l == new_letter && curr->l == new_letter && did_add)) { //avoid overlaps
                    Pair *p = getPairFromHash(hashes, curr->l, curr->v, new_letter);
                    prev_l = curr->l;
                    addToHeap(heap, p);
                    addOcc(heap, p, curr->l);
                    did_add = 1;
                } else {
                    did_add = 0;
                }
            }
            if (curr->r != NULL){
                Pair *p = getPairFromHash(hashes, curr->v, curr->r, new_letter);
                addToHeap(heap, p);
                addOcc(heap, p, curr);
            }
            curr = curr->next_occ;
        }
    }
    free(heap);
    free(nodeMalloc);
    free(init_counts);
    return NULL;
}

int main() {
    srand(time(NULL));

    // Generate random array
    int num_elements = 10;
    int init_tokens = 5;
    int* ids = malloc(sizeof(int) * num_elements);
    if (!ids) {
        perror("Failed to allocate memory");
        return 1;
    }

    for (int i = 0; i < num_elements; ++i) {
        ids[i] = rand() % init_tokens;
    }

    int num_tokens = 10000;
    Token* vocab = train(ids, num_elements, num_tokens, init_tokens);

    free(ids);
    return 0;
}
