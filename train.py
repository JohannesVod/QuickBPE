import os
import time
from tokenizer import RegexTokenizer
from random import randrange

# open some text and train a vocab of 512 tokens
textOr = open("tests/taylorswift.txt", "r", encoding="utf-8").read()
textOr = "".join([textOr for _ in range(60)]) # ~ 100 mb
print(len(textOr))
# create a directory for models, so we don't pollute the current directory
os.makedirs("models", exist_ok=True)
# t0 = time.time()

for TokenizerClass, name in zip([RegexTokenizer], ["regex"]):
    for i in range(1):
        # rand_nd = randrange(len(textOr))
        # text = textOr[rand_nd:(rand_nd+500)]
        text = textOr
        # construct the Tokenizer object and kick off verbose training
        tokenizer = TokenizerClass()
        vocab_size = 300
        init_vocab_size = 256
        merges, vocab = tokenizer.train(text, vocab_size, init_vocab_size)
        tokenizer.checkSolution(text, vocab_size, merges, vocab, init_vocab_size)
        print("ready")
        encoded = tokenizer.encode(text)
        print("compressed to:", f"{round(100*len(encoded)/len(text.encode('utf-8')), 2)}%")
        # tokenizer.decode(encoded)
        # tokenizer.checkSolution(text, vocab_size, merges1, vocab1, init_vocab_size)
        # print("check successful!")
        # writes two files in the models directory: name.model, and name.vocab
        prefix = os.path.join("models", name)
        # tokenizer.save(prefix)

# t1 = time.time()
# {(115, 101): 256, (101, 114): 257, (32, 116): 258, (102, 257): 259}
# 115, 101, 101, 114, 0, 32, 116, 1, 32, 116, 2, 115, 101, 3, 32, 97, 4, 115, 101, 5, 32, 97, 6, 102, 101, 114