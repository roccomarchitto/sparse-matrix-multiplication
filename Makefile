# Sparse matrix vector multiplication (SPMV) makefile
# Call 'make' to recompile
# Call 'make clean' to clean up and remove old binaries
# Call 'make run' to recompile and run
CC = gcc
CFLAGS = -fopenmp -lstdc++ -lm -I eigen-3.4.0
SRC = spmv.cpp
TARGET = spmv

$(TARGET): $(SRC)
	$(CC) $^ $(CFLAGS) -o $@

run: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)
