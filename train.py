
import os
import time
from tokenizer import RegexTokenizer

# open some text and train a vocab of 512 tokens
text = open("tests/taylorswift.txt", "r", encoding="utf-8").read()
text = "".join([text for _ in range(500)]) # 92780500
print(len(text))
# create a directory for models, so we don't pollute the current directory
os.makedirs("models", exist_ok=True)

# t0 = time.time()
for TokenizerClass, name in zip([RegexTokenizer], ["regex"]):
    # construct the Tokenizer object and kick off verbose training
    tokenizer = TokenizerClass()
    vocab_size = 270
    init_vocab_size = 256
    tokenizer.train(text[:10000], vocab_size, init_vocab_size)
    text = "hello world!!!? (안녕하세요!) lol123 😉"
    encoded = tokenizer.encode_ordinary(text)
    encoded2 = tokenizer.encode_ordinary2(text)
    print(encoded, encoded2)
    assert encoded == encoded2
    print("compressed to:", f"{round(100*len(encoded)/len(text.encode('utf-8')), 2)}%")
    # tokenizer.decode(encoded)
    # tokenizer.checkSolution(text, vocab_size, merges1, vocab1, init_vocab_size)
    # print("check successful!")
    # writes two files in the models directory: name.model, and name.vocab
    prefix = os.path.join("models", name)
    # tokenizer.save(prefix)

# t1 = time.time()

# print(f"Training took {t1 - t0:.2f} seconds")

