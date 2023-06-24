#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>
#include <dlfcn.h>
#include "functions.h"
#include "colors.h"

char **array_dlls_path; // array of strings with dlls path
int plugin_count;       // number of plugins
struct option *plugins_options;
int option_count = 0;
/*
Search for .so in dir.
Functhion allocates an array of strings and increments plugin_count.
*/
void seach_plugins_fill_struct(char *dir)
{
    DIR *d = opendir(dir); // try to open dir
    if (d == NULL)
    {
        if (is_debug)
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
                if (is_debug)
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
                if (is_debug)
                    printf(ANSI_COLOR_YELLOW "Processing file:%s\n" ANSI_COLOR_RESET, file_path);
                // init special struct
                struct plugin_info pi;
                // try to open file
                void *dl = dlopen(file_path, RTLD_LAZY);
                if (!dl)
                {
                    if (is_debug)
                        fprintf(stderr, ANSI_COLOR_RED "ERROR: dlopen() failed: %s\n" ANSI_COLOR_RESET, dlerror());
                    goto END;
                }
                // Check for plugin_get_info() func
                void *func = dlsym(dl, "plugin_get_info");
                if (!func)
                {
                    if (is_debug)
                        fprintf(stderr, ANSI_COLOR_RED "ERROR: dlsym() failed: %s\n" ANSI_COLOR_RESET, dlerror());
                    goto END;
                }
                // init plugin_info func
                typedef int (*pgi_func_t)(struct plugin_info *);
                pgi_func_t pgi_func = (pgi_func_t)func;
                int ret = pgi_func(&pi);
                if ((ret < 0))
                {
                    // if ret!=0 then plugin is not valid
                    if (is_debug)
                        fprintf(stderr, ANSI_COLOR_RED "ERROR: plugin_get_info() failed\n" ANSI_COLOR_RESET);
                    goto END;
                }
                // Plugin info
                if (is_debug)
                {
                    fprintf(stdout, ANSI_COLOR_BLUE "Plugin purpose:\t\t%s\n" ANSI_COLOR_RESET, pi.plugin_purpose);
                    fprintf(stdout, ANSI_COLOR_BLUE "Plugin author:\t\t%s\n" ANSI_COLOR_RESET, pi.plugin_author);
                    fprintf(stdout, ANSI_COLOR_BLUE "Supported options: " ANSI_COLOR_RESET);
                    if (pi.sup_opts_len > 0)
                    {
                        fprintf(stdout, "\n");
                        for (size_t i = 0; i < pi.sup_opts_len; i++)
                        {
                            fprintf(stdout, ANSI_COLOR_GREEN "\t--%s\t\t%s\n" ANSI_COLOR_RESET, pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
                        }
                    }
                }

                if (array_dlls_path == NULL)
                {

                    array_dlls_path = malloc(sizeof(char *));
                }
                else
                {
                    array_dlls_path = realloc(array_dlls_path, (sizeof(char *) * plugin_count) + 1);
                }
                array_dlls_path[plugin_count] = malloc(sizeof(file_path));
                sprintf(array_dlls_path[plugin_count], "%s", file_path);

                for (size_t i = 0; i < pi.sup_opts_len; i++)
                {
                    if (plugins_options == NULL)
                    {
                        plugins_options = malloc(sizeof(struct option));
                    }
                    else
                    {
                        plugins_options = realloc(plugins_options, (option_count + 1) * sizeof(struct option));
                    }
                    plugins_options[option_count].name = malloc(sizeof(pi.sup_opts[i].opt.name));
                    sprintf((char *)plugins_options[option_count].name, "%s", pi.sup_opts[i].opt.name);

                    plugins_options[option_count].has_arg = pi.sup_opts[i].opt.has_arg;
                    
                    plugins_options[option_count].flag = NULL; 
                    // use an unused field to store the plugin number.
                    plugins_options[option_count].val = plugin_count;

                    option_count = option_count + 1;
                }

                plugin_count = plugin_count + 1;
            END:
                if (dl)
                    dlclose(dl);
            }

            if (p->d_type == DT_DIR) // if it's a directory start run functhion again
            {
                char buf[PATH_MAX] = {0};
                sprintf(buf, "%s/%s", dir, p->d_name);
                seach_plugins_fill_struct(buf);
            }
        }
    }
    closedir(d);
}