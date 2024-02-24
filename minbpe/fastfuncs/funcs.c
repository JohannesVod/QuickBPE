#include <stdio.h>
#include <stdlib.h>

struct Result {
    int a;
    int b;
    int c;
    int* tokens;
};

struct Result* dummy_function(int* ids, int num_ids, int num_tokens) {
    struct Result* results = (struct Result*)malloc(num_ids * sizeof(struct Result));

    for (int i = 0; i < num_ids; i++) {
        results[i].a = i + 1;
        results[i].b = ids[i] * 2;
        results[i].c = ids[i] * 3;
        int a_len = i*2 + 1;
        a_len = num_tokens;
        results[i].tokens = (int*)malloc(a_len * sizeof(int));
        for (int j = 0; j < a_len; j++) {
            results[i].tokens[j] = (i + 1) * (j + 1);
        }
    }
    return results;
}
