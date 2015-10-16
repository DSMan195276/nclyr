
#include "common.h"

#include <string.h>

#include "cons/color.h"
#include "cons/str.h"
#include "printf/compiler.h"
#include "arg_bool.h"
#include "debug.h"

struct printf_opt_arg_bool {
    struct printf_opt opt;
    int arg;
};

static void print_arg_bool(struct printf_opt *opt, struct cons_printf_compiled *comp, struct cons_str *chstr, size_t arg_count, const struct cons_printf_arg *args)
{

}

struct printf_opt *printf_arg_parse_bool(int index, char *id_par, size_t arg_count, const struct cons_printf_arg *args)
{
    struct printf_opt_arg_bool *bol = malloc(sizeof(*bol));
    memset(bol, 0, sizeof(*bol));
    bol->opt.print = print_arg_bool;
    bol->opt.clear = printf_opt_free;
    bol->arg = index;
    return &bol->opt;
}
