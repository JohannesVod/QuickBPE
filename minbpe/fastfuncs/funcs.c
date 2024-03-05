#include <stdio.h>
#include <stdlib.h>
#include <queue>

struct Token {
    int token_id; // the id of the token
    int first_id; // the first element of the pair
    int second_id; // the second element of the pair
    int token_list_len; // the length of the corresponding tokens
    int* token_list; // the corresponding tokens
};

int* merge(int* ids, int ids_len, int num_pairs, int pair1, int pair2, int tokenID) {
    int* new_ids = (int*)malloc((ids_len - num_pairs) * sizeof(int));
    int i = 0;
    int j = 0;
    while (i < ids_len) {
        if (i < ids_len - 1 && ids[i] == pair1 && ids[i + 1] == pair2) {
            new_ids[j] = tokenID;
            i++;  // Skip the next element as well
        } else {
            new_ids[j] = ids[i];
        }
        i++;
        j++;
    }
    // Free old array:
    return new_ids;
}

int* mergeSmart(int* ids, int ids_len, int num_pairs_in, int pair1, int pair2, int tokenID) {
    int num_pairs = 0;
    // Calculate the number of pairs if not provided
    for (int i = 0; i < ids_len - 1; i++) {
        if (ids[i] == pair1 && ids[i + 1] == pair2) {
            num_pairs++;
            i++; // Skip the next element
        }
    }
    return merge(ids, ids_len, num_pairs, pair1, pair2, tokenID);
}

struct Token* train(int* ids, int num_ids, int num_tokens, int init_tokens) {
    // build vocab
    struct Token* vocab = (struct Token*)malloc(num_tokens * sizeof(struct Token));
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
    // init counting dict
    int **stats = (int **)malloc(num_tokens * sizeof(int *));
    for (int i = 0; i < num_tokens; i++) {
        stats[i] = (int *)malloc(num_tokens * sizeof(int));
    }
    // Accessing and assigning elements of the array
    for (int i = 0; i < num_tokens; i++) {
        for (int j = 0; j < num_tokens; j++) {
            stats[i][j] = 0;
        }
    }
    int do_free = 0;
    int *curr_ids = ids;
    int total_merges = num_tokens-init_tokens;
    
    for (int merge_id = 0; merge_id < total_merges; merge_id++) {
        for (size_t i = 1; i <= merge_id+init_tokens; i++)
        {
            for (size_t j = 1; j <= merge_id+init_tokens; j++)
            {
                stats[i][j] = 0;
            }
        }
        // count every stat:
        for (int i = 0; i < num_ids-1; i++)
        {
            // heavy calculation:
            int first_el = curr_ids[i];
            int second_el = curr_ids[i+1];
            stats[first_el][second_el] += 1;
        }
        // get max stat:
        int max_pair_1 = -1;
        int max_pair_2 = -1;
        int max_count = -1;
        
        // skip first row and column so that splits are ignored
        for (size_t i = 1; i <= merge_id+init_tokens; i++)
        {
            for (size_t j = 1; j <= merge_id+init_tokens; j++)
            {
                int this_count = stats[i][j];
                if (this_count > max_count){
                    max_count = this_count;
                    max_pair_1 = i;
                    max_pair_2 = j;
                }
            }
        }
        
        // set new token:
        int max_token_id = init_tokens + merge_id;
        vocab[max_token_id].token_id = max_token_id;
        vocab[max_token_id].first_id = max_pair_1;
        vocab[max_token_id].second_id = max_pair_2;
        int pair_1_len = vocab[max_pair_1].token_list_len;
        int pair_2_len = vocab[max_pair_2].token_list_len;
        int new_tokens_len = pair_1_len + pair_2_len;
        vocab[max_token_id].token_list_len = new_tokens_len;
        vocab[max_token_id].token_list = (int*)malloc(new_tokens_len * sizeof(int));
        
        // create new token list:
        for (size_t i = 0; i < pair_1_len; i++)
        {
            vocab[max_token_id].token_list[i] = vocab[max_pair_1].token_list[i];
        }
        for (size_t i = 0; i < pair_2_len; i++)
        {
            vocab[max_token_id].token_list[i+pair_1_len] = vocab[max_pair_2].token_list[i];
        }
        
        // merge and adjust resizing
        int* new_ids = mergeSmart(curr_ids, num_ids, max_count, max_pair_1, max_pair_2, max_token_id);

        // readjust id size:
        if (do_free >= 1){ // don't free the original array. This has to be done by the parent process!
            free(curr_ids);
        }
        do_free++;
        curr_ids = new_ids;
        num_ids = num_ids - max_count;
    }
    // Free stats again:
    for (int i = 0; i < num_tokens; i++) {
        free(stats[i]);
    }
    free(stats);
    return vocab;
}

int main() {
    int ids[] = {3, 7, 9, 5, 3, 7, 5, 1, 8, 3};
    int num_ids = sizeof(ids) / sizeof(ids[0]);
    int num_tokens = 258; // Choose an appropriate value
    int init_tokens = 256; // Choose an appropriate value

    struct Token* vocab = train(ids, num_ids, num_tokens, init_tokens);

    for (int i = 0; i < num_tokens - init_tokens; i++) {
        printf("Token ID: %d, First ID: %d, Second ID: %d, Token List Length: %d, Token List: ",
               vocab[i].token_id, vocab[i].first_id, vocab[i].second_id, vocab[i].token_list_len);
        for (int j = 0; j < vocab[i].token_list_len; j++) {
            printf("%d ", vocab[i].token_list[j]);
        }
        printf("\n");
        free(vocab[i].token_list);
    }
    free(vocab);
    return 0;
}

