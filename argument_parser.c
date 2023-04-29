#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
void argument_parser(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"help", no_argument, 0, 0},
        {"version", no_argument, 0, 1}};
    int option_index = 0;
    int choice;
    while ((choice = getopt_long(argc, argv, "vh", long_options, &option_index)) != -1)
    {
        switch (choice)
        {
        case 0://if choice 0 or h - print help

        case 'h':
            printf("Usage: %s -P /path/to/*.so [logical operators][plugins parameters]\n", argv[0]);
            printf("Options:\n");
            printf("-P\t\tPath to plugins, which implements plugin_api.h interface\n");
            printf("-A\t\tLogical AND for plugin options\n");
            printf("-O\t\tLogical OR for plugin options\n");
            printf("-N\t\tLogical NOT for plugin options, after -A or -O options\n");
            printf("-h, --help\tPrint help\n");
            printf("-v, --version\tPrint version\n");
            printf("Example:\n");
            printf("%s -O -N --exe --file-len 1000 /tmp\tSearch any files except for executable with length==1000 bytes\n", argv[0]);
            exit(EXIT_SUCCESS);
        case 1:

        case 'v'://if choice 1 or v - print version
            printf("Kasyanov Maxim\nN32451\nVariant 13\n");
            exit(EXIT_SUCCESS);
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 3)
    {
        walk_dir_impl(argv[1], argv[2]);
    }
}