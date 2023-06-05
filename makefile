CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
OUTPUT = labmksN32451.out
.PHONY: clean,all,build, debug, tar, dll
all: build

build: main.o walk_dir_impl.o argument_parser.o validate_plugin.o
	$(CC) $^ $(CFLAGS) -O3 -o $(OUTPUT)
	strip $(OUTPUT)

debug: main.o walk_dir_impl.o argument_parser.o validate_plugin.o
	$(CC) $^ $(CFLAGS) -O0 -o $(OUTPUT)

main.o: main.c
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