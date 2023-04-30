#include <stdio.h>
#include "plugin_api.h"
#include <dlfcn.h>
void validate_plugin(char *plugin_path)
{
    // init special struct
    printf("%s\n", plugin_path);
    struct plugin_info pi;
    // try to open .so
    void *dl = dlopen(plugin_path, RTLD_LAZY);
    if (!dl)
    {
        fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
        goto END;
    }
    // Check for plugin_get_info() func
    void *func = dlsym(dl, "plugin_get_info");
    if (!func)
    {
        fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
        goto END;
    }
    // init plugin_info func
    typedef int (*pgi_func_t)(struct plugin_info *);
    pgi_func_t pgi_func = (pgi_func_t)func;
    int ret = pgi_func(&pi);
    if (ret < 0)
    {
        // if ret!=0 then plugin is not valid
        fprintf(stderr, "ERROR: plugin_get_info() failed\n");
        goto END;
    }
    // Plugin info
    fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
    fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
    fprintf(stdout, "Supported options: ");
    if (pi.sup_opts_len > 0)
    {
        fprintf(stdout, "\n");
        for (size_t i = 0; i < pi.sup_opts_len; i++)
        {
            fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[i].opt.name, pi.sup_opts[i].opt_descr);
        }
    }
END:
    if (dl)
        dlclose(dl);
}