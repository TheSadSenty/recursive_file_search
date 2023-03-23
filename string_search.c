#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>
void string_search(const char *file_path, char *search_string)
{
    if (getenv("LAB11DEBUG") != NULL)
    {
        fprintf(stderr, "Start search in: %s\n", file_path);
    }

    if (!strcmp(file_path, ".") || !strcmp(file_path, ".."))
        return;

    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        if (getenv("LAB11DEBUG") != NULL)
        {
            fprintf(stderr, "Failed to open file: %s\n", file_path);
            if (errno == EACCES)
            {
                fprintf(stderr, "Access denied: %s\n", file_path);
            }
        }

        return;
    }
    if (fp)
    {
        if (getenv("LAB11DEBUG") != NULL)
        {
            fprintf(stderr, "Successfully open file: %s\n", file_path);
        }
        size_t search_string_char_size = sizeof(search_string[0]);
        size_t len_search_string = strlen(search_string) + 1; // because fgets read until n-1

        char *buffer = calloc(len_search_string, search_string_char_size);
        int step = 0;
        while (fgets(buffer, len_search_string, fp) != NULL)
        {
            if (strcmp(buffer, search_string) == 0)
            {
                printf("Find one at: %s\n", file_path);
                fseek(fp, ++step, SEEK_SET);
            }
            else
            {
                fseek(fp, ++step, SEEK_SET);
            }
        }
        free(buffer);
        buffer = NULL;
    }
    fclose(fp);
}