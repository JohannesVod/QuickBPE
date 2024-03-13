# minbpe

compile tokenizer with linux:
g++ -fpic -shared -O3 -o funcs.so funcs.cpp -static-libstdc++

compile tokenizer with windows:
g++ -fpic -shared -O3 -o funcs.so funcs.cpp -static