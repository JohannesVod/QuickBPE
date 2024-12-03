CC = gcc
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS += -Wno-pointer-arith -Wno-newline-eof -Wno-unused-parameter -Wno-gnu-statement-expression
CFLAGS += -Wno-gnu-compound-literal-initializer -Wno-gnu-zero-variadic-macro-arguments

CXX = g++
CXXFLAGS = -std=c++11 -O3 -g -Wall -Wextra -Wpedantic
TARGET = bpe
SRC = bpe.c
SRC2 = encode.cpp

ENCSO = encode.so
BPESO = bpe.so

all: $(TARGET) $(ENCSO) $(BPESO)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(ENCSO): $(SRC2)
	$(CXX) $(CXXFLAGS) -fPIC -shared -o $(ENCSO) $(SRC2)

$(BPESO): $(SRC)
	$(CC) $(CFLAGS) -shared -fPIC -o $(BPESO) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(ENCSO) $(BPESO)

.PHONY: all run clean