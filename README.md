# QuickBPE

This is a much faster version of the MinBPE tokenizer from Andrej Karpathy. The main functions are optimized in C++ and then connected to Python using ctypes, so that you can call them conveniently. I already successfully tokenized the entire TinyStories dataset in around 8 minutes, which is ~3.5 GB of text. The main bottleneck is now the regex splitting, which is hard to optimize since I decided to keep it integrated into Python (so that it is still easy to change the split pattern).

I implemented a linear time algorithm explained on the wikipedia article for [Re-Pair](https://en.wikipedia.org/wiki/Re-Pair). The training takes about 2 minutes on ~100 MB of text, which seems to be decent. But there is probably still a lot of improvement that can be done. Also the encode function is much slower than the encode_ordinary function if the special tokens are distributed evenly because of the splitting. The only way to fix this as far as i can see is to reimplement this specific function in c++ as well.

# Operating systems
Right now only linux/windows is supported. The rest might work, but i didn't test.

# Quickstart
You can use the repo in the same way as the MinBPE repo. Make sure to use RegexTokenizerFast and encode_ordinary (the encode function is not as fast sometimes, but still faster than the python version)

Here is production ready code that trains a tokenizer on ~50mb of webtext (make sure to pip install datasets first):

```python
from datasets import load_dataset
from QuickBPE import RegexTokenizerFast

print("loading dataset...")
output_file = "openwebtext.txt"
dataset = load_dataset("stas/openwebtext-10k")
current_size = 0
s = "".join([st["text"] for st in dataset["train"]])
# gpt-4 regex splitting:
regex_pattern = r"""'(?i:[sdmt]|ll|ve|re)|[^\r\n\p{L}\p{N}]?+\p{L}+|\p{N}{1,3}| ?[^\s\p{L}\p{N}]++[\r\n]*|\s*[\r\n]|\s+(?!\S)|\s+"""

print("training tokenizer (takes ~2 Minutes)...")
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
```

The tokenizer can be loaded as follows:

```python
from QuickBPE import RegexTokenizerFast
tokenizer = RegexTokenizerFast()
tokenizer.load("webtokenizer.model") # loads the model back from disk
sample_text = "This is a sample text!"
encoded = tokenizer.encode(sample_text, allowed_special="all")
tokens = [tokenizer.decode([tid]) for tid in encoded]  # Decode each token
print("tokenization: ", "|".join(tokens))
decoded = tokenizer.decode(encoded)
print("encoded:", encoded)
print("decoded:", decoded)
print(f"Compression ratio: {len(encoded) / len(decoded)}")
```

You can register special tokens in the following way:

```python
from QuickBPE import RegexTokenizerFast
tokenizer = RegexTokenizerFast()
s = "hello world"
tokenizer.train(s, 256 + 3)
tokenizer.register_special_tokens({"<|endoftext|>": 256 + 3}) # register new special token
res = tokenizer.encode("<|endoftext|>hello world", allowed_special="all")
print(res) # the first token of the result should be 259
```

## License
MIT