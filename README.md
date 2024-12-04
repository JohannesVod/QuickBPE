# QuickBPE

This is a much faster version of the MinBPE tokenizer from Andrej Karpathy. The main functions are optimized in C++ and then connected to Python using ctypes, so that you can call them conveniently. I already successfully tokenized the entire TinyStories dataset in around 8 minutes, which is ~3.5 GB of text. The main bottleneck is now the regex splitting, which is hard to optimize since I decided to keep it integrated into Python (so that it is still easy to change the split pattern).

I implemented a linear time algorithm explained on the wikipedia article for [Re-Pair](https://en.wikipedia.org/wiki/Re-Pair). The training takes about 2 minutes on ~100 MB of text, which seems to be decent. But there is probably still a lot of improvement that can be done. Also the encode function is much slower than the encode_ordinary function if the special tokens are distributed evenly because of the splitting. The only way to fix this as far as i can see is to reimplement this specific function in c++ as well.

# Operating systems
Right now only linux is supported. Windows works as well, but the training might be slower.

# Quickstart
You can use the repo in the same way as the MinBPE repo. Make sure to use RegexTokenizerFast and encode_ordinary (the encode function is not as fast sometimes, but still faster than the python version)

```python
from minbpe import RegexTokenizerFast
tokenizer = RegexTokenizerFast()
text = "aaabdaaabac"
tokenizer.train(text, 256 + 3) # 256 are the byte tokens, then do 3 merges
print(tokenizer.encode_ordinary(text))
# [258, 100, 258, 97, 99]
print(tokenizer.decode([258, 100, 258, 97, 99]))
# aaabdaaabac
tokenizer.save("toy")
```

## todos
- make the encode function faster by implementing it in c++ (or find some other way)
- rename GPT4Tokenizer to GPTTokenizer and support GPT-2/GPT-3/GPT-3.5 as well?
- write a LlamaTokenizer similar to GPT4Tokenizer (i.e. attempt sentencepiece equivalent)

## License

MIT