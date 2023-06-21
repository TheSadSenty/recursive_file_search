#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
void argument_parser(int argc, char *argv[])
{
    static char *path_so;
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
        /* Get path to all .so in dir */
        path_so = seach_so(argv[2]);
    }
    else
    {
        /* Unrecognized options handler */
        printf("%s: Unrecognized options\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf(".so: %s\n", path_so);
    static struct option *detected_opt;   // struct for plugin_process_file()
    static size_t *plugins_options_count; // array of detected options for each plugin
    int plugin_count = 0;
    int detected_option_count = 0;

    plugin_count = parse_plugins_parameters(argc, argv, path_so, &detected_opt, &plugins_options_count);

    for (int i; i < plugin_count; i++)
    {
        detected_option_count=detected_option_count+plugins_options_count[i];
    }
    printf("detected_option_count:%d\n", detected_option_count);
    for (int i = 0; i < detected_option_count; i++)
    {
        printf("struct:%s\n", detected_opt[i].name);
    }
    for (int i = 0; i < plugin_count; i++)
    {
        printf("plugin_opt:%ld\n", plugins_options_count[i]);
    }
    free(plugins_options_count);
    free(detected_opt);
}