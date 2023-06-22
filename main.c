#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
int main(int argc, char *argv[])
{
    char path_to_so_dir[PATH_MAX] = {0}; // path to dir with plugins
    // First, search for the -P param.
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-P") == 0)
        {
            sprintf(path_to_so_dir, "%s", argv[i + 1]);
        }
    }
    // Otherwise use the default path.
    if (strcmp(path_to_so_dir,"")==0)
    {
        sprintf(path_to_so_dir, "./");
    }
    printf("path_to_so_dir:%s\n", path_to_so_dir);
    seach_plugins(path_to_so_dir);
    for (int i = 0; i < plugin_count; i++)
    {
        printf("Found plugin at: %s\n", array_dlls_path[i]);
    }
    // argument_parser(argc, argv);
    return EXIT_SUCCESS;
}
