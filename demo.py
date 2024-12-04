from minbpe import RegexTokenizerFast

tokenizer = RegexTokenizerFast()
tokenizer.load("webtokenizer.model")

while True:
    test_inpt = input("type input here: ")
    token_ids = tokenizer.encode_ordinary(test_inpt)
    tokens = [tokenizer.decode([tid]) for tid in token_ids]  # Decode each token
    print("tokenization: ")
    print("|".join(tokens))

    # Calculate compression ratio
    input_length = len(test_inpt)
    output_length = len(token_ids)
    compression_ratio = output_length / input_length if output_length > 0 else None
    print("compression ratio: ", compression_ratio)