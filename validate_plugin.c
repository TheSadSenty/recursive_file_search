#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "functions.h"
#include "plugin_api.h"
#include "colors.h"
#include <errno.h>

struct option *select_all_plugin_opts(struct option **detected_opt, int plugin_index);

void walk_dir_call_plugin(char *dir)
{
    DIR *d = opendir(dir); // try to open dir
    if (d == NULL)
    {
        if (getenv("LAB11DEBUG") != NULL)
        {
            printf(ANSI_COLOR_RED "Failed to opendir() %s" ANSI_COLOR_RESET "\n", dir);
            if (errno == EACCES)
            {
                fprintf(stderr, ANSI_COLOR_RED "Access denied: %s" ANSI_COLOR_RESET "\n", dir);
            }
        }

        return;
    }

    while (1)
    {
        errno = 0;
        struct dirent *p = readdir(d);
        if (p == NULL)
        {
            if (errno != 0)
            {
                if (getenv("LAB11DEBUG") != NULL)
                {
                    fprintf(stderr, ANSI_COLOR_RED "Failed to read dir: %s" ANSI_COLOR_RESET "\n", dir);
                }

                continue; // issue, go to next element
            }

            else
                break; // There is nothing else in the catalog
        }

        if (strcmp(p->d_name, ".") && strcmp(p->d_name, ".."))
        {

            if (p->d_type == DT_REG) // if it's a regular file start search
            {
                char file_path[PATH_MAX] = {0}; // construct path to files
                sprintf(file_path, "%s/%s", dir, p->d_name);
                printf("file_path:%s\n", file_path);
                for (int plugin_index = 0; plugin_index < plugin_count; plugin_index++)
                {
                    // Iterate through all plugins for each file.
                    printf("Trying to open:%s:%ld\n", array_dlls_path[plugin_index], actual_options_count[plugin_index]);
                    void *dl = dlopen(array_dlls_path[plugin_index], RTLD_LAZY);
                    if (!dl)
                    {
                        fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
                        goto END;
                    }
                    void *func = dlsym(dl, "plugin_process_file");
                    if (!func)
                    {
                        fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
                        goto END;
                    }
                    // typedef int (*ppf_func_t)(const char *, struct option *, size_t);
                    // ppf_func_t ppf_func = (ppf_func_t)func;
                    // struct option *sliced_plug_options = select_all_plugin_opts(&plugins_options, i);
                    struct option *sliced_opt = malloc(sizeof(struct option) * actual_options_count[plugin_index]);
                    printf("option_count:%d\n", option_count);
                    int index = 0;
                    /*
                    To call plugin_process_file(), you need to prepare the structure with parameters from only one plugin.
                    Every parameter signed by plugin index (from array_dlls_path) at the "val" field
                    TODO: Add a check to see if the option was passed.
                    */
                    for (int opt = 0; opt < option_count; opt++)
                    {
                        // Search for plugins opt where "val" field equals current plugin_index
                        if ((plugins_options[opt].val == plugin_index)&&(actual_detected_options[opt]==1))
                        {
                            sliced_opt[index].name = malloc(sizeof(plugins_options[opt].name));
                            sprintf((char *)sliced_opt[index].name, "%s", plugins_options[opt].name);

                            sliced_opt[index].has_arg = plugins_options[opt].has_arg;

                            sliced_opt[index].flag = malloc(sizeof(plugins_options[opt].flag));
                            sliced_opt[index].flag = plugins_options[opt].flag;
                            index = index + 1;
                        }
                    }
                    for (size_t j = 0; j < actual_options_count[plugin_index]; j++)
                    {
                        printf("sliced_plug_options:%s:%d:%s\n", sliced_opt[j].name, sliced_opt[j].has_arg, (char *)sliced_opt[j].flag);
                    }
                    printf("Finish\n");
                    // int ret = ppf_func(file_path, sliced_plug_options, actual_options_count[i]);

                    // printf("fname:%s,result:%d\n", file_path, ret);
                END:
                    if (dl)
                        dlclose(dl);
                }
            }

            if (p->d_type == DT_DIR) // if it's a directory start run functhion again
            {
                char buf[PATH_MAX] = {0};
                sprintf(buf, "%s/%s", dir, p->d_name);
                walk_dir_call_plugin(buf);
            }
        }
    }
    closedir(d);
}