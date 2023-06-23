#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
#include "colors.h"
int is_debug = 0;
int main(int argc, char *argv[])
{
    int is_v_or_h = 0; // If -h or -v are passed, don't start the search.
    if (getenv("LAB1DEBUG") != NULL)
    {
        is_debug = 1;
    }
    char path_to_so_dir[PATH_MAX] = {0}; // path to dir with plugins
    // First, search for the -P param.
    for (int i = 0; i < argc; i++)
    {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "-v") == 0))
        {
            is_v_or_h = 1;
            break;
        }
        if (strcmp(argv[i], "-P") == 0)
        {
            sprintf(path_to_so_dir, "%s", argv[i + 1]);
        }
    }
    // Otherwise use the default path.
    if ((strcmp(path_to_so_dir, "") == 0) && (is_v_or_h == 0))
    {
        sprintf(path_to_so_dir, "./");
    }
    if (strcmp(path_to_so_dir, "") != 0)
    {
        seach_plugins_fill_struct(path_to_so_dir);
    }

    // for getopt_long()
    if (plugins_options == NULL)
    {
        plugins_options = malloc(sizeof(struct option));
    }
    else
    {
        plugins_options = realloc(plugins_options, (option_count + 1) * sizeof(struct option));
    }
    plugins_options[option_count].name = NULL;
    plugins_options[option_count].has_arg = 0;
    plugins_options[option_count].val = 0;
    plugins_options[option_count].flag = NULL;

    if (is_debug)
    {
        for (int i = 0; i < plugin_count; i++)
        {
            printf(ANSI_COLOR_CYAN "Found plugin at: %s\n" ANSI_COLOR_RESET, array_dlls_path[i]);
        }
        for (int i = 0; i < option_count; i++)
        {
            printf(ANSI_COLOR_GREEN "Filled structure:\n\topt_name:%s\n\thas_arg:%d\n\tplugin number:%d\n" ANSI_COLOR_RESET, plugins_options[i].name, plugins_options[i].has_arg, plugins_options[i].val);
        }
    }

    argument_parser(argc, argv);
    walk_dir_call_plugin(argv[argc - 1]);
    return EXIT_SUCCESS;
}
