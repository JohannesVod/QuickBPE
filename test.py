import os
from datasets import load_dataset
from QuickBPE import RegexTokenizerFast

output_file = "openwebtext.txt"
dataset = load_dataset("stas/openwebtext-10k")
current_size = 0
complete_s = "".join([st["text"] for st in dataset["train"]])
print(len(complete_s))
regex_pattern = r"""'(?i:[sdmt]|ll|ve|re)|[^\r\n\p{L}\p{N}]?+\p{L}+|\p{N}{1,3}| ?[^\s\p{L}\p{N}]++[\r\n]*|\s*[\r\n]|\s+(?!\S)|\s+"""

for mb in [i+1 for i in range(50)]:
    s = complete_s[:mb*1000000]
    print("training on", len(s), "tokens")
    tokenizer = RegexTokenizerFast(regex_pattern)
    k = 10000  # steps of BPE
    _, _, time_taken = tokenizer.train(s, 256 + k)
    print("time taken:", time_taken)

tokenizer.save("webtokenizer")
sample_text = "This is a sample text"
encoded = tokenizer.encode_ordinary(sample_text)
decoded = tokenizer.decode(encoded)
print("encoded:", encoded)
print("decoded:", decoded)
print(f"Compression ratio: {len(encoded) / len(decoded)}")