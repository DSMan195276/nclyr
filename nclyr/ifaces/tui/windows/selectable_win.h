#ifndef NCLYR_IFACES_TUI_WINDOWS_SELECTABLE_WIN_H
#define NCLYR_IFACES_TUI_WINDOWS_SELECTABLE_WIN_H

#include "cons/printf.h"
#include "windows/window.h"

struct selectable_win {
    struct nclyr_win super_win;

    int selected;
    int disp_offset;
    int total_lines;

    /* Callback to get a specefic line to display */
    void (*get_line) (struct selectable_win *, int line, int width, struct cons_str *);
};

void selectable_win_handle_ch(struct nclyr_win *, int ch, struct nclyr_mouse_event *);
void selectable_win_handle_mouse(struct nclyr_win *, int ch, struct nclyr_mouse_event *);

void selectable_win_update(struct nclyr_win *);

#define SELECTABLE_KEYPRESSES() \
    N_KEYPRESS('j', selectable_win_handle_ch, "Select one below"), \
    N_KEYPRESS('k', selectable_win_handle_ch, "Select one above"), \
    N_KEYPRESS(KEY_UP, selectable_win_handle_ch, "Select one below"), \
    N_KEYPRESS(KEY_DOWN, selectable_win_handle_ch, "Select one above"), \
    N_KEYPRESS('g', selectable_win_handle_ch, "Go to top"), \
    N_KEYPRESS('G', selectable_win_handle_ch, "Go to bottom"), \
    N_KEYPRESS(K_CONTROL('f'), selectable_win_handle_ch, "Scroll one page down"), \
    N_KEYPRESS(K_CONTROL('b'), selectable_win_handle_ch, "Scroll one page up"), \
    N_KEYPRESS(K_CONTROL('d'), selectable_win_handle_ch, "Scroll one half-page down"), \
    N_KEYPRESS(K_CONTROL('u'), selectable_win_handle_ch, "Scroll one half-page up"), \
    N_MOUSE(SCROLL_UP, selectable_win_handle_mouse, "Scroll up"), \
    N_MOUSE(SCROLL_DOWN, selectable_win_handle_mouse, "Scroll down")

#endif