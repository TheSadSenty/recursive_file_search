#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
#include "colors.h"
void walk_dir_impl(char *dir, char *sequence)
{
    DIR *d = opendir(dir);//try to open dir
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

            if (p->d_type == DT_REG)// if it's a regular file start search
            {
                char file_path[PATH_MAX] = {0}; // construct path to files
                sprintf(file_path, "%s/%s", dir, p->d_name);
                string_search(file_path, sequence);
            }

            if (p->d_type == DT_DIR)//if it's a directory start run functhion again
            {
                char buf[PATH_MAX] = {0};
                sprintf(buf, "%s/%s", dir, p->d_name);
                walk_dir_impl(buf, sequence);
            }
        }
    }

    closedir(d);
}