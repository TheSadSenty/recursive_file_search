#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "plugin_api.h"
#include "colors.h"
#define DEBUG_PREFIX "libmksN32451:"
/*
Options of my plugin
*/
static struct plugin_option my_plug_long_opt[] = {
    {{"pic", required_argument, 0, 0},
     "Specify image formats to search for"}};

int plugin_get_info(struct plugin_info *ppi)
{
    int save_errno = errno;
    char *DEBUG = getenv("LAB1DEBUG");
    if (!ppi)
    {
        if (DEBUG)
            fprintf(stderr, ANSI_COLOR_RED "%s\n" ANSI_COLOR_RESET, strerror(EINVAL));
        return -1;
    }
    ppi->plugin_purpose = "Search JPEG, PNG, GIF and BMP files";
    ppi->plugin_author = "Kasyanov Maxim, N32451";
    ppi->sup_opts_len = 1;

    ppi->sup_opts = my_plug_long_opt;
    errno = save_errno;
    return 0;
}
int plugin_process_file(const char *fname,
                        struct option in_opts[],
                        size_t in_opts_len)
{
    int save_errno = errno;
    char *DEBUG = getenv("LAB1DEBUG");
    int file_formats[] = {0, 0, 0, 0};      // png, bmp, jpg, gif
    int magic_numbers_len[] = {4, 2, 4, 4}; // length of signature for each format

    int magic_numbers[][4] = {{0x89, 0x50, 0x4e, 0x47},  // png signature
                              {0x42, 0x4d},              // bmp signature
                              {0xff, 0xd8, 0xff, 0xe0},  // jpg signature
                              {0x47, 0x49, 0x46, 0x38}}; // gif signature

    if (!fname || !in_opts || !in_opts_len)
    {
        if (DEBUG)
            fprintf(stderr, ANSI_COLOR_RED DEBUG_PREFIX "%s\n" ANSI_COLOR_RESET, strerror(EINVAL));
        return -1;
    }
    // Check for the presence of options
    for (size_t i = 0; i < in_opts_len; i++)
    {
        if ((in_opts[i].name == NULL) || (in_opts[i].flag == NULL))
        {
            if (DEBUG)
                fprintf(stderr, ANSI_COLOR_RED DEBUG_PREFIX "Got empty structure fileds\n" ANSI_COLOR_RESET);
            return -1;
        }
        if (DEBUG)
        {
            fprintf(stderr, ANSI_COLOR_YELLOW DEBUG_PREFIX "Got option %s with value %s\n" ANSI_COLOR_RESET, in_opts[i].name, (char *)in_opts[i].flag);
        }
    }
    if (in_opts_len != 1)
    {
        fprintf(stderr, ANSI_COLOR_RED DEBUG_PREFIX "Plugin expecting 1 option Got %ld instead" ANSI_COLOR_RESET, in_opts_len);
        return -1;
    }

    if (strstr((char *)in_opts[0].flag, "png") != NULL)
    {
        file_formats[0] = 1;
    }
    if (strstr((char *)in_opts[0].flag, "bmp") != NULL)
    {
        file_formats[1] = 1;
    }
    if (strstr((char *)in_opts[0].flag, "jpg") != NULL)
    {
        file_formats[2] = 1;
    }
    if (strstr((char *)in_opts[0].flag, "gif") != NULL)
    {
        file_formats[3] = 1;
    }
    size_t parsed_file_formats = 0;
    for (int i = 0; i < 4; i++)
    {
        if (file_formats[i])
            parsed_file_formats = parsed_file_formats + 3 + 1; // All file formats length is 3+comma.
    }
    parsed_file_formats = parsed_file_formats - 1; // exclude the last comma
    if (parsed_file_formats != strlen((char *)in_opts[0].flag))
    {
        if (DEBUG)
            fprintf(stderr, ANSI_COLOR_RED DEBUG_PREFIX "Got an unexpected sequence of file formats:%s\n" ANSI_COLOR_RESET, (char *)in_opts[0].flag);
        return -1;
    }

    FILE *fp;
    fp = fopen(fname, "rb");
    if (fp == NULL)
    {
        if (DEBUG)
        {
            fprintf(stderr, ANSI_COLOR_RED "Failed to open file: %s" ANSI_COLOR_RESET "\n", fname);
            if (errno == EACCES)
                fprintf(stderr, ANSI_COLOR_RED "Access denied: %s" ANSI_COLOR_RESET "\n", fname);
        }
        return -1;
    }
    else
    {
        errno = save_errno;
        int file_signature[] = {0, 0, 0, 0};
        for (int i = 0; i < 4; i++)
            file_signature[i] = fgetc(fp);
        fclose(fp);
        for (int file_format = 0; file_format < 4; file_format++)
        {
            int score = 0;
            if (file_formats[file_format])
            {
                for (int byte = 0; byte < magic_numbers_len[file_format]; byte++)
                {
                    if (DEBUG)
                    {
                        printf(ANSI_COLOR_YELLOW "file byte:%x\tbyte to compare:%x\n" ANSI_COLOR_RESET, file_signature[byte], magic_numbers[file_format][byte]);
                    }
                    if (file_signature[byte] == magic_numbers[file_format][byte])
                    {
                        if (DEBUG)
                        {
                            printf(ANSI_COLOR_GREEN "file byte equals byte from list\n" ANSI_COLOR_RESET);
                        }

                        score += 1;
                    }
                }
            }
            if (score == magic_numbers_len[file_format])
            {
                return 0;
            }
        }
        return 1;
    }
}