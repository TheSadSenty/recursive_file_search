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
void parse_plugins_parameters(int argc, char *argv[], char *path_to_so)
{
    char *pch = strtok(path_to_so, ":");
    while (pch != NULL)
    {
        // struct option detected_opt = {0};
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
        for (size_t i = 0; i < pi.sup_opts_len; i++)
        {
            // printf("fill_options:%s\n", pi.sup_opts[i].opt.name);
            char *buff;
            buff = malloc(sizeof(pi.sup_opts[i].opt.name) + 2);
            sprintf(buff, "--%s", pi.sup_opts[i].opt.name);

            for (int opt = 0; opt < argc; opt++)
            {

                if (strncmp(buff, argv[opt], strlen(pi.sup_opts[i].opt.name) + 2) == 0)
                {
                    // printf("fill_options:--%s==%s\n", pi.sup_opts[i].opt.name, argv[opt]);
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
                            if (value == NULL)
                            {
                                printf("Options %s require value. Got nothing instead\n", param);
                                exit(EXIT_FAILURE);
                            }
                            printf("parse_plugins_parameters:%s=%s\n", param, value);
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
                        }
                    }
                }
            }
        }

        if (dl)
            dlclose(dl);
    END:
        if (dl)
            dlclose(dl);
        pch = strtok(NULL, ":");
    }
}