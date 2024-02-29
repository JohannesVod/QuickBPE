import ctypes
import os
import struct
# Define the structure for the result tuple
class Result(ctypes.Structure):
    _fields_ = [
        ('token_id', ctypes.c_int),
        ('first_id', ctypes.c_int),
        ('second_id', ctypes.c_int),
        ('token_list_len', ctypes.c_int),
        ('token_list', ctypes.POINTER(ctypes.c_int))
    ]

# Load the DLL
script_dir = os.path.dirname(os.path.abspath(__file__))
dll_path = os.path.join(script_dir, "funcs.dll")
funcs = ctypes.CDLL(dll_path)

# Define the input and output types of the function
funcs.train.restype = ctypes.POINTER(Result)
funcs.train.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int, ctypes.c_int]

def trainFast(ids, num_tokens, init_tokens=256):
    """
    trains on ids which are already separated 
    """
    # Convert the input list to a C-style array
    ids_arr = (ctypes.c_int * len(ids))(*ids)
    # Call the dummy_function#
    results_ptr = funcs.train(ids_arr, len(ids), num_tokens, init_tokens)
    # Convert the results to a Python list of tuples
    results = []
    for i in range(num_tokens):
        result = results_ptr[i]
        tok_len = result.token_list_len
        tokens = [result.token_list[j] for j in range(tok_len)]
        results.append((result.token_id, result.first_id, result.second_id, tokens))
    vocab = {}
    merges = {}
    for el in results:
        if el[0] >= init_tokens:
            merges[(el[1], el[2])] = el[0]
        vocab[el[0]] = el[3]
    return merges, vocab

from random import randrange

if __name__ == "__main__":
    # Example usage
    ids = [randrange(100) for i in range(100)] + [10, 0]
    # [1, 3, 0, 1, 0, 3, 1, 0, 3, 0]
    merges, vocab = trainFast(ids, 259)
    # print("vocab: ", vocab)
    #print("merges: ", merges)