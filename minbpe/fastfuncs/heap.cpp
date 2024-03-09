#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <math.h>
#include <memory>

struct LinkedList {
    int *data;
    int capacity;
    int size;
};

// Function to initialize a new linked list
struct LinkedList createLinkedList(int *arr, int arr_size) {
    struct LinkedList newList = {arr, arr_size, arr_size};
    return newList;
}

// Function to remove a node from the list
void deleteElement(struct LinkedList *list, int index) {
    if (list->data[index] != -2){
        list->size--;
    }
    list->data[index] = -2;
}

void updateElement(struct LinkedList *list, int index, int value) {
    list->data[index] = value;
}

int getNextIndex(struct LinkedList *list, int index){
    int curr_index = index+1;
    while (list->data[curr_index] == -2 && curr_index < list->capacity){
        curr_index++;
    }
    if (curr_index >= list->capacity){
        return -1;
    }
    return curr_index;
}

int getPrevIndex(struct LinkedList *list, int index){
    int curr_index = index-1;
    while (list->data[curr_index] == -2 && curr_index >= 0){
        curr_index--;
    }
    if (curr_index < 0){
        return -1;
    }
    return curr_index;
}

int getNextElement(struct LinkedList *list, int index){
    int next_index = getNextIndex(list, index);
    if (next_index != -1){
        return list->data[next_index];
    }
    return -1;
}

int getPrevElement(struct LinkedList *list, int index){
    int prev_index = getPrevIndex(list, index);
    if (prev_index != -1){
        return list->data[prev_index];
    }
    return -1;
}

