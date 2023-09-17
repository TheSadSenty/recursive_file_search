branch main: Program for recursive string search in selected directory
img_search: Program for recursive file search, which satisfied all conditions. Conditions set by plugins with specific interface. Program supports simple logical expressions
---
make/make all - compilation with -Wall -Wextra -Werror and -O3
make clean - remove *.o
make debug - compilation with -Wall -Wextra -Werror and -O0 -g (for valgrind)
make tar - archive *.c, *.txt, *.h and makefile
---
Usage: ./labmksN32451.out <dir> <string>
<dir> - directory to search
<string> - string to search