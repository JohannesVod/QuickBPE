import ctypes
import os
import numpy as np
import time
from numpy.ctypeslib import ndpointer

# Define the structure for the result tuple
class Result(ctypes.Structure):
    _fields_ = [
        ('token_id', ctypes.c_int),
        ('first_id', ctypes.c_int),
        ('second_id', ctypes.c_int),
        ('token_list_len', ctypes.c_int),
        ('token_list', ctypes.POINTER(ctypes.c_int))
    ]

class TokenizeResult(ctypes.Structure):
    _fields_ = [
        ('result', ctypes.POINTER(ctypes.c_uint16)),
        ('length', ctypes.c_int)
    ]

# Load the DLL
script_dir = os.path.dirname(os.path.abspath(__file__))
dll_path = os.path.join(script_dir, "funcs.dll")
funcs = ctypes.CDLL(dll_path)

# Define the input and output types of the function
funcs.train.restype = ctypes.POINTER(Result)
funcs.train.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int, ctypes.c_int]

funcs.tokenize.restype = TokenizeResult
funcs.tokenize.argtypes = [ndpointer(ctypes.c_uint8, flags="C_CONTIGUOUS"), ctypes.c_int, 
                           ndpointer(ctypes.c_int32, flags="C_CONTIGUOUS"), ctypes.c_int, 
                           ctypes.POINTER(ctypes.c_int), ctypes.c_int,
                           ctypes.c_int, ctypes.c_int]

def trainFast(ids, num_tokens, init_tokens=256):
    """
    trains on ids which are already separated 
    """
    ids_arr = (ctypes.c_int * len(ids))(*ids)
    # Call the c++ function:
    results_ptr = funcs.train(ids_arr, len(ids), num_tokens, init_tokens)
    # Convert the results to a Python list of tuples:
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
        vocab[el[0]] = bytes(el[3])
    return merges, vocab

def tokenizeFast(ids, split_indices, merges, init_tokens, threads=1):
    vocab_size = len(merges)+init_tokens
    merges_l = [pair[0] * vocab_size + pair[1] for pair in merges]
    merges_np = np.array(merges_l, dtype=np.int32)

    merges_arr = merges_np.ctypes.data_as(ctypes.POINTER(ctypes.c_int64))

    # split_indices_arr = split_indices.ctypes.data_as(ctypes.POINTER(ctypes.c_int))
    # ids_arr = ids.ctypes.data_as(ctypes.POINTER(ctypes.c_ubyte))

    results_ptr = funcs.tokenize(
        ids, len(ids), split_indices, len(split_indices),
        merges_arr, len(merges), init_tokens, threads
    )

    tokenized_text_size = results_ptr.length
    tokenized_text_buffer = (ctypes.c_uint32 * tokenized_text_size)()
    ctypes.memmove(tokenized_text_buffer, results_ptr.result, ctypes.sizeof(ctypes.c_uint32) * tokenized_text_size)
    tokenized_text = list(tokenized_text_buffer)
    return tokenized_text

from random import randrange

if __name__ == "__main__":
    # Example usage
    init_tokens = 256
    vocab_size = 5000
    ids = [randrange(1, init_tokens) for i in range(100000)]
    # [1, 3, 0, 1, 0, 3, 1, 0, 3, 0]
    merges, vocab = trainFast(ids, vocab_size)
    size = 100
    to_encode = np.random.randint(1, init_tokens, size)
    to_encode = list(to_encode)
    res = tokenizeFast(to_encode, [0, len(to_encode)], merges, vocab_size, init_tokens)
    # print("vocab: ", vocab)
    # print("merges: ", merges)