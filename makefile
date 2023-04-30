CC = gcc
CFLAGS = -Wall -Wextra -Werror
OUTPUT = labmksN32451.out
.PHONY: clean,all,build, debug, tar, dll
all: build

build: main.o string_search.o walk_dir_impl.o argument_parser.o validate_plugin.o
	$(CC) $^ -O3 -o $(OUTPUT)

debug: main.o string_search.o walk_dir_impl.o argument_parser.o validate_plugin.o
	$(CC) $^ -O0 -g -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c $<

string_search.o: string_search.c
	$(CC) $(CFLAGS) -c $<

walk_dir_impl.o: walk_dir_impl.c validate_plugin.o
	$(CC) $(CFLAGS) -c $<

argument_parser.o: argument_parser.c
	$(CC) $(CFLAGS) -c $<
validate_plugin.o: validate_plugin.c
	$(CC) $(CFLAGS) -c $<
tar:
	tar --create --file archive.tar *.c *.txt *.h makefile
dll: img_search.c
	$(CC) $(CFLAGS) -shared -fPIC -o img_search.so img_search.c -ldl -lm
clean:
	rm -rf *.o