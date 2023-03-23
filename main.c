#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <getopt.h>

#include "functions.h"
int main(int argc, char *argv[])
{
    argument_parser(argc, argv);
    return EXIT_SUCCESS;
}


