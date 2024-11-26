from minbpe import RegexTokenizerFast

file_path = "openwebtext.txt"
with open(file_path, "r", encoding="utf-8") as f:
    training_text = f.read()

pattern = r"""'(?i:[sdmt]|ll|ve|re)|[^\r\n\p{L}\p{N}]?+\p{L}+|\p{N}{1,3}| ?[^\s\p{L}\p{N}]++[\r\n]*|\s*[\r\n]|\s+(?!\S)|\s+"""

tokenizer = RegexTokenizerFast(pattern)
k = 50000 # steps of BPE
tokenizer.train(training_text, 256 + k)

sample_text = "This is a sample text for testing."
encoded = tokenizer.encode_ordinary(sample_text)
decoded = tokenizer.decode(encoded)

print("Encoded:", encoded)
print("Decoded:", decoded)
print(f"compression ratio: {len(encoded)/len(decoded)}")
tokenizer.save("toy")
