import ctypes
import os

# Define the structure for the result tuple
class Result(ctypes.Structure):
    _fields_ = [
        ('token_id', ctypes.c_int),
        ('first_id', ctypes.c_int),
        ('second_id', ctypes.c_int),
        ('token_list', ctypes.POINTER(ctypes.c_int))
    ]

# Load the DLL
script_dir = os.path.dirname(os.path.abspath(__file__))
dll_path = os.path.join(script_dir, "funcs.dll")
funcs = ctypes.CDLL(dll_path)

# Define the input and output types of the function
funcs.dummy_function.restype = ctypes.POINTER(Result)
funcs.dummy_function.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int]

def call_dummy_function(ids, num_tokens):
    # Convert the input list to a C-style array
    ids_arr = (ctypes.c_int * len(ids))(*ids)

    # Call the dummy_function
    results_ptr = funcs.dummy_function(ids_arr, len(ids), num_tokens)

    # Convert the results to a Python list of tuples
    results = []
    for i in range(len(ids)):
        result = results_ptr[i]
        tokens = [result.tokens[j] for j in range(num_tokens)]
        results.append((result.a, result.b, result.c, tokens))
    return results

# Example usage
ids = [1, 2, 3]
num_tokens = 4
output = call_dummy_function(ids, num_tokens)
print(output)
