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
    for (size_t i=0; i < ids.size()-1; i++)
    {
        uint32_t t_1 = ids[i];
        uint32_t t_2 = ids[i+1];
        int64_t key = (int64_t)t_1 * vocab_size + t_2;
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
        // std::cout << "Elapsed time inside tokenize function: " << duration.count() << " milliseconds" << std::endl;
        return tokenizeResult{result, total_size};
    }
}