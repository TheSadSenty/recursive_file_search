#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
#include "colors.h"
int is_A = 0;
int is_O = 0;
int is_N = 0;
size_t *actual_options_count;
int *actual_detected_options;
void argument_parser(int argc, char *argv[])
{
    actual_options_count = calloc(plugin_count, sizeof(size_t)); // How many options were detected for each plugin
    actual_detected_options = calloc(option_count, sizeof(int)); // Set 1 if options from plugins_options were passed

    int option_index = -1;
    int rez;
    while ((rez = getopt_long(argc, argv, "P:AONhv", plugins_options, &option_index)) != -1)
    {
        switch (rez)
        {
        case 'h':
            // Print help
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
            break;
        case 'v':
            // Print version
            printf("Kasyanov Maxim\nN32451\nVariant 13\n");
            exit(EXIT_SUCCESS);
        case 'P':
            // do nothing
            break;
        case 'A':
            is_A = 0;
            is_O = 0;
            break;
        case 'O':
            is_O = 1;
            is_A = 0;
            break;
        case 'N':
            if (is_A || is_O)
            {
                is_N = 0;
            }
            else
            {
                if (is_debug)
                {
                    fprintf(stderr, ANSI_COLOR_RED "-N requires the -A or -O option." ANSI_COLOR_RESET);
                }
                exit(EXIT_FAILURE);
            }
            break;
        case '?':
            exit(EXIT_FAILURE);
            break;
        default:
            if (option_index != -1) // Check for the short option. If getopt_long has short options, it doesn't update option_index.
            {
                if (optarg != NULL)
                {
                    // Fill the struct with values.
                    if (plugins_options[option_index].flag == NULL)
                    {
                        plugins_options[option_index].flag = malloc(sizeof(optarg));
                    }

                    plugins_options[option_index].flag = (int *)optarg;
                }
                // Array of the number of detected options for each plugin
                actual_options_count[plugins_options[option_index].val] = actual_options_count[plugins_options[option_index].val] + 1;

                // Array of the presence of each option
                actual_detected_options[option_index] = 1;
            }
            break;
        }
        option_index = -1;
    }
    if (is_debug)
    {
        for (int i = 0; i < option_count; i++)
        {
            printf(ANSI_COLOR_CYAN "After getopt_long():\n\topt_name:%s\n\thas_arg:%d\n\topt_value:%s\n\tpresence:%d\n" ANSI_COLOR_RESET, plugins_options[i].name, plugins_options[i].has_arg, (char *)plugins_options[i].flag, actual_detected_options[i]);
        }
        for (int i = 0; i < plugin_count; i++)
        {
            printf(ANSI_COLOR_GREEN "detected options for %s:\t%ld\n" ANSI_COLOR_RESET, array_dlls_path[i], actual_options_count[i]);
        }
    }
}
