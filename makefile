CC = gcc
CFLAGS = -Wall -Wextra -Werror
OUTPUT = labmksN32451.out
.PHONY: clean,all
all: hello
hello: main.o string_search.o walk_dir_impl.o argument_parser.o
	gcc main.o string_search.o walk_dir_impl.o argument_parser.o -o $(OUTPUT)
main.o: main.c
	gcc $(CFLAGS) -c main.c
string_search.o: string_search.c
	gcc $(CFLAGS) -c string_search.c
walk_dir_impl.o: walk_dir_impl.c string_search.o
	gcc $(CFLAGS) -c walk_dir_impl.c
argument_parser.o: argument_parser.c
	gcc $(CFLAGS) -c argument_parser.c
clean:
	rm -rf *.o