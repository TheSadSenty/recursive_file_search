CC = gcc
CFLAGS = -Wall -Wextra -Werror
OUTPUT = labmksN32451.out
.PHONY: clean,all
all:
	$(CC) -o $(OUTPUT) $(CFLAGS) main.c
clean:
	rm *.out