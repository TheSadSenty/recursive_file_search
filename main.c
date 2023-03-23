#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

void walk_dir_impl(char *, char *);
void argument_parser(int, char **);
int main(int argc, char *argv[])
{
    argument_parser(argc, argv);
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
        free(buffer);
        buffer = NULL;
    }
    fclose(fp);
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
void argument_parser(int argc, char *argv[])
{
    static struct option long_options[] = {
        {"help", no_argument, 0, 0},
        {"version", no_argument, 0, 1}};
    int option_index = 0;
    int choice;
    while ((choice = getopt_long(argc, argv, "vh", long_options, &option_index)) != -1)
    {
        switch (choice)
        {
        case 0:

        case 'h':
            printf("Usage: %s <dir> <string>\n", argv[0]);
            printf("<dir> - directory to search\n");
            printf("<string> - string to search\n");
            exit(EXIT_SUCCESS);
        case 1:

        case 'v':
            printf("Kasyanov Maxim\nN32451\nVariant 2\n");
            exit(EXIT_SUCCESS);
        case '?':
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 3)
    {
        walk_dir_impl(argv[1], argv[2]);
    }
}