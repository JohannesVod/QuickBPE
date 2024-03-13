
import os
import time
from minbpe import RegexTokenizer

# open some text and train a vocab of 512 tokens
text = open("tests/taylorswift.txt", "r", encoding="utf-8").read()
text = "".join([text for _ in range(500)]) # 92780500

# create a directory for models, so we don't pollute the current directory
os.makedirs("models", exist_ok=True)

# t0 = time.time()
for TokenizerClass, name in zip([RegexTokenizer], ["regex"]):
    # construct the Tokenizer object and kick off verbose training
    tokenizer = TokenizerClass()
    vocab_size = 5000
    init_vocab_size = 256
    merges, vocab = tokenizer.train(text[:100000], vocab_size, init_vocab_size)
    encoded = tokenizer.encode(text)
    print("compression:", len(encoded)/len(text))
    # tokenizer.decode(encoded)
    # tokenizer.checkSolution(text, vocab_size, merges1, vocab1, init_vocab_size)
    #print("check successful!")
    # writes two files in the models directory: name.model, and name.vocab
    prefix = os.path.join("models", name)
    # tokenizer.save(prefix)

# t1 = time.time()

# print(f"Training took {t1 - t0:.2f} seconds")

