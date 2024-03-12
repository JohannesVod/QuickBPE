import numpy as np

# Generate random integers
len_ids = 9278050
ids = np.random.randint(1, 256, size=len_ids)
vocab_size = 5000
print("Hi")
pair_sets = {}
last_added_index = 0
last_added_pair_index = -1
for i in range(0, len_ids-1):
    first_el = ids[i]
    second_el = ids[i+1]
    index = first_el*vocab_size + second_el
    if last_added_pair_index == index and last_added_index == i-1:
        continue
        
    if first_el != 0 and second_el != 0:
        if index not in pair_sets:
            pair_sets[index] = set()
        pair_sets[index].add(i)

    last_added_index = i
    last_added_pair_index = index

print("HI")