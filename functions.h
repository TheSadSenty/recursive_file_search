#pragma once
#include "plugin_api.h"

struct plugin_path_and_opt
{
    const char *so_path;
    const char *so_opt;
};
void seach_plugins_fill_struct(char *dir);
/*Global variavles*/
extern char **array_dlls_path;
extern int plugin_count;
extern int is_debug;
extern struct option *plugins_options;
extern int option_count;

void argument_parser(int, char **);
void string_search(const char *, char *);
int validate_plugin(char *plugin_path);
int parse_plugins_parameters(int argc, char *argv[], char *path_to_so, struct option **detected_opt, size_t **plugins_options_count);
struct option *slice_option_struct(struct option **detected_opt, size_t from, size_t to);