import os
from datasets import load_dataset
from minbpe import RegexTokenizerFast

output_file = "openwebtext.txt"
file_size_limit = 10 * 1024 * 1024  # 10MB

if not os.path.exists(output_file):
    dataset = load_dataset("stas/openwebtext-10k")
    current_size = 0
    with open(output_file, "w", encoding="utf-8") as f:
        for item in dataset["train"]:  # Assuming the data is in the 'train' split
            text = item["text"]  # Adjust the key if necessary
            current_size += len(text.encode("utf-8"))
            if current_size > file_size_limit:
                break
            f.write(text + "\n")

with open(output_file, "r", encoding="utf-8") as f:
    training_text = f.read()

pattern = r"""'(?i:[sdmt]|ll|ve|re)|[^\r\n\p{L}\p{N}]?+\p{L}+|\p{N}{1,3}| ?[^\s\p{L}\p{N}]++[\r\n]*|\s*[\r\n]|\s+(?!\S)|\s+"""

tokenizer = RegexTokenizerFast(pattern)
k = 50000  # steps of BPE
print("start training for", k, "steps")
tokenizer.train(training_text, 256 + k)

sample_text = "This is a sample text"
encoded = tokenizer.encode_ordinary(sample_text)
decoded = tokenizer.decode(encoded)
print("encoded:", encoded)
print("decoded:", decoded)
print(f"Compression ratio: {len(encoded) / len(decoded)}")
tokenizer.save("webtokenizer")