#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
int main(int argc, char *argv[])
{
    char path_to_so_dir[PATH_MAX] = {0};
    // First, search for the -P param.
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-P") == 0)
        {
            sprintf(path_to_so_dir, "%s", argv[i]);
        }
    }
    // Otherwise use the default path.
    if (path_to_so_dir == NULL)
    {
        sprintf(path_to_so_dir, "./");
    }
    // argument_parser(argc, argv);
    return EXIT_SUCCESS;
}
