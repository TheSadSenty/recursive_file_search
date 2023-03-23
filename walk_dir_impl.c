#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
void walk_dir_impl(char *dir, char *sequence)
{
    DIR *d = opendir(dir);
    if (d == NULL)
    {
        if (getenv("LAB11DEBUG") != NULL)
        {
            printf("Failed to opendir() %s\n", dir);
            if (errno == EACCES)
            {
                fprintf(stderr, "Access denied: %s\n", dir);
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
                    fprintf(stderr, "Failed to read dir: %s\n", dir);
                }

                continue; // Проблема, переходим к следующему элементу
            }

            else
                break; // В каталоге больше ничего нет
        }

        if (strcmp(p->d_name, ".") && strcmp(p->d_name, ".."))
        {

            if (p->d_type == DT_REG)
            {
                char file_path[PATH_MAX] = {0}; // construct path to files
                sprintf(file_path, "%s/%s", dir, p->d_name);
                string_search(file_path, sequence);
            }

            if (p->d_type == DT_DIR)
            {
                char buf[PATH_MAX] = {0};
                sprintf(buf, "%s/%s", dir, p->d_name);
                walk_dir_impl(buf, sequence);
            }
        }
    }

    closedir(d);
}