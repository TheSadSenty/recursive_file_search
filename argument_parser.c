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
        case 0:

        case 'h':
            printf("Usage: %s <dir> <string>\n", argv[0]);
            printf("<dir> - directory to search\n");
            printf("<string> - string to search\n");
            exit(EXIT_SUCCESS);
        case 1:

        case 'v':
            printf("Kasyanov Maxim\nN32451\nVariant 2\n");
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