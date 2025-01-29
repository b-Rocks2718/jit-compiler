# Variables
CC = gcc
CFLAGS = -Wall -g
OBJFILES = $(patsubst %.c, %.o, $(wildcard *.c))
EXEC = main

# Rule to build the main executable
$(EXEC): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJFILES)

# Rule to generate .o files from .c files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up generated files
clean:
	rm -f $(OBJFILES) $(EXEC)

# Phony targets
.PHONY: clean