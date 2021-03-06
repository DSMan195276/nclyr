#ifndef NCLYR_ARGS_H
#define NCLYR_ARGS_H

#include "config.h"

#define NCLYR_DEFAULT_ARGS \
    X(help_config, "help-config", '\0', 0, NULL, "Display complete configuration information"), \
    X(version, "version", 'v', 0, NULL, "Display version information"), \
    X(list_players, "list-players", '\0', 0, NULL, "List available players"), \
    X(list_interfaces, "list-interfaces", '\0', 0, NULL, "List available interfaces"), \
    X(no_config, "no-config", '\0', 0, NULL, "Don't load configuration file"), \
    X(help, "help", 'h', 0, NULL, "Display help")

enum arg_index {
    ARG_EXTRA = ARG_PARSER_EXTRA,
    ARG_ERR = ARG_PARSER_ERR,
    ARG_DONE = ARG_PARSER_DONE,
#define X(enu, ...) ARG_##enu
    NCLYR_DEFAULT_ARGS,
#undef X
    ARG_LAST
};

extern const struct arg nclyr_args[];

#endif
