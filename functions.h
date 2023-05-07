#pragma once
#include "plugin_api.h"

struct plugin_path_and_opt
{
    const char *so_path;
    const char *so_opt;
};
char *seach_so(char *dir);
void argument_parser(int, char **);
void string_search(const char *, char *);
int validate_plugin(char *plugin_path);
void parse_plugins_parameters(int argc, char *argv[], char *path_to_so);