#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <math.h>
#include <memory>
#include <time.h>
#include <chrono>
#include <algorithm>
#include <iostream>

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
    std::unordered_map<int, std::unique_ptr<std::unordered_set<int>>> pairSets;
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
    // printf("max value: %f := ", h->arr[0].key);
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
        h->pairSets[pair_index] = std::make_unique<std::unordered_set<int>>();
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


float convertToKey(heap *h, int key, int pair_index){
    float res = key + (1-((float)pair_index/(1.2*h->capacity)));
    return res;
}

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
    h.pairSets = std::unordered_map<int, std::unique_ptr<std::unordered_set<int>>>();

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
                h.pairSets[index] = std::make_unique<std::unordered_set<int>>();
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

extern "C"{
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
        // displayList(list, 0);
        // get max pair:
        int best_pair = extractMaxPair(&h);
        if (best_pair == -1){
            printf("break because not enough pairs:(");
            break;
        }
        int max_pair_1 = (int)(best_pair / h.vocabSize);
        int max_pair_2 = best_pair % h.vocabSize;
        // printf("merging %d, %d\n", max_pair_1, max_pair_2);
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
    freeHeap(&h);
    return vocab;
}
}

void printVector(const std::vector<uint16_t>& vec) {
    for (int value : vec) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

struct tokenStat{
    int pair_id;
    int tok_id;
};

void printVector(const std::vector<struct tokenStat>& vec) {
    for (auto &value : vec) {
        std::cout << value.tok_id << " ";
    }
    std::cout << std::endl;
}

void _tokenizeChunk(std::vector<uint16_t> &ids, std::unordered_map<int, uint16_t> &pair_to_tok, int vocab_size){
    // tokenizes a chunk in-place
    std::vector<struct tokenStat> stats; // Vector of struct stat
    stats.reserve(ids.size()); // <- test later for performance
    for (size_t i = 0; i < ids.size()-1; i++)
    {
        uint16_t t_1 = ids[i];
        uint16_t t_2 = ids[i+1];
        int key = t_1 * vocab_size + t_2;
        if (pair_to_tok.find(key) != pair_to_tok.end()){
            // Construct a struct stat and push it into the vector
            stats.push_back({key, pair_to_tok[key]});
        }
    }
    
    // merge and recalculate stats while merging
    while (stats.size() > 0){
        // find the first pair that has to be swapped:
        int min_tok_id = -1;
        int min_pair_id = -1;
        for (size_t i = 0; i < stats.size(); i++)
        {
            if (stats[i].tok_id < min_tok_id || min_tok_id == -1){
                min_tok_id = stats[i].tok_id;
                min_pair_id = stats[i].pair_id;
            }
        }
        // delete every occurrence of the min pair:
        for (size_t i = 0; i < stats.size(); i++)
        {
            if (stats[i].tok_id == min_tok_id){
                // delete element:
                stats[i].tok_id = stats[stats.size()-1].tok_id;
                stats[i].pair_id = stats[stats.size()-1].pair_id;
                stats.pop_back(); // resize stats
                i--; // last element might also be min_tok_id
            }
        }
        int tok_1 = (int)(min_pair_id / vocab_size);
        int tok_2 = min_pair_id % vocab_size;
        // merge every occurrence:
        int i = 0;
        int curr_append = 0;
        int prev = -1;
        while (i < ids.size()){
            if (ids[i] == tok_1 && i < ids.size() - 1 && ids[i+1] == tok_2){
                ids[curr_append] = min_tok_id;
                i += 2;
                if (prev != -1){
                    int pair_id = prev*vocab_size + ids[curr_append];
                    if (pair_to_tok.find(pair_id) != pair_to_tok.end()){
                        stats.push_back({pair_id, pair_to_tok[pair_id]});
                    }
                }
            }
            else{
                ids[curr_append] = ids[i];
                i += 1;
            }
            if (prev == min_tok_id && ids[curr_append] != min_tok_id){
                int pair_id = prev*vocab_size + ids[curr_append];
                if (pair_to_tok.find(pair_id) != pair_to_tok.end()){
                    stats.push_back({pair_id, pair_to_tok[pair_id]});
                }
            }
            prev = ids[curr_append];
            curr_append++;
        }
        int to_pop = ids.size()-curr_append;
        for (size_t i = 0; i < to_pop; i++)
        {
            ids.pop_back();
        }
    }
}

struct tokenizeResult
{
    uint16_t *ids;
    int ids_size;
};


extern "C"{
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
    struct tokenizeResult tokenize(uint8_t *ids, int num_ids, int *splits, int len_splits, int *token_pairs, int token_pairs_count, int vocab_size, int init_tokens){
        // splits denote the places where the string got splitted by regex
        std::vector<std::vector<uint16_t>> splitted;
        splitted.reserve(len_splits);
        for (size_t i = 0; i < len_splits-1; i++)
        {
            int curr = splits[i];
            int next = splits[i+1];
            std::vector<uint16_t> chunk;
            chunk.reserve(next-curr);
            for (size_t j = curr; j < next; j++)
            {
                chunk.emplace_back((uint16_t)ids[j]);
            }
            splitted.emplace_back(chunk);
        }
        
        std::unordered_map<int, uint16_t> pair_to_token(token_pairs_count);
        for (size_t i = 0; i < token_pairs_count; i++)
        {
            pair_to_token[token_pairs[i]] = (uint16_t)(init_tokens + i);
        }

        int total_size = 0;
        for (size_t i = 0; i < splitted.size(); i++)
        {
            _tokenizeChunk(splitted[i], pair_to_token, vocab_size);
            total_size += splitted[i].size();
        }

        uint16_t *result = (uint16_t*)malloc(sizeof(uint16_t)*total_size);
        int c = 0;
        for (size_t i = 0; i < splitted.size(); i++)
        {
            for (size_t j = 0; j < splitted[i].size(); j++)
            {
                result[c] = splitted[i][j];
                c++;
            }
        }
        return tokenizeResult{result, total_size};
    }
}

// int main() {
//     int vocab_size = 10;
//     std::vector<uint8_t> test = {0, 1, 4, 2,
//                                 4, 1,      
//                                 1, 4, 2,      
//                                 4};
//     std::vector<int> splits = {0, 4, 6, 9, 10};
//     std::vector<int> token_pairs = {
//         4*vocab_size + 2, // 5
//         1*vocab_size + 5, // 6
//         0*vocab_size + 6, // 7
//     };
//     struct tokenizeResult res = tokenize(&test[0], test.size(), &splits[0], splits.size(), &token_pairs[0], token_pairs.size(), vocab_size, 5);
//     printf("Result:");
//     for (size_t i = 0; i < res.ids_size; i++)
//     {
//         printf("%d ", res.ids[i]);
//     }
//     return 0;
// }

// int main() {
//     srand(time(NULL)); // Seed for random number generation
//     int num_ids = 10000; // chunk size
//     int *ids = (int *)malloc(num_ids * sizeof(int));

//     // Fill array with random numbers from 1 to 255
//     for (int i = 0; i < num_ids; i++) {
//         ids[i] = rand() % 255 + 1;
//     }

//     int num_tokens = 10000;
//     int init_tokens = 256;
//     struct Token* vocab = train(ids, num_ids, num_tokens, init_tokens);

//     int *vocab_raw = (int*) malloc((num_tokens-init_tokens)*sizeof(int));
//     for (size_t i = 0; i < num_tokens-init_tokens; i++)
//     {
//         int ind = init_tokens + i;
//         int combine_ind = vocab[ind].first_id*num_tokens + vocab[ind].second_id;
//         vocab_raw[i] = combine_ind;
//     }

//     int tok_num_ids = 100000000;
//     int *ids_tok = (int *)malloc(tok_num_ids * sizeof(int));

//     // Fill array with random numbers from 1 to 255
//     for (int i = 0; i < tok_num_ids; i++) {
//         ids_tok[i] = rand() % 255 + 1;
//     }
//     tokenize(ids_tok, tok_num_ids, vocab_raw, num_tokens-init_tokens, num_tokens, init_tokens);

//     // for (int i = 0; i < num_tokens; i++) {
//     //     printf("Token ID: %d, First ID: %d, Second ID: %d, Token List Length: %d, Token List: ",
//     //            vocab[i].token_id, vocab[i].first_id, vocab[i].second_id, vocab[i].token_list_len);
//     //     for (int j = 0; j < vocab[i].token_list_len; j++) {
//     //         printf("%d ", vocab[i].token_list[j]);
//     //     }
//     //     printf("\n");
//     //     free(vocab[i].token_list);
//     // }
//     free(vocab);
//     free(ids); // Free dynamically allocated memory for ids array
//     return 0;
// }