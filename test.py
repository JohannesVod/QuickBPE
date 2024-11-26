

from datasets import load_dataset

# Load the OpenWebText dataset
dataset = load_dataset("stas/openwebtext-10k")

# Initialize size tracker and output file
output_file = "openwebtext.txt"
size_limit = 10 * 1024 * 1024  # 5MB in bytes
current_size = 0

with open(output_file, "w", encoding="utf-8") as f:
    for item in dataset["train"]:  # Assuming the data is in the 'train' split
        text = item["text"]  # Adjust the key if necessary
        current_size += len(text.encode("utf-8"))
        if current_size > size_limit:
            break
        f.write(text + "\n")

print(f"First 10MB of data saved to {output_file}.")