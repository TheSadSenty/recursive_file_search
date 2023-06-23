CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
OUTPUT = lab1mksN32451.out
.PHONY: clean,all,build, debug, tar, dll
all: build

build: main.o seach_plugins_fill_struct.o argument_parser.o walk_dir_call_plugin.o
	$(CC) $^ $(CFLAGS) -O3 -o $(OUTPUT)
	strip $(OUTPUT)

debug: main.o seach_plugins_fill_struct.o argument_parser.o walk_dir_call_plugin.o
	$(CC) $^ $(CFLAGS) -O0 -o $(OUTPUT)

main.o: main.c
	$(CC) $(CFLAGS) -c $<

seach_plugins_fill_struct.o: seach_plugins_fill_struct.c
	$(CC) $(CFLAGS) -c $<

argument_parser.o: argument_parser.c
	$(CC) $(CFLAGS) -c $<

walk_dir_call_plugin.o: walk_dir_call_plugin.c
	$(CC) $(CFLAGS) -c $<

tar:
	tar --create --file archive.tar *.c *.txt *.h makefile

dll: img_search.c
	$(CC) $(CFLAGS) -shared -fPIC -o libmksN32451.so img_search.c -ldl -lm

clean:
	rm -rf *.o