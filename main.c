#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h> // for MIN()
#include <dirent.h>
#include <memory.h>
void walk_dir_impl(char *, char *);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <dir>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    walk_dir_impl(argv[1], argv[2]);

    return EXIT_SUCCESS;
}

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
    }
}

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