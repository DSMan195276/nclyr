#ifndef NCLYR_TUI_PRINTF_COMPILED_H
#define NCLYR_TUI_PRINTF_COMPILED_H

#include <stdlib.h>
#include <ncurses.h>

#include "cons_color.h"
#include "tui/printf.h"

/* One piece to print - Linked list of pieces, printed one at a time */
struct printf_opt {
    struct printf_opt *next;
    void (*print) (struct printf_opt *, struct tui_printf_compiled *, WINDOW *, size_t arg_count, const struct tui_printf_arg *);
    void (*clear) (struct printf_opt *);
};

struct tui_printf_compiled {
    struct printf_opt *head;
    struct cons_color_pair cur_color;
    attr_t attrs;
};

void printf_opt_free(struct printf_opt *);
char *printf_get_next_param(char *params, char **id, char **val);

#endif
