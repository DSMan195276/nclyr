#ifndef NCLYR_TUI_TUI_COLOR_H
#define NCLYR_TUI_TUI_COLOR_H

#include <ncurses.h>

#include "cons/color.h"
#include "config.h"

void tui_color_init(void);
void tui_color_set(WINDOW *, struct cons_color_pair colors);
void tui_color_unset(WINDOW *, struct cons_color_pair colors);
void tui_color_pair_fb(int pair, struct cons_color_pair *c);
int tui_color_pair_get(struct cons_color_pair *c);

#define tui_color_fb_set(win, fc, bc)   tui_color_set(  (win), (struct cons_color_pair){ .f = (fc), .b = (bc) })
#define tui_color_fb_unset(win, fc, bc) tui_color_unset((win), (struct cons_color_pair){ .f = (fc), .b = (bc) })

static inline void tui_color_set_from_config(WINDOW *win, union config_data *data)
{
    tui_color_set(win, data->c_pair);
}

static inline void tui_color_unset_from_config(WINDOW *win, union config_data *data)
{
    tui_color_unset(win, data->c_pair);
}

#endif
