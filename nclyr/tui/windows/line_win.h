#ifndef NCLYR_CURSES_LINE_WIN_H
#define NCLYR_CURSES_LINE_WIN_H

#include "player.h"
#include "window.h"

struct line_win {
    struct nclyr_win super_win;

    size_t line_count;
    size_t disp_offset;
    char **lines;

    unsigned int center:1;
};

void line_update(struct nclyr_win *);
void line_clean(struct nclyr_win *);
void line_handle_keypress(struct nclyr_win *, int ch);

void line_free_lines(struct line_win *);

#define LINE_KEYPRESSES \
    { 'j', line_handle_keypress, "Scroll page down" }, \
    { 'k', line_handle_keypress, "Scroll page up" }

#endif