#pragma once
#include "plugin_api.h"

struct plugin_path_and_opt
{
    const char *so_path;
    const char *so_opt;
};
void seach_plugins_fill_struct(char *dir);
void argument_parser(int argc, char **argv);
void walk_dir_call_plugin(char *dir);
/*Global variavles*/
extern char **array_dlls_path;
extern int plugin_count;
extern int is_debug;
extern struct option *plugins_options;
extern int option_count;
extern int is_A;
extern int is_O;
extern int is_N;
extern size_t *actual_options_count;
extern int *actual_detected_options;