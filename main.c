#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>      // for MIN()
#include <dirent.h>
#include <memory.h>
void walk_dir(char *);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dir>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    walk_dir(argv[1]);

    return EXIT_SUCCESS;
}

void print_file(const char *fire_path) {//print entry file
    if (!strcmp(fire_path, ".") || !strcmp(fire_path, ".."))
        return;

    FILE *fp = fopen(fire_path,"r");
    char buffer[256];
    if(fp){
        while (fgets(buffer,256,fp)!=NULL)
        {
            printf("%s\n", buffer);
        }
        
    }

    
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

            char file_path[PATH_MAX]={0};//construct path to files
            sprintf(file_path,"%s/%s",dir,p->d_name);
            print_contaiment(file_path);

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
    walk_dir_impl(0, dir);
}