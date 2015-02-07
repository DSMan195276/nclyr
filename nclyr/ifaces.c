
#include "common.h"

#include <unistd.h>

#include "stringcasecmp.h"
#include "iface.h"
#include "tui.h"
#include "console.h"
#include "debug.h"

struct nclyr_iface *nclyr_iface_list[] = {
#if CONFIG_TUI
    &tui_iface.iface,
#endif
#if CONFIG_CONSOLE
    &console_iface,
#endif
    NULL
};

struct nclyr_iface *nclyr_iface_find(const char *name)
{
    struct nclyr_iface **iface;
    for (iface = nclyr_iface_list; *iface; iface++)
        if (stringcasecmp((*iface)->name, name) == 0)
            return *iface;

    return NULL;
}

