"""
Minimal (byte-level) Byte Pair Encoding tokenizer.

Algorithmically follows along the GPT tokenizer:
https://github.com/openai/gpt-2/blob/master/src/encoder.py

Unlike BasicTokenizer:
- RegexTokenizer handles an optional regex splitting pattern.
- RegexTokenizer handles optional special tokens.
"""

import regex as re
from .fastfuncs.funcs import trainFast, tokenizeFast
from .base import Tokenizer, get_stats, merge
import numpy as np
import time 

# the main GPT text split patterns, see
# https://github.com/openai/tiktoken/blob/main/tiktoken_ext/openai_public.py
GPT2_SPLIT_PATTERN = r"""'(?:[sdmt]|ll|ve|re)| ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+"""
GPT4_SPLIT_PATTERN = r"""'(?i:[sdmt]|ll|ve|re)|[^\r\n\p{L}\p{N}]?+\p{L}+|\p{N}{1,3}| ?[^\s\p{L}\p{N}]++[\r\n]*|\s*[\r\n]|\s+(?!\S)|\s+"""

class RegexTokenizer(Tokenizer):
    def __init__(self, pattern=None):
        """
        - pattern: optional string to override the default (GPT-4 split pattern)
        - special_tokens: str -> int dictionary of special tokens
          example: {'<|endoftext|>': 100257}
        """
        super().__init__()
        self.pattern = GPT4_SPLIT_PATTERN if pattern is None else pattern
        self.compiled_pattern = re.compile(self.pattern)
        self.special_tokens = {}
        self.inverse_special_tokens = {}
        self.init_tokens = 256

    def presplit(self, text):
        # "splits" the text by adding 0 as padding between the tokens
        text_chunks = re.findall(self.compiled_pattern, text)
        # input text preprocessing
        ids = [list(ch.encode("utf-8")) for ch in text_chunks]
        bytes_total = 0
        for el in ids:
            bytes_total += len(el)
        id_list = [0 for _ in range(len(ids) + bytes_total)]
        i = 0
        for ch in ids:
            for el in ch:
                #print(i)
                id_list[i] = el
                i += 1
            i += 1
        return id_list

    def train(self, text, vocab_size, verbose=False):
        assert vocab_size >= self.init_tokens
        self.vocab_size = vocab_size
        self.init_tokens = self.init_tokens
        id_list = self.presplit(text)
        self.merges, self.vocab =  trainFast(id_list, vocab_size, self.init_tokens)
        return self.merges, self.vocab

    def checkSolution(self, text, vocab_size, merges, vocab, init_vocab):
        reverse_merges = {merges[i]:i for i in merges}
        tok_ids = {i for i in reverse_merges}
        if tok_ids != set(range(init_vocab, vocab_size)):
            raise RuntimeError("not every token is assigned:(", set(range(init_vocab, vocab_size)), tok_ids)
        
        def getMax(stats, curr_size, suggested_max, curr_ind):
            max_pair = max(stats, key=stats.get)
            max_count = stats.get(max_pair)
            if max_count != stats.get(suggested_max):
                raise RuntimeError(f"At {curr_ind} we have as correct max pair: {max_pair} (with cost {max_count}). But our algorithm gave {suggested_max} with cost real cost {stats.get(suggested_max)}")
            return suggested_max

        assert vocab_size >= self.init_tokens
        num_merges = vocab_size - self.init_tokens

        # split the text up into text chunks
        text_chunks = re.findall(self.compiled_pattern, text)
        ids = [list(ch.encode("utf-8")) for ch in text_chunks]

        merges = {} # (int, int) -> int
        vocab = {idx: bytes([idx]) for idx in range(self.init_tokens)} # idx -> bytes
        for i in range(num_merges):
            idx = self.init_tokens + i
            stats = {}
            for chunk_ids in ids:
                get_stats(chunk_ids, stats)
            suggested_max = reverse_merges[idx]
            pair = getMax(stats, vocab_size, suggested_max, idx)
            ids = [merge(chunk_ids, pair, idx) for chunk_ids in ids]
            merges[pair] = idx
            vocab[idx] = vocab[pair[0]] + vocab[pair[1]]
            print(f"checking {i}")

    def register_special_tokens(self, special_tokens):
        # special_tokens is a dictionary of str -> int
        # example: {"<|endoftext|>": 100257}
        self.special_tokens = special_tokens
        self.inverse_special_tokens = {v: k for k, v in special_tokens.items()}

    def decode(self, ids):
        # given ids (list of integers), return Python string
        part_bytes = []
        for idx in ids:
            if idx in self.vocab:
                part_bytes.append(self.vocab[idx])
            elif idx in self.inverse_special_tokens:
                part_bytes.append(self.inverse_special_tokens[idx].encode("utf-8"))
            else:
                raise ValueError(f"invalid token id: {idx}")
        text_bytes = b"".join(part_bytes)
        text = text_bytes.decode("utf-8", errors="replace")
        return text

    def _encode_chunk(self, text_bytes):
        # return the token ids
        # let's begin. first, convert all bytes to integers in range 0..255
        ids = list(text_bytes)
        while len(ids) >= 2:
            # find the pair with the lowest merge index
            stats = get_stats(ids)
            pair = min(stats, key=lambda p: self.merges.get(p, float("inf")))
            # subtle: if there are no more merges available, the key will
            # result in an inf for every single pair, and the min will be
            # just the first pair in the list, arbitrarily
            # we can detect this terminating case by a membership check
            if pair not in self.merges:
                break # nothing else can be merged anymore
            # otherwise let's merge the best pair (lowest merge index)
            idx = self.merges[pair]
            ids = merge(ids, pair, idx)
        return ids

    def encode_ordinary2(self, text):
        """Encoding that ignores any special tokens."""
        # split text into chunks of text by categories defined in regex pattern
        text_chunks = re.findall(self.compiled_pattern, text)
        # all chunks of text are encoded separately, then results are joined
        ids = []
        for chunk in text_chunks:
            chunk_bytes = chunk.encode("utf-8") # raw bytes
            chunk_ids = self._encode_chunk(chunk_bytes)
            ids.extend(chunk_ids)
        return ids

    def encode_ordinary(self, text, ids=None):
        # encode text. use np because much faster (thanks chatgpt):
        if ids is None:
            ids = np.frombuffer(text.encode('utf-8'), dtype=np.uint8)
        # find all occurences in ids by index:
        text_chunks = re.findall(self.compiled_pattern, text) # slowest operation
        split_indices = np.zeros(len(text_chunks) + 1, dtype=np.int32)
        curr_el = 0
        i = 0
        for el in text_chunks:
            split_indices[i] = curr_el
            curr_el += len(list(el.encode("utf-8")))
            i += 1
        split_indices[-1] = len(ids)
        # call c++ function:
        result = tokenizeFast(ids, split_indices, self.merges, self.init_tokens)
        return result

    def encode(self, text, allowed_special="none_raise"):
        """
        Unlike encode_ordinary, this function handles special tokens.
        allowed_special: can be "all"|"none"|"none_raise" or a custom set of special tokens
        if none_raise, then an error is raised if any special token is encountered in text
        this is the default tiktoken behavior right now as well
        any other behavior is either annoying, or a major footgun
        """
        # decode the user desire w.r.t. handling of special tokens
        special = None
        if allowed_special == "all":
            special = self.special_tokens
        elif allowed_special == "none":
            special = {}
        elif allowed_special == "none_raise":
            special = {}
            assert all(token not in text for token in self.special_tokens)
        elif isinstance(allowed_special, set):
            special = {k: v for k, v in self.special_tokens.items() if k in allowed_special}
        else:
            raise ValueError(f"allowed_special={allowed_special} not understood")
        if not special:
            # shortcut: if no special tokens, just use the ordinary encoding
            return self.encode_ordinary(text)
        # otherwise, we have to be careful with potential special tokens in text
        # we handle special tokens by splitting the text
        # based on the occurrence of any exact match with any of the special tokens
        # we can use re.split for this. note that surrounding the pattern with ()
        # makes it into a capturing group, so the special tokens will be included
        special_pattern = "(" + "|".join(re.escape(k) for k in special) + ")"
        special_chunks = re.split(special_pattern, text)
        # now all the special characters are separated from the rest of the text
        # all chunks of text are encoded separately, then results are joined
        ids = []
        for part in special_chunks:
            if part in special:
                # this is a special token, encode it separately as a special case
                ids.append(special[part])
            else:
                # this is an ordinary sequence, encode it normally
                ids.extend(self.encode_ordinary(part))
        return ids