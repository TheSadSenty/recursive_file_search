//
// Show directory tree with readdir(). Recursive version.
//
// (c) Alexei Guirik, 2023
// This source is licensed under CC BY-NC 4.0
// (https://creativecommons.org/licenses/by-nc/4.0/)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>      // for MIN()
#include <dirent.h>

#define MAX_INDENT_LEVEL 128

void walk_dir(char *dir);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dir>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    walk_dir(argv[1]);

    return EXIT_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

void print_entry(int level, int type, const char *path) {
    if (!strcmp(path, ".") || !strcmp(path, ".."))
        return;

    char indent[MAX_INDENT_LEVEL] = {0};
    memset(indent, ' ', MIN((size_t)level, MAX_INDENT_LEVEL));

    printf("%s[%d] %s\n", indent, type, path);
}

void walk_dir_impl(int ind_level, char *dir) {
    DIR *d = opendir(dir);
    if (d == NULL) {
        printf("Failed to opendir() %s\n", dir);
        return;
    }
    
    while(1) {
        errno = 0;
        struct dirent *p = readdir(d);
        if (p == NULL) {
            if (errno != 0)
                continue;   // Проблема, переходим к следующему элементу
            else
                break;      // В каталоге больше ничего нет
        }

        if (strcmp(p->d_name, ".") && strcmp(p->d_name, "..")) {
            print_entry(ind_level + 1, p->d_type, p->d_name);
            if (p->d_type == DT_DIR) {
                char buf[PATH_MAX] = {0};
                sprintf(buf, "%s/%s", dir, p->d_name);
                walk_dir_impl(ind_level + 1, buf);
            }
        }
    }
    
    closedir(d);
}

void walk_dir(char *dir) {
    print_entry(0, DT_DIR, dir);
    walk_dir_impl(0, dir);
}