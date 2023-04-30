#include "plugin_api.h"
#include <stdio.h>
/*
Options of my plugin
*/
static struct plugin_option my_plug_long_opt[] = {
    {{"pic", required_argument, 0, 0},
     "Specify image formats to search for"}};

int plugin_get_info(struct plugin_info *ppi)
{
    if (!ppi)
    {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }
    ppi->plugin_purpose = "Search JPEG, PNG, GIF and BMP files";
    ppi->plugin_author = "Kasyanov Maxim, N32451";
    ppi->sup_opts_len = 1;

    ppi->sup_opts = my_plug_long_opt;
    return 0;
}