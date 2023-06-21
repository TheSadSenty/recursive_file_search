#include <stdio.h>
#include <stddef.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "plugin_api.h"
/*
Validate plugin by calling plugin_get_info()
return:
    0 - if all OK
    1 - if something wrong
 */
int validate_plugin(char *plugin_path)
{
    // init special struct
    printf("%s\n", plugin_path);
    struct plugin_info pi;
    // try to open .so
    void *dl = dlopen(plugin_path, RTLD_LAZY);
    if (!dl)
    {
        fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
        goto END;
    }
    // Check for plugin_get_info() func
    void *func = dlsym(dl, "plugin_get_info");
    if (!func)
    {
        fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
        goto END;
    }
    // init plugin_info func
    typedef int (*pgi_func_t)(struct plugin_info *);
    pgi_func_t pgi_func = (pgi_func_t)func;
    int ret = pgi_func(&pi);
    if (ret < 0)
    {
        // if ret!=0 then plugin is not valid
        fprintf(stderr, "ERROR: plugin_get_info() failed\n");
        goto END;
    }
    // Plugin info
    fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
    fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
    fprintf(stdout, "Supported options: ");
    if (pi.sup_opts_len > 0)
    {
        fprintf(stdout, "\n");
        for (size_t i = 0; i < pi.sup_opts_len; i++)
        {
            fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
        }
    }
    if (dl)
        dlclose(dl);
    return 0;
END:
    if (dl)
        dlclose(dl);
    return 1;
}
/*
    Function to fill a struct option with option names and values.
    Also checks for extra opthions and missing opthions.
    Function returns plugin count
    Params:
        int argc
        char *argv[]
        char *path_to_so - string "path_to_so_1:path_to_so_2..."
        struct option **detected_opt - struct to fill with plug options and values
        size_t **plugins_options_count - array. len=plugin count. Each element is thecount of detected options for each plugin.
    Return:
        int plugin_count
*/
int parse_plugins_parameters(int argc, char *argv[], char *path_to_so, struct option **detected_opt, size_t **plugins_options_count)
{

    char *pch = strtok(path_to_so, ":");
    int plugin_count = 0;

    int detected_option_count = 0; // all detected options
    while (pch != NULL)
    {
        if ((*plugins_options_count) == NULL)
        {
            (*plugins_options_count) = malloc(sizeof(size_t));
        }
        else
        {
            (*plugins_options_count) = realloc(*plugins_options_count, (plugin_count + 1) * sizeof(size_t));
        }

        struct plugin_info pi = {0};

        void *dl = dlopen(pch, RTLD_LAZY); // try to open .so
        if (!dl)
        {
            fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
            goto END;
        }

        void *func = dlsym(dl, "plugin_get_info"); // try to get plugin_get_info()
        if (!func)
        {
            fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
            goto END;
        }
        // init plugin_info func
        typedef int (*pgi_func_t)(struct plugin_info *);
        pgi_func_t pgi_func = (pgi_func_t)func;
        int ret = pgi_func(&pi);
        if (ret < 0)
        {
            // if ret!=0 then plugin is not valid
            fprintf(stderr, "ERROR: plugin_get_info() failed\n");
            goto END;
        }
        (*plugins_options_count)[plugin_count] = 0;
        for (size_t i = 0; i < pi.sup_opts_len; i++)
        {
            // printf("fill_options:%s\n", pi.sup_opts[i].opt.name);
            char *buff;
            char *param_name;
            buff = malloc(sizeof(pi.sup_opts[i].opt.name) + 2);
            param_name = malloc(sizeof(pi.sup_opts[i].opt.name));
            sprintf(buff, "--%s", pi.sup_opts[i].opt.name);
            sprintf(param_name, "%s", pi.sup_opts[i].opt.name);
            for (int opt = 0; opt < argc; opt++)
            {

                if (strncmp(buff, argv[opt], strlen(pi.sup_opts[i].opt.name) + 2) == 0)
                {
                    (*plugins_options_count)[plugin_count] = (*plugins_options_count)[plugin_count] + 1;
                    if (pi.sup_opts[i].opt.has_arg == required_argument)
                    {

                        size_t last_char = strlen(pi.sup_opts[i].opt.name) + 2;

                        char *copy_argv;
                        copy_argv = malloc(sizeof(argv[opt]) + 1);
                        sprintf(copy_argv, "%s", argv[opt]);
                        if (argv[opt][last_char] == '=')
                        {
                            char *save_res;
                            char *param = __strtok_r(copy_argv, "=", &save_res);
                            char *value = __strtok_r(NULL, "=", &save_res);
                            //
                            if (value == NULL)
                            {
                                printf("Options %s require value. Got nothing instead\n", param);
                                exit(EXIT_FAILURE);
                            }
                            printf("parse_plugins_parameters:%s=%s\n", param, value);
                            // add all plug opt to list to compare them after
                            // for "param=value" format
                            // fill struct with data
                            // What do you think about Italy?
                            if ((*detected_opt) == NULL)
                            {
                                *detected_opt = malloc(sizeof(struct option *));
                            }
                            else
                            {
                                *detected_opt = realloc(*detected_opt, (detected_option_count + 1) * sizeof(struct option));
                            }
                            (*detected_opt)[detected_option_count].name = malloc(sizeof(param_name));
                            (*detected_opt)[detected_option_count].name = param_name;

                            (*detected_opt)[detected_option_count].has_arg = 1;

                            (*detected_opt)[detected_option_count].flag = malloc(sizeof(value));
                            (*detected_opt)[detected_option_count].flag = (int *)value;

                            //free(copy_argv);
                        }
                        else
                        {
                            if (argv[opt + 1] == NULL)
                            {
                                printf("Options %s require value. Got nothing instead\n", argv[opt]);
                                exit(EXIT_FAILURE);
                            }
                            if (((argv[opt + 1][0] == '-') && (argv[opt + 1][1] == '-')))
                            {
                            }
                            printf("parse_plugins_parameters:%s=%s\n", argv[opt], argv[opt + 1]);
                            // add all plug opt to list to compare them after
                            // for "param value" format
                            // What do you think about Italy? x2
                            if ((*detected_opt)== NULL)
                            {
                                (*detected_opt) = malloc(sizeof(struct option *));
                            }
                            else
                            {
                                (*detected_opt) = realloc(*detected_opt, (detected_option_count + 1) * sizeof(struct option));
                            }
                            (*detected_opt)[detected_option_count].name = malloc(sizeof(param_name));
                            (*detected_opt)[detected_option_count].name = param_name;

                            (*detected_opt)[detected_option_count].has_arg = 1;

                            (*detected_opt)[detected_option_count].flag = malloc(sizeof(argv[opt + 1]));
                            (*detected_opt)[detected_option_count].flag = (int *)argv[opt + 1];
                        }
                    }
                    else
                    {
                        // add all plug opt to list to compare them after
                        // for param only format
                        ////What do you think about Italy?X3
                        if ((*detected_opt) == NULL)
                        {
                            (*detected_opt) = malloc(sizeof(struct option *));
                        }
                        else
                        {
                            (*detected_opt) = realloc(*detected_opt, (detected_option_count + 1) * sizeof(struct option));
                        }
                        (*detected_opt)[detected_option_count].name = malloc(sizeof(param_name));
                        (*detected_opt)[detected_option_count].name = param_name;

                        (*detected_opt)[detected_option_count].has_arg = 0;
                    }
                    // increment only if a valid argument
                    detected_option_count = detected_option_count + 1;
                }
            }
            //free(buff);
            //free(param_name);
        }
        if (dl)
            dlclose(dl);
    END:
        if (dl)
            dlclose(dl);
        pch = strtok(NULL, ":");

        plugin_count = plugin_count + 1;
    }
    if ((*detected_opt) == NULL)
    {
        (*detected_opt) = malloc(sizeof(struct option *));
    }
    else
    {
        (*detected_opt) = realloc(*detected_opt, (detected_option_count + 1) * sizeof(struct option));
    }

    (*detected_opt)[detected_option_count].name = NULL;
    (*detected_opt)[detected_option_count].has_arg = 0;
    (*detected_opt)->flag = NULL;
    (*detected_opt)[detected_option_count].val = 0;


    // for getopt_long()
    int choice;
    int option_index = 0;
    while ((choice = getopt_long(argc, argv, "PAON", *detected_opt, &option_index)) != -1)
    {
        switch (choice)
        {
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    return plugin_count;
}