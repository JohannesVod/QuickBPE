from tokenizer import RegexTokenizer

train_text = "Lexical tokenization is conversion of a text into (semantically or syntactically)" \
            "meaningful lexical tokens belonging to categories defined by a 'lexer' program."

tokenizer = RegexTokenizer()
tokenizer.train(train_text, 256+94) # trains on 94 tokens
test_text = "tokenization is cool!"
tokenized = tokenizer.encode(test_text)
decoded = tokenizer.decode(tokenized)
print(decoded)