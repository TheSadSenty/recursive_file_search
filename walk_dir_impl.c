#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
#include "colors.h"

void walk_dir_impl(char *dir);
static char *dlls_path;
/*
Search for .so in dir
return:
    Path to all found .so separated by ":"
*/
char *seach_so(char *dir)
{
    walk_dir_impl(dir);
    return dlls_path;
}

void walk_dir_impl(char *dir)
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

                char *extension;
                extension = memchr(p->d_name, '.', strlen(p->d_name));
                // Process .so files
                if (strcmp(extension, ".so") == 0)
                {
                    if (validate_plugin(file_path) == 0)
                    {
                        if (!dlls_path)
                        {
                            //for the first time
                            dlls_path = calloc(strlen(file_path), sizeof(file_path) + 1);
                            sprintf(dlls_path, "%s", file_path);
                        }
                        else
                        {
                            //for the other times
                            dlls_path = realloc(dlls_path, strlen(dlls_path) + 1 + sizeof(file_path) + 1);
                            strcat(dlls_path, ":");
                            strcat(dlls_path, file_path);
                        }
                    }
                }
                else
                {
                    printf("Regular file\n");
                }
            }

            if (p->d_type == DT_DIR) // if it's a directory start run functhion again
            {
                char buf[PATH_MAX] = {0};
                sprintf(buf, "%s/%s", dir, p->d_name);
                walk_dir_impl(buf);
            }
        }
    }
    closedir(d);
}