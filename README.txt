make/make all - compile the main program and plugin without debug information.
make clean - remove *.o.
make debug - compilation the main program with debug information.
make dll - compilation plugin with debug information. 
make tar - archive *.c, *.txt, *.h and makefile
---
Usage: ./lab1mksN32451.out -P /path/to/*.so [logical operators][plugins parameters]
Options:
-P              Path to plugins, which implements plugin_api.h interface
-A              Logical AND for plugin options
-O              Logical OR for plugin options
-N              Logical NOT for plugin options, after -A or -O options
-h, --help      Print help
-v, --version   Print version
Example:
./lab1mksN32451.out -O -N --exe --file-len 1000 /tmp    Search any files except for executable with length==1000 bytes
---
Launch with debug output: export LAB1DEBUG=1