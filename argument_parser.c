#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
void argument_parser(int argc, char *argv[])
{
    if ((argc == 1) || ((strncmp(argv[1], "-h", sizeof("-h"))) == 0 && (argc == 2)))
    {
        /* Print help */
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
    }
    if ((argc == 2) && strncmp(argv[1], "-v", sizeof("-v")) == 0)
    {
        /* Print version */
        printf("Kasyanov Maxim\nN32451\nVariant 13\n");
        exit(EXIT_SUCCESS);
    }
    if (strncmp(argv[1], "-P", sizeof("-P")) == 0)
    {
        /* Start search for .so files in directory */
        walk_dir_impl(argv[2]);
    }
    else
    {
        /* Unrecognized options handler */
        printf("%s: Unrecognized options\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}