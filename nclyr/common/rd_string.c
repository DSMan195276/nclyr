
#include "common.h"

#include <stdlib.h>
#include <string.h>

void rd_string_free(struct rd_string *str)
{
    if (str->was_malloced) {
        if (str->str) {
            free(str->str);
            str->str = NULL;
        }
    }
}

void rd_string_dup(struct rd_string *str, const char *s)
{
    rd_string_free(str);
    str->str = strdup(s);
    str->was_malloced = 1;
}

