CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3
OUTPUT = labmksN32451.out
.PHONY: clean,all,build, debug
all: build

build: main.o string_search.o walk_dir_impl.o argument_parser.o
	$(CC) $^ -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c $<

string_search.o: string_search.c
	$(CC) $(CFLAGS) -c $<

walk_dir_impl.o: walk_dir_impl.c string_search.o
	$(CC) $(CFLAGS) -c $<

argument_parser.o: argument_parser.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o