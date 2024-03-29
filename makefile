CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
OUTPUT = labmksN32451.out
.PHONY: clean,all,build, debug, tar
all: build

build: main.o string_search.o walk_dir_impl.o argument_parser.o
	$(CC) $^ -O3 -o $(OUTPUT)
	strip $(OUTPUT)

debug: main.o string_search.o walk_dir_impl.o argument_parser.o
	$(CC) $^ -O0 -g -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c $<

string_search.o: string_search.c
	$(CC) $(CFLAGS) -c $<

walk_dir_impl.o: walk_dir_impl.c string_search.o
	$(CC) $(CFLAGS) -c $<

argument_parser.o: argument_parser.c
	$(CC) $(CFLAGS) -c $<
tar:
	tar --create --file archive.tar *.c *.txt *.h makefile
clean:
	rm -rf *.o