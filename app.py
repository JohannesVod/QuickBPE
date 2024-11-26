from flask import Flask, render_template, request
from minbpe import RegexTokenizerFast

# Initialize Flask app
app = Flask(__name__)

# Initialize tokenizer
tokenizer = RegexTokenizerFast()
tokenizer.load("toy.model")

# List of fixed colors
colors = ["#28a745", "#fd7e14", "#007bff", "#dc3545"]  # green, orange, blue, red

@app.route("/", methods=["GET", "POST"])
def home():
    tokenized_output = None
    user_input = ""
    compression_ratio = None

    if request.method == "POST":
        user_input = request.form["text_input"]
        token_ids = tokenizer.encode_ordinary(user_input)
        tokens = [tokenizer.decode([tid]) for tid in token_ids]  # Decode each token

        # Cycle through colors for each token
        tokenized_output = [
            {"token": token, "color": colors[i % len(colors)]}  # Use modulo to cycle through colors
            for i, token in enumerate(tokens)
        ]

        # Calculate compression ratio
        input_length = len(user_input)
        output_length = len(token_ids)
        compression_ratio = output_length / input_length if output_length > 0 else None

    return render_template(
        "index.html",
        user_input=user_input,
        tokenized_output=tokenized_output,
        compression_ratio=compression_ratio,
    )

if __name__ == "__main__":
    app.run(debug=True)
