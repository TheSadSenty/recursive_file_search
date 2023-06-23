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

                int *result = calloc(plugin_count, sizeof(int)); // to store results
                for (int plugin_index = 0; plugin_index < plugin_count; plugin_index++)
                {
                    // Iterate through all plugins for each file.
                    if (is_debug)
                        printf(ANSI_COLOR_YELLOW "Trying to open %s to process %s\n" ANSI_COLOR_RESET, array_dlls_path[plugin_index], file_path);
                    void *dl = dlopen(array_dlls_path[plugin_index], RTLD_LAZY);
                    if (!dl)
                    {
                        fprintf(stderr, ANSI_COLOR_RED "ERROR: dlopen() failed: %s\n" ANSI_COLOR_RESET, dlerror());
                        goto END;
                    }
                    void *func = dlsym(dl, "plugin_process_file");
                    if (!func)
                    {
                        fprintf(stderr, ANSI_COLOR_RED "ERROR: no plugin_process_file() function found\n" ANSI_COLOR_RESET);
                        goto END;
                    }

                    struct option *sliced_opt = malloc(sizeof(struct option) * actual_options_count[plugin_index]);
                    int index = 0;
                    /*
                    To call plugin_process_file(), you need to prepare the structure with parameters from only one plugin.
                    Every parameter signed by plugin index (from array_dlls_path) at the "val" field
                    */
                    for (int opt = 0; opt < option_count; opt++)
                    {
                        // Search for plugins opt where "val" field equals current plugin_index
                        if ((plugins_options[opt].val == plugin_index) && (actual_detected_options[opt] == 1))
                        {
                            sliced_opt[index].name = malloc(sizeof(plugins_options[opt].name));
                            sprintf((char *)sliced_opt[index].name, "%s", plugins_options[opt].name);

                            sliced_opt[index].has_arg = plugins_options[opt].has_arg;

                            sliced_opt[index].flag = plugins_options[opt].flag;
                            index = index + 1;
                        }
                    }
                    if (is_debug)
                    {
                        for (size_t j = 0; j < actual_options_count[plugin_index]; j++)
                            printf(ANSI_COLOR_CYAN "Prepared struct for plugin_process_file():\n\tname:%s\n\thas_arg:%d\n\tflag:%s\n" ANSI_COLOR_RESET, sliced_opt[j].name, sliced_opt[j].has_arg, (char *)sliced_opt[j].flag);
                    }

                    typedef int (*ppf_func_t)(const char *, struct option *, size_t);
                    ppf_func_t ppf_func = (ppf_func_t)func;
                    int ret = ppf_func(file_path, sliced_opt, actual_options_count[plugin_index]);
                    if (ret < 0)
                    {
                        if (is_debug)
                        {
                            fprintf(stderr, ANSI_COLOR_RED "Plugin %s failed to process file %s\n" ANSI_COLOR_RESET, array_dlls_path[plugin_index], file_path);
                            result[plugin_index] = -1;
                            goto END;
                        }
                    }
                    else
                    {
                        result[plugin_index] = ret;
                    }
                    if (is_debug)
                        printf(ANSI_COLOR_GREEN "plugin %s processed file %s with result %d\n" ANSI_COLOR_RESET, array_dlls_path[plugin_index], file_path, ret);
                END:
                    if (dl)
                        dlclose(dl);
                    if (sliced_opt)
                    {
                        for (size_t j = 0; j < actual_options_count[plugin_index]; j++)
                            free((char *)sliced_opt[j].name);
                        free(sliced_opt);
                    }
                }
                // processing results
                int log_result = -1;
                for (int plugin_index = 0; plugin_index < plugin_count; plugin_index++)
                {
                    // skipping errors
                    if (result[plugin_index] > -1)
                    {
                        // Check for only one plugin
                        if (plugin_count > 1)
                        {
                            // init log_result
                            if (log_result == -1)
                                log_result = result[plugin_index];
                            else
                            {
                                switch (is_A)
                                {
                                case -1: // by default
                                case 1:
                                    // if option -A was passed
                                    log_result = log_result && result[plugin_index];
                                    break;

                                case 0:
                                    // if option -A wasn't passed
                                    log_result = log_result || result[plugin_index];
                                    break;
                                }
                            }
                        }
                        else
                            log_result = result[plugin_index];
                    }
                }
                if (is_N)
                    log_result = !log_result;
                if (log_result)
                    printf(ANSI_COLOR_GREEN "Found a file that satisfied all conditions: %s\n" ANSI_COLOR_RESET, file_path);
                if (log_result == -1)
                {
                    if (is_debug)
                    {
                        fprintf(stderr, ANSI_COLOR_RED "All plugins failed to process file: %s\n" ANSI_COLOR_RESET, file_path);
                    }
                }
                if (result)
                    free(result);
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