// Function to display the doubly linked list
void displayList(struct LinkedList list, int raw) {
    int curr = getNextIndex(&list, -1);
    while (curr != -1){
        printf("%d -- ", list.data[curr]);
        if (raw){
            curr++;
            if (curr >= list.capacity){
                curr = -1;
            }
        }
        else{
            curr = getNextIndex(&list, curr);
        }
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
void increaseKey(heap* h, int position, int newKey);
void decreaseKey(heap* h, int position, int newKey);
void insert(heap* h, int key, int pair_index);

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

// returns the maximum pair as integer:
int extractMaxPair(heap* h)
{
    if (h->size == 0) {
        printf("\nHeap is empty.");
        return -1; // Return NULL to indicate an empty heap
    }
    int result = h->arr[0].pair_index;
    // delete element:
    swap(h, 0, h->size-1);
    h->size--;
    heapify(h, 0);
    return result;
}
 
// add new pair to the heap
void insert(heap* h, int key, int pair_index)
{
    // key := position inside the array, pair_index := index of the pair
    if (h->size < h->capacity) {
        h->arr[h->size].key = key;
        h->arr[h->size].pair_index = pair_index;
        h->pairSets[pair_index] = std::make_unique<std::set<int>>();
        h->pairPositions[pair_index] = h->size;
        bubbleUp(h, h->size);
        h->size++;
    }
    else{
        printf("ERROR on insertion:(\n");
    }
}

void decreaseKey(heap* h, int position, int newKey){
    h->arr[position].key = newKey;
    heapify(h, position);
    // TODO: delete element if newKey=0
}

void increaseKey(heap* h, int position, int newKey){
    h->arr[position].key = newKey;
    bubbleUp(h, position);
}

void printHeap(heap* h)
{
    printf("Heap:\n");
    for (int i = 0; i < h->size; i++) {
        int m_len = h->vocabSize;
        int pair_1 = (int)(h->arr[i].pair_index / m_len);
        int pair_2 = (h->arr[i].pair_index) % m_len;
        printf("Key: %d, Pair: (%d, %d) ", h->arr[i].key, pair_1, pair_2);
        printf("Positions: ");
        int pair_index = h->arr[i].pair_index;
        for (auto& pos : *(h->pairSets[pair_index])) {
            printf("%d ", pos);
        }
        printf("\n");
    }
    printf("\nPair indices:\n");
    for (int i = 0; i < h->size; i++) {
        printf("Position in heap: %d\n", h->pairPositions[h->arr[i].pair_index]);
    }
}

void freeHeap(heap *hp){
    free(hp->arr);
    hp->pairPositions.clear();
    hp->pairSets.clear();
}

// **********************************
// Main functions:
// **********************************

/**
 * @brief Creates a heap object and returns it by value
 *
 * Creates a heap object that is used inside the train function.
 *
 * @param vocab_size The size of the vocabulary.
 * @param ids the list containing the text data.
 * @return A heap object representing the created heap.
 */
heap createHeap(int vocab_size, struct LinkedList *ids)
{
    heap h; // create a heap object on the stack
    int capacity = vocab_size*vocab_size;
    // maybe replace with array, but then a lot of memory
    h.pairSets = std::unordered_map<int, std::unique_ptr<std::set<int>>>();

    // count all pairs initially:
    int last_added_index = 0;
    int last_added_pair_index = -1;
    for (int i = 0; i < ids->size-1; i++){
        int first_el = ids->data[i];
        int second_el = ids->data[i + 1];
        int index = first_el*vocab_size + second_el;
        if (last_added_pair_index == index && last_added_index == i-1){
            // don't count pairs of same tokens too often. For example in [1, 1, 1]
            // the pair (1, 1) should only be counted once
            continue;
        }
        if (first_el != 0 && second_el != 0){
            if (h.pairSets.find(index) == h.pairSets.end()){
                h.pairSets[index] = std::make_unique<std::set<int>>();
            }
            h.pairSets[index]->insert(i);
        }
        last_added_index = i;
        last_added_pair_index = index;
    }

    h.pairPositions = std::unordered_map<int, int>(); // initialize the map with the constructor
    h.size = 0;
    h.capacity = capacity;
    h.vocabSize = vocab_size;
    h.arr = (heapNode*)malloc(capacity * sizeof(heapNode));
 
    if (h.arr == NULL) {
        printf("Memory error");
        return h;
    }
    // build inital heap:
    int count = 0;
    for (const auto& pair : h.pairSets) {
        int key = pair.first;
        int size = pair.second->size();
        h.arr[count].key = size;
        h.arr[count].pair_index = key;
        h.pairPositions[key] = count;
        count++;
    }

    // bottom up heap construction (takes O(n) time):
    h.size = count;
    count = (h.size - 2) / 2;
    while (count >= 0) {
        heapify(&h, count);
        count--;
    }
    return h; // return the heap object by value
}

void RemovePosition(heap *h, int index, int tok_1, int tok_2, int disallowed){
    // index denotes the start position of the pair inside the linked list
    // the disallowed token only ensures that the pair is only removed when
    // it is unequal to the disallowed argument. This is needed for the repeating
    // tokens problem.
    if (tok_1 <= 0 || tok_2 <= 0){ // ignore if separated by 0 or out of bound
        return;
    }
    // removes stored pair position and updates the heap
    int pair_index = tok_1 * h->vocabSize + tok_2;
    if (pair_index == disallowed){
        return;
    }
    auto& pairSet = h->pairSets[pair_index];
    pairSet->erase(index);
    int new_len = pairSet->size();
    decreaseKey(h, h->pairPositions[pair_index], new_len);
}



void AddPosition(heap *h, int index, int tok_1, int tok_2){
    // index denotes the start position of the pair inside the linked list
    if (tok_1 <= 0 || tok_2 <= 0){ // ignore if separated by 0 or out of bound
        return;
    }
    // adds a new pair position and updates the heap
    int pair_index = tok_1 * h->vocabSize + tok_2;
    if (h->pairSets.find(pair_index) == h->pairSets.end()){
        // if not added yet (unseen pair), add it to the heap
        insert(h, 0, pair_index);
    }
    auto& pairSet = h->pairSets[pair_index];
    pairSet->insert(index);
    int new_len = pairSet->size();
    increaseKey(h, h->pairPositions[pair_index], new_len);
}

// used as return values. Stores every nessesary information:
struct Token {
    int token_id; // the id of the token
    int first_id; // the first element of the pair
    int second_id; // the second element of the pair
    int token_list_len; // the length of the corresponding tokens
    int* token_list; // the corresponding tokens
};

/**
 * @brief Main algorithm for training tokenization model.
 *
 * This function implements the main algorithm for training a tokenization model
 * based on the provided integer IDs. https://aclanthology.org/2023.findings-acl.38.pdf
 *
 * @param ids An array containing the input IDs.
 * @param num_ids The number of IDs in the 'ids' array.
 * @param num_tokens the number of tokens we want to train in total.
 * @param init_tokens The number of initial tokens to start with.
 * @return A pointer to a struct Token representing the trained model.
 *         Note: Memory for the struct Token and its contents should be managed appropriately.
 */
struct Token* train(int* ids, int num_ids, int num_tokens, int init_tokens) {
    // build vocab
    struct Token* vocab = (struct Token*)malloc(num_tokens * sizeof(struct Token));
    // build initial vocab:
    for (int i = 0; i < num_tokens; i++) {
        int a_len = 1;
        vocab[i].token_id = i;
        vocab[i].first_id = i;
        vocab[i].second_id = i;
        vocab[i].token_list_len = a_len;
        vocab[i].token_list = (int*)malloc(a_len * sizeof(int));
        for (int j = 0; j < a_len; j++) {
            vocab[i].token_list[j] = i;
        }
    }

    // build inital heap:
    struct LinkedList list = createLinkedList(ids, num_ids);
    heap h = createHeap(num_tokens, &list);

    // number of merges we still need:
    int total_merges = num_tokens-init_tokens;

    // main algorithm:
    for (size_t i = 0; i < total_merges; i++)
    {
        // get max pair:
        int best_pair = extractMaxPair(&h);
        if (best_pair == -1){
            printf("break because not enough pairs:(");
            break;
        }
        int max_pair_1 = (int)(best_pair / h.vocabSize);
        int max_pair_2 = best_pair % h.vocabSize;
        printf("merging %d, %d\n", max_pair_1, max_pair_2);
        // store new token in the final output:
        int new_token_id = init_tokens + i;
        vocab[new_token_id].token_id = new_token_id;
        vocab[new_token_id].first_id = max_pair_1;
        vocab[new_token_id].second_id = max_pair_2;
        int pair_1_len = vocab[max_pair_1].token_list_len;
        int pair_2_len = vocab[max_pair_2].token_list_len;
        int new_tokens_len = pair_1_len + pair_2_len;
        vocab[new_token_id].token_list_len = new_tokens_len;
        vocab[new_token_id].token_list = (int*)malloc(new_tokens_len * sizeof(int));
        // create new list of tokens by merging the old ones:
        for (size_t i = 0; i < pair_1_len; i++)
        {
            vocab[new_token_id].token_list[i] = vocab[max_pair_1].token_list[i];
        }
        for (size_t i = 0; i < pair_2_len; i++)
        {
            vocab[new_token_id].token_list[i+pair_1_len] = vocab[max_pair_2].token_list[i];
        }
        auto& pairSet = h.pairSets[best_pair];
        int last_added_index = -1;
        for (auto& pos: *pairSet) {
            if (pos == last_added_index){ // needed for repeating tokens
                continue;
            }
            // pos = position inside the linked list
            int w1 = list.data[pos];
            int w1_prev_ind = getPrevIndex(&list, pos);
            int w1_prev = -1;
            if (w1_prev_ind != -1){
                w1_prev = list.data[w1_prev_ind];
            }
            int w2_ind = getNextIndex(&list, pos);
            int w2 = list.data[w2_ind]; // has to exist, otherwise we made a mistake
            int w2_next = getNextElement(&list, w2_ind);
            RemovePosition(&h, w1_prev_ind, w1_prev, w1, best_pair);
            RemovePosition(&h, w2_ind, w2, w2_next, best_pair);
            int next_id = getNextIndex(&list, pos);
            // merge tokens to create new token:
            deleteElement(&list, next_id);
            updateElement(&list, pos, new_token_id);
            // add new pairs:
            AddPosition(&h, pos, new_token_id, w2_next);
            AddPosition(&h, w1_prev_ind, w1_prev, new_token_id);
            last_added_index = next_id;
        }
    }
    displayList(list, 0);
    freeHeap(&h);
    return vocab;
}

void RemovePositionFromPairs(std::unordered_map<int, std::unique_ptr<std::set<int>>> &pair_sets, 
                            int index, int tok_1, int tok_2, int vocab_size) {
    if (tok_1 <= 0 || tok_2 <= 0) { // ignore if separated by 0 or out of bound
        return;
    }
    int pair_index = tok_1 * vocab_size + tok_2;
    auto& pairSet = pair_sets[pair_index];
    pairSet->erase(index);
}

void AddPositionToPairs(std::unordered_map<int, std::unique_ptr<std::set<int>>> &pair_sets, 
                        int index, int tok_1, int tok_2, int vocab_size) {
    if (tok_1 <= 0 || tok_2 <= 0) { // ignore if separated by 0 or out of bound
        return;
    }
    int pair_index = tok_1 * vocab_size + tok_2;
    if (pair_sets.find(pair_index) == pair_sets.end()) {
        // if not added yet (unseen pair), add it to the corresponding set
        pair_sets[pair_index] = std::make_unique<std::set<int>>();
    }
    auto& pairSet = *pair_sets[pair_index];
    pairSet.insert(index);
}

/**
 * @brief Tokenizes text given by an array of IDs and uses token pairs to decode the text.
 *
 * This function tokenizes text represented by an array of integer IDs using the provided token pairs
 * to decode the text.
 *
 * @param ids An array containing the text to be tokenized.
 * @param num_ids The length of the 'ids' array.
 * @param token_pairs An array containing token pairs obtained from the train function.
 * @param token_pairs_count The number of token pairs in the 'token_pairs' array.
 * @param vocab_size The number of tokens used in the tokenizer.
 * @return A pointer to an array of integers representing the tokenized text.
 *         This array needs to be freed after use.
 */
int* tokenize(int *ids, int num_ids, int *token_pairs, int token_pairs_count, int vocab_size, int init_tokens){
    struct LinkedList list = createLinkedList(ids, num_ids);
    std::unordered_map<int, std::unique_ptr<std::set<int>>> pair_positions;

    // count all pairs initially:
    int last_added_index = 0;
    int last_added_pair_index = -1;
    for (int i = 0; i < num_ids-1; i++){
        int first_el = ids[i];
        int second_el = ids[i + 1];
        int index = first_el*vocab_size + second_el;
        if (pair_positions.find(index) == pair_positions.end()){
            pair_positions[index] = std::make_unique<std::set<int>>();
        }
        pair_positions[index]->insert(i);
        last_added_index = i;
        last_added_pair_index = index;
    }

    for (size_t i = 0; i < token_pairs_count; i++)
    {
        int curr_pair = token_pairs[i];
        int tok_1 = (int)curr_pair/vocab_size;
        int tok_2 = curr_pair%vocab_size;
        auto& pairSet = pair_positions[curr_pair];
        int last_added_index = -1;
        int new_token_id = init_tokens + i;

        for (auto& pos: *pairSet) {
            if (pos == last_added_index){ // needed for repeating tokens
                continue; 
            }
            // pos = position inside the linked list
            int w1 = list.data[pos];
            int w1_prev_ind = getPrevIndex(&list, pos);
            int w1_prev = -1;
            if (w1_prev_ind != -1){
                w1_prev = list.data[w1_prev_ind];
            }
            int w2_ind = getNextIndex(&list, pos);
            int w2 = list.data[w2_ind]; // has to exist, otherwise we made a mistake
            int w2_next = getNextElement(&list, w2_ind);
            RemovePositionFromPairs(pair_positions, w1_prev_ind, w1_prev, w1, vocab_size);
            RemovePositionFromPairs(pair_positions, w2_ind, w2, w2_next, vocab_size);
            int next_id = getNextIndex(&list, pos);
            // merge tokens to create new token:
            deleteElement(&list, next_id);
            updateElement(&list, pos, new_token_id);
            // add new pairs:
            AddPositionToPairs(pair_positions, pos, new_token_id, w2_next, vocab_size);
            AddPositionToPairs(pair_positions, w1_prev_ind, w1_prev, new_token_id, vocab_size);
            last_added_index = next_id;
        }
    }
    int *result = (int *)malloc(list.size*sizeof(int));
    int curr = getNextIndex(&list, -1);
    int curr_ind = 0;
    while (curr != -1){
        result[curr_ind] = list.data[curr];
        curr = getNextIndex(&list, curr);
        curr_ind++;
    }
    return result;
}

// int main() {
//     int ids[] = {3, 4, 0, 4, 3, 4, 0, 5, 2, 5, 1, 0, 2, 0, 2, 0, 1, 2};
//     int num_ids = sizeof(ids) / sizeof(ids[0]);
//     int num_tokens = 15; // Choose an appropriate value
//     int init_tokens = 10; // Choose an appropriate value
//     struct Token* vocab = train(ids, num_ids, num_tokens, init_tokens);

//     for (int i = 0; i < num_tokens; i++) {
//         printf("Token ID: %d, First ID: %d, Second ID: %d, Token List Length: %d, Token List: ",
//                vocab[i].token_id, vocab[i].first_id, vocab[i].second_id, vocab[i].token_list_len);
//         for (int j = 0; j < vocab[i].token_list_len; j++) {
//             printf("%d ", vocab[i].token_list[j]);
//         }
//         printf("\n");
//         free(vocab[i].token_list);
//     }
//     free(vocab);
//     return 0;
// }