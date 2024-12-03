#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <time.h>

typedef struct Node {
    int v;
    int block; // used to prevent certain types of merges
    struct Node *l;
    struct Node *r;
    struct Node *prev_occ;
    struct Node *next_occ;
    struct Pair *pair;
} Node;

typedef struct {
    Node *start;
    int size;
} LinkedList;

typedef struct Pair {
    int a;
    int b;
    int num_occurrences;
    // first occurrence:
    Node *first_occurrence;
    Node *last_occurrence;
    // points to next/prev pair, has same number of occurrences (later used for heap):
    struct Pair *next;
    struct Pair *prev;  
} Pair;

void printL(LinkedList *L) {
    printf("List: \n");
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
                if (curr->next_occ->prev_occ == NULL){
                    printf("|!!(NULL)<-!!|!!->(%d)!!|", curr->next_occ->v);
                }
                else{
                    printf("|!!(%d)<-!!|!!->(%d)!!|", curr->next_occ->prev_occ->v, curr->next_occ->v);
                }
            }
        }
        if (curr->prev_occ != NULL){
            if (curr->prev_occ->next_occ != curr){
                if (curr->prev_occ->next_occ == NULL){
                    printf("|!?!(NULL)<-!?!|!?!->(%d)!?!|", curr->prev_occ->v);
                }
                else{
                    printf("|!?!(%d)<-!!|!!->(%d)!?!|", curr->prev_occ->next_occ->v, curr->prev_occ->v);
                }
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
    printf("(%d, %d) (occs: %d): ", p->a, p->b, p->num_occurrences);
    Node *curr = p->first_occurrence;
    while (curr != NULL) {
        printf("%d", curr->block);
        if (curr->next_occ != NULL){
            if (curr->next_occ->prev_occ != curr) {
                if (curr->next_occ->prev_occ == NULL){
                    printf("!!(NULL)<-!!");
                }
                else{
                    printf("!!(%d)<-!!", curr->next_occ->prev_occ->v);
                }
            }
            else {
                printf("=");
            }
        }
        curr = curr->next_occ;
    }
    printf("\n");
}

void printNode(Node *node){
    printf("Node: (ind: %d, val: %d)", node->block, node->v);
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
                        if (curr->next->prev == NULL){
                            printf("!!((NULL)<-)!!");
                        } else{
                            printf("!!((%d, %d)<-)!!", curr->next->prev->a, curr->next->prev->b);
                        }
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
    // print every pair:
    printf("Pairs: \n");
    for (int i=0; i<heapsize; i++){
        if (h[i] != NULL){
            Pair *curr = h[i];
            while (curr != NULL){
                printPair(curr);
                curr = curr->next;
            }
        }
    }
    printf("\n");
}

void removeFromList(LinkedList *l, Node *node){
    if (node->l != NULL){
        node->l->r = node->r;
    }
    else {
        l->start = node; // make first if necessary: 
    }
    if (node->r != NULL)
        node->r->l = node->l;
}

/* Adds pair to heap at correct slot */
void addToHeap(Pair **h, Pair *p){
    //printf("ADD (%d, %d) %d:\n",p->a, p->b, p->num_occurrences);
    int index = p->num_occurrences;
    if (index > 0){
        if (h[index] == NULL){
            h[index] = p;
        }
        else{
            p->next = h[index];
            h[index]->prev = p;
            h[index] = p;
        }
    }
}

/* removes pair from its slot in heap. Assumes the pair is present. */
void removeFromHeap(Pair **h, Pair *p){
    //printf("REM (%d, %d) %d:\n",p->a, p->b, p->num_occurrences);
    if (p->prev != NULL){
        p->prev->next = p->next;
    } else {
        // make first in heap:
        h[p->num_occurrences] = p->next;
    }
    if (p->next != NULL)
        p->next->prev = p->prev;
    p->next = NULL;
    p->prev = NULL;
}

void removeOccFromPair(Pair *p, Node *node){
    // remove occurrence from linked list:
    if (node->next_occ != NULL)
        node->next_occ->prev_occ = node->prev_occ;
    if (node->prev_occ != NULL)
        node->prev_occ->next_occ = node->next_occ;
    // make first and last if necessary
    if (p->first_occurrence == node)
        p->first_occurrence = node->next_occ;
    if (p->last_occurrence == node)
        p->last_occurrence = node->prev_occ;
    node->prev_occ = NULL;
    node->next_occ = NULL;
    node->pair = NULL;
    p->num_occurrences--;
}

/* removes occurrence from list and adjusts the the heap accordingly */
void removeOcc(Pair **heap, Node *node){
    if (node != NULL && node->pair != NULL){
        Pair *this_pair = node->pair;
        // move down the heap:
        removeFromHeap(heap, this_pair);
        removeOccFromPair(this_pair, node);
        addToHeap(heap, this_pair);
    }
}

void addOcc2Pair(Pair *p, Node *node){
    node->pair = p;
    p->num_occurrences++;
    if (p->first_occurrence == NULL){
        p->first_occurrence = node;
        p->last_occurrence = node;
        // node->prev_occ = NULL; // does not work like that because start/end node not updated maybe
        // node->next_occ = NULL;
    }
    else{
        p->last_occurrence->next_occ = node;
        node->prev_occ = p->last_occurrence;
        // node->next_occ = NULL;
        p->last_occurrence = node;
    }
}

void addOcc(Pair **h, Pair *p, Node *node){
    removeFromHeap(h, p);
    addOcc2Pair(p, node);
    addToHeap(h, p);
}

typedef struct {
    int prev_connector_l;
    int prev_connector_r;
    Pair* ref_l;
    Pair* ref_r;
} HashEntry;

Pair *getPairFromHash(Pair **heap, HashEntry *hashes, int a, int b, int new_letter){
    if (b == new_letter){
        HashEntry *this_entry = &hashes[a];
        if (this_entry->prev_connector_l != new_letter){
            //printf("\nNew!!!(%d, %d) (prev conn: %d)\n", a, b, this_entry->prev_connector_l);
            this_entry->prev_connector_l = new_letter;
            Pair *new_p = malloc(sizeof(Pair));
            *new_p = (Pair) {a, b, 0, NULL, NULL, NULL, NULL};
            this_entry->ref_l = new_p;
            addToHeap(heap, new_p);
        } 
        return this_entry->ref_l;
    }
    else{
        HashEntry *this_entry = &hashes[b];
        if (this_entry->prev_connector_r != new_letter){
            //printf("\nNew!!!(%d, %d) (prev conn: %d)\n", a, b, this_entry->prev_connector_r);
            this_entry->prev_connector_r = new_letter;
            Pair *new_p = malloc(sizeof(Pair));
            *new_p = (Pair) {a, b, 0, NULL, NULL, NULL, NULL};
            this_entry->ref_r = new_p;
            addToHeap(heap, new_p);
        }
        return this_entry->ref_r;
    }
}

typedef struct { 
    int a; // the id of the token
    int b; // the first element of the pair
    int c; // the second element of the pair
    int token_list_len; // the length of the corresponding tokens
    int* token_list; // the corresponding tokens
} Merge;

/**
 * @brief Main algorithm for training tokenization model.
 *
 * This function implements the main algorithm for training a tokenization model
 * based on the provided integer IDs. https://aclanthology.org/2023.findings-acl.38.pdf
 *
 * @param ids An array containing the input IDs.
 * @param n The number of IDs in the 'ids' array.
 * @param k the number of merge rounds.
 * @param init_tokens The number of different initial tokens to start with.
 * @return A pointer to a struct Token representing the trained model.
 */
Merge* train(int *ids, int n, int k, int init_tokens) {
    Merge* vocab = malloc((init_tokens+k) * sizeof(Merge));

    // build initial vocab:
    for (int i = 0; i < init_tokens+k; i++) {
        int a_len = 1;
        vocab[i].a = i;
        vocab[i].b = i;
        vocab[i].c = i;
        vocab[i].token_list_len = a_len;
        vocab[i].token_list = (int*)malloc(a_len * sizeof(int));
        for (int j = 0; j < a_len; j++) {
            vocab[i].token_list[j] = i;
        }
    }

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
            init_counts[index] = (Pair) {i, j, 0, NULL, NULL, NULL, NULL};
        }
    }

    // count initial pairs and build linked list:
    for (int i = 0; i < n; i++) {
        // create new node:
        nodeMalloc[i] = (Node){ids[i], i, curr, NULL, NULL, NULL, NULL};
        Node *this_node = &nodeMalloc[i];
        if (ids[i] == 0) {
            this_node->block = -1;
        }
        if (curr != NULL) {
            // link previous node:
            curr->r = this_node;
            int a = ids[i-1];
            int b = ids[i];
            
            if (this_node->block != -1 && curr->block != -1){
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
    HashEntry *hashes = malloc(sizeof(HashEntry)*(init_tokens+k+1));
    for (int i=0; i<init_tokens+k+1; i++){
        hashes[i] = (HashEntry) {-1, -1, NULL, NULL};
    }
    for (int i=0; i<k; i++){
        // extract most frequent pair:
        while (heap_max > 0 && heap[heap_max] == NULL){
            heap_max--;
        }
        if (heap_max <= 0){
            break;
        }
        Pair *max_pair = heap[heap_max];
        int new_letter = init_tokens+i;

        // fill in new token:
        vocab[new_letter].a = max_pair->a;
        vocab[new_letter].b = max_pair->b;
        vocab[new_letter].c = new_letter;
        int pair_1_len = vocab[max_pair->a].token_list_len;
        int pair_2_len = vocab[max_pair->b].token_list_len;
        int new_tokens_len = pair_1_len + pair_2_len;
        vocab[new_letter].token_list_len = new_tokens_len;
        vocab[new_letter].token_list = (int*)malloc(new_tokens_len * sizeof(int));
        for (int i = 0; i < pair_1_len; i++){
            vocab[new_letter].token_list[i] = vocab[max_pair->a].token_list[i];
        }
        for (int i = 0; i < pair_2_len; i++){
            vocab[new_letter].token_list[i+pair_1_len] = vocab[max_pair->b].token_list[i];
        }

        // delete each occurrence:
        Node *curr = max_pair->first_occurrence;
        // remove from heap
        removeFromHeap(heap, max_pair);
        while (curr != NULL) {
            Node *next_occ = curr->next_occ;
            removeOccFromPair(max_pair, curr); // maybe inefficient that way
            if (next_occ != NULL && next_occ->l == curr){
                Node *next_next_occ = next_occ->next_occ;
                removeOccFromPair(max_pair, next_occ);
                next_occ = next_next_occ; // skip next occurrence
            }
            // skip occurrence:
            removeOcc(heap, curr->l);
            removeOcc(heap, curr->r);
            // skip in doubly linked list:
            removeFromList(&l, curr->r);
            curr->v = new_letter;
            // add new occurrences
            if (curr->l != NULL && curr->l->block != -1){
                Pair *p = getPairFromHash(heap, hashes, curr->l->v, new_letter, new_letter);
                addOcc(heap, p, curr->l);
            }
            if (curr->r != NULL && curr->r->block != -1){
                Pair *p = getPairFromHash(heap, hashes, new_letter, curr->r->v, new_letter);
                addOcc(heap, p, curr);
            }
            curr = next_occ;
        }
    }
   
    // free pairs created during merging:
    while (heap_max >= 0){
        if (heap[heap_max] != NULL){
            Pair *curr = heap[heap_max];
            while (curr != NULL){
                Pair *next = curr->next;
                if (!(curr->a < init_tokens && curr->b < init_tokens)){
                    free(curr); 
                }
                curr = next;
            }
        }
        heap_max--;
    }
    free(init_counts);
    free(hashes);
    free(nodeMalloc);
    return vocab;
}

typedef struct {
    int *st;
    int len;
} MergedList;

MergedList merge(int *s, int len_s, Merge m){
    MergedList res;
    res.st = malloc(sizeof(int)*len_s);
    int curr_ind_new_s = 0;
    int i = 0;
    while (i < len_s){
        if (m.a == s[i] && i < len_s-1 && m.b == s[i + 1]){
            res.st[curr_ind_new_s] = m.c;
            curr_ind_new_s++;
            i += 2;
        }
        else{
            res.st[curr_ind_new_s] = s[i];
            curr_ind_new_s++;
            i += 1;
        }
    }
    free(s);
    res.len = curr_ind_new_s;
    return res;
}

int verifySolution(int *s, int n, int k, int init_tokens, Merge *res){
    int *this_s = malloc(sizeof(int)*n);
    memcpy(this_s, s, sizeof(int)*n);
    int alph = init_tokens + k;
    int *counts = malloc(sizeof(int)*alph*alph);
    int curr_len = n;
    int is_ok = 1;
    for (int curr_k=0; curr_k<k; curr_k++){

        for (int i=0; i<alph*alph; i++){
            counts[i] = 0;
        }
        for (int i=0; i<curr_len-1; i++){
            int a = this_s[i];
            int b = this_s[i+1];
            int index = a*alph+b;
            if (a != 0 && b != 0)
                counts[index] += 1;
        }
        int max_occ = 0;
        int best_a; 
        int best_b; 
        for (int i=0; i<alph*alph; i++){
            if (max_occ < counts[i]){
                best_a = (int)(i/alph);
                best_b = i%alph;
                max_occ = counts[i];
            }
        }
        if (max_occ == 0){
            break;
        }
        int this_occ = counts[res[curr_k].a*alph+res[curr_k].b];
        if (this_occ < max_occ) {
            is_ok = 0;
            printf("ERROR. got: ((%d, %d), occs: %d); best: ((%d, %d), occs: %d)\n", res[curr_k].a, res[curr_k].b, this_occ, best_a, best_b, max_occ);
            break;
        }

        // printf("ok: %d, %d\n", this_occ, max_occ);
        MergedList merged = merge(this_s, curr_len, res[curr_k]);
        curr_len = merged.len;
        this_s = merged.st;
    }
    free(counts);
    free(this_s);
    return is_ok;
}

int main() {
    srand(time(NULL));
    // Generate random array
    for (int test=0; test<1; test++){
        // printf("test: %d\n", test+1);
        int n = 10000000;
        int k = 50000;
        int init_tokens = 10;
        int* ids = malloc(sizeof(int) * n);
        if (!ids) {
            perror("Failed to allocate memory");
            return 1;
        }
        for (int i = 0; i < n; ++i) {
            ids[i] = rand() % init_tokens;
        }
        Merge *res = train(ids, n, k, init_tokens);
        // int ver = verifySolution(ids, n, k, init_tokens, res);
        free(ids);
    }
    return 0;
}