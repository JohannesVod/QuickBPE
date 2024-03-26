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
#include <thread>

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
void displayList(struct LinkedList *list, int raw) {
    int curr = getNextIndex(list, -1);
    while (curr != -1){
        if (raw){
            curr++;
            if (curr >= list->capacity){
                curr = -1;
            }
        }
        else{
            curr = getNextIndex(list, curr);
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

struct HeapPosition{
    int index;
    int inner_index;
};

struct HeapElement{
    std::unordered_set<int> positions;
    uint64_t this_pair;
};

struct Heap {
    std::unordered_map<uint64_t, struct HeapPosition> pair_to_position;
    std::unordered_map<int, std::vector<struct HeapElement*>> heap_data;
    int curr_max;
    int vocab_size;
};

void PrintHeap(Heap *h){
    for (size_t i = 0; i < h->curr_max+1; i++)
    {
        if (h->heap_data.find(i) != h->heap_data.end()){
            printf("pos %d: ", i);
            int c = 0;
            for (struct HeapElement *heap_el : h->heap_data[i]){
                int p1 = (int)(heap_el->this_pair/h->vocab_size);
                int p2 = heap_el->this_pair%h->vocab_size;
                printf("\n(%d, %d)|", p1, p2);
                for (auto& pos : heap_el->positions){
                    printf(" %d, ", pos);
                }
                c++;
            }
            printf("\n");
        }
    }
    printf("\n");
    for (auto& el: h->pair_to_position){
        int tok_1 = (int)el.first/h->vocab_size;
        int tok_2 = el.first%h->vocab_size;
        printf("index of %d, %d is %d->%d\n", tok_1, tok_2, h->pair_to_position[el.first].index, h->pair_to_position[el.first].inner_index);
    }
}

typedef struct Heap heap;
// returns the maximum pair as integer:
struct HeapElement* extractMaxPair(heap &h)
{
    while (h.heap_data.find(h.curr_max) == h.heap_data.end() && h.curr_max > 0){
        h.curr_max--;
    }
    if (h.curr_max <= 0) {
        HeapElement* empty_result = new HeapElement{std::unordered_set<int>(), 0};
        return empty_result; // Return -1 to indicate an empty heap
    }
    HeapElement *result = h.heap_data[h.curr_max].back();
    h.heap_data[h.curr_max].pop_back();
    if (h.heap_data[h.curr_max].size() == 0){
        h.heap_data.erase(h.curr_max);
    }
    h.pair_to_position.erase(result->this_pair);
    return result;
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
heap createHeap(int vocab_size, struct LinkedList &ids, int init_tokens)
{
    heap h; // create a heap object on the stack
    int capacity = vocab_size*vocab_size;
    // maybe replace with array, but then a lot of memory
    h.heap_data = std::unordered_map<int, std::vector<struct HeapElement*>>();
    {
        //auto start = std::chrono::steady_clock::now();
        // performance improvement: store positions in vector first and then add to sets
        std::vector<std::vector<int>> pair_positions_fast(init_tokens*init_tokens);
        // count all pairs initially:
        for (int i = 0; i < ids.size-1; i++){
            int first_el = ids.data[i];
            int second_el = ids.data[i + 1];
            int index = first_el*init_tokens + second_el;
            if (first_el != 0 && second_el != 0){
                pair_positions_fast[index].push_back(i);
            }
        }
        // for (size_t i = 0; i < init_tokens * init_tokens; i++) {
        //     int max_pair_1 = (int)(i / init_tokens);
        //     int max_pair_2 = i % init_tokens;
        //     if (pair_positions_fast[i].size() > 0){
        //         printf("pair %d,%d: positions ", max_pair_1, max_pair_2);
        //         for (auto &el:pair_positions_fast[i])
        //         {
        //             printf("%d,", el);
        //         }
        //         printf("\n");
        //     }
        // }

        //auto end = std::chrono::steady_clock::now();
        //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        //std::cout << "Elapsed time arr: " << duration.count() << " milliseconds" << std::endl;
        for (size_t i = 0; i < init_tokens * init_tokens; i++)
        {
            int tok_1 = (int)i/init_tokens;
            int tok_2 = i%init_tokens;
            uint64_t real_index = (uint64_t)tok_1*vocab_size + tok_2;
            int data_len = pair_positions_fast[i].size();
            if (data_len == 0){
                continue;
            }
            if (h.heap_data.find(data_len) == h.heap_data.end()){
                h.heap_data[data_len] = std::vector<struct HeapElement*>();
            }
            
            h.pair_to_position[real_index] = HeapPosition{data_len, (int)h.heap_data[data_len].size()};
            // insertion is much faster here because the array of the hashmap is loaded into cache:
            auto set_positions = std::unordered_set<int>(pair_positions_fast[i].size());
            for (size_t j = 0; j < pair_positions_fast[i].size(); j++)
            {
                set_positions.insert(pair_positions_fast[i][j]);
            }
            HeapElement* heapElement = new HeapElement{set_positions, real_index};
            h.heap_data[data_len].push_back(heapElement); // maybe bad performance?
        }
        //end = std::chrono::steady_clock::now();
        //duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        //std::cout << "Elapsed time dict: " << duration.count() << " milliseconds" << std::endl;
    }
    h.curr_max = ids.size+1;
    h.vocab_size = vocab_size;
    return h; // return the heap object by value
}

void RemovePosition(heap &h, int index, int tok_1, int tok_2, int disallowed){
    // index denotes the start position of the pair inside the linked list
    // the disallowed token only ensures that the pair is only removed when
    // it is unequal to the disallowed argument. This is needed for the repeating
    // tokens problem.
    if (tok_1 <= 0 || tok_2 <= 0){ // ignore if separated by 0 or out of bound
        return;
    }
    // removes stored pair position and updates the heap
    uint64_t pair_index = (uint64_t)tok_1 * h.vocab_size + tok_2;
    if (pair_index == disallowed){
        return;
    }
    HeapPosition &pos = h.pair_to_position[pair_index];
    HeapElement *res = h.heap_data[pos.index][pos.inner_index]; // seg fault?
    // remove old element:
    std::vector<HeapElement*> &stack = h.heap_data[pos.index];
    
    stack[pos.inner_index] = stack[stack.size()-1];
    h.pair_to_position[stack[pos.inner_index]->this_pair].inner_index = pos.inner_index;
    // add to new stack:
    int pos_save = pos.index;
    pos.index--;
    res->positions.erase(index);
    if (res->positions.size() != 0){
        if (h.heap_data.find(pos.index) == h.heap_data.end()){
            h.heap_data[pos.index] = std::vector<struct HeapElement*>();
        }
        h.heap_data[pos.index].push_back(res);
        pos.inner_index = (int)h.heap_data[pos.index].size()-1;
    }
    else{
        h.pair_to_position.erase(res->this_pair);
        delete res; // free memory
    }
    // delete at old position:
    stack.pop_back();
    if (stack.size() == 0){
        h.heap_data.erase(pos_save);
    }
}

void AddPosition(heap &h, int position, int tok_1, int tok_2){
    // index denotes the start position of the pair inside the linked list
    if (tok_1 <= 0 || tok_2 <= 0){ // ignore if separated by 0 or out of bound
        return;
    }
    // adds a new pair position and updates the heap
    uint64_t pair_index = (uint64_t) tok_1 * h.vocab_size + tok_2;
    if (h.pair_to_position.find(pair_index) == h.pair_to_position.end()){
        // if not added yet (unseen pair), add it to the heap
        if (h.heap_data.find(1) == h.heap_data.end()){
            h.heap_data[1] = std::vector<struct HeapElement*>();
        }
        HeapElement* heapElement = new HeapElement{std::unordered_set<int>(), pair_index};
        heapElement->positions.insert(position);
        h.heap_data[1].push_back(heapElement);
        h.pair_to_position[pair_index] = HeapPosition{1, (int)h.heap_data[1].size()-1};
        return;
    }

    HeapPosition &pos = h.pair_to_position[pair_index];
    HeapElement *res = h.heap_data[pos.index][pos.inner_index];
    std::vector<HeapElement*> &stack = h.heap_data[pos.index];
    stack[pos.inner_index] = stack[stack.size()-1];
    h.pair_to_position[stack[pos.inner_index]->this_pair].inner_index = pos.inner_index;
    // add to new stack:
    int pos_save = pos.index;
    pos.index++;
    if (h.heap_data.find(pos.index) == h.heap_data.end()){
        h.heap_data[pos.index] = std::vector<struct HeapElement*>();
    }
    if (pos.index > h.curr_max){
        h.curr_max = pos.index;
    }
    res->positions.insert(position);
    h.heap_data[pos.index].push_back(res);
    pos.inner_index = (int)h.heap_data[pos.index].size()-1;
    // delete at old position:
    stack.pop_back();
    if (stack.size() == 0){
        h.heap_data.erase(pos_save);
    }
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
    heap h = createHeap(num_tokens, list, init_tokens);
    // number of merges we still need:
    int total_merges = num_tokens-init_tokens;

    // main algorithm:
    for (size_t i = 0; i < total_merges; i++)
    {
        // displayList(list, 0);
        // get max pair:
        struct HeapElement* best_pair = extractMaxPair(h);
        if (best_pair->this_pair == 0){
            delete best_pair;
            printf("break because not enough pairs:(");
            break;
        }
        
        int max_pair_1 = (int)(best_pair->this_pair / h.vocab_size);
        int max_pair_2 = best_pair->this_pair % h.vocab_size;
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
        auto& pairSet = best_pair->positions;
        int last_added_index = -1;
        // iterate in sorted order instead of random:
        std::vector<int> sortedPairSet;
        sortedPairSet.reserve(pairSet.size());
        sortedPairSet.insert(sortedPairSet.end(), pairSet.begin(), pairSet.end());
        std::sort(sortedPairSet.begin(), sortedPairSet.end());
        // perform all merges:
        for (auto& pos: sortedPairSet) {
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
            RemovePosition(h, w1_prev_ind, w1_prev, w1, best_pair->this_pair);
            RemovePosition(h, w2_ind, w2, w2_next, best_pair->this_pair);
            int next_id = getNextIndex(&list, pos);
            // merge tokens to create new token:
            deleteElement(&list, next_id);
            updateElement(&list, pos, new_token_id);
            // add new pairs:
            AddPosition(h, pos, new_token_id, w2_next);
            AddPosition(h, w1_prev_ind, w1_prev, new_token_id);
            last_added_index = next_id;
        }
        delete best_pair;
    }
    // free remaining elements:
    for (auto &el: h.heap_data){
        auto e = el.second;
        for (HeapElement *vec: e)
        {
            delete vec;
        }
    }
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
    int64_t pair_id;
    uint32_t tok_id;
};

void printVector(const std::vector<struct tokenStat>& vec) {
    for (auto &value : vec) {
        std::cout << value.tok_id << " ";
    }
    std::cout << std::endl;
}

void _tokenizeChunk(std::vector<uint32_t> &ids, std::unordered_map<int64_t, uint32_t> &pair_to_tok, int vocab_size){
    // tokenizes a chunk in-place
    std::vector<struct tokenStat> stats; // Vector of struct stat
    stats.reserve(ids.size());
    for (size_t i = 0; i < ids.size()-1; i++)
    {
        uint32_t t_1 = ids[i];
        uint32_t t_2 = ids[i+1];
        int64_t key = t_1 * vocab_size + t_2;
        if (pair_to_tok.find(key) != pair_to_tok.end()){
            // Construct a struct stat and push it into the vector
            stats.push_back({key, pair_to_tok[key]});
        }
    }

    // merge and recalculate stats while merging
    while (stats.size() > 0){
        // find the first pair that has to be swapped:
        uint32_t min_tok_id = -1;
        int64_t min_pair_id = -1;
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
        uint32_t tok_1 = min_pair_id / vocab_size;
        uint32_t tok_2 = min_pair_id % vocab_size;
        // merge every occurrence:
        int i = 0;
        int curr_append = 0;
        uint32_t prev = -1;
        while (i < ids.size()){
            if (ids[i] == tok_1 && i < ids.size() - 1 && ids[i+1] == tok_2){
                ids[curr_append] = min_tok_id;
                i += 2;
                if (prev != -1){
                    int64_t pair_id =(int64_t)prev*vocab_size + ids[curr_append];
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
                int64_t pair_id = (int64_t)prev*vocab_size + ids[curr_append];
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
    uint32_t *ids;
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
    struct tokenizeResult tokenize(uint8_t *ids, int num_ids, int *splits, int len_splits, int64_t *token_pairs, int token_pairs_count, int init_tokens, int num_threads){
        int vocab_size = token_pairs_count+init_tokens;
        auto start = std::chrono::steady_clock::now(); // Record start time
        // splits denote the places where the string got splitted by regex
        std::vector<std::vector<uint32_t>> splitted;
        splitted.reserve(len_splits);
        for (size_t i = 0; i < len_splits-1; i++)
        {
            int curr = splits[i];
            int next = splits[i+1];
            std::vector<uint32_t> chunk;
            chunk.reserve(next-curr);
            for (size_t j = curr; j < next; j++)
            {
                chunk.emplace_back((uint32_t)ids[j]);
            }
            splitted.emplace_back(chunk);
        }
        
        std::unordered_map<int64_t, uint32_t> pair_to_token(token_pairs_count);
        for (size_t i = 0; i < token_pairs_count; i++)
        {
            pair_to_token[token_pairs[i]] = (uint32_t)(init_tokens + i);
        }
        
        // Vector to store threads
        std::vector<std::thread> threads;

        // Function to be executed by each thread
        auto tokenizeChunksThread = [&](size_t start, size_t end) {
            for (size_t i = start; i < end; i++) {
                _tokenizeChunk(splitted[i], pair_to_token, vocab_size);
            }
        };

        // Calculate chunk size based on the number of threads
        size_t chunkSize = splitted.size() / num_threads;

        // Create threads
        for (size_t i = 0; i < num_threads - 1; i++) {
            threads.emplace_back(tokenizeChunksThread, i * chunkSize, (i + 1) * chunkSize);
        }
        // Main thread handles remaining chunks
        for (size_t i = (num_threads - 1) * chunkSize; i < splitted.size(); i++) {
            _tokenizeChunk(splitted[i], pair_to_token, vocab_size);
        }
        // Join threads
        for (auto& thread : threads) {
            thread.join();
        }
        
        int total_size = 0;
        for (size_t i = 0; i < splitted.size(); i++)
        {
            total_size += splitted[i].size();
        }
        
        uint32_t *result = (uint32_t*)malloc(sizeof(uint32_t)*total_size);
        int c = 0;
        for (size_t i = 0; i < splitted.size(); i++)
        {
            for (size_t j = 0; j < splitted[i].size(); j++)
            {
                result[c] = splitted[i][j];
                c++;
            }
        }
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Elapsed time inside tokenize function: " << duration.count() << " milliseconds" << std::endl;
        return tokenizeResult{result, total_size};
    }
}


int main() {
    srand(time(NULL)); // Seed for random number generation
    auto start = std::chrono::steady_clock::now(); // Record start time

    // Generate an array with random numbers of size 10000
    int num_elements = 100000000;
    int init_tokens = 256;
    int* ids = new int[num_elements];
    for (int i = 0; i < num_elements; ++i) {
        ids[i] = rand() % init_tokens; // Random numbers between 0 and 99
    }

    int num_tokens = 10000; // Vocab size
    struct Token* vocab = train(ids, num_elements, num_tokens, init_tokens);

    for (int i = 0; i < num_tokens; i++) {
        //std::cout << "(" << vocab[i].first_id << ", " << vocab[i].second_id << ") => " << vocab[i].token_id << ", Token List Length: " << vocab[i].token_list_len << ", Token List: ";
        for (int j = 0; j < vocab[i].token_list_len; j++) {
           // std::cout << vocab[i].token_list[j] << " ";
        }
        //std::cout << std::endl;
        delete[] vocab[i].token_list;
    }

    delete[] ids;
    delete[] vocab;

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Elapsed time inside tokenize function: " << duration.count() << " milliseconds" << std::endl;
    return 0;
}

// int main() {
//     // little test:
//     int vocab_size = 13;
//     int init_tokens = 6;
//     std::vector<uint8_t> test = {0, 1, 2, 3, 3, 4, 5, 5};
//     std::vector<int> splits = {0, 8};
//     std::vector<int64_t> token_pairs = {
//         2*vocab_size + 3, // 6
//         4*vocab_size + 5, // 7
//         0*vocab_size + 1, // 8
//         7*vocab_size + 5, // 9
//         6*vocab_size + 3, // 10
//         8*vocab_size + 10, // 11
//         11*vocab_size + 9, // 12
//     };
//     struct tokenizeResult res = tokenize(&test[0], test.size(), &splits[0], splits.size(), &token_pairs[0], vocab_size-init_tokens, init_tokens, 1);
//     printf("Result:"); // should print 12
//     for (size_t i = 0; i < res.ids_size; i++)
//     {
//         printf("%d ", res.ids[i]);
//     }
//     return 0;
// }

// int main() {
//     srand(time(NULL)); // Seed for random number generation
//     auto start = std::chrono::steady_clock::now(); // Record start time

//     // Array with specified numbers
//     int ids[] = {2, 1, 1, 1};
//     int num_ids = sizeof(ids) / sizeof(ids[0]); // Calculate number of elements in the array

//     int num_tokens = 6; // Vocab size
//     int init_tokens = 3; // Init size
//     struct Token* vocab = train(ids, num_ids, num_tokens, init_tokens);

//     for (int i = 0; i < num_tokens; i++) {
//         printf("(%d, %d) => %d, Token List Length: %d, Token List: ",
//             vocab[i].first_id, vocab[i].second_id, vocab[i].token_id, vocab[i].token_list_len);
//         for (int j = 0; j < vocab[i].token_list_len; j++) {
//             printf("%d ", vocab[i].token_list[j]);
//         }
//         printf("\n");
//         free(vocab[i].token_list);
//     }
    
//     free(vocab);
    
//     auto end = std::chrono::steady_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//     std::cout << "Elapsed time inside tokenize function: " << duration.count() << " milliseconds" << std::endl;
//     return 0;
// }

